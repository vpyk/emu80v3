#ifndef _PPI8255_H
#define _PPI8255_H

#include "io_device.h"


enum TPPIChMode
{
    pcmOut=0,
    pcmIn=1//,
//    pcmInOut=2
};


class TPPI8255 : public TIODevice
{
    public:
        TPPI8255();
        virtual ~TPPI8255();
        void InitDevice();
        void ProcessTime(int nTicks);
        void WriteReg(uint16_t wReg, uint8_t bValue);
        uint8_t ReadReg(uint16_t wReg);
        uint8_t GetPortA() {return bPortA;};
        uint8_t GetPortB() {return bPortB;};
        uint8_t GetPortC() {return bPortC;};
        void SetPortA(uint8_t bValue) {bPortA=bValue;};
        void SetPortB(uint8_t bValue) {bPortB=bValue;};
        void SetPortC(uint8_t bValue) {bPortC=bValue;};
        void SetPortAInput(uint8_t bValue) {bPortAInput=bValue;};
        void SetPortBInput(uint8_t bValue) {bPortBInput=bValue;};
        void SetPortCInput(uint8_t bValue) {bPortCInput=bValue;};
        uint8_t GetPortAInput() {return bPortAInput;};
        uint8_t GetPortBInput() {return bPortBInput;};
        uint8_t GetPortCInput() {return bPortCInput;};
        TPPIChMode GetChAMode() {return pcmChAMode;};
        TPPIChMode GetChBMode() {return pcmChBMode;};
        TPPIChMode GetChCHiMode() {return pcmChCHiMode;};
        TPPIChMode GetChCLowMode() {return pcmChCLowMode;};
    protected:
    private:
        uint8_t bPortA;
        uint8_t bPortB;
        uint8_t bPortC;
        uint8_t bPortAInput;
        uint8_t bPortBInput;
        uint8_t bPortCInput;
        TPPIChMode pcmChAMode;
        TPPIChMode pcmChBMode;
        TPPIChMode pcmChCHiMode;
        TPPIChMode pcmChCLowMode;
};

extern "C" void InitPPI();
extern "C" void WritePPIReg(uint16_t wReg, uint8_t bValue);
extern "C" uint8_t ReadPPIReg(uint16_t wReg);
void PerformPPIInput();
void PerformPPIOutput();
extern "C" void WritePPI2Reg(uint16_t wReg, uint8_t bValue);
extern "C" uint8_t ReadPPI2Reg(uint16_t wReg);
uint8_t PPIGetPortC();
void PPISetPortC(uint8_t bValue);

#endif // _PPI8255_H
