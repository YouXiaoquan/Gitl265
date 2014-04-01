

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



%macro LOAD_DIFF_SSSE3 5
	MOV r1, r1m
%if 2 == SIZE_OF_PIXEL
	ADD r1, r1
%endif
	IMUL r1, %4

%if 1 == SIZE_OF_PIXEL
%if 8 == %5
	MOVQ %2, [cur+r1]
%elif 4 == %5
	MOVD %2, [cur+r1]
%endif
	PUNPCKLBW %2, %3
%else
%if 8 == %5
	MOVDQU %2, [cur+r1]
%elif 4 == %5
	MOVQ %2, [cur+r1]
%endif
%endif

	MOV r1, r3m
%if 2 == SIZE_OF_PIXEL
	ADD r1, r1
%endif
	IMUL r1, %4

%if 1 == SIZE_OF_PIXEL
%if 8 == %5
	MOVQ %1, [org+r1]
%elif 4 == %5
	MOVD %1, [org+r1]
%endif
	PUNPCKLBW %1, %3
%else
%if 8 == %5
	MOVDQU %1, [org+r1]
%elif 4 == %5
	MOVQ %1, [org+r1]
%endif
%endif

	PSUBW %1, %2
%endmacro

%macro SUMSUB_SSSE3 3
	MOVDQA %3, %2
	MOVDQA %2, %1
	PADDW %1, %3
	PSUBW %2, %3
%endmacro


%macro ABS 2
	PXOR %2, %2
	PSUBW %2, %1
	PMAXSW %1, %2
%endmacro

%macro ABS_AND_ADD_SSSE3 2
%if 8 == %1
%if 1 == %2
	ABS XMM7, XMM0
	ABS XMM6, XMM0
	ABS XMM5, XMM0
	MOVDQA XMM0, [mul_coeff_one]
	PMADDWD XMM7, XMM0
	PMADDWD XMM6, XMM0
	PMADDWD XMM5, XMM0
	PADDD XMM5, XMM7
	PADDD XMM5, XMM6
	MOVDQA XMM6, XMM0
%elif 2 == %2
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

	MOVDQA XMM7, XMM5
	PSHUFD XMM7, XMM7, 01H
	PADDD XMM5, XMM7
%endif
%elif 4 == %1
	ABS XMM0, XMM4
	ABS XMM1, XMM4
	ABS XMM2, XMM4
	ABS XMM3, XMM4

	MOVQ XMM4, [mul_coeff_one]
	PMADDWD XMM0, XMM4
	PMADDWD XMM1, XMM4
	PMADDWD XMM2, XMM4
	PMADDWD XMM3, XMM4

	PADDD XMM0, XMM1
	PADDD XMM0, XMM2
	PADDD XMM0, XMM3

	MOVQ XMM4, XMM0
	PSRLQ XMM4, 32
	PADDD XMM0, XMM4
%endif
%endmacro

%macro LOAD_N_DIFF_SSSE3 1
	PXOR XMM7, XMM7
%if 8 == %1
	LOAD_DIFF_SSSE3 XMM0, XMM6, XMM7, 0, %1
	LOAD_DIFF_SSSE3 XMM1, XMM6, XMM7, 1, %1
	LOAD_DIFF_SSSE3 XMM2, XMM6, XMM7, 2, %1
	LOAD_DIFF_SSSE3 XMM3, XMM6, XMM7, 3, %1
	MOVDQA R1, XMM0
	MOVDQA R2, XMM1
	MOVDQA R3, XMM2
	MOVDQA R4, XMM3

	LOAD_DIFF_SSSE3 XMM0, XMM6, XMM7, 4, %1
	LOAD_DIFF_SSSE3 XMM1, XMM6, XMM7, 5, %1
	LOAD_DIFF_SSSE3 XMM2, XMM6, XMM7, 6, %1
	LOAD_DIFF_SSSE3 XMM3, XMM6, XMM7, 7, %1
%elif 4 == %1
	LOAD_DIFF_SSSE3 XMM0, XMM6, XMM7, 0, %1
	LOAD_DIFF_SSSE3 XMM1, XMM6, XMM7, 1, %1
	LOAD_DIFF_SSSE3 XMM2, XMM6, XMM7, 2, %1
	LOAD_DIFF_SSSE3 XMM3, XMM6, XMM7, 3, %1
