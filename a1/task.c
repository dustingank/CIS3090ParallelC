/* 3D Graphics using ASCII graphics
	-original NCurses code from "Game Programming in C with the Ncurses Library"
	 https://www.viget.com/articles/game-programming-in-c-with-the-ncurses-library/
	 and from "NCURSES Programming HOWTO"
	 http://tldp.org/HOWTO/NCURSES-Programming-HOWTO/
*/


#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include<string.h>
#include<time.h>
#include<float.h>
#include<pthread.h>

#ifndef NOGRAPHICS
#include<unistd.h>
#include<ncurses.h>
#endif

#define DELAY 10000

	// maximum screen size, both height and width
#define SCREENSIZE 100
	// default number of iterations to run before exiting, only used
	// when graphics are turned off
#define ITERATIONS 1000

	// number of points
int pointCount;
	// array of points before transformation
float **pointArray;
	// array of points after transformation
float **drawArray;

	// transformation matrix
float transformArray[4][4];
	// display buffers
char frameBuffer[SCREENSIZE][SCREENSIZE];
float depthBuffer[SCREENSIZE][SCREENSIZE];

struct argStruct{
   int counter;
   long rank;
};

void vectorMult(float a[4], float b[4], float c[4][4]);


#ifndef NOGRAPHICS
	// maximum screen dimensions
int max_y = 0, max_x = 0;
#endif




#ifndef NOGRAPHICS
int drawPoints() {
int c, i, j;
float multx, multy;
float point[4];

	// update screen maximum size
   getmaxyx(stdscr, max_y, max_x);

	// used to scale position of points based on screen size
   multx = (float)max_x / SCREENSIZE;
   multy = (float)max_y / SCREENSIZE;

   clear();

	// display points
   for(i=0; i<SCREENSIZE; i++) {
      for(j=0; j<SCREENSIZE; j++) {
         if (frameBuffer[i][j] == 'X')
            mvprintw(i * multy, j*multx, "X");
         else if (frameBuffer[i][j] == 'o')
            mvprintw(i * multy, j*multx, "o");
         else if (frameBuffer[i][j] == '.')
            mvprintw(i * multy, j*multx, ".");
      }
   }

   refresh();

   usleep(DELAY);

	// read keyboard and exit if 'q' pressed
   c = getch();
   if (c == 'q') return(1);
   else return(0);
}
#endif

	/* calculates the product of matrices b and c
           stores the result in matrix a */
void matrixMult(float a[4][4], float b[4][4], float c[4][4]) {
int row, col, element;

   for (row=0; row<4; row++) {
      for (col=0; col<4; col++) {
         a[row][col] = 0.0;
         for (element=0; element<4; element++) {
            a[row][col] += b[row][element] * c[element][col];
         }
      }
   }
}


	/* calculates the product of vector b and matrix c
           stores the result in vector a */
void vectorMult(float a[4], float b[4], float c[4][4]) {
int col, element;


   for (col=0; col<4; col++) {
      a[col] = 0.0;
      for (element=0; element<4; element++) {
         a[col] += b[element] * c[element][col];
      }
   }

}

void allocateArrays() {
int i;

   pointArray = malloc(sizeof(float *) * pointCount);
   for(i=0; i<pointCount; i++)
      pointArray[i] = malloc(sizeof(float) * 4);

   drawArray = malloc(sizeof(float *) * pointCount);
   for(i=0; i<pointCount; i++)
      drawArray[i] = malloc(sizeof(float) * 4);
}

void cubePointArray() {
   pointArray[0][0] = 0.5;
   pointArray[0][1] = 0.0;
   pointArray[0][2] = 0.5;
   pointArray[0][3] = 1.0;

   pointArray[1][0] = 0.5;
   pointArray[1][1] = 0.0;
   pointArray[1][2] = -0.5;
   pointArray[1][3] = 1.0;

   pointArray[2][0] = -0.5;
   pointArray[2][1] = 0.0;
   pointArray[2][2] = -0.5;
   pointArray[2][3] = 1.0;

   pointArray[3][0] = -0.5;
   pointArray[3][1] = 0.0;
   pointArray[3][2] = 0.5;
   pointArray[3][3] = 1.0;

   pointArray[4][0] = 0.5;
   pointArray[4][1] = 1.0;
   pointArray[4][2] = 0.5;
   pointArray[4][3] = 1.0;

   pointArray[5][0] = 0.5;
   pointArray[5][1] = 1.0;
   pointArray[5][2] = -0.5;
   pointArray[5][3] = 1.0;

   pointArray[6][0] = -0.5;
   pointArray[6][1] = 1.0;
   pointArray[6][2] = -0.5;
   pointArray[6][3] = 1.0;

   pointArray[7][0] = -0.5;
   pointArray[7][1] = 1.0;
   pointArray[7][2] = 0.5;
   pointArray[7][3] = 1.0;

}

