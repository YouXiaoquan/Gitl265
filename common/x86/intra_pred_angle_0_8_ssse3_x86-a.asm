

%include "x86inc.asm"
%include "intra_pred_angle_ssse3_x86-a.asm"

extern zero_to_32
extern zero_to_32_1
extern eight_16
extern angle_ap
extern left_top_pshuffb_0_15
extern pshuffb_reverse_4
extern pshuffw_zero

	%define			SIZE_OF_ANGLE_AP					4
	%define			v_delta_pos							r3
	%define			hor_flags							1
	%define			ver_flags							2

%if 1 == SIZE_OF_PIXEL

%macro STORE_VECTOR 2
	PACKUSWB %1, %1
%if hor_flags == %2
	MOVD [dst], %1
	ADD_ADDRESS dst, i_dst_stride_parameter
	PSRLDQ %1, 4
	MOVD [dst], %1
	ADD_ADDRESS dst, i_dst_stride_parameter
%else
	MOVD [dst], %1
	ADD_ADDRESS dst, i_dst_stride_parameter
%endif
%endmacro

%macro LOAD_FROM_REF_MAIN 2-3
%if 3 == %0
	MOV r0, [angle_ap+I*SIZE_OF_ANGLE_AP+(%3*64)*SIZE_OF_ANGLE_AP]
	ADD r0, ref_main
	MOVD %1, [r0]
	MOVD %2, [r0+SIZE_OF_PIXEL]
%else
	MOVD %1, [ref_main+(%2*SIZE_OF_PIXEL)]
%endif
%endmacro

%macro PREPARE_XMMR1_XMMR2 1
	ADD v_delta_pos, %1
	MOV r0, v_delta_pos
	AND r0, 31
	IMUL r0, 16
	MOVDQA XMMR2, [zero_to_32_1+r0]
%endmacro

%macro CALC_VALUE 0
	PUNPCKLBW XMMR0, XMMR7
	PMADDUBSW XMMR0, XMMR2
	PADDW XMMR0, [eight_16]
	PSRLW XMMR0, 5
%endmacro

%macro GET_REF_MAIN_ADDRESS_TO_R0 1
	MOV r0, [angle_ap+I*SIZE_OF_ANGLE_AP+(%1*64)*SIZE_OF_ANGLE_AP]
	ADD r0, ref_main
%endmacro

%macro COPY_INVERSE_MEMORY 3
	%define	ref_side											r4
	MOV ref_main, %1
	MOV ref_side, %2
	SUB ref_main, 4
	MOVD XMMR0, [ref_side]
	PSHUFB XMMR0, [pshuffb_reverse_4]
	MOVD [ref_main], XMMR0
	%undef ref_side
%endmacro

%macro INIT_REF_MAIN_M_MEMORY_CONTENT 3
	%define			curr_ref_main						r4
	SUB r6, (16+%1*2)*SIZE_OF_PIXEL
	MOV curr_ref_main, r6
	ADD curr_ref_main, (14 + %1)* SIZE_OF_PIXEL
	COPY_M_PIXEL %1, %2, %3
	MOV r0, r6
	ADD r0, (16+%1)*SIZE_OF_PIXEL
	MOV ref_main, r0
	%undef			curr_ref_main
%endmacro

%macro CALC_FILTER_VALUE 0
	PSUBW XMMR2, XMMR5
	PSRAW XMMR2, 1
	PADDW XMMR2, XMMR3
	PMAXSW XMMR2, XMMR6
	PMINSW XMMR2, XMMR7
%endmacro

%macro PREPARE_PARAMETER 0
	MOV r0, 1
	MOV ecx, r5m
	SHL r0, cl
	DEC r0
	MOVD XMMR7, r0
	PSHUFB XMMR7, [pshuffw_zero]
	PXOR XMMR6, XMMR6
	MOV ref_main, ref_main_parameter
	MOV dst, dst_parameter
	MOV ref_side, ref_side_parameter
	MOV r0, [ref_side-1]
	AND r0, 0FFH
	MOVD XMMR5, r0
	PSHUFB XMMR5, [pshuffw_zero]
	MOVD XMMR4, [ref_side]
	MOV r0, [ref_main]
	AND r0, 0FFH
	MOVD XMMR3, r0
	PSHUFB XMMR3, [pshuffw_zero]
