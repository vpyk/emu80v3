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

// sdllayer.h

// Библиотека поддрежки эмулятора.
// Все вызовы SDL осуществляются только из этого модуля,
// остальные модули пользуются функциями, описанными здесь.

#ifndef _SDLLAYER_H
#define _SDLLAYER_H

#include <SDL/SDL.h>

enum TVideoMode {vmNone=-1, vmText30=0, vmText40=1, vmText60=2, vmGraph512=3, vmGraph800=4};

extern "C" char *pcScreenBuf;

extern "C" short ReadScan();
extern "C" void SetCursorPos(int x, int y);
extern "C" void PrintChar(long ch);
extern "C" void SaveCursorPos();
extern "C" void RestoreCursorPos();
extern "C" void SetTextMode();          //устанавливает текст. режим при необх.
extern "C" void EnterInteractiveMode();
extern "C" void LeaveInteractiveMode();
extern "C" void SetVideoMode(TVideoMode vm);
extern "C" void ClearScreen();
extern "C" void CheckKeys();
extern "C" void DrawScreen();
extern "C" void DrawTextScreen();
extern "C" void DrawScreenOrion();
extern "C" void DrawScreenSpec();
extern "C" void Delay(int nMS);
extern "C" void ToggleFullScreen();
extern "C" void ClearTimer();
extern "C" int  GetTimer();

void InitEGraph();
void QuitEGraph();
void HideCursor();
void GetCursorPos(int &x, int &y);
unsigned char * E_BeginPaint();
void E_EndPaint();
int GetPitch();
int IsInInteractiveMode();

extern int bFullScreen;

#endif
