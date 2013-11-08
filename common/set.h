


#ifndef X265_SET_H
#define X265_SET_H




///< list number for quantization matrix
#define SCALING_LIST_NUM 6
///< list number for quantization matrix 32x32
#define SCALING_LIST_NUM_32x32 2
///< remainder of QP/6
#define SCALING_LIST_REM_NUM 6
///< start value for dpcm mode
#define SCALING_LIST_START_VALUE 8
///< max coefficient number for quantization matrix
#define X265_MAX_MATRIX_COEF_NUM 64
///< max size number for quantization matrix
#define X265_MAX_MATRIX_SIZE_NUM 8
///< default DC value
#define SCALING_LIST_DC 16

typedef struct
{
	int32_t i_number_of_pictures ;
	int32_t i_number_of_negative_pictures ;
	int32_t i_number_of_positive_pictures ;
	int32_t i_number_of_longterm_pictures ;
	int32_t delta_poc[X265_MAX_NUM_REF_PICS] ;
	int32_t poc[X265_MAX_NUM_REF_PICS] ;
	int32_t used[X265_MAX_NUM_REF_PICS] ;
	int32_t b_inter_rps_prediction ;
	int32_t i_delta_ridx_minus1 ;
	int32_t i_delta_rps ;
	int32_t i_num_ref_idc ;
	int32_t ref_idc[X265_MAX_NUM_REF_PICS+1] ;
	int32_t check_lt_msb[X265_MAX_NUM_REF_PICS] ;
	int32_t poc_lsb_lt[X265_MAX_NUM_REF_PICS] ;
	int32_t delta_poc_msb_cycle_lt[X265_MAX_NUM_REF_PICS] ;
	int32_t delta_poc_msb_present_flag[X265_MAX_NUM_REF_PICS] ;


} x265_reference_picture_set_t ;


typedef struct
{
	int32_t i_number_of_reference_picture_sets ;
	x265_reference_picture_set_t *reference_picture_sets ;
} x265_rps_list_t ;

typedef struct
{
	//!< the DC value of the matrix coefficient for 16x16
	int32_t scaling_list_dc [SCALING_LIST_SIZE_NUM][SCALING_LIST_NUM] ;
	//!< UseDefaultScalingMatrixFlag
	int32_t use_default_scaling_matrix_flag [SCALING_LIST_SIZE_NUM][SCALING_LIST_NUM] ;
	//!< RefMatrixID
	uint32_t ref_matrix_id [SCALING_LIST_SIZE_NUM][SCALING_LIST_NUM] ;
	//!< flag for using default matrix
	int32_t b_scaling_list_present_flag ;
	//!< reference list index
	uint32_t pred_matrix_id [SCALING_LIST_SIZE_NUM][SCALING_LIST_NUM] ;
	//!< quantization matrix
	int32_t *scaling_list_coef [SCALING_LIST_SIZE_NUM][SCALING_LIST_NUM] ;
	//!< transform skipping flag for setting default scaling matrix for 4x4
	int32_t b_use_transform_skip ;
} x265_scaling_list_t ;

typedef struct
{
	int32_t i_profile_space ;
	int32_t b_tier_flag ;
	int32_t i_profile_idc ;
	int32_t profile_compatibility_flag[32] ;
	int32_t i_level_idc ;

#if X265_L0046_CONSTRAINT_FLAGS
	int32_t b_progressive_source_flag ;
	int32_t b_interlaced_source_flag ;
	int32_t b_non_packed_constraint_flag ;
	int32_t b_frame_only_constraint_flag ;
#endif

} x265_profile_tier_level_t ;


typedef struct
{
	x265_profile_tier_level_t p_general_ptl ;
	x265_profile_tier_level_t sub_layer_ptl[6] ;
	int32_t sub_layer_profile_present_flag[6] ;
	int32_t sub_layer_level_present_flag[6] ;

} x265_ptl_t ;

#if X265_SIGNAL_BITRATE_PICRATE_IN_VPS

