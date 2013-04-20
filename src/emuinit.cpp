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

// emuinit.cpp
//

#include <stdio.h>
#include <string.h>
//#include <mem.h>

#ifdef linux
#include <dirent.h>
#include <sys/stat.h>
#else
#include <io.h>
#endif

#include <stdlib.h>

#include "emu80.h"
#include "asm.h"

#include "sdllayer.h"
#include "emuinit.h"
#include "menus.h"
#include "misc.h"
#include "sound.h"
#include "io_vi53.h"

void Emulate(int nSamples); //!!!

void DebugProc(int arg)
{
printf("%d\n",arg);
}

unsigned char cModel=MODEL_R;
unsigned char cMonitor=MON_DEF;

int ChooseModel();
int ChooseROM();
void RefreshMenu();
void TuneMenu();
void ClearDisabledMenuItems();
void ChooseDisk(long nDisk);
void ChooseDiskName(long nDisk, char *szFileName);
void AlterSpeed(short nSysReq);
void ReadSector(short wDisk, short wTrack, short wHead, short wSector);
void WriteSector(short wDisk, short wTrack, short wHead, short wSector);
int SaveSS();
int LoadSS();
void ShowHelp();
void GeneralError();
void ROMError();
void SymGenError();
void FontsError();
void ROMDiskError();

int LoadRK(FILE *f);
int LoadSSName(char *szCmdLineFileName);

static FILE* fDiskA=0;
static FILE* fDiskB=0;

unsigned char acSectBufA[1024];
unsigned char acSectBufB[1024];

static short wMaxDelaySB,wMinDelaySB;

static unsigned char acDefMons[6]={1,0,1,0,1,2}; // ╠юэшЄюЁ√ яю єьюыўрэш■
static unsigned char acMaxMonitor[6]={3,0,2,0,4,3}; // ╩юы-тю ╠юэшЄюЁют
//static unsigned char acDefMons[7]={1,0,1,0,1,2,1}; // ╠юэшЄюЁ√ яю єьюыўрэш■
//static unsigned char acMaxMonitor[7]={3,0,2,0,4,3,2}; // ╩юы-тю ╠юэшЄюЁют


void SetModel();
void SetMonitor();

void SetModel()
{
use_io_space=0;
keybin_port=1;
romvar_addr=0x7600; //!!!!!
curpos_addr=0x7602; //!!!!!
sh_width=78;
sh_height=30;
sh_scrlen=78*30;
sh_scrbeg=0x76d0;
reset_addr=0xf800;
ticks_per_44100th=40;
delay_sb_dma=26;
delay_sb_norm=40;
switch (cModel)
  {
  case MODEL_P:
    scr_beg=0x77bd;
    scr_end=0x7f0d+74;
    cur_corr=0x304;
    rom_adr=0xf000;
    rom_len=0x1000;
    ppi_portc_adr=0xd902;
    crt_port1_adr=0xd801;
    hook_proc_sb=hook_p_sb;
//    ext_letter='P'; //!!!!!
    ticks_per_44100th=45;
    delay_sb_dma=31;
    delay_sb_norm=45;
    break;
  case MODEL_A:
    sh_scrbeg=0xe1d0;
    scr_beg=0xe2c2;
    scr_end=0xea12;
    cur_corr=0x308;
    romvar_addr=0xe100; //!!!!!
    rom_adr=0xf000;
    rom_len=0x1000;
    ppi_portc_adr=0xee02;
    crt_port1_adr=0xef01;
    romname="roms/roma.bin";
    hook_proc_sb=hook_a_sb;
//    ext_letter='A'; //!!!!!
    break;
  case MODEL_M:
    scr_beg=0x77c2;
    scr_end=0x7f12;
    cur_corr=0x308;
    keybin_port=0;
    rom_adr=0xf800;
    rom_len=0x0800;
    ppi_portc_adr=0xc002;
    crt_port1_adr=0xd001;
    romname="roms/romm.bin";
    hook_proc_sb=hook_m_sb;
//    ext_letter='M'; //!!!!!
    delay_sb_dma=29;   //
    break;
  case MODEL_R:
    scr_beg=0x77c2;
    scr_end=0x7f12;
    cur_corr=0x308;
    rom_adr=0xf800;
    rom_len=0x0800;
    ppi_portc_adr=0x8002;
    crt_port1_adr=0xc001;
    hook_proc_sb=hook_r_sb;
//    ext_letter='R'; /!!!!!
    break;
  case MODEL_S:
    scr_beg=0x9000;
    scr_end=0xbfff;
    cur_corr=0x304;
    rom_adr=0xc000;
    ppi_portc_adr=0xd902;
    crt_port1_adr=0xd801;
    hook_proc_sb=hook_s_sb;
//    ext_letter='S'; //!!!!!
    reset_addr=0xc000;
    ticks_per_44100th=45;
    delay_sb_dma=45;
    delay_sb_norm=45;
    break;
  case MODEL_O:
    scr_beg=0xc000;
    scr_end=0xefff;
    cur_corr=0x304;
    rom_adr=0xf800;
    rom_len=0x800;
    ppi_portc_adr=0xf402;
    crt_port1_adr=0xd801;
    hook_proc_sb=hook_o_sb;
//    ext_letter='O'; //!!!!!
    reset_addr=0xf800;
    ticks_per_44100th=57;
    delay_sb_dma=57;
    delay_sb_norm=57;
    break;
  case MODEL_M80:
    use_io_space=1;
    scr_beg=0xe000;
    scr_end=0xefff;
    cur_corr=0x0;
    rom_adr=0xf800;
    rom_len=0x800;
    hook_proc_sb=hook_m80_sb;
//    ext_letter='8'; //!!!!!
    ticks_per_44100th=450;
    delay_sb_dma=40;
    delay_sb_norm=40;
    break;
  case MODEL_U:
    use_io_space=1;
    scr_beg=0xe000; //###
    scr_end=0xefff; //###
    cur_corr=0x0;
    rom_adr=0xf800;
    rom_len=0x800;
    hook_proc_sb=hook_u_sb;
//    ext_letter='U'; //!!!!!
    ticks_per_44100th=450;
    delay_sb_dma=40;
    delay_sb_norm=40;
    break;
  }
SetMonitor();
curpos_addr=romvar_addr+2; //!!!!!
cur_speed_sb=delay_sb_dma;
corr_key_tbl();
wMaxDelaySB=delay_sb_norm*11/2;
wMinDelaySB=delay_sb_norm*2/11;
//Reset();
}

void SetMonitor()
{
if (cMonitor==0)
  cMonitor=acDefMons[cModel];
switch (cModel)
  {
  case MODEL_P:
    if (cMonitor==MON_P_V3)
      romname="roms/romp.bin";
    else if (cMonitor==MON_P_V2)
      romname="roms/rompv2.bin";
    break;
  case MODEL_R:
    if (cMonitor!=MON_R_4K)
      {
      rom_adr=0xf800;
      rom_len=0x0800;
      }
    else
      {
      rom_adr=0xf000;
      rom_len=0x1000;
      }
    if (cMonitor==MON_R_16)
      romname="roms/romr16.bin";
    else if (cMonitor==MON_R_32)
      romname="roms/romr.bin";
    else if (cMonitor==MON_R_4K)
      romname="roms/romr4k.bin";
//    else if (cMonitor==MON_R_3)
//      romname="romr3.bin";
    break;
  case MODEL_S:
    if (cMonitor==MON_S_M)
      rom_len=0x2000;
    else if (cMonitor==MON_S_L)
      rom_len=0x3000;
    else
      rom_len=0x1000;
    if (cMonitor==MON_S_1)
      romname="roms/roms1.bin";
    else if (cMonitor==MON_S_2)
      romname="roms/roms.bin";
    else if (cMonitor==MON_S_M)
      romname="roms/romsm.bin";
    else if (cMonitor==MON_S_L)
      romname="roms/romsl.bin";
    break;
  case MODEL_O:
    if (cMonitor==MON_O_1)
      romname="roms/romo1.bin";
    else if (cMonitor==MON_O_2)
      romname="roms/romo.bin";
    else if (cMonitor==MON_O_PET)
      romname="roms/romop.bin";
    break;
  case MODEL_M80:
//    if (cMonitor==MON_M80_O)
      romname="roms/rom80.bin";
//    else if (cMonitor==MON_M80_R)
//      romname="rom8r.bin";
    break;
  case MODEL_U:
    romname="roms/romu.bin";
    break;
  }
}

static char *ppcFileErrMess[1]={"Ошибка файловой операции"};
static char *ppcFormatErrMess[1]={"Неподдерживаемая модель компьютера"};
static char *ppcMonitorWarnMess[2]={"  Неизвестный тип Монитора",
                                    "Используется тип по умолчанию"};
