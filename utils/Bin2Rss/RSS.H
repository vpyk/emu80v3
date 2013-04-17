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

// bin2rss.h

#ifndef _RSS_H
#define _RSS_H

//#define USE_OPTIONAL

#define MODEL_R 0
#define MODEL_M 1
#define MODEL_P 2
#define MODEL_A 3
#define MODEL_S 4

struct RKSS_MAIN_HEADER
  {
  unsigned long dwSig;
  unsigned char cModel;
  unsigned short wPC;
  unsigned short wBC;
  unsigned short wDE;
  unsigned short wHL;
  unsigned short wAF;
  unsigned short wSP;
  unsigned char cIF;
  };

struct RKSS_RK_HEADER
  {
  unsigned wHeaderLen;
  unsigned char cMonVer;
  unsigned wScrBegAdr;
  unsigned wScrLen;
  unsigned char cScrLines;
  unsigned char cScrCols;
  unsigned char cPortC;
  unsigned char cCurX;
  unsigned char cCurY;
#ifdef USE_OPTIONAL
  unsigned char acCrtParams[4];
  unsigned char cDmaMR;
#endif
  };

struct RKSS_PARTNER_HEADER
  {
  unsigned wHeaderLen;
  unsigned char cMonVer;
  unsigned wScrBegAdr;
  unsigned wScrLen;
  unsigned char cScrLines;
  unsigned char cScrCols;
  unsigned char cPortC;
  unsigned char cCurX;
  unsigned char cCurY;
#ifdef USE_OPTIONAL
  unsigned char acCrtParams[4];
  unsigned char cDmaMR;
#endif
  };

struct RKSS_MIKROSHA_HEADER
  {
  unsigned wHeaderLen;
  unsigned char cMonVer;
  unsigned wScrBegAdr;
  unsigned wScrLen;
  unsigned char cScrLines;
  unsigned char cScrCols;
  unsigned char cPortC;
  unsigned char cCurX;
  unsigned char cCurY;
#ifdef USE_OPTIONAL
  unsigned char acCrtParams[4];
  unsigned char cDmaMR;
  unsigned short wPitK2;
#endif
  };

struct RKSS_APOGEY_HEADER
  {
  unsigned wHeaderLen;
  unsigned char cMonVer;
  unsigned wScrBegAdr;
  unsigned wScrLen;
  unsigned char cScrLines;
  unsigned char cScrCols;
  unsigned char cPortC;
  unsigned char cCurX;
  unsigned char cCurY;
#ifdef USE_OPTIONAL
  unsigned char acCrtParams[4];
  unsigned char cDmaMR;
#endif
  };

struct RKSS_SPEC_HEADER
  {
  unsigned wHeaderLen;
  unsigned char cMonVer;
  unsigned char cPortC;
  };

struct RKSS_NULLEMUL_HEADER
  {
  unsigned long dwSig;
  unsigned wHeaderLen;
  };

#define COMPRESSION_NONE 0
#define COMPRESSION_RLE 1

#endif
