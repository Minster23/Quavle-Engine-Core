#include "graphModule.h"
#include <imgui.h>
#include <iostream>
#include <algorithm>
#include <iterator>

using namespace NodesModule;
static int nextID = 0;

namespace NodesModule
{
	std::unordered_map<int, std::unique_ptr<BaseNode>> NodesPool;
	std::unordered_map<std::string, std::vector<std::unique_ptr<BaseNode>>> NodesInFile;
	std::vector<Connection> connections;
}

void NodesModule::addNode(std::unique_ptr<BaseNode> Node, std::string FileNow)
{
	NodesInFile[FileNow].push_back(std::move(Node));
}

void NodesModule::connectPin(int selfNode, int selfPin, int targetNode, int targetPin,std::string FileNow)
{
	if (selfNode < 0 || selfPin < 0)
		return;

	auto& Nodes = NodesInFile[FileNow];

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

void NodesModule::connectionDataHandler(int selfNode, int selfPin, int targetNode, int targetPin, std::string FileNow)
{
	auto& Nodes = NodesInFile[FileNow];
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
	float X = 0.0f;
	float Y = 0.0f;
	float Z = 0.0f;
public:
	void setup() override
	{
		setName("Transform");
		setSize(700, 420);
		PinIN("X", Pintype::FLOAT);
		PinIN("Y", Pintype::FLOAT);
		PinIN("Z", Pintype::FLOAT);
	}

	void execute() override 
	{
		getPinIN()[0].setValue<float>(X);
		getPinIN()[1].setValue<float>(Y);
		getPinIN()[2].setValue<float>(Z);
		ImGui::DragFloat("X", &X);
		ImGui::DragFloat("Y", &Y);
		ImGui::DragFloat("Z", &Z);
	}

	void detail() override
	{

	}

	std::unique_ptr<BaseNode> clone() const override {
		return std::make_unique<Transform>(*this);
	}
};

class Mesh : public NodesModule::BaseNode
{
private:
	float X = 0.0f;
	float Y = 0.0f;
	float Z = 0.0f;
	std::string name = "";

public:
	void setup() override
	{
		setName("Mesh");
		setSize(700, 500);
		pinOUT("X", Pintype::FLOAT);
		pinOUT("Y", Pintype::FLOAT);
		pinOUT("Z", Pintype::FLOAT);
	}

	void execute() override
	{
		getPinOUT()[0].getValue<float>(X);
		getPinOUT()[1].getValue<float>(Y);
		getPinOUT()[2].getValue<float>(Z);
		ImGui::Text("%f", X);
		ImGui::Text("%f", Y);
		ImGui::Text("%f", Z);
	}

	void detail() override
	{

	}

	std::unique_ptr<BaseNode> clone() const override {
		return std::make_unique<Mesh>(*this);
	}
};

template<typename T>
int insertNode()
{
	int id = nextID++;
	NodesPool[id] = std::make_unique<T>();
	return id;
}

template<typename... T>
void insertNodes()
{
	(insertNode<T>(), ...);
}

void NodesModule::init(void)
{
	insertNodes<Transform, Mesh>();

	for (const auto& pair : NodesPool) {
		pair.second->setup();
	}
}