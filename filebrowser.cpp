#include "interface.h"
#include <glad/glad.h>
#include "entity.h"
#include <iostream>
#include <filesystem>
#include <imgui.h>
#include "stb_image.h"
#include <unordered_map>
#include <string_view>
#include "game.h"
#include "IconsFontAwesome6.h"


using namespace interface;
using namespace entity;
using namespace entity::importers;
using namespace game;
namespace fs = std::filesystem;

namespace
{
    std::unordered_map<std::string, std::vector<std::string>> fileCache;
	std::unordered_map<std::string, unsigned int> textureCache;

    unsigned int loadTexture(const std::string& path) {

		if (textureCache.find(path) != textureCache.end())
            return textureCache[path];

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

        textureCache[path] = textureID;

        return textureID;
    }

    bool openFile(const std::string& path, std::string type)
    {
		if (type == ".txt" || type == ".lua" || type == ".cpp")
        {
			//Interfaces.setCodeEditorOpened(path);
            Interfaces.setGraphEditorOpened(path);
            return true;
        }
        else
        {
            std::cerr << "Unsupported file type: " << type << std::endl;
            return false;
        }
    }

    static std::string currentPath;
	static std::vector<std::string> HistoryDir;
}

bool interfaces::componentsInterfaces::fileExplorer_ = true;

std::string fileIcon(std::string type)
{
    std::string Icon;
	if (type == ".txt" || type == ".docx" || type == ".pdf")
		Icon = ICON_FA_FILE_LINES;
	else if (type == ".png" || type == ".jpg" || type == ".jpeg" || type == ".bmp")
		Icon = ICON_FA_FILE_IMAGE;
	else if (type == ".mp4" || type == ".avi" || type == ".mkv")
		Icon = ICON_FA_FILE_VIDEO;
	else if (type == ".mp3" || type == ".wav" || type == ".ogg")
		Icon = ICON_FA_FILE_AUDIO;
    else if (type == ".cpp" || type == ".h" || type == ".hpp" || type == ".cs")
        Icon = ICON_FA_FILE_CODE;
    else if (type == ".zip" || type == ".rar" || type == ".7z")
        Icon = ICON_FA_FILE_ZIPPER;
    else
		Icon = ICON_FA_FILE;
    return Icon;
}

void fileList()
{
    fs::path dir_path{ currentPath };

    int counter = 0;
    int foundFiles = 0;

    try {
        for (const auto& entry : fs::directory_iterator(dir_path)) {
            counter++;
            if (fileCache[currentPath].size() <= counter && !entry.is_directory())
            {
                foundFiles++;
                fileCache[currentPath].push_back(entry.path().filename().string());
            }
            if (foundFiles == counter)
            {
                counter = 0;
                foundFiles = 0;
                break;
            }
        }
    }
    catch (const fs::filesystem_error& e) {
        std::cerr << "Filesystem error: " << e.what() << std::endl;
    }

    auto& files = fileCache[currentPath];
    ImGuiIO& io = ImGui::GetIO();

    ImFont* ICON_FONT = io.Fonts->AddFontFromFileTTF("fonts/fa-solid-900.ttf", 30.0f);
    ImFont* TEXT_FONT = io.Fonts->AddFontFromFileTTF("fonts/arial.ttf", 16.0f);

    int P = 0;
    int limit = 7;

    for (const auto& file : files) {
        ImGui::PushID(P);
		fs::path filePath = dir_path / file;
		fs::path ext = filePath.extension();
        std::string ext_str = ext.string();

        ImVec2 cellSize = { 80, 80 };
        ImGui::BeginGroup();

        ImVec2 start = ImGui::GetCursorScreenPos();
		if (ImGui::InvisibleButton("cell", cellSize))
        {
            openFile(filePath.string(), ext_str);
        }

        ImVec2 iconSize = ImGui::CalcTextSize(fileIcon(ext_str).c_str());

        ImGui::SetCursorScreenPos({
            start.x + (cellSize.x - iconSize.x) * 0.5f,
            start.y + (cellSize.y - iconSize.y) * 0.4f
            });

        ImGui::PushFont(ICON_FONT);

        if(ext_str == ".jpg" || ext_str == ".jpeg" || ext_str == ".png" || ext_str == ".bmp" )
			ImGui::Image((void*)(intptr_t)loadTexture((dir_path / file).string()), ImVec2(40, 40));
		else
            ImGui::Text("%s", fileIcon(ext_str).c_str());

        ImGui::PopFont();

        std::string name = file;
        if (name.length() > 10) name = name.substr(0, 10) + "...";

        ImVec2 textSize = ImGui::CalcTextSize(name.c_str());

        ImGui::SetCursorScreenPos({
            start.x + (cellSize.x - textSize.x) * 0.5f,
            start.y + cellSize.y - 18
            });

        ImGui::PushFont(TEXT_FONT);
        ImGui::Text("%s", name.c_str());
        ImGui::PopFont();

        ImGui::EndGroup();
        ImGui::PopID();

        if ((P + 1) % limit != 0)
            ImGui::SameLine();

        P++;
    }
}

void drawDirectory(const std::filesystem::path& path)
{
    for (const auto& entry : std::filesystem::directory_iterator(path))
    {
        if (!entry.is_directory()) continue;

        std::string name = entry.path().filename().string();
        std::string label = std::string(ICON_FA_FOLDER) + " " + name;

        if (ImGui::TreeNode(label.c_str()))
        {
            if (ImGui::IsItemClicked())
            {
				HistoryDir.push_back(currentPath);
                currentPath = entry.path().string();
            }

            drawDirectory(entry.path());

            ImGui::TreePop();
        }
        else
        {
            if (ImGui::IsItemClicked())
            {
                HistoryDir.push_back(currentPath);
                currentPath = entry.path().string();
            }
        }
    }
}

void directoryList(std::string FileLoc)
{
    std::filesystem::path dir_path{ FileLoc };

    ImGui::BeginChild("Directory List", ImVec2(0, 0), true);
    drawDirectory(dir_path);
    ImGui::EndChild();
}

void interfaces::componentsInterfaces::fileExplorer() 
{
    if (currentPath.empty())
        currentPath = game::gameData.projectLocation;

    ImGui::Begin("File Explorer", &fileExplorer_, ImGuiWindowFlags_NoCollapse);
    if (ImGui::Button(ICON_FA_HOUSE))
        currentPath = game::gameData.projectLocation;
    ImGui::SameLine();

	if (currentPath != game::gameData.projectLocation)
    {
        if (ImGui::Button(ICON_FA_BACKWARD_STEP))
        {
            currentPath = HistoryDir.back();
            HistoryDir.pop_back();
        }
        ImGui::SameLine();
    }

	ImGui::Text(currentPath.c_str());

	ImGui::BeginChild("File List", ImVec2(0, 0), true);
    ImGui::Columns(2);

    ImGui::Text("Folder");
    ImGui::NextColumn();
    ImGui::Text("Files");
    ImGui::NextColumn();

	directoryList(currentPath);
    ImGui::NextColumn();
    fileList();
    ImGui::NextColumn();

    ImGui::Columns(1);
	ImGui::EndChild();

    ImGui::End();
}