#pragma once
#include "acquisitor/acquisitor.hpp"

using namespace toolbox;

char* Acquisitor::pick_file(bool multiple, std::vector<const char*> filters) {
    return const_cast<char *>(tinyfd_openFileDialog("Choose image", "", 0, NULL, NULL, 0));
}
