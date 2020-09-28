all: data datapt task taskpt

data: data.c
	gcc -Wall data.c -lpthread -o data -lncurses -lm

datapt: data.c
	gcc -Wall data.c -g -lpthread -o datapt -lm -DNOGRAPHICS

task: task.c
	gcc -Wall task.c -lpthread -o task -lncurses -lm

taskpt: task.c
	gcc -Wall task.c -lpthread -o taskpt -lncurses -lm -DNOGRAPHICS

clean:
	rm graphics graphicspt data datapt task taskpt