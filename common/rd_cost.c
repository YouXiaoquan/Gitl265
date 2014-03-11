

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

	x265_satd_initialize(&rd_cost->satd, cpu) ;
	rd_cost->i_cost = 0 ;
	rd_cost->i_cost_scale = 0 ;
	rd_cost->fp_distort_func[0] = NULL ;
	rd_cost->fp_distort_func[1] = x265_rd_cost_x_get_sse ;

	rd_cost->fp_distort_func[0]  = NULL;                  // for DF_DEFAULT

	rd_cost->fp_distort_func[1] = x265_rd_cost_x_get_sse ;
	rd_cost->fp_distort_func[1]  = x265_rd_cost_x_get_sse;
	rd_cost->fp_distort_func[2]  = x265_rd_cost_x_get_sse4;
	rd_cost->fp_distort_func[3]  = x265_rd_cost_x_get_sse8;
	rd_cost->fp_distort_func[4]  = x265_rd_cost_x_get_sse16;
	rd_cost->fp_distort_func[5]  = x265_rd_cost_x_get_sse32;
	rd_cost->fp_distort_func[6]  = x265_rd_cost_x_get_sse64;
	rd_cost->fp_distort_func[7]  = x265_rd_cost_x_get_sse16N;

	rd_cost->fp_distort_func[8]  = x265_rd_cost_x_get_sad;
	rd_cost->fp_distort_func[9]  = x265_rd_cost_x_get_sad4;
	rd_cost->fp_distort_func[10] = x265_rd_cost_x_get_sad8;
	rd_cost->fp_distort_func[11] = x265_rd_cost_x_get_sad16;
	rd_cost->fp_distort_func[12] = x265_rd_cost_x_get_sad32;
	rd_cost->fp_distort_func[13] = x265_rd_cost_x_get_sad64;
	rd_cost->fp_distort_func[14] = x265_rd_cost_x_get_sad16N;

	rd_cost->fp_distort_func[15] = x265_rd_cost_x_get_sad;
	rd_cost->fp_distort_func[16] = x265_rd_cost_x_get_sad4;
	rd_cost->fp_distort_func[17] = x265_rd_cost_x_get_sad8;
	rd_cost->fp_distort_func[18] = x265_rd_cost_x_get_sad16;
	rd_cost->fp_distort_func[19] = x265_rd_cost_x_get_sad32;
	rd_cost->fp_distort_func[20] = x265_rd_cost_x_get_sad64;
	rd_cost->fp_distort_func[21] = x265_rd_cost_x_get_sad16N;

#if X265_AMP_SAD
	rd_cost->fp_distort_func[43] = x265_rd_cost_x_get_sad12;
	rd_cost->fp_distort_func[44] = x265_rd_cost_x_get_sad24;
	rd_cost->fp_distort_func[45] = x265_rd_cost_x_get_sad48;

	rd_cost->fp_distort_func[46] = x265_rd_cost_x_get_sad12;
	rd_cost->fp_distort_func[47] = x265_rd_cost_x_get_sad24;
	rd_cost->fp_distort_func[48] = x265_rd_cost_x_get_sad48;
#endif

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


void print_rd_cost_offset ()
{
	FILE *file = stderr ;

	fprintf ( file, "%d\n", (uint32_t)&((x265_rd_cost_t*)0)->rd_cost_weight_prediction.i_w0 ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_rd_cost_t*)0)->rd_cost_weight_prediction.i_w1 ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_rd_cost_t*)0)->rd_cost_weight_prediction.i_shift ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_rd_cost_t*)0)->rd_cost_weight_prediction.i_offset ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_rd_cost_t*)0)->rd_cost_weight_prediction.i_round ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_rd_cost_t*)0)->rd_cost_weight_prediction.b_x_set_done ) ;

	fprintf ( file, "%d\n", sizeof(x265_rd_cost_t) ) ;
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

void x265_rd_cost_set_dist_param_p6_1( x265_t *h,
										x265_rd_cost_t *rd_cost,
										uint32_t i_blk_width,
										uint32_t i_blk_height,
										enum d_func_e d_func,
										x265_dist_param_t* dist_param )
{
	// set Block Width / Height
	dist_param->i_cols    = i_blk_width;
	dist_param->i_rows    = i_blk_height;
	dist_param->dist_func = rd_cost->fp_distort_func[d_func + h->global.convert_to_bit[ dist_param->i_cols ] + 1 ];

	// initialize
	dist_param->i_sub_shift  = 0;
}

// Setting the Distortion Parameter for int32_ter (ME)
void x265_rd_cost_set_dist_param_p6_2( x265_t *h,
										x265_rd_cost_t *rd_cost,
										x265_pattern_t* pattern_key,
										pixel* p_ref_y,
										int32_t i_ref_stride,
										x265_dist_param_t *dist_param )
{
	// set Original & Curr Pointer / Stride
	dist_param->org = x265_pattern_get_roi_y ( pattern_key ) ;
	dist_param->cur = p_ref_y;

	dist_param->i_stride_org = x265_pattern_get_pattern_l_stride ( pattern_key ) ;
	dist_param->i_stride_cur = i_ref_stride;

	// set Block Width / Height
	dist_param->i_cols = x265_pattern_get_roi_width ( pattern_key ) ;
	dist_param->i_rows = x265_pattern_get_roi_height ( pattern_key ) ;
	dist_param->dist_func = rd_cost->fp_distort_func[DF_SAD + h->global.convert_to_bit[ dist_param->i_cols ] + 1 ];

#if X265_AMP_SAD
	if (dist_param->i_cols == 12)
	{
		dist_param->dist_func = rd_cost->fp_distort_func[43 ];
	}
	else if (dist_param->i_cols == 24)
	{
		dist_param->dist_func = rd_cost->fp_distort_func[44 ];
	}
	else if (dist_param->i_cols == 48)
	{
		dist_param->dist_func = rd_cost->fp_distort_func[45 ];
	}
#endif

	// initialize
	dist_param->i_sub_shift  = 0;
}

// Setting the Distortion Parameter for int32_ter (subpel ME with step)
#if X265_NS_HAD
void x265_rd_cost_set_dist_param_p9 ( x265_t *h,
									x265_rd_cost_t *rd_cost,
									x265_pattern_t* pattern_key,
									pixel* p_ref_y,
									int32_t i_ref_stride,
									int32_t i_step,
									x265_dist_param_t *dist_param,
									int32_t b_use_ns_had )
#else
void x265_rd_cost_set_dist_param_p8( x265_t *h,
									x265_rd_cost_t *rd_cost,
									x265_pattern_t* pattern_key,
									pixel* p_ref_y,
									int32_t i_ref_stride,
									int32_t i_step,
									x265_dist_param_t *dist_param)
