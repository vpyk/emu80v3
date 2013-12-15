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
#include "asm.h"     //!!!

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
        else  // установка режима, пока просто сброс значений регистров
        {
            bPortA=0;
            bPortB=0;
            bPortC=0;
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
    case MODEL_A:
    case MODEL_O:
        // rk clones code here
        port_a_val=PPPI->GetPortA();
        port_c=PPPI->GetPortC();
        led_state=(led_state&0xFB)|((PPPI->GetPortC()&0x08)>>1);
        break;
    case MODEL_M:
        // mikrosha code here
        port_a_val=PPPI->GetPortB();
        port_c=PPPI->GetPortC();
        break;
    case MODEL_S:
        // specialist code here
        port_c=PPPI->GetPortC();
        port_ac_s=PPPI->GetPortA()|((PPPI->GetPortC()&0x0F)<<8);
        ctrl_keys_s=PPPI->GetPortB()&0x03;
        snd_state=PPPI->GetPortC()&0x0F;
        cur_color_code=color_table[(PPPI->GetPortC()&0xC0)>>6];
    }
}

uint8_t ReadPPIReg(uint16_t wReg)
{
    int i;
    uint8_t bMatrixIn, bMatrixOut;
    uint16_t wMatrixIn, wMatrixOut;

    switch (cModel)
    {
    case MODEL_R:
    case MODEL_A:
    case MODEL_O:
        // rk clones code here
        bMatrixIn=0xFF;
        bMatrixOut=PPPI->GetPortA();
        for (int i=0;i<8;i++)
        {
            if (!(bMatrixOut&1))
                bMatrixIn&=key_bytes[i];
            bMatrixOut>>=1;
        }
        PPPI->SetPortB(bMatrixIn);
        break;
    case MODEL_M:
        // mikrosha code here
        bMatrixIn=0xFF;
        bMatrixOut=PPPI->GetPortB();
        for (int i=0;i<8;i++)
        {
            if (!(bMatrixOut&1))
                bMatrixIn&=key_bytes[i];
            bMatrixOut>>=1;
        }
        PPPI->SetPortA(bMatrixIn);
        break;
    case MODEL_S:
        // specialist code here
        bMatrixIn=0x00;
        wMatrixOut=((PPPI->GetPortC()&0x0F)<<8) & PPPI->GetPortA(); // port_ac_s
        for (int i=0;i<8;i++)
        {
            wMatrixIn>>=1;
            if (key_bytes_s[i]==0xFFF)
                wMatrixIn|=0x80;
        }
        PPPI->SetPortB(bMatrixIn|(PPPI->GetPortB()&0x03)); //ctrl_keys_s;
    }
    return PPPI2->ReadReg(wReg);
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
