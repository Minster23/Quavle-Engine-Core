#include "interface.h"
#include <imgui.h>
#include <misc/cpp/imgui_stdlib.h>
#include <iostream>
#include "saveSystem.h"
#include "window.h"
#include "game.h"

using namespace interface;
using namespace saved;
using namespace window;
using namespace game;

windowsHandler wndxl;

bool interface::interfaces::componentsInterfaces::buildWindow_ = false;

void interface::interfaces::componentsInterfaces::buildWindow()
{
	if (!buildWindow_)
		return;

	ImGui::Begin("Build Game", &buildWindow_, ImGuiWindowFlags_NoCollapse);

	ImGui::Text("Build your game to an executable.");
	ImGui::Separator();

	ImGui::Text("Project Location:");
	ImGui::SameLine();
	ImGui::TextDisabled("%s", wndxl.project_location.c_str());

	ImGui::Spacing();
	ImGui::Text("Metadata");
	ImGui::Separator();

	// LANGSUNG std::string (imgui_stdlib)
	ImGui::InputText("Game Name", &game::gameMetaData::gamePlayName);
	ImGui::InputText("Author", &game::gameMetaData::authorName);
	ImGui::InputText("Version", &game::gameMetaData::version);

	ImGui::Spacing();
	ImGui::Separator();
	ImGui::Spacing();

	if (ImGui::Button("Build", ImVec2(120, 0)))
	{
		// Metadata SUDAH keisi, gak perlu apply
		// saveSystem.buildGame();
	}

	ImGui::End();
}
