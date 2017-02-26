/*
 * kbd.h : keyboard header file
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


/*****************************************************************************
 *          General                                                          *
 *****************************************************************************/
 
#define KBD_TIMEOUT             500000L
#define MAX_ASCII               0x7F

 
/*****************************************************************************
 *          Keyboard controller                                              *
 *****************************************************************************/

/* keyboard controller registers */

#define KBD_CTRL_PORT           0x64
#define KBD_STAT_PORT           0x64
#define KBD_DATA_PORT           0x60

/* keyboard controller commands */

#define KBD_CCMD_SELF_TEST      0xAA
#define KBD_CCMD_TEST           0xAB
#define KBD_CCMD_DISABLE        0xAD
#define KBD_CCMD_ENABLE         0xAE

/* keyboard commands */

#define KBD_CMD_SET_LEDS        0xED
#define KBD_CMD_SET_RATE        0xF3
#define KBD_CMD_ENABLE          0xF4
#define KBD_CMD_DISABLE         0xF5
#define KBD_CMD_RESET           0xFF

/* keyboard replies */

#define KBD_REPLY_POR           0xAA
#define KBD_REPLY_ACK           0xFA
#define KBD_REPLY_NACK          0xFE
#define KBD_REPLY_SELFTEST      0x55


/* keyboard status register bits */

#define KBD_STAT_OBF            0x01
#define KBD_STAT_IBF            0x02
#define KBD_STAT_SELFTEST       0x04
#define KBD_STAT_CMD            0x08
#define KBD_STAT_UNLOCKED       0x10
#define KBD_STAT_MOUSE_OBF      0x20
#define KBD_STAT_GTO            0x40
#define KBD_STAT_PERR           0x80

/* LEDs */
#define KBD_SCRL_LED            0x01
#define KBD_NUM_LED             0x02
#define KBD_CAPS_LED            0x04



/*****************************************************************************
 *          Scancode Conversions                                             *
 *****************************************************************************/

/* extended ASCII */

#define K_F1                    0x80
#define K_F2                    0x81
#define K_F3                    0x82
#define K_F4                    0x83
#define K_F5                    0x84
#define K_F6                    0x85
#define K_F7                    0x86
#define K_F8                    0x87
#define K_F9                    0x88
#define K_F10                   0x89
#define K_F11                   0x8A
#define K_F12                   0x8B

#define K_INS                   0x90
#define K_DEL                   0x91
#define K_HOME                  0x92
#define K_END                   0x93
#define K_PGUP                  0x94
#define K_PGDN                  0x95
#define K_LT                    0x96
#define K_UP                    0x97
#define K_DN                    0x98
#define K_RT                    0x99
#define K_PRINT                 0x9A
#define K_PAUSE                 0x9B

#define K_ESC                   0x1B

/* meta bits */

#define KBD_META_ALT            0x0200
#define KBD_META_CTRL           0x0400
#define KBD_META_SHIFT          0x0800
#define KBD_META_KEY            KBD_META_ALT | KBD_META_CTRL | KBD_META_SHIFT
#define KBD_META_CAPS           0x1000
#define KBD_META_NUM            0x2000
#define KBD_META_SCRL           0x4000

/* real (raw) scancodes */

#define RAW_L_ALT               0x38
#define RAW_R_ALT               0x38
#define RAW_L_CTRL              0x1D
#define RAW_R_CTRL              0x1D
#define RAW_L_SHIFT             0x2A
#define RAW_R_SHIFT             0x36
#define RAW_CAPS_LOCK           0x3A
#define RAW_NUM_LOCK            0x45
#define RAW_SCROLL_LOCK         0x46
#define RAW_DEL                 0x53


#define KBD_BUFFER_SIZE         16

/* keyboard buffer */
typedef struct {
    unsigned char data[KBD_BUFFER_SIZE];
    int head;
    int tail;
} kbd_buffer;

/*
int init_kbd(unsigned ss, unsigned typematic, unsigned xlat);
*/

void init_kbd();
int read_kbd_data();
void write_kbd(unsigned port, unsigned data);
int translate_scancode(unsigned scancode);

void kbd_buffer_init();
int kbd_buffer_empty();
int kbd_buffer_full();
void kbd_buffer_enqueue(unsigned char);
unsigned char kbd_buffer_dequeue();
int ctrl_pressed();
int alt_pressed();
int shift_pressed();
