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

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <unistd.h>
#include <signal.h>

#include <exec/types.h>
#include <exec/nodes.h>
#include <exec/lists.h>
#include <exec/memory.h>

#include <powerup/ppcproto/intuition.h>
#include <powerup/ppclib/interface.h>
#include <powerup/ppclib/time.h>
#include <powerup/gcclib/powerup_protos.h>
#include <powerup/ppcproto/exec.h>
#include <powerup/ppcinline/graphics.h>
#include <powerup/ppcproto/dos.h>
#include <powerup/ppcproto/lowlevel.h>
#include <powerup/ppcproto/asl.h>

#include <cybergraphx/cybergraphics.h>
#include <powerup/ppcinline/cybergraphics.h>

#include "gbe.h"

#include "joypad.h"
#include "cpu.h"
#include "mem.h"
#include "rom.h"
#include "vram.h"
#include "regs.h"
#include "data.h"

#define AGA  0
#define CGFX 1

/* Misc variables */
ULONG rgbtab[1+3*256+1];
struct IntuitionBase *IntuitionBase;
struct GfxBase *GfxBase;
struct Library *AslBase;
struct Library *CyberGfxBase;
struct Library *LowLevelBase;
struct Screen *screen = NULL;
struct Window *window = NULL;
UWORD *EmptyPointer;
struct ScreenModeRequester *screenreq;
struct BitMap agabitmap;
struct ScreenBuffer *sbuffer[3] = { NULL, NULL, NULL };
int cur_sbuffer = 0, free_sbuffer = 0;

/* CGFX variables */
unsigned long pixfmt;
unsigned char *baseaddress[3] = { NULL, NULL, NULL };
int bpr;

int xoffset, yoffset;
int width = 320, height = 256, screentype;

int use_pad = 0;
int use_cgfx = 0;
int use_req = 0;
int nostalgia = 0;
int use_triple = 0;

static unsigned long gbe_window_width = 0, gbe_window_height = 0;
static char *gfx_buffer = NULL;

char *filename = NULL;

extern void open_audio(void);
extern void close_audio(void);

#define twomerges1(a, b, c, d, shift, mask) \
     { \
     temp1 = b; \
     temp2 = d; \
     temp1 >>= shift; \
     temp2 >>= shift; \
     temp1 ^= a; \
     temp2 ^= c; \
     temp1 &= mask; \
     temp2 &= mask; \
     a ^= temp1; \
     c ^= temp2; \
     temp1 <<= shift; \
     temp2 <<= shift; \
     b ^= temp1; \
     d ^= temp2; \
     }

