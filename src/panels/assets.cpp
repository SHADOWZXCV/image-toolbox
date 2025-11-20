#include "panels/assets.hpp"
#include "fontawesome/IconsFontAwesome6.h"
#include <chrono>
#include <ctime>
#include <cctype>

void IAssetsPanel::pre_draw() {}
bool IAssetsPanel::show_condition() {
    // TODO: Reserve names in an internal state
    // The absolute useless getter of all time right here!
    std::shared_ptr<program::IAcquisitorService> acquisitor = program::ServiceManager::get<program::IAcquisitorService>();
    std::vector<std::weak_ptr<toolbox::Asset>> *assets = acquisitor->getAssets();

    if (assets->empty()) {
        return false;
    }

    return true;
}

void IAssetsPanel::handle_events() {
    this->restrictWindowSize();
}

void IAssetsPanel::restrictWindowSize() {
    ImGuiID activeId = ImGui::GetActiveID();
    ImGuiWindow* window = ImGui::GetCurrentWindow();

    if (activeId == ImGui::GetWindowResizeBorderID(window, ImGuiDir_Right) || 
        activeId == ImGui::GetWindowResizeBorderID(window, ImGuiDir_Down) ||
        activeId == ImGui::GetWindowResizeBorderID(window, ImGuiDir_Up) ||
        activeId == ImGui::GetWindowResizeCornerID(window, 0) ||
        activeId == ImGui::GetWindowResizeCornerID(window, 1) ||
        activeId == ImGui::GetWindowResizeCornerID(window, 2) ||
        activeId == ImGui::GetWindowResizeCornerID(window, 3)) {
        ImGui::ClearActiveID();
    } else if (activeId == ImGui::GetWindowResizeBorderID(window, ImGuiDir_Left)) {
        ImVec2 currPosition = ImGui::GetWindowPos();
        ImVec2 currSize = ImGui::GetWindowSize();
        ImGuiIO& io = ImGui::GetIO();

        if (window->Pos.x + io.MouseDelta.x > this->position.x) {
            ImGui::ClearActiveID();
            ImGui::SetWindowPos(this->position);
            ImGui::SetWindowSize(this->size);
        }
    }
}

