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

; File: interf.asm

; —Ó‰ÂÊËÚ ÔÓˆÂ‰Û˚ ÓÚÓ·‡ÊÂÌËˇ ÏÂÌ˛ Ë Á‡ÔÓÒ‡ ËÏÂÌË Ù‡ÈÎ‡

[bits 32]

%include "emu80.inc"

cextern pcScreenBuf

cextern ReadScan
cextern SetCursorPos
cextern PrintChar
cextern SaveCursorPos
cextern RestoreCursorPos
cextern EnterInteractiveMode
cextern LeaveInteractiveMode
cextern ChooseModel
cextern ShowHelp
cextern About
cextern SaveScr
cextern RestScr
cextern ChooseROM
cextern ChooseDisk
cextern AlterSpeed
cextern SaveSS
cextern LoadSS
cextern ChooseFile
cextern ToggleFullScreen

cextern reg_af
cextern reg_bc
cextern reg_de
cextern reg_hl
cextern reg_sp
cextern reg_pc

cextern int_flag
cextern reset_addr

[extern sysreq_code]
[extern sysreq]
[extern f_pause]
[extern translate_sysreq_code]

cglobal filename
cglobal f_file_ss
cglobal f_choose_file
cglobal need_clear_mem
cglobal f_quit
cglobal f_synb

cglobal f_speed
cglobal f_scr
cglobal f_snd
cglobal f_tape

cglobal i_spmed
cglobal i_scrauto
cglobal getfilename

[global setup]
[global set_cursor_pos]
[global f_reset]

;public disable_vga
;public set_fontoem
;public enable_hookf809
;public set_cga_mode
;public set_text_mode
;public bNeedClearMem

[section .bss]

filename resb 260

need_clear_mem resb 1

[section .data]

; 1, ÂÒÎË ÌÛÊÌÓ ‚˚·Ë‡Ú¸ Ù‡ÈÎ
f_choose_file db 0 ; ?

; 1, ÂÒÎË Ù‡ÈÎ ÒÌ‡Ô¯ÓÚ‡
f_file_ss db 0 ; ?

; 1, ÂÒÎË ÌÛÊÌÓ ÔËÒ‡Ú¸ ÒËÌıÓ·‡ÈÚ
f_synb db 0

frame db '…ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕª'
      db '∫  à¨Ô ‰†©´†:                      ∫'
      db '«ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ∂'
      db '∫                                  ∫'
      db '»ÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕÕº'

emp_hint db  '       Esc-Æ‚¨•≠†       ',0
emp_hint_r db  '   Esc - Æ‚¨•≠† ß†Ø®·®  ',0
nemp_hint db 'Enter-¢Î°Æ‡  Esc-Æ‚¨•≠†',0
wr_hint db   '  Esc-Æ‚¨•≠†  F3:ë/°†©‚-[-] ',0
prompt_ss db 'î†©´[.rss]:',0
prompt_odi db 'î†©´[.odi]:',0

f_reset db 0

; 1, •·´® ß†‚‡•°Æ¢†≠ ¢ÎÂÆ§ ®ß Ø‡Æ£‡†¨¨Î
f_quit db 0

i_scrauto:
        db SCR_AUTO
        db 1
        db 21
        dd i_scrgr
        dd i_scrgr
        dd l_snd
        dd l_speed
        db 0
        db '8x16',0
i_scrgr:
        db SCR_GRAPH
        db 1
        db 28
        dd i_scrauto
        dd i_scrauto
        dd l_snd
        dd l_speed
        db 0
        db 'é‡®£®≠.',0

l_scr:
f_scr: db SCR_AUTO
        dd l_snd
        dd l_speed
        dd i_scrauto
        db 1
        db 3
        db 'í•™·‚Æ¢Î© ‡•¶®¨:',0

i_sndon:
        db SND_ON
        db 2
        db 21
        dd i_sndoff
        dd i_sndoff
        dd l_tape
        dd l_scr
        db 0
        db 'Ç™´.',0
i_sndoff:
        db SND_OFF
        db 2
        db 27
        dd i_sndon
        dd i_sndon
        dd l_tape
        dd l_scr
        db 0
        db 'ÇÎ™´.',0
