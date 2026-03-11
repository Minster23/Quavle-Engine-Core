#pragma once
#include <iostream>

namespace game
{
	class states
	{
	public:
		static enum class gameState
		{
			PLAY,
			DEBUG,
			PAUSE,
		} game_state;

		static enum class controlState
		{
			EDITOR,
			GAMEPLAY,
			DEBUG
		} control_state;
	};

	struct gameMetaData
	{
		static std::string projectName;
		static std::string projectLocation;
		static std::string gamePlayName;
		static std::string authorName;
		static std::string version;
		static std::string description;
	};

	extern gameMetaData gameData;
	extern states GameState;
}