static char *ppcFileSuccMess[1]={"Файл сохранен"};
static char *ppcGeneralErrMess[1]={"Фатальная ошибка"};
static char *ppcROMErrMess[1]={"Не найден один из файлов ПЗУ (rom*.bin)"};
static char *ppcSymGenErrMess[1]={"Не найден файл знакогенератора (symgen.bin)"};
static char *ppcFontsErrMess[1]={"Не найден файл шрифтов (fonts/*.bmp)"};
static char *ppcROMDiskErrMess[1]={"Не найден файл ROMDISK'а"};

void GeneralError()
{
ShowMessage(ppcGeneralErrMess,1," Ошибка ",0,0);
}

void SymGenError()
{
ShowMessage(ppcSymGenErrMess,1," Ошибка ",0,0);
}

void FontsError()
{
ShowMessage(ppcFontsErrMess,1," Ошибка ",0,0);
}

void ROMDiskError()
{
ShowMessage(ppcROMDiskErrMess,1," Ошибка ",0,0);
}

void ROMError()
{
ShowMessage(ppcROMErrMess,1," Ошибка ",0,0);
}

void FileError()
{
ShowMessage(ppcFileErrMess,1," Ошибка ",0,0);
}

void FormatError()
{
ShowMessage(ppcFormatErrMess,1," Ошибка ",0,0);
}

void MonitorWarning()
{
ShowMessage(ppcMonitorWarnMess,2," Предупреждение ",0,0);
}

void ChooseDisk(long nDisk)
{
if (cModel!=MODEL_O)
  return;
//EnterInteractiveMode();
f_file_ss=2;
f_choose_file=1;
getfilename();
char *szFileName=(char *)&filename;
int len=strlen(szFileName);
if (len!=0 && len<=16 && (strcspn(szFileName,".")==len))
  strcat(szFileName,".odi");
if (!*szFileName) return;
ChooseDiskName(nDisk, szFileName);
}

void ChooseDiskName(long nDisk, char *szFileName)
{
if (nDisk==0 && fDiskA!=0)
  {
  fclose(fDiskA);
  fDiskA=0;
  }
else if (nDisk==1 && fDiskB!=0)
  {
  fclose(fDiskB);
  fDiskB=0;
  }

FILE *f=fopen(szFileName,"r+b");

if (!f)
  {
  FileError();
  return;
  }

if (nDisk==0)
  fDiskA=f;
else
  fDiskB=f;
}

void ReadSector(long nDisk, long nTrack, long nHead, long nSector)
{
// printf("Read  %d %d %d %d\n",wDisk,wTrack,wHead,wSector);
FILE *f=nDisk?fDiskB:fDiskA;
unsigned char *buf=nDisk?acSectBufB:acSectBufA;
memset(buf,0xe5,1024);
if (f)
  {
  fseek(f,nTrack*10240+nHead*5120+(nSector-1)*1024,SEEK_SET);
  fread(buf,1024,1,f);
  }
}

void WriteSector(long nDisk, long nTrack, long nHead, long nSector)
{
// printf("Write %d %d %d %d\n",wDisk,wTrack,wHead,wSector);
FILE *f=nDisk?fDiskB:fDiskA;
unsigned char *buf=nDisk?acSectBufB:acSectBufA;
if (f)
  {
  fseek(f,nTrack*10240+nHead*5120+(nSector-1)*1024,SEEK_SET);
  fwrite(buf,1024,1,f);
  }
}

int Equals(unsigned short wAddr,unsigned short wEndAddr,char cPage)
{
unsigned char c=mempages[cPage][wAddr];
int n=0;
while ((mempages[cPage][wAddr]==c) && (wAddr<=wEndAddr) && (n<=254))
  {
  wAddr++;
  n++;
  }
return n;
}

void WriteMem(FILE *f, unsigned short wStart, unsigned short wEnd,
              char cCompression, char cPage=0)
{
if (cPage!=0) // Дополнительный блок
  fwrite(&cPage,1,1,f);
struct
 {
 char cCompType;
 unsigned short wCompLen  __attribute__((packed));
 unsigned short wStart    __attribute__((packed));
 unsigned short wLen      __attribute__((packed));
 } h;
h.cCompType=cCompression;
h.wLen=wEnd-wStart+1;
h.wCompLen=wEnd-wStart+1;
h.wStart=wStart;
fwrite(&h,sizeof(h),1,f);
unsigned char byte;
if (cCompression==COMPRESSION_NONE)
  for (long i=(long)wStart;i<=(long)wEnd;i++)
    {
    byte=mempages[cPage][i];
    fwrite(&byte,1,1,f);
    }
else // COMPRESSION_RLE
  {
  long lCompLenPos=ftell(f)-6;
  unsigned short cb;
  unsigned long i=wStart;
  unsigned short wCompLen=0;
  while (i<=wEnd)
    {
    cb=Equals(i,wEnd,cPage);
    if (cb<=3 && mempages[cPage][i]!=(unsigned char)0xCB)
      {
      byte=mempages[cPage][i++];
      fwrite(&byte,1,1,f);
      wCompLen++;
      }
    else
      {
      byte=0xcb;
      fwrite(&byte,1,1,f);
      byte=mempages[cPage][i];
      fwrite(&byte,1,1,f);
      byte=cb;
      fwrite(&byte,1,1,f);
      i+=cb;
      wCompLen+=3;
      }
    }
  unsigned short lCurPos=ftell(f);
  fseek(f,lCompLenPos,SEEK_SET);
  wCompLen+=7;
  fwrite(&wCompLen,2,1,f);
  fseek(f,lCurPos,SEEK_SET);
  }
}

void ReadMem(FILE *f, int bExtended=0)
{
char cPage=0;
if (bExtended)
  fread(&cPage,1,1,f);
char cCompression;
unsigned short wCompLen;
unsigned short wStart;
unsigned short wLen;

fread(&cCompression,1,1,f);
fread(&wCompLen,1,2,f);
fread(&wStart,1,2,f);
fread(&wLen,1,2,f);

if ((long)wStart+(long)wLen>(long)rom_adr)
  f_saverom=1;

unsigned char b;
unsigned short cb;

//unsigned short i=wStart;
long i=(long)wStart;
while (i<=(long)wStart+(long)wLen-1L)
  {
  fread(&b,1,1,f);
  if (b!=(unsigned char)0xcb || cCompression==0)
    mempages[cPage][i++]=b;
  else
    {
    fread(&b,1,1,f);
    fread(&cb,1,1,f);
    cb&=0xff;
    for (int j=0;j<cb;j++)
      mempages[cPage][i++]=b;
    }
  }
}

char back_colors[7]={0,0,0,1,0,2,3};