%endmacro

%macro STORE_V 3
	MOV ecx, 8
	MOVD eax, %3
	MOV J, 4
%%STORE_V_J:
	MOV [%1], al
	ADD_ADDRESS %1, %2
	SHR eax, cl
	DEC J
	JNZ %%STORE_V_J
%endmacro

%macro COPY_ONE_PIXEL_FROM_REF_MAIN		2
	MOV al, %2
	MOV %1, al
%endmacro

%macro		COPY_M_PIXEL		3
	%define			ref_main_src						r3
	%define			ref_side							r3
	%define			inv_angle_sum						r2
	MOV ref_main_src, ref_main_parameter
	COPY_ONE_PIXEL_FROM_REF_MAIN [curr_ref_main+SIZE_OF_PIXEL], [ref_main_src-SIZE_OF_PIXEL]
	MOVD XMMR0, [ref_main_src]
	MOVD [curr_ref_main+2*SIZE_OF_PIXEL], XMMR0
	MOV I, %2
	IMUL I, %1
	SAR I, 5
	NEG I
	CMP I, 2
	JL %%COPY_M_PIXEL_I_EXIT
	DEC I
	MOV ref_side, ref_side_parameter
	MOV inv_angle_sum, 128
%%COPY_M_PIXEL_I_2
	ADD inv_angle_sum, %3
	MOV r1, inv_angle_sum
	SHR r1, 8
	COPY_ONE_PIXEL_FROM_REF_MAIN [curr_ref_main], [ref_side+r1*SIZE_OF_PIXEL-SIZE_OF_PIXEL]
	SUB curr_ref_main, SIZE_OF_PIXEL
	DEC I
	JNZ %%COPY_M_PIXEL_I_2
%%COPY_M_PIXEL_I_EXIT:
	%undef			ref_main_src
	%undef			ref_side
	%undef			inv_angle_sum
%endmacro

%macro PREDANG_CALCROW_HOR 3
	PREPARE_XMMR1_XMMR2 %1
	LOAD_FROM_REF_MAIN XMMR0, XMMR7, %2
	CALC_VALUE
	MOVDQA %3, XMMR0
	INC I
%endmacro

%macro PREDANG_CALCROW_VER 3
	PREPARE_XMMR1_XMMR2 %1
	GET_REF_MAIN_ADDRESS_TO_R0 %2
	MOVD XMMR0, [r0]
	MOVD XMMR7, [r0+SIZE_OF_PIXEL]
	CALC_VALUE
	STORE_VECTOR XMMR0, ver_flags
%endmacro

%macro PREDANG_CALCROW_VER_MODE2 3
	PREPARE_XMMR1_XMMR2 %3
	MOVQ XMMR0, %1
	PUNPCKLBW XMMR0, %2
	PMADDUBSW XMMR0, XMMR2
	PADDW XMMR0, [eight_16]
	PSRLW XMMR0, 5
	STORE_VECTOR XMMR0, ver_flags
%endmacro

%macro PREDANG_CALCROW_HOR_MODE2 4
	PREPARE_XMMR1_XMMR2 %3
	MOVQ XMMR0, %1
	PUNPCKLBW XMMR0, %2
	PMADDUBSW XMMR0, XMMR2
	PADDW XMMR0, [eight_16]
	PSRLW XMMR0, 5
	MOVDQA %4, XMMR0
	INC I
%endmacro

%macro MB8 5
	PUNPCKLWD XMMR%1, XMMR%2
	PUNPCKLWD XMMR%3, XMMR%4
	PUNPCK %1, %3, %5, DQ

	STORE_VECTOR XMMR%1, hor_flags
	STORE_VECTOR XMMR%3, hor_flags
%endmacro