#endif
{
	// set Original & Curr Pointer / Stride
	dist_param->org = x265_pattern_get_roi_y ( pattern_key ) ;
	dist_param->cur = p_ref_y;

	dist_param->i_stride_org = x265_pattern_get_pattern_l_stride ( pattern_key ) ;
	dist_param->i_stride_cur = i_ref_stride * i_step;

	// set Step for interpolated buffer
	dist_param->i_step = i_step;

	// set Block Width / Height
	dist_param->i_cols = x265_pattern_get_roi_width ( pattern_key ) ;
	dist_param->i_rows = x265_pattern_get_roi_height ( pattern_key ) ;

#if X265_NS_HAD
	dist_param->b_use_ns_had = b_use_ns_had;
#endif

	// set distortion function
	dist_param->dist_func = rd_cost->fp_distort_func[DF_SADS + h->global.convert_to_bit[ dist_param->i_cols ] + 1 ];
#if X265_AMP_SAD
	if (dist_param->i_cols == 12)
	{
		dist_param->dist_func = rd_cost->fp_distort_func[46 ];
	}
	else if (dist_param->i_cols == 24)
	{
		dist_param->dist_func = rd_cost->fp_distort_func[47 ];
	}
	else if (dist_param->i_cols == 48)
	{
		dist_param->dist_func = rd_cost->fp_distort_func[48 ];
	}
#endif

	// initialize
	dist_param->i_sub_shift  = 0;
}

#if X265_NS_HAD
void x265_rd_cost_set_dist_param_p11( x265_t *h,
									x265_rd_cost_t *rd_cost,
									x265_dist_param_t* dist_param,
									pixel* p1,
									int32_t i_stride_1,
									pixel* p2,
									int32_t i_stride_2,
									int32_t i_width,
									int32_t i_height
									int32_t b_use_ns_had )
#else
void x265_rd_cost_set_dist_param_p11( x265_t *h,
									x265_rd_cost_t *rd_cost,
									x265_dist_param_t *dist_param,
									int32_t i_bit_depth,
									pixel* p1,
									int32_t i_stride_1,
									pixel* p2,
									int32_t i_stride_2,
									int32_t i_width,
									int32_t i_height )
#endif
{
  dist_param->org       = p1;
  dist_param->cur       = p2;
  dist_param->i_stride_org = i_stride_1;
  dist_param->i_stride_cur = i_stride_2;
  dist_param->i_cols = i_width;
  dist_param->i_rows = i_height;
  dist_param->i_step = 1;
  dist_param->i_sub_shift = 0;
  dist_param->i_bit_depth   = i_bit_depth;
  dist_param->dist_func   = rd_cost->fp_distort_func[ DF_SADS + h->global.convert_to_bit[ i_width ] + 1 ];
#if X265_NS_HAD
  dist_param->b_use_ns_had  = b_use_ns_had;
#endif
}

#if X265_WEIGHTED_CHROMA_DISTORTION
uint32_t x265_rd_cost_get_dist_part(x265_t *h,
									x265_rd_cost_t *rd_cost,
									int32_t i_bit_depth,
									pixel* cur,
									int32_t i_cur_stride,
									pixel* org,
									int32_t i_org_stride,
									uint32_t i_blk_width,
									uint32_t i_blk_height,
									enum text_type_e e_text,
									enum d_func_e d_func)
#else
uint32_t x265_rd_cost_get_dist_part(x265_t *h,
									x265_rd_cost_t *rd_cost,
									int32_t i_bit_depth,
									pixel* cur,
									int32_t i_cur_stride,
									pixel* org,
									int32_t i_org_stride,
									uint32_t i_blk_width,
									uint32_t i_blk_height,
									enum d_func_e d_func )
#endif
{
	x265_dist_param_t dist_param;

	x265_rd_cost_set_dist_param_p6_1( h, rd_cost,  i_blk_width, i_blk_height, d_func, &dist_param );
	dist_param.org = org;
	dist_param.cur = cur;
	dist_param.i_stride_org = i_org_stride;
	dist_param.i_stride_cur = i_cur_stride;
	dist_param.i_step = 1;

	dist_param.b_apply_weight = 0;
	dist_param.i_comp = 255;    // just for assert: to be sure it was set before use, since only values 0,1 or 2 are allowed.
	dist_param.i_bit_depth = i_bit_depth;

#if X265_WEIGHTED_CHROMA_DISTORTION
	if (e_text == TEXT_CHROMA_U)
	{
		return ((int32_t) (rd_cost->f_cb_distortion_weight
							* dist_param.dist_func( rd_cost, &dist_param )));
	}
	else if (e_text == TEXT_CHROMA_V)
	{
		return ((int32_t) (rd_cost->f_cr_distortion_weight
							* dist_param.dist_func( rd_cost, &dist_param )));
	}
	return dist_param.dist_func( rd_cost, &dist_param );
#else
	return dist_param.dist_func( rd_cost, &dist_param );
#endif
}



#if X265_RATE_CONTROL_LAMBDA_DOMAIN
uint32_t x265_rd_cost_get_sad_part(x265_rd_cost_t *rd_cost,
									int32_t i_bit_depth,
									pixel* cur,
									int32_t cur_stride,
									pixel* org,
									int32_t org_stride,
									uint32_t width,
									uint32_t height )
{
	uint32_t SAD = 0;
	int32_t shift = X265_DISTORTION_PRECISION_ADJUSTMENT(i_bit_depth-8);
	for ( int32_t i=0; i<height; i++ )
	{
		for( int32_t j=0; j<width; j++ )
		{
			SAD += abs((cur[j] - org[j])) >> shift;
		}
		cur = cur + cur_stride;
		org = org + org_stride;
	}
	return SAD;
}
#endif

uint32_t rd_cost_get_sse_s_s( x265_rd_cost_t *rd_cost,
								spixel* cur,
								int32_t i_cur_stride,
								spixel* org,
								int32_t i_org_stride,
								uint32_t i_blk_width,
								uint32_t i_blk_height )
{
	int32_t x = 0, y = 0 ;
	uint32_t i_sse = 0 ;
	int32_t i_temp = 0 ;

	for ( y = 0 ; y < i_blk_width ; ++ y )
	{
		for ( x = 0 ; x < i_blk_height ; ++ x )
		{
			i_temp = org[x] - cur[x] ;
			i_sse += (i_temp * i_temp) ;
		}
		cur += i_cur_stride ;
		org += i_org_stride ;
	}
	return i_sse ;
}

#if X265_WEIGHTED_CHROMA_DISTORTION
uint32_t x265_rd_cost_get_sse_s_s(x265_t *h,
									x265_rd_cost_t *rd_cost,
									int32_t i_bit_depth,
									spixel* cur,
									int32_t i_cur_stride,
									spixel* org,
									int32_t i_org_stride,
									uint32_t i_blk_width,
									uint32_t i_blk_height,
									enum text_type_e e_text,
									enum d_func_e d_func)
#else
uint32_t x265_rd_cost_get_sse_s_s(x265_t *h,
									x265_rd_cost_t *rd_cost,
									int32_t i_bit_depth,
									spixel* cur,
									int32_t i_cur_stride,
									spixel* org,
									int32_t i_org_stride,
									uint32_t i_blk_width,
									uint32_t i_blk_height,
									enum d_func_e d_func )
