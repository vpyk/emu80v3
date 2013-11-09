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

// io_vv55.cpp

// Реализация программируемого перефирийного интерфейса КР580ВВ55

#include <stdint.h>
#include <stdlib.h>

#include "io_vv55.h"

TPPI8255::TPPI8255()
{
    //ctor
}

TPPI8255::~TPPI8255()
{
    //dtor
}

void TPPI8255::InitDevice()
{
    //init
}

void TPPI8255::WriteReg(uint16_t wReg, uint8_t bValue)
{
    //write reg
}

uint8_t TPPI8255::ReadReg(uint16_t wReg)
{
    //read reg
}

void TPPI8255::ProcessTime(int nTicks)
{
    return;
}

// --- Temporary Stubs for ASM ---------------------------------------------

static TPPI8255 *PPPI = NULL;

void InitPPI()
{
    if (!PPPI)
        PPPI = new TPPI8255;
    else
        PPPI->InitDevice();
}

