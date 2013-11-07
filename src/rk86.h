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

// rk86.h

#ifndef _RK86_H
#define _RK86_H

#include "platform.h"

class TRK86 : public TPlatform
{
public:
        TRK86();
        virtual ~TRK86();
        virtual uint8_t ReadMem(uint16_t wAddr) = 0;
        virtual void WriteMem(uint16_t wAddr, uint8_t bValue) = 0;
        virtual uint8_t ReadPort(uint8_t wPortAddr) = 0;
        virtual void WritePort(uint8_t wPortAddr, uint8_t bValue) = 0;
        virtual void InitMachine() = 0;
};

#endif // _RK86_H
