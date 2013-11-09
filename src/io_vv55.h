#ifndef _PPI8255_H
#define _PPI8255_H

#include "io_device.h"


class TPPI8255 : public TIODevice
{
    public:
        TPPI8255();
        virtual ~TPPI8255();
        void InitDevice();
        void ProcessTime(int nTicks);
        void WriteReg(uint16_t wReg, uint8_t bValue);
        uint8_t ReadReg(uint16_t wReg);
    protected:
    private:
};

extern "C" void InitPPI();
extern "C" void WritePPIReg(uint16_t wReg, uint8_t bValue);
extern "C" uint8_t ReadPPIReg(uint16_t wReg);
//void LoadPPIState(RKSS_PPI_STATE *ppiState);
//void SavePPIState(RKSS_PPI_STATE *ppiState);

#endif // _PPI8255_H
