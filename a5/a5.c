/* Name: Yizhou Wang
 * ID: 1013411 
*/

#define PROGRAM_FILE "a5_openCL.cl"
#define KERNEL_FUNC "automatonFunction"
#define CL_TARGET_OPENCL_VERSION 220

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <math.h>

//#include <OpenCL/cl.h>

#include <CL/cl.h>

cl_device_id createDevice();
cl_program build_program(cl_context, cl_device_id, const char*);
char* completePattern(int, char[], int);

int main(int argc, char* argv[]) {
    // assume only have one work-group
    int argPtr = 1;
    int workItmes = 1;
    int arraySize = 20;
    int initConfig = 0;
    int middle = 0;
    
    int *sizePtr = NULL;
    char *initialPattern = NULL;
    char *wholeGraph = NULL;

    cl_device_id device;
    cl_context context;
    cl_program program;
    cl_kernel kernel;
    cl_command_queue queue;
    cl_mem inputBuffer, inputSizeBuffer, resultBuffer; // may need to re-name
    cl_int err;
    size_t localSize, globalSize;

    srand(time(NULL));

    if (argc > 1) {
        argPtr = 1;

        while (argPtr < argc) {
            if (strcmp(argv[argPtr], "-n") == 0) {
                sscanf(argv[argPtr + 1], "%d", &workItmes);
            }

            if (strcmp(argv[argPtr], "-s") == 0) {
                sscanf(argv[argPtr + 1], "%d", &arraySize);
            }

            if (strcmp(argv[argPtr], "-i") == 0) {
                sscanf(argv[argPtr + 1], "%d", &initConfig);
            }

            argPtr++;
        }
    }

    initialPattern = (char*)malloc(sizeof(char) * arraySize);
    wholeGraph = (char*) malloc (sizeof(char) * arraySize * arraySize);
    middle = arraySize / 2;

    for(int i = 0; i < arraySize; i++) {
        initialPattern[i] = '.';
    }

    switch(initConfig) {
        case 0:
            for(int i = 0; i < arraySize; i++) {
                if (rand() & 1) {
                    initialPattern[i] = 'X';
                }
            }
            break;
        case 1:
            initialPattern[middle - 1] = 'X';
            initialPattern[middle + 1] = 'X';
            initialPattern[middle + 2] = 'X';
            break;
        case 2:
            initialPattern[middle - 2] = 'X';
            initialPattern[middle - 1] = 'X';
            initialPattern[middle] = 'X';
            initialPattern[middle + 1] = 'X';
            initialPattern[middle + 2] = 'X';
            break;
        case 3:
            initialPattern[middle - 2] = 'X';
            initialPattern[middle] = 'X';
            initialPattern[middle + 1] = 'X';
            initialPattern[middle + 2] = 'X';
            break;
        case 4:
            initialPattern[middle - 3] = 'X';
            initialPattern[middle - 2] = 'X';
            initialPattern[middle - 1]= 'X';
            initialPattern[middle] = 'X';
            initialPattern[middle + 1] = 'X';
            initialPattern[middle + 2] = 'X';
            initialPattern[middle + 3] = 'X';
            break;
    }

    localSize = workItmes;
    globalSize = workItmes;
    sizePtr = &arraySize;

    // get the device ID
    device = createDevice();

    //create the context
    context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
    if (err < 0) {
        perror("Coundn't create a context");
        exit(1);
    }

    // read / create the openCL source code
    program = build_program(context, device, PROGRAM_FILE);

    // create the data buffer
    inputBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY| CL_MEM_COPY_HOST_PTR, arraySize * sizeof(char), initialPattern, &err);
    inputSizeBuffer = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(int), sizePtr, &err);
    resultBuffer = clCreateBuffer(context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, arraySize * arraySize * sizeof(char), wholeGraph, &err);
    if(err < 0) {
        perror("Couldn't create a buffer");
        exit(1);   
    }
    // create the command queue (Link C program with the GPU)
    queue = clCreateCommandQueueWithProperties(context, device, 0, &err);
    if (err < 0) {
        perror("Coundn't create a command queue");
        exit(1);
    }

    //create openCL kernal
    kernel = clCreateKernel(program, KERNEL_FUNC, &err);
    if(err < 0) {
        perror("Couldn't create a kernel");
        exit(1);
    }

    // create kernal arguments
    err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &inputBuffer);
    err |= clSetKernelArg(kernel, 1, sizeof(cl_mem), &inputSizeBuffer);
    err |= clSetKernelArg(kernel, 2, sizeof(cl_mem), &resultBuffer);
    err |= clSetKernelArg(kernel, 3, arraySize * sizeof(char), NULL);
    if(err < 0) {
        perror("Couldn't create a kernel argument");
        exit(1);
    }

    // send kernal to device
    err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &localSize, &globalSize, 0, NULL, NULL);
    if(err < 0) {
        perror("Couldn't enqueue the kernel");
        exit(1);
    }

    // get the result back from kernel
    err = clEnqueueReadBuffer(queue, resultBuffer, CL_TRUE, 0, sizeof(char) * arraySize * arraySize, wholeGraph, 0, NULL, NULL);
    if(err < 0) {
        perror("Couldn't read the kernel");
        exit(1);
    }
    
    
    for (int k = 0; k < arraySize; ++k){
        for(int j = 0; j < arraySize; ++j){
            if (wholeGraph[k * arraySize + j] == '.') {
                printf(" ");
            } else {
                printf("%c", wholeGraph[k * arraySize + j]);
            }
            
        }
        printf("\n");
    }


    /*Free everything*/
    clReleaseKernel(kernel);
    clReleaseMemObject(resultBuffer);
    clReleaseMemObject(inputBuffer);
    clReleaseCommandQueue(queue);
    clReleaseProgram(program);
    clReleaseContext(context);
    free(wholeGraph);
    free(initialPattern);
}


