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

// menus.cpp

// Библиотека поддержки эмулятора.
// Функции работы с меню и диалогами.

#include <string.h>

#include "menus.h"
#include "sdllayer.h"
#include "misc.h"

static void DrawHLine(int x1, int x2, int y);
static void DrawVLine(int x, int y1, int y2);
static void DrawStr(int x, int y, char *s);
static void DrawFrame();
static unsigned GetChar();

void DrawHLine(int x1, int x2, int y)
{
char *p=pcScreenBuf+y*160+x1*2;
for (int i=0;i<=x2-x1;i++,p+=2)
  *p='Н';
}

void DrawHLine1(int x1, int x2, int y)
{
char *p=pcScreenBuf+y*160+x1*2;
for (int i=0;i<=x2-x1;i++,p+=2)
  *p='Д';
}

void DrawVLine(int x, int y1, int y2)
{
char *p=pcScreenBuf+y1*160+x*2;
for (int i=0;i<=y2-y1;i++,p+=160)
  *p='є';
}

void DrawStr(int x, int y, char *s)
{
char *p=pcScreenBuf+y*160+x*2;
while (*s)
  {
  *p=*(s++);
  p+=2;
  }
}

void DrawStrCol(int x, int y, char *s, char col)
{
char *p=pcScreenBuf+y*160+x*2;
while (*s)
  {
  *p=*(s++);
  *(p+1)=col;
  p+=2;
  }
}

void HighlightRect(int x1, int x2, int y1, int y2, int color)
{
int i,j;
char *p=pcScreenBuf+y1*160+x1*2+1;
for (i=0;i<=y2-y1;i++,p+=(160+x1+x1-x2-x2-2))
  for (j=0;j<=x2-x1;j++,p+=2)
    *p=color;
}

void FillRect(int x1, int x2, int y1, int y2, int ch)
{
int i,j;
char *p=pcScreenBuf+y1*160+x1*2;
for (i=0;i<=y2-y1;i++,p+=(160+x1+x1-x2-x2-2))
  for (j=0;j<=x2-x1;j++,p+=2)
    *p=ch;
}

void DrawChar(int x, int y, char ch)
{
pcScreenBuf[y*160+x*2]=ch;
}

static void * pScr;
static int nSavedCurX, nSavedCurY;

void BeginDialog()
{
pScr=SaveScr();
GetCursorPos(nSavedCurX, nSavedCurY);
SetCursorPos(255,255);
}

void EndDialog()
{
RestScr(pScr);
SetCursorPos(nSavedCurX, nSavedCurY);
}

unsigned GetChar()
{
return ReadScan();
}

void DrawFrame(int x, int y, int w, int h, char *szHint, char *szHeader,
               int bDrawScroll=0)
{
FillRect(x,x+w-1,y,y+h-1,0);
DrawHLine(x+1,x+w-2,y);
DrawHLine(x+1,x+w-2,y+h-1);
DrawVLine(x,y+1,y+h-2);
DrawVLine(x+w-1,y+1,y+h-2);
DrawHLine1(x+1,x+w-2,y+h-3);
DrawStr(x,y,"Й");
DrawStr(x,y+h-1,"И");
DrawStr(x+w-1,y,"»");
DrawStr(x+w-1,y+h-1,"ј");
DrawStr(x,y+h-3,"З");
DrawStr(x+w-1,y+h-3,"¶");
HighlightRect(x,x+w-1,y,y+h-1,0x1e);
HighlightRect(x+1,x+w-2,y+1,y+h-4,0x1b);
DrawStrCol(x+(w-strlen(szHint))/2,y+h-2,szHint,0x1a);
DrawStrCol(x+(w-strlen(szHeader))/2,y,szHeader,0x1a);
if (bDrawScroll)
  {
  DrawChar(x+w-1,y+1,0x0b);
  DrawChar(x+w-1,y+h-4,0x0f);
  }
}

void DrawLines(char **ppcMes, int nLines, int nOffset, int nMaxWidth, int xLeft, int yTop,
               int (*fnGetColor)(char *szName)=0)
{
for (int i=0;i<nLines;i++)
  {
  unsigned char col=0x1b;
  if (fnGetColor)
    col=(*fnGetColor)(ppcMes[i+nOffset]);
  if (nMaxWidth==0 || strlen(ppcMes[i+nOffset])<=(nMaxWidth-4))
    DrawStrCol(xLeft+2,yTop+1+i,ppcMes[i+nOffset],col);
  else
    {
    char *szTempStr = new char[nMaxWidth-3];
    strncpy(szTempStr, ppcMes[i+nOffset], nMaxWidth-7);
    szTempStr[nMaxWidth-7]='\0';
    strcat(szTempStr, "..."); //???
    DrawStrCol(xLeft+2,yTop+1+i,szTempStr,col);
    delete[] szTempStr;
    }
  }
}

