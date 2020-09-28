This folder should have data.c task.c makefile results.pdf results.txt readme.txt

makefile doesn't compile graphics.c into graphicspt and graphics.

To compile the program, type "make all".

The warning messages are all from professor source code, not mine.

The data.c will default runs 8 threads to the points, if you need to test different number of thread, you have to change the source code by yourself
on line 39. "#define THREADNUMBER " with thread number.

The task.c has two thread to run clearBuffer() and the loop calculate drawArray parallel. 

