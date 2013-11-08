
#include "common/common.h"

uint32_t pu_offset[8] = { 0, 8, 4, 4, 2, 10, 1, 5};

void x265_enc_entropy_set_bitstream ( x265_enc_entropy_t *enc_entropy,
										x265_bit_if_t *bit_if )
{
	enc_entropy->enc_entropy_if->set_bitstream ( enc_entropy->enc_entropy_if,
												bit_if ) ;
}

void x265_enc_entropy_reset_bits ( x265_enc_entropy_t *enc_entropy )
{
	enc_entropy->enc_entropy_if->reset_bits (enc_entropy->enc_entropy_if) ;
}

void x265_enc_entropy_reset_coeff_cost ( x265_enc_entropy_t *enc_entropy )
{
	enc_entropy->enc_entropy_if->reset_coeff_cost (enc_entropy->enc_entropy_if) ;
}

uint32_t x265_enc_entropy_get_number_of_written_bits ( x265_enc_entropy_t *enc_entropy )
{
	return enc_entropy->enc_entropy_if->get_number_of_written_bits (enc_entropy->enc_entropy_if) ;
}

uint32_t x265_enc_entropy_get_coeff_cost ( x265_enc_entropy_t *enc_entropy )
{
	return enc_entropy->enc_entropy_if->get_coeff_cost (enc_entropy->enc_entropy_if) ;
}


void x265_enc_entropy_reset_entropy ( x265_t *h,
									x265_enc_entropy_t *enc_entropy,
									enum slice_type_e i_slice_type,
									int32_t i_slice_qp )
{
	enc_entropy->enc_entropy_if->reset_entropy ( h,
												enc_entropy->enc_entropy_if,
												i_slice_type,
												i_slice_qp ) ;
}

void x265_enc_entropy_print_cabac_state ( x265_enc_entropy_t *enc_entropy )
{
	enc_entropy->enc_entropy_if->print_cabac_state(enc_entropy->enc_entropy_if) ;
}

void x265_enc_entropy_determine_cabac_init_idx ( x265_t *h,
												x265_enc_entropy_t *enc_entropy,
												enum slice_type_e i_slice_type,
												int32_t i_slice_qp )
{
	enc_entropy->enc_entropy_if->determine_cabac_init_idx ( h,
															(x265_enc_entropy_if_t*)enc_entropy->enc_entropy_if,
															i_slice_type,
															i_slice_qp ) ;
}

void x265_enc_entropy_update_context_tables_p4 ( x265_enc_entropy_t *enc_entropy,
												enum slice_type_e i_slice_type,
												int32_t i_qp,
												int32_t b_execute_finish )
{
	enc_entropy->enc_entropy_if->update_context_tables_p4 ( enc_entropy->enc_entropy_if,
															i_slice_type,
															i_qp,
															b_execute_finish ) ;
}

void x265_enc_entropy_update_context_tables_p3 ( x265_enc_entropy_t *enc_entropy,
												enum slice_type_e i_slice_type,
												int32_t i_qp )
{
	enc_entropy->enc_entropy_if->update_context_tables_p3 ( enc_entropy->enc_entropy_if,
															i_slice_type,
															i_qp ) ;
}

x265_enc_entropy_t *x265_enc_entropy_new ()
{
	x265_enc_entropy_t *enc_entropy = NULL ;

	CHECKED_MALLOCZERO( enc_entropy, sizeof(x265_enc_entropy_t) );
	if ( x265_enc_entropy_init (enc_entropy) )
	{
		goto fail ;
	}

	return enc_entropy ;
fail:
	x265_enc_entropy_delete ( (void*) enc_entropy ) ;
	return NULL ;
}

void x265_enc_entropy_delete ( x265_enc_entropy_t *enc_entropy )
{
	x265_enc_entropy_deinit ( enc_entropy ) ;
	x265_free ( enc_entropy ) ;
}

int x265_enc_entropy_init ( x265_enc_entropy_t *enc_entropy )
{

    return 0 ;
}

void x265_enc_entropy_deinit ( x265_enc_entropy_t *enc_entropy )
{

}

void x265_enc_entropy_set_entropy_coder ( x265_enc_entropy_t *enc_entropy,
										x265_enc_entropy_if_t *e,
										x265_slice_t *slice )
{
	enc_entropy->enc_entropy_if = e;
	enc_entropy->enc_entropy_if->set_slice ( enc_entropy->enc_entropy_if, slice ) ;
}

void x265_enc_entropy_encode_slice_header ( x265_t *h, x265_enc_entropy_t *enc_entropy, x265_slice_t *slice )
{
	x265_sao_param_t *sao_param = NULL ;

	if ( h->sps[0].b_use_sao )
	{
		sao_param = h->sao_param ;
		slice->b_sao_enabled_flag = sao_param->b_sao_flag[0] ;
		slice->b_sao_enabled_flag_chroma = sao_param->b_sao_flag[1] ;
	}

	enc_entropy->enc_entropy_if->code_slice_header ( h,
													enc_entropy->enc_entropy_if,
													slice ) ;

}