l_snd:
f_snd: db SND_ON
        dd l_tape
        dd l_scr
        dd i_sndon
        db 2
        db 3
        db 'á¢„™:',0

i_tapefile:
        db TAPE_FILE
        db 3
        db 21
        dd i_tapesbout
        dd i_tapesbin
        dd l_speed
        dd l_snd
        db 0
        db 'î†©´',0
i_tapesbout:
        db TAPE_SBOUT
        db 3
        db 27
        dd i_tapesbin
        dd i_tapefile
        dd l_speed
        dd l_snd
        db 0
        db 'ÇÆ·Ø‡Æ®ß¢.',0
i_tapesbin:
        db TAPE_SBIN
        db 3
        db 39
        dd i_tapefile
        dd i_tapesbout
        dd l_speed
        dd l_snd
        db 1
        db 'Ç¢Æ§',0
l_tape:
f_tape:db TAPE_FILE
        dd l_speed
        dd l_snd
        dd i_tapefile
        db 3
        db 3
        db 'å†£≠®‚Æ‰Æ≠:',0

i_spnorm:
        db SPEED_NORM
        db 4
        db 21
        dd i_spmed
        dd i_spmax
        dd l_scr
        dd l_tape
        db 0
        db 'çÆ‡¨.',0
i_spmed:
        db SPEED_MEDIUM
        db 4
        db 28
        dd i_spmax
        dd i_spnorm
        dd l_scr
        dd l_tape
        db 0
        db 'Å•ß èÑè',0
i_spmax:
        db SPEED_HIGH
        db 4
        db 37
        dd i_spnorm
        dd i_spmed
        dd l_scr
        dd l_tape
        db 0
        db 'ÅÎ·‚‡.',0
l_speed:
f_speed:
        db SPEED_NORM
        dd l_scr
        dd l_tape
        dd i_spnorm
        db 4
        db 3
        db 'ë™Æ‡Æ·‚Ï:',0

stline  dd l_scr

lastline3 db '«ƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒƒ∂',0
lastline1 db 'ESC/Enter-ÇÆß¢‡†‚ R-Reset Q-ÇÎÂÆ§',0
lastline2 db 'F1-èÆ¨ÆÈÏ A-"é Ø‡Æ£‡†¨¨•"',0

[section .text]

h equ 9
w equ 47
t equ 7
l equ 16

outtextxy:
        push ax
        mov al,dh
        add al,t
        add dl,l
        xor ah,ah
        mov cl,80
        mul cl
        xor dh,dh
        add ax,dx
        shl ax,1
        movzx edi,ax
        push ecx
        mov ecx,dword[pcScreenBuf]
        add edi,ecx
        pop ecx
        pop ax
        cld
otxy1:  lodsb
        test al,al
        jz otxyex
        stosw
        jmp otxy1
otxyex: ret

outtextxy2:
        push ax
        mov al,dh
        xor ah,ah
        mov cl,80
        mul cl
        xor dh,dh
        add ax,dx
        shl ax,1
        movzx edi,ax
        push ecx
        mov ecx,dword[pcScreenBuf]
        add edi,ecx
        pop ecx
        pop ax
        cld
otxy2_1:  lodsb
        test al,al
        jz otxy2_ex
        stosw
        jmp otxy2_1
otxy2_ex: ret

printitem:
        push edx
        push ecx
        push eax
        mov dl,byte [ebx+2]
        mov dh,byte [ebx+1]
        lea esi,[ebx+20]
        call outtextxy
        pop eax
        pop ecx
        pop edx
        ret

printline:
        push esi
        push ecx
        push ebx
        push edx
        push ecx
        push eax
        mov dl,byte [ebx+14]
        mov dh,byte [ebx+13]
        lea esi,[ebx+15]
        mov ah,1bh
        call outtextxy
        pop eax
        pop ecx
        pop edx
        mov ebx,dword [ebx+9]
        mov ecx,ebx
        call printitem
pl1:    mov ebx,dword [ebx+3]
        cmp ecx,ebx
        je plex
        mov ah,17h
        cmp byte [ebx+19],0
        jz pl2
        mov ah,18h
pl2:    call printitem
        jmp pl1
plex:   pop ebx
        pop ecx
        pop esi
        ret

