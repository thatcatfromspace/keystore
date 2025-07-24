#pragma once
#include "kv_store.h"
#include "mode.h"
#include <atomic>
#include <memory>
#include <thread>
#include <vector>

/// @brief Handle running and clean up of the TCP server.
class TcpMode : public Mode {
  public:
	explicit TcpMode(std::shared_ptr<KvStore> kv_store);
	void run() override;
	void cleanup() override;
	void handleSignal(int signal) override;
	ModeStatus getStatus() const override;

  private:
	std::shared_ptr<KvStore> kv_store_;
	std::atomic<ModeStatus> status_{ModeStatus::Starting};
	std::atomic<bool> terminate_{false};
	std::vector<std::thread> worker_threads_;
	void serverLoop();
};
