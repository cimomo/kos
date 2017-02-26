/*
 * mm.c : Memory Manager
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

#include <util.h>
#include <mm.h>
#include <kio.h>



/* Kernel heap */
static header_t *kheap_bottom, *kheap_top;


/*
 * post: returns a pointer to memory of size *size*, or null on error 
 */
void *kmalloc(size_t size)
{
    /* size with header */
    size_t total_size;
    header_t *p, *left_over;

    /* if no memory is requested, return */
    if(!size) return 0;

    total_size = size + sizeof(header_t);

    /* if we have not initialized the kernel heap */
    if(!kheap_bottom){
        p = (header_t *) morecore(total_size);
        
        /* kernel heap overflows (this will be impressive) */
        if((long) p == 0xFFFFFFFF) return 0;

        /* initialize kernel heap */
        kheap_bottom = kheap_top = p;

        p->size = size;
        p->size += (PAGE_FRAME_SIZE - 1);
        p->size &= (-PAGE_FRAME_SIZE);
        p->size -= sizeof(header_t);
        p->next = 0;
        p->magic = MALLOC_MAGIC;
        p->used = 1;
        
        if(p->size>total_size){
            left_over = (header_t *) ((void *)p + total_size);
            left_over->size = p->size - total_size;
            left_over->next = 0;
            left_over->magic = MALLOC_MAGIC;
            left_over->used = 0;
            kheap_top = left_over;
            p->size = size;
            p->next = left_over;
        }
        
        return (void *) p + sizeof(header_t);
    }


    /* search for unused block of memory */
    for(p=kheap_bottom; p; p=p->next){
        /* we found a free block that is sufficiently large */
        if(!p->used && total_size<p->size) break;
    }

    /* we found a free block */
    if(p){ 
        /* save memory that we dont use */
        left_over = (header_t *) ((void *)p + total_size);
        left_over->size = p->size - total_size;
        left_over->next = p->next;
        //printk("%x\n", left_over->next);
        left_over->magic = MALLOC_MAGIC;
        left_over->used = 0;
        
        
        /* last block? update heap top */
        if(!(left_over->next)) kheap_top = left_over;

        p->size = size;
        p->next = left_over;
        p->magic = MALLOC_MAGIC;
        p->used = 1;
        
        return (void *) p + sizeof(header_t);
    }
    
    /* if we did not find a free block large enough */
    p = morecore(total_size);

    /* kernel heap overflows */
    if((long) p == 0xFFFFFFFF) return 0;

    p->size = size;
    p->size += (PAGE_FRAME_SIZE - 1);
    p->size &= (-PAGE_FRAME_SIZE);
    p->size -= sizeof(header_t);
    p->next = 0;
    p->magic = MALLOC_MAGIC;
    p->used = 1;

    /* link in this new block */
    kheap_top->next = p;
    kheap_top = p;
    

    if(p->size>total_size){
        left_over = (header_t *) ((void *)p + total_size);
        left_over->size = p->size - total_size;
        left_over->next = 0;
        left_over->magic = MALLOC_MAGIC;
        left_over->used = 0;

        kheap_top = left_over;
        p->size = size;
        p->next = left_over;
    }

    return  (void *) p + sizeof(header_t);
}


/*
 * post: frees a block of memory.
 *       returns 0 on success, and -1 on error.
 */
int kfree(void *p)
{
    header_t *header;

    /* we need to take the header into consideration */
    p = (void *) p - sizeof(header_t);
    header = (header_t *) p;

    if(header->magic!=MALLOC_MAGIC){
        printk("kfree: illegal free at: %x. (illegal magic number)\n", p);
        return -1;
    }

    /* search the block to free */
    for(header=kheap_bottom; header; header=header->next){
        if(header == p) break;
    }
    
    /* did not find */
    if(!header){
        printk("kfree: illegal free at: %x. (not found in heap)\n", p);
        return -1;
    }

    /* free it */
    header->used = 0;

    /* combine free blocks */
    for(header=kheap_bottom; header; header=header->next){
        while(!(header->used) && (header->next) && !(header->next->used)){
            header->size += sizeof(header_t) + header->next->size;
            header->next = header->next->next;
        }
    }
    
    return 0;
}


