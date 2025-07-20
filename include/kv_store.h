#pragma once
#include <chrono>
#include <functional>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct Metadata {
	std::string value;
	std::optional<std::chrono::steady_clock::time_point> expiry;
	size_t access_count;
	std::chrono::steady_clock::time_point last_accessed;
	// TODO: add type
};

class KVStore {
	friend class EvictionManager;

  private:
	std::unordered_map<std::string, Metadata> store;
	std::unordered_set<std::string> ttl_keys;
	std::chrono::seconds TTL = std::chrono::seconds(10);

	mutable std::mutex ttl_mutex;
	mutable std::mutex store_mutex;

  public:
	KVStore(size_t ttl);

	std::unordered_map<std::string,
	                   std::function<void(const std::vector<std::string>&)>>
	    commands;

	void set(const std::string& key, const std::string& value,
	         bool is_persistent);
	std::string get(const std::string& key);
	void del(const std::string& key);
	bool exists(const std::string& key);
	void runscript(const std::string& filename);

	std::vector<std::string> utilSplit(std::string& cmd);

	void setTTL(size_t ttl);

    void printAll();
};