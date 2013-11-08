

#include "common/common.h"


const x265_mv_t mv_refine_h[9] =
{
	{  0,  0 }, // 0
	{  0, -1 }, // 1
	{  0,  1 }, // 2
	{ -1,  0 }, // 3
	{  1,  0 }, // 4
	{ -1, -1 }, // 5
	{  1, -1 }, // 6
	{ -1,  1 }, // 7
	{  1,  1 },  // 8
};

const x265_mv_t mv_refine_q[9] =
{
	{  0,  0 }, // 0
	{  0, -1 }, // 1
	{  0,  1 }, // 2
	{ -1, -1 }, // 5
	{  1, -1 }, // 6
	{ -1,  0 }, // 3
	{  1,  0 }, // 4
	{ -1,  1 }, // 7
	{  1,  1 },  // 8
};


void x265_enc_search_mv_refine_print ()
{
	int32_t loop = 0 ;

	fprintf ( stderr, "MV Refine H !\n" ) ;
	for ( loop = 0 ; loop < 9 ; ++ loop )
	{
		x265_mv_print (&mv_refine_h[loop]) ;
	}

	fprintf ( stderr, "MV Refine Q !\n" ) ;
	for ( loop = 0 ; loop < 9 ; ++ loop )
	{
		x265_mv_print (&mv_refine_q[loop]) ;
	}

}


void x265_enc_search_set_adaptive_search_range(x265_enc_search_t *enc_search,
												int32_t i_dir,
												int32_t i_ref_idx,
												int32_t i_search_range)
{
	enc_search->adapt_sr[i_dir][i_ref_idx] = i_search_range;
}

void x265_enc_search_set_dist_param_comp(x265_enc_search_t *enc_search, uint32_t i_comp)
{
	enc_search->dist_param.i_comp = i_comp ;
}


void x265_enc_search_x_tz_search_help(x265_t *h,
										x265_enc_search_t *enc_search,
										x265_pattern_t *pattern_key,
										x265_int_tz_search_struct_t *int_tz_search_struct,
										const int32_t i_search_x,
										const int32_t i_search_y,
										const uint8_t i_point_nr,
										const uint32_t i_distance )
{
	uint32_t  i_sad;
	pixel*  p_ref_srch;

	p_ref_srch = int_tz_search_struct->ref_y + i_search_y * int_tz_search_struct->i_y_stride + i_search_x;

	//-- jclee for using the sad function pointer
	x265_rd_cost_set_dist_param_p6_2(h,
									&h->rd_cost,
									pattern_key,
									p_ref_srch,
									int_tz_search_struct->i_y_stride,
									&enc_search->dist_param );

	// fast encoder decision: use subsampled sad when rows > 8 for integer me
	if ( h->param.b_use_fast_enc )
	{
		if ( enc_search->dist_param.i_rows > 8 )
		{
			enc_search->dist_param.i_sub_shift = 1;
		}
	}

	enc_search->dist_param.i_comp = 0 ;  // y component

	// distortion
	enc_search->dist_param.i_bit_depth = h->param.sps.i_bit_depth_y;
	i_sad = enc_search->dist_param.dist_func( &h->rd_cost, &enc_search->dist_param );

	// motion cost
	i_sad += x265_rd_cost_get_cost_p3( &h->rd_cost, i_search_x, i_search_y );

	if( i_sad < int_tz_search_struct->i_best_sad )
	{
		int_tz_search_struct->i_best_sad = i_sad;
		int_tz_search_struct->i_best_x = i_search_x;
		int_tz_search_struct->i_best_y = i_search_y;
		int_tz_search_struct->i_best_distance = i_distance;
		int_tz_search_struct->i_best_round = 0;
		int_tz_search_struct->i_point_nr = i_point_nr;
	}
}

void x265_enc_search_x_tz2_point_search(x265_t *h,
										x265_enc_search_t *enc_search,
										x265_pattern_t *pattern_key,
										x265_int_tz_search_struct_t *int_tz_search_struct,
										x265_mv_t *mv_srch_rng_lt,
										x265_mv_t *mv_srch_rng_rb )
{
	int32_t i_srch_rng_hor_left = 0;
	int32_t i_srch_rng_hor_right = 0;
	int32_t i_srch_rng_ver_top = 0;
	int32_t i_srch_rng_ver_bottom = 0;

	// 2 point search,                   //   1 2 3
	// check only the 2 untested points  //   4 0 5
	// around the start point            //   6 7 8
	int32_t i_start_x = 0;
	int32_t i_start_y = 0;

	i_srch_rng_hor_left = mv_srch_rng_lt->i_hor;
	i_srch_rng_hor_right = mv_srch_rng_rb->i_hor;
	i_srch_rng_ver_top = mv_srch_rng_lt->i_ver;
	i_srch_rng_ver_bottom = mv_srch_rng_rb->i_ver;

	// 2 point search,                   //   1 2 3
	// check only the 2 untested points  //   4 0 5
	// around the start point            //   6 7 8
	i_start_x = int_tz_search_struct->i_best_x;
	i_start_y = int_tz_search_struct->i_best_y;

	switch( int_tz_search_struct->i_point_nr )
	{
	case 1:
	{
		if ( (i_start_x - 1) >= i_srch_rng_hor_left )
		{
			x265_enc_search_x_tz_search_help(h,
											enc_search,
											pattern_key,
											int_tz_search_struct,
											i_start_x - 1,
											i_start_y,
											0,
											2 );
		}
		if ( (i_start_y - 1) >= i_srch_rng_ver_top )
		{
			x265_enc_search_x_tz_search_help(h,
											enc_search,
											pattern_key,
											int_tz_search_struct,
											i_start_x,
											i_start_y - 1,
											0,
											2 );
		}
	}
	break;
	case 2:
	{
		if ( (i_start_y - 1) >= i_srch_rng_ver_top )
		{
			if ( (i_start_x - 1) >= i_srch_rng_hor_left )
			{
				x265_enc_search_x_tz_search_help(h,
												enc_search,
												pattern_key,
												int_tz_search_struct,
												i_start_x - 1,
												i_start_y - 1,
												0,
												2 );
			}
			if ( (i_start_x + 1) <= i_srch_rng_hor_right )
			{
				x265_enc_search_x_tz_search_help(h,
												enc_search,
												pattern_key,
												int_tz_search_struct,
												i_start_x + 1,
												i_start_y - 1,
												0,
												2 );
			}
		}
	}
	break;
	case 3:
	{
		if ( (i_start_y - 1) >= i_srch_rng_ver_top )
		{
			x265_enc_search_x_tz_search_help(h,
											enc_search,
											pattern_key,
											int_tz_search_struct,
											i_start_x,
											i_start_y - 1,
											0,
											2 );
		}
		if ( (i_start_x + 1) <= i_srch_rng_hor_right )
		{
			x265_enc_search_x_tz_search_help(h,
											enc_search,
											pattern_key,
											int_tz_search_struct,
											i_start_x + 1,
											i_start_y,
											0,
											2 );
		}
	}
	break;
	case 4:
	{
		if ( (i_start_x - 1) >= i_srch_rng_hor_left )
		{
			if ( (i_start_y + 1) <= i_srch_rng_ver_bottom )
			{
				x265_enc_search_x_tz_search_help(h,
												enc_search,
												pattern_key,
												int_tz_search_struct,
												i_start_x - 1,
												i_start_y + 1,
												0,
												2 );
			}
			if ( (i_start_y - 1) >= i_srch_rng_ver_top )
			{
				x265_enc_search_x_tz_search_help(h,
												enc_search,
												pattern_key,
												int_tz_search_struct,
												i_start_x - 1,
												i_start_y - 1,
												0,
												2 );
			}
		}
	}
	break;
	case 5:
    {
    	if ( (i_start_x + 1) <= i_srch_rng_hor_right )
    	{
    		if ( (i_start_y - 1) >= i_srch_rng_ver_top )
    		{
    			x265_enc_search_x_tz_search_help(h,
    											enc_search,
    											pattern_key,
    											int_tz_search_struct,
    											i_start_x + 1,
    											i_start_y - 1,
    											0,
    											2 );
    		}
    		if ( (i_start_y + 1) <= i_srch_rng_ver_bottom )
    		{
    			x265_enc_search_x_tz_search_help(h,
    											enc_search,
    											pattern_key,
    											int_tz_search_struct,
    											i_start_x + 1,
    											i_start_y + 1,
    											0,
    											2 );
    		}
    	}
    }
    break;
	case 6:
	{
		if ( (i_start_x - 1) >= i_srch_rng_hor_left )
		{
			x265_enc_search_x_tz_search_help(h,
											enc_search,
											pattern_key,
											int_tz_search_struct,
											i_start_x - 1,
											i_start_y,
											0,
											2 );
		}
		if ( (i_start_y + 1) <= i_srch_rng_ver_bottom )
		{
			x265_enc_search_x_tz_search_help(h,
											enc_search,
											pattern_key,
											int_tz_search_struct,
											i_start_x,
											i_start_y + 1,
											0,
											2 );
		}
	}
	break;
	case 7:
	{
		if ( (i_start_y + 1) <= i_srch_rng_ver_bottom )
		{
			if ( (i_start_x - 1) >= i_srch_rng_hor_left )
			{
				x265_enc_search_x_tz_search_help(h,
												enc_search,
												pattern_key,
												int_tz_search_struct,
												i_start_x - 1,
												i_start_y + 1,
												0,
												2 );
			}
			if ( (i_start_x + 1) <= i_srch_rng_hor_right )
			{
				x265_enc_search_x_tz_search_help(h,
												enc_search,
												pattern_key,
												int_tz_search_struct,
												i_start_x + 1,
												i_start_y + 1,
												0,
												2 );
			}
		}
	}
	break;
	case 8:
	{
		if ( (i_start_x + 1) <= i_srch_rng_hor_right )
		{
			x265_enc_search_x_tz_search_help(h,
											enc_search,
											pattern_key,
											int_tz_search_struct,
											i_start_x + 1,
											i_start_y,
											0,
											2 );
		}
		if ( (i_start_y + 1) <= i_srch_rng_ver_bottom )
		{
			x265_enc_search_x_tz_search_help(h,
											enc_search,
											pattern_key,
											int_tz_search_struct,
											i_start_x,
											i_start_y + 1,
											0,
											2 );
		}
	}
	break;
	default:
	{
		assert( 0 );
	}
	break;
	} // switch( int_tz_search_struct->uc_point_nr )
}

void x265_enc_search_x_tz8_point_square_search(x265_t *h,
												x265_enc_search_t *enc_search,
												x265_pattern_t *pattern_key,
												x265_int_tz_search_struct_t *int_tz_search_struct,
												x265_mv_t *mv_srch_rng_lt,
												x265_mv_t *mv_srch_rng_rb,
												const int32_t i_start_x,
												const int32_t i_start_y,
												const int32_t i_dist )
{
	int32_t i_srch_rng_hor_left = 0;
	int32_t i_srch_rng_hor_right = 0;
	int32_t i_srch_rng_ver_top = 0;
	int32_t i_srch_rng_ver_bottom = 0;

	// 8 point search,                   //   1 2 3
	// search around the start point     //   4 0 5
	// with the required  distance       //   6 7 8
	int32_t i_top = 0;
	int32_t i_bottom = 0;
	int32_t i_left = 0;
	int32_t i_right = 0;


	i_srch_rng_hor_left = mv_srch_rng_lt->i_hor;
	i_srch_rng_hor_right = mv_srch_rng_rb->i_hor;
	i_srch_rng_ver_top = mv_srch_rng_lt->i_ver;
	i_srch_rng_ver_bottom = mv_srch_rng_rb->i_ver;

	// 8 point search,                   //   1 2 3
	// search around the start point     //   4 0 5
	// with the required  distance       //   6 7 8
	assert( i_dist != 0 );
	i_top = i_start_y - i_dist;
	i_bottom = i_start_y + i_dist;
	i_left = i_start_x - i_dist;
	i_right = i_start_x + i_dist;


	int_tz_search_struct->i_best_round += 1;

	if ( i_top >= i_srch_rng_ver_top ) // check top
	{
		if ( i_left >= i_srch_rng_hor_left ) // check top left
		{
			x265_enc_search_x_tz_search_help(h,
											enc_search,
											pattern_key,
											int_tz_search_struct,
											i_left,
											i_top,
											1,
											i_dist );
		}
		// top middle
		x265_enc_search_x_tz_search_help(h,
										enc_search,
										pattern_key,
										int_tz_search_struct,
										i_start_x,
										i_top,
										2,
										i_dist );

		if ( i_right <= i_srch_rng_hor_right ) // check top right
		{
			x265_enc_search_x_tz_search_help(h,
											enc_search,
											pattern_key,
											int_tz_search_struct,
											i_right,
											i_top,
											3,
											i_dist );
		}
	} // check top
	if ( i_left >= i_srch_rng_hor_left ) // check middle left
	{
		x265_enc_search_x_tz_search_help(h,
										enc_search,
										pattern_key,
										int_tz_search_struct,
										i_left,
										i_start_y,
										4,
										i_dist );
	}
	if ( i_right <= i_srch_rng_hor_right ) // check middle right
	{
		x265_enc_search_x_tz_search_help(h,
										enc_search,
										pattern_key,
										int_tz_search_struct,
										i_right,
										i_start_y,
										5,
										i_dist );
	}
	if ( i_bottom <= i_srch_rng_ver_bottom ) // check bottom
	{
		if ( i_left >= i_srch_rng_hor_left ) // check bottom left
		{
			x265_enc_search_x_tz_search_help(h,
											enc_search,
											pattern_key,
											int_tz_search_struct,
											i_left,
											i_bottom,
											6,
											i_dist );
		}
		// check bottom middle
		x265_enc_search_x_tz_search_help(h,
										enc_search,
										pattern_key,
										int_tz_search_struct,
										i_start_x,
										i_bottom,
										7,
										i_dist );

		if ( i_right <= i_srch_rng_hor_right ) // check bottom right
		{
			x265_enc_search_x_tz_search_help(h,
											enc_search,
											pattern_key,
											int_tz_search_struct,
											i_right,
											i_bottom,
											8,
											i_dist );
		}
	} // check bottom
}



