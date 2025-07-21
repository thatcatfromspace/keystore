#include <atomic>
#include <chrono>
#include <csignal>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>

#include "crow.h"
#include "eviction_manager.h"
#include "kv_store.h"
#include "spdlog/spdlog.h"

std::atomic<bool> terminate(false);

auto kv_store = std::make_shared<KVStore>(600);
auto evictor = EvictionManager::getInstance(kv_store);

void runCliMode(std::shared_ptr<KVStore> kv_store) {
	spdlog::debug("[Keystore] Running in CLI mode");

	while (!terminate.load()) {
		std::cout << "keystore> ";
		std::string input;

		if (!std::getline(std::cin, input)) {
			// EOF or input error (e.g., Ctrl+D)
			break;
		}

		// Skip empty lines
		if (input.empty()) {
			continue;
		}

		// Check for exit commands
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

void runTcpMode(std::shared_ptr<KVStore> kv_store) {
	spdlog::debug("[Keystore] Running in TCP mode on port 3000");

	crow::SimpleApp app;
	app.loglevel(crow::LogLevel::Warning);

	CROW_ROUTE(app, "/")([]() {
		return "Stellis vagamur";
	});

	CROW_ROUTE(app, "/set").methods(crow::HTTPMethod::GET)([kv_store](const crow::request& req) {
		return req.body;
	});

	std::signal(SIGTERM, [](int) {
		spdlog::info("SIGTERM received, stopping server...");
	});

	spdlog::info("[Keystore] Starting TCP server on port 3000");

	app.port(3000).multithreaded().run();

	spdlog::info("[Keystore] TCP mode shutting down gracefully");
}

void printUsage(const char* program_name) {
	std::cout << "Usage: " << program_name << " <mode>" << std::endl;
	std::cout << "Modes:" << std::endl;
	std::cout << "  cli  - Run in CLI mode (interactive command line)" << std::endl;
	std::cout << "  tcp  - Run in TCP server mode" << std::endl;
}

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

	if (mode != "cli" && mode != "tcp") {
		std::cout << "Error: Invalid mode '" << mode << "'" << std::endl;
		printUsage(argv[0]);
		return 1;
	}

	evictor->start();

	if (mode == "cli") {
		runCliMode(kv_store);
	} else if (mode == "tcp") {
		runTcpMode(kv_store);
	}

	evictor->stop();

	return 0;
}
