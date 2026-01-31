#include "scene.h"
#include "entity.h"

using namespace scene;
using namespace entity;

namespace scene
{
	Scene scenesHandler;
}

std::vector<scene::Scene::sceneData> scene::Scene::scenes;
int scene::Scene::totalScene = 0;
int scene::Scene::currentScene = -1;
bool scene::Scene::isPlay = false;

void removeObjects(const std::string& UUID)
{
	if (UUID.empty()) return;

	for (auto& [id, data] : components::entity_data)
	{
		if (data.Parent == UUID)
			data.Parent.clear();
	}

	gameobjects.removeAudio(UUID.c_str());

	// 2. Hapus semua item canvas (Text)
	//entity::canvas::item_text.erase(UUID);
	//entity::canvas::item_position.erase(UUID);
	//entity::canvas::item_itemData.erase(UUID);
	//entity::canvas::item_shader.erase(UUID);

	// 3. Hapus semua komponen berdasarkan UUID
	components::entity_transform.erase(UUID);
	components::entity_texture.erase(UUID);
	components::entity_material.erase(UUID);
	components::entity_light.erase(UUID);
	components::entity_mesh.erase(UUID);
	components::entity_shader.erase(UUID);
	components::entity_camera.erase(UUID);
	components::entity_collusion.erase(UUID);
	components::entity_audio.erase(UUID);

	// 4. Hapus world vertices (jika ada)
	entity::gameObject::mesh_world_vertices.erase(UUID);

	components::entity_data.erase(UUID);

	std::cout << "Object Removed Successfully: " << UUID << std::endl;
}

void scene::Scene::addScene(const char* sceneName)
{
	data.sceneName = sceneName;
	data.sceneUUID = sceneName + totalScene;
	data.sceneNum = totalScene;
	data.objectCount = 0;
	data.objectListID.clear();
	scenes.push_back(data);
	totalScene++;
}

void scene::Scene::inputSceneObject(const char* sceneName, const char* objectID)
{
	for (auto& scene : scenes)
	{
		if (scene.sceneName == sceneName)
		{
			scene.objectListID.push_back(objectID);
			scene.objectCount++;
			break;
		}
	}
}

bool scene::Scene::canObjectShown(const char* objectID)
{
	for (scene::Scene::sceneData& scene : scenes)
	{
		if(scene.sceneName == scenes[currentScene].sceneName)
		{
			for(const std::string& objID : scene.objectListID)
			{
				if(objID == objectID)
				{
					return true;
				}
			}
		}
	}
	return false;
}

void scene::Scene::removeScene(const char* sceneName)
{
	for (const auto& scene : scenes)
	{
		if (scene.sceneName == sceneName)
		{
			for (const auto& obj : scene.objectListID)
			{
				removeObjects(obj.c_str());
			}
			break;
		}
	}
	for (auto it = scenes.begin(); it != scenes.end(); ++it)
	{
		if (it->sceneName == sceneName)
		{
			scenes.erase(it);
			totalScene--;
			break;
		}
	}
}
