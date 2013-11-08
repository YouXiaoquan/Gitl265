



#include "common/common.h"

const uint32_t min_in_group[10] = {0,1,2,3,4,6,8,12,16,24};
const uint32_t group_idx[32] = {0,1,2,3,4,4,5,5,6,6,6,6,7,7,7,7,8,8,8,8,8,8,8,8,9,9,9,9,9,9,9,9};

void enc_sbac_init ( x265_enc_sbac_t *enc_sbac, x265_enc_bin_if_t *enc_bin_if )
{
	enc_sbac->enc_bin_if = enc_bin_if ;
}


void enc_sbac_uninit ( x265_enc_entropy_if_t *enc_entropy_if )
{
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t *)enc_entropy_if ;
	enc_sbac->enc_bin_if = 0 ;
}

void x265_enc_sbac_set_bitstream ( x265_enc_entropy_if_t *enc_entropy_if, x265_bit_if_t* bit_if )
{
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	enc_sbac->bit_if = bit_if ;
	enc_sbac->enc_bin_if->init ( enc_sbac->enc_bin_if, bit_if ) ;
}

void x265_enc_sbac_store_bitstream ( x265_enc_sbac_t *enc_sbac, x265_enc_sbac_t *dst_enc_sbac )
{
	enc_sbac->bit_if->store (enc_sbac->bit_if, dst_enc_sbac->bit_if) ;
}

void x265_enc_sbac_set_slice ( x265_enc_entropy_if_t *enc_entropy_if, x265_slice_t* slice )
{
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	enc_sbac->slice = slice ;
}

void x265_enc_sbac_reset_coeff_cost ( x265_enc_entropy_if_t *enc_entropy_if )
{
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	enc_sbac->i_coeff_cost = 0 ;
}

uint32_t x265_enc_sbac_get_coeff_cost ( x265_enc_entropy_if_t *enc_entropy_if )
{
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	return enc_sbac->i_coeff_cost ;
}

void x265_enc_sbac_reset_bits ( x265_enc_entropy_if_t *enc_entropy_if )
{
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	enc_sbac->enc_bin_if->reset_bits(enc_sbac->enc_bin_if) ;
	enc_sbac->bit_if->reset_bits (enc_sbac->bit_if) ;
}

void x265_enc_sbac_clear_bits ( x265_enc_entropy_if_t *enc_entropy_if )
{
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	enc_sbac->enc_bin_if->clear_bits(enc_sbac->enc_bin_if) ;
}

uint32_t x265_enc_sbac_get_number_of_written_bits ( x265_enc_entropy_if_t *enc_entropy_if )
{
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	return enc_sbac->enc_bin_if->get_num_written_bits ( enc_sbac->enc_bin_if ) ;
}

void x265_enc_sbac_code_scaling_list ( x265_t *h,
										x265_enc_entropy_if_t *enc_entropy_if,
										x265_scaling_list_t *scaling_list )
{
	assert (0) ;
}

void x265_enc_sbac_code_df_flag ( x265_enc_entropy_if_t *enc_entropy_if, uint32_t i_code )
{
	printf("Not supported in code_df_flag()\n");
	assert(0);
	exit(1);
}

void x265_enc_sbac_code_df_svlc ( x265_enc_entropy_if_t *enc_entropy_if, int32_t i_code )
{
	printf("Not supported in code_df_svlc()\n");
	assert(0);
	exit(1);
}

void x265_enc_sbac_update_context_tables_p3( x265_enc_entropy_if_t *enc_entropy_if,
											enum slice_type_e i_slice_type,
											int32_t i_slice_qp )
{
	x265_enc_sbac_update_context_tables_p4 (enc_entropy_if,
											i_slice_type,
											i_slice_qp,
											1 ) ;
}

x265_enc_bin_if_t *x265_enc_sbac_get_enc_bin_if ( x265_enc_sbac_t *enc_sbac )
{
	return enc_sbac->enc_bin_if ;
}

x265_enc_entropy_if_t *x265_enc_sbac_new ()
{
	x265_enc_entropy_if_t *enc_entropy_if = NULL ;

	CHECKED_MALLOCZERO( enc_entropy_if, sizeof(x265_enc_sbac_t) );
	if ( x265_enc_sbac_init(enc_entropy_if) )
	{
		goto fail ;
	}

	return enc_entropy_if ;
fail:
	x265_enc_sbac_delete ( enc_entropy_if ) ;
	return NULL ;
}

void x265_enc_sbac_delete ( x265_enc_entropy_if_t *enc_entropy_if )
{
	if ( enc_entropy_if )
	{
		x265_enc_sbac_deinit ( enc_entropy_if ) ;
		x265_free ( enc_entropy_if ) ;
	}
}

int x265_enc_sbac_init ( x265_enc_entropy_if_t *enc_entropy_if )
{
	x265_enc_entropy_if_init ( enc_entropy_if ) ;

	enc_entropy_if->enc_entropy_if_delete = x265_enc_sbac_delete ;
	enc_entropy_if->reset_entropy = x265_enc_sbac_reset_entropy ;
	enc_entropy_if->print_cabac_state = x265_enc_sbac_print_cabac_state ;
	enc_entropy_if->determine_cabac_init_idx = x265_enc_sbac_determine_cabac_init_idx ;
	enc_entropy_if->set_bitstream = x265_enc_sbac_set_bitstream ;
	enc_entropy_if->set_slice = x265_enc_sbac_set_slice ;
	enc_entropy_if->reset_bits = x265_enc_sbac_reset_bits ;
	enc_entropy_if->clear_bits = x265_enc_sbac_clear_bits ;
	enc_entropy_if->reset_coeff_cost = x265_enc_sbac_reset_coeff_cost ;
	enc_entropy_if->get_number_of_written_bits = x265_enc_sbac_get_number_of_written_bits ;
	enc_entropy_if->get_coeff_cost = x265_enc_sbac_get_coeff_cost ;
	enc_entropy_if->code_vps = x265_enc_sbac_code_vps ;
	enc_entropy_if->code_sps = x265_enc_sbac_code_sps ;
	enc_entropy_if->code_pps = x265_enc_sbac_code_pps ;
	enc_entropy_if->code_slice_header = x265_enc_sbac_code_slice_header ;
	enc_entropy_if->code_tiles_wpp_entry_point = x265_enc_sbac_code_tiles_wpp_entry_point ;
	enc_entropy_if->code_terminating_bit = x265_enc_sbac_code_terminating_bit ;
	enc_entropy_if->code_slice_finish = x265_enc_sbac_code_slice_finish ;
	enc_entropy_if->code_mvp_idx = x265_enc_sbac_code_mvp_idx ;
	enc_entropy_if->code_scaling_list = x265_enc_sbac_code_scaling_list ;
	enc_entropy_if->code_cu_transquant_bypass_flag = x265_enc_sbac_code_cu_transquant_bypass_flag ;
	enc_entropy_if->code_skip_flag = x265_enc_sbac_code_skip_flag ;
	enc_entropy_if->code_merge_flag = x265_enc_sbac_code_merge_flag ;
	enc_entropy_if->code_merge_index = x265_enc_sbac_code_merge_index ;
	enc_entropy_if->code_split_flag = x265_enc_sbac_code_split_flag ;
	enc_entropy_if->code_part_size = x265_enc_sbac_code_part_size ;
	enc_entropy_if->code_pred_mode = x265_enc_sbac_code_pred_mode ;
	enc_entropy_if->code_ipcm_info = x265_enc_sbac_code_ipcm_info ;
	enc_entropy_if->code_transform_sub_div_flag = x265_enc_sbac_code_transform_sub_div_flag ;
	enc_entropy_if->code_qt_cbf = x265_enc_sbac_code_qt_cbf ;
	enc_entropy_if->code_qt_root_cbf = x265_enc_sbac_code_qt_root_cbf ;
	enc_entropy_if->code_qt_cbf_zero = x265_enc_sbac_code_qt_cbf_zero ;
	enc_entropy_if->code_qt_root_cbf_zero = x265_enc_sbac_code_qt_root_cbf_zero ;
	enc_entropy_if->code_intra_dir_luma_ang = x265_enc_sbac_code_intra_dir_luma_ang ;
	enc_entropy_if->code_intra_dir_chroma = x265_enc_sbac_code_intra_dir_chroma ;
	enc_entropy_if->code_inter_dir = x265_enc_sbac_code_inter_dir ;
	enc_entropy_if->code_ref_frm_idx = x265_enc_sbac_code_ref_frm_idx ;
	enc_entropy_if->code_mvd = x265_enc_sbac_code_mvd ;
	enc_entropy_if->code_delta_qp = x265_enc_sbac_code_delta_qp ;
	enc_entropy_if->code_coeff_nxn = x265_enc_sbac_code_coeff_nxn ;
	enc_entropy_if->code_transform_skip_flags = x265_enc_sbac_code_transform_skip_flags ;
	enc_entropy_if->code_sao_sign = x265_enc_sbac_code_sao_sign ;
	enc_entropy_if->code_sao_max_uvlc = x265_enc_sbac_code_sao_max_uvlc ;
	enc_entropy_if->code_sao_merge = x265_enc_sbac_code_sao_merge ;
	enc_entropy_if->code_sao_type_idx = x265_enc_sbac_code_sao_type_idx ;
	enc_entropy_if->code_sao_uflc = x265_enc_sbac_code_sao_uflc ;
	enc_entropy_if->est_bit = x265_enc_sbac_est_bit ;
	enc_entropy_if->update_context_tables_p4 = x265_enc_sbac_update_context_tables_p4 ;
	enc_entropy_if->update_context_tables_p3 = x265_enc_sbac_update_context_tables_p3 ;
	enc_entropy_if->code_df_flag = x265_enc_sbac_code_df_flag ;
	enc_entropy_if->code_df_svlc = x265_enc_sbac_code_df_svlc ;
	return 0 ;
}

void x265_enc_sbac_deinit ( x265_enc_entropy_if_t *enc_entropy_if )
{

}

void x265_enc_sbac_reset_entropy ( x265_t *h,
									x265_enc_entropy_if_t *enc_entropy_if,
									enum slice_type_e i_slice_type,
									int32_t i_slice_qp )
{
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	enc_sbac->enc_bin_if->reset_entropy ( h,
										enc_sbac->enc_bin_if,
										i_slice_type,
										i_slice_qp ) ;
	enc_sbac->i_last_qp = i_slice_qp ;
	enc_sbac->enc_bin_if->start (enc_sbac->enc_bin_if) ;
}

