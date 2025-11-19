#pragma once
#include "renderer/processor.hpp"

namespace toolbox {
    struct Enahnce {
        struct HistogramEqualize: public Transformation {
            const char* op_name() const override { return "Histogram Equalization"; }
            void apply(toolbox::Asset&) override;
        };
        struct ContrastStretch: public Transformation {
            short r1, r2, s1, s2;
            ContrastStretch(short r1, short r2, short s1, short s2)
            : r1(r1), r2(r2), s1(s1), s2(s2) {}

            const char* op_name() const override { return "Contrast Stretch"; }
            void apply(toolbox::Asset&) override;
        };
        struct Crop : public Transformation {
            int x, y, w, h; // ROI in image coordinates
            Crop(int ix, int iy, int iw, int ih) : x(ix), y(iy), w(iw), h(ih) {}
            void apply(toolbox::Asset &asset) override;
            const char* op_name() const override { return "Crop"; }
        };
    };
}
