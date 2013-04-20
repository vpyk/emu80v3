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

; File: emucpu.asm

; Содержит процедуры, эмулирующие инструкции МП 8080

[bits 32]

%include "emu80.inc"

global code_tbl
global code_tbl_m
global code_tbl_m2
global rst6
global emu_ret

[extern mem_fetch]

extern save_byte
extern load_byte
extern save_port
extern load_port

cextern enable_ints
cextern disable_ints
cextern use_io_space

[section .data]

; Таблица входов в подпрограммы обработки инструкций 8080 в
; режиме эмуляции видео-ОЗУ и портов клавиатуры
code_tbl_m2:
  dd c00,c01,d02,c03,c04,c05,c06,c07
  dd c08,c09,e0A,c0B,c0C,c0D,c0E,c0F
  dd c10,c11,d12,c13,c14,c15,c16,c17
  dd c18,c19,e1A,c1B,c1C,c1D,c1E,c1F
  dd c20,c21,d22,c23,c24,c25,c26,c27
  dd c28,c29,c2A,c2B,c2C,c2D,c2E,c2F
  dd c30,c31,d32,c33,d34,d35,d36,c37
  dd c38,c39,e3A,c3B,c3C,c3D,c3E,c3F
  dd c40,c41,c42,c43,c44,c45,e46,c47
  dd c48,c49,c4A,c4B,c4C,c4D,e4E,c4F
  dd c50,c51,c52,c53,c54,c55,e56,c57
  dd c58,c59,c5A,c5B,c5C,c5D,e5E,c5F
  dd c60,c61,c62,c63,c64,c65,e66,c67
  dd c68,c69,c6A,c6B,c6C,c6D,e6E,c6F
  dd d70,d71,d72,d73,d74,d75,c76,d77
  dd c78,c79,c7A,c7B,c7C,c7D,e7E,c7F
  dd c80,c81,c82,c83,c84,c85,e86,c87
  dd c88,c89,c8A,c8B,c8C,c8D,e8E,c8F
  dd c90,c91,c92,c93,c94,c95,e96,c97
  dd c98,c99,c9A,c9B,c9C,c9D,e9E,c9F
  dd cA0,cA1,cA2,cA3,cA4,cA5,eA6,cA7
  dd cA8,cA9,cAA,cAB,cAC,cAD,eAE,cAF
  dd cB0,cB1,cB2,cB3,cB4,cB5,eB6,cB7
  dd cB8,cB9,cBA,cBB,cBC,cBD,eBE,cBF
  dd eC0,eC1,cC2,cC3,dC4,dC5,cC6,dC7
  dd eC8,eC9,cCA,cCB,dCC,dCD,cCE,dCF
  dd eD0,eD1,cD2,dD3,dD4,dD5,cD6,dD7
  dd eD8,eD9,cDA,eDB,dDC,dDD,cDE,dDF
  dd eE0,eE1,cE2,cE3,dE4,dE5,cE6,dE7
  dd eE8,cE9,cEA,cEB,d0EC,dED,cEE,dEF
  dd eF0,eF1,cF2,cF3,dF4,dF5,cF6,dF7
  dd eF8,cF9,cFA,cFB,dFC,dFD,cFE,dFF

save_f db 0

[section .text]

; Реализация подпрограмм, соответствующих инструкциям 8080
; Соответствие регистров:
;
;  A - AL
; BC - CX
; DE - DX
; HL - BX
; SP - DI
; PC - SI
; flags - AH

c10:
c20:
c30:
c08:
c18:
c28:
c38:

c00:;nop
        inc si
        mov bp,4
        ret

c01:;lxi b,imm16
        mov ebp,dword[mem_fetch]
        mov cx,word [ebp+esi+1]
        add si,3
        mov bp,10
        ret

c11:;lxi d,imm16
        mov ebp,dword[mem_fetch]
        mov dx,word [ebp+esi+1]
        add si,3
        mov bp,10
        ret

c21:;lxi h,imm16
        mov ebp,dword[mem_fetch]
        mov bx,word [ebp+esi+1]
        add si,3
        mov bp,10
        ret

c31:;lxi sp,imm16
        mov ebp,dword[mem_fetch]
        mov di,word [ebp+esi+1]
        add si,3
        mov bp,10
        ret

c03:;inx b
        inc cx
        inc si
        mov bp,5
        ret

c13:;inx d
        inc dx
        inc si
        mov bp,5
        ret

c23:;inx h
        inc bx
        inc si
        mov bp,5
        ret

c33:;inx sp
        inc di
        inc si
        mov bp,5
        ret

c0B:;dcx b
        dec cx
        inc si
        mov bp,5
        ret

c1B:;dcx d
        dec dx
        inc si
        mov bp,5
        ret

c2B:;dcx h
        dec bx
        inc si
        mov bp,5
        ret

c3B:;dcx sp
        dec di
        inc si
        mov bp,5
        ret

c04:;inr b
        sahf
        inc ch
        lahf
        inc si
        mov bp,5
        ret

c14:;inr d
        sahf
        inc dh
        lahf
        inc si
        mov bp,5
        ret

c24:;inr h
        sahf
        inc bh
        lahf
        inc si
        mov bp,5
        ret

c34:;inr m
        sahf
        mov ebp,dword[mem_fetch]
        inc byte [ebp+ebx]
        lahf
        inc si
        mov bp,10
        ret

c0C:;inr c
        sahf
        inc cl
        lahf
        inc si
        mov bp,5
        ret

c1C:;inr e
        sahf
        inc dl
        lahf
        inc si
        mov bp,5
        ret

c2C:;inr l
        sahf
        inc bl
        lahf
        inc si
        mov bp,5
        ret

c3C:;inr a
        sahf
        inc al
        lahf
        inc si
        mov bp,5
        ret

c05:;dcr b
        sahf
        dec ch
        lahf
        inc si
        mov bp,5
        ret

c15:;dcr d
        sahf
        dec dh
        lahf
        inc si
        mov bp,5
        ret

