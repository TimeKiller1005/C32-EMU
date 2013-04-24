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
 * File: emu_disk.c
 * Description: Simulates functionality of disk I/O device
 *--------------------------------------------------
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "emu.h"
#include "file_utils.h"

/*
 *--------------------------------------------------
 * constants
 *--------------------------------------------------
 */
#define DISK_ERROR_CODE 	0xFFFFFFFE
#define DISK_NEXT_STATE		0xFFFFFFFC
#define DISK_NOOP_CODE		0xFFFFFFFD
#define DISK_WRITE_CODE		0xFFFFFFFF

enum disk_state
{
	DISK_READ_BUFFER_START,
	DISK_READ_BUFFER_SIZE,
	DISK_READ_REQUEST,
	DISK_HOLD
};

/*
 *--------------------------------------------------
 * functions
 *--------------------------------------------------
 */

/*
 *--------------------------------------------------
 * Function: disk_init
 * Description:	
 *		Initialize the disk object
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void
disk_init(disk_t* disk)
{
	memset (disk->address, 0, sizeof (disk->address));
	
	disk->mapped_buffer_size 	= 0;
	disk->mapped_buffer_start 	= 0;
	disk->state 				= DISK_READ_BUFFER_START;
	disk->seek 					= 0;
}

/*
 *--------------------------------------------------
 * Function: disk_rd_port
 * Description:	
 *		Read/update simulated disk I/O port
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void 
disk_rd_port(disk_t *disk, uint32_t *dat, ram_t *ram)
{
	unsigned int i;

    switch (disk->state)
	{
		case DISK_READ_BUFFER_START:

			/*
			 * STATE 0:
			 * the next non zero value on the port will be saved 
			 * as the ram address the disk buffer will start at, 
			 * then write zero to the port
			 */
			if (*dat == DISK_NEXT_STATE) {
				disk->state = DISK_READ_BUFFER_SIZE;
			}
			else if ( (*dat) || *dat != DISK_ERROR_CODE)
			{
				if(*dat >= MEMORY_SIZE)
				{
					*dat = DISK_ERROR_CODE;
					break;
				}
				disk->mapped_buffer_start = *dat;
			}
            *dat = 0;
			break;
		case DISK_READ_BUFFER_SIZE:

			/*
			 * STATE 1:
			 * the next non zero value on the port will be saved 
			 * as the size of disk buffer, 
			 * then write zero to the port
			 * */
			if (*dat == DISK_NEXT_STATE) {
				disk->state = DISK_READ_REQUEST;
                *dat = DISK_NOOP_CODE;
			}
			else if ((*dat) || *dat != DISK_ERROR_CODE)
			{
				if ((*dat) + disk->mapped_buffer_start >= MEMORY_SIZE)
				{
					*dat = DISK_ERROR_CODE;
					break;
				}
				disk->mapped_buffer_size = *dat;
                *dat = 0;
			}
			break;
		case DISK_READ_REQUEST:
			
			/*
			 * STATE 2:
			 * read/write/request state (0xFFFFFFC will go to state 0)
			 */
			switch (*dat)
			{
				case DISK_NEXT_STATE: disk->state = DISK_HOLD; break;
				case DISK_WRITE_CODE: 
					
					/* 
					 * write the mapped region of RAM to the last seeked block
					 */
					for(i = 0; i < disk->mapped_buffer_size; i++)
					{
						disk->address [disk->seek + i] = 
									ram->address [disk->mapped_buffer_start + i];
					}
					*dat = DISK_NOOP_CODE;
					break;
				case DISK_ERROR_CODE:	/*error, do nothing (we generated this)*/ break;
				case DISK_NOOP_CODE:	/*do nothing*/ break;
				case DISK_HOLD: break;
				default:
					/*
					 * seek for data, write to mapped region of RAM
					 * if the seek is outside the size of the disk, 
					 * put error code on the port
					 */
					if (*dat >= DISK_SIZE) 
					{
                        printf ("Error: disk seek request greater than size of disk");
						*dat = DISK_ERROR_CODE;
						return;
					}
					
					disk->seek = *dat;
					
					for (i = 0; i < disk->mapped_buffer_size && 
								i < DISK_SIZE; 
								i++)
					{
						ram->address [disk->mapped_buffer_start + i] = disk->address [disk->seek + i];
					}
					*dat = DISK_NOOP_CODE;
					break;
			}
            break;
        case DISK_HOLD:
            
			/* 
			 * STATE 3: 
			 * hold state, advancing the state will return to state 0 
			 * (useful for initiating a restart)
			 */
            if(*dat == DISK_NEXT_STATE) {
                disk->state = DISK_READ_BUFFER_START;
            }
            *dat = 0;
            break;
		default: disk->state = DISK_READ_BUFFER_START; break;
	}
}

/*
 *--------------------------------------------------
 * Function: disk_get_bootsector
 * Description:	
 *		Copy the bootsector from disk to memory
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void 
disk_get_bootsector(disk_t *disk, ram_t *ram, uint32_t num_bytes)
{
    memcpy ((void *)ram->address, (void *)disk->address, num_bytes);
}

/*
 *--------------------------------------------------
 * Function: disk_load
 * Description:	
 *		Load disk from file
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void 
disk_load(disk_t *disk, const char *filename)
{
    if (!file_utils_read ((void *)disk->address, 
                          sizeof (disk->address), 
                          filename))
    {
        printf ("Error: Could not open "DISK_FILENAME);
        exit(0);
    }
}

/*
 *--------------------------------------------------
 * Function: disk_save
 * Description:	
 *		Save disk to file
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
void 
disk_save(disk_t *disk, const char *filename)
{
    if (!file_utils_write ((void *)disk->address,
                           sizeof (disk->address),
                           filename))
    {
        printf ("Error: Could not open "DISK_FILENAME);
    }
}
