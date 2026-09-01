CC = gcc
CFLAGS = -std=c23 -Wall -Wextra -g
CPPFLAGS = -I. -ISumadorX2/SumadorXAux -I/SumadorX2/Poketests

BIN_DIR := bin
OBJ_DIR := obj

.PHONY: all clean testPokemon
# ==================== REGLAS ====================
all: $(BIN_DIR)/SumarBytes \
     $(BIN_DIR)/SumarInts \
     $(BIN_DIR)/SumarInt_Ns \
     $(BIN_DIR)/SumarDecimalesScanf \
     $(BIN_DIR)/SumarDecimalGetNextDecimal \
     $(BIN_DIR)/SumarX 

testPokemon: $(BIN_DIR)/TestPokeAdder \
			 $(BIN_DIR)/TestPokeParser \
			 $(BIN_DIR)/TestPokeIntegracion \
			 $(BIN_DIR)/TestPokeArithmetic
	./$(BIN_DIR)/TestPokeAdder
	./$(BIN_DIR)/TestPokeParser
	./$(BIN_DIR)/TestPokeIntegracion
	./$(BIN_DIR)/TestPokeArithmetic
$(OBJ_DIR):
	mkdir -p $(OBJ_DIR)

$(BIN_DIR):
	mkdir -p $(BIN_DIR)


clean:
	rm -rf $(OBJ_DIR) $(BIN_DIR) *.o *.exe

# ==================== COMPILACIÓN DE OBJETOS ====================
$(OBJ_DIR)/SumarBytes.o: SumarBytes.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c SumarBytes.c -o $(OBJ_DIR)/SumarBytes.o

$(OBJ_DIR)/SumarInts.o: SumarInts.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c SumarInts.c -o $(OBJ_DIR)/SumarInts.o

$(OBJ_DIR)/SumarInt_Ns.o: SumarInt_Ns.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c SumarInt_Ns.c -o $(OBJ_DIR)/SumarInt_Ns.o

$(OBJ_DIR)/SumarDecimalesScanf.o: SumarDecimalesScanf.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c SumarDecimalesScanf.c -o $(OBJ_DIR)/SumarDecimalesScanf.o

$(OBJ_DIR)/SumarDecimalGetNextDecimal.o: SumarDecimalGetNextDecimal.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c SumarDecimalGetNextDecimal.c -o $(OBJ_DIR)/SumarDecimalGetNextDecimal.o

$(OBJ_DIR)/PokeParser.o: SumarXAux/PokeParser/PokeParser.c SumarXAux/PokeParser/PokeParser.h SumarXAux/PokeParser/ParserErrors.h SumarXAux/Pokedex/Pokedex.h | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c SumarXAux/PokeParser/PokeParser.c -o $(OBJ_DIR)/PokeParser.o

$(OBJ_DIR)/PokeArithmetic.o: SumarXAux/PokeArithmetic/PokeArithmetic.c SumarXAux/PokeArithmetic/PokeArithmetic.h SumarXAux/Pokedex/Pokedex.h | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c SumarXAux/PokeArithmetic/PokeArithmetic.c -o $(OBJ_DIR)/PokeArithmetic.o

$(OBJ_DIR)/Pokedex.o: SumarXAux/Pokedex/Pokedex.c SumarXAux/Pokedex/Pokedex.h | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c SumarXAux/Pokedex/Pokedex.c -o $(OBJ_DIR)/Pokedex.o

$(OBJ_DIR)/SumarX.o: SumarX.c | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c SumarX.c -o $(OBJ_DIR)/SumarX.o

#==================== COMPILACIÓN DE EJECUTABLES ====================	

$(BIN_DIR)/SumarBytes: $(OBJ_DIR)/SumarBytes.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJ_DIR)/SumarBytes.o -o $(BIN_DIR)/SumarBytes

$(BIN_DIR)/SumarInts: $(OBJ_DIR)/SumarInts.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJ_DIR)/SumarInts.o -o $(BIN_DIR)/SumarInts

$(BIN_DIR)/SumarInt_Ns: $(OBJ_DIR)/SumarInt_Ns.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJ_DIR)/SumarInt_Ns.o -o $(BIN_DIR)/SumarInt_Ns

$(BIN_DIR)/SumarDecimalesScanf: $(OBJ_DIR)/SumarDecimalesScanf.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJ_DIR)/SumarDecimalesScanf.o -o $(BIN_DIR)/SumarDecimalesScanf

