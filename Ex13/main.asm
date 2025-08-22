;-------------------------------------------------------------------------------
; MSP430 Assembler Code Template for use with TI Code Composer Studio
;
;
;-------------------------------------------------------------------------------
            .cdecls C,LIST,"msp430.h"       ; Include device header file
            
;-------------------------------------------------------------------------------
            .def    RESET                   ; Export program entry-point to
                                            ; make it known to linker.
;-------------------------------------------------------------------------------
            .text                           ; Assemble into program memory.
            .retain                         ; Override ELF conditional linking
                                            ; and retain current section.
            .retainrefs                     ; And retain any sections that have
                                            ; references to current section.

;-------------------------------------------------------------------------------
RESET       mov.w   #__STACK_END,SP         ; Initialize stackpointer
StopWDT     mov.w   #WDTPW|WDTHOLD,&WDTCTL  ; Stop watchdog timer


;-------------------------------------------------------------------------------
; Main loop here
;-------------------------------------------------------------------------------
main:
	call #mapSum16
                                            
;-------------------------------------------------------------------------------
; Subrotina
;-------------------------------------------------------------------------------
mapSum16:
		push R5
		push R6
		push R7

		mov #2400,R5 ;v1
		mov #2410,R6 ;v2
		mov #2420,R7 ;sum
		mov #0,R12
		mov @R5+,R13
		mov R13,0(R7)
		inc.w R7
mapSum16_loop:
		mov @R5+,R12
		add @R6+,R12
		mov R12,0(R7)
		inc.w R7
		mov #0,R12
		dec R13
		jnz mapSum16_loop
		pop R7
		pop R6
		pop R5
		ret


;-------------------------------------------------------------------------------
; Stack Pointer definition
;-------------------------------------------------------------------------------
            .global __STACK_END
            .sect   .stack
            
;-------------------------------------------------------------------------------
; Interrupt Vectors
;-------------------------------------------------------------------------------
            .sect   ".reset"                ; MSP430 RESET Vector
            .short  RESET
            
