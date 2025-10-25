#include "renderer/transformations/geometric.hpp"

void toolbox::GeometricTransformation::Translate::apply(toolbox::Asset &asset) {
    cv::Mat translation = (cv::Mat_<float>(3, 3) << 1, 0, tx, 0, 1, ty, 0, 0, 1);

    asset.transformation = translation * asset.transformation;
}