typedef struct
{
	int32_t bit_rate_info_present_flag[X265_MAX_TLAYER] ;
	int32_t pic_rate_info_present_flag[X265_MAX_TLAYER] ;
	int32_t avg_bit_rate[X265_MAX_TLAYER] ;
	int32_t max_bit_rate[X265_MAX_TLAYER] ;
	int32_t constant_pic_rate_idc[X265_MAX_TLAYER] ;
	int32_t avg_pic_rate[X265_MAX_TLAYER] ;

} x265_bit_rate_pic_rate_info_t ;


#endif


typedef struct
{
	int32_t  b_fixed_pic_rate_flag ;
	int32_t  b_fixed_pic_rate_within_cvs_flag ;
	uint32_t i_pic_duration_in_tc_minus1 ;
	int32_t  b_low_delay_hrd_flag ;
	uint32_t i_cpb_cnt_minus1 ;
	uint32_t bit_rate_value_minus1 [X265_MAX_CPB_CNT][2] ;
	uint32_t cpb_size_value [X265_MAX_CPB_CNT][2] ;
	uint32_t du_cpb_size_value [X265_MAX_CPB_CNT][2] ;
	uint32_t cbr_flag [X265_MAX_CPB_CNT][2] ;
#if X265_L0363_DU_BIT_RATE
	uint32_t du_bit_rate_value [X265_MAX_CPB_CNT][2] ;
#endif

} x265_hrd_sub_layer_info_t ;


typedef struct
{

#if !X265_L0043_TIMING_INFO
	int32_t b_timing_info_present_flag ;
	uint32_t i_num_units_in_tick ;
	uint32_t i_time_scale ;
#endif

	int32_t b_nal_hrd_parameters_present_flag ;
	int32_t b_vcl_hrd_parameters_present_flag ;
	int32_t b_sub_pic_cpb_params_present_flag ;
	uint32_t i_tick_divisor_minus2 ;
	uint32_t i_du_cpb_removal_delay_length_minus1 ;
	int32_t b_sub_pic_cpb_params_in_pic_timing_sei_flag ;
#if X265_L0044_DU_DPB_OUTPUT_DELAY_HRD
	uint32_t i_dpb_output_delay_du_length_minus1 ;
#endif
	uint32_t i_bit_rate_scale ;
	uint32_t i_cpb_size_scale ;
	uint32_t i_du_cpb_size_scale ;
	uint32_t i_initial_cpb_removal_delay_length_minus1 ;
	uint32_t i_cpb_removal_delay_length_minus1 ;
	uint32_t i_dpb_output_delay_length_minus1 ;
	uint32_t i_num_d_u ;
	x265_hrd_sub_layer_info_t hrd [X265_MAX_TLAYER] ;


} x265_hrd_t ;


typedef struct
{
	int32_t b_timing_info_present_flag ;
	uint32_t i_num_units_in_tick ;
	uint32_t i_time_scale ;
	int32_t b_poc_proportional_to_timing_flag ;
	int32_t i_num_ticks_poc_diff_one_minus1 ;

} x265_timing_info_t ;



typedef struct
{
	int32_t i_vps_id ;
	uint32_t i_max_t_layers ;
	uint32_t i_max_layers ;
	int32_t b_temporal_id_nesting_flag ;

	uint32_t num_reorder_pics[X265_MAX_TLAYER] ;
	uint32_t max_dec_pic_buffering[X265_MAX_TLAYER] ;
	uint32_t max_latency_increase[X265_MAX_TLAYER] ;

	uint32_t i_num_hrd_parameters ;
	uint32_t i_max_nuh_reserved_zero_layer_id ;
	x265_hrd_t * hrd_parameters ;
	uint32_t * hrd_op_set_idx ;
	int32_t * cprms_present_flag ;
	uint32_t i_num_op_sets ;
	int32_t layer_id_included_flag[X265_MAX_VPS_OP_SETS_PLUS1][X265_MAX_VPS_NUH_RESERVED_ZERO_LAYER_ID_PLUS1] ;

	x265_ptl_t ptl ;
#if X265_SIGNAL_BITRATE_PICRATE_IN_VPS
	x265_bit_rate_pic_rate_info_t b_bit_rate_pic_rate_info ;
#endif
#if X265_L0043_TIMING_INFO
	x265_timing_info_t timing_info ;
#endif


} x265_vps_t ;

