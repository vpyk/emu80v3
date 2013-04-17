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

#ifndef IO_VI53_H_INCLUDED
#define IO_VI53_H_INCLUDED

#include "rss.h"

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

class Timer8253 {
public:
    Timer8253();
    void StartCount(int nCnt);
    void InitPIT();
    void WritePITReg(Uint16 wReg, Uint8 bValue);
    Uint8 ReadPITReg(Uint16 wReg);
    void ProcessTime(long lTicks);
    Uint8 GetOutput(Uint8 bCnt);
    Uint8 GetSample();
    void LoadPITState(RKSS_PIT_STATE *pitState);
    void SavePITState(RKSS_PIT_STATE *pitState);
private:
    Uint16 wPITCounter[3];   // Текущие значения счетчиков
    Uint16 wPITInitCnt[3];   // Начальное значение счетчика (для режима 3)
    Uint16 wPITLatch[3];     // Защелкиваемое значение счетчика
    Uint8 bPITState[3];      // Состояние счетчиков (stStopped, stWorking)
    Uint8 bPITRState[3];     // Чтение старшего или младшего байта
    Uint8 bPITWState[3];     // Запись старшего или младшего байта
    Uint8 bPITCntLatched[3]; // Защелкнут ли счетчик
    Uint8 bPITMode[3];       // Режим работы счетчиков (0-5)
    Uint8 bPITOutput[3];     // Состояние выходов счетчиков (0-1)
    Uint8 bPITLoadMode[3];   // Режим загрузки счетчиков
    Uint8 bPITCountBCD[3];   // BCD-режим счета
    Uint8 bPITCSW[3];
    unsigned short wPitK0;
    unsigned short wPitK1;
    unsigned short wPitK2;
    unsigned short wPitCnt2;
    unsigned short wPitCnt0;
    unsigned short wPitCnt1;
    unsigned char cPitMode0;
    unsigned char cPitMode1;
    unsigned char cPitMode2;
    unsigned char cPitLd0;
    unsigned char cPitLd1;
    unsigned char cPitLd2;
};

extern "C" void InitPIT();
extern "C" void WritePITReg(Uint16 wReg, Uint8 bValue);
extern "C" Uint8 ReadPITReg(Uint16 wReg);
extern "C" void ProcessTime(long lTicks);
extern "C" Uint8 GetOutput(Uint8 bCnt);
extern "C" Uint8 GetSample();
void LoadPITState(RKSS_PIT_STATE *pitState);
void SavePITState(RKSS_PIT_STATE *pitState);

#endif // IO_VI53_H_INCLUDED
