/*
 *--------------------------------------------------------------------------
 * Copyright (C) 2013, Joe Mruz
 * Contact email: mruz.joe@gmail.com
 * 
 * This file is part of C32-EMU.
 *
 * C32-EMU is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * C32-EMU is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with C32-EMU.  If not, see <http://www.gnu.org/licenses/>.
 *--------------------------------------------------------------------------
 */
/*
 *--------------------------------------------------
 * File: emu.h
 * Description:
 *--------------------------------------------------
 */

#ifndef _EMU_H_
#define _EMU_H_

#include <stdint.h>
#include "emu_defs.h"

/*
 *--------------------------------------------------
 * constants
 *--------------------------------------------------
 */
#define EMU_VERSION             "0.1a"

#define BOOT_SECTOR_SIZE		(1024 * 64)			/*64 Kbytes*/

/* 
 * 1 MB, can only address 2^20 absolute 
 * addresses in one instruction 
 */
#define MEMORY_SIZE				(1024 * 1024)
#define DISK_SIZE				(1024 * 1024 * 30)	/*30 MB*/
#define RAM_DISK_BUFFER_SIZE	(1024)				/*1 KB*/
#define DISPLAY_ROWS			(40)
#define DISPLAY_COLS			(80)
#define DISPLAY_BUFFER_SIZE		(80 * 40)			/*240 bytes*/
#define KEYBOARD_BUFFER_SIZE	(6)
#define DISK_FILENAME			"disk.dat"
#define ROM_FILENAME			"rom.dat"
#define MONOGREEN               1
#define MONOAMBER               2
#define MONOWHITE               0

/*
 *--------------------------------------------------
 * objects
 *--------------------------------------------------
 */

/* cpu structure */
struct cpu_s
{
    uint8_t         state;                  /* internal cpu state */
    uint8_t         adr_mode;				/* numbers of bits addressed */
    uint32_t        registers [REGISTERS];  /* cpu registers */
    uint32_t        ports [MAX_PORTS];
    uint32_t        cycle;
};

/* ram structure */
struct ram_s
{
	uint8_t         address [MEMORY_SIZE];
    struct instr_s 	*instruction;
};

/* disk structure */
struct disk_s
{
    uint8_t     address [DISK_SIZE];
    uint32_t    mapped_buffer_size;
    uint8_t     state;
    uint32_t    seek;
    uint32_t	mapped_buffer_start;
};

/* clock structure */
struct clk_s
{
    uint32_t    quantum;
    uint32_t    address;
    uint8_t     state;
    uint32_t    prv_tm;
};

/* display monitor structure */

typedef struct disp_hwcurs_s
{
    uint16_t    pos;
    uint8_t     enabled;
    uint8_t     shape;
} disp_hwcursor_st;

struct disp_s
{
    uint8_t             buffer [DISPLAY_BUFFER_SIZE];
    uint32_t            buffer_map_adr;
    uint8_t             state;
    disp_hwcursor_st    cursor;
};


/*
 *--------------------------------------------------
 * data types
 *--------------------------------------------------
 */

typedef struct cpu_s     cpu_t;
typedef struct ram_s     ram_t;
typedef struct disk_s    disk_t;
typedef struct disp_s    disp_t;
typedef struct clk_s     clk_t;

/*
 *--------------------------------------------------
 * global variables
 *--------------------------------------------------
 */

extern cpu_t    Cpu [];
extern disp_t   Display [];
extern int      Emu_run;
extern int		Emu_dbg_option;
extern int      Emu_refresh_rate;
extern int      Emu_max_speed;
extern char     Disk_filn [];
extern char     Rom_filn [];
extern int      Boot_disk;
extern uint32_t Kb_buffer[];
extern int      Text_color;

/*
 *--------------------------------------------------
 * functions
 *--------------------------------------------------
 */

void emu_init(void);
void emu_main_loop(void);
void emu_rd_ports(void);
void emu_kb_hndl(uint32_t *keycode);
void cpu_init(cpu_t *cpu);
void cpu_process(cpu_t *cpu, ram_t *ram);
void cpu_chk_tmr(cpu_t *cpu, ram_t *ram);
void ram_init(ram_t *ram);
void disk_init(disk_t *disk);
void disk_rd_port(disk_t *disk, uint32_t *dat, ram_t *ram);
void disk_get_bootsector(disk_t *disk, ram_t *ram, uint32_t num_bytes);
void disk_load(disk_t *disk, const char *filename);
void disk_save(disk_t *disk, const char *filename);
void disp_init(disp_t *disp);
void disp_rd_port(disp_t *disp, uint32_t *dat, ram_t *ram);
void disp_rd_adr_map(disp_t *disp, ram_t *ram);
void clk_init(clk_t *clk);
void clk_rd_port(clk_t *clk, uint32_t *dat);
void clk_chk_tmr(clk_t *clk, cpu_t *cpu, ram_t *ram);
void clk_get_tv_usec(uint32_t *tv);

#endif  /* _EMU_H_ */
