#pragma once
#include "renderer/processor.hpp"

namespace toolbox {
    struct GreyTransformation: public Transformation {
        void apply(toolbox::Asset&) override;
    };
}
