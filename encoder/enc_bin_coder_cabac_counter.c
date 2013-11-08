

#include "common/common.h"


#if X265_FAST_BIT_EST

x265_enc_bin_cabac_t* enc_bin_cabac_counter_get_enc_bin_cabac ( x265_enc_bin_if_t *enc_bin_if )
{
	x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter = NULL ;

	enc_bin_cabac_counter = (x265_enc_bin_cabac_counter_t*) enc_bin_if ;
	return (x265_enc_bin_cabac_t*)enc_bin_cabac_counter ;
}

void x265_enc_bin_cabac_counter_set_bins_coded ( x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter, uint32_t i_val )
{
	enc_bin_cabac_counter->i_bins_coded = i_val ;
}

uint32_t x265_enc_bin_cabac_counter_get_bins_coded ( x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter )
{
	return enc_bin_cabac_counter->i_bins_coded ;
}

void x265_enc_bin_cabac_counter_set_bin_counting_enable_flag ( x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter, int32_t b_flag )
{
	enc_bin_cabac_counter->i_bin_count_increment = b_flag ? 1 : 0 ;
}

int32_t x265_enc_bin_cabac_counter_get_bin_counting_enable_flag ( x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter )
{
	return enc_bin_cabac_counter->i_bin_count_increment ;
}


x265_enc_bin_if_t *x265_enc_bin_cabac_counter_new ()
{
	x265_enc_bin_if_t *enc_bin_if = NULL ;

	CHECKED_MALLOCZERO( enc_bin_if, sizeof(x265_enc_bin_cabac_counter_t) );
	if ( x265_enc_bin_cabac_counter_init(enc_bin_if) )
	{
		goto fail ;
	}

	return enc_bin_if ;
fail:
	x265_enc_bin_cabac_counter_delete ( enc_bin_if ) ;
	return NULL ;
}


void x265_enc_bin_cabac_counter_delete ( x265_enc_bin_if_t *enc_bin_if )
{
	if ( enc_bin_if )
	{
		x265_enc_bin_cabac_counter_deinit ( enc_bin_if ) ;
		x265_free ( enc_bin_if ) ;
	}
}

int x265_enc_bin_cabac_counter_init ( x265_enc_bin_if_t *enc_bin_if )
{
	x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter = NULL ;

	x265_enc_bin_if_init ( enc_bin_if ) ;
	enc_bin_cabac_counter = (x265_enc_bin_cabac_counter_t*) enc_bin_if ;
	CHECKED_MALLOCZERO( enc_bin_cabac_counter->state, X265_CABAC_STATE_NUMBER * sizeof(uint8_t) );
	CHECKED_MALLOCZERO( enc_bin_cabac_counter->bins_coded, X265_CABAC_STATE_NUMBER * sizeof(uint32_t) );

	enc_bin_if->init = enc_bin_cabac_counter_init ;
	enc_bin_if->uninit = enc_bin_cabac_counter_uninit ;
	enc_bin_if->start = enc_bin_cabac_counter_start ;
	enc_bin_if->finish = enc_bin_cabac_counter_finish ;
	enc_bin_if->copy_state = enc_bin_cabac_counter_copy_state ;
	enc_bin_if->load_intra_dir_mode_luma = enc_bin_cabac_counter_load_intra_dir_mode_luma ;
	enc_bin_if->x_copy_contexts_from = enc_bin_cabac_counter_x_copy_contexts_from ;
	enc_bin_if->flush = enc_bin_cabac_counter_flush ;
	enc_bin_if->reset_bac = enc_bin_cabac_counter_reset_bac ;
	enc_bin_if->encode_pcm_align_bits = enc_bin_cabac_counter_encode_pcm_align_bits ;
	enc_bin_if->x_write_pcm_code = enc_bin_cabac_counter_x_write_pcm_code ;
	enc_bin_if->reset_bits = enc_bin_cabac_counter_reset_bits ;
	enc_bin_if->clear_bits = enc_bin_cabac_counter_clear_bits ;
	enc_bin_if->get_num_written_bits = enc_bin_cabac_counter_get_num_written_bits ;
	enc_bin_if->get_entropy_bits = enc_bin_cabac_counter_get_entropy_bits ;
	enc_bin_if->encode_bin = enc_bin_cabac_counter_encode_bin ;
	enc_bin_if->encode_bin_ep = enc_bin_cabac_counter_encode_bin_ep ;
	enc_bin_if->encode_bins_ep = enc_bin_cabac_counter_encode_bins_ep ;
	enc_bin_if->encode_bin_trm = enc_bin_cabac_counter_encode_bin_trm ;

	enc_bin_if->get_enc_bin_cabac = enc_bin_cabac_counter_get_enc_bin_cabac ;
	enc_bin_if->enc_bin_if_delete = x265_enc_bin_cabac_counter_delete ;
	enc_bin_if->reset_entropy = enc_bin_cabac_counter_reset_entropy ;
	enc_bin_if->print_cabac_state = enc_bin_cabac_counter_print_cabac_state ;
	enc_bin_if->determine_cabac_init_idx = enc_bin_cabac_counter_determine_cabac_init_idx ;
	enc_bin_if->update_context_table = enc_bin_cabac_counter_update_context_table ;


	return 0 ;
fail:
	x265_enc_bin_cabac_counter_deinit ( (void*) enc_bin_if ) ;
	return -1 ;
}