void x265_enc_sbac_print_cabac_state ( x265_enc_entropy_if_t *enc_entropy_if )
{
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	enc_sbac->enc_bin_if->print_cabac_state(enc_sbac->enc_bin_if) ;
}

void x265_enc_sbac_determine_cabac_init_idx ( x265_t *h,
												x265_enc_entropy_if_t *enc_entropy_if,
												enum slice_type_e i_slice_type,
												int32_t i_slice_qp )
{
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	enc_sbac->enc_bin_if->determine_cabac_init_idx ( h,
													enc_sbac->enc_bin_if,
													i_slice_type,
													i_slice_qp ) ;
}


void x265_enc_sbac_update_context_tables_p4( x265_enc_entropy_if_t *enc_entropy_if,
											enum slice_type_e i_slice_type,
											int32_t i_slice_qp,
											int32_t b_execute_finish )
{
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	enc_sbac->enc_bin_if->encode_bin_trm ( enc_sbac->enc_bin_if, 1 ) ;
	if ( b_execute_finish )
	{
		enc_sbac->enc_bin_if->finish ( enc_sbac->enc_bin_if ) ;
	}
	enc_sbac->enc_bin_if->update_context_table ( enc_sbac->enc_bin_if,
												i_slice_type,
												i_slice_qp ) ;
	enc_sbac->enc_bin_if->start (enc_sbac->enc_bin_if) ;
}

void x265_enc_sbac_code_vps( x265_enc_entropy_if_t *enc_entropy_if, x265_vps_t *vps )
{
	assert ( 0 ) ;
}

void x265_enc_sbac_code_sps( x265_t *h, x265_enc_entropy_if_t *enc_entropy_if, x265_sps_t *sps )
{
	assert ( 0 ) ;
}

void x265_enc_sbac_code_pps ( x265_t *h, x265_enc_entropy_if_t *enc_entropy_if, x265_pps_t *pps )
{
	assert ( 0 ) ;
}

void x265_enc_sbac_code_slice_header ( x265_t *h, x265_enc_entropy_if_t *enc_entropy_if, x265_slice_t *slice )
{
	assert ( 0 ) ;
}

int x265_enc_sbac_code_tiles_wpp_entry_point( x265_t *h, x265_enc_entropy_if_t *enc_entropy_if,
												bs_t *s, x265_slice_t *slice )
{
	assert ( 0 ) ;
	return 0 ;
}

void x265_enc_sbac_code_terminating_bit (x265_enc_entropy_if_t *enc_entropy_if,
										uint32_t i_ls_last )
{
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	enc_sbac->enc_bin_if->encode_bin_trm (enc_sbac->enc_bin_if, i_ls_last) ;
}

void x265_enc_sbac_code_slice_finish (x265_enc_entropy_if_t *enc_entropy_if )
{
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	enc_sbac->enc_bin_if->finish (enc_sbac->enc_bin_if) ;
}

//void x265_enc_sbac_x_write_unary_symbol( x265_enc_sbac_t* enc_sbac, uint32_t symbol, x265_context_model_t* i_sc_model_start, int32_t i_offset )
void x265_enc_sbac_x_write_unary_symbol( x265_enc_sbac_t* enc_sbac, uint32_t symbol, uint32_t i_sc_model_start, int32_t i_offset )
{
	enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, symbol ? 1 : 0, i_sc_model_start + 0 );

	if( 0 == symbol)
	{
		return;
	}

	while( symbol-- )
	{
		enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, symbol ? 1 : 0, i_sc_model_start + i_offset );
	}

	return;
}

//void x265_enc_sbac_x_write_unary_max_symbol( x265_enc_sbac_t* enc_sbac, uint32_t symbol, x265_context_model_t* i_sc_model_start, int32_t i_offset, uint32_t max_symbol )
void x265_enc_sbac_x_write_unary_max_symbol( x265_enc_sbac_t* enc_sbac, uint32_t symbol, uint32_t i_sc_model_start, int32_t i_offset, uint32_t max_symbol )
{
	if (max_symbol == 0)
	{
		return;
	}

	enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, symbol ? 1 : 0, i_sc_model_start + 0 );

	if ( symbol == 0 )
	{
		return;
	}

	int32_t b_code_last = ( max_symbol > symbol );

	while( --symbol )
	{
		enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, 1, i_sc_model_start + i_offset );
	}
	if( b_code_last )
	{
		enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, 0, i_sc_model_start + i_offset );
	}

	return;
}

void x265_enc_sbac_x_write_ep_ex_golomb( x265_enc_sbac_t* enc_sbac, uint32_t symbol, uint32_t count )
{
	uint32_t bins = 0;
	int32_t num_bins = 0;

	while( symbol >= (uint32_t)(1<<count) )
	{
		bins = 2 * bins + 1;
		num_bins++;
		symbol -= 1 << count;
		count  ++;
	}
	bins = 2 * bins + 0;
	num_bins++;

	bins = (bins << count) | symbol;
	num_bins += count;

	assert( num_bins <= 32 );
	enc_sbac->enc_bin_if->encode_bins_ep( enc_sbac->enc_bin_if,  bins, num_bins );
}

/** coding of coeff_abs_level_minus3
 * \param symbol value of coeff_abs_level_minus3
 * \param rgo_rice_param reference to rice parameter
 * \returns void
 */
void x265_enc_sbac_x_write_coef_remain_ex_golomb ( x265_enc_sbac_t* enc_sbac, uint32_t symbol, uint32_t i_r_param )
{
	int32_t code_number  = (int32_t)symbol;
	uint32_t length;
	if (code_number < (X265_COEF_REMAIN_BIN_REDUCTION << i_r_param))
	{
		length = code_number>>i_r_param;
		enc_sbac->enc_bin_if->encode_bins_ep( enc_sbac->enc_bin_if,  (1<<(length+1))-2 , length+1);
		enc_sbac->enc_bin_if->encode_bins_ep( enc_sbac->enc_bin_if, (code_number%(1<<i_r_param)),i_r_param);
	}
	else
	{
		length = i_r_param;
		code_number  = code_number - ( X265_COEF_REMAIN_BIN_REDUCTION << i_r_param);
		while (code_number >= (1<<length))
		{
			code_number -=  (1<<(length++));
		}
		enc_sbac->enc_bin_if->encode_bins_ep( enc_sbac->enc_bin_if, (1<<(X265_COEF_REMAIN_BIN_REDUCTION+length+1-i_r_param))-2,X265_COEF_REMAIN_BIN_REDUCTION+length+1-i_r_param);
		enc_sbac->enc_bin_if->encode_bins_ep( enc_sbac->enc_bin_if, code_number,length);
	}
}

void x265_enc_sbac_load ( x265_enc_sbac_t *enc_sbac, x265_enc_sbac_t *src_enc_sbac )
{
	x265_enc_sbac_x_copy_from ( enc_sbac, src_enc_sbac ) ;
}

void x265_enc_sbac_load_intra_dir_mode_luma( x265_enc_sbac_t* enc_sbac, x265_enc_sbac_t* src_enc_sbac )
{
	enc_sbac->enc_bin_if->load_intra_dir_mode_luma ( enc_sbac->enc_bin_if,
													src_enc_sbac->enc_bin_if ) ;
}

void x265_enc_sbac_store ( x265_enc_sbac_t *enc_sbac, x265_enc_sbac_t *dst_enc_sbac )
{
	x265_enc_sbac_x_copy_from ( dst_enc_sbac, enc_sbac ) ;
}

void x265_enc_sbac_x_copy_from  ( x265_enc_sbac_t *enc_sbac, x265_enc_sbac_t *src_enc_sbac )
{
	enc_sbac->enc_bin_if->copy_state ( src_enc_sbac->enc_bin_if, enc_sbac->enc_bin_if ) ;
	enc_sbac->i_coeff_cost = src_enc_sbac->i_coeff_cost ;
	enc_sbac->i_last_qp = src_enc_sbac->i_last_qp ;
}

void x265_enc_sbac_code_mvp_idx ( x265_enc_entropy_if_t* enc_entropy_if,
								x265_data_cu_t* cu,
								uint32_t i_abs_part_idx,
								enum ref_pic_list_e i_ref_list )
{
	int32_t i_symbol = 0 ;
	int32_t i_num = 0 ;

	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	i_symbol = x265_data_cu_get_mvp_idx_p3 ( cu, i_ref_list, i_abs_part_idx);
	i_num = X265_AMVP_MAX_NUM_CANDS;

	x265_enc_sbac_x_write_unary_max_symbol ( enc_sbac, i_symbol, X265_CU_MVP_IDX_START_IDX, 1, i_num-1);
}

void x265_enc_sbac_code_part_size( x265_t* h,
									x265_enc_entropy_if_t* enc_entropy_if,
									x265_data_cu_t* cu,
									uint32_t i_abs_part_idx,
									uint32_t depth )
{
	enum part_size_e i_part_size = 0 ;
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	i_part_size = x265_data_cu_get_partition_size_p2(cu, i_abs_part_idx );
	if ( x265_base_data_cu_is_intra((x265_base_data_cu_t*)cu, i_abs_part_idx ) )
	{
		if( depth == h->cu.pic.i_total_depth - h->cu.pic.i_add_cu_depth )
		{
			enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, i_part_size == SIZE_2Nx2N? 1 : 0, X265_CU_PART_SIZE_START_IDX );
		}
		return;
	}

	switch(i_part_size)
	{
		case SIZE_2Nx2N:
		{
			enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, 1, X265_CU_PART_SIZE_START_IDX );
			break;
		}
		case SIZE_2NxN:
		case SIZE_2NxnU:
		case SIZE_2NxnD:
		{
			enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, 0, X265_CU_PART_SIZE_START_IDX );
			enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, 1, X265_CU_PART_SIZE_START_IDX + 1 );
			if ( h->sps[0].amp_acc[depth] )
			{
				if (i_part_size == SIZE_2NxN)
				{
					enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, 1, X265_CU_PART_SIZE_START_IDX + 3 );
				}
				else
				{
					enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, 0, X265_CU_PART_SIZE_START_IDX + 3);
					enc_sbac->enc_bin_if->encode_bin_ep (enc_sbac->enc_bin_if, (i_part_size == SIZE_2NxnU ? 0 : 1));
				}
			}
			break;
		}
		case SIZE_Nx2N:
		case SIZE_nLx2N:
		case SIZE_nRx2N:
		{
			enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, 0, X265_CU_PART_SIZE_START_IDX );
			enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, 0, X265_CU_PART_SIZE_START_IDX + 1 );
			if( depth == h->cu.pic.i_total_depth - h->cu.pic.i_add_cu_depth
					&& !( x265_data_cu_get_width_p2(cu, i_abs_part_idx) == 8
							&& x265_data_cu_get_height_p2(cu, i_abs_part_idx) == 8 ) )
			{
				enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, 1, X265_CU_PART_SIZE_START_IDX + 2 );
			}
			if ( h->sps[0].amp_acc[depth] )
			{
				if (i_part_size == SIZE_Nx2N)
				{
					enc_sbac->enc_bin_if->encode_bin(enc_sbac->enc_bin_if, 1, X265_CU_PART_SIZE_START_IDX + 3);
				}
				else
				{
					enc_sbac->enc_bin_if->encode_bin(enc_sbac->enc_bin_if, 0, X265_CU_PART_SIZE_START_IDX + 3);
					enc_sbac->enc_bin_if->encode_bin_ep(enc_sbac->enc_bin_if, (i_part_size == SIZE_nLx2N ? 0: 1));
				}
			}
			break;
		}
		case SIZE_NxN:
		{
			if( depth == h->cu.pic.i_total_depth - h->cu.pic.i_add_cu_depth
					&& !( x265_data_cu_get_width_p2(cu, i_abs_part_idx) == 8
						&& x265_data_cu_get_height_p2(cu, i_abs_part_idx) == 8 ) )
			{
				enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, 0, X265_CU_PART_SIZE_START_IDX );
				enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, 0, X265_CU_PART_SIZE_START_IDX + 1 );
				enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, 0, X265_CU_PART_SIZE_START_IDX + 2 );
			}
			break;
		}
		default:
		{
			assert(0);
			break ;
		}
	}
}