int x265_enc_entropy_encode_tiles_wpp_entry_point( x265_t *h,
													x265_enc_entropy_t *enc_entropy,
													bs_t *s,
													x265_slice_t *slice )
{
	return enc_entropy->enc_entropy_if->code_tiles_wpp_entry_point ( h,
																	enc_entropy->enc_entropy_if,
																	s,
																	slice ) ;
}

void x265_enc_entropy_encode_terminating_bit ( x265_enc_entropy_t *enc_entropy, uint32_t i_is_last )
{
	enc_entropy->enc_entropy_if->code_terminating_bit ( enc_entropy->enc_entropy_if,
														i_is_last ) ;
}

void x265_enc_entropy_encode_slice_finish ( x265_enc_entropy_t *enc_entropy )
{
	enc_entropy->enc_entropy_if->code_slice_finish ( enc_entropy->enc_entropy_if  ) ;

}

void x265_enc_entropy_encode_pps ( x265_t *h, x265_enc_entropy_t *enc_entropy, x265_pps_t *pps )
{
	enc_entropy->enc_entropy_if->code_pps ( h, enc_entropy->enc_entropy_if, pps ) ;
}

void x265_enc_entropy_encode_sps( x265_t *h, x265_enc_entropy_t *enc_entropy, x265_sps_t *sps )
{
	enc_entropy->enc_entropy_if->code_sps ( h, enc_entropy->enc_entropy_if, sps ) ;
}

void x265_enc_entropy_encode_cu_transquant_bypass_flag(x265_enc_entropy_t *enc_entropy,
												x265_data_cu_t *cu,
												uint32_t i_abs_part_idx,
												int32_t b_rd )
{
	if ( b_rd )
	{
		i_abs_part_idx = 0 ;
	}
	enc_entropy->enc_entropy_if->code_cu_transquant_bypass_flag ( enc_entropy->enc_entropy_if,
																cu,
																i_abs_part_idx ) ;
}

void x265_enc_entropy_encode_vps( x265_enc_entropy_t *enc_entropy, x265_vps_t *vps )
{
	enc_entropy->enc_entropy_if->code_vps ( enc_entropy->enc_entropy_if, vps ) ;
}

void x265_enc_entropy_encode_skip_flag(x265_t* h,
										x265_enc_entropy_t *enc_entropy,
										x265_data_cu_t *cu,
										uint32_t i_abs_part_idx,
										int32_t b_rd )
{
	if ( I_SLICE == h->slice->i_slice_type )
	{
		return ;
	}

	if( b_rd )
	{
		i_abs_part_idx = 0;
	}
	enc_entropy->enc_entropy_if->code_skip_flag ( h,
												enc_entropy->enc_entropy_if,
												cu,
												i_abs_part_idx ) ;
}

void x265_enc_entropy_encode_merge_flag(x265_t* h,
										x265_enc_entropy_t *enc_entropy,
										x265_data_cu_t *cu,
										uint32_t i_abs_part_idx )
{
	enc_entropy->enc_entropy_if->code_merge_flag ( h,
													enc_entropy->enc_entropy_if,
													cu,
													i_abs_part_idx ) ;
}

void x265_enc_entropy_encode_merge_index(x265_t* h,
										x265_enc_entropy_t *enc_entropy,
										x265_data_cu_t *cu,
										uint32_t i_abs_part_idx,
										int32_t b_rd )
{
	if( b_rd )
	{
		i_abs_part_idx = 0;
		assert ( x265_data_cu_get_partition_size_p2(cu, i_abs_part_idx) == SIZE_2Nx2N) ;
	}
	enc_entropy->enc_entropy_if->code_merge_index ( h,
													enc_entropy->enc_entropy_if,
													cu,
													i_abs_part_idx ) ;
}


void x265_enc_entropy_encode_pred_mode(x265_t *h,
										x265_enc_entropy_t *enc_entropy,
										x265_data_cu_t *cu,
										uint32_t i_abs_part_idx,
										int32_t b_rd )
{
	if( b_rd )
	{
		i_abs_part_idx = 0;
	}

	if ( I_SLICE == h->slice->i_slice_type )
	{
		return ;
	}
	enc_entropy->enc_entropy_if->code_pred_mode ( enc_entropy->enc_entropy_if,
													cu,
													i_abs_part_idx ) ;
}

void x265_enc_entropy_encode_split_flag(x265_t* h,
										x265_enc_entropy_t *enc_entropy,
										x265_data_cu_t *cu,
										uint32_t i_abs_part_idx,
										uint32_t i_depth,
										int32_t b_rd )
{
	if( b_rd )
	{
		i_abs_part_idx = 0;
	}

	enc_entropy->enc_entropy_if->code_split_flag ( h,
													enc_entropy->enc_entropy_if,
													cu,
													i_abs_part_idx,
													i_depth ) ;
}

