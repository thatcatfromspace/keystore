#pragma once
#include "kv_store.h"
#include <memory>

void runHttpsMode(std::shared_ptr<KVStore> kv_store);
