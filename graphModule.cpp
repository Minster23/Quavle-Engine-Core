#include "graphModule.h"
#include <iostream>

namespace NodesModule
{
	std::unordered_map<int, std::unique_ptr<BaseNode>> Nodes;
}

class Transform : public NodesModule::BaseNode
{
	void setup() override
	{
		name = "Transform Node";
		cond = NodesModule::ACTIVE;
		pos = ImVec2(200,200);
		dragging = false;
		dragOffset = ImVec2(0, 0);
	}


};

void NodesModule::init(void)
{
	Nodes[0] = std::make_unique<Transform>();

	for (const auto& pair : Nodes) {
		pair.second->setup();
		std::cout << pair.second->getName() + " - INITIALIZE" << std::endl;
	}
}