/*
 * isr.h -- Interrupt Request Header
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

#define IDT_ENTRIES         0x100
#define IDT_BASE            0x2000
#define IDT_SIZE            0x800
#define BYTES_IDT_ENTRY     0x8
#define PRESENT_RING0       0x8E00
#define PRESENT_RING3       0xEE00

#define IDT_TIMER           0x20
#define IDT_KEYBOARD        0x21

typedef struct {
    unsigned short offset_low;
    unsigned short selector;
    unsigned short settings;
    unsigned short offset_high;
} idt_descriptor;


/* Win98 style. hee hee */
void blue_screen();


extern void defaultISR();
extern void timerISR();
extern void keyboardISR();

extern void INT00Handler();
extern void INT01Handler();
extern void INT02Handler();
extern void INT03Handler();
extern void INT04Handler();
extern void INT05Handler();
extern void INT06Handler();
extern void INT07Handler();
extern void INT08Handler();
extern void INT09Handler();
extern void INT10Handler();
extern void INT11Handler();
extern void INT12Handler();
extern void INT13Handler();
extern void INT14Handler();
extern void INT16Handler();
extern void INT17Handler();
extern void INT18Handler();
extern void INT19Handler();

void init_interrupts();
void set_int_gate(int n, void (*handler)(), int ring);