void x265_enc_entropy_encode_part_size(x265_t* h,
										x265_enc_entropy_t *enc_entropy,
										x265_data_cu_t *cu,
										uint32_t i_abs_part_idx,
										uint32_t i_depth,
										int32_t b_rd )
{
	if( b_rd )
	{
		i_abs_part_idx = 0;
	}

	enc_entropy->enc_entropy_if->code_part_size ( h,
												enc_entropy->enc_entropy_if,
												cu,
												i_abs_part_idx,
												i_depth ) ;
}

void x265_enc_entropy_encode_ipcm_info(x265_t* h,
										x265_enc_entropy_t *enc_entropy,
										x265_data_cu_t *cu,
										uint32_t i_abs_part_idx,
										int32_t b_rd )
{
	if( !h->sps[0].b_use_pcm
		|| x265_data_cu_get_width_p2(cu, i_abs_part_idx) > (1 << h->sps[0].i_pcm_log2_max_size)
		|| x265_data_cu_get_width_p2(cu, i_abs_part_idx) < (1 << h->sps[0].i_pcm_log2_min_size)
		)
	{
		return;
	}

	if( b_rd )
	{
		i_abs_part_idx = 0;
	}

	enc_entropy->enc_entropy_if->code_ipcm_info ( h,
												enc_entropy->enc_entropy_if,
												cu,
												i_abs_part_idx ) ;

}


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
										int32_t *p_code_dqp )
{
	uint32_t i_sub_div = 0 ;
	uint32_t i_log2_trafo_i_size = 0;
	uint32_t i_cbf_y = 0 ;
	uint32_t i_cbf_u = 0 ;
	uint32_t i_cbf_v = 0 ;
	uint32_t i_part_num = 0 ;
	uint32_t i_size = 0 ;
	uint32_t i_tr_depth_curr = 0 ;
	int32_t b_first_cbf_of_cu = 0 ;
	int32_t i_tr_width = 0 ;
	int32_t i_tr_height = 0 ;

	i_sub_div = x265_base_data_cu_get_transform_idx_p2((x265_base_data_cu_t*)cu, i_abs_part_idx)
				+ x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_part_idx ) > i_depth;
	i_log2_trafo_i_size = h->global.convert_to_bit[h->sps[0].i_max_cu_width] + 2 - i_depth;
	i_cbf_y = x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx, TEXT_LUMA, i_tr_idx );
	i_cbf_u = x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx, TEXT_CHROMA_U, i_tr_idx );
	i_cbf_v = x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx, TEXT_CHROMA_V, i_tr_idx );

	if(i_tr_idx==0)
	{
		enc_entropy->i_bak_abs_part_idx_cu = i_abs_part_idx;
	}
	if( i_log2_trafo_i_size == 2 )
	{
		i_part_num = h->cu.pic.i_num_partitions >> ( ( i_depth - 1 ) << 1 );
		if( ( i_abs_part_idx % i_part_num ) == 0 )
		{
			enc_entropy->i_bak_abs_part_idx   = i_abs_part_idx;
			enc_entropy->i_bak_chroma_offset = i_offset_chroma;
		}
		else if( ( i_abs_part_idx % i_part_num ) == (i_part_num - 1) )
		{
			i_cbf_u = x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, enc_entropy->i_bak_abs_part_idx, TEXT_CHROMA_U, i_tr_idx );
			i_cbf_v = x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, enc_entropy->i_bak_abs_part_idx, TEXT_CHROMA_V, i_tr_idx );
		}
	}

	if( x265_base_data_cu_get_prediction_mode_p2((x265_base_data_cu_t*)cu, i_abs_part_idx) == MODE_INTRA
			&& x265_data_cu_get_partition_size_p2 (cu, i_abs_part_idx) == SIZE_NxN
			&& i_depth == x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_part_idx) )
	{
		assert( i_sub_div );
	}
	else if( x265_base_data_cu_get_prediction_mode_p2((x265_base_data_cu_t*)cu, i_abs_part_idx) == MODE_INTRA
			&& (x265_data_cu_get_partition_size_p2 (cu, i_abs_part_idx) != SIZE_2Nx2N)
			&& i_depth == x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_part_idx)
			&&  (h->sps[0].i_quadtree_tu_max_depth_inter == 1) )
	{
		if ( i_log2_trafo_i_size > x265_data_cu_get_quadtree_tu_log2_min_size_in_cu(h, cu, i_abs_part_idx) )
		{
			assert( i_sub_div );
		}
		else
		{
			assert(!i_sub_div );
		}
	}
	else if( i_log2_trafo_i_size > h->sps[0].i_quadtree_tu_log2_max_size )
	{
		assert( i_sub_div );
	}
	else if( i_log2_trafo_i_size == h->sps[0].i_quadtree_tu_log2_min_size )
	{
		assert( !i_sub_div );
	}
	else if( i_log2_trafo_i_size == x265_data_cu_get_quadtree_tu_log2_min_size_in_cu(h, cu, i_abs_part_idx) )
	{
		assert( !i_sub_div );
	}
	else
	{
		assert( i_log2_trafo_i_size > x265_data_cu_get_quadtree_tu_log2_min_size_in_cu(h, cu, i_abs_part_idx) );
		enc_entropy->enc_entropy_if->code_transform_sub_div_flag( h, enc_entropy->enc_entropy_if, i_sub_div, 5 - i_log2_trafo_i_size );
	}

	i_tr_depth_curr = i_depth - x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_part_idx );
	b_first_cbf_of_cu = i_tr_depth_curr == 0;
	if( b_first_cbf_of_cu || i_log2_trafo_i_size > 2 )
	{
		if( b_first_cbf_of_cu
			|| x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx, TEXT_CHROMA_U, i_tr_depth_curr - 1 ) )
		{
			enc_entropy->enc_entropy_if->code_qt_cbf( h, enc_entropy->enc_entropy_if, cu, i_abs_part_idx, TEXT_CHROMA_U, i_tr_depth_curr );
		}
		if( b_first_cbf_of_cu
			|| x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx, TEXT_CHROMA_V, i_tr_depth_curr - 1 ) )
		{
			enc_entropy->enc_entropy_if->code_qt_cbf( h, enc_entropy->enc_entropy_if, cu, i_abs_part_idx, TEXT_CHROMA_V, i_tr_depth_curr );
		}
	}
	else if( i_log2_trafo_i_size == 2 )
	{
		assert( x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx, TEXT_CHROMA_U, i_tr_depth_curr )
				== x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx, TEXT_CHROMA_U, i_tr_depth_curr - 1 ) );
		assert( x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx, TEXT_CHROMA_V, i_tr_depth_curr )
				== x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx, TEXT_CHROMA_V, i_tr_depth_curr - 1 ) );
	}

	if( i_sub_div )
	{
		i_width  >>= 1;
		i_height >>= 1;
		i_size = i_width*i_height;
		i_tr_idx++;
		++i_depth;
		i_part_num = h->cu.pic.i_num_partitions >> (i_depth << 1);

		x265_enc_entropy_x_encode_transform( h, enc_entropy, cu, i_offset_luma, i_offset_chroma, i_abs_part_idx, i_depth, i_width, i_height, i_tr_idx, p_code_dqp );

		i_abs_part_idx += i_part_num;  i_offset_luma += i_size;  i_offset_chroma += (i_size>>2);
		x265_enc_entropy_x_encode_transform( h, enc_entropy, cu, i_offset_luma, i_offset_chroma, i_abs_part_idx, i_depth, i_width, i_height, i_tr_idx, p_code_dqp );

		i_abs_part_idx += i_part_num;  i_offset_luma += i_size;  i_offset_chroma += (i_size>>2);
		x265_enc_entropy_x_encode_transform( h, enc_entropy, cu, i_offset_luma, i_offset_chroma, i_abs_part_idx, i_depth, i_width, i_height, i_tr_idx, p_code_dqp );

		i_abs_part_idx += i_part_num;  i_offset_luma += i_size;  i_offset_chroma += (i_size>>2);
		x265_enc_entropy_x_encode_transform( h, enc_entropy, cu, i_offset_luma, i_offset_chroma, i_abs_part_idx, i_depth, i_width, i_height, i_tr_idx, p_code_dqp );
	}
	else
	{
		if( x265_base_data_cu_get_prediction_mode_p2((x265_base_data_cu_t*)cu, i_abs_part_idx) != MODE_INTRA
				&& i_depth == x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_part_idx )
				&& !x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx, TEXT_CHROMA_U, 0 )
				&& !x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx, TEXT_CHROMA_V, 0 ) )
		{
			assert( x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu, i_abs_part_idx, TEXT_LUMA, 0 ) );
			//      printf( "saved one bin! " );
		}
		else
		{
			enc_entropy->enc_entropy_if->code_qt_cbf( h,
													enc_entropy->enc_entropy_if,
													cu,
													i_abs_part_idx,
													TEXT_LUMA,
													x265_base_data_cu_get_transform_idx_p2((x265_base_data_cu_t*)cu, i_abs_part_idx) );
		}


		if ( i_cbf_y || i_cbf_u || i_cbf_v )
		{
			// d_q_p: only for lcu once
			if ( h->pps[0].b_use_dqp )
			{
				if ( *p_code_dqp )
				{
					x265_enc_entropy_encode_qp( h,
												enc_entropy,
												cu,
												enc_entropy->i_bak_abs_part_idx_cu,
												0 );
					*p_code_dqp = 0;
				}
			}
		}
		if( i_cbf_y )
		{
			i_tr_width = i_width;
			i_tr_height = i_height;
			enc_entropy->enc_entropy_if->code_coeff_nxn( h,
														enc_entropy->enc_entropy_if,
														cu,
														(x265_data_cu_get_coeff_y(cu) + i_offset_luma),
														i_abs_part_idx,
														i_tr_width,
														i_tr_height,
														i_depth,
														TEXT_LUMA );
		}
		if( i_log2_trafo_i_size > 2 )
		{
			i_tr_width = i_width >> 1;
			i_tr_height = i_height >> 1;
			if( i_cbf_u )
			{
				enc_entropy->enc_entropy_if->code_coeff_nxn( h,
															enc_entropy->enc_entropy_if,
															cu,
															(x265_data_cu_get_coeff_cb(cu) + i_offset_chroma),
															i_abs_part_idx,
															i_tr_width,
															i_tr_height,
															i_depth,
															TEXT_CHROMA_U );
			}
			if( i_cbf_v )
			{
				enc_entropy->enc_entropy_if->code_coeff_nxn( h,
															enc_entropy->enc_entropy_if,
															cu,
															(x265_data_cu_get_coeff_cr(cu) + i_offset_chroma),
															i_abs_part_idx,
															i_tr_width,
															i_tr_height,
															i_depth,
															TEXT_CHROMA_V );
			}
		}
		else
		{
			i_part_num = h->cu.pic.i_num_partitions >> ( ( i_depth - 1 ) << 1 );
			if( ( i_abs_part_idx % i_part_num ) == (i_part_num - 1) )
			{
				i_tr_width = i_width;
				i_tr_height = i_height;
				if( i_cbf_u )
				{
					enc_entropy->enc_entropy_if->code_coeff_nxn( h,
																enc_entropy->enc_entropy_if,
																cu,
																(x265_data_cu_get_coeff_cb(cu) + enc_entropy->i_bak_chroma_offset),
																enc_entropy->i_bak_abs_part_idx,
																i_tr_width,
																i_tr_height,
																i_depth,
																TEXT_CHROMA_U );
				}
				if( i_cbf_v )
				{
					enc_entropy->enc_entropy_if->code_coeff_nxn(h,
																enc_entropy->enc_entropy_if,
																cu,
																(x265_data_cu_get_coeff_cr(cu) + enc_entropy->i_bak_chroma_offset),
																enc_entropy->i_bak_abs_part_idx,
																i_tr_width,
																i_tr_height,
																i_depth,
																TEXT_CHROMA_V );
				}
			}
		}
	}
}



