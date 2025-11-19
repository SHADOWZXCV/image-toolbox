#pragma once
#include "renderer/processor.hpp"

namespace toolbox {
    struct GreyTransformation: public Transformation {
        const char* op_name() const override { return "Grayscale"; }
        void apply(toolbox::Asset&) override;
    };
}
