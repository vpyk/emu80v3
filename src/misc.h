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

// misc.h

// Разные функции

#ifndef _MISC_H
#define _MISC_H

#include "i8080.h"

extern "C" void * SaveScr();
extern "C" void RestScr(void * pBuf);

void SetExePath(char *szPath);
FILE * exepath_fopen(const char *szName, const char *szMode);
unsigned char TranslateScanCode(unsigned char ucScanCode);

extern TCPU8080 *PCPU;

extern "C" void CloseTapeFile();
extern "C" void CloseTapeFileDelay();
extern "C" void OpenTapeFileWrite();
extern "C" void OpenTapeFileRead();
extern "C" void WriteTapeFile(int nByte);
extern "C" int ReadTapeFile();
extern "C" int ReadTapeFileSync();


void ClearMem();
void InitOnce();
void LoadROM();

#endif
