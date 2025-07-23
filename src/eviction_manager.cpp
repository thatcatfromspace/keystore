#include "eviction_manager.h"
#include "spdlog/spdlog.h"
#include <iostream>

EvictionManager::EvictionManager() {
}

EvictionManager* EvictionManager::getInstance(std::shared_ptr<KvStore> kv_store) {
	static EvictionManager instance;
	instance.kv_store_ptr = kv_store;
	return &instance;
}

void EvictionManager::start() {
	if (running)
		return;
	running = true;
	spdlog::debug("[EvictionManager] Thread started");
	eviction_worker = std::thread(&EvictionManager::run, this);
}

void EvictionManager::run() {
	while (running) {
		{
			auto store = kv_store_ptr.lock();

			/* exit the loop is the shared pointer no longer exists */
			if (!store) {
				spdlog::error("[EvictionManager] No KVStore found, exiting thread");
				break;
			}

			std::lock_guard<std::mutex> lock_store(store->store_mutex);
			std::lock_guard<std::mutex> lock_ttl(store->ttl_mutex);

			if (store->ttl_keys.empty()) {
				/* nothing to sample here */
			} else {
				/* sample upto 5 keys for expiry check */
				const int SAMPLE_SIZE = 5;
				std::vector<std::string> keys;

				/* copy keys into a vector to allow random access */
				for (const auto& key : store->ttl_keys) {
					keys.push_back(key);
				}

				int num_samples = std::min(SAMPLE_SIZE, (int)keys.size());
				std::unordered_set<std::string> sampled;

				for (int i = 0; i < num_samples; ++i) {
					size_t idx;
					/*  retry sampling until we get a new one */
					do {
						idx = getRandomIndex(keys.size());
					} while (sampled.count(keys[idx]));

					sampled.insert(keys[idx]);

					const std::string& key = keys[idx];
					auto kv_iter = store->store.find(key);

					if (kv_iter != store->store.end()) {
						auto now = std::chrono::steady_clock::now();
						if (now >= kv_iter->second.expiry) {
							spdlog::debug("[EvictionManager] Evicted {} ", key);
							store->store.erase(key);
							store->ttl_keys.erase(key);
						}
					}
				}
			}
		}

		/* sleep until next cleanup cycle */
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

void EvictionManager::stop() {
	running = false;
	if (eviction_worker.joinable()) {
		eviction_worker.join();
	}
	spdlog::debug("[EvictionManager] Thread terminated");
}

size_t EvictionManager::getRandomIndex(size_t max) {
	static thread_local std::mt19937 rng(std::random_device{}());
	std::uniform_int_distribution<size_t> dist(0, max - 1);
	return dist(rng);
}