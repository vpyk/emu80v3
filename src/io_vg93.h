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

// io_vi53.h

#ifndef _IO_VG93_H
#define _IO_VG93_H

#include "rss.h"

#include "io_device.h"

enum TFDCType
{
    ftGeneral=0,
    ftOrion=1
};

enum TSDDirection
{
    sdIn=0,
    sdOut=1
};

enum TFDCOperation
{
    foIdle=0,
    foRead=1,
    foWrite=2
};


class FDC1793 : public TIODevice
{
public:
    FDC1793(TFDCType pkFDCType=ftGeneral);
    void InitDevice();
    void WriteReg(uint16_t wReg, uint8_t bValue);
    uint8_t ReadReg(uint16_t wReg);
    void SetFDCType(TFDCType ptFDCType=ftGeneral);
//    void LoadFDCState(RKSS_FDC_STATE *pitState);
//    void SavePITState(RKSS_FDC_STATE *pitState);
    void ProcessTime(int nTicks);
private:
    uint8_t bDataReg;           // Регистра данных
    uint16_t wDisk;             // Диск
    uint16_t wHead;             // Головка
    uint16_t wCylinder;         // Дорожка
    uint16_t wSector;           // Сектор
    TFDCType ftType;
    TSDDirection sdDirection;   // Направление шага головок
//    int bTransfer;              // Признак обмена с диском
    TFDCOperation foOperation;  // Текущий режим обмена с диском
    int wSectOfs;               // Текущее смещенеи в буфере сектора
    uint8_t aabSectorBufs[2][512];  // Буферы сектора дисков
};

extern "C" void InitFDC();
extern "C" void WriteFDCReg(uint16_t wReg, uint8_t bValue);
extern "C" uint8_t ReadFDCReg(uint16_t wReg);
//void LoadPITState(RKSS_FDC_STATE *fdcState);
//void SavePITState(RKSS_FDC_STATE *fdcState);

#endif // _IO_VG93_H
