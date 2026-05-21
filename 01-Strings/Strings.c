#include "Strings.h"

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