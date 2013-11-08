


%include "x86inc.asm"


SECTION .text align=16


cglobal test_instruction_timer, 0, 7

	RDTSC
	MOV r3, r0
	MOV r4, r1

	;MOV r6, [zero_to_32_1]
	;IMUL r6, r5
%rep 100
	IMUL r6, r5
%endrep

	RDTSC

	MOV r5, r0m
	MOV [r5  ], r3
	MOV [r5+4], r4

	MOV r5, r1m
	MOV [r5  ], r0
	MOV [r5+4], r1

	RET
