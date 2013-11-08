

%define		X265_DST_INT_STRIDE					320

%define		P_FLAGS								 1
%define		S_FLAGS								 2

%define		X265_IF_INTERNAL_PREC_ASM			14
%define		X265_IF_FILTER_PREC_ASM				 6
%define		X265_IF_INTERNAL_OFFS_ASM			(1<<(X265_IF_INTERNAL_PREC_ASM-1))
%define		X265_LUMA_FLAGS						1
%define		X265_CHROMA_FLAGS					2

%macro READ_N_BYTES_DATA 3
%if 16 == %3
	MOVDQU %1, %2
%elif 8 == %3
	MOVQ %1, %2
%elif 6 == %3
	MOVQ %1, %2
%elif 4 == %3
	MOVD %1, %2
%elif 2 == %3
	MOVD %1, %2
%elif 1 == %3
	MOVD %1, %2
%endif
%endmacro

%macro STORE_N_BYTES_DATA 3
%if 16 == %3
	MOVDQU [%1], %2
%elif 12 == %3
	MOVQ [%1], %2
	PSRLDQ %2, 8
	MOVD [%1+8], %2	
%elif 10 == %3
	MOVQ [%1], %2
	PSRLDQ %2, 8
	MOVD r0, %2
	MOV [%1+8], ax	
%elif 8 == %3
	MOVQ [%1], %2
%elif 6 == %3
	MOVD [%1], %2
	PSRLDQ %2, 4
	MOVD r0, %2
	MOV [%1+4], ax
%elif 4 == %3
	MOVD [%1], %2
%elif 2 == %3
	MOVD r0, %2
	MOV [%1], ax
%elif 1 == %3
	MOVD r0, %2
	MOV [%1], al
%endif
%endmacro

%macro STORE_N_BYTES_DATA_ALIGN 3
%if 16 == %3
	MOVDQA [%1], %2
%elif 12 == %3

%elif 8 == %3
	MOVQ [%1], %2
%elif 4 == %3
	MOVD [%1], %2
%endif
%endmacro

%macro CONVERT_W_TO_DW_SSSE3		3
	MOVDQA %2, %1
	PUNPCK %+ %3 %+ WD %2, %2
	PMADDWD %2, [CONVERT_W_TO_DW_CONST_SSSE3]
%endmacro

%macro DECLARE_EXTERN_LUMA_COEF_SSSE3	1
	extern LUMA_FILTER_ %+ %1 %+ _1_SSSE3_COEFF
	extern LUMA_FILTER_ %+ %1 %+ _2_SSSE3_COEFF
	extern LUMA_FILTER_ %+ %1 %+ _3_SSSE3_COEFF
%endmacro

%macro DECLARE_EXTERN_CHROMA_COEF_SSSE3	1
	extern CHROMA_FILTER_ %+ %1 %+ _1_SSSE3_COEFF
	extern CHROMA_FILTER_ %+ %1 %+ _2_SSSE3_COEFF
	extern CHROMA_FILTER_ %+ %1 %+ _3_SSSE3_COEFF
	extern CHROMA_FILTER_ %+ %1 %+ _4_SSSE3_COEFF
	extern CHROMA_FILTER_ %+ %1 %+ _5_SSSE3_COEFF
	extern CHROMA_FILTER_ %+ %1 %+ _6_SSSE3_COEFF
	extern CHROMA_FILTER_ %+ %1 %+ _7_SSSE3_COEFF
%endmacro

%macro DEFINE_ALL_IP_FILTER_LUMA_SSSE3_FUNC 6

X265_IP_FILTER_ %+ %2 %+ _ %+ %4 %+ _SSSE3_HELP ip_filter_ %+ %1 %+ _ %+ luma %+ _ %+ %3 %+ _ %+ %5 %+ _1_ssse3, LUMA_FILTER_ %+ %4 %+ _1_SSSE3_COEFF, X265_LUMA_FLAGS, %6
X265_IP_FILTER_ %+ %2 %+ _ %+ %4 %+ _SSSE3_HELP ip_filter_ %+ %1 %+ _ %+ luma %+ _ %+ %3 %+ _ %+ %5 %+ _2_ssse3, LUMA_FILTER_ %+ %4 %+ _2_SSSE3_COEFF, X265_LUMA_FLAGS, %6
X265_IP_FILTER_ %+ %2 %+ _ %+ %4 %+ _SSSE3_HELP ip_filter_ %+ %1 %+ _ %+ luma %+ _ %+ %3 %+ _ %+ %5 %+ _3_ssse3, LUMA_FILTER_ %+ %4 %+ _3_SSSE3_COEFF, X265_LUMA_FLAGS, %6

%endmacro


%macro DEFINE_ALL_IP_FILTER_CHROMA_SSSE3_FUNC 6

X265_IP_FILTER_ %+ %2 %+ _ %+ %4 %+ _SSSE3_HELP ip_filter_ %+ %1 %+ _ %+ chroma %+ _ %+ %3 %+ _ %+ %5 %+ _1_ssse3, CHROMA_FILTER_ %+ %4 %+ _1_SSSE3_COEFF, X265_CHROMA_FLAGS, %6
X265_IP_FILTER_ %+ %2 %+ _ %+ %4 %+ _SSSE3_HELP ip_filter_ %+ %1 %+ _ %+ chroma %+ _ %+ %3 %+ _ %+ %5 %+ _2_ssse3, CHROMA_FILTER_ %+ %4 %+ _2_SSSE3_COEFF, X265_CHROMA_FLAGS, %6
X265_IP_FILTER_ %+ %2 %+ _ %+ %4 %+ _SSSE3_HELP ip_filter_ %+ %1 %+ _ %+ chroma %+ _ %+ %3 %+ _ %+ %5 %+ _3_ssse3, CHROMA_FILTER_ %+ %4 %+ _3_SSSE3_COEFF, X265_CHROMA_FLAGS, %6
X265_IP_FILTER_ %+ %2 %+ _ %+ %4 %+ _SSSE3_HELP ip_filter_ %+ %1 %+ _ %+ chroma %+ _ %+ %3 %+ _ %+ %5 %+ _4_ssse3, CHROMA_FILTER_ %+ %4 %+ _4_SSSE3_COEFF, X265_CHROMA_FLAGS, %6
X265_IP_FILTER_ %+ %2 %+ _ %+ %4 %+ _SSSE3_HELP ip_filter_ %+ %1 %+ _ %+ chroma %+ _ %+ %3 %+ _ %+ %5 %+ _5_ssse3, CHROMA_FILTER_ %+ %4 %+ _5_SSSE3_COEFF, X265_CHROMA_FLAGS, %6
X265_IP_FILTER_ %+ %2 %+ _ %+ %4 %+ _SSSE3_HELP ip_filter_ %+ %1 %+ _ %+ chroma %+ _ %+ %3 %+ _ %+ %5 %+ _6_ssse3, CHROMA_FILTER_ %+ %4 %+ _6_SSSE3_COEFF, X265_CHROMA_FLAGS, %6
X265_IP_FILTER_ %+ %2 %+ _ %+ %4 %+ _SSSE3_HELP ip_filter_ %+ %1 %+ _ %+ chroma %+ _ %+ %3 %+ _ %+ %5 %+ _7_ssse3, CHROMA_FILTER_ %+ %4 %+ _7_SSSE3_COEFF, X265_CHROMA_FLAGS, %6

%endmacro