%macro CALC_BLND_ROWS 2
	MOV dst, dst_parameter
	PREDANG_CALCROW_HOR %1, %2, XMMR3
	PREDANG_CALCROW_HOR %1, %2, XMMR4
	PREDANG_CALCROW_HOR %1, %2, XMMR5
	PREDANG_CALCROW_HOR %1, %2, XMMR6
	MB8 3, 4, 5, 6, 0
%endmacro CALC_FIRST_BLND_ROWS

%macro CALC_BLND_ROWS_MODE2 5
	MOV dst, dst_parameter
	PREDANG_CALCROW_HOR_MODE2 %1, %2, %4, XMMR3
	PREDANG_CALCROW_HOR_MODE2 %1, %2, %4, XMMR4
	PREDANG_CALCROW_HOR_MODE2 %1, %2, %4, XMMR5
	PREDANG_CALCROW_HOR_MODE2 %1, %2, %4, XMMR6
	MB8 3, 4, 5, 6, 0
%endmacro

%macro CALC_BLND_ROWS_MODE2_VER 4
	MOV I, 0
%%CALC_BLND_ROWS_MODE2_VER_LABEL:
	PREDANG_CALCROW_VER_MODE2 %1, %2, %4
	INC I
	CMP I, 4
	JL %%CALC_BLND_ROWS_MODE2_VER_LABEL
%endmacro

%macro X265_INTRA_PRED_ANGLE_32_HELP_PIXEL_SSSE3 3
	MOV I, 4
%%X265_INTRA_PRED_ANGLE_32_HELP_PIXEL_SSSE3_I:
	MOVD XMMR0, [ref_main]
	MOVD [dst], XMMR0
	ADD_ADDRESS dst, i_dst_stride_parameter
    INC_ADDRESS ref_main
	DEC I
	JNZ %%X265_INTRA_PRED_ANGLE_32_HELP_PIXEL_SSSE3_I
%endmacro

%macro X265_INTRA_PRED_ANGLE_H_HELP_PIXEL_SSSE3 4
	MOV v_delta_pos, 0
	MOV I, 0
	CALC_BLND_ROWS %2, %3
%endmacro

%macro X265_INTRA_PRED_ANGLE_H_2_HELP_PIXEL_SSSE3 3
	MOV v_delta_pos, 0
	MOV I, 0
	LOAD_FROM_REF_MAIN XMMR6, 0
	LOAD_FROM_REF_MAIN XMMR7, 1
	CALC_BLND_ROWS_MODE2 XMMR6, XMMR7, XMMR7, %3, 0
%endmacro

%macro X265_INTRA_PRED_ANGLE_H_0_HELP_PIXEL_SSSE3 3
	MOV dst, dst_parameter
	MOV r0, r4m
	CMP r0, 0
	JNE %%X265_INTRA_PRED_ANGLE_H_0_HELP_PIXEL_SSSE3_LABEL_COPY_1
	PREPARE_PARAMETER
	MOVQ XMMR2, XMMR4
	PUNPCKLBW XMMR2, XMMR6
	CALC_FILTER_VALUE
	PACKUSWB XMMR2, XMMR2
	MOVD [dst], XMMR2
	ADD_ADDRESS dst, i_dst_stride_parameter
	JMP %%X265_INTRA_PRED_ANGLE_H_0_HELP_PIXEL_SSSE3_LABEL_COPY_7
%%X265_INTRA_PRED_ANGLE_H_0_HELP_PIXEL_SSSE3_LABEL_COPY_1:
	MOV ref_main, ref_main_parameter
	MOVD XMMR0, [ref_main]
	PSHUFB XMMR0, [left_top_pshuffb_0_15]
	MOVD [dst], XMMR0
	ADD_ADDRESS dst, i_dst_stride_parameter
%%X265_INTRA_PRED_ANGLE_H_0_HELP_PIXEL_SSSE3_LABEL_COPY_7:
	MOV ref_main, ref_main_parameter
	MOVD XMMR0, [ref_main]
	MOV J, 16
