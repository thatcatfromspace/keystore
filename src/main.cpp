#include <iostream>
#include <sstream>
#include <string>

#include "eviction_manager.h"
#include "kv_store.h"

int main() {
	auto kv_store = std::make_shared<KVStore>(600);
	auto evictor = EvictionManager::getInstance(kv_store);
	evictor->start();

	for (int i = 0; i < 5; i++) {
		std::string input;
		std::getline(std::cin, input);
		std::stringstream ss(input);

		std::vector<std::string> cmds;

		std::string temp;

		while (ss >> temp) {
			cmds.push_back(temp);
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

		kv_store->commands[cmds[0]](cmds);
	}

	evictor->stop();

	return 0;
}
