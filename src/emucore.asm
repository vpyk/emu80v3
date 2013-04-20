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

; File: emucore.asm

; �������� ���� �������������� ������ 8080

[bits 32]

%include "emu80.inc"

cglobal cur_speed_sb
cglobal delay_sb
cglobal rom_len
cglobal rom_adr
cglobal ppi_portc_adr
cglobal crt_port1_adr
cglobal romname
cglobal hook_proc_sb
cglobal hook_p_sb
cglobal hook_r_sb
cglobal hook_a_sb
cglobal hook_m_sb
cglobal hook_s_sb
cglobal hook_o_sb
cglobal hook_m80_sb
cglobal hook_u_sb
cglobal Reset
cglobal Emulate
cglobal ProcessQuery
cglobal PrepareScreen
cglobal Init
cglobal SetDefParams
cglobal mempages
cglobal mempage0
cglobal mempage1
cglobal mempage2
cglobal mempage3
cglobal mempage4
cglobal color_mem
cglobal sym_gen
cglobal romdisk_mem
cglobal reg_af
cglobal reg_bc
cglobal reg_de
cglobal reg_hl
cglobal reg_sp
cglobal reg_pc
cglobal reset_addr
cglobal f_saverom
cglobal delay_sb_dma
cglobal delay_sb_norm
cglobal AfterLoadRom

[global fin]
[global setparms]
[global quit]
[global set_video_auto]
[global set_delay_w_dma]
[global set_delay_wo_dma]
[global set_delay_w_dma28]
;[global LoadROM]
;[global _ClearMem]
[global mem]
[global mem_fetch]
[global fonts_mem]

[global anal_params]
[global f_chscr]
[global f_use8x12]
[global f_pause]

cextern f_speed               ;byte
cextern f_scr                 ;byte
cextern f_snd                 ;byte
cextern f_tape                ;byte

cextern f_quit                ;byte

cextern sh_height             ;byte
cextern EnterInteractiveMode  ;near
cextern LeaveInteractiveMode  ;near
cextern SetVideoMode          ;near
cextern cModel                ;byte
cextern process_port_c        ;near
cextern RefreshMenu           ;near
cextern ClearScreen           ;near
;cextern CheckKeys           ;near
;cextern DrawScreen          ;near
cextern Delay ;!!!          ;near
cextern mem_page_no           ;byte
cextern ticks_per_44100th     ;word
cextern bWaitSnd              ;!!!word
;cextern WaitSound             ;near

;cextern DebugProc

;[extern last]                  ;byte
;[extern emF809]                ;near
;[extern emF803]                ;near
;[extern emF81B]                ;near
;[extern emF812]                ;near
[extern emF80C]                ;near
[extern emF806]                ;near
[extern code_tbl]              ;!!!word
[extern code_tbl_m]            ;!!!word
[extern code_tbl_m2]           ;!!!word
;[extern inittimer]             ;near
;[extern getcount]              ;near
;[extern f_led]                 ;byte
[extern setup]                 ;near
[extern f_reset]               ;byte
;[extern get_key]               ;near
;[extern test_key]              ;near
;[extern prep_key]              ;near
[extern sysreq]                ;byte
;[extern flash_key]             ;near
;[extern setscr25]              ;near
[extern setscr30]              ;near
[extern setscr40]              ;near
[extern setscr60]              ;near
;[extern setscr43]              ;near
[extern refresh_scr]           ;near
;[extern load_8x16]             ;near
;[extern load_8x16rk]           ;near
;[extern load_8x8]              ;near
;[extern load_8x8rk]            ;near
;[extern load_8x12]             ;near
;[extern load_8x12rk]           ;near
;[extern setw8]                 ;near
;[extern set_vga400]            ;near
;[extern set_vga350]            ;near
;[extern symtable_ptr]          ;word
;[extern test_vga]              ;near
;[extern test_win]              ;near
;[extern test_nt]               ;near
;[extern disable_vga]           ;near
;[extern set_fontoem]           ;near
;[extern enable_hookf809]       ;near
;[extern init_sound]            ;near
[extern close_file]            ;near
[extern close_file_delay]      ;near
;[extern set_cga_mode]          ;near
;[extern test_ega]              ;near
;[extern find_path]             ;near
;[extern com_path]              ;byte
;[extern com_path_len]          ;word
;[extern light_led]             ;near
[extern process_int]           ;near
;[extern disable_ints]          ;near
[extern sysreq_code]           ;byte
;[extern GeneralError]          ;near
;[extern ROMError]              ;near
;[extern SymGenError]           ;near
;[extern FontsError]            ;near
;[extern ROMDiskError]          ;near
;[extern cMonitor]              ;byte
[extern emu_ret]               ;near
;[extern f_novesa]              ;byte
;[extern wSBPort]               ;word
;[extern wSBDMA]                ;word
[extern calc_pit]              ;near
[extern vretr_cnt]             ;byte
[extern translate_sysreq_code] ;near
[extern ticks_per_calc]        ;!!!word
[extern perform_reset]         ;near

