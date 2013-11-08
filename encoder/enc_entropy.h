



#ifndef X265_ENCODER_ENTROPY_H
#define X265_ENCODER_ENTROPY_H

typedef struct _x265_enc_entropy_t x265_enc_entropy_t ;

#include "enc_entropy_if.h"
#include "enc_cavlc.h"
#include "enc_sbac.h"

struct _x265_enc_entropy_t
{
	uint32_t i_bak_abs_part_idx ;
	uint32_t i_bak_chroma_offset ;
	uint32_t i_bak_abs_part_idx_cu ;
	x265_enc_entropy_if_t *enc_entropy_if ;

} ;


x265_enc_entropy_t *x265_enc_entropy_new () ;
void x265_enc_entropy_delete ( x265_enc_entropy_t *enc_entropy ) ;
int x265_enc_entropy_init ( x265_enc_entropy_t *enc_entropy ) ;
void x265_enc_entropy_deinit ( x265_enc_entropy_t *enc_entropy ) ;


void x265_enc_entropy_set_entropy_coder ( x265_enc_entropy_t *enc_entropy,
										x265_enc_entropy_if_t *e,
										x265_slice_t *slice ) ;
void x265_enc_entropy_set_bitstream ( x265_enc_entropy_t *enc_entropy,
										x265_bit_if_t *bit_if ) ;
void x265_enc_entropy_reset_bits ( x265_enc_entropy_t *enc_entropy ) ;
void x265_enc_entropy_reset_coeff_cost ( x265_enc_entropy_t *enc_entropy ) ;
uint32_t x265_enc_entropy_get_number_of_written_bits ( x265_enc_entropy_t *enc_entropy );
uint32_t x265_enc_entropy_get_coeff_cost ( x265_enc_entropy_t *enc_entropy );
void x265_enc_entropy_reset_entropy ( x265_t *h,
									x265_enc_entropy_t *enc_entropy,
									enum slice_type_e i_slice_type,
									int32_t i_slice_qp ) ;
void x265_enc_entropy_print_cabac_state ( x265_enc_entropy_t *enc_entropy ) ;

void x265_enc_entropy_determine_cabac_init_idx ( x265_t *h,
												x265_enc_entropy_t *enc_entropy,
												enum slice_type_e i_slice_type,
												int32_t i_slice_qp ) ;

void x265_enc_entropy_encode_slice_header ( x265_t *h, x265_enc_entropy_t *enc_entropy, x265_slice_t *slice ) ;
int x265_enc_entropy_encode_tiles_wpp_entry_point( x265_t *h,
													x265_enc_entropy_t *enc_entropy,
													bs_t *s,
													x265_slice_t *slice ) ;

void x265_enc_entropy_encode_terminating_bit (x265_enc_entropy_t *enc_entropy,
										uint32_t i_ls_last ) ;
void x265_enc_entropy_encode_slice_finish (x265_enc_entropy_t *enc_entropy ) ;
void x265_enc_entropy_encode_vps( x265_enc_entropy_t *enc_entropy, x265_vps_t *vps ) ;
void x265_enc_entropy_encode_sps( x265_t *h, x265_enc_entropy_t *enc_entropy, x265_sps_t *sps ) ;
void x265_enc_entropy_encode_pps ( x265_t *h, x265_enc_entropy_t *enc_entropy, x265_pps_t *pps ) ;


void x265_enc_entropy_encode_split_flag(x265_t* h,
										x265_enc_entropy_t *enc_entropy,
										x265_data_cu_t *cu,
										uint32_t i_abs_part_idx,
										uint32_t i_depth,
										int32_t b_rd ) ;
void x265_enc_entropy_encode_cu_transquant_bypass_flag(x265_enc_entropy_t *enc_entropy,
												x265_data_cu_t *cu,
												uint32_t i_abs_part_idx,
												int32_t b_rd ) ;
void x265_enc_entropy_encode_skip_flag(x265_t* h,
									x265_enc_entropy_t *enc_entropy,
									x265_data_cu_t *cu,
									uint32_t i_abs_part_idx,
									int32_t b_rd ) ;
void x265_enc_entropy_encode_pu_wise(x265_t *h,
									x265_enc_entropy_t *enc_entropy,
									x265_data_cu_t* cu,
									uint32_t i_abs_part_idx,
									int32_t b_rd ) ;
void x265_enc_entropy_encode_inter_dir_pu(x265_t *h,
											x265_enc_entropy_t *enc_entropy,
											x265_data_cu_t* cu,
											uint32_t i_abs_part_idx ) ;
void x265_enc_entropy_encode_ref_frm_idx_pu(x265_t *h,
											x265_enc_entropy_t *enc_entropy,
											x265_data_cu_t* cu,
											uint32_t i_abs_part_idx,
											enum ref_pic_list_e i_ref_pic_list ) ;
void x265_enc_entropy_encode_mvd_pu(x265_t *h,
									x265_enc_entropy_t *enc_entropy,
									x265_data_cu_t* cu,
									uint32_t i_abs_part_idx,
									enum ref_pic_list_e i_ref_pic_list ) ;
void x265_enc_entropy_encode_mvp_idx_pu(x265_enc_entropy_t *enc_entropy,
										x265_data_cu_t* cu,
										uint32_t i_abs_part_idx,
										enum ref_pic_list_e i_ref_pic_list ) ;
void x265_enc_entropy_encode_merge_flag(x265_t* h,
										x265_enc_entropy_t *enc_entropy,
										x265_data_cu_t *cu,
										uint32_t i_abs_part_idx ) ;
void x265_enc_entropy_encode_merge_index(x265_t* h,
										x265_enc_entropy_t *enc_entropy,
										x265_data_cu_t *cu,
										uint32_t i_abs_part_idx,
										int32_t b_rd ) ;
