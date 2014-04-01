

%include "x86inc.asm"


%if 1 == SIZE_OF_PIXEL

extern pshuffq_zero

SECTION .text align=16


%macro PIXEL_SSES_CALC_AND_ADD_SSES_HELP_SSSE3 0
%if P_FLAGS == DST_FLAG
	MOVDQA XSPACE0, XORG
	PSUBUSB XSPACE0, XCUR
	PSUBUSB XCUR, XORG
	PMAXUB XCUR, XSPACE0
	MOVDQA XORG, XCUR
	PUNPCKLBW XCUR, XZERO
	PUNPCKHBW XORG, XZERO
	PMADDWD XCUR, XCUR
	PMADDWD XORG, XORG
	PADDD XCUR, XORG
	PSRAD XCUR, XSHIFT
	PADDD XSSES, XCUR
%else
	PSUBSW XCUR, XORG
	PMADDWD XCUR, XCUR
	PADDD XSSES, XCUR
%endif
%endmacro

%macro PIXEL_SSES_READ_4_BYTES_DATA_HELP_SSSE3 3
	MOVD %1, [%2]
	ADD %2, %3
%endmacro

%macro PIXEL_SSES_READ_8_BYTES_DATA_HELP_SSSE3 3
	MOVQ %1, [%2]
	ADD %2, %3
%endmacro

%macro PIXEL_SSES_READ_16_BYTES_DATA_HELP_SSSE3 3
	MOVDQU %1, [%2]
	ADD %2, %3
%endmacro


%macro PIXEL_SSES_4_READ_AND_APPEND_HELP_SSSE3 5
	PIXEL_SSES_READ_4_BYTES_DATA_HELP_SSSE3 %2, %3, %4
	PSLLDQ %2, %5
	PADDB %1, %2
%endmacro

%macro PIXEL_SSES_4_READ_DATA_HELP_SSSE3 5
	PXOR %1, %1
	PIXEL_SSES_READ_4_BYTES_DATA_HELP_SSSE3 %1, %3, %4
%if %5 > 1
	PIXEL_SSES_4_READ_AND_APPEND_HELP_SSSE3 %1, %2, %3, %4,  4
%endif
%if %5 > 2
	PIXEL_SSES_4_READ_AND_APPEND_HELP_SSSE3 %1, %2, %3, %4,  8
%endif
%if %5 > 3
	PIXEL_SSES_4_READ_AND_APPEND_HELP_SSSE3 %1, %2, %3, %4, 12
%endif
%endmacro

%macro PIXEL_SSES_8_READ_DATA_HELP_SSSE3 4
	PIXEL_SSES_READ_8_BYTES_DATA_HELP_SSSE3 %1, %3, %4
	PIXEL_SSES_READ_8_BYTES_DATA_HELP_SSSE3 %2, %3, %4
	MOVLHPS %1, %2
%endmacro


%macro PIXEL_SSES_4_HELP_SSSE3 1
	MOV I, %1
%%PIXEL_SSES_4_HELP_SSSE3_LOOP_LABEL:
%if P_FLAGS == DST_FLAG
	PIXEL_SSES_4_READ_DATA_HELP_SSSE3 XCUR, XSPACE0, cur, i_stride_cur, 4
	PIXEL_SSES_4_READ_DATA_HELP_SSSE3 XORG, XSPACE0, org, i_stride_org, 4
	PIXEL_SSES_CALC_AND_ADD_SSES_HELP_SSSE3
	SUB I, 4
	CMP I, 0
	JG %%PIXEL_SSES_4_HELP_SSSE3_LOOP_LABEL
%else
%if P_FLAGS == SRC_FLAG
	PIXEL_SSES_4_READ_DATA_HELP_SSSE3 XCUR, XSPACE0, cur, i_stride_cur, 2
	PIXEL_SSES_8_READ_DATA_HELP_SSSE3 XORG, XSPACE0, org, i_stride_org
	PUNPCKLBW XCUR, XZERO
	PIXEL_SSES_CALC_AND_ADD_SSES_HELP_SSSE3
	SUB I, 2
	CMP I, 0
	JG %%PIXEL_SSES_4_HELP_SSSE3_LOOP_LABEL
%else
	PIXEL_SSES_8_READ_DATA_HELP_SSSE3 XCUR, XSPACE0, cur, i_stride_cur
	PIXEL_SSES_8_READ_DATA_HELP_SSSE3 XORG, XSPACE0, org, i_stride_org
	PIXEL_SSES_CALC_AND_ADD_SSES_HELP_SSSE3
	SUB I, 2
	CMP I, 0
	JG %%PIXEL_SSES_4_HELP_SSSE3_LOOP_LABEL
%endif
%endif
%endmacro


