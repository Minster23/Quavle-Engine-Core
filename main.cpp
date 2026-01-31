#include "window.h"

using namespace window;
windowsHandler windH;

int main(int argc, char* argv[])
{
#if WITH_EDITOR
	bool run = false;
	std::cout << "EDITOR" << std::endl;
	for (int i = 0; i < argc; i++) {
		printf("argv[%d]: %s\n", i, argv[i]);
	}
	if (argv[1] == std::string("-Location"))
	{
		windH.project_location = argv[2];
		run = true;
	}
	if(run)windH.windowInit();
#else
	std::cout << "RUNTIME" << std::endl;
	windH.windowInit();
#endif 

	return 0;
}
