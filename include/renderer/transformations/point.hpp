#pragma once
#include "renderer/processor.hpp"

namespace toolbox {
    struct PointProcessing {
        struct PowerLaw : public Transformation {
            float gamma; // exponent only; scaling constant derived from gamma
            PowerLaw(float gamma_) : gamma(gamma_) {}
            const char* op_name() const override { return "Power Law"; }
            void apply(toolbox::Asset &asset) override;
        };
        struct LogTransform : public Transformation {
            LogTransform() {}
            const char* op_name() const override { return "Log Transform"; }
            void apply(toolbox::Asset &asset) override;
        };
        struct Inversion : public Transformation {
            const char* op_name() const override { return "Invert"; }
            void apply(toolbox::Asset &asset) override;
        };
        struct GrayLevelSlice : public Transformation {
            int minI; int maxI; bool preserveOthers; int constantValue;
            GrayLevelSlice(int minI_, int maxI_, bool preserve_, int constVal_) :
                minI(minI_), maxI(maxI_), preserveOthers(preserve_), constantValue(constVal_) {}
            const char* op_name() const override { return "Gray Level Slice"; }
            void apply(toolbox::Asset &asset) override;
        };
        struct BitPlaneSlice : public Transformation {
            unsigned int bitMask; // bits to keep (supports >8 bits)
            BitPlaneSlice(unsigned int mask_) : bitMask(mask_) {}
            const char* op_name() const override { return "Bit Plane Slice"; }
            void apply(toolbox::Asset &asset) override;
        };
    };
}
