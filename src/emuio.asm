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

; File: emuio.asm

; Содержит процедуры, эмулирующие внешние устройства ПК

[bits 32]

%include "emu80.inc"

cglobal scr_beg
cglobal scr_end
cglobal cur_xy
cglobal cur_x
cglobal cur_y
cglobal process_port_c
cglobal port_c
cglobal cur_corr
cglobal keybin_port
cglobal crt_param_1
cglobal crt_param_2
cglobal crt_param_3
cglobal crt_param_4
cglobal crt_mreg
cglobal dma_mr
cglobal dma_begadr
cglobal dma_len
cglobal int_flag
cglobal process_crt
cglobal color_table
cglobal or_color4_table
cglobal ticks_per_44100th
cglobal vmem_off
cglobal mem_page_no
cglobal or_color_mode
cglobal or_scrpage
cglobal process_orion
cglobal mikr_symg
cglobal use_io_space

cglobal snd_state

cglobal port_a_val
cglobal port_ac_s
cglobal cur_color_code

cglobal SaveByte
cglobal LoadByte
cglobal SavePort
cglobal LoadPort

[global save_byte]
[global load_byte]
[global save_port]
[global load_port]
[global write_byte_spec]
[global write_byte_spec_640]
[global write_byte_spec_800]
[global write_byte_orion]
[global write_byte_orion_640]
[global write_byte_orion_800]
[global cur_color_code]
[global calc_pit]
[global ticks_per_calc]
cglobal vretr_cnt
[global old_or_colormode]
[global old_or_scrpage]
[global perform_reset]

cextern sh_scrbeg     ; word
cextern sh_scrlen     ; word
cextern sh_width      ; byte
cextern sh_height     ; byte
cextern f_scr         ; byte
cextern f_tape        ; byte
cextern f_snd         ; byte
cextern cModel        ; byte
cextern mempage0      ; dword
cextern mempage1      ; dword
cextern mempage2      ; dword
cextern mempage3      ; dword
cextern color_mem     ; dword
cextern romdisk_mem   ; dword
cextern ReadSector    ; dword
cextern WriteSector   ; dword
;cextern ReadByteNoWait; dword
cextern PlayByteNoWait; dword
cextern acSectBufA    ; byte
cextern acSectBufB    ; byte
;cextern pcScreenBuf      ; byte

cextern InitPIT       ; dword
cextern WritePITReg   ; dword
cextern ReadPITReg    ; dword
cextern ProcessTime   ; dword
cextern GetSample     ; dword

cextern InitFDC       ; dword
cextern WriteFDCReg   ; dword
cextern ReadFDCReg    ; dword

cextern InitPPI       ; dword
cextern WritePPIReg   ; dword
cextern ReadPPIReg    ; dword
cextern WritePPI2Reg   ; dword
cextern ReadPPI2Reg    ; dword

cextern led_state     ; byte !!!

cextern key_bytes     ; byte
cextern ctrl_keys     ; byte
cextern key_bytes_s   ;
cextern ctrl_keys_s
[extern joy_state]     ; byte

[extern symtable_ptr]  ; dword
[extern cur_offs]      ;
;[extern f_vid]         ; byte
;[extern refresh_scr]   ; dword
;[extern draw_scr_or]   ; dword ?
[extern set_video_auto]; dword ?
[extern rst6]               ; dword
[extern set_delay_w_dma]    ; dword
[extern set_delay_wo_dma]   ; dword
[extern set_delay_w_dma28]  ; dword
;[extern snd61_and]     ; byte
[extern mem]           ; dword
[extern set_cursor_pos]; near

[extern store_regs]
[extern restore_regs]


;- DATA ----------------------------------------------------------------------
[segment .data]

use_io_space db 0 ; Используются ли команды IN/OUT

vmem_off dw 4402

;mag_state db 0 ; Порт 1 для Микро-80 или A1 для ЮТ-88

crt_param_1 db 0 ; ?
crt_param_2 db 0 ; ?
crt_param_3 db 0 ; ?
crt_param_4 db 0 ; ?
dma_mr db 0a4h
crt_mreg db 28h

dma_begadr_old dw 0
dma_len_old dw 0
crt_param_1_old db 0
crt_param_2_old db 0
crt_param_3_old db 0
crt_param_4_old db 0

pit_cnt_p dw 0 ; для "Партнера"

snd_state db 0

vretr_cnt db 0 ; признак бита обратного верт. хлда луча. Ставится приблизит.

color_table db 07h, 04h, 02h, 01h
cur_color_code db 0

or_color_mode db 0
or_color4_table times 4 db 0 ; ?
or_color4_table1 db 00h, 02h, 04h, 01h
or_color4_table2 db 05h, 06h, 07h, 03h

mem_page_no db 0 ; Номер текущей страницы памяти ПК "Орион"

or_scrpage db 0 ; Номер текущего экрана ПК "Орион"

romdisk_addr dw 0 ; Текущий адрес в ROMDISK'е

mikr_symg db 0 ; 0/80h в зависимости от тек. знакогенератора "Микроши"

