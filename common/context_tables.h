

#ifndef X265_CONTEXT_TABLES_H
#define X265_CONTEXT_TABLES_H

#define X265_SAO_MERGE_START_IDX								0
#define X265_SAO_TYPE_START_IDX									1
#define X265_CU_SPLIT_FLAG_START_IDX							2
#define X265_CU_TRANSQUANT_BYPASS_FLAG_START_IDX				5
#define X265_CU_SKIP_FLAG_START_IDX								6
#define X265_CU_PRED_MODE_START_IDX								9
#define X265_CU_PART_SIZE_START_IDX								10
#define X265_CU_INTRA_PRED_START_IDX							14
#define X265_CU_CHROMA_PRED_START_IDX							15
#define X265_CU_QT_ROOT_CBF_START_IDX							17
#define X265_CU_MERGE_FLAG_EXT_START_IDX						18
#define X265_CU_MERGE_IDX_EXT_START_IDX							19
#define X265_CU_INTER_DIR_START_IDX								20
#define X265_CU_REF_PIC_START_IDX								25
#define X265_CU_MVP_IDX_START_IDX								27
#define X265_CU_TRANS_SUBDIV_FLAG_START_IDX						29
#define X265_CU_QT_CBF_START_IDX								32
#define X265_CU_MVD_START_IDX									42
#define X265_CU_DELTA_QP_START_IDX								44
#define X265_TRANSFORM_SKIP_START_IDX							46
#define X265_CU_CTX_LAST_X_START_IDX							48
#define X265_CU_CTX_LAST_Y_START_IDX							78
#define X265_CU_SIG_COEFF_GROP_START_IDX						108
#define X265_CU_SIG_START_IDX									112
#define X265_CU_ONE_START_IDX									154
#define X265_CU_ABS_START_IDX									178




///< number of context models for QT CBF
#define X265_NUM_QT_CBF_CTX 5
///< number of context models for MULTI_LEVEL_SIGNIFICANCE
#define X265_NUM_SIG_CG_FLAG_CTX 2
///< number of context models for sig flag
#define X265_NUM_SIG_FLAG_CTX 42
///< number of context models for luma sig flag
#define X265_NUM_SIG_FLAG_CTX_LUMA 27
///< number of context models for chroma sig flag
#define X265_NUM_SIG_FLAG_CTX_CHROMA 15
///< number of context models for last coefficient position
#define X265_NUM_CTX_LAST_FLAG_XY 15

///< number of context models for greater than 1 flag
#define X265_NUM_ONE_FLAG_CTX 24
///< number of context models for greater than 1 flag of luma
#define X265_NUM_ONE_FLAG_CTX_LUMA 16
///< number of context models for greater than 1 flag of chroma
#define X265_NUM_ONE_FLAG_CTX_CHROMA 8
///< number of context models for greater than 2 flag
#define X265_NUM_ABS_FLAG_CTX 6
///< number of context models for greater than 2 flag of luma
#define X265_NUM_ABS_FLAG_CTX_LUMA 4
///< number of context models for greater than 2 flag of chroma
#define X265_NUM_ABS_FLAG_CTX_CHROMA 2






#endif


