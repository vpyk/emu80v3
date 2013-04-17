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

// emu80.h

#ifndef _EMU80_H
#define _EMU80_H

#include "rss.h"

#define SPEED_NORM 2
#define SPEED_MEDIUM 1
#define SPEED_HIGH 0

#define MEM_NONE 0
#define MEM_W 1
#define MEM_RW 2

#define HOOKF809_YES 1
#define HOOKF809_NO 0

#define SCR_AUTO 2
#define SCR_LOW 0
#define SCR_HIGH 1
#define SCR_GRAPH 3

#define FONT_OEM 0
#define FONT_RK 1

#define REFR_RT 0
#define REFR_50 1
#define REFR_COMB 2

#define SND_ON 0
#define SND_OFF 1
#define SND_SB 2

#define TAPE_FILE 0
#define TAPE_SBOUT 1
#define TAPE_SBIN 2

#define SYSREQ_MENU 0
#define SYSREQ_RESET 1
#define SYSREQ_MODEL 2
#define SYSREQ_SAVESS 3
#define SYSREQ_LOADSS 4
#define SYSREQ_HELP 5
#define SYSREQ_QUIT 6
#define SYSREQ_MODELNOCLEAR 7
#define SYSREQ_DISKA 8
#define SYSREQ_DISKB 9
#define SYSREQ_ROM 10
#define SYSREQ_SPEEDUP 11
#define SYSREQ_SPEEDDOWN 12
#define SYSREQ_PAUSE 13
#define SYSREQ_FULLSCREEN 14
#define SYSREQ_ABOUT 15

struct MENU_ITEM;

struct MENU_LINE
  {
  char cValue;
  MENU_LINE * pmnNext;
  MENU_LINE * pmnPrev;
  MENU_ITEM * pmiItem;
  } __attribute__ ((packed));

struct MENU_ITEM
  {
  char cValue;
  char x;
  char y;
  MENU_ITEM * pmiNext;
  MENU_ITEM * pmiPrev;
  MENU_LINE * pmnNext;
  MENU_LINE * pmnPrev;
  char cDisabled;
  } __attribute__ ((packed));

struct RKSS_EMUL_HEADER
  {
  unsigned long dwSig;
  unsigned short wHeaderLen;
  unsigned char cSpeed;
  unsigned char cMem;
  unsigned char cHookF809;
  unsigned char cScreen;
  unsigned char cFont;
  unsigned char cSound;
  unsigned char cTape;
  unsigned char cRefr;
  } __attribute__ ((packed));

#endif