int SaveSS()
{
char b;
unsigned short w;
int i,j;

//EnterInteractiveMode();
f_file_ss=1;
f_choose_file=0;
getfilename();
char *szFileName=(char *)&filename;
int len=strlen(szFileName);
if (len!=0 && len<=16 && (strcspn(szFileName,".")==len))
  strcat(szFileName,".rss");
if (!*szFileName) return 0;

FILE *f=fopen(szFileName,"wb");
if (!f)
  {
  FileError();
  return 0;
  }

  {
  RKSS_MAIN_HEADER mh;
  memset(&mh,0,sizeof(mh));
  mh.dwSig=0x53534b52; //RKSS
  mh.cModel=cModel;
  mh.wPC=reg_pc;
  mh.wAF=reg_af;
  mh.wBC=reg_bc;
  mh.wDE=reg_de;
  mh.wHL=reg_hl;
  mh.wSP=reg_sp;
  mh.cIF=int_flag;
  fwrite(&mh,sizeof(mh),1,f);
  }

/*if (cModel!=MODEL_S && cModel!=MODEL_O && cModel!=MODEL_M80 &&
    cModel!=MODEL_U && f_vid.cValue==MEM_NONE)
  {
  cur_x=mempages[0][curpos_addr];
  cur_y=mempages[0][curpos_addr+1];
  if (cModel==MODEL_P)
    cur_xy+=0x304;
  crt_param_1=0x4d;
  crt_param_2=0x1d;
  crt_param_3=0x99;
  if (cModel==MODEL_P)
    crt_param_4=0xd9;
  else if (cModel==MODEL_A)
    crt_param_4=0xd3;
  else
    crt_param_4=0x93;
  dma_mr=0xa4;
  pit_k2=1;
  }*/

switch (cModel)
  {
  case MODEL_R:
    {
    RKSS_RK_HEADER rrh;
    memset(&rrh,0,sizeof(rrh));
    rrh.cMonVer=cMonitor;
    rrh.wHeaderLen=sizeof(rrh);
    rrh.wScrBegAdr=sh_scrbeg;
    rrh.wScrLen=sh_scrlen;
    rrh.cScrLines=sh_height;
    rrh.cScrCols=sh_width;
    rrh.acCrtParams[0]=crt_param_1;
    rrh.acCrtParams[1]=crt_param_2;
    rrh.acCrtParams[2]=crt_param_3;
    rrh.acCrtParams[3]=crt_param_4;
    rrh.cDmaMR=dma_mr;
    rrh.wDmaBegAddr=dma_begadr;
    rrh.wDmaLen=dma_len;
    rrh.cCrtMReg=crt_mreg;
    rrh.cPortC=port_c&0xf;
    rrh.cCurX=cur_x;
    rrh.cCurY=cur_y;

    RKSS_PIT_STATE pitState;
    SavePITState(&pitState);
    rrh.wPitK0=pitState.wPitK0;
    rrh.wPitK1=pitState.wPitK1;
    rrh.wPitK2=pitState.wPitK2;
    rrh.wPitCnt0=pitState.wPitCnt0;
    rrh.wPitCnt1=pitState.wPitCnt1;
    rrh.wPitCnt2=pitState.wPitCnt2;
    rrh.cPitMode0=pitState.bPitMode0;
    rrh.cPitMode1=pitState.bPitMode1;
    rrh.cPitMode2=pitState.bPitMode2;
    rrh.cPitLd0=pitState.bPitLd0;
    rrh.cPitLd1=pitState.bPitLd1;
    rrh.cPitLd2=pitState.bPitLd2;

    fwrite(&rrh,sizeof(rrh),1,f);
    break;
    }
  case MODEL_P:
    {
    RKSS_PARTNER_HEADER rph;
    memset(&rph,0,sizeof(rph));
    rph.cMonVer=cMonitor;
    rph.wHeaderLen=sizeof(rph);
    rph.wScrBegAdr=sh_scrbeg;
    rph.wScrLen=sh_scrlen;
    rph.cScrLines=sh_height;
    rph.cScrCols=sh_width;
    rph.acCrtParams[0]=crt_param_1;
    rph.acCrtParams[1]=crt_param_2;
    rph.acCrtParams[2]=crt_param_3;
    rph.acCrtParams[3]=crt_param_4;
    rph.cDmaMR=dma_mr;
    rph.wDmaBegAddr=dma_begadr;
    rph.wDmaLen=dma_len;
    rph.cCrtMReg=crt_mreg;
    rph.cPortC=port_c&0xf;
    rph.cCurX=cur_x;
    rph.cCurY=cur_y;
    fwrite(&rph,sizeof(rph),1,f);
    break;
    }
  case MODEL_M:
    {
    RKSS_MIKROSHA_HEADER rmh;
    memset(&rmh,0,sizeof(rmh));
    rmh.cMonVer=cMonitor;
    rmh.wHeaderLen=sizeof(rmh);
    rmh.wScrBegAdr=sh_scrbeg;
    rmh.wScrLen=sh_scrlen;
    rmh.cScrLines=sh_height;
    rmh.cScrCols=sh_width;
    rmh.acCrtParams[0]=crt_param_1;
    rmh.acCrtParams[1]=crt_param_2;
    rmh.acCrtParams[2]=crt_param_3;
    rmh.acCrtParams[3]=crt_param_4;
    rmh.cDmaMR=dma_mr;
    rmh.wDmaBegAddr=dma_begadr;
    rmh.wDmaLen=dma_len;
    rmh.cCrtMReg=crt_mreg;
    rmh.cPortC=port_c&0xf;
    rmh.cPPI2B=mikr_symg;
    rmh.cCurX=cur_x;
    rmh.cCurY=cur_y;


    RKSS_PIT_STATE pitState;
    SavePITState(&pitState);
    rmh.wPitK0=pitState.wPitK0;
    rmh.wPitK1=pitState.wPitK1;
    rmh.wPitK2=pitState.wPitK2;
    rmh.wPitCnt0=pitState.wPitCnt0;
    rmh.wPitCnt1=pitState.wPitCnt1;
    rmh.wPitCnt2=pitState.wPitCnt2;
    rmh.cPitMode0=pitState.bPitMode0;
    rmh.cPitMode1=pitState.bPitMode1;
    rmh.cPitMode2=pitState.bPitMode2;
    rmh.cPitLd0=pitState.bPitLd0;
    rmh.cPitLd1=pitState.bPitLd1;
    rmh.cPitLd2=pitState.bPitLd2;

    fwrite(&rmh,sizeof(rmh),1,f);
    break;
    }
  case MODEL_A:
    {
    RKSS_APOGEY_HEADER rah;
    memset(&rah,0,sizeof(rah));
    rah.cMonVer=cMonitor;
    rah.wHeaderLen=sizeof(rah);
    rah.wScrBegAdr=sh_scrbeg;
    rah.wScrLen=sh_scrlen;
    rah.cScrLines=sh_height;
    rah.cScrCols=sh_width;
    rah.acCrtParams[0]=crt_param_1;
    rah.acCrtParams[1]=crt_param_2;
    rah.acCrtParams[2]=crt_param_3;
    rah.acCrtParams[3]=crt_param_4;
    rah.cDmaMR=dma_mr;
    rah.wDmaBegAddr=dma_begadr;
    rah.wDmaLen=dma_len;
    rah.cCrtMReg=crt_mreg;
    rah.cPortC=port_c&0xf;
    rah.cCurX=cur_x;
    rah.cCurY=cur_y;

    RKSS_PIT_STATE pitState;
    SavePITState(&pitState);
    rah.wPitK0=pitState.wPitK0;
    rah.wPitK1=pitState.wPitK1;
    rah.wPitK2=pitState.wPitK2;
    rah.wPitCnt0=pitState.wPitCnt0;
    rah.wPitCnt1=pitState.wPitCnt1;
    rah.wPitCnt2=pitState.wPitCnt2;
    rah.cPitMode0=pitState.bPitMode0;
    rah.cPitMode1=pitState.bPitMode1;
    rah.cPitMode2=pitState.bPitMode2;
    rah.cPitLd0=pitState.bPitLd0;
    rah.cPitLd1=pitState.bPitLd1;
    rah.cPitLd2=pitState.bPitLd2;

    fwrite(&rah,sizeof(rah),1,f);
    break;
    }
  case MODEL_S:
    {
    RKSS_SPEC_HEADER rsh;
    memset(&rsh,0,sizeof(rsh));
    rsh.cMonVer=cMonitor;
    rsh.wHeaderLen=sizeof(rsh)+3072+3;
    rsh.cPortC=port_c&0xf0;
    fwrite(&rsh,sizeof(rsh),1,f);
    // Цвет (5цв.)
    b=0x01;
    fwrite(&b,1,1,f);
    w=3075;
    fwrite(&w,2,1,f);
    unsigned char col_b;
    for (i=0;i<3072;i++)
      {
      col_b=0;
      for (j=0;j<4;j++)
        {
        b=color_mem[i*4+j];
        b=back_colors[b];
        col_b=col_b>>2 | (b<<6);
        }
      fwrite(&col_b,1,1,f);
      }
    break;
    }
  case MODEL_O:
    {
    RKSS_ORION_HEADER roh;
    memset(&roh,0,sizeof(roh));
    roh.cMonVer=cMonitor;
    roh.wHeaderLen=sizeof(roh);
    roh.cPortC=port_c&0xf;
    roh.cMemPage=mem_page_no;
    roh.cScrPage=or_scrpage;
    roh.cColorMode=or_color_mode;
    roh.cExtBlocks=1;
    fwrite(&roh,sizeof(roh),1,f);
    break;
    }
  case MODEL_M80:
    {
    RKSS_M80_HEADER r8h;
    memset(&r8h,0,sizeof(r8h));
    r8h.cMonVer=cMonitor;
    r8h.wHeaderLen=sizeof(r8h);
    fwrite(&r8h,sizeof(r8h),1,f);
    break;
    }
  case MODEL_U:
    {
    RKSS_UT88_HEADER ruh;
    memset(&ruh,0,sizeof(ruh));
    ruh.cMonVer=cMonitor;
    ruh.wHeaderLen=sizeof(ruh);
    fwrite(&ruh,sizeof(ruh),1,f);
    break;
    }
  }

  {
  RKSS_EMUL_HEADER reh;
  reh.dwSig=0x30384d45;
  reh.wHeaderLen=sizeof(reh);
  reh.cSpeed=f_speed.cValue;
  reh.cMem=MEM_RW;//f_vid.cValue;
  reh.cHookF809=HOOKF809_NO;//f_f809.cValue;
  reh.cScreen=f_scr.cValue;
  reh.cFont=FONT_RK;//f_font.cValue;
  reh.cSound=f_snd.cValue;
  reh.cTape=f_tape.cValue;
  reh.cRefr=REFR_COMB;//f_refr.cValue;
  fwrite(&reh,sizeof(reh),1,f);
  }

  {
  char temp=f_saverom!=0?2:1;
  fwrite(&temp,1,1,f);
  }

switch (cModel)
  {
  case MODEL_M:
    WriteMem(f,0x0000,0x7fff,COMPRESSION_RLE);
    if (f_saverom!=0)
      WriteMem(f,0xf800,0xffff,COMPRESSION_RLE);
    break;
  case MODEL_R:
    WriteMem(f,0x0000,0x7fff,COMPRESSION_RLE);
    if (f_saverom!=0)
      WriteMem(f,0xf000,0xffff,COMPRESSION_RLE);
    break;
  case MODEL_P:
    WriteMem(f,0x0000,0xd7ff,COMPRESSION_RLE);
    if (f_saverom!=0)
      WriteMem(f,0xf000,0xffff,COMPRESSION_RLE);
    break;
  case MODEL_A:
    WriteMem(f,0x0000,0xeaff,COMPRESSION_RLE);
    if (f_saverom!=0)
      WriteMem(f,0xf000,0xffff,COMPRESSION_RLE);
    break;
  case MODEL_S:
    WriteMem(f,0x0000,0xbfff,COMPRESSION_RLE);
    if (f_saverom!=0)
      WriteMem(f,0xc000,0xdfff,COMPRESSION_RLE);
    break;
  case MODEL_O:
    WriteMem(f,0x0000,0xf3ff,COMPRESSION_RLE);
    if (f_saverom!=0)
      WriteMem(f,0xf800,0xffff,COMPRESSION_RLE);
    // Дополнительные блоки
    WriteMem(f,0x0000,0xf3ff,COMPRESSION_RLE,1);
    break;
  case MODEL_M80:
    WriteMem(f,0x0000,0xf7ff,COMPRESSION_RLE);
    if (f_saverom!=0)
      WriteMem(f,0xf800,0xffff,COMPRESSION_RLE);
    break;
  case MODEL_U:
    WriteMem(f,0x0000,0xf7ff,COMPRESSION_RLE);
    if (f_saverom!=0)
      WriteMem(f,0xf800,0xffff,COMPRESSION_RLE);
  }

fclose(f);
ShowMessage(ppcFileSuccMess,1,"",0,0);
return 1;
}