void x265_enc_bin_cabac_counter_deinit ( x265_enc_bin_if_t *enc_bin_if )
{
	x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter = NULL ;

	enc_bin_cabac_counter = (x265_enc_bin_cabac_counter_t *)enc_bin_if ;

	x265_free ( enc_bin_cabac_counter->bins_coded ) ;
	x265_free ( enc_bin_cabac_counter->state ) ;
}

void enc_bin_cabac_counter_init ( x265_enc_bin_if_t *enc_bin_if, x265_bit_if_t *bit_if )
{
	x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter = NULL ;

	enc_bin_cabac_counter = (x265_enc_bin_cabac_counter_t*) enc_bin_if ;
	enc_bin_cabac_counter->bit_if = bit_if ;
}

void enc_bin_cabac_counter_uninit ( x265_enc_bin_if_t *enc_bin_if )
{
	x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter = NULL ;

	enc_bin_cabac_counter = (x265_enc_bin_cabac_counter_t*) enc_bin_if ;
	enc_bin_cabac_counter->bit_if = NULL ;
}

void enc_bin_cabac_counter_start ( x265_enc_bin_if_t *enc_bin_if )
{
	x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter = NULL ;

	enc_bin_cabac_counter = (x265_enc_bin_cabac_counter_t*) enc_bin_if ;
	enc_bin_cabac_counter->i_low = 0 ;
	enc_bin_cabac_counter->i_range = 510 ;
	enc_bin_cabac_counter->i_queue = -9 ;
	enc_bin_cabac_counter->i_num_buffered_bytes = 0 ;
	enc_bin_cabac_counter->i_buffered_byte = 0xFF ;
}

void enc_bin_cabac_counter_finish ( x265_enc_bin_if_t *enc_bin_if )
{
	x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter = NULL ;

	enc_bin_cabac_counter = (x265_enc_bin_cabac_counter_t*) enc_bin_if ;
	enc_bin_cabac_counter->bit_if->write ( enc_bin_cabac_counter->bit_if,
											0, (int)(enc_bin_cabac_counter->i_frac_bits >> 15) );
	enc_bin_cabac_counter->i_frac_bits &= 32767 ;
}

void enc_bin_cabac_counter_flush ( x265_enc_bin_if_t *enc_bin_if )
{
	x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter = NULL ;

	enc_bin_cabac_counter = (x265_enc_bin_cabac_counter_t*) enc_bin_if ;

	enc_bin_cabac_counter_encode_bin_trm ( enc_bin_if, 1 ) ;
	enc_bin_cabac_counter_finish ( enc_bin_if ) ;
	enc_bin_cabac_counter->bit_if->write ( enc_bin_cabac_counter->bit_if, 1, 1) ;
	enc_bin_cabac_counter->bit_if->write_align_zero ( enc_bin_cabac_counter->bit_if ) ;
	enc_bin_cabac_counter_start ( enc_bin_if ) ;
}

void enc_bin_cabac_counter_reset_bac ( x265_enc_bin_if_t *enc_bin_if )
{
	enc_bin_cabac_counter_start ( enc_bin_if ) ;
}


