#include "panels/assets.hpp"

void IAssetsPanel::pre_draw() {}
bool IAssetsPanel::show_condition() {
    // TODO: Reserve names in an internal state
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
                ImGui::Text(asset_name.c_str());

                if (ImGui::IsItemHovered())
                {
                    ImGui::BeginTooltip();

                    // ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f); // Optional: wrap long text
                    ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.6f), asset->path.c_str());
                    // ImGui::PopTextWrapPos();

                    // 5. End the tooltip window
                    ImGui::EndTooltip();
                }

                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.6f), "Size: %dx%d", asset->original_image.cols, asset->original_image.rows);
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.6f), "Channels: %d", asset->original_image.channels());
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.6f), "Depth: %d", asset->original_image.depth());
                ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.6f), "Total size in bytes: %d", asset->original_image.total());
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

    ImU32 plot_area_color = IM_COL32(20, 20, 20, 255); // Dark grey for the plot
    ImU32 frame_color = IM_COL32(30, 30, 30, 0); // Lighter grey for the frame

    // 2. Push the colors
    ImPlot::PushStyleColor(ImPlotCol_PlotBg, plot_area_color);
    ImPlot::PushStyleColor(ImPlotCol_FrameBg, frame_color);
    ImPlot::PushStyleColor(ImPlotCol_AxisBgHovered, plot_area_color);

    float *histogram_data = (float *) hist.data;
    int hist_size = hist.rows;

    ImVec2 plot_size = ImVec2(ImGui::GetContentRegionAvail().x, 250.0f);

    if (ImPlot::BeginPlot("#Image histogram", plot_size)) {
        ImPlot::SetupAxes("Intensity", "Count");

        ImPlot::PlotBars("Pixel counts", histogram_data, hist_size, 0.7, 0.0);

        ImPlot::EndPlot();
    }

    ImPlot::PopStyleColor();
    ImPlot::PopStyleColor();
}
