/*
 * Author: James Corder Guy
 * Date: 7/31/18
 *
 * instructions.h
 */

#ifndef GAMEBOY_INSTRUCTIONS_H
#define GAMEBOY_INSTRUCTIONS_H

#include <stddef.h>

#include "typedefs.h"

struct instruction {
    char *assembly;
    byte num_operands;
    void *function;
};

const struct instruction instructions[256] = {
        {"NOP", 0, NULL},   		 // 0x00
        {"LD BC, 0x%04X", 0, NULL},	 // 0x01
        {"LD (BC), A", 0, NULL},     // 0x02
        {"INC BC", 0, NULL},   		 // 0x03
        {"INC B", 0, NULL},			 // 0x04
        {"DEC B", 0, NULL},			 // 0x05
        {"LD B, 0x%02X", 0, NULL},	 // 0x06
        {"RLC A", 0, NULL},			 // 0x07
        {"LD (0x%04X), SP", 0, NULL},// 0x08
        {"ADD HL, BC", 0, NULL},	 // 0x09
        {"NOP", 0, NULL},			 // 0x0a
        {"NOP", 0, NULL},			 // 0x0b
        {"NOP", 0, NULL},			 // 0x0c
        {"NOP", 0, NULL},			 // 0x0d
        {"NOP", 0, NULL},			 // 0x0e
        {"NOP", 0, NULL},			 // 0x0f
        {"NOP", 0, NULL},			 // 0x10
        {"NOP", 0, NULL},			 // 0x11
        {"NOP", 0, NULL},			 // 0x12
        {"NOP", 0, NULL},			 // 0x13
        {"NOP", 0, NULL},			 // 0x14
        {"NOP", 0, NULL},			 // 0x15
        {"NOP", 0, NULL},			 // 0x16
        {"NOP", 0, NULL},			 // 0x17
        {"NOP", 0, NULL},			 // 0x18
        {"NOP", 0, NULL},			 // 0x19
        {"NOP", 0, NULL},			 // 0x1a
        {"NOP", 0, NULL},			 // 0x1b
        {"NOP", 0, NULL},			 // 0x1c
        {"NOP", 0, NULL},			 // 0x1d
        {"NOP", 0, NULL},			 // 0x1e
        {"NOP", 0, NULL},			 // 0x1f
        {"NOP", 0, NULL},			 // 0x20
        {"NOP", 0, NULL},			 // 0x21
        {"NOP", 0, NULL},			 // 0x22
        {"NOP", 0, NULL},			 // 0x23
        {"NOP", 0, NULL},			 // 0x24
        {"NOP", 0, NULL},			 // 0x25
        {"NOP", 0, NULL},			 // 0x26
        {"NOP", 0, NULL},			 // 0x27
        {"NOP", 0, NULL},			 // 0x28
        {"NOP", 0, NULL},			 // 0x29
        {"NOP", 0, NULL},			 // 0x2a
        {"NOP", 0, NULL},			 // 0x2b
        {"NOP", 0, NULL},			 // 0x2c
        {"NOP", 0, NULL},			 // 0x2d
        {"NOP", 0, NULL},			 // 0x2e
        {"NOP", 0, NULL},			 // 0x2f
        {"NOP", 0, NULL},			 // 0x30
        {"NOP", 0, NULL},			 // 0x31
        {"NOP", 0, NULL},			 // 0x32
        {"NOP", 0, NULL},			 // 0x33
        {"NOP", 0, NULL},			 // 0x34
        {"NOP", 0, NULL},			 // 0x35
        {"NOP", 0, NULL},			 // 0x36
        {"NOP", 0, NULL},			 // 0x37
        {"NOP", 0, NULL},			 // 0x38
        {"NOP", 0, NULL},			 // 0x39
        {"NOP", 0, NULL},			 // 0x3a
        {"NOP", 0, NULL},			 // 0x3b
        {"NOP", 0, NULL},			 // 0x3c
        {"NOP", 0, NULL},			 // 0x3d
        {"NOP", 0, NULL},			 // 0x3e
        {"NOP", 0, NULL},			 // 0x3f
        {"NOP", 0, NULL},			 // 0x40
        {"NOP", 0, NULL},			 // 0x41
        {"NOP", 0, NULL},			 // 0x42
        {"NOP", 0, NULL},			 // 0x43
        {"NOP", 0, NULL},			 // 0x44
        {"NOP", 0, NULL},			 // 0x45
        {"NOP", 0, NULL},			 // 0x46
        {"NOP", 0, NULL},			 // 0x47
        {"NOP", 0, NULL},			 // 0x48
        {"NOP", 0, NULL},			 // 0x49
        {"NOP", 0, NULL},			 // 0x4a
        {"NOP", 0, NULL},			 // 0x4b
        {"NOP", 0, NULL},			 // 0x4c
        {"NOP", 0, NULL},			 // 0x4d
        {"NOP", 0, NULL},			 // 0x4e
        {"NOP", 0, NULL},			 // 0x4f
        {"NOP", 0, NULL},			 // 0x50
        {"NOP", 0, NULL},			 // 0x51
        {"NOP", 0, NULL},			 // 0x52
        {"NOP", 0, NULL},			 // 0x53
        {"NOP", 0, NULL},			 // 0x54
        {"NOP", 0, NULL},			 // 0x55
        {"NOP", 0, NULL},			 // 0x56
        {"NOP", 0, NULL},			 // 0x57
        {"NOP", 0, NULL},			 // 0x58
        {"NOP", 0, NULL},			 // 0x59
        {"NOP", 0, NULL},			 // 0x5a
        {"NOP", 0, NULL},			 // 0x5b
        {"NOP", 0, NULL},			 // 0x5c
        {"NOP", 0, NULL},			 // 0x5d
        {"NOP", 0, NULL},			 // 0x5e
        {"NOP", 0, NULL},			 // 0x5f
        {"NOP", 0, NULL},			 // 0x60
        {"NOP", 0, NULL},			 // 0x61
        {"NOP", 0, NULL},			 // 0x62
        {"NOP", 0, NULL},			 // 0x63
        {"NOP", 0, NULL},			 // 0x64
        {"NOP", 0, NULL},			 // 0x65
        {"NOP", 0, NULL},			 // 0x66
        {"NOP", 0, NULL},			 // 0x67
        {"NOP", 0, NULL},			 // 0x68
        {"NOP", 0, NULL},			 // 0x69
        {"NOP", 0, NULL},			 // 0x6a
        {"NOP", 0, NULL},			 // 0x6b
        {"NOP", 0, NULL},			 // 0x6c
        {"NOP", 0, NULL},			 // 0x6d
        {"NOP", 0, NULL},			 // 0x6e
        {"NOP", 0, NULL},			 // 0x6f
        {"NOP", 0, NULL},			 // 0x70
        {"NOP", 0, NULL},			 // 0x71
        {"NOP", 0, NULL},			 // 0x72
        {"NOP", 0, NULL},			 // 0x73
        {"NOP", 0, NULL},			 // 0x74
        {"NOP", 0, NULL},			 // 0x75
        {"NOP", 0, NULL},			 // 0x76
        {"NOP", 0, NULL},			 // 0x77
        {"NOP", 0, NULL},			 // 0x78
        {"NOP", 0, NULL},			 // 0x79
        {"NOP", 0, NULL},			 // 0x7a
        {"NOP", 0, NULL},			 // 0x7b
        {"NOP", 0, NULL},			 // 0x7c
        {"NOP", 0, NULL},			 // 0x7d
        {"NOP", 0, NULL},			 // 0x7e
        {"NOP", 0, NULL},			 // 0x7f
        {"NOP", 0, NULL},			 // 0x80
        {"NOP", 0, NULL},			 // 0x81
        {"NOP", 0, NULL},			 // 0x82
        {"NOP", 0, NULL},			 // 0x83
        {"NOP", 0, NULL},			 // 0x84
        {"NOP", 0, NULL},			 // 0x85
        {"NOP", 0, NULL},			 // 0x86
        {"NOP", 0, NULL},			 // 0x87
        {"NOP", 0, NULL},			 // 0x88
        {"NOP", 0, NULL},			 // 0x89
        {"NOP", 0, NULL},			 // 0x8a
        {"NOP", 0, NULL},			 // 0x8b
        {"NOP", 0, NULL},			 // 0x8c
        {"NOP", 0, NULL},			 // 0x8d
        {"NOP", 0, NULL},			 // 0x8e
        {"NOP", 0, NULL},			 // 0x8f
        {"NOP", 0, NULL},			 // 0x90
        {"NOP", 0, NULL},			 // 0x91
        {"NOP", 0, NULL},			 // 0x92
        {"NOP", 0, NULL},			 // 0x93
        {"NOP", 0, NULL},			 // 0x94
        {"NOP", 0, NULL},			 // 0x95
        {"NOP", 0, NULL},			 // 0x96
        {"NOP", 0, NULL},			 // 0x97
        {"NOP", 0, NULL},			 // 0x98
        {"NOP", 0, NULL},			 // 0x99
        {"NOP", 0, NULL},			 // 0x9a
        {"NOP", 0, NULL},			 // 0x9b
        {"NOP", 0, NULL},			 // 0x9c
        {"NOP", 0, NULL},			 // 0x9d
        {"NOP", 0, NULL},			 // 0x9e
        {"NOP", 0, NULL},			 // 0x9f
        {"NOP", 0, NULL},			 // 0xa0
        {"NOP", 0, NULL},			 // 0xa1
        {"NOP", 0, NULL},			 // 0xa2
        {"NOP", 0, NULL},			 // 0xa3
        {"NOP", 0, NULL},			 // 0xa4
        {"NOP", 0, NULL},			 // 0xa5
        {"NOP", 0, NULL},			 // 0xa6
        {"NOP", 0, NULL},			 // 0xa7
        {"NOP", 0, NULL},			 // 0xa8
        {"NOP", 0, NULL},			 // 0xa9
        {"NOP", 0, NULL},			 // 0xaa
        {"NOP", 0, NULL},			 // 0xab
        {"NOP", 0, NULL},			 // 0xac
        {"NOP", 0, NULL},			 // 0xad
        {"NOP", 0, NULL},			 // 0xae
        {"NOP", 0, NULL},			 // 0xaf
        {"NOP", 0, NULL},			 // 0xb0
        {"NOP", 0, NULL},			 // 0xb1
        {"NOP", 0, NULL},			 // 0xb2
        {"NOP", 0, NULL},			 // 0xb3
        {"NOP", 0, NULL},			 // 0xb4
        {"NOP", 0, NULL},			 // 0xb5
        {"NOP", 0, NULL},			 // 0xb6
        {"NOP", 0, NULL},			 // 0xb7
        {"NOP", 0, NULL},			 // 0xb8
        {"NOP", 0, NULL},			 // 0xb9
        {"NOP", 0, NULL},			 // 0xba
        {"NOP", 0, NULL},			 // 0xbb
        {"NOP", 0, NULL},			 // 0xbc
        {"NOP", 0, NULL},			 // 0xbd
        {"NOP", 0, NULL},			 // 0xbe
        {"NOP", 0, NULL},			 // 0xbf
        {"NOP", 0, NULL},			 // 0xc0
        {"NOP", 0, NULL},			 // 0xc1
        {"NOP", 0, NULL},			 // 0xc2
        {"NOP", 0, NULL},			 // 0xc3
        {"NOP", 0, NULL},			 // 0xc4
        {"NOP", 0, NULL},			 // 0xc5
        {"NOP", 0, NULL},			 // 0xc6
        {"NOP", 0, NULL},			 // 0xc7
        {"NOP", 0, NULL},			 // 0xc8
        {"NOP", 0, NULL},			 // 0xc9
        {"NOP", 0, NULL},			 // 0xca
        {"NOP", 0, NULL},			 // 0xcb
        {"NOP", 0, NULL},			 // 0xcc
        {"NOP", 0, NULL},			 // 0xcd
        {"NOP", 0, NULL},			 // 0xce
        {"NOP", 0, NULL},			 // 0xcf
        {"NOP", 0, NULL},			 // 0xd0
        {"NOP", 0, NULL},			 // 0xd1
        {"NOP", 0, NULL},			 // 0xd2
        {"NOP", 0, NULL},			 // 0xd3
        {"NOP", 0, NULL},			 // 0xd4
        {"NOP", 0, NULL},			 // 0xd5
        {"NOP", 0, NULL},			 // 0xd6
        {"NOP", 0, NULL},			 // 0xd7
        {"NOP", 0, NULL},			 // 0xd8
        {"NOP", 0, NULL},			 // 0xd9
        {"NOP", 0, NULL},			 // 0xda
        {"NOP", 0, NULL},			 // 0xdb
        {"NOP", 0, NULL},			 // 0xdc
        {"NOP", 0, NULL},			 // 0xdd
        {"NOP", 0, NULL},			 // 0xde
        {"NOP", 0, NULL},			 // 0xdf
        {"NOP", 0, NULL},			 // 0xe0
        {"NOP", 0, NULL},			 // 0xe1
        {"NOP", 0, NULL},			 // 0xe2
        {"NOP", 0, NULL},			 // 0xe3
        {"NOP", 0, NULL},			 // 0xe4
        {"NOP", 0, NULL},			 // 0xe5
        {"NOP", 0, NULL},			 // 0xe6
        {"NOP", 0, NULL},			 // 0xe7
        {"NOP", 0, NULL},			 // 0xe8
        {"NOP", 0, NULL},			 // 0xe9
        {"NOP", 0, NULL},			 // 0xea
        {"NOP", 0, NULL},			 // 0xeb
        {"NOP", 0, NULL},			 // 0xec
        {"NOP", 0, NULL},			 // 0xed
        {"NOP", 0, NULL},			 // 0xee
        {"NOP", 0, NULL},			 // 0xef
        {"NOP", 0, NULL},			 // 0xf0
        {"NOP", 0, NULL},			 // 0xf1
        {"NOP", 0, NULL},			 // 0xf2
        {"NOP", 0, NULL},			 // 0xf3
        {"NOP", 0, NULL},			 // 0xf4
        {"NOP", 0, NULL},			 // 0xf5
        {"NOP", 0, NULL},			 // 0xf6
        {"NOP", 0, NULL},			 // 0xf7
        {"NOP", 0, NULL},			 // 0xf8
        {"NOP", 0, NULL},			 // 0xf9
        {"NOP", 0, NULL},			 // 0xfa
        {"NOP", 0, NULL},			 // 0xfb
        {"NOP", 0, NULL},			 // 0xfc
        {"NOP", 0, NULL},			 // 0xfd
        {"NOP", 0, NULL},			 // 0xfe
        {"NOP", 0, NULL}		     // 0xff
};

#endif //GAMEBOY_INSTRUCTIONS_H