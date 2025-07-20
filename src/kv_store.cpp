#include "kv_store.h"

#include <fstream>
#include <iostream>
#include <sstream>

void KVStore::set(const std::string& key, const std::string& value, bool is_persistent) {
	Metadata metadata;
	metadata.value = value;
	metadata.expiry =
	    is_persistent
	        ? std::nullopt
	        : std::make_optional(std::chrono::steady_clock::now() + TTL);

	metadata.access_count = 0;
	metadata.last_accessed = std::chrono::steady_clock::now();

	std::lock_guard<std::mutex> lock_store(store_mutex);
	store[key] = metadata;

	if (!is_persistent) {
		std::lock_guard<std::mutex> lock_ttl(ttl_mutex);
		ttl_keys.insert(key);
	}
}

std::string KVStore::get(const std::string& key) {
	std::lock_guard<std::mutex> lock_store(store_mutex);
	auto it = store.find(key);
	auto now = std::chrono::steady_clock::now();
	if (it != store.end()) {
		/* check if expiry is set and has passed */
		if (it->second.expiry && it->second.expiry.value() <= now) {
			store.erase(it);
			return "(nil)";
		}
		it->second.access_count++;
		it->second.last_accessed = now;
		return it->second.value;
	}
	return "(nil)";
}

void KVStore::del(const std::string& key) {
	std::lock_guard<std::mutex> lock_store(store_mutex);
	if (store.find(key) != store.end()) {
		store.erase(key);

		std::lock_guard<std::mutex> lock_ttl(ttl_mutex);
		ttl_keys.erase(key);
	}
}

bool KVStore::exists(const std::string& key) {
	std::lock_guard<std::mutex> lock_store(store_mutex);
	if (store.find(key) != store.end()) {
		return true;
	}
	return false;
}

void KVStore::runscript(const std::string& filename) {
	std::ifstream fs(filename, std::ios::in);
	if (!fs) {
		std::cerr << "Failed to open script file: " << filename << std::endl;
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

std::vector<std::string> KVStore::utilSplit(std::string& cmd) {
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

KVStore::KVStore(size_t ttl = 600) {
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
			exists(args[1]);
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

void KVStore::setTTL(size_t ttl) {
	TTL = std::chrono::seconds(ttl);
}

void KVStore::printAll() {
	std::lock_guard<std::mutex> lock_store(store_mutex);
	for (const auto& [key, metadata] : store) {
		std::cout << "Key: " << key << "\n";
		std::cout << "  Value: " << metadata.value << "\n";
		std::cout << "  Access Count: " << metadata.access_count << "\n";
		std::cout << "  Last Accessed: " << std::chrono::duration_cast<std::chrono::seconds>(
			metadata.last_accessed.time_since_epoch()).count() << "s since epoch\n";
		if (metadata.expiry) {
			std::cout << "  Expiry: " << std::chrono::duration_cast<std::chrono::seconds>(
				metadata.expiry.value().time_since_epoch()).count() << "s since epoch\n";
		} else {
			std::cout << "  Expiry: None\n";
		}
		std::cout << std::endl;
	}
}