void x265_enc_sbac_code_pred_mode( x265_enc_entropy_if_t* enc_entropy_if,
									x265_data_cu_t* cu,
									uint32_t i_abs_part_idx )
{
	// get context function is here
	int32_t i_pred_mode = 0 ;
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	i_pred_mode = x265_base_data_cu_get_prediction_mode_p2 ((x265_base_data_cu_t*)cu, i_abs_part_idx );
	enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, i_pred_mode == MODE_INTER ? 0 : 1, X265_CU_PRED_MODE_START_IDX );
}

void x265_enc_sbac_code_cu_transquant_bypass_flag( x265_enc_entropy_if_t* enc_entropy_if,
													x265_data_cu_t* cu,
													uint32_t i_abs_part_idx )
{
	uint32_t symbol = 0 ;
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	symbol = x265_base_data_cu_get_cu_transquant_bypass_p2 ((x265_base_data_cu_t*)cu, i_abs_part_idx);
	enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, symbol, X265_CU_TRANSQUANT_BYPASS_FLAG_START_IDX );
}

#if 0
#define DTRACE_CABAC_F(x)     if ( ( h->symbol_counter >= X265_COUNTER_START && h->symbol_counter <= X265_COUNTER_END )|| h->just_do_it ) fprintf( h->trace, "%f", x );
#define DTRACE_CABAC_V(x)     if ( ( h->symbol_counter >= X265_COUNTER_START && h->symbol_counter <= X265_COUNTER_END )|| h->just_do_it ) fprintf( h->trace, "%d", x );
#define DTRACE_CABAC_VL(x)    if ( ( h->symbol_counter >= X265_COUNTER_START && h->symbol_counter <= X265_COUNTER_END )|| h->just_do_it ) fprintf( h->trace, "%lld", x );
#define DTRACE_CABAC_T(x)     if ( ( h->symbol_counter >= X265_COUNTER_START && h->symbol_counter <= X265_COUNTER_END )|| h->just_do_it ) fprintf( h->trace, "%s", x );
#define DTRACE_CABAC_X(x)     if ( ( h->symbol_counter >= X265_COUNTER_START && h->symbol_counter <= X265_COUNTER_END )|| h->just_do_it ) fprintf( h->trace, "%x", x );
#define DTRACE_CABAC_R( x,y ) if ( ( h->symbol_counter >= X265_COUNTER_START && h->symbol_counter <= X265_COUNTER_END )|| h->just_do_it ) fprintf( h->trace, x,    y );
#define DTRACE_CABAC_N        if ( ( h->symbol_counter >= X265_COUNTER_START && h->symbol_counter <= X265_COUNTER_END )|| h->just_do_it ) fprintf( h->trace, "\n"    );
#endif
/** code skip flag
 * \param cu_
 * \param i_abs_part_idx
 * \returns void
 */
void x265_enc_sbac_code_skip_flag( x265_t* h,
									x265_enc_entropy_if_t* enc_entropy_if,
									x265_data_cu_t* cu,
									uint32_t i_abs_part_idx )
{
	// get context function is here
	uint32_t symbol = 0 ;
	uint32_t ctx_skip = 0 ;
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	symbol = x265_base_data_cu_is_skipped ((x265_base_data_cu_t*)cu, i_abs_part_idx ) ? 1 : 0;
	ctx_skip = x265_data_cu_get_ctx_skip_flag(h, cu, i_abs_part_idx ) ;
	enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, symbol, X265_CU_SKIP_FLAG_START_IDX + ctx_skip );
}

/** code merge flag
 * \param cu_
 * \param i_abs_part_idx
 * \returns void
 */
void x265_enc_sbac_code_merge_flag( x265_t* h,
									x265_enc_entropy_if_t* enc_entropy_if,
									x265_data_cu_t* cu,
									uint32_t i_abs_part_idx )
{
	uint32_t symbol = 0 ;

	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	symbol = x265_data_cu_get_merge_flag_p2 (cu, i_abs_part_idx ) ? 1 : 0;
	enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, symbol, X265_CU_MERGE_FLAG_EXT_START_IDX );
}

/** code merge index
 * \param cu_
 * \param i_abs_part_idx
 * \returns void
 */
void x265_enc_sbac_code_merge_index( x265_t* h,
									x265_enc_entropy_if_t* enc_entropy_if,
									x265_data_cu_t* cu,
									uint32_t i_abs_part_idx )
{
	uint32_t unary_idx = 0 ;
	uint32_t num_cand = 0 ;
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	unary_idx = x265_data_cu_get_merge_index_p2 (cu, i_abs_part_idx );
	num_cand = h->slice->i_max_num_merge_cand ;
	if ( num_cand > 1 )
	{
		for( uint32_t ui = 0; ui < num_cand - 1; ++ui )
		{
			const uint32_t symbol = ui == unary_idx ? 0 : 1;
			if ( ui==0 )
			{
				enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, symbol, X265_CU_MERGE_IDX_EXT_START_IDX );
			}
			else
			{
				enc_sbac->enc_bin_if->encode_bin_ep( enc_sbac->enc_bin_if, symbol );
			}
			if( symbol == 0 )
			{
				break;
			}
		}
	}
}

void x265_enc_sbac_code_split_flag   ( x265_t* h,
										x265_enc_entropy_if_t* enc_entropy_if,
										x265_data_cu_t* cu,
										uint32_t i_abs_part_idx,
										uint32_t depth )
{
	uint32_t ctx = 0 ;
	uint32_t curr_split_flag = 0 ;
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	if( depth == h->cu.pic.i_total_depth - h->cu.pic.i_add_cu_depth )
	{
		return;
	}

	ctx = x265_data_cu_get_ctx_split_flag( h, cu, i_abs_part_idx, depth );
	curr_split_flag = ( x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_part_idx ) > depth ) ? 1 : 0;

	assert( ctx < 3 );
	enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, curr_split_flag, X265_CU_SPLIT_FLAG_START_IDX + ctx);
	return;
}

void x265_enc_sbac_code_transform_sub_div_flag( x265_t* h,
												x265_enc_entropy_if_t* enc_entropy_if,
												uint32_t symbol,
												uint32_t ctx )
{
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, symbol, X265_CU_TRANS_SUBDIV_FLAG_START_IDX + ctx );
}

void x265_enc_sbac_code_intra_dir_luma_ang( x265_t *h,
											x265_enc_entropy_if_t* enc_entropy_if,
											x265_data_cu_t* cu,
											uint32_t i_abs_part_idx,
											int32_t b_is_multiple)
{
	uint32_t dir[4],j;
	int32_t preds[4][3] = {{-1, -1, -1},{-1, -1, -1},{-1, -1, -1},{-1, -1, -1}};
	int32_t pred_num[4], pred_idx[4] ={ -1,-1,-1,-1};
	enum part_size_e mode = 0 ;
	uint32_t part_num = 0 ;
	uint32_t part_offset = 0 ;
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	mode = x265_data_cu_get_partition_size_p2 (cu, i_abs_part_idx );
	part_num = b_is_multiple?(mode==SIZE_NxN?4:1):1;
	part_offset = ( h->cu.pic.i_num_partitions >> ( x265_base_data_cu_get_depth_p2 ((x265_base_data_cu_t*)cu, i_abs_part_idx) << 1 ) ) >> 2;
	for (j=0;j<part_num;j++)
	{
		dir[j] = x265_base_data_cu_get_luma_intra_dir_p2((x265_base_data_cu_t*)cu, i_abs_part_idx+part_offset*j );
		pred_num[j] = x265_data_cu_get_intra_dir_luma_predictor ( h,
																cu,
																i_abs_part_idx+part_offset*j,
																preds[j],
																NULL );
		for(uint32_t i = 0; i < pred_num[j]; i++)
		{
			if(dir[j] == preds[j][i])
			{
				pred_idx[j] = i;
			}
		}
		enc_sbac->enc_bin_if->encode_bin(enc_sbac->enc_bin_if, (pred_idx[j] != -1)? 1 : 0, X265_CU_INTRA_PRED_START_IDX );
	}
	for (j=0;j<part_num;j++)
	{
		if(pred_idx[j] != -1)
		{
			enc_sbac->enc_bin_if->encode_bin_ep( enc_sbac->enc_bin_if, pred_idx[j] ? 1 : 0 );
			if (pred_idx[j])
			{
				enc_sbac->enc_bin_if->encode_bin_ep( enc_sbac->enc_bin_if, pred_idx[j]-1 );
			}
		}
		else
		{
			if (preds[j][0] > preds[j][1])
			{
				x265_enc_sbac_swap_int32( &preds[j][0], &preds[j][1] ) ;
			}
			if (preds[j][0] > preds[j][2])
			{
				x265_enc_sbac_swap_int32( &preds[j][0], &preds[j][2] ) ;
			}
			if (preds[j][1] > preds[j][2])
			{
				x265_enc_sbac_swap_int32( &preds[j][1], &preds[j][2] ) ;
			}
			for(int32_t i = (pred_num[j] - 1); i >= 0; i--)
			{
				dir[j] = dir[j] > preds[j][i] ? dir[j] - 1 : dir[j];
			}
			enc_sbac->enc_bin_if->encode_bins_ep( enc_sbac->enc_bin_if, dir[j], 5 );
		}
	}
	return;
}

