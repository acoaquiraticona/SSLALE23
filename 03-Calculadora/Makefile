CC ?= cc
CFLAGS ?= -std=c23 -Wall -Wextra -Wpedantic

EXECUTABLE = calc
OBJECTS = main.o Parser.o Scanner.o Semantics.o

.PHONY: all clean

all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $^ -o $@

main.o: main.c Parser.h Semantics.h
	$(CC) $(CFLAGS) -c $< -o $@

Parser.o: Parser.c Parser.h Scanner.h 
	$(CC) $(CFLAGS) -c $< -o $@

Scanner.o: Scanner.c Scanner.h
	$(CC) $(CFLAGS) -c $< -o $@

Semantics.o:Semantics.c Semantics.h
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJECTS) $(EXECUTABLE)	