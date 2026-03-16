#pragma once

#include <string>
#include <memory>
#include <unordered_map>
#include <imgui.h>
#include <variant>

namespace NodesModule
{

	enum class Pintype 
	{
		INT,
		FLOAT,
		STRING,
		BOOL
	};

	enum class PinKind
	{
		IN,
		OUT
	};

	using Value = std::variant<int, float, std::string, bool>;

	struct pinIn
	{
		int id = -1;
		std::string name = "";
		Value val;
		template<typename T>
		void setValue(T& newVal)
		{
			val.template emplace<T>(newVal);
		}
		Pintype type;
		ImVec2 pos = ImVec2{0,0};
	};

	struct pinOut
	{
		int id = -1;
		std::string name = "";
		Value val;  
		Pintype type;
		template<typename T>
		void getValue(T& newVal)
		{
			if (auto* v = std::get_if<T>(&val))
				newVal = *v;
		}
		ImVec2 pos = ImVec2{ 0,0 };
	};

	struct Connection
	{
		int fromNode;
		int fromPin;
		int toNode;
		int toPin;
	};

	class BaseNode
	{
	protected:
		std::string name;
		std::vector<pinIn> pinIns;
		std::vector<pinOut> pinOuts;
		pinIn& PinIN(std::string name, Pintype type)
		{
			int nextPinId = 0;
			pinIn p;
			p.id = nextPinId++;
			p.type = type;

			pinIns.push_back(p);
			return pinIns.back();
		};
		pinOut& pinOUT(std::string name, Pintype type)
		{
			int nextPinId = 0;
			pinOut k;
			k.id = nextPinId++;
			k.type = type;
			pinOuts.push_back(k);
			return pinOuts.back();
		};
		void setName(const std::string& val)
		{
			name = val;
		};

	public:
		virtual ~BaseNode() = default;

		virtual void setup() {};

		virtual void execute() {};

		ImVec2 pos;
		bool dragging;
		ImVec2 dragOffset;

		std::string getName() { return name; }
		std::vector<pinIn>& getPinIN() { return pinIns; }
		std::vector<pinOut>& getPinOUT() { return pinOuts; }

		int getPinIndByName(std::string v, PinKind kind);
	};

	extern std::unordered_map<int, std::unique_ptr<BaseNode>> Nodes;
	extern std::vector<Connection> connections;

	void connectPin(int selfNode, int selfPin, int targetNode, int targetPin);
	void connectionDataHandler(int selfNode, int selfPin, int targetNode, int targetPin);
	void init(void);

}