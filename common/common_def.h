


#ifndef X265_COMMON_DEF_H
#define X265_X265_COMMON_DEF_H

#define X265_SUMMARY_OUT_               0           ///< print-out PSNR results of all slices to summary.txt
#define X265_SUMMARY_PIC_               0           ///< print-out PSNR results for each slice type to summary.txt


#define X265_MAX_GOP                     64          ///< max. value of hierarchical GOP size

#define X265_MAX_NUM_REF_PICS            16          ///< max. number of pictures used for reference
#define X265_MAX_NUM_REF                 16          ///< max. number of entries in picture reference list
#define X265_MAX_NUM_REF_LC              X265_MAX_NUM_REF_PICS  // TODO: remove this macro definition (leftover from combined list concept)

#define X265_MAX_UINT                    0xFFFFFFFFU ///< max. value of unsigned 32-bit integer
#define X265_MAX_INT                     2147483647  ///< max. value of signed 32-bit integer
#define X265_MAX_INT64                   0x7FFFFFFFFFFFFFFFLL  ///< max. value of signed 64-bit integer
#define X265_MAX_DOUBLE                  1.7e+308    ///< max. value of double-type value

#define X265_MIN_QP                      0
#define X265_MAX_QP                      51

#define X265_NOT_VALID                   -1


// ====================================================================================================================
// Coding tool configuration
// ====================================================================================================================

// AMVP: advanced motion vector prediction
#define X265_AMVP_MAX_NUM_CANDS          2           ///< max number of final candidates
#define X265_AMVP_MAX_NUM_CANDS_MEM      3           ///< max number of candidates
// MERGE
#define X265_MRG_MAX_NUM_CANDS           5

// Reference memory management
#define X265_DYN_REF_FREE                0           ///< dynamic free of reference memories

// Explicit temporal layer QP offset
#define X265_MAX_TLAYER                  8           ///< max number of temporal layer
#define X265_HB_LAMBDA_FOR_LDC           1           ///< use of B-style lambda for non-key pictures in low-delay mode

// Fast estimation of generalized B in low-delay mode
#define X265_GPB_SIMPLE                  1           ///< Simple GPB mode
#if     X265_GPB_SIMPLE
#define X265_GPB_SIMPLE_UNI              1           ///< Simple mode for uni-direction
#endif

// Fast ME using smoother MV assumption
#define X265_FASTME_SMOOTHER_MV          1           ///< reduce ME time using faster option

// Adaptive search range depending on POC difference
#define X265_ADAPT_SR_SCALE              1           ///< division factor for adaptive search range

#define X265_CLIP_TO_709_RANGE           0

// Early-skip threshold (encoder)
#define X265_EARLY_SKIP_THRES            1.50        ///< if RD < thres*avg[BestSkipRD]


#define X265_MAX_CHROMA_FORMAT_IDC      3


#define X265_MAX_TS_WIDTH 4
#define X265_MAX_TS_HEIGHT 4


#endif


