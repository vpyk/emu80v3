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

// io_vt57.cpp

// Реализация контроллера DMA КР580ВТ57

#include <stdint.h>
#include <stdlib.h>
#include <mem.h>
//#include "sdllayer.h"
#include "emu80.h"
#include "asm.h"
#include "emuinit.h"
#include "io_vt57.h"

TDMA8257::TDMA8257()
{
    InitDevice();
}

/*TDMA8257::TDMA8257(TFDCType dtDMAType)
{
    InitDevice();
    ftType=fdDMAType;
}*/

/*void TDMA8257::SetDMAType(TDMAType fdDMAType)
{
    dtType=fdDMAType;
}*/

void TDMA8257::InitDevice()
{
wAddr1=wAddr2=wAddr3=wLen1=wLen2=wLen3=0;
blLoByte=true;
}

void TDMA8257::WriteReg(uint16_t wReg, uint8_t bValue)
{
    wReg&=15;
    switch(wReg)
    {
    case 0:
        if (blLoByte) wAddr0=(wAddr0&0xFF00) | bValue; else wAddr0=(wAddr0&0xFF) | (bValue<<8);
        blLoByte=!blLoByte;
        break;
    case 1:
        if (blLoByte) wLen0=(wLen0&0xFF00) | bValue; else wLen0=(wLen0&0xFF) | (bValue<<8);
        blLoByte=!blLoByte;
        break;
    case 2:
        if (blLoByte) wAddr1=(wAddr1&0xFF00) | bValue; else wAddr1=(wAddr1&0xFF) | (bValue<<8);
        blLoByte=!blLoByte;
        break;
    case 3:
        if (blLoByte) wLen1=(wLen1&0xFF00) | bValue; else wLen1=(wLen1&0xFF) | (bValue<<8);
        blLoByte=!blLoByte;
        break;
    case 4:
        if (blLoByte) wAddr2=(wAddr2&0xFF00) | bValue; else wAddr2=(wAddr2&0xFF) | (bValue<<8);
        blLoByte=!blLoByte;
        break;
    case 5:
        if (blLoByte) wLen2=(wLen2&0xFF00) | bValue; else wLen2=(wLen2&0xFF) | (bValue<<8);
        blLoByte=!blLoByte;
        break;
    case 6:
        if (blLoByte) wAddr3=(wAddr3&0xFF00) | bValue; else wAddr3=(wAddr3&0xFF) | (bValue<<8);
        blLoByte=!blLoByte;
        break;
    case 7:
        if (blLoByte) wLen3=(wLen3&0xFF00) | bValue; else wLen3=(wLen3&0xFF) | (bValue<<8);
        blLoByte=!blLoByte;
        break;
    case 8:
        bModeReg=bValue;
        blLoByte=true;
        break;
    default:
        break;
    }
}

uint8_t TDMA8257::ReadReg(uint16_t wReg)
{
    uint8_t bValue;
    wReg&=15;
    switch(wReg)
    {
    case 0:
        if (blLoByte) bValue=(wAddr0&0xFF); else bValue=(wAddr0&0xFF00)>>8;
        blLoByte=!blLoByte;
        break;
    case 1:
        if (blLoByte) bValue=(wLen0&0xFF); else bValue=(wLen0&0xFF00)>>8;
        blLoByte=!blLoByte;
        break;
    case 2:
        if (blLoByte) bValue=(wAddr1&0xFF); else bValue=(wAddr1&0xFF00)>>8;
        blLoByte=!blLoByte;
        break;
    case 3:
        if (blLoByte) bValue=(wLen1&0xFF); else bValue=(wLen1&0xFF00)>>8;
        blLoByte=!blLoByte;
        break;
    case 4:
        if (blLoByte) bValue=(wAddr2&0xFF); else bValue=(wAddr2&0xFF00)>>8;
        blLoByte=!blLoByte;
        break;
    case 5:
        if (blLoByte) bValue=(wLen2&0xFF); else bValue=(wLen2&0xFF00)>>8;
        blLoByte=!blLoByte;
        break;
    case 6:
        if (blLoByte) bValue=(wAddr3&0xFF); else bValue=(wAddr3&0xFF00)>>8;
        blLoByte=!blLoByte;
        break;
    case 7:
        if (blLoByte) bValue=(wLen3&0xFF); else bValue=(wLen3&0xFF00)>>8;
        blLoByte=!blLoByte;
        break;
    case 8:
        // read status
        break;
    default:
        bValue=0xFF;
    }
return bValue;
}

void TDMA8257::ProcessTime(int nTicks)
{
    return;
}

void TDMA8257::GetBlock(int nChannel, int nLen, uint8_t *pbBuf)
{
/*    memset(pbBuf, 0xFF, nLen);

    // пока реализована только с контроллером CRT на канале 3
    if (nChannel!=3)
        return;
    if (!bModeReg&0x80)
        return;
    if ((wLen3&0xC000)>>14!=dctRead)
        return;
*/
    return;
}

uint16_t TDMA8257::GetChAddr(int nChannel)
{
    switch(nChannel)
    {
    case 0:
        return wAddr0;
    case 1:
        return wAddr1;
    case 2:
        return wAddr2;
    case 3:
        return wAddr3;
    }
}

uint16_t TDMA8257::GetChLen(int nChannel)
{
    switch(nChannel)
    {
    case 0:
        return wLen0 & 0x3FFF;
    case 1:
        return wLen1 & 0x3FFF;
    case 2:
        return wLen2 & 0x3FFF;
    case 3:
        return wLen3 & 0x3FFF;
    }
}

uint8_t TDMA8257::GetMR()
{
    return bModeReg;
}

/*void TDMA8257::SaveDMAState(RKSS_DMA_STATE *dmaState)
{
}*/

/*void TDMA8257::LoadDMAtate(RKSS_DMA_STATE *dmaState)
{
}*/

// --- Temporary Stubs for ASM ---------------------------------------------

static class TDMA8257 *PDMA = NULL;

void InitDMA()
{
    if (!PDMA)
        PDMA = new TDMA8257();
    else
    {
        PDMA->InitDevice();
    }
}

void WriteDMAReg(uint16_t wReg, uint8_t bValue)
{
    PDMA->WriteReg(wReg, bValue);
    dma_begadr=PDMA->GetChAddr(2);
    dma_len=PDMA->GetChLen(2);
    dma_mr=PDMA->GetMR();
}

uint8_t ReadDMAReg(uint16_t wReg)
{
    return PDMA->ReadReg(wReg);
}

/*void LoadDMAState(RKSS_DMA_STATE *dmaState)
{
    PDMA->LoadDMAState(dmaState);
}

void SaveDMAState(RKSS_DMA_STATE *dmaState)
{
    PDMA->SaveDMAState(dmaState);
}*/
