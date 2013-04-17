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

// sound.h

// Библиотека поддержки эмулятора.
// Функции работы со звуком.

#ifndef _SOUND_H
#define _SOUND_H

enum SS_MODE {ssIn, ssOut};

//extern "C" int ResetDSP();
extern "C" void StartSound(/*SS_MODE ssMode*/);
extern "C" void InitAudio();
extern "C" void ExitAudio();
extern "C" void StopSound();
//extern "C" void PlayByte(unsigned char b);
extern "C" void PlayByteNoWait(unsigned char b);
//extern "C" unsigned char ReadByteNoWait();
extern "C" void WaitSound();
extern "C" int bWaitSnd;
//extern "C" int bSBPresent;

extern "C" int bWaitSnd;

#endif
