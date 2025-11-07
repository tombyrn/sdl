all: build run

build:
	gcc -O3 -Wall -g -std=c99 ./src/main.c -lSDL2 -o draw -lSDL2_image

run:
	./draw

clean:
	rm -rf draw draw.dSYM