void enc_bin_cabac_counter_encode_pcm_align_bits ( x265_enc_bin_if_t *enc_bin_if )
{
	x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter = NULL ;

	enc_bin_cabac_counter = (x265_enc_bin_cabac_counter_t*) enc_bin_if ;
	enc_bin_cabac_counter_finish ( enc_bin_if ) ;
	enc_bin_cabac_counter->bit_if->write ( enc_bin_cabac_counter->bit_if, 1, 1) ;
	enc_bin_cabac_counter->bit_if->write_align_zero ( enc_bin_cabac_counter->bit_if ) ;
}

void enc_bin_cabac_counter_x_write_pcm_code ( x265_enc_bin_if_t *enc_bin_if, uint32_t i_code, uint32_t i_length )
{
	x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter = NULL ;

	enc_bin_cabac_counter = (x265_enc_bin_cabac_counter_t*) enc_bin_if ;

	enc_bin_cabac_counter->bit_if->write ( enc_bin_cabac_counter->bit_if, i_code, i_length ) ;
}

void enc_bin_cabac_counter_copy_state ( x265_enc_bin_if_t *src_enc_bin_if,
										x265_enc_bin_if_t *dst_enc_bin_if )
{
	x265_enc_bin_cabac_counter_t *src_enc_bin_cabac_counter = NULL ;
	x265_enc_bin_cabac_counter_t *dst_enc_bin_cabac_counter = NULL ;

	src_enc_bin_cabac_counter = (x265_enc_bin_cabac_counter_t*) src_enc_bin_if ;
	dst_enc_bin_cabac_counter = (x265_enc_bin_cabac_counter_t*) dst_enc_bin_if ;

	dst_enc_bin_cabac_counter->i_low = src_enc_bin_cabac_counter->i_low ;
	dst_enc_bin_cabac_counter->i_range = src_enc_bin_cabac_counter->i_range ;
	dst_enc_bin_cabac_counter->i_queue = src_enc_bin_cabac_counter->i_queue ;
	dst_enc_bin_cabac_counter->i_num_buffered_bytes = src_enc_bin_cabac_counter->i_num_buffered_bytes ;
	dst_enc_bin_cabac_counter->i_buffered_byte = src_enc_bin_cabac_counter->i_buffered_byte ;
	dst_enc_bin_cabac_counter->i_frac_bits = src_enc_bin_cabac_counter->i_frac_bits ;
	memcpy( dst_enc_bin_cabac_counter->state, src_enc_bin_cabac_counter->state,
			X265_CABAC_STATE_NUMBER * sizeof( uint8_t ) );
	memcpy( dst_enc_bin_cabac_counter->bins_coded, src_enc_bin_cabac_counter->bins_coded,
			X265_CABAC_STATE_NUMBER * sizeof( uint32_t ) );
}


void enc_bin_cabac_counter_reset_bits ( x265_enc_bin_if_t *enc_bin_if )
{
	x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter = NULL ;

	enc_bin_cabac_counter = (x265_enc_bin_cabac_counter_t*) enc_bin_if ;
	enc_bin_cabac_counter->i_low = 0 ;
	enc_bin_cabac_counter->i_queue = -9 ;
	enc_bin_cabac_counter->i_num_buffered_bytes = 0 ;
	enc_bin_cabac_counter->i_buffered_byte = 0xFF ;
	enc_bin_cabac_counter->i_frac_bits &= 32767 ;
	if ( enc_bin_cabac_counter->i_bin_count_increment )
	{
		enc_bin_cabac_counter->i_bins_coded = 0 ;
	}
}

void enc_bin_cabac_counter_clear_bits ( x265_enc_bin_if_t *enc_bin_if )
{
	x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter = NULL ;

	enc_bin_cabac_counter = (x265_enc_bin_cabac_counter_t*) enc_bin_if ;
	enc_bin_cabac_counter->i_low = 0 ;
	enc_bin_cabac_counter->i_queue = -9 ;
	enc_bin_cabac_counter->i_num_buffered_bytes = 0 ;
	enc_bin_cabac_counter->i_buffered_byte = 0xFF ;
	enc_bin_cabac_counter->i_frac_bits = 0 ;
	if ( enc_bin_cabac_counter->i_bin_count_increment )
	{
		enc_bin_cabac_counter->i_bins_coded = 0 ;
	}
}

