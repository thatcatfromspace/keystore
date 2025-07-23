#include "tcp_mode.h"
#include <chrono>
#include <spdlog/spdlog.h>

TcpMode::TcpMode(std::shared_ptr<KvStore> kv_store)
    : kv_store_(std::move(kv_store)) {
}

void TcpMode::run() {
	status_ = ModeStatus::Running;
	spdlog::info("[TcpMode] Running in TCP mode");
	// Example: Launch 2 worker threads for demonstration
	for (int i = 0; i < 2; ++i) {
		worker_threads_.emplace_back([this, i]() { this->serverLoop(); });
	}
	// Main thread waits for termination
	while (!terminate_) {
		std::this_thread::sleep_for(std::chrono::milliseconds(200));
	}
	status_ = ModeStatus::Stopping;
}

void TcpMode::cleanup() {
	spdlog::info("[TcpMode] Cleaning up TCP server");
	terminate_ = true;
	for (auto& t : worker_threads_) {
		if (t.joinable())
			t.join();
	}
	status_ = ModeStatus::Stopped;
}

void TcpMode::handleSignal(int signal) {
	spdlog::info("[TcpMode] Signal {} received, shutting down...", signal);
	terminate_ = true;
	// TODO: Close server sockets here to unblock accept/read if implemented
}

ModeStatus TcpMode::getStatus() const {
	return status_;
}

void TcpMode::serverLoop() {
	auto tid = std::this_thread::get_id();
	spdlog::info("[TcpMode] Worker thread {} started", std::hash<std::thread::id>()(tid));
	while (!terminate_) {
		// Placeholder for actual server logic
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
	}
	spdlog::info("[TcpMode] Worker thread {} terminated", std::hash<std::thread::id>()(tid));
}
