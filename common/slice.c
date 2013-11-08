
#include "common.h"

x265_slice_t *x265_slice_new ( x265_t *h )
{
	x265_slice_t *slice = NULL ;

	CHECKED_MALLOCZERO( slice, sizeof(x265_slice_t) );
	if ( x265_slice_init ( h, slice ) )
	{
		goto fail ;
	}

	return slice ;
fail:
	x265_slice_delete ( (void*) slice ) ;
	return NULL ;
}


void x265_slice_delete ( x265_slice_t *slice )
{
	x265_slice_deinit ( slice ) ;
	x265_free ( slice ) ;
}

int x265_slice_init ( x265_t *h, x265_slice_t *slice )
{
	int32_t loop = 0 ;

	for ( loop = 0 ; loop < X265_MAX_NUM_REF_LC ; ++ loop )
	{
		h->ref_idx_of_lc[0][loop] = -1 ;
		h->ref_idx_of_lc[1][loop] = -1 ;

		h->list_id_from_idx_of_lc[loop] = 0 ;
		h->ref_idx_from_idx_of_lc[loop] = 0 ;
		h->ref_idx_of_l0_from_ref_idx_of_l1[loop] = -1 ;
		h->ref_idx_of_l1_from_ref_idx_of_l0[loop] = -1 ;
	}
	slice ->tile_byte_location = x265_uint_list_new () ;
	if ( NULL == slice->tile_byte_location )
	{
		goto fail ;
	}

	return 0 ;

fail:
	x265_slice_deinit ( slice ) ;
	return -1 ;
}

void x265_slice_deinit ( x265_slice_t *slice )
{
	if (slice)
	{
		x265_uint_list_delete ( slice->tile_byte_location ) ;
		x265_free ( slice->substream_sizes ) ;
		slice->tile_byte_location = NULL ;
		slice->substream_sizes = NULL ;
	}
}



int x265_slice_create ( x265_slice_t *slice )
{

	return 0 ;
	/*
fail:
	destroy_slice ( slice ) ;
	return -1 ;
	*/
}

void x265_slice_destroy ( x265_slice_t *slice )
{

}

int x265_slice_initialize ( x265_slice_t *slice )
{
	memset ( slice, 0, sizeof(x265_slice_t) ) ;

	if ( x265_slice_create (slice) )
	{
		return -1 ;
	}
	init_reference_picture_set ( &slice->local_rps ) ;

	slice->i_col_from_l0_flag = 1 ;
	x265_slice_init_equal_ref ( slice ) ;
	slice->i_max_num_merge_cand = X265_MRG_MAX_NUM_CANDS ;
	slice->b_enable_tmvp_flag = 1 ;

	slice->i_slice_cur_start_cu_addr = 0 ;
	slice->i_slice_cur_end_cu_addr = 0 ;
	slice->i_slice_segment_cur_start_cu_addr = 0 ;
	slice->i_slice_segment_cur_end_cu_addr = 0 ;
	slice->i_slice_bits = 0 ;
	slice->i_slice_segment_bits = 0 ;

	return 0 ;

}

void x265_slice_deinitialize ( x265_slice_t *slice )
{
	x265_slice_destroy ( slice ) ;
}


void x265_slice_init_equal_ref ( x265_slice_t *slice )
{
	int32_t i_dir = 0 ;
	int32_t i_ref_idx1 = 0 ;
	int32_t i_ref_idx2 = 0 ;

	for ( i_dir = 0 ; i_dir < 2 ; ++ i_dir )
	{
		for ( i_ref_idx1 = 0 ; i_ref_idx1 < X265_MAX_NUM_REF ; ++ i_ref_idx1 )
		{
			for ( i_ref_idx2 = i_ref_idx1 ; i_ref_idx2 < X265_MAX_NUM_REF ; ++ i_ref_idx2 )
			{
				slice->equal_ref[i_dir][i_ref_idx1][i_ref_idx2] =
						slice->equal_ref[i_dir][i_ref_idx2][i_ref_idx1] =
						( i_ref_idx1 == i_ref_idx2 ? 1 : 0 ) ;
			}
		}
	}
}

void x265_slice_decoding_refresh_marking ( x265_t *h, x265_slice_t *slice,
		int32_t * p_poc_cra, int32_t *p_refresh_pending )
{
	int32_t i_poc_curr = 0 ;
	int32_t i_index = 0 ;

	i_poc_curr = h->fdec->i_poc ;

	if ( NAL_SLICE_BLA == slice->i_naul_type
			|| NAL_SLICE_BLANT == slice->i_naul_type
			|| NAL_SLICE_BLA_N_LP == slice->i_naul_type
			|| NAL_SLICE_IDR == slice->i_naul_type
			|| NAL_SLICE_IDR_N_LP == slice->i_naul_type )  // IDR or BLA picture
	{

		while ( h->frames.reference[i_index] )
		{
			h->frames.reference[i_index]->fdec_data->b_reference = 0 ;
			i_index ++ ;
		}
		if ( NAL_SLICE_BLA == slice->i_naul_type
				|| NAL_SLICE_BLANT == slice->i_naul_type
				|| NAL_SLICE_BLA_N_LP == slice->i_naul_type )
		{
			*p_poc_cra = i_poc_curr ;
		}
	}
	else // CRA or No DR
	{
		if ( *p_refresh_pending && i_poc_curr > *p_poc_cra ) // CRA reference marking pending
		{
			while ( h->frames.reference[i_index] )
			{
				if ( *p_poc_cra != h->frames.reference[i_index]->i_poc )
				{
					h->frames.reference[i_index]->fdec_data->b_reference = 0 ;
				}
				i_index ++ ;
			}
			*p_refresh_pending = 0 ;
		}
		if ( NAL_SLICE_CRA == slice->i_naul_type ) // CRA picture found
		{
			*p_refresh_pending = 1 ;
			*p_poc_cra = i_poc_curr ;
		}
	}
}

void x265_slice_select_reference_picture_set ( x265_t *h, x265_slice_t* slice,
								int32_t i_poc_curr, int32_t i_gop_id )
{
	int32_t i_extra_num = 0 ;
	int32_t i_poc_index = 0 ;
	slice->i_rps_idx = i_gop_id ;

	for ( i_extra_num = h->param.gop.i_gop_size ;
			i_extra_num < h->param.gop.i_gop_size + h->param.gop.i_extraRPSs ;
			++ i_extra_num )
	{
		if ( h->param.gop.i_intra_period > 0 && h->param.gop.i_decoding_refresh_type > 0)
		{
			i_poc_index = i_poc_curr % h->param.gop.i_intra_period ;
			if ( 0 == i_poc_index )
			{
				i_poc_index = h->param.gop.i_intra_period ;
			}
			if ( i_poc_index == h->param.gop.gop_list[i_extra_num].i_poc )
			{
				slice->i_rps_idx = i_extra_num ;
			}
		}
		else
		{
			if ( i_poc_curr == h->param.gop.gop_list[i_extra_num].i_poc )
			{
				slice->i_rps_idx = i_extra_num ;
			}
		}
	}

	slice->rps = &h->sps[0].rps_list.reference_picture_sets[slice->i_rps_idx] ;
	slice->rps->i_number_of_pictures = slice->rps->i_number_of_negative_pictures + slice->rps->i_number_of_positive_pictures ;
}

