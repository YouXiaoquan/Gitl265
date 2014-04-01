%include "x86inc.asm"

%if 1 == SIZE_OF_PIXEL

%include "ip_ssse3_x86-a.inc"

extern pshuffq_zero
extern pshuffw_zero

%define		X265_OUTPUT_DST_INT					0

%macro SET_DST_INT_ADDRESS 0
%if 1 == X265_OUTPUT_DST_INT
	MOV dst_int, X265_DST_INT_STRIDE
	IMUL dst_int, row
	ADD dst_int, dst_int_parameter
%endif
%endmacro

%macro SET_REF_AND_DST 0
	MOV ref, ref_stride_parameter
	IMUL ref, row
	ADD ref, ref_parameter

	MOV dst, dst_stride_parameter
	IMUL dst, row
%if S_FLAGS == DST_FLAGS
	ADD dst, dst
%endif
	ADD dst, dst_parameter
	SET_DST_INT_ADDRESS
%endmacro

%macro STORE_N_COL_DATA 3
%if P_FLAGS == DST_FLAGS
%if 16 == COL_SIZE
	STORE_N_BYTES_DATA %1   , %2, 16
%elif 12 == COL_SIZE
	STORE_N_BYTES_DATA %1   , %2, 12
%elif 8 == COL_SIZE
	STORE_N_BYTES_DATA %1   , %2,  8
%elif 6 == COL_SIZE
	STORE_N_BYTES_DATA %1   , %2,  6
%elif 4 == COL_SIZE
	STORE_N_BYTES_DATA %1   , %2,  4
%elif 2 == COL_SIZE
	STORE_N_BYTES_DATA %1   , %2,  2
%elif 1 == COL_SIZE
	STORE_N_BYTES_DATA %1   , %2,  1
%endif
%else
%if 16 == COL_SIZE
	STORE_N_BYTES_DATA %1   , %2, 16
	STORE_N_BYTES_DATA %1+16, %3, 16
%elif 13 == COL_SIZE
	STORE_N_BYTES_DATA %1   , %2, 16
	STORE_N_BYTES_DATA %1+16, %3, 10
%elif 12 == COL_SIZE
	STORE_N_BYTES_DATA %1   , %2, 16
	STORE_N_BYTES_DATA %1+16, %3,  8
%elif 9 == COL_SIZE
	STORE_N_BYTES_DATA %1   , %2, 16
	STORE_N_BYTES_DATA %1+16, %3,  2
%elif 8 == COL_SIZE
	STORE_N_BYTES_DATA %1   , %2, 16
%elif 6 == COL_SIZE
	STORE_N_BYTES_DATA %1   , %2, 12
%elif 5 == COL_SIZE
	STORE_N_BYTES_DATA %1   , %2, 10
%elif 4 == COL_SIZE
	STORE_N_BYTES_DATA %1   , %2,  8
%elif 2 == COL_SIZE
	STORE_N_BYTES_DATA %1   , %2,  4
%elif 1 == COL_SIZE
	STORE_N_BYTES_DATA %1   , %2,  2
%endif
%endif
%endmacro

%macro X265_IP_FILTER_ONE_ROW_ONE_COL_HOR_P_HELP_SSSE3 1
	%define COL_SIZE %1
%if P_FLAGS == DST_FLAGS
	READ_N_BYTES_DATA XMMR0, [ref], 16
	STORE_N_COL_DATA dst, XMMR0, XMMR0
	ADD ref, COL_SIZE
	ADD dst, COL_SIZE
%else
	READ_N_BYTES_DATA XMMR0, [ref], 16
	MOVDQA XMMR1, XMMR0
	PUNPCKLBW XMMR0, XMMR7
	PUNPCKHBW XMMR1, XMMR7

	PSLLW XMMR0, XSHIFT
	PSLLW XMMR1, XSHIFT
	PSUBW XMMR0, XSUB
	PSUBW XMMR1, XSUB

	STORE_N_COL_DATA dst, XMMR0, XMMR1
	ADD ref, COL_SIZE
	ADD dst, COL_SIZE*2
