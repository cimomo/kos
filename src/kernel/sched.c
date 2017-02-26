/*
 * sched.c : Multi-tasking and scheduling
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

#include <sched.h>
#include <util.h>
#include <kio.h>
#include <mm.h>



tss_t *tss0 = &(init_task.tss);
queue_t runqueue;
task_t *curr_task;
task_t *tasks;

/*
 * post: sets up the first task. the context should have been set up in setup.s
 *       initializes all other tasks.
 */
void init_sched()
{
    asm("cli");
    int i, gdt_i = GDT_TSS0_INDEX+1;

    if(!(tasks = (task_t *) kmalloc (MAX_TASKS * sizeof(task_t)))){
        printk("init_sched: out of memory.\n");
        return;
    }

    tasks[0] = init_task;
    
    tasks[0].tss_sel = TSS0_SEL;
    tasks[0].status = TASK_RUNNING;
    tasks[0].next = &(tasks[0]);

    runqueue.head = runqueue.tail = &(tasks[0]);
    curr_task = &tasks[0];

    for(i=1; i<MAX_TASKS; i++){
        tasks[i].tss_sel = gdt_i << 3;
        _set_tss_desc((char *)(&(tasks[i].tss)), gdt_i++);
        tasks[i].status = TASK_NON_EXIST;
    }

    counter = INIT_COUNTER;
    asm("sti");
}


/*
 * post: returns the current task.
 */
task_t *get_curr_task()
{
    return curr_task;
}


/* 
 * post: generates a new pid
 *       returns -1 on error
 */
int new_pid()
{
    int i;
    for(i=0; i<MAX_TASKS; i++){
        if(tasks[i].status == TASK_NON_EXIST)
            return i;
    }
    return -1;
}


/*
 * post: creates a new task
 */
int create_task(void *entry)
{
    int pid;
    task_t *task;
    void *sp;

    asm("cli");

    printk("EIP: %x\n", entry);

    if((pid = new_pid())<0){
        printk("create_task: Too many tasks. Cannot create new.\n");
        return -1;
    }

    task = &tasks[pid];

    task->tss.eip = (unsigned long) entry;
    task->tss.eflags = DEFAULT_EFLAGS;
    /* until we create separate address space */
    task->tss.cr3 = PAGE_DIR_ADDR;
    task->tss.eax = DEFAULT_REG;
    task->tss.ecx = DEFAULT_REG;
    task->tss.edx = DEFAULT_REG;
    task->tss.ebx = DEFAULT_REG;
    task->tss.ebp = DEFAULT_REG;
    task->tss.esi = DEFAULT_REG;
    task->tss.edi = DEFAULT_REG;
    task->tss.es = DATA_SEL;
    task->tss.cs = CODE_SEL;
    task->tss.ss = DATA_SEL;
    task->tss.ds = DATA_SEL;
    task->tss.fs = DATA_SEL;
    task->tss.gs = DATA_SEL;
    task->tss.tflag = DEFAULT_TFLAG;
   
    /* set status to be running for now. */
    task->status = TASK_RUNNING;

    /* link into runqueue */
    (runqueue.tail)->next = task;
    runqueue.tail = task;
    task->next = runqueue.head;

    /* now we need to allocate stack for the new process */
    if(!(sp = kmalloc (STACK_SIZE))){
        printk("create_task: out of memory. \n");
        return -1;
    }

    /* stack is upside-down */
    sp += (STACK_SIZE-1);

    task->tss.esp = (unsigned long) sp;

    asm("sti");
    return pid;
}
    


void switch_to(unsigned short tss_sel)
{
    unsigned short sel[2];

    sel[1] = tss_sel;

    asm("lcall *%0": :"m"(sel));
}



/*
 * post: a toy scheduler.
 */
void schedule()
{
    counter = INIT_COUNTER;

    if(curr_task->status == TASK_RUNNING){
        if(curr_task == curr_task->next) return;
        _context_switch((curr_task->next)->tss_sel);
    }
    else{
        switch_to(runqueue.head->tss_sel);
    }
}

