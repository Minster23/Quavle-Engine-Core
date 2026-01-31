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
    GLuint getFBO(GLuint tex)
    {
        GLuint fbo;
        glGenFramebuffers(1, &fbo);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, tex, 0);

        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        {
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glDeleteFramebuffers(1, &fbo);
            return 0;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        return fbo;
    }

    GLuint getActiveCameraFBO()
    {
        if (!gameobjects.activeCamera || gameobjects.activeCamera->fbo == 0)
        {
            return 0;
        }

        return gameobjects.activeCamera->fbo;
    }

    void displayTexture(GLuint textureId, std::string_view label)
    {
        ImGui::BeginGroup();
		ImGui::Image((void*)(uintptr_t)textureId, ImVec2(100, 100), ImVec2(0, 1), ImVec2(1, 0));
        ImGui::EndGroup();
    }
}

void components::Data::renderUI()
{
    if (ImGui::CollapsingHeader("Data", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::BeginGroup();

        static char nameBuffer[256] = { 0 };
        std::copy_n(Name.begin(), std::min(Name.size(), sizeof(nameBuffer) - 1), nameBuffer);
        nameBuffer[std::min(Name.size(), sizeof(nameBuffer) - 1)] = '\0';

        if (ImGui::InputText("Name", nameBuffer, sizeof(nameBuffer)))
        {
            Name = nameBuffer;
        }

        ImGui::BeginDisabled();
        ImGui::InputText("UUID", UUID.data(), UUID.size(), ImGuiInputTextFlags_ReadOnly);
        ImGui::EndDisabled();

        ImGui::BeginDisabled();
        ImGui::InputText("Parent", Parent.data(), Parent.size(), ImGuiInputTextFlags_ReadOnly);
        ImGui::EndDisabled();

        ImGui::Checkbox("Visible", &show_);

        const char* typeNames[] = { "Object", "Light", "Camera", "Holder", "Empty", "Audio", "Other" };
        int currentType = static_cast<int>(typeObject);
        ImGui::BeginDisabled();
        ImGui::Combo("Type", &currentType, typeNames, IM_ARRAYSIZE(typeNames));
        ImGui::EndDisabled();

        ImGui::EndGroup();
    }
}

void components::Texture::renderUI()
{
    if (ImGui::CollapsingHeader("Textures", ImGuiTreeNodeFlags_DefaultOpen))
    {
        std::unordered_map<std::string, GLuint> textureMap = {
            {"Diffuse", this->Diffuse},
            {"Specular", this->Specular},
            {"Metalness", this->Metalness},
            {"Normal", this->Normal},
            {"Height", this->Height},
            {"Ambient", this->Ambient},
            {"Emissive", this->emissive},
            {"Opacity", this->opacity},
            {"Displacement", this->displacement}
		};

        if (ImGui::BeginTable("table1", 3)) 
        {
            ImGui::TableNextRow();
            ImGui::TableSetColumnIndex(0); ImGui::Text("Image");
            ImGui::TableSetColumnIndex(1); ImGui::Text("Type");

            for (const auto& [label, textureId] : textureMap)
            {
                if (textureId != 0)
                {
                    ImGui::TableNextRow();
                    ImGui::TableSetColumnIndex(0);
                    displayTexture(textureId, label);
                    ImGui::TableSetColumnIndex(1);
					ImGui::Text("%s", label.c_str());
                }
            }
            ImGui::EndTable();
        }
    }
}

void components::Transform::renderUI()
{
    if (ImGui::CollapsingHeader("Transform", ImGuiTreeNodeFlags_DefaultOpen))
    {

        ImGui::Text("Location");
        ImGui::DragFloat3("##Location", glm::value_ptr(location), 0.1f);

        ImGui::Spacing();
        ImGui::Text("Rotation");
        ImGui::DragFloat3("##Rotation", glm::value_ptr(rotation), 1.0f, -180.0f, 180.0f, "%.1f deg");

        ImGui::Spacing();
        ImGui::Text("Scale");
        ImGui::DragFloat3("##Scale", glm::value_ptr(scale), 0.01f, 0.01f, 10.0f);

        ImGui::PopItemWidth();
    }
}

void components::Material::renderUI()
{
    if (ImGui::CollapsingHeader("Material", ImGuiTreeNodeFlags_DefaultOpen))
    {

        ImGui::ColorEdit3("Base Color", glm::value_ptr(baseColor));
        ImGui::Separator();

        ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Metallic", &metallic, 0.0f, 1.0f, "%.2f");
        ImGui::SliderFloat("Emissive Power", &emissivePower, 0.0f, 10.0f, "%.1f");
        ImGui::SliderFloat("Normal Strength", &normalStrength, 0.0f, 10.0f, "%.1f");

        ImGui::PopItemWidth();
    }
}

void components::Light::renderUI()
{
    if (ImGui::CollapsingHeader("Light", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushItemWidth(-FLT_MIN * 0.5f);

        // Light type
        const char* lightTypes[] = { "Directional", "Point", "Spot" };
        int currentType = static_cast<int>(lightType);
        if (ImGui::Combo("Type", &currentType, lightTypes, IM_ARRAYSIZE(lightTypes)))
        {
            lightType = static_cast<LightType>(currentType);
        }

        ImGui::Separator();

        // Color and intensity
        ImGui::ColorEdit3("Color", glm::value_ptr(color));
        ImGui::SliderFloat("Intensity", &intensity, 0.0f, 10.0f, "%.1f");

        // Direction (for directional and spot lights)
        if (lightType != POINT)
        {
            ImGui::Separator();
            ImGui::Text("Direction");
            ImGui::DragFloat3("##Direction", glm::value_ptr(direction), 0.01f, -1.0f, 1.0f);
        }

        // Range (for point and spot lights)
        if (lightType != DIRECTIONAL)
        {
            ImGui::Separator();
            ImGui::SliderFloat("Range", &range, 0.1f, 100.0f, "%.1f m");
        }

        // Cutoff angles (for spot lights only)
        if (lightType == SPOT)
        {
            ImGui::Separator();
            ImGui::Text("Spotlight");
            ImGui::SliderAngle("Inner Cutoff", &innerCutoff, 0.0f, 90.0f);
            ImGui::SliderAngle("Outer Cutoff", &outerCutoff, 0.0f, 90.0f);

            // Visual representation of cutoff angles
            float innerDeg = glm::degrees(innerCutoff);
            float outerDeg = glm::degrees(outerCutoff);
            ImGui::Text("Angles: %.1f° / %.1f°", innerDeg, outerDeg);
        }

        // Quick preset buttons
        ImGui::Separator();
        ImGui::Text("Presets:");

        if (ImGui::Button("Sun"))
        {
            lightType = DIRECTIONAL;
            color = glm::vec3(1.0f, 0.95f, 0.9f);
            intensity = 1.0f;
            direction = glm::vec3(-0.2f, -1.0f, -0.3f);
        }

        ImGui::SameLine();
        if (ImGui::Button("Bulb"))
        {
            lightType = POINT;
            color = glm::vec3(1.0f, 0.9f, 0.8f);
            intensity = 2.0f;
            range = 10.0f;
        }

        ImGui::SameLine();
        if (ImGui::Button("Flashlight"))
        {
            lightType = SPOT;
            color = glm::vec3(1.0f, 1.0f, 0.95f);
            intensity = 3.0f;
            range = 15.0f;
            innerCutoff = glm::radians(12.5f);
            outerCutoff = glm::radians(30.0f);
        }

        ImGui::PopItemWidth();
    }
}

void components::Camera::renderUI()
{
    if (ImGui::CollapsingHeader("Camera", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushItemWidth(-FLT_MIN * 0.5f);

        // Camera properties
        ImGui::SliderFloat("Field of View", &fov, 1.0f, 120.0f, "%.0f°");
        ImGui::DragFloat("Near Plane", &nearPlane, 0.01f, 0.01f, 100.0f, "%.2f");
        ImGui::DragFloat("Far Plane", &farPlane, 1.0f, 1.0f, 10000.0f, "%.0f");

        ImGui::Separator();

        // Camera status
        ImGui::Checkbox("Active", &isActive);
        ImGui::SameLine();
        ImGui::Checkbox("Main Camera", &isMain);

        // Aspect ratio info
        ImGui::Separator();
        ImGui::Text("Aspect Ratio: %.2f:1", aspect);
        ImGui::Text("Resolution: %.0f x %.0f", aspect * 100.0f, 100.0f);

        // Preview section
        ImGui::Separator();
        if (ImGui::TreeNode("Preview"))
        {
            GLuint cameraFBO = getActiveCameraFBO();
            if (cameraFBO != 0 && tex != 0)
            {
                ImVec2 previewSize = ImGui::GetContentRegionAvail();
                previewSize.y = previewSize.x / aspect;

                ImGui::Image(reinterpret_cast<void*>(static_cast<intptr_t>(tex)),
                    previewSize,
                    ImVec2(0, 1),
                    ImVec2(1, 0));

                // Preview info
                ImGui::TextDisabled("Camera Feed");
            }
            else
            {
                ImGui::TextColored(ImVec4(1.0f, 0.5f, 0.5f, 1.0f),
                    "No camera feed available");
            }
            ImGui::TreePop();
        }

        ImGui::PopItemWidth();
    }
}

void components::Audio::renderUI()
{
    if (ImGui::CollapsingHeader("Audio", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushItemWidth(-FLT_MIN * 0.5f);

        // Audio file path
        static char audioPathBuffer[256] = { 0 };
        std::copy_n(audioPath.begin(), std::min(audioPath.size(), sizeof(audioPathBuffer) - 1), audioPathBuffer);
        audioPathBuffer[std::min(audioPath.size(), sizeof(audioPathBuffer) - 1)] = '\0';

        if (ImGui::InputText("Audio File", audioPathBuffer, sizeof(audioPathBuffer)))
        {
            audioPath = audioPathBuffer;
        }

        ImGui::SameLine();
        if (ImGui::Button("Browse..."))
        {
            // TODO: Implement file browser dialog
        }

        ImGui::Separator();

        // Audio controls
        ImGui::BeginGroup();
        if (ImGui::Button(isPlaying ? "Pause" : "Play"))
        {
            play = !isPlaying;
        }

        ImGui::SameLine();
        if (ImGui::Button("Stop"))
        {
            play = false;
            isPlaying = false;
            progress.currentTime = 0.0f;
        }

        ImGui::SameLine();
        ImGui::Checkbox("Loop", &loop);
        ImGui::EndGroup();

        ImGui::Separator();

        // Audio properties
        ImGui::SliderFloat("Volume", &volume, 0.0f, 1.0f, "%.2f");
        ImGui::Checkbox("Spatial Audio", &spatial);

        // Progress bar
        ImGui::Separator();
        ImGui::Text("Progress");
        if (progress.duration > 0.0f)
        {
            float progressFraction = progress.currentTime / progress.duration;
            ImGui::ProgressBar(progressFraction, ImVec2(-FLT_MIN, 0));

            ImGui::Columns(2, "progressColumns", false);
            ImGui::Text("Current: %.2fs", progress.currentTime);
            ImGui::NextColumn();
            ImGui::Text("Total: %.2fs", progress.duration);
            ImGui::Columns(1);
        }
        else
        {
            ImGui::TextDisabled("No audio loaded");
        }

        ImGui::PopItemWidth();
    }
}

void item::Text::renderUI()
{
    if (ImGui::CollapsingHeader("Text Properties", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushItemWidth(-FLT_MIN * 0.5f);

        // Text content
        static char textBuffer[1024] = { 0 };
        std::copy_n(content.begin(), std::min(content.size(), sizeof(textBuffer) - 1), textBuffer);
        textBuffer[std::min(content.size(), sizeof(textBuffer) - 1)] = '\0';

        if (ImGui::InputTextMultiline("Content", textBuffer, sizeof(textBuffer),
            ImVec2(0, ImGui::GetTextLineHeight() * 3)))
        {
            content = textBuffer;
        }

        ImGui::Separator();

        // Text appearance
        ImGui::SliderFloat("Scale", &scale, 0.1f, 10.0f, "%.2f");
        ImGui::ColorEdit3("Color", glm::value_ptr(color));

        // Target camera
        static char camBuffer[128] = { 0 };
        std::copy_n(renderToCam.begin(), std::min(renderToCam.size(), sizeof(camBuffer) - 1), camBuffer);
        camBuffer[std::min(renderToCam.size(), sizeof(camBuffer) - 1)] = '\0';

        if (ImGui::InputText("Render to Camera", camBuffer, sizeof(camBuffer)))
        {
            renderToCam = camBuffer;
        }

        // Quick color presets
        ImGui::Separator();
        ImGui::Text("Presets:");

        if (ImGui::Button("White")) color = glm::vec3(1.0f);
        ImGui::SameLine();
        if (ImGui::Button("Red")) color = glm::vec3(1.0f, 0.2f, 0.2f);
        ImGui::SameLine();
        if (ImGui::Button("Green")) color = glm::vec3(0.2f, 1.0f, 0.2f);
        ImGui::SameLine();
        if (ImGui::Button("Blue")) color = glm::vec3(0.2f, 0.2f, 1.0f);

        ImGui::PopItemWidth();
    }
}

void item::container::renderUI()
{
    if (ImGui::CollapsingHeader("Container", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::Text("UUID: %s", UUID.c_str());
        ImGui::Separator();

        // Add items section
        ImGui::Text("Add Items:");
        ImGui::BeginGroup();

        if (ImGui::Button("Add Text"))
        {
            std::string entityId = GenerateRandomID();
            canvasItems.addItem(entityId.c_str(), "Text", canvasItems.TEXT, UUID.c_str());
            itemListID.push_back(entityId);
        }

        ImGui::SameLine();
        if (ImGui::Button("Add Empty"))
        {
            std::string entityId = GenerateRandomID();
            canvasItems.addItem(entityId.c_str(), "Empty", canvasItems.HOLDER, UUID.c_str());
            itemListID.push_back(entityId);
        }

        ImGui::EndGroup();

        ImGui::Separator();

        // List of items in container
        if (!itemListID.empty())
        {
            if (ImGui::TreeNodeEx("Items", ImGuiTreeNodeFlags_DefaultOpen))
            {
                for (const auto& itemID : itemListID)
                {
                    if (!item::item_itemData.contains(itemID))
                        continue;

                    auto& itemData = item::item_itemData[itemID];
                    bool isText = item::item_text.contains(itemID);

                    ImGui::PushID(itemID.c_str());

                    // Item header with visibility toggle
                    ImGui::AlignTextToFramePadding();
                    ImGui::Checkbox("##Visible", &itemData.show_);
                    ImGui::SameLine();

                    std::string nodeLabel = std::format("{} ({})",
                        isText ? "Text" : "Empty",
                        itemID.substr(0, 8));

                    if (ImGui::TreeNodeEx(nodeLabel.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
                    {
                        // Render item-specific UI
                        if (isText)
                        {
                            item::item_text[itemID].renderUI();
                        }

                        if (item::item_position.contains(itemID))
                        {
                            item::item_position[itemID].renderUI();
                        }

                        // Remove button
                        ImGui::Spacing();
                        if (ImGui::Button("Remove"))
                        {
                            // Mark for removal (handle removal outside this loop)
                            itemData.show_ = false;
                        }

                        ImGui::TreePop();
                    }

                    ImGui::PopID();
                }
                ImGui::TreePop();
            }
        }
        else
        {
            ImGui::TextDisabled("No items in container");
        }

        // Remove hidden items
        auto it = std::remove_if(itemListID.begin(), itemListID.end(),
            [](const std::string& id) {
                auto dataIt = item::item_itemData.find(id);
                return dataIt == item::item_itemData.end() || !dataIt->second.show_;
            });

        if (it != itemListID.end())
        {
            itemListID.erase(it, itemListID.end());
        }
    }
}

void item::position::renderUI()
{
    if (ImGui::CollapsingHeader("Position", ImGuiTreeNodeFlags_DefaultOpen))
    {
        ImGui::PushItemWidth(-FLT_MIN * 0.5f);

        ImGui::Text("Screen Position");
        ImGui::DragFloat("X", &x, 1.0f, -1000.0f, 1000.0f, "%.0f px");
        ImGui::DragFloat("Y", &y, 1.0f, -1000.0f, 1000.0f, "%.0f px");

        // Quick position presets
        ImGui::Separator();
        ImGui::Text("Presets:");

        if (ImGui::Button("Center"))
        {
            x = 0.0f;
            y = 0.0f;
        }

        ImGui::SameLine();
        if (ImGui::Button("Top Left"))
        {
            x = -400.0f;
            y = 300.0f;
        }

        ImGui::SameLine();
        if (ImGui::Button("Bottom Right"))
        {
            x = 400.0f;
            y = -300.0f;
        }

        ImGui::PopItemWidth();
    }
}