#endif
{

#if X265_WEIGHTED_CHROMA_DISTORTION
	if (e_text == TEXT_CHROMA_U)
	{
		return ((int32_t) (rd_cost->f_cb_distortion_weight
							* rd_cost_get_sse_s_s(rd_cost,
													cur,
													i_cur_stride,
													org,
													i_org_stride,
													i_blk_width,
													i_blk_height)
							)
				);
	}
	else if (e_text == TEXT_CHROMA_V)
	{
		return ((int32_t) (rd_cost->f_cr_distortion_weight
							* rd_cost_get_sse_s_s(rd_cost,
													cur,
													i_cur_stride,
													org,
													i_org_stride,
													i_blk_width,
													i_blk_height)
							)
				);
	}
	return rd_cost_get_sse_s_s(rd_cost,
								cur,
								i_cur_stride,
								org,
								i_org_stride,
								i_blk_width,
								i_blk_height);
#else
	return rd_cost_get_sse_s_s(rd_cost,
								cur,
								i_cur_stride,
								org,
								i_org_stride,
								i_blk_width,
								i_blk_height);
#endif
}

uint32_t rd_cost_get_sse_p_s( x265_rd_cost_t *rd_cost,
									pixel* cur,
									int32_t i_cur_stride,
									spixel* org,
									int32_t i_org_stride,
									uint32_t i_blk_width,
									uint32_t i_blk_height )
{
	int32_t x = 0, y = 0 ;
	uint32_t i_sse = 0 ;
	int32_t i_temp = 0 ;

	for ( y = 0 ; y < i_blk_width ; ++ y )
	{
		for ( x = 0 ; x < i_blk_height ; ++ x )
		{
			i_temp = org[x] - cur[x] ;
			i_sse += (i_temp * i_temp) ;
		}
		cur += i_cur_stride ;
		org += i_org_stride ;
	}
	return i_sse ;
}

#if X265_WEIGHTED_CHROMA_DISTORTION
uint32_t x265_rd_cost_get_sse_p_s(x265_t *h,
									x265_rd_cost_t *rd_cost,
									int32_t i_bit_depth,
									pixel* cur,
									int32_t i_cur_stride,
									spixel* org,
									int32_t i_org_stride,
									uint32_t i_blk_width,
									uint32_t i_blk_height,
									enum text_type_e e_text,
									enum d_func_e d_func)
#else
uint32_t x265_rd_cost_get_sse_p_s(x265_t *h,
									x265_rd_cost_t *rd_cost,
									int32_t i_bit_depth,
									pixel* cur,
									int32_t i_cur_stride,
									spixel* org,
									int32_t i_org_stride,
									uint32_t i_blk_width,
									uint32_t i_blk_height,
									enum d_func_e d_func )
#endif
{

#if X265_WEIGHTED_CHROMA_DISTORTION
	if (e_text == TEXT_CHROMA_U)
	{
		return ((int32_t) (rd_cost->f_cb_distortion_weight
							* rd_cost_get_sse_p_s(rd_cost,
													cur,
													i_cur_stride,
													org,
													i_org_stride,
													i_blk_width,
													i_blk_height)
							)
				);
	}
	else if (e_text == TEXT_CHROMA_V)
	{
		return ((int32_t) (rd_cost->f_cr_distortion_weight
							* rd_cost_get_sse_p_s(rd_cost,
													cur,
													i_cur_stride,
													org,
													i_org_stride,
													i_blk_width,
													i_blk_height)
							)
				);
	}
	return rd_cost_get_sse_p_s(rd_cost,
								cur,
								i_cur_stride,
								org,
								i_org_stride,
								i_blk_width,
								i_blk_height);
#else
	return rd_cost_get_sse_p_s(rd_cost,
								cur,
								i_cur_stride,
								org,
								i_org_stride,
								i_blk_width,
								i_blk_height);
#endif
}


uint32_t x265_rd_cost_x_get_sad( x265_rd_cost_t *rd_cost,x265_dist_param_t* dist_param)
{
  if ( dist_param->b_apply_weight )
  {
    return x_get_sadw ( (x265_rd_cost_weight_prediction_t*)rd_cost, dist_param );
  }
  pixel* i_org   = dist_param->org;
  pixel* i_cur   = dist_param->cur;
  int32_t  i_rows   = dist_param->i_rows;
  int32_t  i_cols   = dist_param->i_cols;
  int32_t  i_stride_cur = dist_param->i_stride_cur;
  int32_t  i_stride_org = dist_param->i_stride_org;

  uint32_t i_sum = 0;

  for( ; i_rows != 0; i_rows-- )
  {
    for (int32_t n = 0; n < i_cols; n++ )
    {
      i_sum += abs( i_org[n] - i_cur[n] );
    }
    i_org += i_stride_org;
    i_cur += i_stride_cur;
  }

  return i_sum >> X265_DISTORTION_PRECISION_ADJUSTMENT(dist_param->i_bit_depth-8);
}

uint32_t x265_rd_cost_x_get_sad4( x265_rd_cost_t *rd_cost, x265_dist_param_t* dist_param )
{
  if ( dist_param->b_apply_weight )
  {
    return x_get_sadw ( (x265_rd_cost_weight_prediction_t*)rd_cost, dist_param );
  }
  pixel* i_org   = dist_param->org;
  pixel* i_cur   = dist_param->cur;
  int32_t  i_rows   = dist_param->i_rows;
  int32_t  i_sub_shift  = dist_param->i_sub_shift;
  int32_t  i_sub_step   = ( 1 << i_sub_shift );
  int32_t  i_stride_cur = dist_param->i_stride_cur*i_sub_step;
  int32_t  i_stride_org = dist_param->i_stride_org*i_sub_step;

  uint32_t i_sum = 0;

  for( ; i_rows != 0; i_rows-=i_sub_step )
  {
    i_sum += abs( i_org[0] - i_cur[0] );
    i_sum += abs( i_org[1] - i_cur[1] );
    i_sum += abs( i_org[2] - i_cur[2] );
    i_sum += abs( i_org[3] - i_cur[3] );

    i_org += i_stride_org;
    i_cur += i_stride_cur;
  }

  i_sum <<= i_sub_shift;
  return i_sum >> X265_DISTORTION_PRECISION_ADJUSTMENT(dist_param->i_bit_depth-8);
}

uint32_t x265_rd_cost_x_get_sad8( x265_rd_cost_t *rd_cost, x265_dist_param_t* dist_param )
{
  if ( dist_param->b_apply_weight )
  {
    return x_get_sadw ( (x265_rd_cost_weight_prediction_t*)rd_cost, dist_param );
  }
  pixel* i_org      = dist_param->org;
  pixel* i_cur      = dist_param->cur;
  int32_t  i_rows      = dist_param->i_rows;
  int32_t  i_sub_shift  = dist_param->i_sub_shift;
  int32_t  i_sub_step   = ( 1 << i_sub_shift );
  int32_t  i_stride_cur = dist_param->i_stride_cur*i_sub_step;
  int32_t  i_stride_org = dist_param->i_stride_org*i_sub_step;

  uint32_t i_sum = 0;

  for( ; i_rows != 0; i_rows-=i_sub_step )
  {
    i_sum += abs( i_org[0] - i_cur[0] );
    i_sum += abs( i_org[1] - i_cur[1] );
    i_sum += abs( i_org[2] - i_cur[2] );
    i_sum += abs( i_org[3] - i_cur[3] );
    i_sum += abs( i_org[4] - i_cur[4] );
    i_sum += abs( i_org[5] - i_cur[5] );
    i_sum += abs( i_org[6] - i_cur[6] );
    i_sum += abs( i_org[7] - i_cur[7] );

    i_org += i_stride_org;
    i_cur += i_stride_cur;
  }

  i_sum <<= i_sub_shift;
  return i_sum >> X265_DISTORTION_PRECISION_ADJUSTMENT(dist_param->i_bit_depth-8);
}