%%X265_INTRA_PRED_ANGLE_H_0_HELP_PIXEL_SSSE3_LABEL_J:
	MOVQ XMMR1, XMMR0
	PSHUFB XMMR1, [left_top_pshuffb_0_15+J]
	MOVD [dst], XMMR1
	ADD_ADDRESS dst, i_dst_stride_parameter
	ADD J, 16
	CMP J, 64
	JL %%X265_INTRA_PRED_ANGLE_H_0_HELP_PIXEL_SSSE3_LABEL_J
%endmacro

%macro X265_INTRA_PRED_ANGLE_H_M2_HELP_PIXEL_SSSE3 3
	MOV v_delta_pos, 0
	MOV I, 0
	LOAD_FROM_REF_MAIN XMMR6, -1
	LOAD_FROM_REF_MAIN XMMR7,  0
	CALC_BLND_ROWS_MODE2 XMMR6, XMMR7, XMMR6, %3, 0
%endmacro

%macro X265_INTRA_PRED_ANGLE_V_HELP_PIXEL_SSSE3 4
	MOV v_delta_pos, 0
	MOV dst, dst_parameter
	MOV I, 0
%%X265_INTRA_PRED_ANGLE_V_HELP_PIXEL_SSSE3_LABEL:
	PREDANG_CALCROW_VER %2, %3, %4
	INC I
	CMP I, 4
	JL %%X265_INTRA_PRED_ANGLE_V_HELP_PIXEL_SSSE3_LABEL
%endmacro

%macro X265_INTRA_PRED_ANGLE_V_2_HELP_PIXEL_SSSE3 3
	MOV dst, dst_parameter
	MOV v_delta_pos, 0
	LOAD_FROM_REF_MAIN XMMR6, 0
	LOAD_FROM_REF_MAIN XMMR7, 1
	CALC_BLND_ROWS_MODE2_VER XMMR6, XMMR7, XMMR7, %3
%endmacro

%macro X265_INTRA_PRED_ANGLE_V_0_HELP_PIXEL_SSSE3 3
	%define			ref_side		r3
	MOV ref_main, ref_main_parameter
	MOV dst, dst_parameter
	MOVD XMMR0, [ref_main]
	MOV I, 4
%%X265_INTRA_PRED_ANGLE_V_0_HELP_PIXEL_SSSE3_I:
	MOVD [dst], XMMR0
	ADD_ADDRESS dst, i_dst_stride_parameter
	DEC I
	JNZ %%X265_INTRA_PRED_ANGLE_V_0_HELP_PIXEL_SSSE3_I
	MOV r0, r4m
	CMP r0, 0
	JNE %%X265_INTRA_PRED_ANGLE_V_0_HELP_PIXEL_SSSE3_EXIT
	PREPARE_PARAMETER
	MOVQ XMMR2, XMMR4
	PUNPCKLBW XMMR2, XMMR6
	CALC_FILTER_VALUE
	PACKUSWB XMMR2, XMMR2
	STORE_V dst, i_dst_stride_parameter, XMMR2
%%X265_INTRA_PRED_ANGLE_V_0_HELP_PIXEL_SSSE3_EXIT:
	%undef		ref_side
%endmacro

%macro X265_INTRA_PRED_ANGLE_V_M2_HELP_PIXEL_SSSE3 3
	MOV dst, dst_parameter
	MOV v_delta_pos, 0
	LOAD_FROM_REF_MAIN XMMR6, -1
	LOAD_FROM_REF_MAIN XMMR7,  0
	CALC_BLND_ROWS_MODE2_VER XMMR6, XMMR7, XMMR6, %3
%endmacro

%macro X265_INTRA_PRED_ANGLE_M32_HELP_PIXEL_SSSE3 5
	MOV ref_main, %4
	MOV dst, %5
	MOV I, 4
%%X265_INTRA_PRED_ANGLE_M32_HELP_PIXEL_SSSE3_I:
	MOVD XMMR0, [ref_main]
	MOVD [dst], XMMR0
	ADD_ADDRESS dst, i_dst_stride_parameter
	DEC_ADDRESS ref_main
	DEC I
	JNZ %%X265_INTRA_PRED_ANGLE_M32_HELP_PIXEL_SSSE3_I