int_flag db 0 ; 1, если разрешены прерывания
int_req db 0 ; 1, если есть отложеный запрос на прерывание

cursor_state db 0       ; 1, если координата X положения курсора принята,
                        ; а Y - еще нет
cursor_x db 0 ; ?       ; координата X положения курсора

scr_beg dw 0 ; ?        ; начало области видеопамяти 64x25
scr_end dw 0 ; ?        ; ее конец

port_c db 0
port_a_val db 0ffh ; значение, записанное в порт A ППА (B для Микроши)

crt_param_n db 0
crt_mode db 0 ; 0-курсор, 1-управл.
crt_lines db 0 ; ?
crt_cols db 0 ; ?
dma_begadr dw 0 ; ?
dma_len dw 0 ; ?

cur_xy:
cur_x:
db 0
cur_y:
db 0

old_or_scrpage db 0ffh
old_or_colormode db 0ffh

or_scr_redr_cnt db 3

; Переменные эиуляции дисковода
b_transfer db 0 ; 1, если идет чтение
b_read db 0 ; 0-чтение, 1-запись. Действителен при b_transfer=1
cyl db 0 ; регистр дорожки
sect db 0 ; регистр сектора
disk db 0 ; текущий диск
head db 0 ; текущая сторона
vg_data_reg db 0 ; регистр данных ВГ93
sect_ofs dw 0 ; Текущее смещение в буфере сектора
b_stdn db 0 ; 1, если шаг назад

[segment .bss]

cur_corr resw 1
keybin_port resw 1

ticks_per_44100th resw 1
ticks_per_calc resw 1 ; Одно из 2 вышеперечисленных в зависимости от режима

utrdadr resd 1        ; Адрес RAM-диска ЮТ-88
utrdpag resb 1        ; Номер страницы RAM-диска ЮТ-88

port_ac_s resw 1

write_byte_spec resd 1
write_byte_orion resd 1

cur_sect_buf resd 1 ; Текущий буфер сектора дисковода

temp_byte resb 1 ; Временная ячейка

;- TEXT ----------------------------------------------------------------------
[segment .text]

LoadByte:
    push ebp
    mov ebp,esp
    mov ebp, dword[ebp+8]
    call load_byte
    movzx eax,ah
    pop ebp
    ret

SaveByte:
    push ebp
    mov ebp,esp
    mov eax, dword[ebp+12]
    mov ah,al
    mov ebp, dword[ebp+8]
    call save_byte
    pop ebp
    ret

LoadPort:
    push ebp
    mov ebp,esp
    mov eax, dword[ebp+8]
    mov ah,al
    call load_port
    movzx eax,al
    pop ebp
    ret

SavePort:
    push ebp
    mov ebp,esp
    xor eax,eax
    mov al, byte[ebp+12]
    mov ah, byte[ebp+8]
    call save_port
    pop ebp
    ret

; Вызывается при попытке записи в память
; BP-адрес, AH-значение

save_byte:
        movzx ebp,bp

        cmp byte [cModel],MODEL_O
        je near sb_or

        cmp byte [cModel],MODEL_M80
        je sb_m80u
        cmp byte [cModel],MODEL_U
        je sb_u

        cmp bp,word [scr_beg] ; попали в экран или выше?
        jae near sb1
sb0:    ; пишем
        push ebp
        add ebp,dword[mem]
        mov byte [ebp],ah
        pop ebp
sb00:   ret

sb_u:   ; ЮТ-88
        cmp byte [utrdpag],0
        jz sb_m80u
        push ax
        push ebp
        mov ebp,dword[mem]
        mov al, byte [esi+ebp]
        pop ebp
        and al,0cfh
        cmp al,0c5h ; push ?
        pop ax
        jne sb_m80u

        ; Обращение к RAMDISK'у
        push ebx
        mov ebx, dword [utrdadr]
        mov byte [ebp+ebx],ah
        pop ebx
        ret

sb_m80u:; Микро-80 или неспецифическая обработка ЮТ-88
        cmp bp,0e000h
        jb sb0
        cmp bp, 0f000h
        jb sb_m80_scr
;   jmp sb0
        cmp bp, 0f800h
; jmp sb0
        jb sb0
        jmp sb00

sb_m80_scr:
; Попали в видеопамять Микро-80 или ЮТ-88
        cmp byte [cModel],MODEL_M80
        je sms1
        or bp,800h
        push ebx
        mov ebx, dword [mem]
        mov byte [ebp+ebx],ah
        and bp,0f7ffh
        mov byte [ebp+ebx],ah
        pop ebx
        ret
sms1:
        ; пишем
        push ebp
        add ebp,dword[mem]
        mov byte [ebp],ah
        pop ebp
        ret

sb_or:  ; Орион
        cmp bp,0f000h
        jae sb_or_1
        cmp bp,word [scr_beg]
        jb sb0
        cmp bp,word [scr_end]
        ja sb0
        cmp byte [mem_page_no],0
        je sb_or_2
        cmp byte [mem_page_no],1
        jne sb0
