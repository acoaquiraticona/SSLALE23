#include <assert.h>
#include "Conversion.h"

static void testToInteger(void) {
    assert(ToInteger("") == 0);
    assert(ToInteger("0") == 0);
    assert(ToInteger("123") == 123);
    assert(ToInteger("00123") == 123);
    assert(ToInteger("999") == 999);
    assert(ToInteger("42") == 42);
}

int main(void) {
    testToInteger();
    return 0;
}