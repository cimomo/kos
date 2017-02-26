/*
 * kio.c : kernel level I/O
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
#include <port.h>
#include <util.h>


/******************************************************************************
 *          Low level I/O                                                     *
 ******************************************************************************/ 

/*
 * pre: attr is the text mode attribute
 * post: paints the screen with attr
 */
void k_paint_screen(unsigned char attr)
{
    unsigned char *vidmem = (unsigned char *) VIDEO_MEM;
    int i = 0;

    /* let's clear double size of the video screen, to be safe */
    while (i < (SCREEN_ROWS*SCREEN_COLS*CHAR_BYTES)){
        vidmem[i++] = ' ';
        vidmem[i++] = attr;
    }
    set_cursor_pos(0, 0);
}


/*
 * post: clears the screen
 */
void k_clear_screen()
{
    /* paint with black background */
    k_paint_screen(BG_BLACK | FG_WHITE);
}


/*
 * post: clear the string from certain position on
 */
void k_clear_screen_from(int pos)
{
    unsigned char *vidmem = (unsigned char *) VIDEO_MEM;
    int i = pos;

    while(i < (SCREEN_ROWS*SCREEN_COLS*CHAR_BYTES)){
        vidmem[i++] = ' ';
        vidmem[i++] = BG_BLACK | FG_WHITE;
    }
    set_cursor(pos);
}


/*
 * pre: row, col are valid row and column number.
 *      attr is the text mode attribute
 * post: prints char c at (row, col) with attr
 */ 
void kputc(char c, int row, int col, unsigned char attr)
{
    unsigned char *vidmem = (unsigned char *) VIDEO_MEM;
    int pos = row*SCREEN_COLS*CHAR_BYTES + col*CHAR_BYTES;

    vidmem[pos] = c;
    vidmem[pos+1] = attr;
}


/*
 * pre: row, col are valid row and column number
 *      attr is the text mode attribute
 * post: prints string s starting at (row, col) with attr
 */
void kputs(char *s, int row, int col, unsigned char attr)
{
    unsigned char *vidmem = (unsigned char *) VIDEO_MEM;
    int pos = row*SCREEN_COLS*CHAR_BYTES + col*CHAR_BYTES;

    if(!(*s))
        kputc('c',10,0,BG_BLACK | FG_WHITE);
    while(*s){
        vidmem[pos++] = *s;
        vidmem[pos++] = attr;
        *s ++;
    }
}


/*
 * post: scrolls the screen down by n lines
 */
void scroll(int n)
{
    int i, pos_to, pos_from;
    unsigned char *vidmem = (unsigned char *) VIDEO_MEM;
    
    /* trivial case */
    if(n<=0) return;

    /* starting at line 2 */
    pos_to = 2*SCREEN_COLS*CHAR_BYTES;
    pos_from = (2+n)*SCREEN_COLS*CHAR_BYTES;

    for(i=0; i<(SCREEN_ROWS-2)*SCREEN_COLS*CHAR_BYTES; i++){
        vidmem[pos_to++] = vidmem[pos_from++];
    }

    /* clear the memory outside the screen */
    for(i=SCREEN_ROWS*SCREEN_COLS*CHAR_BYTES;
        i<(SCREEN_ROWS+n)*SCREEN_COLS*CHAR_BYTES;
        i++){
        vidmem[i++] = ' ';
        vidmem[i] = BG_BLACK | FG_WHITE;
    }
}

/******************************************************************************
 *          High level I/O                                                    *
 ******************************************************************************/


/*
 * pre: only implemented formats:
 *      %d - int (decimal)
 *      %x - int (hexidecimal)
 *      %c - char
 *      %s - string
 *
 *      args points to the first argument after fmt 
 *
 * post: prints formatted output,
 *       for unrecognized format, % is ignored. e.g. %a will print out a.
 */

