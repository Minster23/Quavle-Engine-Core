#pragma once
#include <iostream>
#include <string>
#include <vector>

namespace saved
{
    class savesystem
    {
    public:
        static void save();
        static void load();

        std::string gamePlayName();

    };

    struct savePaths
    {
		std::string metadata = "\\saveData\\cache\\gMeta.quadata";
		std::string sceneData = "\\saveData\\scenes\\";
    };

	extern savePaths SavePath;
    extern savesystem saveSystem;
}