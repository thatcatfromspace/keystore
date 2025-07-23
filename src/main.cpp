#include <atomic>
#include <csignal>
#include <iostream>
#include <memory>
#include <string>

#include "cli_mode.h"
#include "eviction_manager.h"
#include "http_mode.h"
#include "kv_store.h"
#include "mode.h"
#include "tcp_mode.h"

std::unique_ptr<Mode> mode;

void globalSignalHandler(int signal) {
	if (mode)
		mode->handleSignal(signal);
}

int main(int argc, char* argv[]) {
	if (argc != 2) {
		std::cout << "Usage: " << argv[0] << " <mode>\nModes: cli | tcp | http" << std::endl;
		return 1;
	}
	std::string mode_arg = argv[1];
	auto kv_store = std::make_shared<KvStore>(600);
	EvictionManager* evictor = EvictionManager::getInstance(kv_store);
	if (mode_arg == "cli") {
		mode = std::make_unique<CliMode>(kv_store);
	} else if (mode_arg == "tcp") {
		mode = std::make_unique<TcpMode>(kv_store);
	} else if (mode_arg == "http") {
		mode = std::make_unique<HttpMode>(kv_store);
	} 
	else {
		std::cout << "Invalid mode: " << mode_arg << std::endl;
		return 1;
	}
	std::signal(SIGINT, globalSignalHandler);
	evictor->start();
	mode->run();
	mode->cleanup();
	evictor->stop();
	return 0;
}
