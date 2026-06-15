
CC = gcc
CFLAGS = -std=c23 -Wall

.PHONY: all test clean
#Regla de compilacion

test_strings: Strings.o StringsTest.o
	$(CC) $(CFLAGS) Strings.o StringsTest.o -o test_strings	

enlineas: enlineas.o
	$(CC) $(CFLAGS) enlineas.o -o enlineas

longitudes: longitudes.o Strings.o
	$(CC) $(CFLAGS) longitudes.o Strings.o -o longitudes

mayorlongitud: mayorlongitud.o Strings.o 
	$(CC) $(CFLAGS) mayorlongitud.o Strings.o -o mayorlongitud

suma: suma.o Strings.o
	$(CC) $(CFLAGS) suma.o Strings.o -o suma

todosiguales: todosiguales.o Strings.o
	$(CC) $(CFLAGS) todosiguales.o Strings.o -o todosiguales

#Construye todo
all: test_strings enlineas longitudes mayorlongitud suma todosiguales
#Regla para ejecutar
test: test_strings
	./test_strings
# limpieza
clean: 
	rm -f *.exe *.o


Strings.o: Strings.c Strings.h
	$(CC) $(CFLAGS) -c Strings.c -o Strings.o

StringsTest.o: StringsTest.c Strings.h
	$(CC) $(CFLAGS) -c StringsTest.c -o StringsTest.o

enlineas.o: enlineas.c
	$(CC) $(CFLAGS) -c enlineas.c -o enlineas.o

longitudes.o: longitudes.c Strings.h
	$(CC) $(CFLAGS) -c longitudes.c -o longitudes.o

mayorlongitud.o: mayorlongitud.c Strings.h
	$(CC) $(CFLAGS) -c mayorlongitud.c -o mayorlongitud.o
	
suma.o :suma.c Strings.h
	$(CC) $(CFLAGS) -c suma.c -o suma.o

todosiguales.o: todosiguales.c Strings.h
	$(CC) $(CFLAGS) -c todosiguales.c -o todosiguales.o