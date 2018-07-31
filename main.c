#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "typedefs.h"
#include "registers.h"
#include "instructions.h"


void print_instruction(byte const *opcode) {
    printf("0x%02X\t", *opcode);

    switch (instructions[*opcode].operand_size) {
        case 0:
            printf("%s", instructions[*opcode].assembly);
            break;
        case 1:
            printf(instructions[*opcode].assembly, *(opcode + 1));
            opcode += 1;
        case 2:
            printf(instructions[*opcode].assembly, *((word*) (opcode + 1)));
            opcode += 2;
        default:
            break;
    }

    printf("\n");
}


byte *read_cart(char *filename) {
    FILE *fp = fopen(filename, "rb");
    if (!fp) {
        perror("Failed to open ROM");
        return NULL;
    }

    fseek(fp, 0, SEEK_END);
    size_t file_length = (size_t) ftell(fp);
    rewind(fp);

    byte *cart = malloc(sizeof(byte) * file_length);
    fread(cart, file_length, 1, fp);
    fclose(fp);
    return cart;
}


int main() {
    byte *cart = read_cart("../tetris.gb");

    registers.pc = 0x0100;
    for (;;) {
        byte *opcode = &cart[registers.pc];
        if (instructions[*opcode].function) {
            instructions[*opcode].function(opcode);
        } else {
            printf("Unimplemented function at: 0x%04X:\t", registers.pc);
            print_instruction(opcode);
            return EXIT_FAILURE;
        }
    }
}