int filelength(FILE *f)
{
long curpos,length;
curpos = ftell(f);
fseek(f, 0L, SEEK_END);
length = ftell(f);
fseek(f, curpos, SEEK_SET);
return(length);
}

// ╟руЁєчър Їрщыют RK (шыш BRU)
int LoadRK(FILE *f)
{
unsigned char bt;
unsigned long dwFileLen=filelength(f);
if (cModel!=MODEL_O)
  {
  if (dwFileLen<8)
    return 0;
  fread(&bt,1,1,f);
  if (bt!=0xe6)
    fseek(f,0,SEEK_SET);
  else
    dwFileLen--;
  unsigned short wBegAdr;
  unsigned short wEndAdr;
  if (cModel!=MODEL_S)
    {
    fread(((char *)&wBegAdr)+1,1,1,f);
    fread((char *)&wBegAdr,1,1,f);
    fread(((char *)&wEndAdr)+1,1,1,f);
    fread((char *)&wEndAdr,1,1,f);
    }
  else
    {
    fread(&wBegAdr,1,2,f);
    fread(&wEndAdr,1,2,f);
    }
  dwFileLen-=4;
  unsigned short wLen=wEndAdr-wBegAdr;
  if (wBegAdr==0xe6e6 | wBegAdr==0xd3d3)
    return 0; // Basic or EDM File
  if (dwFileLen<wLen+2)
    return 0;
  for (unsigned int i=wBegAdr;i<=wEndAdr;i++)
    {
    fread(&bt,1,1,f);
    mempages[0][i]=bt;
    }
  }
else // MODEL_O
  {
  unsigned short wBruOfs=0;
  if (dwFileLen<32)
    return 0;
  unsigned short wReadLen;
  fseek(f,0x0a,SEEK_SET);
  fread(&wReadLen,1,2,f);
  if (wReadLen==0) // RKO
    {
    wBruOfs=0x4d;
    if (dwFileLen<wBruOfs+32)
      return 0;
    dwFileLen-=wBruOfs;
    fseek(f,0x0a+wBruOfs,SEEK_SET);
    fread(&wReadLen,1,2,f);
    }
  wReadLen=(((wReadLen-1)|0xf)+17);
  if (dwFileLen<wReadLen)
    return 0;
//  short wCurAdr=0;
//  while (mempages[2][wCurAdr]!=0xff)
//    wCurAdr=wCurAdr+((((*(unsigned short far *)(mempages[2]+10))-1)|0x0f)+1);
  fseek(f,wBruOfs,SEEK_SET);
  for (unsigned int i=0;i<wReadLen;i++)
    {
    fread(&bt,1,1,f);
    mempages[1][i]=bt;//+++!
    }
  mempages[1][wReadLen]=0xff;//+++!
  }
return 1;
}

int LoadSS()
{
//EnterInteractiveMode();
return LoadSSName(0);
}