// int32_tra direction for luma
void x265_enc_entropy_encode_intra_dir_mode_luma (x265_t* h,
												x265_enc_entropy_t *enc_entropy,
												x265_data_cu_t* cu,
												uint32_t abs_part_idx,
												int32_t b_is_multiple_pu )
{
	enc_entropy->enc_entropy_if->code_intra_dir_luma_ang( h,
														enc_entropy->enc_entropy_if,
														cu,
														abs_part_idx,
														b_is_multiple_pu);
}

// int32_tra direction for chroma
void x265_enc_entropy_encode_intra_dir_mode_chroma(x265_enc_entropy_t *enc_entropy,
													x265_data_cu_t* cu,
													uint32_t i_abs_part_idx,
													int32_t b_rd )
{
	if( b_rd )
	{
		i_abs_part_idx = 0;
	}

	enc_entropy->enc_entropy_if->code_intra_dir_chroma( enc_entropy->enc_entropy_if,
														cu,
														i_abs_part_idx );
}

void x265_enc_entropy_encode_pred_info(x265_t *h,
										x265_enc_entropy_t *enc_entropy,
										x265_data_cu_t* cu,
										uint32_t i_abs_part_idx,
										int32_t b_rd )
{
	if( b_rd )
	{
		i_abs_part_idx = 0;
	}
	if( x265_base_data_cu_is_intra((x265_base_data_cu_t*)cu, i_abs_part_idx ) )                                 // if it is int32_tra mode, encode intra prediction mode.
	{
		x265_enc_entropy_encode_intra_dir_mode_luma  ( h, enc_entropy, cu, i_abs_part_idx, 1 );
		x265_enc_entropy_encode_intra_dir_mode_chroma( enc_entropy, cu, i_abs_part_idx, b_rd );
	}
	else                                                                // if it is int32_ter mode, encode motion vector and reference index
	{
		x265_enc_entropy_encode_pu_wise( h, enc_entropy, cu, i_abs_part_idx, b_rd );
	}
}


