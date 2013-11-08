

#include "common/common.h"

int ConfirmParameter ( int bflag, const char* message )
{
	if ( ! bflag )
	{
		return 0 ;
	}

	fprintf ( stderr, "Error: %s\n", message ) ;
	return X265_PARAM_BAD_VALUE ;
}

#define CONFIRM_PARAMETER(a,b) b_error |= ConfirmParameter(a,b)

int x265_check_gop_list ( x265_param_t *p )
{
    int b_error = 0;
    int loop = 0, i = 0, j = 0, k = 0 ;
    int32_t b_verified_gop = 0 ;
    int32_t b_error_gop = 0 ;
    int32_t i_check_gop = 1 ;
    int32_t i_num_refs = 1 ;
    int32_t ai_ref_list[X265_MAX_NUM_REF_PICS + 1] ;
    int32_t b_is_ok [X265_MAX_GOP] ;
    int32_t i_num_ok = 0 ;
    int32_t i_curr_gop = 0, i_curr_poc = 0, b_before_i = 0, i_abs_poc = 0, b_found = 0 ;
	int32_t i_new_refs = 0, i_num_pref_refs = 0, b_new_ref = 0 ;
	int32_t i_offset = 0, i_off_gop = 0, i_off_poc = 0 ;
    int32_t i_insert_point = 0 ;
    int32_t i_prev = 0, i_prev_used = 0, i_new_prev = 0, i_new_used = 0 ;
    int32_t i_ref_idx = 0, i_ref_poc = 0, i_ref_pics = 0, i_new_idc = 0 ;
    int32_t i_delta_poc = 0, i_abs_poc_ref = 0, i_ref_idc = 0 ;
	int32_t i_highest_decoding_number_with_lower_poc = 0 ;
	int32_t i_num_reorder = 0 ;
	int32_t i_intra_period = 0 ;

	i_intra_period = p->gop.i_intra_period ;
	/* if this is an intra-only sequence, ie IntraPeriod=1, don't verify the GOP structure
     * This permits the ability to omit a GOP structure specification */
    if ( i_intra_period == 1 && p->gop.gop_list[0].i_poc == -1 )
    {
    	x265_init_gop_entry ( &p->gop.gop_list[0] ) ;
		p->gop.gop_list[0].f_qp_factor = 1;
		p->gop.gop_list[0].i_beta_offset_div_2 = 0;
		p->gop.gop_list[0].i_tc_offset_div_2 = 0;
		p->gop.gop_list[0].i_poc = 1;
		p->gop.gop_list[0].i_num_ref_pics_active = 4;
    }

    ai_ref_list[0]=0;
    for ( loop = 0 ; loop < X265_MAX_GOP ; ++ loop )
    {
    	b_is_ok [loop] = 0 ;
    }
    CONFIRM_PARAMETER ( i_intra_period >= 0 && (i_intra_period % p->gop.i_gop_size !=0 ), "Intra period must be a multiple of GOPSize, or -1" ) ;

    for ( loop = 0 ; loop < p->gop.i_gop_size ; ++ loop )
    {
		if( p->gop.gop_list[loop].i_poc == p->gop.i_gop_size )
		{
			CONFIRM_PARAMETER ( p->gop.gop_list[loop].i_temporal_id !=0, "The last frame in each GOP must have temporal ID = 0 " ) ;
		}
    }

    if ( (i_intra_period != 1)
    	&& ! p->deblock.i_loop_filter_offset_in_pps
    	&& p->deblock.i_deblocking_filter_control_present
    	&& (! p->sps.b_loop_filter_disable )
    	)
    {
		for ( loop = 0 ; loop < p->gop.i_gop_size ; ++ loop )
		{
			CONFIRM_PARAMETER ( (p->gop.gop_list[loop].i_beta_offset_div_2 + p->deblock.i_loop_filter_beta_offset_div_2 ) < -6
								|| (p->gop.gop_list[loop].i_beta_offset_div_2 + p->deblock.i_loop_filter_beta_offset_div_2) > 6,
								"Loop Filter Beta Offset div. 2 for one of the GOP entries exceeds supported range (-6 to 6)" ) ;
			CONFIRM_PARAMETER ( (p->gop.gop_list[loop].i_tc_offset_div_2 + p->deblock.i_loop_filter_tc_offset_div_2) < -6
								|| (p->gop.gop_list[loop].i_tc_offset_div_2 + p->deblock.i_loop_filter_tc_offset_div_2) > 6,
								"Loop Filter Tc Offset div. 2 for one of the GOP entries exceeds supported range (-6 to 6)" ) ;
		}
    }
    p->gop.i_extraRPSs = 0 ;
    //start looping through frames in coding order until we can verify that the GOP structure is correct.
    while( ! b_verified_gop && ! b_error_gop )
    {
    	i_curr_gop = (i_check_gop - 1) % p->gop.i_gop_size ;
    	i_curr_poc = ((i_check_gop - 1) / p->gop.i_gop_size) * p->gop.i_gop_size + p->gop.gop_list[i_curr_gop].i_poc ;
		if( p->gop.gop_list[i_curr_gop].i_poc < 0 )
		{
			fprintf ( stderr, "\nError: found fewer Reference Picture Sets than GOPSize\n" ) ;
			b_error_gop = 1 ;
		}
		else
		{
			//check that all reference pictures are available, or have a POC < 0 meaning they might be available in the next GOP.
			b_before_i = 0 ;
			for ( i = 0; i < p->gop.gop_list[i_curr_gop].i_num_ref_pics ; ++ i)
			{
				i_abs_poc = i_curr_poc + p->gop.gop_list[i_curr_gop].reference_pics[i] ;
				if( i_abs_poc < 0 )
				{
					b_before_i = 1 ;
				}
				else
				{
					b_found = 0 ;
					for ( j = 0 ; j < i_num_refs; ++ j )
					{
						if( ai_ref_list[j] == i_abs_poc )
						{
							b_found = 1 ;
							for ( k = 0; k < p->gop.i_gop_size ; ++ k )
							{
								if( i_abs_poc % p->gop.i_gop_size == p->gop.gop_list[k].i_poc % p->gop.i_gop_size )
								{
									if ( p->gop.gop_list[k].i_temporal_id==p->gop.gop_list[i_curr_gop].i_temporal_id )
									{
										p->gop.gop_list[k].b_ref_pic = 1 ;
									}
									p->gop.gop_list[i_curr_gop].used_by_curr_pic[i] = p->gop.gop_list[k].i_temporal_id <= p->gop.gop_list[i_curr_gop].i_temporal_id ;
								}
							}
						}
					}
					if ( ! b_found )
					{
						fprintf(stderr, "\nError: ref pic %d is not available for GOP frame %d\n",
										p->gop.gop_list[i_curr_gop].reference_pics[i], i_curr_gop + 1 ) ;
						b_error_gop = 1 ;
					}
				}
			}
			if( ! b_before_i && ! b_error_gop )
			{
				//all ref frames were present
				if( ! b_is_ok[i_curr_gop] )
				{
					i_num_ok ++ ;
					b_is_ok[i_curr_gop] = 1 ;
					if( i_num_ok == p->gop.i_gop_size)
					{
						b_verified_gop = 1 ;
					}
				}
			}
			else
			{
				//create a new GOPEntry for this frame containing all the reference pictures that were available (POC > 0)
				memcpy ( & p->gop.gop_list[p->gop.i_gop_size + p->gop.i_extraRPSs],
						& p->gop.gop_list[i_curr_gop],
						sizeof(struct gop_entry_t)
						) ;
				i_new_refs = 0 ;
				for ( loop = 0; loop < p->gop.gop_list[i_curr_gop].i_num_ref_pics; ++ loop )
				{
					i_abs_poc = i_curr_poc + p->gop.gop_list[i_curr_gop].reference_pics[loop];
					if ( i_abs_poc >=0 )
					{
						p->gop.gop_list[p->gop.i_gop_size+p->gop.i_extraRPSs].reference_pics[i_new_refs] = p->gop.gop_list[i_curr_gop].reference_pics[loop];
						p->gop.gop_list[p->gop.i_gop_size+p->gop.i_extraRPSs].used_by_curr_pic[i_new_refs] = p->gop.gop_list[i_curr_gop].used_by_curr_pic[loop];
						i_new_refs ++ ;
					}
				}
				i_num_pref_refs = p->gop.gop_list[i_curr_gop].i_num_ref_pics_active ;

				for ( i_offset = -1 ; i_offset > - i_check_gop ; -- i_offset )
				{
					//step backwards in coding order and include any extra available pictures we might find useful to replace the ones with POC < 0.
					i_off_gop = (i_check_gop - 1 + i_offset) % p->gop.i_gop_size;
					i_off_poc = ((i_check_gop - 1 + i_offset) / p->gop.i_gop_size) * p->gop.i_gop_size + p->gop.gop_list[i_off_gop].i_poc ;
					if ( i_off_poc >= 0 && p->gop.gop_list[i_off_gop].i_temporal_id <= p->gop.gop_list[i_curr_gop].i_temporal_id )
					{
						b_new_ref = 0 ;
						for ( loop = 0 ; loop < i_num_refs ; ++ loop )
						{
							if ( ai_ref_list[loop] == i_off_poc )
							{
								b_new_ref = 1 ;
							}
						}
						for ( loop = 0 ; loop < i_new_refs ; ++ loop )
						{
							if ( p->gop.gop_list[p->gop.i_gop_size+p->gop.i_extraRPSs].reference_pics[loop] == i_off_poc - i_curr_poc )
							{
								b_new_ref = 0 ;
							}
						}
						if ( b_new_ref )
						{
			                i_insert_point = i_new_refs ;
			                //this picture can be added, find appropriate place in list and insert it.
			                if ( p->gop.gop_list[i_off_gop].i_temporal_id == p->gop.gop_list[i_curr_gop].i_temporal_id )
			                {
			                	p->gop.gop_list[i_off_gop].b_ref_pic = 1;
			                }
			                for ( loop = 0 ; loop < i_new_refs ; ++ loop )
			                {
			                	if ( p->gop.gop_list[p->gop.i_gop_size + p->gop.i_extraRPSs].reference_pics[loop] < i_off_poc-i_curr_poc
			                		|| p->gop.gop_list[p->gop.i_gop_size + p->gop.i_extraRPSs].reference_pics[loop]>0)
			                	{
			                		i_insert_point = loop ;
			                		break;
			                	}
			                }
			                i_prev = i_off_poc - i_curr_poc ;
			                i_prev_used = p->gop.gop_list[i_off_gop].i_temporal_id<=p->gop.gop_list[i_curr_gop].i_temporal_id;
			                for ( loop = i_insert_point ; loop < i_new_refs + 1 ; ++ loop )
			                {
			                	i_new_prev = p->gop.gop_list[p->gop.i_gop_size+p->gop.i_extraRPSs].reference_pics[loop];
			                	i_new_used = p->gop.gop_list[p->gop.i_gop_size+p->gop.i_extraRPSs].used_by_curr_pic[loop];
			                	p->gop.gop_list[p->gop.i_gop_size+p->gop.i_extraRPSs].reference_pics[loop] = i_prev ;
			                	p->gop.gop_list[p->gop.i_gop_size+p->gop.i_extraRPSs].used_by_curr_pic[loop] = i_prev_used;
			                	i_prev_used = i_new_used ;
			                	i_prev = i_new_prev ;
			                }
			                i_new_refs++;
						}
					}
					if ( i_new_refs >= i_num_pref_refs )
					{
						break;
					}
				}
				p->gop.gop_list[p->gop.i_gop_size+p->gop.i_extraRPSs].i_num_ref_pics = i_new_refs ;
				p->gop.gop_list[p->gop.i_gop_size+p->gop.i_extraRPSs].i_poc = i_curr_poc ;
				if ( p->gop.i_extraRPSs == 0 )
				{
					p->gop.gop_list[p->gop.i_gop_size+p->gop.i_extraRPSs].i_inter_rps_prediction = 0;
					p->gop.gop_list[p->gop.i_gop_size+p->gop.i_extraRPSs].i_num_ref_idc = 0;
				}
				else
				{
					i_ref_idx =  p->gop.i_gop_size + p->gop.i_extraRPSs - 1;
					i_ref_poc = p->gop.gop_list[i_ref_idx].i_poc;
					i_ref_pics = p->gop.gop_list[i_ref_idx].i_num_ref_pics ;
					i_new_idc = 0 ;
					for ( i = 0 ; i <= i_ref_pics ; ++ i )
					{
						i_delta_poc = ((i != i_ref_pics) ? p->gop.gop_list[i_ref_idx].reference_pics[i] : 0 ) ;  // check if the reference abs POC is >= 0
						i_abs_poc_ref = i_ref_poc + i_delta_poc ;
						i_ref_idc = 0 ;
						for ( j = 0 ;
								j < p->gop.gop_list[p->gop.i_gop_size+p->gop.i_extraRPSs].i_num_ref_pics ;
								++ j )
						{
							if ( (i_abs_poc_ref - i_curr_poc) == p->gop.gop_list[p->gop.i_gop_size+p->gop.i_extraRPSs].reference_pics[j])
							{
								if (p->gop.gop_list[p->gop.i_gop_size+p->gop.i_extraRPSs].used_by_curr_pic[j])
								{
									i_ref_idc = 1 ;
								}
								else
								{
									i_ref_idc = 2 ;
								}
							}
						}
						p->gop.gop_list[p->gop.i_gop_size+p->gop.i_extraRPSs].i_ref_idc[i_new_idc] = i_ref_idc ;
						i_new_idc++ ;
					}
					p->gop.gop_list[p->gop.i_gop_size+p->gop.i_extraRPSs].i_inter_rps_prediction = 1 ;
					p->gop.gop_list[p->gop.i_gop_size+p->gop.i_extraRPSs].i_num_ref_idc = i_new_idc ;
					p->gop.gop_list[p->gop.i_gop_size+p->gop.i_extraRPSs].i_delta_rps = i_ref_poc - p->gop.gop_list[p->gop.i_gop_size+p->gop.i_extraRPSs].i_poc ;
				}
				i_curr_gop=p->gop.i_gop_size+p->gop.i_extraRPSs ;
				p->gop.i_extraRPSs++ ;
			}
			i_num_refs=0 ;
			for ( loop = 0; loop < p->gop.gop_list[i_curr_gop].i_num_ref_pics; ++ loop)
			{
				i_abs_poc = i_curr_poc+p->gop.gop_list[i_curr_gop].reference_pics[loop] ;
				if ( i_abs_poc >= 0 )
				{
					ai_ref_list[i_num_refs] = i_abs_poc ;
					i_num_refs++ ;
				}
			}
			ai_ref_list[i_num_refs] = i_curr_poc ;
			i_num_refs++;
      }
      i_check_gop++;
    }
    CONFIRM_PARAMETER ( b_error_gop, "Invalid GOP structure given") ;
    p->sps.i_max_temp_layer = 1;
    for ( loop = 0 ; loop < p->gop.i_gop_size ; ++ loop )
    {
    	if ( p->gop.gop_list[loop].i_temporal_id >= p->sps.i_max_temp_layer )
    	{
    		p->sps.i_max_temp_layer = p->gop.gop_list[loop].i_temporal_id+1;
    	}
    	CONFIRM_PARAMETER ( p->gop.gop_list[loop].i_slice_type != 'B' && p->gop.gop_list[loop].i_slice_type != 'P', "Slice type must be equal to B or P") ;
    }
    for ( loop = 0 ; loop < X265_MAX_TLAYER; ++loop )
    {
    	p->gop.num_reorder_pics[loop] = 0;
    	p->gop.max_dec_pic_buffering[loop] = 0;
    }
    for ( i=0; i < p->gop.i_gop_size; i++)
    {
		if(p->gop.gop_list[i].i_num_ref_pics > p->gop.max_dec_pic_buffering[p->gop.gop_list[i].i_temporal_id])
		{
			p->gop.max_dec_pic_buffering[p->gop.gop_list[i].i_temporal_id] = p->gop.gop_list[i].i_num_ref_pics;
		}
		i_highest_decoding_number_with_lower_poc = 0;
		for ( j=0; j<p->gop.i_gop_size; j++)
		{
			if(p->gop.gop_list[j].i_poc <= p->gop.gop_list[i].i_poc)
			{
				i_highest_decoding_number_with_lower_poc = j;
			}
		}
		i_num_reorder = 0;
		for ( j=0; j<i_highest_decoding_number_with_lower_poc; j++)
		{
			if( p->gop.gop_list[j].i_temporal_id <= p->gop.gop_list[i].i_temporal_id
					&& p->gop.gop_list[j].i_poc > p->gop.gop_list[i].i_poc)
			{
				i_num_reorder++;
			}
		}
		if(i_num_reorder > p->gop.num_reorder_pics[p->gop.gop_list[i].i_temporal_id])
		{
			p->gop.num_reorder_pics[p->gop.gop_list[i].i_temporal_id] = i_num_reorder;
		}
    }
    for ( loop = 0 ; loop < X265_MAX_TLAYER - 1 ; ++ loop )
    {
		// a lower layer can not have higher value of p->gop.num_reorder_pics than a higher layer
		if(p->gop.num_reorder_pics[loop + 1] < p->gop.num_reorder_pics[loop])
		{
			p->gop.num_reorder_pics[loop + 1] = p->gop.num_reorder_pics[loop];
		}
		// the value of num_reorder_pics[ i ] shall be in the range of 0 to max_dec_pic_buffering[ i ], inclusive
		if(p->gop.num_reorder_pics[loop] > p->gop.max_dec_pic_buffering[loop])
		{
			p->gop.max_dec_pic_buffering[loop] = p->gop.num_reorder_pics[loop];
		}
		// a lower layer can not have higher value of m_uiMaxDecPicBuffering than a higher layer
		if(p->gop.max_dec_pic_buffering[loop+1] < p->gop.max_dec_pic_buffering[loop])
		{
			p->gop.max_dec_pic_buffering[loop+1] = p->gop.max_dec_pic_buffering[loop];
		}
    }
    // the value of num_reorder_pics[ i ] shall be in the range of 0 to max_dec_pic_buffering[ i ], inclusive
    if(p->gop.num_reorder_pics[X265_MAX_TLAYER - 1] > p->gop.max_dec_pic_buffering[X265_MAX_TLAYER - 1])
    {
    	p->gop.max_dec_pic_buffering[X265_MAX_TLAYER - 1] = p->gop.num_reorder_pics[X265_MAX_TLAYER - 1];
    }

    return b_error ? X265_PARAM_BAD_VALUE : 0;
}