void x265_enc_search_x_tz8_point_diamond_search(x265_t *h,
												x265_enc_search_t *enc_search,
												x265_pattern_t *pattern_key,
												x265_int_tz_search_struct_t *int_tz_search_struct,
												x265_mv_t *mv_srch_rng_lt,
												x265_mv_t *mv_srch_rng_rb,
												const int32_t i_start_x,
												const int32_t i_start_y,
												const int32_t i_dist )
{
	int32_t i_index = 0 ;
	int32_t i_srch_rng_hor_left = 0;
	int32_t i_srch_rng_hor_right = 0;
	int32_t i_srch_rng_ver_top = 0;
	int32_t i_srch_rng_ver_bottom = 0;

	// 8 point search,                   //   1 2 3
	// search around the start point     //   4 0 5
	// with the required  distance       //   6 7 8
	int32_t i_top = 0;
	int32_t i_bottom = 0;
	int32_t i_left = 0;
	int32_t i_right = 0;
	int32_t i_top2 = 0;
	int32_t i_bottom2 = 0;
	int32_t i_left2 = 0;
	int32_t i_right2 = 0;
	int32_t i_pos_yt = 0;
	int32_t i_pos_yb = 0;
	int32_t i_pos_xl = 0;
	int32_t i_pos_xr = 0;



	i_srch_rng_hor_left   = mv_srch_rng_lt->i_hor;
	i_srch_rng_hor_right  = mv_srch_rng_rb->i_hor;
	i_srch_rng_ver_top    = mv_srch_rng_lt->i_ver;
	i_srch_rng_ver_bottom = mv_srch_rng_rb->i_ver;

	// 8 point search,                   //   1 2 3
	// search around the start point     //   4 0 5
	// with the required  distance       //   6 7 8
	assert ( i_dist != 0 );
	i_top = i_start_y - i_dist;
	i_bottom = i_start_y + i_dist;
	i_left = i_start_x - i_dist;
	i_right = i_start_x + i_dist;



	int_tz_search_struct->i_best_round += 1;

	if ( i_dist == 1 ) // i_dist == 1
	{
		if ( i_top >= i_srch_rng_ver_top ) // check top
		{
			x265_enc_search_x_tz_search_help(h,
											enc_search,
											pattern_key,
											int_tz_search_struct,
											i_start_x,
											i_top,
											2,
											i_dist );
		}
		if ( i_left >= i_srch_rng_hor_left ) // check middle left
		{
			x265_enc_search_x_tz_search_help(h,
											enc_search,
											pattern_key,
											int_tz_search_struct,
											i_left,
											i_start_y,
											4,
											i_dist );
		}
		if ( i_right <= i_srch_rng_hor_right ) // check middle right
		{
			x265_enc_search_x_tz_search_help(h,
											enc_search,
											pattern_key,
											int_tz_search_struct,
											i_right,
											i_start_y,
											5,
											i_dist );
		}
		if ( i_bottom <= i_srch_rng_ver_bottom ) // check bottom
		{
			x265_enc_search_x_tz_search_help(h,
											enc_search,
											pattern_key,
											int_tz_search_struct,
											i_start_x,
											i_bottom,
											7,
											i_dist );
		}
	}
	else // if (i_dist != 1)
	{
		if ( i_dist <= 8 )
		{
			i_top2 = i_start_y - (i_dist>>1);
			i_bottom2 = i_start_y + (i_dist>>1);
			i_left2 = i_start_x - (i_dist>>1);
			i_right2 = i_start_x + (i_dist>>1);

			if (  i_top >= i_srch_rng_ver_top && i_left >= i_srch_rng_hor_left &&
					i_right <= i_srch_rng_hor_right && i_bottom <= i_srch_rng_ver_bottom ) // check border
			{
				x265_enc_search_x_tz_search_help(h,
												enc_search,
												pattern_key,
												int_tz_search_struct,
												i_start_x,
												i_top,
												2,
												i_dist);
				x265_enc_search_x_tz_search_help(h,
												enc_search,
												pattern_key,
												int_tz_search_struct,
												i_left2,
												i_top2,
												1,
												i_dist >> 1);
				x265_enc_search_x_tz_search_help(h,
												enc_search,
												pattern_key,
												int_tz_search_struct,
												i_right2,
												i_top2,
												3,
												i_dist >> 1);
				x265_enc_search_x_tz_search_help(h,
												enc_search,
												pattern_key,
												int_tz_search_struct,
												i_left,
												i_start_y,
												4,
												i_dist);
				x265_enc_search_x_tz_search_help(h,
												enc_search,
												pattern_key,
												int_tz_search_struct,
												i_right,
												i_start_y,
												5,
												i_dist);
				x265_enc_search_x_tz_search_help(h,
												enc_search,
												pattern_key,
												int_tz_search_struct,
												i_left2,
												i_bottom2,
												6,
												i_dist >> 1);
				x265_enc_search_x_tz_search_help(h,
												enc_search,
												pattern_key,
												int_tz_search_struct,
												i_right2,
												i_bottom2,
												8,
												i_dist >> 1);
				x265_enc_search_x_tz_search_help(h,
												enc_search,
												pattern_key,
												int_tz_search_struct,
												i_start_x,
												i_bottom,
												7,
												i_dist);
			}
			else // check border
			{
				if ( i_top >= i_srch_rng_ver_top ) // check top
				{
					x265_enc_search_x_tz_search_help(h,
													enc_search,
													pattern_key,
													int_tz_search_struct,
													i_start_x,
													i_top,
													2,
													i_dist );
				}
				if ( i_top2 >= i_srch_rng_ver_top ) // check half top
				{
					if ( i_left2 >= i_srch_rng_hor_left ) // check half left
					{
						x265_enc_search_x_tz_search_help(h,
														enc_search,
														pattern_key,
														int_tz_search_struct,
														i_left2,
														i_top2,
														1,
														(i_dist >> 1));
					}
					if ( i_right2 <= i_srch_rng_hor_right ) // check half right
					{
						x265_enc_search_x_tz_search_help(h,
														enc_search,
														pattern_key,
														int_tz_search_struct,
														i_right2,
														i_top2,
														3,
														(i_dist >> 1));
					}
				} // check half top
				if ( i_left >= i_srch_rng_hor_left ) // check left
				{
					x265_enc_search_x_tz_search_help(h,
													enc_search,
													pattern_key,
													int_tz_search_struct,
													i_left,
													i_start_y,
													4,
													i_dist);
				}
				if ( i_right <= i_srch_rng_hor_right ) // check right
				{
					x265_enc_search_x_tz_search_help(h,
													enc_search,
													pattern_key,
													int_tz_search_struct,
													i_right,
													i_start_y,
													5,
													i_dist);
				}
				if ( i_bottom2 <= i_srch_rng_ver_bottom ) // check half bottom
				{
					if ( i_left2 >= i_srch_rng_hor_left ) // check half left
					{
						x265_enc_search_x_tz_search_help(h,
														enc_search,
														pattern_key,
														int_tz_search_struct,
														i_left2,
														i_bottom2,
														6,
														(i_dist >> 1));
					}
					if ( i_right2 <= i_srch_rng_hor_right ) // check half right
					{
						x265_enc_search_x_tz_search_help(h,
														enc_search,
														pattern_key,
														int_tz_search_struct,
														i_right2,
														i_bottom2,
														8,
														(i_dist >> 1));
					}
				} // check half bottom
				if ( i_bottom <= i_srch_rng_ver_bottom ) // check bottom
				{
					x265_enc_search_x_tz_search_help(h,
													enc_search,
													pattern_key,
													int_tz_search_struct,
													i_start_x,
													i_bottom,
													7,
													i_dist);
				}
			} // check border
		}
		else // i_dist > 8
		{
			if ( i_top >= i_srch_rng_ver_top && i_left >= i_srch_rng_hor_left &&
					i_right <= i_srch_rng_hor_right && i_bottom <= i_srch_rng_ver_bottom ) // check border
			{
				x265_enc_search_x_tz_search_help(h,
												enc_search,
												pattern_key,
												int_tz_search_struct,
												i_start_x,
												i_top,
												0,
												i_dist);
				x265_enc_search_x_tz_search_help(h,
												enc_search,
												pattern_key,
												int_tz_search_struct,
												i_left,
												i_start_y,
												0,
												i_dist);
				x265_enc_search_x_tz_search_help(h,
												enc_search,
												pattern_key,
												int_tz_search_struct,
												i_right,
												i_start_y,
												0,
												i_dist);
				x265_enc_search_x_tz_search_help(h,
												enc_search,
												pattern_key,
												int_tz_search_struct,
												i_start_x,
												i_bottom,
												0,
												i_dist);
				for ( i_index = 1; i_index < 4; i_index++ )
				{
					i_pos_yt = i_top    + ((i_dist>>2) * i_index);
					i_pos_yb = i_bottom - ((i_dist>>2) * i_index);
					i_pos_xl = i_start_x - ((i_dist>>2) * i_index);
					i_pos_xr = i_start_x + ((i_dist>>2) * i_index);
					x265_enc_search_x_tz_search_help(h,
													enc_search,
													pattern_key,
													int_tz_search_struct,
													i_pos_xl,
													i_pos_yt,
													0,
													i_dist);
					x265_enc_search_x_tz_search_help(h,
													enc_search,
													pattern_key,
													int_tz_search_struct,
													i_pos_xr,
													i_pos_yt,
													0,
													i_dist);
					x265_enc_search_x_tz_search_help(h,
													enc_search,
													pattern_key,
													int_tz_search_struct,
													i_pos_xl,
													i_pos_yb,
													0,
													i_dist);
					x265_enc_search_x_tz_search_help(h,
													enc_search,
													pattern_key,
													int_tz_search_struct,
													i_pos_xr,
													i_pos_yb,
													0,
													i_dist);
				}
			}
			else // check border
			{
				if ( i_top >= i_srch_rng_ver_top ) // check top
				{
					x265_enc_search_x_tz_search_help(h,
													enc_search,
													pattern_key,
													int_tz_search_struct,
													i_start_x,
													i_top,
													0,
													i_dist);
				}
				if ( i_left >= i_srch_rng_hor_left ) // check left
				{
					x265_enc_search_x_tz_search_help(h,
													enc_search,
													pattern_key,
													int_tz_search_struct,
													i_left,
													i_start_y,
													0,
													i_dist );
				}
				if ( i_right <= i_srch_rng_hor_right ) // check right
				{
					x265_enc_search_x_tz_search_help(h,
													enc_search,
													pattern_key,
													int_tz_search_struct,
													i_right,
													i_start_y,
													0,
													i_dist );
				}
				if ( i_bottom <= i_srch_rng_ver_bottom ) // check bottom
				{
					x265_enc_search_x_tz_search_help(h,
													enc_search,
													pattern_key,
													int_tz_search_struct,
													i_start_x,
													i_bottom,
													0,
													i_dist);
				}
				for ( i_index = 1; i_index < 4; i_index++ )
				{
					i_pos_yt = i_top + ((i_dist>>2) * i_index);
					i_pos_yb = i_bottom - ((i_dist>>2) * i_index);
					i_pos_xl = i_start_x - ((i_dist>>2) * i_index);
					i_pos_xr = i_start_x + ((i_dist>>2) * i_index);

					if ( i_pos_yt >= i_srch_rng_ver_top ) // check top
					{
						if ( i_pos_xl >= i_srch_rng_hor_left ) // check left
						{
							x265_enc_search_x_tz_search_help(h,
															enc_search,
															pattern_key,
															int_tz_search_struct,
															i_pos_xl,
															i_pos_yt,
															0,
															i_dist);
						}
						if ( i_pos_xr <= i_srch_rng_hor_right ) // check right
						{
							x265_enc_search_x_tz_search_help(h,
															enc_search,
															pattern_key,
															int_tz_search_struct,
															i_pos_xr,
															i_pos_yt,
															0,
															i_dist);
						}
					} // check top
					if ( i_pos_yb <= i_srch_rng_ver_bottom ) // check bottom
					{
						if ( i_pos_xl >= i_srch_rng_hor_left ) // check left
						{
							x265_enc_search_x_tz_search_help(h,
															enc_search,
															pattern_key,
															int_tz_search_struct,
															i_pos_xl,
															i_pos_yb,
															0,
															i_dist );
						}
						if ( i_pos_xr <= i_srch_rng_hor_right ) // check right
						{
							x265_enc_search_x_tz_search_help(h,
															enc_search,
															pattern_key,
															int_tz_search_struct,
															i_pos_xr,
															i_pos_yb,
															0,
															i_dist );
						}
					} // check bottom
				} // for ...
			} // check border
		} // i_dist <= 8
	} // i_dist == 1
}

//<--

