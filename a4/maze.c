/*
Name: Yizhou Wang
ID: 1013411
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef PARALLEL
#include <unistd.h>
#include <omp.h>
#endif

typedef struct location {
    int x;
    int y;
} location;

typedef struct Stack {
    int top;
    int capacity;
    location* array;
} newStack;

// initialize the stack
newStack* createStack(int);
void showMaze(char *, int);
int updateMaze(char *, int, int, int, char, int);
int isEmpty(newStack *stack);
void push(newStack *, int , int );
location pop(newStack *);
void shuffle(int *, int );

// start of the prgram
int main(int argc, char* argv[]) {
    int mazeSize = -1;
    int argPtr = 1;
    int actionCounter = 0;
    int randomNumber = 0;
    int seed = -1;
    int isComplete = 0;
    #ifdef PARALLEL
    int actionList[4] = {0, 0, 0, 0};
    #endif
    int up = 0;
    int right = 0;
    int down = 0;
    int left = 0;

    char *mazeArray;

    location startIndex;
    newStack *stack;

    // arguments checking
    if (argc > 1) {
        argPtr = 1;
        while (argPtr < argc) {
            if (strcmp(argv[argPtr], "-n") == 0) {
                sscanf(argv[argPtr + 1], "%d", &mazeSize);
            }

            if (strcmp(argv[argPtr], "-s") == 0) {
                sscanf(argv[argPtr + 1], "%d", &seed);
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


    // allocate memory for maze and stack of index
    mazeArray = (char *)malloc(mazeSize * mazeSize * sizeof(char));

    // initialize the maze
    for (int i = 0; i < mazeSize; i++) { // initial the maze
        for (int j = 0; j < mazeSize; j++) {
            *(mazeArray + i * mazeSize + j) = '.';
        }
    }

    if (seed == -1) {
        srand((unsigned)time(NULL));
    } else {
        srand(seed);
    }
    

    #ifdef PARALLEL
    
    updateMaze(mazeArray, 1, 1, mazeSize, '.', 0);
    updateMaze(mazeArray, mazeSize - 2, 1, mazeSize, '.', 1);
    updateMaze(mazeArray, 1, mazeSize - 2, mazeSize, '.', 3);
    updateMaze(mazeArray, mazeSize - 2, mazeSize - 2, mazeSize, '.', 2);

    
    #pragma omp parallel num_threads(4)\
    default(none) private(actionCounter, startIndex, randomNumber, up, right, left, down, isComplete) \
    shared(mazeArray, mazeSize, actionList)
    for (int rounds = 1; rounds > 0; rounds--) {
        int myRank = omp_get_thread_num();
        //int threadCount = omp_get_num_threads();
        
        newStack *stack = createStack(mazeSize);

        switch(myRank) {
            case 0:
                startIndex.x = 1;
                startIndex.y = 1;
                break;
            case 1:
                startIndex.x = mazeSize - 2;
                startIndex.y = 1;
                break;
            case 2:
                startIndex.x = mazeSize - 2;
                startIndex.y = mazeSize - 2;
                break;
            case 3:
                startIndex.x = 1;
                startIndex.y = mazeSize - 2;
              
                break;
        }

        push(stack, startIndex.x, startIndex.y);

        while (1) {
            location currentIndex;
            if (isEmpty(stack)) {
                break;
            }
            currentIndex = pop(stack);

            while (isComplete == 0) {
                randomNumber = rand() % 4;

                if (randomNumber == 0) {
                    #pragma omp critical
                    {
                        if (updateMaze(mazeArray, currentIndex.x + 2, currentIndex.y, mazeSize, 'U', myRank)) {
                            push(stack, currentIndex.x + 2, currentIndex.y);
                            actionCounter += 2;
                            up = 1;
                        } else {
                            up = 1;
                        }
                    }
                } else if (randomNumber == 1) {
                    #pragma omp critical
                    {
                        if (updateMaze(mazeArray, currentIndex.x - 2, currentIndex.y, mazeSize, 'D', myRank)) {
                            push(stack, currentIndex.x - 2, currentIndex.y);
                            actionCounter += 2;
                            down = 1;
                        } else {
                            down = 1;
                        }
                    }

                } else if (randomNumber == 2) {
                    #pragma omp critical
                    {
                        if (updateMaze(mazeArray, currentIndex.x, currentIndex.y + 2, mazeSize, 'R', myRank)) {
                            push(stack, currentIndex.x, currentIndex.y + 2);
                            actionCounter += 2;
                            right = 1;
                        } else {
                            right = 1;
                        }
                    }
                } else if (randomNumber == 3) {
                    #pragma omp critical
                    {
                        if (updateMaze(mazeArray, currentIndex.x, currentIndex.y - 2, mazeSize, 'L', 0)) {
                            push(stack, currentIndex.x, currentIndex.y - 2);
                            actionCounter += 2;
                            left = 1;
                        } else {
                            left = 1;
                        }
                    }
                }
                if (up == 1 && right == 1 && left == 1 && down == 1) {
                    isComplete = 1;
                }
                //sleep(0.1);
            }
            isComplete = 0;
            up = 0;
            left = 0;
            right = 0;
            down = 0;

        }

        switch (myRank) {
            case 0:
                #pragma omp critical
                actionList[0] = actionCounter;
                break;
            
            case 1:
                #pragma omp critical
                actionList[1] = actionCounter;
                break;
            
            case 2:
                #pragma omp critical
                actionList[2] = actionCounter;
                break;
            
            case 3:
                #pragma omp critical
                actionList[3] = actionCounter;
                break;
        }

        free(stack->array);
        free(stack);
        

    }
    showMaze(mazeArray, mazeSize);
    for (int i = 0; i < 4; i++) {
        printf("process %d count: %d\n", i, actionList[i]);
    }
    free(mazeArray);
    return 0;
    #endif

    stack = createStack(mazeSize);
    startIndex.x = 1;
    startIndex.y = 1;

    // set up the starting point
    updateMaze(mazeArray, startIndex.x, startIndex.y, mazeSize, '.', 0);
    push(stack, startIndex.x, startIndex.y);
    
    while(1) {
        location currentIndex;
        currentIndex = pop(stack);
        // leaving condition
        if (currentIndex.x == -2) {
            break;
        }

        while (isComplete == 0) {
            
            randomNumber = rand() % 4;
            //printf("%d\n", randomNumber);
            if (randomNumber == 0) {
                if (updateMaze(mazeArray, currentIndex.x + 2, currentIndex.y, mazeSize, 'U', 0)) {
                    //printf("adding Index: (%d, %d)\n", currentIndex.x + 2, currentIndex.y);
                    push(stack, currentIndex.x + 2, currentIndex.y);
                    actionCounter += 2;
                    up = 1;
                } else {
                    up = 1;
                }
            }else if (randomNumber == 1) {
                if ( updateMaze(mazeArray, currentIndex.x - 2, currentIndex.y, mazeSize, 'D', 0)){
                    //printf("adding Index: (%d, %d)\n", currentIndex.x - 2, currentIndex.y);
                    push(stack, currentIndex.x - 2, currentIndex.y);
                    actionCounter += 2;
                    down = 1;
                } else {
                    down = 1;
                }
            }else if (randomNumber == 2) {
                if (updateMaze(mazeArray, currentIndex.x, currentIndex.y + 2, mazeSize, 'R', 0)) {
                    //printf("adding Index: (%d, %d)\n", currentIndex.x, currentIndex.y + 2);
                    push(stack, currentIndex.x, currentIndex.y + 2);
                    actionCounter += 2;
                     right = 1;
                } else {
                    right = 1;
                }
            }else if (randomNumber == 3) {
                if (updateMaze(mazeArray, currentIndex.x, currentIndex.y - 2, mazeSize, 'L', 0)) {
                    //printf("adding Index: (%d, %d)\n", currentIndex.x, currentIndex.y - 2);
                    push(stack, currentIndex.x, currentIndex.y - 2);
                    actionCounter += 2;
                    left = 1;
                } else {
                    left = 1;
                }
            }

            if (up == 1 && right == 1 && left == 1 && down == 1) {
                isComplete = 1;
            }
        }
        isComplete = 0;
        up = 0;
        left = 0;
        right = 0;
        down = 0;
        /*
        shuffle(direction, 4);

        for (int i = 0; i < 4; i++) {
            randomNumber = direction[i];
            //printf("rand: %d\n", randomNumber);

            switch (randomNumber) {
                case 0:
                    if (updateMaze(mazeArray, currentIndex.x + 2, currentIndex.y, mazeSize, 'U', 0)) {
                        //printf("adding Index: (%d, %d)\n", currentIndex.x + 2, currentIndex.y);
                        push(stack, currentIndex.x + 2, currentIndex.y);
                        actionCounter += 2;
                    }
                    break;
                    
                case 1:
                    if ( updateMaze(mazeArray, currentIndex.x - 2, currentIndex.y, mazeSize, 'D', 0)){
                        //printf("adding Index: (%d, %d)\n", currentIndex.x - 2, currentIndex.y);
                        push(stack, currentIndex.x - 2, currentIndex.y);
                        actionCounter += 2;
                    }
                    break;
                
                case 2:
                    if (updateMaze(mazeArray, currentIndex.x, currentIndex.y + 2, mazeSize, 'R', 0)) {
                        //printf("adding Index: (%d, %d)\n", currentIndex.x, currentIndex.y + 2);
                        push(stack, currentIndex.x, currentIndex.y + 2);
                        actionCounter += 2;
                    }
                    break;
                
                case 3:
                    if (updateMaze(mazeArray, currentIndex.x, currentIndex.y - 2, mazeSize, 'L', 0)) {
                        //printf("adding Index: (%d, %d)\n", currentIndex.x, currentIndex.y - 2);
                        push(stack, currentIndex.x, currentIndex.y - 2);
                        actionCounter += 2;
                    }
                    break;
            
            }
        }
        */
        //printf("outside of break\n");
    }

    showMaze(mazeArray, mazeSize);
    printf("process 0 count: %d\n", actionCounter);
    free(mazeArray);
    free(stack->array);
    free(stack);
}

