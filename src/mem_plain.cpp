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

// mem_plain.cpp

// Реализация плоской модели памяти

#include <mem.h>

#include "mem_plain.h"

TPlainMem::TPlainMem()
{
pbMemBuf = new uint8_t[65536];
}

TPlainMem::~TPlainMem()
{
delete[] pbMemBuf;
}

uint8_t TPlainMem::ReadMem(uint16_t wAddr)
{
return pbMemBuf[wAddr];
}

void TPlainMem::WriteMem(uint16_t wAddr, uint8_t bValue)
{
pbMemBuf[wAddr]=bValue;
}

void TPlainMem::Clear()
{
memset(pbMemBuf,65536,1);
}

