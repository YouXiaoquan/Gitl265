%include "x86inc.asm"

extern pshuffd_zero
extern pshuffq_zero

SECTION .rodata align=16
	sign_shift			DD		31, 0, 0, 0
	four_one			DD		 1, 1, 1, 1
	pandd_w				DW		0FFFFH, 0, 0FFFFH, 0, 0FFFFH, 0, 0FFFFH, 0
	qbits8				DD		 8, 0, 0, 0
	max					DD		 32767,  32767,  32767,  32767
	min					DD		-32768, -32768, -32768, -32768

SECTION .text align=16

%macro X265_TR_QUANT_GET_XSIGN_HELP_SSSE3		1
	MOVDQA XSIGN, %1
	PSRAD XSIGN, [sign_shift]
	PADDD XSIGN, XSIGN
	PADDD XSIGN, [four_one]
	PAND XSIGN, [pandd_w]
%endmacro

%macro GET_QBITS_AND_ADD			0

	MOVD XADD, r4m
	PSHUFB XADD, [pshuffd_zero]
	MOVD X_QBITS, r5m
	PSHUFB X_QBITS, [pshuffq_zero]


	CMP b_use_adapt_qp_select, 0
	JE %%GET_QBITS_AND_ADD_CONTINUE_LABEL

	MOV ecx, r9m
	MOVD XSPACE0, ecx
	PSHUFB XSPACE0, [pshuffq_zero]
	MOVLHPS X_QBITS, XSPACE0

	MOV r0, 1
	DEC ecx
	SHL r0, cl
	MOVD XSPACE0, r0
	PSHUFB XSPACE0, [pshuffd_zero]
	MOVLHPS XADD, XSPACE0

%%GET_QBITS_AND_ADD_CONTINUE_LABEL:

%endmacro

%macro X265_TR_QUANT_X_QUANT_HELP_SSSE3		2
	%define				loop					r6
	%define				p_coef					r5
	%define				p_q_coef				r4
	%define				p_quant_coeff			r3
	%define				p_arl_coef				r4
	%define				delta_u					r4
	%define				b_use_adapt_qp_select	r2
	%define				XLEVEL					XMMR7
	%define				XSIGN					XMMR6
	%define				XTEMP_LEVEL				XMMR5
	%define				X_QBITS					XMMR4
	%define				XADD					XMMR3
	%define				XSUM					XMMR2
	%define				XSPACE0					XMMR0
	%define				XSPACE1					XMMR1
cglobal %1, 0, 7

	MOV b_use_adapt_qp_select, r7m
	GET_QBITS_AND_ADD
	PXOR XSUM, XSUM

	MOV p_coef, r0m
	MOV p_quant_coeff, r2m

	MOV loop, 0
%%X265_TR_QUANT_X_QUANT_HELP_SSSE3_LOOP_LABEL:
	MOVDQA XLEVEL, [p_coef+loop]

	X265_TR_QUANT_GET_XSIGN_HELP_SSSE3 XLEVEL

	PABSD XLEVEL, XLEVEL
	MOVDQA XTEMP_LEVEL, XLEVEL
	PMADDWD XTEMP_LEVEL, [p_quant_coeff+loop]

	CMP b_use_adapt_qp_select, 0
	JE %%X265_TR_QUANT_X_QUANT_HELP_SSSE3_CONTINUE_LABEL

	MOVDQA XSPACE1, XTEMP_LEVEL
	MOVHLPS XSPACE0, XADD
	MOVLHPS XSPACE0, XSPACE0
	PADDD XSPACE1, XSPACE0
	MOVHLPS XSPACE0, X_QBITS
	PSRAD XSPACE1, XSPACE0
	MOV p_arl_coef, r8m
	MOVDQU [p_arl_coef+loop], XSPACE1