void x265_enc_sbac_code_intra_dir_chroma( x265_enc_entropy_if_t* enc_entropy_if,
											x265_data_cu_t* cu,
											uint32_t i_abs_part_idx )
{
	uint32_t intra_dir_chroma = 0 ;
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	intra_dir_chroma = x265_data_cu_get_chroma_intra_dir_p2 ( cu,  i_abs_part_idx );

	if( intra_dir_chroma == X265_DM_CHROMA_IDX )
	{
		enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, 0, X265_CU_CHROMA_PRED_START_IDX );
	}
	else
	{
		uint32_t allowed_chroma_dir[ X265_NUM_CHROMA_MODE ];
		x265_data_cu_get_allowed_chroma_dir (cu, i_abs_part_idx, allowed_chroma_dir );

		for( int32_t i = 0; i < X265_NUM_CHROMA_MODE - 1; i++ )
		{
			if( intra_dir_chroma == allowed_chroma_dir[i] )
			{
				intra_dir_chroma = i;
				break;
			}
		}
		enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, 1, X265_CU_CHROMA_PRED_START_IDX );

		enc_sbac->enc_bin_if->encode_bins_ep( enc_sbac->enc_bin_if, intra_dir_chroma, 2 );
	}
	return;
}

void x265_enc_sbac_code_inter_dir( x265_enc_entropy_if_t* enc_entropy_if,
									x265_data_cu_t* cu,
									uint32_t i_abs_part_idx )
{
	uint32_t inter_dir = 0 ;
	uint32_t ctx = 0 ;
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	inter_dir = x265_base_data_cu_get_inter_dir_p2 ((x265_base_data_cu_t*)cu, i_abs_part_idx ) - 1;
	ctx= x265_data_cu_get_ctx_inter_dir(cu, i_abs_part_idx );
	//x265_context_model_t *p_ctx    = enc_sbac->cu_inter_dir_sc_model.get( 0 );
	if (x265_data_cu_get_partition_size_p2 (cu, i_abs_part_idx) == SIZE_2Nx2N
		|| x265_data_cu_get_height_p2 (cu, i_abs_part_idx) != 8 )
	{
		enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, inter_dir == 2 ? 1 : 0, X265_CU_INTER_DIR_START_IDX + ctx );
	}
	if (inter_dir < 2)
	{
		enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, inter_dir, X265_CU_INTER_DIR_START_IDX + 4 );
	}
	return;
}

void x265_enc_sbac_code_ref_frm_idx( x265_t *h,
									x265_enc_entropy_if_t* enc_entropy_if,
									x265_data_cu_t* cu,
									uint32_t i_abs_part_idx,
									enum ref_pic_list_e i_ref_list )
{
	int32_t i_ref_frame = x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, i_ref_list )->ref_idx[i_abs_part_idx] ;
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	//x265_context_model_t *p_ctx = enc_sbac->cu_ref_pic_sc_model.get( 0 );
	enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, ( i_ref_frame == 0 ? 0 : 1 ), X265_CU_REF_PIC_START_IDX );

	if( i_ref_frame > 0 )
	{
		uint32_t ref_num = h->i_ref[i_ref_list] - 2;
		//p_ctx++;
		i_ref_frame--;
		for( uint32_t ui = 0; ui < ref_num; ++ui )
		{
			const uint32_t symbol = ui == i_ref_frame ? 0 : 1;
			if( ui == 0 )
			{
				enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, symbol, X265_CU_REF_PIC_START_IDX + 1 );
			}
			else
			{
				enc_sbac->enc_bin_if->encode_bin_ep( enc_sbac->enc_bin_if, symbol );
			}
			if( symbol == 0 )
			{
				break;
			}
		}
	}

	return;
}

void x265_enc_sbac_code_mvd( x265_t *h,
							x265_enc_entropy_if_t* enc_entropy_if,
							x265_data_cu_t* cu,
							uint32_t i_abs_part_idx,
							enum ref_pic_list_e i_ref_list )
{
	x265_cu_mv_field_t* cu_mv_field = NULL ;
	int32_t i_hor = 0 ;
	int32_t i_ver = 0 ;
	int32_t b_hor_abs_gr0 = 0 ;
	int32_t b_ver_abs_gr0 = 0 ;
	uint32_t hor_abs = 0 ;
	uint32_t ver_abs = 0 ;
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	if ( h->slice->b_l_mvd_l1_zero && i_ref_list == REF_PIC_LIST_1
		&& x265_base_data_cu_get_inter_dir_p2((x265_base_data_cu_t*)cu, i_abs_part_idx)==3)
	{
		return;
	}

	cu_mv_field = x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, i_ref_list ) ;
	i_hor = cu_mv_field->mvd[i_abs_part_idx].i_hor;
	i_ver = cu_mv_field->mvd[i_abs_part_idx].i_ver;
	//x265_context_model_t* p_ctx = cu_mvd_sc_model.get( 0 );

	enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, i_hor != 0 ? 1 : 0, X265_CU_MVD_START_IDX );
	enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, i_ver != 0 ? 1 : 0, X265_CU_MVD_START_IDX );

	b_hor_abs_gr0 = i_hor != 0;
	b_ver_abs_gr0 = i_ver != 0;
	hor_abs   = 0 > i_hor ? -i_hor : i_hor;
	ver_abs   = 0 > i_ver ? -i_ver : i_ver;
	//p_ctx++;

	if( b_hor_abs_gr0 )
	{
		enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, hor_abs > 1 ? 1 : 0, X265_CU_MVD_START_IDX + 1 );
	}

	if( b_ver_abs_gr0 )
	{
		enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, ver_abs > 1 ? 1 : 0, X265_CU_MVD_START_IDX + 1 );
	}

	if( b_hor_abs_gr0 )
	{
		if( hor_abs > 1 )
		{
			x265_enc_sbac_x_write_ep_ex_golomb( enc_sbac, hor_abs-2, 1 );
		}

		enc_sbac->enc_bin_if->encode_bin_ep( enc_sbac->enc_bin_if, 0 > i_hor ? 1 : 0 );
	}

	if( b_ver_abs_gr0 )
	{
		if( ver_abs > 1 )
		{
			x265_enc_sbac_x_write_ep_ex_golomb( enc_sbac, ver_abs-2, 1 );
		}

		enc_sbac->enc_bin_if->encode_bin_ep( enc_sbac->enc_bin_if, 0 > i_ver ? 1 : 0 );
	}

	return;
}

void x265_enc_sbac_code_delta_qp( x265_t *h,
								x265_enc_entropy_if_t* enc_entropy_if,
								x265_data_cu_t* cu,
								uint32_t i_abs_part_idx )
{
	int32_t i_dqp = 0 ;
	int32_t i_qp_bd_offset_y = 0 ;
	//	cu->get_qp( i_abs_part_idx ) - cu->get_ref_qp( i_abs_part_idx );
	uint32_t abs_dqp = 0 ;
	uint32_t tu_value = 0 ;
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	i_dqp = x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu, i_abs_part_idx)
			- x265_data_cu_get_ref_qp ( h, cu, i_abs_part_idx ) ;
	i_qp_bd_offset_y = h->sps[0].i_qp_bd_offset_y ;
	i_dqp = (i_dqp + 78 + i_qp_bd_offset_y + (i_qp_bd_offset_y / 2))
				% (52 + i_qp_bd_offset_y) - 26 - (i_qp_bd_offset_y/2);

	abs_dqp = (uint32_t)((i_dqp > 0)? i_dqp  : (-i_dqp));
	tu_value = X265_MIN((int32_t)abs_dqp, X265_CU_DQP_TU_CMAX);


	x265_enc_sbac_x_write_unary_max_symbol( enc_sbac, tu_value, X265_CU_DELTA_QP_START_IDX, 1, X265_CU_DQP_TU_CMAX);
	if( abs_dqp >= X265_CU_DQP_TU_CMAX )
	{
		x265_enc_sbac_x_write_ep_ex_golomb( enc_sbac, abs_dqp - X265_CU_DQP_TU_CMAX, X265_CU_DQP_EG_k );
	}

	if ( abs_dqp > 0)
	{
		uint32_t sign = (i_dqp > 0 ? 0 : 1);
		enc_sbac->enc_bin_if->encode_bin_ep( enc_sbac->enc_bin_if, sign ) ;
	}

	return;
}

void x265_enc_sbac_code_qt_cbf( x265_t* h,
								x265_enc_entropy_if_t* enc_entropy_if,
								x265_data_cu_t* cu,
								uint32_t i_abs_part_idx,
								enum text_type_e i_text_type,
								uint32_t i_tr_depth )
{
	uint32_t cbf = 0 ;
	uint32_t ctx = 0 ;
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	cbf = x265_base_data_cu_get_cbf_p4( (x265_base_data_cu_t*)cu,
										i_abs_part_idx,
										i_text_type,
										i_tr_depth ) ;
	ctx = x265_data_cu_get_ctx_qt_cbf( cu, i_text_type, i_tr_depth );
	enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, cbf , X265_CU_QT_CBF_START_IDX + X265_NUM_QT_CBF_CTX * (i_text_type ? TEXT_CHROMA : i_text_type) + ctx );
}

void x265_enc_sbac_code_transform_skip_flags ( x265_t* h,
												x265_enc_entropy_if_t* enc_entropy_if,
												x265_data_cu_t* cu,
												uint32_t i_abs_part_idx,
												uint32_t i_width,
												uint32_t i_height,
												enum text_type_e i_text_type )
{
	uint32_t b_use_transform_skip = 0 ;
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	if ( x265_base_data_cu_get_cu_transquant_bypass_p2( (x265_base_data_cu_t*)cu, i_abs_part_idx))
	{
		return;
	}
	if ( i_width != 4 || i_height != 4)
	{
		return;
	}

	b_use_transform_skip = x265_data_cu_get_transform_skip_p3(cu, i_abs_part_idx,i_text_type);
	enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, b_use_transform_skip, X265_TRANSFORM_SKIP_START_IDX + (i_text_type ? TEXT_CHROMA: TEXT_LUMA) );
}