uint32_t x265_rd_cost_x_get_sad16( x265_rd_cost_t *rd_cost, x265_dist_param_t* dist_param )
{
  if ( dist_param->b_apply_weight )
  {
    return x_get_sadw ( (x265_rd_cost_weight_prediction_t*)rd_cost, dist_param );
  }
  pixel* i_org   = dist_param->org;
  pixel* i_cur   = dist_param->cur;
  int32_t  i_rows   = dist_param->i_rows;
  int32_t  i_sub_shift  = dist_param->i_sub_shift;
  int32_t  i_sub_step   = ( 1 << i_sub_shift );
  int32_t  i_stride_cur = dist_param->i_stride_cur*i_sub_step;
  int32_t  i_stride_org = dist_param->i_stride_org*i_sub_step;

  uint32_t i_sum = 0;

  for( ; i_rows != 0; i_rows-=i_sub_step )
  {
    i_sum += abs( i_org[0] - i_cur[0] );
    i_sum += abs( i_org[1] - i_cur[1] );
    i_sum += abs( i_org[2] - i_cur[2] );
    i_sum += abs( i_org[3] - i_cur[3] );
    i_sum += abs( i_org[4] - i_cur[4] );
    i_sum += abs( i_org[5] - i_cur[5] );
    i_sum += abs( i_org[6] - i_cur[6] );
    i_sum += abs( i_org[7] - i_cur[7] );
    i_sum += abs( i_org[8] - i_cur[8] );
    i_sum += abs( i_org[9] - i_cur[9] );
    i_sum += abs( i_org[10] - i_cur[10] );
    i_sum += abs( i_org[11] - i_cur[11] );
    i_sum += abs( i_org[12] - i_cur[12] );
    i_sum += abs( i_org[13] - i_cur[13] );
    i_sum += abs( i_org[14] - i_cur[14] );
    i_sum += abs( i_org[15] - i_cur[15] );

    i_org += i_stride_org;
    i_cur += i_stride_cur;
  }

  i_sum <<= i_sub_shift;
  return i_sum >> X265_DISTORTION_PRECISION_ADJUSTMENT(dist_param->i_bit_depth-8);
}

#if X265_AMP_SAD
uint32_t x265_rd_cost_x_get_sad12( x265_rd_cost_t *rd_cost, x265_dist_param_t* dist_param )
{
  if ( dist_param->b_apply_weight )
  {
    return x_get_sadw ( (x265_rd_cost_weight_prediction_t*)rd_cost, dist_param );
  }
  pixel* i_org   = dist_param->org;
  pixel* i_cur   = dist_param->cur;
  int32_t  i_rows   = dist_param->i_rows;
  int32_t  i_sub_shift  = dist_param->i_sub_shift;
  int32_t  i_sub_step   = ( 1 << i_sub_shift );
  int32_t  i_stride_cur = dist_param->i_stride_cur*i_sub_step;
  int32_t  i_stride_org = dist_param->i_stride_org*i_sub_step;

  uint32_t i_sum = 0;

  for( ; i_rows != 0; i_rows-=i_sub_step )
  {
    i_sum += abs( i_org[0] - i_cur[0] );
    i_sum += abs( i_org[1] - i_cur[1] );
    i_sum += abs( i_org[2] - i_cur[2] );
    i_sum += abs( i_org[3] - i_cur[3] );
    i_sum += abs( i_org[4] - i_cur[4] );
    i_sum += abs( i_org[5] - i_cur[5] );
    i_sum += abs( i_org[6] - i_cur[6] );
    i_sum += abs( i_org[7] - i_cur[7] );
    i_sum += abs( i_org[8] - i_cur[8] );
    i_sum += abs( i_org[9] - i_cur[9] );
    i_sum += abs( i_org[10] - i_cur[10] );
    i_sum += abs( i_org[11] - i_cur[11] );

    i_org += i_stride_org;
    i_cur += i_stride_cur;
  }

  i_sum <<= i_sub_shift;
  return i_sum >> X265_DISTORTION_PRECISION_ADJUSTMENT(dist_param->i_bit_depth-8);
}
#endif

uint32_t x265_rd_cost_x_get_sad16N( x265_rd_cost_t *rd_cost, x265_dist_param_t* dist_param )
{
  pixel* i_org   = dist_param->org;
  pixel* i_cur   = dist_param->cur;
  int32_t  i_rows   = dist_param->i_rows;
  int32_t  i_cols   = dist_param->i_cols;
  int32_t  i_sub_shift  = dist_param->i_sub_shift;
  int32_t  i_sub_step   = ( 1 << i_sub_shift );
  int32_t  i_stride_cur = dist_param->i_stride_cur*i_sub_step;
  int32_t  i_stride_org = dist_param->i_stride_org*i_sub_step;

  uint32_t i_sum = 0;

  for( ; i_rows != 0; i_rows-=i_sub_step )
  {
    for (int32_t n = 0; n < i_cols; n+=16 )
    {
      i_sum += abs( i_org[n+ 0] - i_cur[n+ 0] );
      i_sum += abs( i_org[n+ 1] - i_cur[n+ 1] );
      i_sum += abs( i_org[n+ 2] - i_cur[n+ 2] );
      i_sum += abs( i_org[n+ 3] - i_cur[n+ 3] );
      i_sum += abs( i_org[n+ 4] - i_cur[n+ 4] );
      i_sum += abs( i_org[n+ 5] - i_cur[n+ 5] );
      i_sum += abs( i_org[n+ 6] - i_cur[n+ 6] );
      i_sum += abs( i_org[n+ 7] - i_cur[n+ 7] );
      i_sum += abs( i_org[n+ 8] - i_cur[n+ 8] );
      i_sum += abs( i_org[n+ 9] - i_cur[n+ 9] );
      i_sum += abs( i_org[n+10] - i_cur[n+10] );
      i_sum += abs( i_org[n+11] - i_cur[n+11] );
      i_sum += abs( i_org[n+12] - i_cur[n+12] );
      i_sum += abs( i_org[n+13] - i_cur[n+13] );
      i_sum += abs( i_org[n+14] - i_cur[n+14] );
      i_sum += abs( i_org[n+15] - i_cur[n+15] );
    }
    i_org += i_stride_org;
    i_cur += i_stride_cur;
  }

  i_sum <<= i_sub_shift;
  return i_sum >> X265_DISTORTION_PRECISION_ADJUSTMENT(dist_param->i_bit_depth-8);
}

