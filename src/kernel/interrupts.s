;;
;; interrupts.s : More interrupt handler wrappers.
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


PIC_MASTER_CODE     EQU         0x20
PIC_SLAVE_CODE      EQU         0xA0
EOI                 EQU         0x20

[EXTERN blue_screen]

;;  Now some other handlers so that we know which interrupt we get.
[GLOBAL INT00Handler]
INT00Handler:
    PUSHAD
    PUSH            DS
    PUSH            ES
    PUSH            FS
    PUSH            GS

    PUSH dword      0

    CALL            blue_screen

    ADD             ESP, 4

    MOV             AL, EOI
    OUT             PIC_MASTER_CODE, AL
    OUT             PIC_SLAVE_CODE, AL

    POP             GS
    POP             FS
    POP             ES
    POP             DS

    IRET


[GLOBAL INT01Handler]
INT01Handler:
    PUSHAD
    PUSH            DS
    PUSH            ES
    PUSH            FS
    PUSH            GS

    PUSH dword      1

    CALL            blue_screen

    ADD             ESP, 4

    MOV             AL, EOI
    OUT             PIC_MASTER_CODE, AL
    OUT             PIC_SLAVE_CODE, AL

    POP             GS
    POP             FS
    POP             ES
    POP             DS

    IRET


[GLOBAL INT02Handler]
INT02Handler:
    PUSHAD
    PUSH            DS
    PUSH            ES
    PUSH            FS
    PUSH            GS

    PUSH dword      2

    CALL            blue_screen

    ADD             ESP, 4

    MOV             AL, EOI
    OUT             PIC_MASTER_CODE, AL
    OUT             PIC_SLAVE_CODE, AL

    POP             GS
    POP             FS
    POP             ES
    POP             DS

    IRET


[GLOBAL INT03Handler]
INT03Handler:
    PUSHAD
    PUSH            DS
    PUSH            ES
    PUSH            FS
    PUSH            GS

    PUSH dword      3

    CALL            blue_screen

    ADD             ESP, 4

    MOV             AL, EOI
    OUT             PIC_MASTER_CODE, AL
    OUT             PIC_SLAVE_CODE, AL

    POP             GS
    POP             FS
    POP             ES
    POP             DS

    IRET

[GLOBAL INT04Handler]
INT04Handler:
    PUSHAD
    PUSH            DS
    PUSH            ES
    PUSH            FS
    PUSH            GS

    PUSH dword      4

    CALL            blue_screen

    ADD             ESP, 4

    MOV             AL, EOI
    OUT             PIC_MASTER_CODE, AL
    OUT             PIC_SLAVE_CODE, AL

    POP             GS
    POP             FS
    POP             ES
    POP             DS

    IRET


[GLOBAL INT05Handler]
INT05Handler:
    PUSHAD
    PUSH            DS
    PUSH            ES
    PUSH            FS
    PUSH            GS

    PUSH dword      5

    CALL            blue_screen

    ADD             ESP, 4

    MOV             AL, EOI
    OUT             PIC_MASTER_CODE, AL
    OUT             PIC_SLAVE_CODE, AL

    POP             GS
    POP             FS
    POP             ES
    POP             DS

    IRET


[GLOBAL INT06Handler]
INT06Handler:
    PUSHAD
    PUSH            DS
    PUSH            ES
    PUSH            FS
    PUSH            GS

    PUSH dword      6

    CALL            blue_screen

    ADD             ESP, 4

    MOV             AL, EOI
    OUT             PIC_MASTER_CODE, AL
    OUT             PIC_SLAVE_CODE, AL

    POP             GS
    POP             FS
    POP             ES
    POP             DS

    IRET


[GLOBAL INT07Handler]
INT07Handler:
    PUSHAD
    PUSH            DS
    PUSH            ES
    PUSH            FS
    PUSH            GS

    PUSH dword      7

    CALL            blue_screen

    ADD             ESP, 4

    MOV             AL, EOI
    OUT             PIC_MASTER_CODE, AL
    OUT             PIC_SLAVE_CODE, AL

    POP             GS
    POP             FS
    POP             ES
    POP             DS

    IRET


[GLOBAL INT08Handler]
INT08Handler:
    PUSHAD
    PUSH            DS
    PUSH            ES
    PUSH            FS
    PUSH            GS

    PUSH dword      8

    CALL            blue_screen

    ADD             ESP, 4

    MOV             AL, EOI
    OUT             PIC_MASTER_CODE, AL
    OUT             PIC_SLAVE_CODE, AL

    POP             GS
    POP             FS
    POP             ES
    POP             DS

    IRET