; Пишем цвет
        push ebx
        mov ebx, dword [mem]
        mov byte [ebp+ebx],ah         ; пишем
;        mov al,ah
;        mov ebx, dword [mempage0]
;        push ax
;        mov ah, byte [ebp+ebx]
;        call dword [write_byte_orion]
;        pop ax
        pop ebx
        ret
; Пишем байт
sb_or_2:push ebx
        mov ebx, dword [mem]
        mov byte [ebp+ebx],ah         ; пишем
;        push ax
;        mov ebx, dword [mempage1]
;        mov al, byte [ebp+ebx]
;        call dword [write_byte_orion]
;        pop ax
        pop ebx
        ret
sb_or_1:
        cmp bp,0f400h
        jb sbm5_1
        cmp bp,0f500h
        jb near ppi1_out
        cmp bp,0f600h
        jb near ppird_out
        cmp bp,0f700h
        jb sb00 ; ВВ55 в будущем ?
        cmp bp,0f800h
        jb near disk_out ; дисковод
        cmp bp,0f900h
        jb near or_color_mode_out
        cmp bp,0fa00h
        jb near or_mp_port_out
        cmp bp,0fb00h
        jb near or_scrpage_out
        jmp sb00
sbm5_1: ; Попали в системное ОЗУ "Ориона"
        push ebp
        add ebp,dword[mempage0]
        mov byte [ebp],ah
        pop ebp
        ret

        ; продолжение анализа (не Микро-80, не ЮТ-88 и не Орион)
sb1:    cmp bp,word [scr_end]
        ja near sb2
; Попали в видеопамять
        push ebx
        mov ebx,dword[mem]
        mov al, byte [ebp+ebx]
        mov byte [ebp+ebx],ah
        pop ebx
        cmp byte [cModel],MODEL_S
        jne sb6

        ; Пишем инф. о цвете в Специалисте
        push ebx
        push ax
        mov ebx,[color_mem]
        mov al, byte [cur_color_code]
        mov byte [ebx+ebp-9000h],al
        pop ax
        pop ebx
sb6:    ret

sb2:    ; попали выше экранной области
        cmp byte [cModel],MODEL_P
        jne sbm_1

        cmp bp,0d800h
        jb sb0
        cmp bp,0d900h
        jae sb3_p
        jmp crt_out
sb3_p:  cmp bp,0da00h
        jae sb4_p
        jmp ppi1_out
sb4_p:  cmp bp,0db00h
        jae sb5_p
        ret
sb5_p:  cmp bp,0dc00h
        jae sb6_p
        jmp dma_out
sb6_p:  ret

sbm_1:
        cmp byte [cModel],MODEL_R
        jne sbm_2

        cmp bp,8000h
        jb sb0
        cmp bp,0a000h
        jae sb3_r
        jmp ppi1_out
sb3_r:  cmp bp,0c000h
        jae sb4_r
        jmp pit_out
sb4_r:  cmp bp,0e000h
        jae sb5_r
        jmp crt_out
sb5_r:  jmp dma_out

sbm_2:
        cmp byte [cModel],MODEL_M
        jne sbm_3

        cmp bp,8000h
        jb sb0
        cmp bp,0c000h
        jae sb3_m
        ret
sb3_m:  cmp bp,0c800h
        jae sb4_m
        jmp ppi1_out
sb4_m:  cmp bp,0d000h
        jae sb5_m
        jmp ppi2_out
sb5_m:  cmp bp,0d800h
        jae sb6_m
        jmp crt_out
sb6_m:  cmp bp,0e000h
        jae sb7_m
        jmp pit_out
sb7_m:  cmp bp,0f800h
        jae sb8_m
        ret
sb8_m:  jmp dma_out

sbm_3:
        cmp byte [cModel],MODEL_A
        jne sbm_4

        cmp bp,0ec00h
        jb sb0
        cmp bp,0ed00h
        jae sb3_a
        jmp pit_out
sb3_a:  cmp bp,0ee00h
        jae sb4_a
        jmp ppi1_out
sb4_a:  cmp bp,0ef00h
        jae sb5_a
        jmp ppi2_out
sb5_a:  cmp bp,0f000h
        jae sb6_a
        jmp crt_out
sb6_a:  cmp bp,0f100h
        jae sb7_a
        jmp dma_out
sb7_a:  ret

sbm_4:
;        cmp byte ptr cs:[cModel],MODEL_S
;        jne sb00 ; Микро-80 или ЮТ-88 ; ###

        cmp bp,0c000h
        jb sb0
        cmp bp,0f800h
        jae near ppi1_out
        jmp sb00

;save_byte endp

save_port:
        cmp ah,7
        je sp_kbd_out
        cmp byte [cModel],MODEL_M80
        je sp_m80
        cmp ah,0a1h
        je sp_mag_out
        and ah,0f0h
        cmp ah,40h
        je rdu_sel
        ;###
        ret
sp_m80: cmp ah,1
        je sp_mag_out
        ret
;save_port endp