uint32_t x265_rd_cost_x_get_sad32( x265_rd_cost_t *rd_cost, x265_dist_param_t* dist_param )
{
  if ( dist_param->b_apply_weight )
  {
    return x_get_sadw ( (x265_rd_cost_weight_prediction_t*)rd_cost, dist_param );
  }
  pixel* i_org   = dist_param->org;
  pixel* i_cur   = dist_param->cur;
  int32_t  i_rows   = dist_param->i_rows;
  int32_t  i_sub_shift  = dist_param->i_sub_shift;
  int32_t  i_sub_step   = ( 1 << i_sub_shift );
  int32_t  i_stride_cur = dist_param->i_stride_cur*i_sub_step;
  int32_t  i_stride_org = dist_param->i_stride_org*i_sub_step;

  uint32_t i_sum = 0;

  for( ; i_rows != 0; i_rows-=i_sub_step )
  {
    i_sum += abs( i_org[0] - i_cur[0] );
    i_sum += abs( i_org[1] - i_cur[1] );
    i_sum += abs( i_org[2] - i_cur[2] );
    i_sum += abs( i_org[3] - i_cur[3] );
    i_sum += abs( i_org[4] - i_cur[4] );
    i_sum += abs( i_org[5] - i_cur[5] );
    i_sum += abs( i_org[6] - i_cur[6] );
    i_sum += abs( i_org[7] - i_cur[7] );
    i_sum += abs( i_org[8] - i_cur[8] );
    i_sum += abs( i_org[9] - i_cur[9] );
    i_sum += abs( i_org[10] - i_cur[10] );
    i_sum += abs( i_org[11] - i_cur[11] );
    i_sum += abs( i_org[12] - i_cur[12] );
    i_sum += abs( i_org[13] - i_cur[13] );
    i_sum += abs( i_org[14] - i_cur[14] );
    i_sum += abs( i_org[15] - i_cur[15] );
    i_sum += abs( i_org[16] - i_cur[16] );
    i_sum += abs( i_org[17] - i_cur[17] );
    i_sum += abs( i_org[18] - i_cur[18] );
    i_sum += abs( i_org[19] - i_cur[19] );
    i_sum += abs( i_org[20] - i_cur[20] );
    i_sum += abs( i_org[21] - i_cur[21] );
    i_sum += abs( i_org[22] - i_cur[22] );
    i_sum += abs( i_org[23] - i_cur[23] );
    i_sum += abs( i_org[24] - i_cur[24] );
    i_sum += abs( i_org[25] - i_cur[25] );
    i_sum += abs( i_org[26] - i_cur[26] );
    i_sum += abs( i_org[27] - i_cur[27] );
    i_sum += abs( i_org[28] - i_cur[28] );
    i_sum += abs( i_org[29] - i_cur[29] );
    i_sum += abs( i_org[30] - i_cur[30] );
    i_sum += abs( i_org[31] - i_cur[31] );

    i_org += i_stride_org;
    i_cur += i_stride_cur;
  }

  i_sum <<= i_sub_shift;
  return i_sum >> X265_DISTORTION_PRECISION_ADJUSTMENT(dist_param->i_bit_depth-8);
}

#if X265_AMP_SAD
uint32_t x265_rd_cost_x_get_sad24( x265_rd_cost_t *rd_cost, x265_dist_param_t* dist_param )
{
  if ( dist_param->b_apply_weight )
  {
    return x_get_sadw ( (x265_rd_cost_weight_prediction_t*)rd_cost, dist_param );
  }
  pixel* i_org   = dist_param->org;
  pixel* i_cur   = dist_param->cur;
  int32_t  i_rows   = dist_param->i_rows;
  int32_t  i_sub_shift  = dist_param->i_sub_shift;
  int32_t  i_sub_step   = ( 1 << i_sub_shift );
  int32_t  i_stride_cur = dist_param->i_stride_cur*i_sub_step;
  int32_t  i_stride_org = dist_param->i_stride_org*i_sub_step;

  uint32_t i_sum = 0;

  for( ; i_rows != 0; i_rows-=i_sub_step )
  {
    i_sum += abs( i_org[0] - i_cur[0] );
    i_sum += abs( i_org[1] - i_cur[1] );
    i_sum += abs( i_org[2] - i_cur[2] );
    i_sum += abs( i_org[3] - i_cur[3] );
    i_sum += abs( i_org[4] - i_cur[4] );
    i_sum += abs( i_org[5] - i_cur[5] );
    i_sum += abs( i_org[6] - i_cur[6] );
    i_sum += abs( i_org[7] - i_cur[7] );
    i_sum += abs( i_org[8] - i_cur[8] );
    i_sum += abs( i_org[9] - i_cur[9] );
    i_sum += abs( i_org[10] - i_cur[10] );
    i_sum += abs( i_org[11] - i_cur[11] );
    i_sum += abs( i_org[12] - i_cur[12] );
    i_sum += abs( i_org[13] - i_cur[13] );
    i_sum += abs( i_org[14] - i_cur[14] );
    i_sum += abs( i_org[15] - i_cur[15] );
    i_sum += abs( i_org[16] - i_cur[16] );
    i_sum += abs( i_org[17] - i_cur[17] );
    i_sum += abs( i_org[18] - i_cur[18] );
    i_sum += abs( i_org[19] - i_cur[19] );
    i_sum += abs( i_org[20] - i_cur[20] );
    i_sum += abs( i_org[21] - i_cur[21] );
    i_sum += abs( i_org[22] - i_cur[22] );
    i_sum += abs( i_org[23] - i_cur[23] );

    i_org += i_stride_org;
    i_cur += i_stride_cur;
  }

  i_sum <<= i_sub_shift;
  return i_sum >> X265_DISTORTION_PRECISION_ADJUSTMENT(dist_param->i_bit_depth-8);
}

#endif

