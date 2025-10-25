#pragma once
#include "renderer/processor.hpp"

namespace toolbox {
    struct GeometricTransformation {
        struct Translate : public Transformation {
            float tx;
            float ty;
            Translate(float tx, float ty): tx(tx), ty(ty) {}

            void apply(toolbox::Asset&) override;
        };
    };
}
