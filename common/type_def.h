
#ifndef X265_TYPE_DEF_H
#define X265_X265_TYPE_DEF_H

//! \ingroup TLibCommon
//! \{
#define X265_L0363_DU_BIT_RATE             1 ///< L0363: add bit_rate_du_value_minus1 to HRD parameters
#define X265_L0328_SPLICING                1 ///< L0328: splicing support in HRD
#define X265_L0044_DU_DPB_OUTPUT_DELAY_HRD 1 ///< L0044: Include dpb_output_delay_du_length_minus1 in hrd_parameters(), dpb_output_du_delay in
                                        ///<        picture timing SEI and DU information SEI
#define X265_L0045_PERSISTENCE_FLAGS  1      ///< L0045: Replace "repetition_period" syntax elements in SEI with "persistence_flag"
#define X265_L0045_NON_NESTED_SEI_RESTRICTIONS 1 ///< L0045; Include restriction on the order of APS and non-nested BP, PT and DU info SEI messages
#define X265_L0044_CPB_DPB_DELAY_OFFSET 1  ///< L0044: Include syntax elements cpb_delay_offset and dpb_delay_offset in the BP SEI message
#define X265_L0047_APS_FLAGS            1  ///< L0047: Include full_random_access_flag and no_param_set_update_flag in the active parameter set SEI message
#define X265_L0043_TIMING_INFO          1  ///< L0043: Timing information is signalled in VUI outside hrd_parameters()
#define X265_L0046_RENAME_PROG_SRC_IDC  1  ///< L0046: Rename progressive_source_idc to source_scan_type
#define X265_L0045_CONDITION_SIGNALLING 1  ///< L0045: Condition the signaling of some syntax elements in picture timing SEI message
#define X265_L0043_MSS_IDC 1
#define X265_L0116_ENTRY_POINT 1
#define X265_L0363_MORE_BITS 1
#define X265_L0363_MVP_POC 1
#define X265_L0363_BYTE_ALIGN 1
#define X265_L0363_SEI_ALLOW_SUFFIX 1
#define X265_L0323_LIMIT_DEFAULT_LIST_SIZE 1
#define X265_L0046_CONSTRAINT_FLAGS 1
#define X265_L0255_MOVE_PPS_FLAGS       1  ///< move some flags to earlier positions in the PPS
#define X265_L0444_FPA_TYPE             1  ///< allow only FPA types 3, 4 and 5
#define X265_L0372 1
#define X265_SIGNAL_BITRATE_PICRATE_IN_VPS               0  ///< K0125: Signal bit_rate and pic_rate in VPS
#define X265_L0232_RD_PENALTY           1  ///< L0232: RD-penalty for 32x32 TU for intra in non-intra slices

#define X265_MAX_VPS_NUM_HRD_PARAMETERS                1
#define X265_MAX_VPS_OP_SETS_PLUS1                     1024
#define X265_MAX_VPS_NUH_RESERVED_ZERO_LAYER_ID_PLUS1  1

#define X265_RATE_CONTROL_LAMBDA_DOMAIN                  1  ///< JCTVC-K0103, rate control by R-lambda model
#define X265_L0033_RC_BUGFIX                             1  ///< JCTVC-L0033, bug fix for R-lambda model based rate control

#define X265_MAX_CPB_CNT                     32  ///< Upper bound of (cpb_cnt_minus1 + 1)
#define X265_MAX_NUM_LAYER_IDS                64

#define X265_COEF_REMAIN_BIN_REDUCTION        3 ///< indicates the level at which the VLC
                                           ///< transitions from Golomb-Rice to TU+EG(k)

#define X265_CU_DQP_TU_CMAX 5                   ///< max number bins for truncated unary
#define X265_CU_DQP_EG_k 0                      ///< expgolomb order

#define X265_SBH_THRESHOLD                    4  ///< I0156: value of the fixed SBH controlling threshold

#define X265_SEQUENCE_LEVEL_LOSSLESS           0  ///< H0530: used only for sequence or frame-level lossless coding

