#include <atomic>
#include <csignal>
#include <iostream>
#include <memory>
#include <string>

#include "cli_mode.h"
#include "eviction_manager.h"
#include "kv_store.h"
#include "spdlog/spdlog.h"
#include "https_mode.h"
#include "utils.h"

std::atomic<bool> terminate(false);

auto kv_store = std::make_shared<KVStore>(600);
auto evictor = EvictionManager::getInstance(kv_store);

void sigintSignalHandler(int signal) {
	if (signal == SIGINT) {
		spdlog::info("Ctrl+C received, initiating graceful shutdown...");
		evictor->stop();
		terminate = true;
		exit(128 + signal); /* https://unix.stackexchange.com/questions/99112/default-exit-code-when-process-is-terminated */
	}
}

int main(int argc, char* argv[]) {
	spdlog::set_level(spdlog::level::debug);

	signal(SIGINT, sigintSignalHandler);

	if (argc != 2) {
		printUsage(argv[0]);
		return 1;
	}

	std::string mode = argv[1];

	if (mode != "cli" && mode != "https" && mode != "tcp") {
		std::cout << "Error: Invalid mode '" << mode << "'" << std::endl;
		printUsage(argv[0]);
		return 1;
	}

	evictor->start();

	if (mode == "cli") {
		runCliMode(kv_store);
	} else if (mode == "https") {
		runHttpsMode(kv_store);
	}

	evictor->stop();

	return 0;
}
