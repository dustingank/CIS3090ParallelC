all: maze mazep
maze: maze.c
	gcc -Wall maze.c -g -o maze
mazep: maze.c 
	gcc -Wall -fopenmp maze.c -g -o mazep -lm -DPARALLEL
clean:
	rm maze mazep