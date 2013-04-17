/*
 *  RrkWin32
 *  Copyright (C) 1998-2013 Viktor Pykhonin <pyk@mail.ru>
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

// rrk.cpp

#include <stdio.h>
#include <conio.h>
#include <io.h>
#include <iostream.h>
#include <string.h>
#include "snd.h"

int speed;
long startcount=0;

#define BUFSIZE 32768

char pcBuf[BUFSIZE];
long nCurPos;

void starttime()
{
startcount=nCurPos;
}

int passed()
{
return ((nCurPos-startcount)>speed*3);
}

int bDif=0;
int bSyn=0;

int getval()
{
int temp;
static int oldval=0;
if ((nCurPos%BUFSIZE)==0)
  {
  RecordBlock(pcBuf);
  if (bDif)
    for (int i=0;i<BUFSIZE;i++)
      {
      temp=oldval;
      oldval=pcBuf[i];
    pcBuf[i]=(oldval-temp)*8+128;
//      if ((signed char)(oldval-temp)>0)
//        pcBuf[i]=192;
//      else
//        pcBuf[i]=64;
      }
  cout << "*";
  }
unsigned char val=pcBuf[nCurPos%BUFSIZE];
if (val>131) val=1;
else val=0;
nCurPos++;
return val;
}

void delay()
{
for (int i=0;i<speed;i++)
  getval();
}

int readbit(int &bit)
{
delay();
int temp;

starttime();
temp=getval();
while (temp==getval())
  if (passed())
    return 0;
bit=temp;
return 1;
}

static unsigned char mask;

int syncbyte()
{
unsigned char byte=0;
int bit;
for(;;)
  {
  if (!readbit(bit))
    return 0;
  byte=(byte<<1) | bit;
  if (byte==0xe6)
    {
    mask=0;
    return 1;
    }
  if (byte==0x19)
    {
    mask=0xff;
    return 1;
    }
  }
}

int readbyte(unsigned char &byte)
{
int bit,i;
for(i=0;i<8;i++)
  {
  if (!readbit(bit))
    return 0;
  byte=(byte<<1) | bit;
  }
byte^=mask;
return 1;
}

unsigned int i;
unsigned char *p;

int readfile()
{
i=0;
nCurPos=0;
StartRecording(BUFSIZE);

cout << "Поиск синхробайта : ";
if (!syncbyte())
  {
  return 0;
  }
cout <<" синхробайт найден.\r\nЧтение файла : ";
while (readbyte(p[i]))
  i++;
StopRecording();
return 1;
}

char *sb="ц";

int main(int argc, char **argv)
{
cout << "\nReadRKWin v.1.50 - утилита чтения файлов в формате РК с МЛ.\n";
cout << "Автор: В.Пыхонин - Н.Новгород, 1998 г.\n\n";

cout << "Использование : RRKWIN32 [/d|-d|-s|/s]\n";
cout << "";
cout << "Ключи:   -d : для чтения плохих кассет\n";
cout << "         -s : для записи файла в формате с первым синхробайтом\n\n";

if (argc>3)
  {
  cout << "Ошибка в параметрах !\n";
  return 1;
  }

if (argc==2)
  if (strcmp(argv[1],"-s")==0 && strcmp(argv[1],"/s")==0 &&
      strcmp(argv[1],"-d")==0 && strcmp(argv[1],"/d")==0)
    {
    cout << "Ошибка в параметрах !\n";
    return 1;
    }
if (argc==3)
  if (strcmp(argv[1],"-s")==0 && strcmp(argv[1],"/s")==0 &&
      strcmp(argv[1],"-d")==0 && strcmp(argv[1],"/d")==0 &&
      strcmp(argv[2],"-s")==0 && strcmp(argv[2],"/s")==0 &&
      strcmp(argv[2],"-d")==0 && strcmp(argv[2],"/d")==0)
    {
    cout << "Ошибка в параметрах !\n";
    return 1;
    }

if (argc>=2)
  {
  if ((strcmp(argv[1],"-d")==0)||(strcmp(argv[1],"/d")==0))
    bDif=1;
  if ((strcmp(argv[1],"-s")==0)||(strcmp(argv[1],"/s")==0))
    bSyn=1;
  }

if (argc==3)
  {
  if ((strcmp(argv[2],"-d")==0)||(strcmp(argv[2],"/d")==0))
    bDif=1;
  if ((strcmp(argv[2],"-s")==0)||(strcmp(argv[2],"/s")==0))
    bSyn=1;
  }

if (!bDif)
  cout << "Нормальный режим\n\n";
else
  cout << "Режим чтения плохих кассет. При использовании этого режима" << endl
       << "убедитесь, что регулятор уровня установлен в положение," << endl
       << "исключающее перегрузки.\n\n";

cout << "Запись синхробайта : " << (bSyn?"да":"нет") << "\n\n";

char szFileName[80];
int res,ch;

if (!FindInputDevice())
  {
  cout << "Не найдено устройство звукогого ввода !\n";
  return 1;
  }
if (!OpenInputDevice(1,44100,8))
  {
  cout << "Не могу открыть устройство ввода !\n";
  return 1;
  }
cout << "Константа чтения (норм.=30) : ";
//while (kbhit()) getch();
//scanf("%d",&speed);
cin >> speed;
p=new unsigned char [40000];
if (!p)
  {
  cout << "Ошибка выделения памяти\n";
  return 1;
  }
//while (kbhit()) getch();
cout << "\nВключите магнитофон и нажмите на любую клавишу (ESC-выход).\n";
if (getch()==27)
  {
  delete[] p;
  CloseInputDevice();
  return 0;
  }
for(;;)
  {
  res=readfile();
  CloseInputDevice();
  if (res)
    cout << "\nФайл прочитан. ESC-выход, Enter-запись, др.-повтор.\n\n";
  else
    cout << "\nЧтение прервано. ESC-выход, др.-повтор.\n\n";
//  while (kbhit()) getch();
  ch=getch();
  if (ch==27 || (res && (ch==13))) break;
  if (!OpenInputDevice(1,44100,8))
    {
    cout << "Не могу открыть устройство ввода !\n";
    return 1;
    }
  }
if (res && (ch==13))
  for(;;)
    {
//    cin.ignore(2);
    cout << "Имя файла на диске : ";
    szFileName[0]=0;
//    while (kbhit()) getch();
//    cscanf(szFileName);
    cin >> szFileName;
    if (szFileName[0]==0)
      {
      res=0;
      break;
      }
    FILE *f=fopen(szFileName,"wb");
    if (!f)
      {
      cout << "Не могу открыть файл " << szFileName << "\n";
      continue;
      }
    if (bSyn)
      fwrite(sb,1,1,f);
    fwrite(p,i,1,f);
    fclose(f);
    break;
    }
delete[] p;
if (res && (ch==13))
  cout << "\nOk! Файл записан.\n";
//while (kbhit()) getch();
return 0;
}


