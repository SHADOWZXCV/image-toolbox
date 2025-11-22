#pragma once
#include <opencv2/opencv.hpp>
#include "acquisitor/acquisitor.hpp"
#include "shared/state.hpp"
#include <typeinfo>

namespace toolbox {
    struct Transformation {
        virtual ~Transformation() = default;
        virtual void apply(toolbox::Asset &) = 0;
        virtual const char* op_name() const = 0;
    };

    class OpenCVProcessor {
        public:
        template <typename T, typename ...Args>
        static void process(toolbox::Asset &asset, Args&& ...args) {
            static_assert(std::is_base_of<Transformation, T>::value, "Unknown transformation");
            auto &flags = program::WindowState::controlsState.selectionFlags;
            auto &sel   = program::WindowState::controlsState.selection;
            bool roi_mode = flags.roi_enabled && sel.has_roi && sel.quad_valid && !asset.base_image.empty();
            bool crop_mode = flags.crop_enabled; // cropping handled in image-preview (not here)

            cv::Mat original_full;
            cv::Mat roi_rect_img; // warped ROI rectangle
            cv::Mat H, Hinv;
            int rx=0, ry=0, rw=0, rh=0; // bounding box metadata

            if (roi_mode) {
                rx = sel.bbox_rx; ry = sel.bbox_ry; rw = sel.bbox_rw; rh = sel.bbox_rh;
                // Extract minimal sub-image
                cv::Rect bbox(rx, ry, rw, rh);
                cv::Mat sub = asset.base_image(bbox);
                // Offset quad points relative to bbox
                std::vector<cv::Point2f> quad_offset(4);
                for (int i=0;i<4;i++) quad_offset[i] = cv::Point2f(sel.quad_base[i].x - rx, sel.quad_base[i].y - ry);
                // Destination rectangle uses displayed selection dimensions if available, else bounding box
                int dstW = sel.disp_rect_w > 0 ? sel.disp_rect_w : rw;
                int dstH = sel.disp_rect_h > 0 ? sel.disp_rect_h : rh;
                std::vector<cv::Point2f> dstRect = { {0.f,0.f}, {(float)dstW-1.f,0.f}, {(float)dstW-1.f,(float)dstH-1.f}, {0.f,(float)dstH-1.f} };
                H = cv::getPerspectiveTransform(quad_offset, dstRect);
                Hinv = H.inv();
                cv::warpPerspective(sub, roi_rect_img, H, cv::Size(dstW, dstH), cv::INTER_LINEAR, cv::BORDER_REPLICATE);
                original_full = asset.base_image; // keep reference
                asset.base_image = roi_rect_img;  // operate only on ROI rectangle
            }

            T transform(std::forward<Args>(args)...);
            transform.apply(asset);

            if (roi_mode) {
                cv::Mat processed_roi = asset.base_image; // rectangle after transform
                asset.base_image = original_full; // restore full image
                // Re-warp processed ROI back into quadrilateral location (skip if crop mode; crop handled earlier)
                if (!crop_mode) {
                    cv::Mat patch; // warped back patch sized bounding box
                    cv::warpPerspective(processed_roi, patch, Hinv, cv::Size(rw, rh), cv::INTER_LINEAR, cv::BORDER_REPLICATE);
                    // Mask for compositing
                    cv::Mat white(processed_roi.size(), CV_8UC1, cv::Scalar(255));
                    cv::Mat mask;
                    cv::warpPerspective(white, mask, Hinv, cv::Size(rw, rh), cv::INTER_NEAREST, cv::BORDER_CONSTANT, cv::Scalar(0));
                    cv::Mat target_roi = asset.base_image(cv::Rect(rx, ry, rw, rh));
                    // If multi-channel, expand mask
                    if (target_roi.channels() > 1) {
                        std::vector<cv::Mat> ch(target_roi.channels(), mask);
                        cv::Mat mask_multi; cv::merge(ch, mask_multi);
                        patch.copyTo(target_roi, mask_multi);
                    } else {
                        patch.copyTo(target_roi, mask);
                    }
                }
            }

            asset.dirty = true;
            program::WindowState::textureUpdate = true;
            bool applied_to_roi_meta = roi_mode; // crop handled externally already snapshots separately
            asset.captureSnapshot(transform.op_name(), applied_to_roi_meta, rx, ry, rw, rh);
        }

