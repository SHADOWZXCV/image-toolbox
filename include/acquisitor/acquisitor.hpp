#pragma once
#include <vector>

extern "C" {
    #include <tinyfiledialogs.h>
}

namespace toolbox {
    class Acquisitor {
        public:
            static char* pick_file(bool multiple, std::vector<const char*> filters = {});
    };
}