int x265_slice_check_that_all_ref_pics_are_available ( x265_t *h, x265_slice_t *slice,
		x265_reference_picture_set_t *rps, int32_t b_print_errors, int32_t i_poc_random_access )
{
	int32_t i = 0, j = 0 ;
	int32_t b_is_available = 0 ;
	int32_t b_at_least_one_lost = 0;
	int32_t b_at_least_one_removed = 0;
	int32_t i_poc_lost = 0;

	// loop through all long-term pictures in the Reference Picture Set
	// to see if the picture should be kept as reference picture
	for ( i = rps->i_number_of_negative_pictures + rps->i_number_of_positive_pictures ;
			i < rps->i_number_of_pictures ;
			++ i )
	{
		j = 0;
		b_is_available = 0;
		while ( h->frames.reference[j] )
		{
			if ( rps->check_lt_msb[i] )
			{
				if ( h->frames.reference[j]->fdec_data->b_is_long_term
					&& h->frames.reference[j]->i_poc == rps->poc[i]
					&& h->frames.reference[j]->fdec_data->b_reference )
				{
					b_is_available = 1;
				}
			}
			else
			{
				if ( h->frames.reference[j]->fdec_data->b_is_long_term
					&&
					(
						(h->frames.reference[j]->i_poc%(1 << h->sps[0].i_bits_for_poc))
						== (rps->poc[i] % (1 << h->sps[0].i_bits_for_poc))
					)
					&& h->frames.reference[j]->fdec_data->b_reference )
				{
					b_is_available = 1;
				}
			}
			j ++ ;
		}
		// if there was no such long-term check the short terms
		if ( !b_is_available )
		{
			j = 0 ;
			while ( h->frames.reference[j] )
			{
				if (
					(
					(h->frames.reference[j]->i_poc % (1<<h->sps[0].i_bits_for_poc))
					== ((h->fdec->i_poc + rps->delta_poc[j]) % (1<<h->sps[0].i_bits_for_poc))
					)
					&& h->frames.reference[j]->fdec_data->b_reference
					)
				{

			          b_is_available = 1;
			          h->frames.reference[j]->fdec_data->b_is_long_term = 1 ;
			          break ;
				}
				j++ ;
			}
		}
		// report that a picture is lost if it is in the Reference Picture Set
		// but not available as reference picture
		if ( b_is_available == 0 )
		{
			if (h->fdec->i_poc + rps->delta_poc[i] >= i_poc_random_access)
			{
				if ( !rps->used[i] )
				{
					if ( b_print_errors )
					{
						fprintf ( stderr,
								"\nLong-term reference picture with POC = %3d seems to have been removed or not correctly decoded.",
								h->fdec->i_poc + rps->delta_poc[i] );
					}
					b_at_least_one_removed = 1;
				}
				else
				{
					if ( b_print_errors )
					{
						fprintf ( stderr,
								"\nLong-term reference picture with POC = %3d is lost or not correctly decoded!",
								h->fdec->i_poc + rps->delta_poc[i]);
					}
					b_at_least_one_lost = 1 ;
					i_poc_lost = h->fdec->i_poc + rps->delta_poc[i] ;
				}
			}
		}
	}
	// loop through all short-term pictures in the Reference Picture Set
	// to see if the picture should be kept as reference picture
	for ( i = 0 ;
			i < rps->i_number_of_negative_pictures + rps->i_number_of_positive_pictures ;
			++ i )
	{
		j = 0;
		b_is_available = 0;
		while ( h->frames.reference[j] )
		{
			if ( !h->frames.reference[j]->fdec_data->b_is_long_term
				&& h->frames.reference[j]->i_poc == h->fdec->i_poc + rps->delta_poc[i]
				&& h->frames.reference[j]->fdec_data->b_reference )
			{
				b_is_available = 1;
			}
			j++ ;
		}
		// report that a picture is lost if it is in the Reference Picture Set
		// but not available as reference picture
		if ( b_is_available == 0 )
		{
			if ( h->fdec->i_poc + rps->delta_poc[i] >= i_poc_random_access )
			{
				if( !rps->used[i] )
				{
					if ( b_print_errors )
					{
						fprintf ( stderr,
								"\nShort-term reference picture with POC = %3d seems to have been removed or not correctly decoded.",
								h->fdec->i_poc + rps->delta_poc[i]);
					}
					b_at_least_one_removed = 1;
				}
				else
				{
					if ( b_print_errors )
					{
						fprintf ( stderr,
								"\nShort-term reference picture with POC = %3d is lost or not correctly decoded!",
								h->fdec->i_poc + rps->delta_poc[i]);
					}
					b_at_least_one_lost = 1;
					i_poc_lost = h->fdec->i_poc + rps->delta_poc[i];
				}
			}
		}
	}
	if ( b_at_least_one_lost )
	{
		return i_poc_lost + 1;
	}
	if ( b_at_least_one_removed )
	{
		return -2;
	}

	return 0;
}