%%X265_TR_QUANT_X_QUANT_HELP_SSSE3_CONTINUE_LABEL:
	MOVDQA XLEVEL, XTEMP_LEVEL
	MOVQ XSPACE0, XADD
	MOVLHPS XSPACE0, XSPACE0
	PADDD XLEVEL, XSPACE0
	MOVQ XSPACE0, X_QBITS
	PSRAD XLEVEL, XSPACE0

	MOVDQA XSPACE1, XLEVEL
	PSLLD XSPACE1, XSPACE0
	PSUBD XTEMP_LEVEL, XSPACE1
	MOV delta_u, r3m

	PSUBD XSPACE0, [qbits8]
	PSRAD XTEMP_LEVEL, XSPACE0
	MOVDQU [delta_u+loop], XTEMP_LEVEL

	PADDD XSUM, XLEVEL

	PMADDWD XLEVEL, XSIGN
	MOV p_q_coef, r1m
	MOVDQU [p_q_coef+loop], XLEVEL

	ADD loop, 4*4
	CMP loop, %2*%2*4
	JL %%X265_TR_QUANT_X_QUANT_HELP_SSSE3_LOOP_LABEL

	MOVHLPS XSPACE0, XSUM
	PADDD XSUM, XSPACE0
	MOVDQA XSPACE0, XSUM
	PSRLDQ XSPACE0, 4
	PADDD XSUM, XSPACE0
	MOVD r1, XSUM
	MOV r0, r6m
	MOV [r0], r1

	RET
%endmacro

%macro GET_XADD_AND_XSHIFT_SSSE3				1
%if POS_FLAG == %1
	NEG shift
	MOV ecx, shift
	MOVD XSHIFT, ecx
	PSHUFB XSHIFT, [pshuffq_zero]
%else
	MOV ecx, shift
	MOVD XSHIFT, ecx
	PSHUFB XSHIFT, [pshuffq_zero]
	MOV r0, 1
	DEC ecx
	SHL r0, cl
	MOVD XADD, r0
	PSHUFB XADD, [pshuffd_zero]
%endif
%endmacro



%macro X265_TR_QUANT_X_DEQUANT_HELP_SSSE3_NOSCALE_HELP_SSSE3			2
	GET_XADD_AND_XSHIFT_SSSE3 %2

	MOV p_q_coef, r0m
	MOV p_coef, r1m
	MOV p_dequant_coef, r2m
	MOV loop, 0
%%X265_TR_QUANT_X_DEQUANT_HELP_SSSE3_NOSCALE_HELP_SSSE3_LOOP_LABEL:
	MOVDQA XMMR0, [p_q_coef+loop]
	PMAXSD XMMR0, XMIN
	PMINSD XMMR0, XMAX

	PMADDWD XMMR0, [p_dequant_coef+loop]
%if POS_FLAG == %2
	PMAXSD XMMR0, XMIN
	PMINSD XMMR0, XMAX
	PSLLD XMMR0, XSHIFT
%else
	PADDD XMMR0, XADD
	PSRAD XMMR0, XSHIFT
%endif
	PMAXSD XMMR0, XMIN
	PMINSD XMMR0, XMAX
	MOVDQU [p_coef+loop], XMMR0

	ADD loop, 4*4
	CMP loop, %1*%1*4
	JL %%X265_TR_QUANT_X_DEQUANT_HELP_SSSE3_NOSCALE_HELP_SSSE3_LOOP_LABEL

%endmacro



%macro X265_TR_QUANT_X_DEQUANT_HELP_SSSE3_SCALE_HELP_SSSE3				1
	MOV ecx, shift
	MOVD XSHIFT, ecx
	PSHUFB XSHIFT, [pshuffq_zero]
	MOV r0, 1
	DEC ecx
	SHL r0, cl
	MOVD XADD, r0
	PSHUFB XADD, [pshuffd_zero]

	MOVD XSCALE, r5m
	PSHUFB XSCALE, [pshuffd_zero]
	MOV p_q_coef, r0m
	MOV p_coef, r1m
	MOV loop, 0
%%X265_TR_QUANT_X_DEQUANT_HELP_SSSE3_SCALE_HELP_SSSE3_LOOP_LABEL:
	MOVDQA XMMR0, [p_q_coef+loop]
	PMAXSD XMMR0, XMIN
	PMINSD XMMR0, XMAX

	PMADDWD XMMR0, XSCALE
	PADDD XMMR0, XADD
	PSRAD XMMR0, XSHIFT

	PMAXSD XMMR0, XMIN
	PMINSD XMMR0, XMAX
	MOVDQU [p_coef+loop], XMMR0

	ADD loop, 4*4
	CMP loop, %1*%1*4
	JL %%X265_TR_QUANT_X_DEQUANT_HELP_SSSE3_SCALE_HELP_SSSE3_LOOP_LABEL
