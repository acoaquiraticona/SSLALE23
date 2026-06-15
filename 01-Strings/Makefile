CC = gcc
CFLAGS = -std=c23 -Wall -Wextra -g

.PHONY: all test clean

# ==================== TODOS LOS EJECUTABLES ====================

all: test_strings test_conversion enlineas longitudes mayorlongitud todosiguales suma

# ====================== TESTS ======================

test_strings: Strings.o StringsTest.o
	$(CC) $(CFLAGS) Strings.o StringsTest.o -o test_strings

test_conversion: Conversion.o ConversionTest.o Strings.o
	$(CC) $(CFLAGS) Conversion.o ConversionTest.o Strings.o -o test_conversion

# ================== PROGRAMAS PRINCIPALES ==================

enlineas: enlineas.o
	$(CC) $(CFLAGS) enlineas.o -o enlineas

longitudes: longitudes.o Strings.o
	$(CC) $(CFLAGS) longitudes.o Strings.o -o longitudes

mayorlongitud: mayorlongitud.o Strings.o
	$(CC) $(CFLAGS) mayorlongitud.o Strings.o -o mayorlongitud

todosiguales: todosiguales.o Strings.o
	$(CC) $(CFLAGS) todosiguales.o Strings.o -o todosiguales

suma: suma.o Strings.o Conversion.o
	$(CC) $(CFLAGS) suma.o Strings.o Conversion.o -o suma

# ====================== COMPILACIÓN DE OBJETOS ======================

Strings.o: Strings.c Strings.h
	$(CC) $(CFLAGS) -c Strings.c -o Strings.o

StringsTest.o: StringsTest.c Strings.h
	$(CC) $(CFLAGS) -c StringsTest.c -o StringsTest.o

Conversion.o: Conversion.c Conversion.h Strings.h
	$(CC) $(CFLAGS) -c Conversion.c -o Conversion.o

ConversionTest.o: ConversionTest.c Conversion.h Strings.h
	$(CC) $(CFLAGS) -c ConversionTest.c -o ConversionTest.o

enlineas.o: enlineas.c
	$(CC) $(CFLAGS) -c enlineas.c -o enlineas.o

longitudes.o: longitudes.c Strings.h
	$(CC) $(CFLAGS) -c longitudes.c -o longitudes.o

mayorlongitud.o: mayorlongitud.c Strings.h
	$(CC) $(CFLAGS) -c mayorlongitud.c -o mayorlongitud.o

todosiguales.o: todosiguales.c Strings.h
	$(CC) $(CFLAGS) -c todosiguales.c -o todosiguales.o

suma.o: suma.c Strings.h Conversion.h
	$(CC) $(CFLAGS) -c suma.c -o suma.o

# ====================== COMANDOS ======================

test: test_strings test_conversion
	./test_strings
	./test_conversion

clean:
	rm -f *.o *.exe test_strings test_conversion enlineas longitudes mayorlongitud todosiguales suma