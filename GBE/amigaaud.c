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
#include <exec/memory.h>
#include <devices/audio.h>

#include <powerup/ppcproto/intuition.h>
#include <powerup/ppclib/interface.h>
#include <powerup/ppclib/time.h>
#include <powerup/gcclib/powerup_protos.h>
#include <powerup/ppcproto/exec.h>
#include <powerup/ppcinline/alib.h>

#define AUDIO_CLOCK 3546895

#define SAMPLERATE 22050
#define AUDIOBUFFER 4096

static struct IOAudio *AudioIO[2], *AudioIO1[2], *AudioIO2[2];
static struct MsgPort *AudioMP[2], *AudioMP1[2], *AudioMP2[2];

static int first_time[2], audio_is_open = 0;
static ULONG long device = 1;
static UBYTE whichannel1[] = {1 + 2};
static UBYTE whichannel2[] = {4 + 8};
static BYTE *audioptr[2];

void play_sample(signed char *dataL, signed char *dataR)
{
  signed char *data;
  int channel;

  for (channel=0; channel<2; channel++) {
    if (channel == 0) {
      data = dataL;
    } else {
      data = dataR;
    }

    if (first_time[channel] == 0) {
      WaitPort(AudioMP[channel]);
      GetMsg(AudioMP[channel]);
    }
    else
      first_time[channel]--;

    if (AudioIO[channel] == AudioIO1[channel]) {
      memcpy(audioptr[channel], data, AUDIOBUFFER);
      AudioIO[channel]->ioa_Request.io_Command = CMD_WRITE;
      AudioIO[channel]->ioa_Request.io_Flags   = ADIOF_PERVOL;
      AudioIO[channel]->ioa_Volume             = 64;
      AudioIO[channel]->ioa_Period             = AUDIO_CLOCK/SAMPLERATE;
      AudioIO[channel]->ioa_Cycles             = 1;
      AudioIO[channel]->ioa_Length             = AUDIOBUFFER;
      AudioIO[channel]->ioa_Data               = (BYTE *)audioptr[channel];
    } else {
      memcpy(audioptr[channel]+AUDIOBUFFER, data, AUDIOBUFFER);
      AudioIO[channel]->ioa_Request.io_Command = CMD_WRITE;
      AudioIO[channel]->ioa_Request.io_Flags   = ADIOF_PERVOL;
      AudioIO[channel]->ioa_Volume             = 64;
      AudioIO[channel]->ioa_Period             = AUDIO_CLOCK/SAMPLERATE;
      AudioIO[channel]->ioa_Cycles             = 1;
      AudioIO[channel]->ioa_Length             = AUDIOBUFFER;
      AudioIO[channel]->ioa_Data               = (BYTE *)audioptr[channel]+AUDIOBUFFER;
    }

    BeginIO((struct IORequest *)AudioIO[channel]);

    if (AudioIO[channel] == AudioIO1[channel]) {
      AudioIO[channel] = AudioIO2[channel];
      AudioMP[channel] = AudioMP2[channel];
    } else {
      AudioIO[channel] = AudioIO1[channel];
      AudioMP[channel] = AudioMP1[channel];
    }
  }
}

void close_audio(void)
{
  int i;

  /* Abort any pending IO requests */
  if ((audio_is_open == 1) && (first_time[0] == 0)) {
    for (i=0; i<2; i++) {
      AbortIO((struct IORequest *)AudioIO1[i]);
      WaitPort(AudioMP1[i]);
      GetMsg(AudioMP1[i]);
    }
    for (i=0; i<2; i++) {
      AbortIO((struct IORequest *)AudioIO2[i]);
      WaitPort(AudioMP2[i]);
      GetMsg(AudioMP2[i]);
    }
  }

  /* Free resources */
  if (device == 0)
    CloseDevice((struct IORequest *)AudioIO1[0]);

  for (i=0; i<2; i++)
    if (AudioIO2[i] != NULL)
      DeleteIORequest(AudioIO2[i]);
  for (i=0; i<2; i++)
    if (AudioIO1[i] != NULL)
      DeleteIORequest(AudioIO1[i]);
  for (i=0; i<2; i++)
    if (AudioMP2[i] != NULL)
      DeleteMsgPort(AudioMP2[i]);
  for (i=0; i<2; i++)
    if (AudioMP1[i] != NULL)
      DeleteMsgPort(AudioMP1[i]);
  for (i=0; i<2; i++)
    if (audioptr[i] != NULL)
      FreeMem(audioptr[i], AUDIOBUFFER*2);

  /* Make sure that the variables are reset */
  device = 1;

  for (i=0; i<2; i++) {
    audioptr[i] = NULL;
    AudioMP[i] = NULL;
    AudioMP1[i] = NULL;
    AudioMP2[i] = NULL;
    AudioIO[i] = NULL;
    AudioIO1[i] = NULL;
    AudioIO2[i] = NULL;
    first_time[i] = 2;
  }

  audio_is_open = 0;
}

