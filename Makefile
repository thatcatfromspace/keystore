# Makefile

CXX = g++
CXXFLAGS = -std=c++17 -Wall -I/usr/include/spdlog -Iinclude -pthread

SRC = src/main.cpp src/kv_store.cpp src/eviction_manager.cpp src/lru_cache.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = redis-lite

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) -lfmt

clean:
	rm -f $(TARGET)