uint32_t x265_enc_search_x_pattern_refinement(x265_t *h,
											x265_enc_search_t *enc_search,
											x265_pattern_t *pattern_key,
											x265_mv_t *base_ref_mv,
											int32_t i_frac,
											x265_mv_t *mv_frac )
{
	int32_t i_ref_stride = 0;
	uint32_t loop = 0 ;
	uint32_t i_dist = 0;
	uint32_t i_dist_best = 0;
	uint32_t i_direc_best = 0;
	int32_t i_hor_val = 0;
	int32_t i_ver_val = 0;
	pixel *p_ref_pos = NULL;
	const x265_mv_t *mv_refine = NULL;
	x265_mv_t mv_test ;

	i_dist_best = X265_MAX_UINT;
	i_ref_stride = x265_image_get_stride(&enc_search->prediction.filtered_block[0][0]);
#if X265_NS_HAD
	x265_rd_cost_set_dist_param_p9(h,
									&h->rd_cost,
									pattern_key,
									x265_image_get_luma_addr_p2(h, &enc_search->prediction.filtered_block[0][0]),
									i_ref_stride,
									1,
									&enc_search->dist_param,
									h->param.b_use_had_me,
									0 );
#else
	x265_rd_cost_set_dist_param_p8(h,
									&h->rd_cost,
									pattern_key,
									x265_image_get_luma_addr_p2(h, &enc_search->prediction.filtered_block[0][0]),
									i_ref_stride,
									1,
									&enc_search->dist_param,
									h->param.b_use_had_me );
#endif

	mv_refine = (i_frac == 2 ? mv_refine_h : mv_refine_q);

	for ( loop = 0; loop < 9; ++ loop )
	{
		memcpy(&mv_test, &mv_refine[loop], sizeof(x265_mv_t));
		x265_mv_add_to(&mv_test, base_ref_mv);

		i_hor_val = mv_test.i_hor * i_frac;
		i_ver_val = mv_test.i_ver * i_frac;
		p_ref_pos = x265_image_get_luma_addr_p2(h,
												&enc_search->prediction.filtered_block[ i_ver_val & 3 ][ i_hor_val & 3 ]);
		if ( i_hor_val == 2 && ( i_ver_val & 1 ) == 0 )
		{
			p_ref_pos += 1;
		}
		if ( ( i_hor_val & 1 ) == 0 && i_ver_val == 2 )
		{
			p_ref_pos += i_ref_stride;
		}
		memcpy(&mv_test, &mv_refine[loop], sizeof(x265_mv_t));
		x265_mv_add_to(&mv_test, mv_frac);

		enc_search->dist_param.i_comp = 0 ;

		enc_search->dist_param.cur = p_ref_pos;
		enc_search->dist_param.i_bit_depth = h->param.sps.i_bit_depth_y;
		i_dist = enc_search->dist_param.dist_func(&h->rd_cost, &enc_search->dist_param);
		i_dist += x265_rd_cost_get_cost_p3(&h->rd_cost,
										mv_test.i_hor,
										mv_test.i_ver);

		if ( i_dist < i_dist_best )
		{
			i_dist_best  = i_dist;
			i_direc_best = loop;
		}
	}

	memcpy(mv_frac, &mv_refine[i_direc_best], sizeof(x265_mv_t));

	return i_dist_best;
}




/** function for encoding and reconstructing luma/chroma samples of a pcm mode cu.
 * \param cu pointer to current cu
 * \param i_abs_part_idx part index
 * \param pi_org pointer to original sample arrays
 * \param pi_p_cm pointer to pcm code arrays
 * \param pi_pred pointer to prediction signal arrays
 * \param pi_resi pointer to residual signal arrays
 * \param pi_reco pointer to reconstructed sample arrays
 * \param i_stride stride of the original/prediction/residual sample arrays
 * \param i_width block width
 * \param i_height block height
 * \param tt_text texture component type
 * \returns void
 */
void x265_enc_search_x_enc_pcm(x265_t* h,
								x265_enc_search_t *enc_search,
								x265_data_cu_t *cu,
								uint32_t i_abs_part_idx,
								pixel *org,
								pixel *pcm,
								pixel *pred,
								spixel *resi,
								pixel *reco,
								uint32_t i_stride,
								uint32_t i_width,
								uint32_t i_height,
								enum text_type_e i_text_type )
{
	uint32_t i_x = 0, i_y = 0;
	uint32_t i_recon_stride = 0;
	pixel *p_org = NULL;
	pixel *p_pcm = NULL;
	pixel *p_pred = NULL;
	spixel *p_resi = NULL;
	pixel *p_reco = NULL;
	pixel *p_reco_pic;
	int32_t shift_pcm;


	p_org = org;
	p_pcm = pcm;
	p_pred = pred;
	p_resi = resi;
	p_reco = reco;

	if( i_text_type == TEXT_LUMA)
	{
		i_recon_stride = h->fdec->i_stride[0] ;
		p_reco_pic = x265_frame_get_luma_addr_p4(h,
												h->fdec,
												x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu),
												x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)cu)+i_abs_part_idx);
		shift_pcm = h->cu.pic.i_bit_depth_y - h->sps[0].i_pcm_bit_depth_luma;
	}
	else
	{
		i_recon_stride = h->fdec->i_stride[1] ;

		if( i_text_type == TEXT_CHROMA_U )
		{
			p_reco_pic = x265_frame_get_cb_addr_p4(h,
													h->fdec,
													x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu),
													x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)cu)+i_abs_part_idx);
		}
		else
		{
			p_reco_pic = x265_frame_get_cr_addr_p4(h,
													h->fdec,
													x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu),
													x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)cu)+i_abs_part_idx);
		}
		shift_pcm = h->cu.pic.i_bit_depth_c - h->sps[0].i_pcm_bit_depth_chroma ;
	}

	// reset pred and residual
	for( i_y = 0; i_y < i_height; i_y++ )
	{
		for( i_x = 0; i_x < i_width; i_x++ )
		{
			p_pred[i_x] = 0;
			p_resi[i_x] = 0;
		}
		p_pred += i_stride;
		p_resi += i_stride;
	}

	// encode
	for( i_y = 0; i_y < i_height; i_y++ )
	{
		for( i_x = 0; i_x < i_width; i_x++ )
		{
			p_pcm[i_x] = p_org[i_x]>> shift_pcm;
		}
		p_pcm += i_width;
		p_org += i_stride;
	}

	p_pcm  = pcm;

	// reconstruction
	for( i_y = 0; i_y < i_height; i_y++ )
	{
		for( i_x = 0; i_x < i_width; i_x++ )
		{
			p_reco   [i_x] = p_pcm[i_x]<< shift_pcm;
			p_reco_pic[i_x] = p_reco[i_x];
		}
		p_pcm += i_width;
		p_reco += i_stride;
		p_reco_pic += i_recon_stride;
	}
}

/**  function for pcm mode estimation.
 * \param cu
 * \param org_yuv
 * \param rpred_yuv
 * \param rresi_yuv
 * \param rreco_yuv
 * \returns void
 */
void x265_enc_search_ipcm_search(x265_t *h,
								x265_enc_search_t *enc_search,
								x265_data_cu_t *cu,
								x265_image_t *p_org_image,
								x265_image_t **pp_pred_image,
								x265_simage_t **pp_resi_image,
								x265_image_t **pp_reco_image )
{
	uint32_t i_depth = 0;
	uint32_t i_width = 0;
	uint32_t i_height = 0;
	uint32_t i_stride = 0;
	uint32_t i_stride_c = 0;
	uint32_t i_width_c  = 0;
	uint32_t i_height_c = 0;
	uint32_t i_distortion = 0;
	uint32_t i_bits = 0;
	uint32_t i_abs_part_idx = 0;
	uint32_t i_min_coeff_size = 0;
	uint32_t i_luma_offset = 0;
	uint32_t i_chroma_offset = 0;
	double f_cost;
	pixel *p_orig = NULL;
	spixel *p_resi = NULL;
	pixel *p_reco = NULL;
	pixel *p_pred = NULL;
	pixel *p_pcm = NULL;






	i_depth = x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0);
	i_width = x265_data_cu_get_width_p2(cu, 0);
	i_height = x265_data_cu_get_height_p2(cu, 0);
	i_stride = x265_image_get_stride((*pp_pred_image));
	i_stride_c = x265_image_get_c_stride((*pp_pred_image));
	i_width_c = i_width  >> 1;
	i_height_c = i_height >> 1;



	i_min_coeff_size = h->cu.pic.i_min_cu_width * h->cu.pic.i_min_cu_height ;
	i_luma_offset = i_min_coeff_size * i_abs_part_idx;
	i_chroma_offset = i_luma_offset >> 2;



	p_orig = x265_image_get_luma_addr_p4(h, p_org_image, 0, i_width) ;
	p_resi = x265_simage_get_luma_addr_p4(h, (*pp_resi_image), 0, i_width) ;
	p_pred = x265_image_get_luma_addr_p4(h, (*pp_pred_image), 0, i_width) ;
	p_reco = x265_image_get_luma_addr_p4(h, (*pp_reco_image), 0, i_width) ;
	p_pcm = x265_data_cu_get_pcm_sample_y(cu) + i_luma_offset;
	x265_enc_search_x_enc_pcm (h,
								enc_search,
								cu,
								0,
								p_orig,
								p_pcm,
								p_pred,
								p_resi,
								p_reco,
								i_stride,
								i_width,
								i_height,
								TEXT_LUMA);

	// chroma u
	p_orig = x265_image_get_cb_addr_p4(h, p_org_image, 0, i_width) ;
	p_resi = x265_simage_get_cb_addr_p4(h, (*pp_resi_image), 0, i_width) ;
	p_pred = x265_image_get_cb_addr_p4(h, (*pp_pred_image), 0, i_width) ;
	p_reco = x265_image_get_cb_addr_p4(h, (*pp_reco_image), 0, i_width) ;
	p_pcm = x265_data_cu_get_pcm_sample_cb(cu) + i_chroma_offset;
	x265_enc_search_x_enc_pcm (h,
								enc_search,
								cu,
								0,
								p_orig,
								p_pcm,
								p_pred,
								p_resi,
								p_reco,
								i_stride_c,
								i_width_c,
								i_height_c,
								TEXT_CHROMA_U);

	// chroma v
	p_orig = x265_image_get_cr_addr_p4(h, p_org_image, 0, i_width) ;
	p_resi = x265_simage_get_cr_addr_p4(h, (*pp_resi_image), 0, i_width) ;
	p_pred = x265_image_get_cr_addr_p4(h, (*pp_pred_image), 0, i_width) ;
	p_reco = x265_image_get_cr_addr_p4(h, (*pp_reco_image), 0, i_width) ;
	p_pcm = x265_data_cu_get_pcm_sample_cr(cu) + i_chroma_offset;
	x265_enc_search_x_enc_pcm (h,
								enc_search,
								cu,
								0,
								p_orig,
								p_pcm,
								p_pred,
								p_resi,
								p_reco,
								i_stride_c,
								i_width_c,
								i_height_c,
								TEXT_CHROMA_V );

	x265_enc_entropy_reset_bits(&h->enc_entropy);
	x265_enc_search_x_enc_intra_header(h, enc_search, cu, i_depth, i_abs_part_idx, 1, 0);
	i_bits = x265_enc_entropy_get_number_of_written_bits(&h->enc_entropy);

	f_cost = x265_rd_cost_calc_rd_cost(&h->rd_cost, i_bits, i_distortion, 0, DF_DEFAULT );

	if(h->param.b_use_sbac_rd)
	{
		x265_enc_sbac_load (&h->rd_go_on_sbac_coder,
							h->pppc_rd_sbac_coder[i_depth][CI_CURR_BEST]);
	}

	cu->i_total_bits = i_bits;
	cu->f_total_cost = f_cost;
	cu->i_total_distortion = i_distortion;

	x265_data_cu_copy_to_pic_p5(h, cu, i_depth, 0, 0);
}

void x265_enc_search_x_get_inter_prediction_error(x265_t *h,
												x265_enc_search_t *enc_search,
												x265_data_cu_t* cu,
												x265_image_t *image_org,
												int32_t i_part_idx,
												uint32_t *p_err,
												int32_t b_hadamard )
{
	uint32_t i_abs_part_idx = 0;
	int32_t i_width = 0;
	int32_t i_height = 0;
	x265_dist_param_t dist_param;




	x265_prediction_motion_compensation(h,
										(x265_prediction_t*)enc_search,
										cu,
										&enc_search->tmp_image_pred,
										REF_PIC_LIST_X,
										i_part_idx );

	x265_data_cu_get_part_index_and_size(cu,
										i_part_idx,
										&i_abs_part_idx,
										&i_width,
										&i_height );

	dist_param.b_apply_weight = 0;


	x265_rd_cost_set_dist_param_p11(h,
									&h->rd_cost,
									&dist_param,
									h->cu.pic.i_bit_depth_y,
									x265_image_get_luma_addr_p3(h, image_org, i_abs_part_idx ),
									x265_image_get_stride(image_org),
									x265_image_get_luma_addr_p3(h, &enc_search->tmp_image_pred, i_abs_part_idx ),
									x265_image_get_stride(&enc_search->tmp_image_pred),
#if X265_NS_HAD
									i_width,
									i_height,
									h->param.b_use_had_me, h->param.b_use_ns_qt);
#else
									i_width,
									i_height,
									h->param.b_use_had_me);
#endif
	*p_err = dist_param.dist_func(&h->rd_cost, &dist_param);
}

/** estimation of best merge coding
 * \param cu
 * \param yuv_org
 * \param i_puidx
 * \param i_inter_dir
 * \param pamv_field
 * \param i_merge_index
 * \param ri_cost
 * \param ri_bits
 * \param puh_neigh_cands
 * \param b_valid
 * \returns void
 */
