
all: graphics graphicspt data datapt

graphics: graphics.c
	gcc -Wall -std=c99 graphics.c -o graphics -lncurses -lm 

graphicspt: graphics.c
	gcc -Wall -std=c99 graphics.c -o graphicspt -lm -DNOGRAPHICS

data: data.c
	gcc -Wall -std=c99 data.c -lpthread -o data -lncurses -lm

datapt:
	gcc -Wall -std=c99 data.c -lpthread -o datapt -lm -DNOGRAPHICS

clean:
	rm graphics graphicspt data datapt