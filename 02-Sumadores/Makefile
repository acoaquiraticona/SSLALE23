CC = gcc
CFLAGS = -std=c23 -Wall -Wextra -g

SumarBytes: SumarBytes.o
	$(CC) $(CFLAGS) SumarBytes.o -o SumarBytes

SumarBytes.o: SumarBytes.c
	$(CC) $(CFLAGS) -c SumarBytes.c -o SumarBytes.o

SumarInts: SumarInts.o
	$(CC) $(CFLAGS) SumarInts.o -o SumarInts

SumarInts.o: SumarInts.c
	$(CC) $(CFLAGS) -c SumarInts.c -o SumarInts.o

SumarDecimalesScanf: SumarDecimalesScanf.o
	$(CC) $(CFLGS) SumarDecimalesScanf.o -o SumarDecimalesScanf

SumarDecimalesScanf.o: SumarDecimalesScanf.c
	$(CC) $(CFLAGS) -c SumarDecimalesScanf.c - SumarDecimalesScanf.o


.PHONY: clean
clean: 
	rm -f *.o *.exe