void x265_slice_create_explicit_reference_picture_set_from_reference (
		x265_t *h, x265_slice_t *slice,
		x265_reference_picture_set_t *reference_picture_set )
{
	int32_t i = 0, j = 0, k = 0;
	int32_t i_num_of_negative_pictures = 0;
	int32_t i_num_of_positive_pictures = 0;
	int32_t i_rps_idx = 0 ;
	int32_t i_delta_rps = 0 ;
	int32_t i_ref_pics = 0 ;
	int32_t i_new_idc=0;
	int32_t i_delta_poc = 0 ;
	int32_t i_ref_idc = 0;
	x265_reference_picture_set_t* rps = NULL ;
	x265_reference_picture_set_t* ref_rps = NULL ;

	rps = & slice->local_rps ;
	// loop through all pictures in the Reference Picture Set
	for( i = 0 ; i < reference_picture_set->i_number_of_pictures ; ++ i )
	{
		j = 0;
		// loop through all pictures in the reference picture buffer
		while ( h->frames.reference[j] )
		{
			if(h->frames.reference[j]->i_poc == h->fdec->i_poc + reference_picture_set->delta_poc[i]
				&& h->frames.reference[j]->fdec_data->b_reference)
			{
				// This picture exists as a reference picture
				// and should be added to the explicit Reference Picture Set
				rps->delta_poc[k] = reference_picture_set->delta_poc[i] ;
				rps->used[k] = reference_picture_set->used[i] ;
				if(rps->delta_poc[k] < 0)
				{
					i_num_of_negative_pictures++;
				}
				else
				{
					i_num_of_positive_pictures++;
				}
				k++;
			}
			j ++ ;
		}
	}
	rps->i_number_of_negative_pictures = i_num_of_negative_pictures ;
	rps->i_number_of_positive_pictures = i_num_of_positive_pictures ;
	rps->i_number_of_pictures = i_num_of_negative_pictures+i_num_of_positive_pictures ;
	// This is a simplistic inter rps example. A smarter encoder will look for a better reference RPS to do the
	// inter RPS prediction with.  Here we just use the reference used by reference_picture_set.
	// If reference_picture_set is not inter_RPS_predicted, then inter_RPS_prediction is for the current RPS also disabled.
	if (!reference_picture_set->b_inter_rps_prediction )
	{
		rps->b_inter_rps_prediction = 0 ;
		rps->i_num_ref_idc = 0 ;
	}
	else
	{
		i_rps_idx = slice->i_rps_idx - reference_picture_set->i_delta_ridx_minus1 - 1;
		i_delta_rps = reference_picture_set->i_delta_rps;
		ref_rps = &h->sps->rps_list.reference_picture_sets[i_rps_idx] ;
		i_ref_pics = ref_rps->i_number_of_pictures ;
		i_new_idc = 0 ;
		for ( i = 0 ; i <= i_ref_pics ; i++ )
		{
			i_delta_poc = ((i != i_ref_pics) ? ref_rps->delta_poc[i] : 0);  // check if the reference abs POC is >= 0
			i_ref_idc = 0;
			for (j=0; j < rps->i_number_of_pictures; j++) // loop through the  pictures in the new RPS
			{
				if ( (i_delta_poc + i_delta_rps) == rps->delta_poc[j])
				{
					if (rps->used[j])
					{
						i_ref_idc = 1;
					}
					else
					{
						i_ref_idc = 2;
					}
				}
			}
			rps->ref_idc[i] = i_ref_idc ;
			i_new_idc++;
		}
		rps->b_inter_rps_prediction = 1 ;
		rps->i_num_ref_idc = i_new_idc ;
		rps->i_delta_rps = i_delta_rps ;
		rps->i_delta_ridx_minus1 = reference_picture_set->i_delta_ridx_minus1 +
				h->sps[0].rps_list.i_number_of_reference_picture_sets - slice->i_rps_idx ;
	}

	slice->rps = rps ;
	slice->i_rps_idx = -1 ;
}


void x265_slice_apply_reference_picture_set ( x265_t *h, x265_slice_t *slice,
								x265_reference_picture_set_t *rps )
{
	int32_t i = 0, j = 0 ;
	int32_t b_is_reference = 0 ;

	while ( h->frames.reference[j] )
	{
		b_is_reference = 0 ;
		// loop through all pictures in the Reference Picture Set
		// to see if the picture should be kept as reference picture
		for( i = 0 ;
				i < rps->i_number_of_positive_pictures + rps->i_number_of_negative_pictures ;
				i++)
		{
			if(!h->frames.reference[j]->fdec_data->b_is_long_term
					&& h->frames.reference[j]->i_poc == h->fdec->i_poc + rps->delta_poc[i] )
			{
				b_is_reference = 1 ;
				h->frames.reference[j]->fdec_data->b_used_by_curr = rps->used[i] ;
				h->frames.reference[j]->fdec_data->b_is_long_term = 0 ;
			}
		}
		for( ; i < rps->i_number_of_pictures ; i++ )
		{
			if(rps->check_lt_msb[i])
			{
				if(h->frames.reference[j]->fdec_data->b_is_long_term && h->frames.reference[j]->i_poc == rps->poc[i] )
				{
					b_is_reference = 1 ;
					h->frames.reference[j]->fdec_data->b_used_by_curr = rps->used[i] ;
				}
			}
			else
			{
				if(h->frames.reference[j]->fdec_data->b_is_long_term
					&& (h->frames.reference[j]->i_poc%(1<<h->sps[0].i_bits_for_poc))
					== (rps->poc[i]%(1<<h->sps[0].i_bits_for_poc))
					)
				{
					b_is_reference = 1 ;
					h->frames.reference[j]->fdec_data->b_used_by_curr = rps->used[i] ;
				}
			}
		}
		// mark the picture as "unused for reference" if it is not in
		// the Reference Picture Set
		if(h->frames.reference[j]->i_poc != h->fdec->i_poc && b_is_reference == 0)
		{
			h->frames.reference[j]->fdec_data->b_reference = 0 ;
			h->frames.reference[j]->fdec_data->b_is_long_term = 0 ;
		}
		//check that pictures of higher temporal layers are not used
		assert ( h->frames.reference[j]->fdec_data->b_reference == 0
				|| h->frames.reference[j]->fdec_data->b_used_by_curr == 0
				|| h->frames.reference[j]->fdec_data->i_t_layer <= h->fdec->fdec_data->i_t_layer );
		//check that pictures of higher or equal temporal layer are not in the RPS if the current picture is a TSA picture
		if ( slice->i_naul_type == NAL_SLICE_TLA
				|| slice->i_naul_type == NAL_SLICE_TSA_N )
		{
			assert(h->frames.reference[j]->fdec_data->b_reference == 0
					||h->frames.reference[j]->fdec_data->i_t_layer < h->fdec->fdec_data->i_t_layer);
		}
		//check that pictures marked as temporal layer non-reference pictures are not used for reference
		if ( h->frames.reference[j]->i_poc != h->fdec->i_poc
			&& h->frames.reference[j]->fdec_data->i_t_layer == h->fdec->fdec_data->i_t_layer)
		{
			assert( h->frames.reference[j]->fdec_data->b_reference == 0
					|| h->frames.reference[j]->fdec_data->b_used_by_curr == 0
					|| h->frames.reference[j]->fdec_data->b_temporal_layer_non_reference_flag == 0 );
		}
		j++ ;
	}
}

int32_t x265_slice_is_temporal_layer_switching_point ( x265_t *h, x265_slice_t *slice )
{
	int32_t i_index = 0 ;
	while ( h->frames.reference[i_index] )
	{
		if ( h->frames.reference[i_index]->fdec_data->b_reference )
		{
			if ( h->frames.reference[i_index]->fdec_data->i_t_layer
					>= h->fdec->fdec_data->i_t_layer )
			{
				return 0 ;
			}
		}
		++ i_index ;
	}
	return 1 ;
}

int32_t x265_slice_is_stepwise_temporal_layer_switching_point_candidate ( x265_t *h, x265_slice_t *slice )
{
	int32_t i_index = 0 ;
	while ( h->frames.reference[i_index] )
	{
		if ( h->frames.reference[i_index]->fdec_data->b_reference
			&& h->frames.reference[i_index]->fdec_data->b_used_by_curr )
		{
			if ( h->frames.reference[i_index]->fdec_data->i_t_layer
					>= h->fdec->fdec_data->i_t_layer )
			{
				return 0 ;
			}
		}
		++ i_index ;
	}
	return 1 ;
}

int32_t x265_slice_get_lsb(int32_t poc, int32_t maxLSB)
{
	if (poc >= 0)
	{
		return poc % maxLSB;
	}
	return (maxLSB - ((-poc) % maxLSB)) % maxLSB;
}