%endif
%endmacro

%macro HADAMARD_HOR_SSSE3		7
%if 4 == %7
	MOVDQA %3, %1
	PUNPCKLQDQ %1, %1
	MOVDQA %2, %1
	PUNPCKHQDQ %3, %3
	PADDW %1, %3
	PSUBW %2, %3
	PUNPCKLQDQ %1, %2
%else
	MOVDQA %3, %1
	PSHUFB %1, %4
	MOVDQA %2, %1
	PSHUFB %3, %5
	PADDW %1, %3
	PSUBW %2, %3
	PUNPCKL%6 %1, %2
%endif
%endmacro


%macro	HADAMARD_8x4_SSSE3		4
%if 4 > %4
	MOVDQA XMM7, %1
	MOVDQA XMM6, %2
%endif
	HADAMARD_HOR_SSSE3 XMM0, XMM4, XMM5, XMM6, XMM7, %3, %4
	HADAMARD_HOR_SSSE3 XMM1, XMM4, XMM5, XMM6, XMM7, %3, %4
	HADAMARD_HOR_SSSE3 XMM2, XMM4, XMM5, XMM6, XMM7, %3, %4
	HADAMARD_HOR_SSSE3 XMM3, XMM4, XMM5, XMM6, XMM7, %3, %4
%endmacro


%macro	HADAMARD_H_SSSE3 1
%if 8 == %1
	HADAMARD_8x4_SSSE3 [hor_HADAMARD_coeff_1_1], [hor_HADAMARD_coeff_1_2], WD, 1
	HADAMARD_8x4_SSSE3 [hor_HADAMARD_coeff_2_1], [hor_HADAMARD_coeff_2_2], DQ, 2
	HADAMARD_8x4_SSSE3 [hor_HADAMARD_coeff_2_1], [hor_HADAMARD_coeff_2_2], DQ, 4

	MOVDQA R5, XMM0
	MOVDQA R6, XMM1
	MOVDQA R7, XMM2
	MOVDQA R8, XMM3

	MOVDQA XMM0, R1
	MOVDQA XMM1, R2
	MOVDQA XMM2, R3
	MOVDQA XMM3, R4

	HADAMARD_8x4_SSSE3 [hor_HADAMARD_coeff_1_1], [hor_HADAMARD_coeff_1_2], WD, 1
	HADAMARD_8x4_SSSE3 [hor_HADAMARD_coeff_2_1], [hor_HADAMARD_coeff_2_2], DQ, 2
	HADAMARD_8x4_SSSE3 [hor_HADAMARD_coeff_2_1], [hor_HADAMARD_coeff_2_2], DQ, 4
%else
	HADAMARD_8x4_SSSE3 [hor_HADAMARD_coeff_1_1], [hor_HADAMARD_coeff_1_2], WD, 1
	HADAMARD_8x4_SSSE3 [hor_HADAMARD_coeff_2_1], [hor_HADAMARD_coeff_2_2], DQ, 2
%endif
%endmacro

%macro HADAMARD_V_SSSE3 1
%if 8 == %1
	MOVDQA XMM4, R5
	MOVDQA XMM5, R6
	SUMSUB_SSSE3 XMM0, XMM1, XMM7
	SUMSUB_SSSE3 XMM2, XMM3, XMM7
	SUMSUB_SSSE3 XMM4, XMM5, XMM7

	SUMSUB_SSSE3 XMM0, XMM2, XMM7
	SUMSUB_SSSE3 XMM1, XMM3, XMM7

	MOVDQA R1, XMM0
	MOVDQA XMM6, R7
	MOVDQA XMM7, R8

	SUMSUB_SSSE3 XMM6, XMM7, XMM0

	SUMSUB_SSSE3 XMM4, XMM6, XMM0
	SUMSUB_SSSE3 XMM5, XMM7, XMM0

	SUMSUB_SSSE3 XMM1, XMM5, XMM0
	SUMSUB_SSSE3 XMM2, XMM6, XMM0
	SUMSUB_SSSE3 XMM3, XMM7, XMM0
	ABS_AND_ADD_SSSE3 8, 1

	MOVDQA XMM0, R1
	SUMSUB_SSSE3 XMM0, XMM4, XMM7
	ABS_AND_ADD_SSSE3 8, 2
