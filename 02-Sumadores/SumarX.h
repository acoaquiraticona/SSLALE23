#ifndef SUMAR_X_H
#define SUMAR_X_H

#include <stdio.h>
#include <stdbool.h>

#define MAX_DIGITS 64
#define BASE 151

typedef struct {
    int digits[MAX_DIGITS];
    int size;
} PokemonNumber;

void number_init(PokemonNumber *n);
void number_add(PokemonNumber *n, int value);
bool GetNextX(FILE *s, int *p);

#endif // SUMAR_X_H