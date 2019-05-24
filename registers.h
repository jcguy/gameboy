/*
 * Author: James Corder Guy
 * Date: 7/31/18
 *
 * registers.h
 */

#ifndef GAMEBOY_REGISTERS_H
#define GAMEBOY_REGISTERS_H

#include <stdint.h>
#include <stdbool.h>

#include "typedefs.h"

struct {
    // AF
    union {
        struct {
            byte f;
            byte a;
        };
        word af;
    };
    // BC
    union {
        struct {
            byte c;
            byte b;
        };
        word bc;
    };
    // DE
    union {
        struct {
            byte e;
            byte d;
        };
        word de;
    };
    // HL
    union {
        struct {
            byte l;
            byte h;
        };
        word hl;
    };
    word pc;
    word sp;

    bool fz;
    bool fn;
    bool fh;
    bool fc;
} registers;
#endif //GAMEBOY_REGISTERS_H