%else
	SUMSUB_SSSE3 XMM0, XMM1, XMM4
	SUMSUB_SSSE3 XMM2, XMM3, XMM4
	SUMSUB_SSSE3 XMM0, XMM2, XMM4
	SUMSUB_SSSE3 XMM1, XMM3, XMM4
	ABS_AND_ADD_SSSE3 4, 1
%endif
%endmacro

%macro PIXEL_HADS_SSSE3 1
	LOAD_N_DIFF_SSSE3 %1

	HADAMARD_H_SSSE3 %1
	HADAMARD_V_SSSE3 %1
%if 8 == %1
	MOVD r1, XMM5
	ADD r1, 2
	SHR r1, 2
%elif 4 == %1
	MOVD r1, XMM0
	ADD r1, 1
	SHR r1, 1
%endif

%endmacro


%macro PIXEL_HADS_4x4_HELP_SSSE3 3
cglobal %1, 0, 7
	%define		I				r6
	%define		J				r5
	%define		cur				r4
	%define		org				r3

	MOV r0, 0
	MOV I, 0
%%PIXEL_HADS_4x4_HELP_SSSE3_I:

	MOV J, 0
%%PIXEL_HADS_4x4_HELP_SSSE3_J:
	MOV cur, r0m
	MOV r1, r1m
%if 2 == SIZE_OF_PIXEL
	ADD r1, r1
%endif
	IMUL r1, I
	ADD cur, r1
	ADD cur, J
%if 2 == SIZE_OF_PIXEL
	ADD cur, J
%endif

	MOV org, r2m
	MOV r1, r3m
%if 2 == SIZE_OF_PIXEL
	ADD r1, r1
%endif
	IMUL r1, I
	ADD org, r1
	ADD org, J
%if 2 == SIZE_OF_PIXEL
	ADD org, J
%endif

	PIXEL_HADS_SSSE3 4

	ADD r0, r1

	ADD J, 4
	CMP J, %2
	JL %%PIXEL_HADS_4x4_HELP_SSSE3_J

	ADD I, 4
	CMP I, %3
	JL %%PIXEL_HADS_4x4_HELP_SSSE3_I

	RET
%endmacro


%macro PIXEL_HADS_8x8_HELP_SSSE3 3
cglobal %1, 0, 7
	%define		I				r5
	%define		J				r4
	%define		cur				r3
	%define		org				r2
	%define		R1				[r6   ]
	%define		R2				[r6+16]
	%define		R3				[r6+32]
	%define		R4				[r6+48]
	%define		R5				[r6+64]
	%define		R6				[r6+80]
	%define		R7				[r6+96]
	%define		R8				[r6+112]

	MOV r6, esp
	AND r6, 0FFFFFFF0H
	SUB r6, 128
	MOV r0, 0

	MOV I, 0
%%PIXEL_HADS_8x8_HELP_SSSE3_I:

	MOV J, 0
%%PIXEL_HADS_8x8_HELP_SSSE3_J:
	MOV cur, r0m
	MOV r1, r1m
%if 2 == SIZE_OF_PIXEL
	ADD r1, r1
%endif
	IMUL r1, I
	ADD cur, r1
	ADD cur, J
%if 2 == SIZE_OF_PIXEL
	ADD cur, J
%endif

	MOV org, r2m
	MOV r1, r3m
%if 2 == SIZE_OF_PIXEL
	ADD r1, r1
%endif
	IMUL r1, I
	ADD org, r1
	ADD org, J
%if 2 == SIZE_OF_PIXEL
	ADD org, J
%endif

	PIXEL_HADS_SSSE3 8
	ADD r0, r1

	ADD J, 8
	CMP J, %2
	JL %%PIXEL_HADS_8x8_HELP_SSSE3_J

	ADD I, 8
	CMP I, %3
	JL %%PIXEL_HADS_8x8_HELP_SSSE3_I

	RET
%endmacro


PIXEL_HADS_4x4_HELP_SSSE3 pixel_hads_4x4_ssse3,    4,  4
PIXEL_HADS_4x4_HELP_SSSE3 pixel_hads_4x8_ssse3,    4,  8
PIXEL_HADS_4x4_HELP_SSSE3 pixel_hads_4x12_ssse3,   4, 12
PIXEL_HADS_4x4_HELP_SSSE3 pixel_hads_4x16_ssse3,   4, 16
PIXEL_HADS_4x4_HELP_SSSE3 pixel_hads_4x24_ssse3,   4, 24
PIXEL_HADS_4x4_HELP_SSSE3 pixel_hads_4x32_ssse3,   4, 32
PIXEL_HADS_4x4_HELP_SSSE3 pixel_hads_4x48_ssse3,   4, 48
PIXEL_HADS_4x4_HELP_SSSE3 pixel_hads_4x64_ssse3,   4, 64

