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

    // ===== Active Camera =====
    components::Camera* activeCam = nullptr;
    for (auto& [UUID, cam] : entity::components::entity_camera)
    {
        if (cam.isActive)
        {
            activeCam = &cam;
            break;
        }
    }
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

    ImGui::RadioButton("Translate", op == ImGuizmo::TRANSLATE);
    ImGui::SameLine();
    ImGui::RadioButton("Rotate", op == ImGuizmo::ROTATE);
    ImGui::SameLine();
    ImGui::RadioButton("Scale", op == ImGuizmo::SCALE);

    ImGui::SameLine();
    ImGui::RadioButton("Local", mode == ImGuizmo::LOCAL);
    ImGui::SameLine();
    ImGui::RadioButton("World", mode == ImGuizmo::WORLD);

    // ===== Fullscreen Rect for Gizmo =====
    int w, h;
    glfwGetWindowSize(glfwGetCurrentContext(), &w, &h);

    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2((float)w, (float)h));
    ImGui::Begin("##GuizmoRoot", nullptr,
        ImGuiWindowFlags_NoBackground |
        ImGuiWindowFlags_NoTitleBar |
        ImGuiWindowFlags_NoInputs |
        ImGuiWindowFlags_NoMove |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoSavedSettings
    );
    ImGuizmo::SetDrawlist();
    ImGui::End();

    ImGuizmo::SetRect(0, 0, (float)w, (float)h);

    // ===== Manipulate =====
    if (ImGuizmo::Manipulate(
        glm::value_ptr(view),
        glm::value_ptr(proj),
        op,
        mode,
        glm::value_ptr(model)
    ))
    {
        // ----- Root Object -----
        if (data.Parent.empty())
        {
            float mT[3], mR[3], mS[3];
            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(model), mT, mR, mS);

            t.location = { mT[0], mT[1], mT[2] };
            t.rotation = { mR[0], mR[1], mR[2] };
            t.scale = { mS[0], mS[1], mS[2] };
        }
        else
        {
            // ----- Child Object (convert World -> Local) -----
            const auto& parentID = data.Parent;
            auto& parentTr = components::entity_transform[parentID];

            glm::mat4 parentInv = glm::inverse(parentTr.worldMatrix);
            glm::mat4 localMtx = parentInv * model;

            float mT[3], mR[3], mS[3];
            ImGuizmo::DecomposeMatrixToComponents(glm::value_ptr(localMtx), mT, mR, mS);

            t.location = { mT[0], mT[1], mT[2] };
            t.rotation = { mR[0], mR[1], mR[2] };
            t.scale = { mS[0], mS[1], mS[2] };
        }
    }
}
