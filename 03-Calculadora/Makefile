# @file Makefile
# @brief Compilación por módulos y ejecución de las cinco suites de pruebas.
#
# Las reglas activas están después de endif. El bloque ifeq (0,1) conserva
# la configuración histórica y no se evalúa como parte de la construcción.
# Cada archivo fuente genera su objeto en .build; el enlace reúne esos objetos
# en pokecalc. Las opciones pueden reemplazarse desde la línea de comandos.
#
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
# La prueba de capacidad consulta al compilador sin generar un ejecutable:
# selecciona C23 cuando está disponible y, si no, el nombre previo c2x.
# GCC anterior a 14 llama c2x al modo del borrador C23. No se usan extensiones.
CSTD := $(shell printf 'int main(void){return 0;}\n' | $(CC) -std=c23 -x c -fsyntax-only - >/dev/null 2>&1 && printf '%s' -std=c23 || printf '%s' -std=c2x)
CFLAGS ?= $(CSTD) -Wall -Wextra -Wpedantic
CPPFLAGS ?=
LDFLAGS ?=
LDLIBS ?=

EXECUTABLE := pokecalc
# SOURCES determina los objetos mediante sustitución de sufijo. TESTS enumera
# los ejecutables de prueba que necesita el objetivo test, no los del programa.
SOURCES := main.c Parser.c Scanner.c Semantics.c errors.c error.c
OBJECTS := $(SOURCES:%.c=.build/%.o)
TESTS := .build/semantics_test .build/scanner_test .build/errors_test .build/parser_test .build/acceptance_test

.PHONY: all test clean
# Objetivo por defecto: deja preparado el intérprete con sus módulos enlazados.
all: $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $^ $(LDLIBS) -o $@

.build:
	mkdir -p $@

# La dependencia de orden crea .build antes de compilar sin forzar recompilación
# al cambiar la fecha del directorio. -MMD -MP genera dependencias de cabeceras;
# $< es el fuente de esta regla y $@ el objeto que se está construyendo.
.build/%.o: %.c | .build
	$(CC) $(CPPFLAGS) -I. $(CFLAGS) -MMD -MP -c $< -o $@

# Cada suite enlaza solo lo que necesita. parser_test aporta un scanner simulado;
# acceptance_test ejecuta pokecalc como proceso externo. -UNDEBUG mantiene activos
# los assert incluso cuando las opciones generales definan NDEBUG.
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

# Primero se construyen programa y pruebas; las suites se ejecutan en secuencia
# porque comparten archivos temporales. Un estado distinto de cero detiene make.
test: $(EXECUTABLE) $(TESTS)
	./.build/semantics_test
	./.build/scanner_test
	./.build/errors_test
	./.build/parser_test
	./.build/acceptance_test

# Elimina los artefactos actuales y los nombres heredados de compilaciones previas.
clean:
	$(RM) -r .build $(EXECUTABLE) $(EXECUTABLE).exe calc calc.exe main.o Parser.o Scanner.o Semantics.o errors.o error.o

# Las dependencias generadas permiten reconstruir un objeto al cambiar una
# cabecera. El prefijo '-' permite la primera compilación, cuando aún no existen.
-include $(OBJECTS:.o=.d)