void x265_slice_swap_i ( int32_t *i, int32_t *j )
{
	int32_t i_temp = 0 ;

	i_temp = *i ;
	*i = *j ;
	*j = i_temp ;
}

void x265_slice_arrange_longterm_pictures_in_rps ( x265_t *h, x265_slice_t *slice )
{
	// Arrange long-term reference pictures in the correct order of LSB and MSB,
	// and assign values for pocLSBLT and MSB present flag
	int32_t longtermPicsPoc[X265_MAX_NUM_REF_PICS] ;
	int32_t longtermPicsLSB[X265_MAX_NUM_REF_PICS] ;
	int32_t indices[X265_MAX_NUM_REF_PICS] ;
	int32_t longtermPicsMSB[X265_MAX_NUM_REF_PICS] ;
	int32_t mSBPresentFlag[X265_MAX_NUM_REF_PICS];
	int32_t tempArray[X265_MAX_NUM_REF_PICS] ;
	// Get the long-term reference pictures
	int32_t offset = 0 ;
	int32_t loop = 0, i = 0, j = 0 ;
	int32_t i_index = 0 ;
	int32_t ctr = 0;
	int32_t maxPicOrderCntLSB = 0 ;
	int32_t numLongPics = 0 ;

	int32_t currMSB = 0, currLSB = 0;


	x265_reference_picture_set_t * rps = NULL ;

	rps = slice->rps ;
	if ( !rps->i_number_of_longterm_pictures )
	{
		return ;
	}


	memset(longtermPicsPoc, 0, sizeof(longtermPicsPoc));    // Store POC values of LTRP
	memset(longtermPicsLSB, 0, sizeof(longtermPicsLSB));    // Store POC LSB values of LTRP
	memset(longtermPicsMSB, 0, sizeof(longtermPicsMSB));    // Store POC LSB values of LTRP
	memset(indices        , 0, sizeof(indices));            // Indices to aid in tracking sorted LTRPs
	memset(mSBPresentFlag , 0, sizeof(mSBPresentFlag));     // Indicate if MSB needs to be present

	// Get the long-term reference pictures
	offset = rps->i_number_of_negative_pictures + rps->i_number_of_positive_pictures ;
	maxPicOrderCntLSB = 1 << h->sps->i_bits_for_poc ;
	for( loop = rps->i_number_of_pictures - 1; loop >= offset; loop--, ctr++)
	{
		longtermPicsPoc[ctr] = rps->poc[loop] ;                                  // LTRP POC
		longtermPicsLSB[ctr] = x265_slice_get_lsb(longtermPicsPoc[ctr], maxPicOrderCntLSB); // LTRP POC LSB
    	indices[ctr]      = loop ;
    	longtermPicsMSB[ctr] = longtermPicsPoc[ctr] - longtermPicsLSB[ctr];
	}
	numLongPics = rps->i_number_of_longterm_pictures;
	assert(ctr == numLongPics);

	// Arrange pictures in decreasing order of MSB;
	for(i = 0; i < numLongPics; i++)
	{
		for( j = 0; j < numLongPics - 1; j++)
		{
			if(longtermPicsMSB[j] < longtermPicsMSB[j+1])
			{
				x265_slice_swap_i(&longtermPicsPoc[j], &longtermPicsPoc[j+1]);
				x265_slice_swap_i(&longtermPicsLSB[j], &longtermPicsLSB[j+1]);
				x265_slice_swap_i(&longtermPicsMSB[j], &longtermPicsMSB[j+1]);
				x265_slice_swap_i(&indices[j]        , &indices[j+1]        );
			}
		}
	}

	for ( loop = 0; loop < numLongPics; ++ loop )
	{
		// Check if MSB present flag should be enabled.
		// Check if the buffer contains any pictures that have the same LSB.
		i_index = 0 ;
		while ( h->frames.reference[i_index] )
    	{
			// Same LSB
			if( (x265_slice_get_lsb(h->frames.reference[i_index]->i_poc, maxPicOrderCntLSB)
					== longtermPicsLSB[loop])
					// Reference picture
					&& h->frames.reference[i_index]->fdec_data->b_reference
					// Not the LTRP itself
					&& (h->frames.reference[i_index]->i_poc != longtermPicsPoc[loop]) )
			{
				mSBPresentFlag[loop] = 1 ;
				break;
			}
			++ i_index ;
    	}
	}

	// tempArray for usedByCurr flag
	memset(tempArray, 0, sizeof(tempArray));
	for(loop = 0; loop < numLongPics; ++ loop)
	{
		tempArray[loop] = rps->used[indices[loop]] ;
	}
	// Now write the final values;
	ctr = 0 ;
	currMSB = 0 ;
	currLSB = 0;
	// currPicPoc = currMSB + currLSB
	currLSB = x265_slice_get_lsb (h->fdec->i_poc, maxPicOrderCntLSB);
	currMSB = h->fdec->i_poc - currLSB;

	for(loop = rps->i_number_of_pictures - 1; loop >= offset; loop--, ctr++)
	{
		rps->poc[loop] = longtermPicsPoc[ctr] ;
		rps->delta_poc[loop] = - h->fdec->i_poc + longtermPicsPoc[ctr] ;
		rps->used[loop] = tempArray[ctr] ;
		rps->poc_lsb_lt[loop] = longtermPicsLSB[ctr] ;
		rps->delta_poc_msb_cycle_lt[loop] = (currMSB - (longtermPicsPoc[ctr] - longtermPicsLSB[ctr])
											/ maxPicOrderCntLSB);
		rps->delta_poc_msb_present_flag[loop] = mSBPresentFlag[ctr] ;

		assert(rps->delta_poc_msb_cycle_lt[loop] >= 0);   // Non-negative value
	}
	for(i = rps->i_number_of_pictures - 1, ctr = 1; i >= offset; i--, ctr++)
	{
		for( j = rps->i_number_of_pictures - 1 - ctr; j >= offset; j--)
		{
			// Here at the encoder we know that we have set the full POC value for the LTRPs, hence we
			// don't have to check the MSB present flag values for this constraint.
			assert( rps->poc[i] != rps->poc[j] ); // If assert fails, LTRP entry repeated in RPS!!!
		}
	}
}