c25:;dcr h
        sahf
        dec bh
        lahf
        inc si
        mov bp,5
        ret

c35:;dcr m
        sahf
        mov ebp,dword[mem_fetch]
        dec byte [ebp+ebx]
        lahf
        inc si
        mov bp,10
        ret

c0D:;dcr c
        sahf
        dec cl
        lahf
        inc si
        mov bp,5
        ret

c1D:;dcr e
        sahf
        dec dl
        lahf
        inc si
        mov bp,5
        ret

c2D:;dcr l
        sahf
        dec bl
        lahf
        inc si
        mov bp,5
        ret

c3D:;dcr a
        sahf
        dec al
        lahf
        inc si
        mov bp,5
        ret

c06:;mvi b,imm8
        mov ebp,dword[mem_fetch]
        mov ch,byte [ebp+esi+1]
        inc si
        inc si
        mov bp,7
        ret

c16:;mvi d,imm8
        mov ebp,dword[mem_fetch]
        mov dh,byte [ebp+esi+1]
        inc si
        inc si
        mov bp,7
        ret

c26:;mvi h,imm8
        mov ebp,dword[mem_fetch]
        mov bh,byte [ebp+esi+1]
        inc si
        inc si
        mov bp,7
        ret

c36:;mvi m,imm8
        mov ebp,dword[mem_fetch]
        mov ah,byte [ebp+esi+1]
        mov byte [ebp+ebx],ah
        inc si
        inc si
        mov bp,10
        ret

c0E:;mvi c,imm8
        mov ebp,dword[mem_fetch]
        mov cl,byte [ebp+esi+1]
        inc si
        inc si
        mov bp,7
        ret

c1E:;mvi e,imm8
        mov ebp,dword[mem_fetch]
        mov dl,byte [ebp+esi+1]
        inc si
        inc si
        mov bp,7
        ret

c2E:;mvi l,imm8
        mov ebp,dword[mem_fetch]
        mov bl,byte [ebp+esi+1]
        inc si
        inc si
        mov bp,7
        ret

c3E:;mvi a,imm8
        mov ebp,dword[mem_fetch]
        mov al,byte [ebp+esi+1]
        inc si
        inc si
        mov bp,7
        ret

c09:;dad b
        mov bp,0fe00h
        and bp,ax
        add bx,cx
        lahf
        and ax,01ffh
        or ax,bp
        inc si
        mov bp,10
        ret

c19:;dad d
        mov bp,0fe00h
        and bp,ax
        add bx,dx
        lahf
        and ax,01ffh
        or ax,bp
        inc si
        mov bp,10
        ret

c29:;dad h
        mov bp,0fe00h
        and bp,ax
        add bx,bx
        lahf
        and ax,01ffh
        or ax,bp
        inc si
        mov bp,10
        ret

c39:;dad sp
        mov bp,0fe00h
        and bp,ax
        add bx,di
        lahf
        and ax,01ffh
        or ax,bp
        inc si
        mov bp,10
        ret

c02:;stax b
        mov ebp,dword[mem_fetch]
        mov byte [ebp+ecx],al
        inc si
        mov bp,7
        ret

c12:;stax d
        mov ebp,dword[mem_fetch]
        mov byte [ebp+edx],al
        inc si
        mov bp,7
        ret

c0A:;ldax b
        mov ebp,dword[mem_fetch]
        mov al,[ebp+ecx]
        inc si
        mov bp,7
        ret

c1A:;ldax d
        mov ebp,dword[mem_fetch]
        mov al, byte[ebp+edx]
        inc si
        mov bp,7
        ret

c22:;shld addr
        mov ebp,dword[mem_fetch]
        movzx ebp,word [ebp+esi+1]
        add ebp,dword[mem_fetch]
        mov word [ebp],bx
        add si,3
        mov bp,16
        ret

c2A:;lhld addr
        mov ebp,dword[mem_fetch]
        movzx ebp,word [ebp+esi+1]
        add ebp,dword[mem_fetch]
        mov bx,word [ebp]
        add si,3
        mov bp,16
        ret

c32:;sta addr
        mov ebp,dword[mem_fetch]
        movzx ebp,word [ebp+esi+1]
        add ebp,dword[mem_fetch]
        mov [ebp],al
        add si,3
        mov bp,13
        ret

c3A:;lda addr
        mov ebp,dword[mem_fetch]
        movzx ebp,word [ebp+esi+1]
        add ebp,dword[mem_fetch]
        mov al,[ebp]
        add si,3
        mov bp,13
        ret

c07:;rlc
        sahf
        rol al,1
        lahf
        inc si
        mov bp,4
        ret

c0F:;rrc
        sahf
        ror al,1
        lahf
        inc si
        mov bp,4
        ret

c17:;ral
        sahf
        rcl al,1
        lahf
        inc si
        mov bp,4
        ret

c1F:;rar
        sahf
        rcr al,1
        lahf
        inc si
        mov bp,4
        ret

c27:;daa
        sahf
        daa
        lahf
        inc si
        mov bp,4
        ret

c37:;stc
        sahf
        stc
        lahf
        inc si
        mov bp,4
        ret

c2F:;cma
        xor al,0ffh
        inc si
        mov bp,4
        ret

c3F:;cmc
        sahf
        cmc
        lahf
        inc si
        mov bp,4
        ret

c40:;mov b,b
c49:;mov c,c
c52:;mov d,d
c5B:;mov e,e
c64:;mov h,h
c6D:;mov l,l
c7F:;mov a,a
        inc si
        mov bp,5
        ret

c41:;mov b,c
        mov ch,cl
        inc si
        mov bp,5
        ret

c42:;mov b,d
        mov ch,dh
        inc si
        mov bp,5
        ret

c43:;mov b,e
        mov ch,dl
        inc si
        mov bp,5
        ret

c44:;mov b,h
        mov ch,bh
        inc si
        mov bp,5
        ret

