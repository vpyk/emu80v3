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

// io_vt57.h

#ifndef _IO_VT57_H
#define _IO_VT57_H

#include "rss.h"

#include "io_device.h"

/*enum TDMAType
{
};*/

enum TDMACycleType
{
dctVerify=0b00,
dctRead=0b01,
dctWrite=0b10,
dctIllegal=0b11
};

class TDMA8257 : public TIODevice
{
public:
    TDMA8257();
    void InitDevice();
    void WriteReg(uint16_t wReg, uint8_t bValue);
    uint8_t ReadReg(uint16_t wReg);
//    void SetDMAType(TDMAType ptDMAType=ftGeneral);
//    void LoadFDCState(RKSS_FDC_STATE *pitState);
//    void SavePITState(RKSS_FDC_STATE *pitState);
    void ProcessTime(int nTicks);
    void GetBlock(int nChannel, int nLen, uint8_t *pbBuf);
    uint16_t GetChAddr(int nChannel);
    uint16_t GetChLen(int nChannel);
    uint8_t GetMR();
    void LoadDMAState(RKSS_DMA_STATE *dmaState);
    void SaveDMAState(RKSS_DMA_STATE *dmaState);
private:
    uint16_t wAddr0;
    uint16_t wAddr1;
    uint16_t wAddr2;
    uint16_t wAddr3;
    uint16_t wLen0;
    uint16_t wLen1;
    uint16_t wLen2;
    uint16_t wLen3;
    uint8_t bModeReg;
    bool blLoByte;
};

extern "C" void InitDMA();
extern "C" void WriteDMAReg(uint16_t wReg, uint8_t bValue);
extern "C" uint8_t ReadDMAReg(uint16_t wReg);
void LoadDMAState(RKSS_DMA_STATE *dmaState);
void SaveDMAState(RKSS_DMA_STATE *dmaState);

#endif // _IO_VT57_H