int ShowMessage(char **ppcMes, int nLines,
                char *szHeader, int bChoice, int nCurChoice,
                int bNoSwap, int nMaxWidth, int nMaxHeight,
                int (*fnGetColor)(char *szName))
{
if (!bNoSwap) nMaxHeight=0;
int res;
BeginDialog();
int nWidth=0;
int nLen;
int i;
for (i=0;i<nLines;i++)
  {
  nLen=strlen(ppcMes[i]);
  if (nLen>nWidth)
    nWidth=nLen;
  }
nWidth+=4;
if (nMaxWidth!=0 && nWidth>nMaxWidth)
  nWidth=nMaxWidth;
if (nWidth<17)
  nWidth=17;
if (bChoice && nWidth<25)
  nWidth=25;
int nMaxOffset=0;
int nHeight=nLines+4;
if (nMaxHeight!=0 && nMaxHeight<nHeight)
  {
  nMaxOffset=nHeight-nMaxHeight;
  nHeight=nMaxHeight;
  }
int nVisLines=nHeight-4;
int nCurOffset=0;
int nPrevOffset=0;
int xLeft=(80-nWidth)/2;
int yTop=(30-nHeight)/2;
DrawFrame(xLeft, yTop, nWidth, nHeight,
          (char *)(bChoice?" Enter-ўлЎ®а  ESC-®в¬. ":" ESC - ў®§ўа в "),
          szHeader, bChoice&&(nMaxOffset!=0));
DrawLines(ppcMes, nVisLines, nCurOffset, nMaxWidth, xLeft, yTop, fnGetColor);
if (bChoice)
  {
  int key;
  res=-1;
  if (nCurChoice>nVisLines-1)
    {
    nCurOffset=nCurChoice-nVisLines+1;
    nCurChoice=nVisLines-1;
    }
  for(;;)
    {
    DrawFrame(xLeft, yTop, nWidth, nHeight,
              (char *)(bChoice?" Enter-ўлЎ®а  ESC-®в¬. ":" ESC - ў®§ўа в "),
              szHeader, bChoice&&(nMaxOffset!=0));
    DrawLines(ppcMes, nVisLines, nCurOffset, nMaxWidth, xLeft, yTop, fnGetColor);
    HighlightRect(xLeft+2,xLeft+nWidth-3,
                  yTop+1+nCurChoice,yTop+1+nCurChoice,fnGetColor?fnGetColor(ppcMes[nCurChoice+nCurOffset])|0x30:0x3e);
    key=GetChar();
    if ((key & 0xff)==0x0d)
      {
      res=nCurChoice+nCurOffset;
      break;
      }
    else if ((key & 0xff)==0x1b)
      break;
    switch (key)
      {
      case 0x4800: // Up
        if (bNoSwap)
          {if (nCurChoice!=0)
            nCurChoice--;
           else if (nCurOffset!=0)
            nCurOffset--;}
        else  
          nCurChoice=(nCurChoice+nLines-1)%nLines;
        break;
      case 0x5000: //Down
        if (bNoSwap)
          {if (nCurChoice!=nVisLines-1)
            nCurChoice++;
           else if (nCurOffset!=nMaxOffset)
            nCurOffset++;}
        else
          nCurChoice=(nCurChoice+1)%nLines;
        break;
      case 0x4900: // PgUp
        if (!bNoSwap)
          break;
        if (nCurOffset>nVisLines)
            nCurOffset-=nVisLines;
        else
          {
          nCurOffset=0;
          nCurChoice=0;
          }        
        break;
      case 0x5100: //PgDn
        if (!bNoSwap)
          break;
        if (nCurOffset+nVisLines<nLines-nVisLines)
            nCurOffset+=nVisLines;
        else
          {
          nCurOffset=nLines-nVisLines;
          nCurChoice=nVisLines-1;
          }        
      }
    }
  }
else
  res=GetChar();
EndDialog();
return res;
}