        static void buildFinalImageFromAsset(toolbox::Asset &asset) {
            if (!asset.dirty)
                return;

            // Use full 3x3 homography (even if affine) for consistency
            cv::warpPerspective(asset.base_image, asset.displayed_image, asset.transformation, asset.base_image.size(), cv::INTER_LINEAR, cv::BORDER_CONSTANT);

            // Recalculate active ROI quadrilateral after any transform change (undo/redo or new op)
            auto &flags = program::WindowState::controlsState.selectionFlags;
            auto &sel   = program::WindowState::controlsState.selection;
            if (flags.roi_enabled && sel.has_roi && !asset.base_image.empty()) {
                // Rectangle corners in displayed image space
                float sx0 = std::min(sel.start_img.x, sel.end_img.x);
                float sy0 = std::min(sel.start_img.y, sel.end_img.y);
                float sx1 = std::max(sel.start_img.x, sel.end_img.x);
                float sy1 = std::max(sel.start_img.y, sel.end_img.y);
                sel.disp_rect_w = (int)std::round(sx1 - sx0);
                sel.disp_rect_h = (int)std::round(sy1 - sy0);
                if (sel.disp_rect_w <= 0 || sel.disp_rect_h <= 0) {
                    sel.quad_valid = false;
                } else {
                    std::vector<cv::Point2f> dispQuad = { {sx0,sy0},{sx1,sy0},{sx1,sy1},{sx0,sy1} };
                    cv::Mat Minv;
                    bool invertible = true;
                    double det = cv::determinant(asset.transformation);
                    if (fabs(det) < 1e-8) invertible = false; else Minv = asset.transformation.inv();
                    if (invertible) {
                        std::vector<cv::Point2f> quad_base_vec;
                        cv::perspectiveTransform(dispQuad, quad_base_vec, Minv);
                        if (quad_base_vec.size() == 4) {
                            int img_w = asset.base_image.cols; int img_h = asset.base_image.rows;
                            float minx = quad_base_vec[0].x, maxx = quad_base_vec[0].x;
                            float miny = quad_base_vec[0].y, maxy = quad_base_vec[0].y;
                            for (int i=0;i<4;i++) {
                                cv::Point2f p = quad_base_vec[i];
                                p.x = std::max(0.f, std::min(p.x, (float)img_w - 1.f));
                                p.y = std::max(0.f, std::min(p.y, (float)img_h - 1.f));
                                sel.quad_base[i] = p;
                                if (i>0) {
                                    minx = std::min(minx, p.x); maxx = std::max(maxx, p.x);
                                    miny = std::min(miny, p.y); maxy = std::max(maxy, p.y);
                                }
                            }
                            sel.bbox_rx = (int)std::floor(minx);
                            sel.bbox_ry = (int)std::floor(miny);
                            sel.bbox_rw = (int)std::ceil(maxx - minx + 1.f);
                            sel.bbox_rh = (int)std::ceil(maxy - miny + 1.f);
                            if (sel.bbox_rx < 0) sel.bbox_rx = 0; if (sel.bbox_ry < 0) sel.bbox_ry = 0;
                            if (sel.bbox_rx + sel.bbox_rw > img_w) sel.bbox_rw = img_w - sel.bbox_rx;
                            if (sel.bbox_ry + sel.bbox_rh > img_h) sel.bbox_rh = img_h - sel.bbox_ry;
                            sel.quad_valid = true;
                        } else {
                            sel.quad_valid = false;
                        }
                    } else {
                        sel.quad_valid = false;
                    }
                }
            }

            asset.dirty = false;
        }
    };
}
