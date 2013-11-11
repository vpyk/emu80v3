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

#include "emuinit.h" //!!!
#include "emu80.h"   //!!!

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
    wReg&=3;
    switch(wReg)
    {
    case 0:
        bPortA = bValue;
        break;
    case 1:
        bPortB = bValue;
        break;
    case 2:
        bPortC = bValue;
        break;
    default: //ctrl reg
        if (!(bValue&0x80))
        {
            int nBit=(bValue&0x0e)>>1;
            uint8_t bMask = ~(1<<nBit);
            bPortC&=~bMask;
            bPortC=(bValue&1)<<nBit;
        }
    }
}

uint8_t TPPI8255::ReadReg(uint16_t wReg)
{
    wReg&=3;
    switch(wReg)
    {
    case 0:
        return bPortA;
    case 1:
        return bPortB;
    case 2:
        return bPortC;
    default: // ctrl reg
        return 0xFF; // undefined
    }
}

void TPPI8255::ProcessTime(int nTicks)
{
    return;
}

// --- Temporary Stubs for ASM ---------------------------------------------

static TPPI8255 *PPPI = NULL;
static TPPI8255 *PPPI2 = NULL;

void InitPPI()
{
    if (!PPPI)
        PPPI = new TPPI8255;
    else
        PPPI->InitDevice();

    if (!PPPI2)
        PPPI2 = new TPPI8255;
    else
        PPPI2->InitDevice();

}

void WritePPIReg(uint16_t wReg, uint8_t bValue)
{
    PPPI->WriteReg(wReg, bValue);
    switch (cModel)
    {
    case MODEL_R:
    case MODEL_M:
    case MODEL_A:
    case MODEL_O:
        // rk clones code here
        break;
    case MODEL_S:
        // specialist code here
        ;
    }
}

void WritePPI2Reg(uint16_t wReg, uint8_t bValue)
{
    PPPI2->WriteReg(wReg, bValue);
    switch (cModel)
    {
    case MODEL_M:
        // mikrosha code here
        break;
    case MODEL_O:
        // orion code here
        ;
    }
}

uint8_t ReadPPI2Reg(uint16_t wReg)
{
    switch (cModel)
    {
    case MODEL_M:
        // mikrosha code here
        break;
    case MODEL_O:
        // orion code here
        ;
    }
    return PPPI2->ReadReg(wReg);
}