void c2p(void *chunky, void *bitplanes, int chunkyx, int chunkyy, int xoffset, int yoffset, int bitplanesize)
{
     ULONG     *c;
     ULONG     temp1, temp2;
     ULONG     *p0, *p1, *p2, *p3, *p4, *p5, *p6, *p7;
     ULONG     d0, d1, d2, d3, d4, d5, d6, d7;
     ULONG     t0, t1, t2, t3, t4, t5, t6, t7;
     ULONG     xadd, addx;
     int  i,j=0;

     c = (ULONG *) chunky;
     p0 = (ULONG *) (&(((char *) bitplanes)[(yoffset*chunkyx + yoffset*xoffset*2 + xoffset) >> 3]));
     p1 = (ULONG *) (&(((char *) p0)[bitplanesize]));
     p2 = (ULONG *) (&(((char *) p1)[bitplanesize]));
     p3 = (ULONG *) (&(((char *) p2)[bitplanesize]));
     p4 = (ULONG *) (&(((char *) p3)[bitplanesize]));
     p5 = (ULONG *) (&(((char *) p4)[bitplanesize]));
     p6 = (ULONG *) (&(((char *) p5)[bitplanesize]));
     p7 = (ULONG *) (&(((char *) p6)[bitplanesize]));

     addx=chunkyx / (sizeof (ULONG) *8);
     xadd=xoffset * 2 / (sizeof (ULONG) * 8);
     i = chunkyx * chunkyy / (sizeof (ULONG) * 8);

     d0 = *c++;
     d1 = *c++;
     d2 = *c++;
     d3 = *c++;
     d4 = *c++;
     d5 = *c++;
     d6 = *c++;
     d7 = *c++;

     twomerges1(d0, d4, d1, d5, 16, 0x0000ffff);
     twomerges1(d2, d6, d3, d7, 16, 0x0000ffff);

     twomerges1(d0, d2, d1, d3, 8, 0x00ff00ff);
     twomerges1(d4, d6, d5, d7, 8, 0x00ff00ff);

     twomerges1(d0, d1, d2, d3, 4, 0x0f0f0f0f);
     twomerges1(d4, d5, d6, d7, 4, 0x0f0f0f0f);

     twomerges1(d0, d4, d1, d5, 2, 0x33333333);
     twomerges1(d2, d6, d3, d7, 2, 0x33333333);

     twomerges1(d0, d2, d1, d3, 1, 0x55555555);
     twomerges1(d4, d6, d5, d7, 1, 0x55555555);

     t0 = d7;
     t1 = d5;
     t2 = d3;
     t3 = d1;
     t4 = d6;
     t5 = d4;
     t6 = d2;
     t7 = d0;

     while (--i)
     {
          d0 = *c++;
          d1 = *c++;
          d2 = *c++;
          d3 = *c++;
          d4 = *c++;
          d5 = *c++;
          d6 = *c++;
          d7 = *c++;

          *p0++ = t0;

          twomerges1(d0, d4, d1, d5, 16, 0x0000ffff);
          twomerges1(d2, d6, d3, d7, 16, 0x0000ffff);

          *p1++ = t1;

          twomerges1(d0, d2, d1, d3, 8, 0x00ff00ff);
          twomerges1(d4, d6, d5, d7, 8, 0x00ff00ff);

          *p2++ = t2;

          twomerges1(d0, d1, d2, d3, 4, 0x0f0f0f0f);
          twomerges1(d4, d5, d6, d7, 4, 0x0f0f0f0f);

          *p3++ = t3;

          twomerges1(d0, d4, d1, d5, 2, 0x33333333);
          twomerges1(d2, d6, d3, d7, 2, 0x33333333);

          *p4++ = t4;

          twomerges1(d0, d2, d1, d3, 1, 0x55555555);
          twomerges1(d4, d6, d5, d7, 1, 0x55555555);

          *p5++ = t5;
          *p6++ = t6;
          *p7++ = t7;

          t0 = d7;
          t1 = d5;
          t2 = d3;
          t3 = d1;
          t4 = d6;
          t5 = d4;
          t6 = d2;
          t7 = d0;

          j++;

          if (j==addx) {
            p0+=xadd;
            p1+=xadd;
            p2+=xadd;
            p3+=xadd;
            p4+=xadd;
            p5+=xadd;
            p6+=xadd;
            p7+=xadd;
            j=0;
          }
     }

     *p0++ = t0;
     *p1++ = t1;
     *p2++ = t2;
     *p3++ = t3;
     *p4++ = t4;
     *p5++ = t5;
     *p6++ = t6;
     *p7++ = t7;
}

