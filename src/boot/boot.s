;;
;;  boot.s -- Boot Sector for K-OS
;;
;;  ------------------------------------------------------------------
;;
;;  @begin[license]
;;  Copyright (C) 2003  Kai Chen, Caltech
;;
;;  This program is free software; you can redistribute it and/or
;;  modify it under the terms of the GNU General Public License
;;  as published by the Free Software Foundation; either version 2
;;  of the License, or (at your option) any later version.
;;
;;  This program is distributed in the hope that it will be useful,
;;  but WITHOUT ANY WARRANTY; without even the implied warranty of
;;  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;  GNU General Public License for more details.
;;
;;  You should have received a copy of the GNU General Public License
;;  along with this program; if not, write to the Free Software
;;  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
;;
;;  @author: Kai Chen
;;  @email{kchen@cs.caltech.edu}
;;  @end[license]
;;
;;

[BITS 16]
[ORG 0x7C00]                            ; BIOS copies boot sector to 0x7C00


;;  GDT Base Address
GDT_BASE        EQU     0x500
GDT_SIZE        EQU     0x1000

;;  Kernel address
KERNEL_ADDR     EQU     0x10000
KERNEL_SEG      EQU     0x1000


;;  Number of bytes per sector
BYTES_PER_SECTOR    EQU 0x200
SECTORS_PER_TRACK   EQU 0x12


;;  Jump to the starting location
    JMP         Start

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;  Data                                   ;;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    bootdrv     db 0
    rebootmsg   db 'reboot', 13, 10, 0
    ldkernelmsg db 'Loading K-OS Kernel ', 32, 0
    dotmsg      db '.',32,0
    pmodemsg    db '13','10','Press any key ', 13, 10, 0
    

;;  Global Descriptor Table
;;
;;  In protected mode, segment registers act as *selectors*
;;  Bits 3 - 15 are used as index
;;  We use LDT instead of GDT if bit 2 is set. Clear it for our purpose.
;;  Bits 1 - 2 (RPL) are REQUESTOR PRIVILEDGE LEVEL. Clear those
;;  NULL_SEL        EQU     0x0             ; 0000 0000 0000 0000
;;  CODE_SEL        EQU     0x8             ; 0000 0000 0000 1000
;;  DATA_SEL        EQU     0x10            ; 0000 0000 0001 0000

gdtr:           
GDTLimit        dw GDT_SIZE - 1         ; GDT limit = GDT size-1
                                        ; GDT size = #descriptors*8
GDTbase         dd GDT_BASE
gdt:
NULL_SEL        EQU     $-gdt
                dw 0                    ; null descriptor
                dw 0                    ; all entries zero
                db 0
                db 0
                db 0
                db 0
CODE_SEL        EQU     $-gdt
                dw 0xFFFF               ; code segment, limit=0xFFFF
                dw 0                    ; base = 0
                db 0                    ; code read/exec (0x9A00)
                db 0x9A
                db 0xCF                 ; 32-bit granularity (0x00CF)
                db 0
DATA_SEL        EQU     $-gdt
                dw 0xFFFF               ; data/stack segment, limit=0xFFFF
                dw 0                    ; base = 0
                db 0                    ; data read/write (0x9200)
                db 0x92
                db 0xCF                 ; 32-bit granularity (0x00CF)
                db 0
gdt_end:



;;
;;  Start executing the boot sector
;;  BIOS has finished POST (Power-On Self Test)
;;  and copied the boot sector to memory location 0x7C00
;;  Currently the state of the registers are:
;;      
;;      DL = Boot drive, 1H = floopy, 80H = primary harddrive ...
;;      CS = 0x7C00
;;      IP = 0x7C00
;;

Start:
    MOV         AX, CS                  ; CS = BOOT_SEG, our data starts here.
    MOV         DS, AX                  ; So we set up DS to point to that
    MOV         ES, AX
    MOV         FS, AX

    MOV         [bootdrv], DL           ; Save where we boot from
    
    CLI                                 ; Clear interrupts before we set up stack
    MOV         AX, 0x1D0               ; Typical location for stack
    MOV         SS, AX
    MOV         SP, 0x200               ; Use the whole segment.
    STI

    MOV         AX, 0xB800              ; Video segment
    MOV         GS, AX

