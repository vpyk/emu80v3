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

; File: emurom.asm

; Содержит процедуры, заменяющие некоторые стандартные из ПЗУ

[bits 32]

%include "emu80.inc"

cglobal sh_height
cglobal sh_width
cglobal sh_scrbeg
cglobal sh_scrlen
cglobal romvar_addr
cglobal curpos_addr

[global emF809]
[global emF803]
[global emF81B]
[global emF812]
[global emF80C]
[global emF806]
[global symtable_ptr]
[global cur_offs]
[global refresh_scr]
[global close_file]
[global close_file_delay]
[global f_novesa]
[global set_graph_mode]

cextern scr_beg               ; word
cextern scr_end               ; word
cextern f_scr                 ; byte
cextern set_new_09_vect       ; near ?
cextern cur_xy                ; word
cextern cur_corr              ; word
cextern cModel                ; byte
cextern dma_begadr            ; word
cextern dma_len               ; word
cextern mikr_symg             ; byte
cextern pcScreenBuf             ; dword
cextern OpenTapeFile
cextern CloseTapeFile
cextern CloseTapeFileDelay
cextern ReadTapeFile
cextern ReadTapeFileSync
cextern WriteTapeFile

[extern mem]                   ; dword
[extern set_cursor_pos]        ; near
;[extern restore_09_vect]       ; near
;[extern f_choose_file]         ; byte
;[extern f_file_ss]             ; byte
;[extern f_synb]                ; byte
;[extern test_sys_key]          ; near
;[extern sysreq]                ; byte
;[extern sysreq_code]           ; byte
;[extern set_text_mode]         ; near
;[extern write_byte_spec]       ; word
;[extern write_byte_spec_640]   ; near
;[extern write_byte_spec_800]   ; near
;[extern write_byte_orion]      ; word
;[extern write_byte_orion_640]  ; near
;[extern write_byte_orion_800]  ; near
;[extern color_mem]             ; dword
;[extern cur_color_code]        ; byte
;[extern StopSound]             ; near
;[extern DrawScr]               ; near
;[extern mempage0]              ; dword
;[extern mempage1]              ; dword
;[extern old_or_colormode]      ; byte
;[extern old_or_scrpage]        ; byte

;- DATA ----------------------------------------------------------------------
[segment .data]

;Переменные, определяющие режим дисплея в режиме Авто
sh_scrbeg dw 76d0h  ; начало экранной области
sh_height db 30     ; количество строк
sh_width db 78      ; количество символов в строке
sh_scrlen dw 78*30  ; длина экранной области (обычно sh_width*sh_height)

s_startline db 0    ; отступ сверху при отображении на PC

close_file_cnt dw 0 ; счетчик вызова 0f81b. при достижении 500 закр файл

; Текущая символьная таблица
symtable_ptr dd 0; ? symtablerk

; Таблица перекодировки для загружаемого знакогенератора РК
symtablerk:
 db 00h,01h,02h,03h,04h,05h,06h,07h,08h,09h,0Ah,0Bh,0Ch,0Dh,0Eh,0Fh
 db 10h,11h,12h,13h,14h,15h,16h,17h,18h,19h,1Ah,1Bh,1Ch,1Dh,1Eh,1Fh
 db 20h,21h,22h,23h,24h,25h,26h,27h,28h,29h,2Ah,2Bh,2Ch,2Dh,2Eh,2Fh
 db 30h,31h,32h,33h,34h,35h,36h,37h,38h,39h,3Ah,3Bh,3Ch,3Dh,3Eh,3Fh
 db 40h,41h,42h,43h,44h,45h,46h,47h,48h,49h,4Ah,4Bh,4Ch,4Dh,4Eh,4Fh
 db 50h,51h,52h,53h,54h,55h,56h,57h,58h,59h,5Ah,5Bh,5Ch,5Dh,5Eh,5Fh
 db 0D0h,0D1h,0D2h,0D3h,0D4h,0D5h,0D6h,0D7h
 db 0D8H,0D9h,0DAh,0DBh,0DCh,0DDh,0DEh,0DFh
 db 0F0h,0F1h,0F2h,0F3h,0F4h,0F5h,0F6h,0F7h
 db 0F8H,0F9h,0FAh,0FBh,0FCh,0FDh,0FEh,0FFh

; Таблица перекодировки для второго знакогенератора Микроши
symtablemikr2:
 db 00h,0BCh,0C9h,0BBh,0CCh,0CEh,0CBh,0CAh,0BAh,0CDh,00h,0CEh,0C8h,00h,0Fh,0Bh
 db 0Eh,1Dh,2Ah,2Ah,2Ah,2Ah,09h,0B0h,0B0h,0Eh,0B0h,09h,0B0h,1Eh,0B0h,0B0h
 db 20h,21h,22h,23h,24h,25h,26h,27h,28h,29h,2Ah,2Bh,2Ch,2Dh,2Eh,2Fh
 db 30h,31h,32h,33h,34h,35h,36h,37h,38h,39h,3Ah,3Bh,3Ch,3Dh,3Eh,3Fh
 db 'юабцдефгхийклмно'
 db 'пярстужвьызшэщч',017h
 db 'ЮАБЦДЕФГХИЙКЛМНО'
 db 'ПЯРСТУЖВЬЫЗШЭЩЧ',017H

