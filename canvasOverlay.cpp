#include "interface.h"
#include "entity.h"
#include "window.h"
#include "imgui.h"

using namespace interface;
using namespace entity::canvas;
using namespace entity;
using namespace window;

windowsHandler windsHemndlsss;

static std::string containerNow = "";

bool interface::interfaces::componentsInterfaces::canvasOverlay_ = false;

void interface::interfaces::componentsInterfaces::canvasOverlay()
{
    float viewportHeight = windsHemndlsss.height;
    ImGuiFocusedFlags overlyFlag = ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking;

    auto& selectedContainer = entity::canvas::canvasItems.item_container[selecetedObject];
    int a = 0;

    if (selectedContainer.overlayEditor) 
    {
		containerNow = selecetedObject;
    }else
    {
		containerNow = "";
    }

    if(!containerNow.empty())
    {
		selecetedObject = containerNow;
    }

    for (const auto& obj : item::item_itemData)
    {
        if (selectedContainer.overlayEditor == false)
        {
            overlyFlag = ImGuiWindowFlags_NoBackground |
                ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse |
				ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove;
        }

        auto& textData = item::item_text[obj.second.UUID];
        auto& shaderComp = item::item_shader[obj.second.UUID];
        auto& transform = item::item_position[obj.second.UUID];

        a++;
        ImGui::PushID(a);
        ImGui::Begin(
            obj.first.c_str(),
            nullptr,
            overlyFlag

        );

        ImVec2 textPos = ImGui::GetCursorScreenPos();

        transform.x = textPos.x;
        transform.y = viewportHeight - textPos.y;

        if (ImGui::IsWindowFocused())
        {
            ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
            selecetedObject = obj.first;
        }
        if (ImGui::IsWindowHovered && ImGui::IsMouseClicked(1))
        {
            ImGui::OpenPopup("MoreInfo");
        }
        if (ImGui::BeginPopup("MoreInfo")) 
        {
			ImGui::LabelText("UUID", "%s", obj.second.UUID.c_str());
			ImGui::Text("Position: (%.1f, %.1f)", transform.x, transform.y);
			ImGui::Text("Scale: %.1f", textData.scale);
			ImGui::ColorEdit3("Color", (float*)&textData.color, ImGuiColorEditFlags_NoInputs);
        }
        ImGui::End();
		ImGui::PopID();
    }
}