c45:;mov b,l
        mov ch,bl
        inc si
        mov bp,5
        ret

c46:;mov b,m
        mov ebp,dword[mem_fetch]
        mov ch,byte [ebp+ebx]
        inc si
        mov bp,7
        ret

c47:;mov b,a
        mov ch,al
        inc si
        mov bp,5
        ret

c48:;mov c,b
        mov cl,ch
        inc si
        mov bp,5
        ret

c4A:;mov c,d
        mov cl,dh
        inc si
        mov bp,5
        ret

c4B:;mov c,e
        mov cl,dl
        inc si
        mov bp,5
        ret

c4C:;mov c,h
        mov cl,bh
        inc si
        mov bp,5
        ret

c4D:;mov c,l
        mov cl,bl
        inc si
        mov bp,5
        ret

c4E:;mov c,m
        mov ebp,dword[mem_fetch]
        mov cl,byte [ebp+ebx]
        inc si
        mov bp,7
        ret

c4F:;mov c,a
        mov cl,al
        inc si
        mov bp,5
        ret

c50:;mov d,b
        mov dh,ch
        inc si
        mov bp,5
        ret

c51:;mov d,c
        mov dh,cl
        inc si
        mov bp,5
        ret

c53:;mov d,e
        mov dh,dl
        inc si
        mov bp,5
        ret

c54:;mov d,h
        mov dh,bh
        inc si
        mov bp,5
        ret

c55:;mov d,l
        mov dh,bl
        inc si
        mov bp,5
        ret

c56:;mov d,m
        mov ebp,dword[mem_fetch]
        mov dh,byte [ebp+ebx]
        inc si
        mov bp,7
        ret

c57:;mov d,a
        mov dh,al
        inc si
        mov bp,5
        ret

c58:;mov e,b
        mov dl,ch
        inc si
        mov bp,5
        ret

c59:;mov e,c
        mov dl,cl
        inc si
        mov bp,5
        ret

c5A:;mov e,d
        mov dl,dh
        inc si
        mov bp,5
        ret

c5C:;mov e,h
        mov dl,bh
        inc si
        mov bp,5
        ret

c5D:;mov e,l
        mov dl,bl
        inc si
        mov bp,5
        ret

c5E:;mov e,m
        mov ebp,dword[mem_fetch]
        mov dl,byte [ebp+ebx]
        inc si
        mov bp,7
        ret

c5F:;mov e,a
        mov dl,al
        inc si
        mov bp,5
        ret

c60:;mov h,b
        mov bh,ch
        inc si
        mov bp,5
        ret

c61:;mov h,c
        mov bh,cl
        inc si
        mov bp,5
        ret

c62:;mov h,d
        mov bh,dh
        inc si
        mov bp,5
        ret

c63:;mov h,e
        mov bh,dl
        inc si
        mov bp,5
        ret

c65:;mov h,l
        mov bh,bl
        inc si
        mov bp,5
        ret

c66:;mov h,m
        mov ebp,dword[mem_fetch]
        mov bh,byte [ebp+ebx]
        inc si
        mov bp,7
        ret

c67:;mov h,a
        mov bh,al
        inc si
        mov bp,5
        ret

c68:;mov l,b
        mov bl,ch
        inc si
        mov bp,5
        ret

c69:;mov l,c
        mov bl,cl
        inc si
        mov bp,5
        ret

c6A:;mov l,d
        mov bl,dh
        inc si
        mov bp,5
        ret

c6B:;mov l,e
        mov bl,dl
        inc si
        mov bp,5
        ret

c6C:;mov l,h
        mov bl,bh
        inc si
        mov bp,5
        ret

c6E:;mov l,m
        mov ebp,dword[mem_fetch]
        mov bl,byte [ebp+ebx]
        inc si
        mov bp,7
        ret

c6F:;mov l,a
        mov bl,al
        inc si
        mov bp,5
        ret

c70:;mov m,b
        mov ebp,dword[mem_fetch]
        mov byte [ebp+ebx],ch
        inc si
        mov bp,7
        ret

c71:;mov m,c
        mov ebp,dword[mem_fetch]
        mov byte [ebp+ebx],cl
        inc si
        mov bp,7
        ret

c72:;mov m,d
        mov ebp,dword[mem_fetch]
        mov byte [ebp+ebx],dh
        inc si
        mov bp,7
        ret

c73:;mov m,e
        mov ebp,dword[mem_fetch]
        mov byte [ebp+ebx],dl
        inc si
        mov bp,7
        ret

c74:;mov m,h
        mov ebp,dword[mem_fetch]
        mov byte [ebp+ebx],bh
        inc si
        mov bp,7
        ret

c75:;mov m,l
        mov ebp,dword[mem_fetch]
        mov byte [ebp+ebx],bl
        inc si
        mov bp,7
        ret

c77:;mov m,a
        mov ebp,dword[mem_fetch]
        mov byte [ebp+ebx],al
        inc si
        mov bp,7
        ret

c78:;mov a,b
        mov al,ch
        inc si
        mov bp,5
        ret

c79:;mov a,c
        mov al,cl
        inc si
        mov bp,5
        ret

c7A:;mov a,d
        mov al,dh
        inc si
        mov bp,5
        ret

c7B:;mov a,e
        mov al,dl
        inc si
        mov bp,5
        ret

c7C:;mov a,h
        mov al,bh
        inc si
        mov bp,5
        ret

c7D:;mov a,l
        mov al,bl
        inc si
        mov bp,5
        ret

c7E:;mov a,m
        mov ebp,dword[mem_fetch]
        mov al,byte [ebp+ebx]
        inc si
        mov bp,7
        ret

c76:;hlt
;       jmp fin
        mov bp,7
        ret

c80:;add b
        add al,ch
        lahf
        inc si
        mov bp,4
        ret

c81:;add c
        add al,cl
        lahf
        inc si
        mov bp,4
        ret

c82:;add d
        add al,dh
        lahf
        inc si
        mov bp,4
        ret

