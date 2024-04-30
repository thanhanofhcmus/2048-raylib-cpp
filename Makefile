build: main.cpp
	c++ -g -fsanitize=address -xc++ -c main.cpp -o out/main.o -std=c++20 -Wall -Wextra -Werror -pedantic $(shell pkg-config --cflags raylib)
	c++ -g -fsanitize=address out/main.o -o out/main $(shell pkg-config --libs raylib)


run: build
	./out/main
