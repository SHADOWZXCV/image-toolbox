#include "panels/assets.hpp"

void IAssetsPanel::pre_draw() {}
bool IAssetsPanel::show_condition() {
    // TODO: Reserve names in an internal state
    program::IAcquisitorService* acquisitor = static_cast<program::IAcquisitorService*>(program::ServiceManager::getByName("acquisitor_service"));
    std::vector<toolbox::Asset *> *assets = acquisitor->getAssets();

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
    program::IAcquisitorService* acquisitor = static_cast<program::IAcquisitorService*>(program::ServiceManager::getByName("acquisitor_service"));
    std::vector<toolbox::Asset *> *assets = acquisitor->getAssets();
    ImVec2 currSize = ImGui::GetWindowSize();
    for(short i = 0; i < (*assets).size(); i++) {
        toolbox::Asset *asset = (*assets)[i];
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