%macro PIXEL_SSES_8_HELP_SSSE3 1
	MOV I, %1
%%PIXEL_SSES_8_HELP_SSSE3_LOOP_LABEL:
%if P_FLAGS == DST_FLAG
	PIXEL_SSES_8_READ_DATA_HELP_SSSE3 XCUR, XSPACE0, cur, i_stride_cur
	PIXEL_SSES_8_READ_DATA_HELP_SSSE3 XORG, XSPACE0, org, i_stride_org
	PIXEL_SSES_CALC_AND_ADD_SSES_HELP_SSSE3
	SUB I, 2
	CMP I, 0
	JG %%PIXEL_SSES_8_HELP_SSSE3_LOOP_LABEL
%else
%if P_FLAGS == SRC_FLAG
	PIXEL_SSES_READ_8_BYTES_DATA_HELP_SSSE3 XCUR, cur, i_stride_cur
	PIXEL_SSES_READ_16_BYTES_DATA_HELP_SSSE3 XORG, org, i_stride_org
	PUNPCKLBW XCUR, XZERO
	PIXEL_SSES_CALC_AND_ADD_SSES_HELP_SSSE3
	SUB I, 1
	CMP I, 0
	JG %%PIXEL_SSES_8_HELP_SSSE3_LOOP_LABEL
%else
	PIXEL_SSES_READ_16_BYTES_DATA_HELP_SSSE3 XCUR, cur, i_stride_cur
	PIXEL_SSES_READ_16_BYTES_DATA_HELP_SSSE3 XORG, org, i_stride_org
	PIXEL_SSES_CALC_AND_ADD_SSES_HELP_SSSE3
	SUB I, 1
	CMP I, 0
	JG %%PIXEL_SSES_8_HELP_SSSE3_LOOP_LABEL
%endif
%endif
%endmacro

%macro PIXEL_SSES_16_HELP_SSSE3 1
	MOV I, %1
%%PIXEL_SSES_16_HELP_SSSE3_LOOP_LABEL:
	PIXEL_SSES_READ_16_BYTES_DATA_HELP_SSSE3 XCUR, cur, i_stride_cur
	PIXEL_SSES_READ_16_BYTES_DATA_HELP_SSSE3 XORG, org, i_stride_org
	PIXEL_SSES_CALC_AND_ADD_SSES_HELP_SSSE3
	SUB I, 1
	CMP I, 0
	JG %%PIXEL_SSES_16_HELP_SSSE3_LOOP_LABEL
%endmacro

%macro PIXEL_SSES_DST_P_HELP_SSSE3 2
%if 4 == %1
	MOV cur, r0m
	MOV org, r2m
	PIXEL_SSES_4_HELP_SSSE3 %2
%elif 8 == %1
	MOV cur, r0m
	MOV org, r2m
	PIXEL_SSES_8_HELP_SSSE3 %2
%elif 12 == %1
	MOV cur, r0m
	MOV org, r2m
	PIXEL_SSES_8_HELP_SSSE3 %2
	MOV cur, r0m
	MOV org, r2m
	ADD cur, 8
	ADD org, 8
	PIXEL_SSES_4_HELP_SSSE3 %2
%elif 24 == %1
	MOV cur, r0m
	MOV org, r2m
	PIXEL_SSES_16_HELP_SSSE3 %2
	MOV cur, r0m
	MOV org, r2m
	ADD cur, 16
	ADD org, 16
	PIXEL_SSES_8_HELP_SSSE3 %2
%else
	MOV J, 0
%%PIXEL_SSES_DST_P_HELP_SSSE3_J_LABEL:
	MOV cur, r0m
	MOV org, r2m
	ADD cur, J
	ADD org, J
	PIXEL_SSES_16_HELP_SSSE3 %2
	ADD J, 16
	CMP J, %1
	JL %%PIXEL_SSES_DST_P_HELP_SSSE3_J_LABEL
%endif
%endmacro

%macro PIXEL_SSES_DST_S_HELP_SSSE3 2
%if 4 == %1
	MOV cur, r0m
	MOV org, r2m
	PIXEL_SSES_4_HELP_SSSE3 %2
%elif 12 == %1
	MOV cur, r0m
	MOV org, r2m
	PIXEL_SSES_8_HELP_SSSE3 %2
	MOV cur, r0m
	MOV org, r2m
	ADD cur, 16
	ADD org, 16
	PIXEL_SSES_4_HELP_SSSE3 %2
%else
	MOV J, 0
%%PIXEL_SSES_DST_S_HELP_SSSE3_J_LABEL:
	MOV cur, r0m
	MOV org, r2m
	ADD cur, J
	ADD org, J
	PIXEL_SSES_8_HELP_SSSE3 %2
	ADD J, 16
	CMP J, %1*2
	JL %%PIXEL_SSES_DST_S_HELP_SSSE3_J_LABEL
