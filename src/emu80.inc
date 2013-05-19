;
;  Emu80/SDL 3.x
;  Copyright (C) 1997-2013 Viktor Pykhonin <pyk@mail.ru>
;
;  This program is free software: you can redistribute it and/or modify
;  it under the terms of the GNU General Public License as published by
;  the Free Software Foundation, either version 3 of the License, or
;  (at your option) any later version.
;
;  This program is distributed in the hope that it will be useful,
;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;  GNU General Public License for more details.
;
;  You should have received a copy of the GNU General Public License
;  along with this program.  If not, see <http://www.gnu.org/licenses/>.

; File: emu80.inc

MODEL_R equ 0
MODEL_M equ 1
MODEL_P equ 2
MODEL_A equ 3
MODEL_S equ 4
MODEL_O equ 5
MODEL_M80 equ 6
MODEL_U equ 7

MON_DEF equ 0

MON_O_1 equ 1
MON_O_2 equ 2
MON_O_PET equ 3

MON_R_32 equ 1
MON_R_16 equ 2
MON_R_4K equ 3
;MON_R_3 equ 4

MON_S_1 equ 1
MON_S_2 equ 2
MON_S_M equ 3
MON_S_L equ 4

MON_M80_O equ 1
MON_M80_R equ 2

SPEED_NORM equ 2
SPEED_MEDIUM equ 1
SPEED_HIGH equ 0

MEM_NONE equ 0
MEM_W equ 1
MEM_RW equ 2

HOOKF809_YES equ 1
HOOKF809_NO equ 0

SCR_AUTO equ 2
SCR_LOW equ 0
SCR_HIGH equ 1
SCR_GRAPH equ 3

FONT_OEM equ 0
FONT_RK equ 1

REFR_RT equ 0
REFR_50 equ 1
REFR_COMB equ 2

SND_ON equ 0
SND_OFF equ 1
SND_SB equ 2

TAPE_FILE equ 0
TAPE_SBOUT equ 1
TAPE_SBIN equ 2

SYSREQ_MENU equ 0
SYSREQ_RESET equ 1
SYSREQ_MODEL equ 2
SYSREQ_SAVESS equ 3
SYSREQ_LOADSS equ 4
SYSREQ_HELP equ 5
SYSREQ_QUIT equ 6
SYSREQ_MODELNOCLEAR equ 7
SYSREQ_DISKA equ 8
SYSREQ_DISKB equ 9
SYSREQ_ROM equ 10
SYSREQ_SPEEDUP equ 11
SYSREQ_SPEEDDOWN equ 12
SYSREQ_PAUSE equ 13
SYSREQ_FULLSCREEN equ 14
SYSREQ_ABOUT equ 15

VM_TEXT30 equ 0
VM_TEXT40 equ 1
VM_TEXT60 equ 2
VM_GRAPH512 equ 3
VM_GRAPH800 equ 4


%macro __align_sp 0
     mov ebx,esp
     and ebx,3
     sub esp,ebx
     push ebx
%endmacro

%macro __restore_sp 0
     pop ebx
     add esp,ebx
%endmacro

%macro cglobal 1
%ifdef win32
          global _%1
%define %1 _%1
%else
          global %1
%endif
%endmacro

%macro cextern 1
%ifdef win32
          extern _%1
%define %1 _%1
%else
          extern %1
%endif

%endmacro