typedef struct
{
	int32_t b_enabled_flag ;
	int32_t i_win_left_offset ;
	int32_t i_win_right_offset ;
	int32_t i_win_top_offset ;
	int32_t i_win_bottom_offset ;
} x265_window_t ;

typedef struct
{

	int32_t b_aspect_ratio_info_present_flag ;
	int32_t i_aspect_ratio_idc ;
	int32_t i_sar_width ;
	int32_t i_sar_height ;
	int32_t b_overscan_info_present_flag ;
	int32_t b_overscan_appropriate_flag ;
	int32_t b_video_signal_type_present_flag ;
	int32_t i_video_format ;
	int32_t b_video_full_range_flag ;
	int32_t b_colour_description_present_flag ;
	int32_t i_colour_primaries ;
	int32_t i_transfer_characteristics ;
	int32_t i_matrix_coefficients ;
	int32_t b_chroma_loc_info_present_flag ;
	int32_t i_chroma_sample_loc_type_top_field ;
	int32_t i_chroma_sample_loc_type_bottom_field ;
	int32_t b_neutral_chroma_indication_flag ;
	int32_t b_field_seq_flag ;

	x265_window_t default_display_window ;
	int32_t b_frame_field_info_present_flag ;
	int32_t b_hrd_parameters_present_flag ;
	int32_t b_bitstream_restriction_flag ;
	int32_t b_tiles_fixed_structure_flag ;
	int32_t b_motion_vectors_over_pic_boundaries_flag ;
	int32_t b_restricted_ref_pic_lists_flag ;
	int32_t i_min_spatial_segmentation_idc ;
	int32_t i_max_bytes_per_pic_denom ;
	int32_t i_max_bits_per_min_cu_denom ;
	int32_t i_log2_max_mv_length_horizontal ;
	int32_t i_log2_max_mv_length_vertical ;
	x265_hrd_t hrd_parameters ;
#if X265_L0043_TIMING_INFO
	x265_timing_info_t timing_info ;
#else
	int32_t b_poc_proportional_to_timing_flag ;
	int32_t i_num_ticks_poc_diff_one_minus1 ;
#endif


} x265_vui_t ;

