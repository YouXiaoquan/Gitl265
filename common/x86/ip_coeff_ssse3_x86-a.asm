



%include "x86inc.asm"

global pshuffq_zero
global pshuffd_zero
global pshuffd_w
global CONVERT_W_TO_DW_CONST_SSSE3

global LUMA_FILTER_P_1_SSSE3_COEFF
global LUMA_FILTER_P_2_SSSE3_COEFF
global LUMA_FILTER_P_3_SSSE3_COEFF

global LUMA_FILTER_S_1_SSSE3_COEFF
global LUMA_FILTER_S_2_SSSE3_COEFF
global LUMA_FILTER_S_3_SSSE3_COEFF

global CHROMA_FILTER_P_1_SSSE3_COEFF
global CHROMA_FILTER_P_2_SSSE3_COEFF
global CHROMA_FILTER_P_3_SSSE3_COEFF
global CHROMA_FILTER_P_4_SSSE3_COEFF
global CHROMA_FILTER_P_5_SSSE3_COEFF
global CHROMA_FILTER_P_6_SSSE3_COEFF
global CHROMA_FILTER_P_7_SSSE3_COEFF

global CHROMA_FILTER_S_1_SSSE3_COEFF
global CHROMA_FILTER_S_2_SSSE3_COEFF
global CHROMA_FILTER_S_3_SSSE3_COEFF
global CHROMA_FILTER_S_4_SSSE3_COEFF
global CHROMA_FILTER_S_5_SSSE3_COEFF
global CHROMA_FILTER_S_6_SSSE3_COEFF
global CHROMA_FILTER_S_7_SSSE3_COEFF

SECTION .rodata align=16

pshuffd_zero								DB		 0, 1, 2, 3, 0, 1, 2, 3
											DB		 0, 1, 2, 3, 0, 1, 2, 3
pshuffq_zero								DB		 0, 1, 2, 3, 80H, 80H, 80H, 80H
											DB		 80H, 80H, 80H, 80H, 80H, 80H, 80H, 80H

pshuffd_w									DB		 0, 1, 4, 5, 8, 9, 12, 13
											DB		 0, 1, 4, 5, 8, 9, 12, 13


%if 1 == SIZE_OF_PIXEL
LUMA_FILTER_P_1_SSSE3_COEFF					DB		 -1,   4,  -1,   4,  -1,   4,  -1,   4
											DB		 -1,   4,  -1,   4,  -1,   4,  -1,   4
											DB		-10,  58, -10,  58, -10,  58, -10,  58
											DB		-10,  58, -10,  58, -10,  58, -10,  58
											DB		 17,  -5,  17,  -5,  17,  -5,  17,  -5
											DB		 17,  -5,  17,  -5,  17,  -5,  17,  -5
											DB		  1,   0,   1,   0,   1,   0,   1,   0
											DB		  1,   0,   1,   0,   1,   0,   1,   0


LUMA_FILTER_P_2_SSSE3_COEFF					DB		 -1,   4,  -1,   4,  -1,   4,  -1,   4
											DB		 -1,   4,  -1,   4,  -1,   4,  -1,   4
											DB		-11,  40, -11,  40, -11,  40, -11,  40
											DB		-11,  40, -11,  40, -11,  40, -11,  40
											DB		 40, -11,  40, -11,  40, -11,  40, -11
											DB		 40, -11,  40, -11,  40, -11,  40, -11
											DB		  4,  -1,   4,  -1,   4,  -1,   4,  -1
											DB		  4,  -1,   4,  -1,   4,  -1,   4,  -1

LUMA_FILTER_P_3_SSSE3_COEFF					DB		  0,   1,   0,   1,   0,   1,   0,   1
											DB		  0,   1,   0,   1,   0,   1,   0,   1
											DB		 -5,  17,  -5,  17,  -5,  17,  -5,  17
											DB		 -5,  17,  -5,  17,  -5,  17,  -5,  17
											DB		 58, -10,  58, -10,  58, -10,  58, -10
											DB		 58, -10,  58, -10,  58, -10,  58, -10
											DB		  4,  -1,   4,  -1,   4,  -1,   4,  -1
											DB		  4,  -1,   4,  -1,   4,  -1,   4,  -1
