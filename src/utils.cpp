#include "utils.h"
#include <iostream>

void printUsage(const char* program_name) {
	std::cout << "Usage: " << program_name << " <mode>" << std::endl;
	std::cout << "Modes:" << std::endl;
	std::cout << "  cli  - Run in CLI mode (interactive command line)" << std::endl;
	std::cout << "  tcp  - Run in TCP server mode" << std::endl;
}
