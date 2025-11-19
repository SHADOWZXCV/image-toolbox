#pragma once
#include <vector>
#include <string>
#include <chrono>
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
            std::string label;
            std::chrono::system_clock::time_point timestamp;
        };
        std::vector<Snapshot> history;
        int history_index = -1;
        std::string last_snapshot_label;
        std::chrono::system_clock::time_point last_snapshot_time{};

        bool setOriginalImage(cv::Mat image) {
            this->original_image = image;
            this->base_image = image;
            this->transformation = cv::Mat::eye(3, 3, CV_32F);

            return true;
        }

        void initHistory() {
            history.clear();
            history_index = -1;
            last_snapshot_label.clear();
            last_snapshot_time = {};
        }

        void captureSnapshot(const std::string &label) {
            // Truncate redo stack if we've undone
            if (history_index + 1 < (int)history.size()) {
                history.erase(history.begin() + history_index + 1, history.end());
            }
            auto now = std::chrono::system_clock::now();
            const auto debounce = std::chrono::milliseconds(450);
            if (!history.empty() && history_index == (int)history.size() - 1 &&
                !last_snapshot_label.empty() && label == last_snapshot_label &&
                (now - last_snapshot_time) <= debounce) {
                Snapshot &s = history.back();
                s.base_image = base_image.clone();
                s.transformation = transformation.clone();
                s.label = label;
                s.timestamp = now;
            } else {
                Snapshot s;
                s.base_image = base_image.clone();
                s.transformation = transformation.clone();
                s.label = label;
                s.timestamp = now;
                history.push_back(std::move(s));
                history_index = (int)history.size() - 1;
            }
            last_snapshot_label = label;
            last_snapshot_time = now;
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
