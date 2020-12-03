__kernel void automatonFunction(__global char* pattern, __global int* size,  __global char* wholeGraph, __local char* row) {

    int arraySize = *size;
    int upperBound = 0;
    int maxUpperBound = 0;
    int totalKernels = get_global_size(0);
    local int arrayBounds[25];
    local int eachSize[25];
    uint kernelRank;
    //char[arraySize][arraySize] board;
    
    // copy global into local
    for (int i = 0; i < arraySize; i++) {
        row[i] = pattern[i];
        wholeGraph[0 * arraySize + i] = pattern[i];
    }
    
    for (int i = 0; i < totalKernels; i++) {
        eachSize[i] = arraySize / totalKernels;
    }

    for(int i = 0; i < arraySize % totalKernels; i++){
        eachSize[i]++;
    }

    for (int i = 0; i <= totalKernels; i++) {
        if (i == 0) {
            arrayBounds[i] = 0;
        } else if (i == totalKernels){
            arrayBounds[i] = arraySize;
        } else {
            arrayBounds[i] = arrayBounds[i - 1] + eachSize[i - 1];
        }
    }

    //or (int i = 0; i <= totalKernels; i++) {
    //    printf("bounds: %d\n", arrayBounds[i]);
    //}
    kernelRank = get_global_id(0);
    
    for(int i = 1; i < arraySize; i++) {

        for (int a = 0; a < arraySize; a++) {
            row[a] = wholeGraph[(i - 1) * arraySize + a];
        }

        for (int j = arrayBounds[kernelRank]; j < arrayBounds[kernelRank + 1]; j++) {
            int prevIndex = 0;
            int nextIndex = 0;
            char neighbourhood[4];

            if (j == 0) {
                prevIndex = arraySize - 1;
            } else {
                prevIndex = j - 1;
            }

            if(j == arraySize - 1) {
                nextIndex = 0;
            } else {
                nextIndex = j + 1;
            }

            if (j != 0 && j != arraySize - 1) {
                neighbourhood[0] = row[prevIndex];
                neighbourhood[1] = row[j];
                neighbourhood[2] = row[nextIndex];
                neighbourhood[3] = '\0';
            } else if ( j == 0) {
                neighbourhood[0] = '.';
                neighbourhood[1] = row[j];
                neighbourhood[2] = row[nextIndex];
                neighbourhood[3] = '\0';
            } else if (j == arraySize - 1) {
                neighbourhood[0] = row[prevIndex];
                neighbourhood[1] = row[j];
                neighbourhood[2] = '.';
                neighbourhood[3] = '\0';
            }


            if(neighbourhood[0] != '.' && neighbourhood[1] != '.' && neighbourhood[2] != '.') {
                wholeGraph[i * arraySize + j] = '.';
            } else if(neighbourhood[0] != '.' && neighbourhood[1] != '.' && neighbourhood[2] == '.') {
                wholeGraph[i * arraySize + j] = '.';
            } else if(neighbourhood[0] != '.' && neighbourhood[1] == '.' && neighbourhood[2] != '.') {
                wholeGraph[i * arraySize + j] = '.';
            } else if(neighbourhood[0] != '.' && neighbourhood[1] == '.' && neighbourhood[2] == '.') {
                wholeGraph[i * arraySize + j] = kernelRank + '0';
            } else if(neighbourhood[0] == '.' && neighbourhood[1] != '.' && neighbourhood[2] != '.') {
                wholeGraph[i * arraySize + j] = kernelRank + '0';
            } else if(neighbourhood[0] == '.' && neighbourhood[1] != '.' && neighbourhood[2] == '.') {
                wholeGraph[i * arraySize + j] = kernelRank + '0';
            } else if(neighbourhood[0] == '.' && neighbourhood[1] == '.' && neighbourhood[2] != '.') {
                wholeGraph[i * arraySize + j] = kernelRank + '0';
            } else if(neighbourhood[0] == '.' && neighbourhood[1] == '.' && neighbourhood[2] == '.') {
                wholeGraph[i * arraySize + j] = '.';
            }
            
        }
        barrier(CLK_LOCAL_MEM_FENCE);
        
        //printf("\n");
    }
}