%else
LUMA_FILTER_P_1_SSSE3_COEFF					DW		 -1,   4,  -1,   4,  -1,   4,  -1,   4
											DW		-10,  58, -10,  58, -10,  58, -10,  58
											DW		 17,  -5,  17,  -5,  17,  -5,  17,  -5
											DW		  1,   0,   1,   0,   1,   0,   1,   0

LUMA_FILTER_P_2_SSSE3_COEFF					DW		 -1,   4,  -1,   4,  -1,   4,  -1,   4
											DW		-11,  40, -11,  40, -11,  40, -11,  40
											DW		 40, -11,  40, -11,  40, -11,  40, -11
											DW		  4,  -1,   4,  -1,   4,  -1,   4,  -1

LUMA_FILTER_P_3_SSSE3_COEFF					DW		  0,   1,   0,   1,   0,   1,   0,   1
											DW		 -5,  17,  -5,  17,  -5,  17,  -5,  17
											DW		 58, -10,  58, -10,  58, -10,  58, -10
											DW		  4,  -1,   4,  -1,   4,  -1,   4,  -1
%endif


LUMA_FILTER_S_1_SSSE3_COEFF					DW		 -1,   4,  -1,   4,  -1,   4,  -1,   4
											DW		-10,  58, -10,  58, -10,  58, -10,  58
											DW		 17,  -5,  17,  -5,  17,  -5,  17,  -5
											DW		  1,   0,   1,   0,   1,   0,   1,   0

LUMA_FILTER_S_2_SSSE3_COEFF					DW		 -1,   4,  -1,   4,  -1,   4,  -1,   4
											DW		-11,  40, -11,  40, -11,  40, -11,  40
											DW		 40, -11,  40, -11,  40, -11,  40, -11
											DW		  4,  -1,   4,  -1,   4,  -1,   4,  -1

LUMA_FILTER_S_3_SSSE3_COEFF					DW		  0,   1,   0,   1,   0,   1,   0,   1
											DW		 -5,  17,  -5,  17,  -5,  17,  -5,  17
											DW		 58, -10,  58, -10,  58, -10,  58, -10
											DW		  4,  -1,   4,  -1,   4,  -1,   4,  -1

%if 1 == SIZE_OF_PIXEL
CHROMA_FILTER_P_1_SSSE3_COEFF				DB		 -2,  58,  -2,  58,  -2,  58,  -2,  58
											DB		 -2,  58,  -2,  58,  -2,  58,  -2,  58
											DB		 10,  -2,  10,  -2,  10,  -2,  10,  -2
											DB		 10,  -2,  10,  -2,  10,  -2,  10,  -2

CHROMA_FILTER_P_2_SSSE3_COEFF				DB		 -4,  54,  -4,  54,  -4,  54,  -4,  54
											DB		 -4,  54,  -4,  54,  -4,  54,  -4,  54
											DB		 16,  -2,  16,  -2,  16,  -2,  16,  -2
											DB		 16,  -2,  16,  -2,  16,  -2,  16,  -2

CHROMA_FILTER_P_3_SSSE3_COEFF				DB		 -6,  46,  -6,  46,  -6,  46,  -6,  46
											DB		 -6,  46,  -6,  46,  -6,  46,  -6,  46
											DB		 28,  -4,  28,  -4,  28,  -4,  28,  -4
											DB		 28,  -4,  28,  -4,  28,  -4,  28,  -4

CHROMA_FILTER_P_4_SSSE3_COEFF				DB		 -4,  36,  -4,  36,  -4,  36,  -4,  36
											DB		 -4,  36,  -4,  36,  -4,  36,  -4,  36
											DB		 36,  -4,  36,  -4,  36,  -4,  36,  -4
											DB		 36,  -4,  36,  -4,  36,  -4,  36,  -4

CHROMA_FILTER_P_5_SSSE3_COEFF				DB		 -4,  28,  -4,  28,  -4,  28,  -4,  28
											DB		 -4,  28,  -4,  28,  -4,  28,  -4,  28
											DB		 46,  -6,  46,  -6,  46,  -6,  46,  -6
											DB		 46,  -6,  46,  -6,  46,  -6,  46,  -6

