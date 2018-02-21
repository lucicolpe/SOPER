all: clean ejercicio3a ejercicio3b ejercicio8

CCFlags = -pedantic -ansi -g

ejercicio3a: ejercicio3a.o
	gcc -o ejercicio3a ejercicio3a.o

ejercicio3b: ejercicio3b.o
	gcc -o ejercicio3b ejercicio3b.o -lpthread

ejercicio3a.o: ejercicio3a.c
	gcc $(CCFlags) -c  ejercicio3a.c

ejercicio3b.o: ejercicio3b.c
	gcc $(CCFlags) -c  ejercicio3b.c

ejercicio8: ejercicio8.o
	gcc -o ejercicio8 ejercicio8.o

ejercicio8.o: ejercicio8.c
	gcc $(CCFlags) -c  ejercicio8.c

clean:
	rm -rf *.o ejercicio3a ejercicio3b ejercicio8 #*.tgz
	