/* PCX screenshot saver */
void write_pcx(unsigned char *data, int width, int height, char *filename)
{
  int start, stop, x, y, i;
  unsigned char *buffer, *dst;
  char *name, *file;
  FILE *fp;

  /* Allocate some buffer memory */
  buffer = (unsigned char *)malloc(width * height + 256 * 3 + 128);
  memset(buffer, 0, width * height + 256 * 3 + 128);
  dst = buffer + 128;

  /* Get filename */
  stop = strlen(filename) - 1;

  while((stop > 0) && (filename[stop] != '.'))
    stop--;

  if (stop == 0)
    return;

  start = stop;

  while((start > 0) && (filename[start] != '/') && (filename[start] != ':'))
    start--;

  if (start == 0) {
    name = (char *)malloc(stop + 5);
    memset(name, 0, (stop + 5));
    strncpy(name, filename, (stop + 1));
  }
  else {
    name = (char *)malloc(stop - start + 4);
    memset(name, 0, (stop - start + 4));
    for (i = 0; i < (stop - start); i++)
      name[i] = filename[start + i + 1];
  }

  if (nostalgia) {
    strcat(name, "smp");
    file = (char *)malloc(strlen(name) + 19);
    memset(file, 0, (strlen(name) + 19));
    strcpy(file, "Nostalgia:samples/");
    strcat(file, name);
  } else {
    strcat(name, "pcx");
    file = strdup(name);
  }

  /* Header (little endian) */
  buffer[ 0] = 10;  /* PCX file */
  buffer[ 1] = 5;   /* 8bit */
  buffer[ 2] = 1;   /* RLE */
  buffer[ 3] = 8;   /* 8bit */

  buffer[ 9] = (width - 1) / 256;               /* Window max width */
  buffer[ 8] = width - (buffer[ 9] * 256) - 1;  /* Window max width */
  buffer[11] = (height - 1) / 256;              /* Window max height */
  buffer[10] = height - (buffer[11] * 256) - 1; /* Window max height */

  buffer[13] = width / 256;                 /* Width */
  buffer[12] = width - (buffer[13] * 256);  /* Width */
  buffer[15] = height / 256;                /* Height */
  buffer[14] = height - (buffer[15] * 256); /* Height */

  buffer[65] = 1;          /* Colorplanes */
  buffer[67] = buffer[13]; /* Bytes per line */
  buffer[66] = buffer[12]; /* Bytes per line */
  buffer[69] = 0;          /* Color */
  buffer[68] = 1;          /* Color */

  for (y = 0; y < height; y++) {
    unsigned char *src, last, this;
    int total = 0, count = 1;

    /* Init */
    src = data + (y * width);
    last = *(src);

    /* Encode one line */
    for (x = 1; x < width; x++) {
      this = *(++src);
      if (this == last) {
        count++;
        if (count == 63) {
          *dst++ = (count | 0xc0);
          *dst++ = last;
          total += 2;
          count = 0;
        }
      } else {
        if (count) {
          if ((count == 1) && ((last & 0xc0) != 0xc0)) {
            *dst++ = last;
            total++;
          } else {
            *dst++ = (count | 0xc0);
            *dst++ = last;
            total += 2;
          }
        }
        last = this;
        count = 1;
      }
    }

    if (count) {
      if ((count == 1) && ((last & 0xc0) != 0xc0)) {
        *dst++ = last;
      } else {
        *dst++ = (count | 0xc0);
        *dst++ = last;
      }
    }
  }

  *dst++ = 12; /* 8bit palette is to follow */

  for (i=0; i<256; i++) {
    *dst++ = rgbtab[1+i*3+0] >> 24;
    *dst++ = rgbtab[1+i*3+1] >> 24;
    *dst++ = rgbtab[1+i*3+2] >> 24;
  }

  /* Save the file */
  fp = fopen(file, "w");

  if (fp != NULL) {
    fwrite(buffer, 1, (int)(dst-buffer), fp);
    fclose(fp);
  }

  /* Free the buffer */
  free(buffer);
}

int open_libraries()
{
  IntuitionBase = (struct IntuitionBase *)OpenLibrary("intuition.library", 39);
  if (IntuitionBase == NULL) {
    return -1;
  }

  GfxBase = (struct GfxBase *)OpenLibrary("graphics.library", 39);
  if (GfxBase == NULL) {
    CloseLibrary(IntuitionBase);
    return -1;
  }

  AslBase = OpenLibrary("asl.library", 39);
  if (AslBase == NULL) {
    CloseLibrary(GfxBase);
    CloseLibrary(IntuitionBase);
    return -1;
  }

  CyberGfxBase = OpenLibrary("cybergraphics.library", 40);
  LowLevelBase = OpenLibrary("lowlevel.library", 39);

  return 0;
}

void close_libraries()
{
  if (LowLevelBase != NULL)
    CloseLibrary(LowLevelBase);
  if (CyberGfxBase != NULL)
    CloseLibrary(CyberGfxBase);
  CloseLibrary(AslBase);
  CloseLibrary(GfxBase);
  CloseLibrary(IntuitionBase);
}

