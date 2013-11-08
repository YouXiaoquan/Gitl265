
#ifndef X265_X265_H
#define X265_X265_H


#include "x265_config.h"

#define X265_BUILD 1

enum nal_unit_type_e
{
	NAL_SLICE_TRAIL_N = 0,   // 0
	NAL_SLICE_TRAIL_R,   // 1

	NAL_SLICE_TSA_N,     // 2
	NAL_SLICE_TLA,       // 3   // Current name in the spec: TSA_R

	NAL_SLICE_STSA_N,    // 4
	NAL_SLICE_STSA_R,    // 5

	NAL_SLICE_RADL_N,    // 6
	NAL_SLICE_DLP,       // 7 // Current name in the spec: RADL_R

	NAL_SLICE_RASL_N,    // 8
	NAL_SLICE_TFD,       // 9 // Current name in the spec: RASL_R

	NAL_UNIT_RESERVED_10,
	NAL_UNIT_RESERVED_11,
	NAL_UNIT_RESERVED_12,
	NAL_UNIT_RESERVED_13,
	NAL_UNIT_RESERVED_14,
	NAL_UNIT_RESERVED_15,

	NAL_SLICE_BLA,       // 16   // Current name in the spec: BLA_W_LP
	NAL_SLICE_BLANT,     // 17   // Current name in the spec: BLA_W_DLP
	NAL_SLICE_BLA_N_LP,  // 18
	NAL_SLICE_IDR,       // 19  // Current name in the spec: IDR_W_DLP
	NAL_SLICE_IDR_N_LP,  // 20
	NAL_SLICE_CRA,       // 21
    NAL_VPS         = 32,
    NAL_SPS         = 33,
    NAL_PPS         = 34,
    /* ref_idc == 0 for 6,9,10,11,12 */
};

/* The data within the payload is already NAL-encapsulated;
 * All data returned in an x265_nal_t, including the data in p_payload, is no longer
 * valid after the next call to x265_encoder_encode.  Thus it must be used or copied
 * before calling x265_encoder_encode or x265_encoder_headers again. */
typedef struct
{
	enum nal_unit_type_e i_type ; ///< nal_unit_type
	uint32_t i_temporal_id ;  ///< temporal_id
	uint32_t i_reserved_zero6_bits ; ///< reserved_zero_6bits

	int32_t b_long_start_code ;

    /* Size of payload in bytes. */
    int     i_payload;
    uint8_t *p_payload;
} x265_nal_t;




/* CPU flags
 */
/* x86 */

#define X265_CPU_CACHELINE_32    0x0000001  /* avoid memory loads that span the border between two cachelines */
#define X265_CPU_CACHELINE_64    0x0000002  /* 32/64 is the size of a cacheline in bytes */
#define X265_CPU_ALTIVEC         0x0000004
#define X265_CPU_MMX             0x0000008
#define X265_CPU_MMX2            0x0000010  /* MMX2 aka MMXEXT aka ISSE */
#define X265_CPU_MMXEXT          X265_CPU_MMX2
#define X265_CPU_SSE             0x0000020
#define X265_CPU_SSE2            0x0000040
#define X265_CPU_SSE2_IS_SLOW    0x0000080  /* avoid most SSE2 functions on Athlon64 */
#define X265_CPU_SSE2_IS_FAST    0x0000100  /* a few functions are only faster on Core2 and Phenom */
#define X265_CPU_SSE3            0x0000200
#define X265_CPU_SSSE3           0x0000400
#define X265_CPU_SHUFFLE_IS_FAST 0x0000800  /* Penryn, Nehalem, and Phenom have fast shuffle units */
#define X265_CPU_STACK_MOD4      0x0001000  /* if stack is only mod4 and not mod16 */
#define X265_CPU_SSE4            0x0002000  /* SSE4.1 */
#define X265_CPU_SSE42           0x0004000  /* SSE4.2 */
#define X265_CPU_SSE_MISALIGN    0x0008000  /* Phenom support for misaligned SSE instruction arguments */
#define X265_CPU_LZCNT           0x0010000  /* Phenom support for "leading zero count" instruction. */
#define X265_CPU_ARMV6           0x0020000
#define X265_CPU_NEON            0x0040000  /* ARM NEON */
#define X265_CPU_FAST_NEON_MRC   0x0080000  /* Transfer from NEON to ARM register is fast (Cortex-A9) */
#define X265_CPU_SLOW_CTZ        0x0100000  /* BSR/BSF x86 instructions are really slow on some CPUs */
#define X265_CPU_SLOW_ATOM       0x0200000  /* The Atom just sucks */
#define X265_CPU_AVX             0x0400000  /* AVX support: requires OS support even if YMM registers
                                             * aren't used. */
