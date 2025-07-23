#pragma once
#include "kv_store.h"
#include "mode.h"
#include <atomic>
#include <crow.h>
#include <memory>
#include <thread>
#include <vector>

class HttpsMode : public Mode {
  public:
	explicit HttpsMode(std::shared_ptr<KvStore> kv_store);
	void run() override;
	void cleanup() override;
	void handleSignal(int signal) override;
	ModeStatus getStatus() const override;

  private:
	std::shared_ptr<KvStore> kv_store_;
	std::atomic<ModeStatus> status_{ModeStatus::Starting};
	std::atomic<bool> terminate_{false};
	std::vector<std::thread> worker_threads_;
	std::unique_ptr<crow::SimpleApp> app_;
	void serverLoop();
};