void x265_enc_search_x_merge_estimation(x265_t *h,
										x265_enc_search_t *enc_search,
										x265_data_cu_t *cu,
										x265_image_t *image_org,
										int32_t i_pu_idx,
										uint32_t *p_inter_dir,
										x265_mv_field_t *mv_field,
										uint32_t *p_merge_index,
										uint32_t *p_cost,
										x265_mv_field_t *mv_field_neighbours,
										uint8_t *inter_dir_neighbours,
										int32_t *p_num_valid_merge_cand )
{
	int32_t i_width = 0;
	int32_t i_height = 0;
	uint32_t i_abs_part_idx = 0;
	uint32_t i_depth = 0;
	uint32_t i_merge_cand = 0;
	uint32_t i_cost_cand = 0;
	uint32_t i_bits_cand = 0;
	enum part_size_e i_part_size = 0;



	x265_data_cu_get_part_index_and_size(cu, i_pu_idx, &i_abs_part_idx, &i_width, &i_height );
	i_depth = x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_part_idx );
	i_part_size = x265_data_cu_get_partition_size_p2(cu, 0);
	if ( h->pps[0].i_log2_parallel_merge_level_minus2
			&& i_part_size != SIZE_2Nx2N && x265_data_cu_get_width_p2(cu, 0 ) <= 8 )
	{
		x265_data_cu_set_part_size_sub_parts(h, cu, SIZE_2Nx2N, 0, i_depth );
		if ( i_pu_idx == 0 )
		{
			x265_data_cu_get_inter_merge_candidates(h,
													cu,
													0,
													0,
													mv_field_neighbours,
													inter_dir_neighbours,
													p_num_valid_merge_cand,
													-1 );
		}
		x265_data_cu_set_part_size_sub_parts(h, cu, i_part_size, 0, i_depth );
	}
	else
	{
		x265_data_cu_get_inter_merge_candidates(h,
												cu,
												i_abs_part_idx,
												i_pu_idx,
												mv_field_neighbours,
												inter_dir_neighbours,
												p_num_valid_merge_cand,
												-1 );
	}
	x265_enc_search_x_restrict_bipred_merge_cand(enc_search,
												cu,
												i_pu_idx,
												mv_field_neighbours,
												inter_dir_neighbours,
												*p_num_valid_merge_cand );

	*p_cost = X265_MAX_UINT;
	for( i_merge_cand = 0; i_merge_cand < *p_num_valid_merge_cand; ++i_merge_cand )
	{
		i_cost_cand = X265_MAX_UINT;
		i_bits_cand = 0;
		i_part_size = x265_data_cu_get_partition_size_p2(cu,  0 );

		x265_cu_mv_field_set_all_mv_field_p6(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu,
																			REF_PIC_LIST_0),
											&mv_field_neighbours[0 + 2*i_merge_cand],
											i_part_size,
											i_abs_part_idx,
											0,
											i_pu_idx) ;
		x265_cu_mv_field_set_all_mv_field_p6(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu,
																			REF_PIC_LIST_1),
											&mv_field_neighbours[1 + 2*i_merge_cand],
											i_part_size,
											i_abs_part_idx,
											0,
											i_pu_idx) ;

		x265_enc_search_x_get_inter_prediction_error(h,
													enc_search,
													cu,
													image_org,
													i_pu_idx,
													&i_cost_cand,
													h->param.b_use_had_me );
		i_bits_cand = i_merge_cand + 1;
		if (i_merge_cand == h->param.i_max_num_merge_cand -1)
		{
			i_bits_cand--;
		}
		i_cost_cand = i_cost_cand + x265_rd_cost_get_cost_p2(&h->rd_cost, i_bits_cand);
		if ( i_cost_cand < *p_cost )
		{
			*p_cost = i_cost_cand;
			memcpy(&mv_field[0], &mv_field_neighbours[0 + 2*i_merge_cand], sizeof(x265_mv_field_t));
			memcpy(&mv_field[1], &mv_field_neighbours[1 + 2*i_merge_cand], sizeof(x265_mv_field_t));
			*p_inter_dir = inter_dir_neighbours[i_merge_cand];
			*p_merge_index = i_merge_cand;
		}
	}
}

/** convert bi-pred merge candidates to uni-pred
 * \param cu
 * \param pu_idx
 * \param mv_field_neighbours
 * \param inter_dir_neighbours
 * \param num_valid_merge_cand
 * \returns void
 */
void x265_enc_search_x_restrict_bipred_merge_cand(x265_enc_search_t *enc_search,
												x265_data_cu_t* cu,
												uint32_t i_pu_idx,
												x265_mv_field_t *mv_field_neighbours,
												uint8_t *inter_dir_neighbours,
												int32_t i_num_valid_merge_cand )
{
	uint32_t i_merge_cand = 0;
	x265_mv_t mv ;

	x265_mv_set_zero (&mv) ;
	if ( x265_data_cu_is_bipred_restriction(cu, i_pu_idx))
	{
		for( i_merge_cand = 0; i_merge_cand < i_num_valid_merge_cand; ++ i_merge_cand )
		{
			if ( inter_dir_neighbours[i_merge_cand] == 3 )
			{
				inter_dir_neighbours[i_merge_cand] = 1;
				x265_mv_field_set_mv_field(&mv_field_neighbours[(i_merge_cand << 1) + 1],
											&mv,
											-1);
			}
		}
	}
}


/** search of the best candidate for inter prediction
 * \param cu
 * \param p_org_image
 * \param pp_pred_image
 * \param pp_resi_image
 * \param pp_reco_image
 * \param b_use_res
 * \returns void
 */
#if X265_AMP_MRG
void x265_enc_search_pred_inter_search(x265_t *h,
										x265_enc_search_t *enc_search,
										x265_data_cu_t *cu,
										x265_image_t *p_org_image,
										x265_image_t **pp_pred_image,
										x265_simage_t **pp_resi_image,
										x265_image_t **pp_reco_image,
										int32_t b_use_res,
										int32_t b_use_mrg )
#else
void x265_enc_search_pred_inter_search(x265_t *h,
										x265_enc_search_t *enc_search,
										x265_data_cu_t *cu,
										x265_image_t *p_org_image,
										x265_image_t **pp_pred_image,
										x265_simage_t **pp_resi_image,
										x265_image_t **pp_reco_image,
										int32_t b_use_res )