/** code i_pcm information.
 * \param cu pointer to cu
 * \param i_abs_part_idx cu index
 * \returns void
 */
void x265_enc_sbac_code_ipcm_info( x265_t *h,
									x265_enc_entropy_if_t* enc_entropy_if,
									x265_data_cu_t* cu,
									uint32_t i_abs_part_idx )
{
	uint32_t b_ipcm = 0 ;
	int32_t b_write_pcm_sample_flag = 0 ;
	uint32_t i_min_coeff_size = 0 ;
	uint32_t i_luma_offset = 0 ;
	uint32_t i_chroma_offset = 0 ;
	pixel *pcm_sample = NULL ;
	uint32_t i_width = 0 ;
	uint32_t i_height = 0 ;
	uint32_t i_sample_bits = 0 ;
	uint32_t i_sample = 0 ;
	uint32_t x = 0, y = 0;
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	b_ipcm = (x265_base_data_cu_get_ipcm_flag_p2((x265_base_data_cu_t*)cu, i_abs_part_idx) == 1) ? 1 : 0;
	b_write_pcm_sample_flag = x265_base_data_cu_get_ipcm_flag_p2((x265_base_data_cu_t*)cu, i_abs_part_idx);
	enc_sbac->enc_bin_if->encode_bin_trm (enc_sbac->enc_bin_if, b_ipcm);

	if ( b_write_pcm_sample_flag )
	{
		enc_sbac->enc_bin_if->encode_pcm_align_bits(enc_sbac->enc_bin_if);

		i_min_coeff_size = h->cu.pic.i_min_cu_width * h->cu.pic.i_min_cu_height ;
		i_luma_offset   = i_min_coeff_size * i_abs_part_idx ;
		i_chroma_offset = i_luma_offset >> 2 ;

		pcm_sample = x265_data_cu_get_pcm_sample_y(cu) + i_luma_offset;
		i_width = x265_data_cu_get_width_p2(cu, i_abs_part_idx);
		i_height = x265_data_cu_get_height_p2(cu, i_abs_part_idx);
		i_sample_bits = h->sps[0].i_pcm_bit_depth_luma ;

		for(y = 0; y < i_height; y++)
		{
			for(x = 0; x < i_width; x++)
			{
				i_sample = pcm_sample[x];
				enc_sbac->enc_bin_if->x_write_pcm_code(enc_sbac->enc_bin_if, i_sample, i_sample_bits);
			}
			pcm_sample += i_width;
		}

		pcm_sample = x265_data_cu_get_pcm_sample_cb(cu) + i_chroma_offset;
		i_width = x265_data_cu_get_width_p2(cu, i_abs_part_idx) / 2 ;
		i_height = x265_data_cu_get_height_p2(cu, i_abs_part_idx) / 2 ;
		i_sample_bits = h->sps[0].i_pcm_bit_depth_chroma ;

		for(y = 0; y < i_height; y++)
		{
			for(x = 0; x < i_width; x++)
			{
				i_sample = pcm_sample[x];
				enc_sbac->enc_bin_if->x_write_pcm_code(enc_sbac->enc_bin_if, i_sample, i_sample_bits);
			}
			pcm_sample += i_width;
		}

		pcm_sample = x265_data_cu_get_pcm_sample_cr(cu) + i_chroma_offset;
		i_width = x265_data_cu_get_width_p2(cu, i_abs_part_idx) / 2 ;
		i_height = x265_data_cu_get_height_p2(cu, i_abs_part_idx) / 2 ;
		i_sample_bits = h->sps[0].i_pcm_bit_depth_chroma ;

		for(y = 0; y < i_height; y++)
		{
			for(x = 0; x < i_width; x++)
			{
				i_sample = pcm_sample[x];
				enc_sbac->enc_bin_if->x_write_pcm_code(enc_sbac->enc_bin_if, i_sample, i_sample_bits);
			}
			pcm_sample += i_width;
		}
		enc_sbac->enc_bin_if->reset_bac(enc_sbac->enc_bin_if);
	}
}

void x265_enc_sbac_code_qt_root_cbf( x265_enc_entropy_if_t* enc_entropy_if,
									x265_data_cu_t* cu,
									uint32_t i_abs_part_idx )
{
	uint32_t i_cbf = 0 ;
	uint32_t i_ctx = 0;
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	i_cbf = x265_base_data_cu_get_qt_root_cbf((x265_base_data_cu_t*)cu, i_abs_part_idx );
	enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, i_cbf , X265_CU_QT_ROOT_CBF_START_IDX + i_ctx );
}

void x265_enc_sbac_code_qt_cbf_zero( x265_enc_entropy_if_t* enc_entropy_if,
									x265_data_cu_t* cu,
									enum text_type_e i_text_type,
									uint32_t i_tr_depth )
{
	// this function is only used to estimate the bits when cbf is 0
	// and will never be called when writing the bistream. do not need to write log
	uint32_t i_cbf = 0 ;
	uint32_t i_ctx = 0 ;
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	i_ctx = x265_data_cu_get_ctx_qt_cbf( cu, i_text_type, i_tr_depth );
	enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, i_cbf , X265_CU_QT_CBF_START_IDX + X265_NUM_QT_CBF_CTX * (i_text_type ? TEXT_CHROMA : i_text_type) + i_ctx );
}

void x265_enc_sbac_code_qt_root_cbf_zero( x265_enc_entropy_if_t* enc_entropy_if,
										x265_data_cu_t* cu )
{
	// this function is only used to estimate the bits when cbf is 0
	// and will never be called when writing the bistream. do not need to write log
	uint32_t cbf = 0;
	uint32_t ctx = 0;
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, cbf , X265_CU_QT_ROOT_CBF_START_IDX + ctx );
}

/** encode (x,y) position of the last significant coefficient
 * \param pos_x x component of last coefficient
 * \param pos_y y component of last coefficient
 * \param width  block width
 * \param height block height
 * \param i_text_type plane type / luminance or chrominance
 * \param scan_idx scan type (zig-zag, hor, ver)
 * this method encodes the x and y component within a block of the last significant coefficient.
 */
void x265_enc_sbac_code_last_significant_xy( x265_t* h,
											x265_enc_entropy_if_t* enc_entropy_if,
											uint32_t i_pos_x,
											uint32_t i_pos_y,
											int32_t i_width,
											int32_t i_height,
											enum text_type_e i_text_type,
											uint32_t i_scan_idx )
{
	int32_t loop = 0 ;
	uint32_t i_ctx_last = 0 ;
	uint32_t i_ctx_start_x = 0 ;
	uint32_t i_ctx_start_y = 0 ;
	uint32_t i_group_idx_x = 0 ;
	uint32_t i_group_idx_y = 0 ;
	uint32_t i_count = 0 ;
	int32_t i_blk_size_offset_x = 0, i_blk_size_offset_y = 0 ;
	int32_t i_shift_x = 0, i_shift_y = 0;
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	// swap
	if( i_scan_idx == SCAN_VER )
	{
		x265_enc_sbac_swap_uint32( &i_pos_x, &i_pos_y );
	}

	i_ctx_start_x = X265_CU_CTX_LAST_X_START_IDX + X265_NUM_CTX_LAST_FLAG_XY * i_text_type;
	i_ctx_start_y = X265_CU_CTX_LAST_Y_START_IDX + X265_NUM_CTX_LAST_FLAG_XY * i_text_type;
	i_group_idx_x = group_idx[ i_pos_x ];
	i_group_idx_y = group_idx[ i_pos_y ];


	i_blk_size_offset_x = i_text_type ? 0: (h->global.convert_to_bit[ i_width ] *3 + ((h->global.convert_to_bit[ i_width ] +1)>>2));
	i_blk_size_offset_y = i_text_type ? 0: (h->global.convert_to_bit[ i_height ]*3 + ((h->global.convert_to_bit[ i_height ]+1)>>2));
	i_shift_x = i_text_type ? h->global.convert_to_bit[ i_width  ] :((h->global.convert_to_bit[ i_width  ]+3)>>2);
	i_shift_y = i_text_type ? h->global.convert_to_bit[ i_height ] :((h->global.convert_to_bit[ i_height ]+3)>>2);
	// pos_x
	for( i_ctx_last = 0; i_ctx_last < i_group_idx_x; i_ctx_last++ )
	{
		enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, 1, i_ctx_start_x + i_blk_size_offset_x + (i_ctx_last >> i_shift_x) );
	}
	if( i_group_idx_x < group_idx[ i_width - 1 ])
	{
		enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, 0, i_ctx_start_x + i_blk_size_offset_x + (i_ctx_last >> i_shift_x) );
	}

	// pos_y
	for( i_ctx_last = 0; i_ctx_last < i_group_idx_y; i_ctx_last++ )
	{
		enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, 1, i_ctx_start_y + i_blk_size_offset_y + (i_ctx_last >> i_shift_y) );
	}
	if( i_group_idx_y < group_idx[ i_height - 1 ])
	{
		enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, 0, i_ctx_start_y + i_blk_size_offset_y + (i_ctx_last >> i_shift_y) );
	}
	if ( i_group_idx_x > 3 )
	{
		i_count = ( i_group_idx_x - 2 ) >> 1;
		i_pos_x = i_pos_x - min_in_group[ i_group_idx_x ];
		for (loop = i_count - 1 ; loop >= 0; loop-- )
		{
			enc_sbac->enc_bin_if->encode_bin_ep( enc_sbac->enc_bin_if, ( i_pos_x >> loop ) & 1 );
		}
	}
	if ( i_group_idx_y > 3 )
	{
		i_count = ( i_group_idx_y - 2 ) >> 1;
		i_pos_y = i_pos_y - min_in_group[ i_group_idx_y ];
		for ( loop = i_count - 1 ; loop >= 0; loop-- )
		{
			enc_sbac->enc_bin_if->encode_bin_ep( enc_sbac->enc_bin_if, ( i_pos_y >> loop ) & 1 );
		}
	}
}

