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
 * File: main.c
 * Description: The entry point for the emulator
 *--------------------------------------------------
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "emu.h"
#include "emu_sdl.h"

/*
 *--------------------------------------------------
 * constants
 *--------------------------------------------------
 */
#define MAX_FILENAME_SIZE   (128)

/*
 *--------------------------------------------------
 * global variables
 *--------------------------------------------------
 */
int Emu_dbg_option;
int Emu_refresh_rate;
int Emu_max_speed;
int Opt_disk;
int Boot_disk;
int Text_color;

/*
 *--------------------------------------------------
 * Functions
 *--------------------------------------------------
 */
void runtime_options(int argc, char **argv);
void print_help(void);

char Disk_filn [MAX_FILENAME_SIZE];
char Rom_filn [MAX_FILENAME_SIZE];

/*
 *--------------------------------------------------
 * Function: runtime_options
 * Description:	
 *		Process command line options
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void 
runtime_options(int argc, char **argv)
{
    int i;

    for (i = 1; i < argc; i++)
    {
        if (!strncmp ("-disk", argv [i], sizeof (argv [i])))
        {
            ++i;
            if(!argv [i])
            {
                printf ("Error: filename not provided.\n");
                print_help ();
                exit (2);
            }
            strncpy (Disk_filn, argv [i], sizeof (Disk_filn));
            ++Opt_disk;
        }
        else if (!strncmp ("-rom", argv [i], sizeof (argv [i])))
        {
            ++i;
            if (!argv [i])
            {
                printf ("Error: filename not provided.\n");
                print_help ();
                exit (2);
            }
            strncpy (Rom_filn, argv [i], sizeof (Disk_filn));
        }
        else if (!strncmp ("-color", argv [i], sizeof (argv [i])))
        {
            ++i;
            if (!argv [i])
            {
                printf ("Error: Color name required.\n");
                print_help ();
                exit (2);
            }
            if (!strncmp ("green", argv [i], sizeof (argv [i])))
            {
                Text_color = MONOGREEN;
            }
            else if (!strncmp ("amber", argv [i], sizeof (argv [i])))
            {
                Text_color = MONOAMBER;
            }
            else if (!strncmp ("red", argv [i], sizeof (argv [i])))
            {
                Text_color = MONORED;
            }
            else
            {
                Text_color = MONOWHITE;
            }
        }
        else if (!strncmp ("-boot_rom", argv [i], sizeof (argv [i])))
        {
            --Boot_disk;
        }
        else if (!strncmp ("-fast", argv [i], sizeof (argv [i])))
        {
            ++Emu_max_speed;
            Emu_refresh_rate *= 4;
        }
        else if (!strncmp ("-slow", argv [i], sizeof (argv [i])))
        {
            Emu_refresh_rate = 1;
        }
        else if (!strncmp ("-debug", argv [i], sizeof (argv [i])))
        {
            ++Emu_dbg_option;
        }
        else if (!strncmp ("-h", argv [i], sizeof (argv [i])) ||
                 !strncmp ("--help", argv [i], sizeof (argv [i])) ||
                 !strncmp ("-help", argv [i], sizeof (argv [i])))
        {
            print_help ();
            exit (2);
        }
        else
        {
            printf ("Unrecognized option: %s\n", argv [i]);
            print_help ();
            exit (2);
        }
    }

    if (!Opt_disk)
    {
        printf ("Error: Must specify a disk file\n");
        print_help ();
        exit (2);
    }
    if (!Boot_disk &&
        !Rom_filn [0])
    {
        printf ("Must provide rom file to boot\n");
        exit (2);
    }
}

/*
 *--------------------------------------------------
 * Function: print_help
 * Description:	
 *		Print help text
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void print_help(void)
{
    printf ("=========================================================\n");
    printf ("C32-EMU, Emulator for the Custom 32-bit RISC architecture\n");
    printf ("Copyright (C) 2013, Joe Mruz\n");
    printf ("v"EMU_VERSION"\n");
    printf ("=========================================================\n");
    printf ("valid options:\n\n");
    printf ("-disk <disk filename>   -- specify the secondary storage\n");
    printf ("-rom <rom filename>     -- specify the read-only memory\n");
    printf ("-boot_rom               -- boot from the rom instead of disk\n");
    printf ("-fast                   -- use 100 percent CPU to speed up emulation\n");
    printf ("-slow                   -- set instruction execution speed equal to the FPS\n");
    printf ("-debug                  -- enable on-screen debug text (can toggle with F1)\n");
    printf ("-color                  -- specify text color (green, amber, white)\n");
    printf ("-help                   -- print this dialog\n");
    printf ("\nusage: emu [-fast] [-slow] [-debug] [-boot_rom] -disk <filename> [-rom <filename>] [-color <name>] [-help]\n");
}

/*
 *--------------------------------------------------
 * Function: main
 * Description:	
 *		the main function
 * Params:
 * Returns: int
 *--------------------------------------------------
 */
int 
main(int argc, char **argv)
{
    Emu_max_speed       = 0;
    Emu_refresh_rate    = FPS;
    Emu_dbg_option      = 0;
    Opt_disk            = 0;
    Boot_disk           = 1;
    Text_color          = MONOWHITE;

    memset (Disk_filn, 0, sizeof (Disk_filn));
    memset (Rom_filn, 0, sizeof (Rom_filn));

    runtime_options (argc, argv);

    printf ("C32-EMU v"EMU_VERSION"\n");
    printf ("Copyright (C) 2013, Joe Mruz\n");
    printf ("=============================\n");
    
    printf ("Initializing everything...\n");
    emu_init ();

    printf ("Starting...\n");
    while (Emu_run)
        emu_main_loop ();
    
    return 0;
}
