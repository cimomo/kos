/*
 * kio.h -- Kernel low level I/O header.
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

/*
 * Video memory (physical)
 */
#define VIDEO_MEM           0xB8000

/*
 * Text mode settings
 */
#define SCREEN_ROWS         25
#define SCREEN_COLS         80
#define CHAR_BYTES          2

/*
 * Text mode attributes
 */
#define FG_BLACK            0x00
#define FG_BLUE             0x01
#define FG_GREEN            0x02
#define FG_CYAN             0x03
#define FG_RED              0x04
#define FG_MAGENTA          0x05
#define FG_BROWN            0x06
#define FG_GRAY             0x07
#define FG_DARKGRAY         0x08
#define FG_BRIGHTBLUE       0x09
#define FG_BRIGHTGREEN      0x0A
#define FG_BRIGHTCYAN       0x0B
#define FG_PINK             0x0C
#define FG_BRIGHTMAGENTA    0x0D
#define FG_BRIGHTYELLOW     0x0E
#define FG_WHITE            0x0F

#define BG_BLACK            0x00
#define BG_BLUE             0x10
#define BG_GREEN            0x20
#define BG_CYAN             0x30
#define BG_RED              0x40
#define BG_MAGENTA          0x50
#define BG_BROWN            0x60
#define BG_GRAY             0x70
#define BG_DARKGRAY         0x80
#define BG_BRIGHTBLUE       0x90
#define BG_BRIGHTGREEN      0xA0
#define BG_BRIGHTCYAN       0xB0
#define BG_PINK             0xC0
#define BG_BRIGHTMAGENTA    0xD0
#define BG_WHITE            0xF0


/*
 * cursor manipulation
 */
#define CRT_CTRL            0x03D4
#define CRT_DATA            0x03D5
#define CURSOR_ADDR_HIGH    0x0E
#define CURSOR_ADDR_LOW     0x0F

#define CURSOR_ADDR_MASK    0xFF 
#define CURSOR_SHIFT        8


/* low level I/O functions */
void k_paint_screen(unsigned char attr);
void k_clear_screen();
void k_clear_screen_from();
void kputc(char c, int row, int col, unsigned char attr);
void kputs(char *s, int row, int col, unsigned char attr);
void kputi(int i, int row, int col, unsigned char attr);
void scroll(int n);

/* high level I/O functions */
extern void printk(const char *fmt, ...);
void vprintk(const char *fmt, void *args);


/* cursor manipulation */
int get_cursor_row(int addr);
int get_cursor_col(int addr);
void set_cursor_pos(int row, int col);
int get_cursor();
void set_cursor(int pos);


/* utilities */
int isblank(int row, int col);
void hex2str(unsigned int n, char *a);
void dec2str(int n, char *a);