c83:;add e
        add al,dl
        lahf
        inc si
        mov bp,4
        ret

c84:;add h
        add al,bh
        lahf
        inc si
        mov bp,4
        ret

c85:;add l
        add al,bl
        lahf
        inc si
        mov bp,4
        ret

c86:;add m
        mov ebp,dword[mem_fetch]
        add al,byte [ebp+ebx]
        lahf
        inc si
        mov bp,7
        ret

c87:;add a
        add al,al
        lahf
        inc si
        mov bp,4
        ret

c88:;adc b
        sahf
        adc al,ch
        lahf
        inc si
        mov bp,4
        ret

c89:;adc c
        sahf
        adc al,cl
        lahf
        inc si
        mov bp,4
        ret

c8A:;adc d
        sahf
        adc al,dh
        lahf
        inc si
        mov bp,4
        ret

c8B:;adc e
        sahf
        adc al,dl
        lahf
        inc si
        mov bp,4
        ret

c8C:;adc h
        sahf
        adc al,bh
        lahf
        inc si
        mov bp,4
        ret

c8D:;adc l
        sahf
        adc al,bl
        lahf
        inc si
        mov bp,4
        ret

c8E:;adc m
        mov ebp,dword[mem_fetch]
        sahf
        adc al,byte [ebp+ebx]
        lahf
        inc si
        mov bp,7
        ret

c8F:;adc a
        sahf
        adc al,al
        lahf
        inc si
        mov bp,4
        ret

c90:;sub b
        sub al,ch
        lahf
        inc si
        mov bp,4
        ret

c91:;sub c
        sub al,cl
        lahf
        inc si
        mov bp,4
        ret

c92:;sub d
        sub al,dh
        lahf
        inc si
        mov bp,4
        ret

c93:;sub e
        sub al,dl
        lahf
        inc si
        mov bp,4
        ret

c94:;sub h
        sub al,bh
        lahf
        inc si
        mov bp,4
        ret

c95:;sub l
        sub al,bl
        lahf
        inc si
        mov bp,4
        ret

c96:;sub m
        mov ebp,dword[mem_fetch]
        sub al,byte [ebp+ebx]
        lahf
        inc si
        mov bp,7
        ret

c97:;sub a
        sub al,al
        lahf
        inc si
        mov bp,4
        ret

c98:;sbb b
        sahf
        sbb al,ch
        lahf
        inc si
        mov bp,4
        ret

c99:;sbb c
        sahf
        sbb al,cl
        lahf
        inc si
        mov bp,4
        ret

c9A:;sbb d
        sahf
        sbb al,dh
        lahf
        inc si
        mov bp,4
        ret

c9B:;sbb e
        sahf
        sbb al,dl
        lahf
        inc si
        mov bp,4
        ret

c9C:;sbb h
        sahf
        sbb al,bh
        lahf
        inc si
        mov bp,4
        ret

c9D:;sbb l
        sahf
        sbb al,bl
        lahf
        inc si
        mov bp,4
        ret

c9E:;sbb m
        mov ebp,dword[mem_fetch]
        sahf
        sbb al,byte [ebp+ebx]
        lahf
        inc si
        mov bp,7
        ret

c9F:;sbb a
        sahf
        sbb al,al
        lahf
        inc si
        mov bp,4
        ret

cA0:;ana b
        and al,ch
        lahf
        inc si
        mov bp,4
        ret

cA1:;ana c
        and al,cl
        lahf
        inc si
        mov bp,4
        ret

cA2:;ana d
        and al,dh
        lahf
        inc si
        mov bp,4
        ret

cA3:;ana e
        and al,dl
        lahf
        inc si
        mov bp,4
        ret

cA4:;ana h
        and al,bh
        lahf
        inc si
        mov bp,4
        ret

cA5:;ana l
        and al,bl
        lahf
        inc si
        mov bp,4
        ret

cA6:;ana m
        mov ebp,dword[mem_fetch]
        and al,byte [ebp+ebx]
        lahf
        inc si
        mov bp,7
        ret

cA7:;ana a
        and al,al
        lahf
        inc si
        mov bp,4
        ret

cA8:;xra b
        xor al,ch
        lahf
        inc si
        mov bp,4
        ret

cA9:;xra c
        xor al,cl
        lahf
        inc si
        mov bp,4
        ret

cAA:;xra d
        xor al,dh
        lahf
        inc si
        mov bp,4
        ret

cAB:;xra e
        xor al,dl
        lahf
        inc si
        mov bp,4
        ret

cAC:;xra h
        xor al,bh
        lahf
        inc si
        mov bp,4
        ret

cAD:;xra l
        xor al,bl
        lahf
        inc si
        mov bp,4
        ret

cAE:;xra m
        mov ebp,dword[mem_fetch]
        xor al,byte [ebp+ebx]
        lahf
        inc si
        mov bp,7
        ret

cAF:;xra a
        xor al,al
        lahf
        inc si
        mov bp,4
        ret

cB0:;ora b
        or al,ch
        lahf
        inc si
        mov bp,4
        ret

cB1:;ora c
        or al,cl
        lahf
        inc si
        mov bp,4
        ret

cB2:;ora d
        or al,dh
        lahf
        inc si
        mov bp,4
        ret

cB3:;ora e
        or al,dl
        lahf
        inc si
        mov bp,4
        ret

cB4:;ora h
        or al,bh
        lahf
        inc si
        mov bp,4
        ret

cB5:;ora l
        or al,bl
        lahf
        inc si
        mov bp,4
        ret

cB6:;ora m
        mov ebp,dword[mem_fetch]
        or al,byte [ebp+ebx]
        lahf
        inc si
        mov bp,7
        ret

cB7:;ora a
        or al,al
        lahf
        inc si
        mov bp,4
        ret

cB8:;cmp b
        cmp al,ch
        lahf
        inc si
        mov bp,4
        ret

cB9:;cmp c
        cmp al,cl
        lahf
        inc si
        mov bp,4
        ret

