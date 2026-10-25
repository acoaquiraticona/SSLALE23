# PROBLEMA: faltaban errors, pruebas, dependencias y el ejecutable pokecalc.
# Makefile recibido conservado sin cambios; version corregida debajo.
ifeq (0,1)
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
endif

CC ?= cc
# GCC anterior a 14 llama c2x al modo del borrador C23. No se usan extensiones.
CSTD := $(shell printf 'int main(void){return 0;}\n' | $(CC) -std=c23 -x c -fsyntax-only - >/dev/null 2>&1 && printf '%s' -std=c23 || printf '%s' -std=c2x)
CFLAGS ?= $(CSTD) -Wall -Wextra -Wpedantic
CPPFLAGS ?=
LDFLAGS ?=
LDLIBS ?=

EXECUTABLE := pokecalc
SOURCES := main.c Parser.c Scanner.c Semantics.c errors.c error.c
OBJECTS := $(SOURCES:%.c=.build/%.o)
TESTS := .build/semantics_test .build/scanner_test .build/errors_test .build/parser_test .build/acceptance_test

.PHONY: all test clean
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

.build:
	mkdir -p $@

.build/%.o: %.c | .build
	$(CC) $(CPPFLAGS) -I. $(CFLAGS) -MMD -MP -c $< -o $@

.build/semantics_test: tests/semantics_test.c tests/test_support.h .build/Semantics.o .build/errors.o $(wildcard *.h)
	$(CC) $(CPPFLAGS) -I. $(CFLAGS) -UNDEBUG $(LDFLAGS) $< .build/Semantics.o .build/errors.o $(LDLIBS) -o $@

.build/scanner_test: tests/scanner_test.c tests/test_support.h .build/Scanner.o .build/errors.o $(wildcard *.h)
	$(CC) $(CPPFLAGS) -I. $(CFLAGS) -UNDEBUG $(LDFLAGS) $< .build/Scanner.o .build/errors.o $(LDLIBS) -o $@

.build/errors_test: tests/errors_test.c tests/test_support.h .build/errors.o errors.h | .build
	$(CC) $(CPPFLAGS) -I. $(CFLAGS) -UNDEBUG $(LDFLAGS) $< .build/errors.o $(LDLIBS) -o $@

.build/acceptance_test: tests/acceptance_test.c tests/casos.h tests/test_support.h | .build
	$(CC) $(CPPFLAGS) -I. $(CFLAGS) -UNDEBUG $(LDFLAGS) $< $(LDLIBS) -o $@

.build/parser_test: tests/parser_test.c .build/Parser.o .build/Semantics.o .build/errors.o tests/test_support.h $(wildcard *.h)
	$(CC) $(CPPFLAGS) -I. $(CFLAGS) -UNDEBUG $(LDFLAGS) $< .build/Parser.o .build/Semantics.o .build/errors.o $(LDLIBS) -o $@

test: $(EXECUTABLE) $(TESTS)
	./.build/semantics_test
	./.build/scanner_test
	./.build/errors_test
	./.build/parser_test
	./.build/acceptance_test

clean:
	$(RM) -r .build $(EXECUTABLE) $(EXECUTABLE).exe calc calc.exe main.o Parser.o Scanner.o Semantics.o errors.o error.o

-include $(OBJECTS:.o=.d)
