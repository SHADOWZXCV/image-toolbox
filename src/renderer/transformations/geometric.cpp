#include "renderer/transformations/geometric.hpp"

void toolbox::GeometricTransformation::Translate::apply(toolbox::Asset &asset) {
    cv::Mat translation = (cv::Mat_<float>(3, 3) << 1, 0, tx, 0, 1, ty, 0, 0, 1);

    asset.transformation = translation * asset.transformation;
}

void toolbox::GeometricTransformation::Rotation::apply(toolbox::Asset &asset) {
    cv::Mat rotation = cv::getRotationMatrix2D(center, angle, 1);
    cv::Mat holder = cv::Mat::eye(3, 3, CV_32F);

    rotation.copyTo(holder(cv::Rect(0, 0, 3, 2)));

    asset.transformation = holder * asset.transformation;
}

void toolbox::GeometricTransformation::Skew::apply(toolbox::Asset &asset) {
    cv::Point2f src[3];
    cv::Point2f dst[3];

    src[0] = p1_b;
    src[1] = p2_b;
    src[2] = p3_b;

    dst[0] = p1_a;
    dst[1] = p2_a;
    dst[2] = p3_a;

    cv::Mat res = cv::getAffineTransform(src, dst);
    cv::Mat holder = cv::Mat::eye(3, 3, CV_32F);

    res.copyTo(holder(cv::Rect(0, 0, 3, 2)));

    asset.transformation = holder * asset.transformation;
}