int LoadSSName(char *szCmdLineFileName)
{
char *szFileName;

unsigned char b;
unsigned short w;

int i,j;

unsigned char cExtBlocks=0;

if (!szCmdLineFileName)
  {
  f_file_ss=1;
  f_choose_file=1;
  getfilename();
  szFileName=(char *)&filename;
  int len=strlen(szFileName);
  if (len!=0 && len<=16 && (strcspn(szFileName,".")==len))
  strcat(szFileName,".rss");
  }
else
  szFileName=szCmdLineFileName;

//puts(szFileName);
//_asm int 20h

if (!*szFileName) return 0;

FILE *f=fopen(szFileName,"rb");
if (!f)
  {
  FileError();
  return 0;
  }

unsigned long dwSig;
if (fread(&dwSig,1,4,f)!=4)
  {
  FileError();
  fclose(f);
  return 0;
  }

fseek(f,0,SEEK_SET);

if (dwSig!=0x53534b52)
  {
  unsigned long dwFileLen=filelength(f);
  if (dwFileLen==819200L) //ODI
    {
    fclose(f);
    ChooseDiskName(0, szFileName);
    return 2;
    }
// printf("%lx",dwSig);
  if (LoadRK(f))
    {
    fclose(f);
    return cModel==MODEL_O?3:2;
//    return 2;
    }
  else
    {
    FileError();
    fclose(f);
    return 0;
    }
  }

RKSS_MAIN_HEADER mh;
if (fread(&mh,sizeof(mh),1,f)!=1)
  {
  FileError();
  fclose(f);
  return 0;
  }
if (mh.cModel>7)
  {
  FormatError();
  fclose(f);
  return 0;
  }

cModel=mh.cModel;
SetModel();

// TuneMenu();
// RefreshMenu();
reg_pc=mh.wPC;
reg_af=mh.wAF;
reg_bc=mh.wBC;
reg_de=mh.wDE;
reg_hl=mh.wHL;
reg_sp=mh.wSP;
if (mh.cIF==1)
  enable_ints();
else
  disable_ints();

char cTemp;
int wHLen;

if (cModel==MODEL_R)
  {
  RKSS_RK_HEADER rrh;
  // єёЄрэютър яюыхщ яю єьюыўрэш■
  rrh.acCrtParams[1]=0x4d;
  rrh.acCrtParams[1]=0x1d;
  rrh.acCrtParams[1]=0x99;
  rrh.acCrtParams[1]=0x93;
  rrh.cDmaMR=0xa4;
  rrh.cCrtMReg=0x27;
  rrh.wDmaBegAddr=0xffff;
  rrh.wDmaLen=0xffff;
  rrh.wPitK0=1;
  rrh.wPitK1=1;
  rrh.wPitK2=1;
  rrh.wPitCnt0=1;
  rrh.wPitCnt1=1;
  rrh.wPitCnt2=0;
  rrh.cPitMode0=0x26;
  rrh.cPitMode1=0x66;
  rrh.cPitMode2=0x90;
  rrh.cPitLd0=1;
  rrh.cPitLd1=1;
  rrh.cPitLd2=0; //####????
  fread(&rrh,2,1,f);
  wHLen=sizeof(rrh)>rrh.wHeaderLen?rrh.wHeaderLen:sizeof(rrh);
  fread((char *)&rrh+2,wHLen-2,1,f);
  for (int i=rrh.wHeaderLen-sizeof(rrh);i>0;i--)
    fread(&cTemp,1,1,f);
  // ╨рчсюЁ ярЁрьхЄЁют
  cMonitor=rrh.cMonVer;
  if (cMonitor>3)
    {
    MonitorWarning();
    cMonitor=0;
    }
  sh_scrbeg=rrh.wScrBegAdr;
  sh_scrlen=rrh.wScrLen;
  sh_height=rrh.cScrLines;
  sh_width=rrh.cScrCols;
  crt_param_1=rrh.acCrtParams[0];
  crt_param_2=rrh.acCrtParams[1];
  crt_param_3=rrh.acCrtParams[2];
  crt_param_4=rrh.acCrtParams[3];
  dma_mr=rrh.cDmaMR;
  crt_mreg=rrh.cCrtMReg;
  dma_begadr=rrh.wDmaBegAddr;
  dma_len=rrh.wDmaLen;
  port_c=rrh.cPortC;
  cur_x=rrh.cCurX;
  cur_y=rrh.cCurY;
  if (dma_begadr==0xffff)
    dma_begadr=sh_scrbeg;
  if (dma_len==0xffff)
    dma_len=sh_scrlen-1;

  RKSS_PIT_STATE pitState;
  pitState.wPitK0=rrh.wPitK0;
  pitState.wPitK1=rrh.wPitK1;
  pitState.wPitK2=rrh.wPitK2;
  pitState.wPitCnt0=rrh.wPitCnt0;
  pitState.wPitCnt1=rrh.wPitCnt1;
  pitState.wPitCnt2=rrh.wPitCnt2;
  pitState.bPitMode0=rrh.cPitMode0;
  pitState.bPitMode1=rrh.cPitMode1;
  pitState.bPitMode2=rrh.cPitMode2;
  pitState.bPitLd0=rrh.cPitLd0;
  pitState.bPitLd1=rrh.cPitLd1;
  pitState.bPitLd2=rrh.cPitLd2;
  LoadPITState(&pitState);
  }
else if (cModel==MODEL_P)
  {
  RKSS_PARTNER_HEADER rph;
  // єёЄрэютър яюыхщ яю єьюыўрэш■
  rph.acCrtParams[1]=0x4d;
  rph.acCrtParams[1]=0x1d;
  rph.acCrtParams[1]=0x99;
  rph.acCrtParams[1]=0xd9;
  rph.cDmaMR=0xa4;
  rph.cCrtMReg=0x2b;
  fread(&rph,2,1,f);
  wHLen=sizeof(rph)>rph.wHeaderLen?rph.wHeaderLen:sizeof(rph);
  fread(((char *)&rph)+2,wHLen-2,1,f);
  for (int i=rph.wHeaderLen-sizeof(rph);i>0;i--)
    fread(&cTemp,1,1,f);
  // ╨рчсюЁ ярЁрьхЄЁют
  cMonitor=rph.cMonVer;
  if (cMonitor>2)
    {
    MonitorWarning();
    cMonitor=0;
    }
  sh_scrbeg=rph.wScrBegAdr;
  sh_scrlen=rph.wScrLen;
  sh_height=rph.cScrLines;
  sh_width=rph.cScrCols;
  crt_param_1=rph.acCrtParams[0];
  crt_param_2=rph.acCrtParams[1];
  crt_param_3=rph.acCrtParams[2];
  crt_param_4=rph.acCrtParams[3];
  dma_mr=rph.cDmaMR;
  crt_mreg=rph.cCrtMReg;
  dma_begadr=rph.wDmaBegAddr;
  dma_len=rph.wDmaLen;
  port_c=rph.cPortC;
  cur_x=rph.cCurX;
  cur_y=rph.cCurY;
  if (dma_begadr==0xffff)
    dma_begadr=sh_scrbeg;
  if (dma_len==0xffff)
    dma_len=sh_scrlen-1;
  }
else if (cModel==MODEL_M)
  {
  RKSS_MIKROSHA_HEADER rmh;
  // єёЄрэютър яюыхщ яю єьюыўрэш■
  rmh.acCrtParams[1]=0x4d;
  rmh.acCrtParams[1]=0x1d;
  rmh.acCrtParams[1]=0x99;
  rmh.acCrtParams[1]=0x93;
  rmh.cDmaMR=0xa4;
  rmh.cCrtMReg=0x27;
  rmh.wPitK0=1;
  rmh.wPitK1=1;
  rmh.wPitK2=1;
  rmh.wPitCnt0=1;
  rmh.wPitCnt1=1;
  rmh.wPitCnt2=1;
  rmh.cPitMode0=0x36;
  rmh.cPitMode1=0x76;
  rmh.cPitMode2=0xb6;
  rmh.cPitLd0=0;
  rmh.cPitLd1=0;
  rmh.cPitLd2=0;
  rmh.cPPI2B=0;
  fread(&rmh,2,1,f);
  wHLen=sizeof(rmh)>rmh.wHeaderLen?rmh.wHeaderLen:sizeof(rmh);
  fread((char *)&rmh+2,wHLen-2,1,f);
  for (int i=rmh.wHeaderLen-sizeof(rmh);i>0;i--)
    fread(&cTemp,1,1,f);
  // ╨рчсюЁ ярЁрьхЄЁют
  sh_scrbeg=rmh.wScrBegAdr;
  sh_scrlen=rmh.wScrLen;
  sh_height=rmh.cScrLines;
  sh_width=rmh.cScrCols;
  crt_param_1=rmh.acCrtParams[0];
  crt_param_2=rmh.acCrtParams[1];
  crt_param_3=rmh.acCrtParams[2];
  crt_param_4=rmh.acCrtParams[3];
  dma_mr=rmh.cDmaMR;
  crt_mreg=rmh.cCrtMReg;
  dma_begadr=rmh.wDmaBegAddr;
  dma_len=rmh.wDmaLen;
  port_c=rmh.cPortC;
  mikr_symg=rmh.cPPI2B;
  cur_x=rmh.cCurX;
  cur_y=rmh.cCurY;
  if (dma_begadr==0xffff)
    dma_begadr=sh_scrbeg;
  if (dma_len==0xffff)
    dma_len=sh_scrlen-1;

  RKSS_PIT_STATE pitState;
  pitState.wPitK0=rmh.wPitK0;
  pitState.wPitK1=rmh.wPitK1;
  pitState.wPitK2=rmh.wPitK2;
  pitState.wPitCnt0=rmh.wPitCnt0;
  pitState.wPitCnt1=rmh.wPitCnt1;
  pitState.wPitCnt2=rmh.wPitCnt2;
  pitState.bPitMode0=rmh.cPitMode0;
  pitState.bPitMode1=rmh.cPitMode1;
  pitState.bPitMode2=rmh.cPitMode2;
  pitState.bPitLd0=rmh.cPitLd0;
  pitState.bPitLd1=rmh.cPitLd1;
  pitState.bPitLd2=rmh.cPitLd2;
  LoadPITState(&pitState);
  }
else if (cModel==MODEL_A)
  {
  RKSS_APOGEY_HEADER rah;
  // єёЄрэютър яюыхщ яю єьюыўрэш■
  rah.acCrtParams[1]=0x4d;
  rah.acCrtParams[1]=0x1d;
  rah.acCrtParams[1]=0x99;
  rah.acCrtParams[1]=0xd3;
  rah.cDmaMR=0xa4;
  rah.cCrtMReg=0x27;
  rah.wPitK0=1;
  rah.wPitK1=1;
  rah.wPitK2=1;
  rah.wPitK0=1;
  rah.wPitK1=1;
  rah.wPitK2=1;
  rah.wPitCnt0=1;
  rah.wPitCnt1=1;
  rah.wPitCnt2=1;
  rah.cPitMode0=0x36;
  rah.cPitMode1=0x76;
  rah.cPitMode2=0xb6;
  rah.cPitLd0=0;
  rah.cPitLd1=0;
  rah.cPitLd2=0;
  fread(&rah,2,1,f);
  wHLen=sizeof(rah)>rah.wHeaderLen?rah.wHeaderLen:sizeof(rah);
  fread((char *)&rah+2,wHLen-2,1,f);
  for (int i=rah.wHeaderLen-sizeof(rah);i>0;i--)
    fread(&cTemp,1,1,f);
  // ╨рчсюЁ ярЁрьхЄЁют
  sh_scrbeg=rah.wScrBegAdr;
  sh_scrlen=rah.wScrLen;
  sh_height=rah.cScrLines;
  sh_width=rah.cScrCols;
  crt_param_1=rah.acCrtParams[0];
  crt_param_2=rah.acCrtParams[1];
  crt_param_3=rah.acCrtParams[2];
  crt_param_4=rah.acCrtParams[3];
  dma_mr=rah.cDmaMR;
  crt_mreg=rah.cCrtMReg;
  dma_begadr=rah.wDmaBegAddr;
  dma_len=rah.wDmaLen;
  port_c=rah.cPortC;
  cur_x=rah.cCurX;
  cur_y=rah.cCurY;
  if (dma_begadr==0xffff)
    dma_begadr=sh_scrbeg;
  if (dma_len==0xffff)
    dma_len=sh_scrlen-1;

  RKSS_PIT_STATE pitState;
  pitState.wPitK0=rah.wPitK0;
  pitState.wPitK1=rah.wPitK1;
  pitState.wPitK2=rah.wPitK2;
  pitState.wPitCnt0=rah.wPitCnt0;
  pitState.wPitCnt1=rah.wPitCnt1;
  pitState.wPitCnt2=rah.wPitCnt2;
  pitState.bPitMode0=rah.cPitMode0;
  pitState.bPitMode1=rah.cPitMode1;
  pitState.bPitMode2=rah.cPitMode2;
  pitState.bPitLd0=rah.cPitLd0;
  pitState.bPitLd1=rah.cPitLd1;
  pitState.bPitLd2=rah.cPitLd2;
  LoadPITState(&pitState);
  }
else if (cModel==MODEL_S)
  {
  RKSS_SPEC_HEADER rsh;
  // єёЄрэютър яюыхщ яю єьюыўрэш■
  fread(&rsh,2,1,f);
  wHLen=sizeof(rsh)>rsh.wHeaderLen?rsh.wHeaderLen:sizeof(rsh);
  fread(((char *)&rsh)+2,wHLen-2,1,f);
  //╓тхЄ
  if (rsh.wHeaderLen>sizeof(rsh))
    {
    fread(&b,1,1,f);
    fread(&w,2,1,f);
    if (b!=1)
      {
      for (i=0;i<12288;i++)
        color_mem[i]=0;
      for (int i=rsh.wHeaderLen-sizeof(rsh)-3;i>0;i--)
        fread(&cTemp,1,1,f);
      }
    else
      {
      // ╤ўшЄ√трхь ЎтхЄ
      for (i=0;i<3072;i++)
        {
        fread(&b,1,1,f);
        for (j=0;j<4;j++)
          {
          color_mem[i*4+j]=color_table[b & 3];
          b>>=2;
          }
        }
      for (int i=rsh.wHeaderLen-sizeof(rsh)-3075;i>0;i--)
        fread(&cTemp,1,1,f);
      }
    }
  // ╨рчсюЁ ярЁрьхЄЁют
  cMonitor=rsh.cMonVer;
  if (cMonitor>4)
    {
    MonitorWarning();
    cMonitor=0;
    }
  port_c=rsh.cPortC;
  }
else if (cModel==MODEL_O)
  {
  RKSS_ORION_HEADER roh;
  // єёЄрэютър яюыхщ яю єьюыўрэш■
  fread(&roh,2,1,f);
  wHLen=sizeof(roh)>roh.wHeaderLen?roh.wHeaderLen:sizeof(roh);
  fread((char *)&roh+2,wHLen-2,1,f);
  for (int i=roh.wHeaderLen-sizeof(roh);i>0;i--)
    fread(&cTemp,1,1,f);
  // ╨рчсюЁ ярЁрьхЄЁют
  cMonitor=roh.cMonVer;
  if (cMonitor>3)
    {
    MonitorWarning();
    cMonitor=0;
    }
  port_c=roh.cPortC;
  or_scrpage=roh.cScrPage;
  mem_page_no=roh.cMemPage;
  or_color_mode=roh.cColorMode;
  cExtBlocks=roh.cExtBlocks;
  }
else if (cModel==MODEL_M80)
  {
  RKSS_M80_HEADER r8h;
  // єёЄрэютър яюыхщ яю єьюыўрэш■
  fread(&r8h,2,1,f);
  wHLen=sizeof(r8h)>r8h.wHeaderLen?r8h.wHeaderLen:sizeof(r8h);
  fread(((char *)&r8h)+2,wHLen-2,1,f);

  // ╨рчсюЁ ярЁрьхЄЁют
  cMonitor=r8h.cMonVer;
  }
else if (cModel==MODEL_U)
  {
  RKSS_UT88_HEADER ruh;
  // єёЄрэютър яюыхщ яю єьюыўрэш■
  fread(&ruh,2,1,f);
  wHLen=sizeof(ruh)>ruh.wHeaderLen?ruh.wHeaderLen:sizeof(ruh);
  fread(((char *)&ruh)+2,wHLen-2,1,f);

  // ╨рчсюЁ ярЁрьхЄЁют
  cMonitor=ruh.cMonVer;
//  port_c=rsh.cPortC;
  }

SetMonitor();

  {
  RKSS_EMUL_HEADER reh;
  // єёЄрэютър яюыхщ яю єьюыўрэш■
  reh.cSpeed=SPEED_NORM;
  reh.cMem=MEM_RW;
  reh.cHookF809=HOOKF809_NO;
  reh.cScreen=SCR_AUTO;
  reh.cFont=FONT_RK;
  reh.cRefr=REFR_COMB;
  reh.cSound=SND_ON;
  reh.cTape=TAPE_FILE;
  fread(&reh,4,1,f);
  fread((char *)&reh+4,2,1,f);
  if (reh.dwSig==0x30384d45) //EM80
    {
    wHLen=sizeof(reh)>reh.wHeaderLen?reh.wHeaderLen:sizeof(reh);
    fread((char *)&reh+6,wHLen-6,1,f);
    }
  else
    wHLen=6;
  for (int i=reh.wHeaderLen-wHLen;i>0;i--)
    fread(&cTemp,1,1,f);
//  if (reh.dwSig==0x30384d45) //EM80
    {
    // ╨рчсюЁ ярЁрьхЄЁют ¤ьєы ЄюЁр
    ClearDisabledMenuItems();
    SetDefParams();
    f_speed.cValue=reh.cSpeed;
    f_scr.cValue=reh.cScreen;
    if (wHLen>=12)
      {
      f_snd.cValue=reh.cSound;
      if (wHLen>=13)
        f_tape.cValue=reh.cTape;
      }
    RefreshMenu();
    }
  }

char cBlocks;
fread(&cBlocks,1,1,f);

ClearMem();

LoadROM();

for (i=1;i<=(int)cBlocks;i++)
  {
  ReadMem(f);
  }

//if (cModel=MODEL_U)
//  // ─єсышЁєхь тшфхююсырёЄ№
//  for (w=0xe000;w<0xe800;w++)
//    mempages[0][w+0x800]=mempages[0][w];

for (i=1;i<=(int)cExtBlocks;i++)
  {
  ReadMem(f,1);
  }

TuneMenu();
RefreshMenu();

fclose(f);

Init();

//SetModel();
process_crt();
process_port_c();

return 1;
}

