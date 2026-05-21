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

void testAreDecimalDigits (void) {
    assert(AreDecimalDigits("123") == true);
    assert(AreDecimalDigits("123a") == false);
    assert(AreDecimalDigits("") == true);
}

void testContains (void) {
    assert(Contains("ssl", 's') == true);
    assert(Contains("ssl", 'a') == false);
    assert(Contains("", 'a') == false);
}

void testToInteger (void) {
    assert(ToInteger("123") == 123);
    assert(ToInteger("0") == 0);
    assert(ToInteger("") == 0);
}

void testIsPalindrome (void) {
    assert(IsPalindrome("aba") == true);
    assert(IsPalindrome("abc") == false);
    assert(IsPalindrome("") == true);
}

void testConcatenate (void) {
    char *result = Concatenate("hello", "world");
    assert(AreEqual(result, "helloworld") == true);
    free(result);
}

void testPower (void) {
    char *result = Power("a", 3);
    assert(AreEqual(result, "aaa") == true);
    free(result);
}

void testReverse (void) {
    char *result = Reverse("hello");
    assert(AreEqual(result, "olleh") == true);
    free(result);
}

int main(void){
    testIsEmpty();
    printf("Pruebas de IsEmpty: OK\n");
    testGetLength();
    printf("Pruebas de Getlength: OK\n");
    testAreEqual();
    printf("Pruebas de AreEqual: OK\n");
    testAreDecimalDigits();
    printf("Pruebas de AreDecimalDigits: OK\n");
    testContains();
    printf("Pruebas de Contains: OK\n");
    testToInteger();
    printf("Pruebas de ToInteger: OK\n");
    testIsPalindrome();
    printf("Pruebas de IsPalindrome: OK\n");
    testConcatenate();
    printf("Pruebas de Concatenate: OK\n");
    testPower();
    printf("Pruebas de Power: OK\n");
    testReverse();
    printf("Pruebas de Reverse: OK\n");
    return 0;
}