rdu_sel:
       push ebx
       xor ebx,ebx
       and al,0fh
       mov byte [utrdpag],0
       je rdusel
       cmp al,0fh
       je rdusel
       test al,1
       mov ebx,4
       mov byte [utrdpag],1
       jz rdusel
       test al,2
       mov ebx,8
       mov byte [utrdpag],2
       jz rdusel
       test al,4
       mov ebx,12
       mov byte [utrdpag],3
       jz rdusel
       mov ebx,16
       mov byte [utrdpag],4
rdusel:
       mov ebx, dword [mempage0+ebx]
       mov dword [utrdadr],ebx
       pop ebx
       ret

sp_kbd_out:
        mov byte [port_a_val],al
        ret

sp_mag_out:
        mov byte [port_c],al
;        cmp byte [cModel],MODEL_M80
;        je smo1
;        cmp byte [f_snd],SND_ON
;        jne smo1
;        push ax
;        mov ah,al
;        and ah,1
;        rol ah,1
;        in al,61h
;        and al,0fdh
;        or al,ah
;        out 61h,al
;        pop ax
smo1:   ret

; Порт переключения страниц ПК "Орион"
or_mp_port_out:
       and ah,3
       mov byte [mem_page_no],ah
       add ah,ah
       add ah,ah
       push ebx
       movzx ebx,ah
       mov ebx, dword [mempage0+ebx]
       mov dword [mem],ebx
       pop ebx
       ret
;or_mp_port_out endp

; Порт управления режимом цвета в Орионе
or_color_mode_out:
       mov byte [or_color_mode],ah
       cmp ah,4
       je ocmo1
       cmp ah,5
       je ocmo2
;       call draw_scr_or ;!!!
       ret
ocmo1: push eax
       mov eax, dword [or_color4_table1]
       mov dword [or_color4_table],eax
       pop eax
       ret
ocmo2: push eax
       mov eax, dword [or_color4_table2]
       mov dword [or_color4_table],eax
       pop eax
       ret
;or_color_mode_out endp

; Порт переключения экранных областей в Орионе
or_scrpage_out:
       and ah,3
       mov byte [or_scrpage],ah
       push bx
       mov bl,ah
       xor bh,bh
       xor bx,3
       ror bx,1
       ror bx,1
       mov word [scr_beg],bx
       add bx,2fffh
       mov word [scr_end],bx
       pop bx
;      call draw_scr_or ;!!!
       ret
;or_scrpage_out endp

; Эмуляция ROMDISK'а ПК "Орион"
ppird_out:
        and bp,3
        cmp bp,2
        jne prdi1
        mov byte [romdisk_addr+1],ah
        ret
prdi1:  cmp bp,1
        jne prdi2
        mov byte [romdisk_addr],ah
prdi2:  ret
;ppird_out endp

ppird_in:
        and bp,3
        cmp bp,0
        jne prdo1
        push ebp
        mov ebp, dword [romdisk_mem]
        push eax
        movzx eax, word [romdisk_addr]
        add ebp, eax
        pop eax
        mov ah, byte [ebp]
        pop ebp
        ret
prdo1:  cmp bp,1
        jne prdo2
        mov ah, byte [romdisk_addr]
        ret
prdo2:  cmp bp,2
        jne prdo3
        mov ah, byte [romdisk_addr+1]
prdo3:  ret
;ppird_in endp

; Эмуляция ВГ75
crt_out:
        test bp,1h
        jz cp1
     cmp byte [crt_mreg],ah  ; Для ускорения/замедления ####
     je co2
       cmp byte [crt_mreg],0e0h
        mov byte [crt_mreg],ah
       jne co1
       ; Устанавливаем видеорежим (было E0 - приостанов)
       cmp byte [dma_mr],0a4h
       jne co1
       call do7 ; чтобы не ломать голову ;) Это работает... Т.ж. пров-ся E0
co1:
       call set_dma_delay
co2:
        cmp ah,0
        jz cp3
        mov byte [crt_mode],0     ; режим управления курсором
        mov byte [cursor_state],0 ; запись в управляющий регистр
        ret
cp1:    cmp byte [crt_mode],0
        jnz cp4                          ; управляющая команда
        cmp byte [cursor_state],0 ; запись в регистр данных
        jne cp2
        mov byte [cur_x],ah       ; запоминаем X и факт его записи
        inc byte [cursor_state]
        ret
cp2:    dec byte [cursor_state]   ; устанавливаем курсор
        mov byte [cur_y],ah
        cmp byte [f_scr],SCR_GRAPH
        je cp8
        push ax
        push bx
        push dx
        mov dl,byte [cur_x]
        mov dh,ah
        ;mov ah,2
        ;xor bh,bh
        sub dx,word [cur_corr]    ; коррекция положения курсора
        add dx,word [cur_offs]
        call set_cursor_pos ; int 10h
        pop dx
        pop bx
        pop ax
cp8:    ret
cp3:    mov byte [crt_mode],1     ; управляющий режим
        mov byte [crt_param_n],0
        ret