void x265_enc_entropy_encode_pu_wise(x265_t *h,
									x265_enc_entropy_t *enc_entropy,
									x265_data_cu_t* cu,
									uint32_t i_abs_part_idx,
									int32_t b_rd )
{
	enum part_size_e i_part_size = 0 ;
	uint32_t i_num_pu = 0 ;
	uint32_t i_depth = 0 ;
	uint32_t i_pu_offset = 0 ;
	uint32_t i_part_idx = 0, i_sub_part_idx = 0 ;
	uint32_t i_ref_list_idx = 0 ;

	i_part_size = x265_data_cu_get_partition_size_p2(cu, i_abs_part_idx );
	i_num_pu = ( i_part_size == SIZE_2Nx2N ? 1 : ( i_part_size == SIZE_NxN ? 4 : 2 ) );
	i_depth = x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_part_idx );
	i_pu_offset = ( pu_offset[(uint32_t)i_part_size]
	                        << ( ( h->sps[0].i_max_cu_depth - i_depth ) << 1 ) ) >> 4;
	if ( b_rd )
	{
		i_abs_part_idx = 0;
	}

	for ( i_part_idx = 0, i_sub_part_idx = i_abs_part_idx;
			i_part_idx < i_num_pu;
			i_part_idx++, i_sub_part_idx += i_pu_offset )
	{
		x265_enc_entropy_encode_merge_flag( h, enc_entropy, cu, i_sub_part_idx );
		if ( x265_data_cu_get_merge_flag_p2(cu,  i_sub_part_idx ) )
		{
			x265_enc_entropy_encode_merge_index(h, enc_entropy, cu, i_sub_part_idx, 0 );
		}
		else
		{
			x265_enc_entropy_encode_inter_dir_pu(h, enc_entropy, cu, i_sub_part_idx );
			for ( i_ref_list_idx = 0; i_ref_list_idx < 2; i_ref_list_idx++ )
			{
				if ( h->i_ref[(enum ref_pic_list_e)i_ref_list_idx] > 0 )
				{
					x265_enc_entropy_encode_ref_frm_idx_pu(h,
															enc_entropy,
															cu,
															i_sub_part_idx,
															(enum ref_pic_list_e) i_ref_list_idx );
					x265_enc_entropy_encode_mvd_pu(h,
													enc_entropy,
													cu,
													i_sub_part_idx,
													(enum ref_pic_list_e) i_ref_list_idx );
					x265_enc_entropy_encode_mvp_idx_pu(enc_entropy,
														cu,
														i_sub_part_idx,
														(enum ref_pic_list_e) i_ref_list_idx );
				}
			}
		}
	}

}

