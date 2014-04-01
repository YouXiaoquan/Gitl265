

#include "common.h"
#include "x86/rd_cost_hads_x86.h"

x265_dist_param_t *x265_dist_param_new ()
{
	x265_dist_param_t *dist_param = NULL ;

	CHECKED_MALLOCZERO( dist_param, sizeof(x265_dist_param_t) );
	if ( x265_dist_param_init ( dist_param ) )
	{
		goto fail ;
	}
	return dist_param ;
fail:
	x265_dist_param_delete ( (void*) dist_param ) ;
	return NULL ;
}

void x265_dist_param_delete ( x265_dist_param_t *dist_param )
{
	x265_free ( dist_param ) ;
}

int x265_dist_param_init ( x265_dist_param_t *dist_param )
{
	memset ( dist_param, 0, sizeof(x265_dist_param_t) );
	dist_param->i_step = 1 ;

	return 0 ;
}

void x265_dist_param_deinit ( x265_dist_param_t *dist_param )
{

}

void x265_rd_cost_get_motion_cost(x265_rd_cost_t *rd_cost,
									int32_t b_sad,
									int32_t i_add )
{
	rd_cost->i_cost = (b_sad ? rd_cost->i_lambda_motion_sad + i_add
							: rd_cost->i_lambda_motion_sse + i_add); }


x265_rd_cost_t *x265_rd_cost_new (int cpu)
{
	x265_rd_cost_t *rd_cost = NULL ;

	CHECKED_MALLOCZERO( rd_cost, sizeof(x265_rd_cost_t) );
	if ( x265_rd_cost_init ( rd_cost, cpu ) )
	{
		goto fail ;
	}
	return rd_cost ;
fail:
	x265_rd_cost_delete ( (void*) rd_cost ) ;
	return NULL ;
}

void x265_rd_cost_delete ( x265_rd_cost_t *rd_cost )
{
	x265_free ( rd_cost ) ;
}

int x265_rd_cost_init ( x265_rd_cost_t *rd_cost, int cpu )
{
	memset ( rd_cost, 0, sizeof(x265_rd_cost_t) );
	if ( x265_rd_cost_weight_prediction_init ((x265_rd_cost_weight_prediction_t*)rd_cost) )
	{
		goto fail ;
	}

	x265_sad_initialize(rd_cost->sads_func, rd_cost->sads_half_func, cpu) ;
	x265_satd_initialize(rd_cost->satd_func, cpu) ;
	x265_sse_initialize ( rd_cost->sse_p_p_func, rd_cost->sse_p_s_func, rd_cost->sse_s_s_func, cpu ) ;
	rd_cost->i_cost = 0 ;
	rd_cost->i_cost_scale = 0 ;
#if X265_FIX203
	x265_mv_init ( &rd_cost->predictor ) ;
#else
	rd_cost->component_cost_origin_p = NULL ;
	rd_cost->component_cost = NULL ;
	rd_cost->ver_cost = NULL ;
	rd_cost->hor_cost = NULL ;
	rd_cost->i_search_limit = 0xdeaddead ;
#endif

	return 0 ;

fail:
	return -1 ;
}

void x265_rd_cost_deinit ( x265_rd_cost_t *rd_cost )
{
#if !X265_FIX203
	if( NULL != rd_cost->component_cost_origin_p )
	{
		x265_free ( rd_cost->component_cost_origin_p ) ;
		rd_cost->component_cost_origin_p = NULL ;
	}
#endif

	x265_rd_cost_weight_prediction_deinit ((x265_rd_cost_weight_prediction_t*)rd_cost) ;
}

void rd_cost_set_lambda( x265_rd_cost_t *rd_cost, double f_ambda )
{
	rd_cost->f_lambda = f_ambda ;
	rd_cost->f_sqrt_lambda = sqrt ( rd_cost->f_lambda ) ;
	rd_cost->i_lambda_motion_sad = (uint32_t)floor(65536.0 * rd_cost->f_sqrt_lambda);
	rd_cost->i_lambda_motion_sse = (uint32_t)floor(65536.0 * rd_cost->f_lambda);
}

void x265_rd_cost_set_predictor( x265_rd_cost_t *rd_cost, x265_mv_t *mv )
{
#if X265_FIX203
	memcpy ( &rd_cost->predictor, mv, sizeof (x265_mv_t) ) ;
#else
	rd_cost->hor_cost = rd_cost->component_cost - mv->i_hor ;
	rd_cost->ver_cost = rd_cost->component_cost - mv->i_ver ;
#endif
}

uint32_t x265_rd_cost_get_cost_p2 ( x265_rd_cost_t *rd_cost, uint32_t i_bit )
{
	return ( rd_cost->i_cost * i_bit ) >> 16;
}


uint32_t x265_rd_cost_get_cost_p3 ( x265_rd_cost_t *rd_cost, int32_t x, int32_t y )
{
#if X265_FIX203
	return rd_cost->i_cost * x265_rd_cost_get_bits ( rd_cost, x, y) >> 16;
#else
	return (( rd_cost->i_cost *
			(rd_cost->hor_cost[ x * (1<<rd_cost->i_cost_scale) ]
			 + rd_cost->ver_cost[ y * (1<<rd_cost->i_cost_scale) ]) ) >> 16);
#endif
}