void x265_slice_set_ref_pic_list ( x265_t *h, x265_slice_t *slice )
{
	int32_t loop = 0 ;
	uint32_t i_num_poc_st_curr0 = 0 ;
	uint32_t i_num_poc_st_curr1 = 0 ;
	uint32_t i_num_poc_lt_curr = 0 ;
	x265_frame_t *ref_pic = NULL ;
	x265_frame_t *ref_pic_set_st_curr0[16] ;
	x265_frame_t *ref_pic_set_st_curr1[16] ;
	x265_frame_t *ref_pic_set_lt_curr[16] ;
	x265_frame_t *rps_curr_list0[X265_MAX_NUM_REF+1];
	x265_frame_t *rps_curr_list1[X265_MAX_NUM_REF+1];
	int32_t i_num_poc_total_curr = 0 ;
	int32_t i_cidx = 0 ;
	int32_t i_ridx = 0 ;

	x265_reference_picture_set_t *rps ;

	if ( I_SLICE == slice->i_slice_type )
	{
		memset ( h->fref[0], 0, h->frames.i_reference_and_current * sizeof (x265_frame_t *));
		memset ( h->fref[1], 0, h->frames.i_reference_and_current * sizeof (x265_frame_t *));
		memset ( h->i_ref,   0, 2 * sizeof ( int ));

		return;
	}

	rps = slice->rps ;
	for ( loop = 0 ; loop < rps->i_number_of_negative_pictures ; ++ loop )
	{
		if(rps->used[loop])
		{
			ref_pic = x265_frame_get_from_poc ( h->frames.reference, h->fdec->i_poc + rps->delta_poc[loop] ) ;
			ref_pic->fdec_data->b_is_long_term = 0 ;
			ref_pic_set_st_curr0[i_num_poc_st_curr0] = ref_pic;
			i_num_poc_st_curr0++;
		}
	}
	for ( ; loop < rps->i_number_of_negative_pictures + rps->i_number_of_positive_pictures ; ++ loop )
	{
		if(rps->used[loop])
		{
			ref_pic = x265_frame_get_from_poc ( h->frames.reference, h->fdec->i_poc + rps->delta_poc[loop] ) ;
			ref_pic->fdec_data->b_is_long_term = 0 ;
			ref_pic_set_st_curr1[i_num_poc_st_curr1] = ref_pic;
			i_num_poc_st_curr1++;
		}
	}
	for ( loop = rps->i_number_of_negative_pictures + rps->i_number_of_positive_pictures
			+ rps->i_number_of_longterm_pictures - 1 ;
			loop > rps->i_number_of_negative_pictures + rps->i_number_of_positive_pictures - 1 ;
			--loop )
	{
		if(rps->used[loop])
		{
			ref_pic = x265_frame_get_long_term_from_poc ( h->frames.reference,
														rps->poc[loop],
														h->sps[0].i_bits_for_poc ) ;
			ref_pic->fdec_data->b_is_long_term = 1 ;
			ref_pic_set_lt_curr[i_num_poc_lt_curr] = ref_pic;
			i_num_poc_lt_curr++;
		}
		if ( NULL == ref_pic )
		{
			ref_pic = x265_frame_get_long_term_from_poc ( h->frames.reference,
														rps->poc[loop],
														h->sps[0].i_bits_for_poc ) ;
		}
	}

	// ref_pic_list_init
	i_num_poc_total_curr = i_num_poc_st_curr0 + i_num_poc_st_curr1 + i_num_poc_lt_curr;

	i_cidx = 0 ;
    for ( loop = 0 ; loop < i_num_poc_st_curr0 ; ++ loop, ++ i_cidx)
    {
      rps_curr_list0[i_cidx] = ref_pic_set_st_curr0[loop];
    }
    for ( loop = 0 ; loop < i_num_poc_st_curr1; ++ loop, ++ i_cidx)
    {
      rps_curr_list0[i_cidx] = ref_pic_set_st_curr1[loop];
    }
    for ( loop = 0 ; loop < i_num_poc_lt_curr ; ++ loop, ++ i_cidx)
    {
      rps_curr_list0[i_cidx] = ref_pic_set_lt_curr[loop];
    }

    i_cidx = 0 ;
    if ( B_SLICE == slice->i_slice_type )
    {
		for ( loop = 0 ; loop < i_num_poc_st_curr1 ; ++ loop, ++ i_cidx)
		{
			rps_curr_list1[i_cidx] = ref_pic_set_st_curr1[loop];
		}
		for ( loop = 0 ; loop < i_num_poc_st_curr0 ; ++ loop, ++ i_cidx)
		{
			rps_curr_list1[i_cidx] = ref_pic_set_st_curr0[loop];
		}
		for ( loop = 0 ; loop < i_num_poc_lt_curr ;  loop ++, ++ i_cidx)
		{
			rps_curr_list1[i_cidx] = ref_pic_set_lt_curr[loop];
		}
	}

    memset(h->fdec->fdec_data->is_used_as_long_term[0], 0, h->frames.i_reference_and_current * sizeof(int32_t));
    memset(h->fdec->fdec_data->is_used_as_long_term[1], 0, h->frames.i_reference_and_current * sizeof(int32_t));

    for ( i_ridx = 0; i_ridx <= (h->i_ref[0]-1); i_ridx ++)
    {
    	h->fref[0][i_ridx] = h->ref_pic_list_modification.b_ref_pic_list_modification_flag_l0
    						? rps_curr_list0[h->ref_pic_list_modification.ref_pic_set_idx_l0[i_ridx]]
    						: rps_curr_list0[i_ridx % i_num_poc_total_curr];
    	h->fdec->fdec_data->is_used_as_long_term[0][i_ridx] = h->ref_pic_list_modification.b_ref_pic_list_modification_flag_l0
    						? (h->ref_pic_list_modification.ref_pic_set_idx_l0[i_ridx] >= (i_num_poc_st_curr0 + i_num_poc_st_curr1))
    						: ((i_ridx % i_num_poc_total_curr) >= (i_num_poc_st_curr0 + i_num_poc_st_curr1));
    }
    if ( P_SLICE == slice->i_slice_type )
    {
    	h->i_ref[1] = 0;
    	memset( h->fref[1], 0, h->frames.i_reference_and_current * sizeof(x265_frame_t*));
    }
    else
    {
    	for ( i_ridx = 0; i_ridx <= (h->i_ref[1]-1); i_ridx ++)
    	{
        	h->fref[1][i_ridx] = h->ref_pic_list_modification.b_ref_pic_list_modification_flag_l1
        						? rps_curr_list1[h->ref_pic_list_modification.ref_pic_set_idx_l1[i_ridx]]
        						: rps_curr_list1[i_ridx % i_num_poc_total_curr];
        	h->fdec->fdec_data->is_used_as_long_term[1][i_ridx] = h->ref_pic_list_modification.b_ref_pic_list_modification_flag_l1
        						? (h->ref_pic_list_modification.ref_pic_set_idx_l1[i_ridx] >= (i_num_poc_st_curr0 + i_num_poc_st_curr1))
        						: ((i_ridx % i_num_poc_total_curr) >= (i_num_poc_st_curr0 + i_num_poc_st_curr1));
    	}
    }
}


void x265_slice_set_ref_poc_list ( x265_t *h )
{
	int32_t i_dir = 0 ;
	int32_t i_num_ref_idx = 0 ;

	for ( i_dir = 0; i_dir < 2; i_dir++)
	{
		for ( i_num_ref_idx = 0; i_num_ref_idx < h->i_ref[i_dir] ; ++ i_num_ref_idx )
		{
			h->fdec->fdec_data->ref_poc[i_dir][i_num_ref_idx] = h->fref[i_dir][i_num_ref_idx]->i_poc ;
		}
	}
}

