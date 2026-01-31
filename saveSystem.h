#pragma once
#include <string>
#include <vector>

namespace saved
{
    class savesystem
    {
    public:
        static void save();
        static void load();
    };

    extern savesystem saveSystem;
}