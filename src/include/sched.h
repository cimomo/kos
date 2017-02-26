/*
 * sched.h : Multi-tasking and scheduling header
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


#define MAX_TASKS               16

#define GDT_BASE                0x500
#define GDT_ENTRY_SIZE          8
#define GDT_TSS0_INDEX          3

#define NULL_SEL                0x0
#define CODE_SEL                0x8
#define DATA_SEL                0x10
#define TSS0_SEL                0x18


#define DEFAULT_REG             0
#define DEFAULT_EFLAGS          0x0200
#define DEFAULT_TFLAG           0
#define STACK_SIZE              4096

#define INIT_COUNTER            2

int counter;


typedef struct {
    unsigned short limit_0_15;
    unsigned short base_0_15;
    unsigned char base_16_23;
    unsigned char dpl_type;
    unsigned char limit_16_19;
    unsigned char base_24_31;
} gdt_desc;


/* Task State Segment */
typedef struct {
    unsigned short  back_link;
    unsigned short  reserved0;
    unsigned long   esp0;
    unsigned short  ss0;
    unsigned short  reserved1;
    unsigned long   esp1;
    unsigned short  ss1;
    unsigned short  reserved2;
    unsigned long   esp2;
    unsigned short  ss2;
    unsigned short  reserved3;
    unsigned long   cr3;
    unsigned long   eip;
    unsigned long   eflags;
    unsigned long   eax;
    unsigned long   ecx;
    unsigned long   edx;
    unsigned long   ebx;
    unsigned long   esp;
    unsigned long   ebp;
    unsigned long   esi;
    unsigned long   edi;
    unsigned short  es;
    unsigned short  reserved4;
    unsigned short  cs;
    unsigned short  reserved5;
    unsigned short  ss;
    unsigned short  reserved6;
    unsigned short  ds;
    unsigned short  reserved7;
    unsigned short  fs;
    unsigned short  reserved8;
    unsigned short  gs;
    unsigned short  reserved9;
    unsigned short  ldt;
    unsigned short  reserved10;
    unsigned short  tflag;
    unsigned short  iomap;
} tss_t;


typedef struct _task {
    tss_t tss;
    unsigned short int tss_sel;
    enum {
        TASK_RUNNING=0, 
        TASK_INTERRUPTIBLE=1, 
        TASK_UNINTERRUPTIBLE=2,
        TASK_NON_EXIST=3
    } status;
    struct _task *next;
} task_t;
    
    
typedef struct {
    task_t *head, *tail;
} queue_t;



extern tss_t *tss0;

task_t init_task;

void init_sched();
task_t *get_curr_task();
int new_pid();
void schedule();
int create_task(void *entry);
//int set_tss_desc(char *tss, int gdt_i);