%endmacro

%else


%macro STORE_VECTOR 2
%if hor_flags == %2
	MOVQ [dst], %1
	ADD_ADDRESS dst, i_dst_stride_parameter
	PSRLDQ %1, 8
	MOVQ [dst], %1
	ADD_ADDRESS dst, i_dst_stride_parameter
%else
	MOVQ [dst], %1
	ADD_ADDRESS dst, i_dst_stride_parameter
%endif
%endmacro

%macro LOAD_FROM_REF_MAIN 2-3
%if 3 == %0
	MOV r0, [angle_ap+I*SIZE_OF_ANGLE_AP+(%3*64)*SIZE_OF_ANGLE_AP]
	ADD r0, r0
	ADD r0, ref_main
	MOVQ %1, [r0]
	MOVQ %2, [r0+SIZE_OF_PIXEL]
%else
	MOVQ %1, [ref_main+(%2*SIZE_OF_PIXEL)]
%endif
%endmacro

%macro PREPARE_XMMR1_XMMR2 1
	ADD v_delta_pos, %1
	MOV r0, v_delta_pos
	AND r0, 31
	IMUL r0, 16
	MOVDQA XMMR1, [zero_to_32+r0]
	MOVDQA XMMR2, [zero_to_32+512]
	PSUBW XMMR2, XMMR1
%endmacro

%macro CALC_VALUE 0
	PMULLW XMMR0, XMMR2
	PMULLW XMMR7, XMMR1
	PADDW XMMR0, XMMR7
	PADDW XMMR0, [eight_16]
	PSRLW XMMR0, 5
%endmacro

%macro GET_REF_MAIN_ADDRESS_TO_R0 1
	MOV r0, [angle_ap+I*SIZE_OF_ANGLE_AP+(%1*64)*SIZE_OF_ANGLE_AP]
	ADD r0, r0
	ADD r0, ref_main
%endmacro

%macro CALC_FILTER_VALUE 0
	PSUBW XMMR2, XMMR5
	PSRAW XMMR2, 1
	PADDW XMMR2, XMMR3
	PMAXSW XMMR2, XMMR6
	PMINSW XMMR2, XMMR7
%endmacro

%macro PREPARE_PARAMETER 0
	MOV r0, 1
	MOV ecx, r5m
	SHL r0, cl
	DEC r0
	MOVD XMMR7, r0
	PSHUFB XMMR7, [pshuffw_zero]
	PXOR XMMR6, XMMR6
	MOV ref_main, ref_main_parameter
	MOV dst, dst_parameter
	MOV ref_side, ref_side_parameter
	MOV r0, [ref_side-SIZE_OF_PIXEL]
	AND r0, 0FFFFH
	MOVD XMMR5, r0
	PSHUFB XMMR5, [pshuffw_zero]
	MOVQ XMMR4, [ref_side]
	MOV r0, [ref_main]
	AND r0, 0FFFFH
	MOVD XMMR3, r0
	PSHUFB XMMR3, [pshuffw_zero]
%endmacro

%macro STORE_V 3
	MOV ecx, 16
	MOV I, 2
%%STORE_V_I:
	MOVD eax, %3
	MOV J, 2
%%STORE_V_J:
	MOV [%1], ax
	ADD_ADDRESS %1, %2
	SHR eax, cl
	DEC J
	JNZ %%STORE_V_J
	PSRLDQ %3, 4
	DEC I
	JNZ %%STORE_V_I
%endmacro



%macro COPY_INVERSE_MEMORY 3
	%define	ref_side											r4
	MOV ref_main, %1
	MOV ref_side, %2
	SUB ref_main, 8
	MOVQ XMMR0, [ref_side]
	PSHUFB XMMR0, [pshuffb_reverse_4]
	MOVQ [ref_main], XMMR0
	%undef ref_side
%endmacro


