#include <assert.h>
#include <stdio.h>
#include "Strings.h"

void testIsEmpty (void) {
    assert(IsEmpty("") == true);
    assert(IsEmpty("ssl") == false);
    assert(IsEmpty(" ") == false);
}

void testGetLength (void) {
    assert(GetLength("ssl") == 3);
    assert(GetLength("") == 0);
    assert(GetLength(" ") == 1);
}

void testAreEqual (void) {
    assert(AreEqual("ssl","utn") == false);
    assert(AreEqual("ss","utnss") == false);
    assert(AreEqual("utn","utn") == true);
    assert(AreEqual(" ","a") == false);
}

int main(void){
    testIsEmpty();
    printf("Pruebas de IsEmpty: OK\n");
    testGetLength();
    printf("Pruebas de Getlength: OK\n");
    testAreEqual();
    printf("Pruebas de AreEqual: OK\n");
    return 0;
}