

#ifndef X265_ENCODER_CAVLC_H
#define X265_ENCODER_CAVLC_H


typedef struct
{
	X265_ENC_ENTROPY_IF_FUNC ;
	x265_bit_if_t *bit_if ;
	x265_slice_t *slice;
	//SBAC RD
	uint32_t i_coeff_cost ;


} x265_enc_cavlc_t ;



x265_enc_entropy_if_t *x265_enc_cavlc_new () ;
void x265_enc_cavlc_delete ( x265_enc_entropy_if_t *enc_entropy_if ) ;
int x265_enc_cavlc_init ( x265_enc_entropy_if_t *enc_entropy_if ) ;
void x265_enc_cavlc_deinit ( x265_enc_entropy_if_t *enc_entropy_if ) ;


void x265_enc_cavlc_x_write_pcm_align_zero(x265_enc_entropy_if_t *enc_entropy_if ) ;
void x265_enc_cavlc_x_write_ep_ex_golomb(x265_enc_entropy_if_t *enc_entropy_if,
										uint32_t i_symbol,
										uint32_t i_count ) ;
void x265_enc_cavlc_x_write_ex_golomb_level(x265_enc_entropy_if_t *enc_entropy_if,
											uint32_t i_symbol ) ;
void x265_enc_cavlc_x_write_unary_max_symbol(x265_enc_entropy_if_t *enc_entropy_if,
											uint32_t i_symbol,
											uint32_t i_max_symbol ) ;

void x265_enc_cavlc_code_short_term_ref_pic_set ( x265_enc_entropy_if_t *enc_entropy_if,
												x265_reference_picture_set_t* rps,
												int32_t b_called_from_slice_header,
												int32_t i_idx) ;
int x265_enc_cavlc_find_matching_ltrp ( x265_t *h,
										x265_slice_t *slice,
										uint32_t *ltrps_index,
										int32_t ltrp_poc,
										int32_t b_used_flag ) ;

void x265_enc_cavlc_reset_entropy ( x265_t *h,
									x265_enc_entropy_if_t *enc_entropy_if,
									enum slice_type_e i_slice_type,
									int32_t i_slice_qp ) ;
void x265_enc_cavlc_print_cabac_state ( x265_enc_entropy_if_t *enc_entropy_if ) ;
void x265_enc_cavlc_determine_cabac_init_idx ( x265_t *h,
												x265_enc_entropy_if_t *enc_entropy_if,
												enum slice_type_e i_slice_type,
												int32_t i_slice_qp ) ;

void x265_enc_cavlc_set_bitstream ( x265_enc_entropy_if_t *enc_entropy_if, x265_bit_if_t* bit_if ) ;
void x265_enc_cavlc_set_slice ( x265_enc_entropy_if_t *enc_entropy_if, x265_slice_t* slice ) ;

void x265_enc_cavlc_reset_bits ( x265_enc_entropy_if_t *enc_entropy_if ) ;
void x265_enc_cavlc_clear_bits ( x265_enc_entropy_if_t *enc_entropy_if ) ;
void x265_enc_cavlc_reset_coeff_cost ( x265_enc_entropy_if_t *enc_entropy_if ) ;

uint32_t x265_enc_cavlc_get_number_of_written_bits ( x265_enc_entropy_if_t *enc_entropy_if ) ;
uint32_t x265_enc_cavlc_get_coeff_cost ( x265_enc_entropy_if_t *enc_entropy_if ) ;


void x265_enc_cavlc_code_vps( x265_enc_entropy_if_t *enc_entropy_if, x265_vps_t *vps ) ;
void x265_enc_cavlc_code_vui ( x265_enc_entropy_if_t *enc_entropy_if, x265_vui_t *vui, x265_sps_t *sps ) ;
void x265_enc_cavlc_code_sps( x265_t *h, x265_enc_entropy_if_t *enc_entropy_if, x265_sps_t *sps ) ;
void x265_enc_cavlc_code_pps ( x265_t *h, x265_enc_entropy_if_t *enc_entropy_if, x265_pps_t *pps ) ;
void x265_enc_cavlc_code_slice_header ( x265_t *h, x265_enc_entropy_if_t *enc_entropy_if, x265_slice_t *slice ) ;
void x265_enc_cavlc_code_ptl ( x265_enc_entropy_if_t *enc_entropy_if,
								x265_ptl_t *ptl,
								int32_t b_profile_present_flag,
								int32_t i_max_num_sub_layers_minus1 ) ;
