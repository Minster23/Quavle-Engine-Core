#pragma once
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/vec4.hpp>

namespace environment
{
	class environment_properties
	{
	public:
		struct env
		{
			bool isCreated = false;
			std::string sceneID = "";
			std::string envName = "Default Environment";
			bool useCubemap = false;
			glm::vec4 cubeMapcolour = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
			std::vector<std::string> cubeMapLocation;

			void renderUI();
		} default_env;

		void localEnv();

		static std::unordered_map<std::string, env> Enviroments;
	};

	extern environment_properties Environment_Properties;
}
