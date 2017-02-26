/*
 * kbd.c : keyboard implementation
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

#include <port.h>
#include <kio.h>
#include <kbd.h>
#include <util.h>


/* the internal keyboard buffer */
kbd_buffer buffer;

/* key status */
static unsigned status;

/*
 * post: initializes the keyboard.
 */
void init_kbd()
{
    /* well, we should have done more here, but let me be lazy for now ... */
    kbd_buffer_init();
}


/*
 * post: read from keyboard controller data port.
 *       returns -1 if timeout.
 */
int read_kbd_data()
{
    unsigned long timeout;
    unsigned stat, data;
    
    for(timeout=0; timeout<KBD_TIMEOUT; timeout++){
        inb(stat, KBD_STAT_PORT);

        /* wait until output buffer full */
        if(stat & KBD_STAT_OBF){
            inb(data, KBD_DATA_PORT);

            /* check if there is parity error or timeout */
            if(!(stat & (KBD_STAT_GTO | KBD_STAT_PERR)))
                return data;
        }
    }
    printk("read_kbd_data: timeout\n");
    return -1;
}


/*
 * pre: port is a valid keyboard controller register port.
 * post: outputs data to port
 */
void write_kbd(unsigned port, unsigned data)
{
    unsigned long timeout;
    unsigned stat;

    for(timeout=0; timeout<KBD_TIMEOUT; timeout++){
        inb(stat, KBD_STAT_PORT);

        /* wait until input buffer empty */
        if(!(stat & KBD_STAT_IBF)) break;
    }

    /* check if timeout */
    if(timeout == KBD_TIMEOUT){
        printk("write_kbd: timeout\n");
        return;
    }

    /* if everything went well, output */
    outb(data, port);
}


/*
 * post: translates the scancode
 */