void vprintk(const char *fmt, void *args)
{
    /* cursor info */
    int addr, row, col;

    /* formatted data */
    char data[20];

    /* arguments */
    int *arg_i;
    char *arg_c;
    char **arg_s;
    
    /* compute the cursor position */
    addr = get_cursor();
    row = get_cursor_row(addr);
    col = get_cursor_col(addr);

    /* format string */
    while(*fmt){
        if(*fmt == '\n'){
            row ++;
            col = 0;
            set_cursor_pos(row, col);
        }

        else if(*fmt == '\t'){
            /*
            if(col+8 >= SCREEN_COLS)
                row ++;
            col = (col+8) % SCREEN_COLS;
            set_cursor_pos(row, col);
            */
            col = (col+8) & ~(8-1);
            set_cursor_pos(row, col);
        }

        else if(*fmt == '\b'){
            if(col == 0){
                row --;
                for(col=SCREEN_COLS-1; isblank(row, col) && col>=0; col--);
                if(col<SCREEN_COLS-1) col ++;
            }
            else col --;
            
            kputc(' ', row, col, BG_BLACK | FG_WHITE);
            set_cursor_pos(row, col);
        }

        else if(*fmt == '%'){
            fmt ++;

            if(*fmt == 'x'){
                arg_i = (int *) args;
                hex2str(*arg_i, data);    
                vprintk(data, args);
            }
        
            else if(*fmt == 'd'){
                arg_i = (int *) args;
                dec2str(*arg_i, data);
                vprintk(data, args);
            }

            else if(*fmt == 'c'){
                arg_c = (char *) args;

                if(*arg_c == '\n') printk("\n");   
                else if(*arg_c == '\b') printk("\b");
                else if(*arg_c == '\t') printk("\t");
               
                else{
                    kputc(*arg_c, row, col, BG_BLACK | FG_WHITE);

                    col ++;
                    if(col == SCREEN_COLS){
                        row ++;
                        col = 0;
                    }
                    set_cursor_pos(row, col);
                }
            }
                
            else if(*fmt == 's'){
                arg_s = (char **) args;
                vprintk(*arg_s, args);
            }

            else continue;

            /* recompute cursor position */
            addr = get_cursor();
            row = get_cursor_row(addr);
            col = get_cursor_col(addr);

            args += 4;
        }
        
        /* output a normal char */
        else{
            kputc(*fmt, row, col, BG_BLACK | FG_WHITE);

            /* update cursor */
            col ++;
            if(col == SCREEN_COLS){
                row ++;
                col = 0;
            }
            set_cursor_pos(row, col);
        }
        fmt ++;
    }

    /* scroll if necessary */
    if(row >= SCREEN_ROWS){
        scroll(row-SCREEN_ROWS+1);
        set_cursor_pos(SCREEN_ROWS-1, col);
    }
}



/******************************************************************************
 *          Cursor Manipulation                                               *
 ******************************************************************************/
 

/*
 * post: returns the address (relative to video memory) of the cursor
 */
int get_cursor()
{
    int addr=0, tmp=0;

    outb(CURSOR_ADDR_HIGH, CRT_CTRL);
    inb(tmp, CRT_DATA);
    addr = tmp << CURSOR_SHIFT;

    outb(CURSOR_ADDR_LOW, CRT_CTRL);
    inb(tmp, CRT_DATA);
    addr = addr | tmp;

    return addr;
}


/*
 * pre: addr is the video memory address of the cursor
 * post: returns the row position of the cursor
 */
int get_cursor_row(int addr)
{
    return addr/SCREEN_COLS;
}


/* 
 * pre: addr is the video memory address of the cursor
 * post: returns the column position of the cursor
 */
int get_cursor_col(int addr)
{
    return addr%SCREEN_COLS;
}

 
/*
 * pre: addr is the video memory address
 * post: set cursor to addr
 */
void set_cursor(int addr)
{
    outb(CURSOR_ADDR_LOW, CRT_CTRL);
    outb((char)(addr&CURSOR_ADDR_MASK), CRT_DATA);

    outb(CURSOR_ADDR_HIGH, CRT_CTRL);
    outb((char)((addr>>CURSOR_SHIFT)&CURSOR_ADDR_MASK), CRT_DATA);
}


/*
 * pre: row, col are valid row, column numbers
 * post: sets the cursor to (row, col)
 */
void set_cursor_pos(int row, int col)
{
    int addr = row*SCREEN_COLS + col;
    set_cursor(addr);
}



/******************************************************************************
 *          Utilities                                                         *
 ******************************************************************************/


/*
 * post: returns true if char at (row, col) is not ' '.
 */
int isblank(int row, int col)
{
    unsigned char * vidmem = (unsigned char *) VIDEO_MEM;
    int pos = row*SCREEN_COLS*CHAR_BYTES + col*CHAR_BYTES;

    return (vidmem[pos] == ' ' || vidmem[pos] == 0);
}


/*
 * post: converts hex to string
 */
void hex2str(unsigned int n, char *a)
{
    long pow16 = 0x10000000;
    int digit;

    *(a++) = '0';
    *(a++) = 'x';

    if (n == 0){
        *a = 48;
        *(a+1) = 0;
        return;
    }
    
    while(n < pow16) pow16 >>= 4;

    while(pow16 > 0){
        digit = n / pow16;
        n = n % pow16;

        if(digit < 10) *a = digit + 48;
        else *a = digit + 55;

        a ++;
        pow16 >>= 4;
    }

    *a = 0;
}


/*
 * post: converts decimal to string
 */
void dec2str(int n, char *a)
{
    long pow10 = 1000000000;
    int digit;

    if(n == 0){
        *a = '0';
        *(a+1) = 0;
        return;
    }

    if(n < 0){
        *(a++) = '-';
        n = -n;
        n = (unsigned int) n;
    }

    while(n < pow10) pow10 /= 10;

    while(pow10 > 0){
        digit = n / pow10;
        n = n % pow10;

        *a = digit + 48;
        a ++;

        pow10 = pow10 / 10;
    }

    *a = 0;
} 
