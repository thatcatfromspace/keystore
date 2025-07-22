#include "cli_mode.h"
#include "spdlog/spdlog.h"
#include <atomic>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

extern std::atomic<bool> terminate;

void runCliMode(std::shared_ptr<KVStore> kv_store) {
	spdlog::debug("[Keystore] Running in CLI mode");

	while (!terminate.load()) {
		std::cout << "keystore> ";
		std::string input;

		if (!std::getline(std::cin, input)) {
			break;
		}

		if (input.empty()) {
			continue;
		}

		if (input == "QUIT" || input == "EXIT") {
			break;
		}

		std::stringstream ss(input);
		std::vector<std::string> cmds;
		std::string temp;

		while (ss >> temp) {
			cmds.push_back(temp);
		}

		if (cmds.empty()) {
			continue;
		}

		if (cmds.size() > 2) {
			std::string value;
			for (size_t i = 2; i < cmds.size(); ++i) {
				if (i > 2)
					value += " ";
				value += cmds[i];
			}
			cmds[2] = value;
			cmds.resize(3);
		}

		try {
			kv_store->commands[cmds[0]](cmds);
		} catch (const std::exception& e) {
			std::cout << "Error: " << e.what() << std::endl;
		}
	}

	spdlog::info("[Keystore] CLI mode shutting down gracefully");
}
