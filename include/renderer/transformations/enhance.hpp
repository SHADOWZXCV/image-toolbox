#pragma once
#include "renderer/processor.hpp"

namespace toolbox {
    struct HistogramEqualize: public Transformation {
        cv::Mat apply(cv::Mat&) override;
    };

    struct ContrastStretch: public Transformation {
        cv::Mat apply(cv::Mat&) override;
    };
}
