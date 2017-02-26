/*
 * vdesktop.h : Virtual Desktops Header
 *
 * ------------------------------------------------------------------
 *
 * @begin[license]
 * Copyright (C) 2003  Kai Chen, Caltech
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 * @author: Kai Chen
 * @email{kchen@cs.caltech.edu}
 * @end[license]
 *
 */

/* number of virtual desktops */
#define NUM_VDS             4

#define CMD_BUFFER_SIZE     80

/* address of vdesktops */
#define VD0_ADDR            0x6C000
#define VD1_ADDR            0x6D000
#define VD2_ADDR            0x6E000
#define VD3_ADDR            0x6F000


/* virtual desktop */
typedef struct {
    int cursor_pos;
    unsigned char *v_screen;
    unsigned char *cmd_buffer;
    int buffer_index;
} vd_t;


void init_vd();
void destroy_vd();
void start_vd();
void save_vd(int vd_id);
void restore_vd(int vd_id);
void switch_vd(int vd_id);

vd_t *get_curr_vd();