#define X265_CPU_XOP             0x0800000  /* AMD XOP */
#define X265_CPU_FMA4            0x1000000  /* AMD FMA4 */
#define X265_CPU_AVX2            0x2000000  /* AVX2 */
#define X265_CPU_FMA3            0x4000000  /* Intel FMA3 */
#define X265_CPU_BMI1            0x8000000  /* BMI1 */
#define X265_CPU_BMI2           0x10000000  /* BMI2 */
#define X265_CPU_TBM            0x20000000  /* AMD TBM */



/* Log level */
#define X265_LOG_NONE          (-1)
#define X265_LOG_ERROR          0
#define X265_LOG_WARNING        1
#define X265_LOG_INFO           2
#define X265_LOG_DEBUG          3



typedef struct gop_entry_t
{
	int32_t         i_poc;
	int32_t         i_qp_Offset;
	double              f_qp_factor;
	int32_t         i_tc_offset_div_2;
	int32_t         i_beta_offset_div_2;
	int32_t         i_temporal_id;
	int32_t         b_ref_pic;
	int32_t         i_num_ref_pics_active;
	int8_t              i_slice_type;
	int32_t         i_num_ref_pics;
	int32_t         reference_pics[X265_MAX_NUM_REF_PICS];
	int32_t         used_by_curr_pic[X265_MAX_NUM_REF_PICS];
#if X265_AUTO_INTER_RPS
	int32_t         i_inter_rps_prediction;
#else
	int32_t         b_inter_rps_prediction;
#endif
	int32_t         i_delta_rps;
	int32_t         i_num_ref_idc;
	int32_t         i_ref_idc[X265_MAX_NUM_REF_PICS+1];

} gop_entry_t ;


