#include "renderer/transformations/enhance.hpp"

void toolbox::HistogramEqualize::apply(toolbox::Asset &asset) {
    cv::equalizeHist(asset.original_image, asset.original_image);
}
