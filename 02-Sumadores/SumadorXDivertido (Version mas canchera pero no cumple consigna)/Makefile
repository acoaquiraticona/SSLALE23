CC := gcc
CFLAGS := -std=c11 -Wall -Wextra -pedantic
CPPFLAGS := -I. -I./SumarXAux -I./SumarXAux/Pokedex -I./SumarXAux/PokeArithmetic -I./SumarXAux/PokeParser
BIN_DIR := bin

OBJECTS := \
	$(BIN_DIR)/Pokedex.o \
	$(BIN_DIR)/PokeArithmetic.o \
	$(BIN_DIR)/PokeParser.o

TESTS := \
	$(BIN_DIR)/TestPokeAdder \
	$(BIN_DIR)/TestPokeArithmetic \
	$(BIN_DIR)/TestPokeIntegracion \
	$(BIN_DIR)/TestPokeParser

.PHONY: all clean test

all: $(TESTS)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(BIN_DIR)/Pokedex.o: SumarXAux/Pokedex/Pokedex.c SumarXAux/Pokedex/Pokedex.h | $(BIN_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/PokeArithmetic.o: SumarXAux/PokeArithmetic/PokeArithmetic.c SumarXAux/PokeArithmetic/PokeArithmetic.h | $(BIN_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/PokeParser.o: SumarXAux/PokeParser/PokeParser.c SumarXAux/PokeParser/PokeParser.h SumarXAux/PokeParser/ParserErrors.h | $(BIN_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/TestPokeAdder: PokeTests/TestPokeAdder.c $(BIN_DIR)/Pokedex.o $(BIN_DIR)/PokeArithmetic.o | $(BIN_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(BIN_DIR)/Pokedex.o $(BIN_DIR)/PokeArithmetic.o -o $@

$(BIN_DIR)/TestPokeArithmetic: PokeTests/TestPokeArithmetic.c $(BIN_DIR)/Pokedex.o $(BIN_DIR)/PokeArithmetic.o | $(BIN_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(BIN_DIR)/Pokedex.o $(BIN_DIR)/PokeArithmetic.o -o $@

$(BIN_DIR)/TestPokeIntegracion: PokeTests/TestPokeIntegracion.c $(BIN_DIR)/Pokedex.o $(BIN_DIR)/PokeArithmetic.o $(BIN_DIR)/PokeParser.o | $(BIN_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(BIN_DIR)/Pokedex.o $(BIN_DIR)/PokeArithmetic.o $(BIN_DIR)/PokeParser.o -o $@

$(BIN_DIR)/TestPokeParser: PokeTests/TestPokeParser.c $(BIN_DIR)/Pokedex.o $(BIN_DIR)/PokeParser.o | $(BIN_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $< $(BIN_DIR)/Pokedex.o $(BIN_DIR)/PokeParser.o -o $@

test: all
	@for t in $(TESTS); do echo "== $$t =="; $$t || exit 1; done

clean:
	rm -rf $(BIN_DIR)