void close_screen()
{
  if (window != NULL) {
    CloseWindow(window);
  }
  if (use_triple == 1) {
    if (sbuffer[0] != NULL) {
      FreeScreenBuffer(screen, sbuffer[0]);
    }
    if (sbuffer[1] != NULL) {
      FreeScreenBuffer(screen, sbuffer[1]);
    }
    if (sbuffer[2] != NULL) {
      FreeScreenBuffer(screen, sbuffer[2]);
    }
  }
  if (screen != NULL) {
    CloseScreen(screen);
  }
}

int open_screen()
{
  unsigned long dispid;
  unsigned char *temp;
  int i, depth;

  if (use_req == 1) {
    screenreq = AllocAslRequestTags(ASL_ScreenModeRequest, TAG_END);

    if (AslRequest(screenreq, TAG_END)) {
      dispid=screenreq->sm_DisplayID;
      width=screenreq->sm_DisplayWidth;
      FreeAslRequest(screenreq);
    } else {
      FreeAslRequest(screenreq);
      return -1;
    }

    if (width != 320) {
      printf("You must choose a width of 320 pixels\n");
      return -1;
    }

    screentype = AGA;

    if (CyberGfxBase != NULL) {
      if (IsCyberModeID(dispid)) {
        depth = GetCyberIDAttr(CYBRIDATTR_DEPTH, dispid);

        if (depth != 8) {
          printf("You must choose a 8bit screenmode\n");
          return -1;
        } else {
          screentype = CGFX;
        }
      }
    }
  } else {
    if (CyberGfxBase != NULL) {
      if ((dispid=BestCModeIDTags(CYBRBIDTG_NominalWidth, width,
                                  CYBRBIDTG_NominalHeight, height,
                                  CYBRBIDTG_Depth, 8,
                                  TAG_DONE))==INVALID_ID)
      {
        printf("Could NOT get best mode ID!\n");
        return -1;   
      }
      screentype = CGFX;
    } else {
      dispid = 0x00021000; /* PAL Lores */
      screentype = AGA;
    }
  }

  if (screentype == AGA) {
    /* Allocate resources */
    temp = (unsigned char*)AllocRaster(width, height*8);
    memset(temp, 0, width*height);

    InitBitMap(&agabitmap, 8, width, height); 

    for(i=0; i<8; i++)
      agabitmap.Planes[i] = temp + i * RASSIZE(width, height);

    /* Open screen */
    screen = OpenScreenTags(NULL,
             SA_BitMap,(int)&agabitmap,
             SA_Width,width,
             SA_Height,height,
             SA_Depth,8,
             SA_Quiet,TRUE,
             SA_ShowTitle,FALSE,
             SA_Type,CUSTOMSCREEN,
             SA_DisplayID,dispid,
             TAG_DONE);

    /* Could the screen be opened? */
    if(screen == NULL) {
      printf("The screen could NOT be opened!\n");
      return -1;
    }
  } else {
    /* Open screen */
    screen = OpenScreenTags(NULL,
             SA_Width,width,
             SA_Height,height,
             SA_Depth,8,
             SA_Quiet,TRUE,
             SA_ShowTitle,FALSE,
             SA_Type,CUSTOMSCREEN,
             SA_DisplayID,dispid,
             TAG_DONE);

    /* Could the screen be opened? */
    if(screen == NULL) {
      printf("The screen could NOT be opened!\n");
      return -1;
    }

    if (use_triple == 1) {
      sbuffer[0] = AllocScreenBuffer(screen, NULL, SB_SCREEN_BITMAP);
      if (sbuffer[0] == NULL) {
        printf("Screenbuffer 1 allocation failed\n");
        return -1;
      }
      sbuffer[1] = AllocScreenBuffer(screen, NULL, 0);
      if (sbuffer[1] == NULL) {
        printf("Screenbuffer 2 allocation failed\n");
        return -1;
      }
      sbuffer[2] = AllocScreenBuffer(screen, NULL, 0);
      if (sbuffer[2] == NULL) {
        printf("Screenbuffer 3 allocation failed\n");
        return -1;
      }

      for (i=0; i<3; i++) {
        /* Get CGFX screenmode information */
        UnLockBitMap(LockBitMapTags(sbuffer[i]->sb_BitMap,
                                    LBMI_PIXFMT, (ULONG)&pixfmt,
                                    LBMI_BASEADDRESS, (ULONG)&baseaddress[i],
                                    LBMI_BYTESPERROW, (ULONG)&bpr,
                                    TAG_END));

        memset(baseaddress[i], 0, (height*bpr));
      }
    } else {
      /* Get CGFX screenmode information */
      UnLockBitMap(LockBitMapTags(screen->RastPort.BitMap,
                                  LBMI_PIXFMT, (ULONG)&pixfmt,
                                  LBMI_BASEADDRESS, (ULONG)&baseaddress[0],
                                  LBMI_BYTESPERROW, (ULONG)&bpr,
                                  TAG_END));

      memset(baseaddress[0], 0, (height*bpr));
    }
  }

  /* Open window */
  window=OpenWindowTags(NULL,
         WA_CustomScreen,(int)screen,
         WA_Width,width,
         WA_Height,height,
         WA_IDCMP,IDCMP_RAWKEY,
         WA_Backdrop,TRUE,
         WA_Borderless,TRUE,
         WA_Activate,TRUE,
         TAG_DONE);

  /* Could the window be opened? */
  if(window == NULL) {
    CloseScreen(screen);
    printf("The window could NOT be opened!\n");
    return -1;
  }

  /* Hide the pointer */
  EmptyPointer = AllocVec(512, MEMF_CHIP|MEMF_CLEAR);
  SetPointer(window, EmptyPointer, 1, 1, 0, 0);

  /* Force CD32 pad (if required) */
  if ((LowLevelBase != NULL) && (use_pad == 1))
    SetJoyPortAttrs(1, SJA_Type, SJA_TYPE_GAMECTLR, TAG_END);

  return 0;
}

