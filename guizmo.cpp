#include <imgui.h>
#include <ImGuizmo.h>
#include "window.h"
#include "interface.h"
#include "entity.h"

using namespace interface;
using namespace entity;
using namespace window;

void interfaces::componentsInterfaces::guizmo()
{
    ImGuizmo::BeginFrame();
    if (selecetedObject.empty()) return;

    components::Camera* activeCam = gameobjects.activeCamera;
    if (!activeCam) return;

    glm::mat4 view = activeCam->viewMatrix;
    glm::mat4 proj = activeCam->projectionMatrix;

    auto& t = components::entity_transform[selecetedObject];
    const auto& data = components::entity_data[selecetedObject];
    glm::mat4 model = t.worldMatrix;
    static ImGuizmo::OPERATION op = ImGuizmo::TRANSLATE;
    static ImGuizmo::MODE mode = ImGuizmo::LOCAL;

    if (ImGui::IsKeyPressed(ImGuiKey_Z)) op = ImGuizmo::TRANSLATE;
    if (ImGui::IsKeyPressed(ImGuiKey_E)) op = ImGuizmo::ROTATE;
    if (ImGui::IsKeyPressed(ImGuiKey_R)) op = ImGuizmo::SCALE;

    ImGuizmo::SetDrawlist(ImGui::GetWindowDrawList());
    ImGuizmo::SetRect(
        sceneViewport.x,
        sceneViewport.y,
        sceneViewport.width,
        sceneViewport.height
    );

    if (ImGuizmo::Manipulate(
        glm::value_ptr(view),
        glm::value_ptr(proj),
        op, mode,
        glm::value_ptr(model)
    ))
    {
        float mT[3], mR[3], mS[3];

        if (data.Parent.empty())
        {
            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(model), mT, mR, mS);
        }
        else
        {
            glm::mat4 parentInv = glm::inverse(
                components::entity_transform[data.Parent].worldMatrix
            );
            glm::mat4 localMtx = parentInv * model;
            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(localMtx), mT, mR, mS);
        }

        t.location = { mT[0], mT[1], mT[2] };
        t.rotation = { mR[0], mR[1], mR[2] };
        t.scale = { mS[0], mS[1], mS[2] };
    }

    ImVec2 topLeft(
        sceneViewport.x,
        sceneViewport.height - 20
    );

    ImGui::SetNextWindowPos(topLeft, ImGuiCond_Always);
    ImGui::Begin("Guizmo Tool List", nullptr,
        ImGuiWindowFlags_NoDocking |
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_AlwaysAutoResize
    );

    ImGui::End();
}
