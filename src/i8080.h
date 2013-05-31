// Intel 8080 (KR580VM80A) microprocessor core object model
//
// Copyright (C) 2012 Alexander Demin <alexander@demin.ws>
// Portions Copyright (C) 2013 Viktor Pykhonin <pyk@mail.ru>
//
// Credits
//
// Viacheslav Slavinsky, Vector-06C FPGA Replica
// http://code.google.com/p/vector06cc/
//
// Dmitry Tselikov, Bashrikia-2M and Radio-86RK on Altera DE1
// http://bashkiria-2m.narod.ru/fpga.html
//
// Ian Bartholomew, 8080/8085 CPU Exerciser
// http://www.idb.me.uk/sunhillow/8080.html
//
// Frank Cringle, The origianal exerciser for the Z80.
//
// Thanks to zx.pk.ru and nedopc.org/forum communities.
//
// This program is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 2, or (at your option)
// any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

#ifndef I8080_H
#define I8080_H

#include <stdint.h>

typedef union {
    struct {
        uint8_t l, h;
    } b;
    uint16_t w;
} reg_pair;

typedef struct {
    uint8_t carry_flag;
    uint8_t unused1;
    uint8_t parity_flag;
    uint8_t unused3;
    uint8_t half_carry_flag;
    uint8_t unused5;
    uint8_t zero_flag;
    uint8_t sign_flag;
} flag_reg;

struct i8080 {
    flag_reg f;
    reg_pair af, bc, de, hl;
    reg_pair sp, pc;
    uint16_t iff;
    uint16_t last_pc;
};

const int parity_table[] = {
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    0, 1, 1, 0, 1, 0, 0, 1, 1, 0, 0, 1, 0, 1, 1, 0,
    1, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0, 1, 0, 0, 1,
};

const int half_carry_table[] = { 0, 0, 1, 0, 1, 0, 1, 1 };
const int sub_half_carry_table[] = { 0, 1, 1, 1, 0, 0, 0, 1 };

class TCPU8080// : public TCPU
{
public:
    void i8080_init();
    int i8080_instruction();

    void i8080_jump(uint16_t addr);

    uint16_t i8080_pc();

    uint16_t i8080_regs_bc();
    uint16_t i8080_regs_de();
    uint16_t i8080_regs_hl();
    uint16_t i8080_regs_sp();

    uint16_t i8080_regs_a();
    uint16_t i8080_regs_b();
    uint16_t i8080_regs_c();
    uint16_t i8080_regs_d();
    uint16_t i8080_regs_e();
    uint16_t i8080_regs_h();
    uint16_t i8080_regs_l();

private:
    struct i8080 cpu;

    uint32_t work32;
    uint16_t work16;
    uint8_t work8;
    int index;
    uint8_t carry, add;

    void i8080_store_flags();
    void i8080_retrieve_flags();
    int i8080_execute(int opcode);
}

#endif