%endif
%endmacro

%macro X265_IP_FILTER_ONE_ROW_N_COL_HOR_P_HELP_SSSE3 1
	SET_REF_AND_DST

	MOV col, width_parameter
	CMP col, MAX_COL_SIZE
	JL %%X265_IP_FILTER_ONE_ROW_N_COL_HOR_P_HELP_SSSE3_LABEL_LESS_THAN_MAX_COL_SIZE

%%X265_IP_FILTER_ONE_ROW_N_COL_HOR_P_HELP_SSSE3_LABEL_MAX_COL_SIZE:
	X265_IP_FILTER_ONE_ROW_ONE_COL_HOR_P_HELP_SSSE3 MAX_COL_SIZE
	SUB col, MAX_COL_SIZE
	CMP col, MAX_COL_SIZE
	JGE %%X265_IP_FILTER_ONE_ROW_N_COL_HOR_P_HELP_SSSE3_LABEL_MAX_COL_SIZE

%%X265_IP_FILTER_ONE_ROW_N_COL_HOR_P_HELP_SSSE3_LABEL_LESS_THAN_MAX_COL_SIZE:
%if %1 > 0
	X265_IP_FILTER_ONE_ROW_ONE_COL_HOR_P_HELP_SSSE3 %1
%endif
%endmacro

%macro X265_IP_FILTER_N_ROW_N_COL_HOR_P_HELP_SSSE3 1
	MOV row, 0
%%X265_IP_FILTER_N_ROW_N_COL_HOR_P_HELP_SSSE3_LABEL_LOOP:
	X265_IP_FILTER_ONE_ROW_N_COL_HOR_P_HELP_SSSE3 %1
	INC row
	CMP row, height_parameter
	JL %%X265_IP_FILTER_N_ROW_N_COL_HOR_P_HELP_SSSE3_LABEL_LOOP
%endmacro

%macro RUN_AVAILABLE_IP_FILTER_N_ROW_N_COL_HOR_P_FUNC 1
	MOV r1, 0
	CMP r0, %1
	JNE %%RUN_AVAILABLE_IP_FILTER_N_ROW_N_COL_HOR_P_FUNC_EXIT
	X265_IP_FILTER_N_ROW_N_COL_HOR_P_HELP_SSSE3 %1
	MOV r1, 1
%%RUN_AVAILABLE_IP_FILTER_N_ROW_N_COL_HOR_P_FUNC_EXIT:
%endmacro


%macro X265_IP_FILTER_COPY_P_SSSE3_HELP 2
	%define			DST_FLAGS					%2
	%define			ref_parameter				r0m
	%define			ref_stride_parameter		r1m
	%define			dst_parameter				r2m
	%define			dst_stride_parameter		r3m
	%define			width_parameter				r4m
	%define			height_parameter			r5m
	%define			bit_depth_parameter			r6m
	%define			dst_int_parameter			r7m
	%define			col							r6
	%define			row							r5
	%define			ref							r4
	%define			dst							r3
	%define			shift						r2
	%define			MAX_COL_SIZE				16
	%define			MAX_COL_SIZE_BITS			4
	%define			X265_IF_INTERNAL_PREC_ASM	14
	%define			X265_IF_INTERNAL_OFFS_ASM	(1<<(X265_IF_INTERNAL_PREC_ASM-1))
	%define			XSHIFT						XMMR6
	%define			XSUB						XMMR5