#define X265_DISABLING_CLIP_FOR_BIPREDME         0  ///< Ticket #175

#define X265_C1FLAG_NUMBER               8 // maximum number of largerThan1 flag coded in one chunk :  16 in HM5
#define X265_C2FLAG_NUMBER               1 // maximum number of largerThan2 flag coded in one chunk:  16 in HM5

#define X265_REMOVE_SAO_LCU_ENC_CONSTRAINTS_3 1  ///< disable the encoder constraint that conditionally disable SAO for chroma for entire slice in interleaved mode

#define X265_REMOVE_SINGLE_SEI_EXTENSION_FLAGS 1 ///< remove display orientation SEI extension flag (there is a generic SEI extension mechanism now)

#define X265_SAO_ENCODING_CHOICE              1  ///< I0184: picture early termination
#if X265_SAO_ENCODING_CHOICE
#define X265_SAO_ENCODING_RATE                0.75
#define X265_SAO_ENCODING_CHOICE_CHROMA       1 ///< J0044: picture early termination Luma and Chroma are handled separately
#if X265_SAO_ENCODING_CHOICE_CHROMA
#define X265_SAO_ENCODING_RATE_CHROMA         0.5
#endif
#endif

#define X265_MAX_NUM_VPS                16
#define X265_MAX_NUM_SPS                16
#define X265_MAX_NUM_PPS                64



#define X265_WEIGHTED_CHROMA_DISTORTION  1   ///< F386: weighting of chroma for RDO
#define X265_RDOQ_CHROMA_LAMBDA          1   ///< F386: weighting of chroma for RDOQ
#define X265_SAO_CHROMA_LAMBDA           1   ///< F386: weighting of chroma for SAO

#define X265_MIN_SCAN_POS_CROSS          4

#define X265_FAST_BIT_EST                1   ///< G763: Table-based bit estimation for CABAC

#define X265_MLS_GRP_NUM                         64     ///< G644 : Max number of coefficient groups, max(16, 64)
#define X265_MLS_CG_SIZE                         4      ///< G644 : Coefficient group size of 4x4

#define X265_ADAPTIVE_QP_SELECTION               1      ///< G382: Adaptive reconstruction levels, non-normative part for adaptive QP selection
#if X265_ADAPTIVE_QP_SELECTION
#define X265_ARL_C_PRECISION                     7      ///< G382: 7-bit arithmetic precision
#define X265_LEVEL_RANGE                         30     ///< G382: max coefficient level in statistics collection
#endif

#define X265_NS_HAD                               0

#define X265_HHI_RQT_INTRA_SPEEDUP             1           ///< tests one best mode with full rqt
#define X265_HHI_RQT_INTRA_SPEEDUP_MOD         0           ///< tests two best modes with full rqt

#if X265_HHI_RQT_INTRA_SPEEDUP_MOD && !X265_HHI_RQT_INTRA_SPEEDUP
#error
#endif

#define X265_VERBOSE_RATE 0 ///< Print additional rate information in encoder

#define X265_AMVP_DECIMATION_FACTOR            4

#define X265_SCAN_SET_SIZE                     16
#define X265_LOG2_SCAN_SET_SIZE                4

#define X265_FAST_UDI_MAX_RDMODE_NUM               35          ///< maximum number of RD comparison in fast-UDI estimation loop

#define X265_ZERO_MVD_EST                          0           ///< Zero Mvd Estimation in normal mode

#define X265_NUM_INTRA_MODE 36
#if !REMOVE_LM_CHROMA
#define X265_LM_CHROMA_IDX  35
#endif

#define X265_WRITE_BACK                      1           ///< Enable/disable the encoder to replace the deltaPOC and Used by current from the config file with the values derived by the refIdc parameter.
#define X265_AUTO_INTER_RPS                  1           ///< Enable/disable the automatic generation of refIdc from the deltaPOC and Used by current from the config file.
#define X265_PRINT_RPS_INFO                  0           ///< Enable/disable the printing of bits used to send the RPS.
                                                    // using one nearest frame as reference frame, and the other frames are high quality (POC%4==0) frames (1+X)
                                                    // this should be done with encoder only decision
                                                    // but because of the absence of reference frame management, the related code was hard coded currently

