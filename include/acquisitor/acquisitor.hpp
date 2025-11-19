#pragma once
#include <vector>
#include <SDL.h>
#include <opencv2/opencv.hpp>
extern "C" {
    #include <tinyfiledialogs.h>
}

#include "manager/panel.hpp"
#include "manager/state.hpp"

namespace toolbox {
    struct Asset {
        std::string path;
        SDL_Texture *SDL_texture;
        cv::Mat original_image;
        // Base image for all invertible transforms; non-invertible ops modify this
        cv::Mat base_image;
        cv::Mat displayed_image;
        
        ImVec2 position;
        ImVec2 size;
        ImVec2 rotation_center;
        float rotation_angle;
        // state
        cv::Mat transformation;
        bool dirty = false;

        // Undo/redo snapshot of base_image + transformation
        struct Snapshot {
            cv::Mat base_image;
            cv::Mat transformation;
        };
        std::vector<Snapshot> history;
        int history_index = -1;

        bool setOriginalImage(cv::Mat image) {
            this->original_image = image;
            this->base_image = image;
            this->transformation = cv::Mat::eye(3, 3, CV_32F);

            return true;
        }

        void initHistory() {
            history.clear();
            history_index = -1;
        }

        void captureSnapshot() {
            // Truncate redo stack if we've undone
            if (history_index + 1 < (int)history.size()) {
                history.erase(history.begin() + history_index + 1, history.end());
            }
            Snapshot s;
            s.base_image = base_image.clone();
            s.transformation = transformation.clone();
            history.push_back(std::move(s));
            history_index = (int)history.size() - 1;
        }

        bool undo() {
            if (history_index <= 0) return false;
            history_index--;
            const Snapshot &s = history[history_index];
            base_image = s.base_image.clone();
            transformation = s.transformation.clone();
            dirty = true;
            return true;
        }

        bool redo() {
            if (history_index + 1 >= (int)history.size()) return false;
            history_index++;
            const Snapshot &s = history[history_index];
            base_image = s.base_image.clone();
            transformation = s.transformation.clone();
            dirty = true;
            return true;
        }
    };

    class Acquisitor {
        public:
            static std::unordered_map<std::string, std::shared_ptr<toolbox::Asset>> assets;
            static std::shared_ptr<toolbox::Asset> pick_image(bool multiple, std::vector<const char*> filters = {});
            static std::shared_ptr<toolbox::Asset> load_image(char *path);
    };
}