%macro COPY_ONE_PIXEL_FROM_REF_MAIN		2
	MOV ax, %2
	MOV %1, ax
%endmacro

%macro		COPY_M_PIXEL		3
	%define			ref_main_src						r3
	%define			ref_side							r3
	%define			inv_angle_sum						r2
	MOV ref_main_src, ref_main_parameter
	COPY_ONE_PIXEL_FROM_REF_MAIN [curr_ref_main+SIZE_OF_PIXEL], [ref_main_src-SIZE_OF_PIXEL]
	MOVQ XMMR0, [ref_main_src]
	MOVQ [curr_ref_main+2*SIZE_OF_PIXEL], XMMR0
	MOV I, %2
	IMUL I, %1
	SAR I, 5
	NEG I
	CMP I, 2
	JL %%COPY_M_PIXEL_I_EXIT
	DEC I
	MOV ref_side, ref_side_parameter
	MOV inv_angle_sum, 128
%%COPY_M_PIXEL_I_2
	ADD inv_angle_sum, %3
	MOV r1, inv_angle_sum
	SHR r1, 8
	COPY_ONE_PIXEL_FROM_REF_MAIN [curr_ref_main], [ref_side+r1*SIZE_OF_PIXEL-SIZE_OF_PIXEL]
	SUB curr_ref_main, SIZE_OF_PIXEL
	DEC I
	JNZ %%COPY_M_PIXEL_I_2
%%COPY_M_PIXEL_I_EXIT:
	%undef			ref_main_src
	%undef			ref_side
	%undef			inv_angle_sum
%endmacro

%macro INIT_REF_MAIN_M_MEMORY_CONTENT 3
	%define			curr_ref_main						r4
	SUB r6, (16+%1*2)*SIZE_OF_PIXEL
	MOV curr_ref_main, r6
	ADD curr_ref_main, (14 + %1)* SIZE_OF_PIXEL
	COPY_M_PIXEL %1, %2, %3
	MOV r0, r6
	ADD r0, (16+%1)*SIZE_OF_PIXEL
	MOV ref_main, r0
	%undef			curr_ref_main
%endmacro


%macro PREDANG_CALCROW_HOR 3
	PREPARE_XMMR1_XMMR2 %1
	LOAD_FROM_REF_MAIN XMMR0, XMMR7, %2
	CALC_VALUE
	MOVDQA %3, XMMR0
	INC I
%endmacro

%macro PREDANG_CALCROW_VER 3
	PREPARE_XMMR1_XMMR2 %1
	GET_REF_MAIN_ADDRESS_TO_R0 %2
	MOVQ XMMR0, [r0]
	MOVQ XMMR7, [r0+SIZE_OF_PIXEL]
	CALC_VALUE
	STORE_VECTOR XMMR0, ver_flags
%endmacro

%macro PREDANG_CALCROW_VER_MODE2 3
	PREPARE_XMMR1_XMMR2 %3
	MOVQ XMMR0, %1
	PMULLW XMMR0, XMMR2
	PMULLW XMMR1, %2
	PADDW XMMR0, XMMR1
	PADDW XMMR0, [eight_16]
	PSRLW XMMR0, 5
	STORE_VECTOR XMMR0, ver_flags
%endmacro

%macro PREDANG_CALCROW_HOR_MODE2 4
	PREPARE_XMMR1_XMMR2 %3
	MOVQ XMMR0, %1
	PMULLW XMMR0, XMMR2
	PMULLW XMMR1, %2
	PADDW XMMR0, XMMR1
	PADDW XMMR0, [eight_16]
	PSRLW XMMR0, 5
	MOVDQA %4, XMMR0
	INC I
%endmacro

%macro MB8 5
	PUNPCKLWD XMMR%1, XMMR%2
	PUNPCKLWD XMMR%3, XMMR%4
	PUNPCK %1, %3, %5, DQ

	STORE_VECTOR XMMR%1, hor_flags
	STORE_VECTOR XMMR%3, hor_flags
%endmacro


