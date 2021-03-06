/*
 *  Bin2Rss
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

// bin2rss.cpp

/*************************************************************\
| BIN2RSS v.1.0                                               |
| (c) Copyright ����� ��宭��, �.�����த, 1999              |
\*************************************************************/

#include <stdio.h>
#include <string.h>
#include <io.h>

#include "rss.h"

int Equals(char *pcBuf, unsigned short wAddr,unsigned short wEndAddr);
void WriteBlock(FILE *f, char *pcBuf, unsigned short wStartAddr,
                unsigned short wEnd, char cCompression);
int main();

int Equals(char *pcBuf, unsigned short wAddr,unsigned short wEndAddr)
{
char c=pcBuf[wAddr];
int n=0;
while ((pcBuf[wAddr]==c) && (wAddr<=wEndAddr) && (n<=254))
  {
  wAddr++;
  n++;
  }
return n;
}

void WriteBlock(FILE *f, char *pcBuf, unsigned short wStartAddr,
                unsigned short wEnd, char cCompression)
{
struct
 {
 char cCompType;
 unsigned short wCompLen;
 unsigned short wStart;
 unsigned short wLen;
 } h;
h.cCompType=cCompression;
h.wLen=wEnd+1;
h.wCompLen=wEnd+1;
h.wStart=wStartAddr;
fwrite(&h,sizeof(h),1,f);
char byte;
if (cCompression==COMPRESSION_NONE)
  for (unsigned short i=0;i<=wEnd;i++)
    {
    byte=pcBuf[i];           // �����, ����筮 � ������॥...
    fwrite(&byte,1,1,f);
    }
else // COMPRESSION_RLE
  {
  long lCompLenPos=ftell(f)-6;
  unsigned short cb;
  unsigned short i=0;
  unsigned short wCompLen=0;
  while (i<=wEnd)
    {
    cb=Equals(pcBuf,i,wEnd);
    if (cb<=3 && pcBuf[i]!=(unsigned char)0xCB)
      {
      byte=pcBuf[i++];
      fwrite(&byte,1,1,f);
      wCompLen++;
      }
    else
      {
      byte=0xcb;
      fwrite(&byte,1,1,f);
      byte=pcBuf[i];
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

int main()
{
int nBlocks;                // ������⢮ ������
char szInputFileName[80];   // ��� �室���� 䠩��
char szOutputFileName[80];  // ��� ��室���� 䠩��
int nFileNameLen;           // ����� ����� 䠩��
FILE *fInput;               // �室��� 䠩�
FILE *fOutput;              // ��室��� 䠩�
char cModel;                // ⨯ ��
unsigned short wStartPC;    // ���� ����᪠
unsigned short wStartAddr;  // ���� ��砫� �����
long lInputFileLen;         // ����� �室���� 䠩��
char *pcBuf;                // ���� ��� �室��� 䠩�
int nTemp;                  // �६����� ��६�����

printf("Rss2Bin v.1.0\n(c) Copyright ����� ��宭��\n������ �����த, 1999\n\n");

printf("��� rss-䠩�� [.rss]: ");
scanf("%s", szOutputFileName);

nFileNameLen=strlen(szOutputFileName);
if (nFileNameLen!=0 && (strcspn(szOutputFileName,".")==nFileNameLen))
  strcat(szOutputFileName,".rss");

fOutput=fopen(szOutputFileName,"wb");
if (!fInput)
  printf("�訡�� ������ 䠩�� %s\n\n",szInputFileName);

do
  {
  printf("��� �������� (1:��-86 2:���⭥� 3:����� 4:������ 5:���樠���� : ");
  scanf("%d",&nTemp);
  }
while (nTemp>5 || nTemp<1);
cModel=char(nTemp)-1;

printf("���� ����᪠ (hex):");
scanf("%x",&wStartPC);

RKSS_MAIN_HEADER mh;
memset(&mh,0,sizeof(mh));
mh.dwSig=0x53534b52; //RKSS
mh.cModel=cModel;
mh.wPC=wStartPC;
fwrite(&mh,sizeof(mh),1,fOutput);

if (cModel!=MODEL_S) // ��� ��樮������ ��ࠬ��஢ ��������� ᮢ������
  {                  // ��� "��-86", "�����", "���⭥�" � "������"
  RKSS_RK_HEADER rrh;
//  memset(&rrh,0,sizeof(rrh));
  rrh.cMonVer=0;
  rrh.wHeaderLen=sizeof(rrh);
  if (cModel!=MODEL_A)
    rrh.wScrBegAdr=0x76d0;
  else
    rrh.wScrBegAdr=0xe1d0;
  rrh.wScrLen=2340;
  rrh.cScrLines=30;
  rrh.cScrCols=78;
  if (cModel!=MODEL_P)
    rrh.cPortC=0;
  else
    rrh.cPortC=2;
  rrh.cCurX=0;
  rrh.cCurY=0;
  fwrite(&rrh,sizeof(rrh),1,fOutput);
  }
else // "���樠����"
  {        
  RKSS_SPEC_HEADER rsh;
//  memset(&rsh,0,sizeof(rsh));
  rsh.cMonVer=0;
  rsh.wHeaderLen=sizeof(rsh);
  rsh.cPortC=0;
  fwrite(&rsh,sizeof(rsh),1,fOutput);
  }

RKSS_NULLEMUL_HEADER reh;
reh.dwSig=0x4c4c554e; // "NULL"
reh.wHeaderLen=sizeof(reh);
fwrite(&reh,sizeof(reh),1,fOutput);

do
  {
  printf("������⢮ ������ ������: ");
  scanf("%d", &nBlocks);
  }
while (nBlocks<1);

fwrite(&nBlocks,1,1,fOutput);

for (int i=1;i<=nBlocks;i++)
  {
  do
    {
    printf("\n��� bin-䠩�� � ������ #%d [.bin]: ",i);
    scanf("%s", szInputFileName);
  
    nFileNameLen=strlen(szInputFileName);
    if (nFileNameLen!=0 && (strcspn(szInputFileName,".")==nFileNameLen))
      strcat(szInputFileName,".bin");
  
    fInput=fopen(szInputFileName,"rb");
    if (!fInput)
      printf("�訡�� ������ 䠩�� %s\n\n",szInputFileName);
    }
  while (!fInput);

  lInputFileLen=filelength(fileno(fInput));
  printf("������ 䠩�� %lXH\n",lInputFileLen);

  printf("��砫�� ���� ����� (hex):");
  scanf("%x",&wStartAddr);

  pcBuf=new char[(unsigned short)lInputFileLen];
  fread(pcBuf,1,(unsigned short)lInputFileLen,fInput);
  WriteBlock(fOutput, pcBuf, wStartAddr,
             (unsigned short)lInputFileLen-1,COMPRESSION_RLE);
  delete[] pcBuf;
  fclose(fInput);
  }
fclose(fOutput);
printf("\nDone !\n");
return 0;
}
