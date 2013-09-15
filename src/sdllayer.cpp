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

// sdllayer.cpp

// Библиотека поддрежки эмулятора.
// Все вызовы SDL осуществляются только из этого модуля,
// остальные модули пользуются функциями, описанными здесь.

#include <stdlib.h>
#include <string.h>
#include <SDL/SDL.h>

#include "sdllayer.h"
#include "main.h"
#include "emuinit.h"
#include "misc.h"
#include "scrdraw.h"
#include "sound.h"
#include "asm.h" //!!!

//-----------------------------------------------------------------------------
static void LoadFonts();
static void FreeFonts();
static void SetPalette();
static void ShowCursor(int x, int y);
static void ChangeResolution();
//-----------------------------------------------------------------------------

char *pcScreenBuf;

static SDL_Surface *sfScreen;
static int nCurX,nCurY;
static int bCurVisible=0;
static TVideoMode vmVideoMode=vmNone;
static int nLinesPerSymbol;
static int nTextLines;
static int nSavedX, nSavedY;
int bFullScreen=0; //!!!
static int bBlinkCursor=0;
static int nBlinkCounter=0;
static int bInteractive=0;
static int nTimerTicks;
//static SDL_sem *semDraw;

// Инициализация библиотеки эмулятора
void InitEGraph()
{
char pcCaption[80];
if (strlen(szVideoDriver)!=0)
  {
  strcpy(pcCaption, "SDL_VIDEODRIVER=");
  strcat(pcCaption,szVideoDriver);
  putenv(pcCaption);
  }
if (strlen(szAudioDriver)!=0)
  {
  strcpy(pcCaption, "SDL_AUDIODRIVER=");
  strcat(pcCaption,szAudioDriver);
  putenv(pcCaption);
  }
//putenv("SDL_VIDEODRIVER=windib");
//putenv("SDL_AUDIODRIVER=waveout");
SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER);
if (!bMute)
  bMute=SDL_InitSubSystem(SDL_INIT_AUDIO);

char pcDrvName[20];
strcpy(pcCaption, "Emu80/SDL v. 3.14 development (video: ");
SDL_VideoDriverName(pcDrvName, 20);
strcat(pcCaption,pcDrvName);
if (!bMute)
  {
  strcat(pcCaption,", audio: ");
  SDL_AudioDriverName(pcDrvName, 20);
  strcat(pcCaption,pcDrvName);
  }
strcat(pcCaption,"), F10 - menu");

//SDL_WM_SetCaption("Emu80 / SDL", 0);
SDL_WM_SetCaption(pcCaption, 0);

atexit(SDL_Quit);

//semDraw=SDL_CreateSemaphore(1);

/*sfScreen = SDL_SetVideoMode(640, 480, 8,
                            SDL_HWSURFACE |
                            SDL_HWPALETTE);*/
pcScreenBuf=new char[160*64];
ClearScreen();
LoadFonts();

SetVideoMode(vmText30);
//ChangeResolution();

//SDL_ShowCursor(SDL_DISABLE);
ClearScreen();

InitAudio();
}

// Деинициализация библиотеки эмулятора
void QuitEGraph()
{
//SDL_SemWait(semDraw); //!!!
//SDL_DestroySemaphore(semDraw);
ExitAudio();
delete[] pcScreenBuf;
FreeFonts();
}

void ToggleFullScreen()
{
bFullScreen=!bFullScreen;
ChangeResolution();
}