void RefreshMenuLine(MENU_LINE *pmlLine)
{
char cValue=pmlLine->cValue;
MENU_ITEM *pmi;

for (pmi=pmlLine->pmiItem;pmi->cValue!=cValue;pmi=pmi->pmiNext);
if (pmi->cDisabled)
  pmlLine->cValue=pmlLine->pmiItem->cValue;
else
  pmlLine->pmiItem=pmi;
}

void RefreshMenu()
{
RefreshMenuLine(&f_speed);
}

void TuneMenu()
{
i_spmed.cDisabled=(cModel==MODEL_S || cModel==MODEL_O ||
    cModel==MODEL_U || cModel==MODEL_M80);
}

void ClearDisabledMenuItems()
{
i_spmed.cDisabled=0;
i_scrauto.cDisabled=0;
}

static char *ppcModelList[8]={" Р А Д И О - 8 6 Р К ",
                              " М И К Р О Ш А",
                              " П А Р Т Н Е Р",
                              " А П О Г Е Й",
                              " С П Е Ц И А Л И С Т",
                              " О Р И О Н",
                              " М И К Р О - 8 0",
                              " Ю Т - 8 8",
                              };

static char *ppcROMOList[3]={"  M o n i t o r - 1  ",
                             "  M o n i t o r - 2  ",
                             "  П е т е р б у р г  ",
                              };

static char *ppcROMRList[3]={" M o n i t o r - 1 6 K ",
                             " M o n i t o r - 3 2 K ",
                             " Р а с ш и р е н н ы й ",
//                             " V e r s i o n   3 ",
                              };

static char *ppcROMSList[4]={" О р и г и н а л ь н ы й ",
                             " У л у ч ш е н н ы й ",
                             " О к о н н ы й ",
                             " П К \" Л И К \" ",
                              };

static char *ppcROMPList[2]={" М о н и т о р - В . 0 2 ",
                             " М о н и т о р - В . 0 3 ",
                              };

//static char *ppcROMM80List[2]={" О р и г и н а л ь н ы й ",
//                               " С о в м е с т и м ы й   с  Р К - 8 6 ",
//                                };
/*
static unsigned wSaveES;
*/
int ChooseModel()
{
//EnterInteractiveMode();

int res=ShowMessage(ppcModelList,8," Выбор типа ПК ",1,cModel);
if (res!=-1)
  {
  ClearDisabledMenuItems();
//  SetDefParams(); //!!!
//  LoadCfgParams();
  cModel=res;
  RefreshMenu();
  TuneMenu();
  RefreshMenu();
  cMonitor=MON_DEF;
  SetModel();
  if (need_clear_mem)
    ClearMem();
  LoadROM();
  Init();
  }
return (res!=-1);
}