;[extern StopSound]             ;near
;[extern ResetDSP]              ;near
;[extern bSBPresent]            ;word ; SB Present

;- DATA ----------------------------------------------------------------------
[segment .data]

; ��� ����� ���������������
symgen_name db 'symgen.bin',0
fontfile_name db 'fonts.bin',0

; ������� ������������� ���������� ��� � ��������
f_saverom db 0 ;!!!

; ������� ��������� �� ������� 8080
mem dd 0 ; ������� ��������
mempages:
mempage0 dd 0 ; �������� 0 ��� ������������
mempage1 dd 0 ; �������� 1
mempage2 dd 0 ; �������� 2
mempage3 dd 0 ; �������� 3
mempage4 dd 0 ; �������� 4 (�� ��, ��� � RomDisk � ������)

; ��������� �� ��������������
sym_gen dd 0

; ��������� �� ������� ����� ��� �� "����������"
color_mem dd 0

; ��������� �� ������� ROMDISK'� ��� �� "�����"
romdisk_mem dd 0

delay_sb_norm dw 0 ; ? ���������� ������ ���������� 8080
delay_sb_dma dw 0 ; ? ��� ���������� ���
delay_sb_28 dw 36 ; ��� ���������� ��� � ������ ����������� ��� ��� ��������

reset_addr dw 0f800h

f_pause db 0 ; ������� �����

f_use8x12 db 1

f_chscr db 1    ; 1, ���� ����� ��������������� �����

cnt_20ms dw 882

[segment .bss]

; ��������
reg_af resw 1
reg_bc resw 1
reg_de resw 1
reg_hl resw 1
reg_sp resw 1
reg_pc resw 1

; ������� ��������. ������������ ��� ���������� ��������
cur_speed_sb resw 1

delay_sb resw 1 ; ������ �� 1/44100 � � ������ ���

rom_len resw 1 ; ����� ���
rom_adr resw 1 ; ���. ����� ���

ppi_portc_adr resw 1 ; ����� ����� C ���
crt_port1_adr resw 1 ; ����� �������� ���������� ��75

; ����� ����� ����� � ���
romname resd 1

; ������� ������� ���������� 8080
c_table resd 1

f806_adr resw 1 ; ����� ��������� ������ ����� � �����������
f80c_adr resw 1 ; ����� ��������� ������ ����� �� ����������

hook_proc resd 1 ;!!! ����� ��������� ���������

hook_proc_sb  resd 1 ;!!! ����� ��������� ��������� - Sound Blaster

;interpret_proc resd 1 ;!!! ����� ��������� ��������� �����

tick_count resw 1 ; ������� ������ 8080

cnt_44100 resw 1 ; ������� ������ �� 1/44100 �

mem_fetch resd 1

;- TEXT ----------------------------------------------------------------------
[segment .text]

; ������������� ��������� �� ���������
SetDefParams:
        pusha

        ; ��������� �������������� ��������
        mov byte [f_speed],SPEED_NORM
        mov byte [f_scr],SCR_AUTO
        mov byte [f_snd],SND_ON
        mov byte [f_tape],TAPE_FILE
        call RefreshMenu

        ; � ������������ �� � ����������� �� �����. � �����. ������������
        ;call test_vga
        ;jz l3
        ;call disable_vga        ; ����������� VGA - ��������� ����� 78x30
l3:     ;call RefreshMenu
        popa
        ret
;SetDefParams endp

; ��������� �������������. ���������� ��� ������ ����� ���� ��.
Init:
        pusha

        mov ebp, dword [mempage0]
        mov dword [mem],ebp
        mov byte [mem_page_no],0

