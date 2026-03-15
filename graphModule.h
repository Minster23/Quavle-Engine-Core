#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <imgui.h>

namespace NodesModule
{

	enum nodeCondition
	{
		ACTIVE,
		NOT_ACTIVE
	};

	class BaseNode
	{
	protected:
		std::string name;
		nodeCondition cond;

	public:
		virtual ~BaseNode() = default;

		virtual void setup()
		{

		}

		virtual void pins()
		{

		}

		virtual void execute()
		{

		}

		ImVec2 pos;
		bool dragging;
		ImVec2 dragOffset;

		std::string getName() { return name; }
	};

	extern std::unordered_map<int, std::unique_ptr<BaseNode>> Nodes;

	void init(void);


}