#endif
{
	//	x265_mv_t mv_srch_rng_lt;
	//	x265_mv_t mv_srch_rng_rb;
	x265_mv_t mv_zero;
	x265_mv_t temp_mv; //kolya
	x265_mv_t mv[2];
	x265_mv_t mv_bi[2];
	x265_mv_t mv_temp[2][33];
	int32_t i_num_part = 0;
	int32_t i_num_pred_dir = 0;
	x265_mv_t mv_pred[2][33];
	x265_mv_t mv_pred_bi[2][33];
	int32_t aai_mvp_idx_bi[2][33];
	int32_t aai_mvp_idx[2][33];
	int32_t aai_mvp_num[2][33];
	x265_amvp_info_t aac_amvp_info[2][33];
	//if un-initialized, may cause segv in bi-directional prediction iterative stage.
	int32_t i_ref_idx[2] = {0, 0} ;
	int32_t i_ref_idx_bi[2];
	uint32_t i_part_addr;
	int32_t i_roi_width, i_roi_height;
	uint32_t i_mb_bits[3] = {1, 1, 0};
	uint32_t i_last_mode = 0;
	int32_t i_ref_start, i_ref_end;
	enum part_size_e i_part_size = 0;
	int32_t best_bi_pref_idx_l1 = 0;
	int32_t best_bi_pmvp_l1 = 0;
	uint32_t bi_p_dist_temp = 0;
#if X265_ZERO_MVD_EST
	int32_t ai_zero_mvd_mvp_idx[2] = {-1, -1};
	int32_t ai_zero_mvd_ref_idx[2] = {0, 0};
	int32_t i_zero_mvd_dir = 0;
#endif
	x265_mv_field_t mv_field_neighbours[X265_MRG_MAX_NUM_CANDS << 1]; // double length for mv of both lists
	uint8_t uh_inter_dir_neighbours[X265_MRG_MAX_NUM_CANDS];
	int32_t num_valid_merge_cand = 0 ;
	int32_t i_part_idx = 0;
	uint32_t i_cost[2];
	uint32_t i_cost_bi = 0;
	uint32_t i_cost_temp;
	uint32_t i_bits[3];
	uint32_t i_bits_temp;
#if X265_ZERO_MVD_EST
	uint32_t i_zero_mvd_cost = 0;
	uint32_t i_zero_mvd_cost_temp;
	uint32_t i_zero_mvd_bits_temp;
	uint32_t i_zero_mvd_dist_temp = 0;
	uint32_t ai_zero_mvd_bits[3];
#endif
	uint32_t best_bi_pdist = 0;
	uint32_t i_cost_temp_l0[X265_MAX_NUM_REF];
	int32_t i_num_ref=0 ;
	uint32_t i_bits_temp_l0[X265_MAX_NUM_REF];
	int32_t b_test_normal_mc = 0;
	int32_t i_ref_list = 0;
	enum ref_pic_list_e  i_ref_pic_list = 0;
	int32_t i_ref_idx_temp = 0;
	x265_image_t * yuv_pred = NULL;
	uint32_t i_mot_bits[2];
	// 4-times iteration (default)
	int32_t i_num_iter = 4;
	int32_t i_iter = 0;
	int32_t b_changed = 0;


#if X265_ZERO_MVD_EST
	int32_t i_l0ref_idx_temp = 0;
	int32_t i_l1ref_idx_temp = 0;
	uint32_t i_ref_idx_bits_temp = 0;
	int32_t i_l0mvpidx = 0;
	int32_t i_l1mvpidx = 0;
#endif
	uint32_t i_m_ebits = 0;

	uint32_t i_m_rg_inter_dir = 0;
	x265_mv_field_t c_m_rgmv_field[2];
	uint32_t i_m_rgindex = 0;
	uint32_t i_m_e_inter_dir = 0;
	x265_mv_field_t c_m_emv_field[2];
#if X265_AMP_MRG
	uint32_t i_m_eerror = 0;
	uint32_t i_m_ecost = 0;
#else
	uint32_t i_m_eerror = 0;
	uint32_t i_m_ecost = 0;
#endif
	uint32_t i_m_rgcost = 0;
	x265_mv_field_t mv_field_zero ;

	x265_mv_set_zero(&mv_zero) ;
	memset (&mv_field_zero, 0, sizeof(x265_mv_field_t) ) ;
	mv_field_zero.i_ref_idx = -1 ;
	x265_image_clear(&enc_search->prediction.image_pred[0]);
	x265_image_clear(&enc_search->prediction.image_pred[1]);
	x265_image_clear(&enc_search->prediction.image_pred_temp);
	x265_image_clear(*pp_pred_image);
	if ( !b_use_res )
	{
		x265_simage_clear(*pp_resi_image);
	}
	x265_image_clear(*pp_reco_image);
	i_num_part = x265_data_cu_get_num_part_inter(cu);
	i_num_pred_dir = (P_SLICE == h->slice->i_slice_type) ? 1 : 2;
	i_part_size = x265_data_cu_get_partition_size_p2(cu, 0);
	bi_p_dist_temp = X265_MAX_INT;
#if X265_ZERO_MVD_EST
	i_zero_mvd_dir = -1;
#endif

	for ( i_part_idx = 0; i_part_idx < i_num_part; i_part_idx++ )
	{
		i_cost[0] = X265_MAX_UINT ;
		i_cost[1] = X265_MAX_UINT ;
		i_cost_bi = X265_MAX_UINT;
#if X265_ZERO_MVD_EST
		i_zero_mvd_cost = X265_MAX_UINT;
		i_zero_mvd_dist_temp = X265_MAX_UINT;
#endif
		best_bi_pdist = X265_MAX_INT;

		for ( i_num_ref=0; i_num_ref < X265_MAX_NUM_REF; i_num_ref++)
		{
			i_cost_temp_l0[i_num_ref] = X265_MAX_UINT;
		}

		x265_enc_search_x_get_blk_bits(enc_search,
									i_part_size,
									(P_SLICE == h->slice->i_slice_type),
									i_part_idx,
									i_last_mode,
									i_mb_bits);

		x265_data_cu_get_part_index_and_size(cu, i_part_idx, &i_part_addr, &i_roi_width, &i_roi_height );

#if X265_AMP_MRG
		b_test_normal_mc = 1;
		if ( b_use_mrg && x265_data_cu_get_width_p2(cu, 0) > 8 && i_num_part == 2 )
		{
			b_test_normal_mc = 0;
		}

		if (b_test_normal_mc)
		{
#endif
		//  uni-directional prediction
		for ( i_ref_list = 0; i_ref_list < i_num_pred_dir; i_ref_list++ )
		{
			i_ref_pic_list = ( i_ref_list ? REF_PIC_LIST_1 : REF_PIC_LIST_0 );

			for ( i_ref_idx_temp = 0;
					i_ref_idx_temp < h->i_ref[i_ref_pic_list]; i_ref_idx_temp++ )
			{
				i_bits_temp = i_mb_bits[i_ref_list];
				if ( h->i_ref[i_ref_pic_list] > 1 )
				{
					i_bits_temp += i_ref_idx_temp + 1;
					if ( i_ref_idx_temp == h->i_ref[i_ref_pic_list] - 1 )
					{
						i_bits_temp--;
					}
				}
#if X265_ZERO_MVD_EST
				x265_enc_search_x_estimate_mv_pred_amvp(h,
														enc_search,
														cu,
														p_org_image,
														i_part_idx,
														i_ref_pic_list,
														i_ref_idx_temp,
														&mv_pred[i_ref_list][i_ref_idx_temp],
														0,
														&bi_p_dist_temp,
														&i_zero_mvd_dist_temp);
#else
				x265_enc_search_x_estimate_mv_pred_amvp(h,
														enc_search,
														cu,
														p_org_image,
														i_part_idx,
														i_ref_pic_list,
														i_ref_idx_temp,
														&mv_pred[i_ref_list][i_ref_idx_temp],
														0,
														&bi_p_dist_temp);
#endif
				aai_mvp_idx[i_ref_list][i_ref_idx_temp] = x265_data_cu_get_mvp_idx_p3(cu, i_ref_pic_list, i_part_addr);
				aai_mvp_num[i_ref_list][i_ref_idx_temp] = x265_data_cu_get_mvp_num_p3(cu, i_ref_pic_list, i_part_addr);

				if(h->slice->b_l_mvd_l1_zero && i_ref_list==1 && bi_p_dist_temp < best_bi_pdist)
				{
					best_bi_pdist = bi_p_dist_temp;
					best_bi_pmvp_l1 = aai_mvp_idx[i_ref_list][i_ref_idx_temp];
					best_bi_pref_idx_l1 = i_ref_idx_temp;
				}

				i_bits_temp += enc_search->mvp_idx_cost[aai_mvp_idx[i_ref_list][i_ref_idx_temp]][X265_AMVP_MAX_NUM_CANDS];
#if X265_ZERO_MVD_EST
				if ((i_ref_list != 1 || (!h->slice->b_no_back_pred_flag)) &&
						(h->i_ref[REF_PIC_LIST_C] <= 0
						|| h->ref_idx_of_lc[i_ref_pic_list][i_ref_idx_temp] >= 0))
				{
					i_zero_mvd_bits_temp = i_bits_temp;
					i_zero_mvd_bits_temp += 2; //zero mvd bits

					x265_rd_cost_get_motion_cost(&h->rd_cost, 1, 0);
					i_zero_mvd_cost_temp = i_zero_mvd_dist_temp
											+ x265_rd_cost_get_cost_p2(&h->rd_cost, i_zero_mvd_bits_temp);

					if (i_zero_mvd_cost_temp < i_zero_mvd_cost)
					{
						i_zero_mvd_cost = i_zero_mvd_cost_temp;
						i_zero_mvd_dir = i_ref_list + 1;
						ai_zero_mvd_ref_idx[i_ref_list] = i_ref_idx_temp;
						ai_zero_mvd_mvp_idx[i_ref_list] = aai_mvp_idx[i_ref_list][i_ref_idx_temp];
						ai_zero_mvd_bits[i_ref_list] = i_zero_mvd_bits_temp;
					}
				}
#endif

#if X265_GPB_SIMPLE_UNI
				if ( h->i_ref[REF_PIC_LIST_C] > 0)
				{
					if ( i_ref_list
							&& ( h->slice->b_no_back_pred_flag
							|| (h->i_ref[REF_PIC_LIST_C] > 0
								&& (!h->slice->b_no_back_pred_flag)
								&& h->ref_idx_of_l0_from_ref_idx_of_l1[i_ref_idx_temp] >= 0)))
					{
						if ( h->slice->b_no_back_pred_flag )
						{
							mv_temp[1][i_ref_idx_temp] = mv_temp[0][i_ref_idx_temp];
							i_cost_temp = i_cost_temp_l0[i_ref_idx_temp];
							/*first subtract the bit-rate part of the cost of the other list*/
							i_cost_temp -= x265_rd_cost_get_cost_p2(&h->rd_cost, i_bits_temp_l0[i_ref_idx_temp] );
						}
						else
						{
							mv_temp[1][i_ref_idx_temp] = mv_temp[0][h->ref_idx_of_l0_from_ref_idx_of_l1[i_ref_idx_temp]];
							i_cost_temp = i_cost_temp_l0[h->ref_idx_of_l0_from_ref_idx_of_l1[i_ref_idx_temp]];
							/*first subtract the bit-rate part of the cost of the other list*/
							i_cost_temp -= x265_rd_cost_get_cost_p2(&h->rd_cost, i_bits_temp_l0[h->ref_idx_of_l0_from_ref_idx_of_l1[i_ref_idx_temp]] );
						}
						/*correct the bit-rate part of the current ref*/
						x265_rd_cost_set_predictor(&h->rd_cost, &mv_pred[i_ref_list][i_ref_idx_temp] );
						i_bits_temp += x265_rd_cost_get_bits(&h->rd_cost, mv_temp[1][i_ref_idx_temp].i_hor, mv_temp[1][i_ref_idx_temp].i_ver);
						/*calculate the correct cost*/
						i_cost_temp += x265_rd_cost_get_cost_p2(&h->rd_cost, i_bits_temp );
					}
					else
					{
						x265_enc_search_x_motion_estimation(h,
															enc_search,
															cu,
															p_org_image,
															i_part_idx,
															i_ref_pic_list,
															&mv_pred[i_ref_list][i_ref_idx_temp],
															i_ref_idx_temp,
															&mv_temp[i_ref_list][i_ref_idx_temp],
															&i_bits_temp,
															&i_cost_temp,
															0 );
					}
				}
				else
				{
					if (i_ref_list && h->slice->b_no_back_pred_flag)
					{
						i_cost_temp = X265_MAX_UINT;
						mv_temp[1][i_ref_idx_temp] = mv_temp[0][i_ref_idx_temp];
					}
					else
					{
						x265_enc_search_x_motion_estimation(h,
															enc_search,
															cu,
															p_org_image,
															i_part_idx,
															i_ref_pic_list,
															&mv_pred[i_ref_list][i_ref_idx_temp],
															i_ref_idx_temp,
															&mv_temp[i_ref_list][i_ref_idx_temp],
															&i_bits_temp,
															&i_cost_temp,
															0 );
					}
				}
#else
				x265_enc_search_x_motion_estimation(h,
													enc_search,
													cu,
													p_org_image,
													i_part_idx,
													i_ref_pic_list,
													&mv_pred[i_ref_list][i_ref_idx_temp],
													i_ref_idx_temp,
													&mv_temp[i_ref_list][i_ref_idx_temp],
													&i_bits_temp,
													&i_cost_temp,
													0 );
#endif
				x265_enc_search_x_copy_amvp_info(enc_search,
												&x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, i_ref_pic_list)->amvp_info,
												&aac_amvp_info[i_ref_list][i_ref_idx_temp]); // must always be done ( also when amvp_mode = am_none )
				x265_enc_search_x_check_best_mvp(h,
												enc_search,
												cu,
												i_ref_pic_list,
												&mv_temp[i_ref_list][i_ref_idx_temp],
												&mv_pred[i_ref_list][i_ref_idx_temp],
												&aai_mvp_idx[i_ref_list][i_ref_idx_temp],
												&i_bits_temp,
												&i_cost_temp);

				if(h->i_ref[REF_PIC_LIST_C] > 0 && (!h->slice->b_no_back_pred_flag))
				{
					if(i_ref_list==REF_PIC_LIST_0)
					{
						i_cost_temp_l0[i_ref_idx_temp] = i_cost_temp;
						i_bits_temp_l0[i_ref_idx_temp] = i_bits_temp;
						if(h->ref_idx_of_lc[REF_PIC_LIST_0][i_ref_idx_temp] < 0)
						{
							i_cost_temp = X265_MAX_UINT;
						}
					}
					else
					{
						if(h->ref_idx_of_lc[REF_PIC_LIST_1][i_ref_idx_temp] < 0)
						{
							i_cost_temp = X265_MAX_UINT;
						}
					}
				}

				if (( i_ref_list == 0 && i_cost_temp < i_cost[i_ref_list])
						|| (i_ref_list == 1
								&& h->slice->b_no_back_pred_flag
								&& i_ref_idx_temp == i_ref_idx[0]
								)
						|| (i_ref_list == 1
								&& (h->i_ref[REF_PIC_LIST_C] > 0)
								&& (i_ref_idx_temp == 0
									|| i_ref_idx_temp == i_ref_idx[0])
								&& (!h->slice->b_no_back_pred_flag)
								&& (i_ref_idx_temp == h->ref_idx_of_l0_from_ref_idx_of_l1[i_ref_idx_temp])
								)
						|| (i_ref_list == 1
								&& (!h->slice->b_no_back_pred_flag)
								&& i_cost_temp < i_cost[i_ref_list]
								)
						)
				{
					i_cost[i_ref_list] = i_cost_temp;
					i_bits[i_ref_list] = i_bits_temp; // storing for bi-prediction

					// set motion
					mv[i_ref_list]= mv_temp[i_ref_list][i_ref_idx_temp];
					i_ref_idx[i_ref_list] = i_ref_idx_temp;
					x265_cu_mv_field_set_all_mv(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, i_ref_pic_list),
												&mv[i_ref_list],
												i_part_size,
												i_part_addr,
												0, i_part_idx );
					x265_cu_mv_field_set_all_ref_idx(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, i_ref_pic_list),
												i_ref_idx[i_ref_list],
												i_part_size,
												i_part_addr,
												0,
												i_part_idx );

					if(!h->slice->b_l_mvd_l1_zero)
					{
						// storing list 1 prediction signal for iterative bi-directional prediction
						if ( i_ref_pic_list == REF_PIC_LIST_1 )
						{
							yuv_pred = &enc_search->prediction.image_pred[i_ref_list];
							x265_prediction_motion_compensation(h,
																(x265_prediction_t*)enc_search,
																cu,
																yuv_pred,
																i_ref_pic_list,
																i_part_idx );
						}
						if ((h->slice->b_no_back_pred_flag
								|| (h->i_ref[REF_PIC_LIST_C] > 0
									&& h->ref_idx_of_l0_from_ref_idx_of_l1[0] ==0
									)
								)
								&& i_ref_pic_list == REF_PIC_LIST_0 )
						{
							yuv_pred = &enc_search->prediction.image_pred[i_ref_list];
							x265_prediction_motion_compensation(h,
																(x265_prediction_t*)enc_search,
																cu,
																yuv_pred,
																i_ref_pic_list,
																i_part_idx );
						}
					}
				}
			}
		}
		//  bi-directional prediction
		if ((B_SLICE == h->slice->i_slice_type)
				&& (x265_data_cu_is_bipred_restriction(cu, i_part_idx) == 0) )
		{

			mv_bi[0] = mv[0];  mv_bi[1] = mv[1];
			i_ref_idx_bi[0] = i_ref_idx[0];    i_ref_idx_bi[1] = i_ref_idx[1];

			memcpy(mv_pred_bi, mv_pred, sizeof(mv_pred));
			memcpy(aai_mvp_idx_bi, aai_mvp_idx, sizeof(aai_mvp_idx));

			if(h->slice->b_l_mvd_l1_zero)
			{
				x265_enc_search_x_copy_amvp_info(enc_search,
												&aac_amvp_info[1][best_bi_pref_idx_l1],
												&x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_1)->amvp_info);
				x265_data_cu_set_mvp_idx_sub_parts(h,
													cu,
													best_bi_pmvp_l1,
													REF_PIC_LIST_1,
													i_part_addr,
													i_part_idx,
													x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));
				aai_mvp_idx_bi[1][best_bi_pref_idx_l1] = best_bi_pmvp_l1;
				memcpy(&mv_pred_bi[1][best_bi_pref_idx_l1],
						&x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_1)->amvp_info.mv_cand[best_bi_pmvp_l1],
						sizeof(x265_mv_t));

				memcpy(&mv_bi[1],
						&mv_pred_bi[1][best_bi_pref_idx_l1],
						sizeof(x265_mv_t));
				i_ref_idx_bi[1] = best_bi_pref_idx_l1;
				x265_cu_mv_field_set_all_mv(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_1),
											&mv_bi[1],
											i_part_size,
											i_part_addr,
											0,
											i_part_idx );
				x265_cu_mv_field_set_all_ref_idx(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_1),
												i_ref_idx_bi[1],
												i_part_size,
												i_part_addr,
												0,
												i_part_idx );
				yuv_pred = &enc_search->prediction.image_pred[1];
				x265_prediction_motion_compensation(h,
													(x265_prediction_t*)enc_search,
													cu,
													yuv_pred,
													REF_PIC_LIST_1,
													i_part_idx );

				i_mot_bits[0] = i_bits[0] - i_mb_bits[0];
				i_mot_bits[1] = i_mb_bits[1];

				if ( h->i_ref[REF_PIC_LIST_1] > 1 )
				{
					i_mot_bits[1] += best_bi_pref_idx_l1+1;
					if ( best_bi_pref_idx_l1 == h->i_ref[REF_PIC_LIST_1] - 1 )
					{
						i_mot_bits[1]--;
					}
				}

				i_mot_bits[1] += enc_search->mvp_idx_cost[aai_mvp_idx_bi[1][best_bi_pref_idx_l1]][X265_AMVP_MAX_NUM_CANDS];

				i_bits[2] = i_mb_bits[2] + i_mot_bits[0] + i_mot_bits[1];

				mv_temp[1][best_bi_pref_idx_l1] = mv_bi[1];
			}
			else
			{
				i_mot_bits[0] = i_bits[0] - i_mb_bits[0];
				i_mot_bits[1] = i_bits[1] - i_mb_bits[1];
				i_bits[2] = i_mb_bits[2] + i_mot_bits[0] + i_mot_bits[1];
			}

			// 4-times iteration (default)
			i_num_iter = 4;

			// fast encoder setting: only one iteration
			if (h->param.b_use_fast_enc || h->slice->b_l_mvd_l1_zero)
			{
				i_num_iter = 1;
			}

			for ( i_iter = 0; i_iter < i_num_iter; i_iter++ )
			{

				i_ref_list = i_iter % 2;
				if ( h->param.b_use_fast_enc
					&& (h->slice->b_no_back_pred_flag
						|| (h->i_ref[REF_PIC_LIST_C] > 0
							&& h->ref_idx_of_l0_from_ref_idx_of_l1[0] == 0)))
				{
					i_ref_list = 1;
				}
				i_ref_pic_list = ( i_ref_list ? REF_PIC_LIST_1 : REF_PIC_LIST_0 );

				if(h->slice->b_l_mvd_l1_zero)
				{
					i_ref_list = 0;
					i_ref_pic_list = REF_PIC_LIST_0;
				}

				b_changed = 0;

				i_ref_start = 0;
				i_ref_end = h->i_ref[i_ref_pic_list] - 1;

				for ( i_ref_idx_temp = i_ref_start; i_ref_idx_temp <= i_ref_end; i_ref_idx_temp++ )
				{
					i_bits_temp = i_mb_bits[2] + i_mot_bits[1-i_ref_list];
					if ( h->i_ref[i_ref_pic_list] > 1 )
					{
						i_bits_temp += i_ref_idx_temp+1;
						if ( i_ref_idx_temp == h->i_ref[i_ref_pic_list] - 1 )
						{
							i_bits_temp--;
						}
					}
					i_bits_temp += enc_search->mvp_idx_cost[aai_mvp_idx_bi[i_ref_list][i_ref_idx_temp]][X265_AMVP_MAX_NUM_CANDS];
					// call me
					x265_enc_search_x_motion_estimation(h,
														enc_search,
														cu,
														p_org_image,
														i_part_idx,
														i_ref_pic_list,
														&mv_pred_bi[i_ref_list][i_ref_idx_temp],
														i_ref_idx_temp,
														&mv_temp[i_ref_list][i_ref_idx_temp],
														&i_bits_temp,
														&i_cost_temp,
														1 );
					x265_enc_search_x_copy_amvp_info(enc_search,
													&aac_amvp_info[i_ref_list][i_ref_idx_temp],
													&x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, i_ref_pic_list)->amvp_info);
					x265_enc_search_x_check_best_mvp(h,
													enc_search,
													cu,
													i_ref_pic_list,
													&mv_temp[i_ref_list][i_ref_idx_temp],
													&mv_pred_bi[i_ref_list][i_ref_idx_temp],
													&aai_mvp_idx_bi[i_ref_list][i_ref_idx_temp],
													&i_bits_temp,
													&i_cost_temp);

					if ( i_cost_temp < i_cost_bi )
					{
						b_changed = 1;

						mv_bi[i_ref_list]= mv_temp[i_ref_list][i_ref_idx_temp];
						i_ref_idx_bi[i_ref_list] = i_ref_idx_temp;

						i_cost_bi  = i_cost_temp;
						i_mot_bits[i_ref_list] = i_bits_temp - i_mb_bits[2] - i_mot_bits[1-i_ref_list];
						i_bits[2] = i_bits_temp;

						if(i_num_iter!=1)
						{
							//  set motion
							x265_cu_mv_field_set_all_mv(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, i_ref_pic_list ),
														&mv_bi[i_ref_list],
														i_part_size,
														i_part_addr,
														0, i_part_idx );
							x265_cu_mv_field_set_all_ref_idx(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, i_ref_pic_list ),
															i_ref_idx_bi[i_ref_list],
															i_part_size,
															i_part_addr,
															0,
															i_part_idx );

							yuv_pred = &enc_search->prediction.image_pred[i_ref_list];
							x265_prediction_motion_compensation(h,
																(x265_prediction_t*)enc_search,
																cu,
																yuv_pred,
																i_ref_pic_list,
																i_part_idx );
						}
					}
				} // for loop-i_ref_idx_temp

				if ( !b_changed )
				{
					if ( i_cost_bi <= i_cost[0] && i_cost_bi <= i_cost[1] )
					{
						x265_enc_search_x_copy_amvp_info(enc_search,
														&aac_amvp_info[0][i_ref_idx_bi[0]],
														&x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_0)->amvp_info);
						x265_enc_search_x_check_best_mvp(h,
														enc_search,
														cu,
														REF_PIC_LIST_0,
														&mv_bi[0],
														&mv_pred_bi[0][i_ref_idx_bi[0]],
														&aai_mvp_idx_bi[0][i_ref_idx_bi[0]],
														&i_bits[2],
														&i_cost_bi);
						if(!h->slice->b_l_mvd_l1_zero)
						{
							x265_enc_search_x_copy_amvp_info(enc_search,
															&aac_amvp_info[1][i_ref_idx_bi[1]],
															&x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_1)->amvp_info);
							x265_enc_search_x_check_best_mvp(h,
															enc_search,
															cu,
															REF_PIC_LIST_1,
															&mv_bi[1],
															&mv_pred_bi[1][i_ref_idx_bi[1]],
															&aai_mvp_idx_bi[1][i_ref_idx_bi[1]],
															&i_bits[2],
															&i_cost_bi);
						}
					}
					break;
				}
			} // for loop-iter
		} // if (b_slice)