uint32_t x265_rd_cost_x_get_sad64( x265_rd_cost_t *rd_cost, x265_dist_param_t* dist_param )
{
  if ( dist_param->b_apply_weight )
  {
    return x_get_sadw ( (x265_rd_cost_weight_prediction_t*)rd_cost, dist_param );
  }
  pixel* i_org   = dist_param->org;
  pixel* i_cur   = dist_param->cur;
  int32_t  i_rows   = dist_param->i_rows;
  int32_t  i_sub_shift  = dist_param->i_sub_shift;
  int32_t  i_sub_step   = ( 1 << i_sub_shift );
  int32_t  i_stride_cur = dist_param->i_stride_cur*i_sub_step;
  int32_t  i_stride_org = dist_param->i_stride_org*i_sub_step;

  uint32_t i_sum = 0;

  for( ; i_rows != 0; i_rows-=i_sub_step )
  {
    i_sum += abs( i_org[0] - i_cur[0] );
    i_sum += abs( i_org[1] - i_cur[1] );
    i_sum += abs( i_org[2] - i_cur[2] );
    i_sum += abs( i_org[3] - i_cur[3] );
    i_sum += abs( i_org[4] - i_cur[4] );
    i_sum += abs( i_org[5] - i_cur[5] );
    i_sum += abs( i_org[6] - i_cur[6] );
    i_sum += abs( i_org[7] - i_cur[7] );
    i_sum += abs( i_org[8] - i_cur[8] );
    i_sum += abs( i_org[9] - i_cur[9] );
    i_sum += abs( i_org[10] - i_cur[10] );
    i_sum += abs( i_org[11] - i_cur[11] );
    i_sum += abs( i_org[12] - i_cur[12] );
    i_sum += abs( i_org[13] - i_cur[13] );
    i_sum += abs( i_org[14] - i_cur[14] );
    i_sum += abs( i_org[15] - i_cur[15] );
    i_sum += abs( i_org[16] - i_cur[16] );
    i_sum += abs( i_org[17] - i_cur[17] );
    i_sum += abs( i_org[18] - i_cur[18] );
    i_sum += abs( i_org[19] - i_cur[19] );
    i_sum += abs( i_org[20] - i_cur[20] );
    i_sum += abs( i_org[21] - i_cur[21] );
    i_sum += abs( i_org[22] - i_cur[22] );
    i_sum += abs( i_org[23] - i_cur[23] );
    i_sum += abs( i_org[24] - i_cur[24] );
    i_sum += abs( i_org[25] - i_cur[25] );
    i_sum += abs( i_org[26] - i_cur[26] );
    i_sum += abs( i_org[27] - i_cur[27] );
    i_sum += abs( i_org[28] - i_cur[28] );
    i_sum += abs( i_org[29] - i_cur[29] );
    i_sum += abs( i_org[30] - i_cur[30] );
    i_sum += abs( i_org[31] - i_cur[31] );
    i_sum += abs( i_org[32] - i_cur[32] );
    i_sum += abs( i_org[33] - i_cur[33] );
    i_sum += abs( i_org[34] - i_cur[34] );
    i_sum += abs( i_org[35] - i_cur[35] );
    i_sum += abs( i_org[36] - i_cur[36] );
    i_sum += abs( i_org[37] - i_cur[37] );
    i_sum += abs( i_org[38] - i_cur[38] );
    i_sum += abs( i_org[39] - i_cur[39] );
    i_sum += abs( i_org[40] - i_cur[40] );
    i_sum += abs( i_org[41] - i_cur[41] );
    i_sum += abs( i_org[42] - i_cur[42] );
    i_sum += abs( i_org[43] - i_cur[43] );
    i_sum += abs( i_org[44] - i_cur[44] );
    i_sum += abs( i_org[45] - i_cur[45] );
    i_sum += abs( i_org[46] - i_cur[46] );
    i_sum += abs( i_org[47] - i_cur[47] );
    i_sum += abs( i_org[48] - i_cur[48] );
    i_sum += abs( i_org[49] - i_cur[49] );
    i_sum += abs( i_org[50] - i_cur[50] );
    i_sum += abs( i_org[51] - i_cur[51] );
    i_sum += abs( i_org[52] - i_cur[52] );
    i_sum += abs( i_org[53] - i_cur[53] );
    i_sum += abs( i_org[54] - i_cur[54] );
    i_sum += abs( i_org[55] - i_cur[55] );
    i_sum += abs( i_org[56] - i_cur[56] );
    i_sum += abs( i_org[57] - i_cur[57] );
    i_sum += abs( i_org[58] - i_cur[58] );
    i_sum += abs( i_org[59] - i_cur[59] );
    i_sum += abs( i_org[60] - i_cur[60] );
    i_sum += abs( i_org[61] - i_cur[61] );
    i_sum += abs( i_org[62] - i_cur[62] );
    i_sum += abs( i_org[63] - i_cur[63] );

    i_org += i_stride_org;
    i_cur += i_stride_cur;
  }

  i_sum <<= i_sub_shift;
  return i_sum >> X265_DISTORTION_PRECISION_ADJUSTMENT(dist_param->i_bit_depth-8);
}

#if X265_AMP_SAD
uint32_t x265_rd_cost_x_get_sad48( x265_rd_cost_t *rd_cost,x265_dist_param_t* dist_param )
{
  if ( dist_param->b_apply_weight )
  {
    return x_get_sadw ( (x265_rd_cost_weight_prediction_t*)rd_cost, dist_param );
  }
  pixel* i_org   = dist_param->org;
  pixel* i_cur   = dist_param->cur;
  int32_t  i_rows   = dist_param->i_rows;
  int32_t  i_sub_shift  = dist_param->i_sub_shift;
  int32_t  i_sub_step   = ( 1 << i_sub_shift );
  int32_t  i_stride_cur = dist_param->i_stride_cur*i_sub_step;
  int32_t  i_stride_org = dist_param->i_stride_org*i_sub_step;

  uint32_t i_sum = 0;

  for( ; i_rows != 0; i_rows-=i_sub_step )
  {
    i_sum += abs( i_org[0] - i_cur[0] );
    i_sum += abs( i_org[1] - i_cur[1] );
    i_sum += abs( i_org[2] - i_cur[2] );
    i_sum += abs( i_org[3] - i_cur[3] );
    i_sum += abs( i_org[4] - i_cur[4] );
    i_sum += abs( i_org[5] - i_cur[5] );
    i_sum += abs( i_org[6] - i_cur[6] );
    i_sum += abs( i_org[7] - i_cur[7] );
    i_sum += abs( i_org[8] - i_cur[8] );
    i_sum += abs( i_org[9] - i_cur[9] );
    i_sum += abs( i_org[10] - i_cur[10] );
    i_sum += abs( i_org[11] - i_cur[11] );
    i_sum += abs( i_org[12] - i_cur[12] );
    i_sum += abs( i_org[13] - i_cur[13] );
    i_sum += abs( i_org[14] - i_cur[14] );
    i_sum += abs( i_org[15] - i_cur[15] );
    i_sum += abs( i_org[16] - i_cur[16] );
    i_sum += abs( i_org[17] - i_cur[17] );
    i_sum += abs( i_org[18] - i_cur[18] );
    i_sum += abs( i_org[19] - i_cur[19] );
    i_sum += abs( i_org[20] - i_cur[20] );
    i_sum += abs( i_org[21] - i_cur[21] );
    i_sum += abs( i_org[22] - i_cur[22] );
    i_sum += abs( i_org[23] - i_cur[23] );
    i_sum += abs( i_org[24] - i_cur[24] );
    i_sum += abs( i_org[25] - i_cur[25] );
    i_sum += abs( i_org[26] - i_cur[26] );
    i_sum += abs( i_org[27] - i_cur[27] );
    i_sum += abs( i_org[28] - i_cur[28] );
    i_sum += abs( i_org[29] - i_cur[29] );
    i_sum += abs( i_org[30] - i_cur[30] );
    i_sum += abs( i_org[31] - i_cur[31] );
    i_sum += abs( i_org[32] - i_cur[32] );
    i_sum += abs( i_org[33] - i_cur[33] );
    i_sum += abs( i_org[34] - i_cur[34] );
    i_sum += abs( i_org[35] - i_cur[35] );
    i_sum += abs( i_org[36] - i_cur[36] );
    i_sum += abs( i_org[37] - i_cur[37] );
    i_sum += abs( i_org[38] - i_cur[38] );
    i_sum += abs( i_org[39] - i_cur[39] );
    i_sum += abs( i_org[40] - i_cur[40] );
    i_sum += abs( i_org[41] - i_cur[41] );
    i_sum += abs( i_org[42] - i_cur[42] );
    i_sum += abs( i_org[43] - i_cur[43] );
    i_sum += abs( i_org[44] - i_cur[44] );
    i_sum += abs( i_org[45] - i_cur[45] );
    i_sum += abs( i_org[46] - i_cur[46] );
    i_sum += abs( i_org[47] - i_cur[47] );

    i_org += i_stride_org;
    i_cur += i_stride_cur;
  }

  i_sum <<= i_sub_shift;
  return i_sum >> X265_DISTORTION_PRECISION_ADJUSTMENT(dist_param->i_bit_depth-8);
}
#endif

// --------------------------------------------------------------------------------------------------------------------
// SSE
// --------------------------------------------------------------------------------------------------------------------

