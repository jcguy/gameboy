#ifndef __SOUND_H__
#define __SOUND_H__

extern int SoundCycle;
extern int SoundLoader;
extern int SoundEnabled;

extern void ProcessSound(int cycles);
extern void SoundWrite(unsigned char reg, unsigned char val);
extern unsigned char SoundRead(int reg);

#endif