; Разница между физ. и логич. координатами курсора
cur_offs dw 0

; Размерность экрана - 64x25 или 78x30
scr_dim dw 183fh

[segment .bss]

romvar_addr resw 1
curpos_addr resw 1

;- TEXT ----------------------------------------------------------------------
[segment .text]

; Закрывает открытый файл
close_file:
        pusha
      __align_sp
        call CloseTapeFile
      __restore_sp 
        popa
        ret
;close_file endp

; Закрывает открытый файл, если вызвана более, чем 500 раз
close_file_delay:
        pusha
      __align_sp
        call CloseTapeFileDelay
      __restore_sp 
        popa
        ret
;close_file_delay endp

;####===-- Эмуляция 0F80CH --===####
; Вывод байта на магнитофон
; C - выводимый байт

emF80C: pusha

        cmp byte [cModel],MODEL_M80
        je emc1
        mov al,cl
emc1:   movzx eax,al
      __align_sp
        push eax
        call WriteTapeFile
        add esp,4
      __restore_sp  
        popa
        ret

;####===-- Эмуляция 0F806H --===####
; Чтение байта с магнитофона
; Вход:
; A=8 - без ожидания синхробайта (E6)
; A=FF - с ожиданием синхробайта

emF806: push ebx
        push ecx
        push edx
        push esi
        push edi
        push ebp
      __align_sp
        cmp al,08h
        jne em06_1
        call ReadTapeFile
        jmp em06_2
em06_1: call ReadTapeFileSync
em06_2: 
      __restore_sp  
        pop ebp
        pop edi
        pop esi
        pop edx
        pop ecx
        pop ebx
        ret

pos_cursor:
        pusha
        mov dx,word [cur_xy]
        sub dx,word [cur_corr]
        add dx,word [cur_offs]
        ;mov ah,2
        ;mov bh,0
        call set_cursor_pos; int 10h
        popa
        ret
;pos_cursor endp

refresh_scr:
        cmp byte [cModel],MODEL_M80
        je near rs_m80u
        cmp byte [cModel],MODEL_U
        je near rs_m80u

        cmp byte [cModel],MODEL_S
        je rs10

        cmp byte [cModel],MODEL_O
        jne rs9
rs10:   ret

rs9:    cmp byte [f_scr],SCR_GRAPH
        jne rs8
        
        push ax
        mov ax, word [sh_scrbeg]
        mov word [dma_begadr],ax
        mov ax, word [sh_scrlen]
        dec ax
        mov word [dma_len],ax
        pop ax
        ret

rs8:    cmp byte [f_scr],SCR_HIGH
        jnz rs1
; 30x78
        mov word [scr_dim],1d4dh

        cmp byte [cModel],MODEL_P
        jne rsm_1

        mov word [scr_beg],76d0h
        mov word [scr_end],7ff3h
        mov word [cur_offs],0303h
        jmp rs22
rsm_1:
        cmp byte [cModel],MODEL_A
        jne rsm_2

        mov word [scr_beg],0e1d0h
        mov word [scr_end],0eaf3h
        mov word [cur_offs],0308h
        jmp rs22
rsm_2:
        mov word [scr_beg],76d0h
        mov word [scr_end],7ff3h
        mov word [cur_offs],0308h
        jmp rs22

rs1:    cmp byte [f_scr],SCR_LOW
        jnz rs2
; 25x64 !!!
        mov word [cur_offs],0
        mov word [scr_dim],184dh

        cmp byte [cModel],MODEL_P
        jne rsm_4

        mov word [scr_beg],77bdh
        mov word [scr_end],7f0dh+74
        jmp rs22
rsm_4:
        cmp byte [cModel],MODEL_A
        jne rsm_5

        mov word [scr_beg],0e2c2h
        mov word [scr_end],0ea12h+74
        jmp rs22
rsm_5:
        mov word [scr_beg],77c2h
        mov word [scr_end],7f12h+74
        jmp rs22
rs2:
; Auto
        push ax
        mov ah,byte [sh_height]
        mov al,byte [sh_width]
        sub ax,0101h
        mov word [scr_dim],ax
        mov ax,word [sh_scrbeg]
        mov word [scr_beg],ax
        add ax,word [sh_scrlen]
        dec ax
        mov word [scr_end],ax

        mov word [cur_offs],0303h
        cmp byte [cModel],MODEL_P
        je rsm_6
        mov word [cur_offs],0308h
rsm_6:
        pop ax

rs22:   push edi
        ;cmp byte ptr cs:[f_font],FONT_OEM
        ;jz rs7
        mov edi,symtablerk
       cmp byte [cModel],MODEL_M
       jne rs6
       cmp byte [mikr_symg],0
       jz rs6
       mov edi,symtablemikr2
        jmp rs6
