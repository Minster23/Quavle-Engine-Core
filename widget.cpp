#include <glad/glad.h>
#include <imgui.h>
#include <format>
#include <string_view>

#include "entity.h"
#include "interface.h"
#include "random.h"

using namespace entity::canvas;
using namespace entity;
using namespace interface;

namespace
{
    void displayTexture(GLuint textureId, std::string_view label)
    {
        ImGui::BeginGroup();
        ImGui::Image((void*)(uintptr_t)textureId, ImVec2(80, 80), ImVec2(0, 1), ImVec2(1, 0));
        // Center label under the thumbnail
        float labelWidth = ImGui::CalcTextSize(label.data()).x;
        ImGui::SetCursorPosX(ImGui::GetCursorPosX() + (80.0f - labelWidth) * 0.5f);
        ImGui::TextDisabled("%s", label.data());
        ImGui::EndGroup();
    }
}

void components::Data::renderUI()
{
    if (ImGui::CollapsingHeader("Data", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Spacing();
        ImGui::Indent();

        // -- Editable --
        static char nameBuffer[256] = { 0 };
        std::copy_n(Name.begin(), std::min(Name.size(), sizeof(nameBuffer) - 1), nameBuffer);
        nameBuffer[std::min(Name.size(), sizeof(nameBuffer) - 1)] = '\0';

        ImGui::SetNextItemWidth(-1);
        if (ImGui::InputText("##Name", nameBuffer, sizeof(nameBuffer)))
            Name = nameBuffer;
        ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Name");

        ImGui::Spacing();
        ImGui::Checkbox("Visible", &show_);

        // -- Read-only identity --
        ImGui::SeparatorText("Identity");

        ImGui::BeginDisabled();
        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##UUID", UUID.data(), UUID.size(), ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("UUID");

        ImGui::SetNextItemWidth(-1);
        ImGui::InputText("##Parent", Parent.data(), Parent.size(), ImGuiInputTextFlags_ReadOnly);
        ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Parent");

        const char* typeNames[] = { "Object", "Light", "Camera", "Holder", "Empty", "Audio", "Other" };
        int currentType = static_cast<int>(typeObject);
        ImGui::SetNextItemWidth(-1);
        ImGui::Combo("##Type", &currentType, typeNames, IM_ARRAYSIZE(typeNames));
        ImGui::SameLine(0, ImGui::GetStyle().ItemInnerSpacing.x);
        ImGui::AlignTextToFramePadding();
        ImGui::TextUnformatted("Type");
        ImGui::EndDisabled();

        ImGui::Unindent();
        ImGui::Spacing();
    }
}

void components::Texture::renderUI()
{
    if (ImGui::CollapsingHeader("Textures", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Spacing();

        std::unordered_map<std::string, GLuint> textureMap = {
            {"Diffuse",      this->Diffuse},
            {"Specular",     this->Specular},
            {"Metalness",    this->Metalness},
            {"Normal",       this->Normal},
            {"Height",       this->Height},
            {"Ambient",      this->Ambient},
            {"Emissive",     this->emissive},
            {"Opacity",      this->opacity},
            {"Displacement", this->displacement}
        };

        constexpr ImGuiTableFlags tableFlags =
            ImGuiTableFlags_BordersOuter |
            ImGuiTableFlags_BordersInnerV |
            ImGuiTableFlags_RowBg |
            ImGuiTableFlags_SizingFixedFit;

        if (ImGui::BeginTable("TextureTable", 2, tableFlags))
        {
            ImGui::TableSetupColumn("Preview", ImGuiTableColumnFlags_WidthFixed, 88.0f);
            ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthStretch);
            ImGui::TableHeadersRow();

            for (const auto& [label, textureId] : textureMap)
            {
                if (textureId != 0)
                {
                    ImGui::TableNextRow(ImGuiTableRowFlags_None, 96.0f);

                    ImGui::TableSetColumnIndex(0);
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() + 4.0f);
                    displayTexture(textureId, label);

                    ImGui::TableSetColumnIndex(1);
                    ImGui::SetCursorPosY(ImGui::GetCursorPosY() +
                        (96.0f - ImGui::GetTextLineHeight()) * 0.5f);
                    ImGui::TextUnformatted(label.c_str());
                }
            }
            ImGui::EndTable();
        }

        ImGui::Spacing();
    }
}

