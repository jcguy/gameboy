#include <stdio.h>
#include <stdint.h>

#include "typedefs.h"
#include "registers.h"


int main() {
    registers.h = 0x13;
    registers.l = 0x37;
    printf("%x", registers.hl);
    return 0;
}