cp4:    inc byte [crt_param_n]
        cmp byte [crt_param_n],1
        jnz cp5
        mov byte [crt_param_1],ah
        and ah,7fh
        mov byte [crt_cols],ah
        ret
cp5:    cmp byte [crt_param_n],2
        jnz cp6
        mov byte [crt_param_2],ah
        and ah,3fh
        mov byte [crt_lines],ah
        ret
cp6:    cmp byte [crt_param_n],3
        jnz cp7
        mov byte [crt_param_3],ah
        ret
cp7:    mov byte [crt_param_4],ah
        mov byte [crt_mode],0
        ret
;crt_out endp

; Производит настройку экрана в соответствии с запомненными параметрами
process_crt:
        cmp byte [cModel],MODEL_S
        je pc4
        cmp byte [cModel],MODEL_O
        je pc4
        push ebp ; ???
        mov ah,byte [crt_param_1]
        and ah,7fh
        mov byte [crt_cols],ah
        mov ah,byte [crt_param_2]
        and ah,3fh
        mov byte [crt_lines],ah
        call set_dma_delay
pc2:    call do7
        pop ebp
pc4:    ret
;process_crt endp

set_dma_delay:
        cmp byte [dma_mr],80h
        je sdd1
        cmp byte [dma_mr],28h
        jne sdd3
        call set_delay_w_dma28
        jmp sdd2
sdd3:   cmp byte [crt_mreg],0e0h
        je sdd1
        call set_delay_w_dma
        jmp sdd2
sdd1:   call set_delay_wo_dma
sdd2:   ret
;set_dma_delay endp

; Эмуляция PPI
ppi1_out:

        cmp byte [cModel],MODEL_P
        jne ppim_1

        cmp bp,0d903h
        cmp bp,0d902h
        je near ppi1_c
        cmp bp,0d903h
        je near ppi1_r
        cmp bp,0d900h
        je near ppi1_a
        ret

ppim_1:
        cmp byte [cModel],MODEL_M
        jne ppim_2

        and bp,0c003h
        cmp bp,0c002h
        je near ppi1_c
;       cmp bp,0c003h
;       je ppi1_r
        cmp bp,0c001h
        je near ppi1_a
        ret

ppim_2:
        cmp byte [cModel],MODEL_R
        jne ppim_3

        and bp,3
        cmp bp,0002h
        je near ppi1_c
        cmp bp,0000h
        je near ppi1_a
        ret

ppim_3:
        cmp byte [cModel],MODEL_A
        jne ppim_7

        and bp,0ed03h
        cmp bp,0ed02h
        je ppi1_c
        cmp bp,0ed00h
        je near ppi1_a
        ret

ppim_7:
        cmp byte [cModel],MODEL_S
        jne ppim_8

        and bp,0f803h
        cmp bp,0f802h
        je ppi1_c
        cmp bp,0f803h
        je near ppi1_r
        cmp bp,0f801h
        je ppi1_b_out_s
        mov byte [port_ac_s],ah
        ret

ppim_8:
        and bp,3
        cmp bp,02h
        je ppi1_c
        cmp bp,00h
        je near ppi1_a
        ret

ppi1_b_out_s:
        push ax
        and ah,0fch
        and byte [ctrl_keys_s],3
        or byte [ctrl_keys_s],ah
        pop ax
        ret

; Обрабатывает состояние порта C ППА
process_port_c:
        push ax
        mov ah, byte [port_c]
        jmp kpc4
;process_port_c endp

; запись в порт C ППА ВВ55
; бит 1 - управление динамиком (0-писк)
ppi1_c:
        mov byte [port_c],ah
        push ax
kpc4:

        cmp byte [cModel],MODEL_S
        jne ppim_5

        ; Специалист
        push ax
        and ah,0fh
        mov byte [port_ac_s+1],ah
        pop ax

        mov byte [snd_state],ah

        ; Цвет
        push ebx
        push eax
        mov ebx, color_table
        movzx eax,byte [port_c]
        rol al,1
        rol al,1
        and al,3
        mov al,byte [ebx+eax]
        mov byte [cur_color_code],al
        pop eax
        pop ebx

; 9 цветов
;   push ax
;   mov al,byte ptr cs:[port_c]
;   rol al,1
;   rol al,1
;   rol al,1
;   and al,6
;   mov ah,al
;   mov al,byte ptr cs:[port_c]
;   rol al,1
;   rol al,1
;   rol al,1
;   rol al,1
;   and al,1
;   or al,ah
;   mov byte ptr cs:[cur_color_code],al
;   pop ax

ppim_5:
        and ah,08h
        ror ah,1
        and byte [led_state],0fbh
        or byte [led_state],ah
;       call light_led

kpc3:   pop ax
kpc5:   ret

; запись в регистр управляющего слова (РУС) ППА
; отслеживаем только звук (см. выше)

        ;cmp byte ptr cs:[cModel],MODEL_P
        ;jne kpc4