void x265_enc_sbac_code_coeff_nxn( x265_t* h,
									x265_enc_entropy_if_t* enc_entropy_if,
									x265_data_cu_t* cu,
									x265_coeff_t* coef,
									uint32_t i_abs_part_idx,
									uint32_t i_width,
									uint32_t i_height,
									uint32_t depth,
									enum text_type_e i_text_type )
{
	uint32_t i_num_sig = 0;
	//----- encode significance map -----
	uint32_t i_log2_block_size = 0 ;
	uint32_t i_scan_idx = 0 ;
	uint32_t *scan = NULL;
	int32_t b_be_valid = 0 ;
	uint32_t * scan_cg = NULL ;
	uint32_t sig_coeff_group_flag[ X265_MLS_GRP_NUM ];
	uint32_t i_shift = 0 ;
	uint32_t i_num_blk_side = 0 ;
	// find position of last coefficient
	int32_t i_scan_pos_last = 0 ;
	int32_t i_pos_last = 0 ;
	uint32_t i_pos_y = 0 ;
	uint32_t i_pos_x = 0 ;
	uint32_t i_blk_idx = 0 ;
	int32_t i_pos_last_y = 0 ;
	int32_t i_pos_last_x = 0 ;
	uint32_t i_base_coeff_group_ctx = 0 ;
	uint32_t i_base_ctx = 0 ;
	int32_t i_last_scan_set = 0 ;
	uint32_t i_c1 = 0 ;
	uint32_t i_go_rice_param = 0 ;
	int32_t i_scan_pos_sig = 0 ;
	int32_t i_sub_set = 0 ;
	int32_t i_num_non_zero = 0;
	int32_t i_sub_pos = 0 ;
	int32_t abs_coeff[16];
	uint32_t i_coeff_signs = 0;
	int32_t i_last_nz_pos_in_cg = 0, i_first_nz_pos_in_cg = 0 ;
	int32_t i_cg_blk_pos = 0 ;
	int32_t i_cg_pos_y = 0 ;
	int32_t i_cg_pos_x = 0 ;
	uint32_t i_sig_coeff_group = 0 ;
	uint32_t i_ctx_sig = 0 ;
	int32_t i_pattern_sig_ctx = 0 ;
	uint32_t i_blk_pos = 0, i_sig = 0 ;
	int32_t b_sign_hidden = 0 ;
	uint32_t i_ctx_set = 0 ;
	uint32_t i_base_ctx_mod = 0 ;
	int32_t i_num_c1_flag = 0 ;
	int32_t i_first_c2_flag_idx = 0 ;
	int32_t i_idx = 0 ;
	uint32_t i_symbol = 0 ;
	int32_t i_first_coeff2 = 0 ;
	uint32_t i_base_level = 0 ;

	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*) enc_entropy_if ;
	if( i_width > h->sps[0].i_max_tr_size )
	{
		i_width  = h->sps[0].i_max_tr_size ;
		i_height = h->sps[0].i_max_tr_size ;
	}


	// compute number of significant coefficients
	i_num_sig = x265_enc_entropy_count_non_zero_coeffs(&h->enc_entropy, coef, i_width * i_height);

	if ( i_num_sig == 0 )
	{
		return;
	}
	if( h->pps[0].b_use_transform_skip )
	{
		x265_enc_sbac_code_transform_skip_flags( h, enc_entropy_if, cu, i_abs_part_idx, i_width, i_height, i_text_type );
	}
	i_text_type = i_text_type == TEXT_LUMA ? TEXT_LUMA : ( i_text_type == TEXT_NONE ? TEXT_NONE : TEXT_CHROMA );

	//----- encode significance map -----
	i_log2_block_size = h->global.convert_to_bit[ i_width ] + 2;
	i_scan_idx = x265_data_cu_get_coef_scan_idx( h,
												cu,
												i_abs_part_idx,
												i_width,
												i_text_type == TEXT_LUMA,
												x265_base_data_cu_is_intra((x265_base_data_cu_t*)cu, i_abs_part_idx));
	scan = h->scan.sig_last_scan[ i_scan_idx ][ i_log2_block_size - 1 ];

	if (x265_base_data_cu_get_cu_transquant_bypass_p2( (x265_base_data_cu_t*)cu,  i_abs_part_idx))
	{
		b_be_valid = 0;
	}
	else
	{
		b_be_valid = h->pps[0].i_sign_hide_flag > 0;
	}

	// find position of last coefficient
	i_scan_pos_last = -1;


	scan_cg = h->scan.sig_last_scan[ i_scan_idx ][ i_log2_block_size > 3 ? i_log2_block_size-2-1 : 0 ];
	if( i_log2_block_size == 3 )
	{
		scan_cg = h->scan.sig_last_scan_8x8[ i_scan_idx ];
	}
	else if( i_log2_block_size == 5 )
	{
		scan_cg = h->scan.sig_last_scan_cg_32x32;
	}


	i_shift = X265_MLS_CG_SIZE >> 1;
	i_num_blk_side = i_width >> i_shift;

	memset( sig_coeff_group_flag, 0, sizeof(uint32_t) * X265_MLS_GRP_NUM );

	do
	{
		i_pos_last = scan[ ++i_scan_pos_last ];

		// get l1 sig map
		i_pos_y = i_pos_last >> i_log2_block_size;
		i_pos_x = i_pos_last - ( i_pos_y << i_log2_block_size );
		i_blk_idx = i_num_blk_side * (i_pos_y >> i_shift) + (i_pos_x >> i_shift);
		if( coef[ i_pos_last ] )
		{
			sig_coeff_group_flag[ i_blk_idx ] = 1;
		}

		i_num_sig -= ( coef[ i_pos_last ] != 0 );
	}
	while ( i_num_sig > 0 );

	// code position of last coefficient
	i_pos_last_y = i_pos_last >> i_log2_block_size;
	i_pos_last_x = i_pos_last - ( i_pos_last_y << i_log2_block_size );
	x265_enc_sbac_code_last_significant_xy(h,
											enc_entropy_if,
											i_pos_last_x,
											i_pos_last_y,
											i_width,
											i_height,
											i_text_type,
											i_scan_idx);

	//===== code significance flag =====
	i_base_coeff_group_ctx = X265_CU_SIG_COEFF_GROP_START_IDX + X265_NUM_SIG_CG_FLAG_CTX * i_text_type;
	i_base_ctx = (i_text_type==TEXT_LUMA) ? X265_CU_SIG_START_IDX : X265_CU_SIG_START_IDX + X265_NUM_SIG_FLAG_CTX_LUMA;
	i_last_scan_set = i_scan_pos_last >> X265_LOG2_SCAN_SET_SIZE;
	i_c1 = 1;
	i_go_rice_param     = 0;
	i_scan_pos_sig = i_scan_pos_last;

	for( i_sub_set = i_last_scan_set; i_sub_set >= 0; i_sub_set-- )
	{
		i_num_non_zero = 0;
		i_sub_pos = i_sub_set << X265_LOG2_SCAN_SET_SIZE;
		i_go_rice_param = 0;
		i_coeff_signs = 0;

		i_last_nz_pos_in_cg = -1 ;
		i_first_nz_pos_in_cg = X265_SCAN_SET_SIZE;

		if( i_scan_pos_sig == i_scan_pos_last )
		{
			abs_coeff[ 0 ] = abs( coef[ i_pos_last ] );
			i_coeff_signs    = ( coef[ i_pos_last ] < 0 );
			i_num_non_zero    = 1;
			i_last_nz_pos_in_cg  = i_scan_pos_sig;
			i_first_nz_pos_in_cg = i_scan_pos_sig;
			i_scan_pos_sig--;
		}

		// encode significant_coeffgroup_flag
		i_cg_blk_pos = scan_cg[ i_sub_set ];
		i_cg_pos_y   = i_cg_blk_pos / i_num_blk_side;
		i_cg_pos_x   = i_cg_blk_pos - (i_cg_pos_y * i_num_blk_side);
		if( i_sub_set == i_last_scan_set || i_sub_set == 0)
		{
			sig_coeff_group_flag[ i_cg_blk_pos ] = 1;
		}
		else
		{
			i_sig_coeff_group   = (sig_coeff_group_flag[ i_cg_blk_pos ] != 0);
			i_ctx_sig  = x265_tr_quant_get_sig_coeff_group_ctx_inc( &h->tr_quant,
																	sig_coeff_group_flag,
																	i_cg_pos_x,
																	i_cg_pos_y,
																	i_width,
																	i_height );
			enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, i_sig_coeff_group, i_base_coeff_group_ctx + i_ctx_sig  );
		}

		// encode significant_coeff_flag
		if( sig_coeff_group_flag[ i_cg_blk_pos ] )
		{
			i_pattern_sig_ctx = x265_tr_quant_calc_pattern_sig_ctx( &h->tr_quant,
																	sig_coeff_group_flag,
																	i_cg_pos_x,
																	i_cg_pos_y,
																	i_width,
																	i_height );
			for( ; i_scan_pos_sig >= i_sub_pos; i_scan_pos_sig-- )
			{
				i_blk_pos  = scan[ i_scan_pos_sig ];
				i_sig = (coef[ i_blk_pos ] != 0);
				if( i_scan_pos_sig > i_sub_pos || i_sub_set == 0 || i_num_non_zero )
				{
					i_ctx_sig  = h->tr_quant.ctx_sig_list[i_text_type][i_pattern_sig_ctx][i_scan_idx][i_log2_block_size][i_blk_pos];
					enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, i_sig, i_base_ctx + i_ctx_sig  );
				}
				if( i_sig )
				{
					abs_coeff[ i_num_non_zero ] = abs( coef[ i_blk_pos ] );
					i_coeff_signs = 2 * i_coeff_signs + ( coef[ i_blk_pos ] < 0 );
					i_num_non_zero++;
					if( i_last_nz_pos_in_cg == -1 )
					{
						i_last_nz_pos_in_cg = i_scan_pos_sig;
					}
					i_first_nz_pos_in_cg = i_scan_pos_sig;
				}
			}
		}
		else
		{
			i_scan_pos_sig = i_sub_pos - 1;
		}

		if( i_num_non_zero > 0 )
		{
			b_sign_hidden = ( i_last_nz_pos_in_cg - i_first_nz_pos_in_cg >= X265_SBH_THRESHOLD );
			i_ctx_set = (i_sub_set > 0 && i_text_type==TEXT_LUMA) ? 2 : 0;

			if( i_c1 == 0 )
			{
				i_ctx_set++;
			}
			i_c1 = 1;
			i_base_ctx_mod = ( i_text_type==TEXT_LUMA ) ? X265_CU_ONE_START_IDX + 4 * i_ctx_set : X265_CU_ONE_START_IDX + X265_NUM_ONE_FLAG_CTX_LUMA + 4 * i_ctx_set;

			i_num_c1_flag = X265_MIN(i_num_non_zero, X265_C1FLAG_NUMBER);
			i_first_c2_flag_idx = -1;
			for( i_idx = 0; i_idx < i_num_c1_flag; i_idx++ )
			{
				i_symbol = abs_coeff[ i_idx ] > 1;
				enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, i_symbol, i_base_ctx_mod + i_c1 );
				if( i_symbol )
				{
					i_c1 = 0;

					if (i_first_c2_flag_idx == -1)
					{
						i_first_c2_flag_idx = i_idx;
					}
				}
				else if( (i_c1 < 3) && (i_c1 > 0) )
				{
					i_c1++;
				}
			}

			if (i_c1 == 0)
			{

				i_base_ctx_mod = ( i_text_type==TEXT_LUMA ) ? X265_CU_ABS_START_IDX + i_ctx_set : X265_CU_ABS_START_IDX + X265_NUM_ABS_FLAG_CTX_LUMA + i_ctx_set;
				if ( i_first_c2_flag_idx != -1)
				{
					i_symbol = abs_coeff[ i_first_c2_flag_idx ] > 2;
					enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, i_symbol, i_base_ctx_mod + 0 );
				}
			}

			if( b_be_valid && b_sign_hidden )
			{
				enc_sbac->enc_bin_if->encode_bins_ep( enc_sbac->enc_bin_if, (i_coeff_signs >> 1), i_num_non_zero-1 );
			}
			else
			{
				enc_sbac->enc_bin_if->encode_bins_ep( enc_sbac->enc_bin_if, i_coeff_signs, i_num_non_zero );
			}

			i_first_coeff2 = 1 ;
			if (i_c1 == 0 || i_num_non_zero > X265_C1FLAG_NUMBER)
			{
				for ( i_idx = 0; i_idx < i_num_non_zero; i_idx++ )
				{
					i_base_level  = (i_idx < X265_C1FLAG_NUMBER)? (2 + i_first_coeff2 ) : 1;

					if( abs_coeff[ i_idx ] >= i_base_level)
					{
						x265_enc_sbac_x_write_coef_remain_ex_golomb( enc_sbac,
																	abs_coeff[ i_idx ] - i_base_level,
																	i_go_rice_param );
						if(abs_coeff[i_idx] > 3*(1 << i_go_rice_param))
						{
							i_go_rice_param = X265_MIN(i_go_rice_param+ 1, 4);
						}
					}
					if(abs_coeff[ i_idx ] >= 2)
					{
						i_first_coeff2 = 0;
					}
				}
			}
		}
	}

	return;
}

