CC = gcc
CFLAGS = -std=c23 -Wall -Wextra -g

all: SumarBytes SumarInts SumarInt_Ns SumarDecimalesScanf SumarDecimalGetNextDecimal

SumarBytes: SumarBytes.o
	$(CC) $(CFLAGS) SumarBytes.o -o SumarBytes

SumarBytes.o: SumarBytes.c
	$(CC) $(CFLAGS) -c SumarBytes.c -o SumarBytes.o

SumarInts: SumarInts.o
	$(CC) $(CFLAGS) SumarInts.o -o SumarInts

SumarInts.o: SumarInts.c
	$(CC) $(CFLAGS) -c SumarInts.c -o SumarInts.o

SumarInt_Ns: SumarInt_Ns.o
	$(CC) $(CFLAGS) SumarInt_Ns.o -o SumarInt_Ns

SumarInt_Ns.o: SumarInt_Ns.c
	$(CC) $(CFLAGS) -c SumarInt_Ns.c -o SumarInt_Ns.o

SumarDecimalesScanf: SumarDecimalesScanf.o
	$(CC) $(CFLGS) SumarDecimalesScanf.o -o SumarDecimalesScanf

SumarDecimalesScanf.o: SumarDecimalesScanf.c
	$(CC) $(CFLAGS) -c SumarDecimalesScanf.c - SumarDecimalesScanf.o

SumarDecimalGetNextDecimal: SumarDecimalGetNextDecimal.o
	$(CC) $(CFLAGS) SumarDecimalGetNextDecimal.o -o SumarDecimalGetNextDecimal

SumarDecimalGetNextDecimal.o: SumarDecimalGetNextDecimal.c
	$(CC) $(CFLAGS) -c SumarDecimalGetNextDecimal.c -o SumarDecimalGetNextDecimal.o


.PHONY: clean
clean: 
	rm -f *.o *.exe