typedef struct
{
	int32_t i_sps_id ;
	int32_t i_vps_id ;
	int32_t i_chroma_format_idc ;

	// maximum number of temporal layers
	uint32_t i_max_t_layers ;

	// _structure
	uint32_t i_pic_width_in_luma_samples ;
	uint32_t i_pic_height_in_luma_samples ;

	x265_window_t conformance_window ;

	uint32_t i_max_cu_width ;
	uint32_t i_max_cu_height ;
	uint32_t i_max_cu_depth ;
	uint32_t  i_add_cu_depth ;
	uint32_t i_min_tr_depth ;
	uint32_t i_max_tr_depth ;
	x265_rps_list_t rps_list ;
	int32_t b_long_term_refs_present ;
	int32_t b_tmvp_flags_present ;
	int32_t num_reorder_pics[X265_MAX_TLAYER] ;

	// _tool list
	uint32_t i_quadtree_tu_log2_max_size ;
	uint32_t i_quadtree_tu_log2_min_size ;
	uint32_t i_quadtree_tu_max_depth_inter ;
	uint32_t i_quadtree_tu_max_depth_intra ;
	int32_t b_use_pcm ;
	uint32_t i_pcm_log2_max_size ;
	uint32_t i_pcm_log2_min_size ;
	int32_t b_use_amp ;

	int32_t b_use_l_comb ;

	// _parameter
	int32_t i_bit_depth_y ;
	int32_t i_bit_depth_c ;
	int32_t i_qp_bd_offset_y ;
	int32_t i_qp_bd_offset_c ;

	int32_t b_use_lossless ;

	uint32_t i_pcm_bit_depth_luma ;
	uint32_t i_pcm_bit_depth_chroma ;
	int32_t b_pcm_filter_disable_flag ;

	uint32_t i_bits_for_poc ;
	uint32_t i_num_long_term_ref_pic_sps ;
	uint32_t lt_ref_pic_poc_lsb_sps[33] ;
	int32_t used_by_curr_pic_lt_sps_flag[33] ;
	// _max physical transform size
	uint32_t i_max_tr_size ;

	int32_t amp_acc[X265_MAX_CU_DEPTH] ;
	int32_t b_use_sao ;

	// temporal_id_nesting_flag
	int32_t b_temporal_id_nesting_flag ;

	int32_t b_scaling_list_enabled_flag ;
	int32_t b_scaling_list_present_flag ;

	uint32_t max_dec_pic_buffering[X265_MAX_TLAYER] ;
	uint32_t max_latency_increase[X265_MAX_TLAYER] ;

	int32_t b_use_df ;
	int32_t b_use_strong_intra_smoothing ;

	int32_t b_vui_parameters_present_flag ;
	x265_vui_t vui_parameters ;

	x265_ptl_t ptl ;

} x265_sps_t ;


typedef struct
{
	uint32_t b_ref_pic_list_modification_flag_l0 ;
	uint32_t b_ref_pic_list_modification_flag_l1 ;
	uint32_t ref_pic_set_idx_l0[32] ;
	uint32_t ref_pic_set_idx_l1[32] ;

} x265_ref_pic_list_modification_t ;




typedef struct
{
	// pic_parameter_set_id
	int32_t i_pps_id ;
	// seq_parameter_set_id
	int32_t i_sps_id ;
	int32_t i_pic_init_qp_minus26 ;
	int32_t b_use_dqp ;
	// constrained_intra_pred_flag
	int32_t b_constrained_intra_pred ;
	// slicelevel_chroma_qp_flag
	int32_t b_slice_chroma_qp_flag ;

	// access channel
	uint32_t i_max_cu_dqp_depth ;
	uint32_t i_min_cu_dqp_size ;

	int32_t i_chroma_cb_qp_offset ;
	int32_t i_chroma_cr_qp_offset ;

	uint32_t i_num_ref_idx_l0_default_active ;
	uint32_t i_num_ref_idx_l1_default_active ;

	// use of Weighting Prediction (P_SLICE)
	int32_t b_use_weight_pred ;
	// Use of Weighting Bi-Prediction (B_SLICE)
	int32_t b_use_weighted_bi_pred ;
	// Indicates the presence of output_flag in slice header
	int32_t b_output_flag_present_flag ;

	// Indicates presence of cu_transquant_bypass_flag in _c_us.
	int32_t b_transquant_bypass_enable_flag ;
	int32_t b_use_transform_skip ;
	//!< _indicates the presence of dependent slices
	int32_t b_dependent_slice_segments_enabled_flag ;
	//!< Indicates the presence of tiles
	int32_t b_tiles_enabled_flag ;
	//!< Indicates the presence of wavefronts
	int32_t b_entropy_coding_sync_enabled_flag ;

	int32_t b_loop_filter_across_tiles_enabled_flag ;
	int32_t i_uniform_spacing_flag ;
	int32_t i_num_columns_minus1 ;
	uint32_t * column_width ;
	int32_t i_num_rows_minus1 ;
	uint32_t * row_height ;

	int32_t i_num_substreams ;

	int32_t i_sign_hide_flag ;

	int32_t b_cabac_init_present_flag ;

	// Used to transmit table selection across slices
	uint32_t i_enc_cabac_table_idx ;

	int32_t b_slice_header_extension_present_flag ;
	int32_t b_loop_filter_across_slices_enabled_flag ;
	int32_t b_deblocking_filter_control_present_flag ;
	int32_t b_deblocking_filter_override_enabled_flag ;
	int32_t b_pic_disable_deblocking_filter_flag ;
	//< beta offset for deblocking filter
	int32_t i_deblocking_filter_beta_offset_div2 ;
	//< tc offset for deblocking filter
	int32_t i_deblocking_filter_tc_offset_div2 ;
	int32_t b_scaling_list_present_flag ;
	int32_t b_lists_modification_present_flag ;
	uint32_t i_log2_parallel_merge_level_minus2 ;
	int32_t i_num_extra_slice_header_bits ;



} x265_pps_t ;


