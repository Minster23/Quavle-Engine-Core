#include "environment.h"
#include "imgui.h"
#include "scene.h"

using namespace scene;

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
