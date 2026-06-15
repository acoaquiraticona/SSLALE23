#include <assert.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "Strings.h"

/* Funciones de prueba */

static void testIsEmpty(void) {
    assert(IsEmpty("") == true);
    assert(IsEmpty("ssl") == false);
    assert(IsEmpty(" ") == false);
}

static void testGetLength(void) {
    assert(GetLength("ssl") == 3);
    assert(GetLength("") == 0);
    assert(GetLength(" ") == 1);
    assert(GetLength("hello world") == 11);
}

static void testAreEqual (void) {
    assert(AreEqual("", "") == true);
    assert(AreEqual("utn", "utn") == true);

    assert(AreEqual("ssl", "utn") == false);
    assert(AreEqual("ss", "utnss") == false);
    assert(AreEqual("a", "A") == false);
    assert(AreEqual("", "a") == false);
    assert(AreEqual("", " ") == false);
}

static void testAreDecimalDigits(void) {
    assert(AreDecimalDigits("") == true);
    assert(AreDecimalDigits("123") == true);
    assert(AreDecimalDigits("00123") == true);
    assert(AreDecimalDigits("123a") == false);
    assert(AreDecimalDigits("123.45") == false);
    assert(AreDecimalDigits("12 34") == false);
}

static void testContains(void) {
    assert(Contains("ssl", 's') == true);
    assert(Contains("ssl", 'l') == true);
    assert(Contains("ssl", 'a') == false);
    assert(Contains("", 'a') == false);
    assert(Contains("abc", '\0') == false);
}

static void testIsPalindrome(void) {
    assert(IsPalindrome("") == true);
    assert(IsPalindrome(" ") == true);
    assert(IsPalindrome("a") == true);
    assert(IsPalindrome("aba") == true);
    assert(IsPalindrome("abba") == true);
    assert(IsPalindrome("abcba") == true);
    assert(IsPalindrome("neuquen") == true);

    assert(IsPalindrome("abc") == false);
    assert(IsPalindrome("abca") == false);
    assert(IsPalindrome("ab") == false);
}

static void testConcatenate(void) {
    char *result; // Por la naturaleza de Concatenate, necesito si o si una variable

    result = Concatenate("hello", "world"); // Esto me causa un memory leak si después no lo libero
    assert(AreEqual(result, "helloworld") == true);
    free(result); // Acá tenes tu free

    result = Concatenate("", "hola");
    assert(AreEqual(result, "hola") == true);
    free(result);

    result = Concatenate("hola", "");
    assert(AreEqual(result, "hola") == true);
    free(result);

    result = Concatenate("", "");
    assert(AreEqual(result, "") == true);
    free(result);

    result = Concatenate("", " ");
    assert(AreEqual(result, " ") == true);
    free(result);
}

static void testPower(void) {
    char *result; // Idem caso anterior, no safamos de esta

    result = Power("a", 3);
    assert(AreEqual(result, "aaa") == true);
    free(result);

    result = Power("xy", 0);
    assert(AreEqual(result, "") == true);
    free(result);

    result = Power("", 5);
    assert(AreEqual(result, "") == true);
    free(result);

    result = Power("ab", 1);
    assert(AreEqual(result, "ab") == true);
    free(result);

    result = Power("ji", 3);
    assert(AreEqual(result, "jijiji") == true); // No lo soñie
    free(result);
}

static void testReverse(void) {
    char *result; // Idem caso anterior, no safamos mas de esta.
    result = Reverse("hello");
    assert(AreEqual(result, "olleh") == true);
    free(result);

    result = Reverse("");
    assert(AreEqual(result, "") == true);
    free(result);

    result = Reverse(" ");
    assert(AreEqual(result, " ") == true);
    free(result);

    result = Reverse("a");
    assert(AreEqual(result, "a") == true);
    free(result);

    result = Reverse("ab");
    assert(AreEqual(result, "ba") == true);
    free(result);

    result = Reverse("abc");
    assert(AreEqual(result, "cba") == true);
    free(result);
}

/* Main para probar los tests */

int main(void) {
    testIsEmpty();
    testGetLength();
    testAreEqual();
    testAreDecimalDigits();
    testContains();
    testIsPalindrome();
    testConcatenate();
    testPower();
    testReverse();

    return 0;
}
