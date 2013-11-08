
%macro INITIALIZE 1
	mov r6, esp
	and r6, 0xFFFFFFF0
	mov ref_main, %1
%endmacro


%macro DEFINE_COMMON_VARIABLE 0
	%define			I									r5
	%define			J									r4
	%define			v_delta_pos							r3
	%define			dst									r2
	%define			ref_main							r1
	%define			R1									[r6+  0]
	%define			R2									[r6+ 16]
	%define			R3									[r6+ 32]
	%define			R4									[r6+ 48]
	%define			R5									[r6+ 64]
	%define			R6									[r6+ 80]
	%define			R7									[r6+ 96]
	%define			R8									[r6+ 112]
	%define			R9									[r6+ 128]
	%define			R10									[r6+ 144]
	%define			R11									[r6+ 160]
	%define			R12									[r6+ 176]
	%define			R13									[r6+ 192]
	%define			R14									[r6+ 208]
	%define			R15									[r6+ 224]
	%define			R16									[r6+ 240]
	%define			dst_parameter						r2m
	%define			i_dst_stride_parameter				r3m
%endmacro

%macro UNDEFINE_COMMON_VARIABLE 0
	%undef			I
	%undef			J
	%undef			v_delta_pos
	%undef			dst
	%undef			ref_main
	%undef			R1
	%undef			R2
	%undef			R3
	%undef			R4
	%undef			R5
	%undef			R6
	%undef			R7
	%undef			R8
	%undef			R9
	%undef			R10
	%undef			R11
	%undef			R12
	%undef			R13
	%undef			R14
	%undef			R15
	%undef			R16
	%undef			dst_parameter
	%undef			i_dst_stride_parameter
%endmacro

%macro X265_INTRA_PRED_ANGLE_32_MODE_HELP_SSSE3					3
	%define	I													r6
	%define	J													r5
	%define	K													r4
	%define	temp_dst											r3
	%define	dst													r2
	%define	ref_main											r1
	%define	i_dst_stride_parameter								r3m
cglobal %1, 0, 7
	;	i	j	k	dst		temp_dst	refMain
	MOV ref_main, %3
	ADD ref_main, SIZE_OF_PIXEL
	MOV dst, r2m
	X265_INTRA_PRED_ANGLE_32_HELP_PIXEL_SSSE3 %1, (%2*SIZE_OF_PIXEL/16), (16/SIZE_OF_PIXEL)
	RET
	%undef I
	%undef J
	%undef K
	%undef dst
	%undef temp_dst
	%undef ref_main
	%undef i_dst_stride
%endmacro

%macro X265_INTRA_PRED_ANGLE_H_MODE_HELP_SSSE3 5
	DEFINE_COMMON_VARIABLE
cglobal %1, 0, 7
	INITIALIZE r0m
	sub r6, (256/SIZE_OF_PIXEL)+16
	X265_INTRA_PRED_ANGLE_H_HELP_PIXEL_SSSE3 %1, %2, %4, %5
	RET
	UNDEFINE_COMMON_VARIABLE
%endmacro

%macro X265_INTRA_PRED_ANGLE_H_MODE_2_HELP_SSSE3 5
cglobal %1, 0, 7
	DEFINE_COMMON_VARIABLE
	%define			ROW1								[r6+ (256/SIZE_OF_PIXEL)   ]
	%define			ROW2								[r6+ (256/SIZE_OF_PIXEL)+16]
	INITIALIZE r0m
	sub r6, (256/SIZE_OF_PIXEL)+32+16
	X265_INTRA_PRED_ANGLE_H_2_HELP_PIXEL_SSSE3 %1, %5, %2
	RET
	UNDEFINE_COMMON_VARIABLE
	%undef			ROW1
	%undef			ROW2
%endmacro

%macro X265_INTRA_PRED_ANGLE_H_MODE_0_HELP_SSSE3 5
cglobal %1, 0, 7
	%define			I						r6
	%define			J						r5
	%define			K						r4
	%define			dst						r3
	%define			ref_side				r2
	%define			ref_main				r1
	%define			ref_main_parameter 		r0m
	%define			ref_side_parameter 		r1m
	%define			dst_parameter	 		r2m
	%define			i_dst_stride_parameter 	r3m
	X265_INTRA_PRED_ANGLE_H_0_HELP_PIXEL_SSSE3 %1, %5, r0m
	RET
	%undef		I
	%undef		J
	%undef		K
	%undef		ref_main
	%undef		temp_dst
	%undef		dst
	%undef		i_dst_stride
%endmacro

