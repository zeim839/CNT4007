all: src/*.cpp main.cpp
	g++ -std=c++11 -Wall -pthread -I./src -o peerProcess $^

clean:
	rm -f *.o
