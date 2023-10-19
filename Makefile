all: src/*.cpp main.cpp
	g++ -std=c++11 -Wall -I./src -o build/proj1.out $^

clean:
	rm -f build.*.o
	rm build/proj1.out
