

%include "x86inc.asm"


SECTION .text align=16

hor_HADAMARD_coeff_1_1			DB		 0,  1,  4,  5,  8,  9, 12, 13
								DB		 0,  0,  0,  0,  0,  0,  0,  0
hor_HADAMARD_coeff_1_2			DB		 2,  3,  6,  7,  10, 11, 14, 15
								DB		 0,  0,  0,  0,  0,  0,  0,  0

hor_HADAMARD_coeff_2_1			DB		 0,  1,  2,  3,  8,  9, 10, 11
								DB		 0,  0,  0,  0,  0,  0,  0,  0

hor_HADAMARD_coeff_2_2			DB		 4,  5,  6,  7, 12, 13, 14, 15
								DB		 0,  0,  0,  0,  0,  0,  0,  0

mul_coeff_one					DW		 1,  1,  1,  1,  1,  1,  1,  1



%macro ABS 2
	PXOR %2, %2
	PSUBW %2, %1
	PMAXSW %1, %2
%endmacro


%macro ABS_AND_ADD_MMX2 0
	ABS MM0, MM4
	ABS MM1, MM4
	ABS MM2, MM4
	ABS MM3, MM4

	MOVQ MM4, [mul_coeff_one]
	PMADDWD MM0, MM4
	PMADDWD MM1, MM4
	PMADDWD MM2, MM4
	PMADDWD MM3, MM4

	PADDD MM0, MM1
	PADDD MM0, MM2
	PADDD MM0, MM3

	MOVQ MM4, MM0
	PSRLQ MM4, 32
	PADDD MM0, MM4
%endmacro

%macro ABS_AND_ADD_1_SSSE3 0
	ABS XMM7, XMM0
	ABS XMM6, XMM0
	ABS XMM5, XMM0
	MOVAPS XMM0, [mul_coeff_one]
	PMADDWD XMM7, XMM0
	PMADDWD XMM6, XMM0
	PMADDWD XMM5, XMM0
	PADDD XMM5, XMM7
	PADDD XMM5, XMM6
	MOVAPS XMM6, XMM0
%endmacro

%macro ABS_AND_ADD_2_SSSE3 0
	ABS XMM4, XMM7
	ABS XMM3, XMM7
	ABS XMM2, XMM7
	ABS XMM1, XMM7
	ABS XMM0, XMM7
	PMADDWD XMM4, XMM6
	PMADDWD XMM3, XMM6
	PMADDWD XMM2, XMM6
	PMADDWD XMM1, XMM6
	PMADDWD XMM0, XMM6
	PADDD XMM5, XMM4
	PADDD XMM5, XMM3
	PADDD XMM5, XMM2
	PADDD XMM5, XMM1
	PADDD XMM5, XMM0

	MOVHLPS XMM7, XMM5
	PADDD XMM5, XMM7

	MOVAPS XMM7, XMM5
	PSHUFD XMM7, XMM7, 01H
	PADDD XMM5, XMM7
%endmacro

%macro LOAD_DIFF_MMX2 4

	MOV r1, r1m
	IMUL r1, %4
	MOVD %2, [cur+r1]
	PUNPCKLBW %2, %3

	MOV r1, r3m
	IMUL r1, %4
	MOVD %1, [org+r1]
	PUNPCKLBW %1, %3

	PSUBW %1, %2
%endmacro

%macro LOAD_DIFF_SSSE3 4
	MOV r1, r1m
	IMUL r1, %4
	MOVQ %2, [cur+r1]
	PUNPCKLBW %2, %3

	MOV r1, r3m
	IMUL r1, %4
	MOVQ %1, [org+r1]
	PUNPCKLBW %1, %3

	PSUBW %1, %2
%endmacro

%macro LOAD_4_DIFF_MMX2 0
	LOAD_DIFF_MMX2 MM0, MM6, MM7, 0
	LOAD_DIFF_MMX2 MM1, MM6, MM7, 1
	LOAD_DIFF_MMX2 MM2, MM6, MM7, 2
	LOAD_DIFF_MMX2 MM3, MM6, MM7, 3
%endmacro

%macro LOAD_8_DIFF_SSSE3 0
	LOAD_DIFF_SSSE3 XMM0, XMM6, XMM7, 0
	LOAD_DIFF_SSSE3 XMM1, XMM6, XMM7, 1
	LOAD_DIFF_SSSE3 XMM2, XMM6, XMM7, 2
	LOAD_DIFF_SSSE3 XMM3, XMM6, XMM7, 3
	MOVAPS R1, XMM0
	MOVAPS R2, XMM1
	MOVAPS R3, XMM2
	MOVAPS R4, XMM3

	LOAD_DIFF_SSSE3 XMM0, XMM6, XMM7, 4
	LOAD_DIFF_SSSE3 XMM1, XMM6, XMM7, 5
	LOAD_DIFF_SSSE3 XMM2, XMM6, XMM7, 6
	LOAD_DIFF_SSSE3 XMM3, XMM6, XMM7, 7