; �����, ����� ���������, ��������� vertical refresh ����� ������ �����
; ��75 �� �������� - ��������� �� ���� ���
; � ����� ����� �� ����� ������ ������� ��, �� � ���/���.
        cmp byte [cModel],MODEL_S
        je l7
        cmp byte [cModel],MODEL_M80
        je l17
        cmp byte [cModel],MODEL_U
        jne l7
        cmp byte [cModel],MODEL_O
        je l8

        movzx ebx, word [crt_port1_adr]
        mov byte [ebp+ebx],20h

l8:     movzx ebx, word [ppi_portc_adr]
        mov byte [ebp+ebx],0f0h
l17:
        mov word [ebp+0f75ch],2436h  ; ��������� ������/������ ��� �����-80
l7:
        call cls

        mov byte [f_chscr],1
        call anal_params        ; ��������� ��������� �� ������ ����������

;        call Reset ;!!!

        popa
        ret
;Init endp

AfterLoadRom:
        mov byte [f_saverom],0 ; ���������� ������� ���������� ���

        ; ���������� ������ �������� ����� ����� � �������
        push ebp
        mov ebp, dword [mempage0]
        mov ax, word [ebp+0f807h]
        mov word [f806_adr],ax
        mov ax, word [ebp+0f80dh]
        mov word [f80c_adr],ax
        pop ebp
        ret

; ������������� �������� � DMA, ���� ����������� ����. ��������
set_delay_w_dma:
        push ax
        mov ax, word [delay_sb_dma]
        mov word [cur_speed_sb],ax
        cmp byte [f_speed],SPEED_NORM
        jne sdn1
        mov word [delay_sb],ax
sdn1:   pop ax
        ret
;set_delay_w_dma endp

; ������������� �������� ��� DMA, ���� ����������� ����. ��������
set_delay_wo_dma:
        push ax
        mov ax, word [delay_sb_norm]
        mov word [cur_speed_sb],ax
        cmp byte [f_speed],SPEED_NORM
        jne sdd1
        mov word [delay_sb],ax
sdd1:   pop ax
        ret
;set_delay_wo_dma endp

; ������������� �������� ������ DMA ����������� ������,
; ���� ����������� ����. �������� ��� ��������
set_delay_w_dma28:
        cmp byte [cModel],MODEL_P
        jne set_delay_w_dma
        push ax
        mov ax, word [delay_sb_28]
        mov word [cur_speed_sb],ax
        cmp byte [f_speed],SPEED_NORM
        jne sd28_1
        mov word [delay_sb],ax
sd28_1: pop ax
        ret
;set_delay_w_dma28 endp

; ������������� ������ ������ � ������������ � ����������� � ������ ���-��
set_video:
        cmp byte [f_scr],SCR_GRAPH
        je set_graph_mode

        cmp byte [cModel],MODEL_M80
        je set_video_40
        cmp byte [cModel],MODEL_U
        je set_video_40
        cmp byte [cModel],MODEL_S
        je set_graph_mode
        cmp byte [cModel],MODEL_O
        je set_graph_mode
        ;cmp byte [f_scr],SCR_LOW ; !!!
        ;jnz sv1
        ;jmp set_video_25
sv1:    cmp byte [f_scr],SCR_HIGH
        jne sv2
        jmp set_video_30
sv2:    call set_video_auto
sv3:
        ret
;set_video endp

set_video_30: ; !!!
        call setscr30
        ret
;set_video_30 endp

set_video_40:
        call setscr40
        ret
;set_video_40 endp

set_video_60:
        call setscr60
        ret
;set_video_60 endp

set_video_auto:
        cmp byte [sh_height],30
        jbe set_video_30
        cmp byte [sh_height],60
        jae set_video_60
;        cmp byte ptr cs:[f_use8x12],0
;        je set_video_43
        jmp set_video_40
;set_video_auto endp

set_graph_mode:
        push eax
        push ebp
        push ecx

        cmp byte [cModel],MODEL_S
        je sgm6
        cmp byte [cModel],MODEL_O
        je sgm6

        pusha
      __align_sp
        mov eax,VM_GRAPH512
        push eax
        call SetVideoMode
        add esp,4
;        call DrawTextScreenInGraph
      __restore_sp
        popa
        jmp sgm7

sgm6:   ;!!! ��������� ������������ ������ "������" � "�����������"

        pusha
      __align_sp
        mov eax,VM_GRAPH800
        push eax
        call SetVideoMode
        add esp,4
;        call _DrawTextScreenInGraph
      __restore_sp
        popa

sgm7:
        pop ecx
        pop ebp
        pop eax
        ret
