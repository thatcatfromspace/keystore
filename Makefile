# Makefile

CXX = g++
CXXFLAGS = -std=c++17 -Wall -I/usr/include/spdlog -Iinclude -pthread -lfmt

SRC = $(wildcard src/*.cpp)
OBJ = $(SRC:.cpp=.o)
TARGET = keystore

all: $(TARGET)

client: client/tcp_client.cpp
	$(CXX) $(CXXFLAGS) client/tcp_client.cpp -o client 

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)

clean:
	rm -f $(TARGET)
