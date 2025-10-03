#pragma once
#include "acquisitor/acquisitor.hpp"

using namespace toolbox;

const char* Acquisitor::pick_file(bool multiple, std::vector<const char*> filters) {
    return tinyfd_openFileDialog("Choose image", "", 0, NULL, NULL, 0);
}
