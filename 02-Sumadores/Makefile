CC = gcc
CFLAGS = -std=c23 -Wall -Wextra -g
CPPFLAGS = -I.

BIN_DIR := bin
OBJ_DIR := obj

.PHONY: all clean

# ==================== REGLAS ====================
all: $(BIN_DIR)/SumarBytes \
     $(BIN_DIR)/SumarInts \
     $(BIN_DIR)/SumarInt_Ns \
     $(BIN_DIR)/SumarDecimalesScanf \
     $(BIN_DIR)/SumarDecimalGetNextDecimal \
     $(BIN_DIR)/SumarX

$(OBJ_DIR):
	@if not exist "$(OBJ_DIR)" mkdir "$(OBJ_DIR)"

$(BIN_DIR):
	@if not exist "$(BIN_DIR)" mkdir "$(BIN_DIR)"

clean:
	@if exist "$(OBJ_DIR)" rmdir /s /q "$(OBJ_DIR)"
	@if exist "$(BIN_DIR)" rmdir /s /q "$(BIN_DIR)"
	@del /q /f *.o *.exe 2> NUL

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

