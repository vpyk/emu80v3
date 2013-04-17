/*
 *  RrkWin32
 *  Copyright (C) 1999-2013 Viktor Pykhonin <pyk@mail.ru>
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

// bru2rko.cpp

#include <iostream.h>
#include <stdio.h>
#include <io.h>
#include <process.h>
#include <string.h>

// Прототипы функций
void ReadError();
void WriteError();

// --------------------------------------------------------------

int main(int argn, char **argc)
{
cout << "\nBRU2RKO v.1.0\n";
cout << "(c) Copyright В.Пыхонин, Н.Новгород, 1999 г.\n\n";
cout << "Использование: bru2rko <имя_файла>\n\n";

if (argn!=2)
  {
  cout << "Ошибка в параметрах\n";
  return 1;
  }

FILE *f=fopen(argc[1],"rb");
if (!f)
  {
  cout << "Не могу открыть файл " << argc[1] << "\n";
  return 1;
  }

long lFileSize=filelength(fileno(f));

if (lFileSize<17 || lFileSize>65552)
  {
  cout << "Неверный формат файла" << "\n";
  return 1;
  }

char acFileName[13];
if (fread(acFileName,8,1,f)!=1)
  ReadError();
for (int i=0;(i<8) && (acFileName[i]!=' ');i++);
acFileName[i]=0;
strcat(acFileName,".rko");

unsigned short wBegAdr=0;
unsigned short wEndAdr;
unsigned short wLen;

wLen=lFileSize;
wEndAdr=wLen;//-1;

unsigned char *pbtBuf=new unsigned char [wLen+3];
if (!pbtBuf)
  {
  cout << "Слишком большой файл. Ошибка." << "\n";
  return 1;
  }

fseek(f,0,SEEK_SET);

if (fread(pbtBuf,wLen,1,f)!=1)
  ReadError();

pbtBuf[wLen]=0;
pbtBuf[wLen+1]=0;
pbtBuf[wLen+2]=0;

fclose(f);

unsigned short sl=0,sh=0;
for(unsigned short wAdr=0;wAdr<=wLen-2+3;wAdr++)
  {
  sl+=pbtBuf[wAdr];
  sh+=(pbtBuf[wAdr]+(sl>>8));
  sl&=0xff;
  }
sl=(sl+pbtBuf[wLen-1+3])&0xff;
unsigned short wCS=sl+((sh&0xff)<<8);

f=fopen(acFileName,"wb");
if (!f)
  {
  cout << "Не могу открыть файл " << acFileName << "\n";
  return 1;
  }

acFileName[strlen(acFileName)-4]=0; // Убираем расширение .rko

while (strlen(acFileName)<8)
  strcat(acFileName," ");

if (fwrite(acFileName,8,1,f)!=1)
  WriteError();

unsigned char btTemp=0;
for (i=0;i<64;i++)
  if (fwrite(&btTemp,1,1,f)!=1)
    WriteError();
btTemp=0xe6;
if (fwrite(&btTemp,1,1,f)!=1)
  WriteError();

if (fwrite(&wBegAdr,2,1,f)!=1)
  WriteError();
if (fwrite(((char *)&wEndAdr)+1,1,1,f)!=1)
  WriteError();
if (fwrite((char *)&wEndAdr,1,1,f)!=1)
  WriteError();

if (fwrite(pbtBuf,wLen+3,1,f)!=1)
  WriteError();

btTemp=0xe6;
if (fwrite(&btTemp,1,1,f)!=1)
  WriteError();

if (fwrite(((char *)&wCS)+1,1,1,f)!=1)
  WriteError();
if (fwrite((char *)&wCS,1,1,f)!=1)
  WriteError();

fclose(f);
cout << "Done.\n";
return 0;
}

void ReadError()
{
cout << "Ошибка чтения\n";
exit(1);
}

void WriteError()
{
cout << "Ошибка записи\n";
exit(1);
}