%endif
%endmacro

%macro PIXEL_SSES_HELP_SSSE3 5
	%define		I				r6
	%define		J				r5
	%define		cur				r4
	%define		i_stride_cur	r3
	%define		org				r2
	%define		i_stride_org	r1
	%define		XCUR			XMMR0
	%define		XORG			XMMR1
	%define		XSSES			XMMR2
	%define		XSHIFT			XMMR3
	%define		XZERO			XMMR4
	%define		XSPACE0			XMMR7
	%define		SRC_FLAG		%4
	%define		DST_FLAG		%5
	%define		P_FLAGS			1
	%define		S_FLAGS			2
cglobal %1, 0, 7
	PXOR XZERO, XZERO
	MOV r0, r4m
	SUB r0, 8
	ADD r0, r0
	MOVD XSHIFT, r0
	PSHUFB XSHIFT, [pshuffq_zero]
	PXOR XSSES, XSSES

	MOV i_stride_cur, r1m
%if S_FLAGS == SRC_FLAG
	ADD i_stride_cur, i_stride_cur
%endif
	MOV i_stride_org, r3m
%if S_FLAGS == DST_FLAG
	ADD i_stride_org, i_stride_org
%endif

%if P_FLAGS == DST_FLAG
	PIXEL_SSES_DST_P_HELP_SSSE3	%2, %3
%else
	PIXEL_SSES_DST_S_HELP_SSSE3	%2, %3
%endif
	MOVHLPS XSPACE0, XSSES
	PADDD XSSES, XSPACE0
	MOVDQA XSPACE0, XSSES
	PSRLDQ XSPACE0, 4
	PADDD XSSES, XSPACE0
	MOVD r0, XSSES

	RET
%endmacro























%else

SECTION .text align=16


%macro PIXEL_SSES_CALC_AND_ADD_SSES_HELP_SSSE3 0
	PSUBSW XCUR, XORG
	PMADDWD XCUR, XCUR
	PADDD XSSES, XCUR
%endmacro



%macro PIXEL_SSES_READ_4_BYTES_DATA_HELP_SSSE3 3
	MOVQ %1, [%2]
	ADD %2, %3
%endmacro

%macro PIXEL_SSES_4_READ_DATA_HELP_SSSE3 4
	PIXEL_SSES_READ_4_BYTES_DATA_HELP_SSSE3 %1, %3, %4
	PIXEL_SSES_READ_4_BYTES_DATA_HELP_SSSE3 %2, %3, %4
	MOVLHPS %1, %2
%endmacro

%macro PIXEL_SSES_4_HELP_SSSE3 1
	MOV I, %1
%%PIXEL_SSES_4_HELP_SSSE3_LOOP_LABEL:
	PIXEL_SSES_4_READ_DATA_HELP_SSSE3 XCUR, XSPACE0, cur, i_stride_cur
	PIXEL_SSES_4_READ_DATA_HELP_SSSE3 XORG, XSPACE0, org, i_stride_org
	PIXEL_SSES_CALC_AND_ADD_SSES_HELP_SSSE3
	SUB I, 2
	CMP I, 0
	JG %%PIXEL_SSES_4_HELP_SSSE3_LOOP_LABEL
%endmacro

%macro PIXEL_SSES_READ_8_BYTES_DATA_HELP_SSSE3 3
	MOVDQU %1, [%2]
	ADD %2, %3
%endmacro

%macro PIXEL_SSES_8_HELP_SSSE3 1
	MOV I, %1
%%PIXEL_SSES_8_HELP_SSSE3_LOOP_LABEL:
	PIXEL_SSES_READ_8_BYTES_DATA_HELP_SSSE3 XCUR, cur, i_stride_cur
	PIXEL_SSES_READ_8_BYTES_DATA_HELP_SSSE3 XORG, org, i_stride_org
	PIXEL_SSES_CALC_AND_ADD_SSES_HELP_SSSE3
	SUB I, 1
	CMP I, 0
	JG %%PIXEL_SSES_8_HELP_SSSE3_LOOP_LABEL
%endmacro


%macro PIXEL_SSES_HELP_SSSE3 5
	%define		I				r6
	%define		J				r5
	%define		cur				r4
	%define		i_stride_cur	r3
	%define		org				r2
	%define		i_stride_org	r1
	%define		XCUR			XMMR0
	%define		XORG			XMMR1
	%define		XSSES			XMMR2
	%define		XSPACE0			XMMR7
	%define		SRC_FLAG		%4
	%define		DST_FLAG		%5
	%define		P_FLAGS			1
	%define		S_FLAGS			2