%endmacro

%macro HADAMARD_HOR_MMX2		6
	MOVQ %3, %1
	PSHUFB %1, %4
	MOVQ %2, %1
	PSHUFB %3, %5
	PADDW %1, %3
	PSUBW %2, %3

	PUNPCKL%6 %1, %2
%endmacro

%macro HADAMARD_HOR_SSSE3		6
	MOVAPS %3, %1
	PSHUFB %1, %4
	MOVAPS %2, %1
	PSHUFB %3, %5
	PADDW %1, %3
	PSUBW %2, %3
	PUNPCKL%6 %1, %2
%endmacro

%macro HADAMARD_HOR_4_MMX2	3
	MOVQ %3, %1
	PUNPCKLDQ %1, %1
	MOVQ %2, %1
	PUNPCKHDQ %3, %3
	PADDW %1, %3
	PSUBW %2, %3

	PUNPCKLDQ %1, %2
%endmacro

%macro HADAMARD_HOR_4_SSSE3	3
	MOVAPS %3, %1
	PUNPCKLQDQ %1, %1
	MOVAPS %2, %1
	PUNPCKHQDQ %3, %3
	PADDW %1, %3
	PSUBW %2, %3

	PUNPCKLQDQ %1, %2
%endmacro

%macro	HADAMARD_4x4_MMX2		3
	MOVQ MM7, %1
	MOVQ MM6, %2

	HADAMARD_HOR_MMX2 MM0, MM4, MM5, MM6, MM7, %3
	HADAMARD_HOR_MMX2 MM1, MM4, MM5, MM6, MM7, %3
	HADAMARD_HOR_MMX2 MM2, MM4, MM5, MM6, MM7, %3
	HADAMARD_HOR_MMX2 MM3, MM4, MM5, MM6, MM7, %3
%endmacro

%macro	HADAMARD_8x4_SSSE3		3
	MOVAPS XMM7, %1
	MOVAPS XMM6, %2

	HADAMARD_HOR_SSSE3 XMM0, XMM4, XMM5, XMM6, XMM7, %3
	HADAMARD_HOR_SSSE3 XMM1, XMM4, XMM5, XMM6, XMM7, %3
	HADAMARD_HOR_SSSE3 XMM2, XMM4, XMM5, XMM6, XMM7, %3
	HADAMARD_HOR_SSSE3 XMM3, XMM4, XMM5, XMM6, XMM7, %3
%endmacro

%macro	HADAMARD_4x4_4_MMX2 0
	HADAMARD_HOR_4_MMX2 MM0, MM4, MM5
	HADAMARD_HOR_4_MMX2 MM1, MM4, MM5
	HADAMARD_HOR_4_MMX2 MM2, MM4, MM5
	HADAMARD_HOR_4_MMX2 MM3, MM4, MM5
%endmacro

%macro	HADAMARD_8x4_4_SSSE3 0
	HADAMARD_HOR_4_SSSE3 XMM0, XMM4, XMM5
	HADAMARD_HOR_4_SSSE3 XMM1, XMM4, XMM5
	HADAMARD_HOR_4_SSSE3 XMM2, XMM4, XMM5
	HADAMARD_HOR_4_SSSE3 XMM3, XMM4, XMM5
%endmacro

%macro	HADAMARD_4x4_H_MMX2 0
	HADAMARD_4x4_MMX2 [hor_HADAMARD_coeff_1_1], [hor_HADAMARD_coeff_1_2], WD
	HADAMARD_4x4_4_MMX2
%endmacro


%macro	HADAMARD_8x8_H_SSSE3 0
	HADAMARD_8x4_SSSE3 [hor_HADAMARD_coeff_1_1], [hor_HADAMARD_coeff_1_2], WD
	HADAMARD_8x4_SSSE3 [hor_HADAMARD_coeff_2_1], [hor_HADAMARD_coeff_2_2], DQ
	HADAMARD_8x4_4_SSSE3

	MOVAPS R5, XMM0
	MOVAPS R6, XMM1
	MOVAPS R7, XMM2
	MOVAPS R8, XMM3

	MOVAPS XMM0, R1
	MOVAPS XMM1, R2
	MOVAPS XMM2, R3
	MOVAPS XMM3, R4

	HADAMARD_8x4_SSSE3 [hor_HADAMARD_coeff_1_1], [hor_HADAMARD_coeff_1_2], WD
	HADAMARD_8x4_SSSE3 [hor_HADAMARD_coeff_2_1], [hor_HADAMARD_coeff_2_2], DQ
	HADAMARD_8x4_4_SSSE3
%endmacro

%macro SUMSUB_MMX2 3
	MOVQ %3, %2
	MOVQ %2, %1
	PADDW %1, %3
	PSUBW %2, %3
%endmacro

%macro SUMSUB_SSSE3 3
	MOVAPS %3, %2
	MOVAPS %2, %1
	PADDW %1, %3
	PSUBW %2, %3