%macro CALC_BLND_ROWS 2
	MOV dst, dst_parameter
	PREDANG_CALCROW_HOR %1, %2, XMMR3
	PREDANG_CALCROW_HOR %1, %2, XMMR4
	PREDANG_CALCROW_HOR %1, %2, XMMR5
	PREDANG_CALCROW_HOR %1, %2, XMMR6
	MB8 3, 4, 5, 6, 0
%endmacro CALC_FIRST_BLND_ROWS

%macro CALC_BLND_ROWS_MODE2 5
	MOV dst, dst_parameter
	PREDANG_CALCROW_HOR_MODE2 %1, %2, %4, XMMR3
	PREDANG_CALCROW_HOR_MODE2 %1, %2, %4, XMMR4
	PREDANG_CALCROW_HOR_MODE2 %1, %2, %4, XMMR5
	PREDANG_CALCROW_HOR_MODE2 %1, %2, %4, XMMR6
	MB8 3, 4, 5, 6, 0
%endmacro

%macro CALC_BLND_ROWS_MODE2_VER 4
	MOV I, 0
%%CALC_BLND_ROWS_MODE2_VER_LABEL:
	PREDANG_CALCROW_VER_MODE2 %1, %2, %4
	INC I
	CMP I, 4
	JL %%CALC_BLND_ROWS_MODE2_VER_LABEL
%endmacro


%macro X265_INTRA_PRED_ANGLE_32_HELP_PIXEL_SSSE3 3
	MOV I, 4
%%X265_INTRA_PRED_ANGLE_32_HELP_PIXEL_SSSE3_I:
	MOVQ XMMR0, [ref_main]
	MOVQ [dst], XMMR0
	ADD_ADDRESS dst, i_dst_stride_parameter
    INC_ADDRESS ref_main
	DEC I
	JNZ %%X265_INTRA_PRED_ANGLE_32_HELP_PIXEL_SSSE3_I
%endmacro

%macro X265_INTRA_PRED_ANGLE_H_HELP_PIXEL_SSSE3 4
	MOV v_delta_pos, 0
	MOV I, 0
	CALC_BLND_ROWS %2, %3
%endmacro

%macro X265_INTRA_PRED_ANGLE_H_2_HELP_PIXEL_SSSE3 3
	MOV v_delta_pos, 0
	MOV I, 0
	LOAD_FROM_REF_MAIN XMMR6, 0
	LOAD_FROM_REF_MAIN XMMR7, 1
	CALC_BLND_ROWS_MODE2 XMMR6, XMMR7, XMMR7, %3, 0
%endmacro

%macro X265_INTRA_PRED_ANGLE_H_0_HELP_PIXEL_SSSE3 3
	MOV dst, dst_parameter
	MOV r0, r4m
	CMP r0, 0
	JNE %%X265_INTRA_PRED_ANGLE_H_0_HELP_PIXEL_SSSE3_LABEL_COPY_1
	PREPARE_PARAMETER
	MOVQ XMMR2, XMMR4
	CALC_FILTER_VALUE
	MOVQ [dst], XMMR2
	ADD_ADDRESS dst, i_dst_stride_parameter
	JMP %%X265_INTRA_PRED_ANGLE_H_0_HELP_PIXEL_SSSE3_LABEL_COPY_7
%%X265_INTRA_PRED_ANGLE_H_0_HELP_PIXEL_SSSE3_LABEL_COPY_1:
	MOV ref_main, ref_main_parameter
	MOVQ XMMR0, [ref_main]
	PSHUFB XMMR0, [left_top_pshuffb_0_15]
	MOVQ [dst], XMMR0
	ADD_ADDRESS dst, i_dst_stride_parameter
%%X265_INTRA_PRED_ANGLE_H_0_HELP_PIXEL_SSSE3_LABEL_COPY_7:
	MOV ref_main, ref_main_parameter
	MOVQ XMMR0, [ref_main]
	MOV J, 16