#if X265_ZERO_MVD_EST
		if ((B_SLICE == h->slice->i_slice_type)
			&& (x265_data_cu_is_bipred_restriction(cu, i_part_idx) == 0) )
		{
			x265_rd_cost_get_motion_cost(&h->rd_cost, 1, 0 );

			for ( i_l0ref_idx_temp = 0;
					i_l0ref_idx_temp <= h->i_ref[REF_PIC_LIST_0] - 1;
					i_l0ref_idx_temp++ )
			{
				for ( i_l1ref_idx_temp = 0;
						i_l1ref_idx_temp <= h->i_ref[REF_PIC_LIST_1] - 1;
						i_l1ref_idx_temp++ )
				{
					i_ref_idx_bits_temp = 0;
					if ( h->i_ref[REF_PIC_LIST_0] > 1 )
					{
						i_ref_idx_bits_temp += i_l0ref_idx_temp+1;
						if ( i_l0ref_idx_temp == h->i_ref[REF_PIC_LIST_0 ] - 1 )
						{
							i_ref_idx_bits_temp--;
						}
					}
					if ( h->i_ref[REF_PIC_LIST_1] > 1 )
					{
						i_ref_idx_bits_temp += i_l1ref_idx_temp+1;
						if ( i_l1ref_idx_temp == h->i_ref[REF_PIC_LIST_1] - 1 )
						{
							i_ref_idx_bits_temp--;
						}
					}
					i_l0mvpidx = 0;
					i_l1mvpidx = 0;

					for (i_l0mvpidx = 0; i_l0mvpidx < aai_mvp_num[0][i_l0ref_idx_temp]; i_l0mvpidx++)
					{
						for (i_l1mvpidx = 0; i_l1mvpidx < aai_mvp_num[1][i_l1ref_idx_temp]; i_l1mvpidx++)
						{
							i_zero_mvd_bits_temp = i_ref_idx_bits_temp;
							i_zero_mvd_bits_temp += i_mb_bits[2];
							i_zero_mvd_bits_temp += enc_search->mvp_idx_cost[i_l0mvpidx][aai_mvp_num[0][i_l0ref_idx_temp]]
							                      + enc_search->mvp_idx_cost[i_l1mvpidx][aai_mvp_num[1][i_l1ref_idx_temp]];
							i_zero_mvd_bits_temp += 4; //zero mvd for both directions
							x265_cu_mv_field_set_all_mv_field_p7(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_0),
																&aac_amvp_info[0][i_l0ref_idx_temp].mv_cand[i_l0mvpidx],
																i_l0ref_idx_temp,
																i_part_size,
																i_part_addr,
																i_part_idx,
																0 );
							x265_cu_mv_field_set_all_mv_field_p7(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_1),
																&aac_amvp_info[1][i_l1ref_idx_temp].mv_cand[i_l1mvpidx],
																i_l1ref_idx_temp,
																i_part_size,
																i_part_addr,
																i_part_idx,
																0 );

							x265_enc_search_x_get_inter_prediction_error(h,
																		enc_search,
																		cu,
																		p_org_image,
																		i_part_idx,
																		&i_zero_mvd_dist_temp,
																		h->param.b_use_had_me);
							i_zero_mvd_cost_temp = i_zero_mvd_dist_temp + x265_rd_cost_get_cost_p2(&h->rd_cost, i_zero_mvd_bits_temp);
							if (i_zero_mvd_cost_temp < i_zero_mvd_cost)
							{
								i_zero_mvd_cost = i_zero_mvd_cost_temp;
								i_zero_mvd_dir = 3;
								ai_zero_mvd_mvp_idx[0] = i_l0mvpidx;
								ai_zero_mvd_mvp_idx[1] = i_l1mvpidx;
								ai_zero_mvd_ref_idx[0] = i_l0ref_idx_temp;
								ai_zero_mvd_ref_idx[1] = i_l1ref_idx_temp;
								ai_zero_mvd_bits[2] = i_zero_mvd_bits_temp;
							}
						}
					}
				}
			}
		}
#endif

#if X265_AMP_MRG
		} //end if b_test_normal_mc
#endif
		//  clear motion field
		x265_cu_mv_field_set_all_mv_field_p6(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_0),
										&mv_field_zero,
										i_part_size,
										i_part_addr,
										0,
										i_part_idx );
		x265_cu_mv_field_set_all_mv_field_p6(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_1),
										&mv_field_zero,
										i_part_size,
										i_part_addr,
										0,
										i_part_idx );
		x265_cu_mv_field_set_all_mvd(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_0),
									&mv_zero,
									i_part_size,
									i_part_addr,
									0,
									i_part_idx );
		x265_cu_mv_field_set_all_mvd(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_1),
									&mv_zero,
									i_part_size,
									i_part_addr,
									0,
									i_part_idx );
		x265_data_cu_set_mvp_idx_sub_parts(h,
											cu,
											-1,
											REF_PIC_LIST_0,
											i_part_addr,
											i_part_idx,
											x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));
		x265_data_cu_set_mvp_num_sub_parts(h,
											cu,
											-1,
											REF_PIC_LIST_0,
											i_part_addr,
											i_part_idx,
											x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));
		x265_data_cu_set_mvp_idx_sub_parts(h,
											cu,
											-1,
											REF_PIC_LIST_1,
											i_part_addr,
											i_part_idx,
											x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));
		x265_data_cu_set_mvp_num_sub_parts(h,
											cu,
											-1,
											REF_PIC_LIST_1,
											i_part_addr,
											i_part_idx,
											x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));
		i_m_ebits = 0;
		// set motion field_
		if ( h->slice->b_no_back_pred_flag
			|| (h->i_ref[REF_PIC_LIST_C] > 0
				&& h->ref_idx_of_l0_from_ref_idx_of_l1[0]==0 ) )
		{
			i_cost[1] = X265_MAX_UINT;
		}
#if X265_AMP_MRG
		if (b_test_normal_mc)
		{
#endif
#if X265_ZERO_MVD_EST
			if (i_zero_mvd_cost <= i_cost_bi && i_zero_mvd_cost <= i_cost[0] && i_zero_mvd_cost <= i_cost[1])
			{
				if (i_zero_mvd_dir == 3)
				{
					i_last_mode = 2;

					x265_cu_mv_field_set_all_mv_field_p7(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_0),
														&aac_amvp_info[0][ai_zero_mvd_ref_idx[0]].mv_cand[ai_zero_mvd_mvp_idx[0]],
														ai_zero_mvd_ref_idx[0],
														i_part_size,
														i_part_addr,
														i_part_idx,
														0 );
					x265_cu_mv_field_set_all_mv_field_p7(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_1),
														&aac_amvp_info[1][ai_zero_mvd_ref_idx[1]].mv_cand[ai_zero_mvd_mvp_idx[1]],
														ai_zero_mvd_ref_idx[1],
														i_part_size,
														i_part_addr,
														i_part_idx,
														0 );

					x265_data_cu_set_inter_dir_sub_parts(h,
														cu,
														3,
														i_part_addr,
														i_part_idx,
														x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0));

					x265_data_cu_set_mvp_idx_sub_parts(h,
														cu,
														ai_zero_mvd_mvp_idx[0],
														REF_PIC_LIST_0,
														i_part_addr,
														i_part_idx,
														x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));
					x265_data_cu_set_mvp_num_sub_parts(h,
														cu,
														aai_mvp_num[0][ai_zero_mvd_ref_idx[0]],
														REF_PIC_LIST_0,
														i_part_addr,
														i_part_idx,
														x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));
					x265_data_cu_set_mvp_idx_sub_parts(h,
														cu,
														ai_zero_mvd_mvp_idx[1],
														REF_PIC_LIST_1,
														i_part_addr,
														i_part_idx,
														x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));
					x265_data_cu_set_mvp_num_sub_parts(h,
														cu,
														aai_mvp_num[1][ai_zero_mvd_ref_idx[1]],
														REF_PIC_LIST_1,
														i_part_addr,
														i_part_idx,
														x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));
					i_m_ebits = ai_zero_mvd_bits[2];
				}
				else if (i_zero_mvd_dir == 1)
				{
					i_last_mode = 0;

					x265_cu_mv_field_set_all_mv_field_p7(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_0),
														&aac_amvp_info[0][ai_zero_mvd_ref_idx[0]].mv_cand[ai_zero_mvd_mvp_idx[0]],
														ai_zero_mvd_ref_idx[0],
														i_part_size,
														i_part_addr,
														i_part_idx,
														0 );

					x265_data_cu_set_inter_dir_sub_parts(h,
														cu,
														1,
														i_part_addr,
														i_part_idx,
														x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0));

					x265_data_cu_set_mvp_idx_sub_parts(h,
														cu,
														ai_zero_mvd_mvp_idx[0],
														REF_PIC_LIST_0,
														i_part_addr,
														i_part_idx,
														x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0));
					x265_data_cu_set_mvp_num_sub_parts(h,
														cu,
														aai_mvp_num[0][ai_zero_mvd_ref_idx[0]],
														REF_PIC_LIST_0,
														i_part_addr,
														i_part_idx,
														x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0));
					i_m_ebits = ai_zero_mvd_bits[0];
				}
				else if (i_zero_mvd_dir == 2)
				{
					i_last_mode = 1;

					x265_cu_mv_field_set_all_mv_field_p7(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_1),
														&aac_amvp_info[1][ai_zero_mvd_ref_idx[1]].mv_cand[ai_zero_mvd_mvp_idx[1]],
														ai_zero_mvd_ref_idx[1],
														i_part_size,
														i_part_addr,
														i_part_idx,
														0 );

					x265_data_cu_set_inter_dir_sub_parts(h,
														cu,
														2,
														i_part_addr,
														i_part_idx,
														x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0));

					x265_data_cu_set_mvp_idx_sub_parts(h,
														cu,
														ai_zero_mvd_mvp_idx[1],
														REF_PIC_LIST_1,
														i_part_addr,
														i_part_idx,
														x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0));
					x265_data_cu_set_mvp_num_sub_parts(h,
														cu,
														aai_mvp_num[1][ai_zero_mvd_ref_idx[1]],
														REF_PIC_LIST_1,
														i_part_addr,
														i_part_idx,
														x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0));
					i_m_ebits = ai_zero_mvd_bits[1];
				}
				else
				{
					assert(0);
				}
			}
			else
