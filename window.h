#pragma once
#include "EngineConfig.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <glm/glm.hpp>

namespace window
{
	class windowsHandler {
	public:
		static GLFWwindow* window;
		static int width, height;
		void windowInit();
		static std::string project_location;
		static glm::vec2 size;
	private:
		bool initGLFW();
		bool initGL();
		void windowLoop();
		void cleanUp();
	};
}