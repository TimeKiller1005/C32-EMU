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
 * File: emu.c
 * Description:
 *--------------------------------------------------
 */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "emu.h"
#include "emu_sdl.h"
#include "emu_utils.h"

/*
 *--------------------------------------------------
 * Global variables
 *--------------------------------------------------
 */
cpu_t       Cpu [1];
ram_t       Ram [1];
disk_t      Disk [1];
disk_t      Rom [1];
disp_t      Display [1];
clk_t       Clock [1];
int         Emu_run;
uint32_t    Kb_buffer [KEYBOARD_BUFFER_SIZE];


/*
 *--------------------------------------------------
 * Functions
 *--------------------------------------------------
 */
void emu_dbg_txt(void);

/*
 *--------------------------------------------------
 * Function: emu_init
 * Description:	
 *		initialize emulator objects
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void
emu_init(void)
{
	sdl_init ();
    cpu_init (Cpu);
    ram_init (Ram);
    disk_init (Disk);
    disk_init (Rom);
    disp_init (Display);
    clk_init (Clock);

    memset (Kb_buffer, 0, sizeof (Kb_buffer));

    printf ("loading disk...\n");
    disk_load (Disk, Disk_filn);

    if (Rom_filn[0])
    {
        printf ("loading rom...\n");
        disk_load (Rom, Rom_filn);
    }
    
    /* check boot device */
    if (Boot_disk)
    {
       disk_get_bootsector (Disk, Ram, BOOT_SECTOR_SIZE);
    }
    else
        disk_get_bootsector (Rom, Ram, BOOT_SECTOR_SIZE);
    
    Emu_run = 1;
}

/*
 *--------------------------------------------------
 * Function: emu_main_loop
 * Description:	
 *		application loop
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void
emu_main_loop(void)
{
    Emu_run = sdl_event_hndl ();
    
    /* 
     * This is an optimization...
     * we should not have to draw the screen before
     * every cycle 
     */
    if (Cpu->cycle % Emu_refresh_rate == 0 &&
        Cpu->state != STATE_HALT) 
    {
        sdl_clear ();
        sdl_draw (Display->buffer);
		if (Emu_dbg_option) 
            emu_dbg_txt ();
		sdl_flip ();

        if ((!Emu_max_speed)) 
            sdl_sleep ();
    }

    if (Cpu->state != STATE_HALT)
    {
    	emu_rd_ports ();
        cpu_process (Cpu, Ram);
    }
    else
    {
        sdl_clear ();
        sdl_draw_info ();
        sdl_flip ();
        sdl_sleep ();
    }

    /*todo: can be removed when replaced with hardware clock*/
    /*cpu_chk_tmr (Cpu, Ram);*/
    clk_chk_tmr (Clock, Cpu, Ram);
       
    if (!Emu_run)
    {
        printf ("cleaning up...\n");
        sdl_quit ();
        printf ("saving %s...\n", Disk_filn);
        disk_save (Disk, Disk_filn); 
    }
}

/*
 *--------------------------------------------------
 * Function: emu_rd_ports
 * Description:	
 *		read, update simulated hardware I/O ports
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void
emu_rd_ports(void)
{
	disk_rd_port(Disk, &(Cpu->ports [PORT_DISK]), Ram);
    disk_rd_port(Rom, &(Cpu->ports [PORT_ROM]), Ram);
	disp_rd_port(Display, &(Cpu->ports [PORT_DISPLAY]), Ram);
    clk_rd_port(Clock, &(Cpu->ports [PORT_CLOCK]));
}

/*
 *--------------------------------------------------
 * Function: emu_kb_hndl
 * Description:	
 *		Handle the keyboard buffer.
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void
emu_kb_hndl(uint32_t *keycode)
{
    int i;

    /* if the keyboard port is 0, set to next item */
    if (! Cpu->ports [PORT_KEYBOARD] &&
            Kb_buffer[0])
    {
        Cpu->ports [PORT_KEYBOARD] = Kb_buffer [0];
        Kb_buffer[0] = 0;
        
        /* advance the queue */
        for (i = 1; i < KEYBOARD_BUFFER_SIZE; i++)
        {   
            Kb_buffer [i - 1] = Kb_buffer [i];
        }
        Kb_buffer [KEYBOARD_BUFFER_SIZE - 1] = 0;
    }

    /* if keycode read is non-zero, add to end of buffer */
    for(i = 0; i < KEYBOARD_BUFFER_SIZE && *keycode; i++)
    {
        if (! Kb_buffer [i])
        {
            Kb_buffer [i] = *keycode;

            if (i != KEYBOARD_BUFFER_SIZE - 1)
                Kb_buffer [i + 1] = 0;

            break;
        }
    }
}

/*
 *--------------------------------------------------
 * Function: emu_dbg_txt
 * Description:	
 *		Display debug text to the emulator display
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void
emu_dbg_txt(void)
{
    char buffer [32];
    unsigned int i;
    uint32_t tmp, tmp1;
	
	sdl_render_box (530, 90, 275, 260);

    for (i = 0; i < REGISTERS; i++)
    {
        memset (buffer, 0, sizeof (buffer));
        sprintf (buffer, "0x%x", Cpu->registers [i]);
        sdl_render_text ((void *)buffer, 670, 110+(i*10));
    }

    for (i = 0; i < MAX_PORTS; i++)
    {
        memset (buffer, 0, sizeof (buffer));
        sprintf (buffer, "0x%x", Cpu->ports [i]);
        sdl_render_text ((void *)buffer, 630, 110+((i+REGISTERS)*10)+10);
    }

    memset (buffer, 0, sizeof (buffer));
    sprintf (buffer, "0x%x", Cpu->state);
    sdl_render_text ((void *)buffer, 630, 330);

    sdl_render_text ((void *)"REGISTERS",630,100);   
    sdl_render_text ((void *)"A",       630, 110);
    sdl_render_text ((void *)"B",       630, 120);
    sdl_render_text ((void *)"C",       630, 130);
    sdl_render_text ((void *)"X",       630, 140);
    sdl_render_text ((void *)"Y",       630, 150);
    sdl_render_text ((void *)"Z",       630, 160);
    sdl_render_text ((void *)"CMP",     630, 170);
    sdl_render_text ((void *)"P",       630, 180);
    sdl_render_text ((void *)"TQ",      630, 190);
    sdl_render_text ((void *)"TA",      630, 200);
    sdl_render_text ((void *)"ACC",     630, 210);
    sdl_render_text ((void *)"OFS",     630, 220);
    sdl_render_text ((void *)"SB",      630, 230);
    sdl_render_text ((void *)"SP",      630, 240);
    sdl_render_text ((void *)"PC",      630, 250);
    sdl_render_text ((void *)"0",       630, 260);

    sdl_render_text ((void *)"DISP",    570, 280);
    sdl_render_text ((void *)"KEY",     570, 290);
    sdl_render_text ((void *)"DISK",    570, 300);
    sdl_render_text ((void *)"ROM",     570, 310);

    sdl_render_text ((void *)"STA",   570, 330);

    i = 0;
    for(tmp = Cpu->registers [REGISTER_STACKBASE]; 
			tmp >= Cpu->registers [REGISTER_STACKPOINTER]; 
			tmp-=4)
    {
        memset (buffer, 0, sizeof (buffer));
        utils_load (Ram, &tmp1, tmp, 32);
        sprintf (buffer, "0x%lx", (unsigned long)tmp1);
        sdl_render_text ((void *)buffer, 540, 110 + (i*10));
        ++ i;
    }
    
    sdl_render_text((void *)"STACK",    540, 100);
}