#endif
			if ( i_cost_bi <= i_cost[0] && i_cost_bi <= i_cost[1])
			{
				i_last_mode = 2;
				{
					x265_cu_mv_field_set_all_mv(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_0),
																					&mv_bi[0],
																					i_part_size,
																					i_part_addr,
																					0,
																					i_part_idx );
					x265_cu_mv_field_set_all_ref_idx(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_0),
													i_ref_idx_bi[0],
													i_part_size,
													i_part_addr,
													0,
													i_part_idx );
					x265_cu_mv_field_set_all_mv(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_1),
																					&mv_bi[1],
																					i_part_size,
																					i_part_addr,
																					0,
																					i_part_idx );
					x265_cu_mv_field_set_all_ref_idx(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_1),
													i_ref_idx_bi[1],
													i_part_size,
													i_part_addr,
													0,
													i_part_idx );
				}
				{
					x265_mv_sub(&temp_mv, &mv_bi[0], &mv_pred_bi[0][i_ref_idx_bi[0]]) ;
					x265_cu_mv_field_set_all_mvd(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_0),
												&temp_mv,
												i_part_size,
												i_part_addr,
												0,
												i_part_idx );
				}
				{
					x265_mv_sub(&temp_mv, &mv_bi[1], &mv_pred_bi[1][i_ref_idx_bi[1]]) ;
					x265_cu_mv_field_set_all_mvd(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_1),
												&temp_mv,
												i_part_size,
												i_part_addr,
												0,
												i_part_idx );
				}

				x265_data_cu_set_inter_dir_sub_parts(h,
													cu,
													3,
													i_part_addr,
													i_part_idx,
													x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0));

				x265_data_cu_set_mvp_idx_sub_parts(h,
													cu,
													aai_mvp_idx_bi[0][i_ref_idx_bi[0]],
													REF_PIC_LIST_0,
													i_part_addr,
													i_part_idx,
													x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));
				x265_data_cu_set_mvp_num_sub_parts(h,
													cu,
													aai_mvp_num[0][i_ref_idx_bi[0]],
													REF_PIC_LIST_0,
													i_part_addr,
													i_part_idx,
													x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));
				x265_data_cu_set_mvp_idx_sub_parts(h,
													cu,
													aai_mvp_idx_bi[1][i_ref_idx_bi[1]],
													REF_PIC_LIST_1,
													i_part_addr,
													i_part_idx,
													x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));
				x265_data_cu_set_mvp_num_sub_parts(h,
													cu,
													aai_mvp_num[1][i_ref_idx_bi[1]],
													REF_PIC_LIST_1,
													i_part_addr,
													i_part_idx,
													x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));

				i_m_ebits = i_bits[2];
			}
			else if ( i_cost[0] <= i_cost[1] )
			{
				i_last_mode = 0;
				x265_cu_mv_field_set_all_mv(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_0),
											&mv[0],
											i_part_size,
											i_part_addr,
											0,
											i_part_idx );
				x265_cu_mv_field_set_all_ref_idx(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_0),
												i_ref_idx[0],
												i_part_size,
												i_part_addr,
												0,
												i_part_idx );
				{
					x265_mv_sub(&temp_mv, &mv[0], &mv_pred[0][i_ref_idx[0]]) ;
					x265_cu_mv_field_set_all_mvd(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_0),
												&temp_mv,
												i_part_size,
												i_part_addr,
												0,
												i_part_idx );
				}
				x265_data_cu_set_inter_dir_sub_parts(h,
													cu,
													1,
													i_part_addr,
													i_part_idx,
													x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));

				x265_data_cu_set_mvp_idx_sub_parts(h,
													cu,
													aai_mvp_idx[0][i_ref_idx[0]],
													REF_PIC_LIST_0,
													i_part_addr,
													i_part_idx,
													x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));
				x265_data_cu_set_mvp_num_sub_parts(h,
													cu,
													aai_mvp_num[0][i_ref_idx[0]],
													REF_PIC_LIST_0,
													i_part_addr,
													i_part_idx,
													x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));

				i_m_ebits = i_bits[0];
			}
			else
			{
				i_last_mode = 1;
				x265_cu_mv_field_set_all_mv(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_1),
											&mv[1],
											i_part_size,
											i_part_addr,
											0,
											i_part_idx );
				x265_cu_mv_field_set_all_ref_idx(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_1),
												i_ref_idx[1],
												i_part_size,
												i_part_addr,
												0,
												i_part_idx );
				{
					x265_mv_sub(&temp_mv, &mv[1], &mv_pred[1][i_ref_idx[1]]) ;
					x265_cu_mv_field_set_all_mvd(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_1),
												&temp_mv,
												i_part_size,
												i_part_addr,
												0,
												i_part_idx );
				}
				x265_data_cu_set_inter_dir_sub_parts(h,
													cu,
													2,
													i_part_addr,
													i_part_idx,
													x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, 0));

				x265_data_cu_set_mvp_idx_sub_parts(h,
													cu,
													aai_mvp_idx[1][i_ref_idx[1]],
													REF_PIC_LIST_1,
													i_part_addr,
													i_part_idx,
													x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));
				x265_data_cu_set_mvp_num_sub_parts(h,
													cu,
													aai_mvp_num[1][i_ref_idx[1]],
													REF_PIC_LIST_1,
													i_part_addr,
													i_part_idx,
													x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));

				i_m_ebits = i_bits[1];
			}
#if X265_AMP_MRG
		} // end if b_test_normal_mc
#endif

		if ( x265_data_cu_get_partition_size_p2(cu, i_part_addr ) != SIZE_2Nx2N )
		{
			i_m_rgindex = 0;
			i_m_e_inter_dir = 0;

			x265_rd_cost_get_motion_cost(&h->rd_cost, 1, 0 );
#if X265_AMP_MRG
			// calculate me cost
			i_m_eerror = X265_MAX_UINT;
			i_m_ecost = X265_MAX_UINT;

			if (b_test_normal_mc)
			{
				x265_enc_search_x_get_inter_prediction_error(h,
															enc_search,
															cu,
															p_org_image,
															i_part_idx,
															&i_m_eerror,
															h->param.b_use_had_me );
				i_m_ecost = i_m_eerror + x265_rd_cost_get_cost_p2(&h->rd_cost, i_m_ebits);
			}
#else
			// calculate me cost
			i_m_eerror = X265_MAX_UINT;
			x265_enc_search_x_get_inter_prediction_error(h,
														enc_search,
														cu,
														p_org_image,
														i_part_idx,
														&i_m_eerror,
														h->param.b_use_had_me);
			i_m_ecost = i_m_eerror + x265_rd_cost_get_cost_p2(&h->rd_cost, i_m_ebits );
#endif
			// save me result.
			i_m_e_inter_dir = x265_base_data_cu_get_inter_dir_p2((x265_base_data_cu_t*)cu, i_part_addr);
			x265_base_data_cu_get_mv_field((x265_base_data_cu_t*)cu,
											(x265_base_data_cu_t*)cu,
											i_part_addr,
											REF_PIC_LIST_0,
											&c_m_emv_field[0]);
			x265_base_data_cu_get_mv_field((x265_base_data_cu_t*)cu,
											(x265_base_data_cu_t*)cu,
											i_part_addr,
											REF_PIC_LIST_1,
											&c_m_emv_field[1] );

			// find merge result
			i_m_rgcost = X265_MAX_UINT;
			x265_enc_search_x_merge_estimation(h,
												enc_search,
												cu,
												p_org_image,
												i_part_idx,
												&i_m_rg_inter_dir,
												c_m_rgmv_field,
												&i_m_rgindex,
												&i_m_rgcost,
												mv_field_neighbours,
												uh_inter_dir_neighbours,
												&num_valid_merge_cand);
			if ( i_m_rgcost < i_m_ecost )
			{
				// set merge result
				x265_data_cu_set_merge_flag_sub_parts(h,
													cu,
													1,
													i_part_addr,
													i_part_idx,
													x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));
				x265_data_cu_set_merge_index_sub_parts(h,
														cu,
														i_m_rgindex,
														i_part_addr,
														i_part_idx,
														x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));
				x265_data_cu_set_inter_dir_sub_parts(h,
													cu,
													i_m_rg_inter_dir,
													i_part_addr,
													i_part_idx,
													x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));
				{
					x265_cu_mv_field_set_all_mv_field_p6(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_0),
													&c_m_rgmv_field[0],
													i_part_size,
													i_part_addr,
													0,
													i_part_idx );
					x265_cu_mv_field_set_all_mv_field_p6(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_1),
													&c_m_rgmv_field[1],
													i_part_size,
													i_part_addr,
													0,
													i_part_idx );
				}

				x265_cu_mv_field_set_all_mvd(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_0),
											&mv_zero,
											i_part_size,
											i_part_addr,
											0,
											i_part_idx );
				x265_cu_mv_field_set_all_mvd(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_1),
											&mv_zero,
											i_part_size,
											i_part_addr,
											0,
											i_part_idx );

				x265_data_cu_set_mvp_idx_sub_parts(h,
													cu,
													-1,
													REF_PIC_LIST_0,
													i_part_addr,
													i_part_idx,
													x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));
				x265_data_cu_set_mvp_num_sub_parts(h,
													cu,
													-1,
													REF_PIC_LIST_0,
													i_part_addr,
													i_part_idx,
													x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));
				x265_data_cu_set_mvp_idx_sub_parts(h,
													cu,
													-1,
													REF_PIC_LIST_1,
													i_part_addr,
													i_part_idx,
													x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));
				x265_data_cu_set_mvp_num_sub_parts(h,
													cu,
													-1,
													REF_PIC_LIST_1,
													i_part_addr,
													i_part_idx,
													x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));
			}
			else
			{
				// set me result
				x265_data_cu_set_merge_flag_sub_parts(h,
													cu,
													0,
													i_part_addr,
													i_part_idx,
													x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));
				x265_data_cu_set_inter_dir_sub_parts(h,
													cu,
													i_m_e_inter_dir,
													i_part_addr,
													i_part_idx,
													x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));
				{
					x265_cu_mv_field_set_all_mv_field_p6(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_0),
													&c_m_emv_field[0],
													i_part_size,
													i_part_addr,
													0,
													i_part_idx );
					x265_cu_mv_field_set_all_mv_field_p6(x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, REF_PIC_LIST_1),
													&c_m_emv_field[1],
													i_part_size,
													i_part_addr,
													0,
													i_part_idx );
				}
			}
		}

		//  mc
		x265_prediction_motion_compensation(h,
											(x265_prediction_t*)enc_search,
											cu,
											(*pp_pred_image),
											REF_PIC_LIST_X,
											i_part_idx );

	} //  end of for ( int32_t i_part_idx = 0; i_part_idx < i_num_part; i_part_idx++ )

	x265_enc_search_set_wp_scaling_dist_param(h, enc_search, cu, -1, REF_PIC_LIST_X );

	return;

}

// amvp
#if X265_ZERO_MVD_EST
void x265_enc_search_x_estimate_mv_pred_amvp(x265_t *h,
											x265_enc_search_t *enc_search,
											x265_data_cu_t *cu,
											x265_image_t *p_org_image,
											uint32_t i_part_idx,
											enum ref_pic_list_e i_ref_pic_list,
											int32_t i_ref_idx,
											x265_mv_t *p_mv_pred,
											int32_t b_filled,
											uint32_t *p_dist_bip,
											uint32_t *p_dist  )
#else
void x265_enc_search_x_estimate_mv_pred_amvp(x265_t *h,
											x265_enc_search_t *enc_search,
											x265_data_cu_t *cu,
											x265_image_t *p_org_image,
											uint32_t i_part_idx,
											enum ref_pic_list_e i_ref_pic_list,
											int32_t i_ref_idx,
											x265_mv_t *p_mv_pred,
											int32_t b_filled,
											uint32_t* p_dist_bip )
#endif
{
	int32_t loop = 0;
	x265_amvp_info_t *amvp_info = NULL;
	x265_mv_t c_best_mv;
	int32_t i_best_idx = 0;
	x265_mv_t c_zero_mv;
	uint32_t i_best_cost = 0;
	uint32_t i_part_addr = 0;
	int32_t i_roi_width, i_roi_height;

	i_best_cost = X265_MAX_INT;
	x265_mv_set_zero (&c_zero_mv) ;
	amvp_info = &x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, i_ref_pic_list)->amvp_info;
	x265_data_cu_get_part_index_and_size(cu,
										i_part_idx,
										&i_part_addr,
										&i_roi_width,
										&i_roi_height );
	// fill the mv candidates
	if (!b_filled)
	{
		x265_data_cu_fill_mvp_cand(h,
									cu,
									i_part_idx,
									i_part_addr,
									i_ref_pic_list,
									i_ref_idx,
									amvp_info );
	}

	// initialize mvp index & mvp
	i_best_idx = 0;
	memcpy(&c_best_mv, &amvp_info->mv_cand[0], sizeof(x265_mv_t)) ;
#if !X265_ZERO_MVD_EST
	if (amvp_info->i_n <= 1)
	{
		memcpy(p_mv_pred, &c_best_mv, sizeof(x265_mv_t)) ;

		x265_data_cu_set_mvp_idx_sub_parts(h,
											cu,
											i_best_idx,
											i_ref_pic_list,
											i_part_addr,
											i_part_idx,
											x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));
		x265_data_cu_set_mvp_num_sub_parts(h,
											cu,
											amvp_info->i_n,
											i_ref_pic_list,
											i_part_addr,
											i_part_idx,
											x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));

		if(h->slice->b_l_mvd_l1_zero && i_ref_pic_list==REF_PIC_LIST_1)
		{
#if X265_ZERO_MVD_EST
			(*p_dist_bip) = x265_enc_search_x_get_template_cost(h,
																enc_search,
																cu,
																i_part_idx,
																i_part_addr,
																p_org_image,
																&enc_search->prediction.image_pred_temp,
																p_mv_pred,
																0,
																X265_AMVP_MAX_NUM_CANDS,
																i_ref_pic_list,
																i_ref_idx,
																i_roi_width,
																i_roi_height,
																&i_dist );
#else
			(*p_dist_bip) = x265_enc_search_x_get_template_cost(h,
																enc_search,
																cu,
																i_part_idx,
																i_part_addr,
																p_org_image,
																&enc_search->prediction.image_pred_temp,
																p_mv_pred,
																0,
																X265_AMVP_MAX_NUM_CANDS,
																i_ref_pic_list,
																i_ref_idx,
																i_roi_width,
																i_roi_height);
#endif
		}
		return;
	}