void ChangeResolution()
{
if (bFullScreen)
  {
  if (vmVideoMode==vmGraph512)
    sfScreen = SDL_SetVideoMode(512, 384, 8,
                                SDL_SWSURFACE | SDL_FULLSCREEN |
                                SDL_HWPALETTE);
  else if (vmVideoMode==vmGraph800)
    sfScreen = SDL_SetVideoMode(800, 600, 8,
                                SDL_SWSURFACE | SDL_FULLSCREEN |
                                SDL_HWPALETTE);
  else
    sfScreen = SDL_SetVideoMode(640, 480, 8,
                                SDL_SWSURFACE | SDL_FULLSCREEN |
                                SDL_HWPALETTE);
  SDL_ShowCursor(SDL_DISABLE);
  }
else
  {
  if (vmVideoMode==vmGraph512)
    sfScreen = SDL_SetVideoMode(512, 384, 8,
                                SDL_SWSURFACE | SDL_HWPALETTE);
  else if (vmVideoMode==vmGraph800)
    sfScreen = SDL_SetVideoMode(800, 600, 8,
                                SDL_SWSURFACE | SDL_HWPALETTE);
  else
    sfScreen = SDL_SetVideoMode(640, 480, 8,
                                SDL_SWSURFACE | SDL_HWPALETTE);
  SDL_ShowCursor(SDL_ENABLE);
  }
//printf("w=%d, h=%d, pitch=%d\n",SDL_GetVideoSurface()->w,SDL_GetVideoSurface()->h,SDL_GetVideoSurface()->pitch);
SetPalette();
DrawScreen();
}

// Проверка, является ли режим графическим
static int IsGraphMode(TVideoMode vm)
{
return (vm==vmGraph512 || vm==vmGraph800);
}

// Установка размера шрифта (8, 12 или 16 сконлиний на символ) или гр. режима
void SetVideoMode(TVideoMode vm)
{
if (vm==vmVideoMode) return;
TVideoMode vmOldVM=vmVideoMode;
vmVideoMode=vm;
switch (vmVideoMode)
  {
  case vmText30:
    nLinesPerSymbol=16;
    nTextLines=30;
    break;
  case vmText40:
    nLinesPerSymbol=12;
    nTextLines=40;
    break;
  case vmText60:
    nLinesPerSymbol=8;
    nTextLines=60;
  case vmGraph512:
    nLinesPerSymbol=12;
    nTextLines=32;
    break;
  case vmGraph800:
    nLinesPerSymbol=16;
    nTextLines=30;
    break;
  }
if ((!IsGraphMode(vmOldVM) && IsGraphMode(vmVideoMode)) ||
   (IsGraphMode(vmOldVM) && !IsGraphMode(vmVideoMode)) ||
   (IsGraphMode(vmOldVM) && IsGraphMode(vmVideoMode) && vmOldVM!=vmVideoMode) ||
    vmOldVM==vmNone)
  ChangeResolution();
//if (vmOldVM!=vmVideoMode /*|| IsGraphMode(vmVideoMode)*/)
//  DrawScreen();
}

// Очистка буфера текстового экрана
void ClearScreen()
{
memset(pcScreenBuf,0,160*64);
}

// Ввод скан-кода клавиши с ожиданием нажатия
// Необработанные эвенты передаются функции HandleEvent
short ReadScan()
{
short res;
SDL_Event event;
DrawScreen();
for(;;)
  {
  SDL_WaitEvent(&event);
  if (event.type==SDL_KEYDOWN && !(event.key.keysym.mod & KMOD_ALT))
    {
    res=(event.key.keysym.scancode&0x7f); // в драйвере directx бит 7 установлен?
    res=TranslateScanCode(res)<<8;
//    if (event.key.keysym.sym<128)
//      res|=event.key.keysym.sym;
    if (event.key.keysym.unicode<128)
      res|=event.key.keysym.unicode;
    return res;
    }
  HandleEvent(&event);
  }
}

// Установка интерактивного режима
// После вызова этой функции можно вызывать ReadScan()
void EnterInteractiveMode()
{
// SDL_WM_SetCaption("Interactive",0);
StopSound();
bInteractive=1;
SDL_EnableKeyRepeat(SDL_DEFAULT_REPEAT_DELAY , SDL_DEFAULT_REPEAT_INTERVAL);
SDL_EnableUNICODE(1);
bBlinkCursor=0;
if (IsGraphMode(vmVideoMode))
  ClearScreen();
//............
}

