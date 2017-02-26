/*
 * paging.c : virtual memory with paging
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
#include "../include/util.h"
#include "../include/mm.h"
#include "../include/kio.h"


/* number of pages, we use 4Mb of memory, thus 1024 pages */
static unsigned char page_use_count[NUM_PAGES];

/*
 * post: sets up paging
 *       identity mapping for memory before the kernel heap.
 *       currently we only use 4Mb of memory, which mounts to 1024 pages
 */
void setup_paging()
{
    unsigned long *page_dir = (unsigned long *) PAGE_DIR_ADDR;
    unsigned long *page_table = (unsigned long *) PAGE_TABLE_ADDR;

    unsigned long addr;

    unsigned int i;

    /* identity mapping for memory up to kernel heap */
    for(i=0, addr=0; addr<KERNEL_HEAP_ADDR; i++, addr+=PAGE_FRAME_SIZE)
        page_table[i] = addr | PAGE_RW_PRESENT;

    /* zero out rest of the page table */
    for(; i<NUM_PTES; i++)
        page_table[i] = PAGE_UNMAPPED_ADDR | PAGE_RW_NPRESENT;

    /* set up the first page directory */
    page_dir[0] = (unsigned long) page_table;
    page_dir[0] |= PAGE_RW_PRESENT;

    /* zero rest of the page directories, access will fire page_fault */
    for(i=1; i<NUM_PDES-1; i++){
        page_dir[i] = PAGE_UNMAPPED_ADDR | PAGE_RW_NPRESENT;
    }

    /* now we map the page directory into itself, so that we can access
       it through virtual memory */
    page_dir[PDE_SELF] = (unsigned long) page_dir | PAGE_RW_PRESENT;

    /* keep track of which pages are being used */
    for(i=0, addr=0; addr<KERNEL_HEAP_ADDR; i++, addr+=PAGE_FRAME_SIZE)
        page_use_count[i] = (unsigned char) PAGE_LOCKED;

    for(; i<NUM_PAGES; i++)
        page_use_count[i] = (unsigned char) PAGE_FREE;
    
    _write_cr3((unsigned long) page_dir);
    _write_cr0(_read_cr0() | CR0_ENABLE_PAGING);
}


/* 
 * pre: vaddr is the virtual address
 *      paddr is the corresponding physical address
 *      attr is the attribute flags
 * post: mapps the virtual address to the physical address
 *       returns 0 on success and -1 on error
 */
int mmap(unsigned long vaddr, unsigned long paddr, unsigned int attr)
{
    unsigned long *page_dir = (unsigned long *) PAGE_DIR_VADDR;
    unsigned long *page_table = (unsigned long *) PAGE_TABLE_VADDR;
    
    unsigned long pde = GET_PDE_INDEX(vaddr);
    unsigned long pte = GET_PTE_INDEX(vaddr);

    if(!(page_dir[pde] & PAGE_ATTR_PRESENT)){
        printk("mmap: no page while mapping %x to %x\n", vaddr, paddr);
        return -1;
    }
    
    /* clear the lower 12 bits, so the addr is page-aligned */
    paddr &= -PAGE_FRAME_SIZE;

    /* clear the upper 20 bits so we dont corrupt address */
    attr &= (PAGE_FRAME_SIZE - 1); 

    page_table[pte] = paddr | attr;

    return 0;
}


/*
 * post: allocates one page (4Kb) of memory.
 *       returns the physical address of the page, or NULL on error
 */
unsigned long alloc_page()
{
    int i;

    /* the first 4Kb of memory is used to access page directory,
       so skip the first page */
    for(i=1; i<NUM_PAGES; i++){
        if(page_use_count[i] == PAGE_FREE){
            page_use_count[i] ++;
            return (unsigned long) FROM_PAGE_INDEX(i);
        }
    }

    /* no free page. we are out of memory. */
    return 0;
}


/*
 * pre: paddr is the physical address of the page to be freed
 * post: frees the page, or return -1 on error
 */
int free_page(unsigned long paddr)
{
    unsigned long i = TO_PAGE_INDEX(paddr);
    
    if(i >= NUM_PAGES){
        printk("free_page: physical addr too big: %x\n", paddr);
        return -1;
    }

    if(page_use_count[i] == PAGE_FREE){
        printk("free_page: page already free: %x\n", paddr);
        return -1;
    }

    page_use_count[i] -- ;
    
    return 0;
}


/*
 * post: returns the address of the next available byte in the kernel heap
 *       at this level allocation is 4Kb aligned
 */
void *morecore(int size)
{
    static unsigned char *brk=0;

    void *old_brk;
    unsigned long paddr, vaddr;

    /* initialize kernel heap for the first time */
    if(!brk){
        printk("Kernel heap initialized.\n");
        brk = (unsigned char *) KERNEL_HEAP_ADDR;
    }

    old_brk = brk;

    /* size must be positive for now */
    if(size <=0 ) return old_brk;
    
    /* make sure we allocate multiple of 4Kb memory */
    size += (PAGE_FRAME_SIZE - 1);
    size &= (-PAGE_FRAME_SIZE);

    for(vaddr = (unsigned long) brk; 
        vaddr < (unsigned long) brk + size; 
        vaddr += PAGE_FRAME_SIZE){

        /* allocate a new page */
        if(!(paddr = alloc_page())){
            printk("morecore: allocation failed.\n");
            return old_brk;
        }

        /* now map the newly allocated memory */
        if(mmap(vaddr, paddr, PAGE_ATTR_PRESENT | PAGE_ATTR_RW)<0){
            printk("morecore: memory mapping failed.\n");
            return old_brk;
        }
    }


    brk += size;
    
    return old_brk;
}




















    