;rs7:    lea di,symtable
rs6:    mov dword [symtable_ptr],edi
        call pos_cursor
        push esi
        push dx
        push ecx
        push ebx
        mov edi, dword [pcScreenBuf]
        movzx esi, word [scr_beg]
        add esi, dword [mem]
        movzx ecx,byte [scr_dim+1]
        inc cl
        push ax
        mov ah,07h
;        mov dl,ah
        cld
rs3:    push ecx
;        mov ah,07h
        push edi
        mov ebx,dword [symtable_ptr]
        mov ecx,78
rs4:    lodsb
       xor dl,dl
        test al,al
        jns rs5

        cmp al,0c0h
        jb rs53
        cmp al,0f0h
        jae rs52
        mov al,0b0h
        jmp rs51
rs52:   xor al,al
        jmp rs51

; Обработка цвета и атрибутов
rs53:   mov ah,7
        cmp byte [cModel],MODEL_R ; #### еще пров. подсв (Апогей).
        jne rs532
        push bx
        mov bl,al
        mov ah,1
        and ah,bl
        rol ah,1
        rol ah,1
        mov al,4
        and al,bl
        ror al,1
        or ah,al
        mov al,8
        and al,bl
        ror al,1
        ror al,1
        ror al,1
        or ah,al
        test ah,ah
        jnz rs533
;        or ah,08h
        mov ah,7
rs533:  mov al,bl
        pop bx
        ; DL=цв (для коррекции пред. симв.)
        mov dl,ah
rs532:
        test al,10h
        je rs531
        cmp byte [cModel],MODEL_M
        je rs531 ; На Микроше нет негатива
        rol ah,1
        rol ah,1
        rol ah,1
        rol ah,1
rs531:  test al,02h
        jz rs52
        or ah,80h
        jmp rs52
rs5:
        xlatb ;symtable_ptr
rs51:
        stosw
       test dl,dl
       jz rs511
     test ah,0fh
     jnz rs513
     test ah,70h
     jz rs513
     mov al, byte [edi-3]
     test al,0fh
     jz rs513
     test al,70h
     jnz rs513
     jmp rs511
;       push ax
;       mov al, byte ptr es:[di-3]
;       test al,07h
;       jnz rs512
;       ; Пред-инв.
;       push dx
;       rol dl,1
;       rol dl,1
;       rol dl,1
;       rol dl,1
;       and al,8fh
;       or al,dl
;       pop dx
;       jmp rs513
;       ; Пред-норм.
;rs512: and al,0f8h
;       or al,dl
rs513:
        push ax
        and byte [edi-3],80h
        and ah,7fh
        or byte [edi-3],ah
        pop ax
 ;       mov byte ptr es:[di-3],ah
;       pop ax
rs511:
        ;loop rs4
        dec ecx
        jnz rs4

        pop edi
        pop ecx
        add edi,160
        ;loop rs3
        dec ecx
        jnz rs3

        pop ax
        pop ebx
        pop ecx
        pop dx
        pop esi
        pop edi
        ret

rs_m80u:
        cmp byte [f_scr],SCR_GRAPH
        jne rsu8
        mov word [dma_begadr],0e800h
        mov word [dma_len],7ffh
        ret
rsu8:
        push edi
        push esi
        push dx
        push ecx
        push ebx

       ; Это цже 2-й раз. Можно вынести в отдельную процедуру. ###
       ;lea di,symtable
       ;cmp byte ptr cs:[f_font],FONT_OEM
       ;jz rsu9
       mov edi,symtablerk
rsu9:  mov dword [symtable_ptr],edi

       ; Прячем курсор.
       mov dx,0ffffh
       ;mov ah,2
       ;mov bh,0
       call set_cursor_pos; int 10h
       ; call _HideCursor
        mov edi,816 ;###
        movzx esi,word [scr_beg]
        add esi, dword [mem]
        mov ecx,32
        push ax
        add edi,dword [pcScreenBuf]
        mov ah,07h
        cld
rsu3:   push ecx
        push edi
        mov ebx,dword [symtable_ptr]
        mov ecx,64

rsu4:   cmp byte [cModel],MODEL_M80
        jne rsu6

        lodsb
        mov ah,7
        test al,al
        jz rsu5
        mov ah,70h
        jmp rsu5

rsu6:   lodsb
        mov ah,7
        test al,al
        jns rsu7
        mov ah,70h
        jmp rsu7

rsu5:   mov al, byte [esi+7ffh]
        and al,07fh
rsu7:   xlatb ;symtable_ptr
        stosw
        loop rsu4
        pop edi
        pop ecx
        add edi,160
        loop rsu3
        pop ax
        pop ebx
        pop ecx
        pop dx
        pop esi
        pop edi
        ret
;refresh_scr endp

;##################################################################
;### Установка экранных настроек и отображение экранной области ###
;##################################################################

;end
