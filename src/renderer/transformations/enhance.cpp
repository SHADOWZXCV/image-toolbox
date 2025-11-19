#include "renderer/transformations/enhance.hpp"

void toolbox::Enahnce::HistogramEqualize::apply(toolbox::Asset &asset) {
    cv::equalizeHist(asset.base_image, asset.base_image);
}

void toolbox::Enahnce::ContrastStretch::apply(toolbox::Asset &asset) {
    // WE ONLY IMPLEMENT IMAGES WITH DEPTH OF 8
    assert(asset.original_image.depth() == 0 && "Only 8-bit images are supported!");

    float alpha = (r1 != 0) ? (float(s1) / r1) : 0;
    float beta  = ((r2 - r1) != 0) ? (float(s2 - s1) / (r2 - r1)) : 0;
    float gamma = (r2 != 255) ? (float(255 - s2) / (255 - r2)) : 0;

    for (int i = 0; i < asset.base_image.rows; i++) {
        for (int j = 0; j < asset.base_image.cols; j++) {
            uchar intensity = asset.base_image.at<uchar>(i, j);
            int new_intensity = 0;

            if (intensity >= 0 && intensity < r1) {
                new_intensity = int(std::round(alpha * intensity));
            } else if (intensity >= r1 && intensity < r2) {
                new_intensity = int(std::round(beta * (intensity - r1) + s1));
            } else if (intensity >= r2 && intensity < 255) {
                new_intensity = int(std::round(gamma * (intensity - r2) + s2));
            } else {
                new_intensity = 255;
            }

            asset.base_image.at<uchar>(i, j) = cv::saturate_cast<uchar>(new_intensity);
        }
    }
}

void toolbox::Enahnce::Crop::apply(toolbox::Asset &asset) {
    // Clamp ROI to image bounds and enforce positive size
    int img_w = asset.base_image.cols;
    int img_h = asset.base_image.rows;
    int x0 = std::max(0, std::min(x, img_w - 1));
    int y0 = std::max(0, std::min(y, img_h - 1));
    int x1 = std::max(0, std::min(x + w, img_w));
    int y1 = std::max(0, std::min(y + h, img_h));
    int rw = std::max(0, x1 - x0);
    int rh = std::max(0, y1 - y0);
    if (rw <= 0 || rh <= 0) return;

    cv::Rect roi(x0, y0, rw, rh);
    asset.base_image = asset.base_image(roi).clone();

    // Reset transformation since image space changed
    asset.transformation = cv::Mat::eye(3, 3, CV_32F);
}
