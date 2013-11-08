


#ifndef X265_ENCODER_ENTROPY_IF_H
#define X265_ENCODER_ENTROPY_IF_H

typedef struct _x265_enc_entropy_if_t x265_enc_entropy_if_t ;


#define X265_ENC_ENTROPY_IF_FUNC \
	void (*enc_entropy_if_delete) ( x265_enc_entropy_if_t *enc_entropy_if ) ; \
	void (*reset_entropy) ( x265_t *h, \
							x265_enc_entropy_if_t *enc_entropy_if, \
							enum slice_type_e i_slice_type, \
							int32_t i_slice_qp ) ; \
	void (*print_cabac_state) ( x265_enc_entropy_if_t *enc_entropy_if ) ; \
	void (*determine_cabac_init_idx) ( x265_t *h, \
										x265_enc_entropy_if_t *enc_entropy_if, \
										enum slice_type_e i_slice_type, \
										int32_t i_slice_qp ) ; \
	void (*set_bitstream) ( x265_enc_entropy_if_t *enc_entropy_if, x265_bit_if_t* p ); \
	void (*set_slice) ( x265_enc_entropy_if_t *enc_entropy_if, x265_slice_t* slice ); \
	void (*reset_bits) ( x265_enc_entropy_if_t *enc_entropy_if ); \
	void (*clear_bits) ( x265_enc_entropy_if_t *enc_entropy_if ); \
	void (*reset_coeff_cost) ( x265_enc_entropy_if_t *enc_entropy_if ); \
	uint32_t (*get_number_of_written_bits) ( x265_enc_entropy_if_t *enc_entropy_if ); \
	uint32_t (*get_coeff_cost) ( x265_enc_entropy_if_t *enc_entropy_if ); \
	void (*code_vps) ( x265_enc_entropy_if_t *enc_entropy_if, x265_vps_t *vps ); \
	void (*code_sps) ( x265_t *h, x265_enc_entropy_if_t *enc_entropy_if, x265_sps_t *sps ); \
	void (*code_pps) ( x265_t *h, x265_enc_entropy_if_t *enc_entropy_if, x265_pps_t *pps ); \
	void (*code_slice_header) ( x265_t *h, x265_enc_entropy_if_t *enc_entropy_if, x265_slice_t *slice ) ; \
	int (*code_tiles_wpp_entry_point) ( x265_t *h, x265_enc_entropy_if_t *enc_entropy_if, \
										bs_t *s, x265_slice_t *slice ) ; \
	void (*code_terminating_bit) ( x265_enc_entropy_if_t *enc_entropy_if, uint32_t i_ls_last ); \
	void (*code_slice_finish) ( x265_enc_entropy_if_t *enc_entropy_if ); \
	void (*code_mvp_idx) ( x265_enc_entropy_if_t *enc_entropy_if, x265_data_cu_t* cu, uint32_t i_abs_part_idx, enum ref_pic_list_e i_ref_list ); \
	void (*code_scaling_list) ( x265_t* h, x265_enc_entropy_if_t *enc_entropy_if, x265_scaling_list_t* scaling_list ); \
	void (*code_cu_transquant_bypass_flag) ( x265_enc_entropy_if_t *enc_entropy_if, x265_data_cu_t* cu, uint32_t i_abs_part_idx ); \
	void (*code_skip_flag) ( x265_t* h, x265_enc_entropy_if_t *enc_entropy_if, x265_data_cu_t* cu, uint32_t i_abs_part_idx ); \
	void (*code_merge_flag) ( x265_t* h, x265_enc_entropy_if_t *enc_entropy_if, x265_data_cu_t* cu, uint32_t i_abs_part_idx ); \
	void (*code_merge_index) ( x265_t* h, x265_enc_entropy_if_t *enc_entropy_if, x265_data_cu_t* cu, uint32_t i_abs_part_idx ); \
	void (*code_split_flag) ( x265_t* h, x265_enc_entropy_if_t *enc_entropy_if, x265_data_cu_t* cu, uint32_t i_abs_part_idx, uint32_t i_depth ); \
	void (*code_part_size) ( x265_t* h, x265_enc_entropy_if_t *enc_entropy_if, x265_data_cu_t* cu, uint32_t i_abs_part_idx, uint32_t i_depth ); \
	void (*code_pred_mode) ( x265_enc_entropy_if_t *enc_entropy_if, x265_data_cu_t* cu, uint32_t i_abs_part_idx ); \
	void (*code_ipcm_info) ( x265_t* h, x265_enc_entropy_if_t *enc_entropy_if, x265_data_cu_t* cu, uint32_t i_abs_part_idx ); \
	void (*code_transform_sub_div_flag) ( x265_t* h, x265_enc_entropy_if_t *enc_entropy_if, uint32_t i_symbol, uint32_t i_ctx ); \
	void (*code_qt_cbf) ( x265_t* h, x265_enc_entropy_if_t *enc_entropy_if, x265_data_cu_t* cu, uint32_t i_abs_part_idx, enum text_type_e i_text_type, uint32_t i_tr_depth ); \
	void (*code_qt_root_cbf) ( x265_enc_entropy_if_t *enc_entropy_if, x265_data_cu_t* cu, uint32_t i_abs_part_idx ); \
	void (*code_qt_cbf_zero) ( x265_enc_entropy_if_t *enc_entropy_if, x265_data_cu_t* cu, enum text_type_e i_text_type, uint32_t i_tr_depth ); \
	void (*code_qt_root_cbf_zero) ( x265_enc_entropy_if_t *enc_entropy_if, x265_data_cu_t* cu ); \
	void (*code_intra_dir_luma_ang) ( x265_t* h, x265_enc_entropy_if_t *enc_entropy_if, x265_data_cu_t* cu, uint32_t i_abs_part_idx, int32_t b_is_multiple_pu ); \
	void (*code_intra_dir_chroma) ( x265_enc_entropy_if_t *enc_entropy_if, x265_data_cu_t* cu, uint32_t i_abs_part_idx ); \
	void (*code_inter_dir) ( x265_enc_entropy_if_t *enc_entropy_if, x265_data_cu_t* cu, uint32_t i_abs_part_idx ); \
	void (*code_ref_frm_idx) ( x265_t* h, x265_enc_entropy_if_t *enc_entropy_if, x265_data_cu_t* cu, uint32_t i_abs_part_idx, enum ref_pic_list_e i_ref_list ); \
	void (*code_mvd) ( x265_t* h, x265_enc_entropy_if_t *enc_entropy_if, x265_data_cu_t* cu, uint32_t i_abs_part_idx, enum ref_pic_list_e i_ref_list ); \
	void (*code_delta_qp) ( x265_t* h, x265_enc_entropy_if_t *enc_entropy_if, x265_data_cu_t* cu, uint32_t i_abs_part_idx ); \
	void (*code_coeff_nxn) ( x265_t* h, x265_enc_entropy_if_t *enc_entropy_if, x265_data_cu_t* cu, x265_coeff_t* pc_coef, uint32_t i_abs_part_idx, uint32_t i_width, uint32_t i_height, uint32_t i_depth, enum text_type_e i_text_type ); \
	void (*code_transform_skip_flags) ( x265_t* h, x265_enc_entropy_if_t *enc_entropy_if, x265_data_cu_t* cu, uint32_t i_abs_part_idx, uint32_t width, uint32_t height, enum text_type_e i_text_type ); \
	void (*code_sao_sign) ( x265_enc_entropy_if_t *enc_entropy_if, uint32_t code ); \
	void (*code_sao_max_uvlc) ( x265_enc_entropy_if_t *enc_entropy_if, uint32_t code, uint32_t maxSymbol ); \
	void (*code_sao_merge) ( x265_enc_entropy_if_t *enc_entropy_if, uint32_t i_code ) ; \
	void (*code_sao_type_idx) ( x265_enc_entropy_if_t *enc_entropy_if, uint32_t   i_code ); \
	void (*code_sao_uflc) ( x265_enc_entropy_if_t *enc_entropy_if, uint32_t i_length, uint32_t   i_code ); \
	void (*est_bit) ( x265_t *h, \
						x265_enc_entropy_if_t *enc_entropy_if, \
						x265_est_bits_sbac_struct_t* est_bits_sbac, \
						int32_t i_width, \
						int32_t i_height, \
						enum text_type_e i_text_type ) ; \
	void (*update_context_tables_p4) ( x265_enc_entropy_if_t *enc_entropy_if, enum slice_type_e i_slice_type, int32_t i_qp, int32_t b_execute_finish ); \
	void (*update_context_tables_p3) ( x265_enc_entropy_if_t *enc_entropy_if, enum slice_type_e i_slice_type, int32_t i_qp ); \
	void (*code_df_flag) ( x265_enc_entropy_if_t *enc_entropy_if, uint32_t i_code ); \
	void (*code_df_svlc) ( x265_enc_entropy_if_t *enc_entropy_if, int32_t i_code )

struct _x265_enc_entropy_if_t
{
	X265_ENC_ENTROPY_IF_FUNC ;

} ;


void x265_enc_entropy_if_delete ( x265_enc_entropy_if_t *enc_entropy_if ) ;
void x265_enc_entropy_if_init ( x265_enc_entropy_if_t * enc_entropy_if ) ;














#endif

