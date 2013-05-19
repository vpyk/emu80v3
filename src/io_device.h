#ifndef _IO_DEVICE_H
#define _IO_DEVICE_H

#include <stdint.h>

class TIODevice
{
    public:
        TIODevice();
        virtual ~TIODevice();
        virtual void WriteReg(uint16_t wReg, uint8_t bValue) = 0;
        virtual uint8_t ReadReg(uint16_t wReg) = 0;
        virtual void InitDevice() = 0;
        virtual void ProcessTime(int nTicks) = 0;
        //virtual void SaveState();
        //virtual void LoadState();
    protected:
    private:
};

#endif // _IO_DEVICE_H