void gbe_close()
{
  free_rom();
  free_memory();

  close_screen();
  if (screentype == AGA) {
    FreeRaster(agabitmap.Planes[0], width, height*8);
  }
  close_audio();
  close_libraries();
}

void gbe_create_window(char *title, int width, int height)
{
  gbe_window_width = width;
  gbe_window_height = height;
  xoffset = (320-width)/2;
  yoffset = (256-height)/2;

  /* AmigaOS is all good, allocate buffer */
  gfx_buffer = (char *)malloc(width * height);
  if(!gfx_buffer) {
    printf("unable to alloc buffer!\n");
    exit(0);
  }

  if (open_screen() < 0) {
    close_libraries();
    exit(0);
  }
}

int gbe_poll_events()
{
  ULONG imCode, imClass, portstate;
  struct IntuiMessage *imsg = NULL;

  if ((use_pad == 0) || (LowLevelBase == NULL)) {

  /* Check for IDCMP messages */
  if ((imsg=(struct IntuiMessage *)GetMsg(window->UserPort))) {
    imClass=imsg->Class;
    imCode=imsg->Code;

    ReplyMsg((struct Message *)imsg);

    if(imClass==IDCMP_RAWKEY) {

      if (imCode == 0xdf) { /* HELP */
        write_pcx(gfx_buffer, gbe_window_width, gbe_window_height, filename);
      }

      if(imCode<128) {
        /* Key is pressed */
        switch(imCode) {
          case 69: /* ESC */
            exit(0);
            break;

          case 17: /* W */
          case 68: /* ENTER */
            joypad_press(GB_START);
            break;

          case 16: /* Q */
          case 66: /* TAB */
            joypad_press(GB_SELECT);
            break;

          case 32: /* A */
          case 99: /* CONTROL */
            joypad_press(GB_B);
            break;

          case 33: /* S */
          case 100: /* LALT */
            joypad_press(GB_A);
            break;

          case 78: /* RIGHT */
            joypad_press(GB_RIGHT);
            break;

          case 79: /* LEFT */
            joypad_press(GB_LEFT);
            break;

          case 77: /* DOWN */
            joypad_press(GB_DOWN);
            break;

          case 76: /* UP */
            joypad_press(GB_UP);
            break;

          default:
            break;
        }
      } else {
        imCode-=128; 
        /* Key was released */
        switch(imCode) {
          case 69: /* ESC */
            exit(0);
            break;

          case 17: /* W */
          case 68: /* ENTER */
            joypad_release(GB_START);
            break;

          case 16: /* Q */
          case 66: /* TAB */
            joypad_release(GB_SELECT);
            break;

          case 32: /* A */
          case 99: /* CONTROL */
            joypad_release(GB_B);
            break;

          case 33: /* S */
          case 100: /* LALT */
            joypad_release(GB_A);
            break;

          case 78: /* RIGHT */
            joypad_release(GB_RIGHT);
            break;

          case 79: /* LEFT */
            joypad_release(GB_LEFT);
            break;

          case 77: /* DOWN */
            joypad_release(GB_DOWN);
            break;

          case 76: /* UP */
            joypad_release(GB_UP);
            break;

          default:
            break;
        }
      }
    }
  }

  } else {

  /* Check for IDCMP messages */
  if ((imsg=(struct IntuiMessage *)GetMsg(window->UserPort))) {
    imClass=imsg->Class;
    imCode=imsg->Code;

    ReplyMsg((struct Message *)imsg);

    if(imClass==IDCMP_RAWKEY) {

      if (imCode == 0xdf) { /* HELP */
        write_pcx(gfx_buffer, gbe_window_width, gbe_window_height, filename);
      }

      if(imCode<128) {
        /* Key is pressed */
        switch(imCode) {
          case 69: /* ESC */
            exit(0);
            break;

          default:
            break;
        }
      } else {
        imCode-=128;
        /* Key was released */
        switch(imCode) {
          case 69: /* ESC */
            exit(0);
            break;

          default:
            break;
        }
      }
    }
  }

  /* Read port 1 (joyport) */
  portstate = ReadJoyPort(1);

  if (portstate&JPF_JOY_RIGHT) /* RIGHT */
    joypad_press(GB_RIGHT);
  else
    joypad_release(GB_RIGHT);

  if (portstate&JPF_JOY_LEFT) /* LEFT */
    joypad_press(GB_LEFT);
  else
    joypad_release(GB_LEFT);

  if (portstate&JPF_JOY_DOWN) /* DOWN */
    joypad_press(GB_DOWN);
  else
    joypad_release(GB_DOWN);

  if (portstate&JPF_JOY_UP) /* UP */
    joypad_press(GB_UP);
  else
    joypad_release(GB_UP);

  if (portstate&JPF_BUTTON_GREEN) /* Q */
    joypad_press(GB_SELECT);
  else
    joypad_release(GB_SELECT);

  if (portstate&JPF_BUTTON_YELLOW) /* W */
    joypad_press(GB_START);
  else
    joypad_release(GB_START);

  if (portstate&JPF_BUTTON_RED) /* A */
    joypad_press(GB_B);
  else
    joypad_release(GB_B);

  if (portstate&JPF_BUTTON_BLUE) /* S */
    joypad_press(GB_A);
  else
    joypad_release(GB_A);

  }

  return 1;
}

