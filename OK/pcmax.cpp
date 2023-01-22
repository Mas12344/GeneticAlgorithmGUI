#include "Application.hpp"

#include <string>
#include <sstream>

#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
int main() {
#if 0
	int processors = 100;
	int taskCount = 10000;
	for (int i = 0; i < 15; i++) {
		std::stringstream ss;
		ss << "generated\\" << "m100n1000\\" << i << ".txt";
		Instance::GenRandomInstance(processors, taskCount).SaveToFile(ss.str());
	}

	processors = 20;
	taskCount = 100;
	for (int i = 0; i < 15; i++) {
		std::stringstream ss;
		ss << "generated\\" << "progresja\\m20n" << taskCount << ".txt";
		Instance::GenRandomInstance(processors, taskCount).SaveToFile(ss.str());
		taskCount += 50;
	}
#endif
	Application app = Application();
	return 0;
}


