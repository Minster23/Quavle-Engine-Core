#include <QuavleFormat.h>
#include <sstream>
#include <cstring>


#include "entity.h"
#include "saveSystem.h"
#include "window.h"

using namespace saved;
using namespace entity;
using namespace entity::canvas;
using namespace window;
using namespace Quavle;

SaveOptions options;
Saver saver;
Loader loader;

void saved::savesystem::save() 
{
#if WITH_EDITOR

#endif
}

void saved::savesystem::load() 
{
#if WITH_EDITOR
	std::cout << "Load Game Projects\n";
#else
	std::cout << "Load Game Assets\n";
#endif
}