void x265_enc_entropy_encode_inter_dir_pu(x265_t *h,
											x265_enc_entropy_t *enc_entropy,
											x265_data_cu_t* cu,
											uint32_t i_abs_part_idx )
{
	if ( !(B_SLICE == h->slice->i_slice_type) )
	{
		return;
	}

	enc_entropy->enc_entropy_if->code_inter_dir( enc_entropy->enc_entropy_if, cu, i_abs_part_idx );
	return;
}


void x265_enc_entropy_encode_ref_frm_idx_pu(x265_t *h,
											x265_enc_entropy_t *enc_entropy,
											x265_data_cu_t* cu,
											uint32_t i_abs_part_idx,
											enum ref_pic_list_e i_ref_pic_list )
{
	assert( !x265_base_data_cu_is_intra((x265_base_data_cu_t*)cu, i_abs_part_idx ) );

	if ( ( h->i_ref[i_ref_pic_list] == 1 ) )
    {
    	return;
    }

    if ( x265_base_data_cu_get_inter_dir_p2((x265_base_data_cu_t*)cu, i_abs_part_idx )
    		& ( 1 << i_ref_pic_list ) )
    {
    	enc_entropy->enc_entropy_if->code_ref_frm_idx( h,
    													enc_entropy->enc_entropy_if,
    													cu,
    													i_abs_part_idx,
    													i_ref_pic_list );
    }

    return;
}


void x265_enc_entropy_encode_mvd_pu(x265_t *h,
									x265_enc_entropy_t *enc_entropy,
									x265_data_cu_t* cu,
									uint32_t i_abs_part_idx,
									enum ref_pic_list_e i_ref_pic_list )
{
	assert( !x265_base_data_cu_is_intra((x265_base_data_cu_t*)cu, i_abs_part_idx ) );

	if ( x265_base_data_cu_get_inter_dir_p2((x265_base_data_cu_t*)cu, i_abs_part_idx )
			& ( 1 << i_ref_pic_list ) )
	{
		enc_entropy->enc_entropy_if->code_mvd( h,
												enc_entropy->enc_entropy_if,
												cu,
												i_abs_part_idx,
												i_ref_pic_list );
	}
	return;
}

void x265_enc_entropy_encode_mvp_idx_pu(x265_enc_entropy_t *enc_entropy,
										x265_data_cu_t* cu,
										uint32_t i_abs_part_idx,
										enum ref_pic_list_e i_ref_pic_list )
{
	if ( (x265_base_data_cu_get_inter_dir_p2((x265_base_data_cu_t*)cu, i_abs_part_idx )
			& ( 1 << i_ref_pic_list )) )
	{
		enc_entropy->enc_entropy_if->code_mvp_idx( enc_entropy->enc_entropy_if,
													cu,
													i_abs_part_idx,
													i_ref_pic_list );
	}

	return;
}

void x265_enc_entropy_encode_qt_cbf(x265_t *h,
									x265_enc_entropy_t *enc_entropy,
									x265_data_cu_t* cu,
									uint32_t i_abs_part_idx,
									enum text_type_e i_text_type,
									uint32_t i_tr_depth )
{
	enc_entropy->enc_entropy_if->code_qt_cbf( h,
												enc_entropy->enc_entropy_if,
												cu,
												i_abs_part_idx,
												i_text_type,
												i_tr_depth );
}