void x265_slice_generate_combined_list ( x265_t *h )
{
	int32_t i_num_count = 0 ;
	int32_t i_num_ref_idx = 0 ;
	int32_t b_temp_ref_idx_in_l2 = 0 ;
	int32_t i_ref_idx_lc = 0 ;

	if ( h->i_ref[REF_PIC_LIST_C] > 0 )
	{
		h->i_ref[REF_PIC_LIST_C] = 0 ;
		for( i_num_count = 0; i_num_count < X265_MAX_NUM_REF_LC; i_num_count++)
		{
			h->ref_idx_of_lc[REF_PIC_LIST_0][i_num_count] = -1;
			h->ref_idx_of_lc[REF_PIC_LIST_1][i_num_count] = -1;
			h->list_id_from_idx_of_lc[i_num_count] = 0 ;
			h->ref_idx_from_idx_of_lc[i_num_count] = 0 ;
			h->ref_idx_of_l0_from_ref_idx_of_l1[i_num_count] = -1 ;
			h->ref_idx_of_l1_from_ref_idx_of_l0[i_num_count] = -1 ;
		}

		for ( i_num_ref_idx = 0; i_num_ref_idx < X265_MAX_NUM_REF; i_num_ref_idx++)
		{
			if(i_num_ref_idx < h->i_ref[REF_PIC_LIST_0])
			{
				b_temp_ref_idx_in_l2 = 1;
				for ( i_ref_idx_lc = 0; i_ref_idx_lc < h->i_ref[REF_PIC_LIST_C]; i_ref_idx_lc++ )
				{
					if ( h->fref[REF_PIC_LIST_0][i_num_ref_idx]->i_poc == h->fref[h->list_id_from_idx_of_lc[i_ref_idx_lc]][h->ref_idx_from_idx_of_lc[i_ref_idx_lc]]->i_poc )
					{
						h->ref_idx_of_l1_from_ref_idx_of_l0[i_num_ref_idx] = h->ref_idx_from_idx_of_lc[i_ref_idx_lc];
						h->ref_idx_of_l0_from_ref_idx_of_l1[h->ref_idx_from_idx_of_lc[i_ref_idx_lc]] = i_num_ref_idx;
						b_temp_ref_idx_in_l2 = 0;
						break;
					}
				}

				if ( b_temp_ref_idx_in_l2 )
				{
					h->list_id_from_idx_of_lc[h->i_ref[REF_PIC_LIST_C]] = REF_PIC_LIST_0;
					h->ref_idx_from_idx_of_lc[h->i_ref[REF_PIC_LIST_C]] = i_num_ref_idx;
					h->ref_idx_of_lc[REF_PIC_LIST_0][i_num_ref_idx] = h->i_ref[REF_PIC_LIST_C]++;
				}
			}

			if(i_num_ref_idx < h->i_ref[REF_PIC_LIST_1])
			{
				b_temp_ref_idx_in_l2 = 1 ;
				for ( i_ref_idx_lc = 0; i_ref_idx_lc < h->i_ref[REF_PIC_LIST_C]; i_ref_idx_lc++ )
				{
					if ( h->fref[REF_PIC_LIST_1][i_num_ref_idx]->i_poc == h->fref[h->list_id_from_idx_of_lc[i_ref_idx_lc]][h->ref_idx_from_idx_of_lc[i_ref_idx_lc]]->i_poc )
					{
						h->ref_idx_of_l0_from_ref_idx_of_l1[i_num_ref_idx] = h->ref_idx_from_idx_of_lc[i_ref_idx_lc];
						h->ref_idx_of_l1_from_ref_idx_of_l0[h->ref_idx_from_idx_of_lc[i_ref_idx_lc]] = i_num_ref_idx;
						b_temp_ref_idx_in_l2 = 0 ;
						break;
					}
				}
				if ( b_temp_ref_idx_in_l2 )
				{
					h->list_id_from_idx_of_lc[h->i_ref[REF_PIC_LIST_C]] = REF_PIC_LIST_1;
					h->ref_idx_from_idx_of_lc[h->i_ref[REF_PIC_LIST_C]] = i_num_ref_idx;
					h->ref_idx_of_lc[REF_PIC_LIST_1][i_num_ref_idx] = h->i_ref[REF_PIC_LIST_C]++;
				}
			}
		}
	}
}

#if X265_SAO_CHROMA_LAMBDA

void x265_slice_set_lambda ( x265_slice_t *slice, double f_lambda_luma, double f_lambda_chroma )
{
	slice->f_lambda_luma = f_lambda_luma ;
	slice->f_lambda_chroma = f_lambda_chroma ;
}

#else

void x265_slice_setLambda ( x265_slice_t* slice, double f_lambda )
{
	slice->f_lambda = f_lambda ;
}

#endif

int x265_slice_alloc_substream_sizes ( x265_slice_t *slice, uint32_t i_num_substreams )
{
	x265_free ( slice->substream_sizes ) ;
	slice->substream_sizes = NULL ;

	if ( i_num_substreams > 1 )
	{
		CHECKED_MALLOCZERO( slice->substream_sizes, (i_num_substreams-1) * sizeof(uint32_t) );
	}

	return 0 ;
fail:
	return -1 ;
}