// Отмена интерактьивного режима
// После вызова этой функции можно вызывать CheckKeys()
void LeaveInteractiveMode()
{
// SDL_WM_SetCaption("Not Interactive",0);
bInteractive=0;
SDL_EnableKeyRepeat(0, 0);
SDL_EnableUNICODE(0);
bBlinkCursor=1;
StartSound();
FlushKeys();
if (vmVideoMode==vmGraph512)
  SDL_FillRect(SDL_GetVideoSurface(),0,0);
//............
}

// Проверка наличия событий нажатия/отжатия клавиш. В случае обнаружения
// таких событий скан-коды клавиш передаются обработчику ProcessKeyboard
// Необработанные эвенты передаются функции HandleEvent
void CheckKeys()
{
unsigned char cScanCode;
SDL_Event event;
while(SDL_PollEvent(&event))
  {
  if (event.type==SDL_KEYDOWN || event.type==SDL_KEYUP)
    {
    cScanCode=event.key.keysym.scancode;
#ifdef linux
      // в новой версии SDL бит 7 установлен?
      cScanCode-=0x80;
#endif
    ProcessKeyboard(event.type==SDL_KEYDOWN?cScanCode&0x7f:cScanCode|0x80);
    }
/*  else if (event.type==SDL_ACTIVEEVENT)
    {
    if (bFullScreen && event.active.state==SDL_APPACTIVE &&
        event.active.gain==1)
    SetPalette();
    }*/
  HandleEvent(&event);
  }
}

void Delay(int nMS)
{
SDL_Delay(nMS);
}

// Установка курсора в необходимую позицию
void SetCursorPos(int x, int y)
{
nCurX=x;
nCurY=y;
bCurVisible=1;
}

// Запрос положения курсора
void GetCursorPos(int &x, int &y)
{
x=nCurX;
y=nCurY;
}

// Запоминание положения курсора
void SaveCursorPos()
{
nSavedX=nCurX;
nSavedY=nCurY;
}

// Восстановление положения курсора
void RestoreCursorPos()
{
SetCursorPos(nSavedX, nSavedY);
}

// Скрытие курсора
void HideCursor()
{
bCurVisible=0;
}

// Печать символа в текущей позиции курсора
void PrintChar(long ch)
{
if (!bCurVisible or nCurY>=nTextLines)
  return;
if (ch==8)
  {
  nCurX--;
  return;
  }
*(pcScreenBuf+nCurY*160+(nCurX++)*2)=(ch&0xff);
}

// Установка текстового режима
void SetTextMode()
{
//SetVideoMode(vmText30);
}

static SDL_Surface *sfPaintSurface;

unsigned char * E_BeginPaint()
{
sfPaintSurface=SDL_GetVideoSurface();
SDL_LockSurface(sfPaintSurface);
unsigned char *pbPixels=(unsigned char *)(sfPaintSurface->pixels);
return pbPixels;
}

void E_EndPaint()
{
SDL_UnlockSurface(sfPaintSurface);
SDL_UpdateRect(sfPaintSurface,0,0,0,0);
}

int GetPitch()
{
return sfPaintSurface->pitch;
}

void ClearTimer()
{
nTimerTicks=SDL_GetTicks();
}

int GetTimer()
{
return SDL_GetTicks()-nTimerTicks;
}

int IsInInteractiveMode()
{
return bInteractive;
}

//-----------------------------------------------------------------------------

/*Uint32 ColorMap[16]={0x000000,0x000080,0x008000,0x008080,
                     0x800000,0x800080,0x808000,0xA8A8A8,
                     0x404040,0x0000FF,0x00FF00,0x00FFFF,
                     0xFF0000,0xFF00FF,0xFFFF00,0xFFFFFF};*/

Uint32 ColorMap[16]={0x000000,0x0000AA,0x00AA00,0x00AAAA,
                     0xAA0000,0xAA00AA,0xAA5500,0xAAAAAA,
                     0x555555,0x5555FF,0x55FF55,0x55FFFF,
                     0xFF5555,0xFF55FF,0xFFFF55,0xFFFFFF};

unsigned char *pbFonts;

