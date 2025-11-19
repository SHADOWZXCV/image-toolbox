#pragma once
#include "renderer/transformations/color.hpp"

void toolbox::GreyTransformation::apply(toolbox::Asset& asset) {
    cv::cvtColor(asset.base_image, asset.base_image, cv::COLOR_BGR2GRAY);
}