void IAssetsPanel::draw() {
    this->handle_events();

    ImGui::SeparatorText("Layers");
    // TODO: Reserve names in an internal state
    std::shared_ptr<program::IAcquisitorService> acquisitor = program::ServiceManager::get<program::IAcquisitorService>();
    std::vector<std::weak_ptr<toolbox::Asset>> *assets = acquisitor->getAssets();
    ImVec2 currSize = ImGui::GetWindowSize();

    for(short i = 0; i < (*assets).size(); i++) {
        if (std::shared_ptr<toolbox::Asset> asset = (*assets)[i].lock()) {
            std::string asset_name;
            size_t asset_name_pos = asset->path.rfind("\\");

            if (asset_name_pos != std::string::npos) {
                asset_name = asset->path.substr(asset_name_pos + 1);
            } else {
                asset_name = asset->path;
            }

            if (asset) {
                // ImVec2(currSize.x -  10, 90)
                ImGui::BeginChild(asset->path.c_str(), ImVec2(), ImGuiChildFlags_AlwaysAutoResize | ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders, ImGuiWindowFlags_HorizontalScrollbar);
                ImGui::Text("%s", asset_name.c_str());

                if (ImGui::IsItemHovered()) {
                    ImGui::BeginTooltip();
                    ImGui::Text("%s", asset->path.c_str());
                    ImGui::EndTooltip();
                }

                ImGui::Text("Size: %dx%d", asset->original_image.cols, asset->original_image.rows);
                ImGui::Text("Channels: %d", asset->original_image.channels());
                ImGui::Text("Depth: %s", asset->original_image.depth() == 0 ? "CV_8U" :
                                                    asset->original_image.depth() == 1 ? "CV_8S" :
                                                    asset->original_image.depth() == 2 ? "CV_16U" :
                                                    asset->original_image.depth() == 3 ? "CV_16S" :
                                                    asset->original_image.depth() == 4 ? "CV_32S" :
                                                    asset->original_image.depth() == 5 ? "CV_32F" :
                                                    asset->original_image.depth() == 6 ? "CV_64F" : "Unknown");
                ImGui::Text("Total size in bytes: %d", asset->original_image.total());
                ImGui::EndChild();
            }
        }
    }

    // Histogram
    std::shared_ptr<toolbox::Asset> chosenAsset = program::getChosenAsset();
    if (chosenAsset == nullptr)
        return;

    cv::Mat hist;
    float range[] = { 0, 256 }; // The upper boundary is non-inclusive
    const float* histRange[] = { range };
    int channels[] = { 0 };
    int histSize[] = { 256 };
    cv::calcHist(
        &chosenAsset->displayed_image,
        1,
        channels,
        cv::Mat(),
        hist,
        1,
        histSize,
        histRange,
        true,
        false
    );

    double max_val = 0.0;
    // cv::minMaxLoc(hist, 0, &max_val, 0, 0);

    // ImGui::PlotHistogram(
    //     "Histogram",
    //     (float*)hist.data,
    //     histSize[0],
    //     0,
    //     NULL,
    //     0.0f,
    //     (float)max_val,
    //     ImVec2(0, 80)
    // );

    // ImGui::SeparatorText("Image details");

    // Use current ImGui theme colors for plot styling
    ImGuiStyle &style = ImGui::GetStyle();
    ImVec4 plotBg      = style.Colors[ImGuiCol_ChildBg];
    ImVec4 frameBg     = style.Colors[ImGuiCol_WindowBg];
    ImVec4 axisHoverBg = style.Colors[ImGuiCol_TabHovered];
    ImVec4 barFill     = style.Colors[ImGuiCol_SliderGrab];

    ImPlot::PushStyleColor(ImPlotCol_PlotBg, plotBg);
    ImPlot::PushStyleColor(ImPlotCol_FrameBg, frameBg);
    ImPlot::PushStyleColor(ImPlotCol_AxisBgHovered, axisHoverBg);
    ImPlot::PushStyleColor(ImPlotCol_Fill, barFill);

    float *histogram_data = (float *) hist.data;
    int hist_size = hist.rows;

    ImVec2 plot_size = ImVec2(ImGui::GetContentRegionAvail().x, 250.0f);

    if (ImPlot::BeginPlot("#Image histogram", plot_size)) {
        ImPlot::SetupAxes("Intensity", "Count");

        ImPlot::PlotBars("Pixel counts", histogram_data, hist_size, 0.7, 0.0);

        ImPlot::EndPlot();
    }

    // Pop all 4 colors we pushed
    ImPlot::PopStyleColor(4);

    {
        ImGuiStyle &st = ImGui::GetStyle();
        ImVec4 accent = st.Colors[ImGuiCol_CheckMark];
        ImVec4 text   = st.Colors[ImGuiCol_Text];
        ImVec4 muted  = st.Colors[ImGuiCol_TextDisabled];
        auto clamp01 = [](float v){ return v < 0.f ? 0.f : (v > 1.f ? 1.f : v); };
        ImVec4 base = st.Colors[ImGuiCol_WindowBg];
        ImVec4 cardBg = ImVec4(clamp01(base.x + 0.06f), clamp01(base.y + 0.06f), clamp01(base.z + 0.06f), 1.0f);
        // Undone cards use a slightly dimmer solid bg (no transparency to avoid overlap look)
        ImVec4 cardBgInactive = ImVec4(cardBg.x * 0.96f, cardBg.y * 0.96f, cardBg.z * 0.96f, 1.0f);
        ImVec4 cardBorder = st.Colors[ImGuiCol_Border];

        // Header (icon + title)
        ImGui::PushStyleColor(ImGuiCol_Text, accent);
        ImGui::TextUnformatted(ICON_FA_CLOCK_ROTATE_LEFT);
        ImGui::PopStyleColor();
        ImGui::SameLine();
        ImGui::Text("History");

        // Icon resolver (based on exact names coming from operations)
        auto pick_icon = [&](const std::string &name)->const char* {
            if (name == "Load Image") return ICON_FA_IMAGE;
            if (name == "Grayscale") return ICON_FA_CIRCLE_HALF_STROKE;
            if (name == "Histogram Equalization") return ICON_FA_CHART_SIMPLE;
            if (name == "Contrast Stretch") return ICON_FA_CHART_SIMPLE;
            if (name == "Rotate") return ICON_FA_ARROW_ROTATE_RIGHT;
            if (name == "Scale") return ICON_FA_MAXIMIZE;
            if (name == "Move") return ICON_FA_ARROWS_UP_DOWN_LEFT_RIGHT;
            if (name == "Skew") return ICON_FA_ARROWS_LEFT_RIGHT;
            if (name == "Flip") return ICON_FA_ARROWS_LEFT_RIGHT;
            if (name == "Crop") return ICON_FA_CROP;
            return ICON_FA_CIRCLE;
        };

        const float history_height = 220.0f;
        const float card_rounding = st.FrameRounding > 0.0f ? st.FrameRounding : 6.0f;
        const float card_pad = 10.0f;
        const float spacing = 8.0f;
        const float card_h = 78.0f;        // Increased for scope line
        const float time_offset_y = 22.0f; // Timestamp line offset

        // Ensure scrollbar colors follow theme within this panel
        ImGui::PushStyleColor(ImGuiCol_ScrollbarBg, st.Colors[ImGuiCol_ChildBg]);
        ImGui::PushStyleColor(ImGuiCol_ScrollbarGrab, st.Colors[ImGuiCol_SliderGrab]);
        ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabHovered, st.Colors[ImGuiCol_SliderGrabActive]);
        ImGui::PushStyleColor(ImGuiCol_ScrollbarGrabActive, st.Colors[ImGuiCol_SliderGrabActive]);

        // Track current item for scroll-to-current behavior on undo/redo
        static std::string s_lastAssetId;
        static int s_lastScrollIndex = -1;
        std::string assetId = chosenAsset->path;
        if (assetId != s_lastAssetId) { s_lastAssetId = assetId; s_lastScrollIndex = -1; }

        if (ImGui::BeginChild("##history_cards", ImVec2(0, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_AlwaysUseWindowPadding)) {
            int current = chosenAsset->history_index;
            ImDrawList* dl = ImGui::GetWindowDrawList();

            // Newest (top) to oldest (bottom)
            const int N = (int)chosenAsset->history.size();
            for (int i = 0; i < N; ++i) {
                int histIdx = N - 1 - i; // reverse index
                const auto &snap = chosenAsset->history[histIdx];
                // timestamp
                std::time_t tt = std::chrono::system_clock::to_time_t(snap.timestamp);
                char time_buf[32];
                std::strftime(time_buf, sizeof(time_buf), "%H:%M:%S", std::localtime(&tt));

                bool is_applied = (histIdx <= current);
                bool is_current = (histIdx == current);

                // A real child per card avoids any overlap and preserves spacing
                ImGui::PushID(histIdx);
                ImGui::PushStyleColor(ImGuiCol_ChildBg, is_applied ? cardBg : cardBgInactive);
                if (ImGui::BeginChild("##history_card", ImVec2(0, 0), ImGuiChildFlags_Borders | ImGuiChildFlags_AutoResizeY)) {
                    // Smaller text scale for compact cards
                    ImGui::SetWindowFontScale(0.85f);

                    // Begin content with padding
                    ImGui::SetCursorPos(ImVec2(card_pad, card_pad));

                    // Status icon
                    ImGui::PushStyleColor(ImGuiCol_Text, is_applied ? accent : muted);
                    ImGui::TextUnformatted(is_applied ? ICON_FA_CIRCLE_CHECK : ICON_FA_CIRCLE);
                    ImGui::PopStyleColor();
                    ImGui::SameLine();

                    // Operation icon + label (wrapped)
                    std::string label = snap.label.empty() ? std::string("Edit") : snap.label;
                    const char* op_icon = pick_icon(label);
                    float wrap_width = ImGui::GetContentRegionAvail().x - card_pad;
                    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + wrap_width);
                    if (!is_applied) ImGui::PushStyleColor(ImGuiCol_Text, muted);
                    ImGui::Text("%s  %s", op_icon, label.c_str());
                    if (!is_applied) ImGui::PopStyleColor();
                    ImGui::PopTextWrapPos();

                    // Timestamp line
                    ImGui::PushStyleColor(ImGuiCol_Text, muted);
                    ImGui::TextUnformatted(time_buf);
                    ImGui::PopStyleColor();

                    // Scope line (ROI start + size or Asset)
                    std::string fileName;
                    {
                        size_t pos = chosenAsset->path.find_last_of("/\\");
                        fileName = (pos == std::string::npos) ? chosenAsset->path : chosenAsset->path.substr(pos + 1);
                    }
                    ImGui::PushTextWrapPos(ImGui::GetCursorPosX() + wrap_width);
                    if (snap.applied_to_roi && snap.roi_w > 0 && snap.roi_h > 0) {
                        ImGui::PushStyleColor(ImGuiCol_Text, accent);
                        ImGui::Text("ROI %s start:(%d,%d) size:(%dx%d)", fileName.c_str(), snap.roi_x, snap.roi_y, snap.roi_w, snap.roi_h);
                        ImGui::PopStyleColor();
                    } else {
                        ImGui::PushStyleColor(ImGuiCol_Text, muted);
                        ImGui::Text("Asset %s", fileName.c_str());
                        ImGui::PopStyleColor();
                    }
                    ImGui::PopTextWrapPos();

                    // Restore font scale
                    ImGui::SetWindowFontScale(1.0f);

                    // Chevron indicator (overlay top-right)
                    if (is_current) {
                        ImVec2 childSize = ImGui::GetWindowSize();
                        float chevron_w = ImGui::CalcTextSize(ICON_FA_CHEVRON_RIGHT).x;
                        ImGui::SetCursorPos(ImVec2(childSize.x - card_pad - chevron_w, card_pad));
                        ImGui::PushStyleColor(ImGuiCol_Text, accent);
                        ImGui::TextUnformatted(ICON_FA_CHEVRON_RIGHT);
                        ImGui::PopStyleColor();
                    }
                }
                ImGui::EndChild();
                ImGui::PopStyleColor();

                // Click to jump to this history state
                if (ImGui::IsItemClicked()) {
                    chosenAsset->history_index = histIdx;
                    const auto &s = chosenAsset->history[histIdx];
                    chosenAsset->base_image = s.base_image.clone();
                    chosenAsset->transformation = s.transformation.clone();
                    chosenAsset->dirty = true;
                    program::WindowState::textureUpdate = true;
                }

                // Highlight current with accent border overlay
                if (is_current) {
                    ImVec2 rmin = ImGui::GetItemRectMin();
                    ImVec2 rmax = ImGui::GetItemRectMax();
                    dl->AddRect(rmin, rmax, ImGui::GetColorU32(accent), card_rounding, 0, 2.0f);
                    if (s_lastScrollIndex != histIdx) {
                        ImGui::SetScrollHereY(0.15f);
                        s_lastScrollIndex = histIdx;
                    }
                }

                ImGui::PopID();
                ImGui::Dummy(ImVec2(0, spacing));
            }
        }
        ImGui::EndChild();
        ImGui::PopStyleColor(4);
    }
}