int x265_check_gs_parameters ( x265_param_t *p )
{
	int32_t loop = 0 ;
	int32_t i_width_in_cu = 0 ;
	int32_t i_height_in_cu = 0 ;
	uint32_t  i_cummulative_column_width = 0;
	uint32_t  i_cummulative_row_height = 0;

    i_width_in_cu = (p->i_width % p->sps.i_max_cu_width)
								? p->i_width / p->sps.i_max_cu_width + 1
								: p->i_width / p->sps.i_max_cu_width ;
    i_height_in_cu = (p->i_height % p->sps.i_max_cu_height)
								? p->i_height / p->sps.i_max_cu_height + 1
								: p->i_height / p->sps.i_max_cu_height ;

    //check the column relative parameters
    if (p->pps.i_num_columns_minus_1 >= (1<< ( X265_LOG2_MAX_NUM_COLUMNS_MINUS1 + 1)))
    {
    	fprintf( stderr, "The number of columns is larger than the maximum allowed number of columns.\n" ) ;
    	return -1 ;
    }

    if ( p->pps.i_num_columns_minus_1 >= i_width_in_cu )
    {
    	fprintf( stderr, "The current picture can not have so many columns.\n" ) ;
    	return -1 ;
    }

    if ( p->pps.i_num_columns_minus_1 && 0 == p->pps.i_uniform_spacing_idr )
    {
    	for( loop = 0 ; loop < p->pps.i_num_columns_minus_1 ; ++ loop )
    	{
    		i_cummulative_column_width += p->pps.column_width[loop] ;
    	}

    	if( i_cummulative_column_width >= i_width_in_cu )
    	{
    		fprintf( stderr, "The width of the column is too large.\n" ) ;
    		return -1 ;
    	}
    }

    //check the row relative parameters
    if (p->pps.i_num_rows_minus_1 >= (1<<(X265_LOG2_MAX_NUM_ROWS_MINUS1 + 1)))
    {
    	fprintf ( stderr, "The number of rows is larger than the maximum allowed number of rows.\n" ) ;
    	return -1 ;
    }

    if( p->pps.i_num_rows_minus_1 >= i_height_in_cu )
    {
    	fprintf ( stderr, "The current picture can not have so many rows.\n" ) ;
    	return -1 ;
    }

    if( p->pps.i_num_rows_minus_1 && 0 == p->pps.i_uniform_spacing_idr )
    {
    	for ( loop = 0 ; loop < p->pps.i_num_rows_minus_1 ; ++ loop )
    	{
    		i_cummulative_row_height += p->pps.row_height[loop] ;
    	}

    	if( i_cummulative_row_height >= i_height_in_cu )
    	{
    		fprintf ( stderr, "The height of the row is too large.\n" ) ;
    		return -1 ;
    	}
    }

    return 0 ;
}

