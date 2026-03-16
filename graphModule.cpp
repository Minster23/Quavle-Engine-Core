#include "graphModule.h"
#include <imgui.h>
#include <iostream>
#include <algorithm>
#include <iterator>

using namespace NodesModule;

namespace NodesModule
{
	std::unordered_map<int, std::unique_ptr<BaseNode>> Nodes;
	std::vector<Connection> connections;
}

int NodesModule::BaseNode::getPinIndByName(std::string v, PinKind kind)
{
	if(kind == PinKind::IN)
	{
		for (auto& p : BaseNode::getPinIN())
		{
			if (p.name.c_str() == v)
				return p.id;
		}
	}
	if (kind == PinKind::OUT)
	{
		for (auto& p : BaseNode::getPinOUT())
		{
			if (p.name.c_str() == v)
				return p.id;
		}
	}

}

void NodesModule::connectPin(int selfNode, int selfPin, int targetNode, int targetPin)
{
	if (selfNode < 0 || selfPin < 0)
		return;

	auto& nFT = Nodes[selfNode]->getPinIN()[selfPin].type;
	auto& nTT = Nodes[targetNode]->getPinOUT()[targetPin].type;

	if (nFT != nTT)
		return;

	for (auto& c : NodesModule::connections)
	{
		if (c.fromNode == selfNode &&
			c.fromPin == selfPin &&
			c.toNode == targetNode &&
			c.toPin == targetPin)
			return;
	}

	Connection con;
	con.fromNode = selfNode;
	con.fromPin = selfPin;
	con.toNode = targetNode;
	con.toPin = targetPin;
	connections.push_back(con);
	return;
}

void NodesModule::connectionDataHandler(int selfNode, int selfPin, int targetNode, int targetPin)
{
	auto& N1 = Nodes[selfNode];
	auto& N2 = Nodes[targetNode];

	if (N1.get()->getName() == "" || N2.get()->getName() == "")
		return;

	auto& P1 = N1->getPinIN()[selfPin];
	auto& P2 = N2->getPinOUT()[targetPin];

	if (P1.type != P2.type)
		return;

	P2.val = P1.val;
}
class Transform : public NodesModule::BaseNode
{
private:
	float value = 0.0f;
	std::string name = "KK";
public:
	void setup() override
	{
		setName("Transform");
		PinIN("X", Pintype::STRING);
	}

	void execute() override 
	{
		getPinIN()[0].setValue<std::string>(name);
		ImGui::DragFloat("Val", &value);
	}
};

class Mesh : public NodesModule::BaseNode
{
private:
	float Size = 0.0f;
	std::string name = "";

public:
	void setup() override
	{
		setName("Mesh");
		pinOUT("Size", Pintype::STRING);
	}

	void execute() override
	{
		getPinOUT()[0].getValue<std::string>(name);
		ImGui::Text(name.c_str());
	}
};

void NodesModule::init(void)
{
	Nodes[0] = std::make_unique<Transform>();
	Nodes[1] = std::make_unique<Mesh>();

	for (const auto& pair : Nodes) {
		pair.second->setup();
		std::cout << pair.second->getName() + " - INITIALIZE" << std::endl;
	}
}