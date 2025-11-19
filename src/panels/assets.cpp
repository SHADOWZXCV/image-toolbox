#include "panels/assets.hpp"

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
}
