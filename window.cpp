#include <glad/glad.h>
#include "scene.h"
#include "window.h"
#include "entity.h"
#include "pipeline.h"
#include "saveSystem.h"
#include "random.h"

#if WITH_EDITOR
#include <imgui.h>
#include "interface.h"
#include "game.h"
using namespace interface;
using namespace game;
#endif

using namespace window;
using namespace entity;
using namespace importers;
using namespace pipeline;
using namespace entity::canvas;
using namespace scene;
using namespace saved;

GLFWwindow* windowsHandler::window;
glm::vec2 windowsHandler::size;

int windowsHandler::width, windowsHandler::height;
std::string windowsHandler::project_location = "";

bool firstMouse = false;
float lastX = 0.0f, lastY = 0.0f;
static bool cursorDisabled = true;

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
#if WITH_EDITOR
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS && !ImGui::IsAnyItemHovered() && !ImGui::IsWindowHovered())
    {
        if (cursorDisabled)
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            firstMouse = true;
        }
        else
        {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        }
        cursorDisabled = !cursorDisabled;
    }else
    {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
        firstMouse = true;
		cursorDisabled = false;
    }
#endif
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;


    if(cursorDisabled)
    {
		gameobjects.activeCamera->cameraRotation(xoffset, yoffset);
    }
}

bool canIntract()
{
#if WITH_EDITOR
	if (ImGui::IsAnyItemActive() || ImGui::IsAnyItemHovered() || ImGui::IsWindowHovered())
    {
        return false;
    }
    else { return true; }
#endif
}

void processInput(GLFWwindow* window)
{
    if (!gameobjects.activeCamera) return;

    float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && canIntract())
        gameobjects.activeCamera->position += gameobjects.activeCamera->Forward() * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS && canIntract())
        gameobjects.activeCamera->position -= gameobjects.activeCamera->Forward() * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS && canIntract())
        gameobjects.activeCamera->position -= gameobjects.activeCamera->Right() * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS && canIntract())
        gameobjects.activeCamera->position += gameobjects.activeCamera->Right() * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS && canIntract())
        gameobjects.activeCamera->position.y -= cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS && canIntract())
        gameobjects.activeCamera->position.y += cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
    {
        float fastSpeed = cameraSpeed * 2.0f;
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            gameobjects.activeCamera->position += gameobjects.activeCamera->Forward() * fastSpeed;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            gameobjects.activeCamera->position -= gameobjects.activeCamera->Forward() * fastSpeed;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            gameobjects.activeCamera->position -= gameobjects.activeCamera->Right() * fastSpeed;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            gameobjects.activeCamera->position += gameobjects.activeCamera->Right() * fastSpeed;
    }
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void window::windowsHandler::windowInit()
{
	size = glm::vec2(1920, 1080);
	if (!initGLFW() || size == glm::vec2(0, 0))return;
	if (!initGL())return;
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

#if WITH_EDITOR
	Interfaces.init();
	componentss.prepareComponentsList();
#endif
    shadows.init();
	GodRays.init(size.x, size.y);
    //postProcessings.init(1920, 1080);

#if WITH_EDITOR

    scenesHandler.addScene("default_scene");
    scenesHandler.setCurrentScene("default_scene");
    gameobjects.addObject("ENGINE_CAM", "ENGINE_CAM", componentss.type::CAMERA);
    gameobjects.addListener("ENGINE_CAM");

#else
    scenesHandler.setPlayMode(true);
    scenesHandler.addScene("default_scene");
    scenesHandler.setCurrentScene("default_scene");
    gameobjects.addObject("ENGINE_CAM", "ENGINE_CAM", componentss.type::CAMERA);
    gameobjects.addListener("ENGINE_CAM");
	gameobjects.addObject("PLAYER", "PLAYER", componentss.type::OBJECT);
	componentss.entity_transform["PLAYER"].location = glm::vec3(0.0f, 0.0f, 0.0f);

    saveSystem.load();
#endif

	windowLoop();

	cleanUp();
}

bool window::windowsHandler::initGLFW()
{

#if WITH_EDITOR
    std::string title = "Quavle Editor " + project_location;
#else
    std::string title = "NAMA GAME";
#endif

    const char* titleWindow = title.c_str();
	if (!glfwInit()) {
		return false;
	}

    GLFWmonitor* primary = glfwGetPrimaryMonitor();
    const GLFWvidmode* mode = glfwGetVideoMode(primary);

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
	window = glfwCreateWindow(mode->width, mode->height, titleWindow, NULL, NULL);
	if (!window) {
		glfwTerminate(); 
		return false;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	return true;
}

bool window::windowsHandler::initGL()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cout << "Failed to initialize GLAD" << std::endl;
		glfwTerminate();
		return -1;
	}
	glfwGetWindowSize(window, &width, &height);

	glViewport(0, 0, width, height);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	std::cout << "OpenGL Initialized: "
		<< glGetString(GL_VERSION) << std::endl;

	return true;
}

void window::windowsHandler::windowLoop()
{
	while (!glfwWindowShouldClose(window)) {
        glfwGetWindowSize(window, &width, &height);
#if WITH_EDITOR
        if(GameState.control_state == states::controlState::EDITOR)processInput(window);
        prepareTexCam(width, height);
#endif
        gameobjects.childrenHandler();
        gameobjects.draw();
        //GodRays.start(gameobjects.activeCamera->tex, shadows.depthMap);
        canvasItems.draw();

#if WITH_EDITOR
        Interfaces.draw();
#endif
        gameobjects.updateAudio();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
}

void window::windowsHandler::cleanUp()
{
#if WITH_EDITOR
	Interfaces.cleanup();
#endif
    GodRays.clean();
    postProcessings.cleanup();
	glfwDestroyWindow(window);
	glfwTerminate();
}