#define X265_RVM_VCEGAM10_M 4

#define X265_PLANAR_IDX             0
#define X265_VER_IDX                26                    // index for intra VERTICAL   mode
#define X265_HOR_IDX                10                    // index for intra HORIZONTAL mode
#define X265_DC_IDX                 1                     // index for intra DC mode
#define X265_NUM_CHROMA_MODE        5                     // total number of chroma modes
#define X265_DM_CHROMA_IDX          36                    // chroma mode index for derived from luma intra mode


#define X265_FAST_UDI_USE_MPM 1

#define X265_RDO_WITHOUT_DQP_BITS              0           ///< Disable counting dQP bits in RDO-based mode decision

#define X265_FULL_NBIT 0 ///< When enabled, compute costs using full sample bitdepth.  When disabled, compute costs as if it is 8-bit source video.
#if X265_FULL_NBIT
# define X265_DISTORTION_PRECISION_ADJUSTMENT(x) 0
#else
# define X265_DISTORTION_PRECISION_ADJUSTMENT(x) (x)
#endif

#define X265_LOG2_MAX_NUM_COLUMNS_MINUS1        7
#define X265_LOG2_MAX_NUM_ROWS_MINUS1           7
#define X265_LOG2_MAX_COLUMN_WIDTH              13
#define X265_LOG2_MAX_ROW_HEIGHT                13

#define X265_MATRIX_MULT                             0   // Brute force matrix multiplication instead of partial butterfly

#define X265_REG_DCT 65535

#define X265_AMP_SAD                               1           ///< dedicated SAD functions for AMP
#define X265_AMP_ENC_SPEEDUP                       1           ///< encoder only speed-up by AMP mode skipping
#if X265_AMP_ENC_SPEEDUP
#define X265_AMP_MRG                               1           ///< encoder only force merge for AMP partition (no motion search for AMP)
#endif

#define X265_SCALING_LIST_OUTPUT_RESULT    0 //JCTVC-G880/JCTVC-G1016 quantization matrices

#define X265_CABAC_INIT_PRESENT_FLAG     1


#define X265_NUM_DOWN_PART 4

enum profile_name_e
{
	PROFILE_NONE = 0,
	PROFILE_MAIN = 1,
	PROFILE_MAIN10 = 2,
	PROFILE_MAINSTILLPICTURE = 3,
};

enum level_tier_e
{
	LEVEL_TIER_MAIN = 0,
	LEVEL_TIER_HIGH = 1,
};

enum level_name_e
{
	LEVEL_NONE     = 0,
	LEVEL_LEVEL1   = 30,
	LEVEL_LEVEL2   = 60,
	LEVEL_LEVEL2_1 = 63,
	LEVEL_LEVEL3   = 90,
	LEVEL_LEVEL3_1 = 93,
	LEVEL_LEVEL4   = 120,
	LEVEL_LEVEL4_1 = 123,
	LEVEL_LEVEL5   = 150,
	LEVEL_LEVEL5_1 = 153,
	LEVEL_LEVEL5_2 = 156,
	LEVEL_LEVEL6   = 180,
	LEVEL_LEVEL6_1 = 183,
	LEVEL_LEVEL6_2 = 186,
};

enum scaling_list_parameter_t
{
	SCALING_LIST_OFF,
	SCALING_LIST_DEFAULT,
	SCALING_LIST_FILE_READ
};

// ====================================================================================================================
// Enumeration
// ====================================================================================================================

/// supported slice type
enum slice_type_e
{
	B_SLICE,
	P_SLICE,
	I_SLICE
};

enum slice_constraint_e
{
	///< don't use slices / slice segments
	SLICES_NONE = 0,
	///< Limit maximum number of largest coding tree blocks in a slice / slice segments
	FIXED_NUMBER_OF_LCU = 1,
	///< Limit maximum number of bytes in a slice / slice segment
	FIXED_NUMBER_OF_BYTES = 2,
	///< slices / slice segments span an integer number of tiles
	FIXED_NUMBER_OF_TILES = 3,
};


