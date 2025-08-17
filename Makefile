CFLAGS = -Wall -Wextra
CC = clang
SRC = main

build:$(SRC).c
	$(CC) $(CFLAGS) $(SRC).c -o bin/$(SRC)
run:build
	./bin/$(SRC)

clean:
	rm bin/*

