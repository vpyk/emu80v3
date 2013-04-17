/*
 *  RrkWin32
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

// snd_in.cpp

#include <windows.h>
#include <mmsystem.h>

//======== Common variables ========

static WAVEFORMATEX wf;
static WAVEHDR wh1,wh2;
static char *pcData1,*pcData2;
static HANDLE wi;
static int nCurBuf;
static nRecBufSize;
static volatile nPrepBufs;
static HANDLE FreeBufEv;
static BOOL bOpen=FALSE;

//======== Common functions ========

static void CALLBACK WaveInFunc(HWAVEIN hwi,
                                UINT wMsg,
                                DWORD dwInstance,
                                DWORD dwParam1,
                                DWORD dwParam2)
{
if (wMsg==WIM_DATA)
  {
  nPrepBufs--;
  SetEvent(FreeBufEv);
  }
}

//======== Intput functions ========

int FindInputDevice()
{
return waveInGetNumDevs!=0;
}

int OpenInputDevice(int nChannels,
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
MMRESULT mmr=waveInOpen(&wi,WAVE_MAPPER,(WAVEFORMAT *)&wf,
                        (long)WaveInFunc,0,CALLBACK_FUNCTION);
if (mmr==0)
  {
  FreeBufEv=CreateEvent(NULL,TRUE,TRUE,NULL);
  bOpen=TRUE;
  }
return mmr==0;
}

void CloseInputDevice()
{
CloseHandle(FreeBufEv);
waveInReset(wi);
waveInClose(wi);
bOpen=FALSE;
}

void StartRecording(int nSize)
{
nRecBufSize=nSize;
pcData1=new char[nSize];
pcData2=new char[nSize];
wh1.lpData=pcData1;
wh2.lpData=pcData2;
wh1.dwBufferLength=nSize;
wh2.dwBufferLength=nSize;
waveInPrepareHeader(wi,&wh1,sizeof(wh1));
waveInAddBuffer(wi,&wh1,sizeof(wh1));
waveInPrepareHeader(wi,&wh2,sizeof(wh2));
waveInAddBuffer(wi,&wh2,sizeof(wh2));
nPrepBufs=2;
ResetEvent(FreeBufEv);
waveInStart(wi);
}

void RecordBlock(char *pcData)
{
WaitForSingleObject(FreeBufEv,INFINITE);
if (nCurBuf==1)
  {
  waveInUnprepareHeader(wi,&wh1,sizeof(wh1));
  memcpy(pcData,pcData1,nRecBufSize);
  waveInPrepareHeader(wi,&wh1,sizeof(wh1));
  waveInAddBuffer(wi,&wh1,sizeof(wh1));
  }
else
  {
  waveInUnprepareHeader(wi,&wh2,sizeof(wh2));
  memcpy(pcData,pcData2,nRecBufSize);
  waveInPrepareHeader(wi,&wh2,sizeof(wh2));
  waveInAddBuffer(wi,&wh2,sizeof(wh2));
  }
nPrepBufs++;
nCurBuf=3-nCurBuf;
ResetEvent(FreeBufEv);
}

void StopRecording()
{
waveInReset(wi);
nPrepBufs=0;
nCurBuf=1;
SetEvent(FreeBufEv);
waveInUnprepareHeader(wi,&wh1,sizeof(wh1));
delete[] pcData1;
pcData1=NULL;
waveInUnprepareHeader(wi,&wh2,sizeof(wh2));
delete[] pcData2;
pcData2=NULL;
}

//======== End of file ========

