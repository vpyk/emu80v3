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

// asm.h

#ifndef _E_ASM_H
#define _E_ASM_H

#include "emu80.h"

extern "C" void setup();

extern "C" void query();
extern "C" void getfilename();
extern "C" void ex_int_09(short wScanCode);

//extern "C" void InitOnce();            // процедура иниц-ии при запуске
extern "C" unsigned char *mempages[4];   // память эмулируемого ПК (4 стр.)
extern "C" unsigned char *mempage0;      // память эмулируемого ПК (0 стр.)
extern "C" unsigned char *mempage1;      // память эмулируемого ПК (1 стр.)
extern "C" unsigned char *mempage2;      // память эмулируемого ПК (2 стр.)
extern "C" unsigned char *mempage3;      // память эмулируемого ПК (3 стр.)
extern "C" unsigned char *mempage4;      // память эмулируемого ПК (4 стр.)
extern "C" unsigned char *color_mem;     // память цвета "Специалиста"
extern "C" unsigned char *romdisk_mem;   // память
extern "C" unsigned char *sym_gen;       // знакогенератор
extern "C" char color_table[4];        // таблица соответствия цветов
extern "C" char or_color4_table[4];
extern "C" void Reset();
extern "C" void Emulate(int nCycles);
extern "C" int ProcessQuery();
extern "C" int PrepareScreen();
extern "C" void Init();                // иниц-ия при смене типа ПК
//extern "C" void LoadROM();             // загрузка соответствующего ПЗУ
//extern "C" void ClearMem();            // очистка памяти сегмента 8080
extern "C" void SetDefParams();        // устанавливает параметры по умолчанию
extern "C" void FlushKeys();

extern "C" MENU_LINE f_speed;
extern "C" MENU_LINE f_scr;
extern "C" MENU_LINE f_snd;
extern "C" MENU_LINE f_tape;

extern "C" MENU_ITEM i_spmed;
extern "C" MENU_ITEM i_scrauto;

extern "C" unsigned char use_io_space; // используются ли команды IN/OUT
extern "C" unsigned short scr_beg;           // начало экрана
extern "C" unsigned short scr_end;           // конец экрана
extern "C" unsigned short cur_corr;          // текущая коррекция чего-то
extern "C" unsigned short dma_begadr;        // начало экрана (по ПДП)
extern "C" unsigned short dma_len;           // размер экрана -1 (по ПДП)
 extern "C" unsigned short romvar_addr;       // адрес переменной с адресом поз. курс.
 extern "C" unsigned short curpos_addr;       // адрес переменной с коорд. курсора
extern "C" unsigned short cur_speed_sb;      // текущая задержка
extern "C" unsigned short delay_sb;          // текущая задержка (не запом.)
extern "C" unsigned short delay_sb_dma; // тактов на 1/44100 с учетом ПДП
extern "C" unsigned short delay_sb_norm;// тактов на 1/44100 без учета ПДП
extern "C" unsigned short rom_len;           // длина ПЗУ
extern "C" unsigned short rom_adr;           // нач. адрес ПЗУ
extern "C" unsigned short reset_addr;        // стартовый адрес
extern "C" unsigned char keybin_port;  // порт ввода с клавиатуры
extern "C" unsigned short ppi_portc_adr;     // адрес порта C ППА
extern "C" unsigned short crt_port1_adr;     // адрес регистра управления ВГ75
extern "C" char * romname;        // имя файла с ПЗУ
extern "C" void (*hook_proc_sb)();       // текущая фунуция перехв. - Sound Blast.
extern "C" void hook_p_sb();           // функция перехвата для "Партнера"
extern "C" void hook_r_sb();           // функция перехвата для РК
extern "C" void hook_m_sb();           // функция перехвата для "Микроши"
extern "C" void hook_a_sb();           // функция перехвата для "Апогея"
extern "C" void hook_s_sb();           // функция перехвата для "Специалиста"
extern "C" void hook_o_sb();           // функция перехвата для "Ориона"
extern "C" void hook_m80_sb();         // функция перехвата для "Микро-80"
extern "C" void hook_u_sb();           // функция перехвата для "ЮТ-88"
extern "C" void corr_key_tbl();        // настраивает таблицы клав. под модель
//extern "C" unsigned char ext_letter;   // последняя буква расширения RKx
extern "C" void set_new_09_vect();       //
extern "C" void restore_09_vect();       //
extern "C" void AfterLoadRom();       //

