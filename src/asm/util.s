;;
;; util.s : asm utilities
;;
;; ------------------------------------------------------------------
;;
;; @begin[license]
;; Copyright (C) 2003  Kai Chen, Caltech
;;
;; This program is free software; you can redistribute it and/or
;; modify it under the terms of the GNU General Public License
;; as published by the Free Software Foundation; either version 2
;; of the License, or (at your option) any later version.
;;
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;;
;; You should have received a copy of the GNU General Public License
;; along with this program; if not, write to the Free Software
;; Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
;;
;; @author: Kai Chen
;; @email{kchen@cs.caltech.edu}
;; @end[license]
;;
;;

[GLOBAL _context_switch]
[GLOBAL _set_tss_desc]
[GLOBAL _get_sp]
[GLOBAL printk]                     ;; This routine is exported to kio.h
[GLOBAL _read_cr0]
[GLOBAL _write_cr0]
[GLOBAL _read_cr3]
[GLOBAL _write_cr3]
[GLOBAL _reboot]


GDT_BASE                EQU     0x500
GDT_TSS_LIMIT           EQU     103
GDT_TSS_TYPE            EQU     0x89

TSS_LIMIT_0_15_OFF      EQU     0
TSS_BASE_0_15_OFF       EQU     2
TSS_BASE_16_23_OFF      EQU     4
TSS_TYPE_OFF            EQU     5
TSS_LIMIT_16_19_OFF     EQU     6
TSS_BASE_24_31_OFF      EQU     7


[BITS 32]

[SEGMENT .text]


;;  This routine performs the context switch
_context_switch:
    PUSH        EBP
    MOV         EBP, ESP

    JMP dword far    [EBP+8]

    MOV         ESP, EBP
    POP         EBP
    RET

    

;;  This routine sets the TSS descriptor in GDT: _set_tss_desc(addr, gdt_index)
_set_tss_desc:
    PUSH        EBP
    MOV         EBP, ESP
    PUSHA

    MOV         EAX, [EBP+8]
    MOV         EBX, [EBP+12]

    SHL         EBX, 3
    ADD         EBX, GDT_BASE

    MOV word    [EBX+TSS_LIMIT_0_15_OFF], GDT_TSS_LIMIT
    MOV         [EBX+TSS_BASE_0_15_OFF], AX
    SHR         EAX, 16
    MOV         [EBX+TSS_BASE_16_23_OFF], AL
    MOV byte    [EBX+TSS_TYPE_OFF], GDT_TSS_TYPE
    MOV byte    [EBX+TSS_LIMIT_16_19_OFF], 0
    MOV         [EBX+TSS_BASE_24_31_OFF], AH
    
    POPA
    MOV         ESP, EBP
    POP         EBP
    RET

;;  This routine returns the stack pointer
_get_sp:
    MOV         EAX, ESP
    RET


[EXTERN vprintk]

;;  This routine figures out the argument list and calls vprintk
printk:
    PUSH        EBP
    MOV         EBP, ESP
    
    MOV         EAX, EBP            ;; EBP points to EBP
    ADD         EAX, 12             ;; EBP + 4 points to the return address
    PUSH        EAX                 ;; EBP + 8 points to the first argument fmt
                                    ;; EBP + 12 points to the second argument
    PUSH        dword [EBP+8]

    CALL        vprintk
    
    MOV         ESP, EBP
    POP         EBP
    RET



;;  This routine reads in the value of CR0
_read_cr0:
    MOV         EAX, CR0
    RET


;;  This routine writes to CR0
_write_cr0:
    PUSH        EBP
    MOV         EBP, ESP
    MOV         EAX, [EBP+8]
    MOV         CR0, EAX
    POP         EBP
    RET


;;  This routine reads from CR3
_read_cr3:
    MOV         EAX, CR3
    RET


;;  This routine writes to CR3
_write_cr3:
    PUSH        EBP
    MOV         EBP, ESP
    MOV         EAX, [EBP+8]
    MOV         CR3, EAX
    POP         EBP
    RET

;;  This routine reboots the machine by asking the keyboard controller
;;  to pulse the reset line low
;;  This might be problematic

_reboot:
WKC:
    XOR         AL, AL
    IN          AL, 0x64
    TEST        AL, 0x02
    JNZ         WKC

    MOV         AL, 0xFC
    OUT         0x64, AL


[SEGMENT .data]


