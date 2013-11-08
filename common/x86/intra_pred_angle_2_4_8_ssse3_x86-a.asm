



%include "x86inc.asm"
%include "intra_pred_angle_ssse3_x86-a.asm"
%include "intra_pred_angle_2_4_8_ssse3_x86-a.inc"

extern zero_to_32
extern zero_to_32_1
extern eight_16
extern angle_ap
extern left_top_pshuffb_0_15
extern pshuffb_reverse
extern pshuffw_zero


SECTION .text align=16

DEFINE_INTRA_PRED_FUNC 2_8, 16
DEFINE_INTRA_PRED_FUNC 3_8, 32
DEFINE_INTRA_PRED_FUNC 4_8, 64

