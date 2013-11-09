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

// io_vi53.cpp

// Реализация контроллера FDD КР580ВГ93

#include <stdint.h>
#include <stdlib.h>
//#include "sdllayer.h"
#include "emu80.h"
#include "asm.h"
#include "emuinit.h"
#include "io_vg93.h"

TFDC1793::TFDC1793(TFDCType ftFDCType)
{
    InitDevice();
    ftType=ftFDCType;
}

void TFDC1793::SetFDCType(TFDCType ftFDCType)
{
    ftType=ftFDCType;
}

void TFDC1793::InitDevice()
{
    wDisk=0;
    wHead=0;
    wCylinder=0;
    wSector=0;
    wSectOfs=0;
    foOperation=foIdle;
    sdDirection=sdIn;
}

void TFDC1793::WriteReg(uint16_t wReg, uint8_t bValue)
{
    if (ftType==ftOrion && (wReg&0x24)) // Запись в решистр управления "Ориона"
    { // AFAIR 0x24 - для двух типов подключения (0x20 и 0x04)
        wDisk=bValue&0x01;
        wHead=(bValue&0x10)>>8;
    }
    else
    {
        wReg&=0x3;
        switch (wReg)
        {
        case 0: // command reg
            bValue=(bValue&0xF0>>4);
            switch (bValue)
            {
            case 0: // restore
                wCylinder=0;
                wSectOfs=0;
                foOperation=foIdle;
                sdDirection=sdIn;
                break;
            case 1: // seek
                if (bDataReg<80)
                    wCylinder=bDataReg;
                break;
            case 2: // step
            case 3:
                if (sdDirection==sdIn && wCylinder<80)
                    wCylinder++;
                else if (sdDirection==sdIn && wCylinder>=0)
                    wCylinder--;
                break;
            case 4: // step in
            case 5:
                sdDirection=sdIn;
                if (wCylinder<90)
                    wCylinder++;
                break;
            case 6: // step out
            case 7:
                sdDirection=sdOut;
                if (wCylinder>=0)
                    wCylinder--;
                break;
            case 8: // read sector
            case 9:
                foOperation=foRead;
                wSectOfs=0;
                ReadSector(wDisk, wCylinder, wHead, wSector, aabSectorBufs[wDisk]);
                break;
            case 0xA: // write sector
            case 0xB:
                foOperation=foWrite;
                wSectOfs=0;
                break;
            }
        case 1: // track reg
            wCylinder=bValue;
            break;
        case 2: // sector reg
            wSector=bValue;
            break;
        case 3: // data reg
            bDataReg=bValue;
            if (foOperation==foWrite)
            {
                aabSectorBufs[wDisk][wSectOfs++]=bValue;
                if (wSectOfs==512)
                {
                    wSectOfs=0;
                    foOperation=foIdle;
                    WriteSector(wDisk, wCylinder, wHead, wSector, aabSectorBufs[wDisk]);
                }
            }
            break;
        }

    }
}

uint8_t TFDC1793::ReadReg(uint16_t wReg)
{
    uint8_t bValue;

    if (ftType==ftOrion && (wReg&0x24)) // Запись в решистр управления "Ориона"
        return 0;
    wReg&=0x03;
    switch (wReg)
    {
    case 0: //status
        return foOperation==foIdle?0:3;
        break;
    case 1: //cylinder
        return wCylinder;
        break;
    case 2: //sector
        return wSector;
        break;
    case 3: //data
        if (foOperation==foIdle)
            return 0;
        bValue=aabSectorBufs[wDisk][wSectOfs++];
        if (wSectOfs==512)
        {
            wSectOfs=0;
            foOperation=foIdle;
        }
    }
}

void TFDC1793::ProcessTime(int nTicks)
{
    return;
}

/*void TFDC1793::SaveFDCState(RKSS_FDC_STATE *fdcState)
{
    pitState->wPitK0=wPITInitCnt[0];
}**/

/*void TFDC1793::LoadDFCState(RKSS_FDC_STATE *fdcState)
{
    bPITCSW[0]=pitState->bPitMode0;
}*/

// --- Temporary Stubs for ASM ---------------------------------------------

static class TFDC1793 *PFDC = NULL;

void InitFDC()
{
    if (!PFDC)
        PFDC = new TFDC1793(cModel!=MODEL_O?ftGeneral:ftOrion);
    else
    {
        PFDC->InitDevice();
        PFDC->SetFDCType(cModel!=MODEL_O?ftGeneral:ftOrion);
    }
}

void WriteFDCReg(uint16_t wReg, uint8_t bValue)
{
    PFDC->WriteReg(wReg, bValue);
}

uint8_t ReadFDCReg(uint16_t wReg)
{
    return PFDC->ReadReg(wReg);
}

/*void LoadFDCState(RKSS_FDC_STATE *fdcState)
{
    PFDC->LoadFDCState(fdcState);
}

void SaveFDCState(RKSS_FDC_STATE *fdcState)
{
    PFDC->SaveFDCState(fdcState);
}*/