;    MOV         SI, bootmsg     
;    CALL        Print

    CALL        TestCPU
   
.386                                    ; Assume 386+ from now on.
    



;;
;;  Now we enable A20 line
;;

    CLI                                 ; Disable interrupts
    CALL        WKC
    MOV         AL, 0xD1                ; Command write
    OUT         0x64, AL

    CALL        WKC
    MOV         AL, 0xDF                ; A20 on
    OUT         0x60, AL
    
    CALL        WKC                     ; wait for the kbd to clear
    
    MOV         CX, 0x10                ; Wait for the command to be executed
WaitKBC:
    OUT         0xED, AX        
    LOOP        WaitKBC

;;  Test if A20 was really enabled
    MOV         AL, 0xd0
    OUT         0x64, AL
    CALL        WKF
    IN          AL, 0x60
    TEST        AL, 2
    JNZ         A20Success
    CALL        Reboot


A20Success:
    STI



;;  A20 enabled
;    MOV         SI, a20msg
;    CALL        Print

;;
;;  Load kernel here
;;
;;  AH = BIOS function
;;  AL = Number of sectors to read
;;  ES:BX = SEGMENT:OFFSET - memory location to read into
;;  CH = Track number
;;  CL = Starting Sector
;;  DH = Head Number
;;  DL = Drive number
;;  INT 0x13 sets CF on error, AH = sectors that were read in
;;
;;
    MOV         SI, ldkernelmsg         ; loading kernel
    CALL        Print

    
    MOV         SI, KERNEL_SIZE         ; # of sectors
    XOR         CH, CH                  ; Cylinder
    MOV         CL, 0x02                ; Sector
    XOR         DH, DH                  ; Head
    MOV         DL, [bootdrv]           ; boot driver
    MOV         AX, KERNEL_SEG          ; Kernel segment
    MOV         ES, AX          
    XOR         BX, BX                  ; Kernel offset

LOAD_KERNEL_TOP_LOOP:
    CMP         SI, 0x0                 ; Have we read in everything?
    JLE         LOAD_KERNEL_SUCCESS

    XOR         DI, DI                  ; We try 5 times before giving up

LOAD_KERNEL_ATTEMPT_LOOP:
    INC         DI

    CMP         DI, 0x5                 ; How many times have we tried?
    JG          LOAD_KERNEL_FAILURE

    XOR         AX, AX                  ; Reset disk
    INT         0x13
    JC          LOAD_KERNEL_FAILURE

    MOV         AH, 0x2                 ; Read a sector
    MOV         AL, 0x1
    INT         0x13
    JC          LOAD_KERNEL_ATTEMPT_LOOP

;;  We have just read a sector

;;;;;;;;;; print a dot ;;;;;;;;;;;
    PUSHA
    MOV         SI, dotmsg
    CALL        Print
    POPA
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    DEC         SI
    ADD         BX, BYTES_PER_SECTOR    ; Update offset

    INC         CL
    CMP         CL, SECTORS_PER_TRACK   ; Finished current track?
    JLE         LOAD_KERNEL_TOP_LOOP    ; Not yet done with the current track

;;  Now we have finished reading the current track
    MOV         CL, 0x1                 ; There is no boot sector here, 
                                        ; so start with sector 1
    CMP         DL, 0x0                 ; Head position
    JNE         LOAD_KERNEL_NEW_CYLINDER

    MOV         DH, 0x1
    JMP         LOAD_KERNEL_TOP_LOOP

LOAD_KERNEL_NEW_CYLINDER:
    XOR         DH, DH
    INC         CH
    JMP         LOAD_KERNEL_TOP_LOOP

LOAD_KERNEL_SUCCESS:
    MOV         DL, [bootdrv]
    XOR         AX, AX
    INT         0x13
    JMP         LOAD_KERNEL_END
    
