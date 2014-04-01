
%include "x86inc.asm"

%if 1 == SIZE_OF_PIXEL
SECTION .text align=16

%macro PIXEL_SADS_READ_4_BYTES_DATA_HELP_SSSE3 3
	MOVD %1, [%2]
	ADD %2, %3
%endmacro


%macro PIXEL_SADS_CALC_AND_ADD_SADS_HELP_SSSE3 0
	PSADBW XCUR, XORG
	PXOR XSPACE0, XSPACE0
	MOVDQA XORG, XCUR
	PUNPCKLWD XCUR, XSPACE0
	PUNPCKHWD XORG, XSPACE0
	PADDD XSADS, XCUR
	PADDD XSADS, XORG
%endmacro

%macro PIXEL_SADS_READ_8_BYTES_DATA_HELP_SSSE3 3
	MOVQ %1, [%2]
	ADD %2, %3
%endmacro

%macro PIXEL_SADS_READ_16_BYTES_DATA_HELP_SSSE3 3
	MOVDQU %1, [%2]
	ADD %2, %3
%endmacro

%macro PIXEL_SADS_4_READ_AND_APPEND_HELP_SSSE3 5
	PIXEL_SADS_READ_4_BYTES_DATA_HELP_SSSE3 %2, %3, %4
	PSLLDQ %2, %5
	PADDB %1, %2
%endmacro

%macro PIXEL_SADS_4_READ_DATA_HELP_SSSE3 4
	PXOR %1, %1
	PIXEL_SADS_READ_4_BYTES_DATA_HELP_SSSE3 %1, %3, %4
	PIXEL_SADS_4_READ_AND_APPEND_HELP_SSSE3 %1, %2, %3, %4,  4
	PIXEL_SADS_4_READ_AND_APPEND_HELP_SSSE3 %1, %2, %3, %4,  8
	PIXEL_SADS_4_READ_AND_APPEND_HELP_SSSE3 %1, %2, %3, %4, 12
%endmacro

%macro PIXEL_SADS_4_HELP_SSSE3 1
	MOV I, %1
%%PIXEL_SADS_4_HELP_SSSE3_LOOP_LABEL:
	PIXEL_SADS_4_READ_DATA_HELP_SSSE3 XCUR, XSPACE0, cur, i_stride_cur
	PIXEL_SADS_4_READ_DATA_HELP_SSSE3 XORG, XSPACE0, org, i_stride_org
	PIXEL_SADS_CALC_AND_ADD_SADS_HELP_SSSE3
%if HALF_FLAG == TYPE_FLAG
	SUB I, 8
%else
	SUB I, 4
%endif
	CMP I, 0
	JG %%PIXEL_SADS_4_HELP_SSSE3_LOOP_LABEL
%endmacro

%macro PIXEL_SADS_8_READ_DATA_HELP_SSSE3 4
	PIXEL_SADS_READ_8_BYTES_DATA_HELP_SSSE3 %1, %3, %4
	PIXEL_SADS_READ_8_BYTES_DATA_HELP_SSSE3 %2, %3, %4
	MOVLHPS %1, %2
%endmacro

%macro PIXEL_SADS_8_HELP_SSSE3 1
	MOV I, %1
%%PIXEL_SADS_8_HELP_SSSE3_LOOP_LABEL:
	PIXEL_SADS_8_READ_DATA_HELP_SSSE3 XCUR, XSPACE0, cur, i_stride_cur
	PIXEL_SADS_8_READ_DATA_HELP_SSSE3 XORG, XSPACE0, org, i_stride_org
	PIXEL_SADS_CALC_AND_ADD_SADS_HELP_SSSE3
%if HALF_FLAG == TYPE_FLAG
	SUB I, 4
%else
	SUB I, 2
%endif
	CMP I, 0
	JG %%PIXEL_SADS_8_HELP_SSSE3_LOOP_LABEL
%endmacro

%macro PIXEL_SADS_16_HELP_SSSE3 1
	MOV I, %1
%%PIXEL_SADS_16_HELP_SSSE3_LOOP_LABEL:
	PIXEL_SADS_READ_16_BYTES_DATA_HELP_SSSE3 XCUR, cur, i_stride_cur
	PIXEL_SADS_READ_16_BYTES_DATA_HELP_SSSE3 XORG, org, i_stride_org
	PIXEL_SADS_CALC_AND_ADD_SADS_HELP_SSSE3

%if HALF_FLAG == TYPE_FLAG
	SUB I, 2
%else
	SUB I, 1
%endif
	CMP I, 0
	JG %%PIXEL_SADS_16_HELP_SSSE3_LOOP_LABEL
%endmacro

%macro PIXEL_SADS_HELP_SSSE3 4
	%define		I				r6
	%define		J				r5
	%define		cur				r4
	%define		i_stride_cur	r3
	%define		org				r2
	%define		i_stride_org	r1
	%define		XCUR			XMMR0
	%define		XORG			XMMR1
	%define		XSADS			XMMR2
	%define		XSPACE0			XMMR7
	%define		TYPE_FLAG		%4
	%define		HALF_FLAG		 1
	%define		FULL_FLAG		 2