typedef struct x265_param_t
{
	unsigned int    cpu ;
    void            (*pf_log)( void *, int i_level, const char *psz, va_list );
    void            *p_log_private;
    int             i_log_level;
    char            *psz_dump_yuv ;                            ///< output reconstruction file
    int             i_slice_count ;

    int             b_repeat_headers;                          ///< put SPS/PPS before each keyframe
    int             b_annexb;                                  ///< if set, place start codes (4 bytes) before NAL units,
    														   ///<otherwise place size (4 bytes) before NAL units.

    struct
    {
    	double         f_ad_lambda_modifier[ X265_MAX_TLAYER ];///< Lambda modifier array for each temporal layer
    	int32_t        i_max_delta_qp;		                   ///< max. |delta QP|
    	uint32_t       i_delta_qp_rd;                          ///< dQP range for multi-pass slice QP optimization
    	int32_t        i_max_cu_delta_qp_depth;                ///< Max. depth for a minimum CuDQPSize (0:default)
    	int32_t        b_use_rdo_q;                            ///< flag for using RD optimized quantization
    	int32_t        b_use_rdo_q_ts;                         ///< flag for using RD optimized quantization for transform skip
#if X265_L0232_RD_PENALTY
    	int32_t        i_rd_penalty;                           ///< RD-penalty for 32x32 TU for intra in non-intra slices (0: no RD-penalty, 1: RD-penalty, 2: maximum RD-penalty)
#endif
    } rdo ;

    int32_t         i_width;                                   ///< source width in pixel
    int32_t         i_height;                                  ///< source height in pixel
    int32_t         i_frame_total;                             ///< number of encoded frames
    uint32_t        i_frame_skip;                              ///< number of skipped frames from the beginning

    struct
    {
#if X265_SIGNAL_BITRATE_PICRATE_IN_VPS
    	int32_t     i_bit_rate_pic_rate_max_t_layers;          ///< Indicates max. number of sub-layers for which bit rate is signalled.
    	int32_t*    bit_rate_info_present_flag;                ///< Indicates whether bit rate information is signalled
    	int32_t*    pic_rate_info_present_flag;                ///< Indicates whether pic rate information is signalled
    	int32_t*    avg_bit_rate;                              ///< Indicates avg. bit rate information for various sub-layers
    	int32_t*    max_bit_rate;                              ///< Indicates max. bit rate information for various sub-layers
    	int32_t*    avg_pic_rate;                              ///< Indicates avg. picture rate information for various sub-layers
    	int32_t*    constant_pic_rate_idc;                     //< Indicates constant picture rate idc for various sub-layers
#endif
    } vps ;
    struct
    {
    	int32_t         i_conformance_mode;
        int32_t         i_conf_left;
        int32_t         i_conf_right;
        int32_t         i_conf_top;
        int32_t         i_conf_bottom;
        enum profile_name_e i_profile;
        enum level_tier_e   i_level_tier;
        enum level_name_e   i_level;
#if X265_L0046_CONSTRAINT_FLAGS
        int32_t         b_progressive_source_flag;
        int32_t         b_interlaced_source_flag;
        int32_t         b_non_packed_constraint_flag;
        int32_t         b_frame_only_constraint_flag;
#endif

        int32_t         i_num_reorder_pics[X265_MAX_TLAYER];   ///< total number of reorder pictures
        int32_t         max_dec_pic_buffering[X265_MAX_TLAYER];///< total number of reference pictures needed for decoding
        int32_t         i_max_temp_layer;                      ///< Max temporal layer

    	// coding unit (CU) definition
    	uint32_t        i_max_cu_width;                        ///< max. CU width in pixel
    	uint32_t        i_max_cu_height;                       ///< max. CU height in pixel
    	uint32_t        i_max_cu_depth;                        ///< max. CU depth

    	// transfom unit (TU) definition
    	uint32_t        i_quadtree_tu_log2_max_size;
    	uint32_t        i_quadtree_tu_log2_min_size;

    	uint32_t        i_quadtree_tu_max_depth_inter;
    	uint32_t        i_quadtree_tu_max_depth_intra;

        int32_t         i_bit_depth_y;                         ///< bit-depth codec operates at in luma (input/output files will be converted)
        int32_t         i_bit_depth_c;                         ///< bit-depth codec operates at in chroma (input/output files will be converted)

    	int32_t         b_use_lossless;                        ///< flag for using lossless coding
    	int32_t         b_use_sao;
    	int32_t         b_loop_filter_disable;                 ///< flag for using deblocking filter
    	int32_t         b_use_pcm;                             ///< flag for using IPCM

    	int32_t         i_tmvp_mode_id;

    	int32_t         i_use_scaling_list_id;                 ///< using quantization matrix
        char            *psz_cqm_file ;                        ///< quantization matrix file name
    	int32_t         b_use_strong_intra_smoothing;          ///< enable strong intra smoothing for 32x32 blocks where the reference samples are flat

    	int32_t         b_vui_parameters_present_flag;         ///< enable generation of VUI parameters
    	struct
    	{
    		int32_t         b_aspec_ratio_info_present_flag;       ///< Signals whether aspectRatioIdc is present
    		int32_t         i_aspect_ratio_idc;                    ///< aspectRatioIdc
    		int32_t         i_sar_width;                           ///< horizontal size of the sample aspect ratio
    		int32_t         i_sar_height;                          ///< vertical size of the sample aspect ratio
    		int32_t         b_overscan_info_present_flag;          ///< Signals whether overscanAppropriateFlag is present
    		int32_t         b_overscan_appropriate_flag;           ///< Indicates whether conformant decoded pictures are suitable for display using overscan
    		int32_t         b_video_signal_type_present_flag;      ///< Signals whether video_format, videoFullRangeFlag, and colourDescriptionPresentFlag are present
    		int32_t         i_video_format;                        ///< Indicates representation of pictures
    		int32_t         b_video_full_range_flag;               ///< Indicates the black level and range of luma and chroma signals
    		int32_t         b_colour_description_present_flag;     ///< Signals whether colourPrimaries, transferCharacteristics and matrixCoefficients are present
    		int32_t         i_colour_primaries;                    ///< Indicates chromaticity coordinates of the source primaries
    		int32_t         i_transfer_characteristics;            ///< Indicates the opto-electronic transfer characteristics of the source
    		int32_t         i_matrix_coefficients;                 ///< Describes the matrix coefficients used in deriving luma and chroma from RGB primaries
    		int32_t         b_chroma_loc_info_present_flag;        ///< Signals whether chromaSampleLocTypeTopField and chromaSampleLocTypeBottomField are present
    		int32_t         i_chroma_sample_loc_type_top_field;    ///< Specifies the location of chroma samples for top field
    		int32_t         i_chroma_sample_loc_type_bottom_field; ///< Specifies the location of chroma samples for bottom field
    		int32_t         b_neutral_chroma_indication_flag;      ///< Indicates that the value of all decoded chroma samples is equal to 1<<(_bit_depth_cr-1)
    		int32_t         b_default_display_window_flag;         ///< Indicates the presence of the default window parameters
    		int32_t         i_def_disp_win_left_offset;            ///< Specifies the left offset from the conformance window of the default window
    		int32_t         i_def_disp_win_right_offset;           ///< Specifies the right offset from the conformance window of the default window
    		int32_t         i_def_disp_win_top_offset;             ///< Specifies the top offset from the conformance window of the default window
    		int32_t         i_def_disp_win_bottom_offset;          ///< Specifies the bottom offset from the conformance window of the default window
    		int32_t         b_frame_field_info_present_flag;       ///< Indicates that picStruct values are present in pictureTiming SEI messages
    		int32_t         b_poc_proportional_to_timing_flag;     ///< Indicates that the POC value is proportional to the output time w.r.t. first picture in CVS
    		int32_t         i_num_ticks_poc_diff_one_minus1;       ///< Number of ticks minus 1 that for a POC difference of one
    		int32_t         b_bitstream_restriction_flag;          ///< Signals whether bitstream restriction parameters are present
    		int32_t         b_tiles_fixed_structure_flag;          ///< Indicates that each active picture parameter set has the same values of the syntax elements related to tiles
    		int32_t         b_motion_vectors_over_pic_boundaries_flag;///< Indicates that no samples outside the picture boundaries are used for inter prediction
    		int32_t         i_min_spatial_segmentation_idc;        ///< Indicates the maximum size of the spatial segments in the pictures in the coded video sequence
    		int32_t         i_max_bytes_per_pic_denom;             ///< Indicates a number of bytes not exceeded by the sum of the sizes of the VLC NAL units associated with any coded picture
    		int32_t         i_max_bits_per_min_cu_denom;           ///< Indicates an upper bound for the number of bits of coding_unit() data
    		int32_t         i_log2_max_mv_length_horizontal;       ///< Indicate the maximum absolute value of a decoded horizontal MV component in quarter-pel luma units
    		int32_t         i_log2_max_mv_length_vertical;         ///< Indicate the maximum absolute value of a decoded vertical MV component in quarter-pel luma units
    	} vui ;

    } sps ;

    struct
    {
		int32_t         i_uniform_spacing_idr;
		int32_t         i_num_columns_minus_1;
		int32_t         i_num_rows_minus_1;
		uint32_t*       column_width;
		uint32_t*       row_height;

		int32_t         i_wave_front_synchro;                  ///< 0: no WPP. >= 1: WPP is enabled, the "Top right" from which inheritance occurs is this LCU offset in the line above the current.
		int32_t         i_wave_front_substreams;               ///< If iWaveFrontSynchro, this is the number of substreams per frame (dependent tiles) or per tile (independent tiles).

		int32_t         b_use_weighted_pred;                   ///< Use of weighted prediction in P slices
		int32_t         b_use_weighted_bi_pred;                ///< Use of bi-directional weighted prediction in B slices

		uint32_t        i_log2_parallel_merge_level;           ///< Parallel merge estimation region
		int32_t         i_sign_hide_flag;

		int32_t         b_transquant_bypass_enable_flag;       ///< transquant_bypass_enable_flag setting in PPS.

    } pps ;

    struct
    {
    	uint32_t        i_intra_period;                        ///< period of I-slice (random access period)
    	int32_t         i_decoding_refresh_type;               ///< random access type
    	int32_t         i_gop_size;                            ///< GOP size of hierarchical structure
    	int32_t         i_extraRPSs;                           ///< extra RPSs added to handle CRA
    	gop_entry_t     gop_list[X265_MAX_GOP];                ///< the coding structure entries from the config file
    	int32_t         num_reorder_pics[X265_MAX_TLAYER];     ///< total number of reorder pictures
    	int32_t         max_dec_pic_buffering[X265_MAX_TLAYER];///< total number of reference pictures needed for decoding
    } gop ;

    struct
    {
    	// coding tool (lossless)
    	int32_t         i_max_num_offsets_per_pic;             ///< SAO maximun number of offset per picture
    	int32_t         i_sao_lcu_boundary;                    ///< SAO parameter estimation using non-deblocked pixels for LCU bottom and right boundary areas
    	int32_t         i_sao_lcu_based_optimization;          ///< SAO LCU-based optimization

    } sao ;

    struct
    {
    	// coding tools (loop filter)
    	int32_t         i_loop_filter_offset_in_pps;           ///< offset for deblocking filter in 0 = slice header, 1 = PPS
    	int32_t         i_loop_filter_beta_offset_div_2;       ///< beta offset for deblocking filter
    	int32_t         i_loop_filter_tc_offset_div_2;         ///< tc offset for deblocking filter
    	int32_t         i_deblocking_filter_control_present;   ///< deblocking filter control present flag in PPS

    	int32_t         b_lf_cross_slice_boundary_flag;        ///< 1: filter across slice boundaries 0: do not filter across slice boundaries
    	int32_t         b_lf_cross_tile_boundary_flag;         ///< 1: filter across tile boundaries  0: do not filter across tile boundaries

    } deblock ;

    struct
    {
    	// coding tools (PCM)
    	uint32_t        i_pcm_log2_max_size;                   ///< log2 of maximum PCM block size
    	uint32_t        i_pcm_log2_min_size;                   ///< log2 of minimum PCM block size
    	int32_t         b_pcm_filter_disable_flag;             ///< PCM filter disable flag
    } pcm ;

    struct
    {
		int32_t         i_decoded_picture_hash_sei_enabled;    ///< Checksum(3)/CRC(2)/MD5(1)/disable(0) acting on decoded picture hash _sei_ message
		int32_t         i_recovery_point_sei_enabled;
		int32_t         i_buffering_period_sei_enabled;
		int32_t         i_picture_timing_sei_enabled;
		int32_t         i_framePacking_sei_enabled;
		int32_t         i_framePacking_sei_type;
		int32_t         i_framePacking_sei_id;
		int32_t         i_framePacking_sei_quincunx;
		int32_t         i_framePacking_sei_interpretation;
		int32_t         i_display_orientation_sei_Angle;
		int32_t         i_temporal_level0_index_sei_enabled;
		int32_t         i_gradual_decoding_refresh_info_enabled;
		int32_t         i_decoding_unit_info_sei_enabled;
		int32_t         i_active_parameter_sets_sei_enabled;

    } sei ;

#if X265_RATE_CONTROL_LAMBDA_DOMAIN
    struct
    {
		int32_t         b_enable_rate_control;                 ///< enable rate control or not
		int32_t         i_target_bitrate;                      ///< target bitrate when rate control is enabled
		int32_t         b_keep_hierarchical_bit;               ///< whether keeping hierarchical bit allocation structure or not
		int32_t         b_lcu_level_rc;                        ///< true: LCU level rate control; false: picture level rate control
		int32_t         b_use_lcu_separate_model;              ///< use separate R-lambda model at LCU level
		int32_t         i_initial_qp;                          ///< inital QP for rate control
		int32_t         b_force_intra_qp;                      ///< force all intra picture to use initial QP or not
    } rc ;
#endif

	int32_t         i_pad[2];
	int32_t         b_use_list_combination;                    ///< flag for using combined reference list for uni-prediction in B-slices (JCTVC-D421)
	int32_t         b_use_transform_skip;                      ///< flag for enabling intra transform skipping
	int32_t         b_use_transform_skip_fast;                 ///< flag for enabling fast intra transform skipping
	int32_t         b_enable_amp;

	// coding quality
	double          f_qp;                                      ///< QP value of key-picture (floating point)
	int32_t         i_qp;                                      ///< QP value of key-picture (integer)
	int32_t*        delta_qp;                                  ///< array of slice QP values

	int32_t         i_cb_qp_offset;                            ///< Chroma Cb QP Offset (0:default)
	int32_t         i_cr_qp_offset;                            ///< Chroma Cr QP Offset (0:default)

#if X265_ADAPTIVE_QP_SELECTION
	int32_t         b_use_adapt_qp_select;
#endif

	int32_t         b_use_adaptive_qp;                         ///< Flag for enabling QP adaptation based on a psycho-visual model
	int32_t         i_qp_adaptation_range;                     ///< dQP range by QP adaptation
	int32_t         i_frame_rate;                              ///< source frame-rates (Hz)

	// coding tools (bit-depth)
	int32_t         i_input_bit_depth_y;                       ///< bit-depth of input file (luma component)
	int32_t         i_input_bit_depth_c;                       ///< bit-depth of input file (chroma component)
	int32_t         i_output_bit_depth_y;                      ///< bit-depth of output file (luma component)
	int32_t         i_output_bit_depth_c;                      ///< bit-depth of output file (chroma component)

	int32_t         b_pcm_input_bit_depth_flag;                ///< 0: PCM bit-depth is internal bit-depth. 1: PCM bit-depth is input bit-depth.

	// coding tools (encoder-only parameters)
	int32_t         b_use_sbac_rd;                             ///< flag for using RD optimization based on SBAC
	int32_t         b_use_asr;                                 ///< flag for using adaptive motion search range
	int32_t         b_use_had_me;                              ///< flag for using HAD in sub-pel ME
	int32_t         i_fast_search;                             ///< ME mode, 0 = full, 1 = diamond, 2 = PMVFAST
	int32_t         i_search_range;                            ///< ME search range
	int32_t         i_bipred_search_range;                     ///< ME search range for bipred refinement
	int32_t         b_use_fast_enc;                            ///< flag for using fast encoder setting
	int32_t         b_use_early_cu;                            ///< flag for using Early CU setting
	int32_t         b_use_fast_decision_for_merge;             ///< flag for using Fast Decision Merge RD-Cost
	int32_t         b_use_cbf_fast_mode;                       ///< flag for using Cbf Fast PU Mode Decision
	int32_t         b_use_early_skip_detection;                ///< flag for using Early SKIP Detection
	int32_t         i_slice_mode;                              ///< 0: no slice limits,
															   ///< 1: max number of CTBs per slice,
															   ///< 2: max number of bytes per slice,
															   ///< 3: max number of tiles per slice
	int32_t         i_slice_argument;                          ///< argument according to selected slice mode
	int32_t         i_slice_segment_mode;                      ///< 0: no slice segment limits,
															   ///< 1: max number of CTBs per slice segment,
															   ///< 2: max number of bytes per slice segment,
															   ///< 3: max number of tiles per slice segment
	int32_t         i_slice_segment_argument;                  ///< argument according to selected slice segment mode


	int32_t         b_use_constrained_intra_pred;              ///< flag for using constrained intra prediction

	uint32_t        i_max_num_merge_cand;                      ///< Max number of merge candidates

#if !X265_RATE_CONTROL_LAMBDA_DOMAIN
	int32_t         b_enable_rate_ctrl;                        ///< Flag for using rate control algorithm
	int32_t         i_target_bitrate;                          ///< target bitrate
	int32_t         i_num_lcu_in_unit;                         ///< Total number of LCUs in a frame should be completely divided by the NumLCUInUnit
#endif

	int32_t         b_cu_transquant_bypass_flag_value;         ///< if transquant_bypass_enable_flag, the fixed value to use for the per-CU cu_transquant_bypass_flag.
	int32_t         b_recalculate_qp_according_to_lambda;      ///< recalculate QP value according to the lambda value

	void (*nalu_process) ( x265_t *h, x265_nal_t *nal );

} x265_param_t ;

