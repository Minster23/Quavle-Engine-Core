#pragma once

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

	extern states GameState;
}
