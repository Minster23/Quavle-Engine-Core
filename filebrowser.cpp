#include "interface.h"
#include <glad/glad.h>
#include "window.h"
#include "entity.h"
#include <iostream>
#include <filesystem>
#include <imgui.h>
#include "stb_image.h"
#include <unordered_map>
#include <string_view>

using namespace interface;
using namespace window;
using namespace entity;
using namespace entity::importers;
namespace fs = std::filesystem;

bool interfaces::componentsInterfaces::fileExplorer_ = true;
window::windowsHandler wndF;

struct IconCache {
    unsigned int folder = 0;
    unsigned int defaultFile = 0;
    unsigned int image = 0;
    unsigned int model3D = 0;

    void initialize(const std::string& iconPath) {
        folder = loadTexture(iconPath + "folder.png");
        defaultFile = loadTexture(iconPath + "file.png");
        image = loadTexture(iconPath + "image.png");
        model3D = loadTexture(iconPath + "3D.png");
    }

    void cleanup() {
        glDeleteTextures(1, &folder);
        glDeleteTextures(1, &defaultFile);
        glDeleteTextures(1, &image);
        glDeleteTextures(1, &model3D);
    }

private:
    static unsigned int loadTexture(const std::string& path) {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        glBindTexture(GL_TEXTURE_2D, textureID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        int width, height, channels;
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);

        if (data) {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            stbi_image_free(data);
        }
        else {
            std::cerr << "Failed to load texture: " << path << std::endl;
            stbi_image_free(data);
            return 0;
        }

        return textureID;
    }
};

static IconCache iconCache;
static bool iconsInitialized = false;

struct Settings {
    int columns = 5;
    float iconSize = 32.0f;
    ImVec2 iconButtonSize{ 32.0f, 32.0f };
    ImVec4 buttonBgColor{ 0, 0, 0, 0 };
};

static Settings settings;

unsigned int getIconForExtension(std::string_view ext) {
    static const std::unordered_map<std::string_view, unsigned int*> extensionMap = {
        {".png", &iconCache.image},
        {".jpg", &iconCache.image},
        {".jpeg", &iconCache.image},
        {".gltf", &iconCache.model3D},
        {".glb", &iconCache.model3D}
    };

    auto it = extensionMap.find(ext);
    return it != extensionMap.end() ? *it->second : iconCache.defaultFile;
}

void clickHandler(std::string ext, std::string fileLoc, std::string foldLoc)
{
    if(ext == ".gltf" || ext == ".obj" || ext == ".fbx")
    {
        Entity_Importer.load(fileLoc.c_str(), foldLoc.c_str());
    }
}

void interfaces::componentsInterfaces::fileExplorer() {
    if (!iconsInitialized) {
        iconCache.initialize(fs::current_path().string() + "\\fonts\\icons\\");
        iconsInitialized = true;
    }

    const std::string& currentPath = wndF.project_location;

    ImGui::Begin("File Explorer", &fileExplorer_, ImGuiWindowFlags_NoCollapse);

    if (ImGui::Button("Back")) {
        fs::path path(currentPath);
        if (path != path.root_path()) {
            wndF.project_location = path.parent_path().string();
        }
    }

    ImGui::SameLine();
    ImGui::Text("%s", currentPath.c_str());

    ImGui::Separator();
    ImGui::Columns(settings.columns, "file_explorer_columns", false);

    int id = 0;
    for (const auto& entry : fs::directory_iterator(currentPath)) {
        ImGui::PushID(id++);

        unsigned int icon;
        bool isDirectory = entry.is_directory();

        if (isDirectory) {
            icon = iconCache.folder;
        }
        else {
            icon = getIconForExtension(entry.path().extension().string());
        }

        if (ImGui::ImageButton(
            entry.path().filename().string().c_str(),
            (ImTextureID)(intptr_t)icon,
            settings.iconButtonSize,
            ImVec2(0, 0),
            ImVec2(1, 1),
            settings.buttonBgColor
        )) {
            if (isDirectory) {
                wndF.project_location = entry.path().string();
            }
            else {
				clickHandler(entry.path().extension().string(), entry.path().string(), entry.path().parent_path().string());
            }
        }

        const std::string filename = entry.path().filename().string();
        ImGui::TextWrapped("%s", filename.c_str());

        ImGui::PopID();
        ImGui::NextColumn();
    }

    ImGui::Columns(1);
    ImGui::End();
}