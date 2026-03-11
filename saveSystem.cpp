#include <QuavleFormat.h>
#include <sstream>
#include <cstring>


#include "entity.h"
#include "saveSystem.h"
#include "game.h"

using namespace saved;
using namespace entity;
using namespace entity::canvas;
using namespace QuavleData;
using namespace QuaveleShaderFormater;
using namespace game;

namespace saved
{
    savesystem saveSystem;
	saved::savePaths SavePath;
}

void saveMetadata()
{
    QuavleData::Light MetaData;

    MetaData.setString("GameName", game::gameData.gamePlayName);
    MetaData.setString("AuthorName", game::gameData.authorName);
    MetaData.setString("Version", game::gameData.version);

   
    MetaData.saveToFile(gameData.projectLocation + saved::SavePath.metadata);
}

void saved::savesystem::save()
{
#if WITH_EDITOR
    saveMetadata();
#else

#endif
}

std::string saved::savesystem::gamePlayName()
{
    QuavleData::Light LoadedData;
    std::string name = "QUAVLE ENGINE";

    if (LoadedData.loadFromFile(gameData.projectLocation + saved::SavePath.metadata))
    {
        name = LoadedData.getString("GameName");
    }

    return name;
}

void saved::savesystem::load()
{
#if WITH_EDITOR
    std::cout << "Load Game Projects" << std::endl;
#else

#endif
}