void x265_slice_copy_slice_info ( x265_slice_t *slice, x265_slice_t *slice_src )
{
	int32_t i, j, k;
	int32_t e, n ;

	assert( slice_src != NULL );

	slice->i_naul_type = slice_src->i_naul_type ;
	slice->i_slice_type = slice_src->i_slice_type ;
	slice->i_slice_qp = slice_src->i_slice_qp ;

#if X265_ADAPTIVE_QP_SELECTION
	slice->i_slice_qp_base = slice_src->i_slice_qp_base ;
#endif
	slice->b_deblocking_filter_disable = slice_src->b_deblocking_filter_disable ;
	slice->b_deblocking_filter_override_flag = slice_src->b_deblocking_filter_override_flag ;
	slice->i_deblocking_filter_beta_offset_div2 = slice_src->i_deblocking_filter_beta_offset_div2 ;
	slice->i_deblocking_filter_tc_offset_div2 = slice_src->i_deblocking_filter_tc_offset_div2 ;

	slice->b_check_ldc = slice_src->b_check_ldc ;
	slice->i_slice_qp_delta = slice_src->i_slice_qp_delta ;
	slice->i_slice_qp_delta_cb = slice_src->i_slice_qp_delta_cb ;
	slice->i_slice_qp_delta_cr = slice_src->i_slice_qp_delta_cr ;

	slice->i_depth = slice_src->i_depth ;
	slice->i_last_idr = slice_src->i_last_idr ;
	slice->i_col_from_l0_flag = slice_src->i_col_from_l0_flag ;
	slice->i_col_ref_idx = slice_src->i_col_ref_idx ;

#if X265_SAO_CHROMA_LAMBDA
	slice->f_lambda_luma = slice_src->f_lambda_luma ;
	slice->f_lambda_chroma = slice_src->f_lambda_chroma ;
#else
	slice->f_lambda = slice_src->f_lambda ;
#endif

	for (i = 0; i < 2; i++)
	{
		for (j = 0; j < X265_MAX_NUM_REF; j++)
		{
			for (k =0; k < X265_MAX_NUM_REF; k++)
			{
				slice->equal_ref[i][j][k] = slice_src->equal_ref[i][j][k];
			}
		}
	}

	slice->b_no_back_pred_flag = slice_src->b_no_back_pred_flag ;
	slice->b_t_layer_switching_flag = slice_src->b_t_layer_switching_flag ;

	slice->i_slice_mode = slice_src->i_slice_mode ;
	slice->i_slice_argument = slice_src->i_slice_argument ;
	slice->i_slice_cur_start_cu_addr = slice_src->i_slice_cur_start_cu_addr ;
	slice->i_slice_cur_end_cu_addr = slice_src->i_slice_cur_end_cu_addr ;
	slice->i_slice_idx = slice_src->i_slice_idx ;
	slice->i_slice_segment_mode = slice_src->i_slice_segment_mode ;
	slice->i_slice_segment_argument = slice_src->i_slice_segment_argument ;
	slice->i_slice_segment_cur_start_cu_addr = slice_src->i_slice_segment_cur_start_cu_addr ;
	slice->i_slice_segment_cur_end_cu_addr = slice_src->i_slice_segment_cur_end_cu_addr ;

	slice->b_next_slice = slice_src->b_next_slice ;
	slice->b_next_slice_segment = slice_src->b_next_slice_segment ;


	for ( e = 0 ; e < 2 ; e++ )
	{
		for ( n = 0 ; n < X265_MAX_NUM_REF ; n++ )
		{
			memcpy( slice->weight_pred_table[e][n],
					slice_src->weight_pred_table[e][n],
					sizeof(x265_wp_scaling_param_t)*3 );
		}
	}

	slice->b_sao_enabled_flag = slice_src->b_sao_enabled_flag ;
	slice->b_sao_enabled_flag_chroma = slice_src->b_sao_enabled_flag_chroma ;
	slice->b_cabac_init_flag = slice_src->b_cabac_init_flag ;
	slice->i_num_entry_point_offsets = slice_src->i_num_entry_point_offsets ;
	slice->b_l_mvd_l1_zero = slice_src->b_l_mvd_l1_zero ;
	slice->b_lf_cross_slice_boundary_flag = slice_src->b_lf_cross_slice_boundary_flag ;
	slice->b_enable_tmvp_flag = slice_src->b_enable_tmvp_flag ;
	slice->i_max_num_merge_cand = slice_src->i_max_num_merge_cand ;
}

void x265_slice_set_tile_location ( x265_slice_t *slice, int32_t i_idx, uint32_t i_location )
{

}

int32_t x265_slice_set_get_rap_pic_flag ( x265_slice_t *slice )
{
	return slice->i_naul_type == NAL_SLICE_IDR
			|| slice->i_naul_type == NAL_SLICE_IDR_N_LP
			|| slice->i_naul_type == NAL_SLICE_BLA_N_LP
			|| slice->i_naul_type == NAL_SLICE_BLANT
			|| slice->i_naul_type == NAL_SLICE_BLA
			|| slice->i_naul_type == NAL_SLICE_CRA;
}

int32_t x265_slice_get_idr_pic_flag ( x265_slice_t *slice )
{
	return slice->i_naul_type == NAL_SLICE_IDR || slice->i_naul_type == NAL_SLICE_IDR_N_LP;
}


int32_t x265_slice_get_num_rps_curr_temp_list ( x265_slice_t *slice )
{
	uint32_t loop = 0 ;
	int32_t i_num_rps_curr_temp_list = 0;

	if ( I_SLICE == slice->i_slice_type )
	{
		return 0;
	}
	for ( loop = 0 ;
			loop < slice->rps->i_number_of_negative_pictures + slice->rps->i_number_of_positive_pictures ;
			++ loop )
	{
		if ( slice->rps->used[loop] )
		{
			i_num_rps_curr_temp_list++;
		}
	}
	return i_num_rps_curr_temp_list;
}

uint32_t x265_slice_get_tile_location_count ( x265_slice_t *slice )
{
	return x265_uint_list_size( slice->tile_byte_location ) ;
}

uint32_t x265_slice_get_tile_location ( x265_slice_t *slice, int32_t i_idx )
{
	return x265_uint_list_get_element( slice->tile_byte_location, i_idx ) ;
}

int32_t x265_slice_is_equal_ref ( x265_slice_t *slice,
									enum ref_pic_list_e i_ref_pic_list,
									int32_t i_ref_idx1,
									int32_t i_ref_idx2 )
{
	if (i_ref_idx1 < 0 || i_ref_idx2 < 0)
	{
		return 0;
	}
	return slice->equal_ref[i_ref_pic_list][i_ref_idx1][i_ref_idx2];
}