void x265_enc_entropy_encode_pred_mode(x265_t *h,
										x265_enc_entropy_t *enc_entropy,
										x265_data_cu_t *cu,
										uint32_t i_abs_part_idx,
										int32_t b_rd ) ;
void x265_enc_entropy_encode_part_size(x265_t* h,
										x265_enc_entropy_t *enc_entropy,
										x265_data_cu_t *cu,
										uint32_t i_abs_part_idx,
										uint32_t i_depth,
										int32_t b_rd ) ;
void x265_enc_entropy_encode_ipcm_info(x265_t* h,
										x265_enc_entropy_t *enc_entropy,
										x265_data_cu_t *cu,
										uint32_t i_abs_part_idx,
										int32_t b_rd ) ;
void x265_enc_entropy_encode_pred_info(x265_t *h,
										x265_enc_entropy_t *enc_entropy,
										x265_data_cu_t* cu,
										uint32_t i_abs_part_idx,
										int32_t b_rd ) ;
void x265_enc_entropy_encode_intra_dir_mode_luma (x265_t* h,
												x265_enc_entropy_t *enc_entropy,
												x265_data_cu_t* cu,
												uint32_t abs_part_idx,
												int32_t b_is_multiple_pu ) ;
void x265_enc_entropy_encode_intra_dir_mode_chroma(x265_enc_entropy_t *enc_entropy,
													x265_data_cu_t* cu,
													uint32_t i_abs_part_idx,
													int32_t b_rd ) ;
void x265_enc_entropy_encode_transform_sub_div_flag(x265_t *h,
													x265_enc_entropy_t *enc_entropy,
													uint32_t i_symbol,
													uint32_t i_ctx ) ;
void x265_enc_entropy_encode_qt_cbf(x265_t *h,
									x265_enc_entropy_t *enc_entropy,
									x265_data_cu_t* cu,
									uint32_t i_abs_part_idx,
									enum text_type_e i_text_type,
									uint32_t i_tr_depth ) ;
void x265_enc_entropy_encode_qt_cbf_zero(x265_enc_entropy_t *enc_entropy,
										x265_data_cu_t* cu,
										enum text_type_e i_text_type,
										uint32_t i_tr_depth ) ;
void x265_enc_entropy_encode_qt_root_cbf_zero(x265_enc_entropy_t *enc_entropy,
											x265_data_cu_t* cu ) ;
void x265_enc_entropy_encode_qt_root_cbf(x265_enc_entropy_t *enc_entropy,
										x265_data_cu_t* cu,
										uint32_t i_abs_part_idx ) ;
void x265_enc_entropy_encode_qp(x265_t *h,
								x265_enc_entropy_t *enc_entropy,
								x265_data_cu_t* cu,
								uint32_t i_abs_part_idx,
								int32_t b_rd ) ;
void x265_enc_entropy_update_context_tables_p4 ( x265_enc_entropy_t *enc_entropy,
												enum slice_type_e i_slice_type,
												int32_t i_qp,
												int32_t b_execute_finish ) ;
void x265_enc_entropy_update_context_tables_p3 ( x265_enc_entropy_t *enc_entropy,
												enum slice_type_e i_slice_type,
												int32_t i_qp ) ;
void x265_enc_entropy_encode_scaling_list(x265_t *h,
										x265_enc_entropy_t *enc_entropy,
										x265_scaling_list_t* scaling_list ) ;

void x265_enc_entropy_x_encode_transform( x265_t *h,
										x265_enc_entropy_t *enc_entropy,
										x265_data_cu_t* cu,
										uint32_t i_offset_luma,
										uint32_t i_offset_chroma,
										uint32_t i_abs_part_idx,
										uint32_t i_depth,
										uint32_t i_width,
										uint32_t i_height,
										uint32_t i_tr_idx,
										int32_t *p_code_dqp ) ;

void x265_enc_entropy_encode_coeff( x265_t *h,
									x265_enc_entropy_t *enc_entropy,
									x265_data_cu_t* cu,
									uint32_t i_abs_part_idx,
									uint32_t i_depth,
									uint32_t i_width,
									uint32_t i_height,
									int32_t* p_code_dqp ) ;
void x265_enc_entropy_encode_coeff_nxn(x265_t *h,
										x265_enc_entropy_t *enc_entropy,
										x265_data_cu_t *cu,
										x265_coeff_t* pc_coeff,
										uint32_t i_abs_part_idx,
										uint32_t i_tr_width,
										uint32_t i_tr_height,
										uint32_t i_depth,
										enum text_type_e i_text_type ) ;
void x265_enc_entropy_estimate_bit (x265_t *h,
									x265_enc_entropy_t *enc_entropy,
									x265_est_bits_sbac_struct_t* pc_est_bits_sbac,
									int32_t i_width,
									int32_t i_height,
									enum text_type_e i_text_type) ;

void x265_enc_entropy_encode_sao_offset ( x265_t *h,
										x265_enc_entropy_t *enc_entropy,
										x265_sao_lcu_param_t *sao_lcu_param,
										uint32_t i_comp_idx ) ;
void x265_enc_entropy_encode_sao_unit_interleaving ( x265_t *h,
													x265_enc_entropy_t *enc_entropy,
													int32_t comp_idx,
													int32_t b_sao_flag,
													int32_t rx,
													int32_t ry,
													x265_sao_lcu_param_t *sao_lcu_param,
													int32_t cu_addr_in_slice,
													int32_t cu_addr_up_in_slice,
													int32_t allow_merge_left,
													int32_t allow_merge_up) ;

int32_t x265_enc_entropy_count_non_zero_coeffs(x265_enc_entropy_t *enc_entropy,
												x265_coeff_t* coeff,
												uint32_t i_size ) ;
#endif



