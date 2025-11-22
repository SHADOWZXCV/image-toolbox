#include "renderer/transformations/point.hpp"
#include <opencv2/opencv.hpp>
#include <cmath>

using namespace toolbox;

static inline void ensure_grayscale(toolbox::Asset &asset) {
    // Only support 8-bit single channel for now
    CV_Assert(asset.base_image.channels() == 1);
}

static inline double computeL(const cv::Mat &img) {
    int depth = img.depth();
    switch (depth) {
        case CV_8U: return 256.0; // 2^8
        case CV_16U: return 65536.0; // 2^16
        case CV_32F: {
            double minv,maxv; cv::minMaxLoc(img, &minv, &maxv); return maxv + 1.0; }
        default: return 256.0; // fallback
    }
}

void PointProcessing::PowerLaw::apply(toolbox::Asset &asset) {
    ensure_grayscale(asset);
    cv::Mat &img = asset.base_image;
    const float L = (float)computeL(img);
    // c = (L-1) / ( (L-1)^gamma )
    float c_const = (L - 1.0f) / std::pow(L - 1.0f, gamma);
    float inv255 = 1.0f / 255.0f;
    for (int r = 0; r < img.rows; ++r) {
        uchar *row = img.ptr<uchar>(r);
        for (int cc = 0; cc < img.cols; ++cc) {
            float nr = row[cc] * inv255; // in [0,1]
            float val = c_const * std::pow(nr, gamma);
            val = std::max(0.0f, std::min(1.0f, val));
            row[cc] = static_cast<uchar>(std::round(val * 255.0f));
        }
    }
}

void PointProcessing::LogTransform::apply(toolbox::Asset &asset) {
    ensure_grayscale(asset);
    cv::Mat &img = asset.base_image;
    const float L = (float)computeL(img);
    // c = (L-1)/log(L)
    const float c_const = (L - 1.0f) / std::log(L);
    for (int r = 0; r < img.rows; ++r) {
        uchar *row = img.ptr<uchar>(r);
        for (int cc = 0; cc < img.cols; ++cc) {
            float val = c_const * std::log(1.0f + row[cc]);
            val = std::max(0.0f, std::min(255.0f, val));
            row[cc] = static_cast<uchar>(std::round(val));
        }
    }
}

void PointProcessing::Inversion::apply(toolbox::Asset &asset) {
    ensure_grayscale(asset);
    cv::Mat &img = asset.base_image;
    for (int r = 0; r < img.rows; ++r) {
        uchar *row = img.ptr<uchar>(r);
        for (int ccol = 0; ccol < img.cols; ++ccol) {
            row[ccol] = static_cast<uchar>(255 - row[ccol]);
        }
    }
}

void PointProcessing::GrayLevelSlice::apply(toolbox::Asset &asset) {
    ensure_grayscale(asset);
    cv::Mat &img = asset.base_image;
    int lo = std::max(0, std::min(minI, 255));
    int hi = std::max(0, std::min(maxI, 255));
    if (lo > hi) std::swap(lo, hi);
    const int maxVal = 255; // 8-bit assumption; extend for higher depths later.
    int clampedConst = std::max(0, std::min(constantValue, 255));
    for (int r = 0; r < img.rows; ++r) {
        uchar *row = img.ptr<uchar>(r);
        for (int ccol = 0; ccol < img.cols; ++ccol) {
            uchar intensity = row[ccol];
            if (intensity >= lo && intensity <= hi) {
                row[ccol] = static_cast<uchar>(maxVal);
            } else if (!preserveOthers) {
                row[ccol] = static_cast<uchar>(clampedConst);
            }
        }
    }
}

void PointProcessing::BitPlaneSlice::apply(toolbox::Asset &asset) {
    ensure_grayscale(asset);
    cv::Mat &img = asset.base_image;
    unsigned int mask = bitMask;
    for (int r = 0; r < img.rows; ++r) {
        uchar *row = img.ptr<uchar>(r);
        for (int ccol = 0; ccol < img.cols; ++ccol) {
            unsigned int v = row[ccol];
            unsigned int kept = v & mask; // keep selected bits with original weights
            row[ccol] = static_cast<uchar>(kept);
        }
    }
}