cglobal %1, 0, 7
	MOV i_stride_cur, r1m
	ADD i_stride_cur, i_stride_cur
	MOV i_stride_org, r3m
	ADD i_stride_org, i_stride_org
	PXOR XSSES, XSSES

%if 4 == %2
	MOV cur, r0m
	MOV org, r2m
	PIXEL_SSES_4_HELP_SSSE3 %3
%elif 12 == %2
	MOV cur, r0m
	MOV org, r2m
	PIXEL_SSES_8_HELP_SSSE3 %3
	MOV cur, r0m
	MOV org, r2m
	ADD cur, 16
	ADD org, 16
	PIXEL_SSES_4_HELP_SSSE3 %3
%else
	MOV J, 0
%%PIXEL_SSES_HELP_SSSE3_J_LABEL:
	MOV cur, r0m
	MOV org, r2m
	ADD cur, J
	ADD org, J
	PIXEL_SSES_8_HELP_SSSE3 %3
	ADD J, 16
	CMP J, %2*2
	JL %%PIXEL_SSES_HELP_SSSE3_J_LABEL
%endif
	MOVHLPS XSPACE0, XSSES
	PADDD XSSES, XSPACE0
	MOVDQA XSPACE0, XSSES
	PSRLDQ XSPACE0, 4
	PADDD XSSES, XSPACE0
	MOVD r0, XSSES
	RET
%endmacro

%endif



%macro DEFINE_ALL_PIXEL_SSES_HELP_SSSE3 4

	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _4x4_ssse3,      4,   4,  %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _4x8_ssse3,      4,   8,  %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _4x12_ssse3,     4,   12, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _4x16_ssse3,     4,   16, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _4x24_ssse3,     4,   24, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _4x32_ssse3,     4,   32, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _4x48_ssse3,     4,   48, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _4x64_ssse3,     4,   64, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _8x4_ssse3,      8,   4,  %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _8x8_ssse3,      8,   8,  %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _8x12_ssse3,     8,   12, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _8x16_ssse3,     8,   16, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _8x24_ssse3,     8,   24, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _8x32_ssse3,     8,   32, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _8x48_ssse3,     8,   48, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _8x64_ssse3,     8,   64, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _12x4_ssse3,     12,   4, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _12x8_ssse3,     12,   8, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _12x12_ssse3,    12,   12, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _12x16_ssse3,    12,   16, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _12x24_ssse3,    12,   24, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _12x32_ssse3,    12,   32, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _12x48_ssse3,    12,   48, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _12x64_ssse3,    12,   64, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _16x4_ssse3,     16,   4,  %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _16x8_ssse3,     16,   8,  %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _16x12_ssse3,    16,   12, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _16x16_ssse3,    16,   16, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _16x24_ssse3,    16,   24, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _16x32_ssse3,    16,   32, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _16x48_ssse3,    16,   48, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _16x64_ssse3,    16,   64, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _24x4_ssse3,     24,   4,  %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _24x8_ssse3,     24,   8,  %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _24x12_ssse3,    24,   12, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _24x16_ssse3,    24,   16, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _24x24_ssse3,    24,   24, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _24x32_ssse3,    24,   32, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _24x48_ssse3,    24,   48, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _24x64_ssse3,    24,   64, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _32x4_ssse3,     32,   4,  %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _32x8_ssse3,     32,   8,  %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _32x12_ssse3,    32,   12, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _32x16_ssse3,    32,   16, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _32x24_ssse3,    32,   24, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _32x32_ssse3,    32,   32, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _32x48_ssse3,    32,   48, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _32x64_ssse3,    32,   64, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _48x4_ssse3,     48,   4,  %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _48x8_ssse3,     48,   8,  %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _48x12_ssse3,    48,   12, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _48x16_ssse3,    48,   16, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _48x24_ssse3,    48,   24, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _48x32_ssse3,    48,   32, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _48x48_ssse3,    48,   48, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _48x64_ssse3,    48,   64, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _64x4_ssse3,     64,   4,  %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _64x8_ssse3,     64,   8,  %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _64x12_ssse3,    64,   12, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _64x16_ssse3,    64,   16, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _64x24_ssse3,    64,   24, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _64x32_ssse3,    64,   32, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _64x48_ssse3,    64,   48, %1, %3
	PIXEL_SSES_HELP_SSSE3 pixel_sses_ %+ %2 %+ _ %+ %4 %+ _64x64_ssse3,    64,   64, %1, %3

%endmacro

DEFINE_ALL_PIXEL_SSES_HELP_SSSE3 P_FLAGS, p, P_FLAGS, p
DEFINE_ALL_PIXEL_SSES_HELP_SSSE3 P_FLAGS, p, S_FLAGS, s
DEFINE_ALL_PIXEL_SSES_HELP_SSSE3 S_FLAGS, s, S_FLAGS, s


