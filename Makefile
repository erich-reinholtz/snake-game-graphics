.PHONY: game

game:
	gcc -std=c99 -pedantic -Wall -O3 game.c -lSDL2 -o game