void x265_nal_encode( x265_t *h, uint8_t *dst, x265_nal_t *nal );

extern char *low_delay_config[] ;

void x265_init_gop_entry ( gop_entry_t * p_gop_entry ) ;
/* x265_param_default:
 *      fill x265_param_t with default values */
void    x265_param_default( x265_param_t * );

/* x265_param_parse:
 *  set one parameter by name.
 *  returns 0 on success, or returns one of the following errors.
 *  note: BAD_VALUE occurs only if it can't even parse the value,
 *  numerical range is not checked until x265_encoder_open() or
 *  x265_encoder_reconfig().
 *  value=NULL means "true" for boolean options, but is a BAD_VALUE for non-booleans. */
#define X265_PARAM_BAD_NAME  (-1)
#define X265_PARAM_BAD_VALUE (-2)
int x265_param_parse( x265_param_t *, const char *name, const char *value );
int x265_config_file_parse( x265_param_t *, const char *config_file_name );
void x265_print_parameters ( x265_param_t *p ) ;
int x265_check_parameters ( x265_param_t *p ) ;

int x265_param_default_preset( x265_param_t * );
void x265_param_destroy ( x265_param_t * ) ;

typedef struct
{
	int64_t i_dts;
	int64_t i_pts;
    /* In: custom encoding parameters to be set from this frame forwards
           (in coded order, not display order). If NULL, continue using
           parameters from the previous frame.  Some parameters, such as
           aspect ratio, can only be changed per-GOP due to the limitations
           of H.265 itself; in this case, the caller must force an IDR frame
           if it needs the changed parameter to apply immediately. */
    x265_param_t *param;
    /* In: raw image data */
    /* Out: reconstructed image data.  x265 may skip part of the reconstruction process,
            e.g. deblocking, in frames where it isn't necessary.  To force complete
            reconstruction, at a small speed cost, set b_full_recon. */
    x265_image_t img;
} x265_picture_t;

