#pragma once
#include <opencv2/opencv.hpp>

namespace toolbox {
    struct Transformation {
        virtual cv::Mat apply(cv::Mat&) = 0;
    };

    class OpenCVProcessor {
        public:
        template <typename T, typename ...Args>
        static void process(cv::Mat &mat) {
            static_assert(std::is_base_of<Transformation, T>::value, "Unknown transformation");

            T transform(std::forward<Args>(args)...);
            transform.apply(mat);
        }
    };
}
