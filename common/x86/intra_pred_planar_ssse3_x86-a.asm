



%include "x86inc.asm"

extern left_top_pshuffb_0_15

%if 1 == SIZE_OF_PIXEL

%define					left						r6
%define					top							r5
%define					dst							r4
%define					i_dst_stride				r3m

%macro INITIALIZE 0
	MOV left, r0m
	MOV top, r1m
	MOV dst, r2m
%endmacro

%macro CALC_PLANAR_1_8 5
	%define			num_sub_x		%1
	%define			top_x			%2
	%define			add_num			%3
	%define			shift_num		%4
	%define			calc_dst_data	%5

	;((15 - x) * left[y]
	MOVQ XMMR2, XMMR0
	PSHUFB XMMR2, [left_top_pshuffb_0_15+r3]
	;(x + 1) * top[8]
	PUNPCKLBW XMMR2, XMMR3
	PMADDUBSW XMMR2, num_sub_x

	;(15 - y) * top[x]
	MOVQ XMMR7, top_x
	PUNPCKLBW XMMR7, XMMR1
	PMADDUBSW XMMR7, XMMR5

	PADDW XMMR2, XMMR7

	PADDW XMMR2, add_num
	PSRLW XMMR2, shift_num

	PACKUSWB XMMR2, XMMR2
	MOVQ calc_dst_data, XMMR2

	%undef num_sub_x
	%undef x_add_one
	%undef top_x
	%undef add_num
	%undef shift_num
	%undef calc_dst_data
%endmacro

%macro CALC_PLANAR	5
	%define			num_sub_x		%1
	%define			top_x			%2
	%define			add_num			%3
	%define			shift_num		%4
	%define			calc_dst_data	%5

	MOVDQA XMMR4, top_x

	MOVQ XMMR7, XMMR2
	PUNPCKLBW XMMR7, XMMR3
	PMADDUBSW XMMR7, [num_sub_x]

	;(15 - y) * top[x]
	MOVDQA XMMR6, XMMR4
	PUNPCKLBW XMMR6, XMMR1
	PMADDUBSW XMMR6, XMMR5

	PADDW XMMR7, XMMR6
	PADDW XMMR7, add_num
	PSRLW XMMR7, shift_num

	PACKUSWB XMMR7, XMMR7
	MOVQ [calc_dst_data], XMMR7

	MOVQ XMMR7, XMMR2
	PUNPCKLBW XMMR7, XMMR3
	PMADDUBSW XMMR7, [num_sub_x+16]

	;(15 - y) * top[x]
	MOVDQA XMMR6, XMMR4
	PUNPCKHBW XMMR6, XMMR1
	PMADDUBSW XMMR6, XMMR5

	PADDW XMMR7, XMMR6
	PADDW XMMR7, add_num
	PSRLW XMMR7, shift_num

	PACKUSWB XMMR7, XMMR7
	MOVQ [calc_dst_data+8], XMMR7

	%undef num_sub_x
	%undef x_add_one
	%undef top_x
	%undef add_num
	%undef shift_num
	%undef calc_dst_data
%endmacro

%macro LOAD_LEFT_TOP_N 1
	%define		N		%1
	MOVQ XMMR1, [left + SIZE_OF_PIXEL * N]
	MOVQ XMMR3, [top + SIZE_OF_PIXEL * N]
	PSHUFB XMMR1, [left_top_pshuffb_0]
	PSHUFB XMMR3, [left_top_pshuffb_0]
	%undef N
%endmacro

%macro X265_INTRA_PRED_PLANAR_SSE2_HELP 4
	MOV r3, 0
%%X265_INTRA_PRED_PLANAR_SSE2_HELP1:
	MOVDQA XMMR2, XMMR0
	PSHUFB XMMR2, [left_top_pshuffb_0_15+r3]
	PADDB XMMR5, [eight_minus1_eight_one_ssse3]
	MOV r1, 0
%%X265_INTRA_PRED_PLANAR_SSE2_HELP2:
	CALC_PLANAR %1+r1+r1, [top+r1], %2, %3, dst+r1
	ADD r1, 16
	CMP r1, %4
	JL %%X265_INTRA_PRED_PLANAR_SSE2_HELP2

	ADD_ADDRESS dst, i_dst_stride
	ADD r3, 16
	CMP r3, 256
	JL %%X265_INTRA_PRED_PLANAR_SSE2_HELP1
%endmacro

%macro X265_INTRA_PRED_PLANAR_HELP_1_8_SSE2 4
	MOV r3, 0
%%X265_INTRA_PRED_PLANAR_HELP_1_8_SSE2:
	PADDB XMMR5, [eight_minus1_eight_one_ssse3]
	CALC_PLANAR_1_8 [%1], [top], %2, %3, [dst]
	ADD_ADDRESS dst, i_dst_stride
	ADD r3, 16
	CMP r3, 128
	JL %%X265_INTRA_PRED_PLANAR_HELP_1_8_SSE2
%endmacro

%macro x265_intra_pred_planar_ssse3 5
cglobal %1, 0, 7
	INITIALIZE
	LOAD_LEFT_TOP_N %5

	MOVDQA XMMR5, %3
	PXOR XMMR6, XMMR6
	MOV r0, 0
%%x265_intra_pred_planar_ssse3_label:
	;top[x]
	MOVDQA XMMR0, [left + SIZE_OF_PIXEL * r0]
	X265_INTRA_PRED_PLANAR_SSE2_HELP %2, %3, %4, %5
	ADD r0, 16
	CMP r0, %5
	JL %%x265_intra_pred_planar_ssse3_label
	RET
%endmacro

SECTION .rodata align=16

three_to_0_3_to_0_ssse3				DB			 3,  1,  2,  2,  1,  3,  0,  4
									DB			 3,  1,  2,  2,  1,  3,  0,  4

four_3_four_2_ssse3					DB			 3,  1,  3,  1,  3,  1,  3,  1
									DB			 2,  2,  2,  2,  2,  2,  2,  2

four_1_four_0_ssse3					DB			 1,  3,  1,  3,  1,  3,  1,  3
									DB			 0,  4,  0,  4,  0,  4,  0,  4


eight_minus1_eight_one_ssse3			DB			0FFH,  1H,  0FFH,  1H,  0FFH,  1H,  0FFH,  1H
									DB			0FFH,  1H,  0FFH,  1H,  0FFH,  1H,  0FFH,  1H

num_desc_asc_1_8_ssse3				DB			 7,  1,  6,  2,  5,  3,  4,  4
									DB			 3,  5,  2,  6,  1,  7,  0,  8

num_desc_asc_2_8_ssse3				DB			15,  1, 14,  2, 13,  3, 12,  4
									DB			11,  5, 10,  6,  9,  7,  8,  8
									DB			 7,  9,  6, 10,  5, 11,  4, 12
									DB			 3, 13,  2, 14,  1, 15,  0, 16


num_desc_asc_3_8_ssse3				DB			31,  1, 30,  2, 29,  3, 28,  4
									DB			27,  5, 26,  6, 25,  7, 24,  8
									DB			23,  9, 22, 10, 21, 11, 20, 12
									DB			19, 13, 18, 14, 17, 15, 16, 16
									DB			15, 17, 14, 18, 13, 19, 12, 20
									DB			11, 21, 10, 22,  9, 23,  8, 24
									DB			 7, 25,  6, 26,  5, 27,  4, 28
									DB			 3, 29,  2, 30,  1, 31,  0, 32

num_desc_asc_4_8_ssse3				DB			63,  1, 62,  2, 61,  3, 60,  4
									DB			59,  5, 58,  6, 57,  7, 56,  8
									DB			55,  9, 54, 10, 53, 11, 52, 12
									DB			51, 13, 50, 14, 49, 15, 48, 16
									DB			47, 17, 46, 18, 45, 19, 44, 20
									DB			43, 21, 42, 22, 41, 23, 40, 24
									DB			39, 25, 38, 26, 37, 27, 36, 28
									DB			35, 29, 34, 30, 33, 31, 32, 32
									DB			31, 33, 30, 34, 29, 35, 28, 36
									DB			27, 37, 26, 38, 25, 39, 24, 40
									DB			23, 41, 22, 42, 21, 43, 20, 44
									DB			19, 45, 18, 46, 17, 47, 16, 48
									DB			15, 49, 14, 50, 13, 51, 12, 52
									DB			11, 53, 10, 54,  9, 55,  8, 56
									DB			 7, 57,  6, 58,  5, 59,  4, 60
									DB			 3, 61,  2, 62,  1, 63,  0, 64



eight_4_ssse3						DW			 4,  4,  4,  4,  4,  4,  4,  4
eight_8_ssse3						DW			 8,  8,  8,  8,  8,  8,  8,  8
eight_16_ssse3						DW			16, 16, 16, 16, 16, 16, 16, 16
eight_32_ssse3						DW			32, 32, 32, 32, 32, 32, 32, 32
eight_64_ssse3						DW			64, 64, 64, 64, 64, 64, 64, 64

left_top_pshuffb_4					DB			04H, 04H, 04H, 04H, 04H, 04H, 04H, 04H
									DB			04H, 04H, 04H, 04H, 04H, 04H, 04H, 04H

left_top_pshuffb_0					DB			00H, 00H, 00H, 00H, 00H, 00H, 00H, 00H
									DB			00H, 00H, 00H, 00H, 00H, 00H, 00H, 00H

left_0_1							DB			00H, 00H, 00H, 00H, 01H, 01H, 01H, 01H
									DB			00H, 00H, 00H, 00H, 01H, 01H, 01H, 01H

left_2_3							DB			02H, 02H, 02H, 02H, 03H, 03H, 03H, 03H
									DB			02H, 02H, 02H, 02H, 03H, 03H, 03H, 03H


SECTION .text align=16

cglobal intra_pred_mode_0_0_8_ssse3, 0, 7
	INITIALIZE

	MOVQ XMMR6, [left]

	;left[4]
	MOVQ XMMR0, XMMR6
	PSHUFB XMMR0, [left_top_pshuffb_4]

	;left[y], (y = 0, y = 1)
	MOVQ XMMR1, XMMR6
	PSHUFB XMMR1, [left_0_1]

	;left[y], (y = 2, y = 3)
	MOVQ XMMR2, XMMR6
	PSHUFB XMMR2, [left_2_3]

	MOVQ XMMR6, [top]

	;top[4]
	MOVQ XMMR3, XMMR6
	PSHUFB XMMR3, [left_top_pshuffb_4]

	;top[x]
											;11100100
	MOVQ XMMR4, XMMR6
	PUNPCKLDQ XMMR4, XMMR4

	PUNPCKLBW XMMR1, XMMR3
	;((3 - x) * left[y], (y = 0, y = 1)
	PMADDUBSW XMMR1, [three_to_0_3_to_0_ssse3]


	;(3 - y) * top[x], (y = 0, y = 1)
	MOVQ XMMR5, XMMR4
	PUNPCKLBW XMMR5, XMMR0
	PMADDUBSW XMMR5, [four_3_four_2_ssse3]
	PADDW XMMR1, XMMR5

	PADDW XMMR1, [eight_4_ssse3]
	PSRLW XMMR1, 3

	PUNPCKLBW XMMR2, XMMR3
	;((3 - x) * left[y], (y = 2, y = 3)
	PMADDUBSW XMMR2, [three_to_0_3_to_0_ssse3]

	MOVQ XMMR5, XMMR4
	PUNPCKLBW XMMR5, XMMR0
	PMADDUBSW XMMR5, [four_1_four_0_ssse3]
	PADDW XMMR2, XMMR5

	PADDW XMMR2, [eight_4_ssse3]
	PSRLW XMMR2, 3

	PACKUSWB XMMR1, XMMR1
	MOVD [dst], XMMR1
	ADD dst, i_dst_stride

	PSRLDQ XMMR1, 4 * SIZE_OF_PIXEL
	MOVD [dst], XMMR1
	ADD dst, i_dst_stride

	PACKUSWB XMMR2, XMMR2
	MOVD [dst], XMMR2
	ADD dst, i_dst_stride

	PSRLDQ XMMR2, 4 * SIZE_OF_PIXEL
	MOVD [dst], XMMR2
	RET

cglobal intra_pred_mode_0_1_8_ssse3, 0, 7
	INITIALIZE
	LOAD_LEFT_TOP_N 8
	MOVDQA XMMR5, [eight_8_ssse3]
	;top[x]
	MOVQ XMMR0, [left]
	X265_INTRA_PRED_PLANAR_HELP_1_8_SSE2 num_desc_asc_1_8_ssse3,  [eight_8_ssse3],  4,  8
	RET

x265_intra_pred_planar_ssse3 intra_pred_mode_0_2_8_ssse3, num_desc_asc_2_8_ssse3, [eight_16_ssse3],  5, 16
x265_intra_pred_planar_ssse3 intra_pred_mode_0_3_8_ssse3, num_desc_asc_3_8_ssse3, [eight_32_ssse3],  6, 32
x265_intra_pred_planar_ssse3 intra_pred_mode_0_4_8_ssse3, num_desc_asc_4_8_ssse3, [eight_64_ssse3],  7, 64



%undef left
%undef top
%undef dst
%undef i_dst_stride


%else


%define					left						r6
%define					top							r5
%define					dst							r4
%define					i_dst_stride				r3m


%macro INITIALIZE 0
	MOV left, r0m
	MOV top, r1m
	MOV dst, r2m

%endmacro

%macro CALC_PLANAR	6
	%define			num_sub_x		%1
	%define			x_add_one		%2
	%define			top_x			%3
	%define			add_num			%4
	%define			shift_num		%5
	%define			calc_dst_data	%6

	;((15 - x) * left[y]
	MOVDQA XMMR3, XMMR1
	PMULLW XMMR3, num_sub_x

	;(x + 1) * top[8]
	MOVDQA XMMR7, XMMR4
	PMULLW XMMR7, x_add_one
	PADDW XMMR3, XMMR7

	;(15 - y) * top[x]
	MOVDQA XMMR7, top_x
	PMULLW XMMR7, XMMR5
	PADDW XMMR3, XMMR7

	;(y + 1) * left[8], (y = 2, y = 3)
	MOVDQA XMMR7, XMMR2
	PMULLW XMMR7, XMMR6
	PADDW XMMR3, XMMR7

	PADDW XMMR3, add_num
	PSRLW XMMR3, shift_num

	MOVDQA calc_dst_data, XMMR3

	%undef num_sub_x
	%undef x_add_one
	%undef top_x
	%undef add_num
	%undef shift_num
	%undef calc_dst_data

%endmacro

%macro LOAD_LEFT_TOP_N 1
	%define		N		%1
	MOVDQA XMMR2, [left + SIZE_OF_PIXEL * N]
	MOVDQA XMMR4, [top + SIZE_OF_PIXEL * N]
	PSHUFB XMMR2, [top_left_shufb_byte]
	PSHUFB XMMR4, [top_left_shufb_byte]
	%undef N
%endmacro

%macro X265_INTRA_PRED_PLANAR_SSE2_HELP 4
	MOV r3, 0
%%x265_intra_pred_planar_ssse3_help1:
	MOVDQA XMMR1, XMMR0
	PSHUFB XMMR1, [left_top_pshuffb_0_15+r3]
	PSUBUSW XMMR5, [eight_1_ssse3]
	PADDUSW XMMR6, [eight_1_ssse3]
	MOV r1, 0
%%x265_intra_pred_planar_ssse3_help2:
	CALC_PLANAR [%1+r1], [num_asc_ssse3+r1], [top+r1], %2, %3, [dst+r1]
	ADD r1, 16
	CMP r1, %4*2
	JL %%x265_intra_pred_planar_ssse3_help2

	ADD_ADDRESS dst, i_dst_stride
	ADD r3, 16
	CMP r3, 128
	JL %%x265_intra_pred_planar_ssse3_help1
%endmacro

%macro x265_intra_pred_planar_ssse3 5
cglobal %1, 0, 7
	INITIALIZE
	LOAD_LEFT_TOP_N %5
	MOVDQA XMMR5, %3
	PXOR XMMR6, XMMR6

	MOV r0, 0
%%x265_intra_pred_planar_ssse3_label:
	;top[x]
	MOVDQA XMMR0, [left + SIZE_OF_PIXEL * r0]
	X265_INTRA_PRED_PLANAR_SSE2_HELP %2, %3, %4, %5
	ADD r0, 8
	CMP r0, %5
	JL %%x265_intra_pred_planar_ssse3_label

	RET

%endmacro



SECTION .text align=16

three_to_0_3_to_0_ssse3				DW			 3,  2,  1,  0,  3,  2,  1,  0
one_to_4_1_to_4_ssse3				DW			 1,  2,  3,  4,  1,  2,  3,  4
four_3_four_2_ssse3					DW			 3,  3,  3,  3,  2,  2,  2,  2
four_1_four_0_ssse3					DW			 1,  1,  1,  1,  0,  0,  0,  0
four_1_four_2_ssse3					DW			 1,  1,  1,  1,  2,  2,  2,  2
four_3_four_4_ssse3					DW			 3,  3,  3,  3,  4,  4,  4,  4
eight_4_ssse3						DW			 4,  4,  4,  4,  4,  4,  4,  4

num_desc_ssse3						DW			63, 62, 61, 60, 59, 58, 57, 56
									DW			55, 54, 53, 52, 51, 50, 49, 48
									DW			47, 46, 45, 44, 43, 42, 41, 40
									DW			39, 38, 37, 36, 35, 34, 33, 32
									DW			31, 30, 29, 28, 27, 26, 25, 24
									DW			23, 22, 21, 20, 19, 18, 17, 16
									DW			15, 14, 13, 12, 11, 10,  9,  8
									DW			 7,  6,  5,  4,  3,  2,  1,  0

num_asc_ssse3						DW			 1,  2,  3,  4,  5,  6,  7,  8
									DW			 9, 10, 11, 12, 13, 14, 15, 16
									DW			17, 18, 19, 20, 21, 22, 23, 24
									DW			25, 26, 27, 28, 29, 30, 31, 32
									DW			33, 34, 35, 36, 37, 38, 39, 40
									DW			41, 42, 43, 44, 45, 46, 47, 48
									DW			49, 50, 51, 52, 53, 54, 55, 56
									DW			57, 58, 59, 60, 61, 62, 63, 64

eight_1_ssse3						DW			 1,  1,  1,  1,  1,  1,  1,  1
eight_8_ssse3						DW			 8,  8,  8,  8,  8,  8,  8,  8
eight_16_ssse3						DW			16, 16, 16, 16, 16, 16, 16, 16
eight_32_ssse3						DW			32, 32, 32, 32, 32, 32, 32, 32
eight_64_ssse3						DW			64, 64, 64, 64, 64, 64, 64, 64
top_left_shufb_byte					DB		  	0H, 1H, 0H, 1H, 0H, 1H, 0H, 1H
									DB			0H, 1H, 0H, 1H, 0H, 1H, 0H, 1H

cglobal intra_pred_mode_0_0_8_ssse3, 0, 7
	INITIALIZE
	MOVDQA XMMR6, [left]

	;left[4]
	PSHUFHW XMMR0, XMMR6, 0
	PUNPCKHWD XMMR0, XMMR0

	;left[y], (y = 0, y = 1)
	PSHUFLW XMMR5, XMMR6, 55H
	PSHUFLW XMMR1, XMMR6, 0
	PUNPCKLQDQ XMMR1, XMMR5

		;left[y], (y = 2, y = 3)
	PSHUFLW XMMR5, XMMR6, 0FFH
	PSHUFLW XMMR2, XMMR6, 0AAH
	PUNPCKLQDQ XMMR2, XMMR5

	MOVDQA XMMR6, [top]
	;top[4]
	PSHUFHW XMMR3, XMMR6, 0
	PUNPCKHWD XMMR3, XMMR3
	;(x + 1) * top[4]
	PMULLW XMMR3, [one_to_4_1_to_4_ssse3]

	;top[x]
											;11100100
	PSHUFLW XMMR4, XMMR6, 0E4H
	PUNPCKLQDQ XMMR4, XMMR4

	;((3 - x) * left[y], (y = 0, y = 1)
	PMULLW XMMR1, [three_to_0_3_to_0_ssse3]
	;(x + 1) * top[4]
	PADDW XMMR1, XMMR3

	;(3 - y) * top[x], (y = 0, y = 1)
	MOVDQA XMMR5, XMMR4
	PMULLW XMMR5, [four_3_four_2_ssse3]
	PADDW XMMR1, XMMR5

	;(y + 1) * left[4], (y = 0, y = 1)
	MOVDQA XMMR5, XMMR0
	PMULLW XMMR5, [four_1_four_2_ssse3]
	PADDW XMMR1, XMMR5

	PADDW XMMR1, [eight_4_ssse3]
	PSRLW XMMR1, 3

	;((3 - x) * left[y], (y = 0, y = 1)
	PMULLW XMMR2, [three_to_0_3_to_0_ssse3]
	;(x + 1) * top[4]
	PADDW XMMR2, XMMR3

	;(3 - y) * top[x], (y = 2, y = 3)
	MOVDQA XMMR5, XMMR4
	PMULLW XMMR5, [four_1_four_0_ssse3]
	PADDW XMMR2, XMMR5

	;(y + 1) * left[4], (y = 2, y = 3)
	MOVDQA XMMR5, XMMR0
	PMULLW XMMR5, [four_3_four_4_ssse3]
	PADDW XMMR2, XMMR5

	PADDW XMMR2, [eight_4_ssse3]
	PSRLW XMMR2, 3

	MOVQ [dst], XMMR1
	ADD_ADDRESS dst, i_dst_stride
	PSRLDQ XMMR1, 4 * SIZE_OF_PIXEL

	MOVQ [dst], XMMR1
	ADD_ADDRESS dst, i_dst_stride

	MOVQ [dst], XMMR2
	ADD_ADDRESS dst, i_dst_stride
	PSRLDQ XMMR2, 4 * SIZE_OF_PIXEL
	MOVQ [dst], XMMR2
	RET


x265_intra_pred_planar_ssse3 intra_pred_mode_0_1_8_ssse3, num_desc_ssse3+2*56, [eight_8_ssse3],  4, 8
x265_intra_pred_planar_ssse3 intra_pred_mode_0_2_8_ssse3, num_desc_ssse3+2*48, [eight_16_ssse3], 5, 16
x265_intra_pred_planar_ssse3 intra_pred_mode_0_3_8_ssse3, num_desc_ssse3+2*32, [eight_32_ssse3], 6, 32
x265_intra_pred_planar_ssse3 intra_pred_mode_0_4_8_ssse3, num_desc_ssse3,      [eight_64_ssse3], 7, 64


%undef left
%undef top
%undef dst
%undef i_dst_stride


%endif


