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
    bPortA=0;
    bPortB=0;
    bPortC=0;
    bPortAInput=0;
    bPortBInput=0;
    bPortCInput=0;
    pcmChAMode=pcmIn;
    pcmChBMode=pcmIn;
    pcmChCHiMode=pcmIn;
    pcmChCLowMode=pcmIn;
}

void TPPI8255::WriteReg(uint16_t wReg, uint8_t bValue)
{
    wReg&=3;
    switch(wReg)
    {
    case 0:
        if (pcmChAMode==pcmOut)
            bPortA = bValue;
        break;
    case 1:
        if (pcmChBMode==pcmOut)
            bPortB = bValue;
        break;
    case 2:
        if (pcmChCHiMode==pcmOut)
            bPortC = bPortC&0x0F | bValue&0xF0;
        if (pcmChCLowMode==pcmOut)
            bPortC = bPortC&0xF0 | bValue&0x0F;
        break;
    default: //ctrl reg
        if (!(bValue&0x80))
        {
            int nBit=(bValue&0x0e)>>1;
            if ((nBit<4 && pcmChCLowMode==pcmOut) || (nBit>=4 && pcmChCHiMode==pcmOut))
            {
                uint8_t bMask = ~(1<<nBit);
                bPortC&=bMask;
                bPortC|=((bValue&1)<<nBit);
            }
        }
        else if (!(bValue&0x40)) // установка режима, недвунаправленный режим        {
        {
            bPortA=0x0;
            bPortB=0x0;
            bPortC=0x0;
            pcmChAMode=bValue&0x10?pcmIn:pcmOut;
            pcmChBMode=bValue&0x02?pcmIn:pcmOut;
            pcmChCHiMode=bValue&0x08?pcmIn:pcmOut;
            pcmChCLowMode=bValue&0x01?pcmIn:pcmOut;
            // режимы 0 и 1 пока не различаются
        }
        else // двунаправленный режим, пока просто сброс значений регистров
        {
            bPortA=0x0;
            bPortB=0x0;
            bPortC=0x0;
        }

    }
}

uint8_t TPPI8255::ReadReg(uint16_t wReg)
{
    uint8_t bByte;

    wReg&=3;
    switch(wReg)
    {
    case 0:
        return pcmChAMode==pcmOut?bPortA:bPortAInput;
    case 1:
        return pcmChBMode==pcmOut?bPortB:bPortBInput;
    case 2:
        bByte=(pcmChCHiMode==pcmOut?bPortC:bPortCInput)&0xF0;;
        bByte|=((pcmChCLowMode==pcmOut?bPortC:bPortCInput)&0x0F);;
        return bByte;
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
    PerformPPIOutput();
}

uint8_t ReadPPIReg(uint16_t wReg)
{
    PerformPPIInput();
    return PPPI->ReadReg(wReg);
}

void PerformPPIInput()
{
    int i;
    uint8_t bMatrixIn, bMatrixOut;
    uint16_t wMatrixIn, wMatrixOut;

    switch (cModel)
    {
    case MODEL_R:
    case MODEL_A:
    case MODEL_O:
    case MODEL_P:
        // rk clones code here
        bMatrixIn=0xFF;
        bMatrixOut=PPPI->GetPortA();
        for (int i=0;i<8;i++)
        {
            if (!(bMatrixOut&1))
                bMatrixIn&=key_bytes[i];
            bMatrixOut>>=1;
        }
        PPPI->SetPortBInput(bMatrixIn);
//        port_c=PPPI->GetPortC()&0x0F | ctrl_keys&0xF0; //!!!
        PPPI->SetPortCInput(ctrl_keys&0xF0 | PPPI->GetPortC()&0x0F);
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
        PPPI->SetPortAInput(bMatrixIn);
//        port_c=port_c&0x0f | ctrl_keys&0xF0; //!!!
        PPPI->SetPortCInput(ctrl_keys&0xF0 | PPPI->GetPortC()&0x0F);
        break;
    case MODEL_S:
        // specialist code here
        if (PPPI->GetChAMode()==pcmOut && PPPI->GetChBMode()==pcmIn && PPPI->GetChCLowMode()==pcmOut)
        {
            bMatrixIn=0x00;
            wMatrixOut=((PPPI->GetPortC()&0x0F)<<8) | PPPI->GetPortA(); // port_ac_s
            for (int i=0;i<6;i++)
            {
                bMatrixIn>>=1;
                if ((key_bytes_s[i] | wMatrixOut)==0xFFF)
                    bMatrixIn|=0x80;
            }
            PPPI->SetPortBInput(bMatrixIn | ctrl_keys_s&0x03);
        }
        else if (PPPI->GetChAMode()==pcmIn && PPPI->GetChBMode()==pcmOut && PPPI->GetChCLowMode()==pcmIn)
        {
            wMatrixIn=0xFFF;
            bMatrixOut=PPPI->GetPortB()>>2;
            for (int i=0;i<6;i++)
            {
                if (!(bMatrixOut&1))
                    wMatrixIn&=key_bytes_s[i];
                bMatrixOut>>=1;
            }
            PPPI->SetPortAInput(wMatrixIn&0xFF);
            PPPI->SetPortCInput((wMatrixIn&0xF00)>>8);
        }
    }
}

void PerformPPIOutput()
{
    switch (cModel)
    {
    case MODEL_R:
    case MODEL_A:
    case MODEL_O:
    case MODEL_P:
        // rk clones code here
        port_a_val=PPPI->GetPortA();
        port_c=(PPPI->GetPortCInput()&0xF0) | (PPPI->GetPortC()&0x0F);
        led_state=(led_state&0xFB)|((PPPI->GetPortC()&0x08)>>1);
        break;
    case MODEL_M:
        // mikrosha code here
        port_a_val=PPPI->GetPortB();
        port_c=(PPPI->GetPortCInput()&0xF0) | (PPPI->GetPortC()&0x0F);
        led_state=(led_state&0xFB)|((PPPI->GetPortC()&0x08)>>1);
        break;
    case MODEL_S:
        // specialist code here
        port_c=PPPI->GetPortC()&0xF0;
        port_ac_s=PPPI->GetPortA()|((PPPI->GetPortC()&0x0F)<<8);
        snd_state=PPPI->GetPortC()&0xF0;
        cur_color_code=color_table[(PPPI->GetPortC()&0xC0)>>6];
    }
}

uint8_t PPIGetPortC()
{
switch (cModel)
    {
    case MODEL_R:
    case MODEL_A:
    case MODEL_O:
    case MODEL_P:
        return PPPI->GetPortC()&0x0F;
    case MODEL_S:
        return PPPI->GetPortC()&0xF0;
    }
}

void PPISetPortC(uint8_t bValue)
{
    PPPI->SetPortC(bValue);
}

void WritePPI2Reg(uint16_t wReg, uint8_t bValue)
{
    PPPI2->WriteReg(wReg, bValue);
    switch (cModel)
    {
    case MODEL_M:
        mikr_symg=PPPI2->GetPortB() & 0x80;
        break;
    case MODEL_O:
        romdisk_addr=PPPI2->GetPortB() | (PPPI2->GetPortC()<<8);
    }
}

uint8_t ReadPPI2Reg(uint16_t wReg)
{
    switch (cModel)
    {
    case MODEL_R:
        PPPI2->SetPortAInput(joy_state);
        break;
    case MODEL_O:
        // orion code here
        PPPI2->SetPortAInput(romdisk_mem[romdisk_addr]);
    }
    return PPPI2->ReadReg(wReg);
}