cglobal %1, 0, 7
	MOV i_stride_cur, r1m
%if HALF_FLAG == TYPE_FLAG
	ADD i_stride_cur, i_stride_cur
%endif
	MOV i_stride_org, r3m
%if HALF_FLAG == TYPE_FLAG
	ADD i_stride_org, i_stride_org
%endif
	PXOR XSADS, XSADS

%if 4 == %2
	MOV cur, r0m
	MOV org, r2m
	PIXEL_SADS_4_HELP_SSSE3 %3

%elif 8 == %2
	MOV cur, r0m
	MOV org, r2m
	PIXEL_SADS_8_HELP_SSSE3 %3
%elif 12 == %2
	MOV cur, r0m
	MOV org, r2m
	PIXEL_SADS_8_HELP_SSSE3 %3
	MOV cur, r0m
	MOV org, r2m
	ADD cur, 8
	ADD org, 8
	PIXEL_SADS_4_HELP_SSSE3 %3
%elif 24 == %2
	MOV cur, r0m
	MOV org, r2m
	PIXEL_SADS_16_HELP_SSSE3 %3
	MOV cur, r0m
	MOV org, r2m
	ADD cur, 16
	ADD org, 16
	PIXEL_SADS_8_HELP_SSSE3 %3
%else
	MOV J, 0
%%PIXEL_SADS_HELP_SSSE3_J_LABEL:
	MOV cur, r0m
	MOV org, r2m
	ADD cur, J
	ADD org, J
	PIXEL_SADS_16_HELP_SSSE3 %3
	ADD J, 16
	CMP J, %2
	JL %%PIXEL_SADS_HELP_SSSE3_J_LABEL
%endif

	MOVHLPS XSPACE0, XSADS
	PADDD XSADS, XSPACE0
	MOVD r0, XSADS
%if HALF_FLAG == TYPE_FLAG
	ADD r0, r0
%endif
	MOV ecx, r4m
	SUB ecx, 8
	SAR r0,  cl
	RET
%endmacro
%else
SECTION .text align=16

%macro PIXEL_SADS_CALC_AND_ADD_SADS_HELP_SSSE3 0
	PSUBW XCUR, XORG
	PABSW XCUR, XCUR
	PXOR XSPACE0, XSPACE0
	MOVDQA XORG, XCUR
	PUNPCKLWD XCUR, XSPACE0
	PUNPCKHWD XORG, XSPACE0
	PADDD XSADS, XCUR
	PADDD XSADS, XORG
%endmacro



%macro PIXEL_SADS_READ_4_BYTES_DATA_HELP_SSSE3 3
	MOVQ %1, [%2]
	ADD %2, %3
%endmacro

%macro PIXEL_SADS_4_READ_DATA_HELP_SSSE3 4
	PIXEL_SADS_READ_4_BYTES_DATA_HELP_SSSE3 %1, %3, %4
	PIXEL_SADS_READ_4_BYTES_DATA_HELP_SSSE3 %2, %3, %4
	MOVLHPS %1, %2
%endmacro

%macro PIXEL_SADS_4_HELP_SSSE3 1
	MOV I, %1
%%PIXEL_SADS_4_HELP_SSSE3_LOOP_LABEL:
	PIXEL_SADS_4_READ_DATA_HELP_SSSE3 XCUR, XSPACE0, cur, i_stride_cur
	PIXEL_SADS_4_READ_DATA_HELP_SSSE3 XORG, XSPACE0, org, i_stride_org
	PIXEL_SADS_CALC_AND_ADD_SADS_HELP_SSSE3
%if HALF_FLAG == TYPE_FLAG
	SUB I, 4
%else
	SUB I, 2
%endif
	CMP I, 0
	JG %%PIXEL_SADS_4_HELP_SSSE3_LOOP_LABEL
%endmacro

%macro PIXEL_SADS_READ_8_BYTES_DATA_HELP_SSSE3 3
	MOVDQU %1, [%2]
	ADD %2, %3
%endmacro

%macro PIXEL_SADS_8_HELP_SSSE3 1
	MOV I, %1
%%PIXEL_SADS_8_HELP_SSSE3_LOOP_LABEL:
	PIXEL_SADS_READ_8_BYTES_DATA_HELP_SSSE3 XCUR, cur, i_stride_cur
	PIXEL_SADS_READ_8_BYTES_DATA_HELP_SSSE3 XORG, org, i_stride_org
	PIXEL_SADS_CALC_AND_ADD_SADS_HELP_SSSE3
%if HALF_FLAG == TYPE_FLAG
	SUB I, 2
%else
	SUB I, 1
%endif
	CMP I, 0
	JG %%PIXEL_SADS_8_HELP_SSSE3_LOOP_LABEL
%endmacro


%macro PIXEL_SADS_HELP_SSSE3 4
	%define		I				r6
	%define		J				r5
	%define		cur				r4
	%define		i_stride_cur	r3
	%define		org				r2
	%define		i_stride_org	r1
	%define		XCUR			XMMR0
	%define		XORG			XMMR1
	%define		XSADS			XMMR2
	%define		XSPACE0			XMMR7
	%define		TYPE_FLAG		%4
	%define		HALF_FLAG		 1
	%define		FULL_FLAG		 2