%endmacro

%macro X265_TR_QUANT_X_DEQUANT_HELP_SSSE3		3
	%define				loop							r6
	%define				b_scaling_list_enabled_flag		r5
	%define				shift							r4
	%define				p_q_coef						r3
	%define				p_coef							r2
	%define				p_dequant_coef					r1
	%define				X265_MAX_TR_DYNAMIC_RANGE_ASM	15
	%define				X265_QUANT_IQUANT_SHIFT_ASM		20
	%define				X265_QUANT_SHIFT_ASM			14
	%define				POS_FLAG						1
	%define				NEG_FLAG						2
	%define				XADD							XMMR6
	%define				XSHIFT							XMMR5
	%define				XSCALE							XMMR4
	%define				XMAX							XMMR3
	%define				XMIN							XMMR2


cglobal %1, 0, 7
	MOVDQA XMAX, [max]
	MOVDQA XMIN, [min]
	MOV shift, (X265_QUANT_IQUANT_SHIFT_ASM - X265_QUANT_SHIFT_ASM)
	ADD shift, %3
	ADD shift, r3m
	SUB shift, X265_MAX_TR_DYNAMIC_RANGE_ASM

	MOV b_scaling_list_enabled_flag, r6m
	CMP b_scaling_list_enabled_flag, 0
	JE %%X265_TR_QUANT_X_DEQUANT_HELP_SSSE3_SCALE_HELP_LABEL

	ADD shift, 4
	SUB shift, r4m
	CMP shift, 0
	JG %%X265_TR_QUANT_X_DEQUANT_HELP_SSSE3_NOSCALE_NEG_HELP_LABEL
	X265_TR_QUANT_X_DEQUANT_HELP_SSSE3_NOSCALE_HELP_SSSE3 %2, POS_FLAG
	JMP %%X265_TR_QUANT_X_DEQUANT_HELP_SSSE3_EXIT_LABEL

%%X265_TR_QUANT_X_DEQUANT_HELP_SSSE3_NOSCALE_NEG_HELP_LABEL:
	X265_TR_QUANT_X_DEQUANT_HELP_SSSE3_NOSCALE_HELP_SSSE3 %2, NEG_FLAG
	JMP %%X265_TR_QUANT_X_DEQUANT_HELP_SSSE3_EXIT_LABEL

%%X265_TR_QUANT_X_DEQUANT_HELP_SSSE3_SCALE_HELP_LABEL:
	X265_TR_QUANT_X_DEQUANT_HELP_SSSE3_SCALE_HELP_SSSE3		%2


%%X265_TR_QUANT_X_DEQUANT_HELP_SSSE3_EXIT_LABEL:

	RET
%endmacro


X265_TR_QUANT_X_QUANT_HELP_SSSE3 tr_quant_x_quant_4x4_ssse3,    4
X265_TR_QUANT_X_QUANT_HELP_SSSE3 tr_quant_x_quant_8x8_ssse3,    8
X265_TR_QUANT_X_QUANT_HELP_SSSE3 tr_quant_x_quant_16x16_ssse3, 16
X265_TR_QUANT_X_QUANT_HELP_SSSE3 tr_quant_x_quant_32x32_ssse3, 32

X265_TR_QUANT_X_DEQUANT_HELP_SSSE3 tr_quant_x_dequant_4x4_ssse3,    4, 2
X265_TR_QUANT_X_DEQUANT_HELP_SSSE3 tr_quant_x_dequant_8x8_ssse3,    8, 3
X265_TR_QUANT_X_DEQUANT_HELP_SSSE3 tr_quant_x_dequant_16x16_ssse3, 16, 4
X265_TR_QUANT_X_DEQUANT_HELP_SSSE3 tr_quant_x_dequant_32x32_ssse3, 32, 5