cglobal %1, 0, 7
	MOV shift, X265_IF_INTERNAL_PREC_ASM
	SUB shift, bit_depth_parameter
	MOVD XSHIFT, shift
	PSHUFB XSHIFT, [pshuffq_zero]

	MOV r0, X265_IF_INTERNAL_OFFS_ASM
	MOVD XSUB, r0
	PSHUFB XSUB, [pshuffw_zero]

	PXOR XMMR7, XMMR7


	MOV col, width_parameter
	MOV r0, col
	MOV r1, col
	SHR r1, MAX_COL_SIZE_BITS
	SHL r1, MAX_COL_SIZE_BITS
	SUB r0, r1
	RUN_AVAILABLE_IP_FILTER_N_ROW_N_COL_HOR_P_FUNC  0
	CMP r1, 1
	JE %%X265_IP_FILTER_HOR_P_SSSE3_HELP_EXIT
	RUN_AVAILABLE_IP_FILTER_N_ROW_N_COL_HOR_P_FUNC  8
	CMP r1, 1
	JE %%X265_IP_FILTER_HOR_P_SSSE3_HELP_EXIT
	RUN_AVAILABLE_IP_FILTER_N_ROW_N_COL_HOR_P_FUNC  4
	CMP r1, 1
	JE %%X265_IP_FILTER_HOR_P_SSSE3_HELP_EXIT
	RUN_AVAILABLE_IP_FILTER_N_ROW_N_COL_HOR_P_FUNC 12
	CMP r1, 1
	JE %%X265_IP_FILTER_HOR_P_SSSE3_HELP_EXIT
	RUN_AVAILABLE_IP_FILTER_N_ROW_N_COL_HOR_P_FUNC  1
	CMP r1, 1
	JE %%X265_IP_FILTER_HOR_P_SSSE3_HELP_EXIT
	RUN_AVAILABLE_IP_FILTER_N_ROW_N_COL_HOR_P_FUNC  9
	CMP r1, 1
	JE %%X265_IP_FILTER_HOR_P_SSSE3_HELP_EXIT
	RUN_AVAILABLE_IP_FILTER_N_ROW_N_COL_HOR_P_FUNC  5
	CMP r1, 1
	JE %%X265_IP_FILTER_HOR_P_SSSE3_HELP_EXIT
	RUN_AVAILABLE_IP_FILTER_N_ROW_N_COL_HOR_P_FUNC 13
	CMP r1, 1
	JE %%X265_IP_FILTER_HOR_P_SSSE3_HELP_EXIT
	RUN_AVAILABLE_IP_FILTER_N_ROW_N_COL_HOR_P_FUNC  2
	CMP r1, 1
	JE %%X265_IP_FILTER_HOR_P_SSSE3_HELP_EXIT
	RUN_AVAILABLE_IP_FILTER_N_ROW_N_COL_HOR_P_FUNC  6
	CMP r1, 1
	JE %%X265_IP_FILTER_HOR_P_SSSE3_HELP_EXIT
%%X265_IP_FILTER_HOR_P_SSSE3_HELP_EXIT:






	RET
%endmacro




X265_IP_FILTER_COPY_P_SSSE3_HELP ip_filter_hor_luma_p_p_0_ssse3, P_FLAGS
X265_IP_FILTER_COPY_P_SSSE3_HELP ip_filter_hor_luma_p_s_0_ssse3, S_FLAGS
X265_IP_FILTER_COPY_P_SSSE3_HELP ip_filter_ver_luma_p_p_0_ssse3, P_FLAGS
X265_IP_FILTER_COPY_P_SSSE3_HELP ip_filter_ver_luma_p_s_0_ssse3, S_FLAGS

X265_IP_FILTER_COPY_P_SSSE3_HELP ip_filter_hor_chroma_p_p_0_ssse3, P_FLAGS
X265_IP_FILTER_COPY_P_SSSE3_HELP ip_filter_hor_chroma_p_s_0_ssse3, S_FLAGS
X265_IP_FILTER_COPY_P_SSSE3_HELP ip_filter_ver_chroma_p_p_0_ssse3, P_FLAGS
X265_IP_FILTER_COPY_P_SSSE3_HELP ip_filter_ver_chroma_p_s_0_ssse3, S_FLAGS

%endif