printall:
        push ebx
        push ecx
        mov ebx,[stline]
        mov ecx,ebx
pa1:    mov ah,1eh
        call printline
        mov ebx,dword [ebx+1]
        cmp ebx,ecx
        jne pa1
        pop ecx
        pop ebx
        ret

prframe:
        mov edi,dword[pcScreenBuf]
        add edi,160*t+(l*2)
        mov ecx,h
        cld
        mov ax,1e00h
gw1:    push ecx
        push edi
        mov ecx,w
        rep stosw
        pop edi
        pop ecx
        add edi,160
        loop gw1
        mov edi,dword[pcScreenBuf]
        add edi,160*t+(l*2)+2
        mov eax,1ecdh
        mov ecx,w-2
        rep stosw
        mov edi,dword[pcScreenBuf]
        add edi,160*t+160*(h-1)+(l*2)+2
        mov eax,1ecdh
        mov ecx,w-2
        rep stosw
        mov edi,dword[pcScreenBuf]
        add edi,160*t+(l*2)
        mov byte [edi],'…'
        mov byte [edi+(w*2-2)],'ª'
        mov byte [edi+(h-1)*160],'»'
        mov byte [edi+(h-1)*160+(w*2-2)],'º'
        mov ecx,h-2
gw2:    add edi,160
        mov byte [edi],'∫'
        loop gw2
        mov ecx,h-2
        mov edi,dword[pcScreenBuf]
        add edi,160*(t+1)+(l*2)+(w*2-2)
gw3:    mov byte [edi],'∫'
        add edi,160
        loop gw3
        mov ah,1ah
        mov dh,h-3
        mov dl,7
        mov esi,lastline1
        call outtextxy
        mov ah,1ah
        mov dh,h-2
        mov dl,10
        mov esi,lastline2
        call outtextxy
        mov ah,1eh
        mov dh,h-4
        mov dl,0
        mov esi,lastline3
        call outtextxy
        ret

readscan:
  push ebx
  push ecx
  push edx
  push esi
  push edi
  push ebp
__align_sp
  call ReadScan
__restore_sp
  pop ebp
  pop edi
  pop esi
  pop edx
  pop ecx
  pop ebx
  ret

input:
        call SaveCursorPos
        mov dx,0ffffh
        call set_cursor_pos
        mov ebx,dword [stline]
inpaa:  mov esi,ebx
        mov ebx,dword [ebx+9]
inpl:   mov ah,3eh
        push ebx
        mov ebx,esi
        call printline
        pop ebx
        mov al,byte [ebx]
        mov byte [esi],al
inpr:
;        mov ah,10h
;        int 16h
        call readscan
;  mov ah,50h
;  ret
        cmp ah,4bh ; <-
        jne inp1
inpb:   mov ebx,dword [ebx+7]
        mov dword [esi+9],ebx
        cmp byte [ebx+19],0
        jnz inpb
        jmp inpl
inp1:   cmp ah,4dh ; ->
        jne inp2
inpc:   mov ebx,dword [ebx+3]
        mov dword [esi+9],ebx
        cmp byte [ebx+19],0
        jnz inpc
        jmp inpl
inp2:   cmp ah,48h ; ^
        jne inp3
        mov ebx,dword [ebx+15]
        jmp inpa
inp3:   cmp ah,50h ; \/
        jne inp4
inp5:   mov ebx,dword [ebx+11]
        jmp inpa
inp4:  cmp ah,43h ; F9
       jne inpd
       push ebx
       push esi
     __align_sp
       call ChooseModel
     __restore_sp
       pop esi
       pop ebx
;       test ax,ax
;       jnz inp7
       jmp inpr
inpd:  cmp ah,3bh ; F1
       jne inpf
       push ebx
       push esi
     __align_sp
       call ShowHelp
     __restore_sp
       pop esi
       pop ebx
       jmp inpr
inpf:  cmp ah,1eh ; A
       jne inpe
       push ebx
       push esi
     __align_sp
       call About
     __restore_sp
       pop esi
       pop ebx
       jmp inpr
inpe:   cmp al,0dh
        je inpex
        cmp al,1bh
        je inpex
        and al,0dfh
        cmp al,51h
        je inp6
        cmp al,52h
        je inp7
        jmp inpr