int translate_scancode(unsigned scancode)
{
    static const unsigned char keymap[] = 
    {
        0,      K_ESC,  '1',    '2',    '3',    '4',    '5',    '6',
        '7',    '8',    '9',    '0',    '-',    '=',    '\b',   '\t',
        'q',    'w',    'e',    'r',    't',    'y',    'u',    'i',
        'o',    'p',    '[',    ']',    '\n',   0,      'a',    's',
        'd',    'f',    'g',    'h',    'j',    'k',    'l',    ';',
        '\'',   '`',    0,      '\\',   'z',    'x',    'c',    'v',
        'b',    'n',    'm',    ',',    '.',    '/',    0,      0,
        0,      ' ',    0,      K_F1,   K_F2,   K_F3,   K_F4,   K_F5,
        K_F6,   K_F7,   K_F8,   K_F9,   K_F10,  0,      0,      K_HOME,
        K_UP,   K_PGUP, '-',    K_LT,   '5',    K_RT,   '+',    K_END,
        K_DN,   K_PGDN, K_INS,  K_DEL,  0,      0,      0,      K_F11,
        K_F12
    };

    static const unsigned char shift_keymap[] =
    {
        0,      K_ESC,  '!',    '@',    '#',    '$',    '%',    '^',
        '&',    '*',    '(',    ')',    '_',    '+',    '\b',   '\t',
        'Q',    'W',    'E',    'R',    'T',    'Y',    'U',    'I',
        'O',    'P',    '{',    '}',    '\n',   0,      'A',    'S',
        'D',    'F',    'G',    'H',    'J',    'K',    'L',    ':',
        '"',    '~',    0,      '|',    'Z',    'X',    'C',    'V',
        'B',    'N',    'M',    '<',    '>',    '?',    0,      0,
        0,      ' ',    0,      K_F1,   K_F2,   K_F3,   K_F4,   K_F5,
        K_F6,   K_F7,   K_F8,   K_F9,   K_F10,  0,      0,      K_HOME,
        K_UP,   K_PGUP, '-',    K_LT,   '5',    K_RT,   '+',    K_END,
        K_DN,   K_PGDN, K_INS,  K_DEL,  0,      0,      0,      K_F11,
        K_F12
    };

    
    static unsigned break_code; 
    unsigned tmp;

    /* reboot on Ctrl + Alt + Del */
    if(scancode==RAW_DEL && (status&KBD_META_CTRL) && (status&KBD_META_ALT))
        _reboot();

    /* check for break code, that is, when a key is released */
    if(scancode > MAX_ASCII){
        break_code = 1;
        scancode &= MAX_ASCII;
    }

    /* reset appropriate bits if alt, ctrl, or shift is released */
    if(break_code){
        if(scancode == RAW_L_ALT || scancode == RAW_R_ALT)
            status &= ~KBD_META_ALT;
        else if(scancode == RAW_L_CTRL || scancode == RAW_R_CTRL)
            status &= ~KBD_META_CTRL;
        else if(scancode == RAW_L_SHIFT || scancode == RAW_R_SHIFT)
            status &= ~KBD_META_SHIFT;

        break_code = 0;
        return -1;
    }

    /* set the appropriate bits if alt, ctrl, or shift is pressed */
    if(scancode == RAW_L_ALT || scancode == RAW_R_ALT){
        status |= KBD_META_ALT;
        return -1;
    }
    if(scancode == RAW_L_CTRL || scancode == RAW_R_CTRL){
        status |= KBD_META_CTRL;
        return -1;
    }
    if(scancode == RAW_L_SHIFT || scancode == RAW_R_SHIFT){
        status |= KBD_META_SHIFT;
        return -1;
    }

    /* scroll lock, num lock, caps lock. LEDs */
    if(scancode == RAW_SCROLL_LOCK || 
       scancode == RAW_NUM_LOCK || 
       scancode == RAW_CAPS_LOCK){
        if(scancode == RAW_SCROLL_LOCK) status ^= KBD_META_SCRL;
        else if(scancode == RAW_NUM_LOCK) status ^= KBD_META_NUM;
        else if(scancode == RAW_CAPS_LOCK) status ^= KBD_META_CAPS;

        write_kbd(KBD_DATA_PORT, KBD_CMD_SET_LEDS);
        tmp = 0;
        if(status & KBD_META_SCRL) tmp |= KBD_SCRL_LED;
        if(status & KBD_META_NUM)  tmp |= KBD_NUM_LED;
        if(status & KBD_META_CAPS) tmp |= KBD_CAPS_LED;

        write_kbd(KBD_DATA_PORT, tmp);
        return -1;
    }

    /* do not convert if ALT is pressed */
    if(status & KBD_META_ALT) return keymap[scancode];

    /* if CTRL is pressed */
    if(status & KBD_META_CTRL){
        if(scancode >= sizeof(keymap)/sizeof(keymap[0])) return -1;

        tmp = keymap[scancode];

        if(tmp >= 'a' && tmp <= 'z') return tmp - 'a';
        if(tmp >= '[' && tmp <= '_') return tmp - '[' + 0x1B;

        return -1;
    }

    /* if SHIFT is pressed */
    if(status & KBD_META_SHIFT){
        if(scancode >= sizeof(shift_keymap)/sizeof(shift_keymap[0])) return -1;

        tmp = shift_keymap[scancode];

        /* something wrong here. maybe non-us keyboard ? */
        if(tmp == 0) return -1;

        if(status & KBD_META_CAPS){
            if(tmp >= 'A' && tmp <= 'Z') tmp = keymap[scancode];
        }
    }

    else{
        if(scancode >= sizeof(keymap)/sizeof(keymap[0])) return -1;

        tmp = keymap[scancode];

        if(tmp == 0) return -1;

        if(status & KBD_META_CAPS){
            if(tmp >= 'a' && tmp <= 'z') tmp = shift_keymap[scancode];
        }
    }

    return tmp;
}


/*
 * post: initializes the keyboard buffer.
 */
void kbd_buffer_init()
{
    buffer.head = buffer.tail = 0;
}


/*
 * post: returns true if the buffer is empty
 */
int kbd_buffer_empty()
{
    return buffer.head == buffer.tail;
}


/*
 * post: returns true if the buffer is full
 */
int kbd_buffer_full()
{
    return (buffer.tail+1) % KBD_BUFFER_SIZE == buffer.head;
}


/*
 * post: adds one char to the end of the buffer
 *       or block if buffer full
 */
void kbd_buffer_enqueue(unsigned char c)
{
    while(kbd_buffer_full());

    buffer.data[buffer.tail] = c;
    buffer.tail = (buffer.tail + 1) % KBD_BUFFER_SIZE;
}


/* 
 * post: reads off one char from the buffer
 *       or block if buffer empty
 */
unsigned char kbd_buffer_dequeue()
{
    unsigned char c;
    while(kbd_buffer_empty());

    c = buffer.data[buffer.head];
    buffer.head = (buffer.head + 1) % KBD_BUFFER_SIZE;
    
    return c;
}


/*
 * post: returns true if ctrl is pressed
 */
int ctrl_pressed()
{
    return status & KBD_META_CTRL;
}


/*
 * post: returns true if alt is pressed
 */
int alt_pressed()
{
    return status & KBD_META_ALT;
}


/*
 * post: returns true if shift is pressed
 */
int shift_pressed()
{
    return status & KBD_META_SHIFT;
}

