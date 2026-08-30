CC = gcc
CFLAGS = -std=c23 -Wall -Wextra -g
CPPFLAGS = -I. -ISumarXAux/PokeParser -ISumarXAux/Pokedex -ISumarXAux/PokeArithmetic
VPATH = . SumarXAux/PokeParser SumarXAux/Pokedex SumarXAux/PokeArithmetic

BIN_DIR := bin
OBJ_DIR := obj

all: $(BIN_DIR)/SumarBytes \
     $(BIN_DIR)/SumarInts \
     $(BIN_DIR)/SumarInt_Ns \
     $(BIN_DIR)/SumarDecimalesScanf \
     $(BIN_DIR)/SumarDecimalGetNextDecimal \
     $(BIN_DIR)/SumarX

$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)

$(OBJ_DIR)/%.o: %.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BIN_DIR)/SumarBytes: $(OBJ_DIR)/SumarBytes.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/SumarInts: $(OBJ_DIR)/SumarInts.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/SumarInt_Ns: $(OBJ_DIR)/SumarInt_Ns.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/SumarDecimalesScanf: $(OBJ_DIR)/SumarDecimalesScanf.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/SumarDecimalGetNextDecimal: $(OBJ_DIR)/SumarDecimalGetNextDecimal.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $^ -o $@

$(BIN_DIR)/SumarX: $(OBJ_DIR)/SumarX.o $(OBJ_DIR)/PokeArithmetic.o $(OBJ_DIR)/PokeParser.o $(OBJ_DIR)/Pokedex.o | $(BIN_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $^ -o $@

$(OBJ_DIR)/PokeParser.o: PokeParser.c PokeParser.h ParserErrors.h Pokedex.h | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/PokeArithmetic.o: PokeArithmetic.c PokeArithmetic.h Pokedex.h | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/Pokedex.o: Pokedex.c Pokedex.h | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(OBJ_DIR)/SumarX.o: SumarX.c PokeArithmetic.h PokeParser.h Pokedex.h | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR)