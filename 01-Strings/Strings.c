#include "Strings.h"
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>

bool IsEmpty (const char *s) {
    if(*s == '\0'){
        return true;
    } else {
        return false;
    }
}

unsigned GetLength (const char *s){
    return IsEmpty(s) ? 0 :1 + GetLength(s + 1);
}

bool AreEqual (const char *s1, const char *s2){
    if(IsEmpty(s1) && IsEmpty(s2)){
        return true;
    }else if(*s1 == *s2){
        return AreEqual(s1 + 1, s2 + 1);
    } else {
        return false;
    }

}
bool AreDecimalDigits (const char *s){
    if(IsEmpty(s)){
        return true;
    } else if(*s >= '0' && *s <= '9'){
        return AreDecimalDigits(s + 1);
    } else {
        return false;
    }
}
bool Contains (const char *s, char c){
    if(IsEmpty(s)){
        return false;
    } else if(*s == c){
        return true;
    } else {
        return Contains(s + 1, c);
    }
}
int ToInteger (const char *s){
    if(IsEmpty(s)){
        return 0;
    } else {
        return (*s - '0') * pow(10, GetLength(s) - 1) + ToInteger(s + 1);
    }
}

bool IsPalindrome (const char *s){
    if(IsEmpty(s)){
        return true;
    } else if(*s == *(s + GetLength(s) - 1)){
        char temp[GetLength(s) - 1];
        for(int i = 0; i < GetLength(s) - 1; i++){
            temp[i] = *(s + i + 1);
        }
        temp[GetLength(s) - 1] = '\0';
        return IsPalindrome(temp);
    } else {
        return false;
    }
}
char *Concatenate (const char *s1, const char *s2){
    char *result = malloc(GetLength(s1) + GetLength(s2) + 1);
    for(int i = 0; i < GetLength(s1); i++){
        result[i] = s1[i];
    }
    for(int j = 0; j < GetLength(s2); j++){
        result[GetLength(s1) + j] = s2[j];
    }
    result[GetLength(s1) + GetLength(s2)] = '\0';
    return result;
}
char *Power (const char *s, unsigned n){
    char *result = malloc(GetLength(s) * n + 1);
    for(int i = 0; i < n; i++){
        for(int j = 0; j < GetLength(s); j++){
            result[i * GetLength(s) + j] = s[j];
        }
    }
    result[GetLength(s) * n] = '\0';
    return result;
}

char *Reverse (const char *s){
    char *result = malloc(GetLength(s) + 1);
    for(int i = 0; i < GetLength(s); i++){
        result[i] = s[GetLength(s) - 1 - i];
    }
    result[GetLength(s)] = '\0';
    return result;
}