uint32_t x265_rd_cost_x_get_sse( x265_rd_cost_t *rd_cost,x265_dist_param_t* x265_dt_param )
{
  if ( x265_dt_param->b_apply_weight )
  {
    return x_get_ssew ( (x265_rd_cost_weight_prediction_t*)rd_cost, x265_dt_param );
  }
  pixel* i_org   = x265_dt_param->org;
  pixel* i_cur   = x265_dt_param->cur;
  int32_t  i_rows   = x265_dt_param->i_rows;
  int32_t  i_cols   = x265_dt_param->i_cols;
  int32_t  i_stride_org = x265_dt_param->i_stride_org;
  int32_t  i_stride_cur = x265_dt_param->i_stride_cur;

  uint32_t i_sum = 0;
  uint32_t uiShift = X265_DISTORTION_PRECISION_ADJUSTMENT((x265_dt_param->i_bit_depth-8) << 1);

  int32_t i_temp;

  for( ; i_rows != 0; i_rows-- )
  {
    for (int32_t n = 0; n < i_cols; n++ )
    {
      i_temp = i_org[n  ] - i_cur[n  ];
      i_sum += ( i_temp * i_temp ) >> uiShift;
    }
    i_org += i_stride_org;
    i_cur += i_stride_cur;
  }

  return ( i_sum );
}

uint32_t x265_rd_cost_x_get_sse4( x265_rd_cost_t *rd_cost,x265_dist_param_t* x265_dt_param )
{
  if ( x265_dt_param->b_apply_weight )
  {
    assert( x265_dt_param->i_cols == 4 );
    return x_get_ssew ( (x265_rd_cost_weight_prediction_t*)rd_cost, x265_dt_param );
  }
  pixel* i_org   = x265_dt_param->org;
  pixel* i_cur   = x265_dt_param->cur;
  int32_t  i_rows   = x265_dt_param->i_rows;
  int32_t  i_stride_org = x265_dt_param->i_stride_org;
  int32_t  i_stride_cur = x265_dt_param->i_stride_cur;

  uint32_t i_sum = 0;
  uint32_t uiShift = X265_DISTORTION_PRECISION_ADJUSTMENT((x265_dt_param->i_bit_depth-8) << 1);

  int32_t  i_temp;

  for( ; i_rows != 0; i_rows-- )
  {

    i_temp = i_org[0] - i_cur[0]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[1] - i_cur[1]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[2] - i_cur[2]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[3] - i_cur[3]; i_sum += ( i_temp * i_temp ) >> uiShift;

    i_org += i_stride_org;
    i_cur += i_stride_cur;
  }

  return ( i_sum );
}

uint32_t x265_rd_cost_x_get_sse8( x265_rd_cost_t *rd_cost,x265_dist_param_t* x265_dt_param )
{
  if ( x265_dt_param->b_apply_weight )
  {
    assert( x265_dt_param->i_cols == 8 );
    return x_get_ssew ( (x265_rd_cost_weight_prediction_t*)rd_cost, x265_dt_param );
  }
  pixel* i_org   = x265_dt_param->org;
  pixel* i_cur   = x265_dt_param->cur;
  int32_t  i_rows   = x265_dt_param->i_rows;
  int32_t  i_stride_org = x265_dt_param->i_stride_org;
  int32_t  i_stride_cur = x265_dt_param->i_stride_cur;

  uint32_t i_sum = 0;
  uint32_t uiShift = X265_DISTORTION_PRECISION_ADJUSTMENT((x265_dt_param->i_bit_depth-8) << 1);

  int32_t  i_temp;

  for( ; i_rows != 0; i_rows-- )
  {
    i_temp = i_org[0] - i_cur[0]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[1] - i_cur[1]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[2] - i_cur[2]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[3] - i_cur[3]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[4] - i_cur[4]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[5] - i_cur[5]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[6] - i_cur[6]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[7] - i_cur[7]; i_sum += ( i_temp * i_temp ) >> uiShift;

    i_org += i_stride_org;
    i_cur += i_stride_cur;
  }

  return ( i_sum );
}

uint32_t x265_rd_cost_x_get_sse16( x265_rd_cost_t *rd_cost,x265_dist_param_t* x265_dt_param )
{
  if ( x265_dt_param->b_apply_weight )
  {
    assert( x265_dt_param->i_cols == 16 );
    return x_get_ssew ( (x265_rd_cost_weight_prediction_t*)rd_cost, x265_dt_param );
  }
  pixel* i_org   = x265_dt_param->org;
  pixel* i_cur   = x265_dt_param->cur;
  int32_t  i_rows   = x265_dt_param->i_rows;
  int32_t  i_stride_org = x265_dt_param->i_stride_org;
  int32_t  i_stride_cur = x265_dt_param->i_stride_cur;

  uint32_t i_sum = 0;
  uint32_t uiShift = X265_DISTORTION_PRECISION_ADJUSTMENT((x265_dt_param->i_bit_depth-8) << 1);

  int32_t  i_temp;

  for( ; i_rows != 0; i_rows-- )
  {

    i_temp = i_org[ 0] - i_cur[ 0]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[ 1] - i_cur[ 1]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[ 2] - i_cur[ 2]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[ 3] - i_cur[ 3]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[ 4] - i_cur[ 4]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[ 5] - i_cur[ 5]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[ 6] - i_cur[ 6]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[ 7] - i_cur[ 7]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[ 8] - i_cur[ 8]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[ 9] - i_cur[ 9]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[10] - i_cur[10]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[11] - i_cur[11]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[12] - i_cur[12]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[13] - i_cur[13]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[14] - i_cur[14]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[15] - i_cur[15]; i_sum += ( i_temp * i_temp ) >> uiShift;

    i_org += i_stride_org;
    i_cur += i_stride_cur;
  }

  return ( i_sum );
}