int palette_has_changed = 0;

void set_color(int num, int color)
{
  palette_has_changed = 1;
  rgbtab[1+num*3+0]=((color & 0x001f) << 3) << 24;
  rgbtab[1+num*3+1]=((color & 0x03e0) >> 2) << 24;
  rgbtab[1+num*3+2]=((color & 0x7c00) >> 7) << 24;
}

void vram_blit()
{
  unsigned char *src, *dst;
  int i;

  if (palette_has_changed == 1) {
    palette_has_changed = 0;
    rgbtab[0]=(256<<16)|0;
    rgbtab[1+256*3]=0;
    LoadRGB32(&screen->ViewPort, rgbtab);
  }

  if (screen != NULL) {
    if (screentype == AGA) {
      c2p((void *)gfx_buffer, (void *)agabitmap.Planes[0], gbe_window_width, gbe_window_height, xoffset, yoffset, RASSIZE(width, height));
    } else {
      if (use_cgfx == 1) {
          WritePixelArray(gfx_buffer, 0, 0, gbe_window_width, &screen->RastPort, xoffset, yoffset, gbe_window_width, gbe_window_height, RECTFMT_LUT8);
      } else {
        if (use_triple == 1) {
          /* Next buffer please */
          cur_sbuffer++;
          if (cur_sbuffer > 2) {
            cur_sbuffer = 0;
          }

          /* Display the next buffer */
          sbuffer[cur_sbuffer]->sb_DBufInfo->dbi_SafeMessage.mn_ReplyPort = NULL;
          while (!ChangeScreenBuffer(screen, sbuffer[cur_sbuffer]));

          free_sbuffer = cur_sbuffer + 1;
          if (free_sbuffer > 2) {
            free_sbuffer = 0;
          }
        }

        /* Init pointers */
        dst = baseaddress[free_sbuffer]+(yoffset*bpr)+(xoffset);
        src = gfx_buffer;

        /* Perform copy */
        for (i=0; i<gbe_window_height; i++) {
          memcpy(dst, src, gbe_window_width);
          dst += bpr;
          src += gbe_window_width;
        }
      }
    }
  }
}