%macro X265_INTRA_PRED_ANGLE_H_MODE_M2_HELP_SSSE3 5
cglobal %1, 0, 7
	DEFINE_COMMON_VARIABLE
	%define			ROW1								[r6+ (256/SIZE_OF_PIXEL)   ]
	%define			ROW2								[r6+ (256/SIZE_OF_PIXEL)+16]
	%define			ref_main_parameter					r0m
	%define			ref_side_parameter					r1m
	MOV r6, esp
	AND r6, 0xFFFFFFF0
	INIT_REF_MAIN_M_MEMORY_CONTENT %5, %2, %3
	sub r6, (256/SIZE_OF_PIXEL)+32+16
	X265_INTRA_PRED_ANGLE_H_M2_HELP_PIXEL_SSSE3 %1, %5, %2
	RET
	UNDEFINE_COMMON_VARIABLE
	%undef			ROW1
	%undef			ROW2
	%undef			ref_main_parameter
	%undef			ref_side_parameter
%endmacro

%macro X265_INTRA_PRED_ANGLE_H_MODE_M_HELP_SSSE3 5
cglobal %1, 0, 7
	DEFINE_COMMON_VARIABLE
	%define			ref_main_parameter					r0m
	%define			ref_side_parameter					r1m
	MOV r6, esp
	AND r6, 0xFFFFFFF0
	INIT_REF_MAIN_M_MEMORY_CONTENT %5, %2, %3
	sub r6, (256/SIZE_OF_PIXEL)+16
	X265_INTRA_PRED_ANGLE_H_HELP_PIXEL_SSSE3 %1, %2, %4, %5
	RET
	UNDEFINE_COMMON_VARIABLE
	%undef			ref_main_parameter
	%undef			ref_side_parameter
%endmacro


%macro X265_INTRA_PRED_ANGLE_M32_MODE_HELP_SSSE3 2
	%define	I													r6
	%define	J													r5
	%define	K													r4
	%define	temp_dst											r3
	%define	dst													r2
	%define	ref_main											r1
	%define	i_dst_stride_parameter								r3m
cglobal %1, 0, 7
	MOV r6, esp
	AND r6, 0xFFFFFFF0
	SUB r6, (16+%2*2)*SIZE_OF_PIXEL

	MOV r3, r6
%if %2 > 8
	ADD r3, %2*SIZE_OF_PIXEL
%else
	ADD r3, 16 * SIZE_OF_PIXEL
%endif
	MOV ESI, r1m
	SUB ESI, SIZE_OF_PIXEL
	MOV EDI, r3
	MOV ECX, ((%2+1)*SIZE_OF_PIXEL)
	REP MOVSB

	COPY_INVERSE_MEMORY r3, r0m, %2
	X265_INTRA_PRED_ANGLE_M32_HELP_PIXEL_SSSE3 %1, (%2*SIZE_OF_PIXEL/16), (16/SIZE_OF_PIXEL), r3, r2m

	RET
	%undef I
	%undef J
	%undef K
	%undef dst
	%undef temp_dst
	%undef ref_main
	%undef i_dst_stride
%endmacro

%macro X265_INTRA_PRED_ANGLE_V_MODE_HELP_SSSE3 5
cglobal %1, 0, 7
	DEFINE_COMMON_VARIABLE
	INITIALIZE r1m
	sub r6, 16
	X265_INTRA_PRED_ANGLE_V_HELP_PIXEL_SSSE3 %1, %2, %4, %5
	RET
	UNDEFINE_COMMON_VARIABLE
%endmacro


%macro X265_INTRA_PRED_ANGLE_V_MODE_2_HELP_SSSE3 5
cglobal %1, 0, 7
	DEFINE_COMMON_VARIABLE
	%define			ROW1								[r6     ]
	%define			ROW2								[r6+  16]
	INITIALIZE r1m
	sub r6, 32+16
	X265_INTRA_PRED_ANGLE_V_2_HELP_PIXEL_SSSE3 %1, %5, %2
	RET
	UNDEFINE_COMMON_VARIABLE
	%undef			ROW1
	%undef			ROW2
%endmacro

%macro X265_INTRA_PRED_ANGLE_V_MODE_0_HELP_SSSE3 5
cglobal %1, 0, 7
	%define			I						r6
	%define			J						r5
	%define			dst						r4
	%define			ref_side				r3
	%define			ref_main				r2
	%define			ref_main_parameter 		r1m
	%define			ref_side_parameter 		r0m
	%define			dst_parameter	 		r2m
	%define			i_dst_stride_parameter 	r3m
	X265_INTRA_PRED_ANGLE_V_0_HELP_PIXEL_SSSE3 %1, %5, r1m
	RET
	%undef		I
	%undef		J
	%undef		K
	%undef		ref_main
	%undef		temp_dst
	%undef		dst
	%undef		dst_parameter
	%undef		i_dst_stride