void components::Transform::renderUI()
{
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Spacing();
        ImGui::Indent();

        ImGui::SeparatorText("Location");
        ImGui::SetNextItemWidth(-1);
        ImGui::DragFloat3("##Location", glm::value_ptr(location), 0.1f);

        ImGui::SeparatorText("Rotation");
        ImGui::SetNextItemWidth(-1);
        ImGui::DragFloat3("##Rotation", glm::value_ptr(rotation), 1.0f, -180.0f, 180.0f, "%.1f deg");

        ImGui::SeparatorText("Scale");
        ImGui::SetNextItemWidth(-1);
        ImGui::DragFloat3("##Scale", glm::value_ptr(scale), 0.01f, 0.01f, 10.0f);

        ImGui::Unindent();
        ImGui::Spacing();
    }
}

void components::Material::renderUI()
{
    if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Spacing();
        ImGui::Indent();

        ImGui::SeparatorText("Base Color");
        ImGui::SetNextItemWidth(-1);
        ImGui::ColorEdit3("##BaseColor", glm::value_ptr(baseColor));

        ImGui::SeparatorText("PBR Properties");
        ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Emissive Power", &emissivePower, 0.0f, 10.0f, "%.1f");
        ImGui::SliderFloat("Normal Strength", &normalStrength, 0.0f, 10.0f, "%.1f");

        ImGui::Unindent();
        ImGui::Spacing();
    }
}

void components::Light::renderUI()
{
    if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Spacing();
        ImGui::Indent();

        // -- Type --
        const char* lightTypes[] = { "Directional", "Point", "Spot" };
        int currentType = static_cast<int>(lightType);
        if (ImGui::Combo("Type", &currentType, lightTypes, IM_ARRAYSIZE(lightTypes)))
            lightType = static_cast<LightType>(currentType);

        // -- Appearance --
        ImGui::SeparatorText("Appearance");
        ImGui::ColorEdit3("Color", glm::value_ptr(color));
        ImGui::SliderFloat("Intensity", &intensity, 0.0f, 10.0f, "%.1f");

        // -- Direction (Directional / Spot) --
        if (lightType != POINT)
        {
            ImGui::SeparatorText("Direction");
            ImGui::SetNextItemWidth(-1);
            ImGui::DragFloat3("##Direction", glm::value_ptr(direction), 0.01f, -1.0f, 1.0f);
        }

        // -- Range (Point / Spot) --
        if (lightType != DIRECTIONAL)
        {
            ImGui::SeparatorText("Attenuation");
            ImGui::SliderFloat("Range", &range, 0.1f, 100.0f, "%.1f m");
        }

        // -- Cutoff (Spot only) --
        if (lightType == SPOT)
        {
            ImGui::SeparatorText("Spotlight Cone");
            ImGui::SliderAngle("Inner Cutoff", &innerCutoff, 0.0f, 90.0f);
            ImGui::SliderAngle("Outer Cutoff", &outerCutoff, 0.0f, 90.0f);

            float innerDeg = glm::degrees(innerCutoff);
            float outerDeg = glm::degrees(outerCutoff);
            ImGui::TextDisabled("%.1f\xb0 inner  /  %.1f\xb0 outer", innerDeg, outerDeg);
        }

        // -- Presets --
        ImGui::SeparatorText("Presets");

        const float btnWidth = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x * 2) / 3.0f;

        if (ImGui::Button("Sun", ImVec2(btnWidth, 0)))
        {
            lightType = DIRECTIONAL;
            color = glm::vec3(1.0f, 0.95f, 0.9f);
            intensity = 1.0f;
            direction = glm::vec3(-0.2f, -1.0f, -0.3f);
        }
        ImGui::SameLine();
        if (ImGui::Button("Bulb", ImVec2(btnWidth, 0)))
        {
            lightType = POINT;
            color = glm::vec3(1.0f, 0.9f, 0.8f);
            intensity = 2.0f;
            range = 10.0f;
        }
        ImGui::SameLine();
        if (ImGui::Button("Flashlight", ImVec2(btnWidth, 0)))
        {
            lightType = SPOT;
            color = glm::vec3(1.0f, 1.0f, 0.95f);
            intensity = 3.0f;
            range = 15.0f;
            innerCutoff = glm::radians(12.5f);
            outerCutoff = glm::radians(30.0f);
        }

        ImGui::Unindent();
        ImGui::Spacing();
    }
}