inpa:   push ebx
        mov ebx,esi
        mov ah,1eh
        call printline
        pop ebx
        mov dword [stline],ebx
jmp inpaa
inp6:   mov byte [f_quit],1
        ;call rest_scr
        ;jmp quit
inp7:   mov byte [f_reset],1
inpex:  call RestoreCursorPos
        ret


query:
        push eax
        push edi
        push esi
        push ecx
        mov edi,dword[pcScreenBuf]
        add edi,160*10+44
        mov esi,frame
        cld
        mov dx,5
em0:    push edi
        mov ecx,36
        mov ah,1eh
em1:    lodsb
        stosw
        loop em1
        pop edi
        add edi,160
        dec dx
        jnz em0
        cmp byte [f_file_ss],1
        jne em2
       cmp byte [f_choose_file],0
       jne em2
        mov esi,prompt_ss
        mov edx,0b18h
        mov ah,1eh
        call outtextxy2
em2:    cmp byte [f_file_ss],2
        jne em3
        mov esi,prompt_odi
        mov edx,0b18h
        mov ah,1eh
        call outtextxy2
em3:    pop ecx
        pop esi
        pop edi
        pop eax
        ret

setup:
        pushf
        push eax
        push ebx
        push ecx
        push edx
        push esi
        push edi
        push ebp
      cmp byte [sysreq_code+1],SYSREQ_MENU
      jne near set1
     __align_sp
        call SaveScr
        push eax
        call EnterInteractiveMode
;        call StopSound
        call prframe
        call printall
        call input
;        pop eax
;        push eax
        call RestScr
        call LeaveInteractiveMode
        add esp,4
     __restore_sp
set7:   cmp byte [f_reset],1
       jne set10
set6:  mov word [reg_af],0
       mov word [reg_bc],0
       mov word [reg_de],0
       mov word [reg_hl],0
       mov word [reg_sp],0
       mov byte [int_flag],0
       push ax
       mov ax, word [reset_addr]
       mov word [reg_pc],ax
       pop ax
       mov byte [f_reset],1
set10:  jmp set2
set1:   cmp byte [sysreq_code+1],SYSREQ_RESET
        je set6
        cmp byte [sysreq_code+1],SYSREQ_SAVESS
        jne set4
     __align_sp
        call SaveSS
        call LeaveInteractiveMode
     __restore_sp
        mov byte [f_reset],0
        jmp set2
set4:   cmp byte [sysreq_code+1],SYSREQ_LOADSS
        jne set11
     __align_sp
        call LoadSS
        push eax
        call LeaveInteractiveMode
        pop eax
     __restore_sp
        mov byte [f_reset],0
        cmp al,3
        jne near set2
        mov byte [f_reset],1
        jmp set7 ; ???
set11:  cmp byte [sysreq_code+1],SYSREQ_DISKA
        jne set12
        mov eax,0
        push eax
      __align_sp
        call ChooseDisk
        call LeaveInteractiveMode
      __restore_sp
        add esp,4
        mov byte [f_reset],0
        jmp set2
set12:  cmp byte [sysreq_code+1],SYSREQ_DISKB
        jne set5
        mov eax,1
        push eax
      __align_sp
        call ChooseDisk
        call LeaveInteractiveMode
      __restore_sp
        add esp,4
        mov byte [f_reset],0
        jmp set2
set5:   cmp byte [sysreq_code+1],SYSREQ_ROM
        je near set13
        cmp byte [sysreq_code+1],SYSREQ_SPEEDUP
        je near set14
        cmp byte [sysreq_code+1],SYSREQ_SPEEDDOWN
        je near set14
        mov byte [need_clear_mem],1
        cmp byte [sysreq_code+1],SYSREQ_MODEL
        je set8
        mov byte [need_clear_mem],0
        cmp byte [sysreq_code+1],SYSREQ_MODELNOCLEAR
        je set8
        cmp byte [sysreq_code+1],SYSREQ_QUIT
        je near set9
        cmp byte [sysreq_code+1],SYSREQ_FULLSCREEN
        je near set16
        cmp byte [sysreq_code+1],SYSREQ_PAUSE
        je near set15
        cmp byte [sysreq_code+1],SYSREQ_ABOUT
        jne set17
     __align_sp
        call About
        call LeaveInteractiveMode
     __restore_sp
        jmp set2
