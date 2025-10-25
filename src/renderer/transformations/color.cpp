#pragma once
#include "renderer/transformations/color.hpp"

void toolbox::GreyTransformation::apply(toolbox::Asset& asset) {
    cv::cvtColor(asset.original_image, asset.original_image, cv::COLOR_BGR2GRAY);
}
