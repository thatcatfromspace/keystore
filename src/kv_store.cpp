#include "kv_store.h"
#include "spdlog/spdlog.h"

#include <fstream>
#include <iostream>
#include <sstream>

void KvStore::set(const std::string& key, const std::string& value, bool is_persistent) {
	Metadata metadata;
	metadata.value = value;
	metadata.expiry = is_persistent ? std::nullopt : std::make_optional(std::chrono::steady_clock::now() + TTL);
	metadata.access_count = 0;
	metadata.last_accessed = std::chrono::steady_clock::now();

	std::lock_guard<std::mutex> lock_store(store_mutex);

	/* evict if full */
	if (store.size() >= lru_cache.MAX_SIZE) {
		/* evict least recently used */
		if (lru_cache.size() > 0) {
			auto lru_it = lru_cache.item_list.rbegin();
			if (lru_it != lru_cache.item_list.rend()) {
				store.erase(lru_it->first);
				lru_cache.erase(lru_it->first);
			}
		}
	}

	store[key] = metadata;
	lru_cache.put(key, metadata);

	if (!is_persistent) {
		std::lock_guard<std::mutex> lock_ttl(ttl_mutex);
		ttl_keys.insert(key);
	}
}

std::string KvStore::get(const std::string& key) {
	std::lock_guard<std::mutex> lock_store(store_mutex);
	auto it = store.find(key);
	auto now = std::chrono::steady_clock::now();
	if (it != store.end()) {
		// check expiry
		if (it->second.expiry && it->second.expiry.value() <= now) {
			store.erase(it);
			this->lru_cache.erase(key);
			return "(nil)";
		}
		it->second.access_count++;
		it->second.last_accessed = now;
		lru_cache.put(key, it->second); /* update LRU order */
		return it->second.value;
	}
	return "(nil)";
}

void KvStore::del(const std::string& key) {
	std::lock_guard<std::mutex> lock_store(store_mutex);
	if (store.find(key) != store.end()) {
		store.erase(key);
		lru_cache.erase(key);
		std::lock_guard<std::mutex> lock_ttl(ttl_mutex);
		ttl_keys.erase(key);
	}
}

bool KvStore::exists(const std::string& key) {
	std::lock_guard<std::mutex> lock_store(store_mutex);
	if (store.find(key) != store.end()) {
		return true;
	}
	return false;
}

void KvStore::runscript(const std::string& filename) {
	std::ifstream fs(filename, std::ios::in);
	if (!fs) {
		spdlog::error("[Keystore] Failed to open script file: {}", filename);
		return;
	}

	std::string line;

	while (std::getline(fs, line)) {
		if (line.empty())
			continue;
		auto args = utilSplit(line);
		if (!args.empty()) {
			commands[args[0]](args);
		}
	}
}

std::vector<std::string> KvStore::utilSplit(std::string& cmd) {
	std::stringstream ss(cmd);
	std::vector<std::string> cmd_split;
	std::string temp;

	while (ss >> temp) {
		cmd_split.push_back(temp);
	}

	if (cmd_split.size() > 2) {
		std::string value;
		for (size_t i = 2; i < cmd_split.size(); ++i) {
			if (i > 2)
				value += " ";
			value += cmd_split[i];
		}
		cmd_split[2] = value;
		cmd_split.resize(3);
	}

	return cmd_split;
}

KvStore::KvStore(size_t ttl = 600) {
	LRUCache lru_cache(32768);

	commands["SET"] = [this](const std::vector<std::string>& args) {
		if (args.size() == 3) {
			set(args[1], args[2], false);
		}
	};

	commands["SETNEXP"] = [this](const std::vector<std::string>& args) {
		if (args.size() == 3) {
			set(args[1], args[2], true);
		}
	};

	commands["GET"] = [this](const std::vector<std::string>& args) {
		std::string res;
		if (args.size() == 2) {
			std::cout << get(args[1]) << std::endl;
		}
	};

	commands["DEL"] = [this](const std::vector<std::string>& args) {
		if (args.size() == 2) {
			del(args[1]);
		}
	};

	commands["EXISTS"] = [this](const std::vector<std::string>& args) {
		if (args.size() == 2) {
			std::cout << (exists(args[1]) ? "true" : "false") << std::endl;
		}
	};
	commands["RUNSCRIPT"] = [this](const std::vector<std::string>& args) {
		if (args.size() == 2) {
			runscript(args[1]);
		}
	};

	commands["PRINT"] = [this](const std::vector<std::string>& args) {
		if (args.size() == 1) {
			printAll();
		}
	};
}

// strictly for debugging purposes only, not advisable to run in production
void KvStore::printAll() {
	std::lock_guard<std::mutex> lock_store(store_mutex);
	for (const auto& [key, metadata] : store) {
		std::cout << "Key: " << key << "\n";
		std::cout << "  Value: " << metadata.value << "\n";
		std::cout << "  Access Count: " << metadata.access_count << "\n";
		std::cout << "  Last Accessed: " << std::chrono::duration_cast<std::chrono::seconds>(metadata.last_accessed.time_since_epoch()).count() << "s since epoch\n";
		if (metadata.expiry) {
			std::cout << "  Expiry: " << std::chrono::duration_cast<std::chrono::seconds>(metadata.expiry.value().time_since_epoch()).count() << "s since epoch\n";
		} else {
			std::cout << "  Expiry: None\n";
		}
		std::cout << std::endl;
	}
}