# Makefile

CXX = g++
CXXFLAGS = -std=c++17 -Wall -I/usr/include/spdlog -Iinclude -pthread

SRC = src/main.cpp src/cli_mode.cpp src/https_mode.cpp src/utils.cpp src/eviction_manager.cpp src/kv_store.cpp src/lru_cache.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = keystore

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) -lfmt

clean:
	rm -f $(TARGET)