CHROMA_FILTER_P_6_SSSE3_COEFF				DB		 -2,  16,  -2,  16,  -2,  16,  -2,  16
											DB		 -2,  16,  -2,  16,  -2,  16,  -2,  16
											DB		 54,  -4,  54,  -4,  54,  -4,  54,  -4
											DB		 54,  -4,  54,  -4,  54,  -4,  54,  -4

CHROMA_FILTER_P_7_SSSE3_COEFF				DB		 -2,  10,  -2,  10,  -2,  10,  -2,  10
											DB		 -2,  10,  -2,  10,  -2,  10,  -2,  10
											DB		 58,  -2,  58,  -2,  58,  -2,  58,  -2
											DB		 58,  -2,  58,  -2,  58,  -2,  58,  -2
%else
CHROMA_FILTER_P_1_SSSE3_COEFF				DW		 -2,  58,  -2,  58,  -2,  58,  -2,  58
											DW		 10,  -2,  10,  -2,  10,  -2,  10,  -2

CHROMA_FILTER_P_2_SSSE3_COEFF				DW		 -4,  54,  -4,  54,  -4,  54,  -4,  54
											DW		 16,  -2,  16,  -2,  16,  -2,  16,  -2

CHROMA_FILTER_P_3_SSSE3_COEFF				DW		 -6,  46,  -6,  46,  -6,  46,  -6,  46
											DW		 28,  -4,  28,  -4,  28,  -4,  28,  -4

CHROMA_FILTER_P_4_SSSE3_COEFF				DW		 -4,  36,  -4,  36,  -4,  36,  -4,  36
											DW		 36,  -4,  36,  -4,  36,  -4,  36,  -4

CHROMA_FILTER_P_5_SSSE3_COEFF				DW		 -4,  28,  -4,  28,  -4,  28,  -4,  28
											DW		 46,  -6,  46,  -6,  46,  -6,  46,  -6

CHROMA_FILTER_P_6_SSSE3_COEFF				DW		 -2,  16,  -2,  16,  -2,  16,  -2,  16
											DW		 54,  -4,  54,  -4,  54,  -4,  54,  -4

CHROMA_FILTER_P_7_SSSE3_COEFF				DW		 -2,  10,  -2,  10,  -2,  10,  -2,  10
											DW		 58,  -2,  58,  -2,  58,  -2,  58,  -2
%endif

CHROMA_FILTER_S_1_SSSE3_COEFF				DW		 -2,  58,  -2,  58,  -2,  58,  -2,  58
											DW		 10,  -2,  10,  -2,  10,  -2,  10,  -2

CHROMA_FILTER_S_2_SSSE3_COEFF				DW		 -4,  54,  -4,  54,  -4,  54,  -4,  54
											DW		 16,  -2,  16,  -2,  16,  -2,  16,  -2

CHROMA_FILTER_S_3_SSSE3_COEFF				DW		 -6,  46,  -6,  46,  -6,  46,  -6,  46
											DW		 28,  -4,  28,  -4,  28,  -4,  28,  -4

CHROMA_FILTER_S_4_SSSE3_COEFF				DW		 -4,  36,  -4,  36,  -4,  36,  -4,  36
											DW		 36,  -4,  36,  -4,  36,  -4,  36,  -4

CHROMA_FILTER_S_5_SSSE3_COEFF				DW		 -4,  28,  -4,  28,  -4,  28,  -4,  28
											DW		 46,  -6,  46,  -6,  46,  -6,  46,  -6

CHROMA_FILTER_S_6_SSSE3_COEFF				DW		 -2,  16,  -2,  16,  -2,  16,  -2,  16
											DW		 54,  -4,  54,  -4,  54,  -4,  54,  -4

CHROMA_FILTER_S_7_SSSE3_COEFF				DW		 -2,  10,  -2,  10,  -2,  10,  -2,  10
											DW		 58,  -2,  58,  -2,  58,  -2,  58,  -2

CONVERT_W_TO_DW_CONST_SSSE3					DD		  1, 1, 1, 1

