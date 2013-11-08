

#ifndef X265_ENCODER_SBAC_H
#define X265_ENCODER_SBAC_H

typedef struct
{
	X265_ENC_ENTROPY_IF_FUNC ;
	x265_bit_if_t *bit_if ;
	x265_slice_t *slice;
	x265_enc_bin_if_t * enc_bin_if ;
	//SBAC RD
	uint32_t i_coeff_cost ;
	uint32_t i_last_qp ;

} x265_enc_sbac_t ;

extern const uint32_t group_idx[32] ;

x265_enc_entropy_if_t *x265_enc_sbac_new () ;
void x265_enc_sbac_delete ( x265_enc_entropy_if_t *enc_entropy_if ) ;
int x265_enc_sbac_init ( x265_enc_entropy_if_t *enc_entropy_if ) ;
void x265_enc_sbac_deinit ( x265_enc_entropy_if_t *enc_entropy_if ) ;

void enc_sbac_init ( x265_enc_sbac_t *enc_sbac, x265_enc_bin_if_t *enc_bin_if ) ;
void enc_sbac_uninit ( x265_enc_entropy_if_t *enc_entropy_if ) ;

void x265_enc_sbac_reset_entropy ( x265_t *h,
									x265_enc_entropy_if_t *enc_entropy_if,
									enum slice_type_e i_slice_type,
									int32_t i_slice_qp ) ;
void x265_enc_sbac_print_cabac_state ( x265_enc_entropy_if_t *enc_entropy_if ) ;

void x265_enc_sbac_determine_cabac_init_idx ( x265_t *h,
												x265_enc_entropy_if_t *enc_entropy_if,
												enum slice_type_e i_slice_type,
												int32_t i_slice_qp ) ;

void x265_enc_sbac_set_bitstream ( x265_enc_entropy_if_t *enc_entropy_if, x265_bit_if_t* bit_if ) ;
void x265_enc_sbac_store_bitstream ( x265_enc_sbac_t *enc_sbac, x265_enc_sbac_t *dst_enc_sbac ) ;
void x265_enc_sbac_set_slice ( x265_enc_entropy_if_t *enc_entropy_if, x265_slice_t* slice ) ;
void x265_enc_sbac_reset_coeff_cost ( x265_enc_entropy_if_t *enc_entropy_if ) ;
uint32_t x265_enc_sbac_get_coeff_cost ( x265_enc_entropy_if_t *enc_entropy_if ) ;


void x265_enc_sbac_load ( x265_enc_sbac_t *enc_sbac, x265_enc_sbac_t *src_enc_sbac ) ;
void x265_enc_sbac_load_intra_dir_mode_luma( x265_enc_sbac_t* enc_sbac, x265_enc_sbac_t* src_enc_sbac ) ;
void x265_enc_sbac_store ( x265_enc_sbac_t *enc_sbac, x265_enc_sbac_t *dst_enc_sbac ) ;
void x265_enc_sbac_load_contexts ( x265_enc_sbac_t* enc_sbac, x265_enc_sbac_t* src_enc_sbac) ;

void x265_enc_sbac_reset_bits ( x265_enc_entropy_if_t *enc_entropy_if ) ;
void x265_enc_sbac_clear_bits ( x265_enc_entropy_if_t *enc_entropy_if ) ;
uint32_t x265_enc_sbac_get_number_of_written_bits ( x265_enc_entropy_if_t *enc_entropy_if ) ;


void x265_enc_sbac_code_vps( x265_enc_entropy_if_t *enc_entropy_if, x265_vps_t *vps ) ;
void x265_enc_sbac_code_sps( x265_t *h, x265_enc_entropy_if_t *enc_entropy_if, x265_sps_t *sps ) ;
void x265_enc_sbac_code_pps ( x265_t *h, x265_enc_entropy_if_t *enc_entropy_if, x265_pps_t *pps ) ;
void x265_enc_sbac_code_slice_header ( x265_t *h, x265_enc_entropy_if_t *enc_entropy_if, x265_slice_t *slice ) ;
int x265_enc_sbac_code_tiles_wpp_entry_point( x265_t *h, x265_enc_entropy_if_t *enc_entropy_if,
												bs_t *s, x265_slice_t *slice ) ;
void x265_enc_sbac_code_terminating_bit (x265_enc_entropy_if_t *enc_entropy_if,
										uint32_t i_ls_last ) ;
