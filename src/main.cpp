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

// main.cpp

// Главный модуль. Функция main(), основной цикл SDL

#include <string.h>
#include <stdlib.h>

#include "main.h"

#include "emu80.h"
#include "asm.h"
#include "sdllayer.h"
#include "emuinit.h"
#include "misc.h"

int main(int argc, char *argv[])
{
SetExePath(argv[0]);

InitOnce();
SetDefParams();

//AnalizeOptions(argc, argv);
LoadCfgParams();

InitEGraph();

MainFunc(argc, argv);

QuitEGraph();
}

void HandleEvent(SDL_Event *event)
{
if (event->type==SDL_QUIT)
  {
  f_quit=1;
  if (IsInInteractiveMode())
    exit(0); //!!!
  }
else if (event->type==SDL_ACTIVEEVENT)
  { // Потеря фокуса - сброс состояния клавиатуры во избеж. залипания клавиш
  if (event->active.gain==0 && event->active.state & SDL_APPINPUTFOCUS)
    FlushKeys();
  }
else if (event->type == SDL_KEYDOWN)
  if (IsInInteractiveMode() && (event->key.keysym.mod & KMOD_ALT) &&
      ((event->key.keysym.sym==SDLK_RETURN || event->key.keysym.sym==SDLK_KP_ENTER)))
        ToggleFullScreen();
//........
}

// cScanCode - старший бит=1 при отжатии
void ProcessKeyboard(unsigned char cScanCode)
{
//if (cScanCode==0x46)
//  ToggleFullScreen();
//if (cScanCode==1)
//  f_quit=1;
cScanCode=TranslateScanCode(cScanCode);
//printf(">%x\n",(unsigned)cScanCode);
ex_int_09((unsigned short)cScanCode);
}