void x265_slice_print ( x265_t *h, x265_slice_t *slice )
{
	int32_t loop = 0 ;
	int32_t i_dir = 0 ;
	int32_t i_ref_idx1 = 0 ;
	int32_t i_ref_idx2 = 0 ;
	FILE *file = NULL ;
	static int32_t b_first_print_slice = 1 ;
	static int32_t i_slice_number = 1 ;

	if ( b_first_print_slice )
	{
		file = fopen ( "Slice2", "wb" ) ;
		b_first_print_slice = 0 ;
	}
	else
	{
		file = fopen ( "Slice2", "r+b" ) ;
		fseek ( file, 0, SEEK_END ) ;
	}

	fprintf ( file, "Slice%d:\n", i_slice_number ) ;
	i_slice_number ++ ;

	fprintf ( file, "%d " , slice->b_sao_enabled_flag ) ;
	fprintf ( file, "%d " , slice->b_sao_enabled_flag_chroma ) ;
	fprintf ( file, "%d " , slice->i_pps_id ) ;
	fprintf ( file, "%d " , slice->b_pic_output_flag ) ;
	fprintf ( file, "%d " , h->fdec->i_poc ) ;
	fprintf ( file, "%d " , slice->i_last_idr ) ;
	fprintf ( file, "\n" ) ;

	print_rps ( slice->rps, file ) ;

	fprintf ( file, "%d " , slice->i_rps_idx ) ;
	fprintf ( file, "\n" ) ;

	print_ref_pic_list_modification ( &h->ref_pic_list_modification, file ) ;

	fprintf ( file, "%d " , (uint32_t)slice->i_slice_type ) ;
	fprintf ( file, "%d " , (uint32_t)slice->i_naul_type ) ;

	fprintf ( file, "%d ", slice->i_slice_qp ) ;
	fprintf ( file, "%d ", slice->b_dependent_slice_segment_flag ) ;
#if X265_ADAPTIVE_QP_SELECTION
	fprintf ( file, "%d ", slice->i_slice_qp_base ) ;
#endif

	fprintf ( file, "%d ", slice->b_deblocking_filter_disable ) ;
	fprintf ( file, "%d ", slice->b_deblocking_filter_override_flag ) ;
	fprintf ( file, "%d ", slice->i_deblocking_filter_beta_offset_div2 ) ;
	fprintf ( file, "%d ", slice->i_deblocking_filter_tc_offset_div2 ) ;
	fprintf ( file, "\n" ) ;








	fprintf ( file, "%d ", h->i_ref[0] ) ;
	fprintf ( file, "\n" ) ;
	for ( loop = 0 ; loop < h->i_ref[0] ; ++ loop )
	{
		fprintf ( file, "%d ", h->fref[0][loop]->i_poc ) ;
	}
	fprintf ( file, "\n" ) ;

	fprintf ( file, "%d ", h->i_ref[1] ) ;
	fprintf ( file, "\n" ) ;
	for ( loop = 0 ; loop < h->i_ref[1] ; ++ loop )
	{
		fprintf ( file, "%d ", h->fref[1][loop]->i_poc ) ;
	}
	fprintf ( file, "\n" ) ;

	fprintf ( file, "%d ", h->i_ref[2] ) ;
	fprintf ( file, "%d ", h->b_ref_pic_list_modification_flag_lc ) ;
	fprintf ( file, "%d ", h->b_ref_pic_list_combination_flag ) ;
	fprintf ( file, "\n" ) ;

	for ( loop = 0 ; loop < X265_MAX_NUM_REF_LC ; ++ loop )
	{
		fprintf ( file, "%d ", h->ref_idx_of_lc[0][loop] ) ;
	}
	fprintf ( file, "\n" ) ;

	for ( loop = 0 ; loop < X265_MAX_NUM_REF_LC ; ++ loop )
	{
		fprintf ( file, "%d ", h->ref_idx_of_lc[1][loop] ) ;
	}
	fprintf ( file, "\n" ) ;

	for ( loop = 0 ; loop < X265_MAX_NUM_REF_LC ; ++ loop )
	{
		fprintf ( file, "%d ", h->list_id_from_idx_of_lc[loop] ) ;
	}
	fprintf ( file, "\n" ) ;
	for ( loop = 0 ; loop < X265_MAX_NUM_REF_LC ; ++ loop )
	{
		fprintf ( file, "%d ", h->ref_idx_from_idx_of_lc[loop] ) ;
	}
	fprintf ( file, "\n" ) ;

	for ( loop = 0 ; loop < X265_MAX_NUM_REF_LC ; ++ loop )
	{
		fprintf ( file, "%d ", h->ref_idx_of_l1_from_ref_idx_of_l0[loop] ) ;
	}
	fprintf ( file, "\n" ) ;
	for ( loop = 0 ; loop < X265_MAX_NUM_REF_LC ; ++ loop )
	{
		fprintf ( file, "%d ", h->ref_idx_of_l0_from_ref_idx_of_l1[loop] ) ;
	}
	fprintf ( file, "\n" ) ;



	fprintf ( file, "%d ", slice->b_check_ldc ) ;
	fprintf ( file, "%d ", slice->i_slice_qp_delta ) ;
	fprintf ( file, "%d ", slice->i_slice_qp_delta_cb ) ;
	fprintf ( file, "%d ", slice->i_slice_qp_delta_cr ) ;
	fprintf ( file, "%d ", slice->i_depth ) ;
	fprintf ( file, "%d ", h->fdec->fdec_data->b_reference ) ;
	fprintf ( file, "%d ", slice->i_col_from_l0_flag ) ;
	fprintf ( file, "%d ", slice->i_col_ref_idx ) ;
	fprintf ( file, "%d ", slice->i_max_num_merge_cand ) ;
	fprintf ( file, "\n" ) ;
#if X265_SAO_CHROMA_LAMBDA
	print_double ( slice->f_lambda_luma, file )  ;
	fprintf ( file, "\n" ) ;
	print_double ( slice->f_lambda_chroma, file )  ;
	fprintf ( file, "\n" ) ;
#else
	print_double ( slice->f_lambda, file )  ;
	fprintf ( file, "\n" ) ;
#endif
	for ( i_dir = 0 ; i_dir < 2 ; ++ i_dir )
	{
		for ( i_ref_idx1 = 0 ; i_ref_idx1 < X265_MAX_NUM_REF ; ++ i_ref_idx1 )
		{
			for ( i_ref_idx2 = i_ref_idx1 ; i_ref_idx2 < X265_MAX_NUM_REF ; ++ i_ref_idx2 )
			{
				fprintf ( file, "%d ",
						slice->equal_ref[i_dir][i_ref_idx1][i_ref_idx2] ) ;
				fprintf ( file, "%d ",
						slice->equal_ref[i_dir][i_ref_idx2][i_ref_idx1] ) ;
			}
		}
	}
	fprintf ( file, "\n" ) ;

	fprintf ( file, "%d ", slice->b_no_back_pred_flag ) ;
	fprintf ( file, "%d ", h->fdec->fdec_data->i_t_layer ) ;
	fprintf ( file, "%d ", slice->b_t_layer_switching_flag ) ;

	fprintf ( file, "%d ", slice->i_slice_mode ) ;
	fprintf ( file, "%d ", slice->i_slice_argument ) ;
	fprintf ( file, "%d ", slice->i_slice_cur_start_cu_addr ) ;
	fprintf ( file, "%d ", slice->i_slice_cur_end_cu_addr ) ;
	fprintf ( file, "%d ", slice->i_slice_idx ) ;
	fprintf ( file, "%d ", slice->i_slice_segment_mode ) ;
	fprintf ( file, "%d ", slice->i_slice_segment_argument ) ;
	fprintf ( file, "%d ", slice->i_slice_segment_cur_start_cu_addr ) ;
	fprintf ( file, "%d ", slice->i_slice_segment_cur_end_cu_addr ) ;
	fprintf ( file, "%d ", slice->b_next_slice ) ;
	fprintf ( file, "%d ", slice->b_next_slice_segment ) ;
	fprintf ( file, "%d ", slice->i_slice_bits ) ;
	fprintf ( file, "%d ", slice->i_slice_segment_bits ) ;
	fprintf ( file, "%d ", slice->b_finalized ) ;
	fprintf ( file, "%d ", slice->i_tile_offst_for_mult_es ) ;

	fprintf ( file, "%d ", slice->b_cabac_init_flag ) ;

	fprintf ( file, "%d ", slice->b_l_mvd_l1_zero ) ;
	fprintf ( file, "%d ", slice->i_num_entry_point_offsets) ;
	fprintf ( file, "%d ", h->fdec->fdec_data->b_temporal_layer_non_reference_flag ) ;
	fprintf ( file, "%d ", slice->b_lf_cross_slice_boundary_flag ) ;

	fprintf ( file, "%d ", slice->b_enable_tmvp_flag ) ;
	fprintf ( file, "\n\n\n" ) ;


	fclose ( file ) ;

}

