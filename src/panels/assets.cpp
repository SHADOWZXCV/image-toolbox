#include "panels/assets.hpp"

void IAssetsPanel::pre_draw() {}
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
    ImVec2 currPosition = ImGui::GetWindowPos();
    ImVec2 currSize = ImGui::GetWindowSize();

    // Display window position and size information
    // ImGui::BeginChild("Window Info", ImVec2(currSize.x, 130));
    // ImGui::Text("Current Position: (%.1f, %.1f)", currPosition.x, currPosition.y);
    // ImGui::Text("Current Size: (%.1f, %.1f)", currSize.x, currSize.y);
    // ImGui::Text("Previous Position: (%.1f, %.1f)", this->prevPosition.x, this->prevPosition.y);
    // ImGui::Text("Previous Size: (%.1f, %.1f)", this->prevSize.x, this->prevSize.y);
    // ImGui::EndChild();


    ImGui::SeparatorText("Layers");

    program::IAcquisitorService* acquisitor = static_cast<program::IAcquisitorService*>(program::ServiceManager::getByName("acquisitor_service"));
    std::vector<toolbox::Asset *> *assets = acquisitor->getAssets();
    for(short i = 0; i < (*assets).size(); i++) {
        toolbox::Asset *asset = (*assets)[i];

        if (asset) {
            ImGui::BeginChild(asset->path.c_str(), ImVec2(this->size.x - 10, 80), ImGuiChildFlags_AutoResizeY | ImGuiChildFlags_Borders);
            ImGui::Text(asset->path.c_str());
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.6f), "Size: %dx%d", asset->image.cols, asset->image.rows);
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.6f), "Channels: %d", asset->image.channels());
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.6f), "Depth: %d", asset->image.depth());
            ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.6f), "Total size in bytes: %d", asset->image.total());
            ImGui::EndChild();
        }
    }
    //     ImGui::SeparatorText("Layers");

    // // Get the current preview image's properties
    // program::IAcquisitorService* acquisitor = static_cast<program::IAcquisitorService*>(program::ServiceManager::getByName("acquisitor_service"));
    // toolbox::Asset *asset = acquisitor->getLatestAsset();

    // if (asset) {
    //     ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.6f), "Size: %dx%d", asset->image.cols, asset->image.rows);
    //     ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.6f), "Channels: %d", asset->image.channels());
    //     ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.6f), "Depth: %d", asset->image.depth());
    //     ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, 0.6f), "Total size in bytes: %d", asset->image.total());
    // }
}