ppi1_r:
        cmp ah,80h
        jb kpc6
        ; ###########?
        ; Сброс
        mov byte [port_c],0
        mov byte [port_a_val],0
        mov word [port_ac_s],0
        and byte [ctrl_keys_s],3

        ; ###########?
        ;mov byte [cur_color_code],00h
        ;jmp kpc5
kpc6:
        push ax
        push cx
        mov cl,ah
        ror cl,1
        and cl,07h;03h
        mov al,0feh
        rol al,cl
        and byte [port_c],al
        and ah,1
        rol ah,cl
        or byte [port_c],ah
        mov ah, byte [port_c]
        pop cx
        jmp kpc4

ppi1_a:
        mov byte [port_a_val],ah
        ret

; Эмуляция PIT
pit_out:
        pusha
      __align_sp
        mov al,ah
        and eax,0FFh
        push eax
        push ebp
        call WritePITReg
        add esp,8
      __restore_sp
        popa
        ret
;pit_out endp

ppi2_out:
        cmp byte [cModel],MODEL_M
        jne ppi2_1
        and bp,3
        cmp bp,1
        jne ppi2_1
        push ax
        mov al,byte [mikr_symg]
        xor al,ah
        mov byte [mikr_symg],ah
        and byte [mikr_symg],80h
        pop ax
ppi2_1: ret

dma_out:
        and bp,000fh
        cmp bp,4
        jne do1
        push cx
        mov cl,8
        ror word [dma_begadr],cl
        pop cx
        mov byte [dma_begadr+1],ah
        ret
do1:    cmp bp,5
        jne do2
        push cx
        mov cl,8
        ror word [dma_len],cl
        pop cx
        mov byte [dma_len+1],ah
        ret
do2:    cmp bp,8
        jne near do3
        cmp ah, byte [dma_mr]
        je near do3
        mov byte [dma_mr],ah

        call set_dma_delay
;        cmp ah,80h
;        je do5
;        cmp ah,28h
;        jne do8
;        call set_delay_w_dma28
;        jmp do6
;do8:    cmp byte ptr cs:[crt_mreg],0e0h
;        je do5
;        call set_delay_w_dma
;        jmp do6
;do5:    call set_delay_wo_dma

do6:    test ah,4
        jz near do3
        and word [dma_len],3fffh
        push ax
        mov ax,word [dma_begadr]
        mov word [sh_scrbeg],ax
        mov ax,word [dma_len]
        inc ax
        mov word [sh_scrlen],ax
        mov al,byte [crt_lines]
        inc al
        mov byte [sh_height],al
        mov al,byte [crt_cols]
        inc al
        mov byte [sh_width],al
; Делим длину экрана на количество столбцов
        push cx
        mov cl,al
        mov ax,word [sh_scrlen]
        div cl
        pop cx
; Сравниваем результат с числом строк и берем меньшее
        cmp al,byte [sh_height]
        jae do4
        mov byte [sh_height],al
do4:    pop ax
; refresh screen
do7:    cmp byte [f_scr],MEM_RW
        jne near do3
        cmp byte [crt_mreg],0e0h
        je near do3
        push ax
        push bx
        push cx
        push dx
        push si
        push di

     xor bx,bx
     xor dx,dx

     mov ax, word [dma_begadr]
     mov cx,ax
     xor ax, word [dma_begadr_old]
     or bx,ax
     mov word [dma_begadr_old],cx

     mov ax, word [dma_len]
     mov cx,ax
     xor ax, word [dma_len_old]
     or dx,ax
     mov word [dma_len_old],cx

     mov ax, word [crt_param_1]
     mov cx,ax
     xor ax, word [crt_param_1_old]
     or dx,ax
     mov word [crt_param_1_old],cx

     mov ax, word [crt_param_3]
     mov cx,ax
     xor ax, word [crt_param_3_old]
     or dx,ax
     mov word [crt_param_3_old],cx

     test dx,dx
     jz do9

     call set_video_auto
     jmp do10

do9:
     test bx,bx
     jz do8

do10:
;        call refresh_scr ;???
do8:
        pop di
        pop si
        pop dx
        pop cx
        pop bx
        pop ax
do3:    ret
;dma_out endp


; Вызывается при попытке чтения из памяти
; BP-адрес Возврат : AH-значение

load_byte:
        movzx ebp,bp

        cmp byte [cModel],MODEL_M80
        je near lb_m80u
        cmp byte [cModel],MODEL_U
        je near lb_u

        cmp byte [cModel],MODEL_P
        jne lbm_1

        cmp bp,0d800h
        jb near lb0
        cmp bp,0d900h
        jae lb3_p
        jmp crt_in
lb3_p:  cmp bp,0da00h
        jae lb4_p
        jmp ppi1_in
lb4_p:  cmp bp,0db00h
        jae lb5_p
        ret
lb5_p:  cmp bp,0dc00h
        jae lb6_p
        jmp dma_in
lb6_p:  cmp bp,0f000h
        jae near lb0
        mov ah,0ffh
        ret

lbm_1:
        cmp byte [cModel],MODEL_R
        jne lbm_2

        cmp bp,8000h
        jb near lb0
        cmp bp,0a000h
        jae lb3_r
        jmp ppi1_in