%endmacro

%macro X265_INTRA_PRED_ANGLE_V_MODE_M2_HELP_SSSE3 5
cglobal %1, 0, 7
	DEFINE_COMMON_VARIABLE
	%define			ROW1								[r6     ]
	%define			ROW2								[r6+  16]
	%define			ref_main_parameter					r1m
	%define			ref_side_parameter					r0m
	MOV r6, esp
	AND r6, 0xFFFFFFF0
	INIT_REF_MAIN_M_MEMORY_CONTENT %5, %2, %3
	sub r6, 32+16
	X265_INTRA_PRED_ANGLE_V_M2_HELP_PIXEL_SSSE3 %1, %5, %2
	RET
	UNDEFINE_COMMON_VARIABLE
	%undef			ROW1
	%undef			ROW2
	%undef			ref_main_parameter
	%undef			ref_side_parameter
%endmacro

%macro X265_INTRA_PRED_ANGLE_V_MODE_M_HELP_SSSE3		5
cglobal %1, 0, 7
	DEFINE_COMMON_VARIABLE
	%define			ref_main_parameter					r1m
	%define			ref_side_parameter					r0m

	MOV r6, esp
	AND r6, 0xFFFFFFF0
	INIT_REF_MAIN_M_MEMORY_CONTENT %5, %2, %3
	sub r6, 16
	X265_INTRA_PRED_ANGLE_V_HELP_PIXEL_SSSE3 %1, %2, %4, %5
	RET
	UNDEFINE_COMMON_VARIABLE
	%undef			ref_main_parameter
	%undef			ref_side_parameter
%endmacro


%macro DEFINE_INTRA_PRED_FUNC 2


X265_INTRA_PRED_ANGLE_32_MODE_HELP_SSSE3  intra_pred_mode_2_  %+ %1 %+ _ssse3, %2, r0m
X265_INTRA_PRED_ANGLE_H_MODE_HELP_SSSE3 	  intra_pred_mode_3_ %+ %1 %+ _ssse3,   26,  315,   1, %2
X265_INTRA_PRED_ANGLE_H_MODE_HELP_SSSE3 	  intra_pred_mode_4_ %+ %1 %+ _ssse3,   21,  390,   2, %2
X265_INTRA_PRED_ANGLE_H_MODE_HELP_SSSE3 	  intra_pred_mode_5_ %+ %1 %+ _ssse3,   17,  482,   3, %2
X265_INTRA_PRED_ANGLE_H_MODE_HELP_SSSE3 	  intra_pred_mode_6_ %+ %1 %+ _ssse3,   13,  630,   4, %2
X265_INTRA_PRED_ANGLE_H_MODE_HELP_SSSE3 	  intra_pred_mode_7_ %+ %1 %+ _ssse3,    9,  910,   5, %2

%if 4 == %2
X265_INTRA_PRED_ANGLE_H_MODE_2_HELP_SSSE3	  intra_pred_mode_8_ %+ %1 %+ _ssse3,    5, 1638,   6, %2
%else
X265_INTRA_PRED_ANGLE_H_MODE_HELP_SSSE3 	  intra_pred_mode_8_ %+ %1 %+ _ssse3,    5, 1638,   6, %2
%endif

X265_INTRA_PRED_ANGLE_H_MODE_2_HELP_SSSE3	  intra_pred_mode_9_ %+ %1 %+ _ssse3,    2, 4096,   7, %2
X265_INTRA_PRED_ANGLE_H_MODE_0_HELP_SSSE3	  intra_pred_mode_10_ %+ %1 %+ _ssse3,   0,    0,   8, %2
X265_INTRA_PRED_ANGLE_H_MODE_M2_HELP_SSSE3	  intra_pred_mode_11_ %+ %1 %+ _ssse3,  -2, 4096,   9, %2

%if 4 == %2
X265_INTRA_PRED_ANGLE_H_MODE_M2_HELP_SSSE3	  intra_pred_mode_12_ %+ %1 %+ _ssse3,  -5, 1638,  10, %2
%else
X265_INTRA_PRED_ANGLE_H_MODE_M_HELP_SSSE3	  intra_pred_mode_12_ %+ %1 %+ _ssse3,  -5, 1638,  10, %2
%endif