cBA:;cmp d
        cmp al,dh
        lahf
        inc si
        mov bp,4
        ret

cBB:;cmp e
        cmp al,dl
        lahf
        inc si
        mov bp,4
        ret

cBC:;cmp h
        cmp al,bh
        lahf
        inc si
        mov bp,4
        ret

cBD:;cmp l
        cmp al,bl
        lahf
        inc si
        mov bp,4
        ret

cBE:;cmp m
        mov ebp,dword[mem_fetch]
        cmp al,byte [ebp+ebx]
        lahf
        inc si
        mov bp,7
        ret

cBF:;cmp a
        cmp al,al
        lahf
        inc si
        mov bp,4
        ret

cC6:;adi imm8
        mov ebp,dword[mem_fetch]
        add al,byte [ebp+esi+1]
        lahf
        inc si
        inc si
        mov bp,7
        ret

cD6:;sui imm8
        mov ebp,dword[mem_fetch]
        sub al,byte [ebp+esi+1]
        lahf
        inc si
        inc si
        mov bp,7
        ret

cE6:;ani imm8
        mov ebp,dword[mem_fetch]
        and al,byte [ebp+esi+1]
        lahf
        inc si
        inc si
        mov bp,7
        ret

cF6:;ori imm8
        mov ebp,dword[mem_fetch]
        or al,byte [ebp+esi+1]
        lahf
        inc si
        inc si
        mov bp,7
        ret

cCE:;aci imm8
        mov ebp,dword[mem_fetch]
        sahf
        adc al,byte [ebp+esi+1]
        lahf
        inc si
        inc si
        mov bp,7
        ret

cDE:;sbi imm8
        mov ebp,dword[mem_fetch]
        sahf
        sbb al,byte [ebp+esi+1]
        lahf
        inc si
        inc si
        mov bp,7
        ret

cEE:;xri imm8
        mov ebp,dword[mem_fetch]
        xor al,byte [ebp+esi+1]
        lahf
        inc si
        inc si
        mov bp,7
        ret

cFE:;cpi imm8
        mov ebp,dword[mem_fetch]
        cmp al,byte [ebp+esi+1]
        lahf
        inc si
        inc si
        mov bp,7
        ret

cCB:
cC3:;jmp addr
        mov ebp,dword[mem_fetch]
        mov si,word [ebp+esi+1]
        mov bp,10
        ret

cCA:;jz addr
        sahf
        jz cC3
        add si,3
        mov bp,7
        ret

cDA:;jc addr
        sahf
        jc cC3
        add si,3
        mov bp,7
        ret

cEA:;jpe addr
        sahf
        jpe cC3
        add si,3
        mov bp,7
        ret

cFA:;jm addr
        sahf
        js cC3
        add si,3
        mov bp,7
        ret

cC2:;jnz addr
        sahf
        jnz cC3
        add si,3
        mov bp,7
        ret

cD2:;jnc addr
        sahf
        jnc cC3
        add si,3
        mov bp,7
        ret

cE2:;jpo addr
        sahf
        jpo cC3
        add si,3
        mov bp,7
        ret

cF2:;jp addr
        sahf
        jns cC3
        add si,3
        mov bp,7
        ret

cDD:
cED:
cFD:
cCD:;call addr
        mov ebp,dword[mem_fetch]
        dec di
        dec di
        add si,3
        mov word [ebp+edi],si
        mov si,word [ebp+esi-2]
        mov bp,17
        ret

cCC:;сz addr
        sahf
        jz cCD
        add si,3
        mov bp,11
        ret

cDC:;сc addr
        sahf
        jc cCD
        add si,3
        mov bp,11
        ret

cEC:;сpe addr
        sahf
        jpe cCD
        add si,3
        mov bp,11
        ret

cFC:;сm addr
        sahf
        js cCD
        add si,3
        mov bp,11
        ret

cC4:;сnz addr
        sahf
        jnz cCD
        add si,3
        mov bp,11
        ret

cD4:;сnc addr
        sahf
        jnc cCD
        add si,3
        mov bp,11
        ret

cE4:;сpo addr
        sahf
        jpo cCD
        add si,3
        mov bp,11
        ret

cF4:;сp addr
        sahf
        jns cCD
        add si,3
        mov bp,11
        ret

emu_ret: ;####

cD9:
cC9:;ret
        mov ebp,dword[mem_fetch]
        mov si,word [ebp+edi]
        inc di
        inc di
        mov bp,11
        ret

cC8:;rz
        sahf
        jz cC9
        inc si
        mov bp,5
        ret

cD8:;rc
        sahf
        jc cC9
        inc si
        mov bp,5
        ret

cE8:;rpe
        sahf
        jpe cC9
        inc si
        mov bp,5
        ret

cF8:;rm
        sahf
        js cC9
        inc si
        mov bp,5
        ret

cC0:;rnz
        sahf
        jnz cC9
        inc si
        mov bp,5
        ret

cD0:;rnc
        sahf
        jnc cC9
        inc si
        mov bp,5
        ret

cE0:;rpo
        sahf
        jpo cC9
        inc si
        mov bp,5
        ret

cF0:;rp
        sahf
        jns cC9
        inc si
        mov bp,5
        ret

cC7:;rst 0
        mov ebp,dword[mem_fetch]
        dec di
        dec di
        inc si
        mov word [ebp+edi],si
        mov si,00h
        mov bp,11
        ret

cD7:;rst 2
        mov ebp,dword[mem_fetch]
        dec di
        dec di
        inc si
        mov word [ebp+edi],si
        mov si,10h
        mov bp,11
        ret

cE7:;rst 4
        mov ebp,dword[mem_fetch]
        dec di
        dec di
        inc si
        mov word [ebp+edi],si
        mov si,20h
        mov bp,11
        ret

rst6:
cF7:;rst 6
        mov ebp,dword[mem_fetch]
        dec di
        dec di
        inc si
        mov word [ebp+edi],si
        mov si,30h
        mov bp,11
        ret

