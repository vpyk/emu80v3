/*
 *  WrkWin32
 *  Copyright (C) 1998-2013 Viktor Pykhonin <pyk@mail.ru>
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

// sndout.cpp

#include <windows.h>
#include <mmsystem.h>

//======== Common variables ========

static WAVEFORMATEX wf;
static WAVEHDR wh1,wh2;
static char *pcData1,*pcData2;
static HANDLE wo,wi;
static int nCurBuf;
static nRecBufSize;
static volatile nPrepBufs;
static HANDLE FreeBufEv;
static BOOL bOpen=FALSE;

//======== Common functions ========

static void CALLBACK WaveOutFunc(HWAVEOUT hwo,
                                 UINT wMsg,
                                 DWORD dwInstance,
                                 DWORD dwParam1,
                                 DWORD dwParam2)
{
if (wMsg==WOM_DONE)
  {
  nPrepBufs--;
  SetEvent(FreeBufEv);
  }
}

//======== Output functions ========

int FindOutputDevice()
{
return waveOutGetNumDevs!=0;
}

int OpenOutputDevice(int nChannels,
                     int nRate,
                     int nBits)
{
if (bOpen)
  return FALSE;
pcData1=NULL;
pcData2=NULL;
nCurBuf=1;
nPrepBufs=0;
wf.wFormatTag=WAVE_FORMAT_PCM;
wf.nChannels=nChannels;
wf.nSamplesPerSec=nRate;
wf.nAvgBytesPerSec=nRate*nBits/8;
wf.nBlockAlign=nBits/8;
wf.wBitsPerSample=nBits;
wf.cbSize=sizeof(wf);
wh1.lpData=NULL;
wh1.dwBufferLength=0;
wh1.dwFlags=0;
wh1.dwLoops=1;
wh2.lpData=NULL;
wh2.dwBufferLength=0;
wh2.dwFlags=0;
wh2.dwLoops=1;
MMRESULT mmr=waveOutOpen(&wo,WAVE_MAPPER,(WAVEFORMAT *)&wf,
                         (long)WaveOutFunc,0,CALLBACK_FUNCTION);
if (mmr==0)
  {
  FreeBufEv=CreateEvent(NULL,TRUE,TRUE,NULL);
  bOpen=TRUE;
  }
return mmr==0;
}

void CloseOutputDevice()
{
CloseHandle(FreeBufEv);
waveOutReset(wo);
waveOutClose(wo);
bOpen=FALSE;
}

void PlayBlock(char *pcData, int nSize)
{
//while (nPrepBufs>1) ;
WaitForSingleObject(FreeBufEv,INFINITE);
if (nCurBuf==1)
  {
  if (pcData1)
    {
    waveOutUnprepareHeader(wo,&wh1,sizeof(wh1));
    delete[] pcData1;
    }
  pcData1=new char[nSize];
  memcpy(pcData1,pcData,nSize);
  wh1.lpData=pcData1;
  wh1.dwBufferLength=nSize;
  waveOutPrepareHeader(wo,&wh1,sizeof(wh1));
  waveOutWrite(wo,&wh1,sizeof(wh1));
  }
else
  {
  if (pcData2)
    {
    waveOutUnprepareHeader(wo,&wh2,sizeof(wh2));
    delete[] pcData2;
    }
  pcData2=new char[nSize];
  memcpy(pcData2,pcData,nSize);
  wh2.lpData=pcData2;
  wh2.dwBufferLength=nSize;
  waveOutPrepareHeader(wo,&wh2,sizeof(wh2));
  waveOutWrite(wo,&wh2,sizeof(wh2));
  }
nPrepBufs++;
nCurBuf=3-nCurBuf;
if (nPrepBufs==2)
  ResetEvent(FreeBufEv);
}

void WaitForEndPlaying()
{
//while (nPrepBufs>0) ;
WaitForSingleObject(FreeBufEv,INFINITE);
if (nPrepBufs!=0)
  {
  ResetEvent(FreeBufEv);
  WaitForSingleObject(FreeBufEv,INFINITE);
  }
if (pcData1)
  {
  waveOutUnprepareHeader(wo,&wh1,sizeof(wh1));
  delete[] pcData1;
  pcData1=NULL;
  }
if (pcData2)
  {
  waveOutUnprepareHeader(wo,&wh2,sizeof(wh2));
  delete[] pcData2;
  pcData2=NULL;
  }
}

void StopPlaying()
{
waveOutReset(wo);
nPrepBufs=0;
nCurBuf=1;
SetEvent(FreeBufEv);
if (pcData1)
  {
  waveOutUnprepareHeader(wo,&wh1,sizeof(wh1));
  delete[] pcData1;
  pcData1=NULL;
  }
if (pcData2)
  {
  waveOutUnprepareHeader(wo,&wh2,sizeof(wh2));
  delete[] pcData2;
  pcData2=NULL;
  }
}

//======== End of file ========
