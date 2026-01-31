#pragma once
#include <string>
#include <vector>

namespace scene
{
	class Scene
	{
	private:
		struct sceneData
		{
			std::string sceneName = "Untitled";
			std::string sceneUUID = "";
			std::int16_t sceneNum = 0;
			std::vector<std::string> objectListID;
			int objectCount = 0;
		} data;
		static bool isPlay;

	public:
		void addScene(const char* sceneName);

		void removeScene(const char* sceneName);
		void inputSceneObject(const char* sceneName, const char* objectID);
		bool canObjectShown(const char* objectID);
		void setCurrentScene(const char* sceneName)
		{
			for (int i = 0; i < totalScene; i++)
			{
				if (scenes[i].sceneName == sceneName)
				{
					currentScene = i;
					return;
				}
			}
		}

		std::string currentSceneName()
		{
			if (currentScene >= 0 && currentScene < totalScene)
			{
				return scenes[currentScene].sceneName;
			}
			return "No Scene";
		}	

		void setPlayMode(bool playmode)
		{
			isPlay = playmode;
		}
		static int totalScene;
		static int currentScene;
		static std::vector<sceneData> scenes;
	};

	extern Scene scenesHandler;
}