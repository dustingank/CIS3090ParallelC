/*
 * Name: Yizhou Wang
 * ID: 1013411
*/


#include <stdio.h>
#include <string.h> 
#include <ctype.h>
#include <stdlib.h>
#include <mpi.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>


const int INT_MAX = 1000000;


int main(int argc, char** argv) {
    
    int isShellContinue = 0;
    int processID = -2;
    int rankID = 1;
    int comSize;
    int myRank;
    int spaces = 0;
    int indexFileTracker = 0;
    int a = 0;
    int isEmpty  = -1;

    char shellCommand[40];
    char command[10];
    char fileName[40];
    char fileContents[INT_MAX];
    char commandRecived[10];
    char fileNameRecived[40];
    char fileContentsRecivied[INT_MAX];
    char folderName[80];
    char tempFolderName[10];
    char fileListTracker[100][10000];
    char temp[10000];
    char tempProcessID[5];
    char rank[5];
    char tempfileName[40];
    char *token;

    FILE *newFilePtr;
    FILE *newFileListPtr;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comSize);
    MPI_Comm_rank(MPI_COMM_WORLD, &myRank);

    folderName[0] = 'p';
    folderName[1] = '\0';

    for (int i = 1; i < comSize; i++) {
        sprintf(tempFolderName, "%d", i);
        strcat(folderName, tempFolderName);
        mkdir(folderName, S_IRUSR | S_IRGRP | S_IROTH | S_IXUSR | S_IXGRP | S_IXOTH | S_IWOTH | S_IWUSR | S_IWGRP);
        folderName[0] = 'p';
        folderName[1] = '\0';
    }

    if (myRank == 0) {
        newFilePtr = fopen("fileList.txt", "r");

        while (fgets(temp, 10000, newFilePtr) != NULL) {
            strtok(temp, "\n");
            strcpy(fileListTracker[indexFileTracker], temp);
            indexFileTracker++;
        }
        fclose(newFilePtr);
    }

    while(isShellContinue == 0) {
        if (myRank == 0) {
            fgets(shellCommand, 40, stdin);

            for (int i = 0; i < strlen(shellCommand); i++) { // calculate the sapce in the command line
                if (shellCommand[i] == ' ') {
                    spaces += 1;
                }
            }
            
            switch(spaces){
                case 0:
                    if (strcmp(shellCommand, "lls\n") == 0) {
                        char *args[2];
                        int status;

                        args[0] = "/bin/ls";
                        args[1] = NULL;

                        if (fork() == 0) {
                            execv(args[0], args);
                        } else {
                            wait(&status);
                        }
                    }

                    if (strcmp(shellCommand, "ls\n") == 0) {
                        for (int i = 1; i < comSize; i++) {
                            printf(" p%d\n", i);
                            isEmpty = 1;

                            for (int j = 0; j < indexFileTracker; j++) {
                                fileName[0] = '\0';
                                tempProcessID[0] = '\0';
                                sscanf(fileListTracker[j], "%s %s", fileName, tempProcessID);
                                sprintf(rank, "%d", i);
                                if (strcmp(tempProcessID, rank) == 0) {
                                    printf("  %s\n", fileName);
                                    isEmpty = 0;
                                }
                            }
                            if (isEmpty == 1) {
                                printf(" -none-\n");
                            }
                        }
                    }

                    if (strcmp(shellCommand, "exit\n") == 0) { // exit the program
                        newFileListPtr = fopen("fileList.txt", "w");
                        for (int i = 0; i < indexFileTracker; i++) {
                            if (i == indexFileTracker - 1) {
                                fprintf(newFileListPtr, "%s", fileListTracker[i]);
                            } else {
                                fprintf(newFileListPtr, "%s\n", fileListTracker[i]);
                            }

                        }
                        fclose(newFileListPtr);

                        for (int i = 0; i < comSize; i++) {
                            MPI_Send(shellCommand, 10, MPI_CHAR, i, 0, MPI_COMM_WORLD);

                        }
                        isShellContinue = 1;
                    }
                    break;
                case 1:
                    sscanf(shellCommand, "%s %s", command, fileName);
                    if (strcmp(command, "lrm") == 0) { // remove a file
                        char *args[3];
                        int status;

                        args[0] = "/bin/rm";
                        args[1] = fileName;
                        args[2] = NULL;

                        if (fork() == 0) {
                            execv(args[0], args);
                        } else {
                            wait(&status);
                        }
                    }

                    if (strcmp(command, "put") == 0 ) { // if the command is put
                       
                        int fileContentsIndex = 0;
                        newFilePtr = fopen(fileName, "r");
            
                        do {
                            char eachCharacter = fgetc(newFilePtr);

                            if (feof(newFilePtr)) break;
                            fileContents[fileContentsIndex] = eachCharacter;
                            fileContentsIndex++;

                        } while(1); // get the info from the selected file

                        fclose(newFilePtr);
                        
                        int local = 0;

                        if (indexFileTracker == 0) {
                            MPI_Send(command, 10, MPI_CHAR, rankID, 0, MPI_COMM_WORLD);
                            MPI_Send(fileName, 40, MPI_CHAR, rankID, 0, MPI_COMM_WORLD);
                            MPI_Send(fileContents,INT_MAX, MPI_CHAR, rankID, 0, MPI_COMM_WORLD);
                            for (int i = 0; i < strlen(fileName); i++) {
                                fileListTracker[indexFileTracker][i] = fileName[i];
                            }
                        
                            char temp = rankID + '0';
                            fileListTracker[indexFileTracker][strlen(fileName)] = ' ';
                            fileListTracker[indexFileTracker][strlen(fileName) + 1] = temp;
                       
                            indexFileTracker++;
                        } else {
                            for (int i = 0; i < indexFileTracker; i++) {
                                sscanf(fileListTracker[i], "%s %s", tempfileName, tempProcessID);
                                if (strcmp(tempfileName, fileName) == 0) {
                                    int newID = tempProcessID[0] - '0';
                                    MPI_Send(command, 10, MPI_CHAR, newID, 0, MPI_COMM_WORLD);
                                    MPI_Send(fileName, 40, MPI_CHAR, newID, 0, MPI_COMM_WORLD);
                                    MPI_Send(fileContents,INT_MAX, MPI_CHAR, newID, 0, MPI_COMM_WORLD);
                                    local = 1;
                                    break;
                                }
                            }

                            if (local == 0) {
                                MPI_Send(command, 10, MPI_CHAR, rankID, 0, MPI_COMM_WORLD);
                                MPI_Send(fileName, 40, MPI_CHAR, rankID, 0, MPI_COMM_WORLD);
                                MPI_Send(fileContents,INT_MAX, MPI_CHAR, rankID, 0, MPI_COMM_WORLD);
                                for (int i = 0; i < strlen(fileName); i++) {
                                    fileListTracker[indexFileTracker][i] = fileName[i];
                                }
                        
                                char temp = rankID + '0';
                                fileListTracker[indexFileTracker][strlen(fileName)] = ' ';
                                fileListTracker[indexFileTracker][strlen(fileName) + 1] = temp;
                            }
                            indexFileTracker++;
                        }
                        
                        rankID++;
                        rankID = (rankID == comSize) ? 1 : rankID;
                        
                    }

                    if (strcmp(command, "get") == 0) {
                        int targetIndex = 0;
                        int isFound = 0;
                        for (int i = 0; i < 100; i++) {
                            if (strstr(fileListTracker[i], fileName) != NULL) {
                                int length = strlen(fileListTracker[i]);
                                targetIndex = fileListTracker[i][length - 1] - '0';
                                isFound = 1;
                                break;
                            }
                        }

                        if (isFound == 0) {
                            printf("File not found\n");
                        } else {
                            MPI_Send(command, 10, MPI_CHAR, targetIndex, 0, MPI_COMM_WORLD);
                            MPI_Send(fileName, 40, MPI_CHAR, targetIndex, 0, MPI_COMM_WORLD);
                            MPI_Recv(fileContents, INT_MAX, MPI_CHAR, targetIndex, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                            newFilePtr = fopen(fileName, "w");
                            fwrite(fileContents, strlen(fileContents), 1, newFilePtr);
                            fclose(newFilePtr);
                        }
                        
                    }
                    break;

                case 2:
                    sscanf(shellCommand, "%s %s %d", command, fileName, &processID);
                    int fileContentsIndex = 0;
                    newFilePtr = fopen(fileName, "r");
                        
                    do {
                        char eachCharacter = fgetc(newFilePtr);

                        if (feof(newFilePtr)) break;
                        fileContents[fileContentsIndex] = eachCharacter;
                        fileContentsIndex++;
                    } while(1); // get the info from the selected file
                    fclose(newFilePtr);

                    int local = 0;

                    if (indexFileTracker == 0) {
                        for (int i = 0; i < strlen(fileName); i++) {
                            fileListTracker[indexFileTracker][i] = fileName[i];
                        }
                        
                        char temp = processID + '0';
                        fileListTracker[indexFileTracker][strlen(fileName)] = ' ';
                        fileListTracker[indexFileTracker][strlen(fileName) + 1] = temp;
                        
                        indexFileTracker++;

                        MPI_Send(command, 10, MPI_CHAR, processID, 0, MPI_COMM_WORLD);
                        MPI_Send(fileName, 40, MPI_CHAR, processID, 0, MPI_COMM_WORLD);
                        MPI_Send(fileContents,INT_MAX, MPI_CHAR, processID, 0, MPI_COMM_WORLD);
                    } else {
                        for (int i = 0; i < indexFileTracker; i++) {
                            sscanf(fileListTracker[i], "%s %s", tempfileName, tempProcessID);
                            if (strcmp(tempfileName, fileName) == 0) {
                                int newID = tempProcessID[0] - '0';
                                MPI_Send(command, 10, MPI_CHAR, newID, 0, MPI_COMM_WORLD);
                                MPI_Send(fileName, 40, MPI_CHAR, newID, 0, MPI_COMM_WORLD);
                                MPI_Send(fileContents,INT_MAX, MPI_CHAR, newID, 0, MPI_COMM_WORLD);
                                local = 1;
                                break;
                            }
                        }

                        if (local == 0) {
                            for (int i = 0; i < strlen(fileName); i++) {
                                fileListTracker[indexFileTracker][i] = fileName[i];
                            }
                        
                            char temp = processID + '0';
                            fileListTracker[indexFileTracker][strlen(fileName)] = ' ';
                            fileListTracker[indexFileTracker][strlen(fileName) + 1] = temp;
                            
                            indexFileTracker++;

                            MPI_Send(command, 10, MPI_CHAR, processID, 0, MPI_COMM_WORLD);
                            MPI_Send(fileName, 40, MPI_CHAR, processID, 0, MPI_COMM_WORLD);
                            MPI_Send(fileContents,INT_MAX, MPI_CHAR, processID, 0, MPI_COMM_WORLD);
                        }
                    }
                    break;
            }
            //reset everything
            spaces = 0;
            memset(command, 0, sizeof(command));
            memset(fileName, 0, sizeof(fileName));
            memset(fileContents, 0, sizeof(fileContents));
            memset(shellCommand, 0, sizeof(shellCommand));

        } else { // the process is not 0
                MPI_Recv(commandRecived, 10, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);

                //printf("commandRecived: %s fileNameRecived: %s fileContentsRecivied: %s at Rank: %d\n",commandRecived,fileNameRecived,fileContentsRecivied, myRank);
                if (strcmp(commandRecived, "put") == 0) {
                    MPI_Recv(fileNameRecived, 40, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    MPI_Recv(fileContentsRecivied, INT_MAX, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    sprintf(tempFolderName, "%d", myRank);
                    strcat(folderName, tempFolderName);
                    strcat(folderName, "/");
                    strcat(folderName, fileNameRecived);
                   
                    newFilePtr = fopen(folderName, "w");

                    fprintf(newFilePtr, "%s", fileContentsRecivied);
                    folderName[0] = 'p';
                    folderName[1] = '\0';

                    fclose(newFilePtr);
                } else if (strcmp(commandRecived, "get") == 0){
                    MPI_Recv(fileNameRecived, 40, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                    folderName[0] = '\0';
                    sprintf(folderName, "p%d/%s", myRank, fileNameRecived);

                    
                    newFilePtr = fopen(folderName, "r");
                    int fileContentsIndex = 0;
                    do {
                        char eachCharacter = fgetc(newFilePtr);
                        
                        if (feof(newFilePtr)) break;
                        fileContents[fileContentsIndex] = eachCharacter;
                        fileContentsIndex++;
                    } while(1); // get the info from the selected file 
                    
                    fclose(newFilePtr);
                    
                 
                    MPI_Send(fileContents, INT_MAX, MPI_CHAR, 0, 0, MPI_COMM_WORLD);
                } else if (strcmp(commandRecived, "exit\n") == 0) {
                    break;
                }

        }
    }

    MPI_Finalize();
    return 0;
}
