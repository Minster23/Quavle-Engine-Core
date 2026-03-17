#include "interface.h"
#include "IconsFontAwesome6.h"
#include "graphModule.h"

#include <functional>
#include <unordered_map>
#include <random>

#include <iostream>
#include <string>
#include <variant>

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
	static bool _showConnList = false;

	static void drawLine(ImVec2 source, ImVec2 target, ImVec2 position)
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
			5.0f
		);
	}

	static bool canDrag = false;
	static int fromNode = -1;
	static int fromPin = -1;
}

void renderNodes(ImVec2 size, ImVec2 position)
{
	for (const auto& node : NodesModule::Nodes)
	{
		ImGui::PushID(node.first);

		ImGui::SetCursorScreenPos(ImVec2(
			node.second->pos.x + position.x + viewPos.x,
			node.second->pos.y + position.y + viewPos.y
		));

		ImGui::SetNextWindowSizeConstraints(ImVec2(180, 120), node.second->getSize());

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 8));
		ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(6, 6));

		ImGui::BeginChild(node.second->getName().c_str(), ImVec2(180, 0), true, ImGuiChildFlags_Borders);

		// HEADER
		ImGui::PushStyleColor(ImGuiCol_Header, ImVec4(0.2f, 0.2f, 0.2f, 1));
		ImGui::PushStyleColor(ImGuiCol_HeaderHovered, ImVec4(0.3f, 0.3f, 0.3f, 1));

		if (ImGui::CollapsingHeader(node.second->getName().c_str(), ImGuiTreeNodeFlags_Leaf))
		{
		}

		ImGui::PopStyleColor(2);

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

		ImGui::Separator();

		// NODE CONTENT
		node.second->execute();

		ImGui::Spacing();
		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Columns(2, nullptr, false);

		for (auto& o : node.second->getPinOUT())
		{
			ImGui::PushID(o.id);

			ImGui::AlignTextToFramePadding();
			ImGui::Text(ICON_FA_ARROW_LEFT);

			ImVec2 itemMax = ImGui::GetItemRectMax();
			ImVec2 itemMin = ImGui::GetItemRectMin();

			o.pos = ImVec2(
				itemMax.x - viewPos.x - position.x + (itemMin.x - itemMax.x),
				itemMax.y - viewPos.y - position.y + (itemMin.y - itemMax.y)
			);

			if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && canDrag)
			{
				NodesModule::connectPin(fromNode, fromPin, node.first, o.id);
				canDrag = false;
			}

			ImGui::PopID();
		}

		ImGui::NextColumn();

		for (auto& i : node.second->getPinIN())
		{
			ImGui::PushID(i.id);

			ImGui::AlignTextToFramePadding();
			ImGui::Text(ICON_FA_ARROW_RIGHT);

			ImVec2 itemMax = ImGui::GetItemRectMax();
			ImVec2 itemMin = ImGui::GetItemRectMin();

			i.pos = ImVec2(
				itemMax.x - viewPos.x - position.x + (itemMin.x - itemMax.x),
				itemMax.y - viewPos.y - position.y + (itemMin.y - itemMax.y)
			);

			if (ImGui::IsItemClicked(ImGuiMouseButton_Left) && !canDrag)
			{
				fromNode = node.first;
				fromPin = i.id;
				canDrag = true;
			}

			ImGui::PopID();
		}

		ImGui::Columns(1);

		ImGui::EndChild();

		ImGui::PopStyleVar(2);
		ImGui::PopID();
	}
	
}

void Canvas(ImVec2 size, ImVec2 position)
{
	ImDrawList* draw_list = ImGui::GetWindowDrawList();
	const ImU32 GRID_COLOR = ImGui::GetColorU32(IM_COL32(200, 200, 200, 40));
	static ImVec2 lastMouse;

	ImGui::BeginChild("Blueprint Timeline", ImVec2(ImGui::GetContentRegionAvail()), false);

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

	renderNodes(size, position);

	for (auto& con : NodesModule::connections)
	{
		auto& source = NodesModule::Nodes[con.fromNode]->getPinIN()[con.fromPin].pos;
		auto& target = NodesModule::Nodes[con.toNode]->getPinOUT()[con.toPin].pos;


		drawLine(source, target, position);
		NodesModule::connectionDataHandler(con.fromNode, con.fromPin, con.toNode, con.toPin);
	}

	if(canDrag)
	{
		if (ImGui::IsMouseClicked(ImGuiMouseButton_Right))
			canDrag = false;

		auto& d = NodesModule::Nodes[fromNode]->getPinIN()[fromPin];
		ImVec2 mousePos = ImVec2{ ImGui::GetMousePos().x - viewPos.x - position.x  , ImGui::GetMousePos().y - viewPos.x - position.x };

		drawLine(d.pos, mousePos, position);
	}

	ImGui::EndChild();
}

void InfoPanel()
{
	auto it = Nodes.find(choosedNode);
	if (it == Nodes.end())
		return;

	auto& Data = it->second;

	ImGui::BeginChild("Inspector Node");

	if (ImGui::CollapsingHeader("Data"))
	{
		ImGui::Text("%s", Data->getName().c_str());
		ImGui::TextDisabled("X: %f", Data->pos.x);
		ImGui::SameLine();
		ImGui::TextDisabled("Y: %f", Data->pos.y);
	}

	ImGui::EndChild();
}

void ConnectionList()
{
	if (!_showConnList)
		return;

	ImGui::BeginChild("Connection List");

	if (ImGui::BeginTable("connections_table", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
	{
		ImGui::TableSetupColumn("From Node");
		ImGui::TableSetupColumn("From Pin");
		ImGui::TableSetupColumn("To Node");
		ImGui::TableSetupColumn("To Pin");
		ImGui::TableHeadersRow();

		for (auto& con : NodesModule::connections)
		{
			ImGui::TableNextRow();

			ImGui::TableSetColumnIndex(0);
			ImGui::Text("%d", con.fromNode);

			ImGui::TableSetColumnIndex(1);
			ImGui::Text("%d", con.fromPin);

			ImGui::TableSetColumnIndex(2);
			ImGui::Text("%d", con.toNode);

			ImGui::TableSetColumnIndex(3);
			ImGui::Text("%d", con.toPin);
		}

		ImGui::EndTable();
	}

	ImGui::EndChild();
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

	ImGui::Begin(ICON_FA_TACHOGRAPH_DIGITAL " Graph Editor", nullptr, ImGuiWindowFlags_MenuBar);

	if (ImGui::BeginMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			ImGui::Checkbox("Connection List", &_showConnList);
			if (ImGui::MenuItem("Save"))
			{
			}
			ImGui::EndMenu();

		}
		ImGui::EndMenuBar();
	}

	ImGui::BeginTable("MainLayout", 2, ImGuiTableFlags_Resizable);

	ImGui::TableNextColumn();
	ImGui::BeginChild("LeftSide", ImVec2(0, 0), false);

	float connectionHeight = 150.0f;

	ImGui::BeginChild("CanvasChild", ImVec2(0, -connectionHeight), true);
	Canvas(ImGui::GetContentRegionAvail(), ImGui::GetCursorScreenPos());
	ImGui::EndChild();
	ImGui::BeginChild("ConnectionChild", ImVec2(0, connectionHeight), true);
	ConnectionList();
	ImGui::EndChild();
	ImGui::EndChild();
	ImGui::TableNextColumn();
	ImGui::BeginChild("InfoPanelChild", ImVec2(0, 0), true);
	InfoPanel();
	ImGui::EndChild();

	ImGui::EndTable();

	ImGui::End();
}