void x265_enc_cavlc_code_profile_tier_level ( x265_enc_entropy_if_t *enc_entropy_if,
											x265_profile_tier_level_t *profile_tier_level ) ;
#if X265_SIGNAL_BITRATE_PICRATE_IN_VPS
void x265_enc_cavlc_bit_rate_pic_rate_info_write ( x265_enc_entropy_if_t *enc_entropy_if,
													x265_bit_rate_pic_rate_info_t *bit_rate_pic_rate_info,
													int i_temp_level_low, int i_temp_level_high ) ;
#endif
void x265_enc_cavlc_code_hrd_parameters ( x265_enc_entropy_if_t *enc_entropy_if,
										x265_hrd_t *hrd,
										int32_t b_common_inf_present_flag,
										uint32_t i_max_num_sub_layers_minus1 ) ;
int x265_enc_cavlc_code_tiles_wpp_entry_point( x265_t *h, x265_enc_entropy_if_t *enc_entropy_if,
												bs_t *s, x265_slice_t *slice ) ;

void x265_enc_cavlc_code_terminating_bit (x265_enc_entropy_if_t *enc_entropy_if,
										uint32_t i_ls_last ) ;
void x265_enc_cavlc_code_slice_finish (x265_enc_entropy_if_t *enc_entropy_if ) ;


void x265_enc_cavlc_code_mvp_idx (x265_enc_entropy_if_t *enc_entropy_if,
								x265_data_cu_t *cu,
								uint32_t i_abs_part_idx,
								enum ref_pic_list_e i_ref_list ) ;
void x265_enc_cavlc_code_sao_sign( x265_enc_entropy_if_t* enc_entropy_if,
									uint32_t code ) ;

void x265_enc_cavlc_code_sao_max_uvlc( x265_enc_entropy_if_t* enc_entropy_if,
									uint32_t code,
									uint32_t max_symbol ) ;
void x265_enc_cavlc_code_sao_merge ( x265_enc_entropy_if_t* enc_entropy_if,
									uint32_t code ) ;
void x265_enc_cavlc_code_sao_type_idx ( x265_enc_entropy_if_t* enc_entropy_if,
										uint32_t code) ;
void x265_enc_cavlc_code_sao_uflc ( x265_enc_entropy_if_t* enc_entropy_if,
									uint32_t length,
									uint32_t code ) ;


void x265_enc_cavlc_code_cu_transquant_bypass_flag(x265_enc_entropy_if_t *enc_entropy_if,
												x265_data_cu_t *cu,
												uint32_t i_abs_part_idx ) ;
void x265_enc_cavlc_code_skip_flag(x265_t* h,
									x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu, uint32_t
									i_abs_part_idx ) ;

void x265_enc_cavlc_code_merge_flag(x265_t* h,
									x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx ) ;
void x265_enc_cavlc_code_merge_index(x265_t* h,
									x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx ) ;

void x265_enc_cavlc_code_inter_mode_flag(x265_enc_entropy_if_t *enc_entropy_if,
										x265_data_cu_t *cu,
										uint32_t i_abs_part_idx,
										uint32_t i_depth,
										uint32_t i_enc_mode ) ;
void x265_enc_cavlc_code_split_flag(x265_t* h,
									x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx,
									uint32_t i_depth ) ;

void x265_enc_cavlc_code_part_size(x265_t* h,
									x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx,
									uint32_t i_depth ) ;
void x265_enc_cavlc_code_pred_mode(x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx ) ;

void x265_enc_cavlc_code_ipcm_info(x265_t* h,
									x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx ) ;

void x265_enc_cavlc_code_transform_sub_div_flag(x265_t* h,
												x265_enc_entropy_if_t *enc_entropy_if,
												uint32_t i_symbol,
												uint32_t i_ctx ) ;


void x265_enc_cavlc_code_qt_cbf(x265_t* h,
								x265_enc_entropy_if_t *enc_entropy_if,
								x265_data_cu_t *cu,
								uint32_t i_abs_part_idx,
								enum text_type_e i_text_type,
								uint32_t i_tr_depth ) ;
void x265_enc_cavlc_code_qt_root_cbf(x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx ) ;
void x265_enc_cavlc_code_qt_cbf_zero(x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									enum text_type_e i_text_type,
									uint32_t i_tr_depth ) ;
