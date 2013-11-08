

#ifndef X265_SLICE_H
#define X265_SLICE_H

typedef struct
{
	int32_t b_sao_enabled_flag  ;
	int32_t b_sao_enabled_flag_chroma  ;
	int32_t i_pps_id  ;
	int32_t b_pic_output_flag  ;
	int32_t i_last_idr  ;
	x265_reference_picture_set_t *rps ;
	x265_reference_picture_set_t local_rps ;

	int32_t i_rps_idx ;
	enum slice_type_e i_slice_type ;
	enum nal_unit_type_e i_naul_type ;
	int32_t i_slice_qp  ;
	int32_t b_dependent_slice_segment_flag  ;
#if X265_ADAPTIVE_QP_SELECTION
	int32_t i_slice_qp_base  ;
#endif
	int32_t b_deblocking_filter_disable  ;
	int32_t b_deblocking_filter_override_flag  ;
	int32_t i_deblocking_filter_beta_offset_div2  ;
	int32_t i_deblocking_filter_tc_offset_div2  ;

	int32_t b_check_ldc  ;

	//  _data
	int32_t i_slice_qp_delta  ;
	int32_t i_slice_qp_delta_cb  ;
	int32_t i_slice_qp_delta_cr  ;
	int32_t i_depth  ;

	uint32_t i_col_from_l0_flag  ;

	uint32_t i_col_ref_idx  ;
	uint32_t i_max_num_merge_cand  ;


#if X265_SAO_CHROMA_LAMBDA
	double      f_lambda_luma  ;
	double      f_lambda_chroma  ;
#else
	double      f_lambda  ;
#endif

	int32_t equal_ref  [2][X265_MAX_NUM_REF][X265_MAX_NUM_REF]  ;

	int32_t b_no_back_pred_flag  ;
	int32_t b_t_layer_switching_flag  ;

	uint32_t i_slice_mode  ;
	uint32_t i_slice_argument  ;
	uint32_t i_slice_cur_start_cu_addr  ;
	uint32_t i_slice_cur_end_cu_addr  ;
	uint32_t i_slice_idx  ;
	uint32_t i_slice_segment_mode  ;
	uint32_t i_slice_segment_argument  ;
	uint32_t i_slice_segment_cur_start_cu_addr  ;
	uint32_t i_slice_segment_cur_end_cu_addr  ;
	int32_t b_next_slice  ;
	int32_t b_next_slice_segment  ;
	uint32_t i_slice_bits  ;
	uint32_t i_slice_segment_bits  ;
	int32_t b_finalized  ;

	x265_wp_scaling_param_t weight_pred_table[2][X265_MAX_NUM_REF][3]  ;
	x265_wp_ac_dc_param_t weight_ac_dc_param[3]  ;

	x265_uint_list_t *tile_byte_location ;
	uint32_t i_tile_offst_for_mult_es  ;

	uint32_t *substream_sizes  ;
	x265_scaling_list_t * scaling_list  ;
	int32_t b_cabac_init_flag  ;

	int32_t b_l_mvd_l1_zero  ;
	int32_t i_num_entry_point_offsets  ;
	int32_t b_lf_cross_slice_boundary_flag  ;

	int32_t b_enable_tmvp_flag  ;
} x265_slice_t ;


x265_slice_t *x265_slice_new ( x265_t *h ) ;
void x265_slice_delete ( x265_slice_t *slice ) ;
int x265_slice_init ( x265_t *h, x265_slice_t *slice ) ;
void x265_slice_deinit ( x265_slice_t *slice ) ;
int x265_slice_create ( x265_slice_t *slice ) ;
void x265_slice_destroy ( x265_slice_t *slice ) ;

int x265_slice_initialize ( x265_slice_t *slice ) ;
void x265_slice_deinitialize ( x265_slice_t *slice ) ;

void x265_slice_init_equal_ref ( x265_slice_t *slice ) ;

void x265_slice_decoding_refresh_marking ( x265_t *h, x265_slice_t *slice,
								int32_t * p_poc_cra, int32_t *p_refresh_pending ) ;
void x265_slice_select_reference_picture_set ( x265_t *h, x265_slice_t* slice,
								int32_t i_poc_curr, int32_t i_gop_id ) ;
int x265_slice_check_that_all_ref_pics_are_available ( x265_t *h, x265_slice_t *slice,
											x265_reference_picture_set_t *rps,
											int32_t b_print_errors,
											int32_t i_poc_random_access ) ;
void x265_slice_create_explicit_reference_picture_set_from_reference (
		x265_t *h, x265_slice_t *slice,
		x265_reference_picture_set_t *reference_picture_set ) ;
void x265_slice_apply_reference_picture_set ( x265_t *h, x265_slice_t *slice,
								x265_reference_picture_set_t *rps ) ;

int32_t x265_slice_is_temporal_layer_switching_point ( x265_t *h, x265_slice_t *slice ) ;
int32_t x265_slice_is_stepwise_temporal_layer_switching_point_candidate ( x265_t *h, x265_slice_t *slice ) ;
void x265_slice_arrange_longterm_pictures_in_rps ( x265_t *h, x265_slice_t *slice ) ;
void x265_slice_set_ref_pic_list ( x265_t *h, x265_slice_t *slice ) ;
void x265_slice_set_ref_poc_list ( x265_t *h ) ;
void x265_slice_generate_combined_list ( x265_t *h ) ;


#if X265_SAO_CHROMA_LAMBDA

void x265_slice_set_lambda ( x265_slice_t *slice, double f_lambda_luma, double f_lambda_chroma ) ;

#else

void x265_slice_setLambda ( x265_slice_t* slice, double f_lambda ) ;

#endif

int x265_slice_alloc_substream_sizes ( x265_slice_t *slice, uint32_t i_num_substreams ) ;
void x265_slice_copy_slice_info ( x265_slice_t *slice, x265_slice_t *slice_src ) ;
void x265_slice_set_tile_location ( x265_slice_t *slice, int32_t i_idx, uint32_t i_location ) ;
int32_t x265_slice_set_get_rap_pic_flag ( x265_slice_t *slice ) ;
int32_t x265_slice_get_idr_pic_flag ( x265_slice_t *slice ) ;
int32_t x265_slice_get_num_rps_curr_temp_list ( x265_slice_t *slice ) ;
uint32_t x265_slice_get_tile_location_count ( x265_slice_t *slice ) ;
uint32_t x265_slice_get_tile_location ( x265_slice_t *slice, int32_t i_idx ) ;

int32_t x265_slice_is_equal_ref ( x265_slice_t *slice,
									enum ref_pic_list_e i_ref_pic_list,
									int32_t i_ref_idx1,
									int32_t i_ref_idx2 ) ;

void x265_slice_swap_i ( int32_t *i, int32_t *j ) ;

void x265_slice_print ( x265_t *h, x265_slice_t *slice ) ;


#endif

