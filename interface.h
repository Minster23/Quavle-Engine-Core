#pragma once
#include <iostream>
#include <string>
#include <vector>

namespace interface
{
	class interfaces 
	{
	public:
		bool init();
		void draw();
		void cleanup();
		static std::string forcedCameraUUID;

		void setSelectoredObject(const std::string& UID)
		{
			componentsInterfaces::selecetedObject = UID;
		}

		enum ComponentType
		{
			COMP_TRANSFORM,
			COMP_MATERIAL,
			COMP_TEXTURE,
			COMP_LIGHT,
			COMP_COLLUSION,
			COMP_AUDIO,
			COMP_CAMERA,
			COMP_TEXT,
			COMP_SHADER,
			COMP_MESH
		};

	private:

		struct componentsInterfaces
		{
			static void menuBar();
			static bool profiler_;
			static void profiler(bool open);
			static std::string selecetedObject;
			static bool hirarcy_;
			static void drawTree(const char* UID);
			static void hirarcy();
			static bool inspector_;
			static void inspector();
			static void guizmo();
			static bool sceneManager_;
			static void scenesManager();
			static bool fileExplorer_;
			static void fileExplorer();
		};

		static void changeParent();
	};

	extern interfaces Interfaces;
}