cCF:;rst 1
        mov ebp,dword[mem_fetch]
        dec di
        dec di
        inc si
        mov word [ebp+edi],si
        mov si,08h
        mov bp,11
        ret

cDF:;rst 3
        mov ebp,dword[mem_fetch]
        dec di
        dec di
        inc si
        mov word [ebp+edi],si
        mov si,18h
        mov bp,11
        ret

cEF:;rst 5
        mov ebp,dword[mem_fetch]
        dec di
        dec di
        inc si
        mov word [ebp+edi],si
        mov si,28h
        mov bp,11
        ret

cFF:;rst 7
        mov ebp,dword[mem_fetch]
        dec di
        dec di
        inc si
        mov word [ebp+edi],si
        mov si,38h
        mov bp,11
        ret

cC5:;push b
        mov ebp,dword[mem_fetch]
        dec di
        dec di
        mov word [ebp+edi],cx
        inc si
        mov bp,11
        ret

cD5:;push d
        mov ebp,dword[mem_fetch]
        dec di
        dec di
        mov word [ebp+edi],dx
        inc si
        mov bp,11
        ret

cE5:;push h
        mov ebp,dword[mem_fetch]
        dec di
        dec di
        mov word [ebp+edi],bx
        inc si
        mov bp,11
        ret

cF5:;push psw
        mov ebp,dword[mem_fetch]
        dec di
        dec di
        xchg ah,al
        mov word [ebp+edi],ax
        xchg ah,al
        inc si
        mov bp,11
        ret

cC1:;pop b
        mov ebp,dword[mem_fetch]
        mov cl,byte [ebp+edi]
        mov ch,byte [ebp+edi+1]
        inc di
        inc di
        inc si
        mov bp,10
        ret

cD1:;pop d
        mov ebp,dword[mem_fetch]
        mov dl,byte [ebp+edi]
        mov dh,byte [ebp+edi+1]
        inc di
        inc di
        inc si
        mov bp,10
        ret

cE1:;pop h
        mov ebp,dword[mem_fetch]
        mov bl,byte [ebp+edi]
        mov bh,byte [ebp+edi+1]
        inc di
        inc di
        inc si
        mov bp,10
        ret

cF1:;pop psw
        mov ebp,dword[mem_fetch]
        mov al,byte [ebp+edi+1]
        mov ah,byte [ebp+edi]
        inc di
        inc di
        inc si
        mov bp,10
        ret

cF3:;di
        inc si
        push ax
        call disable_ints
        pop ax
        mov bp,4
        ret
cFB:;ei
        inc si
        push ax
        call enable_ints
        pop ax
        mov bp,4
        ret

cD3:;out port8
        mov ebp,dword[mem_fetch]
        push bx
        mov bh,byte [ebp+esi+1]
        mov bl,bh
        mov byte [ebp+ebx],al
        pop bx
        inc si
        inc si
        mov bp,10
        ret

cDB:;in port8
        mov ebp,dword[mem_fetch]
        push bx
        mov bh,byte [ebp+esi+1]
        mov bl,bh
        mov al,byte [ebp+ebx]
        pop bx
        inc si
        inc si
        mov bp,10
        ret

cE3:;xthl
        mov ebp,dword[mem_fetch]
        xchg word [ebp+edi],bx
        inc si
        mov bp,18
        ret

cEB:;xchg
        xchg dx,bx
        inc si
        mov bp,4
        ret

cF9:;sphl
        mov di,bx
        inc si
        mov bp,5
        ret

cE9:;pchl
        mov si,bx
        mov bp,5
        ret

;#### С перехватом записи ####

d02:;stax b
        push ax
        mov bp,cx
        mov ah,al
        call save_byte
        inc si
        pop ax
        mov bp,7
        ret

d12:;stax d
        push ax
        mov bp,dx
        mov ah,al
        call save_byte
        inc si
        pop ax
        mov bp,7
        ret

d36:;mvi m,imm8
        mov ebp,dword[mem_fetch]
        push ax
        mov ah,byte [ebp+esi+1]
        mov bp,bx
        call save_byte
        inc si
        inc si
        pop ax
        mov bp,10
        ret

d32:;sta addr
        mov ebp,dword[mem_fetch]
        push ax
        mov bp,word [ebp+esi+1]
        mov ah,al
        call save_byte
        add si,3
        pop ax
        mov bp,13
        ret

d34:;inr m
        mov ebp,dword[mem_fetch]
        push cx
        mov cl,byte [ebp+ebx]
        sahf
        inc cl
        lahf
        push ax
        mov ah,cl
        mov bp,bx
        call save_byte
        inc si
        pop ax
        pop cx
        mov bp,10
        ret

d35:;dcr m
        mov ebp,dword[mem_fetch]
        push cx
        mov cl,byte [ebp+ebx]
        sahf
        dec cl
        lahf
        push ax
        mov ah,cl
        mov bp,bx
        call save_byte
        inc si
        pop ax
        pop cx
        mov bp,10
        ret

d22:;shld addr
        mov ebp,dword[mem_fetch]
        push ax
        mov bp,word [ebp+esi+1]
        mov ah,bl
        push bp
        call save_byte
        pop bp
        inc bp
        mov ah,bh
        call save_byte
        add si,3
        pop ax
        mov bp,16
        ret

d70:;mov m,b
        push ax
        mov bp,bx
        mov ah,ch
        call save_byte
        inc si
        pop ax
        mov bp,7
        ret

d71:;mov m,c
        push ax
        mov bp,bx
        mov ah,cl
        call save_byte
        inc si
        pop ax
        mov bp,7
        ret

d72:;mov m,d
        push ax
        mov bp,bx
        mov ah,dh
        call save_byte
        inc si
        pop ax
        mov bp,7
        ret

d73:;mov m,e
        push ax
        mov bp,bx
        mov ah,dl
        call save_byte
        inc si
        pop ax
        mov bp,7
        ret

