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

// misc.cpp

// Разные функции

//#include <string.h>
#include <string>

//#include "emu80.h"
#include "sdllayer.h"
#include "misc.h"
#include "emuinit.h"

#include "asm.h"

using namespace std;

static int nExePathLen;
static string strExePath;

TCPU8080 *PCPU;

void SetExePath(char *szPath)
{
strExePath=szPath;
string::size_type l=strExePath.find_last_of("\\/");
strExePath=strExePath.substr(0,l);
}

FILE * exepath_fopen(const char *szName, const char *szMode)
{
string strFileName(strExePath);
strFileName+="/";
strFileName+=szName;
//#ifdef windows
//for (char *szTmp=szFileName;*szTmp!='\0';szTmp++)
//  if (*szTmp=='/') *szTmp='\\';
//#endif*/
FILE *res=fopen(strFileName.c_str(),szMode);
//delete[] szFileName;
return res;
}

void * SaveScr()
{
char * pBuf=new char[80*64*2];
memcpy(pBuf,pcScreenBuf,80*64*2); //!!!
return pBuf;
}

void RestScr(void * pBuf)
{
memcpy(pcScreenBuf,pBuf,80*64*2); //!!!
}

// Процедура инициализации. Вызывается один раз при запуске эмулятора

void InitOnce()
{
// Выделяем область памяти под сегмент 8080
mempage0=new unsigned char[65536+32]+16;

// Выделяем еще 3 области по 64К памяти под RAMDISK ПК "Орион"
mempage1=new unsigned char[65536+32]+16; //!!!
mempage2=new unsigned char[65536+32]+16; //!!!
mempage3=new unsigned char[65536+32]+16; //!!!

// Выделяем область памяти под область хранения информации о цвете "Спец-та"
color_mem=new unsigned char[0x3000];

// Выделяем область памяти под область хранения ROMDISK'а "Ориона"
romdisk_mem=new unsigned char[65536];
mempage4=romdisk_mem; // а для ЮТ это входит в эл. диск.

// Очищаем ROMDISK
memset(romdisk_mem,0,65536);

// Выделяем область памяти под область хранения знакогенератора
sym_gen=new unsigned char[0x3000];

// Создаем полный путь к файлам с ПЗУ и знакогенератором
//        call find_path //!!!

FILE *f=exepath_fopen("fonts/symgen.bin","r+b"); //!!!
fread(sym_gen,0x3000,1,f);
fclose(f);

//f_novesa=0;
//f_led=1;
//f_use8x12=1;

//wSBPort=0x220;
//wSBDMA=1;

PCPU = new TCPU8080; //!!!
PCPU->i8080_init();               //!!!
}

// Очистка памяти сегмента 8080
void ClearMem()
{
// Очистка памяти основного сегмента 8080
if (cModel!=MODEL_U)
  memset(mempage0,0,65536);
else
  memset(mempage0,0xe5,65536);

// Очистка RAMDISK ПК "Орион"
memset(mempage1,0,65536);
memset(mempage2,0,65536);
memset(mempage3,0,65536);

memset(color_mem,0x7,12288);

//if (cModel==MODEL_U)
//  memset(mempage0,0xe5,65536);

}

void LoadROM()
{
// Открываем файл с ПЗУ
FILE *f=exepath_fopen(romname,"r+b");
fread(mempage0+rom_adr,rom_len,1,f);
fclose(f);

AfterLoadRom();

if (cModel==MODEL_O)
    {
//    memset(romdisk_mem,0xff,65536);
    FILE *f=exepath_fopen("roms/romdisk.bin","r+b");
    fread(romdisk_mem,56208,1,f); //!!! ???
    fclose(f);
    }
}

// Процедуры работы с магнитофоном

static unsigned char bCurByte;
static FILE* fTapeFile=0;
static int nCloseFileCnt=0;
static int bSyncFound=0;
static int bWorking=0;
static unsigned char btSyncByte=0xE6;

void CloseTapeFile()
{
nCloseFileCnt=0;
bSyncFound=0;
bWorking=0;
if (fTapeFile)
  {
  fclose(fTapeFile);
  fTapeFile=0;
  }
}

void CloseTapeFileDelay()
{
if (++nCloseFileCnt==500)
  CloseTapeFile();
}

// Запрашивает имя и открывает файл на запись
// Если установлен признак записи синхробайта,
//; то записывает его (0E6H)
void OpenTapeFileWrite()
{
CloseTapeFile();
bWorking=1;
EnterInteractiveMode();
SetTextMode();

f_choose_file=0;
f_file_ss=0;
getfilename();
char *szFileName=(char *)&filename;
LeaveInteractiveMode();

fTapeFile=fopen(szFileName,"w+b");
if (f_synb)
  fwrite(&btSyncByte,1,1,fTapeFile);
}

// Запрашивает имя и открывает файл на чтение
// Если формат с синхробайтом, то пропускает первый байт (0E6H)
void OpenTapeFileRead()
{
unsigned char abtFirst5Bytes[5];
CloseTapeFile();
bWorking=1;
EnterInteractiveMode();
SetTextMode();

f_choose_file=1;
f_file_ss=0;
getfilename();
char *szFileName=(char *)&filename;
LeaveInteractiveMode();

fTapeFile=fopen(szFileName,"r+b");

if (!fTapeFile)
  return;

int nRead=fread(&abtFirst5Bytes,1,5,fTapeFile);
fseek(fTapeFile,0,SEEK_SET);

if (abtFirst5Bytes[0]==btSyncByte &&
    (nRead!=5 ||
     abtFirst5Bytes[1]!=btSyncByte ||
     abtFirst5Bytes[2]!=btSyncByte ||
     abtFirst5Bytes[3]!=btSyncByte ||
     abtFirst5Bytes[4]==btSyncByte))
  fread(&abtFirst5Bytes,1,1,fTapeFile); //пропускаем синхробайт
}

void WriteTapeFile(int nByte)
{
nCloseFileCnt=0;
if (!bWorking)
  OpenTapeFileWrite();
if (!fTapeFile) return;
if (bSyncFound)
  fwrite(&nByte,1,1,fTapeFile);
else
 bSyncFound=(nByte==btSyncByte);
}

int ReadTapeFile()
{
int res=0;
nCloseFileCnt=0;
if (!fTapeFile || !bWorking)
  {
  return 0xDA;
  }
//fread(&res,1,1,fTapeFile);
if (fread(&res,1,1,fTapeFile)!=1)
  {
  CloseTapeFile();
  bWorking=1;
  }
return res;
}

int ReadTapeFileSync()
{
int nByte;
if (!bWorking)
  {
  OpenTapeFileRead();
  }
else
  do
    {
    nByte=ReadTapeFile();
    }
  while (fTapeFile && nByte!=btSyncByte);
return ReadTapeFile();
}

unsigned char TranslateScanCode(unsigned char ucScanCode)
{
#ifdef linux
    const char acTranslateTable[] = { 0x1c, 0x61, 0x35, 0x63, 0x64, 0x65, 0x47, 0x48, 0x49, 0x4b, 0x4d, 0x4f, 0x50, 0x51, 0x52, 0x53 };
    ucScanCode-=0x8;
    unsigned char ucPress=ucScanCode&0x80;
    ucScanCode&=0x7f;
    if (ucScanCode>=0x60 && ucScanCode<=0x6F)
       ucScanCode=acTranslateTable[ucScanCode-0x60];
    ucScanCode|=ucPress;
#endif
return ucScanCode;
}
