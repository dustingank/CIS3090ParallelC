#include <stdlib.h>
#include <stdio.h>
#include <string.h>

struct location {
    int x;
    int y;
};

void showMaze(char *, int);
int updateMaze(char *, int, int, int, char);
void push(struct location *, int , int, int);

int main (int argc, char* argv[]) {

    int mazeSize = -1;
    int argPtr = 1;
    int count = 0;
    int stackSize = 0;

    char *mazeArray;

    struct location startIndex;
    struct location *indexStack;

    if (argc > 1) {
        argPtr = 1;
        while (argPtr < argc) {
            if (strcmp(argv[argPtr], "-n") == 0) {
                sscanf(argv[argPtr + 1], "%d", &mazeSize);
            }
            argPtr += 1;
        }
    } else {
        printf("Invalid Input: Unknow Maze Size\n");
        exit(1);
    }

    if (mazeSize == -1) {
        printf("Invalid Input: Unknow Maze Size\n");
        exit(1);
    }

    printf("Maze Size: %d\n", mazeSize); // testing / testing / testing

    startIndex.x = 1;
    startIndex.y = 1;

    // allocate memory for maze and stack of index
    mazeArray = (char *)malloc(mazeSize * mazeSize * sizeof(char));
    indexStack = (struct location*)malloc(10 * sizeof(struct location));

    // initialize the maze
    for (int i = 0; i < mazeSize; i++) { // initial the maze
        for (int j = 0; j < mazeSize; j++) {
            *(mazeArray + i * mazeSize + j) = '.';
        }
    }

    //showMaze(mazeArray, mazeSize);
    
    // set up the starting point
    updateMaze(mazeArray, startIndex.x, startIndex.y, mazeSize, '.');
    push(indexStack, startIndex.x, startIndex.y, count);
    

    struct location currentIndex = startIndex;
    
    if (updateMaze(mazeArray, currentIndex.x + 2, currentIndex.y, mazeSize, 'U')) { 
        count++;
        push(indexStack, startIndex.x + 2, startIndex.y, count);
    }
    if ( updateMaze(mazeArray, currentIndex.x - 2, currentIndex.y, mazeSize, 'D')){
        count++;
        push(indexStack, startIndex.x - 2, startIndex.y, count);
    }
    if (updateMaze(mazeArray, currentIndex.x, currentIndex.y + 2, mazeSize, 'R')) {
        count++;
        push(indexStack, startIndex.x, startIndex.y + 2, count);
    }
    if (updateMaze(mazeArray, currentIndex.x, currentIndex.y - 2, mazeSize, 'L')) {
        count++;
        push(indexStack, startIndex.x, startIndex.y - 2, count);
    }
    
    int stackSize = (sizeof(indexStack)) / (sizeof(struct location));

    for (int i = 0; i <= stackSize; i++) {
        printf("Index: (%d, %d)\n", (indexStack + i)->x, (indexStack + i)->y);
    }
    showMaze(mazeArray, mazeSize);
    free(mazeArray); // free the maze at end of the program
    free(indexStack); // free the indexStack
}

void push(struct location *stackPtr, int x, int y, int index) {
    struct location temp;
    temp.x = x;
    temp.y = y;
    *(stackPtr + index) = temp;
}


void showMaze(char *mazePtr, int size) {
    for (int i = 0; i < size; i++) { // initial the maze
        for (int j = 0; j < size; j++) {
            printf("%c ", *(mazePtr + i * size + j));
        }
        printf("\n");
    }
}

int updateMaze(char *mazePtr, int x, int y, int size, char direction) {
    if (x <= 0 || y <= 0 || x >= size || y >= size) {
        return 0;
    }

    switch(direction) {
        case 'R':
            *(mazePtr + x * size + (y - 1)) = '0'; 
            *(mazePtr + x * size + y) = '0'; 
            break;

        case 'L':
            *(mazePtr + x * size + (y + 1)) = '0'; 
            *(mazePtr + x * size + y) = '0'; 
            break;
        
        case 'U':
            *(mazePtr + (x - 1) * size + y) = '0'; 
            *(mazePtr + x * size + y) = '0'; 
            break;
        
        case 'D':
            *(mazePtr + (x + 1) * size + y) = '0'; 
            *(mazePtr + x * size + y) = '0'; 
            break;
        
        default:
            *(mazePtr + x * size + y) = '0'; 
            break;
    }
    return 1;
}