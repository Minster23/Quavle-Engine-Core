#include "game.h"
#include <string>

using namespace game;

namespace game
{
	states GameState;
    gameMetaData gameData;

    std::string gameMetaData::projectName;
    std::string gameMetaData::projectLocation;
    std::string gameMetaData::gamePlayName;
    std::string gameMetaData::authorName;
    std::string gameMetaData::version;
    std::string gameMetaData::description;
}

states::gameState states::game_state = states::gameState::PAUSE;
states::controlState states::control_state = states::controlState::EDITOR;