extern "C" unsigned short sh_scrbeg;   // начало экранной области
extern "C" unsigned char sh_height;    // количество строк
extern "C" unsigned char sh_width;     // количество символов в строке
extern "C" unsigned short sh_scrlen;   // длина экранной области
extern "C" unsigned char crt_param_1;  // параметр 1 программирования ВГ75
extern "C" unsigned char crt_param_2;  // параметр 2 программирования ВГ75
extern "C" unsigned char crt_param_3;  // параметр 3 программирования ВГ75
extern "C" unsigned char crt_param_4;  // параметр 4 программирования ВГ75
extern "C" unsigned char dma_mr;       // регистр режима ВТ57
extern "C" unsigned char crt_mreg;     // регистр режима ВГ75
extern "C" unsigned char mikr_symg;    // тек. знакогенератор "Микроши"
extern "C" unsigned char port_c;       // порт C ВВ55
extern "C" unsigned char int_flag;     // признак разрешения прерываний
extern "C" unsigned char cur_x;        // координата X курсора
extern "C" unsigned char cur_y;        // координата Y курсора
extern "C" unsigned short cur_xy;      // координаты XY курсора
extern "C" unsigned char or_color_mode;// текущий режим экрана "Ориона"
extern "C" unsigned char mem_page_no;  // текущая страница памяти "Ориона"
extern "C" unsigned char or_scrpage;   // текущий экран "Ориона"
//extern "C" unsigned char f_chscr;      // 1, если нужно перепрогр. экран
extern "C" unsigned char f_saverom;    // 1, если нужно сохранять ПЗУ в снапш.
extern "C" unsigned short ticks_per_44100th; // тактов на 1/44100-ю секунды

extern "C" void getfilename();         // запрос имени файла
extern "C" void enable_ints();         // разрешение прерываний
extern "C" void disable_ints();        // запрещение прерываний
extern "C" void process_crt();         // прогр. экрана в сотв. с настройками
extern "C" void process_pit();         // прогр. таймера в сотв. с настройками
extern "C" void process_port_c();      // интерпр. содерж. порта C ППА
extern "C" void process_orion();       // обработка пар. Ориона при загр. снапш.
extern "C" char filename;              // введенное имя файла (1-й символ)
extern "C" char f_choose_file;         // 1, если выбираем файл
extern "C" char f_file_ss;             // 1, если файл снапшота, 2-ODI
extern "C" char f_synb;
//extern "C" char f_novesa;              // 1, если не используем SVGA (VESA)
//extern "C" char f_led;                 // 1, работаем со светодиодами клавиатуры
//extern "C" char f_use8x12;             // 1, исп. шрифт 8x12 и режим 80x40
//extern "C" unsigned short wSBPort;     // базовый адрес портов SB
//extern "C" unsigned short wSBDMA;      // канал ПДП для SB
extern "C" char need_clear_mem;          // 1, если нужно очищ. пам. при выб. ПК
//extern "C" char com_path;              // путь к com-файлу
//extern "C" char com_path_len;          // длина пути к com-файлу
//extern "C" unsigned short vmem_off;    // смещ. в видеопам. экрана Спец. 800x600
extern "C" char f_quit;                  //

extern "C" unsigned char snd_state;                  //

extern "C" unsigned short reg_af;      // регистр AF
extern "C" unsigned short reg_bc;      // регистр BC
extern "C" unsigned short reg_de;      // регистр DE
extern "C" unsigned short reg_hl;      // регистр HL
extern "C" unsigned short reg_sp;      // регистр SP
extern "C" unsigned short reg_pc;      // регистр PC

#endif