uint32_t enc_bin_cabac_counter_get_num_written_bits ( x265_enc_bin_if_t *enc_bin_if )
{
	x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter = NULL ;

	enc_bin_cabac_counter = (x265_enc_bin_cabac_counter_t*) enc_bin_if ;

	return enc_bin_cabac_counter->bit_if->get_number_of_written_bits (enc_bin_cabac_counter->bit_if)
			+ (uint32_t) ( enc_bin_cabac_counter->i_frac_bits >> 15 ) ;
}


void enc_bin_cabac_counter_encode_bin ( x265_enc_bin_if_t *enc_bin_if, int32_t b, int32_t i_ctx )
{
    int i_state = 0 ;
	x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter = NULL ;

	enc_bin_cabac_counter = (x265_enc_bin_cabac_counter_t*) enc_bin_if ;

	enc_bin_cabac_counter->i_bins_coded += enc_bin_cabac_counter->i_bin_count_increment ;
    i_state = enc_bin_cabac_counter->state[i_ctx];
    enc_bin_cabac_counter->i_frac_bits += x265_entropy_bits[i_state^b] ;
    //	print_int64_state ( enc_bin_cabac_counter->i_frac_bits ) ;
    enc_bin_cabac_counter->state[i_ctx] = x265_cabac_transition[i_state][b];
}

void enc_bin_cabac_counter_encode_bin_ep ( x265_enc_bin_if_t *enc_bin_if, int32_t b )
{
	x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter = NULL ;

	enc_bin_cabac_counter = (x265_enc_bin_cabac_counter_t*) enc_bin_if ;

	enc_bin_cabac_counter->i_bins_coded += enc_bin_cabac_counter->i_bin_count_increment ;
    enc_bin_cabac_counter->i_frac_bits += 32768 ;
}

void enc_bin_cabac_counter_encode_bins_ep ( x265_enc_bin_if_t *enc_bin_if, int32_t i_bin_values, uint32_t i_num_bins )
{
	x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter = NULL ;

	enc_bin_cabac_counter = (x265_enc_bin_cabac_counter_t*) enc_bin_if ;

	enc_bin_cabac_counter->i_bins_coded += (i_num_bins & (-enc_bin_cabac_counter->i_bin_count_increment)) ;
    enc_bin_cabac_counter->i_frac_bits += 32768 * i_num_bins ;
}

void enc_bin_cabac_counter_encode_bin_trm ( x265_enc_bin_if_t *enc_bin_if, int32_t b )
{
	x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter = NULL ;

	enc_bin_cabac_counter = (x265_enc_bin_cabac_counter_t*) enc_bin_if ;
	enc_bin_cabac_counter->i_bins_coded += enc_bin_cabac_counter->i_bin_count_increment ;
	enc_bin_cabac_counter->i_frac_bits += x265_entropy_bits[126^b] ;
}


void enc_bin_cabac_counter_print_cabac_state ( x265_enc_bin_if_t *enc_bin_if )
{
	int32_t loop = 0 ;
	FILE *file = NULL ;
	static int32_t b_first_print_cabac_state = 1 ;
	x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter = NULL ;

	enc_bin_cabac_counter = (x265_enc_bin_cabac_counter_t*) enc_bin_if ;

	if ( b_first_print_cabac_state )
	{
		file = fopen ( "CabacState2", "wb" ) ;
		b_first_print_cabac_state = 0 ;
	}
	else
	{
		file = fopen ( "CabacState2", "r+b" ) ;
		fseek ( file, 0, SEEK_END ) ;
	}

	for ( loop = 0 ; loop < X265_CABAC_STATE_NUMBER ; ++ loop )
	{
		fprintf ( file, "%d\n", enc_bin_cabac_counter->state[loop] ) ;
	}



	fclose (file) ;
}


int32_t enc_bin_cabac_counter_get_entropy_bits ( x265_enc_bin_if_t *enc_bin_if, int32_t b, int32_t i_ctx )
{
	x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter = NULL ;

	enc_bin_cabac_counter = (x265_enc_bin_cabac_counter_t*) enc_bin_if ;
	return x265_entropy_bits[enc_bin_cabac_counter->state[i_ctx] ^ b];
}

