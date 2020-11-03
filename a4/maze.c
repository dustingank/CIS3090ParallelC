#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

typedef struct location {
    int x;
    int y;
}location;

typedef struct LinkedList {
    location index;
    struct LinkedList *next;
} node;

void showMaze(char *, int);
int updateMaze(char *, int, int, int, char);
void add(node **, int , int);
location getNode(node **, int);

// start of the prgram
int main (int argc, char* argv[]) {

    int mazeSize = -1;
    int argPtr = 1;
    int isContinue = 1;
    int actionCounter = 0;
    int itemAmountTracker = 1;
    int randomNumber = 0;

    char *mazeArray;

    location startIndex;
    node *listHead = NULL;
    node *listPtr = NULL;

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

    // initialize the maze
    for (int i = 0; i < mazeSize; i++) { // initial the maze
        for (int j = 0; j < mazeSize; j++) {
            *(mazeArray + i * mazeSize + j) = '.';
        }
    }

    //showMaze(mazeArray, mazeSize);
    
    // set up the starting point
    updateMaze(mazeArray, startIndex.x, startIndex.y, mazeSize, '.');
    add(&listHead, startIndex.x, startIndex.y);

    while (isContinue) {
        location currentIndex;
        if (itemAmountTracker <= 1) {
            currentIndex = getNode(&listHead, 0);
        } else {
            srand (time(NULL));
            randomNumber  = rand() % itemAmountTracker;
            //printf("rando: %d\n", randomNumber);
            currentIndex = getNode(&listHead, randomNumber);

        }
        itemAmountTracker--;
        //printf("current Index: (%d, %d)\n", currentIndex.x, currentIndex.y);
        //showMaze(mazeArray, mazeSize);

        if (currentIndex.x == -2) {
            //printf("Leave\n");
            isContinue = 0;
            itemAmountTracker++;
            break;
        }

        if (updateMaze(mazeArray, currentIndex.x + 2, currentIndex.y, mazeSize, 'U')) { 
            //printf("adding Index: (%d, %d)\n", currentIndex.x + 2, currentIndex.y);
            add(&listHead,currentIndex.x + 2, currentIndex.y);
            actionCounter++;
            itemAmountTracker++;
        }
        if ( updateMaze(mazeArray, currentIndex.x - 2, currentIndex.y, mazeSize, 'D')){
            //printf("adding Index: (%d, %d)\n", currentIndex.x - 2, currentIndex.y);
            add(&listHead,currentIndex.x - 2, currentIndex.y);
            actionCounter++;
            itemAmountTracker++;
        }
        if (updateMaze(mazeArray, currentIndex.x, currentIndex.y + 2, mazeSize, 'R')) {
            //printf("adding Index: (%d, %d)\n", currentIndex.x, currentIndex.y + 2);
            add(&listHead,currentIndex.x, currentIndex.y + 2);
            actionCounter++;
            itemAmountTracker++;
        }
            
        if (updateMaze(mazeArray, currentIndex.x, currentIndex.y - 2, mazeSize, 'L')) {
            //printf("adding Index: (%d, %d)\n", currentIndex.x, currentIndex.y - 2);
            add(&listHead,currentIndex.x, currentIndex.y - 2);
            actionCounter++;
            itemAmountTracker++;
        }
        //printf("done\n");


    } 
    
    // testing and free, end of program
    showMaze(mazeArray, mazeSize);

    listPtr = listHead;
    /*
    while (listPtr != NULL) {
        printf("Index: (%d, %d)\n", listPtr->index.x, listPtr->index.y);
        listPtr = listPtr->next;
    }
    */
    while (listHead != NULL) {
        node *temp = listHead;
        listHead = listHead->next;
        free(temp);
    }

    free(mazeArray); // free the maze at end of the program
    printf("process 0 count: %d ss %d\n", actionCounter, itemAmountTracker);
}

/** Below are all helper funcitons **/

location getNode(node **headPtr, int index) {
    int count = 0;
    location toBeReturn;
    node *head = *headPtr;
    node *pre = *headPtr;

    if (*headPtr == NULL) {
        toBeReturn.x = -2;
        toBeReturn.y = -2;
        return toBeReturn;
    }
    
    if (head->next == NULL ) {
        toBeReturn = head->index;
        *headPtr = NULL;
        free(head);
        return toBeReturn;
    }

    if (count == index) {
        toBeReturn = head->index;
        (*headPtr) = (*headPtr)->next;
        free(head);

        return toBeReturn;
    }
    
    while (count != index) {
        if (count == index ) {
            break;
        }
        count++;
        pre = head;
        head = head->next;
    }

    toBeReturn = head->index;
    pre->next = head->next;
    free(head);
    return toBeReturn;
}

void add(node **headPtr, int x, int y) {
    node *newNode = (node*)malloc(sizeof(node));
    node *last = *headPtr;
    
    newNode->index.x = x;
    newNode->index.y = y;
    newNode->next = NULL;

    if(*headPtr == NULL) {
        *headPtr = newNode;
        //printf("new node have been add\n");
        return;
    }

    while (last->next != NULL) {
        last = last->next;
    }

    last->next = newNode;
    //printf("new node have been add\n");
    return;
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

    char temp = *(mazePtr + x * size + y);
    if (temp != '.') {
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