/* x265_picture_init:
 *  initialize an x265_picture_t.  Needs to be done if the calling application
 *  allocates its own x265_picture_t as opposed to using x265_picture_alloc. */
void x265_picture_init( x265_picture_t *pic );

/* x265_picture_alloc:
 *  alloc data for a picture. You must call x265_picture_clean on it.
 *  returns 0 on success, or -1 on malloc failure or invalid colorspace. */
int x265_picture_alloc( x265_picture_t *pic, int i_csp, int i_width, int i_height );

/* x265_picture_clean:
 *  free associated resource for a x265_picture_t allocated with
 *  x265_picture_alloc ONLY */
void x265_picture_clean( x265_picture_t *pic );



/****************************************************************************
 * Encoder functions
 ****************************************************************************/

/* Force a link error in the case of linking against an incompatible API version.
 * Glue #defines exist to force correct macro expansion; the final output of the macro
 * is x265_encoder_open_##X265_BUILD (for purposes of dlopen). */
#define x265_encoder_glue1(x,y) x##y
#define x265_encoder_glue2(x,y) x265_encoder_glue1(x,y)
#define x265_encoder_open x265_encoder_glue2(x265_encoder_open_,X265_BUILD)

/* x265_encoder_open:
 *      create a new encoder handler, all parameters from x265_param_t are copied */
x265_t *x265_encoder_open( x265_param_t * );


/* x265_encoder_headers:
 *      return the VPS, SPS and PPS that will be used for the whole stream.
 *      *pi_nal is the number of NAL units outputted in pp_nal.
 *      returns negative on error.
 *      the payloads of all output NALs are guaranteed to be sequential in memory. */
int     x265_encoder_headers( x265_t *, x265_nal_t **pp_nal, int *pi_nal );

/* x265_encoder_encode:
 *      encode one picture.
 *      *pi_nal is the number of NAL units outputted in pp_nal.
 *      returns negative on error, zero if no NAL units returned.
 *      the payloads of all output NALs are guaranteed to be sequential in memory. */
int     x265_encoder_encode( x265_t *, x265_nal_t **pp_nal, int *pi_nal, x265_picture_t *pic_in, x265_picture_t *pic_out );
/* x265_encoder_close:
 *      close an encoder handler */
void    x265_encoder_close  ( x265_t * );
/* x265_encoder_delayed_frames:
 *      return the number of currently delayed (buffered) frames
 *      this should be used at the end of the stream, to know when you have all the encoded frames. */
int     x265_encoder_delayed_frames( x265_t * );

void x265_encoder_print_summary(x265_t *h) ;

#endif