void x265_enc_sbac_code_slice_finish (x265_enc_entropy_if_t *enc_entropy_if ) ;


void x265_enc_sbac_code_sao_max_uvlc( x265_enc_entropy_if_t* enc_entropy_if,
									uint32_t code,
									uint32_t max_symbol ) ;
void x265_enc_sbac_code_sao_merge ( x265_enc_entropy_if_t* enc_entropy_if,
									uint32_t code ) ;
void x265_enc_sbac_code_sao_type_idx ( x265_enc_entropy_if_t* enc_entropy_if,
										uint32_t code) ;
void x265_enc_sbac_code_sao_uflc ( x265_enc_entropy_if_t* enc_entropy_if,
									uint32_t length,
									uint32_t code ) ;
void x265_enc_sbac_code_sao_sign( x265_enc_entropy_if_t* enc_entropy_if,
									uint32_t code ) ;
void x265_enc_sbac_code_scaling_list ( x265_t *h,
										x265_enc_entropy_if_t *enc_entropy_if,
										x265_scaling_list_t *scaling_list ) ;

void x265_enc_sbac_x_write_unary_symbol( x265_enc_sbac_t* enc_sbac, uint32_t symbol, uint32_t i_sc_model_start, int32_t i_offset ) ;
void x265_enc_sbac_x_write_unary_max_symbol( x265_enc_sbac_t* enc_sbac, uint32_t symbol, uint32_t i_sc_model_start, int32_t i_offset, uint32_t max_symbol ) ;
void x265_enc_sbac_x_write_ep_ex_golomb( x265_enc_sbac_t* enc_sbac, uint32_t symbol, uint32_t count ) ;
void x265_enc_sbac_x_write_coef_remain_ex_golomb ( x265_enc_sbac_t* enc_sbac, uint32_t symbol, uint32_t i_r_param ) ;
void x265_enc_sbac_x_write_terminating_bit ( x265_enc_sbac_t* enc_sbac, uint32_t i_bit ) ;

void x265_enc_sbac_x_copy_from  ( x265_enc_sbac_t *enc_sbac, x265_enc_sbac_t *src_enc_sbac ) ;
void x265_enc_sbac_x_copy_contexts_from( x265_enc_sbac_t* enc_sbac, x265_enc_sbac_t* src_enc_sbac ) ;

void x265_enc_sbac_code_df_flag ( x265_enc_entropy_if_t *enc_entropy_if, uint32_t i_code ) ;
void x265_enc_sbac_code_df_svlc ( x265_enc_entropy_if_t *enc_entropy_if, int32_t i_code ) ;


void x265_enc_sbac_code_cu_transquant_bypass_flag(x265_enc_entropy_if_t *enc_entropy_if,
												x265_data_cu_t *cu,
												uint32_t i_abs_part_idx ) ;
void x265_enc_sbac_code_skip_flag(x265_t* h,
									x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu, uint32_t
									i_abs_part_idx ) ;

void x265_enc_sbac_code_merge_flag(x265_t* h,
									x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx ) ;
void x265_enc_sbac_code_merge_index(x265_t* h,
									x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx ) ;
void x265_enc_sbac_code_split_flag(x265_t* h,
									x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx,
									uint32_t i_depth ) ;
void x265_enc_sbac_code_mvp_idx (x265_enc_entropy_if_t *enc_entropy_if,
								x265_data_cu_t *cu,
								uint32_t i_abs_part_idx,
								enum ref_pic_list_e i_ref_list ) ;


void x265_enc_sbac_code_part_size(x265_t* h,
									x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx,
									uint32_t i_depth ) ;
void x265_enc_sbac_code_pred_mode(x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx ) ;

void x265_enc_sbac_code_ipcm_info(x265_t* h,
									x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx ) ;

void x265_enc_sbac_code_transform_sub_div_flag(x265_t* h,
												x265_enc_entropy_if_t *enc_entropy_if,
												uint32_t i_symbol,
												uint32_t i_ctx ) ;


void x265_enc_sbac_code_qt_cbf(x265_t* h,
								x265_enc_entropy_if_t *enc_entropy_if,
								x265_data_cu_t *cu,
								uint32_t i_abs_part_idx,
								enum text_type_e i_text_type,
								uint32_t i_tr_depth ) ;
void x265_enc_sbac_code_qt_root_cbf(x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx ) ;
void x265_enc_sbac_code_qt_cbf_zero(x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									enum text_type_e i_text_type,
									uint32_t i_tr_depth ) ;
