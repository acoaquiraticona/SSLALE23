CC = gcc
CFLAGS = -std=c23 -Wall

#Regla de compilacion
test_strings: Strings.c StringsTest.c
	$(CC) $(CFLAGS) Strings.c StringsTest.c -o test_strings	

#Regla para ejecutar
test: test_strings
	./test_strings.exe

# limpieza
clean: 
	rm -f test_strings test_strings.exe

