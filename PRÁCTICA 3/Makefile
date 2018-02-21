all: clean ejercicio2 semaforos.o semaforos_test ejercicio6 

CCFlags = -pedantic -g

ejercicio2: ejercicio2.o
	gcc -o ejercicio2 ejercicio2.o

semaforos_test: semaforos_test.o semaforos.o
	gcc -o semaforos_test semaforos_test.o semaforos.o

ejercicio6: ejercicio6.o semaforos.o
	gcc -o ejercicio6 ejercicio6.o semaforos.o

ejercicio2.o: ejercicio2.c
	gcc $(CCFlags) -c  ejercicio2.c
 
semaforos.o: semaforos.c semaforos.h
	gcc $(CCFlags) -c semaforos.c

clean:
	rm -rf *.o ejercicio2 semaforos_test ejercicio6