$(BIN_DIR)/SumarDecimalGetNextDecimal: $(OBJ_DIR)/SumarDecimalGetNextDecimal.o | $(BIN_DIR)
	$(CC) $(CFLAGS) $(OBJ_DIR)/SumarDecimalGetNextDecimal.o -o $(BIN_DIR)/SumarDecimalGetNextDecimal

$(BIN_DIR)/SumarX: $(OBJ_DIR)/SumarX.o | $(BIN_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(OBJ_DIR)/SumarX.o -o $(BIN_DIR)/SumarX

#==================== COMPILACIÓN DE TESTS ====================
$(BIN_DIR)/TestPokeAdder: $(OBJ_DIR)/TestPokeAdder.o $(OBJ_DIR)/PokeArithmetic.o $(OBJ_DIR)/PokeParser.o $(OBJ_DIR)/Pokedex.o | $(BIN_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(OBJ_DIR)/TestPokeAdder.o $(OBJ_DIR)/PokeArithmetic.o $(OBJ_DIR)/PokeParser.o $(OBJ_DIR)/Pokedex.o -o $(BIN_DIR)/TestPokeAdder

$(BIN_DIR)/TestPokeParser: $(OBJ_DIR)/TestPokeParser.o $(OBJ_DIR)/PokeParser.o $(OBJ_DIR)/Pokedex.o | $(BIN_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(OBJ_DIR)/TestPokeParser.o $(OBJ_DIR)/PokeParser.o $(OBJ_DIR)/Pokedex.o -o $(BIN_DIR)/TestPokeParser

$(BIN_DIR)/TestPokeIntegracion: $(OBJ_DIR)/TestPokeIntegracion.o $(OBJ_DIR)/PokeArithmetic.o $(OBJ_DIR)/PokeParser.o $(OBJ_DIR)/Pokedex.o | $(BIN_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(OBJ_DIR)/TestPokeIntegracion.o $(OBJ_DIR)/PokeArithmetic.o $(OBJ_DIR)/PokeParser.o $(OBJ_DIR)/Pokedex.o -o $(BIN_DIR)/TestPokeIntegracion

$(BIN_DIR)/TestPokeArithmetic: $(OBJ_DIR)/TestPokeArithmetic.o $(OBJ_DIR)/PokeArithmetic.o $(OBJ_DIR)/PokeParser.o $(OBJ_DIR)/Pokedex.o | $(BIN_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) $(OBJ_DIR)/TestPokeArithmetic.o $(OBJ_DIR)/PokeArithmetic.o $(OBJ_DIR)/PokeParser.o $(OBJ_DIR)/Pokedex.o -o $(BIN_DIR)/TestPokeArithmetic

# ==================== COMPILACIÓN DE OBJETOS DE TESTS ====================	

$(OBJ_DIR)/TestPokeAdder.o: Tests/PokeTests/TestPokeAdder.c SumarXAux/PokeArithmetic/PokeArithmetic.h SumarXAux/PokeParser/PokeParser.h SumarXAux/Pokedex/Pokedex.h | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c Tests/PokeTests/TestPokeAdder.c -o $(OBJ_DIR)/TestPokeAdder.o

$(OBJ_DIR)/TestPokeParser.o: Tests/PokeTests/TestPokeParser.c SumarXAux/PokeParser/PokeParser.h SumarXAux/Pokedex/Pokedex.h | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c Tests/PokeTests/TestPokeParser.c -o $(OBJ_DIR)/TestPokeParser.o

$(OBJ_DIR)/TestPokeArithmetic.o: Tests/PokeTests/TestPokeArithmetic.c SumarXAux/PokeArithmetic/PokeArithmetic.h SumarXAux/PokeParser/PokeParser.h SumarXAux/Pokedex/Pokedex.h | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c Tests/PokeTests/TestPokeArithmetic.c -o $(OBJ_DIR)/TestPokeArithmetic.o

$(OBJ_DIR)/TestPokeIntegracion.o: Tests/PokeTests/TestPokeIntegracion.c SumarXAux/PokeArithmetic/PokeArithmetic.h SumarXAux/PokeParser/PokeParser.h SumarXAux/Pokedex/Pokedex.h | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c Tests/PokeTests/TestPokeIntegracion.c -o $(OBJ_DIR)/TestPokeIntegracion.o

