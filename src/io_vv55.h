#ifndef _PPI8255_H
#define _PPI8255_H

#include "io_device.h"


enum TPPIChMode
{
    pcmWrite=0,
    pcmRead=1//,
//    pcmReadWrite=2
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
        void SetPortAReadValue(uint8_t bValue) {bPortAReadValue=bValue;};
        void SetPortBReadValue(uint8_t bValue) {bPortBReadValue=bValue;};
        void SetPortCReadValue(uint8_t bValue) {bPortCReadValue=bValue;};
    protected:
    private:
        uint8_t bPortA;
        uint8_t bPortB;
        uint8_t bPortC;
        uint8_t bPortAReadValue;
        uint8_t bPortBReadValue;
        uint8_t bPortCReadValue;
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
//void LoadPPIState(RKSS_PPI_STATE *ppiState);
//void SavePPIState(RKSS_PPI_STATE *ppiState);

#endif // _PPI8255_H
