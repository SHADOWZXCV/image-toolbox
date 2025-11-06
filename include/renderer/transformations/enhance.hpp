#pragma once
#include "renderer/processor.hpp"

namespace toolbox {
    struct Enahnce {
        struct HistogramEqualize: public Transformation {
            void apply(toolbox::Asset&) override;
        };
        struct ContrastStretch: public Transformation {
            short r1, r2, s1, s2;
            ContrastStretch(short r1, short r2, short s1, short s2)
            : r1(r1), r2(r2), s1(s1), s2(s2) {}

            void apply(toolbox::Asset&) override;
        };
    };
}
