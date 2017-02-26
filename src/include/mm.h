/*
 * mm.h : Memory Manager Header
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

/* address of page directory and the first page table */
#define PAGE_DIR_ADDR           0x70000
#define PAGE_TABLE_ADDR         0x71000

/* total number of pages. currently we only use 4Mb of memory */
#define NUM_PAGES               1024

/* number of PDEs and PTEs */
#define NUM_PDES                1024
#define NUM_PTES                1024

/* each page directory entry or page table entry has 4 bytes */
#define PDE_SIZE                4
#define PTE_SIZE                4

/* each page directory or page table takes 4Kb to store */
#define PAGE_DIR_SIZE           PDE_SIZE * NUM_PDES
#define PAGE_TABLE_SIZE         PTE_SIZE * NUM_PTES

/* each page frame is 4Kb contiguous physical memory */
#define PAGE_FRAME_SIZE         4096

/* page dir/table entry attribute bits */
#define PAGE_ATTR_PRESENT       0x0001
#define PAGE_ATTR_RW            0x0002
#define PAGE_ATTR_US            0x0004
#define PAGE_ATTR_ACCESSED      0x0020
#define PAGE_ATTR_DIRTY         0x0040

/* handy values */
#define PAGE_RW_PRESENT         PAGE_ATTR_PRESENT | PAGE_ATTR_RW
#define PAGE_RW_NPRESENT        PAGE_ATTR_RW

#define PAGE_UNMAPPED_ADDR      0

/* bit 31 of CR0 enables paging */
#define CR0_ENABLE_PAGING       0x80000000

/* 
 * we map each page directory into itself, so we can access it via
 * virtual memory. It can be any entry of choice, we use the last one.
 */
#define PDE_SELF                1023

/* and good things happens ... */

/* virtual address of page table */
#define PAGE_TABLE_VADDR        (PDE_SELF << 22)
/* virtual address of the page directory */
#define PAGE_DIR_VADDR          ((PDE_SELF << 22) | (PDE_SELF << 12))

/* get PDE index from virtual address */
#define GET_PDE_INDEX(vaddr)    (vaddr >> 22)
/* get PTE index from virtual address */
#define GET_PTE_INDEX(vaddr)    (vaddr >> 12)     


/* address of the kernel heap */
#define KERNEL_HEAP_ADDR        0x100000


/* page usage */
#define PAGE_LOCKED             0xFF
#define PAGE_FREE               0


/* converts physical address to page index */
#define TO_PAGE_INDEX(paddr)    (paddr >> 12)
#define FROM_PAGE_INDEX(i)      (i << 12)


/* size_t */
typedef unsigned int size_t;


/* Some magic value to tell whether some memory was allocated via malloc */
#define MALLOC_MAGIC            0x7CF2

/* header of allocated memory */
typedef struct _header{
    size_t size;
    struct _header *next;
    unsigned magic : 31;
    unsigned used : 1;
} header_t;



/* function protocols exported from paging.c */
void setup_paging();
int mmap(unsigned long vaddr, unsigned long paddr, unsigned int attr);
unsigned long alloc_page();
int free_page(unsigned long paddr);
void *morecore(int size);


/* functions exported from mm.c */
void *kmalloc(size_t size);
int kfree(void *p);
//int kmemcpy(unsigned char *from, unsigned char *to, int n);
