#include "interface.h"
#include <implot.h>
#include <imgui.h>
#include <imgui_internal.h>
#include <chrono>

using namespace interface;

bool interface::interfaces::componentsInterfaces::profiler_ = false;

void interface::interfaces::componentsInterfaces::profiler(bool open)
{
    if (!open)
        return;

    // ===== STATIC STATE =====
    static bool initialized = false;
    static auto lastTime = std::chrono::high_resolution_clock::now();
    static int frameCount = 0;

    static float fps = 0.0f;
    static float cpuMs = 0.0f;
    static float gpuMs = 0.0f;

    static float fpsHistory[120] = {};
    static float cpuHistory[120] = {};
    static float gpuHistory[120] = {};
    static int offset = 0;

    if (!initialized)
    {
        ImPlot::CreateContext();
        initialized = true;
    }

    // ===== FPS / CPU / GPU CALC =====
    frameCount++;
    auto now = std::chrono::high_resolution_clock::now();
    float delta = std::chrono::duration<float>(now - lastTime).count();

    if (delta >= 1.0f)
    {
        fps = frameCount / delta;
        cpuMs = 1000.0f / fps;
        gpuMs = cpuMs * 0.85f;

        fpsHistory[offset] = fps;
        cpuHistory[offset] = cpuMs;
        gpuHistory[offset] = gpuMs;

        offset = (offset + 1) % 120;
        frameCount = 0;
        lastTime = now;
    }

    // ===== COLORS =====
    ImVec4 fpsColor =
        fps < 30 ? ImVec4(1, 0.2f, 0.2f, 1) :
        fps < 55 ? ImVec4(1, 0.9f, 0.2f, 1) :
        ImVec4(0.2f, 1, 0.3f, 1);

    ImVec4 cpuColor = ImVec4(0.3f, 0.7f, 1.0f, 1.0f);
    ImVec4 gpuColor = ImVec4(0.9f, 0.4f, 1.0f, 1.0f);

    // ===== WINDOW =====
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration |
        ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings |
        ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav |
        ImGuiWindowFlags_NoBackground;

    ImGui::SetNextWindowPos(ImVec2(12, 12), ImGuiCond_Always);
    ImGui::Begin("Profiler", &open, flags);
    {
        ImGui::TextColored(fpsColor, "FPS : %.1f", fps);
        ImGui::TextColored(cpuColor, "CPU : %.2f ms", cpuMs);
        ImGui::TextColored(gpuColor, "GPU : %.2f ms", gpuMs);

        ImVec2 plotSize(260, 70);

        // ===== FPS GRAPH =====
        ImPlot::PushStyleColor(ImPlotCol_Line, fpsColor);
        if (ImPlot::BeginPlot("##FPS", plotSize,
            ImPlotFlags_NoLegend | ImPlotFlags_NoFrame | ImPlotFlags_NoMouseText))
        {
            ImPlot::SetupAxes(nullptr, nullptr,
                ImPlotAxisFlags_NoDecorations,
                ImPlotAxisFlags_NoDecorations);
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 144, ImGuiCond_Always);
            ImPlot::PlotLine("FPS", fpsHistory, 120, 1, 0, offset);
            ImPlot::EndPlot();
        }
        ImPlot::PopStyleColor();

        // ===== CPU GRAPH =====
        ImPlot::PushStyleColor(ImPlotCol_Line, cpuColor);
        if (ImPlot::BeginPlot("##CPU", plotSize,
            ImPlotFlags_NoLegend | ImPlotFlags_NoFrame | ImPlotFlags_NoMouseText))
        {
            ImPlot::SetupAxes(nullptr, nullptr,
                ImPlotAxisFlags_NoDecorations,
                ImPlotAxisFlags_NoDecorations);
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 50, ImGuiCond_Always);
            ImPlot::PlotLine("CPU", cpuHistory, 120, 1, 0, offset);
            ImPlot::EndPlot();
        }
        ImPlot::PopStyleColor();

        // ===== GPU GRAPH =====
        ImPlot::PushStyleColor(ImPlotCol_Line, gpuColor);
        if (ImPlot::BeginPlot("##GPU", plotSize,
            ImPlotFlags_NoLegend | ImPlotFlags_NoFrame | ImPlotFlags_NoMouseText))
        {
            ImPlot::SetupAxes(nullptr, nullptr,
                ImPlotAxisFlags_NoDecorations,
                ImPlotAxisFlags_NoDecorations);
            ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 50, ImGuiCond_Always);
            ImPlot::PlotLine("GPU", gpuHistory, 120, 1, 0, offset);
            ImPlot::EndPlot();
        }
        ImPlot::PopStyleColor();
    }
    ImGui::End();
}