void components::Camera::renderUI()
{
    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Spacing();
        ImGui::Indent();

        // -- Projection --
        ImGui::SeparatorText("Projection");
        ImGui::SliderFloat("Field of View", &fov, 1.0f, 120.0f, "%.0f\xb0");
        ImGui::DragFloat("Near Plane", &nearPlane, 0.01f, 0.01f, 100.0f, "%.2f");
        ImGui::DragFloat("Far Plane", &farPlane, 1.0f, 1.0f, 10000.0f, "%.0f");

        // -- Flags --
        ImGui::SeparatorText("Flags");
        ImGui::Checkbox("Active", &isActive);
        ImGui::SameLine();
        ImGui::Checkbox("Main Camera", &isMain);

        // -- Info --
        ImGui::SeparatorText("Info");
        ImGui::TextDisabled("Aspect  %.2f : 1", aspect);
        ImGui::TextDisabled("Resolution  %.0f x %.0f", aspect * 100.0f, 100.0f);

        ImGui::Unindent();
        ImGui::Spacing();
    }
}

void components::Audio::renderUI()
{
    if (ImGui::CollapsingHeader("Audio", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Spacing();
        ImGui::Indent();

        // -- File --
        ImGui::SeparatorText("File");

        static char audioPathBuffer[256] = { 0 };
        std::copy_n(audioPath.begin(), std::min(audioPath.size(), sizeof(audioPathBuffer) - 1), audioPathBuffer);
        audioPathBuffer[std::min(audioPath.size(), sizeof(audioPathBuffer) - 1)] = '\0';

        const float browseWidth = ImGui::CalcTextSize("Browse...").x + ImGui::GetStyle().FramePadding.x * 2;
        ImGui::SetNextItemWidth(-browseWidth - ImGui::GetStyle().ItemSpacing.x);
        if (ImGui::InputText("##AudioFile", audioPathBuffer, sizeof(audioPathBuffer)))
            audioPath = audioPathBuffer;

        ImGui::SameLine();
        if (ImGui::Button("Browse..."))
        {
            // TODO: Implement file browser dialog
        }

        // -- Playback Controls --
        ImGui::SeparatorText("Playback");

        const float ctrlWidth = (ImGui::GetContentRegionAvail().x
            - ImGui::GetStyle().ItemSpacing.x * 2) / 3.0f;

        if (ImGui::Button(isPlaying ? "Pause" : "Play", ImVec2(ctrlWidth, 0)))
            play = !isPlaying;

        ImGui::SameLine();
        if (ImGui::Button("Stop", ImVec2(ctrlWidth, 0)))
        {
            play = false;
            isPlaying = false;
            progress.currentTime = 0.0f;
        }

        ImGui::SameLine();
        ImGui::Checkbox("Loop", &loop);

        // -- Properties --
        ImGui::SeparatorText("Properties");
        ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f, "%.2f");
        ImGui::Checkbox("Spatial Audio", &spatial);

        // -- Progress --
        ImGui::SeparatorText("Progress");
        if (progress.duration > 0.0f)
        {
            float progressFraction = progress.currentTime / progress.duration;
            ImGui::SetNextItemWidth(-1);
            ImGui::ProgressBar(progressFraction, ImVec2(-1, 0));

            ImGui::Spacing();
            ImGui::TextDisabled("%.2fs", progress.currentTime);
            ImGui::SameLine(ImGui::GetContentRegionAvail().x * 0.5f + ImGui::GetCursorPosX() - 8.0f);
            ImGui::TextDisabled("/ %.2fs", progress.duration);
        }
        else
        {
            ImGui::TextDisabled("No audio loaded");
        }

        ImGui::Unindent();
        ImGui::Spacing();
    }
}

