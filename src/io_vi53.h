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

#ifndef _IO_VI53_H
#define _IO_VI53_H

#include "rss.h"

#include "io_device.h"

// bPITCountBCD values
enum TPITBCD
{
    bcdBinary=0,
    bcdBCD=1
};

// bPITLoadMode values
enum TPITReadLoad
{
    rlLatch=0,
    rlLowByte=1,
    rlHighByte=2,
    rlWord=3
};

// bPITState  values
enum TPITCntState
{
    stStopped=1,
    stWorking=0
};

// bPITRWState  values
enum TPITCntRWState
{
    rwstWaitLow=1,
    rwstWaitHigh=0
};

enum TPITType
{
    ptGeneral=0,
    ptRK86=1
};

class TTimer8253 : public TIODevice
{
public:
    TTimer8253(TPITType pkPITType=ptGeneral);
    void StartCount(int nCnt);
    void InitDevice();
    void WriteReg(uint16_t wReg, uint8_t bValue);
    uint8_t ReadReg(uint16_t wReg);
    void ProcessTime(int nTicks);
    uint8_t GetOutput(uint8_t bCnt);
    uint8_t GetSample();
    void SetPITType(TPITType ptPITType=ptGeneral);
    void LoadPITState(RKSS_PIT_STATE *pitState);
    void SavePITState(RKSS_PIT_STATE *pitState);
private:
    uint16_t wPITCounter[3];   // Текущие значения счетчиков
    uint16_t wPITInitCnt[3];   // Начальное значение счетчика (для режима 3)
    uint16_t wPITLatch[3];     // Защелкиваемое значение счетчика
    uint8_t bPITState[3];      // Состояние счетчиков (stStopped, stWorking)
    uint8_t bPITRState[3];     // Чтение старшего или младшего байта
    uint8_t bPITWState[3];     // Запись старшего или младшего байта
    uint8_t bPITCntLatched[3]; // Защелкнут ли счетчик
    uint8_t bPITMode[3];       // Режим работы счетчиков (0-5)
    uint8_t bPITOutput[3];     // Состояние выходов счетчиков (0-1)
    uint8_t bPITLoadMode[3];   // Режим загрузки счетчиков
    uint8_t bPITCountBCD[3];   // BCD-режим счета
    uint8_t bPITCSW[3];
    uint16_t wPitK0;
    uint16_t wPitK1;
    uint16_t wPitK2;
    uint16_t wPitCnt2;
    uint16_t wPitCnt0;
    uint16_t wPitCnt1;
    uint8_t cPitMode0;
    uint8_t cPitMode1;
    uint8_t cPitMode2;
    uint8_t cPitLd0;
    uint8_t cPitLd1;
    uint8_t cPitLd2;
    TPITType ptType;
};

extern "C" void InitPIT();
extern "C" void WritePITReg(uint16_t wReg, uint8_t bValue);
extern "C" uint8_t ReadPITReg(uint16_t wReg);
extern "C" void ProcessTime(long lTicks);
extern "C" uint8_t GetOutput(uint8_t bCnt);
extern "C" uint8_t GetSample();
void LoadPITState(RKSS_PIT_STATE *pitState);
void SavePITState(RKSS_PIT_STATE *pitState);

#endif // _IO_VI53_H