void randomPointArray() {
int i;
float val;

   for (i=0; i<pointCount; i++) {
       val = (float) random() / 10000.0;
       pointArray[i][0] = 2.5 * ((val - trunc(val)) - 0.5);
       val = (float) random() / 10000.0;
       pointArray[i][1] = 2.5 * ((val - trunc(val)) - 0.5);
       val = (float) random() / 10000.0;
       pointArray[i][2] = 2.5 * ((val - trunc(val)) - 0.5);
       val = (float) random() / 10000.0;
       pointArray[i][3] = 2.5 * ((val - trunc(val)) - 0.5);
   }
}

void initTransform() {
int i, j;

   for (i=0; i<4; i++)
      for (j=0; j<4; j++)
        if (i == j)
           transformArray[i][j] = 1.0;
        else
           transformArray[i][j] = 0.0;
}


void xRot(int rot) {
float oneDegree = 0.017453;
float angle, sinAngle, cosAngle;
float result[4][4];
int i, j;
float rotx[4][4]  = { {1.0, 0.0, 0.0, 0.0},
                         {0.0, 1.0, 0.0, 0.0},
                         {0.0, 0.0, 1.0, 0.0},
                         {0.0, 0.0, 0.0, 1.0}};

   angle = (float) rot * oneDegree;
   sinAngle = sinf(angle);
   cosAngle = cosf(angle);

   rotx[1][1] = cosAngle;
   rotx[2][2] = cosAngle;
   rotx[1][2] = -sinAngle;
   rotx[2][1] = sinAngle;

   matrixMult(result, transformArray, rotx);

	// copy result to transformation matrix
   for (i=0; i<4; i++)
      for (j=0; j<4; j++)
         transformArray[i][j] = result[i][j];
}

void yRot(int rot) {
float oneDegree = 0.017453;
float angle, sinAngle, cosAngle;
float result[4][4];
int i, j;
float roty[4][4]  = { {1.0, 0.0, 0.0, 0.0},
                         {0.0, 1.0, 0.0, 0.0},
                         {0.0, 0.0, 1.0, 0.0},
                         {0.0, 0.0, 0.0, 1.0}};

   angle = (float) rot * oneDegree;
   sinAngle = sinf(angle);
   cosAngle = cosf(angle);

   roty[0][0] = cosAngle;
   roty[2][2] = cosAngle;
   roty[0][2] = sinAngle;
   roty[2][0] = -sinAngle;

   matrixMult(result, transformArray, roty);

	// copy result to transformation matrix
   for (i=0; i<4; i++)
      for (j=0; j<4; j++)
         transformArray[i][j] = result[i][j];
}

void zRot(int rot) {
float oneDegree = 0.017453;
float angle, sinAngle, cosAngle;
float result[4][4];
int i, j;
float rotz[4][4]  = {{1.0, 0.0, 0.0, 0.0},
                        {0.0, 1.0, 0.0, 0.0},
                        {0.0, 0.0, 1.0, 0.0},
                        {0.0, 0.0, 0.0, 1.0}};

   angle = (float) rot * oneDegree;
   sinAngle = sinf(angle);
   cosAngle = cosf(angle);

   rotz[0][0] = cosAngle;
   rotz[1][1] = cosAngle;
   rotz[0][1] = -sinAngle;
   rotz[1][0] = sinAngle;

   matrixMult(result, transformArray, rotz);

	// copy result to transformation matrix
   for (i=0; i<4; i++)
      for (j=0; j<4; j++)
         transformArray[i][j] = result[i][j];
}

void translate(float x, float y, float z) {
   transformArray[3][0] = x;
   transformArray[3][1] = y;
   transformArray[3][2] = z;
}

void *clearBuffers(void* arg) {
   int i, j;

	// empty the frame buffer
	// set the depth buffer to a large distance
   for(i=0; i<SCREENSIZE; i++) {
      for(j=0; j<SCREENSIZE; j++) {
         frameBuffer[i][j] = ' ';
         depthBuffer[i][j] = -1000.0;
      }
   }

   return NULL;
}

void *transformThePoints(void* arg) {
   //struct argStruct *args = (struct argStruct*) arg;
   //int counter = args->counter;

   for (int i = 0; i < pointCount; i++) {
      vectorMult(drawArray[i], pointArray[i], transformArray);

      drawArray[i][0] *= 20;
      drawArray[i][1] *= 20;
      drawArray[i][0] += 50;
      drawArray[i][1] += 50;

      drawArray[i][2] *= 20;
      drawArray[i][2] += 50;
   }
   return NULL;
}

