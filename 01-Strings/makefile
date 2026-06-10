SHELL = C:/msys64/usr/bin/bash.exe
CC = gcc
CFLAGS = -std=c23 -Wall

.Phony: all test clean
#Regla de compilacion

test_strings: Strings.o StringsTest.o
	$(CC) $(CFLAGS) Strings.o StringsTest.o -o test_strings	

all: test_strings
#Regla para ejecutar
test: test_strings
	./test_strings
	echo -e "\nPress any key to continue..."
	read -r -t 1 

# limpieza
clean: 
	rm -f test_strings test_strings.exe *.o

Strings.o: Strings.c Strings.h
	$(CC) $(CFLAGS) -c Strings.c -o Strings.o

StringsTest.o: StringsTest.c Strings.h
	$(CC) $(CFLAGS) -c StringsTest.c -o StringsTest.o
