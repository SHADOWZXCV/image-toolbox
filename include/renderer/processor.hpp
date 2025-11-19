#pragma once
#include <opencv2/opencv.hpp>
#include "acquisitor/acquisitor.hpp"
#include "shared/state.hpp"

namespace toolbox {
    struct Transformation {
        virtual void apply(toolbox::Asset &) = 0;
    };

    class OpenCVProcessor {
        public:
        template <typename T, typename ...Args>
        static void process(toolbox::Asset &asset, Args&& ...args) {
            static_assert(std::is_base_of<Transformation, T>::value, "Unknown transformation");

            T transform(std::forward<Args>(args)...);
            transform.apply(asset);
            asset.dirty = true;
            program::WindowState::textureUpdate = true;
        }

        static void buildFinalImageFromAsset(toolbox::Asset &asset) {
            if (!asset.dirty)
                return;

            cv::Mat M_2x3 = asset.transformation(cv::Rect(0, 0, 3, 2));
            cv::warpAffine(asset.base_image, asset.displayed_image, M_2x3, asset.base_image.size());

            asset.dirty = false;
        }
    };
}
