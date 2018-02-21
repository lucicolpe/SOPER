
all: cadena_montaje ejercicio_final

cadena_montaje: cadena_montaje.c
	gcc -o cadena_montaje cadena_montaje.c

ejercicio_final: ejercicio_final.c semaforos.c semaforos.h
	gcc -o ejercicio_final ejercicio_final.c semaforos.c -lpthread

clean:
	rm -f cadena_montaje ejercicio_final