newStack* createStack(int size) {
    newStack* stack = (newStack*) malloc(sizeof(newStack));
    stack->capacity = size * size;
    stack->top = -1;
    stack->array = (location*) malloc (stack->capacity * sizeof(location));
    return stack;
}

void showMaze(char *mazePtr, int size) {
    for (int i = 0; i < size; i++) { // initial the maze
        for (int j = 0; j < size; j++) {
            printf("%c ", *(mazePtr + i * size + j));
        }
        printf("\n");
    }
}

int updateMaze(char *mazePtr, int x, int y, int size, char direction, int threadID) {
    char threadIDChar = threadID + '0';
    
    if (x <= 0 || y <= 0 || x >= size || y >= size) {
        return 0;
    }

    char temp = *(mazePtr + x * size + y);
    //printf("    temp: %c at (%d, %d) tring to add %c\n", temp, x, y, threadIDChar);
    if (temp != '.') {

        //printf("    Block index (%d, %d) alreay fill with (%c), trying to add (%c)\n", x, y, temp, threadIDChar);
        return 0;
    }

    switch(direction) {
        case 'R':
            *(mazePtr + x * size + (y - 1)) = threadIDChar;
            *(mazePtr + x * size + y) = threadIDChar;
            break;

        case 'L':
            *(mazePtr + x * size + (y + 1)) = threadIDChar;
            *(mazePtr + x * size + y) = threadIDChar;
            break;

        case 'U':
            *(mazePtr + (x - 1) * size + y) = threadIDChar;
            *(mazePtr + x * size + y) = threadIDChar;
            break;

        case 'D':
            *(mazePtr + (x + 1) * size + y) = threadIDChar;
            *(mazePtr + x * size + y) = threadIDChar;
            break;

        default:
            *(mazePtr + x * size + y) = threadIDChar;
            break;
    }
    return 1;
}

int isEmpty(newStack *stack) {
    return stack->top == -1;
}

void push(newStack *stack, int x, int y) {
    // adding a checking full later
    location new;
    new.x = x;
    new.y = y;
    stack->array[++stack->top] = new;
   
}

location pop(newStack *stack) {
    if(isEmpty(stack)) {
        location dummy;
        dummy.x = -2;
        dummy.y = -2;
        return dummy;
    }

    return stack->array[stack->top--];
}

void shuffle(int *array, int n) {
    srand((unsigned)time(NULL));
    for (int i = 0; i < n; i++) {
        size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
        int t = array[j];
        array[j] = array[i];
        array[i] = t;
    } 
}