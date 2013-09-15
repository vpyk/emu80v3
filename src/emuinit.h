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

// emuinit.h

#ifndef _EMUINIT_H
#define _EMUINIT_H

extern "C" int ChooseModel();
extern "C" int ChooseROM();
extern "C" void AlterSpeed(short nSysReq);
extern "C" void ReadSector(long nDisk, long nTrack, long nHead, long nSector, uint8_t *pbBuf);
extern "C" void WriteSector(long nDisk, long nTrack, long nHead, long nSector, uint8_t *pbBuf);
extern "C" void RefreshMenu();
extern "C" void TuneMenu();
extern "C" void LoadIniParams();
extern "C" void ChooseDisk(long nDisk);
extern "C" int SaveSS();
extern "C" int LoadSS();
extern "C" void ShowHelp();
extern "C" void About();
extern "C" void GeneralError();
extern "C" void ROMError();
extern "C" void SymGenError();
extern "C" void FontsError();
extern "C" void ROMDiskError();
extern "C" void ChooseFile();
//extern "C" int LoadSSName(char *szCmdLineFileName=0);

extern "C" void DebugProc(int arg);

extern "C" unsigned char cModel;
extern "C" unsigned char acSectBufA[1024];
extern "C" unsigned char acSectBufB[1024];

void MainFunc(int arg_n, char **arg_c);
int AnalizeOptions(int arg_n, char **arg_c);
void LoadCfgParams();

extern char *szVideoDriver;
extern char *szAudioDriver;
extern int bMute;

#endif
