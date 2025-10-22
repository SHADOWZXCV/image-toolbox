#include "renderer/transformations/enhance.hpp"

cv::Mat toolbox::HistogramEqualize::apply(cv::Mat &mat) {
    cv::equalizeHist(mat, mat);

    return mat;
}
