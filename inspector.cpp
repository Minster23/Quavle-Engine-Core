#include "interface.h"
#include "entity.h"
#include "environment.h"
#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

using namespace interface;
using namespace entity;
using namespace entity::canvas;
using namespace environment;

bool interfaces::componentsInterfaces::inspector_ = true;

void setComponents(const std::string& Data_Id)
{
    if (ImGui::Button("Add Components"))
        ImGui::OpenPopup("Components");

    if (ImGui::BeginPopup("Components"))
    {
        for (int i = 0; i < components::ComponentsList.size(); i++)
        {
            ImGui::PushID(i);

            auto& item = components::ComponentsList[i];

            if (ImGui::MenuItem(item.name.c_str()))
            {
                item.add(Data_Id);
                ImGui::CloseCurrentPopup();
            }

            ImGui::PopID();
        }
        ImGui::EndPopup();
    }
}

void interfaces::componentsInterfaces::inspector()
{
    if(inspector_)
    {
        ImGui::Begin("Inspector", &inspector_);
        if (components::entity_data.count(interfaces::componentsInterfaces::selecetedObject))
        {
            for (int i = 0; i < components::ComponentsList.size(); i++)
            {
                ImGui::PushID(i);
                auto& item = components::ComponentsList[i];
                item.callRender(interfaces::componentsInterfaces::selecetedObject);
				ImGui::PopID();
            }

			setComponents(interfaces::componentsInterfaces::selecetedObject);
        }
        else
        {
			ImGui::TextDisabled("No object selected");
        }

        ImGui::End();
    }
}