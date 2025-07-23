#include "CLI/CLI.hpp"
#include "spdlog/spdlog.h"

#include "cli_mode.h"

#include <atomic>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

extern std::atomic<bool> terminate;

CliMode::CliMode(std::shared_ptr<KvStore> kv_store)
    : kv_store_(std::move(kv_store)) {
}

void CliMode::run() {
	status_ = ModeStatus::Running;
	spdlog::info("[CliMode] Running in CLI mode");
	while (!terminate_) {
		std::cout << "keystore> ";
		std::string input;
		if (!std::getline(std::cin, input)) {
			break;
		}
		if (input.empty())
			continue;
		if (input == "QUIT" || input == "EXIT")
			break;
		std::stringstream ss(input);
		std::vector<std::string> cmds;
		std::string temp;
		while (ss >> temp)
			cmds.push_back(temp);
		if (cmds.empty())
			continue;
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
			kv_store_->commands[cmds[0]](cmds);
		} catch (const std::exception& e) {
			std::cout << "Error: " << e.what() << std::endl;
		}
	}
	status_ = ModeStatus::Stopping;
}

void CliMode::cleanup() {
	spdlog::info("[CliMode] Cleaning up CLI mode");
	status_ = ModeStatus::Stopped;
}

void CliMode::handleSignal(int signal) {
	spdlog::info("[CliMode] Signal {} received, shutting down...", signal);
	terminate_ = true;
	fclose(stdin); // Unblock std::getline
}

ModeStatus CliMode::getStatus() const {
	return status_;
}