uint32_t x265_rd_cost_x_get_sse16N( x265_rd_cost_t *rd_cost,x265_dist_param_t* x265_dt_param )
{
  if ( x265_dt_param->b_apply_weight )
  {
    return x_get_ssew ( (x265_rd_cost_weight_prediction_t*)rd_cost, x265_dt_param );
  }
  pixel* i_org   = x265_dt_param->org;
  pixel* i_cur   = x265_dt_param->cur;
  int32_t  i_rows   = x265_dt_param->i_rows;
  int32_t  i_cols   = x265_dt_param->i_cols;
  int32_t  i_stride_org = x265_dt_param->i_stride_org;
  int32_t  i_stride_cur = x265_dt_param->i_stride_cur;

  uint32_t i_sum = 0;
  uint32_t uiShift = X265_DISTORTION_PRECISION_ADJUSTMENT((x265_dt_param->i_bit_depth-8) << 1);
  int32_t  i_temp;

  for( ; i_rows != 0; i_rows-- )
  {
    for (int32_t n = 0; n < i_cols; n+=16 )
    {

      i_temp = i_org[n+ 0] - i_cur[n+ 0]; i_sum += ( i_temp * i_temp ) >> uiShift;
      i_temp = i_org[n+ 1] - i_cur[n+ 1]; i_sum += ( i_temp * i_temp ) >> uiShift;
      i_temp = i_org[n+ 2] - i_cur[n+ 2]; i_sum += ( i_temp * i_temp ) >> uiShift;
      i_temp = i_org[n+ 3] - i_cur[n+ 3]; i_sum += ( i_temp * i_temp ) >> uiShift;
      i_temp = i_org[n+ 4] - i_cur[n+ 4]; i_sum += ( i_temp * i_temp ) >> uiShift;
      i_temp = i_org[n+ 5] - i_cur[n+ 5]; i_sum += ( i_temp * i_temp ) >> uiShift;
      i_temp = i_org[n+ 6] - i_cur[n+ 6]; i_sum += ( i_temp * i_temp ) >> uiShift;
      i_temp = i_org[n+ 7] - i_cur[n+ 7]; i_sum += ( i_temp * i_temp ) >> uiShift;
      i_temp = i_org[n+ 8] - i_cur[n+ 8]; i_sum += ( i_temp * i_temp ) >> uiShift;
      i_temp = i_org[n+ 9] - i_cur[n+ 9]; i_sum += ( i_temp * i_temp ) >> uiShift;
      i_temp = i_org[n+10] - i_cur[n+10]; i_sum += ( i_temp * i_temp ) >> uiShift;
      i_temp = i_org[n+11] - i_cur[n+11]; i_sum += ( i_temp * i_temp ) >> uiShift;
      i_temp = i_org[n+12] - i_cur[n+12]; i_sum += ( i_temp * i_temp ) >> uiShift;
      i_temp = i_org[n+13] - i_cur[n+13]; i_sum += ( i_temp * i_temp ) >> uiShift;
      i_temp = i_org[n+14] - i_cur[n+14]; i_sum += ( i_temp * i_temp ) >> uiShift;
      i_temp = i_org[n+15] - i_cur[n+15]; i_sum += ( i_temp * i_temp ) >> uiShift;

    }
    i_org += i_stride_org;
    i_cur += i_stride_cur;
  }

  return ( i_sum );
}

uint32_t x265_rd_cost_x_get_sse32( x265_rd_cost_t *rd_cost,x265_dist_param_t* x265_dt_param )
{
  if ( x265_dt_param->b_apply_weight )
  {
    assert( x265_dt_param->i_cols == 32 );
    return x_get_ssew ( (x265_rd_cost_weight_prediction_t*)rd_cost, x265_dt_param );
  }
  pixel* i_org   = x265_dt_param->org;
  pixel* i_cur   = x265_dt_param->cur;
  int32_t  i_rows   = x265_dt_param->i_rows;
  int32_t  i_stride_org = x265_dt_param->i_stride_org;
  int32_t  i_stride_cur = x265_dt_param->i_stride_cur;

  uint32_t i_sum = 0;
  uint32_t uiShift = X265_DISTORTION_PRECISION_ADJUSTMENT((x265_dt_param->i_bit_depth-8) << 1);
  int32_t  i_temp;

  for( ; i_rows != 0; i_rows-- )
  {

    i_temp = i_org[ 0] - i_cur[ 0]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[ 1] - i_cur[ 1]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[ 2] - i_cur[ 2]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[ 3] - i_cur[ 3]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[ 4] - i_cur[ 4]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[ 5] - i_cur[ 5]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[ 6] - i_cur[ 6]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[ 7] - i_cur[ 7]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[ 8] - i_cur[ 8]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[ 9] - i_cur[ 9]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[10] - i_cur[10]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[11] - i_cur[11]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[12] - i_cur[12]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[13] - i_cur[13]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[14] - i_cur[14]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[15] - i_cur[15]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[16] - i_cur[16]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[17] - i_cur[17]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[18] - i_cur[18]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[19] - i_cur[19]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[20] - i_cur[20]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[21] - i_cur[21]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[22] - i_cur[22]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[23] - i_cur[23]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[24] - i_cur[24]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[25] - i_cur[25]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[26] - i_cur[26]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[27] - i_cur[27]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[28] - i_cur[28]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[29] - i_cur[29]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[30] - i_cur[30]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[31] - i_cur[31]; i_sum += ( i_temp * i_temp ) >> uiShift;

    i_org += i_stride_org;
    i_cur += i_stride_cur;
  }

  return ( i_sum );
}

uint32_t x265_rd_cost_x_get_sse64( x265_rd_cost_t *rd_cost,x265_dist_param_t* x265_dt_param )
{
  if ( x265_dt_param->b_apply_weight )
  {
    assert( x265_dt_param->i_cols == 64 );
    return x_get_ssew ( (x265_rd_cost_weight_prediction_t*)rd_cost, x265_dt_param );
  }
  pixel* i_org   = x265_dt_param->org;
  pixel* i_cur   = x265_dt_param->cur;
  int32_t  i_rows   = x265_dt_param->i_rows;
  int32_t  i_stride_org = x265_dt_param->i_stride_org;
  int32_t  i_stride_cur = x265_dt_param->i_stride_cur;

  uint32_t i_sum = 0;
  uint32_t uiShift = X265_DISTORTION_PRECISION_ADJUSTMENT((x265_dt_param->i_bit_depth-8) << 1);
  int32_t  i_temp;

  for( ; i_rows != 0; i_rows-- )
  {
    i_temp = i_org[ 0] - i_cur[ 0]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[ 1] - i_cur[ 1]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[ 2] - i_cur[ 2]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[ 3] - i_cur[ 3]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[ 4] - i_cur[ 4]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[ 5] - i_cur[ 5]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[ 6] - i_cur[ 6]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[ 7] - i_cur[ 7]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[ 8] - i_cur[ 8]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[ 9] - i_cur[ 9]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[10] - i_cur[10]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[11] - i_cur[11]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[12] - i_cur[12]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[13] - i_cur[13]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[14] - i_cur[14]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[15] - i_cur[15]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[16] - i_cur[16]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[17] - i_cur[17]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[18] - i_cur[18]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[19] - i_cur[19]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[20] - i_cur[20]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[21] - i_cur[21]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[22] - i_cur[22]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[23] - i_cur[23]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[24] - i_cur[24]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[25] - i_cur[25]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[26] - i_cur[26]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[27] - i_cur[27]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[28] - i_cur[28]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[29] - i_cur[29]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[30] - i_cur[30]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[31] - i_cur[31]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[32] - i_cur[32]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[33] - i_cur[33]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[34] - i_cur[34]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[35] - i_cur[35]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[36] - i_cur[36]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[37] - i_cur[37]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[38] - i_cur[38]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[39] - i_cur[39]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[40] - i_cur[40]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[41] - i_cur[41]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[42] - i_cur[42]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[43] - i_cur[43]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[44] - i_cur[44]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[45] - i_cur[45]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[46] - i_cur[46]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[47] - i_cur[47]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[48] - i_cur[48]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[49] - i_cur[49]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[50] - i_cur[50]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[51] - i_cur[51]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[52] - i_cur[52]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[53] - i_cur[53]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[54] - i_cur[54]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[55] - i_cur[55]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[56] - i_cur[56]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[57] - i_cur[57]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[58] - i_cur[58]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[59] - i_cur[59]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[60] - i_cur[60]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[61] - i_cur[61]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[62] - i_cur[62]; i_sum += ( i_temp * i_temp ) >> uiShift;
    i_temp = i_org[63] - i_cur[63]; i_sum += ( i_temp * i_temp ) >> uiShift;

    i_org += i_stride_org;
    i_cur += i_stride_cur;
  }

  return ( i_sum );
}
