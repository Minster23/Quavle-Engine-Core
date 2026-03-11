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
std::string interfaces::componentsInterfaces::detailItemId_ = "";

ImVec2 screenPos;


bool WorldToScreen(
    const glm::vec3& worldPos,
    const glm::mat4& view,
    const glm::mat4& projection,
    const ImVec2& screenSize,
    ImVec2& outPos
)
{
    glm::vec4 clip = projection * view * glm::vec4(worldPos, 1.0f);

    // di belakang kamera
    if (clip.w <= 0.0f)
        return false;

    glm::vec3 ndc = glm::vec3(clip) / clip.w;

    outPos.x = (ndc.x * 0.5f + 0.5f) * screenSize.x;
    outPos.y = (1.0f - (ndc.y * 0.5f + 0.5f)) * screenSize.y;

    return true;
}

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

void interfaces::componentsInterfaces::detailItem()
{
    ImVec2 viewportSize(sceneViewport.width, sceneViewport.height);

    if (detailItemId_ != "")
    {
        if (WorldToScreen(glm::vec3(entity::componentss.entity_transform[detailItemId_].worldMatrix[3]), gameobjects.activeCamera->viewMatrix, gameobjects.activeCamera->projectionMatrix, viewportSize, screenPos))
        {
            screenPos.x += sceneViewport.x;
            screenPos.y += sceneViewport.y;

            ImGui::SetNextWindowPos(screenPos, ImGuiCond_Always);
            ImGui::Begin("Detail Item", nullptr,
                ImGuiWindowFlags_NoDocking |
                ImGuiWindowFlags_NoCollapse |
                ImGuiWindowFlags_NoMove
            );
            ImGui::End();
        }
    }
}

void interfaces::componentsInterfaces::floatingInfo()
{
    ImVec2 viewportSize(sceneViewport.width, sceneViewport.height);

    for (const auto& pair : entity::componentss.entity_data) {

        for (int i = 0; i < components::entity_data.size(); i++)
        {
            if (WorldToScreen(glm::vec3(entity::componentss.entity_transform[pair.first].worldMatrix[3]), gameobjects.activeCamera->viewMatrix, gameobjects.activeCamera->projectionMatrix, viewportSize, screenPos))
            {
                screenPos.x += sceneViewport.x;
                screenPos.y += sceneViewport.y;
                ImGui::GetForegroundDrawList()->AddText(
                    ImGui::GetFont(), ImGui::GetFontSize(),
                    ImVec2(screenPos.x + 15, screenPos.y - 15),
                    IM_COL32(255, 255, 255, 255),
					pair.second.Name.c_str()
                );
            }
        }

    }
}