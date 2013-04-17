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

// scrdraw.cpp

// Отрисовка экрана эмулируемых ПК
// Используются функции модуля sdllayer

#include <string.h>

#include "scrdraw.h"
#include "asm.h"
//#include "emu80.h"
#include "sdllayer.h"
#include "emuinit.h"

void DrawTextScreenInGraph()
{
int i,j,k,l;

unsigned char linebuf[1280];

memset(linebuf,0,1280);

////////////////////////////////////////////////
unsigned start=dma_begadr;
int lines,cols,height,u_line,curs_x,curs_y;
if (cModel==MODEL_M80 || cModel==MODEL_U)
  {
  lines=32;
  cols=64;
  height=8;
  curs_x=-1;
  curs_y=-1;
  }
else
  {
  lines=(crt_param_2&0x3f)+1;
  cols=(crt_param_1&0x7f)+1;
  height=(crt_param_3&0xf)+1;
  curs_x=cur_x;
  if (cModel==MODEL_P)
    curs_x--;
  curs_y=cur_y;
  if (cols>80)
    cols=80;
  u_line=((crt_param_3&0xf0)>>4);
  }
////////////////////////////////////////////////

static int old_vidofs=0;
static int old_full_height=0;

unsigned char *vid=E_BeginPaint();
int pitch=GetPitch();

int height_ofs=height>8?1:0;

//int vidofs=153600-((lines*80+cols)*height*4);
int vidofs=98304-(lines*pitch*height/2)+pitch/2-cols*3;
if (vidofs<0) vidofs=128;

if (vidofs!=old_vidofs || old_full_height!=height*lines)
  {
  for (long l=0;l<pitch*384;l++)
    vid[l]=0;
  }
old_vidofs=vidofs;
old_full_height=height*lines;

int h=height<=8?height:9;
static int blick_cnt=0;

unsigned char and_mask;
unsigned char xor_mask;
unsigned char outbyte;
unsigned long bufval;

unsigned char line_vsp;
unsigned char scr_vsp;

unsigned char *lp,*tlp;
unsigned char *vp, *tvp, *mp;

// д®а¬ЁагҐ¬ нЄа ­
unsigned char bt;
unsigned short ofs=0;
int neg=0;
int blink=0;
int und=0;
scr_vsp=0xff;
if (cModel!=MODEL_P)
  ofs=128*3;
if (cModel==MODEL_A)
  ofs=int_flag?128*9:128*8;
else if (cModel==MODEL_M)
  ofs=mikr_symg?128*10:128*11;
vp=vid+vidofs;
mp=(unsigned char *)(mempage0+start);
for (i=0;i<lines;i++)
  {
  line_vsp=0xff;
  if (cModel!=MODEL_M80 && cModel!=MODEL_U)
    for (j=0;j<cols;j++)
      {
  //    bt=mem[start+i*cols+j];
      bt=*mp++;
      if (bt<0x80)
        {
        and_mask=(blink && (blick_cnt<16))?0:0x3f;
        xor_mask=neg?0x3f:0;
        for (k=0;k<h;k++)
          {
  //        outbyte=sym_gen[(ofs+bt)*8+k];
  
          if (height_ofs)
            {
            if (k)
              outbyte=sym_gen[(ofs+bt)*8+k-1];
            else
              outbyte=0;
            }
          else
            outbyte=sym_gen[(ofs+bt)*8+k];
  
          if ((k==u_line) && und)
            outbyte=0x3f;
          if ((k==u_line) && (curs_x==j) && (curs_y==i) && (blick_cnt&8))
            outbyte^=0x3f;
          outbyte=outbyte&and_mask^xor_mask;
          outbyte&=scr_vsp;
          outbyte&=line_vsp;
          linebuf[j+k*80]=outbyte;
          }
        for (k=h;k<height;k++)
          {
          outbyte=0;
          if ((k==u_line) && und)
            outbyte=0x3f;
          if ((k==u_line) && (curs_x==j) && (curs_y==i) && (blick_cnt&8))
            outbyte=0x3f;
          outbyte=outbyte^xor_mask;
          linebuf[j+k*80]=outbyte;
          }
        }
      else if (bt<0xc0)
        {
        if (cModel==MODEL_P)
          {
          ofs=(((bt&4)>>2)|((bt&8)>>2)|((bt&1)<<2))*128;
          neg=bt&0x10;
          blink=bt&0x2;
          und=bt&0x20;
          }
        else if (cModel==MODEL_A || cModel==MODEL_R)
          {
          neg=bt&0x10;
          blink=bt&0x2;
  //        und=bt&0x20;
          }
        else if (cModel==MODEL_M)
          {
          blink=bt&0x2;
          }
  //      else if (cModel==MODEL_U)
  //        {
  //        neg=bt&0x80;
  //        }
        for (k=0;k<height;k++)
          {
          if (neg || ((k==u_line) && (curs_x==j) && (curs_y==i) && (blick_cnt&8)))
            linebuf[j+k*80]=0x3f;
          else
            linebuf[j+k*80]=0;
          }
        }
      else if (bt>=0xf0)
        {
        scr_vsp=bt&2?0:0xff;
        line_vsp=~scr_vsp;
        }  
      else
        { // Псевдографический код - заменяем на полосатый прямоугольник ;)
        for (k=0;k<h;k+=2)
          {
          ;
          outbyte=scr_vsp&line_vsp&((bt&2 && (blick_cnt<16))?0:0x3f);
          linebuf[j+k*80]=outbyte;
          linebuf[j+k*80+80]=0;
          }
        }
      }
  else  // Микро-80 или ЮТ-88
    for (j=0;j<cols;j++)
      {
      bt=*mp++;
        for (k=0;k<8;k++)
          {
          outbyte=sym_gen[(384+(bt&0x7f))*8+k];
          if (cModel==MODEL_U && (bt&0x80))
            outbyte^=0x3f;
          else if (cModel==MODEL_M80 && *(mp-0x801))
            outbyte^=0x3f;
          linebuf[j+k*80]=outbyte;
          }
      }

  lp=linebuf;
  for (k=0;k<height;k++)
    {
    tvp=vp;
    tlp=lp;
    for (j=0;j<cols;j++)
      {
      bt=*(tlp++);
      for (l=0;l<6;l++)
        *(tvp++)=((bt<<=1)&64)?7:0;
      }
    vp=vp+pitch;
    lp=lp+80;
    }
  }

blick_cnt++;
blick_cnt%=32;
////////////////////////////////////////////////
E_EndPaint();
}