void x265_enc_sbac_code_qt_root_cbf_zero(x265_enc_entropy_if_t *enc_entropy_if,
										x265_data_cu_t *cu ) ;
void x265_enc_sbac_code_intra_dir_luma_ang(x265_t* h,
											x265_enc_entropy_if_t *enc_entropy_if,
											x265_data_cu_t *cu,
											uint32_t i_abs_part_idx,
											int32_t b_is_multiple) ;
void x265_enc_sbac_code_intra_dir_chroma(x265_enc_entropy_if_t *enc_entropy_if,
											x265_data_cu_t *cu,
											uint32_t i_abs_part_idx ) ;
void x265_enc_sbac_code_inter_dir(x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx ) ;
void x265_enc_sbac_code_ref_frm_idx(x265_t* h,
									x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx,
									enum ref_pic_list_e i_ref_pic_list ) ;
void x265_enc_sbac_code_mvd(x265_t* h,
							x265_enc_entropy_if_t *enc_entropy_if,
							x265_data_cu_t *cu,
							uint32_t i_abs_part_idx,
							enum ref_pic_list_e i_ref_pic_list ) ;
void x265_enc_sbac_code_delta_qp ( x265_t *h,
									x265_enc_entropy_if_t *enc_entropy_if,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx ) ;

void x265_enc_sbac_code_last_significant_xy( x265_t* h,
											x265_enc_entropy_if_t* enc_entropy_if,
											uint32_t i_pos_x,
											uint32_t i_pos_y,
											int32_t i_width,
											int32_t i_height,
											enum text_type_e i_text_type,
											uint32_t i_scan_idx ) ;
void x265_enc_sbac_code_coeff_nxn( x265_t* h,
									x265_enc_entropy_if_t* enc_entropy_if,
									x265_data_cu_t* cu,
									x265_coeff_t* coef,
									uint32_t i_abs_part_idx,
									uint32_t i_width,
									uint32_t i_height,
									uint32_t depth,
									enum text_type_e i_text_type ) ;
void x265_enc_sbac_code_transform_skip_flags(x265_t* h,
											x265_enc_entropy_if_t *enc_entropy_if,
											x265_data_cu_t *cu,
											uint32_t i_abs_part_idx,
											uint32_t width,
											uint32_t height,
											enum text_type_e i_text_type ) ;


void x265_enc_sbac_est_bit( x265_t *h,
							x265_enc_entropy_if_t *enc_entropy_if,
							x265_est_bits_sbac_struct_t* est_bits_sbac,
							int32_t i_width,
							int32_t i_height,
							enum text_type_e i_text_type ) ;
void x265_enc_sbac_est_cbf_bit( x265_enc_sbac_t* enc_sbac,
								x265_est_bits_sbac_struct_t* est_bits_sbac ) ;
void x265_enc_sbac_est_significant_coeff_group_map_bit( x265_enc_sbac_t* enc_sbac,
														x265_est_bits_sbac_struct_t* est_bits_sbac,
														enum text_type_e i_text_type ) ;
void x265_enc_sbac_est_significant_map_bit( x265_t* h,
											x265_enc_sbac_t* enc_sbac,
											x265_est_bits_sbac_struct_t* est_bits_sbac,
											int32_t i_width,
											int32_t i_height,
											enum text_type_e i_text_type ) ;
void x265_enc_sbac_est_significant_coefficients_bit( x265_enc_sbac_t* enc_sbac,
													x265_est_bits_sbac_struct_t* est_bits_sbac,
													enum text_type_e i_text_type ) ;

void x265_enc_sbac_update_context_tables_p4( x265_enc_entropy_if_t *enc_entropy_if,
											enum slice_type_e i_slice_type,
											int32_t i_slice_qp,
											int32_t b_execute_finish ) ;
void x265_enc_sbac_update_context_tables_p3( x265_enc_entropy_if_t *enc_entropy_if,
											enum slice_type_e i_slice_type,
											int32_t i_slice_qp ) ;

x265_enc_bin_if_t *x265_enc_sbac_get_enc_bin_if ( x265_enc_sbac_t *enc_sbac ) ;

void x265_enc_sbac_swap_int32 ( int32_t *p_value1, int32_t *p_value2 ) ;
void x265_enc_sbac_swap_uint32 ( uint32_t *p_value1, uint32_t *p_value2 ) ;
















#endif



