#pragma once
#include "renderer/processor.hpp"

namespace toolbox {
    struct GeometricTransformation {
        struct Translate : public Transformation {
            float tx;
            float ty;
            Translate(float tx, float ty) noexcept 
                : tx(tx), ty(ty)  {}

            const char* op_name() const override { return "Move"; }
            void apply(toolbox::Asset&) override;
        };
        struct Rotation : public Transformation {
            cv::Point2f center;
            float angle;
            Rotation(float &cx, float &cy, float &angle) noexcept
                : center(cx, cy), angle(angle) {}

            const char* op_name() const override { return "Rotate"; }
            void apply(toolbox::Asset&) override;
        };
        struct Skew : public Transformation {
            cv::Point2f p1_b, p2_b, p3_b, p1_a, p2_a, p3_a;
            Skew(const cv::Point2f& p1b, const cv::Point2f& p2b, const cv::Point2f& p3b,
                 const cv::Point2f& p1a, const cv::Point2f& p2a, const cv::Point2f& p3a) noexcept
                : p1_b(p1b), p2_b(p2b), p3_b(p3b), p1_a(p1a), p2_a(p2a), p3_a(p3a) {}

            const char* op_name() const override { return "Skew"; }
            void apply(toolbox::Asset&) override;
        };
        struct Scale : public Transformation {
            float sx, sy;
            Scale(float sx, float sy) noexcept
                : sx(sx), sy(sy) {}

            const char* op_name() const override { return "Scale"; }
            void apply(toolbox::Asset&) override;
        };
        struct Flip : public Transformation {
            bool x, y;
            Flip(bool x, bool y) noexcept
                : x(x), y(y) {}

            const char* op_name() const override { return "Flip"; }
            void apply(toolbox::Asset&) override;
        };
    };
}