/// chroma formats (according to semantics of chroma_format_idc)
enum chroma_format_e
{
	CHROMA_400  = 0,
	CHROMA_420  = 1,
	CHROMA_422  = 2,
	CHROMA_444  = 3
};

/// supported partition shape
enum part_size_e
{
	SIZE_2Nx2N,           ///< symmetric motion partition,  2Nx2N
	SIZE_2NxN,            ///< symmetric motion partition,  2Nx N
	SIZE_Nx2N,            ///< symmetric motion partition,   Nx2N
	SIZE_NxN,             ///< symmetric motion partition,   Nx N
	SIZE_2NxnU,           ///< asymmetric motion partition, 2Nx( N/2) + 2Nx(3N/2)
	SIZE_2NxnD,           ///< asymmetric motion partition, 2Nx(3N/2) + 2Nx( N/2)
	SIZE_nLx2N,           ///< asymmetric motion partition, ( N/2)x2N + (3N/2)x2N
	SIZE_nRx2N,           ///< asymmetric motion partition, (3N/2)x2N + ( N/2)x2N
	SIZE_NONE = 15
};

/// supported prediction type
enum pred_mode_e
{
	MODE_INTER,           ///< inter-prediction mode
	MODE_INTRA,           ///< intra-prediction mode
	MODE_NONE = 15
};

/// texture component type
enum text_type_e
{
	TEXT_LUMA,            ///< luma
	TEXT_CHROMA,          ///< chroma (U+V)
	TEXT_CHROMA_U,        ///< chroma U
	TEXT_CHROMA_V,        ///< chroma V
	TEXT_ALL,             ///< Y+U+V
	TEXT_NONE = 15
};


enum ref_pic_list_e
{
	REF_PIC_LIST_0 = 0,   ///< reference list 0
	REF_PIC_LIST_1 = 1,   ///< reference list 1
	REF_PIC_LIST_C = 2,   ///< combined reference list for uni-prediction in B-Slices
	REF_PIC_LIST_X = 100  ///< special mark
};

enum d_func_e
{
	DF_DEFAULT  = 0,
	DF_SSE      = 1,      ///< general size SSE
	DF_SSE4     = 2,      ///<   4xM SSE
	DF_SSE8     = 3,      ///<   8xM SSE
	DF_SSE16    = 4,      ///<  16xM SSE
	DF_SSE32    = 5,      ///<  32xM SSE
	DF_SSE64    = 6,      ///<  64xM SSE
	DF_SSE16N   = 7,      ///< 16NxM SSE

	DF_SAD      = 8,      ///< general size SAD
	DF_SAD4     = 9,      ///<   4xM SAD
	DF_SAD8     = 10,     ///<   8xM SAD
	DF_SAD16    = 11,     ///<  16xM SAD
	DF_SAD32    = 12,     ///<  32xM SAD
	DF_SAD64    = 13,     ///<  64xM SAD
	DF_SAD16N   = 14,     ///< 16NxM SAD

	DF_SADS     = 15,     ///< general size SAD with step
	DF_SADS4    = 16,     ///<   4xM SAD with step
	DF_SADS8    = 17,     ///<   8xM SAD with step
	DF_SADS16   = 18,     ///<  16xM SAD with step
	DF_SADS32   = 19,     ///<  32xM SAD with step
	DF_SADS64   = 20,     ///<  64xM SAD with step
	DF_SADS16N  = 21,     ///< 16NxM SAD with step

	DF_HADS     = 22,     ///< general size Hadamard with step
	DF_HADS4    = 23,     ///<   4xM HAD with step
	DF_HADS8    = 24,     ///<   8xM HAD with step
	DF_HADS16   = 25,     ///<  16xM HAD with step
	DF_HADS32   = 26,     ///<  32xM HAD with step
	DF_HADS64   = 27,     ///<  64xM HAD with step
	DF_HADS16N  = 28,     ///< 16NxM HAD with step

#if X265_AMP_SAD
	DF_SAD12    = 43,
	DF_SAD24    = 44,
	DF_SAD48    = 45,

