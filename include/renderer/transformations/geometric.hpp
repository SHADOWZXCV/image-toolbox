#pragma once
#include "renderer/processor.hpp"

namespace toolbox {
    struct GeometricTransformation {
        struct Translate : public Transformation {
            float tx;
            float ty;
            Translate(float tx, float ty) noexcept 
                : tx(tx), ty(ty)  {}

            void apply(toolbox::Asset&) override;
        };
        struct Rotation : public Transformation {
            cv::Point2f center;
            float angle;
            Rotation(float cx, float cy, float angle) noexcept
                : center(cx, cy), angle(angle) {}

            void apply(toolbox::Asset&) override;
        };
    };
}