set17:
     __align_sp
        call ShowHelp
        call LeaveInteractiveMode
     __restore_sp
        jmp set2
set8:
     __align_sp
        call ChooseModel
        call LeaveInteractiveMode
     __restore_sp
        mov byte [f_reset],al
        jmp set7
set13:
     __align_sp
        call ChooseROM
        call LeaveInteractiveMode
     __restore_sp
        mov byte [f_reset],al
        jmp set7
set15:  xor byte [f_pause],0ffh
        mov byte [f_reset],0
        jmp set2
set14:  mov al,byte [sysreq_code+1]
        xor ah,ah
        push ax
        call AlterSpeed
        pop ax
        mov byte [f_reset],0
        jmp set2
set16:
     __align_sp
        call ToggleFullScreen
     __restore_sp
        jmp set2

set9:   ;jmp quit
        mov byte [f_quit],1
set2:
        pop ebp
        pop edi
        pop esi
        pop edx
        pop ecx
        pop ebx
        pop eax
        popf
        ret

getfilename:
        pushf
        push eax
        push ebx
        push ecx
        push edx
        push esi
        push edi
        push ebp
     __align_sp
        call SaveScr
        push eax
        call query
        call readstr
        ;pop eax
        ;push eax
        call RestScr
        add esp,4
     __restore_sp
        pop ebp
        pop edi
        pop esi
        pop edx
        pop ecx
        pop ebx
        pop eax
        popf
        ret

prsymb:
        pusha
        movzx eax,al
      __align_sp
        push eax
        call PrintChar
        add esp,4
      __restore_sp
        popa
        ret

set_cursor_pos:
        pusha
;      __align_sp
        movzx eax,dh
        push eax
        movzx eax,dl
        push eax
        call SetCursorPos
        add esp,8
;      __restore_sp
        popa
        ret

readstr:
        call SaveCursorPos
        mov dx,0b24h
        call set_cursor_pos
        mov ebx,filename
rstr1:  mov byte [ebx],0
rstr2:  call show_hint
        call readscan
        cmp al,0dh ; Enter
        je rstr3
        cmp al,1bh ; ESC
        je rstr9
        cmp ax,3d00h ; F3
        je near rstrc
        cmp al,8 ; Bsp
        je rstr5
        cmp ebx,filename+20
        je rstr2
        test al,al
        jz rstr4
        cmp al,20h
        jl rstr2
;        cmp al,7fh ; ç•Æ°Ôß†‚•´Ï≠Æ ¢ SDL-¢•‡·®®
;        jg rstr2
        mov byte [ebx],al
        inc ebx
        call prsymb
        jmp rstr1
rstr4:  cmp ax,4b00h ; <-
        jne rstr2
rstr5:  cmp ebx,filename
        je rstr2
        mov al,8
        call prsymb
        mov al,' '
        call prsymb
        mov al,8
        call prsymb
        dec ebx
        jmp rstr1
rstr9:  mov byte [filename],0
        jmp rstr6
rstr3:
        cmp byte [filename],0
        jne rstr6
        push ebx
      __align_sp
        call ChooseFile
      __restore_sp
        pop ebx
        cmp byte [filename],0
        jz rstr2
rstr6:
        call RestoreCursorPos
        ret
rstrc:  xor byte [f_synb],1
        mov al,byte [f_synb]
        xor al,1
        shl al,1
        add al,'+'
        mov byte [wr_hint+19h],al
        jmp rstr2
show_hint:
        cmp byte [f_choose_file],0
        jnz rstra
; á†Ø®·Ï
        mov esi,wr_hint
        cmp byte [f_file_ss],0
        jne rstre
        mov dx,0d1ah
        jmp rstrb
rstre:  mov esi,emp_hint_r
        jmp rstr8
;; ó‚•≠®•
rstra:  cmp byte [filename],0
        jz rstr7
rstrd:  mov esi,emp_hint
        jmp rstr8
rstr7:  mov esi,nemp_hint
rstr8:  mov dx,0d1ch
rstrb:  mov ah,1ah
        call outtextxy2
        ret
