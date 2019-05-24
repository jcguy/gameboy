CC = pgcc -V 2.95.2 -warpup
LFLAGS =
CFLAGS = -Wall -O6 -ffast-math -fomit-frame-pointer -finline-functions -DMSB_FIRST
OBJS = rom.o mem.o cpu.o vram.o joypad.o sound.o amigavid.o amigaaud.o
BIN = gbe

all: $(BIN)

$(BIN): $(OBJS)
	$(CC) $(CFLAGS) -o $(BIN) $(OBJS) $(LFLAGS)
	elf2exe2 $(BIN) $(BIN)

clean:
	rm -f $(BIN) $(OBJS)

