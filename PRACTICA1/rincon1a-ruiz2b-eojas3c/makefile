# Makefile para compilar el programa de búsqueda de autos

CC = gcc
CFLAGS = -Wall -O2

all: buscador interfaz

buscador: p1-dataProgram.c
	$(CC) $(CFLAGS) p1-dataProgram.c -o buscador

interfaz: interfaz.c
	$(CC) $(CFLAGS) interfaz.c -o interfaz

clean:
	rm -f buscador interfaz
	rm -f /tmp/fifo_peticion /tmp/fifo_respuesta