void x265_enc_sbac_code_sao_sign( x265_enc_entropy_if_t* enc_entropy_if,
									uint32_t code )
{
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*)enc_entropy_if ;
	enc_sbac->enc_bin_if->encode_bin_ep( enc_sbac->enc_bin_if, code );
}

void x265_enc_sbac_code_sao_max_uvlc( x265_enc_entropy_if_t* enc_entropy_if,
									uint32_t code,
									uint32_t max_symbol )
{
	int32_t loop = 0 ;
	int32_t b_code_last = 0 ;
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*)enc_entropy_if ;
	if (max_symbol == 0)
	{
		return;
	}

	b_code_last = ( max_symbol > code );
	if ( code == 0 )
	{
		enc_sbac->enc_bin_if->encode_bin_ep( enc_sbac->enc_bin_if, 0 );
	}
	else
	{
		enc_sbac->enc_bin_if->encode_bin_ep( enc_sbac->enc_bin_if, 1 );
		for ( loop = 0 ; loop < code - 1; ++ loop )
		{
			enc_sbac->enc_bin_if->encode_bin_ep( enc_sbac->enc_bin_if, 1 );
		}
		if( b_code_last )
		{
			enc_sbac->enc_bin_if->encode_bin_ep( enc_sbac->enc_bin_if, 0 );
		}
	}
}


void x265_enc_sbac_code_sao_uflc ( x265_enc_entropy_if_t* enc_entropy_if,
									uint32_t length,
									uint32_t code )
{
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*)enc_entropy_if ;
	enc_sbac->enc_bin_if->encode_bins_ep ( enc_sbac->enc_bin_if, code, length );
}

void x265_enc_sbac_code_sao_merge ( x265_enc_entropy_if_t* enc_entropy_if,
									uint32_t code )
{
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*)enc_entropy_if ;
	if (code == 0)
	{
		enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, 0,  X265_SAO_MERGE_START_IDX);
	}
	else
	{
		enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, 1,  X265_SAO_MERGE_START_IDX);
	}
}

void x265_enc_sbac_code_sao_type_idx ( x265_enc_entropy_if_t* enc_entropy_if,
										uint32_t code)
{
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*)enc_entropy_if ;
	if (code == 0)
	{
		enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, 0, X265_SAO_TYPE_START_IDX );
	}
	else
	{
		enc_sbac->enc_bin_if->encode_bin( enc_sbac->enc_bin_if, 1, X265_SAO_TYPE_START_IDX );
		enc_sbac->enc_bin_if->encode_bin_ep( enc_sbac->enc_bin_if, code <= 4 ? 1 : 0 );
	}
}


void x265_enc_sbac_est_bit( x265_t *h,
							x265_enc_entropy_if_t *enc_entropy_if,
							x265_est_bits_sbac_struct_t* est_bits_sbac,
							int32_t i_width,
							int32_t i_height,
							enum text_type_e i_text_type )
{
	x265_enc_sbac_t *enc_sbac = NULL ;

	enc_sbac = (x265_enc_sbac_t*)enc_entropy_if ;
	x265_enc_sbac_est_cbf_bit( enc_sbac, est_bits_sbac );

	x265_enc_sbac_est_significant_coeff_group_map_bit( enc_sbac,
														est_bits_sbac,
														i_text_type );

	// encode significance map
	x265_enc_sbac_est_significant_map_bit( h,
											enc_sbac,
											est_bits_sbac,
											i_width,
											i_height,
											i_text_type );

	// encode significant coefficients
	x265_enc_sbac_est_significant_coefficients_bit( enc_sbac,
													est_bits_sbac,
													i_text_type );
}


void x265_enc_sbac_est_cbf_bit( x265_enc_sbac_t* enc_sbac,
								x265_est_bits_sbac_struct_t* est_bits_sbac )
{
	int32_t i_ctx_start = 0 ;
	int32_t i_ctx_inc = 0 ;

	i_ctx_start = X265_CU_QT_CBF_START_IDX ;

	for( i_ctx_inc = 0; i_ctx_inc < 2 * X265_NUM_QT_CBF_CTX; i_ctx_inc++ )
	{
		est_bits_sbac->block_cbp_bits[ i_ctx_inc ][ 0 ]
		             = enc_sbac->enc_bin_if->get_entropy_bits( enc_sbac->enc_bin_if,
		            		 	 	 	 	 	 	 	 	 	 0,
		            		 	 	 	 	 	 	 	 	 	 i_ctx_start + i_ctx_inc );
		est_bits_sbac->block_cbp_bits[ i_ctx_inc ][ 1 ]
		             = enc_sbac->enc_bin_if->get_entropy_bits( enc_sbac->enc_bin_if,
		            		 	 	 	 	 	 	 	 	 	 1,
		            		 	 	 	 	 	 	 	 	 	 i_ctx_start + i_ctx_inc );
	}

	i_ctx_start = X265_CU_QT_ROOT_CBF_START_IDX ;

	for( i_ctx_inc = 0; i_ctx_inc < 1; i_ctx_inc++ )
	{
		est_bits_sbac->block_root_cbp_bits[ i_ctx_inc ][ 0 ]
		             = enc_sbac->enc_bin_if->get_entropy_bits( enc_sbac->enc_bin_if,
		            		 	 	 	 	 	 	 	 	 	 0,
		            		 	 	 	 	 	 	 	 	 	 i_ctx_start + i_ctx_inc );
		est_bits_sbac->block_root_cbp_bits[ i_ctx_inc ][ 1 ]
		             = enc_sbac->enc_bin_if->get_entropy_bits( enc_sbac->enc_bin_if,
		            		 	 	 	 	 	 	 	 	 	 1,
		            		 	 	 	 	 	 	 	 	 	 i_ctx_start + i_ctx_inc );
	}
}


void x265_enc_sbac_est_significant_coeff_group_map_bit( x265_enc_sbac_t* enc_sbac,
														x265_est_bits_sbac_struct_t* est_bits_sbac,
														enum text_type_e i_text_type )
{
	int32_t i_ctx_start = 0 ;
	int32_t i_ctx_idx = 0 ;
	int32_t i_first_ctx = 0, i_num_ctx = 0 ;
	uint32_t i_bin = 0 ;

	i_ctx_start = X265_CU_SIG_COEFF_GROP_START_IDX + i_text_type * X265_NUM_SIG_CG_FLAG_CTX ;
	i_num_ctx = X265_NUM_SIG_CG_FLAG_CTX;

	for ( i_ctx_idx = i_first_ctx; i_ctx_idx < i_first_ctx + i_num_ctx; i_ctx_idx++ )
	{
		for( i_bin = 0; i_bin < 2; i_bin++ )
		{
			est_bits_sbac->significant_coeff_group_bits[ i_ctx_idx ][ i_bin ]
			             = enc_sbac->enc_bin_if->get_entropy_bits( enc_sbac->enc_bin_if,
			            		 	 	 	 	 	 	 	 	 	 i_bin,
			            		 	 	 	 	 	 	 	 	 	 i_ctx_start + i_ctx_idx );
		}
	}
}



void x265_enc_sbac_est_significant_map_bit( x265_t* h,
											x265_enc_sbac_t* enc_sbac,
											x265_est_bits_sbac_struct_t* est_bits_sbac,
											int32_t i_width,
											int32_t i_height,
											enum text_type_e i_text_type )
{
	uint32_t i_bin = 0 ;
	int32_t i_first_i_ctx = 0, i_num_i_ctx = 8;
	int32_t i_bits_x = 0, i_bits_y = 0;
	int32_t i_blk_size_offset_x = 0, i_blk_size_offset_y = 0, i_shift_x = 0, i_shift_y = 0 ;
	int32_t i_ctx = 0;
	int32_t i_ctx_idx = 0 ;
	int32_t i_ctx_offset = 0 ;
	int32_t i_ctx_start = 0 ;

	i_first_i_ctx = 1 ;
	if (X265_MAX(i_width, i_height) >= 16)
	{
		i_first_i_ctx = (i_text_type == TEXT_LUMA) ? 21 : 12;
		i_num_i_ctx = (i_text_type == TEXT_LUMA) ? 6 : 3;
	}
	else if (i_width == 8)
	{
		i_first_i_ctx = 9;
		i_num_i_ctx = (i_text_type == TEXT_LUMA) ? 12 : 3;
	}

