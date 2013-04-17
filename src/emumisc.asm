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

; File: emumics.asm

; Содержит разнообразные служебные процедуры

[bits 32]

%include "emu80.inc"

[global sysreq]
[global sysreq_code]

;public setscr25
[global setscr30]
[global setscr40]
[global setscr60]

[global find_path]
[global com_path]
[global com_path_len]

[global test_sys_key_scan]
[global alt_flag]
[global translate_sysreq_code]

cextern SetVideoMode

[section .bss]

; Код системной клавиши
;sysreq_code resw 1

; Признак нажатия Alt (режим чт/зап)
alt_flag resb 1
alt_flag_mem resb 1

[section .data]

; Код системной клавиши
sysreq_code dw 0

; Буфер из одного символа
key dw 0

; Флаг - устанавливается при нажатии F10
sysreq db 0

[section .text]

;Z=1, если в AX - скан-код системной клавиши и запоминает ее код в sysreq_code
test_sys_key_scan:
        cmp ah,44h   ; F10
        je tsks1
        cmp ah,58h   ; F12
        je tsks1
        cmp ah,43h   ; F9
        je tsks1
        cmp byte [alt_flag],1
        jne tsks1
        cmp ah,57h ; Alt-F11
        je tsks1
        cmp ah,3bh ; Alt-F1
        je tsks1
        cmp ah,3ch ; Alt-F2
        je tsks1
        cmp ah,3dh ; Alt-F3
        je tsks1
        cmp ah,3fh ; Alt-F5
        je tsks1
        cmp ah,40h ; Alt-F6
        je tsks1
        cmp ah,42h ; Alt-F8
        je tsks1
        cmp ah,49h ; Alt-PgUp
        je tsks1
        cmp ah,51h ; Alt-PgDn
        je tsks1
        cmp ah,1ch ; Alt-Enter
        je tsks1
        cmp ah,1eh ; Alt-A
        je tsks1
        ret
tsks1:  mov word [sysreq_code],ax
        mov al, byte [alt_flag]
        mov byte [alt_flag_mem],al
        ret

; Преобразовывает код системной клавиши в код действия
translate_sysreq_code:
        push ebx
        mov ebx, sysreq_code+1
        cmp byte [ebx],44h
        jne tsc0
        mov byte [ebx],SYSREQ_MENU
        cmp byte [alt_flag_mem],1
;        jne tscQ
       je tsc01
       jmp tscQ
tsc01:  mov byte [ebx],SYSREQ_QUIT
        jmp tscQ
tsc0:   cmp byte [ebx],71h
        je tsc01
tsc1:   cmp byte [ebx],57h
        jne tsc2
tsc3:   mov byte [ebx],SYSREQ_RESET
        jmp tscQ
tsc2:   cmp byte [ebx],8Bh
        je tsc3

        cmp byte [ebx],42h
        jne tsc18
tsc19:  mov byte [ebx],SYSREQ_ROM
        jmp tscQ
tsc18:  cmp byte [ebx],6fh
        je tsc19

tsc16:  cmp byte [ebx],49h
        jne tsc17
        mov byte [ebx],SYSREQ_SPEEDUP
        jmp tscQ

tsc17:  cmp byte [ebx],51h
        jne tsc44
        mov byte [ebx],SYSREQ_SPEEDDOWN
        jmp tscQ

tsc44:  cmp byte [ebx],58h
        jne tsc4
        mov byte [ebx],SYSREQ_PAUSE
        jmp tscQ

tsc4:   cmp byte [ebx],43h
        jne tsc5
        mov byte [ebx],SYSREQ_MODEL
        cmp byte [alt_flag_mem],1
        jne tscQ
tsc02:  mov byte [ebx],SYSREQ_MODELNOCLEAR
        jmp tscQ
tsc5:   cmp byte [ebx],70h
        je tsc02

        cmp byte [ebx],3ch
        jne tsc6
tsc7:   mov byte [ebx],SYSREQ_SAVESS
        jmp tscQ
tsc6:   cmp byte [ebx],69h
        je tsc7

        cmp byte [ebx],3dh
        jne tsc8
tsc9:   mov byte [ebx],SYSREQ_LOADSS
        jmp tscQ
tsc8:   cmp byte [ebx],6Ah
        je tsc9

        cmp byte [ebx],3fh
        jne tsc12
tsc13:  mov byte [ebx],SYSREQ_DISKA
        jmp tscQ
tsc12:  cmp byte [ebx],6Ch
        je tsc13

        cmp byte [ebx],40h
        jne tsc14
tsc15:  mov byte [ebx],SYSREQ_DISKB
        jmp tscQ
tsc14:  cmp byte [ebx],6Dh
        je tsc15

        cmp byte [ebx],1Ch
        jne tsc20
        mov byte [ebx],SYSREQ_FULLSCREEN
        jmp tscQ
tsc20:
        cmp byte [ebx],1Eh
        jne tsc21
        mov byte [ebx],SYSREQ_ABOUT
        jmp tscQ
tsc21:
        cmp byte [ebx],3bh
        jne tsc10
tsc11:  mov byte [ebx],SYSREQ_HELP
        jmp tscQ
tsc10:  jmp tsc11
tscQ:   ;mov byte ptr cs:[alt_flag],0
        pop ebx
        ret

;### Процедуры для работы с экраном ###

;Устанавливает режим 80x30
setscr30:
        pusha
      __align_sp
        mov eax,VM_TEXT30
        push eax
        call SetVideoMode
        add esp,4
      __restore_sp
        popa
        ret

setscr40:
        pusha
      __align_sp
        mov eax,VM_TEXT40
        push eax
        call SetVideoMode
        add esp,4
      __restore_sp
        popa
        ret

setscr60:
        pusha
      __align_sp
        mov eax,VM_TEXT60
        push eax
        call SetVideoMode
        add esp,4
      __restore_sp
        popa
        ret

;### Прочие процедуры ###

; заносит путь к com-файлу в сторку com_path
;find_path proc
;        push es
;        push ds
;        push cs
;        pop es
;        push si
;        mov si,cs
;        mov ds,si
;        mov si,word ptr cs:[2ch]
;        mov ds,si
;        xor si,si
;fp1:    cmp byte ptr ds:[si],0
;        jz fp2
;fp4:    lodsb
;        test al,al
;        jnz fp4
;        jmp fp1
;fp2:    inc si
;        cmp word ptr ds:[si],1
;        jne fp5
;        inc si
;        inc si
;        mov di,offset com_path
;fp6:    lodsb 
;        test al,al
;        je fp7
;        stosb
;        inc word ptr es:[com_path_len]
;        jmp fp6
;fp7:    dec si
;        dec word ptr es:[com_path_len]
;        cmp byte ptr ds:[si],'\'
;        jne fp7
;        inc word ptr es:[com_path_len]
;        inc word ptr es:[com_path_len]
;fp5:    pop si
;        pop ds
;        pop es
;        ret
;com_path db 128 dup (0)
;com_path_len dw 0
;find_path endp

;end
