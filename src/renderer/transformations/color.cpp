#pragma once
#include "renderer/transformations/color.hpp"

cv::Mat toolbox::GreyTransformation::apply(cv::Mat& mat) {
    cv::cvtColor(mat, mat, cv::COLOR_BGR2GRAY);

    return mat;
}
