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

// machine.cpp
//

#include "rk86.h"
#include "mem_plain.h"

TRK86::TRK86()
{
PMemory = new TPlainMem;
}

TRK86::~TRK86()
{
delete PMemory;
}

uint8_t TRK86::ReadPort(uint8_t wPortAddr)
{
    return ReadMem((wPortAddr<<8)|wPortAddr);
}

void TRK86::WritePort(uint8_t wPortAddr, uint8_t bValue)
{
    WriteMem((wPortAddr<<8)|wPortAddr, bValue);
}

uint8_t TRK86::ReadMem(uint16_t wAddr)
{
    // if wAddr ... проверки на попадание в порты
    // else
    return PMemory->ReadMem(wAddr);
}

void TRK86::WriteMem(uint16_t wAddr, uint8_t bValue)
{
    // if wAddr ... проверки на попадание в порты
    // else
    PMemory->WriteMem(wAddr, bValue);
}
