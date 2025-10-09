#pragma once
#include "acquisitor/acquisitor.hpp"

using namespace toolbox;

char* Acquisitor::pick_file(bool multiple, std::vector<const char*> filters) {
    const char* filter_array[] = {"*.jpg","*.png", NULL};
    return const_cast<char *>(tinyfd_openFileDialog("Choose image", "", 2, filter_array, "image files", (int) multiple));
}