int ChooseROM()
{
int res;
if (cModel!=MODEL_O && cModel!=MODEL_R && cModel!=MODEL_S &&
    cModel!=MODEL_P)// && cModel!=MODEL_M80)
  return 0;
//EnterInteractiveMode();
//StopSound(); //!!!!!
if (cModel==MODEL_O)
  res=ShowMessage(ppcROMOList,3," Выбор Монитора ",1,cMonitor-1)+1;
else if (cModel==MODEL_R)
  {
  res=ShowMessage(ppcROMRList,3," Выбор Монитора ",1,
  (cMonitor>2)?(cMonitor-1):(cMonitor-1)^1);
  res=res>1?res+1:(res^1)+1;
  }
else if (cModel==MODEL_S)
  {
  res=ShowMessage(ppcROMSList,4," Выбор Монитора ",1,
  (cMonitor>2)?(cMonitor-1):(cMonitor-1)^1);
  res=res>1?res+1:(res^1)+1;
  }
else if (cModel==MODEL_P)
  res=(ShowMessage(ppcROMPList,2," Выбор Монитора ",1,(cMonitor-1)^1)^1)+1;
//else if (cModel==MODEL_M80)
//  res=ShowMessage(ppcROMM80List,2," Выбор Монитора ",1,cMonitor-1)+1;
if (res>0)
  {
  cMonitor=res;
  SetModel(); //##### ╦єў°х яЁюёЄю шчьхэшЄ№ шь  Їрщыр ё ╧╟╙
  LoadROM();
  Init(); //!!!
  }
return (res>0);
}

void AlterSpeed(short nSysReq)
{
if (nSysReq==SYSREQ_SPEEDUP)
  {
  if (delay_sb*6/5<wMaxDelaySB)
    delay_sb=delay_sb*6/5;
  }
else
  {
  if (delay_sb*5/6>wMinDelaySB)
    delay_sb=delay_sb*5/6;
  }
}

void trim(char *s)
{
char *p=s;
char *t=s;
while (*p==' ' || *p=='\t')
  p++;
while (*p!=' ' && *p!='\t' && *p!='\0' && *p!='\n')
  *t++=*p++;
*t='\0';
}

char *szVideoDriver="";
char *szAudioDriver="";
int bMute=0;

void AnalizeCfgString(char *s)
{
if (!strncasecmp(s,"speed=",6))
  {
  if (!strcasecmp(s+6,"normal"))
    f_speed.cValue=SPEED_NORM;
  else if (!strcasecmp(s+6,"medium"))
    f_speed.cValue=SPEED_MEDIUM;
  else if (!strcasecmp(s+6,"high"))
    f_speed.cValue=SPEED_HIGH;
  }
else if (!strncasecmp(s,"textmode=",9))
  {
  if (!strcasecmp(s+9,"8x16"))
    f_scr.cValue=SCR_AUTO;
  else if (!strcasecmp(s+9,"original"))
    f_scr.cValue=SCR_GRAPH;
  }
else if (!strncasecmp(s,"sound=",6))
  {
  if (!strcasecmp(s+6,"on"))
    f_snd.cValue=SND_SB;
  else if (!strcasecmp(s+6,"off"))
    f_snd.cValue=SND_OFF;
  }
else if (!strncasecmp(s,"tape=",5))
  {
  /*if (!strcasecmp(s+5,"in"))
    f_tape.cValue=TAPE_SBIN;
  else */if (!strcasecmp(s+5,"out"))
    f_tape.cValue=TAPE_SBOUT;
  }
else if (!strncasecmp(s,"model=",6))
  {
  if (!strcasecmp(s+6,"rk86"))
    cModel=MODEL_R;
  else if (!strcasecmp(s+6,"partner"))
    cModel=MODEL_P;
  else if (!strcasecmp(s+6,"apogey"))
    cModel=MODEL_A;
  else if (!strcasecmp(s+6,"mikrosha"))
    cModel=MODEL_M;
  else if (!strcasecmp(s+6,"spec"))
    cModel=MODEL_S;
  else if (!strcasecmp(s+6,"orion"))
    cModel=MODEL_O;
  else if (!strcasecmp(s+6,"ut88"))
    cModel=MODEL_U;
  else if (!strcasecmp(s+6,"mikro80"))
    cModel=MODEL_M80;
  }
else if (!strncasecmp(s,"videodriver=",12))
  {
#ifdef linux
#else
  if (!strcasecmp(s+12,"directx"))
    szVideoDriver="directx";
  else if (!strcasecmp(s+12,"windib"))
    szVideoDriver="windib";
#endif
//  else if (!strcasecmp(s+12,"auto"))
//    szVideoDriver="";
  }
else if (!strncasecmp(s,"audiodriver=",12))
  {
#ifdef linux
  if (!strcasecmp(s+12,"alsa"))
    szAudioDriver="alsa";
#else
  if (!strcasecmp(s+12,"dsound"))
    szAudioDriver="dsound";
#endif
//  else if (!strcasecmp(s+12,"waveout"))
//    szAudioDriver="waveout";
  else if (!strcasecmp(s+12,"none"))
    bMute=1;
//  else if (!strcasecmp(s+12,"auto"))
//    szAudioDriver="";
  }
else if (!strncasecmp(s,"fullscreen=",11))
  {
  if (!strcasecmp(s+11,"on"))
    bFullScreen=1;
//  else if (!strcasecmp(s+11,"off"))
//    bFullScreen=1;
  }
RefreshMenu();
}

// ╟руЁєцрхЄ ярЁрьхЄЁ√ шч cfg-Їрщыр
void LoadCfgParams()
{
char s[81];
FILE *f;
f=fopen("emu80.cfg","r");
if (!f)
  {
//  (&com_path)[com_path_len]=0; //!!!
//  f=fopen(strcat(&com_path,"emu80.ini"),"r");
  f=exepath_fopen("emu80.cfg","r"); //!!!
  }
if (!f) return;
while (!feof(f))
  {
  fgets(s,81,f);
  trim(s);
  if ((*s)!='\0' && (*s)!=';')
    AnalizeCfgString(s);
  }
fclose(f);
}

static char *ppcHelp[19]={" Горячие клавиши в режиме эмуляции: ",
                         " ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~",
                         "  F10 ........ Меню",
                         "  F9  ........ Выбор компьютера ",
                         "  Alt-F9  .... Выбор без оч пам. ",
                         "  Alt-F8  .... Выбор Монитора ",
                         "  Alt-F10 .... Выход ",
                         "  Alt-F11 .... Сброс ",
                         "  F12 ........ Пауза/возобновление ",
                         "  Alt-F1 ..... Эта подсказка",
                         "  Alt-F2 ..... Запись snapshot'а ",
                         "  Alt-F3 ..... Загрузка (универс.) ",
                         "  Alt-F5 ..... Выбор диска A: ",
                         "  Alt-F6 ..... Выбор диска B: ",
                         "  Alt-PgUp ... Ускорение работы ",
                         "  Alt-PgDn ... Замедление работы ",
                         "  ScrollLock . Перекл. QWERTY/ЙЦУКЕН ",
                         "  Alt-Enter .. Полноэкр./Оконный  ",
                         "  Alt-A ...... О программе  ",
                         };

static char *ppcAbout[2]={"   Emu80/SDL  v. 3.11 alpha",
                          "(c) 1997-2013 Victor Pykhonin",
                          };

void ShowHelp()
{
//EnterInteractiveMode();
ShowMessage(ppcHelp,19," Подсказка ",0,0);
}

void About()
{
ShowMessage(ppcAbout,2," О программе ",0,0);
}

int fnGetColor(char *szName)
{
int nPos;
int nLen=strlen(szName);
for (nPos=nLen-1;nPos>0 && szName[nPos]!='.';nPos--);
szName=szName+nPos;
if (f_file_ss==2) // ODI
  {
  if (!strcasecmp(szName,".odi"))
    return 0x1c;
  }
else // if (f_file_ss==0 || f_file_ss==1) // RK & RSS
  {
  if (cModel!=MODEL_S && cModel!=MODEL_O && (!strcasecmp(szName,".rk")))
    return 0x1a;
  if (cModel==MODEL_R && (!strcasecmp(szName,".rkr") || !strcasecmp(szName,".gam")))
    return 0x1a;
  if (cModel==MODEL_P && !strcasecmp(szName,".rkp"))
    return 0x1a;
  if (cModel==MODEL_M && !strcasecmp(szName,".rkm"))
    return 0x1a;
  if (cModel==MODEL_A && !strcasecmp(szName,".rka"))
    return 0x1a;
  if (cModel==MODEL_S && !strcasecmp(szName,".rks"))
    return 0x1a;
  if (cModel==MODEL_O && !strcasecmp(szName,".rko"))
    return 0x1a;
  if (cModel==MODEL_O && !strcasecmp(szName,".bru"))
    return 0x1a;
  if (f_file_ss==0)
    {
    if (!strcasecmp(szName,".bsm"))
      return 0x1d;
    if (!strcasecmp(szName,".bss"))
      return 0x1d;
    if (!strcasecmp(szName,".edm"))
      return 0x1d;
    }
  else // if (f_file_ss==1) //RSS
    {
    if (!strcasecmp(szName,".rss"))
      return 0x1b;
    if (cModel==MODEL_O && !strcasecmp(szName,".odi"))
      return 0x1c;
    }
  }
return 0x17;
}