;set_graph_mode endp

; ������ ���������� �� ������ ����������
anal_params:
        push eax
        ;call ResetDSP ; !!!
        ;mov dword [interpret_proc], interpret
        mov eax, dword [hook_proc_sb]
        mov dword [hook_proc],eax
        mov ax, word [ticks_per_44100th]
        mov word [ticks_per_calc],ax
ap10:   cmp byte [f_speed],SPEED_HIGH
        jnz ap1
        mov word [delay_sb],60h ; !!!
        jmp ap2
ap1:    cmp byte [f_speed],SPEED_MEDIUM
        jnz ap5
        mov ax, word [delay_sb_norm]
        mov word [delay_sb],ax
        jmp ap2
ap5:    mov ax,word [cur_speed_sb]
        mov word [delay_sb],ax
ap2:   cmp byte [f_scr],SCR_GRAPH
       jne ap3
       ;call RefreshMenu ; !!!
       jmp ap3
ap3:    call LeaveInteractiveMode
        mov dword [c_table], code_tbl_m2
       call process_port_c
        cmp byte [f_chscr],0 ; ???
        jz ap9
        call set_video
ap9:    call refresh_scr ;!!!
ap7:    mov byte [f_chscr],1 ; ����� ???
        pop eax
        ret
;anal_params endp

; ���������� ��� ������� F10
setparms:
        pusha
        call translate_sysreq_code
        cmp byte [sysreq_code+1],SYSREQ_SPEEDUP
        je sp3 ; �� ��������������� ���������
        cmp byte [sysreq_code+1],SYSREQ_SPEEDDOWN
        je sp3 ; �� ��������������� ���������
        cmp byte [sysreq_code+1],SYSREQ_PAUSE
        je sp3 ; �� ��������������� ���������
        mov al,byte [f_scr]             ; !!!
        call EnterInteractiveMode       ; ������������ ��� �������
        call setup
        xor al,byte [f_scr]
        mov byte [f_chscr],al
sp2:    call anal_params
        popa
        ret
sp3:    call setup
        popa
        ret
;setparms endp

; ������� ������
cls: ;!!!
        pusha
        call ClearScreen
        popa
        ret
;cls endp

store_regs:
        mov word [reg_af],ax
        mov word [reg_bc],cx
        mov word [reg_de],dx
        mov word [reg_hl],bx
        mov word [reg_sp],di
        mov word [reg_pc],si
        ret
;store_regs endp

restore_regs:
        movzx eax, word [reg_af]
        movzx ecx, word [reg_bc]
        movzx edx, word [reg_de]
        movzx ebx, word [reg_hl]
        movzx edi, word [reg_sp]
        movzx esi, word [reg_pc]
        ret
;restore_regs endp


;#########################################################################

Reset:
        ; �������� ��������� �������� � �������� 8080
        xor eax,eax
        mov word [reg_af],ax
        mov word [reg_bc],ax
        mov word [reg_de],ax
        mov word [reg_hl],ax
        mov word [reg_sp],ax
        mov ax,word [reset_addr]
        mov word [reg_pc],ax
;        call store_regs
        ; ��������� �������� ������
        mov word [tick_count],0
        call perform_reset
        ret

InterpretOp:
        push ebp
        mov ebp,esp
        call restore_regs

; ������ ��������� �����
cont_interpret:
        ; ���������, �� ������ �� � �������
        cmp si,word [rom_adr]
        jb inter2
        jmp dword [hook_proc_sb]

inter2:
        ; ����� ����� ��������� �� ������� ...
        mov ebp,dword [c_table]
        push ebx
        mov ebx, dword [mem]
        ; ���������� ����� ���. �������� ������
        mov dword [mem_fetch], ebx
        ; �� ������ �� � ��������� �������?
        cmp si,0f000h
        jb i_pag
        ; ���� ��, �� ����� ������ �� 0-�
        mov ebx, dword [mempage0]
        mov dword [mem_fetch],ebx
i_pag:  movzx ebx,byte [ebx+esi]
        add ebx,ebx
        add ebx,ebx
        add ebp,ebx
        pop ebx
        mov ebp,dword [ebp]
        ; � ������ ����� ������ �� ������
        cmp byte [f_pause],0
        jnz ip2
        ; �������� ���������� �������
        call ebp

        call store_regs
        movzx eax,bp
        pop ebp
        ret
ip2:    call store_regs
        mov eax,10
        pop ebp
        ret