typedef struct
{
	// Explicit weighted prediction parameters parsed in slice header,
	// or Implicit weighted prediction parameters (8 bits depth values).
	int32_t b_present_flag ;
	uint32_t i_log2_weight_denom ;
	int32_t i_weight ;
	int32_t i_offset ;

	// Weighted prediction scaling values built from above parameters (bitdepth scaled):
	int32_t w, o, offset, shift, i_round ;
} x265_wp_scaling_param_t ;

typedef struct
{
	int64_t i_ac ;
	int64_t i_dc ;
} x265_wp_ac_dc_param_t ;


void print_rps ( x265_reference_picture_set_t * rps, FILE *file ) ;
void sort_delta_poc ( x265_reference_picture_set_t *reference_picture_set ) ;

int init_scaling_list ( x265_scaling_list_t *scaling_list ) ;
void deinit_scaling_list ( x265_scaling_list_t *scaling_list ) ;

void init_vps ( x265_vps_t *vps ) ;
void deinit_vps ( x265_vps_t *vps ) ;

int init_sps ( x265_sps_t *sps ) ;
void deinit_sps ( x265_sps_t *sps ) ;

int init_pps ( x265_pps_t *pps ) ;
void deinit_pps ( x265_pps_t *pps ) ;

void init_ref_pic_list_modification ( x265_ref_pic_list_modification_t *ref_pic_list_modification ) ;
void print_ref_pic_list_modification ( x265_ref_pic_list_modification_t *ref_pic_list_modification,
										FILE *file ) ;


int create_hrd_param_buffer( x265_vps_t *vps ) ;

void set_window ( x265_window_t *window,
				int32_t i_offset_left,
				int32_t i_offset_right,
				int32_t i_offset_top,
				int32_t i_offset_bottom ) ;
void sps_create_rps_list ( x265_sps_t *sps, int i_num_rps ) ;
void init_reference_picture_set ( x265_reference_picture_set_t *reference_picture_set ) ;

void process_default_marix ( x265_scaling_list_t *scaling_list,
							uint32_t i_size_id,
							uint32_t i_list_id) ;
int parse_scaling_list ( x265_scaling_list_t *scaling_list, const char *filename) ;
int check_default_scaling_list ( x265_scaling_list_t *scaling_list ) ;
void check_dc_of_matrix ( x265_scaling_list_t *scaling_list ) ;
int check_pred_mode ( x265_scaling_list_t *scaling_list,
					uint32_t i_size_id, uint32_t i_list_id ) ;

int set_column_width ( x265_pps_t *pps, uint32_t* column_width ) ;
int set_row_height ( x265_pps_t *pps, uint32_t* row_height ) ;
int x265_scaling_list_get_coef_num ( uint32_t i_size_id ) ;
int x265_scaling_list_get_scaling_list_num ( uint32_t i_size_id ) ;
int x265_scaling_list_get_scaling_list_size ( uint32_t i_size_id ) ;
int x265_scaling_list_get_scaling_list_size_x ( uint32_t i_size_id ) ;

const int32_t *get_scaling_list_default_address ( uint32_t i_size_id, uint32_t i_list_id ) ;

#endif