void x265_enc_cavlc_code_qt_root_cbf_zero(x265_enc_entropy_if_t *enc_entropy_if,
										x265_data_cu_t *cu ) ;
void x265_enc_cavlc_code_intra_dir_luma_ang(x265_t* h,
											x265_enc_entropy_if_t *enc_entropy_if,
											x265_data_cu_t *cu,
											uint32_t i_abs_part_idx,
											int32_t b_is_multiple) ;
void x265_enc_cavlc_code_intra_dir_chroma(x265_enc_entropy_if_t *enc_entropy_if,
											x265_data_cu_t *cu,
											uint32_t i_abs_part_idx ) ;
void x265_enc_cavlc_code_inter_dir(x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx ) ;
void x265_enc_cavlc_code_ref_frm_idx(x265_t* h,
									x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx,
									enum ref_pic_list_e i_ref_pic_list ) ;
void x265_enc_cavlc_code_mvd(x265_t* h,
							x265_enc_entropy_if_t *enc_entropy_if,
							x265_data_cu_t *cu,
							uint32_t i_abs_part_idx,
							enum ref_pic_list_e i_ref_pic_list ) ;
void x265_enc_cavlc_code_delta_qp ( x265_t *h,
									x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx ) ;
void x265_enc_cavlc_code_coeff_nxn( x265_t* h,
									x265_enc_entropy_if_t* enc_entropy_if,
									x265_data_cu_t* cu,
									x265_coeff_t* coef,
									uint32_t i_abs_part_idx,
									uint32_t i_width,
									uint32_t i_height,
									uint32_t depth,
									enum text_type_e i_text_type ) ;
void x265_enc_cavlc_code_transform_skip_flags(x265_t* h,
											x265_enc_entropy_if_t *enc_entropy_if,
											x265_data_cu_t *cu,
											uint32_t i_abs_part_idx,
											uint32_t width,
											uint32_t height,
											enum text_type_e i_text_type ) ;

void x265_enc_cavlc_est_bit(x265_t *h,
							x265_enc_entropy_if_t *enc_entropy_if,
							x265_est_bits_sbac_struct_t* est_bits_sbac_struct,
							int32_t i_width,
							int32_t i_height,
							enum text_type_e i_text_type ) ;
void x265_enc_cavlc_x_code_pred_weight_table ( x265_t *h,
											x265_enc_entropy_if_t *enc_entropy_if,
											x265_slice_t *slice ) ;

void x265_enc_cavlc_update_context_tables_p4 ( x265_enc_entropy_if_t *enc_entropy_if,
												enum slice_type_e i_slice_type,
												int32_t i_qp,
												int32_t b_execute_finish ) ;
void x265_enc_cavlc_update_context_tables_p3 ( x265_enc_entropy_if_t *enc_entropy_if,
												enum slice_type_e i_slice_type,
												int32_t i_qp ) ;

void x265_enc_cavlc_code_scaling_list ( x265_t *h,
										x265_enc_entropy_if_t *enc_entropy_if,
										x265_scaling_list_t *scaling_list ) ;
void x265_enc_cavlc_x_code_scaling_list_write ( x265_t *h,
												x265_enc_entropy_if_t *enc_entropy_if,
												x265_scaling_list_t *scaling_list,
												uint32_t i_size_id,
												uint32_t i_list_id) ;

void x265_enc_cavlc_code_df_flag ( x265_enc_entropy_if_t *enc_entropy_if, uint32_t i_code ) ;
void x265_enc_cavlc_code_df_svlc ( x265_enc_entropy_if_t *enc_entropy_if, int32_t i_code ) ;

int x265_enc_cavlc_scaling_list_check_pred_mode ( x265_scaling_list_t *scaling_list,
												uint32_t i_size_id,
												uint32_t i_list_id ) ;

void x265_enc_cavlc_write ( x265_enc_cavlc_t *enc_cavlc, int i_count, uint32_t i_bits ) ;
void x265_enc_cavlc_write1 ( x265_enc_cavlc_t *enc_cavlc, uint32_t i_bits ) ;
void x265_enc_cavlc_write_ue ( x265_enc_cavlc_t *enc_cavlc, uint32_t val ) ;
void x265_enc_cavlc_write_ue_big( x265_enc_cavlc_t *enc_cavlc, unsigned int val ) ;
void x265_enc_cavlc_write_se ( x265_enc_cavlc_t *enc_cavlc, uint32_t val ) ;






















#endif