	DF_SADS12   = 46,
	DF_SADS24   = 47,
	DF_SADS48   = 48,

	DF_SSE_FRAME = 50     ///< Frame-based SSE
#else
	DF_SSE_FRAME = 33     ///< Frame-based SSE
#endif
};

/// index for SBAC based RD optimization
enum ci_idx_e
{
	CI_CURR_BEST = 0,     ///< best mode index
	CI_NEXT_BEST,         ///< next best index
	CI_TEMP_BEST,         ///< temporal index
	CI_CHROMA_INTRA,      ///< chroma intra index
	CI_QT_TRAFO_TEST,
	CI_QT_TRAFO_ROOT,
	CI_NUM,               ///< total number
} ;

/// motion vector predictor direction used in AMVP
enum mvp_dir_e
{
	MD_LEFT = 0,          ///< MVP of left block
	MD_ABOVE,             ///< MVP of above block
	MD_ABOVE_RIGHT,       ///< MVP of above right block
	MD_BELOW_LEFT,        ///< MVP of below left block
	MD_ABOVE_LEFT         ///< MVP of above left block
};

enum scaling_list_size_e
{
	SCALING_LIST_4x4 = 0,
	SCALING_LIST_8x8,
	SCALING_LIST_16x16,
	SCALING_LIST_32x32,
	SCALING_LIST_SIZE_NUM
};

enum sao_type_len_e
{
	SAO_EO_LEN    = 4,
	SAO_BO_LEN    = 4,
	SAO_MAX_BO_CLASSES = 32
};

enum sao_type_e
{
	SAO_EO_0 = 0,
	SAO_EO_1,
	SAO_EO_2,
	SAO_EO_3,
	SAO_BO,
	MAX_NUM_SAO_TYPE
};

typedef struct _x265_sao_qt_part_t
{
	int32_t i_best_type;
	int32_t i_length;
	int32_t i_sub_type_idx ; ///< indicates EO class or BO band position
	int32_t i_offset[4];
	int32_t i_start_cu_x;
	int32_t i_start_cu_y;
	int32_t i_end_cu_x;
	int32_t i_end_cu_y;

	int32_t i_part_idx;
	int32_t i_part_level;
	int32_t i_part_col;
	int32_t i_part_row;

	int32_t i_down_parts_idx[X265_NUM_DOWN_PART];
	int32_t i_up_part_idx;

	int32_t b_split;

	//---- encoder only start -----//
	int32_t b_processed;
	double d_min_cost;
	int64_t i_min_dist;
	int32_t i_min_rate;
	//---- encoder only end -----//
} x265_sao_qt_part_t;

typedef struct _x265_sao_lcu_param_t
{
	int32_t b_merge_up_flag;
	int32_t b_merge_left_flag;
	int32_t i_type_idx;
	int32_t i_sub_type_idx;  ///< indicates EO class or BO band position
	int32_t offset[4];
	int32_t i_part_idx;
	int32_t i_part_idx_tmp;
	int32_t i_length;
} x265_sao_lcu_param_t;

typedef struct _x265_sao_param_t
{
	int32_t b_sao_flag[2];
	x265_sao_qt_part_t* psSaoPart[3];
	int32_t i_max_split_level;
	int32_t b_one_unit_flag[3];
	x265_sao_lcu_param_t* sao_lcu_param[3];
	int32_t i_num_cu_in_width;
	int32_t i_num_cu_in_height;
	int32_t i_num_cu_in_width_mul_two ;
	//~SAOParam();
} x265_sao_param_t ;

typedef struct _x265_lf_cu_param_t
{
	int32_t b_internal_edge;     ///< indicates internal edge
	int32_t b_left_edge; ///< indicates left edge
	int32_t b_top_edge;  ///< indicates top edge
} x265_lf_cu_param_t;

#endif

