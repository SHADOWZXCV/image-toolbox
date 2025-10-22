#pragma once
#include "renderer/processor.hpp"

namespace toolbox {
    struct GreyTransformation: public Transformation {
        cv::Mat apply(cv::Mat&) override;
    };
}