void LoadFonts()
{
// Выделяем область памяти под область хранения шрифтов
pbFonts=new unsigned char[9216];
// Создаем полный путь к файлам с ПЗУ и знакогенератором
//        call find_path //!!!
FILE *f=exepath_fopen("fonts/fonts.bin","r+b"); //!!!
fread(pbFonts,9216,1,f);
fclose(f);
}

void SetPalette()
{
SDL_Color clColor;
for (int i=0;i<16;i++)
  {
  clColor.r=(ColorMap[i]&0xFF0000)>>16;
  clColor.g=(ColorMap[i]&0xFF00)>>8;
  clColor.b=ColorMap[i]&0xFF;
  SDL_SetColors(SDL_GetVideoSurface(), &clColor, i, 1);
  }
}

void FreeFonts()
{
delete[] pbFonts;
}

int CurColor=0;

void ShowCursor(int x, int y)
{
if (x>79 or y>=nTextLines)
  return;
SDL_Rect rRectDst;
rRectDst.x=x*8;rRectDst.y=(y+1)*nLinesPerSymbol-2;rRectDst.w=8;rRectDst.h=2;
int nColor=pcScreenBuf[y*160+x*2+1]&0xf;
SDL_FillRect(SDL_GetVideoSurface(), &rRectDst, nColor);
}

/*void DrawScreen()
{
if (IsGraphMode(vmVideoMode))
  {
  SDL_FillRect(SDL_GetVideoSurface(),0,0);
  return;
  }
//pcScreenBuf[0]++; // Test
int i,j,c;
char cSymb, cAttr;
for (c=0;c<16;c++)
  {
  SetColor(c);
  for (i=0;i<nTextLines;i++) //!!!
    for (j=0;j<80;j++)
      {
      cSymb=pcScreenBuf[i*160+j*2];
      cAttr=pcScreenBuf[i*160+j*2+1];
      if ((cAttr&0xf)==c)
        (*PutChar)(j,i,cSymb,(cAttr&0xf0)>>4);
      }
   }
nBlinkCounter=(nBlinkCounter+1)%16;
if (bCurVisible && (!bBlinkCursor || (nBlinkCounter>8)))
  ShowCursor(nCurX,nCurY);
SDL_UpdateRect(SDL_GetVideoSurface(),0,0,0,0);
}*/

void DrawTextScreen()
{
if (! bInteractive && IsGraphMode(vmVideoMode)) //!!!
  {
  SDL_FillRect(SDL_GetVideoSurface(),0,0);
  return;
  }
int i,j,k,l;
unsigned char bSymb, bAttr, bFColor, bBColor;;
unsigned char * pbPixels=E_BeginPaint();
int pitch=GetPitch();

char *pcScreenBuf=::pcScreenBuf;

int nColumns=80;

int nOffset=0;

if (bInteractive)
  {
  if (vmVideoMode==vmGraph800)
    pbPixels+=pitch*60+80;
  else
  if (vmVideoMode==vmGraph512)
    {
    pcScreenBuf+=16;
    nColumns=64;
    nTextLines=32;
    nLinesPerSymbol=12; //!!!
    nOffset=64;
    }
  }
unsigned char * pbFnt=pbFonts;;

if (nLinesPerSymbol==12) pbFnt+=4096;
else if (nLinesPerSymbol==8) pbFnt+=7168;
unsigned char * pbF;
unsigned char * pbPix;
unsigned char bt;

for (i=0;i<nTextLines;i++)
  for (j=0;j<nColumns;j++)
    {
    bSymb=pcScreenBuf[i*160+j*2];
    bAttr=pcScreenBuf[i*160+j*2+1];
    bFColor=bAttr&0x0f;
//    bBColor=(bAttr&0xf0)>>4;
    bBColor=(bAttr&0x70)>>4;
    if (bAttr&0x80 && !bInteractive && nBlinkCounter&0x10)
      bFColor=0;
    pbF=pbFnt+(int)bSymb*nLinesPerSymbol;
    pbPix=pbPixels+i*nLinesPerSymbol*pitch+j*8;
    for (k=0;k<nLinesPerSymbol;k++)
      {
      bt=*(pbF++);
        for (l=0;l<8;l++)
        {
        pbPix[l]=bt&0x80?bFColor:bBColor;
        bt<<=1;
        }
      pbPix+=pitch;
      }
/*    if (i==nCurY && j==nCurX && bCurVisible && (!bBlinkCursor || (nBlinkCounter>8)) && nCurX<80 && nCurY<nTextLines)
      {
      pbPix-=pitch*2;
      for (k=0;k<8;k++)
        {
        *(pbPix+k)=bFColor;
        *(pbPix+pitch+k)=bFColor;
        }
      }*/
    }
nBlinkCounter=(nBlinkCounter+1)&0x1f;
if (bCurVisible && (!bBlinkCursor || (nBlinkCounter&8)) && nCurX<80 && nCurY<nTextLines)
  {
  bFColor=pcScreenBuf[nCurY*160+nCurX*2+1]&0x0f;
  for (i=0;i<8;i++)
    {
    pbPixels[(nCurY*nLinesPerSymbol+nLinesPerSymbol-2)*pitch+nCurX*8+i-nOffset]=bFColor;
    pbPixels[(nCurY*nLinesPerSymbol+nLinesPerSymbol-1)*pitch+nCurX*8+i-nOffset]=bFColor;
    }
  }
E_EndPaint();
//SDL_UpdateRect(SDL_GetVideoSurface(),0,0,0,0);
}

