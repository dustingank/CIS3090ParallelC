#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#ifdef PARALLEL
#include <omp.h>
#endif


    

typedef struct location {
    int x;
    int y;
}location;

typedef struct LinkedList {
    location index;
    struct LinkedList *next;
} node;

void showMaze(char *, int);
int updateMaze(char *, int, int, int, char, int);
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
    int seed = -1;

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


    printf("Maze Size: %d\n", mazeSize); // testing / testing / testing

    // allocate memory for maze and stack of index
    mazeArray = (char *)malloc(mazeSize * mazeSize * sizeof(char));

    // initialize the maze
    for (int i = 0; i < mazeSize; i++) { // initial the maze
        for (int j = 0; j < mazeSize; j++) {
            *(mazeArray + i * mazeSize + j) = '.';
        }
    }

    #ifdef PARALLEL
    
    
    #pragma omp parallel num_threads(4) \
    default(none) private(itemAmountTracker, actionCounter, startIndex, randomNumber) shared(mazeArray, mazeSize)
    for (int rounds = 1; rounds > 0; rounds--) {
        int myRank = omp_get_thread_num();
        int threadCount = omp_get_num_threads();
        node *listHead = NULL;

        #pragma omp critical
        {  
            switch(myRank) {
                case 0:
                    startIndex.x = 1;
                    startIndex.y = 1;
                    updateMaze(mazeArray, startIndex.x, startIndex.y, mazeSize, '.', myRank);
                    break;
                case 1:
                    startIndex.x = mazeSize - 2;
                    startIndex.y = 1;
                    updateMaze(mazeArray, startIndex.x, startIndex.y, mazeSize, '.', myRank);
                    break;
                case 2:
                    startIndex.x = 1;
                    startIndex.y = mazeSize - 2;
                    updateMaze(mazeArray, startIndex.x, startIndex.y, mazeSize, '.', myRank);
                    break;
                case 3:
                    startIndex.x = mazeSize - 2;
                    startIndex.y = mazeSize - 2;
                    updateMaze(mazeArray, startIndex.x, startIndex.y, mazeSize, '.', myRank);
                    break;
            }
        }

        //printf("Thread: %d, listHead adress: %p\n",myRank, &listHead);
        add(&listHead, startIndex.x, startIndex.y);

        #pragma omp critical
        printf("thread %d trying to add at (%d, %d)\n", myRank, mazeSize - 2, mazeSize - 2);
        #pragma omp critical
        updateMaze(mazeArray, mazeSize - 2, mazeSize - 2, mazeSize, '.', myRank);

        /*
        while (1) {
            location currentIndex;
            if ((itemAmountTracker + 1) == 1) {
                currentIndex = getNode(&listHead, 0);
            } else {
                srand(time(NULL));
                randomNumber = rand() % itemAmountTracker;
                currentIndex = getNode(&listHead, randomNumber);
            }
            itemAmountTracker--;

            if (currentIndex.x == -2) {
                itemAmountTracker++;
                break;
            }

            #pragma omp critical
            {
                if (updateMaze(mazeArray, currentIndex.x + 2, currentIndex.y, mazeSize, 'U', myRank)) {
                    //printf("adding Index: (%d, %d)\n", currentIndex.x + 2, currentIndex.y);
                    add(&listHead,currentIndex.x + 2, currentIndex.y);
                    actionCounter += 2;
                    itemAmountTracker++;
                }
                if (updateMaze(mazeArray, currentIndex.x - 2, currentIndex.y, mazeSize, 'D', myRank)){
                    //printf("adding Index: (%d, %d)\n", currentIndex.x - 2, currentIndex.y);
                    add(&listHead,currentIndex.x - 2, currentIndex.y);
                    actionCounter += 2;
                    itemAmountTracker++;
                }
                if (updateMaze(mazeArray, currentIndex.x, currentIndex.y + 2, mazeSize, 'R', myRank)) {
                    //printf("adding Index: (%d, %d)\n", currentIndex.x, currentIndex.y + 2);
                    add(&listHead,currentIndex.x, currentIndex.y + 2);
                    actionCounter += 2;
                    itemAmountTracker++;
                }

                if (updateMaze(mazeArray, currentIndex.x, currentIndex.y - 2, mazeSize, 'L', myRank)) {
                    //printf("adding Index: (%d, %d)\n", currentIndex.x, currentIndex.y - 2);
                    add(&listHead,currentIndex.x, currentIndex.y - 2);
                    actionCounter += 2;
                    itemAmountTracker++;
                }
            }
        }
        
        
        while (listHead != NULL) {
            node *temp = listHead;
            listHead = listHead->next;
            free(temp);
        }
        */
    }
    
    showMaze(mazeArray, mazeSize);
    free(mazeArray);
    return 0;
    #endif

    startIndex.x = 1;
    startIndex.y = 1;

    //showMaze(mazeArray, mazeSize);

    // set up the starting point
    updateMaze(mazeArray, startIndex.x, startIndex.y, mazeSize, '.', 0);
    add(&listHead, startIndex.x, startIndex.y);

    while (isContinue) {
        location currentIndex;
        if ( (itemAmountTracker + 1) == 1) {
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

        if (updateMaze(mazeArray, currentIndex.x + 2, currentIndex.y, mazeSize, 'U', 0)) {
            //printf("adding Index: (%d, %d)\n", currentIndex.x + 2, currentIndex.y);
            add(&listHead,currentIndex.x + 2, currentIndex.y);
            actionCounter += 2;
            itemAmountTracker++;
        }
        if ( updateMaze(mazeArray, currentIndex.x - 2, currentIndex.y, mazeSize, 'D', 0)){
            //printf("adding Index: (%d, %d)\n", currentIndex.x - 2, currentIndex.y);
            add(&listHead,currentIndex.x - 2, currentIndex.y);
            actionCounter += 2;
            itemAmountTracker++;
        }
        if (updateMaze(mazeArray, currentIndex.x, currentIndex.y + 2, mazeSize, 'R', 0)) {
            //printf("adding Index: (%d, %d)\n", currentIndex.x, currentIndex.y + 2);
            add(&listHead,currentIndex.x, currentIndex.y + 2);
            actionCounter += 2;
            itemAmountTracker++;
        }

        if (updateMaze(mazeArray, currentIndex.x, currentIndex.y - 2, mazeSize, 'L', 0)) {
            //printf("adding Index: (%d, %d)\n", currentIndex.x, currentIndex.y - 2);
            add(&listHead,currentIndex.x, currentIndex.y - 2);
            actionCounter += 2;
            itemAmountTracker++;
        }



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
    printf("process 0 count: %d ss %d\n", actionCounter + 1, itemAmountTracker);
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

int updateMaze(char *mazePtr, int x, int y, int size, char direction, int threadID) {
    char threadIDChar = threadID + '0';
    
    if (x <= 0 || y <= 0 || x >= size || y >= size) {
        return 0;
    }

    char temp = *(mazePtr + x * size + y);
    printf("    temp: %c at (%d, %d) tring to add %c\n", temp, x, y, threadIDChar);
    if (temp != '.') {

        printf("    Block index (%d, %d) alreay fill with (%c), trying to add (%c)\n", x, y, temp, threadIDChar);
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
