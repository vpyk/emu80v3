/*
 *  Emu80/SDL 3.x
 *  Copyright (C) 1997-2013 Viktor Pykhonin <pyk@mail.ru>
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

// sound.cpp

// Библиотека поддержки эмулятора.
// Функции работы со звуком.

#include <SDL/SDL.h>
#include <string.h>
#include "emuinit.h"
#include "sound.h"

const int nBufSize=2048;

static Uint8 *pbSndBuf;
static int bStarted=0;

static int nCurPos;

int bSBPresent=0;

SDL_AudioSpec asSpec;
SDL_sem *semAudio;

int bWaitSnd=0;

void AudioCallback(void *pUserData, Uint8 *pbStream, int nLen)
{
if (nLen!=nBufSize)
  return; // Error
if (nCurPos>=2048)
  memcpy(pbStream,pbSndBuf,nBufSize);
else
  memcpy(pbStream,pbSndBuf+nBufSize,nBufSize);
SDL_SemPost(semAudio);
}

void InitAudio()
{
if (bMute) return; // на всякий случай
bSBPresent=1;
pbSndBuf = new Uint8[nBufSize*2];
semAudio=SDL_CreateSemaphore(1);

asSpec.freq=44100;
asSpec.format=AUDIO_S8;
asSpec.channels=1;
asSpec.samples=nBufSize;
asSpec.callback=AudioCallback;

SDL_OpenAudio(&asSpec,0);
//for (int i=0;i<nBufSize;i++)
//  PlayByteNoWait(asSpec.silence);
memset(pbSndBuf,asSpec.silence,nBufSize*2);
nCurPos=nBufSize;
bWaitSnd=0;
}

void ExitAudio()
{
StopSound();
delete[] pbSndBuf;
SDL_DestroySemaphore(semAudio);
}

void StartSound(/*SS_MODE ssMode*/)
{
bStarted=1;
SDL_PauseAudio(0);
}

int prev=0;

void StopSound()
{
bStarted=0;
SDL_PauseAudio(1);
}

void WaitSound()
{
bWaitSnd=0;
int ttt=SDL_GetTicks();
int sss=SDL_SemValue(semAudio);
SDL_SemWait(semAudio);
//printf("%d - %d\n",sss,SDL_GetTicks()-ttt);
}

void PlayByteNoWait(unsigned char b)
{
if (!bSBPresent)
  return;
if (!bStarted)
  StartSound(/*ssOut*/);
pbSndBuf[nCurPos]=(b)>>1;
nCurPos=(nCurPos+1)%(nBufSize*2);
if ((nCurPos==0) || (nCurPos==nBufSize))
//  SDL_SemWait(semAudio);
  bWaitSnd=(SDL_SemTryWait(semAudio)==SDL_MUTEX_TIMEDOUT);
}

unsigned char ReadByteNoWait()
{
if (!bStarted)
  StartSound(/*ssIn*/);
unsigned char res=((char *)pbSndBuf)[nCurPos]>0?1:0;
nCurPos=(nCurPos+1)%nBufSize;
if ((nCurPos==0) || (nCurPos==nBufSize))
  bWaitSnd=(SDL_SemTryWait(semAudio)==SDL_MUTEX_TIMEDOUT);
return res;
}
