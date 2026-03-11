#include <imgui.h>
#include "interface.h"
#include <fstream>
#include <filesystem>
namespace fs = std::filesystem;
using namespace interface;

namespace
{
    static bool loaded = false;
    static std::string oldFile;
    static std::string fileContent;

    std::string readFileIntoString(const fs::path& path) {
        std::ifstream stream(path);
        if (!stream.is_open()) {
            std::cerr << "Error opening file: " << path << std::endl;
            return "";
        }
        stream.seekg(0, std::ios::end);
        size_t fileSize = stream.tellg();
        stream.seekg(0, std::ios::beg);

        std::string content(fileSize, '\0');
        stream.read(content.data(), fileSize);
        loaded = true;
        return content;
    }

    void saveFile(const fs::path& path, const std::string& content)
    {
        std::ofstream out(path);
        if (!out.is_open()) {
            std::cerr << "Error saving file: " << path << std::endl;
            return;
        }
        out.write(content.c_str(), content.size());
    }
}

std::string interface::interfaces::componentsInterfaces::codeEditorFileLoc_ = "";

void interface::interfaces::componentsInterfaces::codeEditor()
{
    if (codeEditorFileLoc_ == "")
        return;

    if (!loaded)
        fileContent = readFileIntoString(codeEditorFileLoc_);

    if (codeEditorFileLoc_ != oldFile)
        fileContent = readFileIntoString(codeEditorFileLoc_);

    ImGui::Begin("Code Editor", nullptr, ImGuiWindowFlags_MenuBar);

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            if (ImGui::MenuItem("Save"))
            {
				oldFile = codeEditorFileLoc_;
                saveFile(codeEditorFileLoc_, fileContent);
            }

            if (ImGui::MenuItem("Reload"))
            {
                loaded = false;
                fileContent = readFileIntoString(codeEditorFileLoc_);
            }

            ImGui::EndMenu();
        }
        ImGui::EndMenuBar();
    }

    ImGui::Text("%s", codeEditorFileLoc_.c_str());

    if (loaded)
        ImGui::InputTextMultiline("##codeEditor", fileContent.data(), fileContent.capacity() + 1, ImVec2(-FLT_MIN, -FLT_MIN));

    ImGui::End();
}