d74:;mov m,h
        push ax
        mov bp,bx
        mov ah,bh
        call save_byte
        inc si
        pop ax
        mov bp,7
        ret

d75:;mov m,l
        push ax
        mov bp,bx
        mov ah,bl
        call save_byte
        inc si
        pop ax
        mov bp,7
        ret

d77:;mov m,a
        push ax
        mov bp,bx
        mov ah,al
        call save_byte
        inc si
        pop ax
        mov bp,7
        ret

dC5:;push b
        push ax
        dec di
        mov bp,di
        mov ah,ch
        call save_byte
        dec di
        mov bp,di
        mov ah,cl
        call save_byte
        inc si
        pop ax
        mov bp,11
        ret

dD5:;push b
        push ax
        dec di
        mov bp,di
        mov ah,dh
        call save_byte
        dec di
        mov bp,di
        mov ah,dl
        call save_byte
        inc si
        pop ax
        mov bp,11
        ret

dE5:;push b
        push ax
        dec di
        mov bp,di
        mov ah,bh
        call save_byte
        dec di
        mov bp,di
        mov ah,bl
        call save_byte
        inc si
        pop ax
        mov bp,11
        ret

dF5:;push psw
        push ax
        dec di
        mov bp,di
        push ax
        xchg ah,al
        call save_byte
        pop ax
        dec di
        mov bp,di
        call save_byte
        inc si
        pop ax
        mov bp,11
        ret

dC7:;rst 0
        push ax
        inc si
        dec di
        mov bp,di
        mov ax,si
        call save_byte ; из AH уйдет старший байт
        dec di
        dec bp
        mov ax,si
        xchg al,ah
        call save_byte ; из AH уйдет младший байт
        mov si,0
        pop ax
        mov bp,11
        ret

dCF:;rst 1
        push ax
        inc si
        dec di
        mov bp,di
        mov ax,si
        call save_byte ; из AH уйдет старший байт
        dec di
        dec bp
        mov ax,si
        xchg al,ah
        call save_byte ; из AH уйдет младший байт
        mov si,8
        pop ax
        mov bp,11
        ret

dD7:;rst 2
        push ax
        inc si
        dec di
        mov bp,di
        mov ax,si
        call save_byte ; из AH уйдет старший байт
        dec di
        dec bp
        mov ax,si
        xchg al,ah
        call save_byte ; из AH уйдет младший байт
        mov si,10h
        pop ax
        mov bp,11
        ret

dDF:;rst 3
        push ax
        inc si
        dec di
        mov bp,di
        mov ax,si
        call save_byte ; из AH уйдет старший байт
        dec di
        dec bp
        mov ax,si
        xchg al,ah
        call save_byte ; из AH уйдет младший байт
        mov si,18h
        pop ax
        mov bp,11
        ret

dE7:;rst 4
        push ax
        inc si
        dec di
        mov bp,di
        mov ax,si
        call save_byte ; из AH уйдет старший байт
        dec di
        dec bp
        mov ax,si
        xchg al,ah
        call save_byte ; из AH уйдет младший байт
        mov si,20h
        pop ax
        mov bp,11
        ret

dEF:;rst 5
        push ax
        inc si
        dec di
        mov bp,di
        mov ax,si
        call save_byte ; из AH уйдет старший байт
        dec di
        dec bp
        mov ax,si
        xchg al,ah
        call save_byte ; из AH уйдет младший байт
        mov si,28h
        pop ax
        mov bp,11
        ret

dF7:;rst 6
        push ax
        inc si
        dec di
        mov bp,di
        mov ax,si
        call save_byte ; из AH уйдет старший байт
        dec di
        dec bp
        mov ax,si
        xchg al,ah
        call save_byte ; из AH уйдет младший байт
        mov si,30h
        pop ax
        mov bp,11
        ret

dFF:;rst 7
        push ax
        inc si
        dec di
        mov bp,di
        mov ax,si
        call save_byte ; из AH уйдет старший байт
        dec di
        dec bp
        mov ax,si
        xchg al,ah
        call save_byte ; из AH уйдет младший байт
        mov si,38h
        pop ax
        mov bp,11
        ret

dD3:;out port8
        mov ebp,dword[mem_fetch]
        cmp byte [use_io_space],0
        jz eD31
        push ax
        mov ah,[ebp+esi+1]
        call save_port
        pop ax
        jmp eD32
eD31:   push ax
        push bx
        mov bh,byte [ebp+esi+1]
        mov bl,bh
        mov bp,bx
        mov ah,al
        call save_byte
        pop bx
        pop ax
eD32:   inc si
        inc si
        mov bp,10
        ret

dDD:
dED:
dFD:
dCD:;call addr
        add si,3
        push ax
        mov ax,si
        dec di
        mov bp,di
        push ax
        call save_byte
        pop ax
        mov ah,al
        dec di
        mov bp,di
        call save_byte
        pop ax
        mov ebp,dword[mem_fetch]
        mov si,word [ebp+esi-2]
        mov bp,17
        ret

dCC:;сz addr
        sahf
        jz dCD
        add si,3
        mov bp,11
        ret

dDC:;сc addr
        sahf
        jc dCD
        add si,3
        mov bp,11
        ret

d0EC:;сpe addr
        sahf
        jpe dCD
        add si,3
        mov bp,11
        ret

dFC:;сm addr
        sahf
        js dCD
        add si,3
        mov bp,11
        ret

dC4:;сnz addr
        sahf
        jnz dCD
        add si,3
        mov bp,11
        ret

dD4:;сnc addr
        sahf
        jnc dCD
        add si,3
        mov bp,11
        ret

dE4:;сpo addr
        sahf
        jpo dCD
        add si,3
        mov bp,11
        ret

dF4:;сp addr
        sahf
        jns dCD
        add si,3
        mov bp,11
        ret

;#### С перехватом чтения ####

e0A:;ldax b
        mov [save_f],ah
        mov bp,cx
        call load_byte
        mov al,ah
        inc si
        mov ah,[save_f]
        mov bp,7
        ret