uint32_t x265_rd_cost_get_bits( x265_rd_cost_t *rd_cost, int32_t x, int32_t y )
{
#if X265_FIX203
	return rd_cost_x_get_component_bits(rd_cost,
										(x << rd_cost->i_cost_scale) - rd_cost->predictor.i_hor)
			+ rd_cost_x_get_component_bits(rd_cost,
										(y << rd_cost->i_cost_scale) - rd_cost->predictor.i_ver);
#else
	return rd_cost->hor_cost[ x * (1<<rd_cost->i_cost_scale)] + rd_cost->ver_cost[ y * (1<<rd_cost->i_cost_scale) ];
#endif
}



// Calculate RD functions
double x265_rd_cost_calc_rd_cost( x265_rd_cost_t *rd_cost, uint32_t i_bits, uint32_t i_distortion, int32_t b_flag, enum d_func_e d_func )
{
	double f_rd_cost = 0.0;
	double f_lambda = 0.0;

	switch ( d_func )
	{
	case DF_SSE:
		assert(0);
		break;
	case DF_SAD:
    	f_lambda = (double)rd_cost->i_lambda_motion_sad;
    	break;
	case DF_DEFAULT:
		f_lambda = rd_cost->f_lambda;
		break;
	case DF_SSE_FRAME:
		f_lambda = rd_cost->f_frame_lambda;
		break;
	default:
		assert (0);
		break;
	}

	if (b_flag)
	{
		// int32_tra8x8, int32_tra4x4 Block only...
#if X265_SEQUENCE_LEVEL_LOSSLESS
		f_rd_cost = (double)(i_bits);
#else
		f_rd_cost = (((double)i_distortion) + ((double)i_bits * f_lambda));
#endif
	}
	else
	{
		if (d_func == DF_SAD)
		{
			f_rd_cost = ((double)i_distortion + (double)((int32_t)(i_bits * f_lambda+.5)>>16));
			f_rd_cost = (double)(uint32_t)floor(f_rd_cost);
		}
		else
		{
#if X265_SEQUENCE_LEVEL_LOSSLESS
			f_rd_cost = (double)(i_bits);
#else
			f_rd_cost = ((double)i_distortion + (double)((int32_t)(i_bits * f_lambda+.5)));
			f_rd_cost = (double)(uint32_t)floor(f_rd_cost);
#endif
		}
	}

	return f_rd_cost;
}

double x265_rd_cost_calc_rd_cost_64( x265_rd_cost_t *rd_cost, uint64_t i_bits, uint64_t i_distortion, int32_t b_flag, enum d_func_e d_func )
{
	double f_rd_cost = 0.0;
	double f_lambda = 0.0;

	switch ( d_func )
	{
	case DF_SSE:
		assert(0);
		break;
	case DF_SAD:
		f_lambda = (double)rd_cost->i_lambda_motion_sad;
		break;
	case DF_DEFAULT:
		f_lambda = rd_cost->f_lambda;
		break;
	case DF_SSE_FRAME:
		f_lambda = rd_cost->f_frame_lambda;
		break;
	default:
		assert (0);
		break;
	}

	if (b_flag)
	{
    // int32_tra8x8, int32_tra4x4 Block only...
#if X265_SEQUENCE_LEVEL_LOSSLESS
		f_rd_cost = (double)(i_bits);
#else
		f_rd_cost = (((double)(int64_t)i_distortion) + ((double)(int64_t)i_bits * f_lambda));
#endif
	}
	else
	{
		if (d_func == DF_SAD)
		{
			f_rd_cost = ((double)(int64_t)i_distortion + (double)((int32_t)((int64_t)i_bits * f_lambda+.5)>>16));
			f_rd_cost = (double)(uint32_t)floor(f_rd_cost);
		}
		else
		{
#if X265_SEQUENCE_LEVEL_LOSSLESS
			f_rd_cost = (double)(i_bits);
#else
			f_rd_cost = ((double)(int64_t)i_distortion + (double)((int32_t)((int64_t)i_bits * f_lambda+.5)));
			f_rd_cost = (double)(uint32_t)floor(f_rd_cost);
#endif
		}
	}

	return f_rd_cost;
}






#if !X265_FIX203
void rd_cost_init_rate_distortion_model( x265_rd_cost_t *rd_cost, int32_t i_sub_pel_search_limit )
{
	// make it larger
	i_sub_pel_search_limit += 4;
	i_sub_pel_search_limit *= 8;

	if( rd_cost->i_search_limit != i_sub_pel_search_limit )
	{
		x265_rd_cost_deinit ( rd_cost ) ;

		rd_cost->i_search_limit = i_sub_pel_search_limit;
		NOCHECKED_MALLOCZERO ( rd_cost->component_cost_origin_p,
								(4 * i_sub_pel_search_limit) * sizeof(uint32_t) );
		i_sub_pel_search_limit *= 2;

		rd_cost->component_cost = rd_cost->component_cost_origin_p + i_sub_pel_search_limit;

		for( int32_t n = -i_sub_pel_search_limit; n < i_sub_pel_search_limit; n++)
		{
			rd_cost->component_cost[n] = rd_cost_x_get_component_bits ( rd_cost, n );
		}
	}
}

#endif

uint32_t rd_cost_x_get_component_bits( x265_rd_cost_t *rd_cost, int32_t i_val )
{
  uint32_t i_length = 1;
  uint32_t i_temp   = ( i_val <= 0) ? (-i_val<<1)+1: (i_val<<1);

  assert ( i_temp );

  while ( 1 != i_temp )
  {
    i_temp >>= 1;
    i_length += 2;
  }

  return i_length;
}