int fnFileNameCompare(const void *str1, const void *str2)
{
char * szStr1=*(char**)str1;
char * szStr2=*(char**)str2;
char * szExt1;
char * szExt2;
int nLen1=strlen(szStr1);
int nLen2=strlen(szStr2);

//printf("%s  %s", szStr1, szStr2);
//return strcmp(szStr1, szStr2);

int nPos1, nPos2;

for (nPos1=nLen1-1;nPos1>0 && szStr1[nPos1]!='.';nPos1--);
for (nPos2=nLen2-1;nPos2>0 && szStr2[nPos2]!='.';nPos2--);
if (nPos1==0) nPos1=nLen1;
if (nPos2==0) nPos2=nLen2;

szExt1=szStr1+nPos1;
szExt2=szStr2+nPos2;

int res=strcasecmp(szExt1, szExt2);
if (res) return res;
 return strcasecmp(szStr1, szStr2);
}

#ifdef linux

void ChooseFile()
{
    char *szFileName=(char *)&filename;
    szFileName[0]='\0';
    int nMaxFiles=0, nFiles=0;
    char **pszFiles=NULL;

    struct dirent *ent;
    struct stat stbuf;

    DIR *dir = opendir(".");
    if(dir)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
                continue;
            if (stat(ent->d_name, &stbuf) == -1)
                continue;
            if ((stbuf.st_mode & S_IFMT) != S_IFREG)
                continue;
            nMaxFiles++;
        }
        closedir(dir);
    }

    pszFiles = new char*[nMaxFiles];

    dir = opendir(".");
    if(dir)
    {
        while ((ent = readdir(dir)) != NULL)
        {
            if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0)
                continue;
            if (stat(ent->d_name, &stbuf) == -1)
                continue;
            if ((stbuf.st_mode & S_IFMT) != S_IFREG)
                continue;
            pszFiles[nFiles]=new char[strlen(ent->d_name)+1];
            strcpy(pszFiles[nFiles],ent->d_name);
            nFiles++;
        }
        closedir(dir);
    }
    else
        return;

    if (nFiles>0)
    {
        qsort((void *)pszFiles, nFiles, sizeof(char *),fnFileNameCompare);

        int res=ShowMessage(pszFiles,nFiles," Выбор файла ",1,0,1,70,25,fnGetColor);

        if (res!=-1 && strlen(pszFiles[res])<=255)
            strcpy(szFileName, pszFiles[res]);

        for (int i=0;i<nFiles;i++)
            delete[] pszFiles[i];
    }
    if (pszFiles)
        delete[] pszFiles;
}

#else

void ChooseFile()
{
char *szFileName=(char *)&filename;
szFileName[0]='\0';
_finddata_t fd;
int nMaxFiles=0;
long hFile=_findfirst("*",&fd);
if (hFile==-1)
  return;
if (!(fd.attrib & (_A_HIDDEN | _A_SYSTEM | _A_VOLID | _A_SUBDIR)))
  nMaxFiles++;
while(_findnext(hFile,&fd)==0)
  if (!(fd.attrib & (_A_HIDDEN | _A_SYSTEM | _A_VOLID | _A_SUBDIR)))
    nMaxFiles++;
_findclose(hFile);
char ** pszFiles;
pszFiles = new char*[nMaxFiles];

int nFiles=0;
hFile=_findfirst("*",&fd);
if (hFile==-1)
  {
  delete[] pszFiles;
  return;
  }
if (!(fd.attrib & (_A_HIDDEN | _A_SYSTEM | _A_VOLID | _A_SUBDIR)))
  {
  pszFiles[nFiles]=new char[strlen(fd.name)+1];
  strcpy(pszFiles[nFiles],fd.name);
  nFiles++;
  }
while(_findnext(hFile,&fd)==0 && nFiles<=nMaxFiles)
  if (!(fd.attrib & (_A_HIDDEN | _A_SYSTEM | _A_VOLID | _A_SUBDIR)))
    {
    pszFiles[nFiles]=new char[strlen(fd.name)+1];
    strcpy(pszFiles[nFiles],fd.name);
    nFiles++;
    }
_findclose(hFile);

qsort((void *)pszFiles, nFiles, sizeof(char *),fnFileNameCompare);

int res=ShowMessage(pszFiles,nFiles," Выбор файла ",1,0,1,70,25,fnGetColor);

if (res!=-1 && strlen(pszFiles[res])<=255)
  strcpy(szFileName, pszFiles[res]);

for (int i=0;i<nFiles;i++)
  delete[] pszFiles[i];
delete[] pszFiles;
}

#endif

char *szFileName;

int AnalizeOptions(int arg_n, char **arg_c)
{
int i, res=0;
szFileName="";
for (i=1;i<arg_n;i++)
  {
  if (!strcasecmp(arg_c[i],"-r"))
    cModel=MODEL_R;
  else if (!strcasecmp(arg_c[i],"-p"))
    cModel=MODEL_P;
  else if (!strcasecmp(arg_c[i],"-a"))
    cModel=MODEL_A;
  else if (!strcasecmp(arg_c[i],"-m"))
    cModel=MODEL_M;
  else if (!strcasecmp(arg_c[i],"-s"))
    cModel=MODEL_S;
  else if (!strcasecmp(arg_c[i],"-o"))
    cModel=MODEL_O;
  else if (!strcasecmp(arg_c[i],"-8"))
    cModel=MODEL_M80;
  else if (!strcasecmp(arg_c[i],"-u"))
    cModel=MODEL_U;
  else
    {
    szFileName=arg_c[i];
    res=1;
    };
  }
return res;
}

void Resume()
{
int nTime;
LeaveInteractiveMode();
for(;;)
  {
  ClearTimer();

// 43 FPS
  CheckKeys();
  Emulate(1024);
  if (!ProcessQuery()) break;
  PrepareScreen();
  DrawScreen();
  nTime=GetTimer();
  if (nTime<23) Delay(23-nTime);

// 65 FPS
/*  CheckKeys();
  Emulate1(683);
  if (!ProcessQuery()) break;
  PrepareScreen();
  DrawScreen();
  nTime=GetTimer();
  if (nTime<15) Delay(15-nTime);
  CheckKeys();
  Emulate1(683);
  if (!ProcessQuery()) break;
  PrepareScreen();
  DrawScreen();
  nTime=GetTimer();
  if (nTime<16) Delay(16-nTime);
*/
  if (!bMute)
    { // ┬ЄюЁющ Ўшъы - ё юцшфрэшхь рєфшю
    CheckKeys();
    Emulate(0);
    if (!ProcessQuery()) break;
    PrepareScreen();
    DrawScreen();
    if (bWaitSnd) WaitSound();
    }
  }
}

void MainFunc(int arg_n, char **arg_c)
{
//InitOnce();
//SetDefParams();
//LoadCfgParams();
ClearMem();
int res=0;
//  Init();
res=AnalizeOptions(arg_n, arg_c);
RefreshMenu();
if (res)
  {
//  printf("%s", szFileName);
  res=LoadSSName(szFileName);
  }
else
  {
  SetModel();
  LoadROM();
  TuneMenu();
  RefreshMenu();
  Init();
  Reset();
  Resume();
  }
if (res==2 || res==3)
  {
  SetModel();
  LoadROM();
  TuneMenu();
  RefreshMenu();
  Init();
  Reset();
  Resume();
  }
else if (res==1)
  Resume();
//if (res==1)
//  Resume();
//else
//  Start();
}

//-------------------------------------------------------------

static int nVretrCnt=0;
static int nCnt20ms=882;

void ProcessSample()
{
    if (nVretrCnt)
        nVretrCnt--;
    vretr_cnt=nVretrCnt;
    ProcessTime(ticks_per_44100th);
    nCnt20ms--;
    if (!nCnt20ms)
    {
        nCnt20ms=882;
        nVretrCnt=28;
        process_int();
    }
    PlayByteNoWait(GetSample());
}

void Emulate(int nSamples)
{
    //static unsigned long long qwTickCount=0;

    int nTicks;
    int nTickCnt=0;
    for(int i=0;nSamples?i<nSamples:!bWaitSnd;i++)
    {
        nTicks=InterpretOp();
        //qwTickCount+=nTicks;
        nTickCnt+=nTicks;
        if (nTickCnt>delay_sb)
        {
            nTickCnt-=delay_sb;
            ProcessSample();
        }
    }
}
