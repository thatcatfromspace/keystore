#pragma once
#include "kv_store.h"
#include <atomic>
#include <memory>
#include <random>
#include <thread>

class EvictionManager {
  private:
	std::thread eviction_worker;
	std::atomic<bool> running;
	std::weak_ptr<KVStore> kv_store_ptr;
	const double expiry_threshold = 0.25;
	const int sample_size = 20;

	EvictionManager();

	EvictionManager(const EvictionManager&) = delete;
	EvictionManager& operator=(const EvictionManager&) = delete;

  public:
	static EvictionManager* getInstance(std::shared_ptr<KVStore> kv_store);

	/*  The idea is to create a per-thread Mersenne twister with device based randomness (if available) */
	static size_t getRandomIndex(size_t max);

	/* worker thread functions */

	void start();
	void run();
	void stop();
};