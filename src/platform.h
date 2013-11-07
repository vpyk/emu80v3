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

// platform.h

#ifndef _PLATFORM_H
#define _PLATFORM_H

#include "memory.h"

class TPlatform
{
    public:
        TPlatform();
        virtual ~TPlatform();
        virtual uint8_t ReadMem(uint16_t wAddr) = 0;
        virtual void WriteMem(uint16_t wAddr, uint8_t bValue) = 0;
        virtual uint8_t ReadPort(uint16_t wPortAddr) = 0;
        virtual void WritePort(uint16_t wPortAddr, uint8_t bValue) = 0;
        virtual void Reset() = 0;
    protected:
        TMemory *PMemory;
};


#endif  // _MEMORY_H