#endif
	if (b_filled)
	{
		assert(x265_data_cu_get_mvp_idx_p3(cu, i_ref_pic_list,i_part_addr) >= 0);
		memcpy(p_mv_pred,
				&amvp_info->mv_cand[x265_data_cu_get_mvp_idx_p3(cu, i_ref_pic_list,i_part_addr)],
				sizeof(x265_mv_t)) ;
		return;
	}

	x265_image_clear(&enc_search->prediction.image_pred_temp);
#if X265_ZERO_MVD_EST
	uint32_t i_dist;
#endif
	//-- check minimum cost.
	for ( loop = 0 ; loop < amvp_info->i_n; loop++)
	{
		uint32_t i_tmp_cost;
#if X265_ZERO_MVD_EST
		i_tmp_cost = x265_enc_search_x_get_template_cost(h,
														enc_search,
														cu,
														i_part_idx,
														i_part_addr,
														p_org_image,
														&enc_search->prediction.image_pred_temp,
														&amvp_info->mv_cand[loop],
														loop,
														X265_AMVP_MAX_NUM_CANDS,
														i_ref_pic_list,
														i_ref_idx,
														i_roi_width,
														i_roi_height,
														&i_dist );
#else
		i_tmp_cost = x265_enc_search_x_get_template_cost(h,
														enc_search,
														cu,
														i_part_idx,
														i_part_addr,
														p_org_image,
														&enc_search->prediction.image_pred_temp,
														&amvp_info->mv_cand[loop],
														loop,
														X265_AMVP_MAX_NUM_CANDS,
														i_ref_pic_list,
														i_ref_idx,
														i_roi_width,
														i_roi_height);
#endif
		if ( i_best_cost > i_tmp_cost )
		{
			i_best_cost = i_tmp_cost;
			memcpy(&c_best_mv, &amvp_info->mv_cand[loop], sizeof(x265_mv_t)) ;
			i_best_idx  = loop;
			(*p_dist_bip) = i_tmp_cost;
#if X265_ZERO_MVD_EST
			(*p_dist) = i_dist;
#endif
		}
	}

	x265_image_clear(&enc_search->prediction.image_pred_temp);

	// setting best mvp
	memcpy(p_mv_pred, &c_best_mv, sizeof(x265_mv_t)) ;
	x265_data_cu_set_mvp_idx_sub_parts(h,
										cu,
										i_best_idx,
										i_ref_pic_list,
										i_part_addr,
										i_part_idx,
										x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));
	x265_data_cu_set_mvp_num_sub_parts(h,
										cu,
										amvp_info->i_n,
										i_ref_pic_list,
										i_part_addr,
										i_part_idx,
										x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_part_addr));
	return;
}

void x265_enc_search_x_get_blk_bits(x265_enc_search_t *enc_search,
									enum part_size_e i_part_size,
									int32_t b_p_slice,
									int32_t i_part_idx,
									uint32_t i_last_mode,
									uint32_t *p_blk_bit)
{
	if ( i_part_size == SIZE_2Nx2N )
	{
		p_blk_bit[0] = (! b_p_slice) ? 3 : 1;
		p_blk_bit[1] = 3;
		p_blk_bit[2] = 5;
	}
	else if((i_part_size == SIZE_2NxN || i_part_size == SIZE_2NxnU)
				|| i_part_size == SIZE_2NxnD )
	{
		uint32_t aai_mb_bits[2][3][3] = { { {0,0,3}, {0,0,0}, {0,0,0} } , { {5,7,7}, {7,5,7}, {9-3,9-3,9-3} } };
		if ( b_p_slice )
		{
			p_blk_bit[0] = 3;
			p_blk_bit[1] = 0;
			p_blk_bit[2] = 0;
		}
		else
		{
			memcpy( p_blk_bit, aai_mb_bits[i_part_idx][i_last_mode], 3*sizeof(uint32_t) );
		}
	}
	else if ((i_part_size == SIZE_Nx2N || i_part_size == SIZE_nLx2N)
				|| i_part_size == SIZE_nRx2N )
	{
		uint32_t aai_mb_bits[2][3][3] = { { {0,2,3}, {0,0,0}, {0,0,0} } , { {5,7,7}, {7-2,7-2,9-2}, {9-3,9-3,9-3} } };
		if ( b_p_slice )
		{
			p_blk_bit[0] = 3;
			p_blk_bit[1] = 0;
			p_blk_bit[2] = 0;
		}
		else
		{
			memcpy( p_blk_bit, aai_mb_bits[i_part_idx][i_last_mode], 3*sizeof(uint32_t) );
		}
	}
	else if ( i_part_size == SIZE_NxN )
	{
		p_blk_bit[0] = (! b_p_slice) ? 3 : 1;
		p_blk_bit[1] = 3;
		p_blk_bit[2] = 5;
	}
	else
	{
		printf("wrong!\n");
		assert( 0 );
	}
}

void x265_enc_search_x_copy_amvp_info(x265_enc_search_t *enc_search,
									x265_amvp_info_t *p_src,
									x265_amvp_info_t *p_dst)
{
	p_dst->i_n = p_src->i_n;
	for (int32_t i = 0; i < p_src->i_n; i++)
	{
		memcpy(&p_dst->mv_cand[i],
				&p_src->mv_cand[i],
				sizeof(x265_mv_t)) ;
	}
}

void x265_enc_search_x_check_best_mvp(x265_t *h,
									x265_enc_search_t *enc_search,
									x265_data_cu_t *cu,
									enum ref_pic_list_e
									e_ref_pic_list,
									x265_mv_t *p_mv,
									x265_mv_t *p_mv_pred,
									int32_t *p_mvp_idx,
									uint32_t *p_bits,
									uint32_t *p_cost )
{
	int32_t i_best_mvpidx = 0;
	int32_t i_org_mv_bits = 0;
	int32_t i_best_mv_bits = 0;
	int32_t i_mvp_idx = 0;
	int32_t i_mv_bits = 0;
	uint32_t i_org_bits = 0;
	x265_amvp_info_t *amvp_info = NULL;

	int mv_print = 0 ;

	mv_print = (p_mv->i_ver & 0xFFFF);
	mv_print = (mv_print << 16) ;
	mv_print = (mv_print | (p_mv->i_hor & 0xFFFF)) ;
	//	print_int_state (mv_print) ;

	amvp_info = &x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu, e_ref_pic_list)->amvp_info;

	assert(x265_mv_is_equal_mv(&amvp_info->mv_cand[*p_mvp_idx], p_mv_pred)) ;

	if (amvp_info->i_n < 2)
	{
		return;
	}

	x265_rd_cost_get_motion_cost(&h->rd_cost, 1, 0);
	h->rd_cost.i_cost_scale = 0;

	i_best_mvpidx = *p_mvp_idx;

	x265_rd_cost_set_predictor(&h->rd_cost, p_mv_pred);
	i_org_mv_bits  = x265_rd_cost_get_bits(&h->rd_cost, p_mv->i_hor, p_mv->i_ver );
	i_org_mv_bits += enc_search->mvp_idx_cost[*p_mvp_idx][X265_AMVP_MAX_NUM_CANDS];
	i_best_mv_bits = i_org_mv_bits;

	for (i_mvp_idx = 0; i_mvp_idx < amvp_info->i_n; i_mvp_idx++)
	{
		if (i_mvp_idx == (*p_mvp_idx))
		{
			continue;
		}

		x265_rd_cost_set_predictor(&h->rd_cost, &amvp_info->mv_cand[i_mvp_idx] );

		i_mv_bits = x265_rd_cost_get_bits(&h->rd_cost, p_mv->i_hor, p_mv->i_ver );;
		i_mv_bits += enc_search->mvp_idx_cost[i_mvp_idx][X265_AMVP_MAX_NUM_CANDS];

		if (i_mv_bits < i_best_mv_bits)
		{
			i_best_mv_bits = i_mv_bits;
			i_best_mvpidx = i_mvp_idx;
		}
	}

	if (i_best_mvpidx != (*p_mvp_idx))  //if changed
	{
		memcpy(p_mv_pred, &amvp_info->mv_cand[i_best_mvpidx], sizeof(x265_mv_t) );

		*p_mvp_idx = i_best_mvpidx;
		i_org_bits = *p_bits;
		*p_bits = i_org_bits - i_org_mv_bits + i_best_mv_bits;
		*p_cost = (*p_cost - x265_rd_cost_get_cost_p2(&h->rd_cost, i_org_bits))
					+ x265_rd_cost_get_cost_p2(&h->rd_cost, *p_bits );
	}
}



uint32_t x265_enc_search_x_get_template_cost(x265_t *h,
											x265_enc_search_t *enc_search,
											x265_data_cu_t *cu,
											uint32_t i_part_idx,
											uint32_t i_part_addr,
											x265_image_t *p_org_image,
											x265_image_t *template_cand,
											x265_mv_t *p_mv_cand,
											int32_t i_mvp_idx,
											int32_t i_mvp_num,
											enum ref_pic_list_e e_ref_pic_list,
											int32_t i_ref_idx,
											int32_t i_size_x,
											int32_t i_size_y
#if X265_ZERO_MVD_EST
											,uint32_t *p_dist
#endif
												)
{
	x265_mv_t mv_cand ;
	uint32_t i_cost  = X265_MAX_INT;
#if X265_ZERO_MVD_EST
	x265_dist_param_t dist_param;
#endif
	x265_simage_t *image_temp_cand = NULL ;

	memcpy (&mv_cand, p_mv_cand, sizeof(x265_mv_t)) ;
	x265_data_cu_clip_mv( h, cu, &mv_cand );

	// prediction pattern
	if ( h->pps[0].b_use_weight_pred && P_SLICE == h->slice->i_slice_type )
	{
		image_temp_cand = &enc_search->prediction.image_temp_cand ;
		x265_prediction_x_pred_inter_luma_blk_s(h,
											(x265_prediction_t*)enc_search,
											cu,
											h->fref[e_ref_pic_list][i_ref_idx],
											i_part_addr,
											&mv_cand,
											i_size_x,
											i_size_y,
											&image_temp_cand,
											1 );
	}
	else
	{
		x265_prediction_x_pred_inter_luma_blk_p(h,
											(x265_prediction_t*)enc_search,
											cu,
											h->fref[e_ref_pic_list][i_ref_idx],
											i_part_addr,
											&mv_cand,
											i_size_x,
											i_size_y,
											&template_cand,
											0 );
	}

	if ( h->pps[0].b_use_weight_pred && P_SLICE == h->slice->i_slice_type )
	{
		x265_weight_prediction_x_weighted_prediction_uni(h,
														(x265_weight_prediction_t*)enc_search,
														cu,
														&enc_search->prediction.image_temp_cand,
														i_part_addr,
														i_size_x,
														i_size_y,
														e_ref_pic_list,
														&template_cand,
														i_ref_idx );
	}

	// calc distortion
#if X265_ZERO_MVD_EST
	x265_rd_cost_get_motion_cost(&h->rd_cost, 1, 0 );
	x265_rd_cost_set_dist_param_p11(h,
									&h->rd_cost,
									&dist_param,
									h->cu.pic.i_bit_depth_y,
									x265_image_get_luma_addr_p3(h, p_org_image, i_part_addr),
									x265_image_get_stride(p_org_image),
									x265_image_get_luma_addr_p3(h, template_cand, i_part_addr),
									x265_image_get_stride(template_cand),
#if X265_NS_HAD
									i_size_x,
									i_size_y,
									h->param.b_use_had_me,
									h->param.b_use_nsqt() );
#else
									i_size_x,
									i_size_y,
									h->param.b_use_had_me );
#endif
	*p_dist = dist_param.dist_func(&h->rd_cost, &dist_param);
	i_cost = *p_dist + x265_rd_cost_get_cost_p2(&h->rd_cost, enc_search->mvp_idx_cost[i_mvp_idx][i_mvp_num]);
#else
#if X265_WEIGHTED_CHROMA_DISTORTION
	i_cost = x265_rd_cost_get_dist_part(h,
										&h->rd_cost,
										h->cu.pic.i_bit_depth_y,
										x265_image_get_luma_addr_p3(h, template_cand, i_part_addr),
										x265_image_get_stride(template_cand),
										x265_image_get_luma_addr_p3(h, p_org_image, i_part_addr),
										x265_image_get_stride(p_org_image),
										i_size_x,
										i_size_y,
										TEXT_LUMA,
										DF_SAD );
#else
	i_cost = x265_rd_cost_get_dist_part(h,
										&h->rd_cost,
										h->cu.pic.i_bit_depth_y,
										x265_image_get_luma_addr_p3(h, template_cand, i_part_addr),
										x265_image_get_stride(template_cand),
										x265_image_get_luma_addr_p3(h, p_org_image, i_part_addr),
										x265_image_get_stride(p_org_image),
										i_size_x,
										i_size_y,
										DF_SAD );
#endif
	i_cost = (uint32_t) x265_rd_cost_calc_rd_cost(&h->rd_cost,
												enc_search->mvp_idx_cost[i_mvp_idx][i_mvp_num],
												i_cost,
												0,
												DF_SAD );
#endif
	return i_cost;
}