e1A:;ldax d
        mov [save_f],ah
        mov bp,dx
        call load_byte
        mov al,ah
        inc si
        mov ah,[save_f]
        mov bp,7
        ret

e3A:;lda addr
        mov ebp,dword[mem_fetch]
        mov [save_f],ah
        mov bp,word [ebp+esi+1]
        call load_byte
        mov al,ah
        add si,3
        mov ah,[save_f]
        mov bp,13
        ret

e46:;mov b,m
        push ax
        mov bp,bx
        call load_byte
        mov ch,ah
        inc si
        pop ax
        mov bp,7
        ret

e4E:;mov c,m
        push ax
        mov bp,bx
        call load_byte
        mov cl,ah
        inc si
        pop ax
        mov bp,7
        ret

e56:;mov d,m
        push ax
        mov bp,bx
        call load_byte
        mov dh,ah
        inc si
        pop ax
        mov bp,7
        ret

e5E:;mov e,m
        push ax
        mov bp,bx
        call load_byte
        mov dl,ah
        inc si
        pop ax
        mov bp,7
        ret

e66:;mov h,m
        push ax
        mov bp,bx
        call load_byte
        mov bh,ah
        inc si
        pop ax
        mov bp,7
        ret

e6E:;mov l,m
        push ax
        mov bp,bx
        call load_byte
        mov bl,ah
        inc si
        pop ax
        mov bp,7
        ret

e7E:;mov a,m
        mov [save_f],ah
        mov bp,bx
        call load_byte
        mov al,ah
        inc si
        mov ah,[save_f]
        mov bp,7
        ret

e86:;add m
        push cx
        mov cl,al
        inc si
        mov bp,bx
        call load_byte
        mov al,ah
        add al,cl
        lahf
        pop cx
        mov bp,7
        ret

e8E:;adc m
        push cx
        mov [save_f],ah
        mov cl,al
        inc si
        mov bp,bx
        call load_byte
        mov al,ah
        mov ah,[save_f]
        sahf
        adc al,cl
        lahf
        pop cx
        mov bp,7
        ret

e96:;sub m
        push cx
        mov cl,al
        inc si
        mov bp,bx
        call load_byte
        sub cl,ah
        mov al,cl
        lahf
        pop cx
        mov bp,7
        ret

e9E:;sbb m
        push cx
        mov [save_f],ah
        mov cl,al
        inc si
        mov bp,bx
        call load_byte
        mov al,ah
        mov ah,[save_f]
        sahf
        sbb cl,al
        mov al,cl
        lahf
        pop cx
        mov bp,7
        ret

eA6:;ana m
        push cx
        mov cl,al
        inc si
        mov bp,bx
        call load_byte
        mov al,ah
        and al,cl
        lahf
        pop cx
        mov bp,7
        ret

eAE:;xra m
        push cx
        mov cl,al
        inc si
        mov bp,bx
        call load_byte
        mov al,ah
        xor al,cl
        lahf
        pop cx
        mov bp,7
        ret

eB6:;ora m
        push cx
        mov cl,al
        inc si
        mov bp,bx
        call load_byte
        mov al,ah
        or al,cl
        lahf
        pop cx
        mov bp,7
        ret

eBE:;cmp m
        push cx
        push ax
        mov cl,al
        inc si
        mov bp,bx
        call load_byte
        mov al,ah
        sub cl,al
        pop ax
        pop cx
        lahf
        mov bp,7
        ret

eDB:;in port8
        mov ebp,dword[mem_fetch]
        mov [save_f],ah
        mov ah,byte [ebp+esi+1]
        cmp byte [use_io_space],0
        jz eDB1
        call load_port
        jmp eDB2
eDB1:   mov al,ah
        mov bp,ax
        call load_byte
        mov al,ah
eDB2:   inc si
        inc si
        mov ah,[save_f]
        mov bp,10
        ret

eC1:;pop d
        push ax
        mov bp,di
        mov bp,di
        call load_byte
        mov cl,ah
        inc di
        mov bp,di
        call load_byte
        mov ch,ah
        inc di
        inc si
        pop ax
        mov bp,10
        ret

eD1:;pop d
        push ax
        mov bp,di
        call load_byte
        mov dl,ah
        inc di
        mov bp,di
        call load_byte
        mov dh,ah
        inc di
        inc si
        pop ax
        mov bp,10
        ret

eE1:;pop h
        push ax
        mov bp,di
        call load_byte
        mov bl,ah
        inc di
        mov bp,di
        call load_byte
        mov bh,ah
        inc di
        inc si
        pop ax
        mov bp,10
        ret

eF1:;pop psw
        push cx
        mov bp,di
        call load_byte
        mov cl,ah
        inc di
        mov bp,di
        call load_byte
        mov al,cl
        xchg al,ah
        inc di
        inc si
        pop cx
        mov bp,10
        ret

eD9:
eC9:;ret
        push ax
        push bx
        mov bp,di
        call load_byte
        mov bl,ah
        inc di
        mov bp,di
        call load_byte
        mov bh,ah
        inc di
        mov si,bx
        pop bx
        pop ax
        mov bp,11
        ret

eC8:;rz
        sahf
        jz eC9
        inc si
        mov bp,5
        ret

eD8:;rc
        sahf
        jc eC9
        inc si
        mov bp,5
        ret

eE8:;rpe
        sahf
        jpe eC9
        inc si
        mov bp,5
        ret

eF8:;rm
        sahf
        js eC9
        inc si
        mov bp,5
        ret

eC0:;rnz
        sahf
        jnz eC9
        inc si
        mov bp,5
        ret

eD0:;rnc
        sahf
        jnc eC9
        inc si
        mov bp,5
        ret

eE0:;rpo
        sahf
        jpo eC9
        inc si
        mov bp,5
        ret

eF0:;rp
        sahf
        jns eC9
        inc si
        mov bp,5
        ret

end
