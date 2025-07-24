#pragma once
#include <cassert>
#include <list>
#include <string>
#include <unordered_map>

// Forward declaration for Metadata
struct Metadata;

/// @brief Minimal LRU cache. Maintains a linked list internally to keep track of LRU items
/// and uses a map for `O(1)` time search and deletion.
class LRUCache {
  public:
	explicit LRUCache(size_t max_size);
    explicit LRUCache();
    
    size_t MAX_SIZE;
    std::list<std::pair<std::string, Metadata>> item_list;
    std::unordered_map<std::string, std::list<std::pair<std::string, Metadata>>::iterator> item_map;
	
    void clean();

	void put(const std::string& key, const Metadata& value);
	bool exist(const std::string& key) const;
	Metadata* get(const std::string& key);
	void erase(const std::string& key);
	size_t size() const;
	void clear();
};
