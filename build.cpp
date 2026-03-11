#include <QuavleFormat.h>

#include "build.h"
#include "game.h"

using namespace QuaveleShaderFormater;
using namespace build;
using namespace std;
using namespace game;

namespace build
{
	compile Compile;
}

void compile::compileShaders(std::string costumShader)
{
	if (costumShader != "Null")
	{
		cout << "Compiling custom shader: " << costumShader << endl;
		auto shaderVert = ShaderFormatter::readGlsl(costumShader, ShaderType::Vertex);
		if (shaderVert)
			ShaderFormatter::saveToFile(game::gameData.projectLocation, *shaderVert);
	}
	else
	{
		cout << "Compiling default shaders..." << endl;
		auto shaderVert = ShaderFormatter::readGlsl(gameData.projectLocation + "\\saveData\\shader\\vertex.glsl", ShaderType::Vertex);
		if (shaderVert)
			ShaderFormatter::saveToFile(gameData.projectLocation + compile::CompiledLocation.vertexShader, *shaderVert);
		cout << gameData.projectLocation + compile::CompiledLocation.vertexShader << endl;

		auto shaderFrag = ShaderFormatter::readGlsl(gameData.projectLocation + "\\saveData\\shader\\fragment.glsl", ShaderType::Fragment);
		if (shaderFrag)
			ShaderFormatter::saveToFile(gameData.projectLocation + compile::CompiledLocation.fragmentShader, *shaderFrag);

		auto shaderVertText = ShaderFormatter::readGlsl(gameData.projectLocation + "\\saveData\\shader\\textVert.vs", ShaderType::Vertex);
		if (shaderVertText)
			ShaderFormatter::saveToFile(gameData.projectLocation + compile::CompiledLocation.vertexText, *shaderVert);

		auto shaderFragText = ShaderFormatter::readGlsl(gameData.projectLocation + "\\saveData\\shader\\textFrag.fs", ShaderType::Fragment);
		if (shaderFragText)
			ShaderFormatter::saveToFile(gameData.projectLocation + compile::CompiledLocation.FragmentText, *shaderFrag);
	}
}