	i_ctx_start = X265_CU_SIG_START_IDX ;
	if (i_text_type == TEXT_LUMA )
	{
		for( i_bin = 0; i_bin < 2; i_bin++ )
		{
			est_bits_sbac->significant_bits[ 0 ][ i_bin ]
			             = enc_sbac->enc_bin_if->get_entropy_bits( enc_sbac->enc_bin_if,
			            		 	 	 	 	 	 	 	 	 	 i_bin,
			            		 	 	 	 	 	 	 	 	 	 i_ctx_start + 0 );
		}

		for ( i_ctx_idx = i_first_i_ctx; i_ctx_idx < i_first_i_ctx + i_num_i_ctx; i_ctx_idx++ )
		{
			for( i_bin = 0; i_bin < 2; i_bin++ )
			{
				est_bits_sbac->significant_bits[ i_ctx_idx ][ i_bin ]
				             = enc_sbac->enc_bin_if->get_entropy_bits( enc_sbac->enc_bin_if,
				            		 	 	 	 	 	 	 	 	 	 i_bin,
				            		 	 	 	 	 	 	 	 	 	 i_ctx_start + i_ctx_idx );
			}
		}
	}
	else
	{
		for( i_bin = 0; i_bin < 2; i_bin++ )
		{
			est_bits_sbac->significant_bits[ 0 ][ i_bin ]
			             = enc_sbac->enc_bin_if->get_entropy_bits( enc_sbac->enc_bin_if,
			            		 	 	 	 	 	 	 	 	 	 i_bin,
			            		 	 	 	 	 	 	 	 	 	 i_ctx_start + X265_NUM_SIG_FLAG_CTX_LUMA + 0 );
		}
		for ( i_ctx_idx = i_first_i_ctx; i_ctx_idx < i_first_i_ctx + i_num_i_ctx; i_ctx_idx++ )
		{
			for( i_bin = 0; i_bin < 2; i_bin++ )
			{
				est_bits_sbac->significant_bits[ i_ctx_idx ][ i_bin ]
				             = enc_sbac->enc_bin_if->get_entropy_bits( enc_sbac->enc_bin_if,
				            		 	 	 	 	 	 	 	 	 	 i_bin,
				            		 	 	 	 	 	 	 	 	 	 i_ctx_start + X265_NUM_SIG_FLAG_CTX_LUMA + i_ctx_idx );
			}
		}
	}

	i_blk_size_offset_x = i_text_type ? 0: (h->global.convert_to_bit[ i_width ] *3 + ((h->global.convert_to_bit[ i_width ] +1)>>2));
	i_blk_size_offset_y = i_text_type ? 0: (h->global.convert_to_bit[ i_height ]*3 + ((h->global.convert_to_bit[ i_height ]+1)>>2));
	i_shift_x = i_text_type ? h->global.convert_to_bit[ i_width  ] :((h->global.convert_to_bit[ i_width  ]+3)>>2);
	i_shift_y = i_text_type ? h->global.convert_to_bit[ i_height ] :((h->global.convert_to_bit[ i_height ]+3)>>2);

	i_ctx_start = X265_CU_CTX_LAST_X_START_IDX + i_text_type * X265_NUM_CTX_LAST_FLAG_XY ;
	for (i_ctx = 0; i_ctx < group_idx[ i_width - 1 ]; i_ctx++)
	{
		i_ctx_offset = i_blk_size_offset_x + (i_ctx >>i_shift_x);
		est_bits_sbac->last_x_bits[ i_ctx ] = i_bits_x
					+ enc_sbac->enc_bin_if->get_entropy_bits( enc_sbac->enc_bin_if,
															0,
															i_ctx_start + i_ctx_offset );
		i_bits_x += enc_sbac->enc_bin_if->get_entropy_bits( enc_sbac->enc_bin_if,
															1,
															i_ctx_start + i_ctx_offset );
	}
	est_bits_sbac->last_x_bits[i_ctx] = i_bits_x;

	i_ctx_start = X265_CU_CTX_LAST_Y_START_IDX + i_text_type * X265_NUM_CTX_LAST_FLAG_XY ;
	for (i_ctx = 0; i_ctx < group_idx[ i_height - 1 ]; i_ctx++)
	{
		i_ctx_offset = i_blk_size_offset_y + (i_ctx >>i_shift_y);
		est_bits_sbac->last_y_bits[ i_ctx ] = i_bits_y
					+ enc_sbac->enc_bin_if->get_entropy_bits( enc_sbac->enc_bin_if,
															0,
															i_ctx_start + i_ctx_offset );
		i_bits_y += enc_sbac->enc_bin_if->get_entropy_bits( enc_sbac->enc_bin_if,
															1,
															i_ctx_start + i_ctx_offset );
	}
	est_bits_sbac->last_y_bits[i_ctx] = i_bits_y;
}

void x265_enc_sbac_est_significant_coefficients_bit( x265_enc_sbac_t* enc_sbac,
													x265_est_bits_sbac_struct_t* est_bits_sbac,
													enum text_type_e i_text_type )
{
	int32_t i_ctx_start = 0 ;
	int32_t i_ctx_idx = 0 ;
	if (i_text_type==TEXT_LUMA)
	{
		i_ctx_start = X265_CU_ONE_START_IDX ;
		for ( i_ctx_idx = 0; i_ctx_idx < X265_NUM_ONE_FLAG_CTX_LUMA; i_ctx_idx++)
		{
			est_bits_sbac->greater_one_bits[ i_ctx_idx ][ 0 ]
			             = enc_sbac->enc_bin_if->get_entropy_bits( enc_sbac->enc_bin_if,
			            		 	 	 	 	 	 	 	 	 	 0,
			            		 	 	 	 	 	 	 	 	 	 i_ctx_start + i_ctx_idx );
			est_bits_sbac->greater_one_bits[ i_ctx_idx ][ 1 ]
			             = enc_sbac->enc_bin_if->get_entropy_bits( enc_sbac->enc_bin_if,
			            		 	 	 	 	 	 	 	 	 	 1,
			            		 	 	 	 	 	 	 	 	 	 i_ctx_start + i_ctx_idx );
		}

		i_ctx_start = X265_CU_ABS_START_IDX ;
		for ( i_ctx_idx = 0; i_ctx_idx < X265_NUM_ABS_FLAG_CTX_LUMA; i_ctx_idx++)
		{
			est_bits_sbac->level_abs_bits[ i_ctx_idx ][ 0 ]
			             = enc_sbac->enc_bin_if->get_entropy_bits( enc_sbac->enc_bin_if,
			            		 	 	 	 	 	 	 	 	 	 0,
			            		 	 	 	 	 	 	 	 	 	 i_ctx_start + i_ctx_idx );
			est_bits_sbac->level_abs_bits[ i_ctx_idx ][ 1 ]
			             = enc_sbac->enc_bin_if->get_entropy_bits( enc_sbac->enc_bin_if,
			            		 	 	 	 	 	 	 	 	 	 1,
			            		 	 	 	 	 	 	 	 	 	 i_ctx_start + i_ctx_idx );
		}
	}
	else
	{
		i_ctx_start = X265_CU_ONE_START_IDX + X265_NUM_ONE_FLAG_CTX_LUMA ;
		for ( i_ctx_idx = 0; i_ctx_idx < X265_NUM_ONE_FLAG_CTX_CHROMA; i_ctx_idx++)
		{
			est_bits_sbac->greater_one_bits[ i_ctx_idx ][ 0 ]
			             = enc_sbac->enc_bin_if->get_entropy_bits( enc_sbac->enc_bin_if,
			            		 	 	 	 	 	 	 	 	 	 0,
			            		 	 	 	 	 	 	 	 	 	 i_ctx_start + i_ctx_idx );
			est_bits_sbac->greater_one_bits[ i_ctx_idx ][ 1 ]
			             = enc_sbac->enc_bin_if->get_entropy_bits( enc_sbac->enc_bin_if,
			            		 	 	 	 	 	 	 	 	 	 1,
			            		 	 	 	 	 	 	 	 	 	 i_ctx_start + i_ctx_idx );
		}

		i_ctx_start = X265_CU_ABS_START_IDX + X265_NUM_ABS_FLAG_CTX_LUMA ;
		for ( i_ctx_idx = 0; i_ctx_idx < X265_NUM_ABS_FLAG_CTX_CHROMA; i_ctx_idx++)
		{
			est_bits_sbac->level_abs_bits[ i_ctx_idx ][ 0 ]
			             = enc_sbac->enc_bin_if->get_entropy_bits( enc_sbac->enc_bin_if,
			            		 	 	 	 	 	 	 	 	 	 0,
			            		 	 	 	 	 	 	 	 	 	 i_ctx_start + i_ctx_idx );
			est_bits_sbac->level_abs_bits[ i_ctx_idx ][ 1 ]
			             = enc_sbac->enc_bin_if->get_entropy_bits( enc_sbac->enc_bin_if,
			            		 	 	 	 	 	 	 	 	 	 1,
			            		 	 	 	 	 	 	 	 	 	 i_ctx_start + i_ctx_idx );
		}
	}
}



void x265_enc_sbac_x_copy_contexts_from( x265_enc_sbac_t* enc_sbac, x265_enc_sbac_t* src_enc_sbac )
{
	enc_sbac->enc_bin_if->x_copy_contexts_from ( enc_sbac->enc_bin_if,
												src_enc_sbac->enc_bin_if ) ;
}

void x265_enc_sbac_load_contexts ( x265_enc_sbac_t* enc_sbac, x265_enc_sbac_t* src_enc_sbac)
{
	x265_enc_sbac_x_copy_contexts_from(enc_sbac, src_enc_sbac) ;
}

void x265_enc_sbac_x_write_terminating_bit ( x265_enc_sbac_t* enc_sbac, uint32_t i_bit )
{

}


void x265_enc_sbac_swap_int32 ( int32_t *p_value1, int32_t *p_value2 )
{
	int32_t i_temp_value = 0 ;

	i_temp_value = *p_value1 ;
	*p_value1 = *p_value2 ;
	*p_value2 = i_temp_value ;
}

void x265_enc_sbac_swap_uint32 ( uint32_t *p_value1, uint32_t *p_value2 )
{
	uint32_t i_temp_value = 0 ;

	i_temp_value = *p_value1 ;
	*p_value1 = *p_value2 ;
	*p_value2 = i_temp_value ;
}

