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

            // Temporarily restrict operations to ROI when ROI mode is active
            bool use_roi = false;
            cv::Rect roi_rect;
            // If we're in crop mode, we must NOT scope to ROI here, because Crop
            // needs the full image to replace it with the cropped content.
            bool crop_mode = program::WindowState::controlsState.selectionFlags.crop_enabled;
            if (program::WindowState::controlsState.selectionFlags.roi_enabled &&
                program::WindowState::controlsState.selection.has_roi &&
                !asset.base_image.empty()) {
                const auto &sel = program::WindowState::controlsState.selection;
                int img_w = asset.base_image.cols;
                int img_h = asset.base_image.rows;
                int x0 = (int)std::floor(std::min(sel.start_img.x, sel.end_img.x));
                int y0 = (int)std::floor(std::min(sel.start_img.y, sel.end_img.y));
                int x1 = (int)std::ceil (std::max(sel.start_img.x, sel.end_img.x));
                int y1 = (int)std::ceil (std::max(sel.start_img.y, sel.end_img.y));
                x0 = std::max(0, std::min(x0, img_w - 1));
                y0 = std::max(0, std::min(y0, img_h - 1));
                x1 = std::max(0, std::min(x1, img_w));
                y1 = std::max(0, std::min(y1, img_h));
                int w = std::max(0, x1 - x0);
                int h = std::max(0, y1 - y0);
                if (w > 0 && h > 0) {
                    roi_rect = cv::Rect(x0, y0, w, h);
                    use_roi = !crop_mode; // do not scope when cropping
                }
            }

            cv::Mat full_base;
            if (use_roi) {
                full_base = asset.base_image;
                asset.base_image = full_base(roi_rect);
            }

            T transform(std::forward<Args>(args)...);
            transform.apply(asset);

            if (use_roi) {
                asset.base_image = full_base;
            }

            asset.dirty = true;
            program::WindowState::textureUpdate = true;
            asset.captureSnapshot(transform.op_name());
        }

        static void buildFinalImageFromAsset(toolbox::Asset &asset) {
            if (!asset.dirty)
                return;

            cv::Mat M_2x3 = asset.transformation(cv::Rect(0, 0, 3, 2));
            cv::warpAffine(asset.base_image, asset.displayed_image, M_2x3, asset.base_image.size());

            asset.dirty = false;
        }
    };
}