%endmacro

%macro HADAMARD_4x4_V_MMX2 0
	SUMSUB_MMX2 MM0, MM1, MM4
	SUMSUB_MMX2 MM2, MM3, MM4
	SUMSUB_MMX2 MM0, MM2, MM4
	SUMSUB_MMX2 MM1, MM3, MM4
%endmacro

%macro HADAMARD_8x8_V_SSSE3 0
	SUMSUB_SSSE3 XMM0, XMM1, XMM7
	SUMSUB_SSSE3 XMM2, XMM3, XMM7
	SUMSUB_SSSE3 XMM4, XMM5, XMM7

	SUMSUB_SSSE3 XMM0, XMM2, XMM7
	SUMSUB_SSSE3 XMM1, XMM3, XMM7

	MOVAPS R1, XMM0
	MOVAPS XMM6, R7
	MOVAPS XMM7, R8

	SUMSUB_SSSE3 XMM6, XMM7, XMM0

	SUMSUB_SSSE3 XMM4, XMM6, XMM0
	SUMSUB_SSSE3 XMM5, XMM7, XMM0

	SUMSUB_SSSE3 XMM1, XMM5, XMM0
	SUMSUB_SSSE3 XMM2, XMM6, XMM0
	SUMSUB_SSSE3 XMM3, XMM7, XMM0
	ABS_AND_ADD_1_SSSE3

	MOVAPS XMM0, R1
	SUMSUB_SSSE3 XMM0, XMM4, XMM7
	ABS_AND_ADD_2_SSSE3
%endmacro


%macro PIXEL_HADS_4x4_MMX2 0
	PXOR MM7, MM7
	LOAD_4_DIFF_MMX2

	HADAMARD_4x4_H_MMX2
	HADAMARD_4x4_V_MMX2
	ABS_AND_ADD_MMX2

	MOVD r1, MM0
	ADD r1, 1
	SHR r1, 1
%endmacro


%macro PIXEL_HADS_8x8_SSSE3 0
	PXOR XMM7, XMM7

	LOAD_8_DIFF_SSSE3

	HADAMARD_8x8_H_SSSE3
	MOVAPS XMM4, R5
	MOVAPS XMM5, R6
	HADAMARD_8x8_V_SSSE3

	MOVD r1, XMM5
	ADD r1, 2
	SHR r1, 2
%endmacro

%macro PIXEL_HADS_4x4_HELP_MMX2 3
cglobal %1, 0, 7
	%define		I				r6
	%define		J				r5
	%define		cur				r4
	%define		org				r3

	MOV r0, 0
	MOV I, 0
%%PIXEL_HADS_4x4_HELP_MMX2_I:

	MOV J, 0
%%PIXEL_HADS_4x4_HELP_MMX2_J:
	MOV cur, r0m
	MOV r1, r1m
	IMUL r1, I
	ADD cur, r1
	ADD cur, J

	MOV org, r2m
	MOV r1, r3m
	IMUL r1, I
	ADD org, r1
	ADD org, J

	PIXEL_HADS_4x4_MMX2
	ADD r0, r1

	ADD J, 4
	CMP J, %3
	JL %%PIXEL_HADS_4x4_HELP_MMX2_J

	ADD I, 4
	CMP I, %2
	JL %%PIXEL_HADS_4x4_HELP_MMX2_I

	RET
%endmacro


%macro PIXEL_HADS_8x8_HELP_SSSE3 3
cglobal %1, 0, 7
	%define		I				r6
	%define		J				r5
	%define		cur				r4
	%define		org				r3
	%define		R1				[r2   ]
	%define		R2				[r2+16]
	%define		R3				[r2+32]
	%define		R4				[r2+48]
	%define		R5				[r2+64]
	%define		R6				[r2+80]
	%define		R7				[r2+96]
	%define		R8				[r2+112]

	MOV r2, esp
	AND r2, 0FFFFFFF0H
	SUB r2, 128
	MOV r0, 0

	MOV I, 0
%%PIXEL_HADS_8x8_HELP_SSSE3_I:

	MOV J, 0
%%PIXEL_HADS_8x8_HELP_SSSE3_J:
	MOV cur, r0m
	MOV r1, r1m
	IMUL r1, I
	ADD cur, r1
	ADD cur, J

	MOV org, r2m
	MOV r1, r3m
	IMUL r1, I
	ADD org, r1
	ADD org, J

	PIXEL_HADS_8x8_SSSE3
	ADD r0, r1

	ADD J, 8
	CMP J, %3
	JL %%PIXEL_HADS_8x8_HELP_SSSE3_J

	ADD I, 8
	CMP I, %2
	JL %%PIXEL_HADS_8x8_HELP_SSSE3_I

	RET
%endmacro


PIXEL_HADS_4x4_HELP_MMX2 pixel_hads_12x16_mmx2, 12, 16
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_8x8_ssse3  ,  8,  8
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_32x32_ssse3, 32, 32