cl_device_id createDevice() {
    cl_platform_id platform;
    cl_device_id dev;
    int err;

    /* Identify a platform */
    err = clGetPlatformIDs(1, &platform, NULL);
    if(err < 0) {
        perror("Couldn't identify a platform");
        exit(1);
    } 

    /* Access a device */
    err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &dev, NULL);
    if(err == CL_DEVICE_NOT_FOUND) {
        err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_CPU, 1, &dev, NULL);
    }
    if(err < 0) {
        perror("Couldn't access any devices");
        exit(1);   
    }

   return dev;
}

cl_program build_program(cl_context ctx, cl_device_id dev, const char* filename) {

   cl_program program;
   FILE *program_handle;
   char *program_buffer, *program_log;
   size_t program_size, log_size;
   int err;

   /* Read program file and place content into buffer */
   program_handle = fopen(filename, "r");
   if(program_handle == NULL) {
      perror("Couldn't find the program file");
      exit(1);
   }
   fseek(program_handle, 0, SEEK_END);
   program_size = ftell(program_handle);
   rewind(program_handle);
   program_buffer = (char*)malloc(program_size + 1);
   program_buffer[program_size] = '\0';
   fread(program_buffer, sizeof(char), program_size, program_handle);
   fclose(program_handle);

   /* Create program from file */
   program = clCreateProgramWithSource(ctx, 1, 
      (const char**)&program_buffer, &program_size, &err);
   if(err < 0) {
      perror("Couldn't create the program");
      exit(1);
   }
   free(program_buffer);

   /* Build program */
   err = clBuildProgram(program, 0, NULL, NULL, NULL, NULL);
   if(err < 0) {

      /* Find size of log and print to std output */
      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 
            0, NULL, &log_size);
      program_log = (char*) malloc(log_size + 1);
      program_log[log_size] = '\0';
      clGetProgramBuildInfo(program, dev, CL_PROGRAM_BUILD_LOG, 
            log_size + 1, program_log, NULL);
      printf("%s\n", program_log);
      free(program_log);
      exit(1);
   }

   return program;
}

char* completePattern(int size, char pattern[], int arrayLength) {
    char *initialPattern = (char*)malloc(size * sizeof(char));
    int patternMid = size / 2;
    

    for (int i = 0; i < size; i++) {
        initialPattern[i] = '.';
    }

    if (arrayLength == 1) {
        initialPattern[patternMid] = pattern[0];
        return initialPattern;
    } 

    int subMid = arrayLength / 2 - 1;
    int lowerPtr = subMid;
    int upperPtr = subMid;
    int rounds = 1;

    *(initialPattern + patternMid) = pattern[subMid];

    while (lowerPtr != 0 || upperPtr != (arrayLength - 1) ) {

        if (lowerPtr != 0) {
            lowerPtr -= 1;
            *(initialPattern + patternMid - rounds) = pattern[lowerPtr];
        }

        if (upperPtr != (arrayLength - 1)) {
            upperPtr += 1;
            *(initialPattern + patternMid + rounds) = pattern[upperPtr];
        }
        rounds += 1;
    }

    return initialPattern;
}