void movePoints() {
   static int counter = 1;
   int i;
   int x, y;
   pthread_t *threadsArray = NULL;
   long threadID = 0;
   threadsArray = malloc(2 * sizeof(pthread_t));
   struct argStruct argumment;

	// initialize transformation matrix
	// this needs to be done before the transformation is performed
   initTransform();

	// add some rotations to the transformation matrix
	// this needs to be done before the transformation is performed
   xRot(counter);
   yRot(counter);
   counter++;


	// transform the points using the transformation matrix
	// store the results of the transformation in the drawing array

   pthread_create(&threadsArray[0], NULL, clearBuffers, (void*)threadID);
   // clears buffers before drawing screen
   argumment.rank = 1;
   argumment.counter = counter;
   pthread_create(&threadsArray[1], NULL, transformThePoints, (void*)&argumment);


   for (i = 0; i < 2; i++) {
      pthread_join(threadsArray[i], NULL);
   }

   free(threadsArray);

	// draw the screen
	// adds points to the frame buffer, use depth buffer to
	// sort points based upon distance from the viewer
   for (i=0; i<pointCount; i++) {
      x = (int) drawArray[i][0];
      y = (int) drawArray[i][1];

      if (depthBuffer[x][y] < drawArray[i][2]) {
         if (drawArray[i][2] > 60.0)
            frameBuffer[x][y] = 'X';
         else if (drawArray[i][2] < 40.0)
            frameBuffer[x][y] = '.';
         else
            frameBuffer[x][y] = 'o';

        depthBuffer[x][y] = drawArray[i][2];
      }
   }

}

int main(int argc, char *argv[]) {
int i, count;
int argPtr;
int drawCube, drawRandom;

	// set number of iterations, only used for timing tests
	// not used in curses version
   count = ITERATIONS;

	// initialize drawing objects
   drawCube = 0;
   drawRandom = 0;


	// read command line arguments for number of iterations
   if (argc > 1) {
      argPtr = 1;
      while(argPtr < argc) {
         if (strcmp(argv[argPtr], "-i") == 0) {
            sscanf(argv[argPtr+1], "%d", &count);
            argPtr += 2;
         } else if (strcmp(argv[argPtr], "-cube") == 0) {
            drawCube = 1;
            pointCount = 8;
            argPtr += 1;
         } else if (strcmp(argv[argPtr], "-points") == 0) {
            drawRandom = 1;
            sscanf(argv[argPtr+1], "%d", &pointCount);
            argPtr += 2;
         } else {
            printf("USAGE: %s <-i iterations> <-cube | -points #>\n", argv[0]);
            printf(" iterations -the number of times the population will be updated\n");
	    printf("    the number of iterations only affects the non-curses program\n");
	    printf(" the curses program exits when q is pressed\n");
	    printf(" choose either -cube to draw the cube shape or -points # to\n");
	    printf("    draw random points where # is an integer number of points to draw\n");
            exit(1);
         }
      }
   }

	// allocate space for arrays to store point position
   allocateArrays();
   if (drawCube == 1)
      cubePointArray();
   else if (drawRandom == 1)
      randomPointArray();
   else {
      printf("You must choose either <-cube> or <-points #> on the command line.\n");
      exit(1);
   }


#ifndef NOGRAPHICS
	// initialize ncurses
   initscr();
   noecho();
   cbreak();
   timeout(0);
   curs_set(FALSE);
     // Global var `stdscr` is created by the call to `initscr()`
   getmaxyx(stdscr, max_y, max_x);
#endif


	// draw and move points using ncurses
	// do not calculate timing in this loop, ncurses will reduce performance
#ifndef NOGRAPHICS
   while(1) {
      if (drawPoints() == 1) break;
      movePoints();
   }
#endif

	// calculate movement of points but do not use ncurses to draw
#ifdef NOGRAPHICS
   printf("Number of iterations %d\n", count);

	/*** Start timing here ***/
   struct timespec start, end;
   clock_gettime(CLOCK_REALTIME, &start);

   for(i=0; i<count; i++) {
      movePoints();
   }
clock_gettime(CLOCK_REALTIME, &end);

double timeSpent = (end.tv_sec - start.tv_sec) +
						(end.tv_nsec - start.tv_nsec) / 1000000000.0;
printf("Time elpased is %f seconds from task parallism is: \n", timeSpent);

	/*** End timing here ***/
#endif

#ifndef NOGRAPHICS
	// shut down ncurses
   endwin();
#endif

}
