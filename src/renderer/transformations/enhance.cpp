#include "renderer/transformations/enhance.hpp"

void toolbox::Enahnce::HistogramEqualize::apply(toolbox::Asset &asset) {
    cv::equalizeHist(asset.original_image, asset.original_image);
}

void toolbox::Enahnce::ContrastStretch::apply(toolbox::Asset &asset) {
    // WE ONLY IMPLEMENT IMAGES WITH DEPTH OF 8
    assert(asset.original_image.depth() == 0 && "Only 8-bit images are supported!");

    float alpha = (r1 != 0) ? (float(s1) / r1) : 0;
    float beta  = ((r2 - r1) != 0) ? (float(s2 - s1) / (r2 - r1)) : 0;
    float gamma = (r2 != 255) ? (float(255 - s2) / (255 - r2)) : 0;

    for (int i = 0; i < asset.original_image.rows; i++) {
        for (int j = 0; j < asset.original_image.cols; j++) {
            uchar intensity = asset.original_image.at<uchar>(i, j);
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

            asset.original_image.at<uchar>(i, j) = cv::saturate_cast<uchar>(new_intensity);
        }
    }
}