void enc_bin_cabac_counter_reset_entropy ( x265_t *h,
											x265_enc_bin_if_t *enc_bin_if,
											enum slice_type_e i_slice_type,
											int32_t i_slice_qp )
{
	int32_t loop = 0 ;
	int32_t i_qp = 0 ;
	int32_t i_slope = 0 ;
	int32_t i_offset = 0 ;
	int32_t i_init_state = 0 ;
	int32_t b_mp_state = 0 ;

	uint32_t i_enc_cabac_counter_table_idx = 0 ;
	const uint8_t *cabac_counter_context_init = NULL ;
	x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter = NULL ;

	enc_bin_cabac_counter = (x265_enc_bin_cabac_counter_t*) enc_bin_if ;

	i_enc_cabac_counter_table_idx = h->pps[0].i_enc_cabac_table_idx ;
	if ( I_SLICE != i_slice_type
		&& (B_SLICE == i_enc_cabac_counter_table_idx || P_SLICE == i_enc_cabac_counter_table_idx )
		&& h->pps[0].b_cabac_init_present_flag )
	{
		i_slice_type = (enum slice_type_e) i_enc_cabac_counter_table_idx ;
	}

	if ( I_SLICE == i_slice_type )
	{
		cabac_counter_context_init = x265_cabac_context_init_I ;
	}
	else if ( P_SLICE == i_slice_type )
	{
		cabac_counter_context_init = x265_cabac_context_init_PB1 ;
	}
	else if ( B_SLICE == i_slice_type )
	{
		cabac_counter_context_init = x265_cabac_context_init_PB2 ;
	}

	for ( loop = 0 ; loop < X265_CABAC_STATE_NUMBER ; ++ loop )
	{
		i_qp = x265_clip3_int32 ( i_slice_qp, 0, 51 ) ;
		i_slope = ( cabac_counter_context_init[loop] >> 4 ) * 5 - 45 ;
		i_offset = ( ( cabac_counter_context_init[loop] & 15 ) << 3 ) - 16 ;
		i_init_state = X265_MIN ( X265_MAX ( 1, ( ( ( i_slope * i_qp ) >> 4 ) + i_offset ) ), 126 );
		b_mp_state = ( i_init_state >= 64 ) ;
		enc_bin_cabac_counter->state[loop] = ((b_mp_state
											? (i_init_state - 64)
											: (63 - i_init_state)) <<1) + b_mp_state ;
		enc_bin_cabac_counter->bins_coded[loop] = 0 ;
	}
}

void enc_bin_cabac_counter_determine_cabac_init_idx ( x265_t *h,
													x265_enc_bin_if_t *enc_bin_if,
													enum slice_type_e i_slice_type,
													int32_t i_slice_qp )
{
	uint32_t i_idx = 0 ;
	uint32_t loop = 0 ;
	uint32_t i_best_cost = 0 ;
	uint32_t i_cur_cost = 0 ;
	const uint8_t *cabac_counter_context_inits[2] ;
	enum slice_type_e i_best_slice_type = 0 ;
	enum slice_type_e i_cur_slice_type = 0 ;
	enum slice_type_e slice_type_choices[2] ;

	x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter = NULL ;

	enc_bin_cabac_counter = (x265_enc_bin_cabac_counter_t*) enc_bin_if ;
	cabac_counter_context_inits[0] = x265_cabac_context_init_PB2 ;
	cabac_counter_context_inits[1] = x265_cabac_context_init_PB1 ;

	if ( I_SLICE != i_slice_type )
	{
		slice_type_choices[0] = B_SLICE ;
		slice_type_choices[1] = P_SLICE ;
		i_best_cost = X265_MAX_UINT ;
		i_best_slice_type = slice_type_choices[0] ;

		for ( i_idx = 0 ; i_idx < 2 ; ++ i_idx )
		{
			i_cur_cost = 0 ;
			i_cur_slice_type = slice_type_choices[i_idx] ;
			for ( loop = 0 ; loop < X265_CABAC_STATE_NUMBER ; ++ loop )
			{
				i_cur_cost += enc_bin_cabac_counter_calc_cost ( enc_bin_cabac_counter, i_slice_qp,
														cabac_counter_context_inits[i_idx][loop], loop ) ;
			}
			if ( i_cur_cost < i_best_cost )
			{
				i_best_cost = i_cur_cost ;
				i_best_slice_type = i_cur_slice_type ;
			}
		}
		h->pps[0].i_enc_cabac_table_idx = i_best_slice_type ;
	}
	else
	{
		h->pps[0].i_enc_cabac_table_idx = I_SLICE ;
	}
}