%%X265_INTRA_PRED_ANGLE_H_0_HELP_PIXEL_SSSE3_LABEL_J:
	MOVQ XMMR1, XMMR0
	PSHUFB XMMR1, [left_top_pshuffb_0_15+J]
	MOVQ [dst], XMMR1
	ADD_ADDRESS dst, i_dst_stride_parameter
	ADD J, 16
	CMP J, 64
	JL %%X265_INTRA_PRED_ANGLE_H_0_HELP_PIXEL_SSSE3_LABEL_J
%endmacro

%macro X265_INTRA_PRED_ANGLE_H_M2_HELP_PIXEL_SSSE3 3
	MOV v_delta_pos, 0
	MOV I, 0
	LOAD_FROM_REF_MAIN XMMR6, -1
	LOAD_FROM_REF_MAIN XMMR7,  0
	CALC_BLND_ROWS_MODE2 XMMR6, XMMR7, XMMR6, %3, 0
%endmacro

%macro X265_INTRA_PRED_ANGLE_V_HELP_PIXEL_SSSE3 4
	MOV v_delta_pos, 0
	MOV dst, dst_parameter
	MOV I, 0
%%X265_INTRA_PRED_ANGLE_V_HELP_PIXEL_SSSE3_LABEL:
	PREDANG_CALCROW_VER %2, %3, %4
	INC I
	CMP I, 4
	JL %%X265_INTRA_PRED_ANGLE_V_HELP_PIXEL_SSSE3_LABEL
%endmacro

%macro X265_INTRA_PRED_ANGLE_V_2_HELP_PIXEL_SSSE3 3
	MOV dst, dst_parameter
	MOV v_delta_pos, 0
	LOAD_FROM_REF_MAIN XMMR6, 0
	LOAD_FROM_REF_MAIN XMMR7, 1
	CALC_BLND_ROWS_MODE2_VER XMMR6, XMMR7, XMMR7, %3
%endmacro

%macro X265_INTRA_PRED_ANGLE_V_0_HELP_PIXEL_SSSE3 3
	MOV ref_main, ref_main_parameter
	MOV dst, dst_parameter
	MOVQ XMMR0, [ref_main]
	MOV I, 4
%%X265_INTRA_PRED_ANGLE_V_0_HELP_PIXEL_SSSE3_I:
	MOVQ [dst], XMMR0
	ADD_ADDRESS dst, i_dst_stride_parameter
	DEC I
	JNZ %%X265_INTRA_PRED_ANGLE_V_0_HELP_PIXEL_SSSE3_I
	MOV r0, r4m
	CMP r0, 0
	JNE %%X265_INTRA_PRED_ANGLE_V_0_HELP_PIXEL_SSSE3_EXIT
	PREPARE_PARAMETER
	MOVQ XMMR2, XMMR4
	CALC_FILTER_VALUE
	STORE_V dst, i_dst_stride_parameter, XMMR2
%%X265_INTRA_PRED_ANGLE_V_0_HELP_PIXEL_SSSE3_EXIT:
%endmacro

%macro X265_INTRA_PRED_ANGLE_V_M2_HELP_PIXEL_SSSE3 3
	MOV dst, dst_parameter
	MOV v_delta_pos, 0
	LOAD_FROM_REF_MAIN XMMR6, -1
	LOAD_FROM_REF_MAIN XMMR7,  0
	CALC_BLND_ROWS_MODE2_VER XMMR6, XMMR7, XMMR6, %3
%endmacro

%macro X265_INTRA_PRED_ANGLE_M32_HELP_PIXEL_SSSE3 5
	MOV ref_main, %4
	MOV dst, %5
	MOV I, 4
%%X265_INTRA_PRED_ANGLE_M32_HELP_PIXEL_SSSE3_I:
	MOVQ XMMR0, [ref_main]
	MOVQ [dst], XMMR0
	ADD_ADDRESS dst, i_dst_stride_parameter
	DEC_ADDRESS ref_main
	DEC I
	JNZ %%X265_INTRA_PRED_ANGLE_M32_HELP_PIXEL_SSSE3_I
%endmacro

%endif

SECTION .text align=16
DEFINE_INTRA_PRED_FUNC 0_8, 4
