/*
Name: Yizhou Wang
ID: 1013411
Date: Octtober 10th 2020
Description: A2, pool.c file
*/


#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<pthread.h>
#include<unistd.h>


	// struct queue which stores tasks to be run as function pointers
struct queue_t {
   void *(**fn)(void);
   int head, tail, qLength;
   pthread_mutex_t qLock;
   pthread_cond_t qCon;
   int qcount;
};

struct queue_t *queue;

	// struct which stores pool of threads
struct threadpool_t {
   pthread_mutex_t lock;
   pthread_cond_t con;
   pthread_t *pool;
   long count;
};

struct threadpool_t *poolStruct;

	// globals used to identify when all tasks are complete and when
	// all threads are idle - used to end the program
int noMoreTasks;
int numThreadsIdle;


	// manages the threads, extracts task from queue and has thread
	// run the task, makes threads wait when no tasks to run
void * emptyTask(void *r) {
	long rank = (long) r;
		// used to store function pointer which was copied from queue
	void *(*fn)(void) = NULL;
		// identifies if the current thread is locked, only changes by that thread
	int threadLocked = 0;


	// continuously loop until progam exits
   while(1) {

	// lock queue structure before removing a task
	// only do this if the thread is not already locked (threadLocked == 0)


	// set the thread locked variable to 1 (locked - it's after the mutex)


	//      -- remove task from queue if there is one --
	// if there is a task in the queue (the queue is not empty) then
	//   copy the task function ptr from the queue to local variable
	//   set task pointer in queue to NULL 
	//   move tail of queue to next item in queue
	//   reduce the count of items in the queue by one
	//   unlock the queue
	//   send a conditional signal to the mutex on the queue to wake up another thread
	//   set the thread locked variable to 0 (unlocked)
      if (queue->qcount != 0) {
         printf("emptyTask: thread %ld will execute task1 %d\n", rank, queue->tail+1);

      }


	//          -- execute the task -- 
	// if a function pointer was extracted from the queue (fn != NULL) then
	//    execute that function (this is done without the queue being locked)
	//    set the local function pointer to NULL when task complete
	// else
	//    -this should only execute if the queue is still locked-
	//    increment the idle threads count
	//    use a conditional wait on the thread (thread waits and lock on queue released)
	//    decrement the idle threads count (this runs when the waiting thread awakens
	//            and reclaims the lock it released earlier)
      if (fn != NULL) {

      } else {

      }

   } // while

	// never reached, program just exits when all tasks complete and threads
	// are destroyed by exiting program - not a tidy way to end the program  
   return(NULL);
}


	// creates data structures and starts threads
	//    numberThreads - the number of threads to create
	//    qsize - the number of elements to put in the queue
void init(long numberThreads, int qsize) {

	// allocate queue structure and initialize head, tail, length of queue,
	//    and number of items currently in queue  variables
	queue = (struct queue_t*)malloc(sizeof(struct queue_t) * qsize);

	
	// initialize lock and condition for the queue structure


	// initialize array to store function pointers and set all pointers to NULL


	// initialize thread pool struct
	poolStruct = (struct threadpool_t*) malloc (sizeof(struct threadpool_t));

	// set the number of threads variable in the pool struct
	poolStruct->count = (long) numberThreads;

	// create array of threads (in the struct) and have each thread
	// execute emptyTask()
	for (int index = 0; index < numberThreads; index++) {
		// do something here
	}

}



	// pushes a task (a function pointer to a task function) onto the queue
	// the function pointer is passed as a parameter
	// the task number is also a parameter
void addTask(void * (fn)(void), int tasknum) {

	// lock queue mutex before making changes to the  queue


	// update the head of the queue


	// increment the number of items in the queue 


	// copy passed function pointer to the queue


	// unlock queue mutex after new task added to queue
	// and then signal queue condition to wake up a thread (in case all threads waiting)

}


	// task performed by the threads
	// the sleep() makes it easier to see the results when using multiple threads
void *task1(void) {
   sleep(1);
   return(NULL);
}



int main(int argc, char *argv[]) {
	int i;
		// variable used to read command line args
	long tcount;
	int tasks, qsize;
	int argPtr;

	// set number of threads, tasks, and size of queue
	tcount = 4;
	tasks = 10;
	qsize = 100;

	// read command line arguments for number of iterations 
   if (argc > 1) {
      argPtr = 1;
      while(argPtr < argc) {
         if (strcmp(argv[argPtr], "-n") == 0) {
            sscanf(argv[argPtr+1], "%ld", &tcount);
            argPtr += 2;
         } else  if (strcmp(argv[argPtr], "-t") == 0) {
            sscanf(argv[argPtr+1], "%d", &tasks);
            argPtr += 2;
         } else  if (strcmp(argv[argPtr], "-q") == 0) {
            sscanf(argv[argPtr+1], "%d", &qsize);
            argPtr += 2;
         } else {
            printf("USAGE: %s <-n number of threads> <-t number of tasks>\n", argv[0]);
            printf(" -n number of threads : how many threads to create in the pool\n");
            printf(" -t number of tasks : how many tasks to put in the queuel\n");
            printf(" -q size of queue : how many tasks can the queue hold \n");
            exit(1);
         }
      }
   }

   printf("Number of threads: %ld\n", tcount);
   printf("Number of tasks: %d\n", tasks);
   printf("Size of the queue: %d\n", qsize);


	// initialize variables which allow program to exit
	//    noMoreTasks set to 1 when all tasks added to queue
	//    numThreadsIdle set to 1 when all threads are waiting for a cond signal
   noMoreTasks = 0;
   numThreadsIdle = 0;

	// init() - build data structures, create thread pool
   init(tcount, qsize);

	// submit tasks to queue using addTask()
   for(i=0; i<tasks; i++) {
      addTask(task1, i);
      noMoreTasks = 1;
   }
	// loop until all tasks have been submitted to the queue and the
	// number of threads which are idle equals the total number of threads (all
	// threads are idle)
   while(1){
      if ((noMoreTasks == 1) && (numThreadsIdle == tcount) && (queue->qcount == 0)) break; 
   };

}

