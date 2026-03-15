#include "interface.h"
#include "IconsFontAwesome6.h"
#include "graphModule.h"
#include "graphModule.h"

#include <functional>
#include <fstream>
#include <any>
#include <vector>
#include <unordered_map>
#include <random>
#include <cmath>

using namespace interface;
using namespace NodesModule;

std::string interface::interfaces::componentsInterfaces::graphEditorFileLoc_;
static bool initNodeModule = false;

namespace
{
	const float GRID_SZ = 64.0f;
	static int choosedNode = -1;
	static int nextNodeId = 0;
	static ImVec2 viewPos;
	
	// BUAT NODE PAKE CHILD (REWRITE NODE SYSTEM)

	void drawLine(ImVec2 source, ImVec2 target, ImVec2 position)
	{
		ImVec2 p1 = ImVec2(source.x + position.x + viewPos.x, source.y + position.y + viewPos.y);
		ImVec2 p2 = ImVec2(target.x + position.x + viewPos.x, target.y + position.y + viewPos.y);

		float dx = p2.x - p1.x;

		ImVec2 cp1 = ImVec2(p1.x + dx * 0.5f, p1.y);
		ImVec2 cp2 = ImVec2(p2.x - dx * 0.5f, p2.y);

		ImGui::GetWindowDrawList()->AddBezierCubic(
			p1,
			cp1,
			cp2,
			p2,
			IM_COL32(255, 0, 200, 200),
			12.0f
		);
	}
}

void renderNodes(ImVec2 size, ImVec2 position)
{
	for (const auto& node : NodesModule::Nodes)
	{
		ImGui::PushID(node.second->getName().c_str());
		ImGui::SetCursorScreenPos(ImVec2(node.second->pos.x + position.x + viewPos.x, node.second->pos.y + position.y + viewPos.y));
		ImGui::SetNextWindowSizeConstraints(ImVec2(100, 500), ImVec2(500, 1000));
		ImGui::BeginChild(node.second->getName().c_str(), ImVec2(100, 500), false, ImGuiChildFlags_Borders);
		ImDrawList* draw_list = ImGui::GetWindowDrawList();

		if (ImGui::CollapsingHeader(node.second->getName().c_str(), ImGuiTreeNodeFlags_Leaf))

			if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
			{
				node.second->dragging = true;
				choosedNode = node.first;

				node.second->dragOffset = ImVec2(
					ImGui::GetMousePos().x - (node.second->pos.x + position.x - viewPos.x),
					ImGui::GetMousePos().y - (node.second->pos.y + position.y - viewPos.y)
				);
			}
		if (node.second->dragging && ImGui::IsMouseReleased(0))
			node.second->dragging = false;

		if (node.second->dragging)
		{
			node.second->pos = ImVec2(
				ImGui::GetMousePos().x - position.x + viewPos.x - node.second->dragOffset.x,
				ImGui::GetMousePos().y - position.y + viewPos.y - node.second->dragOffset.y
			);
		}

		ImGui::EndChild();
		ImGui::PopID();
	}
	
}

void Canvas(ImVec2 size, ImVec2 position)
{
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	const ImU32 GRID_COLOR = ImGui::GetColorU32(IM_COL32(200, 200, 200, 40));
	static ImVec2 lastMouse;


	ImGui::BeginChild("Blueprint Timeline", ImVec2(ImGui::GetContentRegionAvail()), false, ImGuiWindowFlags_NoScrollbar | ImGuiChildFlags_AlwaysAutoResize);

	if (ImGui::IsMouseClicked(1 | 0) && ImGui::IsWindowHovered)
		choosedNode = -1;

	if (ImGui::IsMouseDoubleClicked(1) && ImGui::IsWindowHovered && ImGui::IsWindowFocused)
		ImGui::OpenPopup("OPEN_NODE_LIST");

	if (ImGui::BeginPopup("OPEN_NODE_LIST"))
	{
		ImGui::LabelText("Nodes", "List of Nodes");

		ImGui::EndPopup();
	}

	if (ImGui::IsWindowHovered() && ImGui::IsMouseDown(1))
	{
		ImVec2 mouse = ImGui::GetMousePos();
		viewPos.x += mouse.x - lastMouse.x;
		viewPos.y += mouse.y - lastMouse.y;
	}

	lastMouse = ImGui::GetMousePos();

	float xoff = fmodf(viewPos.x, GRID_SZ);
	float yoff = fmodf(viewPos.y, GRID_SZ);

	for (float x = -GRID_SZ; x < size.x + GRID_SZ; x += GRID_SZ)
		draw_list->AddLine(
			ImVec2(position.x + x + xoff, position.y),
			ImVec2(position.x + x + xoff, position.y + size.y),
			GRID_COLOR);

	for (float y = -GRID_SZ; y < size.y + GRID_SZ; y += GRID_SZ)
		draw_list->AddLine(
			ImVec2(position.x, position.y + y + yoff),
			ImVec2(position.x + size.x, position.y + y + yoff),
			GRID_COLOR);

	//drawLine(nodeList[2].pins[1].pos, nodeList[0].pins.back().pos, position);

	renderNodes(size, position);

	ImGui::EndChild();
}

void InfoPanel()
{
	auto it = Nodes.find(choosedNode);
	if (it == Nodes.end())
		return;

	auto& Data = it->second;

	ImGui::Begin("Inspector Node");

	if (ImGui::CollapsingHeader("Data"))
	{
		ImGui::Text("%s", Data->getName().c_str());
		ImGui::TextDisabled("X: %f", Data->pos.x);
		ImGui::SameLine();
		ImGui::TextDisabled("Y: %f", Data->pos.y);
	}

	ImGui::End();
}

void interface::interfaces::componentsInterfaces::graphEditor()
{
	if (graphEditorFileLoc_ == "")
		return;

	if(!initNodeModule)
	{
		NodesModule::init();
		initNodeModule = true;
	}

	ImGui::Begin(ICON_FA_TACHOGRAPH_DIGITAL "Graph Editor", nullptr, ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoScrollbar);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Save"))
			{
			}
			ImGui::EndMenu();
		}
		ImGui::EndMenuBar();
	}
	Canvas(ImGui::GetContentRegionAvail(), ImGui::GetCursorScreenPos());
	ImGui::End();

	InfoPanel();
}