void x265_enc_entropy_encode_transform_sub_div_flag(x265_t *h,
													x265_enc_entropy_t *enc_entropy,
													uint32_t i_symbol,
													uint32_t i_ctx )
{
	enc_entropy->enc_entropy_if->code_transform_sub_div_flag( h,
															enc_entropy->enc_entropy_if,
															i_symbol,
															i_ctx );
}

void x265_enc_entropy_encode_qt_root_cbf(x265_enc_entropy_t *enc_entropy,
										x265_data_cu_t* cu,
										uint32_t i_abs_part_idx )
{
	enc_entropy->enc_entropy_if->code_qt_root_cbf( enc_entropy->enc_entropy_if, cu, i_abs_part_idx );
}

void x265_enc_entropy_encode_qt_cbf_zero(x265_enc_entropy_t *enc_entropy,
										x265_data_cu_t* cu,
										enum text_type_e i_text_type,
										uint32_t i_tr_depth )
{
	enc_entropy->enc_entropy_if->code_qt_cbf_zero( enc_entropy->enc_entropy_if,
													cu, i_text_type, i_tr_depth );
}
void x265_enc_entropy_encode_qt_root_cbf_zero(x265_enc_entropy_t *enc_entropy,
											x265_data_cu_t* cu )
{
	enc_entropy->enc_entropy_if->code_qt_root_cbf_zero( enc_entropy->enc_entropy_if, cu );
}

// dqp
void x265_enc_entropy_encode_qp(x265_t *h,
								x265_enc_entropy_t *enc_entropy,
								x265_data_cu_t* cu,
								uint32_t i_abs_part_idx,
								int32_t b_rd )
{
	if( b_rd )
	{
		i_abs_part_idx = 0;
	}

	if ( h->pps[0].b_use_dqp )
	{
		enc_entropy->enc_entropy_if->code_delta_qp( h,
													enc_entropy->enc_entropy_if,
													cu,
													i_abs_part_idx );
	}
}

void x265_enc_entropy_encode_coeff( x265_t *h,
									x265_enc_entropy_t *enc_entropy,
									x265_data_cu_t* cu,
									uint32_t i_abs_part_idx,
									uint32_t i_depth,
									uint32_t i_width,
									uint32_t i_height,
									int32_t* p_code_dqp )
{
	uint32_t i_min_coeff_size = 0 ;
	uint32_t i_luma_offset = 0 ;
	uint32_t i_chroma_offset = 0 ;

	i_min_coeff_size = h->cu.pic.i_min_cu_width * h->cu.pic.i_min_cu_height ;
	i_luma_offset = i_min_coeff_size*i_abs_part_idx;
	i_chroma_offset = i_luma_offset >> 2;

	if( x265_base_data_cu_is_intra((x265_base_data_cu_t*)cu, i_abs_part_idx) )
	{

	}
	else
	{
		if( !(x265_data_cu_get_merge_flag_p2(cu, i_abs_part_idx )
				&& x265_data_cu_get_partition_size_p2(cu, i_abs_part_idx) == SIZE_2Nx2N ) )
		{
			enc_entropy->enc_entropy_if->code_qt_root_cbf( enc_entropy->enc_entropy_if, cu, i_abs_part_idx );
		}
		if ( !x265_base_data_cu_get_qt_root_cbf((x265_base_data_cu_t*)cu, i_abs_part_idx ) )
		{
			return;
		}
	}

	x265_enc_entropy_x_encode_transform( h,
										enc_entropy,
										cu,
										i_luma_offset,
										i_chroma_offset,
										i_abs_part_idx,
										i_depth,
										i_width,
										i_height,
										0,
										p_code_dqp);
}

void x265_enc_entropy_encode_coeff_nxn(x265_t *h,
										x265_enc_entropy_t *enc_entropy,
										x265_data_cu_t *cu,
										x265_coeff_t* pc_coeff,
										uint32_t i_abs_part_idx,
										uint32_t i_tr_width,
										uint32_t i_tr_height,
										uint32_t i_depth,
										enum text_type_e i_text_type )
{
	// this is for transform unit processing. this may be used at mode selection stage for int32_ter.
	enc_entropy->enc_entropy_if->code_coeff_nxn( h,
												enc_entropy->enc_entropy_if,
												cu,
												pc_coeff,
												i_abs_part_idx,
												i_tr_width,
												i_tr_height,
												i_depth, i_text_type );
}

void x265_enc_entropy_estimate_bit (x265_t *h,
									x265_enc_entropy_t *enc_entropy,
									x265_est_bits_sbac_struct_t* pc_est_bits_sbac,
									int32_t i_width,
									int32_t i_height,
									enum text_type_e i_text_type)
{
	i_text_type = i_text_type == TEXT_LUMA ? TEXT_LUMA : TEXT_CHROMA;

	enc_entropy->enc_entropy_if->est_bit ( h,
											enc_entropy->enc_entropy_if,
											pc_est_bits_sbac,
											i_width,
											i_height,
											i_text_type );
}

