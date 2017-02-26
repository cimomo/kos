/*
 * vdesktop.c : Virtual Desktops
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

#include <kio.h>
#include <vdesktop.h>
#include <mm.h>


vd_t *vd[NUM_VDS];

static int vd_curr_id = 0;


/*
 * post: allocate memory for virtual desktops
 */
void init_vd()
{
    int i, j;

    unsigned char header_attr, screen_attr = FG_WHITE | BG_BLACK;
    
    for(i=0; i<NUM_VDS; i++){
        switch(i){
            case 0:
                header_attr = FG_WHITE | BG_BLUE;
                break;
            case 1:
                header_attr = FG_WHITE | BG_GREEN;
                break;
            case 2:
                header_attr = FG_WHITE | BG_MAGENTA;
                break;
            case 3:
                header_attr = FG_WHITE | BG_BROWN;
                break;
        }
        /* allocate memory for virtual desktop and its components */
        if(!(vd[i] = (vd_t *) kmalloc (sizeof(vd_t)))){
            printk("init_vd: out of memory.\n");
            return;
        }
        vd[i]->cursor_pos = SCREEN_COLS*CHAR_BYTES;
        if(!(vd[i]->v_screen = (unsigned char *) kmalloc 
            (sizeof(unsigned char) * SCREEN_COLS*SCREEN_ROWS*CHAR_BYTES))){
            printk("init_vd: out of memory.\n");
            return;
        }
        if(!(vd[i]->cmd_buffer = (unsigned char *) kmalloc
            (sizeof(unsigned char) * CMD_BUFFER_SIZE))){
            printk("init_vd: out of memory.\n");
            return;
        }
        
        vd[i]->buffer_index = 0;
      
        
        for(j=0; j<SCREEN_COLS*CHAR_BYTES; j++){
            (vd[i]->v_screen)[j++] = ' ';
            (vd[i]->v_screen)[j] = header_attr;
        }
        /* put in some string, ok ... this part is ugly ... */
        (vd[i]->v_screen)[0] = '[';
        (vd[i]->v_screen)[2] = '+';
        (vd[i]->v_screen)[4] = ']';

        (vd[i]->v_screen)[60] = 'K';
        (vd[i]->v_screen)[62] = '-';
        (vd[i]->v_screen)[64] = 'O';
        (vd[i]->v_screen)[66] = 'S';
        
        (vd[i]->v_screen)[70] = 'V';
        (vd[i]->v_screen)[72] = 'e';
        (vd[i]->v_screen)[74] = 'r';
        (vd[i]->v_screen)[76] = 's';
        (vd[i]->v_screen)[78] = 'i';
        (vd[i]->v_screen)[80] = 'o';
        (vd[i]->v_screen)[82] = 'n';

        (vd[i]->v_screen)[86] = '0';
        (vd[i]->v_screen)[88] = '.';
        (vd[i]->v_screen)[90] = '0';
        (vd[i]->v_screen)[92] = '1';
        
        (vd[i]->v_screen)[124] = 'V';
        (vd[i]->v_screen)[126] = 'i';
        (vd[i]->v_screen)[128] = 'r';
        (vd[i]->v_screen)[130] = 't';
        (vd[i]->v_screen)[132] = 'u';
        (vd[i]->v_screen)[134] = 'a';
        (vd[i]->v_screen)[136] = 'l';

        (vd[i]->v_screen)[140] = 'D';
        (vd[i]->v_screen)[142] = 'e';
        (vd[i]->v_screen)[144] = 's';
        (vd[i]->v_screen)[146] = 'k';
        (vd[i]->v_screen)[148] = 't';
        (vd[i]->v_screen)[150] = 'o';
        (vd[i]->v_screen)[152] = 'p';

        (vd[i]->v_screen)[156] = i + 48;
        
        for(j=SCREEN_COLS*CHAR_BYTES; 
            j<SCREEN_ROWS*SCREEN_COLS*CHAR_BYTES; 
            j++){
            (vd[i]->v_screen)[j++] = ' ';
            (vd[i]->v_screen)[j] = screen_attr;
        }
    }
}


/*
 * post: destroys all virtual desktops
 */
void destroy_vd()
{
    int i;
    for(i=0; i<NUM_VDS; i++){
        if((kfree(vd[i]->v_screen))<0)
            printk("destroy_vd: memory error.\n");
            
        if((kfree(vd[i]->cmd_buffer))<0)
            printk("destroy_vd: memory error.\n");
            
        if((kfree(vd[i]))<0)
            printk("destroy_vd: memory error.\n");
    }
}

/*
 * pre: vd_id is a valid id of a virtual desktop
 * post: saves the virtual desktop with vd_id
 */
void save_vd(int vd_id)
{
    unsigned char *vidmem = (unsigned char *)VIDEO_MEM;

    int i;

    for(i=0; i<SCREEN_ROWS*SCREEN_COLS*CHAR_BYTES; i++){
        (vd[vd_id]->v_screen)[i] = vidmem[i];
    }
    vd[vd_id]->cursor_pos = get_cursor();
}

/* 
 * pre: vd_id is a valid id for a virtual desktop
 * post: restores the virtual desktop with vd_id
 */
void restore_vd(int vd_id)
{
    unsigned char *vidmem = (unsigned char *)VIDEO_MEM;

    int i;

    for(i=0; i<SCREEN_ROWS*SCREEN_COLS*CHAR_BYTES; i++){
        vidmem[i] = (vd[vd_id]->v_screen)[i];
    }
    vd_curr_id = vd_id;
    set_cursor(vd[vd_id]->cursor_pos);
}


/*
 * pre: vd_id is a valid id for a virtual desktop
 * post: swtich to virtual desktop with vd_id
 */
void switch_vd(int vd_id)
{
    save_vd(vd_curr_id);
    restore_vd(vd_id);
}


/*
 * post: load vd0
 */
void start_vd()
{
    restore_vd(0);
}


/*
 * post: returns the current virtual desktop.
 */
vd_t *get_curr_vd()
{
    return vd[vd_curr_id];
}