void enc_bin_cabac_counter_update_context_table( x265_enc_bin_if_t *enc_bin_if,
												enum slice_type_e i_slice_type,
												int32_t i_slice_qp )
{
	int32_t loop = 0 ;
	int32_t i_qp = 0 ;
	int32_t i_slope = 0 ;
	int32_t i_offset = 0 ;
	int32_t i_init_state = 0 ;
	int32_t b_mp_state = 0 ;

	const uint8_t *cabac_context_init = NULL ;
	x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter = NULL ;

	enc_bin_cabac_counter = (x265_enc_bin_cabac_counter_t*) enc_bin_if ;


	if ( I_SLICE == i_slice_type )
	{
		cabac_context_init = x265_cabac_context_init_I ;
	}
	else if ( P_SLICE == i_slice_type )
	{
		cabac_context_init = x265_cabac_context_init_PB1 ;
	}
	else if ( B_SLICE == i_slice_type )
	{
		cabac_context_init = x265_cabac_context_init_PB2 ;
	}

	for ( loop = 0 ; loop < X265_CABAC_STATE_NUMBER ; ++ loop )
	{
		i_qp = x265_clip3_int32 ( i_slice_qp, 0, 51 ) ;
		i_slope = ( cabac_context_init[loop] >> 4 ) * 5 - 45 ;
		i_offset = ( ( cabac_context_init[loop] & 15 ) << 3 ) - 16 ;
		i_init_state = X265_MIN ( X265_MAX ( 1, ( ( ( i_slope * i_qp ) >> 4 ) + i_offset ) ), 126 );
		b_mp_state = ( i_init_state >= 64 ) ;
		enc_bin_cabac_counter->state[loop] = ((b_mp_state ? (i_init_state - 64)
												: (63 - i_init_state)) <<1) + b_mp_state ;
		enc_bin_cabac_counter->bins_coded[loop] = 0 ;
	}
}

void enc_bin_cabac_counter_load_intra_dir_mode_luma ( x265_enc_bin_if_t *enc_bin_if,
													x265_enc_bin_if_t *src_enc_bin_if )
{
	x265_enc_bin_cabac_counter_t *src_enc_bin_cabac_counter = NULL ;
	x265_enc_bin_cabac_counter_t *dst_enc_bin_cabac_counter = NULL ;

	src_enc_bin_cabac_counter = (x265_enc_bin_cabac_counter_t*) src_enc_bin_if ;
	dst_enc_bin_cabac_counter = (x265_enc_bin_cabac_counter_t*) enc_bin_if ;

	dst_enc_bin_cabac_counter->i_low = src_enc_bin_cabac_counter->i_low ;
	dst_enc_bin_cabac_counter->i_range = src_enc_bin_cabac_counter->i_range ;
	dst_enc_bin_cabac_counter->i_queue = src_enc_bin_cabac_counter->i_queue ;
	dst_enc_bin_cabac_counter->i_num_buffered_bytes = src_enc_bin_cabac_counter->i_num_buffered_bytes ;
	dst_enc_bin_cabac_counter->i_buffered_byte = src_enc_bin_cabac_counter->i_buffered_byte ;
	dst_enc_bin_cabac_counter->i_frac_bits = src_enc_bin_cabac_counter->i_frac_bits ;

	dst_enc_bin_cabac_counter->state[X265_CU_INTRA_PRED_START_IDX] = src_enc_bin_cabac_counter->state[X265_CU_INTRA_PRED_START_IDX] ;
	dst_enc_bin_cabac_counter->bins_coded[X265_CU_INTRA_PRED_START_IDX] = src_enc_bin_cabac_counter->bins_coded[X265_CU_INTRA_PRED_START_IDX] ;
}

