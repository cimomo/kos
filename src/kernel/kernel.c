/*
 * kernel.c : kernel main
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
#include <isr.h>
#include <kbd.h>
#include <mm.h>
#include <vdesktop.h>
#include <console.h>
#include <login.h>
#include <sched.h>

void wait()
{
    int i, j;
    for(i=0; i<0xFFFF; i++)
        for(j=0; j<0xFFF0; j++);
}



int _k_main()
{
    extern void hello();

    //init_interrupts();
    setup_paging();
    init_sched();
    init_kbd();
    init_login();
    k_clear_screen();
    //create_task(hello);
    
    
    print_login();

    return 0;
}

