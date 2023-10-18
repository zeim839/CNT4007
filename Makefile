SRC=$(wildcard *.cpp)
OBJ=$(SRC:src/%.cpp=build/%.o)

all: $(OBJ)
	g++ -std=c++11 -Wall -I src/ -o build/proj1.out $^

build/%.o: src/%.cpp
	g++ $@ -c $<

clean:
	rm -f build.*.o
	rm build/proj1.out
