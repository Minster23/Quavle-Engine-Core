#include <glad/glad.h>

#include "environment.h"
#include "imgui.h"
#include "scene.h"
#include "entity.h"

using namespace scene;
using namespace entity;
namespace environment
{
	environment_properties Environment_Properties;
}

using namespace environment;

std::unordered_map<std::string, environment_properties::env> environment_properties::Enviroments;

void environment_properties::env::renderUI()
{
	if (ImGui::CollapsingHeader("Environment Properties") && this->isCreated)
	{
		ImGui::TextDisabled("Scene ID: %s", this->sceneID.c_str());
		ImGui::InputText("Name", (char*)this->envName.c_str(), 256);
		ImGui::Checkbox("Use Cubemap", &useCubemap);
		ImGui::ColorPicker4("Cubemap Colour", (float*)&cubeMapcolour);
		if (useCubemap)
		{
			static char cubeMapPath[256] = "";
			ImGui::InputText("Cubemap Location", cubeMapPath, IM_ARRAYSIZE(cubeMapPath));
			if (ImGui::Button("Add Cubemap"))
			{
				cubeMapLocation.push_back(std::string(cubeMapPath));
				cubeMapPath[0] = '\0';
			}
			ImGui::Separator();
			for (size_t i = 0; i < cubeMapLocation.size(); ++i)
			{
				ImGui::Text("%s", cubeMapLocation[i].c_str());
				ImGui::SameLine();
				if (ImGui::Button(("Remove##" + std::to_string(i)).c_str()))
				{
					cubeMapLocation.erase(cubeMapLocation.begin() + i);
				}
			}
		}
	}
	if (!this->isCreated && ImGui::Button("Create"))
	{
		this->isCreated = true;
		this->sceneID = scenesHandler.currentSceneName();
	}
}

void environment_properties::localEnv()
{
	bool found = false;
	bool cubeMap = false;

	for (const auto& pair : environment::environment_properties::Enviroments) {
		if (pair.second.isCreated &&
			pair.second.sceneID == scenesHandler.currentSceneName() &&
			pair.second.useCubemap == false)
		{
			glClearColor(
				pair.second.cubeMapcolour.r,
				pair.second.cubeMapcolour.g,
				pair.second.cubeMapcolour.b,
				pair.second.cubeMapcolour.a
			);
			found = true;
			cubeMap = pair.second.useCubemap;
			break;
		}
	}

	if (!found && !cubeMap) {
		glClearColor(0.82f, 0.87f, 0.98f, 1.0f);
	}

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}