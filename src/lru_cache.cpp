#include "lru_cache.h"
#include "kv_store.h"
#include "spdlog/spdlog.h"

LRUCache::LRUCache(size_t max_size)
    : MAX_SIZE(max_size) {
}

LRUCache::LRUCache(){
    MAX_SIZE = 32768;
	spdlog::debug("[LRUCache] Cache initalized with max size {}", MAX_SIZE);
}

void LRUCache::put(const std::string& key, const Metadata& value) {
	auto it = item_map.find(key);
	if (it != item_map.end()) {
		item_list.erase(it->second);
		item_map.erase(it);
	}
	item_list.push_front(std::make_pair(key, value));
	item_map[key] = item_list.begin();
	clean();
}

bool LRUCache::exist(const std::string& key) const {
	return item_map.find(key) != item_map.end();
}

Metadata* LRUCache::get(const std::string& key) {
	auto it = item_map.find(key);
	if (it == item_map.end())
		return nullptr;
	item_list.splice(item_list.begin(), item_list, it->second);
	return &it->second->second;
}

void LRUCache::erase(const std::string& key) {
	auto it = item_map.find(key);
	if (it != item_map.end()) {
		item_list.erase(it->second);
		item_map.erase(it);
	}
}

size_t LRUCache::size() const {
	return item_map.size();
}

void LRUCache::clear() {
	item_list.clear();
	item_map.clear();
}

void LRUCache::clean() {
	while (item_map.size() > MAX_SIZE) {
		auto last_it = item_list.end();
		--last_it;
		item_map.erase(last_it->first);
		item_list.pop_back();
	}
}