cglobal %1, 0, 7
	MOV i_stride_cur, r1m
	ADD i_stride_cur, i_stride_cur
%if HALF_FLAG == TYPE_FLAG
	ADD i_stride_cur, i_stride_cur
%endif
	MOV i_stride_org, r3m
	ADD i_stride_org, i_stride_org
%if HALF_FLAG == TYPE_FLAG
	ADD i_stride_org, i_stride_org
%endif
	PXOR XSADS, XSADS

%if 4 == %2
	MOV cur, r0m
	MOV org, r2m
	PIXEL_SADS_4_HELP_SSSE3 %3
%elif 12 == %2
	MOV cur, r0m
	MOV org, r2m
	PIXEL_SADS_8_HELP_SSSE3 %3
	MOV cur, r0m
	MOV org, r2m
	ADD cur, 16
	ADD org, 16
	PIXEL_SADS_4_HELP_SSSE3 %3
%else
	MOV J, 0
%%PIXEL_SADS_HELP_SSSE3_J_LABEL:
	MOV cur, r0m
	MOV org, r2m
	ADD cur, J
	ADD org, J
	PIXEL_SADS_8_HELP_SSSE3 %3
	ADD J, 16
	CMP J, %2*2
	JL %%PIXEL_SADS_HELP_SSSE3_J_LABEL
%endif

	MOVHLPS XSPACE0, XSADS
	PADDD XSADS, XSPACE0
	MOVDQA XSPACE0, XSADS
	PSRLDQ XSPACE0, 4
	PADDD XSADS, XSPACE0

	MOVD r0, XSADS
%if HALF_FLAG == TYPE_FLAG
	ADD r0, r0
%endif
	MOV ecx, r4m
	SUB ecx, 8
	SAR r0,  cl
	RET
%endmacro
%endif

%macro DEFINE_ALL_PIXEL_SADS_HELP_SSSE3 2

	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 4x4_ssse3,      4,   4,  %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 4x8_ssse3,      4,   8,  %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 4x12_ssse3,     4,   12, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 4x16_ssse3,     4,   16, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 4x24_ssse3,     4,   24, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 4x32_ssse3,     4,   32, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 4x48_ssse3,     4,   48, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 4x64_ssse3,     4,   64, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 8x4_ssse3,      8,   4,  %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 8x8_ssse3,      8,   8,  %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 8x12_ssse3,     8,   12, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 8x16_ssse3,     8,   16, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 8x24_ssse3,     8,   24, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 8x32_ssse3,     8,   32, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 8x48_ssse3,     8,   48, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 8x64_ssse3,     8,   64, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 12x4_ssse3,     12,   4, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 12x8_ssse3,     12,   8, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 12x12_ssse3,    12,   12, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 12x16_ssse3,    12,   16, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 12x24_ssse3,    12,   24, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 12x32_ssse3,    12,   32, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 12x48_ssse3,    12,   48, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 12x64_ssse3,    12,   64, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 16x4_ssse3,     16,   4,  %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 16x8_ssse3,     16,   8,  %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 16x12_ssse3,    16,   12, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 16x16_ssse3,    16,   16, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 16x24_ssse3,    16,   24, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 16x32_ssse3,    16,   32, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 16x48_ssse3,    16,   48, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 16x64_ssse3,    16,   64, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 24x4_ssse3,     24,   4,  %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 24x8_ssse3,     24,   8,  %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 24x12_ssse3,    24,   12, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 24x16_ssse3,    24,   16, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 24x24_ssse3,    24,   24, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 24x32_ssse3,    24,   32, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 24x48_ssse3,    24,   48, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 24x64_ssse3,    24,   64, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 32x4_ssse3,     32,   4,  %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 32x8_ssse3,     32,   8,  %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 32x12_ssse3,    32,   12, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 32x16_ssse3,    32,   16, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 32x24_ssse3,    32,   24, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 32x32_ssse3,    32,   32, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 32x48_ssse3,    32,   48, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 32x64_ssse3,    32,   64, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 48x4_ssse3,     48,   4,  %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 48x8_ssse3,     48,   8,  %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 48x12_ssse3,    48,   12, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 48x16_ssse3,    48,   16, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 48x24_ssse3,    48,   24, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 48x32_ssse3,    48,   32, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 48x48_ssse3,    48,   48, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 48x64_ssse3,    48,   64, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 64x4_ssse3,     64,   4,  %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 64x8_ssse3,     64,   8,  %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 64x12_ssse3,    64,   12, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 64x16_ssse3,    64,   16, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 64x24_ssse3,    64,   24, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 64x32_ssse3,    64,   32, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 64x48_ssse3,    64,   48, %2
	PIXEL_SADS_HELP_SSSE3 pixel_sads %+ %1 %+ 64x64_ssse3,    64,   64, %2

%endmacro

DEFINE_ALL_PIXEL_SADS_HELP_SSSE3 _half_, HALF_FLAG
DEFINE_ALL_PIXEL_SADS_HELP_SSSE3      _, FULL_FLAG


