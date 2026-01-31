#include "interface.h"
#include "window.h"
#include "scene.h"
#include "entity.h"
#include "game.h"
#include "environment.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

#include "saveSystem.h"

using namespace interface;
using namespace window;
using namespace entity;
using namespace saved;
using namespace scene;
using namespace game;
using namespace environment;

windowsHandler wndHandler;

void Theme(ImGuiIO& io)
{
    ImGuiStyle& style = ImGui::GetStyle();
    ImVec4* colors = style.Colors;

    // ===============================
    // Layout & Spacing
    // ===============================
    style.WindowPadding = ImVec2(14, 14);
    style.FramePadding = ImVec2(10, 7);
    style.ItemSpacing = ImVec2(10, 8);
    style.ItemInnerSpacing = ImVec2(8, 6);

    style.ScrollbarSize = 14.0f;

    style.WindowRounding = 10.0f;
    style.FrameRounding = 8.0f;
    style.PopupRounding = 10.0f;
    style.GrabRounding = 6.0f;
    style.TabRounding = 8.0f;
    style.ChildRounding = 10.0f;
    style.ScrollbarRounding = 12.0f;

    style.WindowBorderSize = 0.0f;
    style.FrameBorderSize = 0.0f;
    style.TabBorderSize = 0.0f;

    // ===============================
    // Base Colors (Dark Neutral)
    // ===============================
    colors[ImGuiCol_Text] = ImVec4(0.92f, 0.93f, 0.95f, 1.00f);
    colors[ImGuiCol_TextDisabled] = ImVec4(0.45f, 0.48f, 0.55f, 1.00f);

    colors[ImGuiCol_WindowBg] = ImVec4(0.07f, 0.08f, 0.10f, 1.00f);
    colors[ImGuiCol_ChildBg] = ImVec4(0.05f, 0.06f, 0.08f, 1.00f);
    colors[ImGuiCol_PopupBg] = ImVec4(0.06f, 0.07f, 0.09f, 1.00f);

    // ===============================
    // Frames & Inputs
    // ===============================
    colors[ImGuiCol_FrameBg] = ImVec4(0.13f, 0.14f, 0.18f, 1.00f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.20f, 0.22f, 0.30f, 1.00f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.23f, 0.25f, 0.35f, 1.00f);

    // ===============================
    // Titles
    // ===============================
    colors[ImGuiCol_TitleBg] = ImVec4(0.05f, 0.06f, 0.08f, 1.00f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.10f, 0.12f, 0.18f, 1.00f);
    colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.04f, 0.05f, 0.07f, 1.00f);

    // ===============================
    // Buttons
    // ===============================
    colors[ImGuiCol_Button] = ImVec4(0.16f, 0.18f, 0.26f, 1.00f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.28f, 0.32f, 0.48f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.24f, 0.28f, 0.42f, 1.00f);

    // ===============================
    // Headers (Tree, Selectable)
    // ===============================
    colors[ImGuiCol_Header] = ImVec4(0.15f, 0.18f, 0.26f, 1.00f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.30f, 0.34f, 0.50f, 1.00f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.25f, 0.30f, 0.45f, 1.00f);

    // ===============================
    // Tabs
    // ===============================
    colors[ImGuiCol_Tab] = ImVec4(0.10f, 0.12f, 0.18f, 1.00f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.30f, 0.34f, 0.55f, 1.00f);
    colors[ImGuiCol_TabActive] = ImVec4(0.22f, 0.26f, 0.42f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.07f, 0.08f, 0.10f, 1.00f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.12f, 0.14f, 0.20f, 1.00f);

    // ===============================
    // Sliders & Scrollbars
    // ===============================
    colors[ImGuiCol_SliderGrab] = ImVec4(0.45f, 0.50f, 0.70f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(0.60f, 0.65f, 0.90f, 1.00f);

    colors[ImGuiCol_ScrollbarBg] = ImVec4(0.05f, 0.06f, 0.08f, 1.00f);
    colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.20f, 0.22f, 0.30f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.30f, 0.34f, 0.45f, 1.00f);
    colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.35f, 0.40f, 0.55f, 1.00f);

    // ===============================
    // Misc
    // ===============================
    colors[ImGuiCol_CheckMark] = ImVec4(0.60f, 0.70f, 1.00f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.30f, 0.35f, 0.50f, 1.00f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.45f, 0.50f, 0.70f, 1.00f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.55f, 0.60f, 0.85f, 1.00f);

    colors[ImGuiCol_TextSelectedBg] = ImVec4(0.30f, 0.45f, 0.85f, 0.35f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.30f, 0.45f, 0.85f, 0.60f);

    // Optional scaling
    style.ScaleAllSizes(io.FontGlobalScale);
}


namespace interface
{
	interfaces Interfaces;
};

bool interface::interfaces::init()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    ImGui_ImplGlfw_InitForOpenGL(wndHandler.window, true);
    ImGui_ImplOpenGL3_Init("#version 330");
	Theme(io);
	return true;
}

void interface::interfaces::componentsInterfaces::menuBar()
{
    if (!ImGui::BeginMainMenuBar())
        return;

    ImGui::TextUnformatted("Quavle Editor");
    ImGui::Separator();

    if (ImGui::Button("Play"))
    {
        scenesHandler.setPlayMode(true);
        GameState.control_state = states::controlState::GAMEPLAY;
        GameState.game_state = states::gameState::PLAY;
        profiler_ = true;
    }

    ImGui::SameLine();

    if (ImGui::Button("Stop"))
    {
        scenesHandler.setPlayMode(false);
        GameState.control_state = states::controlState::EDITOR;
        GameState.game_state = states::gameState::PAUSE;
    }

    ImGui::Separator();

    if (ImGui::BeginMenu("File"))
    {
        if (ImGui::MenuItem("Save", "Ctrl+S"))
            saveSystem.save();

        if (ImGui::MenuItem("Load", "Ctrl+O"))
            saveSystem.load();

        ImGui::EndMenu();
    }

    // ===== Window Menu =====
    if (ImGui::BeginMenu("Window"))
    {
        ImGui::MenuItem("Hierarchy", nullptr, &hirarcy_);
        ImGui::MenuItem("Inspector", nullptr, &inspector_);
        ImGui::MenuItem("Scene Manager", nullptr, &sceneManager_);
        ImGui::MenuItem("File Browser", nullptr, &fileExplorer_);
        ImGui::EndMenu();
    }

    // ===== Debug Menu =====
    if (ImGui::BeginMenu("Debug"))
    {
        ImGui::MenuItem("Profiler", nullptr, &profiler_);
        ImGui::EndMenu();
    }

    // ===== Scene Info =====
    ImGui::Separator();
    ImGui::Text("Scene: %s", scenesHandler.currentSceneName());

    ImGui::EndMainMenuBar();
}


void interface::interfaces::draw()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();
    componentsInterfaces::menuBar();
    componentsInterfaces::profiler(interfaces::componentsInterfaces::profiler_);
    if(GameState.game_state == states::gameState::PAUSE)
    {
        interfaces::componentsInterfaces::hirarcy();
        interfaces::componentsInterfaces::inspector();
        interfaces::componentsInterfaces::scenesManager();
        interfaces::componentsInterfaces::fileExplorer();
        interfaces::componentsInterfaces::guizmo();
    }
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void interface::interfaces::cleanup()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}