void plot_line(int y, int *index)
{
  char *cdst;
  int i;

  cdst = &gfx_buffer[(gbe_window_width*y)];

  for (i=0; i<gbe_window_width; i++) {
    *cdst++ = index[i];
  }
}

force_type force_system = NONE;
int ready;

const char ver[] = "$VER: GBE 0.4.8b (000917)";

#define TEMPLATE "FILE/A,JOYPAD/S,CGFX/S,REQUESTER/S,NOSTALGIA/S,TRIPLE/S"

#define OPT_FILE       0
#define OPT_JOYPAD     1
#define OPT_CGFX       2
#define OPT_REQUESTER  3
#define OPT_NOSTALGIA  4
#define OPT_TRIPLE     5

#define OPT_COUNT      6

long opts[OPT_COUNT];
struct RDArgs *rdargs = NULL;

void initialize_rom();

/* main function */
int main(int argc, char *argv[])
{
  if (open_libraries() < 0) {
    printf("Could not open required libraries\n");
    return 0;
  }

  memset((char *)opts, 0, sizeof(opts));

  if ((rdargs = ReadArgs(TEMPLATE, opts, NULL)))
  {
    /* Copy filename */
    filename = strdup((char *)opts[OPT_FILE]);

    if (opts[OPT_JOYPAD]) use_pad = 1;
    if (opts[OPT_CGFX]) use_cgfx = 1;
    if (opts[OPT_REQUESTER]) use_req = 1;
    if (opts[OPT_NOSTALGIA]) nostalgia = 1;
    if (opts[OPT_TRIPLE]) use_triple = 1;

    FreeArgs((struct RDArgs *) rdargs);
  }
  else
  {
    printf("%s\n\nusage: %s <romfile/system>\n\n", &ver[6], argv[0]);

    printf("JOYPAD/S     : use CD32 joypad (def: keyboard).\n");
    printf("CGFX/S       : use CyberGraphics instead of direct gfxmem access (def: off).\n");
    printf("REQUESTER/S  : use screenmode requester (def: CGFX BestModeID/PAL Lores).\n");
    printf("NOSTALGIA/S  : save the screenshot in Nostalgia:samples/ (def: current directory).\n");
    printf("TRIPLE/S     : use triple buffering (def: off).\n\n");
    exit(0);
  }

  if (load_rom(filename)) {
    if (initialize_memory() == 0) {
      free_memory();

      printf("Unable to allocate memory\n");
      return 0;
    }

    open_audio();

    gbe_create_window("GBE", 160, 144);

    atexit(gbe_close);

    initialize_rom();

    gameboy_cpu_hardreset();

    gameboy_cpu_run();  
  } else {
    printf("Unable to load '%s'\n", filename);
  }

  return 0;
}
