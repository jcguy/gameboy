/*
 *  gbe - gameboy emulator
 *  Copyright (C) 1999  Chuck Mason, Steven Fuller
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
 *
 *
 *  Chuck Mason <chuckjr@sinclair.net>
 *  Steven Fuller <relnev@atdot.org>
 */

#ifndef __MEM_H__
#define __MEM_H__

extern unsigned char *gameboy_memory[16];
extern unsigned char hiram[0x1000];
extern unsigned char *video_ram;
extern unsigned char *sprite_oam;
extern unsigned short int bkg_palettes[8][4];
extern unsigned short int obj_palettes[8][4];

void install_memory(int, void *);
int  initialize_memory();
void free_memory();
unsigned char memory_read_byte(unsigned int);
unsigned short int memory_read_word(unsigned int);

void memory_write_byte(unsigned int, unsigned char);
void memory_write_word(unsigned int, unsigned short int);

extern int memory_read_hibyte(int reg);
extern void memory_write_hibyte(int reg, int val);

#endif

