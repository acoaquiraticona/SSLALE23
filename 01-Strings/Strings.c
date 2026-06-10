#include "Strings.h"

bool IsEmpty (const char *s) {
    if(*s == '\0'){
        return true;
    } else {
        return false;
    }
}

size_t GetLength (const char *s){
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
    int length = (int)GetLength(s);
    int start = 0;
    int end = length - 1;

  if (IsEmpty(s)) {
        return true; 
    }
    while (start < end) {
        if (s[start] != s[end]) {
            return false; 
        }
        start++;
        end--;
    }
    return true;
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