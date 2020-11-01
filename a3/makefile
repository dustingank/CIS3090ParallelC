all: mpish
run: mpish
	mpiexec -np 4 ./mpish
mpish: mpish.c
	mpicc -g mpish.c -o mpish
clean:
	rm mpish