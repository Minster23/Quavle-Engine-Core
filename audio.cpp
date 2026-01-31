#include "entity.h"
#include "scene.h"
#include <Audio/sound.hpp>
#include <Audio/Device.hpp>
#include <iostream>
#include <filesystem>
namespace fs = std::filesystem;

using namespace entity;
using namespace scene;

std::unordered_map<std::string, components::Audio> components::entity_audio;
static std::unordered_map<std::string, Audio::Sound> audioPlace;
static std::unordered_map<std::string, Audio::Listener> listenerPlace;

void entity::gameObject::removeAudio(const char* UUID)
{
	if (componentss.entity_audio.find(UUID) != componentss.entity_audio.end())
	{
		components::entity_audio.erase(UUID);
		audioPlace.erase(UUID);
	}
}

void updateSpaceAudio(const char* UUID, const char* listenerUUID)
{
	auto& audioComp = components::entity_audio[UUID];

	// dapatkan posisi audio source
	glm::vec3 srcPos = components::entity_transform[UUID].location;

	// dapatkan posisi listener
	glm::vec3 lstPos = components::entity_transform[listenerUUID].location;

	// hitung distance
	float distance = glm::distance(srcPos, lstPos);

	// parameter falloff
	float maxDistance = 25.0f;      // sesuaikan
	float minDistance = 1.0f;       // jarak minimum
	float baseVolume = audioComp.volume; // volume original slider

	// clamp
	distance = glm::clamp(distance, minDistance, maxDistance);

	// linear falloff
	float attenuated = 1.0f - ((distance - minDistance) / (maxDistance - minDistance));

	// apply final volume
	float finalVolume = baseVolume * attenuated;

	audioPlace[UUID].setVolume(finalVolume);
}

void entity::gameObject::addAudio(const char* Location, const char* UUID)
{
	fs::path file_path = Location;

	if (!fs::exists(file_path))
		return;

	components::entity_data[UUID] = 
	{
		.Name = "Location",
		.Parent = "",
		.UUID = UUID,
		.show_ = true,
		.typeObject = components::type::OBJECT,
	};

	components::entity_transform[UUID] = 
	{
		.location = { 0.0f, 0.0f, 0.0f },
		.rotation = { 0.0f, 0.0f, 0.0f },
		.scale = { 1.0f, 1.0f, 1.0f },
		.localMatrix = glm::mat4(1.0f),
		.worldMatrix = glm::mat4(1.0f)
	};

	components::Audio audioComponent;
	audioComponent.audioPath = std::string(Location);
	audioComponent.isPlaying = false;
	audioComponent.volume = 1.0f;
	audioComponent.loop = false;
	audioComponent.spatial = false;
	components::entity_audio[UUID] = audioComponent;
	audioPlace[UUID] = Audio::Sound(Location, Audio::Sound::Type::Sound);
	scenesHandler.inputSceneObject(scenesHandler.currentSceneName().c_str(), UUID);
}

void entity::gameObject::addListener(const char* UUID)
{
	listenerPlace[UUID] = Audio::Device::getListener();;
	listenerPlace[UUID].setPosition(
	{
		components::entity_transform[UUID].location.x,
		components::entity_transform[UUID].location.y,
		components::entity_transform[UUID].location.z
		});
}

void entity::gameObject::playAudio(const char* UUID)
{
	auto& audioComp = components::entity_audio[UUID];
	audioPlace[UUID].loadMusic(audioComp.audioPath);
	audioPlace[UUID].play();
}

void entity::gameObject::stopAudio(const char* UUID)
{
	auto& audioComp = components::entity_audio[UUID];
	audioPlace[UUID].stop();
}

void entity::gameObject::updateAudio()
{
	for (auto& obj : listenerPlace)
	{
		Audio::Vector position;
		position =
		{
			components::entity_transform[obj.first].location.x,
			components::entity_transform[obj.first].location.y,
			components::entity_transform[obj.first].location.z
		};
		obj.second.setPosition(position);
	}

	for (auto& [UUID, audioComp] : components::entity_audio)
	{
		audioPlace[UUID].setVolume(audioComp.volume);
		audioComp.progress.duration = audioPlace[UUID].getDurationInSeconds();
		audioComp.progress.currentTime = audioPlace[UUID].getCursorInSeconds();
		audioComp.isPlaying = audioPlace[UUID].isPlaying();
		updateSpaceAudio(UUID.c_str(), "ENGINE_CAM");
		if(audioComp.spatial)
		{
			Audio::Vector position;
			position = 
			{
				components::entity_transform[UUID].location.x,
				components::entity_transform[UUID].location.y,
				components::entity_transform[UUID].location.z
			};
			audioPlace[UUID].setPosition(position);
		}
	}
}


