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
Main:
		mov		#0x89AB,R12
		mov		#string,R13
		call 	#W16_ASC
		jmp 	$

W16_ASC:
		push R4			;mask
		push R5			;save R12
		push R6			;N
		;mover o ponteiro para o final da string,escrever 0
		add #4,R13
		mov #0,0(R13)
		dec	R13

		mov	#0x000F,R4		;mask
		mov #4,R6			;N
		mov R12,R5			;save R12
W16_ASC_loop:
		and	R4,R12			;nibble
		cmp #10,R12
		jhs W16_ASC_letra				;salto está certo?
W16_ASC_numero:
		add #0x30,R12
		jmp	W16_ASC_salvar
W16_ASC_letra:
        add #0x37,R12
W16_ASC_salvar:

		mov.b R12,0(R13)				;salvar valor na string
		dec	 R13
		call #subroot_desloca4bits
		mov R5,R12
		dec R6
		jnz W16_ASC_loop
		pop	R6
		pop	R5
		pop	R4
		ret


subroot_desloca4bits:
		clrc
		rrc R5
		rrc R5
		rrc R5
		rrc R5
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
            


			.data
string:		.byte 0,0,0,0,0
