#include "interface.h"
#include <imgui.h>
#include "scene.h"
#include <iostream>
#include <stdio.h>
#include <unordered_map>

using namespace interface;
using namespace scene;

static std::unordered_map<std::string, std::string> sceneDesc;
static std::string sceneSelected;

bool interface::interfaces::componentsInterfaces::sceneManager_ = false;

void interface::interfaces::componentsInterfaces::scenesManager()
{
	if (!interfaces::componentsInterfaces::sceneManager_)
		return;

	ImGui::Begin("Scenes Manager", &interfaces::componentsInterfaces::sceneManager_);

	ImGui::BeginChild("TopBar", ImVec2(0, 40), false);
	{
		if (ImGui::Button("+ Add Scene", ImVec2(120, 0)))
		{
			std::string newSceneName = "New Scene";
			scenesHandler.addScene(newSceneName.c_str());
		}

		ImGui::SameLine();

		if (ImGui::Button("- Remove Scene", ImVec2(140, 0)))
		{
			scenesHandler.removeScene(sceneSelected.c_str());
		}
	}
	ImGui::EndChild();

	ImGui::Separator();

	ImGui::Columns(2, nullptr, true);

	ImGui::BeginChild("SceneList", ImVec2(0, 0), true);
	{
		ImGui::Text("Scenes");
		ImGui::Separator();

		for (int i = 0; i < scenesHandler.totalScene; i++)
		{
			ImGui::PushID(i);
			const auto& sceneData = scenesHandler.scenes[i];

			if (ImGui::Selectable(
				sceneData.sceneName.c_str(),
				scenesHandler.currentScene == i))
			{
				sceneSelected = sceneData.sceneName;
				scenesHandler.setCurrentScene(sceneData.sceneName.c_str());
				interfaces::componentsInterfaces::selecetedObject.clear();
			}
			ImGui::PopID();
		}
	}
	ImGui::EndChild();

	ImGui::NextColumn();

	ImGui::BeginChild("SceneDetails", ImVec2(0, 0), true);
	{
		ImGui::Text("Scene Details");
		ImGui::Separator();

		if (scenesHandler.currentScene >= 0 &&
			scenesHandler.currentScene < scenesHandler.totalScene)
		{
			auto& currentSceneData = scenesHandler.scenes[scenesHandler.currentScene];

			char nameBuffer[256];
			strncpy_s(nameBuffer, currentSceneData.sceneName.c_str(), sizeof(nameBuffer));

			ImGui::Text("Name");
			if (ImGui::InputText("##SceneName", nameBuffer, sizeof(nameBuffer)))
			{
				currentSceneData.sceneName = nameBuffer;
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::TextDisabled("UUID");
			ImGui::TextWrapped("%s", currentSceneData.sceneUUID.c_str());

			ImGui::Spacing();

			ImGui::Text("Objects Count: %d", currentSceneData.objectCount);

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::Text("Objects");
			for (const auto& objID : currentSceneData.objectListID)
			{
				ImGui::BulletText("%s", objID.c_str());
			}

			ImGui::Spacing();
			ImGui::Separator();
			ImGui::Spacing();

			ImGui::Text("Description");
			char descBuffer[256];
			std::string& desc = sceneDesc[currentSceneData.sceneUUID];
			strncpy_s(descBuffer, desc.c_str(), sizeof(descBuffer));

			if (ImGui::InputTextMultiline(
				"##SceneDesc",
				descBuffer,
				sizeof(descBuffer),
				ImVec2(-1.0f, 120)))
			{
				desc = descBuffer;
			}
		}
		else
		{
			ImGui::TextDisabled("No scene selected");
		}
	}
	ImGui::EndChild();

	ImGui::Columns(1);
	ImGui::End();
}
