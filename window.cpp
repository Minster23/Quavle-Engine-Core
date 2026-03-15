#include <glad/glad.h>
#include "game.h"
#include "scene.h"
#include "window.h"
#include "entity.h"
#include "saveSystem.h"
#include "random.h"
#include "environment.h"

#if WITH_EDITOR
#include "interface.h"
using namespace interface;
#endif

using namespace window;
using namespace entity;
using namespace importers;
using namespace entity::canvas;
using namespace scene;
using namespace game;
using namespace saved;
using namespace environment;

GLFWwindow* windowsHandler::window;
glm::vec2 windowsHandler::size;

int windowsHandler::width, windowsHandler::height;
std::string windowsHandler::project_location = "";

bool firstMouse = false;
float lastX = 0.0f, lastY = 0.0f;
static bool cursorDisabled = true;

void cameraSelector()
{
    static std::string realActiveUUID = "ENGINE_CAM";

    if (GameState.game_state == states::gameState::PLAY)
    {
        for (auto& cam : components::entity_camera)
        {
            if (cam.second.isMain && cam.second.isActive)
            {
                realActiveUUID = cam.first;
                break;
            }
        }
    }
    else
    {
        realActiveUUID = "ENGINE_CAM";
    }

    gameobjects.activeCamera = &components::entity_camera[realActiveUUID];
    gameobjects.activeCamera->UpdateView();
    gameobjects.activeCamera->UpdateProjection();
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
#if WITH_EDITOR
    if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS && gameobjects.activeCamera->canMoveInEditor)
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
    }
    else
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


    if (cursorDisabled && gameobjects.activeCamera->canMoveInEditor)
    {
        gameobjects.activeCamera->cameraRotation(xoffset, yoffset);
    }
}

void processInput(GLFWwindow* window)
{
    if (!gameobjects.activeCamera || !gameobjects.activeCamera->canMoveInEditor) return;

    float cameraSpeed = 0.05f;
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        gameobjects.activeCamera->position += gameobjects.activeCamera->Forward() * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        gameobjects.activeCamera->position -= gameobjects.activeCamera->Forward() * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        gameobjects.activeCamera->position -= gameobjects.activeCamera->Right() * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        gameobjects.activeCamera->position += gameobjects.activeCamera->Right() * cameraSpeed;

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        gameobjects.activeCamera->position.y -= cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
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
    saveSystem.load();
    size = glm::vec2(1920, 1080);
    game::gameData.projectLocation = project_location;
    if (!initGLFW() || size == glm::vec2(0, 0))return;
    if (!initGL())return;
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

#if WITH_EDITOR
    Interfaces.init();
    componentss.prepareComponentsList();
    std::cout << "Project Location: " << gameData.projectLocation << std::endl;
#endif

#if WITH_EDITOR

    scenesHandler.addScene("default_scene");
    scenesHandler.setCurrentScene("default_scene");
    gameobjects.addObject("PLAYER", "PLAYER", componentss.type::OBJECT);
    gameobjects.addObject("ENGINE_CAM", "ENGINE_CAM", componentss.type::CAMERA);

#else
    scenesHandler.setPlayMode(true);
    scenesHandler.addScene("default_scene");
    scenesHandler.setCurrentScene("default_scene");
    gameobjects.addObject("ENGINE_CAM", "ENGINE_CAM", componentss.type::CAMERA);
    gameobjects.addListener("ENGINE_CAM");
#endif

    windowLoop();
    cleanUp();
}

bool window::windowsHandler::initGLFW()
{
#if WITH_EDITOR
    std::string Title = "Quavle Engine - " + gameData.projectLocation;
#else
    std::string Title = saveSystem.gamePlayName();
#endif

    const char* titleWindow = Title.c_str();
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
        cameraSelector();
        gameobjects.updateAudio();
        gameobjects.childrenHandler();
        
        gameobjects.activeCamera->bind();
        Environment_Properties.localEnv();
        gameobjects.draw();
        canvasItems.draw();
        gameobjects.activeCamera->unbind();

#if WITH_EDITOR

        if (GameState.control_state == states::controlState::EDITOR)processInput(window);
        Interfaces.draw();
#endif

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void window::windowsHandler::cleanUp()
{
#if WITH_EDITOR
    Interfaces.cleanup();
#endif
    glfwDestroyWindow(window);
    glfwTerminate();
}