Emulate:
        push ebp
        mov ebp,esp
        mov eax, dword[ebp+8]
        mov word [cnt_44100],ax
;        call restore_regs
        mov ebp,0

interpret:
        call InterpretOp
        mov ebp,eax


        ; ��������������� ����� ������� �������� ������
        push ebx
     ;mov ebx, dword [mem_mem]
     ;mov dword [mem],ebx
        pop ebx
ip21:
        add bp,word [tick_count]
        mov word [tick_count],bp
        sub bp,word [delay_sb]
        jae inter3
        jmp interpret ; � ��������� �������

; ���� ���������� �������� ������ 1/44100 �
inter3: mov word [tick_count],bp
        call calc_pit

        dec word [cnt_20ms]
        jnz inter4

        ; ���� ���������� �������� 50 ��� � �������
        mov word [cnt_20ms],882
        mov byte [vretr_cnt],28
        call process_int

inter4:
         cmp word [cnt_44100],0
        jnz inter5

        cmp byte [bWaitSnd],0
        jnz inter6
        jmp interpret

inter5: dec word [cnt_44100]
        jnz interpret

inter6: pop ebp
        ret

; ���� ���������� �������� 50 ��� � �������
ProcessQuery:
        push ebp
        xor eax,eax
        cmp byte [f_quit],1
        jz pq1

        cmp byte [sysreq],0
        jz pq2

        mov byte [sysreq],0
        call setparms
        cmp byte [f_reset],0
        jz pq2
        call Reset
        mov byte [f_reset],0
pq2:    mov eax,1
pq1:    pop ebp
        ret

PrepareScreen:
        jmp refresh_scr

;######################

hook_a_sb:
        cmp si,0fe61h
        jz near _f803
        cmp si,0fe70h
        jz near _f81b
        cmp si,0fc46h
        jz near _f80c
        cmp si,0fb98h
        jz near _f806
        jmp inter2

hook_p_sb:
        cmp si,0fd7bh
        jz near _f803
        cmp si,0fce9h
        jz near _f81b
        cmp si,0fc55h
        jz near _f80c
        cmp si,0fba2h
        jz near _f806
        jmp inter2

hook_r_sb:
        cmp si,0fe63h
        jz near _f803
        cmp si,0fe72h
        jz near _f81b
        cmp si,0fc46h
        jz near _f80c
        cmp si,0fb98h
        jz near _f806
        jmp inter2

hook_m_sb:
        cmp si,0fed0h
        jz near _f803
        cmp si,0feeah
        jz near _f81b
        cmp si,0fcabh
        jz near _f80c
        cmp si,0fc0dh
        jz near _f806
        jmp inter2

hook_s_sb:
        cmp si,0c337h
        jz near _f803
        cmp si,0ce5ch
        jz near _f81b
        cmp si,0c3d0h
        jz near _f80c_s
        cmp si,0c377h
        jz near _f806
        jmp inter2

hook_o_sb:
        cmp si,0f803h
        jz near _f803
        cmp si,0faeeh
        jz near _f81b
;        cmp si,0fa53h
        cmp si,word [f80c_adr]
        jz _f80c
;        cmp si,0f9cdh
        cmp si,word [f806_adr]
        jz near _f806
        jmp inter2

hook_m80_sb:
        cmp si,0ff41h
        jz near _f803
        cmp si,0fde6h
        jz _f80c
        cmp si,0fd95h
        jz _f806
        jmp inter2

hook_u_sb:
        cmp si,0fd57h
        jz _f803
        cmp si,0fd9ah
        jz _f81b
        cmp si,0fbeeh
        jz _f80c
        cmp si,0fb71h
        jz _f806
        jmp inter2

_f80c:
        cmp byte [f_tape],TAPE_FILE
        jne inter2
        call emF80C
        call emu_ret
        jmp cont_interpret

_f80c_s:
        cmp byte [f_tape],TAPE_FILE
        jne inter2
        push cx
        mov cl,al
        call emF80C
        pop cx
        call emu_ret
        jmp cont_interpret

_f806:
        cmp byte [f_tape],TAPE_FILE
        jne inter2
        call emF806
        call emu_ret
        jmp cont_interpret

_f803:
        call close_file ; ���� �������� � ������, �� ��� �������
        jmp inter2

_f81b:
        call close_file_delay ; ���� �������� � ������, �� ��� �������
        jmp inter2

;##############################################################

;end