lb3_r:  cmp bp,0c000h
        jae lb4_r
        cmp bp,0a000h
        je near ppi2_in
        cmp bp,0a001h
        je near no_in
        cmp bp,0a002h
        je near no_in
        cmp bp,0a003h
        je near no_in
        jmp pit_in
lb4_r:  cmp bp,0e000h
        jae near lb0
        jmp crt_in

lbm_2:
        cmp byte [cModel],MODEL_M
        jne lbm_3

        cmp bp,8000h
        jb near lb0
        cmp bp,0c000h
        jae lb3_m
        ret
lb3_m:  cmp bp,0c800h
        jae lb4_m
        jmp ppi1_in
lb4_m:  cmp bp,0d000h
        jae lb5_m
        jmp ppi2_in
lb5_m:  cmp bp,0d800h
        jae lb6_m
        jmp crt_in
lb6_m:  cmp bp,0e000h
        jae lb7_m
        jmp pit_in
lb7_m:  cmp bp,0f800h
        jae near lb0
lb8_m:  jmp dma_in

lbm_3:
        cmp byte [cModel],MODEL_A
        jne lbm_4

        cmp bp,0ec00h
        jb near lb0
        cmp bp,0ed00h
        jae lb3_a
        jmp pit_in
lb3_a:  cmp bp,0ee00h
        jae lb4_a
        jmp ppi1_in
lb4_a:  cmp bp,0ef00h
        jae lb5_a
        jmp ppi2_in
lb5_a:  cmp bp,0f000h
        jae lb0
        jmp crt_in

lbm_4:
        cmp byte [cModel],MODEL_S
        jne lbm_5

        cmp bp,0f800h
        jae near ppi1_in
        jmp lb0

lbm_5:  ; Орион
        cmp bp,0f000h
        jae lb_or_1
        jmp lb0
lb_or_1:cmp bp,0f400h
        jb lbm5_1
        cmp bp,0f500h
        jb near ppi1_in
        cmp bp,0f600h
        jb ppird_in
        cmp bp,0f700h
        jb lbm5_1 ; ####
        cmp bp,0f800h
        jb near disk_in
lbm5_1: ; Попали в системное ОЗУ "Ориона"
        push ebx
        mov ebx, dword [mempage0]
        jmp lb000

lb0:    push ebx
        mov ebx, dword [mem]
lb000:  mov byte ah,[ebp+ebx];                ; читаем
        pop ebx
lb00:   ret

lb_u:
        cmp byte [utrdpag],0
        jz lb_m80u

        push ebx
        mov ebx, dword [mem]
        push ax
        mov al, byte [ebx+esi]
        and al,0cfh
        cmp al,0c1h ; pop ?
        pop ax
        pop ebx
        jne lb_m80u

        ; Обращение к RAMDISK'у
        push ebx
        mov ebx, dword [utrdadr]
        mov ah,byte [ebp+ebx]
        pop ebx
        ret

lb_m80u:
        cmp bp,0e000h
        jb lb0
        cmp bp,0f000h
        jae lb0
        cmp byte [cModel],MODEL_U
        je lb0
        mov ah,0ffh
        jmp lb00
;load_byte endp

load_port:
        cmp ah,6
        je lp_kbd_in
        cmp ah,7
        je lp_kbdout_in
        cmp ah,5
        je lp_kbdc_in
        cmp byte [cModel],MODEL_M80
        je lp_m80
        cmp ah,0a1h
        je lp_mag_in
        ret
lp_m80:
        cmp ah,1h
        je lp_mag_in
        ret
;load_port endp

lp_kbdc_in:
        mov al,byte [ctrl_keys]
        and al,07h
        ret

lp_kbd_in:
        push ecx
        push ax
        push ebx
        mov ebx, key_bytes
        mov ecx,8
        mov ah,07fh
        mov al,byte [port_a_val]
lki1:   rcr al,1
        jc lki2
        and ah, byte [ebx]
lki2:   inc ebx
        loop lki1
        pop ebx
        mov cl,ah
        pop ax
        mov al,cl
        pop ecx
        ret

lp_kbdout_in:
        mov al,byte [port_a_val]
        ret

lp_mag_in:
        ;###

crt_in:
        xor ah,ah
        cmp byte [vretr_cnt],0
        jz ci_1
        mov ah,20h
ci_1:   ret
dma_in:
ppi2_in2:
no_in:
        mov ah,0ffh
        ret

pit_in:
        pusha
      __align_sp
        push ebp
        call ReadPITReg
        add esp,4
        mov byte [temp_byte], al
      __restore_sp
        popa
        mov ah,byte [temp_byte]
        ret

ppi2_in:
        cmp byte [cModel],MODEL_M
        jne ppi2_in3
        and bp,3
        cmp bp,1
        jne no_in
        mov ah,byte [mikr_symg]
        ret
ppi2_in3:
        cmp byte [cModel],MODEL_R
        jne ppi2_in2
        mov ah, byte [joy_state]
        ret

