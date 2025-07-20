# Makefile

CXX = g++
CXXFLAGS = -std=c++17 -Wall -Iinclude -pthread

SRC = src/main.cpp src/kv_store.cpp src/eviction_manager.cpp
OBJ = $(SRC:.cpp=.o)
TARGET = redis-lite

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