void enc_bin_cabac_counter_x_copy_contexts_from ( x265_enc_bin_if_t *enc_bin_if,
													x265_enc_bin_if_t *src_enc_bin_if )
{
	x265_enc_bin_cabac_counter_t *src_enc_bin_cabac_counter = NULL ;
	x265_enc_bin_cabac_counter_t *dst_enc_bin_cabac_counter = NULL ;

	src_enc_bin_cabac_counter = (x265_enc_bin_cabac_counter_t*) src_enc_bin_if ;
	dst_enc_bin_cabac_counter = (x265_enc_bin_cabac_counter_t*) enc_bin_if ;

	memcpy( dst_enc_bin_cabac_counter->state, src_enc_bin_cabac_counter->state,
			X265_CABAC_STATE_NUMBER * sizeof( uint8_t ) );
	memcpy( dst_enc_bin_cabac_counter->bins_coded, src_enc_bin_cabac_counter->bins_coded,
			X265_CABAC_STATE_NUMBER * sizeof( uint32_t ) );
}


uint32_t enc_bin_cabac_counter_calc_cost ( x265_enc_bin_cabac_counter_t *enc_bin_cabac_counter,
											int32_t i_slice_qp,
											uint8_t i_init_value,
											int32_t i_index )
{
	uint8_t i_state = 0 ;
	uint32_t i_cost = 0;
	double f_prob_lps = 0.0 ;
	double f_prob0 = 0.0, f_prob1 = 0.0 ;
	static double state_to_prob_lps[] = {0.50000000, 0.47460857, 0.45050660, 0.42762859, 0.40591239, 0.38529900, 0.36573242, 0.34715948, 0.32952974, 0.31279528, 0.29691064, 0.28183267, 0.26752040, 0.25393496, 0.24103941, 0.22879875, 0.21717969, 0.20615069, 0.19568177, 0.18574449, 0.17631186, 0.16735824, 0.15885931, 0.15079198, 0.14313433, 0.13586556, 0.12896592, 0.12241667, 0.11620000, 0.11029903, 0.10469773, 0.09938088, 0.09433404, 0.08954349, 0.08499621, 0.08067986, 0.07658271, 0.07269362, 0.06900203, 0.06549791, 0.06217174, 0.05901448, 0.05601756, 0.05317283, 0.05047256, 0.04790942, 0.04547644, 0.04316702, 0.04097487, 0.03889405, 0.03691890, 0.03504406, 0.03326442, 0.03157516, 0.02997168, 0.02844963, 0.02700488, 0.02563349, 0.02433175, 0.02309612, 0.02192323, 0.02080991, 0.01975312, 0.01875000};

	i_state = x265_enc_bin_if_get_state ( i_slice_qp, i_init_value ) ;

	// Map the 64 CABAC states to their corresponding probability values

	f_prob_lps = state_to_prob_lps[(enc_bin_cabac_counter->state[i_index] >> 1)] ;
	if ( 1 == ( enc_bin_cabac_counter->state[i_index] & 1) )
	{
		f_prob0 = f_prob_lps ;
		f_prob1 = 1.0 - f_prob0 ;
	}
	else
	{
		f_prob1 = f_prob_lps ;
		f_prob0 = 1.0 - f_prob1 ;
	}

    if ( enc_bin_cabac_counter->bins_coded[i_index] )
    {
    	i_cost += (uint32_t) (f_prob0 * x265_entropy_bits[i_state^0]
    	                      + f_prob1 * x265_entropy_bits[i_state^1] );
    }

    return i_cost ;
}

void x265_enc_bin_cabac_counter_print_offset ()
{
	FILE *file = stderr ;

	// 20
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->init ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->uninit ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->start ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->finish ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->copy_state ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->flush ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->reset_bac ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->encode_pcm_align_bits ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->x_write_pcm_code ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->clear_bits ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->reset_bits ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->get_num_written_bits ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->encode_bin ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->encode_bin_ep ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->encode_bins_ep ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->encode_bin_trm ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->get_enc_bin_cabac ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->enc_bin_if_delete ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->reset_entropy ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->determine_cabac_init_idx ) ;

	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->bit_if ) ;

	// 5
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->i_bins_coded ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->i_bin_count_increment ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->state ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->bins_coded ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->i_low ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->i_range ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->i_queue ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->i_num_buffered_bytes ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->i_buffered_byte ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_counter_t*)0)->i_frac_bits ) ;


	fprintf ( file, "%d\n", sizeof(x265_enc_bin_cabac_counter_t) ) ;
}

#endif

