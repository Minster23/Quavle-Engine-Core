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

		void setDetailId(std::string id)
		{
			componentsInterfaces::detailItemId_ = id;
		}

		void setCodeEditorOpened(std::string fileLoc)
		{
			componentsInterfaces::codeEditorFileLoc_ = fileLoc;
		}

		void setGraphEditorOpened(std::string fileLoc)
		{
			componentsInterfaces::graphEditorFileLoc_ = fileLoc;
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
			static bool buildWindow_;
			static void buildWindow();
			static bool canvasOverlay_;
			static void canvasOverlay();
			static std::string detailItemId_;
			static void detailItem();
			static void floatingInfo();
			static std::string codeEditorFileLoc_;
			static void codeEditor();
			static std::string graphEditorFileLoc_;
			static void graphEditor();
		};

		void viewport(std::string title, unsigned int tex);

		static void changeParent();
	};

	struct ViewportRect {
		float x = 0, y = 0;
		float width = 1920, height = 1080;
	};

	extern ViewportRect sceneViewport; 
	extern interfaces Interfaces;
}