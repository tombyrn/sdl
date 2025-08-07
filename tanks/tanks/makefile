main: main.o tanks.o windows.o 
	gcc  main.o tanks.o windows.o -o tank_game -lSDL2

main.o: main.c includes.h
	gcc -c main.c

tanks.o: tanks.c includes.h
	gcc -c tanks.c

windows.o: windows.c includes.h
	gcc -c windows.c

clean:
	rm tanks.o windows.o main.o tank_game