LOAD_KERNEL_FAILURE:
    MOV         DL, [bootdrv]
    XOR         AX, AX
    INT         0x13
    CALL        Reboot


LOAD_KERNEL_END:


;;  And we turn off the floppy driver's motor
    MOV         DX, 0x3F2
    MOV         AL, 0x0C
    OUT         DX, AL
   
;;  Copy GDT to it's location   
    XOR         AX, AX
    MOV         DS, AX
    MOV         ES, AX
    MOV         SI, gdt
    MOV         DI, GDT_BASE
    MOV         CX, gdt_end-gdt+1
    CLD
    REP         MOVSB

;;  Load GDT here
    LGDT        [gdtr]


;;
;;  Now let's enter the protected mode
;; 
    CLI
    MOV         SI, pmodemsg
    CALL        Print
;;    CALL        Getkey

    MOV         EAX, CR0                ; Load control register
    OR          AL, 1                   ; Set bit 1: protected mode bit
    MOV         CR0, EAX                ; Copy back to control register


;;  Start running the kernel
;;  Notice that this instruction also flushes CS/IP/EIP
    JMP         CODE_SEL:FlushReg

;;  
;;  This routine blocks until the keyboard command queue is empty.
;;  If something goes wrong, it will loop forever, but we can do little
;;  in that case anyways ...
;;
WKC:
    XOR         AL, AL
    IN          AL, 0x64                ; Read from 8042 status port
    TEST        AL, 0x02                ; Check if full
    JNZ         WKC                     ; If not empty, loop
    RET

;;
;;  This routine blocks until the keyboard command queue is full
;;
WKF:
    XOR         CX, CX
    IN          AL, 0x64
    TEST        AL, 1
    JZ          WKF
    RET

;;
;;  Test if CPU is 386 or above
;;  Bits 12 - 15 set if 8088
;;  Bits 12 - 15 cleared if 80286
;;
TestCPU:

    PUSHF                               ; Copy ax into the flags 
    XOR         AH, AH                  ; with bits 12-15 cleared
    PUSH        AX
    POPF

    PUSHF                               ; Read flags back into AX
    POP         AX              
    AND         AH, 0f0H                ; Check if bits 12-15 are set
    CMP         AH, 0f0H
    JE          CPUFailure              ; (8088/8086)

    MOV         AH, 0f0H                ; Set bits 12-15
    PUSH        AX
    POPF

    PUSHF                               ; Copy flags back into AX
    POP         AX
    AND         AH, 0F0h                ; Test if bits 12-15 are cleared
    JZ          CPUFailure              ; (80286)
    POPF
    
    RET

CPUFailure:
    CALL        Reboot                  ; And reboot


;;  Printing a message to the screen
Print:
    LODSB                               ; load byte at ds:si into AL
    OR          AL, AL                  ; test for null (0)
    JZ          PrintEnd                ; end of string
    MOV         AH, 0eh                 ; put a character
    MOV         BX, 0007                ; attribute, black bg, white fg
    INT         0x10                    ; call BIOS
    JMP         Print
PrintEnd:
    RET


;;  Getting key stroke
Getkey:
    MOV         AH, 0                   ; wait for key
    INT         016h
    RET

    
;;  Reboot
Reboot:
    MOV         SI, rebootmsg           ; Display a reboot message
    CALL        Print
    CALL        Getkey                  ; Reboot after any key press

    db          0EAh                    ; Jump to FFFF:0000 (reboot)
    dw          0000h
    dw          0FFFFh


;;  We are now in 32 bits protected mode
[bits 32]
FlushReg:
    MOV         EAX, DATA_SEL
    MOV         DS, EAX
    MOV         ES, EAX
    MOV         FS, EAX
    MOV         GS, EAX
    MOV         SS, EAX
    MOV         ESP, 0xFFFF

;;  Jump to the kernel 

    JMP         CODE_SEL:KERNEL_ADDR
    

	
;;  Boot Sector has 512 bytes, ending with the word 0xAA55
    times       510-($-$$) db 0
    dw          0xAA55