int x265_check_parameters ( x265_param_t *p )
{
    int b_error = 0;
    int b_tile_flag = 0 ;
    // max CU width and height should be power of 2
    uint32_t ui = 0 ;
    int32_t i_pic_size_in_samples_y =  p->i_width * p->i_height;
    int32_t i_max_tile_width = 0 ;
    int32_t i_max_tile_height = 0 ;
    int32_t i_max_size_in_samples_y = 0 ;
    int32_t i_width_in_cu = 0 ;
    int32_t i_height_in_cu = 0 ;
    int32_t i_acc_column_width = 0 ;
    int32_t col = 0 ;
    int32_t i_acc_row_height = 0 ;
    int32_t row = 0 ;
    int32_t i_num_part_in_cu = 0 ;
    int32_t i_intra_period = 0 ;

#if ! X265_RATE_CONTROL_LAMBDA_DOMAIN
    int32_t i_num_lcu_in_width = 0 ;
    int32_t i_num_lcu_in_height = 0 ;
    int32_t i_num_lcu_in_pic = 0 ;
#endif

    i_intra_period = p->gop.i_intra_period ;

    CONFIRM_PARAMETER ( p->i_input_bit_depth_y < 8,                                                                             "InputBitDepth must be at least 8" ) ;
    CONFIRM_PARAMETER ( p->i_frame_rate <= 0,                                                                                   "Frame rate must be more than 1" ) ;
    CONFIRM_PARAMETER ( p->i_frame_total <= 0,                                                                                  "Total Number Of Frames encoded must be more than 0" ) ;
    CONFIRM_PARAMETER ( p->gop.i_gop_size < 1 ,                                                                                 "GOP Size must be greater or equal to 1" ) ;
    CONFIRM_PARAMETER ( p->gop.i_gop_size > 1 &&  p->gop.i_gop_size % 2,                                          "              GOP Size must be a multiple of 2, if GOP Size is greater than 1" ) ;
    CONFIRM_PARAMETER ( (i_intra_period > 0 && i_intra_period < p->gop.i_gop_size) || i_intra_period == 0, "Intra period must be more than GOP size, or -1 , not 0" ) ;
    CONFIRM_PARAMETER ( p->gop.i_decoding_refresh_type < 0 || p->gop.i_decoding_refresh_type > 2,                               "Decoding Refresh Type must be equal to 0, 1 or 2" ) ;
    CONFIRM_PARAMETER ( p->i_qp <  -6 * (p->sps.i_bit_depth_y - 8) || p->i_qp > 51,                                             "QP exceeds supported range (-QpBDOffsety to 51)" ) ;
    CONFIRM_PARAMETER ( p->deblock.i_loop_filter_beta_offset_div_2 < -13 || p->deblock.i_loop_filter_beta_offset_div_2 > 13,    "Loop Filter Beta Offset div. 2 exceeds supported range (-13 to 13)") ;
    CONFIRM_PARAMETER ( p->deblock.i_loop_filter_tc_offset_div_2 < -13 || p->deblock.i_loop_filter_tc_offset_div_2 > 13,        "Loop Filter Tc Offset div. 2 exceeds supported range (-13 to 13)") ;
    CONFIRM_PARAMETER ( p->i_fast_search < 0 || p->i_fast_search > 2,                                                           "Fast Search Mode is not supported value (0:Full search  1:Diamond  2:PMVFAST)" ) ;
    CONFIRM_PARAMETER ( p->i_search_range < 0 ,                                                                                 "Search Range must be more than 0" ) ;
    CONFIRM_PARAMETER ( p->i_bipred_search_range < 0 ,                                                                          "Search Range must be more than 0" ) ;
    CONFIRM_PARAMETER ( p->rdo.i_max_delta_qp > 7,                                                                              "Absolute Delta QP exceeds supported range (0 to 7)" ) ;
    CONFIRM_PARAMETER ( p->rdo.i_max_cu_delta_qp_depth > p->sps.i_max_cu_depth - 1,                                             "Absolute depth for a minimum CuDQP exceeds maximum coding unit depth" ) ;

    CONFIRM_PARAMETER ( p->i_cb_qp_offset < -12,                                                                                "Min. Chroma Cb QP Offset is -12" ) ;
    CONFIRM_PARAMETER ( p->i_cb_qp_offset >  12,                                                                                "Max. Chroma Cb QP Offset is  12" ) ;
    CONFIRM_PARAMETER ( p->i_cr_qp_offset < -12,                                                                                "Min. Chroma Cr QP Offset is -12" ) ;
    CONFIRM_PARAMETER ( p->i_cr_qp_offset >  12,                                                                                "Max. Chroma Cr QP Offset is  12" ) ;

    CONFIRM_PARAMETER ( p->i_qp_adaptation_range <= 0,                                                                          "QP Adaptation Range must be more than 0" ) ;
    if (p->gop.i_decoding_refresh_type == 2)
    {
      CONFIRM_PARAMETER ( i_intra_period > 0 && i_intra_period <= p->gop.i_gop_size ,                             "Intra period must be larger than GOP size for periodic IDR pictures") ;
    }
    CONFIRM_PARAMETER ( (p->sps.i_max_cu_width  >> p->sps.i_max_cu_depth) < 4,                                                  "Minimum partition width size should be larger than or equal to 8") ;
    CONFIRM_PARAMETER ( (p->sps.i_max_cu_height >> p->sps.i_max_cu_depth) < 4,                                                  "Minimum partition height size should be larger than or equal to 8") ;
    CONFIRM_PARAMETER ( p->sps.i_max_cu_width < 16,                                                                             "Maximum partition width size should be larger than or equal to 16") ;
    CONFIRM_PARAMETER ( p->sps.i_max_cu_height < 16,                                                                            "Maximum partition height size should be larger than or equal to 16") ;
    CONFIRM_PARAMETER ( (p->i_width  % (p->sps.i_max_cu_width  >> (p->sps.i_max_cu_depth - 1))) != 0,                           "Resulting coded frame width must be a multiple of the minimum CU size") ;
    CONFIRM_PARAMETER ( (p->i_height % (p->sps.i_max_cu_height >> (p->sps.i_max_cu_depth - 1))) != 0,                           "Resulting coded frame height must be a multiple of the minimum CU size") ;

    CONFIRM_PARAMETER ( p->sps.i_quadtree_tu_log2_min_size < 2,                                                                 "QuadtreeTULog2MinSize must be 2 or greater.") ;
    CONFIRM_PARAMETER ( p->sps.i_quadtree_tu_log2_max_size > 5,                                                                 "QuadtreeTULog2MaxSize must be 5 or smaller.") ;
    CONFIRM_PARAMETER ( (1<<p->sps.i_quadtree_tu_log2_max_size) > p->sps.i_max_cu_width,                                        "QuadtreeTULog2MaxSize must be log2(maxCUSize) or smaller.") ;

    CONFIRM_PARAMETER ( p->sps.i_quadtree_tu_log2_max_size < p->sps.i_quadtree_tu_log2_min_size,                                "QuadtreeTULog2MaxSize must be greater than or equal to p->sps.i_quadtree_tu_log2_min_size.") ;
    CONFIRM_PARAMETER ( (1 << p->sps.i_quadtree_tu_log2_min_size) > (p->sps.i_max_cu_width >> (p->sps.i_max_cu_depth -1 )),     "QuadtreeTULog2MinSize must not be greater than minimum CU size" ) ; // HS
    CONFIRM_PARAMETER ( (1 << p->sps.i_quadtree_tu_log2_min_size) > (p->sps.i_max_cu_height >> (p->sps.i_max_cu_depth -1 )),    "QuadtreeTULog2MinSize must not be greater than minimum CU size" ) ; // HS
    CONFIRM_PARAMETER ( (1 << p->sps.i_quadtree_tu_log2_min_size) > (p->sps.i_max_cu_width  >> p->sps.i_max_cu_depth ),         "Minimum CU width must be greater than minimum transform size." ) ;
    CONFIRM_PARAMETER ( (1 << p->sps.i_quadtree_tu_log2_min_size) > (p->sps.i_max_cu_height >> p->sps.i_max_cu_depth ),         "Minimum CU height must be greater than minimum transform size." ) ;
    CONFIRM_PARAMETER ( p->sps.i_quadtree_tu_max_depth_inter < 1,                                                                         "QuadtreeTUMaxDepthInter must be greater than or equal to 1" ) ;
    CONFIRM_PARAMETER ( p->sps.i_max_cu_width < ( 1 << (p->sps.i_quadtree_tu_log2_min_size + p->sps.i_quadtree_tu_max_depth_inter - 1) ), "QuadtreeTUMaxDepthInter must be less than or equal to the difference between log2(maxCUSize) and QuadtreeTULog2MinSize plus 1" ) ;
    CONFIRM_PARAMETER ( p->sps.i_quadtree_tu_max_depth_intra < 1,                                                                         "QuadtreeTUMaxDepthIntra must be greater than or equal to 1" ) ;
    CONFIRM_PARAMETER ( p->sps.i_max_cu_width < ( 1 << (p->sps.i_quadtree_tu_log2_min_size + p->sps.i_quadtree_tu_max_depth_intra - 1) ), "QuadtreeTUMaxDepthIntra must be less than or equal to the difference between log2(maxCUSize) and QuadtreeTULog2MinSize plus 1" ) ;

    CONFIRM_PARAMETER ( p->i_max_num_merge_cand < 1,  "MaxNumMergeCand must be 1 or greater.") ;
    CONFIRM_PARAMETER (  p->i_max_num_merge_cand > 5,  "MaxNumMergeCand must be 5 or smaller.") ;

#if X265_ADAPTIVE_QP_SELECTION
    CONFIRM_PARAMETER ( p->b_use_adapt_qp_select != 0 && p->i_qp < 0,                                                                     "AdaptiveQpSelection must be disabled when QP < 0.") ;
    CONFIRM_PARAMETER ( p->b_use_adapt_qp_select != 0 && (p->i_cb_qp_offset !=0 || p->i_cr_qp_offset != 0 ),                              "AdaptiveQpSelection must be disabled when ChromaQpOffset is not equal to 0.") ;
#endif

    if( p->sps.b_use_pcm )
    {
		CONFIRM_PARAMETER ( p->pcm.i_pcm_log2_min_size < 3,                                                                               "PCMLog2MinSize must be 3 or greater.") ;
		CONFIRM_PARAMETER ( p->pcm.i_pcm_log2_min_size > 5,                                                                               "PCMLog2MinSize must be 5 or smaller.") ;
		CONFIRM_PARAMETER ( p->pcm.i_pcm_log2_max_size > 5,                                                                               "PCMLog2MaxSize must be 5 or smaller.") ;
		CONFIRM_PARAMETER ( p->pcm.i_pcm_log2_max_size < p->pcm.i_pcm_log2_min_size,                                                      "PCMLog2MaxSize must be equal to or greater than PCMLog2MinSize.") ;
    }

    CONFIRM_PARAMETER ( p->i_slice_mode < 0 || p->i_slice_mode > 3,                                                                       "SliceMode exceeds supported range (0 to 3)" ) ;
    if ( p->i_slice_mode !=0 )
    {
    	CONFIRM_PARAMETER ( p->i_slice_argument < 1 ,                                                                                       "SliceArgument should be larger than or equal to 1" ) ;
    }
    CONFIRM_PARAMETER ( p->i_slice_segment_mode < 0 || p->i_slice_segment_mode > 3,                                                       "SliceSegmentMode exceeds supported range (0 to 3)" ) ;
    if ( p->i_slice_segment_mode !=0 )
    {
    	CONFIRM_PARAMETER ( p->i_slice_segment_argument < 1 ,                                                                               "SliceSegmentArgument should be larger than or equal to 1" ) ;
    }

    b_tile_flag = (p->pps.i_num_columns_minus_1 > 0 || p->pps.i_num_rows_minus_1 > 0 ) ;
    CONFIRM_PARAMETER ( b_tile_flag && p->pps.i_wave_front_synchro,                                                                       "Tile and Wavefront can not be applied together") ;

    //TODO:ChromaFmt assumes 4:2:0 below
    CONFIRM_PARAMETER ( p->i_width  % 2 != 0,                                                                                             "Picture width must be an integer multiple of the specified chroma subsampling") ;
    CONFIRM_PARAMETER ( p->i_height % 2 != 0,                                                                                             "Picture height must be an integer multiple of the specified chroma subsampling") ;

    CONFIRM_PARAMETER ( p->i_pad[0] % 2 != 0,                                                                                             "Horizontal padding must be an integer multiple of the specified chroma subsampling") ;
    CONFIRM_PARAMETER ( p->i_pad[1] % 2 != 0,                                                                                             "Vertical padding must be an integer multiple of the specified chroma subsampling") ;

    CONFIRM_PARAMETER ( p->sps.i_conf_left   % 2 != 0,                                                                                    "Left conformance window offset must be an integer multiple of the specified chroma subsampling") ;
    CONFIRM_PARAMETER ( p->sps.i_conf_right  % 2 != 0,                                                                                    "Right conformance window offset must be an integer multiple of the specified chroma subsampling") ;
    CONFIRM_PARAMETER ( p->sps.i_conf_top    % 2 != 0,                                                                                    "Top conformance window offset must be an integer multiple of the specified chroma subsampling") ;
    CONFIRM_PARAMETER ( p->sps.i_conf_bottom % 2 != 0,                                                                                    "Bottom conformance window offset must be an integer multiple of the specified chroma subsampling") ;

    ui = p->sps.i_max_cu_width;
    while(ui)
    {
    	ui >>= 1;
		if( (ui & 1) == 1)
		{
			CONFIRM_PARAMETER ( ui != 1 ,                                                                                                 "Width should be 2^n") ;
		}
    }
    ui = p->sps.i_max_cu_height;
    while(ui)
    {
		ui >>= 1;
		if( (ui & 1) == 1)
		{
			CONFIRM_PARAMETER ( ui != 1 ,                                                                                                 "Height should be 2^n") ;
		}
    }

    if ( x265_check_gop_list ( p ) )
    {
    	return X265_PARAM_BAD_VALUE ;
    }


    if( p->sps.b_vui_parameters_present_flag && p->sps.vui.b_bitstream_restriction_flag )
    {
    	i_pic_size_in_samples_y =  p->i_width * p->i_height ;
    	if ( b_tile_flag )
    	{
			i_width_in_cu = (p->i_width % p->sps.i_max_cu_width) ? p->i_width / p->sps.i_max_cu_width + 1 : p->i_width / p->sps.i_max_cu_width ;
			i_height_in_cu = (p->i_height % p->sps.i_max_cu_height) ? p->i_height / p->sps.i_max_cu_height + 1: p->i_height / p->sps.i_max_cu_height ;
			if ( p->pps.i_uniform_spacing_idr )
			{
				i_max_tile_width = p->sps.i_max_cu_width * ((i_width_in_cu + p->pps.i_num_columns_minus_1) / (p->pps.i_num_columns_minus_1 + 1)) ;
				i_max_tile_height = p->sps.i_max_cu_height * ((i_height_in_cu + p->pps.i_num_rows_minus_1) / (p->pps.i_num_rows_minus_1 + 1)) ;
				// if only the last tile-row is one treeblock higher than the others
				// the i_max_tile_height becomes smaller if the last row of treeblocks has lower height than the others
				if( ! ((i_height_in_cu - 1) % (p->pps.i_num_rows_minus_1 + 1)) )
				{
					i_max_tile_height = i_max_tile_height - p->sps.i_max_cu_height + (p->i_height % p->sps.i_max_cu_height) ;
				}
				// if only the last tile-column is one treeblock wider than the others
				// the i_max_tile_width becomes smaller if the last column of treeblocks has lower width than the others
				if( ! ((i_width_in_cu - 1) % (p->pps.i_num_columns_minus_1 + 1)) )
				{
					i_max_tile_width = i_max_tile_width - p->sps.i_max_cu_width + (p->i_width % p->sps.i_max_cu_width) ;
				}
			}
			else // not uniform spacing
			{
				if(p->pps.i_num_columns_minus_1<1)
				{
					i_max_tile_width = p->i_width ;
				}
				else
				{
					for ( col = 0 ; col<(p->pps.i_num_columns_minus_1) ; col++)
					{
						i_max_tile_width = p->pps.column_width[col] > i_max_tile_width ? p->pps.column_width[col] : i_max_tile_width ;
						i_acc_column_width += p->pps.column_width[col] ;
					}
					i_max_tile_width = (i_width_in_cu - i_acc_column_width) > i_max_tile_width ? p->sps.i_max_cu_width * (i_width_in_cu - i_acc_column_width) : p->sps.i_max_cu_width * i_max_tile_width ;
				}
				if(p->pps.i_num_rows_minus_1 < 1)
				{
					i_max_tile_height = p->i_height ;
				}
				else
				{
					for ( row = 0 ; row < (p->pps.i_num_rows_minus_1) ; ++ row )
					{
						i_max_tile_height = p->pps.row_height[row] > i_max_tile_height ? p->pps.row_height[row] : i_max_tile_height ;
						i_acc_row_height += p->pps.row_height[row] ;
					}
					i_max_tile_height = (i_height_in_cu - i_acc_row_height) > i_max_tile_height ? p->sps.i_max_cu_height * (i_height_in_cu - i_acc_row_height) : p->sps.i_max_cu_height * i_max_tile_height ;
				}
			}
			i_max_size_in_samples_y = i_max_tile_width * i_max_tile_height ;
			p->sps.vui.i_min_spatial_segmentation_idc = 4 * i_pic_size_in_samples_y / i_max_size_in_samples_y - 4 ;
    	}
    	else if(p->pps.i_wave_front_synchro)
    	{
    		p->sps.vui.i_min_spatial_segmentation_idc = 4 * i_pic_size_in_samples_y / ((2*p->i_height + p->i_width) * p->sps.i_max_cu_height) - 4 ;
    	}
    	else if(p->i_slice_mode == 1)
    	{
    		p->sps.vui.i_min_spatial_segmentation_idc = 4 * i_pic_size_in_samples_y / (p->i_slice_argument * p->sps.i_max_cu_width * p->sps.i_max_cu_height) - 4 ;
    	}
    	else
    	{
    		p->sps.vui.i_min_spatial_segmentation_idc = 0 ;
    	}
    }
    CONFIRM_PARAMETER ( p->b_use_list_combination == 0 && p->gop.num_reorder_pics[X265_MAX_TLAYER-1] != 0,                                  "ListCombination can only be 0 in low delay coding (more precisely when L0 and L1 are identical)" ) ;  // Note however this is not the full necessary condition as ref_pic_list_combination_flag can only be 0 if L0 == L1.
    CONFIRM_PARAMETER ( p->pps.i_wave_front_synchro < 0,                                                                                  "WaveFrontSynchro cannot be negative" ) ;
    CONFIRM_PARAMETER ( p->pps.i_wave_front_substreams <= 0,                                                                              "WaveFrontSubstreams must be positive" ) ;
    CONFIRM_PARAMETER ( p->pps.i_wave_front_substreams > 1 && ! p->pps.i_wave_front_synchro,                                              "Must have WaveFrontSynchro > 0 in order to have WaveFrontSubstreams > 1" ) ;

    CONFIRM_PARAMETER ( p->sei.i_decoded_picture_hash_sei_enabled < 0 || p->sei.i_decoded_picture_hash_sei_enabled > 3,                   "this hash type is not correct!\n") ;

#if X265_RATE_CONTROL_LAMBDA_DOMAIN
    if ( p->rc.b_enable_rate_control )
    {
		if ( p->rc.b_force_intra_qp )
		{
			if ( p->rc.i_initial_qp == 0 )
			{
				fprintf( stderr, "\nInitial QP for rate control is not specified. Reset not to use force intra QP!" ) ;
				p->rc.b_force_intra_qp = 0;
			}
		}
		CONFIRM_PARAMETER ( p->rdo.i_delta_qp_rd > 0,                                                                                     "Rate control cannot be used together with slice level multiple-QP optimization!\n" ) ;
    }
#else
    if( p->b_enable_rate_ctrl )
    {
    	i_num_lcu_in_width = (p->i_width  / p->sps.i_max_cu_width) + (( p->i_width  %  p->sps.i_max_cu_width ) ? 1 : 0) ;
    	i_num_lcu_in_height = (p->i_height / p->sps.i_max_cu_height)+ (( p->i_height %  p->sps.i_max_cu_height) ? 1 : 0) ;
        i_num_lcu_in_pic = i_num_lcu_in_width * i_num_lcu_in_height;

        CONFIRM_PARAMETER ( (i_num_lcu_in_pic % p->i_num_lcu_in_unit) != 0,                                                               "total number of LCUs in a frame should be completely divided by NumLCUInUnit" ) ;

		p->rdo.i_max_delta_qp = X265_MAX_DELTA_QP;
		p->rdo.i_max_cu_delta_qp_depth = X265_MAX_CUDQP_DEPTH;
    }
#endif

    CONFIRM_PARAMETER ( ! p->pps.b_transquant_bypass_enable_flag && p->b_cu_transquant_bypass_flag_value,                                 "CUTransquantBypassFlagValue cannot be 1 when TransquantBypassEnableFlag is 0") ;

    CONFIRM_PARAMETER ( p->pps.i_log2_parallel_merge_level < 2,                                                                           "Log2ParallelMergeLevel should be larger than or equal to 2") ;
#if X265_L0444_FPA_TYPE
    if ( p->sei.i_framePacking_sei_enabled )
    {
    	CONFIRM_PARAMETER ( p->sei.i_framePacking_sei_type < 3 || p->sei.i_framePacking_sei_type > 5 ,                                    "SEIFramePackingType must be in rage 3 to 5") ;
    }
#endif

    if ( x265_check_gs_parameters ( p ) )
    {
    	return X265_PARAM_BAD_VALUE ;
    }

    i_num_part_in_cu = 1 << (p->sps.i_max_cu_depth << 1) ;
    if ( FIXED_NUMBER_OF_LCU == p->i_slice_segment_mode )
    {
    	p->i_slice_segment_argument = p->i_slice_segment_argument * i_num_part_in_cu ;
    }
    if ( FIXED_NUMBER_OF_LCU == p->i_slice_mode )
    {
    	p->i_slice_argument = p->i_slice_argument * i_num_part_in_cu ;
    }

    return b_error ? X265_PARAM_BAD_VALUE : 0;
}

#undef CONFIRM_PARAMETER