void DrawScreenOrion()
{
int i,j,k;
unsigned char * pbPixels=E_BeginPaint();
int pitch=GetPitch();
unsigned char bt, bt2, color;
pbPixels+=(44*pitch+16);
unsigned char * pbPix;
for (i=0;i<256;i++)
  {
  pbPix=pbPixels;
  for (j=0;j<48;j++)
    {
    bt=mempage0[scr_beg+j*256+i];
    bt2=mempage1[scr_beg+j*256+i];
    for (k=0;k<8;k++)
      {
      switch (or_color_mode)
        {
        case 0:
          color=(bt&0x80)?2:0;
          bt<<=1;
          break;
        case 1:
          color=(bt&0x80)?14:11;
          bt<<=1;
          break;
        case 2:
        case 3:
          color=0;
        case 4:
        case 5:
          color=or_color4_table[((bt&0x80)>>7)|((bt2&0x80)>>6)];
          bt<<=1;
          bt2<<=1;
          break;
        case 6:
        case 7:
          color=(bt&0x80)?(bt2&0x0f):((bt2&0xf0)>>4);
          bt<<=1;
        }
      *(pbPix+pitch)=color;
      *(pbPix+pitch+1)=color;
      *(pbPix++)=color;
      *(pbPix++)=color;
      }
    }
  pbPixels+=pitch;
  pbPixels+=pitch;
  }
E_EndPaint();
}

void DrawScreenSpec()
{
int i,j,k;
unsigned char * pbPixels=E_BeginPaint();
int pitch=GetPitch();
unsigned char bt, cl, color;
pbPixels+=(44*pitch+16);
unsigned char * pbPix;
for (i=0;i<256;i++)
  {
  pbPix=pbPixels;
  for (j=0;j<48;j++)
    {
    bt=mempage0[0x9000+(j<<8)+i];
    cl=color_mem[(j<<8)+i];
    for (k=0;k<8;k++)
      {
      color=(bt&0x80)?cl:0;
      bt<<=1;
      *(pbPix+pitch)=color;
      *(pbPix+pitch+1)=color;
      *(pbPix++)=color;
      *(pbPix++)=color;
      }
    }
  pbPixels+=pitch;
  pbPixels+=pitch;
  }
E_EndPaint();
}

void DrawScreen()
{
if (bInteractive)
    {
    DrawTextScreen();
    return;
    }
switch (vmVideoMode)
  {
  case vmText30:
  case vmText40:
  case vmText60:
    DrawTextScreen();
    break;
  case vmGraph512:
    DrawTextScreenInGraph();
    break;
  case vmGraph800:
    if (cModel==MODEL_O)
      DrawScreenOrion();
    else if (cModel==MODEL_S)
      DrawScreenSpec();
  };
}