void item::Text::renderUI()
{
    if (ImGui::CollapsingHeader("Text Properties", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Spacing();
        ImGui::Indent();

        ImGui::SeparatorText("Content");

        static char textBuffer[1024] = { 0 };
        std::copy_n(content.begin(), std::min(content.size(), sizeof(textBuffer) - 1), textBuffer);
        textBuffer[std::min(content.size(), sizeof(textBuffer) - 1)] = '\0';

        ImGui::SetNextItemWidth(-1);
        if (ImGui::InputTextMultiline("##Content", textBuffer, sizeof(textBuffer),
            ImVec2(-1, ImGui::GetTextLineHeight() * 4)))
        {
            content = textBuffer;
        }

        ImGui::SeparatorText("Style");
        ImGui::SliderFloat("Scale", &scale, 0.1f, 10.0f, "%.2f");
        ImGui::ColorEdit3("Color", glm::value_ptr(color));

        ImGui::Unindent();
        ImGui::Spacing();
    }
}

void item::container::renderUI()
{
    if (ImGui::CollapsingHeader("Container", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Spacing();
        ImGui::Indent();

        ImGui::SeparatorText("Identity");
        ImGui::TextDisabled("%s", UUID.c_str());

        ImGui::SeparatorText("Items");
        ImGui::Checkbox("Overlay Editor", &overlayEditor);

        ImGui::Spacing();

        const float btnWidth = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x) / 2.0f;

        if (ImGui::Button("Add Text", ImVec2(btnWidth, 0)))
        {
            std::string entityId = GenerateRandomID();
            canvasItems.addItem(entityId.c_str(), "Text", canvasItems.TEXT, UUID.c_str());
            itemListID.push_back(entityId);
        }
        ImGui::SameLine();
        if (ImGui::Button("Add Empty", ImVec2(btnWidth, 0)))
        {
            std::string entityId = GenerateRandomID();
            canvasItems.addItem(entityId.c_str(), "Empty", canvasItems.HOLDER, UUID.c_str());
            itemListID.push_back(entityId);
        }

        ImGui::Unindent();
        ImGui::Spacing();
    }
}

void item::position::renderUI()
{
    if (ImGui::CollapsingHeader("Position", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Spacing();
        ImGui::Indent();

        ImGui::SeparatorText("Screen Position");
        ImGui::DragFloat("X", &x, 1.0f, -1000.0f, 1000.0f, "%.0f px");
        ImGui::DragFloat("Y", &y, 1.0f, -1000.0f, 1000.0f, "%.0f px");

        ImGui::SeparatorText("Presets");

        const float btnWidth = (ImGui::GetContentRegionAvail().x - ImGui::GetStyle().ItemSpacing.x * 2) / 3.0f;

        if (ImGui::Button("Center", ImVec2(btnWidth, 0)))
        {
            x = 0.0f;
            y = 0.0f;
        }
        ImGui::SameLine();
        if (ImGui::Button("Top Left", ImVec2(btnWidth, 0)))
        {
            x = -400.0f;
            y = 300.0f;
        }
        ImGui::SameLine();
        if (ImGui::Button("Bot Right", ImVec2(btnWidth, 0)))
        {
            x = 400.0f;
            y = -300.0f;
        }

        ImGui::Unindent();
        ImGui::Spacing();
    }
}