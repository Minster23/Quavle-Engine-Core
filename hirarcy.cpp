#include "interface.h"
#include "entity.h"
#include "scene.h"
#include "random.h"
#include <imgui.h>

using namespace interface;
using namespace entity;
using namespace scene;
using namespace entity::canvas;

std::string interfaces::componentsInterfaces::selecetedObject = "";

bool interfaces::componentsInterfaces::hirarcy_ = true;

void interfaces::componentsInterfaces::drawTree(const char* UID)
{
    auto& data = components::entity_data[UID];

    ImGuiTreeNodeFlags flags =
        ImGuiTreeNodeFlags_OpenOnArrow |
        ImGuiTreeNodeFlags_OpenOnDoubleClick;

    if (selecetedObject == UID)
        flags |= ImGuiTreeNodeFlags_Selected;

    bool hasParent = !components::entity_data[UID].Parent.empty();

    if (hasParent)
    {
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.9f, 0.8f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(1.0f, 0.9f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.9f, 0.8f, 0.2f, 1.0f));

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.6f, 0.3f, 0.9f, 1.0f));
    }

    if(componentss.entity_data[UID].typeObject == componentss.HOLDER)
    {
        ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.9f, 0.8f, 0.2f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(1.0f, 0.9f, 0.3f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_HeaderActive, ImVec4(0.9f, 0.8f, 0.2f, 1.0f));

        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(92.0f, 234.0f, 255.0f, 1.0f));
    }

    bool opened = ImGui::TreeNodeEx(UID, flags, "%s", data.Name.c_str());

    if (componentss.entity_data[UID].typeObject == componentss.HOLDER)
    {
        ImGui::PopStyleColor(4);
    }
    if (hasParent)
    {
        ImGui::PopStyleColor(4);
    }
    if (ImGui::IsItemClicked())
    {
        selecetedObject = UID;
    }
    if (opened)
    {
        for (const auto& obj : components::entity_data)
        {
            if (obj.second.Parent == UID)
                drawTree(obj.first.c_str());
        }
        ImGui::TreePop();
    }
}

void interfaces::componentsInterfaces::hirarcy()
{
	if(hirarcy_)
	{
		ImGui::Begin("Hirarcy", &hirarcy_);
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Left) && !ImGui::IsWindowHovered())
        {
            //selecetedObject = "";
        }
		ImGui::BeginChild("HirarcyChild", ImVec2(0, 0), false, ImGuiWindowFlags_HorizontalScrollbar);
		for (const auto& obj : components::entity_data)
		{
			ImGui::PushID(obj.first.c_str());
			if(obj.second.Parent == "" && obj.first != "ENGINE_CAM" && scenesHandler.canObjectShown(obj.first.c_str()))
				drawTree(obj.first.c_str());
			ImGui::PopID();
		}
		ImGui::EndChild();
        if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(1))
        {
            ImGui::OpenPopup("More_Info");
        }
        if (ImGui::BeginPopup("More_Info"))
        {
            ImGui::SeparatorText("Menu");
            if (ImGui::BeginMenu("Create Object"))
            {
                if (ImGui::MenuItem("Empty Object"))
                {
                    gameobjects.addObject(GenerateRandomID().c_str(), "Empty Object", components::EMPTY);
                }
                if (ImGui::MenuItem("Holder"))
                {
                    gameobjects.addObject(GenerateRandomID().c_str(), "Holder", components::HOLDER);
                }
                if (ImGui::MenuItem("Add Light"))
                {
                    gameobjects.addObject(GenerateRandomID().c_str(), "Light", componentss.type::LIGHT);
                }
                if (ImGui::MenuItem("Add Canvas"))
                {
                    canvasItems.addItem(GenerateRandomID().c_str(), "Holder Canvas", canvasItems.HOLDER, "");
                }
                if (ImGui::MenuItem("Add Mesh"))
                {
                    gameobjects.addObject(GenerateRandomID().c_str(), "Object", componentss.type::OBJECT);
                }
                ImGui::EndMenu();
            }
            ImGui::EndPopup();
        }
		ImGui::End();
	}
}