#include "game.h"

using namespace game;

namespace game
{
	states GameState;
}

states::gameState states::game_state = states::gameState::PAUSE;
states::controlState states::control_state = states::controlState::EDITOR;