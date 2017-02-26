/*
 * isr.c: Interrupt Service Routines
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


#include <isr.h>
#include <kio.h>
#include <kbd.h>
#include <port.h>
#include <util.h>
#include <vdesktop.h>
#include <kstring.h>
#include <sched.h>

/*
idt_descriptor * idt = (idt_descriptor *) IDT_BASE;

void init_interrupts()
{
    
    set_int_gate(0,  INT00Handler, 0);
    set_int_gate(1,  INT01Handler, 0);
    set_int_gate(2,  INT02Handler, 0);
    set_int_gate(3,  INT03Handler, 0);
    set_int_gate(4,  INT04Handler, 0);
    set_int_gate(5,  INT05Handler, 0);
    set_int_gate(6,  INT06Handler, 0);
    set_int_gate(7,  INT07Handler, 0);
    set_int_gate(8,  INT08Handler, 0);
    set_int_gate(9,  INT09Handler, 0);
    set_int_gate(10, INT10Handler, 0);
    set_int_gate(11, INT11Handler, 0);
    set_int_gate(12, INT12Handler, 0);

    set_int_gate(13, INT13Handler, 0);
 
    set_int_gate(14, INT14Handler, 0);
    set_int_gate(16, INT16Handler, 0);
    set_int_gate(17, INT17Handler, 0);
    set_int_gate(18, INT18Handler, 0);
    set_int_gate(19, INT19Handler, 0);

}


void set_int_gate(int n, void (*handler)(), int ring)
{
    unsigned short selector = CODE_SEL;
    unsigned short settings;
    unsigned long offset = (unsigned long) handler;

    asm ("movw %%cs,%0" :"=g"(selector));

    switch(ring){
        case 0: 
            settings = PRESENT_RING0; 
            break;
        case 1:
        case 2:
        case 3:
            settings = PRESENT_RING3;
            break;
    }

    idt[n].offset_low = (offset & 0xFFFF);
    idt[n].selector = selector;
    idt[n].settings = settings;
    idt[n].offset_high = (offset >> 16);
}        
*/
/*
 * post: for now, just prints out something funny =)
 */
void blue_screen(int int_num)
{
    char error[100];
    k_paint_screen(BG_BLUE | FG_BLUE);
    
    if(int_num<0){
        kstrcpy(error, "An uncaught exception has occured at unknown location.");
    }
    else{
        kstrcpy(error, "A fatal error has occured. Interrupt No.: ");
        hex2str(int_num, &error[kstrlen(error)]);
    }

    kputs("  K-OS  ", 5, 35, BG_GRAY | FG_BLUE);
    kputs(error, 7, 10, BG_BLUE | FG_WHITE);

    kputs("The current application will be terminated.",
        8, 10, BG_BLUE | FG_WHITE);

    kputs("*  Oops, there does not exist an application yet ...",
        10, 10, BG_BLUE | FG_WHITE);

    kputs("*  And you cannot reboot either ...",
        11, 10, BG_BLUE | FG_WHITE);

    kputs("Haha, today I feel like a Windows =)",
        14, 10, BG_BLUE | FG_WHITE);

    asm("CLI");
    asm("HLT");
}


void kbd_isr()
{
    int scancode, ascii;
    
    scancode = read_kbd_data();
    ascii = translate_scancode(scancode);

    if(ascii == K_ESC) blue_screen(-1);
    else if(ascii >= 0){
        kbd_buffer_enqueue((unsigned char) ascii);
    }
}


void timer_isr()
{
    if(--counter<=0) schedule();
}
