#pragma once
#include <iostream>
#include <string>

namespace build
{
	class compile
	{
	private:
		struct compiledLocation
		{
			std::string vertexShader = "\\game\\compiled\\vertex.quashader";
			std::string fragmentShader = "\\game\\compiled\\fragment.quashader";
			std::string vertexText = "\\game\\compiled\\textVert.quashader";
			std::string FragmentText = "\\game\\compiled\\textFrag.quashader";

		} CompiledLocation;
	public:

		void compileShaders(std::string costumShader = "Null");
	};

	extern compile Compile;
}