[GLOBAL INT09Handler]
INT09Handler:
    PUSHAD
    PUSH            DS
    PUSH            ES
    PUSH            FS
    PUSH            GS

    PUSH dword      9

    CALL            blue_screen

    ADD             ESP, 4

    MOV             AL, EOI
    OUT             PIC_MASTER_CODE, AL
    OUT             PIC_SLAVE_CODE, AL

    POP             GS
    POP             FS
    POP             ES
    POP             DS

    IRET


[GLOBAL INT10Handler]
INT10Handler:
    PUSHAD
    PUSH            DS
    PUSH            ES
    PUSH            FS
    PUSH            GS

    PUSH dword      10

    CALL            blue_screen

    ADD             ESP, 4

    MOV             AL, EOI
    OUT             PIC_MASTER_CODE, AL
    OUT             PIC_SLAVE_CODE, AL

    POP             GS
    POP             FS
    POP             ES
    POP             DS

    IRET


[GLOBAL INT11Handler]
INT11Handler:
    PUSHAD
    PUSH            DS
    PUSH            ES
    PUSH            FS
    PUSH            GS

    PUSH dword      11

    CALL            blue_screen

    ADD             ESP, 4

    MOV             AL, EOI
    OUT             PIC_MASTER_CODE, AL
    OUT             PIC_SLAVE_CODE, AL

    POP             GS
    POP             FS
    POP             ES
    POP             DS

    IRET


[GLOBAL INT12Handler]
INT12Handler:
    PUSHAD
    PUSH            DS
    PUSH            ES
    PUSH            FS
    PUSH            GS

    PUSH dword      12

    CALL            blue_screen

    ADD             ESP, 4

    MOV             AL, EOI
    OUT             PIC_MASTER_CODE, AL
    OUT             PIC_SLAVE_CODE, AL

    POP             GS
    POP             FS
    POP             ES
    POP             DS

    IRET


[GLOBAL INT13Handler]
INT13Handler:
    PUSHAD
    PUSH            DS
    PUSH            ES
    PUSH            FS
    PUSH            GS

    PUSH dword      13

    CALL            blue_screen

    ADD             ESP, 4

    MOV             AL, EOI
    OUT             PIC_MASTER_CODE, AL
    OUT             PIC_SLAVE_CODE, AL

    POP             GS
    POP             FS
    POP             ES
    POP             DS

    IRET


[GLOBAL INT14Handler]
INT14Handler:
    PUSHAD
    PUSH            DS
    PUSH            ES
    PUSH            FS
    PUSH            GS

    PUSH dword      14

    CALL            blue_screen

    ADD             ESP, 4

    MOV             AL, EOI
    OUT             PIC_MASTER_CODE, AL
    OUT             PIC_SLAVE_CODE, AL

    POP             GS
    POP             FS
    POP             ES
    POP             DS

    IRET


[GLOBAL INT16Handler]
INT16Handler:
    PUSHAD
    PUSH            DS
    PUSH            ES
    PUSH            FS
    PUSH            GS

    PUSH dword      16

    CALL            blue_screen

    ADD             ESP, 4

    MOV             AL, EOI
    OUT             PIC_MASTER_CODE, AL
    OUT             PIC_SLAVE_CODE, AL

    POP             GS
    POP             FS
    POP             ES
    POP             DS

    IRET


[GLOBAL INT17Handler]
INT17Handler:
    PUSHAD
    PUSH            DS
    PUSH            ES
    PUSH            FS
    PUSH            GS

    PUSH dword      17

    CALL            blue_screen

    ADD             ESP, 4

    MOV             AL, EOI
    OUT             PIC_MASTER_CODE, AL
    OUT             PIC_SLAVE_CODE, AL

    POP             GS
    POP             FS
    POP             ES
    POP             DS

    IRET


[GLOBAL INT18Handler]
INT18Handler:
    PUSHAD
    PUSH            DS
    PUSH            ES
    PUSH            FS
    PUSH            GS

    PUSH dword      18

    CALL            blue_screen

    ADD             ESP, 4

    MOV             AL, EOI
    OUT             PIC_MASTER_CODE, AL
    OUT             PIC_SLAVE_CODE, AL

    POP             GS
    POP             FS
    POP             ES
    POP             DS

    IRET


[GLOBAL INT19Handler]
INT19Handler:
    PUSHAD
    PUSH            DS
    PUSH            ES
    PUSH            FS
    PUSH            GS

    PUSH dword      19

    CALL            blue_screen

    ADD             ESP, 4

    MOV             AL, EOI
    OUT             PIC_MASTER_CODE, AL
    OUT             PIC_SLAVE_CODE, AL

    POP             GS
    POP             FS
    POP             ES
    POP             DS

    IRET


