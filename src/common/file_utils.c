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
 * File: file_utils.c
 * Description:
 *  File read & write functions
 *
 * author: joe mruz
 *--------------------------------------------------
 */

#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdint.h>
#include "file_utils.h"

/*
 *--------------------------------------------------
 * Functions
 *--------------------------------------------------
 */

/*
 *--------------------------------------------------
 * Function: file_utils_read
 * Description:
 *		Read from file
 * Params:
 * Returns: void
 *--------------------------------------------------
 */
int 
file_utils_read(char *buffer, size_t size, const char *filename)
{
    FILE *fd;

    /* check if file exists, if not create it */
    fd = fopen (filename, "r");
    if (fd)
        fclose (fd);
    else {
        printf ("failed to open: %s\n", filename);
        exit(0);
		/*disabled creation is not found*/
#if 0
        file_utils_write (buffer, size, filename);
        printf("creating %s\n",filename);
#endif
    }
    fd = fopen (filename, "rb");
    
    if(!fd)
        return 0;

    fseek (fd, 0L, SEEK_END);
    unsigned long sz = ftell (fd);
    fseek (fd, 0L, SEEK_SET);
    fread (buffer, sz, 1, fd);
    fclose (fd);

    return 1;
}

/*
 *--------------------------------------------------
 * Function: file_utils_write
 * Description:
 *		Write to file
 * Params:
 * Returns: int
 *--------------------------------------------------
 */
int 
file_utils_write(char *buffer, size_t size, const char *filename)
{
    FILE *fd;
    
    fd = fopen (filename, "wb");
    if(!fd)
    {
        return 0;
    }
    
    fwrite (buffer, sizeof(uint8_t), size, fd);
    fclose (fd);
    
    return 1;
}