X265_INTRA_PRED_ANGLE_H_MODE_M_HELP_SSSE3	  intra_pred_mode_13_ %+ %1 %+ _ssse3,  -9,  910,  11, %2
X265_INTRA_PRED_ANGLE_H_MODE_M_HELP_SSSE3	  intra_pred_mode_14_ %+ %1 %+ _ssse3, -13,  630,  12, %2
X265_INTRA_PRED_ANGLE_H_MODE_M_HELP_SSSE3	  intra_pred_mode_15_ %+ %1 %+ _ssse3, -17,  482,  13, %2
X265_INTRA_PRED_ANGLE_H_MODE_M_HELP_SSSE3	  intra_pred_mode_16_ %+ %1 %+ _ssse3, -21,  390,  14, %2
X265_INTRA_PRED_ANGLE_H_MODE_M_HELP_SSSE3	  intra_pred_mode_17_ %+ %1 %+ _ssse3, -26,  315,  15, %2


X265_INTRA_PRED_ANGLE_M32_MODE_HELP_SSSE3 intra_pred_mode_18_ %+ %1 %+ _ssse3, %2

X265_INTRA_PRED_ANGLE_V_MODE_M_HELP_SSSE3 	  intra_pred_mode_19_ %+ %1 %+ _ssse3, -26,  315,  15, %2
X265_INTRA_PRED_ANGLE_V_MODE_M_HELP_SSSE3 	  intra_pred_mode_20_ %+ %1 %+ _ssse3, -21,  390,  14, %2
X265_INTRA_PRED_ANGLE_V_MODE_M_HELP_SSSE3 	  intra_pred_mode_21_ %+ %1 %+ _ssse3, -17,  482,  13, %2
X265_INTRA_PRED_ANGLE_V_MODE_M_HELP_SSSE3 	  intra_pred_mode_22_ %+ %1 %+ _ssse3, -13,  630,  12, %2
X265_INTRA_PRED_ANGLE_V_MODE_M_HELP_SSSE3 	  intra_pred_mode_23_ %+ %1 %+ _ssse3,  -9,  910,  11, %2

%if 4 == %2
X265_INTRA_PRED_ANGLE_V_MODE_M2_HELP_SSSE3	  intra_pred_mode_24_ %+ %1 %+ _ssse3,  -5, 1638,  10, %2
%else
X265_INTRA_PRED_ANGLE_V_MODE_M_HELP_SSSE3 	  intra_pred_mode_24_ %+ %1 %+ _ssse3,  -5, 1638,  10, %2
%endif

X265_INTRA_PRED_ANGLE_V_MODE_M2_HELP_SSSE3	  intra_pred_mode_25_ %+ %1 %+ _ssse3,  -2, 4096,   9, %2
X265_INTRA_PRED_ANGLE_V_MODE_0_HELP_SSSE3	  intra_pred_mode_26_ %+ %1 %+ _ssse3,   0,    0,   8, %2
X265_INTRA_PRED_ANGLE_V_MODE_2_HELP_SSSE3	  intra_pred_mode_27_ %+ %1 %+ _ssse3,   2, 4096,   7, %2

%if 4 == %2
X265_INTRA_PRED_ANGLE_V_MODE_2_HELP_SSSE3	  intra_pred_mode_28_ %+ %1 %+ _ssse3,   5, 1638,   6, %2
%else
X265_INTRA_PRED_ANGLE_V_MODE_HELP_SSSE3 	  intra_pred_mode_28_ %+ %1 %+ _ssse3,   5, 1638,   6, %2
%endif

X265_INTRA_PRED_ANGLE_V_MODE_HELP_SSSE3 	  intra_pred_mode_29_ %+ %1 %+ _ssse3,   9,  910,   5, %2
X265_INTRA_PRED_ANGLE_V_MODE_HELP_SSSE3 	  intra_pred_mode_30_ %+ %1 %+ _ssse3,  13,  630,   4, %2
X265_INTRA_PRED_ANGLE_V_MODE_HELP_SSSE3 	  intra_pred_mode_31_ %+ %1 %+ _ssse3,  17,  482,   3, %2
X265_INTRA_PRED_ANGLE_V_MODE_HELP_SSSE3 	  intra_pred_mode_32_ %+ %1 %+ _ssse3,  21,  390,   2, %2
X265_INTRA_PRED_ANGLE_V_MODE_HELP_SSSE3 	  intra_pred_mode_33_ %+ %1 %+ _ssse3,  26,  315,   1, %2
X265_INTRA_PRED_ANGLE_32_MODE_HELP_SSSE3  intra_pred_mode_34_ %+ %1 %+ _ssse3, %2, r1m



%endmacro