void x265_enc_entropy_encode_sao_offset ( x265_t *h,
										x265_enc_entropy_t *enc_entropy,
										x265_sao_lcu_param_t *sao_lcu_param,
										uint32_t i_comp_idx )
{
	int32_t loop = 0 ;
	uint32_t i_symbol;
    int32_t i_bit_depth = 0 ;
    int32_t i_offset_th = 0 ;
    uint32_t i_abs_offset = 0 ;
    uint32_t i_sign = 0 ;

	i_symbol = sao_lcu_param->i_type_idx + 1;
	if (i_comp_idx!=2)
	{
		enc_entropy->enc_entropy_if->code_sao_type_idx(enc_entropy->enc_entropy_if, i_symbol);
	}
	if (i_symbol)
	{
		if (sao_lcu_param->i_type_idx < 4 && i_comp_idx != 2)
		{
			sao_lcu_param->i_sub_type_idx = sao_lcu_param->i_type_idx;
		}
		i_bit_depth = i_comp_idx ? h->param.sps.i_bit_depth_c : h->param.sps.i_bit_depth_y;
		i_offset_th = 1 << X265_MIN(i_bit_depth - 5,5);
		if( sao_lcu_param->i_type_idx == SAO_BO )
		{
			for( loop = 0; loop < sao_lcu_param->i_length; ++ loop )
			{
				i_abs_offset = ( (sao_lcu_param->offset[loop] < 0)
								? -sao_lcu_param->offset[loop]
								: sao_lcu_param->offset[loop]);
				enc_entropy->enc_entropy_if->code_sao_max_uvlc(enc_entropy->enc_entropy_if, i_abs_offset, i_offset_th-1);
			}
			for( loop = 0; loop < sao_lcu_param->i_length; ++ loop )
			{
				if (sao_lcu_param->offset[loop] != 0)
				{
					i_sign = (sao_lcu_param->offset[loop] < 0) ? 1 : 0 ;
					enc_entropy->enc_entropy_if->code_sao_sign(enc_entropy->enc_entropy_if, i_sign);
				}
			}
			i_symbol = (uint32_t) (sao_lcu_param->i_sub_type_idx);
			enc_entropy->enc_entropy_if->code_sao_uflc(enc_entropy->enc_entropy_if, 5, i_symbol);
		}
		else if( sao_lcu_param->i_type_idx < 4 )
		{
			enc_entropy->enc_entropy_if->code_sao_max_uvlc(enc_entropy->enc_entropy_if,  sao_lcu_param->offset[0], i_offset_th-1);
			enc_entropy->enc_entropy_if->code_sao_max_uvlc(enc_entropy->enc_entropy_if,  sao_lcu_param->offset[1], i_offset_th-1);
			enc_entropy->enc_entropy_if->code_sao_max_uvlc(enc_entropy->enc_entropy_if, -sao_lcu_param->offset[2], i_offset_th-1);
			enc_entropy->enc_entropy_if->code_sao_max_uvlc(enc_entropy->enc_entropy_if, -sao_lcu_param->offset[3], i_offset_th-1);
			if (i_comp_idx!=2)
			{
				i_symbol = (uint32_t) (sao_lcu_param->i_sub_type_idx);
				enc_entropy->enc_entropy_if->code_sao_uflc(enc_entropy->enc_entropy_if, 2, i_symbol);
			}
		}
	}
}


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
													int32_t allow_merge_up)
{
	if (b_sao_flag)
	{
		if (rx>0 && cu_addr_in_slice!=0 && allow_merge_left)
		{
			enc_entropy->enc_entropy_if->code_sao_merge(enc_entropy->enc_entropy_if,
														sao_lcu_param->b_merge_left_flag);
		}
		else
		{
			sao_lcu_param->b_merge_left_flag = 0;
		}
		if (sao_lcu_param->b_merge_left_flag == 0)
		{
			if ( (ry > 0) && (cu_addr_up_in_slice>=0) && allow_merge_up )
			{
				enc_entropy->enc_entropy_if->code_sao_merge(enc_entropy->enc_entropy_if,
															sao_lcu_param->b_merge_up_flag);
			}
			else
			{
				sao_lcu_param->b_merge_up_flag = 0;
			}
			if (!sao_lcu_param->b_merge_up_flag)
			{
				x265_enc_entropy_encode_sao_offset(h, enc_entropy, sao_lcu_param, comp_idx);
			}
		}
	}
}


int32_t x265_enc_entropy_count_non_zero_coeffs(x265_enc_entropy_t *enc_entropy,
												x265_coeff_t* coeff,
												uint32_t i_size )
{
	int32_t loop = 0 ;
	int32_t i_count = 0;

	for ( loop = 0; loop < i_size; ++ loop )
	{
		i_count += coeff[loop] != 0;
	}

	return i_count;
}

void x265_enc_entropy_encode_scaling_list(x265_t *h,
										x265_enc_entropy_t *enc_entropy,
										x265_scaling_list_t* scaling_list )
{
	enc_entropy->enc_entropy_if->code_scaling_list( h,
													enc_entropy->enc_entropy_if,
													scaling_list );
}