PIXEL_HADS_4x4_HELP_SSSE3 pixel_hads_8x4_ssse3,    8,  4
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_8x8_ssse3,    8,  8
PIXEL_HADS_4x4_HELP_SSSE3 pixel_hads_8x12_ssse3,   8, 12
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_8x16_ssse3,   8, 16
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_8x24_ssse3,   8, 24
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_8x32_ssse3,   8, 32
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_8x48_ssse3,   8, 48
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_8x64_ssse3,   8, 64


PIXEL_HADS_4x4_HELP_SSSE3 pixel_hads_12x4_ssse3,  12,  4
PIXEL_HADS_4x4_HELP_SSSE3 pixel_hads_12x8_ssse3,  12,  8
PIXEL_HADS_4x4_HELP_SSSE3 pixel_hads_12x12_ssse3, 12, 12
PIXEL_HADS_4x4_HELP_SSSE3 pixel_hads_12x16_ssse3, 12, 16
PIXEL_HADS_4x4_HELP_SSSE3 pixel_hads_12x24_ssse3, 12, 24
PIXEL_HADS_4x4_HELP_SSSE3 pixel_hads_12x32_ssse3, 12, 32
PIXEL_HADS_4x4_HELP_SSSE3 pixel_hads_12x48_ssse3, 12, 48
PIXEL_HADS_4x4_HELP_SSSE3 pixel_hads_12x64_ssse3, 12, 64

PIXEL_HADS_4x4_HELP_SSSE3 pixel_hads_16x4_ssse3,  16,  4
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_16x8_ssse3,  16,  8
PIXEL_HADS_4x4_HELP_SSSE3 pixel_hads_16x12_ssse3, 16, 12
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_16x16_ssse3, 16, 16
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_16x24_ssse3, 16, 24
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_16x32_ssse3, 16, 32
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_16x48_ssse3, 16, 48
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_16x64_ssse3, 16, 64
PIXEL_HADS_4x4_HELP_SSSE3 pixel_hads_24x4_ssse3,  24,  4
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_24x8_ssse3,  24,  8
PIXEL_HADS_4x4_HELP_SSSE3 pixel_hads_24x12_ssse3, 24, 12
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_24x16_ssse3, 24, 16
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_24x24_ssse3, 24, 24
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_24x32_ssse3, 24, 32
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_24x48_ssse3, 24, 48
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_24x64_ssse3, 24, 64
PIXEL_HADS_4x4_HELP_SSSE3 pixel_hads_32x4_ssse3,  32,  4
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_32x8_ssse3,  32,  8
PIXEL_HADS_4x4_HELP_SSSE3 pixel_hads_32x12_ssse3, 32, 12
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_32x16_ssse3, 32, 16
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_32x24_ssse3, 32, 24
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_32x32_ssse3, 32, 32
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_32x48_ssse3, 32, 48
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_32x64_ssse3, 32, 64
PIXEL_HADS_4x4_HELP_SSSE3 pixel_hads_48x4_ssse3,  48,  4
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_48x8_ssse3,  48,  8
PIXEL_HADS_4x4_HELP_SSSE3 pixel_hads_48x12_ssse3, 48, 12
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_48x16_ssse3, 48, 16
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_48x24_ssse3, 48, 24
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_48x32_ssse3, 48, 32
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_48x48_ssse3, 48, 48
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_48x64_ssse3, 48, 64
PIXEL_HADS_4x4_HELP_SSSE3 pixel_hads_64x4_ssse3,  64,  4
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_64x8_ssse3,  64,  8
PIXEL_HADS_4x4_HELP_SSSE3 pixel_hads_64x12_ssse3, 64, 12
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_64x16_ssse3, 64, 16
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_64x24_ssse3, 64, 24
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_64x32_ssse3, 64, 32
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_64x48_ssse3, 64, 48
PIXEL_HADS_8x8_HELP_SSSE3 pixel_hads_64x64_ssse3, 64, 64

