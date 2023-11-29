all: src/*.cpp main.cpp
	g++ -std=c++20 -Wall -pthread -I./src -o peerProcess $^

debug: src/*.cpp main.cpp
	g++ -std=c++20 -g -Wall -pthread -I./src -o peerProcess $^
