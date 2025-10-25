#pragma once
#include "renderer/processor.hpp"

namespace toolbox {
    struct HistogramEqualize: public Transformation {
        void apply(toolbox::Asset&) override;
    };

    struct ContrastStretch: public Transformation {
        void apply(toolbox::Asset&) override;
    };
}