void open_audio(void)
{
  int i, error = 0, shifter = 0;

//  volatile unsigned char *filter = (unsigned char *)0xbfe001;

  /* Make sure that the variables are reset */
  audio_is_open = 0;
  device = 1;

  for (i=0; i<2; i++) {
    audioptr[i] = NULL;
    AudioMP[i] = NULL;
    AudioMP1[i] = NULL;
    AudioMP2[i] = NULL;
    AudioIO[i] = NULL;
    AudioIO1[i] = NULL;
    AudioIO2[i] = NULL;
    first_time[i] = 2;
  }

  /* Allocate resources */
  for (i=0; i<2; i++) {
    audioptr[i] = (BYTE *)AllocMem(AUDIOBUFFER*2, MEMF_CHIP|MEMF_PUBLIC);
    if (audioptr[i] == NULL) error = 1;
  }

  if (!error) {
    for (i=0; i<2; i++) {
      AudioMP1[i] = CreateMsgPort();
      if (AudioMP1[i] == NULL) error = 1;
    }
  }

  if (!error) {
    for (i=0; i<2; i++) {
      AudioMP2[i] = CreateMsgPort();
      if (AudioMP2[i] == NULL) error = 1;
    }
  }

  if (!error) {
    for (i=0; i<2; i++) {
      AudioIO1[i] = CreateIORequest(AudioMP1[i], sizeof(struct IOAudio));
      if (AudioIO1[i] == NULL) error = 1;
    }
  }

  if (!error) {
    for (i=0; i<2; i++) {
      AudioIO2[i] = CreateIORequest(AudioMP2[i], sizeof(struct IOAudio));
      if (AudioIO2[i] == NULL) error = 1;
    }
  }

  if (!error) {
    AudioIO1[0]->ioa_Request.io_Message.mn_ReplyPort   = AudioMP1[0];
    AudioIO1[0]->ioa_Request.io_Message.mn_Node.ln_Pri = 127;
    AudioIO1[0]->ioa_Request.io_Command                = ADCMD_ALLOCATE;
    AudioIO1[0]->ioa_Request.io_Flags                  = ADIOF_NOWAIT;
    AudioIO1[0]->ioa_AllocKey                          = 0;

    AudioIO1[0]->ioa_Data   = whichannel1;
    AudioIO1[0]->ioa_Length = sizeof(whichannel1);
    device = OpenDevice(AUDIONAME, 0L, (struct IORequest *)AudioIO1[0], 0L);
    shifter = 0;
    if (device != 0) {
      AudioIO1[0]->ioa_Data   = whichannel2;
      AudioIO1[0]->ioa_Length = sizeof(whichannel2);
      device = OpenDevice(AUDIONAME, 0L, (struct IORequest *)AudioIO1[0], 0L);
      shifter = 2;
      if (device != 0) error = 1;
    }
  }

  if (!error) {
    *AudioIO1[1] = *AudioIO1[0];

    for (i=0; i<2; i++) {
      AudioIO1[i]->ioa_Request.io_Command = CMD_WRITE;
      AudioIO1[i]->ioa_Request.io_Flags   = ADIOF_PERVOL;
      AudioIO1[i]->ioa_Volume             = 64;
      AudioIO1[i]->ioa_Period             = AUDIO_CLOCK/SAMPLERATE;
      AudioIO1[i]->ioa_Cycles             = 1;
      AudioIO1[i]->ioa_Length             = AUDIOBUFFER;

      /* Copy Settings */
      *AudioIO2[i] = *AudioIO1[i];

      /* Customize */
      AudioIO1[i]->ioa_Data = (BYTE *)audioptr[i];
      AudioIO2[i]->ioa_Data = (BYTE *)audioptr[i]+AUDIOBUFFER;
      AudioIO1[i]->ioa_Request.io_Message.mn_ReplyPort = AudioMP1[i];
      AudioIO2[i]->ioa_Request.io_Message.mn_ReplyPort = AudioMP2[i];

      AudioIO1[i]->ioa_Request.io_Unit = (struct Unit *)((1 << shifter) << i);
      AudioIO2[i]->ioa_Request.io_Unit = (struct Unit *)((1 << shifter) << i);
    }

    audio_is_open = 1;
  }

  if (audio_is_open != 1) {
    close_audio();
  } else {
    for (i=0; i<2; i++) {
      AudioIO[i] = AudioIO1[i];
      AudioMP[i] = AudioMP1[i];
    }
  }

//  /* Turn off audio filter */
//  *filter |= 0x02;

//  /* Turn on audio filter */
//  *filter &= 0xfd;
}

static signed char sample[2][AUDIOBUFFER];
static int sample_pos = 0;

void WriteSample(short int l, short int r)
{
  if (audio_is_open == 0) {
    return;
  }

  sample[0][sample_pos] = (l >> 8);
  sample[1][sample_pos] = (r >> 8);
  sample_pos++;

  if (sample_pos >= AUDIOBUFFER) {
    play_sample(sample[0], sample[1]);
    sample_pos = 0;
  }
}