ppi1_in:
        push ecx
        push ax
        mov ax,bp
        and al,03h

        cmp byte [cModel],MODEL_S
        je ppi1_s

        cmp al,byte [keybin_port]

        jne ppi1_1

        push ebx
        mov ebx, key_bytes
        mov ecx,8
        mov ah,0ffh
        mov al,byte [port_a_val]
ppi1_2: rcr al,1
        jc ppi1_3
        and ah, byte [ebx]
ppi1_3: inc ebx
        loop ppi1_2
        pop ebx
ppi1_5: mov cl,ah
        pop ax
        mov ah,cl
        pop ecx
        ret
ppi1_1: cmp al,2
        je ppi1_4
        mov ah,byte [port_a_val]
        jmp ppi1_5
ppi1_4: mov ah,byte [ctrl_keys]
        and ah,0f0h
        mov al,byte [port_c]
        and al,0fh
        or ah,al
        jmp ppi1_5

ppi1_s:
        cmp al,1
        jne ppi1_1_s

        push ebx
        push bp
        mov ebx, key_bytes_s
        xor al,al
        mov ecx,6
ppi1_s3:ror al,1
        mov bp, word [port_ac_s]
        or bp, word [ebx]
        cmp bp,0fffh
        jne ppi1_s4
        or al,80h
ppi1_s4:inc ebx
        inc ebx
        loop ppi1_s3
        pop bp
        pop ebx
        mov cl, byte [ctrl_keys_s]
        and cl,3
        or cl,al
        pop ax
        mov ah,cl
        xor ah,1
        pop ecx
;        pop ax
;        mov ah, byte ptr cs:[ctrl_keys_s]
;        pop cx
        ret
ppi1_1_s: call ppi1_s_or
        cmp al,0
        jne ppi1_s2
        pop ax
        mov ah,cl
        pop ecx
        ret
ppi1_s2:mov al,byte [port_c]
        and al,0f0h
        or ch,al
        pop ax
        mov ah,ch
        pop ecx
        ret

ppi1_s_or:
        push ax
        push ebx
        push dx
        mov ebx, key_bytes_s
        mov ecx,6
        mov dx,0fffh
        mov al,byte [ctrl_keys_s]
        rcr al,1
        rcr al,1
ppi1s_2:rcr al,1
        jc ppi1s_3
        and dx, word [ebx]
ppi1s_3:inc ebx
        inc ebx
        loop ppi1s_2
        mov cx,dx
        pop dx
        pop ebx
        pop ax
        ret
;ppi1_in endp

; Вызывается каждые 1/44100 с или 50 мс в зависимости от режима.
; Рассчитывает следующие значения счетчиков и программирует таймер PC
calc_pit:
        cmp byte [vretr_cnt],0
        jz cpt0
        dec byte [vretr_cnt]
cpt0:
        pusha
      __align_sp
        movzx eax,word [ticks_per_calc]
        push eax
        call ProcessTime
        add esp,4
        call GetSample
        push eax
        call PlayByteNoWait
        add esp,4
      __restore_sp
        popa
        ret

; Вызывается при Reset'е эмулируемого компьютера
perform_reset:
; Reset таймера

        pusha
      __align_sp
        call InitPIT
        call InitFDC
        call InitPPI
      __restore_sp
        popa

       mov word [dma_begadr_old],0
       mov word [dma_len_old],0
       mov byte [crt_param_1_old],0
       mov byte [crt_param_2_old],0
       mov byte [crt_param_3_old],0
       mov byte [crt_param_4_old],0
       cmp byte [cModel],MODEL_U
       je pr2
       push ebx
       mov ebx, dword [mempage0]
       mov dword [utrdadr],ebx
       mov byte [utrdpag],0
       pop ebx
pr2:
        ret
;perform_reset endp

; Устанавливает параметры Ориона, считанные из снапшота
_process_orion:
       cmp byte [cModel],MODEL_O
       je po1
       mov ah, byte [or_scrpage]
       push ebx
       mov bl,ah
       xor bh,bh
       xor bx,3
       ror bx,1
       ror bx,1
       mov word [scr_beg],bx
       add bx,2fffh
       mov word [scr_end],bx
       mov ah, byte [mem_page_no]
       and ah,3
       mov byte [mem_page_no],ah
       add ah,ah
       add ah,ah
       mov bl,ah
       xor bh,bh
       mov ebx, dword [mempage0+ebx]
       mov dword [mem],ebx
       pop ebx
po1:   ret
;process_orion endp


; ######################### ЭМУЛЯЦИЯ ДИСКОВОДА #########################

disk_in:
        pusha
      __align_sp
        push ebp
        call ReadFDCReg
        add esp,4
        mov byte [temp_byte], al
      __restore_sp
        popa
        mov ah,byte [temp_byte]
        ret

disk_out:
        pusha
      __align_sp
        mov al,ah
        and eax,0FFh
        push eax
        push ebp
        call WriteFDCReg
        add esp,8
      __restore_sp
        popa
        ret

;end
