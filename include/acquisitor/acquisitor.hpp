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
            bool applied_to_roi = false;
            int roi_x = 0;
            int roi_y = 0;
            int roi_w = 0;
            int roi_h = 0;
            struct PointParamsCopy {
                bool has_power = false;
                float gamma = 1.0f;
                bool log_used = false;
                bool invert_used = false;
                bool has_slice = false;
                int slice_min = 0;
                int slice_max = 255;
                bool slice_preserve = true;
                bool slice_constant = true;
                int slice_constant_value = 255;
                bool bit_plane_used = false;
                int bit_plane = 0;
            } point_params;
        };
        std::vector<Snapshot> history;
        int history_index = -1;
        std::string last_snapshot_label;
        std::chrono::system_clock::time_point last_snapshot_time{};
        // live point processing parameters (mirrors last operation applied)
        struct PointParamsLive {
            bool has_power = false;
            float gamma = 1.0f;
            bool log_used = false;
            bool invert_used = false;
            bool has_slice = false;
            int slice_min = 0;
            int slice_max = 255;
            bool slice_preserve = true;
            bool slice_constant = true;
            int slice_constant_value = 255;
            bool bit_plane_used = false;
            int bit_plane = 0;
        } pointParams;

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

        void captureSnapshot(const std::string &label,
                     bool applied_to_roi = false,
                     int rx = 0, int ry = 0, int rw = 0, int rh = 0) {
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
                s.applied_to_roi = applied_to_roi;
                s.roi_x = rx; s.roi_y = ry; s.roi_w = rw; s.roi_h = rh;
                s.point_params.has_power = pointParams.has_power;
                s.point_params.gamma = pointParams.gamma;
                s.point_params.log_used = pointParams.log_used;
                s.point_params.invert_used = pointParams.invert_used;
                s.point_params.has_slice = pointParams.has_slice;
                s.point_params.slice_min = pointParams.slice_min;
                s.point_params.slice_max = pointParams.slice_max;
                s.point_params.slice_preserve = pointParams.slice_preserve;
                s.point_params.slice_constant = pointParams.slice_constant;
                s.point_params.slice_constant_value = pointParams.slice_constant_value;
                s.point_params.bit_plane_used = pointParams.bit_plane_used;
                s.point_params.bit_plane = pointParams.bit_plane;
            } else {
                Snapshot s;
                s.base_image = base_image.clone();
                s.transformation = transformation.clone();
                s.label = label;
                s.timestamp = now;
                s.applied_to_roi = applied_to_roi;
                s.roi_x = rx; s.roi_y = ry; s.roi_w = rw; s.roi_h = rh;
                s.point_params.has_power = pointParams.has_power;
                s.point_params.gamma = pointParams.gamma;
                s.point_params.log_used = pointParams.log_used;
                s.point_params.invert_used = pointParams.invert_used;
                s.point_params.has_slice = pointParams.has_slice;
                s.point_params.slice_min = pointParams.slice_min;
                s.point_params.slice_max = pointParams.slice_max;
                s.point_params.slice_preserve = pointParams.slice_preserve;
                s.point_params.slice_constant = pointParams.slice_constant;
                s.point_params.slice_constant_value = pointParams.slice_constant_value;
                s.point_params.bit_plane_used = pointParams.bit_plane_used;
                s.point_params.bit_plane = pointParams.bit_plane;
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
            // restore live point params for UI sync
            pointParams.has_power = s.point_params.has_power;
            pointParams.gamma = s.point_params.gamma;
            pointParams.log_used = s.point_params.log_used;
            pointParams.invert_used = s.point_params.invert_used;
            pointParams.has_slice = s.point_params.has_slice;
            pointParams.slice_min = s.point_params.slice_min;
            pointParams.slice_max = s.point_params.slice_max;
            pointParams.slice_preserve = s.point_params.slice_preserve;
            pointParams.slice_constant = s.point_params.slice_constant;
            pointParams.slice_constant_value = s.point_params.slice_constant_value;
            pointParams.bit_plane_used = s.point_params.bit_plane_used;
            pointParams.bit_plane = s.point_params.bit_plane;
            return true;
        }

        bool redo() {
            if (history_index + 1 >= (int)history.size()) return false;
            history_index++;
            const Snapshot &s = history[history_index];
            base_image = s.base_image.clone();
            transformation = s.transformation.clone();
            dirty = true;
            pointParams.has_power = s.point_params.has_power;
            pointParams.gamma = s.point_params.gamma;
            pointParams.log_used = s.point_params.log_used;
            pointParams.invert_used = s.point_params.invert_used;
            pointParams.has_slice = s.point_params.has_slice;
            pointParams.slice_min = s.point_params.slice_min;
            pointParams.slice_max = s.point_params.slice_max;
            pointParams.slice_preserve = s.point_params.slice_preserve;
            pointParams.slice_constant = s.point_params.slice_constant;
            pointParams.slice_constant_value = s.point_params.slice_constant_value;
            pointParams.bit_plane_used = s.point_params.bit_plane_used;
            pointParams.bit_plane = s.point_params.bit_plane;
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
