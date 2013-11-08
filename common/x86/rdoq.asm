


%include "x86inc.asm"


	%define			I										r6
	%define			temp_src_coef							r5
	%define			temp_q_coef								r4
	%define			temp_level_double						r3
	%define			temp_arl_dst_coeff_array				r2
	%define			temp_max_level_array					r1

	%define			err_scale								r5
	%define			cost_coeff0								r4

	%define			i_q_bits								r5m
	%define			i_q_bits_c								r6m
	%define			MAX_INT									2147483647

%macro SET_32 2
	MOVD %1, %2
	MOVDQA XMM0, %1
	PSLLDQ XMM0, 4
	PADDD %1, XMM0
	PSLLDQ XMM0, 4
	PADDD %1, XMM0
	PSLLDQ XMM0, 4
	PADDD %1, XMM0
%endmacro

SECTION .text align=16


%macro X265_RDO_LEVEL_SSE2_HELP 2
cglobal %1, 0, 7

	MOV r0, MAX_INT
	MOV r1, 1
	MOV ecx, i_q_bits
	SUB ecx, 1
	SHL r1, cl
	SUB r0, r1
	SET_32 XMM7, r0

	MOV r0, 1
	MOV ecx, i_q_bits
	SUB ecx, 1
	SHL r0, cl
	SET_32 XMM6, r0

	MOV r0, 1
	MOV ecx, i_q_bits_c
	SUB ecx, 1
	SHL r0, cl
	SET_32 XMM5, r0

	PXOR XMM4, XMM4
	MOV r0, i_q_bits
	MOVD XMM4, r0

	PXOR XMM3, XMM3
	MOV r0, i_q_bits_c
	MOVD XMM3, r0


	MOV temp_src_coef, r0m
	MOV temp_q_coef, r1m
	MOV temp_level_double, r2m
	MOV temp_arl_dst_coeff_array, r3m
	MOV temp_max_level_array, r4m




	MOV I, 0
%%x265_rdoq_ssse3_level_i1:
	MOVDQA XMM2, [temp_src_coef]
	PABSD XMM2, XMM2
	MOVD r0, XMM2
	IMUL r0, [temp_q_coef]
	MOVD XMM0, r0

	PSRLDQ XMM2, 4
	MOVD r0, XMM2
	IMUL r0, [temp_q_coef+4]
	MOVD XMM1, r0
	PSLLDQ XMM1, 4
	PADDD XMM0, XMM1

	PSRLDQ XMM2, 4
	MOVD r0, XMM2
	IMUL r0, [temp_q_coef+8]
	MOVD XMM1, r0
	PSLLDQ XMM1, 8
	PADDD XMM0, XMM1

	PSRLDQ XMM2, 4
	MOVD r0, XMM2
	IMUL r0, [temp_q_coef+12]
	MOVD XMM1, r0
	PSLLDQ XMM1, 12
	PADDD XMM0, XMM1

	MOVDQA [temp_level_double], XMM0
	MOVDQA XMM1, XMM0
	PADDD XMM1, XMM5
	PSRLD XMM1, XMM3
	MOVDQA [temp_arl_dst_coeff_array], XMM1

	MOVDQA XMM1, XMM0
	PADDD XMM1, XMM6
	PSRLD XMM1, XMM4
	MOVDQA [temp_max_level_array], XMM1

	ADD temp_src_coef, 16
	ADD temp_q_coef, 16
	ADD temp_level_double, 16
	ADD temp_arl_dst_coeff_array, 16
	ADD temp_max_level_array, 16

	ADD I, 1

%if 4 == %2
	CMP I, 4
%endif

%if 8 == %2
	CMP I, 16
%endif

%if 16 == %2
	CMP I, 64
%endif

%if 32 == %2
	CMP I, 256
%endif


	JL %%x265_rdoq_ssse3_level_i1


	MOV r1, r9m
	MOVLPD XMM4, [r1]

	MOV temp_level_double, r2m
	MOV err_scale, r7m
	MOV cost_coeff0, r8m

	MOV I, 0
%%x265_rdoq_ssse3_level_i2:

	MOVDQA XMM5, [temp_level_double]

	MOVAPD XMM7, [err_scale]
	CVTDQ2PD XMM6, XMM5
	MULPD XMM6, XMM6
	MULPD XMM7, XMM6
	MOVAPD [cost_coeff0], XMM7

	ADDSD XMM4, XMM7
	SHUFPD XMM7, XMM7, 3
	ADDSD XMM4, XMM7

	ADD err_scale, 16
	ADD cost_coeff0, 16

	PSRLDQ XMM5, 8

	MOVAPD XMM7, [err_scale]
	CVTDQ2PD XMM6, XMM5
	MULPD XMM6, XMM6
	MULPD XMM7, XMM6
	MOVAPD [cost_coeff0], XMM7

	ADDSD XMM4, XMM7
	SHUFPD XMM7, XMM7, 3
	ADDSD XMM4, XMM7

	ADD err_scale, 16
	ADD cost_coeff0, 16


	ADD temp_level_double, 16

	INC I
%if 4 == %2
	CMP I, 4
%endif

%if 8 == %2
	CMP I, 16
%endif

%if 16 == %2
	CMP I, 64
%endif

%if 32 == %2
	CMP I, 256
%endif

	JL %%x265_rdoq_ssse3_level_i2

	MOV r1, r9m
	MOVLPD [r1], XMM4


	RET


%endmacro


X265_RDO_LEVEL_SSE2_HELP rdoq_0_8_ssse3, 4
X265_RDO_LEVEL_SSE2_HELP rdoq_1_8_ssse3, 8
X265_RDO_LEVEL_SSE2_HELP rdoq_2_8_ssse3, 16
X265_RDO_LEVEL_SSE2_HELP rdoq_3_8_ssse3, 32
