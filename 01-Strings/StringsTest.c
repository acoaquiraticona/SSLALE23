#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
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
    assert(AreEqual(Concatenate("hello", "world"), "helloworld") == true);
}

void testPower (void) {
    assert(AreEqual(Power("a", 3), "aaa") == true);
}

void testReverse (void) {
    assert(AreEqual(Reverse("hello"), "olleh") == true);
}

int main(void){
    testIsEmpty();
    testGetLength();
    testAreEqual();
    testAreDecimalDigits();
    testContains();
    testToInteger();
    testIsPalindrome();
    testConcatenate();
    testPower();
    testReverse();
    return 0;
}
