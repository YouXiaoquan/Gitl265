

#include "common.h"


#define X265_RDOQ_CHROMA                 1           ///< use of RDOQ in chroma

int32_t inv_quant_scales[6] =
{
	40,45,51,57,64,72
};

int32_t quant_scales[6] =
{
	26214,23302,20560,18396,16384,14564
};

int32_t et_table[4] = {0,3,1,2};

const int16_t t4[4][4] =
{
	{ 64, 64, 64, 64},
	{ 83, 36,-36,-83},
	{ 64,-64,-64, 64},
	{ 36,-83, 83,-36}
};

const int16_t t8[8][8] =
{
	{ 64, 64, 64, 64, 64, 64, 64, 64},
	{ 89, 75, 50, 18,-18,-50,-75,-89},
	{ 83, 36,-36,-83,-83,-36, 36, 83},
	{ 75,-18,-89,-50, 50, 89, 18,-75},
	{ 64,-64,-64, 64, 64,-64,-64, 64},
	{ 50,-89, 18, 75,-75,-18, 89,-50},
	{ 36,-83, 83,-36,-36, 83,-83, 36},
	{ 18,-50, 75,-89, 89,-75, 50,-18}
};

const int16_t t16[16][16] =
{
	{ 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64},
	{ 90, 87, 80, 70, 57, 43, 25,  9, -9,-25,-43,-57,-70,-80,-87,-90},
	{ 89, 75, 50, 18,-18,-50,-75,-89,-89,-75,-50,-18, 18, 50, 75, 89},
	{ 87, 57,  9,-43,-80,-90,-70,-25, 25, 70, 90, 80, 43, -9,-57,-87},
	{ 83, 36,-36,-83,-83,-36, 36, 83, 83, 36,-36,-83,-83,-36, 36, 83},
	{ 80,  9,-70,-87,-25, 57, 90, 43,-43,-90,-57, 25, 87, 70, -9,-80},
	{ 75,-18,-89,-50, 50, 89, 18,-75,-75, 18, 89, 50,-50,-89,-18, 75},
	{ 70,-43,-87,  9, 90, 25,-80,-57, 57, 80,-25,-90, -9, 87, 43,-70},
	{ 64,-64,-64, 64, 64,-64,-64, 64, 64,-64,-64, 64, 64,-64,-64, 64},
	{ 57,-80,-25, 90, -9,-87, 43, 70,-70,-43, 87,  9,-90, 25, 80,-57},
	{ 50,-89, 18, 75,-75,-18, 89,-50,-50, 89,-18,-75, 75, 18,-89, 50},
	{ 43,-90, 57, 25,-87, 70,  9,-80, 80, -9,-70, 87,-25,-57, 90,-43},
	{ 36,-83, 83,-36,-36, 83,-83, 36, 36,-83, 83,-36,-36, 83,-83, 36},
	{ 25,-70, 90,-80, 43,  9,-57, 87,-87, 57, -9,-43, 80,-90, 70,-25},
	{ 18,-50, 75,-89, 89,-75, 50,-18,-18, 50,-75, 89,-89, 75,-50, 18},
	{  9,-25, 43,-57, 70,-80, 87,-90, 90,-87, 80,-70, 57,-43, 25, -9}
};

const int16_t t32[32][32] =
{
	{ 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64},
	{ 90, 90, 88, 85, 82, 78, 73, 67, 61, 54, 46, 38, 31, 22, 13,  4, -4,-13,-22,-31,-38,-46,-54,-61,-67,-73,-78,-82,-85,-88,-90,-90},
	{ 90, 87, 80, 70, 57, 43, 25,  9, -9,-25,-43,-57,-70,-80,-87,-90,-90,-87,-80,-70,-57,-43,-25, -9,  9, 25, 43, 57, 70, 80, 87, 90},
	{ 90, 82, 67, 46, 22, -4,-31,-54,-73,-85,-90,-88,-78,-61,-38,-13, 13, 38, 61, 78, 88, 90, 85, 73, 54, 31,  4,-22,-46,-67,-82,-90},
	{ 89, 75, 50, 18,-18,-50,-75,-89,-89,-75,-50,-18, 18, 50, 75, 89, 89, 75, 50, 18,-18,-50,-75,-89,-89,-75,-50,-18, 18, 50, 75, 89},
	{ 88, 67, 31,-13,-54,-82,-90,-78,-46, -4, 38, 73, 90, 85, 61, 22,-22,-61,-85,-90,-73,-38,  4, 46, 78, 90, 82, 54, 13,-31,-67,-88},
	{ 87, 57,  9,-43,-80,-90,-70,-25, 25, 70, 90, 80, 43, -9,-57,-87,-87,-57, -9, 43, 80, 90, 70, 25,-25,-70,-90,-80,-43,  9, 57, 87},
	{ 85, 46,-13,-67,-90,-73,-22, 38, 82, 88, 54, -4,-61,-90,-78,-31, 31, 78, 90, 61,  4,-54,-88,-82,-38, 22, 73, 90, 67, 13,-46,-85},
	{ 83, 36,-36,-83,-83,-36, 36, 83, 83, 36,-36,-83,-83,-36, 36, 83, 83, 36,-36,-83,-83,-36, 36, 83, 83, 36,-36,-83,-83,-36, 36, 83},
	{ 82, 22,-54,-90,-61, 13, 78, 85, 31,-46,-90,-67,  4, 73, 88, 38,-38,-88,-73, -4, 67, 90, 46,-31,-85,-78,-13, 61, 90, 54,-22,-82},
	{ 80,  9,-70,-87,-25, 57, 90, 43,-43,-90,-57, 25, 87, 70, -9,-80,-80, -9, 70, 87, 25,-57,-90,-43, 43, 90, 57,-25,-87,-70,  9, 80},
	{ 78, -4,-82,-73, 13, 85, 67,-22,-88,-61, 31, 90, 54,-38,-90,-46, 46, 90, 38,-54,-90,-31, 61, 88, 22,-67,-85,-13, 73, 82,  4,-78},
	{ 75,-18,-89,-50, 50, 89, 18,-75,-75, 18, 89, 50,-50,-89,-18, 75, 75,-18,-89,-50, 50, 89, 18,-75,-75, 18, 89, 50,-50,-89,-18, 75},
	{ 73,-31,-90,-22, 78, 67,-38,-90,-13, 82, 61,-46,-88, -4, 85, 54,-54,-85,  4, 88, 46,-61,-82, 13, 90, 38,-67,-78, 22, 90, 31,-73},
	{ 70,-43,-87,  9, 90, 25,-80,-57, 57, 80,-25,-90, -9, 87, 43,-70,-70, 43, 87, -9,-90,-25, 80, 57,-57,-80, 25, 90,  9,-87,-43, 70},
	{ 67,-54,-78, 38, 85,-22,-90,  4, 90, 13,-88,-31, 82, 46,-73,-61, 61, 73,-46,-82, 31, 88,-13,-90, -4, 90, 22,-85,-38, 78, 54,-67},
	{ 64,-64,-64, 64, 64,-64,-64, 64, 64,-64,-64, 64, 64,-64,-64, 64, 64,-64,-64, 64, 64,-64,-64, 64, 64,-64,-64, 64, 64,-64,-64, 64},
	{ 61,-73,-46, 82, 31,-88,-13, 90, -4,-90, 22, 85,-38,-78, 54, 67,-67,-54, 78, 38,-85,-22, 90,  4,-90, 13, 88,-31,-82, 46, 73,-61},
	{ 57,-80,-25, 90, -9,-87, 43, 70,-70,-43, 87,  9,-90, 25, 80,-57,-57, 80, 25,-90,  9, 87,-43,-70, 70, 43,-87, -9, 90,-25,-80, 57},
	{ 54,-85, -4, 88,-46,-61, 82, 13,-90, 38, 67,-78,-22, 90,-31,-73, 73, 31,-90, 22, 78,-67,-38, 90,-13,-82, 61, 46,-88,  4, 85,-54},
	{ 50,-89, 18, 75,-75,-18, 89,-50,-50, 89,-18,-75, 75, 18,-89, 50, 50,-89, 18, 75,-75,-18, 89,-50,-50, 89,-18,-75, 75, 18,-89, 50},
	{ 46,-90, 38, 54,-90, 31, 61,-88, 22, 67,-85, 13, 73,-82,  4, 78,-78, -4, 82,-73,-13, 85,-67,-22, 88,-61,-31, 90,-54,-38, 90,-46},
	{ 43,-90, 57, 25,-87, 70,  9,-80, 80, -9,-70, 87,-25,-57, 90,-43,-43, 90,-57,-25, 87,-70, -9, 80,-80,  9, 70,-87, 25, 57,-90, 43},
	{ 38,-88, 73, -4,-67, 90,-46,-31, 85,-78, 13, 61,-90, 54, 22,-82, 82,-22,-54, 90,-61,-13, 78,-85, 31, 46,-90, 67,  4,-73, 88,-38},
	{ 36,-83, 83,-36,-36, 83,-83, 36, 36,-83, 83,-36,-36, 83,-83, 36, 36,-83, 83,-36,-36, 83,-83, 36, 36,-83, 83,-36,-36, 83,-83, 36},
	{ 31,-78, 90,-61,  4, 54,-88, 82,-38,-22, 73,-90, 67,-13,-46, 85,-85, 46, 13,-67, 90,-73, 22, 38,-82, 88,-54, -4, 61,-90, 78,-31},
	{ 25,-70, 90,-80, 43,  9,-57, 87,-87, 57, -9,-43, 80,-90, 70,-25,-25, 70,-90, 80,-43, -9, 57,-87, 87,-57,  9, 43,-80, 90,-70, 25},
	{ 22,-61, 85,-90, 73,-38, -4, 46,-78, 90,-82, 54,-13,-31, 67,-88, 88,-67, 31, 13,-54, 82,-90, 78,-46,  4, 38,-73, 90,-85, 61,-22},
	{ 18,-50, 75,-89, 89,-75, 50,-18,-18, 50,-75, 89,-89, 75,-50, 18, 18,-50, 75,-89, 89,-75, 50,-18,-18, 50,-75, 89,-89, 75,-50, 18},
	{ 13,-38, 61,-78, 88,-90, 85,-73, 54,-31,  4, 22,-46, 67,-82, 90,-90, 82,-67, 46,-22, -4, 31,-54, 73,-85, 90,-88, 78,-61, 38,-13},
	{  9,-25, 43,-57, 70,-80, 87,-90, 90,-87, 80,-70, 57,-43, 25, -9, -9, 25,-43, 57,-70, 80,-87, 90,-90, 87,-80, 70,-57, 43,-25,  9},
	{  4,-13, 22,-31, 38,-46, 54,-61, 67,-73, 78,-82, 85,-88, 90,-90, 90,-90, 88,-85, 82,-78, 73,-67, 61,-54, 46,-38, 31,-22, 13, -4}
};

// Mode-Dependent DCT/DST
const int16_t dst_mat_4 [4][4]=
{
	{29,   55,    74,   84},
	{74,   74,    0 ,  -74},
	{84,  -29,   -74,   55},
	{55,  -84,    74,  -29},
};

uint8_t dct_dst_mode_vert[X265_NUM_INTRA_MODE];
uint8_t dct_dst_mode_hor[X265_NUM_INTRA_MODE];

const uint32_t go_rice_range[5] =
{
	7, 14, 26, 46, 78
};

const uint32_t go_rice_prefix_len[5] =
{
	8, 7, 6, 5, 4
};



x265_est_bits_sbac_struct_t *x265_est_bits_sbac_struct_new ()
{
	x265_est_bits_sbac_struct_t *est_bits_sbac_struct = NULL ;

	CHECKED_MALLOCZERO( est_bits_sbac_struct, sizeof(x265_est_bits_sbac_struct_t) );
	if ( x265_est_bits_sbac_struct_init(est_bits_sbac_struct) )
	{
		goto fail ;
	}

	return est_bits_sbac_struct ;
fail:
	x265_est_bits_sbac_struct_delete ( (void*) est_bits_sbac_struct ) ;
	return NULL ;
}

void x265_est_bits_sbac_struct_delete ( x265_est_bits_sbac_struct_t *est_bits_sbac_struct )
{
	x265_est_bits_sbac_struct_deinit ( est_bits_sbac_struct ) ;
	x265_free ( est_bits_sbac_struct ) ;
}

int x265_est_bits_sbac_struct_init ( x265_est_bits_sbac_struct_t *est_bits_sbac_struct )
{

	return 0 ;
}

void x265_est_bits_sbac_struct_deinit ( x265_est_bits_sbac_struct_t *est_bits_sbac_struct )
{

}

void x265_qp_param_set_qp_param( x265_qp_param_t *qp_param, int32_t i_qp_scaled )
{
	qp_param->i_qp = i_qp_scaled ;
	qp_param->i_per = i_qp_scaled / 6 ;
	qp_param->i_rem = i_qp_scaled % 6 ;
	qp_param->i_bits = X265_QP_BITS + qp_param->i_per ;
}

void x265_qp_param_clear_qp_param( x265_qp_param_t *qp_param )
{
	qp_param->i_qp = 0 ;
	qp_param->i_per = 0 ;
	qp_param->i_rem = 0 ;
	qp_param->i_bits = 0 ;
}


#if X265_RDOQ_CHROMA_LAMBDA

void x265_tr_quant_set_lambda ( x265_tr_quant_t *tr_quant, double f_lambda_luma, double f_lambda_chroma )
{
	tr_quant->f_lambda_luma = f_lambda_luma ;
	tr_quant->f_lambda_chroma = f_lambda_chroma ;
}

void x265_tr_quant_select_lambda ( x265_tr_quant_t *tr_quant, enum text_type_e i_text_type )
{
	tr_quant->f_lambda = ( i_text_type == TEXT_LUMA) ? tr_quant->f_lambda_luma : tr_quant->f_lambda_chroma ;
}

#else

void x265_tr_quant_set_lambda ( x265_tr_quant_t* tr_quant, double f_lambda )
{
	tr_quant->f_lambda = f_lambda ;
}

#endif

void x265_tr_quant_set_rdoq_offset ( x265_tr_quant_t *tr_quant, uint32_t i_rdoq_offset )
{
	tr_quant->i_rdoq_offset = i_rdoq_offset;
}

//!< get Error Scale Coefficent
double *x265_tr_quant_get_err_scale_coeff ( x265_tr_quant_t *tr_quant,
											uint32_t i_list,
											uint32_t i_size,
											uint32_t i_qp )
{
	return tr_quant->err_scale[i_size][i_list][i_qp];
}


//!< get Quant Coefficent
int32_t *x265_tr_quant_get_quant_coeff ( x265_tr_quant_t *tr_quant,
										uint32_t i_list,
										uint32_t i_qp,
										uint32_t i_size )
{
	return tr_quant->quant_coef[i_size][i_list][i_qp];
}

//!< get DeQuant Coefficent
int32_t *x265_tr_quant_get_dequant_coeff ( x265_tr_quant_t *tr_quant,
											uint32_t i_list,
											uint32_t i_qp,
											uint32_t i_size )
{
	return tr_quant->dequant_coef[i_size][i_list][i_qp];
}

void x265_tr_quant_set_use_scaling_list ( x265_tr_quant_t *tr_quant,
											int32_t b_use_scaling_list )
{
	tr_quant->b_scaling_list_enabled_flag = b_use_scaling_list ;
}

int32_t x265_tr_quant_get_use_scaling_list ( x265_tr_quant_t *tr_quant )
{
	return tr_quant->b_scaling_list_enabled_flag;
}

int32_t x265_tr_quant_get_qp_delta ( x265_tr_quant_t *tr_quant, int32_t i_qp )
{
	return tr_quant->qp_delta[i_qp];
}

int32_t *x265_tr_quant_get_slice_n_samples ( x265_tr_quant_t *tr_quant )
{
	return tr_quant->slice_n_samples ;
}

double *x265_tr_quant_get_slice_sum_c ( x265_tr_quant_t *tr_quant )
{
	return tr_quant->slice_sum_c;
}






x265_tr_quant_t *x265_tr_quant_new (x265_t *h)
{
	x265_tr_quant_t *tr_quant = NULL ;

	CHECKED_MALLOCZERO( tr_quant, sizeof(x265_tr_quant_t) );
	if ( x265_tr_quant_init(h, tr_quant) )
	{
		goto fail ;
	}

	return tr_quant ;
fail:
	x265_tr_quant_delete ( (void*) tr_quant ) ;
	return NULL ;
}

void x265_tr_quant_delete ( x265_tr_quant_t *tr_quant )
{
	x265_tr_quant_deinit ( tr_quant ) ;
	x265_free ( tr_quant ) ;
}

int x265_tr_quant_init (x265_t *h, x265_tr_quant_t *tr_quant )
{
	x265_qp_param_clear_qp_param(&tr_quant->qp_param) ;
	CHECKED_MALLOCZERO (tr_quant->temp_coeff,
						sizeof(int32_t) * X265_MAX_CU_SIZE * X265_MAX_CU_SIZE ) ;
	tr_quant->est_bits_sbac = x265_est_bits_sbac_struct_new () ;
	if (NULL == tr_quant->est_bits_sbac)
	{
		goto fail ;
	}

	if ( x265_tr_quant_init_scaling_list ( tr_quant ) )
	{
		goto fail ;
	}

	if ( x265_tr_quant_ctx_sig_list_init ( tr_quant ) )
	{
		goto fail ;
	}
	x265_tr_quant_ic_rate_cost_init( tr_quant ) ;
	x265_transform_initialize (&tr_quant->transform, h->param.cpu ) ;
	return 0 ;

fail:
	x265_tr_quant_deinit ( tr_quant ) ;
	return -1 ;
}

void x265_tr_quant_deinit ( x265_tr_quant_t *tr_quant )
{
	x265_tr_quant_ctx_sig_list_deinit ( tr_quant ) ;

	x265_free (tr_quant->temp_coeff) ;
	tr_quant->temp_coeff = NULL ;

	x265_est_bits_sbac_struct_delete (tr_quant->est_bits_sbac) ;
	tr_quant->est_bits_sbac = NULL ;

	x265_tr_quant_destroy_scaling_list ( tr_quant ) ;
}

int32_t x265_tr_quant_ctx_sig_list_init ( x265_tr_quant_t *tr_quant )
{
	int32_t i_text_type = 0 ;
	int32_t i_pattern_sig_ctx = 0 ;
	int32_t i_scan_idx = 0 ;
	int32_t i_log2_blk_size = 0 ;
	int32_t i_blk_pos = 0 ;
	int32_t i_pos_x = 0 ;
	int32_t i_pos_y = 0 ;
	int32_t i_blk_size = 0 ;
	int32_t i_ctx_sig = 0 ;

	for ( i_text_type = TEXT_LUMA ; i_text_type <= TEXT_CHROMA_V ; ++ i_text_type )
	{
		for ( i_pattern_sig_ctx = 0 ; i_pattern_sig_ctx < 4 ; ++ i_pattern_sig_ctx )
		{
			for ( i_scan_idx = SCAN_DIAG ; i_scan_idx <= SCAN_VER ; ++ i_scan_idx )
			{
				for ( i_log2_blk_size = 2 ; i_log2_blk_size <= 5 ; ++ i_log2_blk_size )
				{
					i_blk_size = (1 << (i_log2_blk_size << 1)) ;
					CHECKED_MALLOCZERO( tr_quant->ctx_sig_list[i_text_type][i_pattern_sig_ctx][i_scan_idx][i_log2_blk_size],
										i_blk_size * sizeof(int32_t) );


				}
			}
		}
	}


	for ( i_text_type = TEXT_LUMA ; i_text_type <= TEXT_CHROMA_V ; ++ i_text_type )
	{
		for ( i_pattern_sig_ctx = 0 ; i_pattern_sig_ctx < 4 ; ++ i_pattern_sig_ctx )
		{
			for ( i_scan_idx = SCAN_DIAG ; i_scan_idx <= SCAN_VER ; ++ i_scan_idx )
			{
				for ( i_log2_blk_size = 2 ; i_log2_blk_size <= 5 ; ++ i_log2_blk_size )
				{
					i_blk_size = (1 << (i_log2_blk_size << 1)) ;
					for ( i_blk_pos = 0 ; i_blk_pos < i_blk_size ; ++ i_blk_pos )
					{
						i_pos_y = i_blk_pos >> i_log2_blk_size;
						i_pos_x = i_blk_pos - ( i_pos_y << i_log2_blk_size );

		    			i_ctx_sig = x265_tr_quant_get_sig_ctx_inc( tr_quant,
		    														i_pattern_sig_ctx,
		    														i_scan_idx,
		    														i_pos_x,
		    														i_pos_y,
		    														i_log2_blk_size,
		    														i_text_type );
		    			tr_quant->ctx_sig_list[i_text_type][i_pattern_sig_ctx][i_scan_idx][i_log2_blk_size][i_blk_pos]
		    			        = i_ctx_sig ;
					}
				}
			}
		}
	}
	return 0 ;

fail:
	x265_tr_quant_ctx_sig_list_deinit ( tr_quant ) ;
	return -1 ;
}

void x265_tr_quant_ctx_sig_list_deinit ( x265_tr_quant_t *tr_quant )
{

	int32_t i_text_type = 0 ;
	int32_t i_pattern_sig_ctx = 0 ;
	int32_t i_scan_idx = 0 ;
	int32_t i_log2_blk_size = 0 ;

	for ( i_text_type = TEXT_LUMA ; i_text_type <= TEXT_CHROMA_V ; ++ i_text_type )
	{
		for ( i_pattern_sig_ctx = 0 ; i_pattern_sig_ctx < 4 ; ++ i_pattern_sig_ctx )
		{
			for ( i_scan_idx = SCAN_DIAG ; i_scan_idx <= SCAN_VER ; ++ i_scan_idx )
			{
				for ( i_log2_blk_size = 2 ; i_log2_blk_size <= 5 ; ++ i_log2_blk_size )
				{
					x265_free (tr_quant->ctx_sig_list[i_text_type][i_pattern_sig_ctx][i_scan_idx][i_log2_blk_size]);
				}
			}
		}
	}
}




void x265_tr_quant_print ( x265_tr_quant_t *tr_quant, FILE *file )
{
	print_double ( tr_quant->f_lambda_luma, file ) ;
	fprintf ( file, "\n" ) ;
	print_double ( tr_quant->f_lambda_chroma, file ) ;
	fprintf ( file, "\n" ) ;
}


#if X265_ADAPTIVE_QP_SELECTION
void x265_tr_quant_store_slice_qp_next(x265_tr_quant_t *tr_quant,
										x265_slice_t *slice)
{
	int32_t loop = 0 ;
	int32_t k = 0 ;
	int32_t i_qp_base = 0 ;
	int32_t i_slice_qp_used = 0 ;
	int32_t i_slice_qp_next;
	int32_t i_cnt = 0 ;
    double q[X265_MAX_QP+1] ;
    double f_alpha = 0.0 ;
	double f_sum = 0.0 ;
    double f_q_next = 0.0 ;


	i_qp_base = slice->i_slice_qp_base ;
	i_slice_qp_used = slice->i_slice_qp ;
	f_alpha = i_qp_base < 17 ? 0.5 : 1;

	i_cnt = 0;
	for( loop = 1 ; loop <= X265_LEVEL_RANGE ; ++ loop )
	{
		i_cnt += tr_quant->slice_n_samples[loop] ;
	}

	if( !tr_quant->b_use_rdoq )
	{
		i_slice_qp_used = i_qp_base;
		f_alpha = 0.5;
	}

	if( i_cnt > 120 )
	{
		f_sum = 0;
		k = 0;
		for( loop = 1 ; loop < X265_LEVEL_RANGE ; ++ loop )
		{
			f_sum += loop * tr_quant->slice_sum_c[loop];
			k += loop * loop * tr_quant->slice_n_samples[loop];
		}

		for( loop = 0 ; loop <= X265_MAX_QP ; ++ loop )
		{
			q[loop] = (double)(inv_quant_scales[loop%6] * (1<<(loop/6)))/64 ;
		}

		f_q_next = f_sum / k * q[i_slice_qp_used] / (1 << X265_ARL_C_PRECISION);

		for ( loop = 0 ; loop < X265_MAX_QP ; loop ++ )
		{
			if(f_q_next < f_alpha * q[loop] + (1 - f_alpha) * q[loop + 1] )
			{
				break;
			}
		}
		i_slice_qp_next = x265_clip3_int32(loop, i_slice_qp_used - 3, i_slice_qp_used + 3) ;
	}
	else
	{
		i_slice_qp_next = i_slice_qp_used;
	}

	tr_quant->qp_delta[i_qp_base] = i_slice_qp_next - i_qp_base;
}


void x265_tr_quant_init_slice_qp_delta ( x265_tr_quant_t *tr_quant )
{
	int32_t qp = 0 ;

	for ( qp=0 ; qp <= X265_MAX_QP ; ++ qp )
	{
		tr_quant->qp_delta[qp] = qp < 17 ? 0 : 1 ;
	}
}

void x265_tr_quant_clear_slice_arl_cnt ( x265_tr_quant_t *tr_quant )
{
	memset(tr_quant->slice_sum_c, 0, sizeof(double)*(X265_LEVEL_RANGE+1));
	memset(tr_quant->slice_n_samples, 0, sizeof(int32_t)*(X265_LEVEL_RANGE+1));
}

#endif


/** Set qP for Quantization.
 * \param qpy QPy
 * \param bLowpass
 * \param eSliceType
 * \param eTxtType
 * \param qpBdOffset
 * \param chromaQPOffset
 *
 * return void
 */
void x265_tr_quant_set_qp_for_quant( x265_tr_quant_t *tr_quant,
									int32_t i_qp_y,
									enum text_type_e i_text_type,
									int32_t i_qp_bd_offset,
									int32_t i_chroma_qp_offset)
{
	int32_t i_qp_scaled = 0;

	if(i_text_type == TEXT_LUMA)
	{
		i_qp_scaled = i_qp_y + i_qp_bd_offset;
	}
	else
	{
		i_qp_scaled = x265_clip3_int32( i_qp_y + i_chroma_qp_offset, - i_qp_bd_offset, 57);

		if(i_qp_scaled < 0)
		{
			i_qp_scaled = i_qp_scaled + i_qp_bd_offset;
		}
		else
		{
			i_qp_scaled = chroma_scale[ i_qp_scaled ] + i_qp_bd_offset;
		}
	}
	x265_qp_param_set_qp_param ( &tr_quant->qp_param, i_qp_scaled );
}


#if X265_MATRIX_MULT
/** NxN forward transform (2D) using brute force matrix multiplication (3 nested loops)
 *  \param block pointer to input data (residual)
 *  \param coeff pointer to output data (transform coefficients)
 *  \param i_stride stride of input data
 *  \param i_tr_size transform size (i_tr_size x i_tr_size)
 *  \param i_mode is int32_tra Prediction mode used in Mode-Dependent DCT/DST only
 */
void x_tr(x265_t *h,
		int32_t i_bit_depth,
		pixel *block,
		int32_t *coeff,
		uint32_t i_stride,
		uint32_t i_tr_size,
		uint32_t i_mode)
{
	int32_t i,j,k,i_sum;
	int32_t tmp[32*32];
	const int16_t *i_t;
	uint32_t i_log2_tr_size = h->global.convert_to_bit[ i_tr_size ] + 2;
	int32_t i_shift_1st = 0 ;
	int32_t i_add_1st = 0 ;
	int32_t i_shift_2nd = 0 ;
	int32_t i_add_2nd = 0 ;

	if (i_tr_size==4)
	{
		i_t  = t4[0];
	}
	else if (i_tr_size==8)
	{
		i_t = t8[0];
	}
	else if (i_tr_size==16)
	{
		i_t = t16[0];
	}
	else if (i_tr_size==32)
	{
		i_t = t32[0];
	}
	else
	{
		assert(0);
	}

	// log2(N) - 1 + i_bit_depth-8
	i_shift_1st = i_log2_tr_size - 1 + i_bit_depth - 8;
	i_add_1st = 1 << (i_shift_1st - 1);
	i_shift_2nd = i_log2_tr_size + 6;
	i_add_2nd = 1 << (i_shift_2nd - 1);

	/* Horizontal transform */
	if (i_tr_size==4)
	{
		if (i_mode != X265_REG_DCT && dct_dst_mode_hor[i_mode])
		{
			i_t  =  dst_mat_4[0];
		}
	}
	for (i=0; i<i_tr_size; i++)
	{
		for (j=0; j<i_tr_size; j++)
		{
			i_sum = 0;
			for (k=0; k<i_tr_size; k++)
			{
				i_sum += i_t[i*i_tr_size+k]*block[j*i_stride+k];
			}
			tmp[i*i_tr_size+j] = (i_sum + i_add_1st)>>i_shift_1st;
		}
	}

	/* Vertical transform */
	if (i_tr_size==4)
	{
		if (i_mode != X265_REG_DCT && dct_dst_mode_vert[i_mode])
		{
			i_t  =  dst_mat_4[0];
		}
		else
		{
			i_t  = t4[0];
		}
	}
	for (i=0; i<i_tr_size; i++)
	{
		for (j=0; j<i_tr_size; j++)
		{
			i_sum = 0;
			for (k=0; k<i_tr_size; k++)
			{
				i_sum += i_t[i*i_tr_size+k]*tmp[j*i_tr_size+k];
			}
			coeff[i*i_tr_size+j] = (i_sum + i_add_2nd)>>i_shift_2nd;
		}
	}
}

/** NxN inverse transform (2D) using brute force matrix multiplication (3 nested loops)
 *  \param coeff pointer to input data (transform coefficients)
 *  \param block pointer to output data (residual)
 *  \param i_stride stride of output data
 *  \param i_tr_size transform size (i_tr_size x i_tr_size)
 *  \param i_mode is int32_tra Prediction mode used in Mode-Dependent DCT/DST only
 */
void x_i_tr(x265_t *h,
		int32_t *coeff,
		pixel *block,
		uint32_t i_stride,
		uint32_t i_tr_size,
		uint32_t i_mode)
{
	int32_t i,j,k,i_sum;
	int32_t tmp[32*32];
	const int16_t *i_t;
	int32_t i_shift_1st = 0 ;
	int32_t i_add_1st = 0 ;
	int32_t i_shift_2nd = 0 ;
	int32_t i_add_2nd = 0 ;

	if (i_tr_size==4)
	{
		i_t  = t4[0];
	}
	else if (i_tr_size==8)
	{
		i_t = t8[0];
	}
	else if (i_tr_size==16)
	{
		i_t = t16[0];
	}
	else if (i_tr_size==32)
	{
		i_t = t32[0];
	}
	else
	{
		assert(0);
	}

	i_shift_1st = X265_SHIFT_INV_1ST ;
	i_add_1st = 1 << (i_shift_1st - 1) ;
	i_shift_2nd = X265_SHIFT_INV_2ND - h->param.sps.i_bit_depth_y - 8 ;
	i_add_2nd = 1 << (i_shift_2nd - 1) ;
	if (i_tr_size==4)
	{
		if (i_mode != X265_REG_DCT && dct_dst_mode_vert[i_mode] ) // Check for DCT or DST
		{
			i_t  =  dst_mat_4[0];
		}
	}

	/* Horizontal transform */
	for (i=0; i<i_tr_size; i++)
	{
		for (j=0; j<i_tr_size; j++)
		{
			i_sum = 0;
			for (k=0; k<i_tr_size; k++)
			{
				i_sum += i_t[k*i_tr_size+i]*coeff[k*i_tr_size+j];
			}
			tmp[i*i_tr_size+j] = x265_clip3_int32((i_sum + i_add_1st)>>i_shift_1st, -32768, 32767); // Clipping is normative
		}
	}

	if (i_tr_size==4)
	{
		if (i_mode != X265_REG_DCT && dct_dst_mode_hor[i_mode] )   // Check for DCT or DST
		{
			i_t  =  dst_mat_4[0];
		}
		else
		{
			i_t  = t4[0];
		}
	}

	/* Vertical transform */
	for (i=0; i<i_tr_size; i++)
	{
		for (j=0; j<i_tr_size; j++)
		{
			i_sum = 0;
			for (k=0; k<i_tr_size; k++)
			{
				i_sum += i_t[k*i_tr_size+j]*tmp[i*i_tr_size+k];
			}
			block[i*i_stride+j] = x265_clip3pixel((i_sum + i_add_2nd)>>i_shift_2nd, -32768, 32767); // Clipping is non-normative
		}
	}
}

#else

void partial_butterfly4(int16_t *src,
						int16_t *dst,
						int32_t i_shift,
						int32_t i_line)
{
	int32_t j;
	int32_t e[2],o[2];
	int32_t i_add = 0;

	i_add = 1<<(i_shift-1);
	for (j=0; j<i_line; j++)
	{
		/* e and o */
		e[0] = src[0] + src[3];
		o[0] = src[0] - src[3];
		e[1] = src[1] + src[2];
		o[1] = src[1] - src[2];

		dst[0] = (t4[0][0]*e[0] + t4[0][1]*e[1] + i_add)>>i_shift;
		dst[2*i_line] = (t4[2][0]*e[0] + t4[2][1]*e[1] + i_add)>>i_shift;
		dst[i_line] = (t4[1][0]*o[0] + t4[1][1]*o[1] + i_add)>>i_shift;
		dst[3*i_line] = (t4[3][0]*o[0] + t4[3][1]*o[1] + i_add)>>i_shift;

		src += 4;
		dst ++;
	}
}

// fast dst algorithm. full matrix multiplication for dst and fast dst algorithm
// give identical results
void fast_forward_dst(int16_t *block,
						int16_t *coeff,
						int32_t i_shift)  // input block, output coeff
{
	int32_t i, c[4];
	int32_t i_rnd_factor = 0;

	i_rnd_factor = 1 << (i_shift - 1);
	for (i=0; i<4; i++)
	{
		// int32_termediate variables
		c[0] = block[4*i+0] + block[4*i+3];
		c[1] = block[4*i+1] + block[4*i+3];
		c[2] = block[4*i+0] - block[4*i+1];
		c[3] = 74* block[4*i+2];

		coeff[   i] =  ( 29 * c[0] + 55 * c[1]         + c[3]               + i_rnd_factor ) >> i_shift;
		coeff[ 4+i] =  ( 74 * (block[4*i+0]+ block[4*i+1] - block[4*i+3])   + i_rnd_factor ) >> i_shift;
		coeff[ 8+i] =  ( 29 * c[2] + 55 * c[0]         - c[3]               + i_rnd_factor ) >> i_shift;
		coeff[12+i] =  ( 55 * c[2] - 29 * c[1]         + c[3]               + i_rnd_factor ) >> i_shift;
	}
}

void fast_inverse_dst(int16_t *tmp,
						int16_t *block,
						int32_t i_shift)  // input tmp, output block
{
	int32_t i, c[4];
	int32_t i_rnd_factor = 0;

	i_rnd_factor = 1<<(i_shift-1);
	for (i=0; i<4; i++)
	{
		// int32_termediate variables
		c[0] = tmp[  i] + tmp[ 8+i];
		c[1] = tmp[8+i] + tmp[12+i];
		c[2] = tmp[  i] - tmp[12+i];
		c[3] = 74* tmp[4+i];

		block[4*i+0] = x265_clip3_int16( ( 29 * c[0] + 55 * c[1] + c[3] + i_rnd_factor ) >> i_shift, -32768, 32767 );
		block[4*i+1] = x265_clip3_int16( ( 55 * c[2] - 29 * c[1] + c[3] + i_rnd_factor ) >> i_shift, -32768, 32767 );
		block[4*i+2] = x265_clip3_int16( ( 74 * (tmp[i] - tmp[8+i] + tmp[12+i]) + i_rnd_factor ) >> i_shift, -32768, 32767 );
		block[4*i+3] = x265_clip3_int16( ( 55 * c[0] + 29 * c[2] - c[3] + i_rnd_factor ) >> i_shift, -32768, 32767 );
	}
}

void partial_butterfly_inverse4(int16_t *src,
								int16_t *dst,
								int32_t i_shift,
								int32_t i_line)
{
	int32_t j;
	int32_t e[2],o[2];
	int32_t i_add = 0;

	i_add = 1<<(i_shift-1);
	for (j=0; j<i_line; j++)
	{
		/* utilizing symmetry properties to the maximum to minimize the number of multiplications */
		o[0] = t4[1][0]*src[i_line] + t4[3][0]*src[3*i_line];
		o[1] = t4[1][1]*src[i_line] + t4[3][1]*src[3*i_line];
		e[0] = t4[0][0]*src[0] + t4[2][0]*src[2*i_line];
		e[1] = t4[0][1]*src[0] + t4[2][1]*src[2*i_line];

		/* combining even and odd terms at each hierarchy levels to calculate the final spatial domain vector */
		dst[0] = x265_clip3_int16( (e[0] + o[0] + i_add) >> i_shift, -32768, 32767 );
		dst[1] = x265_clip3_int16( (e[1] + o[1] + i_add) >> i_shift, -32768, 32767 );
		dst[2] = x265_clip3_int16( (e[1] - o[1] + i_add) >> i_shift, -32768, 32767 );
		dst[3] = x265_clip3_int16( (e[0] - o[0] + i_add) >> i_shift, -32768, 32767 );

		src   ++;
		dst += 4;
	}
}


void partial_butterfly8(int16_t *src,
						int16_t *dst,
						int32_t i_shift,
						int32_t i_line)
{
	int32_t j,k;
	int32_t e[4],o[4];
	int32_t ee[2],eo[2];
	int32_t i_add = 0;

	i_add = 1<<(i_shift-1);
	for (j=0; j<i_line; j++)
	{
		/* e and o*/
		for (k=0;k<4;k++)
		{
			e[k] = src[k] + src[7-k];
			o[k] = src[k] - src[7-k];
		}
		/* ee and eo */
		ee[0] = e[0] + e[3];
		eo[0] = e[0] - e[3];
		ee[1] = e[1] + e[2];
		eo[1] = e[1] - e[2];

		dst[0] = (t8[0][0]*ee[0] + t8[0][1]*ee[1] + i_add)>>i_shift;
		dst[4*i_line] = (t8[4][0]*ee[0] + t8[4][1]*ee[1] + i_add)>>i_shift;
		dst[2*i_line] = (t8[2][0]*eo[0] + t8[2][1]*eo[1] + i_add)>>i_shift;
		dst[6*i_line] = (t8[6][0]*eo[0] + t8[6][1]*eo[1] + i_add)>>i_shift;

		dst[i_line] = (t8[1][0]*o[0] + t8[1][1]*o[1] + t8[1][2]*o[2] + t8[1][3]*o[3] + i_add)>>i_shift;
		dst[3*i_line] = (t8[3][0]*o[0] + t8[3][1]*o[1] + t8[3][2]*o[2] + t8[3][3]*o[3] + i_add)>>i_shift;
		dst[5*i_line] = (t8[5][0]*o[0] + t8[5][1]*o[1] + t8[5][2]*o[2] + t8[5][3]*o[3] + i_add)>>i_shift;
		dst[7*i_line] = (t8[7][0]*o[0] + t8[7][1]*o[1] + t8[7][2]*o[2] + t8[7][3]*o[3] + i_add)>>i_shift;

		src += 8;
		dst ++;
	}
}


void partial_butterfly_inverse8(int16_t *src,
								int16_t *dst,
								int32_t i_shift,
								int32_t i_line)
{
	int32_t j,k;
	int32_t e[4],o[4];
	int32_t ee[2],eo[2];
	int32_t i_add = 0 ;

	i_add = 1 << (i_shift - 1);
	for (j=0; j<i_line; j++)
	{
		/* utilizing symmetry properties to the maximum to minimize the number of multiplications */
		for (k=0;k<4;k++)
		{
			o[k] = t8[ 1][k]*src[i_line] + t8[ 3][k]*src[3*i_line] + t8[ 5][k]*src[5*i_line] + t8[ 7][k]*src[7*i_line];
		}

		eo[0] = t8[2][0]*src[ 2*i_line ] + t8[6][0]*src[ 6*i_line ];
		eo[1] = t8[2][1]*src[ 2*i_line ] + t8[6][1]*src[ 6*i_line ];
		ee[0] = t8[0][0]*src[ 0      ] + t8[4][0]*src[ 4*i_line ];
		ee[1] = t8[0][1]*src[ 0      ] + t8[4][1]*src[ 4*i_line ];

		/* combining even and odd terms at each hierarchy levels to calculate the final spatial domain vector */
		e[0] = ee[0] + eo[0];
		e[3] = ee[0] - eo[0];
		e[1] = ee[1] + eo[1];
		e[2] = ee[1] - eo[1];
		for (k=0;k<4;k++)
		{
			dst[ k   ] = x265_clip3_int16( (e[k] + o[k] + i_add) >> i_shift, -32768, 32767 );
			dst[ k+4 ] = x265_clip3_int16( (e[3-k] - o[3-k] + i_add) >> i_shift, -32768, 32767 );
		}
		src ++;
		dst += 8;
	}
}

void partial_butterfly16(int16_t *src,
						int16_t *dst,
						int32_t i_shift,
						int32_t i_line)
{
	int32_t j,k;
	int32_t e[8],o[8];
	int32_t ee[4],eo[4];
	int32_t eee[2],eeo[2];
	int32_t i_add = 0;

	i_add = 1 << (i_shift - 1);
	for (j=0; j<i_line; j++)
	{
		/* e and o*/
		for (k=0;k<8;k++)
		{
			e[k] = src[k] + src[15-k];
			o[k] = src[k] - src[15-k];
		}
		/* ee and eo */
		for (k=0;k<4;k++)
		{
			ee[k] = e[k] + e[7-k];
			eo[k] = e[k] - e[7-k];
		}
		/* eee and eeo */
		eee[0] = ee[0] + ee[3];
		eeo[0] = ee[0] - ee[3];
		eee[1] = ee[1] + ee[2];
		eeo[1] = ee[1] - ee[2];

		dst[ 0      ] = (t16[ 0][0]*eee[0] + t16[ 0][1]*eee[1] + i_add)>>i_shift;
		dst[ 8*i_line ] = (t16[ 8][0]*eee[0] + t16[ 8][1]*eee[1] + i_add)>>i_shift;
		dst[ 4*i_line ] = (t16[ 4][0]*eeo[0] + t16[ 4][1]*eeo[1] + i_add)>>i_shift;
		dst[ 12*i_line] = (t16[12][0]*eeo[0] + t16[12][1]*eeo[1] + i_add)>>i_shift;

		for (k=2;k<16;k+=4)
		{
			dst[ k*i_line ] = (t16[k][0]*eo[0] + t16[k][1]*eo[1] + t16[k][2]*eo[2] + t16[k][3]*eo[3] + i_add)>>i_shift;
		}

		for (k=1;k<16;k+=2)
		{
			dst[ k*i_line ] = (t16[k][0]*o[0] + t16[k][1]*o[1] + t16[k][2]*o[2] + t16[k][3]*o[3] +
								t16[k][4]*o[4] + t16[k][5]*o[5] + t16[k][6]*o[6] + t16[k][7]*o[7] + i_add)>>i_shift;
		}

		src += 16;
		dst ++;

	}
}


void partial_butterfly_inverse16(int16_t *src,
								int16_t *dst,
								int32_t i_shift,
								int32_t i_line)
{
	int32_t j,k;
	int32_t e[8],o[8];
	int32_t ee[4],eo[4];
	int32_t eee[2],eeo[2];
	int32_t i_add = 0 ;

	i_add = 1 << (i_shift - 1);
	for (j=0; j<i_line; j++)
	{
		/* utilizing symmetry properties to the maximum to minimize the number of multiplications */
		for (k=0;k<8;k++)
		{
			o[k] = t16[ 1][k]*src[ i_line] + t16[ 3][k]*src[ 3*i_line] + t16[ 5][k]*src[ 5*i_line] + t16[ 7][k]*src[ 7*i_line] +
					t16[ 9][k]*src[ 9*i_line] + t16[11][k]*src[11*i_line] + t16[13][k]*src[13*i_line] + t16[15][k]*src[15*i_line];
		}
		for (k=0;k<4;k++)
		{
			eo[k] = t16[ 2][k]*src[ 2*i_line] + t16[ 6][k]*src[ 6*i_line] + t16[10][k]*src[10*i_line] + t16[14][k]*src[14*i_line];
		}
		eeo[0] = t16[4][0]*src[ 4*i_line ] + t16[12][0]*src[ 12*i_line ];
		eee[0] = t16[0][0]*src[ 0      ] + t16[ 8][0]*src[ 8*i_line  ];
		eeo[1] = t16[4][1]*src[ 4*i_line ] + t16[12][1]*src[ 12*i_line ];
		eee[1] = t16[0][1]*src[ 0      ] + t16[ 8][1]*src[ 8*i_line  ];

		/* combining even and odd terms at each hierarchy levels to calculate the final spatial domain vector */
		for (k=0;k<2;k++)
		{
			ee[k] = eee[k] + eeo[k];
			ee[k+2] = eee[1-k] - eeo[1-k];
		}
		for (k=0;k<4;k++)
		{
			e[k] = ee[k] + eo[k];
			e[k+4] = ee[3-k] - eo[3-k];
		}
		for (k=0;k<8;k++)
		{
			dst[k]   = x265_clip3_int16( (e[k] + o[k] + i_add) >> i_shift, -32768, 32767 );
			dst[k+8] = x265_clip3_int16( (e[7-k] - o[7-k] + i_add) >> i_shift, -32768, 32767 );
		}
		src ++;
		dst += 16;
	}
}


void partial_butterfly32(int16_t *src,
						int16_t *dst,
						int32_t i_shift,
						int32_t i_line)
{
	int32_t j,k;
	int32_t e[16],o[16];
	int32_t ee[8],eo[8];
	int32_t eee[4],eeo[4];
	int32_t eeee[2],eeeo[2];
	int32_t i_add = 0;

	//	print_all_t32() ;
	i_add = 1<<(i_shift-1);
	for (j=0; j<i_line; j++)
	{
		/* e and o*/
		for (k=0;k<16;k++)
		{
			e[k] = src[k] + src[31-k];
			o[k] = src[k] - src[31-k];
		}
		/* ee and eo */
		for (k=0;k<8;k++)
		{
			ee[k] = e[k] + e[15-k];
			eo[k] = e[k] - e[15-k];
		}
		/* eee and eeo */
		for (k=0;k<4;k++)
		{
			eee[k] = ee[k] + ee[7-k];
			eeo[k] = ee[k] - ee[7-k];
		}
		/* eeee and eeeo */
		eeee[0] = eee[0] + eee[3];
		eeeo[0] = eee[0] - eee[3];
		eeee[1] = eee[1] + eee[2];
		eeeo[1] = eee[1] - eee[2];

		/*
		if ( 2 == ww )
		{
			fprintf ( stderr, "%d\n", (t32[ 0][0]*eeee[0] + t32[ 0][1]*eeee[1])) ;
			exit(0) ;
		}
		*/

		dst[ 0       ] = (t32[ 0][0]*eeee[0] + t32[ 0][1]*eeee[1] + i_add)>>i_shift;
		dst[ 16*i_line ] = (t32[16][0]*eeee[0] + t32[16][1]*eeee[1] + i_add)>>i_shift;
		dst[ 8*i_line  ] = (t32[ 8][0]*eeeo[0] + t32[ 8][1]*eeeo[1] + i_add)>>i_shift;
		dst[ 24*i_line ] = (t32[24][0]*eeeo[0] + t32[24][1]*eeeo[1] + i_add)>>i_shift;
		for (k=4;k<32;k+=8)
		{
			dst[ k*i_line ] = (t32[k][0]*eeo[0] + t32[k][1]*eeo[1] + t32[k][2]*eeo[2] + t32[k][3]*eeo[3] + i_add)>>i_shift;
		}
		for (k=2;k<32;k+=4)
		{
			dst[ k*i_line ] = (t32[k][0]*eo[0] + t32[k][1]*eo[1] + t32[k][2]*eo[2] + t32[k][3]*eo[3] +
								t32[k][4]*eo[4] + t32[k][5]*eo[5] + t32[k][6]*eo[6] + t32[k][7]*eo[7] + i_add)>>i_shift;
		}
		for (k=1;k<32;k+=2)
		{
			dst[ k*i_line ] = (t32[k][ 0]*o[ 0] + t32[k][ 1]*o[ 1] + t32[k][ 2]*o[ 2] + t32[k][ 3]*o[ 3] +
								t32[k][ 4]*o[ 4] + t32[k][ 5]*o[ 5] + t32[k][ 6]*o[ 6] + t32[k][ 7]*o[ 7] +
								t32[k][ 8]*o[ 8] + t32[k][ 9]*o[ 9] + t32[k][10]*o[10] + t32[k][11]*o[11] +
								t32[k][12]*o[12] + t32[k][13]*o[13] + t32[k][14]*o[14] + t32[k][15]*o[15] + i_add)>>i_shift;
		}
		src += 32;
		dst ++;
	}
}



void partial_butterfly_inverse32(int16_t *src,
								int16_t *dst,
								int32_t i_shift,
								int32_t i_line)
{
	int32_t j,k;
	int32_t e[16],o[16];
	int32_t ee[8],eo[8];
	int32_t eee[4],eeo[4];
	int32_t eeee[2],eeeo[2];
	int32_t i_add = 0;

	//	print_partial_butterfly_inverse_32_coeff() ;

	i_add = 1 << (i_shift - 1);
	for (j=0; j<i_line; j++)
	{
		/* utilizing symmetry properties to the maximum to minimize the number of multiplications */
		for (k=0;k<16;k++)
		{
			o[k] = t32[ 1][k]*src[ i_line  ] + t32[ 3][k]*src[ 3*i_line  ] + t32[ 5][k]*src[ 5*i_line  ] + t32[ 7][k]*src[ 7*i_line  ] +
					t32[ 9][k]*src[ 9*i_line  ] + t32[11][k]*src[ 11*i_line ] + t32[13][k]*src[ 13*i_line ] + t32[15][k]*src[ 15*i_line ] +
					t32[17][k]*src[ 17*i_line ] + t32[19][k]*src[ 19*i_line ] + t32[21][k]*src[ 21*i_line ] + t32[23][k]*src[ 23*i_line ] +
					t32[25][k]*src[ 25*i_line ] + t32[27][k]*src[ 27*i_line ] + t32[29][k]*src[ 29*i_line ] + t32[31][k]*src[ 31*i_line ];
		}
		for (k=0;k<8;k++)
		{
			eo[k] = t32[ 2][k]*src[ 2*i_line  ] + t32[ 6][k]*src[ 6*i_line  ] + t32[10][k]*src[ 10*i_line ] + t32[14][k]*src[ 14*i_line ] +
					t32[18][k]*src[ 18*i_line ] + t32[22][k]*src[ 22*i_line ] + t32[26][k]*src[ 26*i_line ] + t32[30][k]*src[ 30*i_line ];
		}

		for (k=0;k<4;k++)
		{
			eeo[k] = t32[4][k]*src[ 4*i_line ] + t32[12][k]*src[ 12*i_line ] + t32[20][k]*src[ 20*i_line ] + t32[28][k]*src[ 28*i_line ];
		}
		eeeo[0] = t32[8][0]*src[ 8*i_line ] + t32[24][0]*src[ 24*i_line ];
		eeeo[1] = t32[8][1]*src[ 8*i_line ] + t32[24][1]*src[ 24*i_line ];
		eeee[0] = t32[0][0]*src[ 0      ] + t32[16][0]*src[ 16*i_line ];
		eeee[1] = t32[0][1]*src[ 0      ] + t32[16][1]*src[ 16*i_line ];

		/* combining even and odd terms at each hierarchy levels to calculate the final spatial domain vector */
		eee[0] = eeee[0] + eeeo[0];
		eee[3] = eeee[0] - eeeo[0];
		eee[1] = eeee[1] + eeeo[1];
		eee[2] = eeee[1] - eeeo[1];
		for (k=0;k<4;k++)
		{
			ee[k] = eee[k] + eeo[k];
			ee[k+4] = eee[3-k] - eeo[3-k];
		}

		for (k=0;k<8;k++)
		{
			e[k] = ee[k] + eo[k];
			e[k+8] = ee[7-k] - eo[7-k];
		}
		for (k=0;k<16;k++)
		{
			dst[k]    = x265_clip3_int16( (e[k] + o[k] + i_add) >> i_shift, -32768, 32767 );
			dst[k+16] = x265_clip3_int16( (e[15-k] - o[15-k] + i_add) >> i_shift, -32768, 32767 );
		}
		src ++;
		dst += 32;
	}
}


/** mx_n forward transform (2d)
*  \param block input data (residual)
*  \param coeff output data (transform coefficients)
*  \param i_width input data (width of transform)
*  \param i_height input data (height of transform)
*/
void x_tr_mxn(x265_t *h,
				int32_t bit_depth,
				int16_t *block,
				int16_t *coeff,
				int32_t i_width,
				int32_t i_height,
				uint32_t i_mode)
{
	int32_t i_shift_1st = 0 ;
	int32_t i_shift_2nd = 0 ;
	int16_t tmp[ 64 * 64 ];

	i_shift_1st = h->global.convert_to_bit[i_width]  + 1 + bit_depth-8; // log2(i_width) - 1 + g_bit_depth - 8
	i_shift_2nd = h->global.convert_to_bit[i_height]  + 8;                   // log2(i_height) + 6
	if( i_width == 4 && i_height == 4)
	{
		if (i_mode != X265_REG_DCT)
		{
			fast_forward_dst(block,tmp,i_shift_1st); // forward dst by fast algorithm, block input, tmp output
			fast_forward_dst(tmp,coeff,i_shift_2nd); // forward dst by fast algorithm, tmp input, coeff output
		}
		else
		{
			partial_butterfly4(block, tmp, i_shift_1st, i_height);
			partial_butterfly4(tmp, coeff, i_shift_2nd, i_width);
		}

	}
	else if( i_width == 8 && i_height == 8)
	{
		partial_butterfly8( block, tmp, i_shift_1st, i_height );
		partial_butterfly8( tmp, coeff, i_shift_2nd, i_width );
	}
	else if( i_width == 16 && i_height == 16)
	{
		partial_butterfly16( block, tmp, i_shift_1st, i_height );
		partial_butterfly16( tmp, coeff, i_shift_2nd, i_width );
	}
	else if( i_width == 32 && i_height == 32)
	{
		partial_butterfly32( block, tmp, i_shift_1st, i_height );
		partial_butterfly32( tmp, coeff, i_shift_2nd, i_width );
	}
}

/** mx_n inverse transform (2d)
*  \param coeff input data (transform coefficients)
*  \param block output data (residual)
*  \param i_width input data (width of transform)
*  \param i_height input data (height of transform)
*/
void x_i_tr_mxn(int32_t bit_depth,
				int16_t *coeff,
				int16_t *block,
				int32_t i_width,
				int32_t i_height,
				uint32_t i_mode)
{
	int32_t i_shift_1st = 0;
	int32_t i_shift_2nd = 0;
	int16_t tmp[ 64*64];

	i_shift_1st = X265_SHIFT_INV_1ST ;
	i_shift_2nd = X265_SHIFT_INV_2ND - (bit_depth-8);
	if( i_width == 4 && i_height == 4)
	{
		if (i_mode != X265_REG_DCT)
		{
			fast_inverse_dst(coeff,tmp,i_shift_1st);    // inverse dst by fast algorithm, coeff input, tmp output
			fast_inverse_dst(tmp,block,i_shift_2nd); // inverse dst by fast algorithm, tmp input, coeff output
		}
		else
		{
			partial_butterfly_inverse4(coeff,tmp,i_shift_1st,i_width);
			partial_butterfly_inverse4(tmp,block,i_shift_2nd,i_height);
		}
	}
	else if( i_width == 8 && i_height == 8)
	{
		partial_butterfly_inverse8(coeff,tmp,i_shift_1st,i_width);
		partial_butterfly_inverse8(tmp,block,i_shift_2nd,i_height);
	}
	else if( i_width == 16 && i_height == 16)
	{
		partial_butterfly_inverse16(coeff,tmp,i_shift_1st,i_width);
		partial_butterfly_inverse16(tmp,block,i_shift_2nd,i_height);
	}
	else if( i_width == 32 && i_height == 32)
	{
		partial_butterfly_inverse32(coeff,tmp,i_shift_1st,i_width);
		partial_butterfly_inverse32(tmp,block,i_shift_2nd,i_height);
	}
}


#endif




// to minimize the distortion only. no rate is considered.
void x265_tr_quant_sign_bit_hiding_hdq(x265_tr_quant_t *tr_quant,
										x265_coeff_t* p_q_coef,
										x265_coeff_t* p_coef,
										uint32_t const *scan,
										int32_t* delta_u,
										int32_t i_width,
										int32_t i_height )
{
	int32_t i_last_cg = 0;
	int32_t i_abs_sum = 0 ;
	int32_t n ;
	int32_t i_sub_set ;
    int32_t i_sub_pos = 0 ;
    int32_t i_first_nz_pos_in_cg = 0, i_last_nz_pos_in_cg = 0 ;
    uint32_t i_signbit = 0 ;
    int32_t i_min_cost_inc = 0, i_min_pos = 0, i_final_change = 0, i_cur_cost = 0, i_cur_change = 0 ;
    uint32_t i_blk_pos = 0 ;
    uint32_t i_this_sign_bit = 0 ;


    i_last_cg = -1;

    for( i_sub_set = (i_width * i_height - 1) >> X265_LOG2_SCAN_SET_SIZE;
    		i_sub_set >= 0;
    		i_sub_set-- )
    {
    	i_sub_pos = i_sub_set << X265_LOG2_SCAN_SET_SIZE;
    	i_first_nz_pos_in_cg=X265_SCAN_SET_SIZE , i_last_nz_pos_in_cg=-1 ;
    	i_abs_sum = 0 ;

    	for(n = X265_SCAN_SET_SIZE-1; n >= 0; --n )
    	{
    		if( p_q_coef[ scan[ n + i_sub_pos ]] )
    		{
    			i_last_nz_pos_in_cg = n;
    			break;
    		}
    	}

    	for(n = 0; n <X265_SCAN_SET_SIZE; n++ )
    	{
    		if( p_q_coef[ scan[ n + i_sub_pos ]] )
    		{
    			i_first_nz_pos_in_cg = n;
    			break;
    		}
    	}

    	for(n = i_first_nz_pos_in_cg; n <=i_last_nz_pos_in_cg; n++ )
    	{
    		i_abs_sum += p_q_coef[ scan[ n + i_sub_pos ]];
    	}

    	if(i_last_nz_pos_in_cg>=0 && i_last_cg==-1)
    	{
    		i_last_cg = 1 ;
    	}

    	if( i_last_nz_pos_in_cg-i_first_nz_pos_in_cg>=X265_SBH_THRESHOLD )
    	{
    		i_signbit = (p_q_coef[scan[i_sub_pos+i_first_nz_pos_in_cg]]>0?0:1) ;
    		if( i_signbit!=(i_abs_sum&0x1) )  //compare i_signbit with sum_parity
    		{
    			i_min_cost_inc = X265_MAX_INT ;
    			i_min_pos =-1 ;
    			i_final_change = 0 ;
    			i_cur_cost=X265_MAX_INT ;
    			i_cur_change = 0 ;

    			for( n = ( i_last_cg == 1 ? i_last_nz_pos_in_cg : X265_SCAN_SET_SIZE - 1) ;
    					n >= 0; --n )
    			{
    				i_blk_pos = scan[ n+i_sub_pos ];
    				if(p_q_coef[ i_blk_pos ] != 0 )
    				{
    					if(delta_u[i_blk_pos]>0)
    					{
    						i_cur_cost = - delta_u[i_blk_pos];
    						i_cur_change=1 ;
    					}
    					else
    					{
    						//i_cur_change =-1;
    						if(n==i_first_nz_pos_in_cg && abs(p_q_coef[i_blk_pos])==1)
    						{
    							i_cur_cost=X265_MAX_INT ;
    						}
    						else
    						{
    							i_cur_cost = delta_u[i_blk_pos];
    							i_cur_change =-1;
    						}
    					}
    				}
    				else
    				{
    					if( n < i_first_nz_pos_in_cg)
    					{
    						i_this_sign_bit = (p_coef[i_blk_pos]>=0?0:1);
    						if(i_this_sign_bit != i_signbit )
    						{
    							i_cur_cost = X265_MAX_INT;
    						}
    						else
    						{
    							i_cur_cost = - (delta_u[i_blk_pos])  ;
    							i_cur_change = 1 ;
    						}
    					}
    					else
    					{
    						i_cur_cost = - (delta_u[i_blk_pos])  ;
    						i_cur_change = 1 ;
    					}
    				}

    				if( i_cur_cost<i_min_cost_inc)
    				{
    					i_min_cost_inc = i_cur_cost ;
    					i_final_change = i_cur_change ;
    					i_min_pos = i_blk_pos ;
    				}
    			} //cg loop

    			if(p_q_coef[i_min_pos] == 32767 || p_q_coef[i_min_pos] == -32768)
    			{
    				i_final_change = -1;
    			}

    			if(p_coef[i_min_pos]>=0)
    			{
    				p_q_coef[i_min_pos] += i_final_change ;
    			}
    			else
    			{
    				p_q_coef[i_min_pos] -= i_final_change ;
    			}
    		} // hide
    	}
    	if(i_last_cg==1)
    	{
    		i_last_cg=0 ;
    	}
    } // tu loop

    return;
}


void x265_tr_quant_print_quant_residual (spixel *p_quant_residual,
										uint32_t i_stride,
										uint32_t i_width,
										uint32_t i_height )
{
	FILE *file = NULL ;
	static int32_t b_first_print_quant_residual = 1 ;
	spixel *p_curr = NULL ;
	int32_t x = 0, y = 0 ;
	static int ww = 0 ;
	ww ++ ;
	if ( ww == 27 )
	{
		fprintf ( stderr, "Good\n" ) ;
	}

	if ( b_first_print_quant_residual )
	{
		file = fopen ( "QuantResidual2", "wb" ) ;
		b_first_print_quant_residual = 0 ;
	}
	else
	{
		file = fopen ( "QuantResidual2", "r+b" ) ;
		fseek ( file, 0, SEEK_END ) ;
	}

	p_curr = p_quant_residual ;
	for ( y = 0 ; y < i_height ; ++ y )
	{
		for ( x = 0 ; x < i_width ; ++ x )
		{
			fprintf ( file, "%d ", p_curr[x] ) ;
		}
		fprintf ( file, "\n" ) ;
		p_curr += i_stride ;
	}

	fclose ( file ) ;
}

void x265_tr_quant_print_quant_src (int32_t *p_src, int32_t i_width, int32_t i_height )
{
	FILE *file = NULL ;
	static int32_t b_first_print_quant_src = 1 ;
	int32_t *p_curr = NULL ;
	int32_t x = 0, y = 0 ;

	if ( b_first_print_quant_src )
	{
		file = fopen ( "QuantSrc2", "wb" ) ;
		b_first_print_quant_src = 0 ;
	}
	else
	{
		file = fopen ( "QuantSrc2", "r+b" ) ;
		fseek ( file, 0, SEEK_END ) ;
	}

	p_curr = p_src ;
	for ( y = 0 ; y < i_height ; ++ y )
	{
		for ( x = 0 ; x < i_width ; ++ x )
		{
			fprintf ( file, "%d ", p_curr[x] ) ;
		}
		fprintf ( file, "\n" ) ;
		p_curr += i_width ;
	}

	fclose ( file ) ;
}

void x265_tr_quant_print_quant_des (x265_coeff_t *p_des, int32_t i_width, int32_t i_height )
{
	FILE *file = NULL ;
	static int32_t b_first_print_quant_des = 1 ;
	x265_coeff_t *p_curr = NULL ;
	int32_t x = 0, y = 0 ;

	if ( b_first_print_quant_des )
	{
		file = fopen ( "QuantDes2", "wb" ) ;
		b_first_print_quant_des = 0 ;
	}
	else
	{
		file = fopen ( "QuantDes2", "r+b" ) ;
		fseek ( file, 0, SEEK_END ) ;
	}

	p_curr = p_des ;
	for ( y = 0 ; y < i_height ; ++ y )
	{
		for ( x = 0 ; x < i_width ; ++ x )
		{
			fprintf ( file, "%d ", p_curr[x] ) ;
		}
		fprintf ( file, "\n" ) ;
		p_curr += i_width ;
	}

	fclose ( file ) ;
}


void x265_tr_quant_print_dequant_residual (spixel *p_dequant_residual,
										uint32_t i_stride,
										uint32_t i_width,
										uint32_t i_height )
{
	FILE *file = NULL ;
	static int32_t b_first_print_dequant_residual = 1 ;
	spixel *p_curr = NULL ;
	int32_t x = 0, y = 0 ;

	if ( b_first_print_dequant_residual )
	{
		file = fopen ( "DequantResidual2", "wb" ) ;
		b_first_print_dequant_residual = 0 ;
	}
	else
	{
		file = fopen ( "DequantResidual2", "r+b" ) ;
		fseek ( file, 0, SEEK_END ) ;
	}

	p_curr = p_dequant_residual ;
	for ( y = 0 ; y < i_height ; ++ y )
	{
		for ( x = 0 ; x < i_width ; ++ x )
		{
			fprintf ( file, "%d ", p_curr[x] ) ;
		}
		fprintf ( file, "\n" ) ;
		p_curr += i_stride ;
	}

	fclose ( file ) ;
}

void x265_tr_quant_print_dequant_src (const x265_coeff_t *p_src, int32_t i_width, int32_t i_height )
{
	FILE *file = NULL ;
	static int32_t b_first_print_dequant_src = 1 ;
	const x265_coeff_t *p_curr = NULL ;
	int32_t x = 0, y = 0 ;

	if ( b_first_print_dequant_src )
	{
		file = fopen ( "DequantSrc2", "wb" ) ;
		b_first_print_dequant_src = 0 ;
	}
	else
	{
		file = fopen ( "DequantSrc2", "r+b" ) ;
		fseek ( file, 0, SEEK_END ) ;
	}

	p_curr = p_src ;
	for ( y = 0 ; y < i_height ; ++ y )
	{
		for ( x = 0 ; x < i_width ; ++ x )
		{
			fprintf ( file, "%d ", p_curr[x] ) ;
		}
		fprintf ( file, "\n" ) ;
		p_curr += i_width ;
	}

	fclose ( file ) ;
}

void x265_tr_quant_print_dequant_des (int32_t *p_des, int32_t i_width, int32_t i_height )
{
	FILE *file = NULL ;
	static int32_t b_first_print_dequant_des = 1 ;
	int32_t *p_curr = NULL ;
	int32_t x = 0, y = 0 ;

	if ( b_first_print_dequant_des )
	{
		file = fopen ( "DequantDes2", "wb" ) ;
		b_first_print_dequant_des = 0 ;
	}
	else
	{
		file = fopen ( "DequantDes2", "r+b" ) ;
		fseek ( file, 0, SEEK_END ) ;
	}

	p_curr = p_des ;
	for ( y = 0 ; y < i_height ; ++ y )
	{
		for ( x = 0 ; x < i_width ; ++ x )
		{
			fprintf ( file, "%d ", p_curr[x] ) ;
		}
		fprintf ( file, "\n" ) ;
		p_curr += i_width ;
	}

	fclose ( file ) ;
}

void x265_tr_quant_print_int_state (int32_t i_state, int32_t i_width )
{
	FILE *file = NULL ;
	static int32_t b_first_print_state = 1 ;
	static int32_t i_state_num = 0 ;

	if ( b_first_print_state )
	{
		file = fopen ( "TrQuantState2", "wb" ) ;
		b_first_print_state = 0 ;
	}
	else
	{
		file = fopen ( "TrQuantState2", "r+b" ) ;
		fseek ( file, 0, SEEK_END ) ;
	}

	fprintf ( file, "%d ", i_state ) ;
	i_state_num ++ ;
	if ( i_state_num % i_width == 0 )
	{
		fprintf ( file, "\n" ) ;
	}

	fclose ( file ) ;
}

void x265_tr_quant_x_quant(x265_t *h,
							x265_tr_quant_t *tr_quant,
							x265_data_cu_t* cu,
							int32_t *p_src,
							x265_coeff_t *p_des,
#if X265_ADAPTIVE_QP_SELECTION
							int32_t **pp_arl_des,
#endif
							int32_t i_width,
							int32_t i_height,
							uint32_t *p_ac_sum,
							enum text_type_e i_text_type,
							uint32_t i_abs_part_idx )
{
	int32_t loop = 0 ;
	int32_t *p_coef = NULL;
	x265_coeff_t* p_q_coef = NULL;
#if X265_ADAPTIVE_QP_SELECTION
	int32_t *p_arl_ccoef = NULL;
#endif
	int32_t i_add = 0;
	int32_t b_use_rdoq = 0;
	uint32_t i_log2_block_size = 0 ;
	uint32_t i_scan_idx = 0;
	uint32_t *scan = NULL;
	int32_t delta_u[32*32] ;
#if X265_ADAPTIVE_QP_SELECTION
	x265_qp_param_t qp_base;
	int32_t i_qp_base = 0;
	int32_t i_qp_scaled;
	int32_t i_qp_bd_offset = 0;
	int32_t i_chroma_qp_offset = 0;
#endif
	uint32_t i_log2_tr_size = 0;
	int32_t i_scaling_list_type = 0;
	int32_t *p_quant_coeff = NULL;
	uint32_t i_bit_depth = 0;
	int32_t i_transform_shift = 0;
	int32_t i_q_bits = 0;
#if X265_ADAPTIVE_QP_SELECTION
	int32_t i_q_bits_c = 0;
	int32_t i_add_c = 0;
#endif
	int32_t i_q_bits8 = 0 ;
	int32_t i_level = 0;
	int32_t i_sign = 0;
	uint32_t i_block_pos = 0;
#if X265_ADAPTIVE_QP_SELECTION
	int32_t i_tmp_level = 0 ;
#endif


	p_coef = p_src;
	p_q_coef = p_des;
#if X265_ADAPTIVE_QP_SELECTION
	p_arl_ccoef = *pp_arl_des;
#endif

	b_use_rdoq = x265_data_cu_get_transform_skip_p3(cu, i_abs_part_idx, i_text_type)
				? tr_quant->b_use_rdoq_ts : tr_quant->b_use_rdoq;

	//	x265_tr_quant_print_quant_src (p_src, i_width, i_height ) ;
	if ( b_use_rdoq && (i_text_type == TEXT_LUMA || X265_RDOQ_CHROMA))
	{
#if X265_ADAPTIVE_QP_SELECTION
		x265_tr_quant_x_rate_dist_opt_quant_ssse3(h,
												tr_quant,
												cu,
												p_coef,
												p_des,
												pp_arl_des,
												i_width,
												i_height,
												p_ac_sum,
												i_text_type,
												i_abs_part_idx );
#else
		x265_tr_quant_x_rate_dist_opt_quant_ssse3(h,
												tr_quant,
												cu,
												p_coef,
												p_des,
												i_width,
												i_height,
												p_ac_sum,
												i_text_type,
												i_abs_part_idx );
#endif
	}
	else
	{
		i_log2_block_size = h->global.convert_to_bit[ i_width ] + 2;

		i_scan_idx = x265_data_cu_get_coef_scan_idx(h,
													cu,
													i_abs_part_idx,
													i_width,
													i_text_type == TEXT_LUMA,
													x265_base_data_cu_is_intra((x265_base_data_cu_t*)cu, i_abs_part_idx));
		scan = h->scan.sig_last_scan[ i_scan_idx ][ i_log2_block_size - 1 ];

#if X265_ADAPTIVE_QP_SELECTION
		i_qp_base = h->slice->i_slice_qp_base;
		i_qp_bd_offset = (i_text_type == TEXT_LUMA)
						? h->sps[0].i_qp_bd_offset_y
						: h->sps[0].i_qp_bd_offset_c;

		if(i_text_type == TEXT_LUMA)
		{
			i_qp_scaled = i_qp_base + i_qp_bd_offset;
		}
		else
		{
			if(i_text_type == TEXT_CHROMA_U)
			{
				i_chroma_qp_offset = h->pps[0].i_chroma_cb_qp_offset
									+ h->slice->i_slice_qp_delta_cb ;
			}
			else
			{
				i_chroma_qp_offset = h->pps[0].i_chroma_cr_qp_offset
									+ h->slice->i_slice_qp_delta_cr ;
			}
			i_qp_base = i_qp_base + i_chroma_qp_offset;

			i_qp_scaled = x265_clip3_int32(i_qp_base, - i_qp_bd_offset, 57 );

			if(i_qp_scaled < 0)
			{
				i_qp_scaled = i_qp_scaled +  i_qp_bd_offset;
			}
			else
			{
				i_qp_scaled = chroma_scale[ i_qp_scaled ] + i_qp_bd_offset;
			}
		}
		x265_qp_param_set_qp_param(&qp_base, i_qp_scaled);
#endif

		i_log2_tr_size = h->global.convert_to_bit[ i_width ] + 2;
		i_scaling_list_type = (x265_base_data_cu_is_intra((x265_base_data_cu_t*)cu, i_abs_part_idx) ? 0 : 3)
								+ et_table[(int32_t)i_text_type];
		assert(i_scaling_list_type < 6);
		p_quant_coeff = tr_quant->quant_coef[i_log2_tr_size - 2][i_scaling_list_type][tr_quant->qp_param.i_rem];

		i_bit_depth = i_text_type == TEXT_LUMA ? h->param.sps.i_bit_depth_y : h->param.sps.i_bit_depth_c;
		i_transform_shift = X265_MAX_TR_DYNAMIC_RANGE - i_bit_depth - i_log2_tr_size;  // represents scaling through forward transform

		i_q_bits = X265_QUANT_SHIFT + tr_quant->qp_param.i_per + i_transform_shift;                // right shift of non-rdoq quantizer;  level = (coeff*i_q + offset)>>q_bits

		i_add = (h->slice->i_slice_type == I_SLICE ? 171 : 85) << (i_q_bits-9);

#if X265_ADAPTIVE_QP_SELECTION
		i_q_bits = X265_QUANT_SHIFT + qp_base.i_per + i_transform_shift;
		i_add = (h->slice->i_slice_type == I_SLICE ? 171 : 85) << (i_q_bits-9);
		i_q_bits_c = X265_QUANT_SHIFT + qp_base.i_per + i_transform_shift - X265_ARL_C_PRECISION;
		i_add_c   = 1 << (i_q_bits_c-1);
#endif

		i_q_bits8 = i_q_bits-8;
		for( loop = 0; loop < i_width*i_height; loop++ )
		{
			i_block_pos = loop;
			i_level = p_coef[i_block_pos];
			i_sign = (i_level < 0 ? -1: 1);

#if X265_ADAPTIVE_QP_SELECTION
			i_tmp_level = (int32_t)abs(i_level) * p_quant_coeff[i_block_pos];
			if( tr_quant->b_use_adapt_qp_select )
			{
				p_arl_ccoef[i_block_pos] = (int32_t)((i_tmp_level + i_add_c ) >> i_q_bits_c);
			}
			i_level = (int32_t)((i_tmp_level + i_add ) >> i_q_bits);
			delta_u[i_block_pos] = (int32_t)((i_tmp_level - (i_level<<i_q_bits) )>> i_q_bits8);
#else
			i_level = ((int32_t)abs(i_level) * p_quant_coeff[i_block_pos] + i_add ) >> i_q_bits;
			delta_u[i_block_pos] = (int32_t)(((int32_t)abs(p_coef[i_block_pos]) * p_quant_coeff[i_block_pos] - (i_level << i_q_bits)) >> i_q_bits8 );
#endif
			*p_ac_sum += i_level;
			i_level *= i_sign;
			p_q_coef[i_block_pos] = x265_clip3_int32( i_level, -32768, 32767 );
		} // for n
		if( h->pps[0].i_sign_hide_flag )
		{
			if(*p_ac_sum>=2)
			{
				x265_tr_quant_sign_bit_hiding_hdq(tr_quant,
													p_q_coef,
													p_coef,
													scan,
													delta_u,
													i_width,
													i_height ) ;
			}
		}
	} //if rdoq

	//	x265_tr_quant_print_quant_des (p_des, i_width, i_height ) ;

	//return;

}

void x265_tr_quant_x_dequant(x265_t *h,
							x265_tr_quant_t *tr_quant,
							int32_t i_bit_depth,
							const x265_coeff_t* p_src,
							int32_t* p_des,
							int32_t i_width,
							int32_t i_height,
							int32_t i_scaling_list_type )
{
	int32_t loop = 0 ;
	const x265_coeff_t *p_q_coef = NULL;
	int32_t *p_coef = NULL;
	int32_t i_shift, i_add, i_coeff_q;
	uint32_t i_log2_tr_size = 0;
	int32_t i_transform_shift = 0;
	x265_coeff_t clip_q_coef;
	int32_t *p_dequant_coef = NULL;
	int32_t i_scale = 0;

	p_q_coef = p_src;
	p_coef = p_des;

	//	x265_tr_quant_print_dequant_src (p_src, i_width, i_height ) ;
	if ( i_width > (int32_t)tr_quant->i_max_tr_size )
	{
		i_width  = tr_quant->i_max_tr_size;
		i_height = tr_quant->i_max_tr_size;
	}

	i_log2_tr_size = h->global.convert_to_bit[ i_width ] + 2;
	i_transform_shift = X265_MAX_TR_DYNAMIC_RANGE - i_bit_depth - i_log2_tr_size;

	i_shift = X265_QUANT_IQUANT_SHIFT - X265_QUANT_SHIFT - i_transform_shift;

	if(tr_quant->b_scaling_list_enabled_flag)
	{
		i_shift += 4;
		p_dequant_coef = tr_quant->dequant_coef[i_log2_tr_size - 2][i_scaling_list_type][tr_quant->qp_param.i_rem];

		if(i_shift > tr_quant->qp_param.i_per)
		{
			i_add = 1 << (i_shift - tr_quant->qp_param.i_per - 1);

			for( loop = 0; loop < i_width * i_height; loop++ )
			{
				clip_q_coef = x265_clip3_int32( p_q_coef[loop], -32768, 32767 );
				i_coeff_q = ((clip_q_coef * p_dequant_coef[loop]) + i_add ) >> (i_shift -  tr_quant->qp_param.i_per);
				p_coef[loop] = x265_clip3_int32(i_coeff_q, -32768, 32767);
			}
		}
		else
		{
			for( loop = 0; loop < i_width*i_height; loop++ )
			{
				clip_q_coef = x265_clip3_int32( p_q_coef[loop], -32768, 32767 );
				i_coeff_q   = x265_clip3_int32( clip_q_coef * p_dequant_coef[loop], -32768, 32767 ); // clip to avoid possible overflow in following shift left operation
				p_coef[loop] = x265_clip3_int32( i_coeff_q << (tr_quant->qp_param.i_per - i_shift), -32768, 32767 );
			}
		}
	}
	else
	{
		i_add = 1 << (i_shift-1);
		i_scale = inv_quant_scales[tr_quant->qp_param.i_rem] << tr_quant->qp_param.i_per;

		for( loop = 0; loop < i_width*i_height; loop++ )
		{
			clip_q_coef = x265_clip3_int32( p_q_coef[loop], -32768, 32767 );
			i_coeff_q = ( clip_q_coef * i_scale + i_add ) >> i_shift;
			p_coef[loop] = x265_clip3_int32(i_coeff_q, -32768, 32767);
		}
	}

	//	x265_tr_quant_print_dequant_des (p_des, i_width, i_height ) ;

}

int x265_tr_quant_initialize ( x265_t *h, x265_tr_quant_t *tr_quant )
{
	tr_quant->i_max_tr_size = (1 << h->param.sps.i_quadtree_tu_log2_max_size) ;
	tr_quant->b_enc = 1 ;
	tr_quant->b_use_rdoq = h->param.rdo.b_use_rdo_q ;
	tr_quant->b_use_rdoq_ts = h->param.rdo.b_use_rdo_q_ts ;

#if X265_ADAPTIVE_QP_SELECTION
	tr_quant->b_use_adapt_qp_select = h->param.b_use_adapt_qp_select ;
#endif
	tr_quant->b_use_transform_skip_fast = h->param.b_use_transform_skip_fast ;

	return 0 ;
}

void x265_tr_quant_deinitialize ( x265_t *h, x265_tr_quant_t *tr_quant )
{

}

void x265_tr_quant_my_x_t(x265_t *h,
						x265_tr_quant_t *tr_quant,
						int32_t bit_depth,
						uint32_t i_mode,
						spixel* p_blk_resi,
						uint32_t i_stride,
						int32_t* p_coeff,
						int32_t i_width,
						int32_t i_height )
{
	int64_t i_start_time = 0 ;
	int64_t i_end_time = 0 ;
	int32_t temp_coeff[1024] ;

	i_start_time = x265_get_timer_state () ;
	i_end_time = x265_get_timer_state () ;
	add_timer_status_2 ( i_end_time - i_start_time ) ;
	i_start_time = x265_get_timer_state () ;
	x265_tr_quant_x_t(h,
					tr_quant,
					bit_depth,
					i_mode,
					p_blk_resi,
					i_stride,
					temp_coeff,
					i_width,
					i_height ) ;
	i_end_time = x265_get_timer_state () ;
	add_timer_status_1 ( i_end_time - i_start_time ) ;
	if ( memory_compare_int32(p_coeff, i_width, temp_coeff, i_width, i_width, i_height ) )
	{
		fprintf ( stderr, "Partial Butter Fly 32 Error\n" ) ;
		exit (0) ;
	}
}

void x265_tr_quant_transform_nxn(x265_t *h,
									x265_tr_quant_t *tr_quant,
									x265_data_cu_t* cu,
									spixel *p_residual,
									uint32_t i_stride,
									x265_coeff_t *p_coeff,
#if X265_ADAPTIVE_QP_SELECTION
									int32_t **pp_arl_coeff,
#endif
									uint32_t i_width,
									uint32_t i_height,
									uint32_t *p_abs_sum,
									enum text_type_e i_text_type,
									uint32_t i_abs_part_idx,
									int32_t b_use_transform_skip
									)
{
	uint32_t j = 0, k = 0 ;
	uint32_t i_mode;  //luma intra pred
	int32_t i_bit_depth = 0;
	int32_t i_index = 0 ;
	x265_transform_t *transform = NULL ;

	//	x265_tr_quant_print_quant_residual (p_residual, i_stride, i_width, i_height ) ;
	if (x265_base_data_cu_get_cu_transquant_bypass_p2((x265_base_data_cu_t*)cu, i_abs_part_idx))
	{
		*p_abs_sum=0;
		for ( k = 0; k<i_height; k++)
		{
			for ( j = 0; j<i_width; j++)
			{
				p_coeff[k*i_width+j]= p_residual[k*i_stride+j];
				*p_abs_sum += abs(p_residual[k*i_stride+j]);
			}
		}
		return;
	}
	if(i_text_type == TEXT_LUMA
			&& x265_base_data_cu_get_prediction_mode_p2((x265_base_data_cu_t*)cu, i_abs_part_idx) == MODE_INTRA )
	{
		i_mode = x265_base_data_cu_get_luma_intra_dir_p2((x265_base_data_cu_t*)cu, i_abs_part_idx );
	}
	else
	{
		i_mode = X265_REG_DCT;
	}

	*p_abs_sum = 0;
	assert( ( h->sps[0].i_max_tr_size >= i_width) );
	i_bit_depth = i_text_type == TEXT_LUMA
									? h->param.sps.i_bit_depth_y
									: h->param.sps.i_bit_depth_c;

	i_index = h->global.convert_to_bit[i_width] ;
	transform = &tr_quant->transform ;
	if(b_use_transform_skip)
	{
		x265_tr_quant_x_transform_skip(h,
										tr_quant,
										i_bit_depth,
										p_residual,
										i_stride,
										tr_quant->temp_coeff,
										i_width,
										i_height );
	}
	else
	{
		if ( i_width > 4 )
		{
			transform->tr[i_index](p_residual,
									i_stride,
									tr_quant->temp_coeff,
									i_bit_depth) ;
		}
		else
		{
			x265_tr_quant_x_t(h,
							tr_quant,
							i_bit_depth,
							i_mode,
							p_residual,
							i_stride,
							tr_quant->temp_coeff,
							i_width,
							i_height );
		}
	}

	x265_tr_quant_x_quant( h,
							tr_quant,
							cu,
							tr_quant->temp_coeff,
							p_coeff,
#if X265_ADAPTIVE_QP_SELECTION
							pp_arl_coeff,
#endif
							i_width,
							i_height,
							p_abs_sum,
							i_text_type,
							i_abs_part_idx );
}


void x265_tr_quant_my_x_it(x265_tr_quant_t *tr_quant,
						int32_t i_bit_depth,
						uint32_t i_mode,
						int32_t* p_coef,
						spixel* p_residual,
						uint32_t i_stride,
						int32_t i_width,
						int32_t i_height )
{
	int64_t i_start_time = 0 ;
	int64_t i_end_time = 0 ;
	spixel residual[8192] ;

	i_start_time = x265_get_timer_state () ;
	i_end_time = x265_get_timer_state () ;
	add_timer_status_2 ( i_end_time - i_start_time ) ;
	i_start_time = x265_get_timer_state () ;
	x265_tr_quant_x_it(tr_quant,
						i_bit_depth,
						i_mode,
						p_coef,
						residual,
						i_stride,
						i_width,
						i_height ) ;
	i_end_time = x265_get_timer_state () ;
	add_timer_status_1 ( i_end_time - i_start_time ) ;
	if ( memory_compare_int16(p_residual, i_stride, residual, i_stride, i_width, i_height ) )
	{
		fprintf ( stderr, "Partial Butter Fly 32 Error\n" ) ;
		exit (0) ;
	}
}

void x265_tr_quant_inv_transform_nxn(x265_t *h,
									x265_tr_quant_t *tr_quant,
									int32_t b_trans_quant_bypass,
									enum text_type_e i_text_type,
									uint32_t i_mode,
									spixel *p_residual,
									uint32_t i_stride,
									x265_coeff_t *p_coeff,
									uint32_t i_width,
									uint32_t i_height,
									int32_t i_scaling_list_type,
									int32_t b_use_transform_skip )
{
	uint32_t k = 0 ;
	uint32_t j = 0 ;
	int32_t i_bit_depth = 0 ;
	int32_t i_index = 0 ;
	x265_transform_t *transform = NULL ;

	if(b_trans_quant_bypass)
	{
		for (k = 0; k<i_height; k++)
		{
			for (j = 0; j<i_width; j++)
			{
				p_residual[k*i_stride+j] = p_coeff[k*i_width+j];
			}
		}
		return;
	}
	i_bit_depth = i_text_type == TEXT_LUMA
					? h->param.sps.i_bit_depth_y
					: h->param.sps.i_bit_depth_c;
	i_index = h->global.convert_to_bit[i_width] ;
	transform = &tr_quant->transform ;

	x265_tr_quant_x_dequant(h,
							tr_quant,
							i_bit_depth,
							p_coeff,
							tr_quant->temp_coeff,
							i_width,
							i_height,
							i_scaling_list_type);
	if(b_use_transform_skip)
	{
		x265_tr_quant_x_i_transform_skip(h,
										tr_quant,
										i_bit_depth,
										tr_quant->temp_coeff,
										p_residual,
										i_stride,
										i_width,
										i_height );
	}
	else
	{
		if (i_width > 4)
		{
			transform->itr[i_index](p_residual,
									i_stride,
									tr_quant->temp_coeff,
									i_bit_depth) ;
		}
		else
		{
			x265_tr_quant_x_it(tr_quant,
							i_bit_depth,
							i_mode,
							tr_quant->temp_coeff,
							p_residual,
							i_stride,
							i_width,
							i_height );
		}
	}

	/*
	x265_tr_quant_print_dequant_residual (p_residual,
											i_stride,
											i_width,
											i_height ) ;
											*/
}

void x265_tr_quant_inv_recur_transform_nxn(x265_t *h,
											x265_tr_quant_t *tr_quant,
											x265_data_cu_t *cu,
											uint32_t i_abs_part_idx,
											enum text_type_e i_text_type,
											spixel* p_residual,
											uint32_t i_addr,
											uint32_t i_stride,
											uint32_t i_width,
											uint32_t i_height,
											uint32_t i_max_tr_mode,
											uint32_t i_tr_mode,
											x265_coeff_t* p_coeff )
{
	uint32_t i_stop_tr_mode = 0;
	uint32_t i_depth = 0 ;
	uint32_t i_log2_tr_size = 0 ;
	uint32_t i_q_pdiv = 0 ;
	spixel* p_resi = NULL;
	int32_t i_scaling_list_type = 0 ;
	int32_t i_tr_width = 0, i_tr_height = 0;
	uint32_t i_addr_offset = 0 ;
	uint32_t i_coef_offset = 0 ;
	uint32_t i_part_offset = 0 ;

	if( !x265_base_data_cu_get_cbf_p4((x265_base_data_cu_t*)cu,
										i_abs_part_idx,
										i_text_type,
										i_tr_mode) )
	{
		return;
	}
	i_stop_tr_mode = x265_base_data_cu_get_transform_idx_p2((x265_base_data_cu_t*)cu,
															i_abs_part_idx );

	if( i_tr_mode == i_stop_tr_mode )
	{
		i_depth = x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_part_idx ) + i_tr_mode;
		i_log2_tr_size = h->global.convert_to_bit[ h->sps[0].i_max_cu_width >> i_depth ] + 2;
		if( i_text_type != TEXT_LUMA && i_log2_tr_size == 2 )
		{
			i_q_pdiv = x265_base_data_cu_get_total_num_part((x265_base_data_cu_t*)cu) >> ( ( i_depth - 1 ) << 1 );
			if( ( i_abs_part_idx % i_q_pdiv ) != 0 )
			{
				return;
			}
			i_width  <<= 1;
			i_height <<= 1;
		}
		p_resi = p_residual + i_addr;
		i_scaling_list_type = (x265_base_data_cu_is_intra((x265_base_data_cu_t*)cu, i_abs_part_idx)
								? 0 : 3) + et_table[(int32_t)i_text_type];
		assert(i_scaling_list_type < 6);
		x265_tr_quant_inv_transform_nxn(h,
										tr_quant,
										x265_base_data_cu_get_cu_transquant_bypass_p2((x265_base_data_cu_t*)cu, i_abs_part_idx),
										i_text_type,
										X265_REG_DCT,
										p_resi,
										i_stride,
										p_coeff,
										i_width,
										i_height,
										i_scaling_list_type,
										x265_data_cu_get_transform_skip_p3(cu, i_abs_part_idx, i_text_type)
										) ;
	}
	else
	{
		i_tr_mode++;
		i_width  >>= 1;
		i_height >>= 1;
		i_tr_width = i_width ;
		i_tr_height = i_height;
		i_addr_offset = i_tr_height * i_stride;
		i_coef_offset = i_tr_width * i_tr_height;
		i_part_offset = x265_base_data_cu_get_total_num_part((x265_base_data_cu_t*)cu) >> ( i_tr_mode << 1 );
		{
			x265_tr_quant_inv_recur_transform_nxn(h,
												tr_quant,
												cu,
												i_abs_part_idx,
												i_text_type,
												p_residual,
												i_addr,
												i_stride,
												i_width,
												i_height,
												i_max_tr_mode,
												i_tr_mode,
												p_coeff
												) ;
			p_coeff += i_coef_offset;
			i_abs_part_idx += i_part_offset;
			x265_tr_quant_inv_recur_transform_nxn(h,
												tr_quant,
												cu,
												i_abs_part_idx,
												i_text_type,
												p_residual,
												i_addr + i_tr_width,
												i_stride,
												i_width,
												i_height,
												i_max_tr_mode,
												i_tr_mode,
												p_coeff );
			p_coeff += i_coef_offset;
			i_abs_part_idx += i_part_offset;
			x265_tr_quant_inv_recur_transform_nxn(h,
												tr_quant,
												cu,
												i_abs_part_idx,
												i_text_type,
												p_residual,
												i_addr + i_addr_offset,
												i_stride,
												i_width,
												i_height,
												i_max_tr_mode,
												i_tr_mode,
												p_coeff );
			p_coeff += i_coef_offset;
			i_abs_part_idx += i_part_offset;
			x265_tr_quant_inv_recur_transform_nxn(h,
												tr_quant,
												cu,
												i_abs_part_idx,
												i_text_type,
												p_residual,
												i_addr + i_addr_offset + i_tr_width,
												i_stride,
												i_width,
												i_height,
												i_max_tr_mode,
												i_tr_mode, p_coeff );
		}
	}
}

// ------------------------------------------------------------------------------------------------
// logical transform
// ------------------------------------------------------------------------------------------------

/** wrapper function between hm interface and core nx_n forward transform (2d)
 *  \param p_blk_resi input data (residual)
 *  \param ps_coeff output data (transform coefficients)
 *  \param i_stride stride of input residual data
 *  \param i_size transform size (i_size x i_size)
 *  \param i_mode is int32_tra prediction mode used in mode-dependent dct/dst only
 */
void x265_tr_quant_x_t(x265_t *h,
						x265_tr_quant_t *tr_quant,
						int32_t bit_depth,
						uint32_t i_mode,
						spixel* p_blk_resi,
						uint32_t i_stride,
						int32_t* p_coeff,
						int32_t i_width,
						int32_t i_height )
{
#if X265_MATRIX_MULT
	int32_t i_size = i_width;
	x_tr(bit_depth, p_blk_resi,ps_coeff,i_stride,(uint32_t)i_size,i_mode);
#else
	int32_t j;
	{
		short block[ 64 * 64 ];
		short coeff[ 64 * 64 ];
		{
			for (j = 0; j < i_height; j++)
			{
				memcpy( block + j * i_width, p_blk_resi + j * i_stride, i_width * sizeof( short ) );
			}
		}
		x_tr_mxn(h, bit_depth, block, coeff, i_width, i_height, i_mode );
		for ( j = 0; j < i_height * i_width; j++ )
		{
			p_coeff[ j ] = coeff[ j ];
		}
		return ;
	}
#endif
}


/** wrapper function between hm interface and core nx_n inverse transform (2d)
 *  \param pl_coef input data (transform coefficients)
 *  \param p_residual output data (residual)
 *  \param i_stride stride of input residual data
 *  \param i_size transform size (i_size x i_size)
 *  \param i_mode is int32_tra prediction mode used in mode-dependent dct/dst only
 */
void x265_tr_quant_x_it(x265_tr_quant_t *tr_quant,
						int32_t i_bit_depth,
						uint32_t i_mode,
						int32_t* p_coef,
						spixel* p_residual,
						uint32_t i_stride,
						int32_t i_width,
						int32_t i_height )
{
#if X265_MATRIX_MULT
	int32_t i_size = i_width;
	x_i_tr(i_bit_depth, pl_coef,p_residual,i_stride,(uint32_t)i_size,i_mode);
#else
	int32_t j;
	{
		short block[ 64 * 64 ];
		short coeff[ 64 * 64 ];
		for ( j = 0; j < i_height * i_width; j++ )
		{
			coeff[j] = (short)p_coef[j];
		}
		x_i_tr_mxn(i_bit_depth, coeff, block, i_width, i_height, i_mode );
		{
			for ( j = 0; j < i_height; j++ )
			{
				memcpy( p_residual + j * i_stride, block + j * i_width, i_width * sizeof(short) );
			}
		}
		return ;
	}
#endif
}

/** wrapper function between hm interface and core 4x4 transform skipping
 *  \param p_blk_resi input data (residual)
 *  \param ps_coeff output data (transform coefficients)
 *  \param i_stride stride of input residual data
 *  \param i_size transform size (i_size x i_size)
 */
void x265_tr_quant_x_transform_skip(x265_t *h,
									x265_tr_quant_t *tr_quant,
									int32_t bit_depth,
									spixel* p_blk_resi,
									uint32_t i_stride,
									int32_t* ps_coeff,
									int32_t width,
									int32_t height )
{
	assert( width == height );
	uint32_t i_log2_tr_size = h->global.convert_to_bit[ width ] + 2;
	int32_t  shift = X265_MAX_TR_DYNAMIC_RANGE - bit_depth - i_log2_tr_size;
	uint32_t transform_skip_shift;
	int32_t  j,k;
	if(shift >= 0)
	{
		transform_skip_shift = shift;
		for (j = 0; j < height; j++)
		{
			for(k = 0; k < width; k ++)
			{
				ps_coeff[j*height + k] = p_blk_resi[j * i_stride + k] << transform_skip_shift;
			}
		}
	}
	else
	{
		//the case when i_bit_depth > 13
		int32_t offset;
		transform_skip_shift = -shift;
		offset = (1 << (transform_skip_shift - 1));
		for (j = 0; j < height; j++)
		{
			for(k = 0; k < width; k ++)
			{
				ps_coeff[j*height + k] = (p_blk_resi[j * i_stride + k] + offset) >> transform_skip_shift;
			}
		}
	}
}

/** wrapper function between hm interface and core nx_n transform skipping
 *  \param pl_coef input data (coefficients)
 *  \param p_residual output data (residual)
 *  \param i_stride stride of input residual data
 *  \param i_size transform size (i_size x i_size)
 */
void x265_tr_quant_x_i_transform_skip(x265_t *h,
										x265_tr_quant_t *tr_quant,
										int32_t i_bit_depth,
										int32_t* p_coef,
										spixel* p_residual,
										uint32_t i_stride,
										int32_t i_width,
										int32_t i_height )
{
	assert( i_width == i_height );
	uint32_t i_log2_tr_size = h->global.convert_to_bit[ i_width ] + 2;
	int32_t  shift = X265_MAX_TR_DYNAMIC_RANGE - i_bit_depth - i_log2_tr_size;
	uint32_t transform_skip_shift;
	int32_t  j,k;
	if(shift > 0)
	{
		int32_t offset;
		transform_skip_shift = shift;
		offset = (1 << (transform_skip_shift -1));
		for ( j = 0; j < i_height; j++ )
		{
			for(k = 0; k < i_width; k ++)
			{
				p_residual[j * i_stride + k] = (p_coef[j*i_width+k] + offset) >> transform_skip_shift;
			}
		}
	}
	else
	{
		//the case when i_bit_depth >= 13
		transform_skip_shift = - shift;
		for ( j = 0; j < i_height; j++ )
		{
			for(k = 0; k < i_width; k ++)
			{
				p_residual[j * i_stride + k] = p_coef[j*i_width+k] << transform_skip_shift;
			}
		}
	}
}


void x265_tr_quant_x_rate_dist_opt_quant (x265_t *h,
										x265_tr_quant_t *tr_quant,
										x265_data_cu_t* cu,
										int32_t* p_src_coeff,
										x265_coeff_t *p_dst_coeff,
#if X265_ADAPTIVE_QP_SELECTION
                                        int32_t **pp_arl_dst_coeff,
#endif
                                        uint32_t i_width,
                                        uint32_t i_height,
                                        uint32_t *p_abs_sum,
                                        enum text_type_e i_text_type,
                                        uint32_t i_abs_part_idx )
{
	int64_t i_start_time = 0 ;
	int64_t i_end_time = 0 ;

	i_start_time = x265_get_timer_state () ;

	int32_t loop = 0;
	int32_t i_q_bits = 0;
	uint32_t i_log2_tr_size = 0;
	int32_t i_q = 0;
	uint32_t i_bit_depth = 0;
	int32_t i_transform_shift = 0;
	uint32_t i_go_rice_param = 0;
	uint32_t i_log2_blk_size = 0;
	uint32_t i_max_num_coeff = 0;
	int32_t i_scaling_list_type = 0 ;
	int32_t *p_q_coef_org = NULL;
	int32_t *p_q_coef = NULL;
#if X265_ADAPTIVE_QP_SELECTION
	int32_t i_q_bits_c = 0;
	int32_t i_add_c =  0;
#endif
	uint32_t i_scan_idx = 0;
	int32_t rate_inc_up   [ 32 * 32 ];
	int32_t rate_inc_down [ 32 * 32 ];
	int32_t sig_rate_delta[ 32 * 32 ];
	int32_t delta_u      [ 32 * 32 ];
	uint32_t *scan_cg = NULL;
	uint32_t i_cg_size = 0;
	uint32_t sig_coeff_group_flag[ X265_MLS_GRP_NUM ];
	uint32_t i_num_blk_side = 0;
	int32_t i_cg_last_scan_pos = 0;
	uint32_t i_ctx_set = 0;
	int32_t i_c1 = 0;
	int32_t i_c2 = 0;
	int32_t i_last_scan_pos = 0;
	uint32_t i_c1_idx = 0;
	uint32_t i_c2_idx = 0;
	int32_t i_base_level = 0;
	uint32_t *scan = NULL;
	uint32_t i_cg_num = 0;
	int32_t i_scan_pos = 0;
	x265_coeff_group_rd_stats_t rd_stats;
	int32_t i_cg_scan_pos = 0 ;
	uint32_t i_cg_blk_pos = 0;
	uint32_t i_cg_pos_y = 0;
	uint32_t i_cg_pos_x = 0;
	int32_t i_pattern_sig_ctx = 0;
	int32_t i_scan_posin_cg = 0;
	uint32_t i_blk_pos = 0;
	int32_t i_level_double = 0;
	uint32_t i_max_abs_level = 0;
	uint32_t i_level = 0;
	uint32_t i_one_ctx = 0;
	uint32_t i_abs_ctx = 0;
	uint32_t i_pos_y = 0;
	uint32_t i_pos_x = 0;
	uint16_t i_ctx_sig = 0;
	int32_t i_rate_now = 0;
	int32_t i_ctx_cbf = 0;
	int32_t i_best_last_idx_p1 = 0;
	int32_t b_found_last = 0;
	int32_t i_rd_factor = 0;
	int32_t i_last_cg = 0;
	int32_t i_abs_sum = 0 ;
	int32_t i_sub_set = 0;
	int32_t i_sub_pos = 0;
	int32_t i_first_nz_pos_in_cg = 0;
	int32_t i_last_nz_pos_in_cg = 0 ;
	uint32_t i_sign_bit = 0;
	int64_t i_min_cost_inc = 0, i_cur_cost = 0;
	int32_t i_min_pos = 0, i_final_change = 0, i_cur_change=0;
	int32_t i_cost_up = 0;
	int32_t i_cost_down = 0;
	uint32_t i_this_sign_bit = 0;

	double f_temp = 0.0;
	double f_block_uncoded_cost = 0.0;
	double f_base_cost = 0.0;
	double f_err = 0.0;
	double f_cost_zero_cg = 0.0;
	double f_best_cost = 0.0;
	double f_cost_last= 0.0;
	double f_total_cost = 0.0;
	double *p_err_scale_org = NULL;
	double *p_err_scale = NULL;
	double cost_coeff [ 32 * 32 ];
	double cost_sig   [ 32 * 32 ];
	double cost_coeff0[ 32 * 32 ];
	double cost_coeff_group_sig[ X265_MLS_GRP_NUM ];

	i_q_bits = tr_quant->qp_param.i_bits;
	i_log2_tr_size = h->global.convert_to_bit[ i_width ] + 2;
	i_q = quant_scales[tr_quant->qp_param.i_rem];
	i_bit_depth = i_text_type == TEXT_LUMA ? h->param.sps.i_bit_depth_y : h->param.sps.i_bit_depth_c;
	i_transform_shift = X265_MAX_TR_DYNAMIC_RANGE - i_bit_depth - i_log2_tr_size;  // represents scaling through forward transform
	i_log2_blk_size = h->global.convert_to_bit[ i_width ] + 2;
	i_max_num_coeff = i_width * i_height;
	i_scaling_list_type = (x265_base_data_cu_is_intra((x265_base_data_cu_t*)cu, i_abs_part_idx) ? 0 : 3) + et_table[(int32_t)i_text_type];
	assert(i_scaling_list_type < 6);

	i_q_bits = X265_QUANT_SHIFT + tr_quant->qp_param.i_per + i_transform_shift;                   // right shift of non-rdoq quantizer;  level = (coeff*i_q + offset)>>q_bits

	p_err_scale_org = tr_quant->err_scale[i_log2_tr_size - 2][i_scaling_list_type][tr_quant->qp_param.i_rem] ;
	p_q_coef_org = tr_quant->quant_coef[i_log2_tr_size-2][i_scaling_list_type][tr_quant->qp_param.i_rem] ;
	p_q_coef = p_q_coef_org;
	p_err_scale = p_err_scale_org;
#if X265_ADAPTIVE_QP_SELECTION
	i_q_bits_c = i_q_bits - X265_ARL_C_PRECISION ;
	i_add_c =  1 << (i_q_bits_c-1);
#endif
	i_scan_idx = x265_data_cu_get_coef_scan_idx(h,
												cu,
												i_abs_part_idx,
												i_width,
												i_text_type == TEXT_LUMA,
												x265_base_data_cu_is_intra((x265_base_data_cu_t*)cu, i_abs_part_idx));
#if X265_ADAPTIVE_QP_SELECTION
	memset(*pp_arl_dst_coeff, 0, sizeof(int32_t) *  i_max_num_coeff);
#endif
	memset( cost_coeff, 0, sizeof(double) *  i_max_num_coeff );
	memset( cost_sig,   0, sizeof(double) *  i_max_num_coeff );

	memset( rate_inc_up,    0, sizeof(int32_t) *  i_max_num_coeff );
	memset( rate_inc_down,  0, sizeof(int32_t) *  i_max_num_coeff );
	memset( sig_rate_delta, 0, sizeof(int32_t) *  i_max_num_coeff );
	memset( delta_u,       0, sizeof(int32_t) *  i_max_num_coeff );

	scan_cg = h->scan.sig_last_scan[ i_scan_idx ][ i_log2_blk_size > 3 ? i_log2_blk_size-2-1 : 0  ];
	if( i_log2_blk_size == 3 )
	{
		scan_cg = h->scan.sig_last_scan_8x8[ i_scan_idx ];
	}
	else if( i_log2_blk_size == 5 )
	{
		scan_cg = h->scan.sig_last_scan_cg_32x32;
	}

	i_cg_size = (1 << X265_MLS_CG_SIZE);         // 16
	i_num_blk_side = i_width / X265_MLS_CG_SIZE;
	i_cg_last_scan_pos = -1;
	i_c1 = 1;
	i_last_scan_pos = -1;
	scan = h->scan.sig_last_scan[ i_scan_idx ][ i_log2_blk_size - 1 ];
	memset( cost_coeff_group_sig,   0, sizeof(double) * X265_MLS_GRP_NUM );
	memset( sig_coeff_group_flag,   0, sizeof(uint32_t) * X265_MLS_GRP_NUM );
	i_cg_num = i_width * i_height >> X265_MLS_CG_SIZE ;
	for (i_cg_scan_pos = i_cg_num-1; i_cg_scan_pos >= 0; i_cg_scan_pos--)
	{
		i_cg_blk_pos = scan_cg[ i_cg_scan_pos ];
	    i_cg_pos_y = i_cg_blk_pos / i_num_blk_side;
	    i_cg_pos_x = i_cg_blk_pos - (i_cg_pos_y * i_num_blk_side);
	    memset( &rd_stats, 0, sizeof (x265_coeff_group_rd_stats_t));

	    i_pattern_sig_ctx = x265_tr_quant_calc_pattern_sig_ctx(tr_quant,
	    														sig_coeff_group_flag,
	    														i_cg_pos_x,
	    														i_cg_pos_y,
	    														i_width,
	    														i_height);
	    for ( i_scan_posin_cg = i_cg_size - 1;
	    		i_scan_posin_cg >= 0; i_scan_posin_cg-- )
	    {
	    	i_scan_pos = i_cg_scan_pos * i_cg_size + i_scan_posin_cg;
	    	//===== quantization =====
	    	i_blk_pos = scan[i_scan_pos];
	    	// set coeff
	    	i_q  = p_q_coef[i_blk_pos];
	    	f_temp = p_err_scale[i_blk_pos];
	    	i_level_double = p_src_coeff[ i_blk_pos ];
	    	i_level_double = (int32_t)X265_MIN((int32_t)abs((int32_t)i_level_double) * i_q,
	    										X265_MAX_INT - (1 << (i_q_bits - 1)));

#if X265_ADAPTIVE_QP_SELECTION
	    	if( tr_quant->b_use_adapt_qp_select )
	    	{
	    		*pp_arl_dst_coeff[i_blk_pos]   = (int32_t)((i_level_double + i_add_c) >> i_q_bits_c);
	    	}
#endif
	    	i_max_abs_level = (i_level_double + (1 << (i_q_bits - 1))) >> i_q_bits;
	    	f_err = (double)(i_level_double);
	    	cost_coeff0[ i_scan_pos ] = f_err * f_err * f_temp;
	    	f_block_uncoded_cost += cost_coeff0[ i_scan_pos ];
	    	p_dst_coeff[ i_blk_pos ]    = i_max_abs_level;

	    	if ( i_max_abs_level > 0 && i_last_scan_pos < 0 )
	    	{
	    		i_last_scan_pos = i_scan_pos;
	    		i_ctx_set = (i_scan_pos < X265_SCAN_SET_SIZE || i_text_type != TEXT_LUMA) ? 0 : 2;
	    		i_cg_last_scan_pos = i_cg_scan_pos;
	    	}

	    	if ( i_last_scan_pos >= 0 )
	    	{
	    		//===== coefficient level estimation =====
	    		i_one_ctx = 4 * i_ctx_set + i_c1;
	    		i_abs_ctx = i_ctx_set + i_c2;

	    		if( i_scan_pos == i_last_scan_pos )
	    		{
	    			i_level = x265_tr_quant_x_get_coded_level( tr_quant,
	    														&cost_coeff[ i_scan_pos ],
	    														&cost_coeff0[ i_scan_pos ],
	    														&cost_sig[ i_scan_pos ],
	    														i_level_double,
	    														i_max_abs_level,
	    														0,
	    														i_one_ctx,
	    														i_abs_ctx,
	    														i_go_rice_param,
	    														i_c1_idx,
	    														i_c2_idx,
	    														i_q_bits,
	    														f_temp,
	    														1 );
	    		}
	    		else
	    		{
	    			i_pos_y = i_blk_pos >> i_log2_blk_size;
	    			i_pos_x = i_blk_pos - ( i_pos_y << i_log2_blk_size );
	    			i_ctx_sig = x265_tr_quant_get_sig_ctx_inc( tr_quant,
	    														i_pattern_sig_ctx,
	    														i_scan_idx,
	    														i_pos_x,
	    														i_pos_y,
	    														i_log2_blk_size,
	    														i_text_type );
	    			i_level = x265_tr_quant_x_get_coded_level( tr_quant,
	    														&cost_coeff[ i_scan_pos ],
	    														&cost_coeff0[ i_scan_pos ],
	    														&cost_sig[ i_scan_pos ],
	    														i_level_double,
	    														i_max_abs_level,
	    														i_ctx_sig,
	    														i_one_ctx,
	    														i_abs_ctx,
	    														i_go_rice_param,
	    														i_c1_idx,
	    														i_c2_idx,
	    														i_q_bits,
	    														f_temp,
	    														0 );
	    			sig_rate_delta[ i_blk_pos ] = tr_quant->est_bits_sbac->significant_bits[ i_ctx_sig ][ 1 ]
	    			                            - tr_quant->est_bits_sbac->significant_bits[ i_ctx_sig ][ 0 ];
	    		}

	            //	print_double_state (cost_coeff[ i_scan_pos ]) ;

	    		delta_u[ i_blk_pos ] = (i_level_double - ((int32_t)i_level << i_q_bits)) >> (i_q_bits-8);
	    		if( i_level > 0 )
	    		{
	    			i_rate_now = x265_tr_quant_x_get_ic_rate(tr_quant,
	    													i_level,
	    													i_one_ctx,
	    													i_abs_ctx,
	    													i_go_rice_param,
	    													i_c1_idx,
	    													i_c2_idx );
	    			rate_inc_up   [ i_blk_pos ] = x265_tr_quant_x_get_ic_rate(tr_quant,
	    																		i_level + 1,
	    																		i_one_ctx,
	    																		i_abs_ctx,
	    																		i_go_rice_param,
	    																		i_c1_idx,
	    																		i_c2_idx )
	    											- i_rate_now;
	    			rate_inc_down [ i_blk_pos ] = x265_tr_quant_x_get_ic_rate(tr_quant,
	    																		i_level-1,
	    																		i_one_ctx,
	    																		i_abs_ctx,
	    																		i_go_rice_param,
	    																		i_c1_idx,
	    																		i_c2_idx )
	    											- i_rate_now;
	    		}
	    		else // i_level == 0
	    		{
	    			rate_inc_up   [ i_blk_pos ] = tr_quant->est_bits_sbac->greater_one_bits[ i_one_ctx ][ 0 ];
	    		}
	    		p_dst_coeff[ i_blk_pos ] = i_level;
	    		f_base_cost += cost_coeff [ i_scan_pos ];

	    		i_base_level = (i_c1_idx < X265_C1FLAG_NUMBER)
								? (2 + (i_c2_idx < X265_C2FLAG_NUMBER)) : 1;
	    		if( i_level >= i_base_level )
	    		{
	    			if(i_level  > 3*(1<<i_go_rice_param))
	    			{
	    				i_go_rice_param = X265_MIN(i_go_rice_param+ 1, 4);
	    			}
	    		}
	    		if ( i_level >= 1)
	    		{
	    			i_c1_idx ++;
	    		}

	    		//===== update bin model =====
	    		if( i_level > 1 )
	    		{
	    			i_c1 = 0;
	    			i_c2 += (i_c2 < 2);
	    			i_c2_idx ++;
	    		}
	    		else if( (i_c1 < 3) && (i_c1 > 0) && i_level)
	    		{
	    			i_c1++;
	    		}

	    		//===== context set update =====
	    		if( ( i_scan_pos % X265_SCAN_SET_SIZE == 0 ) && ( i_scan_pos > 0 ) )
	    		{
	    			i_c2 = 0;
	    			i_go_rice_param = 0;

	    			i_c1_idx = 0;
	    			i_c2_idx = 0;
	    			i_ctx_set = (i_scan_pos == X265_SCAN_SET_SIZE
	    						|| i_text_type != TEXT_LUMA) ? 0 : 2;
	    			if( i_c1 == 0 )
	    			{
	    				i_ctx_set++;
	    			}
	    			i_c1 = 1;
	    		}
	    	}
	    	else
	    	{
	    		f_base_cost += cost_coeff0[ i_scan_pos ];
	    	}
	    	rd_stats.f_sig_cost += cost_sig[ i_scan_pos ];
	    	if (i_scan_posin_cg == 0 )
	    	{
	    		rd_stats.f_sig_cost_0 = cost_sig[ i_scan_pos ];
	    	}
	    	if (p_dst_coeff[ i_blk_pos ] )
	    	{
	    		sig_coeff_group_flag[ i_cg_blk_pos ] = 1;
	    		rd_stats.f_coded_leveland_dist += cost_coeff[ i_scan_pos ] - cost_sig[ i_scan_pos ];
	    		rd_stats.f_uncoded_dist += cost_coeff0[ i_scan_pos ];
	    		if ( i_scan_posin_cg != 0 )
	    		{
	    			rd_stats.i_nnz_before_pos0++;
	    		}
	    	}
	    } //end for (i_scan_posin_cg)

	    if (i_cg_last_scan_pos >= 0)
	    {
	    	if( i_cg_scan_pos )
	    	{
	    		if (sig_coeff_group_flag[ i_cg_blk_pos ] == 0)
	    		{
	    			i_ctx_sig = x265_tr_quant_get_sig_coeff_group_ctx_inc(tr_quant,
	    																sig_coeff_group_flag,
	    																i_cg_pos_x,
	    																i_cg_pos_y,
	    																i_width,
	    																i_height);
	    			f_base_cost += x265_tr_quant_x_get_rate_sig_coeff_group(tr_quant,
	    																	0,
	    																	i_ctx_sig)
									- rd_stats.f_sig_cost;;
	    			cost_coeff_group_sig[ i_cg_scan_pos ] = x265_tr_quant_x_get_rate_sig_coeff_group(tr_quant,
	    																							0,
	    																							i_ctx_sig);
	    		}
	    		else
	    		{
	    			if (i_cg_scan_pos < i_cg_last_scan_pos) //skip the last coefficient group, which will be handled together with last position below.
	    			{
	    				if ( rd_stats.i_nnz_before_pos0 == 0 )
	    				{
	    					f_base_cost -= rd_stats.f_sig_cost_0;
	    					rd_stats.f_sig_cost -= rd_stats.f_sig_cost_0;
	    				}
	    				// rd-cost if sig_coeff_group_flag = 0, initialization
	    				f_cost_zero_cg = f_base_cost;

	    				// add sig_coeff_group_flag cost to total cost
	    				i_ctx_sig = x265_tr_quant_get_sig_coeff_group_ctx_inc(tr_quant,
	    																		sig_coeff_group_flag,
	    																		i_cg_pos_x,
	    																		i_cg_pos_y,
	    																		i_width,
	    																		i_height);
	    				if (i_cg_scan_pos < i_cg_last_scan_pos)
	    				{
	    					f_base_cost  += x265_tr_quant_x_get_rate_sig_coeff_group(tr_quant,
	    																			1,
	    																			i_ctx_sig);
	    					f_cost_zero_cg += x265_tr_quant_x_get_rate_sig_coeff_group(tr_quant,
	    																				0,
	    																				i_ctx_sig);
	    					cost_coeff_group_sig[ i_cg_scan_pos ] = x265_tr_quant_x_get_rate_sig_coeff_group(tr_quant,
	    																									1,
	    																									i_ctx_sig);
	    				}

	    				// try to convert the current coeff group from non-zero to all-zero
	    				f_cost_zero_cg += rd_stats.f_uncoded_dist;  // distortion for resetting non-zero levels to zero levels
	    				f_cost_zero_cg -= rd_stats.f_coded_leveland_dist;   // distortion and level cost for keeping all non-zero levels
	    				f_cost_zero_cg -= rd_stats.f_sig_cost;     // sig cost for all coeffs, including zero levels and non-zerl levels

	    				// if we can save cost, change this block to all-zero block
	    				if ( f_cost_zero_cg < f_base_cost )
	    				{
	    					sig_coeff_group_flag[ i_cg_blk_pos ] = 0;
	    					f_base_cost = f_cost_zero_cg;
	    					if (i_cg_scan_pos < i_cg_last_scan_pos)
	    					{
	    						cost_coeff_group_sig[ i_cg_scan_pos ] = x265_tr_quant_x_get_rate_sig_coeff_group(tr_quant,
	    																										0,
	    																										i_ctx_sig);
	    					}
	    					// reset coeffs to 0 in this block
	    					for ( i_scan_posin_cg = i_cg_size-1; i_scan_posin_cg >= 0; i_scan_posin_cg--)
	    					{
	    						i_scan_pos = i_cg_scan_pos*i_cg_size + i_scan_posin_cg;
	    						i_blk_pos = scan[ i_scan_pos ];

	    						if (p_dst_coeff[ i_blk_pos ])
	    						{
	    							p_dst_coeff [ i_blk_pos ] = 0;
	    							cost_coeff[ i_scan_pos ] = cost_coeff0[ i_scan_pos ];
	    							cost_sig  [ i_scan_pos ] = 0;
	    						}
	    					}
	    				} // end if ( f_cost_all_zeros < f_base_cost )
	    			}
	    		} // end if if (sig_coeff_group_flag[ i_cg_blk_pos ] == 0)
	    	}
	    	else
	    	{
	    		sig_coeff_group_flag[ i_cg_blk_pos ] = 1;
	    	}
	    }
	} //end for (i_cg_scan_pos)

	//===== estimate last position =====
	if ( i_last_scan_pos < 0 )
	{
		return;
	}


	if( !x265_base_data_cu_is_intra((x265_base_data_cu_t*)cu, i_abs_part_idx )
		&& i_text_type == TEXT_LUMA
		&& x265_base_data_cu_get_transform_idx_p2((x265_base_data_cu_t*)cu, i_abs_part_idx ) == 0 )
	{
		f_best_cost  = f_block_uncoded_cost + x265_tr_quant_x_get_i_cost(tr_quant,
																		tr_quant->est_bits_sbac->block_root_cbp_bits[ i_ctx_cbf ][ 0 ] );
		f_base_cost += x265_tr_quant_x_get_i_cost(tr_quant,
												tr_quant->est_bits_sbac->block_root_cbp_bits[ i_ctx_cbf ][ 1 ] );
	}
	else
	{
		i_ctx_cbf = x265_data_cu_get_ctx_qt_cbf(cu,
												i_text_type,
												x265_base_data_cu_get_transform_idx_p2((x265_base_data_cu_t*)cu, i_abs_part_idx ) );
		i_ctx_cbf = ( i_text_type ? TEXT_CHROMA : i_text_type ) * X265_NUM_QT_CBF_CTX + i_ctx_cbf;
		f_best_cost  = f_block_uncoded_cost + x265_tr_quant_x_get_i_cost(tr_quant,
																		tr_quant->est_bits_sbac->block_cbp_bits[ i_ctx_cbf ][ 0 ] );
		f_base_cost += x265_tr_quant_x_get_i_cost(tr_quant,
												tr_quant->est_bits_sbac->block_cbp_bits[ i_ctx_cbf ][ 1 ] );
	}

	b_found_last = 0;
	for (i_cg_scan_pos = i_cg_last_scan_pos; i_cg_scan_pos >= 0; i_cg_scan_pos--)
	{
	    i_cg_blk_pos = scan_cg[ i_cg_scan_pos ];

	    f_base_cost -= cost_coeff_group_sig [ i_cg_scan_pos ];
	    if (sig_coeff_group_flag[ i_cg_blk_pos ])
	    {
	    	for (i_scan_posin_cg = i_cg_size-1; i_scan_posin_cg >= 0; i_scan_posin_cg--)
	    	{
	    		i_scan_pos = i_cg_scan_pos * i_cg_size + i_scan_posin_cg;
	    		if (i_scan_pos > i_last_scan_pos)
	    		{
	    			continue;
	    		}
	    		i_blk_pos = scan[i_scan_pos];

	    		if( p_dst_coeff[ i_blk_pos ] )
	    		{
	    			i_pos_y = i_blk_pos >> i_log2_blk_size;
	    			i_pos_x = i_blk_pos - ( i_pos_y << i_log2_blk_size );

	    			f_cost_last = i_scan_idx == SCAN_VER
	    										? x265_tr_quant_x_get_rate_last(tr_quant,
	    																		i_pos_y,
	    																		i_pos_x )
	    										: x265_tr_quant_x_get_rate_last(tr_quant,
	    																		i_pos_x,
	    																		i_pos_y );
	    			f_total_cost = f_base_cost + f_cost_last - cost_sig[ i_scan_pos ];

	    			if( f_total_cost < f_best_cost )
	    			{
	    				i_best_last_idx_p1 = i_scan_pos + 1;
	    				f_best_cost = f_total_cost;
	    			}
	    			if( p_dst_coeff[ i_blk_pos ] > 1 )
	    			{
	    				b_found_last = 1;
	    				break;
	    			}
	    			f_base_cost -= cost_coeff[ i_scan_pos ];
	    			f_base_cost += cost_coeff0[ i_scan_pos ];
	    		}
	    		else
	    		{
	    			f_base_cost -= cost_sig[ i_scan_pos ];
	    		}
	    	} //end for
	    	if (b_found_last)
	    	{
	    		break;
	    	}
	    } // end if (sig_coeff_group_flag[ i_cg_blk_pos ])
	} // end for

	for ( i_scan_pos = 0; i_scan_pos < i_best_last_idx_p1; i_scan_pos++ )
	{
	    i_blk_pos = scan[ i_scan_pos ];
	    i_level  = p_dst_coeff[ i_blk_pos ];
	    *p_abs_sum += i_level;
	    p_dst_coeff[ i_blk_pos ] = ( p_src_coeff[ i_blk_pos ] < 0 ) ? -i_level : i_level;
	}

	//===== clean uncoded coefficients =====
	for ( i_scan_pos = i_best_last_idx_p1; i_scan_pos <= i_last_scan_pos; i_scan_pos++ )
	{
		p_dst_coeff[ scan[ i_scan_pos ] ] = 0;
	}

	if( h->pps[0].i_sign_hide_flag && *p_abs_sum>=2)
	{
		i_rd_factor = (int64_t) (
	                     inv_quant_scales[tr_quant->qp_param.i_rem]
	                     * inv_quant_scales[tr_quant->qp_param.i_rem]
	                     * (1 << (2 * tr_quant->qp_param.i_per))
	                     / tr_quant->f_lambda / 16 / (1 << X265_DISTORTION_PRECISION_ADJUSTMENT(2 * (i_bit_depth - 8)))
	                     + 0.5);
		i_last_cg = -1;

	    for( i_sub_set = (i_width*i_height-1) >> X265_LOG2_SCAN_SET_SIZE;
	    		i_sub_set >= 0; i_sub_set-- )
	    {
	    	i_sub_pos = i_sub_set << X265_LOG2_SCAN_SET_SIZE;
	    	i_first_nz_pos_in_cg = X265_SCAN_SET_SIZE ;
	    	i_last_nz_pos_in_cg = -1 ;
	    	i_abs_sum = 0 ;

	    	for( loop = X265_SCAN_SET_SIZE - 1; loop >= 0; --loop )
	    	{
	    		if( p_dst_coeff[ scan[ loop + i_sub_pos ]] )
	    		{
	    			i_last_nz_pos_in_cg = loop;
	    			break;
	    		}
	    	}

	    	for(loop = 0; loop < X265_SCAN_SET_SIZE ; loop++ )
	    	{
	    		if( p_dst_coeff[ scan[ loop + i_sub_pos ]] )
	    		{
	    			i_first_nz_pos_in_cg = loop;
	    			break;
	    		}
	    	}

	    	for(loop = i_first_nz_pos_in_cg; loop <= i_last_nz_pos_in_cg; loop++ )
	    	{
	    		i_abs_sum += p_dst_coeff[ scan[ loop + i_sub_pos ]];
	    	}

	    	if(i_last_nz_pos_in_cg>=0 && i_last_cg==-1)
	    	{
	    		i_last_cg = 1;
	    	}

	    	if( i_last_nz_pos_in_cg - i_first_nz_pos_in_cg >= X265_SBH_THRESHOLD )
	    	{
	    		i_sign_bit = (p_dst_coeff[scan[i_sub_pos+i_first_nz_pos_in_cg]]>0?0:1);
	    		if( i_sign_bit!=(i_abs_sum&0x1) )  // hide but need tune
	    		{
	    			// calculate the cost
	    			i_min_cost_inc = X265_MAX_INT64 ;
	    			i_cur_cost = X265_MAX_INT64;
	    			i_min_pos = -1 ;
	    			i_final_change = 0 ;
	    			i_cur_change = 0 ;

	    			for( loop = (i_last_cg==1 ? i_last_nz_pos_in_cg : X265_SCAN_SET_SIZE - 1) ;
	    					loop >= 0; --loop )
	    			{
	    				i_blk_pos = scan[ loop + i_sub_pos ];
	    				if(p_dst_coeff[ i_blk_pos ] != 0 )
	    				{
	    					i_cost_up = i_rd_factor * ( - delta_u[i_blk_pos] ) + rate_inc_up[i_blk_pos] ;
	    					i_cost_down = i_rd_factor * (   delta_u[i_blk_pos] ) + rate_inc_down[i_blk_pos]
	    					              -   ( abs(p_dst_coeff[i_blk_pos])==1?((1<<15)+sig_rate_delta[i_blk_pos]):0 );

	    					if(i_last_cg==1 && i_last_nz_pos_in_cg==loop && abs(p_dst_coeff[i_blk_pos])==1)
	    					{
	    						i_cost_down -= (4<<15) ;
	    					}

	    					if(i_cost_up < i_cost_down)
	    					{
	    						i_cur_cost = i_cost_up;
	    						i_cur_change =  1 ;
	    					}
	    					else
	    					{
	    						i_cur_change = -1 ;
	    						if(loop==i_first_nz_pos_in_cg && abs(p_dst_coeff[i_blk_pos])==1)
	    						{
	    							i_cur_cost = X265_MAX_INT64 ;
	    						}
	    						else
	    						{
	    							i_cur_cost = i_cost_down ;
	    						}
	    					}
	    				}
	    				else
	    				{
	    					i_cur_cost = i_rd_factor * ( - (abs(delta_u[i_blk_pos])) ) + (1<<15) + rate_inc_up[i_blk_pos] + sig_rate_delta[i_blk_pos] ;
	    					i_cur_change = 1 ;

	    					if(loop<i_first_nz_pos_in_cg)
	    					{
	    						i_this_sign_bit = (p_src_coeff[i_blk_pos] >= 0 ? 0 : 1 ) ;
	    						if(i_this_sign_bit != i_sign_bit )
	    						{
	    							i_cur_cost = X265_MAX_INT64;
	    						}
	    					}
	    				}

	    				if( i_cur_cost<i_min_cost_inc)
	    				{
	    					i_min_cost_inc = i_cur_cost ;
	    					i_final_change = i_cur_change ;
	    					i_min_pos = i_blk_pos ;
	    				}
	    			}

	    			if(p_q_coef[i_min_pos] == 32767 || p_q_coef[i_min_pos] == -32768)
	    			{
	    				i_final_change = -1;
	    			}

	    			if(p_src_coeff[i_min_pos]>=0)
	    			{
	    				p_dst_coeff[i_min_pos] += i_final_change ;
	    			}
	    			else
	    			{
	    				p_dst_coeff[i_min_pos] -= i_final_change ;
	    			}
	    		}
	    	}

	    	if(i_last_cg==1)
	    	{
	    		i_last_cg=0 ;
	    	}
	    }
	}

	i_end_time = x265_get_timer_state () ;
	add_part_time ( i_end_time - i_start_time ) ;
}



int32_t x265_tr_quant_calc_pattern_sig_ctx( x265_tr_quant_t *tr_quant,
											const uint32_t* sig_coeff_group_flag,
											uint32_t i_pos_x_cg,
											uint32_t i_pos_y_cg,
											int32_t i_width,
											int32_t i_height )
{
	uint32_t i_sig_right = 0;
	uint32_t i_sig_lower = 0;

	if( i_width == 4 && i_height == 4 )
	{
	//	return -1;
	}
	i_width >>= 2;
	i_height >>= 2;
	if( i_pos_x_cg < i_width - 1 )
	{
		i_sig_right = (sig_coeff_group_flag[ i_pos_y_cg * i_width + i_pos_x_cg + 1 ] != 0);
	}
	if( i_pos_y_cg < i_height - 1 )
	{
		i_sig_lower = (sig_coeff_group_flag[(i_pos_y_cg + 1) * i_width + i_pos_x_cg ] != 0);
	}
	return i_sig_right + (i_sig_lower<<1);
}

int32_t x265_tr_quant_get_sig_ctx_inc( x265_tr_quant_t *tr_quant,
										int32_t i_pattern_sig_ctx,
										uint32_t i_scan_idx,
										int32_t i_pos_x,
										int32_t i_pos_y,
										int32_t i_log2_block_size,
										enum text_type_e i_text_type )
{
	int32_t i_offset = 0 ;
	int32_t i_pos_x_in_subset = 0 ;
	int32_t i_pos_y_in_subset = 0 ;
	int32_t i_cnt = 0;

	const int32_t ctx_ind_map[16] =
	{
		0, 1, 4, 5,
		2, 3, 4, 5,
		6, 6, 8, 8,
		7, 7, 8, 8
	};

	if( i_pos_x + i_pos_y == 0 )
	{
		return 0;
	}

	if ( i_log2_block_size == 2 )
	{
		return ctx_ind_map[ 4 * i_pos_y + i_pos_x ];
	}

	i_offset = i_log2_block_size == 3 ? (i_scan_idx==SCAN_DIAG ? 9 : 15) : (i_text_type == TEXT_LUMA ? 21 : 12);
	i_pos_x_in_subset = i_pos_x - ((i_pos_x >> 2) << 2);
	i_pos_y_in_subset = i_pos_y - ((i_pos_y >> 2) << 2);
	i_cnt = 0;
	if (i_pattern_sig_ctx == 0)
	{
		i_cnt = i_pos_x_in_subset + i_pos_y_in_subset <= 2
				? (i_pos_x_in_subset + i_pos_y_in_subset == 0 ? 2 : 1)
						: 0;
	}
	else if(i_pattern_sig_ctx == 1)
	{
		i_cnt = i_pos_y_in_subset <= 1 ? (i_pos_y_in_subset == 0 ? 2 : 1) : 0;
	}
	else if(i_pattern_sig_ctx == 2)
	{
		i_cnt = i_pos_x_in_subset <= 1 ? (i_pos_x_in_subset == 0 ? 2 : 1) : 0;
	}
	else
	{
		i_cnt = 2;
	}

	return (( i_text_type == TEXT_LUMA && ((i_pos_x>>2) + (i_pos_y>>2)) > 0 ) ? 3 : 0)
			+ i_offset + i_cnt;
}


/** Get the best level in RD sense
 * \param rd64CodedCost reference to coded cost
 * \param rd64CodedCost0 reference to cost when coefficient is 0
 * \param rd64CodedCostSig reference to cost of significant coefficient
 * \param lLevelDouble reference to unscaled quantized level
 * \param i_max_abs_level scaled quantized level
 * \param ui16CtxNumSig current ctxInc for coeff_abs_significant_flag
 * \param i_ctx_num_one current ctxInc for coeff_abs_level_greater1 (1st bin of coeff_abs_level_minus1 in AVC)
 * \param i_ctx_num_abs current ctxInc for coeff_abs_level_greater2 (remaining bins of coeff_abs_level_minus1 in AVC)
 * \param i_abs_go_rice current Rice parameter for coeff_abs_level_minus3
 * \param iQBits quantization step size
 * \param f_temp correction factor
 * \param bLast indicates if the coefficient is the last significant
 * \returns best quantized transform level for given scan position
 * This method calculates the best quantized transform level for a given scan position.
 */
uint32_t x265_tr_quant_x_get_coded_level ( x265_tr_quant_t *tr_quant,
									double *p_rd64_coded_cost,
									double *p_rd64_coded_cost0,
									double *p_rd64_coded_cost_sig,
									int32_t i_level_double,
									uint32_t i_max_abs_level,
									uint16_t i_ctx_num_sig,
									uint16_t i_ctx_num_one,
									uint16_t i_ctx_num_abs,
									uint16_t i_abs_go_rice,
									uint32_t i_c1_idx,
									uint32_t i_c2_idx,
									int32_t i_q_bits,
									double f_temp,
									int32_t b_last )
{
	uint32_t i_best_abs_level = 0;
	uint32_t i_min_abs_level = 0;
	int32_t i_abs_level = 0;
	double f_err = 0 ;
	double f_curr_cost = 0.0;
	double f_curr_cost_sig = 0;

	if( !b_last && i_max_abs_level < 3 )
	{
		*p_rd64_coded_cost_sig = tr_quant->f_lambda
								* tr_quant->est_bits_sbac->significant_bits[i_ctx_num_sig][0] ;
		*p_rd64_coded_cost = *p_rd64_coded_cost0 + *p_rd64_coded_cost_sig;
		if( i_max_abs_level == 0 )
		{
			return i_best_abs_level;
		}
	}
	else
	{
		*p_rd64_coded_cost = X265_MAX_DOUBLE;
	}

	if( !b_last )
	{
		f_curr_cost_sig = x265_tr_quant_x_get_rate_sig_coef(tr_quant,
															1,
															i_ctx_num_sig );
	}

	i_min_abs_level = ( i_max_abs_level > 1 ? i_max_abs_level - 1 : 1 );
	for( i_abs_level = i_max_abs_level;
			i_abs_level >= i_min_abs_level ;
			i_abs_level-- )
	{
		f_err = (double)( i_level_double - ( i_abs_level << i_q_bits ) );
		f_curr_cost = f_err * f_err * f_temp +
						x265_tr_quant_x_get_ic_rate_cost(tr_quant,
														i_abs_level,
														i_ctx_num_one,
														i_ctx_num_abs,
														i_abs_go_rice,
														i_c1_idx,
														i_c2_idx );
		f_curr_cost += f_curr_cost_sig ;


		if( f_curr_cost < *p_rd64_coded_cost )
		{
			i_best_abs_level = i_abs_level;
			*p_rd64_coded_cost = f_curr_cost;
			*p_rd64_coded_cost_sig = f_curr_cost_sig;
		}
	}

	return i_best_abs_level;
}

/** Calculates the cost for specific absolute transform level
 * \param i_abs_level scaled quantized level
 * \param i_ctx_num_one current ctxInc for coeff_abs_level_greater1 (1st bin of coeff_abs_level_minus1 in AVC)
 * \param i_ctx_num_abs current ctxInc for coeff_abs_level_greater2 (remaining bins of coeff_abs_level_minus1 in AVC)
 * \param i_abs_go_rice Rice parameter for coeff_abs_level_minus3
 * \returns cost of given absolute transform level
 */
double x265_tr_quant_x_get_ic_rate_cost ( x265_tr_quant_t *tr_quant,
										uint32_t i_abs_level,
										uint16_t i_ctx_num_one,
										uint16_t i_ctx_num_abs,
										uint16_t i_abs_go_rice,
										uint32_t i_c1_idx,
										uint32_t i_c2_idx )
{
	double f_rate = x265_tr_quant_x_get_iep_rate(tr_quant);
	uint32_t i_base_level = 0;
	uint32_t i_symbol = 0;
	uint32_t i_length = 0;

	i_base_level = (i_c1_idx < X265_C1FLAG_NUMBER)? (2 + (i_c2_idx < X265_C2FLAG_NUMBER)) : 1;
	if ( i_abs_level >= i_base_level )
	{
		i_symbol = i_abs_level - i_base_level;

		if (i_symbol < (X265_COEF_REMAIN_BIN_REDUCTION << i_abs_go_rice))
		{
			f_rate += ((i_symbol>>i_abs_go_rice)+1+i_abs_go_rice)<< 15;
		}
		else
		{
			i_length = i_abs_go_rice;
			i_symbol  = i_symbol - ( X265_COEF_REMAIN_BIN_REDUCTION << i_abs_go_rice);
			while (i_symbol >= (1<<i_length))
			{
				i_symbol -=  (1<<(i_length++));
			}
			f_rate += (X265_COEF_REMAIN_BIN_REDUCTION
						+ i_length + 1 - i_abs_go_rice + i_length ) << 15;
		}
		if (i_c1_idx < X265_C1FLAG_NUMBER)
		{
			f_rate += tr_quant->est_bits_sbac->greater_one_bits[ i_ctx_num_one ][ 1 ];

			if (i_c2_idx < X265_C2FLAG_NUMBER)
			{
				f_rate += tr_quant->est_bits_sbac->level_abs_bits[ i_ctx_num_abs ][ 1 ];
			}
		}
	}
	else
	{
		if( i_abs_level == 1 )
		{
			f_rate += tr_quant->est_bits_sbac->greater_one_bits[ i_ctx_num_one ][ 0 ];
		}
		else if( i_abs_level == 2 )
		{
			f_rate += tr_quant->est_bits_sbac->greater_one_bits[ i_ctx_num_one ][ 1 ];
			f_rate += tr_quant->est_bits_sbac->level_abs_bits[ i_ctx_num_abs ][ 0 ];
		}
		else
		{
			assert (0);
		}
	}
    return tr_quant->f_lambda * f_rate ;
}


int32_t x265_tr_quant_x_get_ic_rate ( x265_tr_quant_t *tr_quant,
									uint32_t i_abs_level,
									uint16_t i_ctx_num_one,
									uint16_t i_ctx_num_abs,
									uint16_t i_abs_go_rice,
									uint32_t i_c1_idx,
									uint32_t i_c2_idx )
{
	int32_t i_rate = 0;
	uint32_t i_base_level = 0;
    uint32_t i_symbol = 0;
    uint32_t i_max_vlc = 0;
    int32_t b_exp_golomb = 0;
    int32_t i_egs = 0 ;
    uint32_t i_max = 0 ;
    uint16_t i_pref_len = 0;
    uint16_t i_num_bins = 0;


	i_base_level = (i_c1_idx < X265_C1FLAG_NUMBER) ? (2 + (i_c2_idx < X265_C2FLAG_NUMBER)) : 1;
	if ( i_abs_level >= i_base_level )
	{
		i_symbol = i_abs_level - i_base_level;
		i_max_vlc = go_rice_range[ i_abs_go_rice ];
		b_exp_golomb = ( i_symbol > i_max_vlc );

		if( b_exp_golomb )
		{
			i_abs_level = i_symbol - i_max_vlc;
			i_egs = 1;
			for( i_max = 2; i_abs_level >= i_max; i_max <<= 1, i_egs += 2 );
			i_rate += i_egs << 15;
			i_symbol = X265_MIN ( i_symbol, ( i_max_vlc + 1 ) );
		}

		i_pref_len = (uint16_t)( i_symbol >> i_abs_go_rice ) + 1;
    	i_num_bins = X265_MIN( i_pref_len, go_rice_prefix_len[ i_abs_go_rice ] ) + i_abs_go_rice;

    	i_rate += i_num_bins << 15;

    	if (i_c1_idx < X265_C1FLAG_NUMBER)
    	{
    		i_rate += tr_quant->est_bits_sbac->greater_one_bits[ i_ctx_num_one ][ 1 ];

    		if (i_c2_idx < X265_C2FLAG_NUMBER)
    		{
    			i_rate += tr_quant->est_bits_sbac->level_abs_bits[ i_ctx_num_abs ][ 1 ];
    		}
    	}
	}
	else
	{
		if( i_abs_level == 0 )
		{
			return 0;
		}
		else if( i_abs_level == 1 )
		{
			i_rate += tr_quant->est_bits_sbac->greater_one_bits[ i_ctx_num_one ][ 0 ];
		}
		else if( i_abs_level == 2 )
		{
			i_rate += tr_quant->est_bits_sbac->greater_one_bits[ i_ctx_num_one ][ 1 ];
			i_rate += tr_quant->est_bits_sbac->level_abs_bits[ i_ctx_num_abs ][ 0 ];
		}
		else
		{
			assert(0);
		}
	}
	return i_rate;
}

double x265_tr_quant_x_get_rate_sig_coeff_group ( x265_tr_quant_t *tr_quant,
												uint16_t i_significance_coeff_group,
                                                uint16_t i_ctx_num_sig )
{
	return x265_tr_quant_x_get_i_cost( tr_quant,
										tr_quant->est_bits_sbac->significant_coeff_group_bits[ i_ctx_num_sig ][ i_significance_coeff_group ] ) ;
}

/** Calculates the cost of signaling the last significant coefficient in the block
 * \param uiPosX X coordinate of the last significant coefficient
 * \param uiPosY Y coordinate of the last significant coefficient
 * \returns cost of last significant coefficient
 */
/*
 * \param ui_width width of the transform unit (TU)
*/
double x265_tr_quant_x_get_rate_last ( x265_tr_quant_t *tr_quant,
										const uint32_t i_pos_x,
										const uint32_t i_pos_y )
{
	uint32_t i_ctx_x = 0;
	uint32_t i_ctx_y = 0;
	double f_cost = 0.0;

	i_ctx_x = group_idx[i_pos_x];
	i_ctx_y = group_idx[i_pos_y];
	f_cost = tr_quant->est_bits_sbac->last_x_bits[ i_ctx_x ]
	         + tr_quant->est_bits_sbac->last_y_bits[ i_ctx_y ];

	if( i_ctx_x > 3 )
	{
		f_cost += x265_tr_quant_x_get_iep_rate( tr_quant ) * ((i_ctx_x-2)>>1);
	}
	if( i_ctx_y > 3 )
	{
		f_cost += x265_tr_quant_x_get_iep_rate( tr_quant ) * ((i_ctx_y-2)>>1);
	}
	return x265_tr_quant_x_get_i_cost( tr_quant, f_cost );
}

 /** Calculates the cost for specific absolute transform level
 * \param i_abs_level scaled quantized level
 * \param i_ctx_num_one current ctxInc for coeff_abs_level_greater1 (1st bin of coeff_abs_level_minus1 in AVC)
 * \param i_ctx_num_abs current ctxInc for coeff_abs_level_greater2 (remaining bins of coeff_abs_level_minus1 in AVC)
 * \param ui16CtxBase current global offset for coeff_abs_level_greater1 and coeff_abs_level_greater2
 * \returns cost of given absolute transform level
 */
double x265_tr_quant_x_get_rate_sig_coef  ( x265_tr_quant_t *tr_quant,
											uint16_t i_significance,
											uint16_t i_ctx_num_sig )
{
	return x265_tr_quant_x_get_i_cost( tr_quant,
										tr_quant->est_bits_sbac->significant_bits[i_ctx_num_sig][i_significance] );
}

/** Get the cost for a specific rate
 * \param dRate rate of a bit
 * \returns cost at the specific rate
 */
double x265_tr_quant_x_get_i_cost ( x265_tr_quant_t *tr_quant, double f_rate )
{
	return tr_quant->f_lambda * f_rate;
}

/** Get the cost of an equal probable bit
 * \returns cost of equal probable bit
 */
double x265_tr_quant_x_get_iep_rate ( x265_tr_quant_t *tr_quant )
{
	return 32768;
}

uint32_t x265_tr_quant_get_sig_coeff_group_ctx_inc( x265_tr_quant_t *tr_quant,
													const uint32_t *sig_coeff_group_flag,
													const uint32_t i_cg_pos_x,
													const uint32_t i_cg_pos_y,
													int32_t i_width,
													int32_t i_height)
{
	uint32_t i_right = 0;
	uint32_t i_lower = 0;

	i_width >>= 2;
	i_height >>= 2;
	if( i_cg_pos_x < i_width - 1 )
	{
		i_right = (sig_coeff_group_flag[i_cg_pos_y * i_width + i_cg_pos_x + 1] != 0);
	}
	if (i_cg_pos_y < i_height - 1 )
	{
		i_lower = (sig_coeff_group_flag[(i_cg_pos_y  + 1 ) * i_width + i_cg_pos_x] != 0);
	}
	return (i_right || i_lower);
}

void x265_tr_quant_set_scaling_list( x265_t* h, x265_tr_quant_t *tr_quant, x265_scaling_list_t *scaling_list )
{
	uint32_t size,list;
	uint32_t qp;

	for(size=0;size<SCALING_LIST_SIZE_NUM;size++)
	{
		for(list = 0; list < x265_scaling_list_get_scaling_list_num(size); list++)
		{
			for(qp=0;qp<SCALING_LIST_REM_NUM;qp++)
			{
				x265_tr_quant_x_set_scaling_list_enc( h, tr_quant, scaling_list,list,size,qp);
				x265_tr_quant_x_set_scaling_list_dec( h, tr_quant, scaling_list,list,size,qp);
				x265_tr_quant_set_err_scale_coeff( h, tr_quant, list, size, qp ) ;
			}
		}
	}
}

void x265_tr_quant_set_scaling_list_dec( x265_t* h, x265_tr_quant_t *tr_quant, x265_scaling_list_t *scaling_list )
{
	uint32_t size,list;
	uint32_t qp;

	for(size=0;size<SCALING_LIST_SIZE_NUM;size++)
	{
		for(list = 0; list < x265_scaling_list_get_scaling_list_num(size); list++)
		{
			for(qp=0;qp<SCALING_LIST_REM_NUM;qp++)
			{
				x265_tr_quant_x_set_scaling_list_dec(h,tr_quant,scaling_list,list,size,qp);
			}
		}
	}
}

void x265_tr_quant_set_err_scale_coeff( x265_t* h,
										x265_tr_quant_t *tr_quant,
										uint32_t list,
										uint32_t size,
										uint32_t qp )
{
	uint32_t i_log_2_tr_size = h->global.convert_to_bit[ x265_scaling_list_get_scaling_list_size_x(size) ] + 2;
	int32_t i_bit_depth = (size < SCALING_LIST_32x32 && list != 0 && list != 3) ? h->param.sps.i_bit_depth_c : h->param.sps.i_bit_depth_y;
	int32_t i_transform_shift = X265_MAX_TR_DYNAMIC_RANGE - i_bit_depth - i_log_2_tr_size;  // Represents scaling through forward transform
	uint32_t i = 0, i_max_num_coeff = 0 ;
	int32_t *quant_coeff;
	double *err_scale;
	double f_err_scale = 0.0 ;

	i_max_num_coeff = x265_scaling_list_get_scaling_list_size(size);
	quant_coeff = tr_quant->quant_coef[size][list][qp] ;
	err_scale = tr_quant->err_scale[size][list][qp] ; ;

	f_err_scale = (double)(1<<X265_SCALE_BITS);                              // Compensate for scaling of bitcount in Lagrange cost function
	f_err_scale = f_err_scale*pow(2.0,-2.0*i_transform_shift);                     // Compensate for scaling through forward transform
	for(i=0;i<i_max_num_coeff;i++)
	{
		err_scale[i] = f_err_scale / quant_coeff[i] / quant_coeff[i] / (1<<X265_DISTORTION_PRECISION_ADJUSTMENT(2*(i_bit_depth-8)));
	}
}

void x265_tr_quant_x_set_scaling_list_enc( x265_t* h,
											x265_tr_quant_t *tr_quant,
											x265_scaling_list_t *scaling_list,
											uint32_t list_id,
											uint32_t size_id,
											uint32_t qp )
{
	uint32_t i_width = x265_scaling_list_get_scaling_list_size_x(size_id);
	uint32_t i_height = x265_scaling_list_get_scaling_list_size_x(size_id);
	uint32_t ratio = x265_scaling_list_get_scaling_list_size_x(size_id)
						/ X265_MIN ( X265_MAX_MATRIX_SIZE_NUM,
						(int32_t)x265_scaling_list_get_scaling_list_size_x(size_id));
	int32_t *quant_coeff;
	int32_t *coeff = scaling_list->scaling_list_coef[size_id][list_id];
	quant_coeff = tr_quant->quant_coef[size_id][list_id][qp];

	x265_tr_quant_process_scaling_list_enc ( coeff,
											quant_coeff,
											quant_scales[qp]<<4,
											i_height,
											i_width,
											ratio,
											X265_MIN( X265_MAX_MATRIX_SIZE_NUM,
											(int32_t)x265_scaling_list_get_scaling_list_size_x(size_id) ),
											scaling_list->scaling_list_dc[size_id][list_id] ) ;
}

void x265_tr_quant_x_set_scaling_list_dec( x265_t* h,
											x265_tr_quant_t *tr_quant,
											x265_scaling_list_t *scaling_list,
											uint32_t list_id,
											uint32_t size_id,
											uint32_t qp )
{
	uint32_t i_width = x265_scaling_list_get_scaling_list_size_x(size_id);
	uint32_t i_height = x265_scaling_list_get_scaling_list_size_x(size_id);
	uint32_t ratio = x265_scaling_list_get_scaling_list_size_x(size_id)
						/ X265_MIN ( X265_MAX_MATRIX_SIZE_NUM,
						(int32_t)x265_scaling_list_get_scaling_list_size_x(size_id));
	int32_t *dequant_coeff;
	int32_t *coeff = scaling_list->scaling_list_coef[size_id][list_id] ;

	dequant_coeff = tr_quant->dequant_coef[size_id][list_id][qp];
	x265_tr_quant_process_scaling_list_dec ( coeff,
											dequant_coeff,
											inv_quant_scales[qp],
											i_height,
											i_width,
											ratio,
											X265_MIN( X265_MAX_MATRIX_SIZE_NUM,
											(int32_t)x265_scaling_list_get_scaling_list_size_x(size_id) ),
											scaling_list->scaling_list_dc[size_id][list_id] );
}


void x265_tr_quant_set_flat_scaling_list( x265_t* h, x265_tr_quant_t *tr_quant )
{
	uint32_t size,list;
	uint32_t qp;

	for(size=0;size<SCALING_LIST_SIZE_NUM;size++)
	{
		for(list = 0; list < x265_scaling_list_get_scaling_list_num(size); list++)
		{
			for(qp=0;qp<SCALING_LIST_REM_NUM;qp++)
			{
				x265_tr_quant_x_set_flat_scaling_list(tr_quant,list,size,qp);
				x265_tr_quant_set_err_scale_coeff( h, tr_quant, list, size, qp ) ;
			}
		}
	}
}

void x265_tr_quant_x_set_flat_scaling_list( x265_tr_quant_t *tr_quant, uint32_t list, uint32_t size, uint32_t qp )
{
	uint32_t i = 0 , num = 0 ;
	int32_t *quant_coeff;
	int32_t *dequant_coeff;
	int32_t quant_scale = quant_scales[qp];
	int32_t inv_quant_scale = inv_quant_scales[qp]<<4;

	num = x265_scaling_list_get_scaling_list_size(size) ;
	quant_coeff = tr_quant->quant_coef[size][list][qp] ;
	dequant_coeff = tr_quant->dequant_coef[size][list][qp] ;

	for(i=0;i<num;i++)
	{
		*quant_coeff++ = quant_scale;
		*dequant_coeff++ = inv_quant_scale;
	}
}


void x265_tr_quant_process_scaling_list_enc( int32_t *coeff, int32_t *quant_coeff, int32_t quant_scale, uint32_t i_height, uint32_t i_width, uint32_t ratio, int32_t sizu_num, uint32_t dc)
{
	int32_t nsqth = (i_height < i_width) ? 4: 1; //i_height ratio for NSQT
	int32_t nsqtw = (i_width < i_height) ? 4: 1; //i_width ratio for NSQT
	for(uint32_t j=0;j<i_height;j++)
	{
		for(uint32_t i=0;i<i_width;i++)
		{
			quant_coeff[j*i_width + i] = quant_scale / coeff[sizu_num * (j * nsqth / ratio) + i * nsqtw /ratio];
		}
	}
	if(ratio > 1)
	{
		quant_coeff[0] = quant_scale / dc;
	}
}


void x265_tr_quant_process_scaling_list_dec( int32_t *coeff, int32_t *dequant_coeff, int32_t inv_quant_scale, uint32_t i_height, uint32_t i_width, uint32_t ratio, int32_t sizu_num, uint32_t dc)
{
	for(uint32_t j=0;j<i_height;j++)
	{
		for(uint32_t i=0;i<i_width;i++)
		{
			dequant_coeff[j*i_width + i] = inv_quant_scale * coeff[sizu_num * (j / ratio) + i / ratio];
		}
	}
	if(ratio > 1)
	{
		dequant_coeff[0] = inv_quant_scale * dc;
	}
}



int x265_tr_quant_init_scaling_list ( x265_tr_quant_t *tr_quant )
{
	uint32_t i_size_id = 0 ;
	uint32_t i_list_id = 0 ;
	uint32_t i_qp = 0 ;

	for( i_size_id = 0; i_size_id < SCALING_LIST_SIZE_NUM; i_size_id++)
	{
		for( i_list_id = 0;
				i_list_id < x265_scaling_list_get_scaling_list_num(i_size_id);
				i_list_id++)
		{
			for( i_qp = 0; i_qp < SCALING_LIST_REM_NUM; i_qp++)
			{
				CHECKED_MALLOCZERO ( tr_quant->quant_coef[i_size_id][i_list_id][i_qp],
									x265_scaling_list_get_scaling_list_size(i_size_id) * sizeof(int32_t) ) ;
				CHECKED_MALLOCZERO ( tr_quant->dequant_coef[i_size_id][i_list_id][i_qp],
									x265_scaling_list_get_scaling_list_size(i_size_id) * sizeof(int32_t) ) ;
				CHECKED_MALLOCZERO ( tr_quant->err_scale[i_size_id][i_list_id][i_qp],
									x265_scaling_list_get_scaling_list_size(i_size_id) * sizeof(double) ) ;
			}
		}
	}

	// alias list [1] as [3].
	for( i_qp = 0; i_qp < SCALING_LIST_REM_NUM; i_qp++)
	{
		tr_quant->quant_coef[SCALING_LIST_32x32][3][i_qp] = tr_quant->quant_coef[SCALING_LIST_32x32][1][i_qp];
		tr_quant->dequant_coef[SCALING_LIST_32x32][3][i_qp] = tr_quant->dequant_coef[SCALING_LIST_32x32][1][i_qp];
		tr_quant->err_scale[SCALING_LIST_32x32][3][i_qp] = tr_quant->err_scale[SCALING_LIST_32x32][1][i_qp];
	}

	return 0 ;

fail:
	x265_tr_quant_destroy_scaling_list ( tr_quant ) ;
	return -1 ;
}

void x265_tr_quant_destroy_scaling_list ( x265_tr_quant_t *tr_quant )
{
	uint32_t i_size_id = 0 ;
	uint32_t i_list_id = 0 ;
	uint32_t i_qp = 0 ;

	for( i_size_id = 0; i_size_id < SCALING_LIST_SIZE_NUM; i_size_id++)
	{
		for( i_list_id = 0;
				i_list_id < x265_scaling_list_get_scaling_list_num(i_size_id);
				i_list_id++)
		{
			for( i_qp = 0; i_qp < SCALING_LIST_REM_NUM; i_qp++)
			{
				x265_free ( tr_quant->quant_coef[i_size_id][i_list_id][i_qp] ) ;
				x265_free ( tr_quant->dequant_coef[i_size_id][i_list_id][i_qp] ) ;
				x265_free ( tr_quant->err_scale[i_size_id][i_list_id][i_qp] ) ;

				tr_quant->quant_coef[i_size_id][i_list_id][i_qp] = NULL ;
				tr_quant->dequant_coef[i_size_id][i_list_id][i_qp] = NULL ;
				tr_quant->err_scale[i_size_id][i_list_id][i_qp] = NULL ;
			}
		}
	}
}







/*
void x265_tr_quant_inv_transform_nxn( x265_t* h, x265_tr_quant_t* tr_quant, int32_t b_trans_quant_bypass, enum text_type e_text_type, uint32_t i_mode,pixel* residual, uint32_t i_stride, TCoeff*   coeff, uint32_t i_width, uint32_t i_height,  int32_t scaling_list_type, int32_t b_use_transform_skip )
{
	if(b_trans_quant_bypass)
	{
		for (uint32_t k = 0; k<i_height; k++)
		{
			  for (uint32_t j = 0; j<i_width; j++)
			  {
				residual[k*i_stride+j] = coeff[k*i_width+j];
			  }
		}
		return;
	}
	int32_t bit_depth = e_text_type == TEXT_LUMA ? h->param.sps.i_bit_depth_y : h->param.sps.i_bit_depth_c;
	xDeQuant(bit_depth, coeff, tr_quant->temp_coeff, i_width, i_height, scaling_list_type);
	if(b_use_transform_skip == 1)
	{
		xITransformSkip(bit_depth, tr_quant->temp_coeff, residual, i_stride, i_width, i_height );
	}
	else
	{
		xIT(bit_depth, i_mode, tr_quant->temp_coeff, residual, i_stride, i_width, i_height );
	}
}

void x265_tr_quant_inv_recur_transform_nxn( x265_t* h, x265_data_cu_t* cu, uint32_t i_abs_part_idx, enum text_type e_text_type, pixel* residual, uint32_t i_addr, uint32_t i_stride, uint32_t i_width, uint32_t i_height, uint32_t i_max_tr_mode, uint32_t i_tr_mode, TCoeff* coeff )
{
	if( !cu->getCbf(i_abs_part_idx, e_text_type, i_tr_mode) )
	{
		return;
	}
	const uint32_t stop_tr_mode = cu->getTransformIdx( i_abs_part_idx );

	if( i_tr_mode == stop_tr_mode )
	{
		uint32_t i_depth      = cu->getDepth( i_abs_part_idx ) + i_tr_mode;
		uint32_t i_log_2_tr_size = h->global.convert_to_bit[ cu->getSlice()->getSPS()->getMaxCUWidth() >> i_depth ] + 2;
		if( e_text_type != TEXT_LUMA && i_log_2_tr_size == 2 )
		{
			uint32_t i_qp_div = cu->getPic()->getNumPartInCU() >> ( ( i_depth - 1 ) << 1 );
			if( ( i_abs_part_idx % i_qp_div ) != 0 )
			{
			  return;
			}
			i_width  <<= 1;
			i_height <<= 1;
		}
		pixel* p_resi = residual + i_addr;
		int32_t scaling_list_type = (cu->isint32_tra(i_abs_part_idx) ? 0 : 3) + ettable[(int32_t)e_text_type];
		assert(scaling_list_type < 6);
		invtransformNxN( cu->getCUTransquantBypass(i_abs_part_idx), e_text_type, X265_REG_DCT, p_resi, i_stride, coeff, i_width, i_height, scaling_list_type, cu->getTransformSkip(i_abs_part_idx, e_text_type) );
	}
	else
	{
		i_tr_mode++;
		i_width  >>= 1;
		i_height >>= 1;
		int32_t trWidth = i_width, trHeight = i_height;
		uint32_t i_addrOffset = trHeight * i_stride;
		uint32_t uiCoefOffset = trWidth * trHeight;
		uint32_t uiPartOffset = cu->getTotalNumPart() >> ( i_tr_mode << 1 );
		{
			invRecurTransformNxN( cu, i_abs_part_idx, e_text_type, residual, i_addr                         , i_stride, i_width, i_height, i_max_tr_mode, i_tr_mode, coeff ); coeff += uiCoefOffset; i_abs_part_idx += uiPartOffset;
			invRecurTransformNxN( cu, i_abs_part_idx, e_text_type, residual, i_addr + trWidth               , i_stride, i_width, i_height, i_max_tr_mode, i_tr_mode, coeff ); coeff += uiCoefOffset; i_abs_part_idx += uiPartOffset;
			invRecurTransformNxN( cu, i_abs_part_idx, e_text_type, residual, i_addr + i_addrOffset          , i_stride, i_width, i_height, i_max_tr_mode, i_tr_mode, coeff ); coeff += uiCoefOffset; i_abs_part_idx += uiPartOffset;
			invRecurTransformNxN( cu, i_abs_part_idx, e_text_type, residual, i_addr + i_addrOffset + trWidth, i_stride, i_width, i_height, i_max_tr_mode, i_tr_mode, coeff );
		}
	}
}

int32_t  x265_tr_quant_calc_pattern_sig_ctx( const uint32_t* sig_coeff_group_flag, uint32_t pos_x_cg, uint32_t pos_y_cg, int32_t width, int32_t height )
{
	if( width == 4 && height == 4 ) return -1;

	uint32_t sig_right = 0;
	uint32_t sig_lower = 0;

	width >>= 2;
	height >>= 2;
	if( pos_x_cg < width - 1 )
	{
		sig_right = (sig_coeff_group_flag[ pos_y_cg * width + pos_x_cg + 1 ] != 0);
	}
	if (pos_y_cg < height - 1 )
	{
		sig_lower = (sig_coeff_group_flag[ (pos_y_cg  + 1 ) * width + pos_x_cg ] != 0);
	}
	return sig_right + (sig_lower<<1);
}

int32_t x265_tr_quant_get_sig_ctx_inc( int32_t pattern_sig_ctx, uint32_t scan_idx, int32_t pos_x, int32_t pos_y, int32_t log_2_block_size, enum text_type texture_type )
{
	const int32_t ctx_ind_map[16] =
	{
	0, 1, 4, 5,
	2, 3, 4, 5,
	6, 6, 8, 8,
	7, 7, 8, 8
	};

	if( pos_x + pos_y == 0 )
	{
		return 0;
	}

	if ( log_2_block_size == 2 )
	{
		return ctx_ind_map[ 4 * pos_y + pos_x ];
	}

	int32_t offset = log_2_block_size == 3 ? (scan_idx==SCAN_DIAG ? 9 : 15) : (texture_type == TEXT_LUMA ? 21 : 12);

	int32_t pos_x_in_subset = pos_x-((pos_x>>2)<<2);
	int32_t pos_y_in_subset = pos_y-((pos_y>>2)<<2);
	int32_t cnt = 0;
	if(pattern_sig_ctx==0)
	{
		cnt = pos_x_in_subset+pos_y_in_subset<=2 ? (pos_x_in_subset+pos_y_in_subset==0 ? 2 : 1) : 0;
	}
	else if(pattern_sig_ctx==1)
	{
		cnt = pos_y_in_subset<=1 ? (pos_y_in_subset==0 ? 2 : 1) : 0;
	}
	else if(pattern_sig_ctx==2)
	{
		cnt = pos_x_in_subset<=1 ? (pos_x_in_subset==0 ? 2 : 1) : 0;
	}
	else
	{
		cnt = 2;
	}

	return (( texture_type == TEXT_LUMA && ((pos_x>>2) + (pos_y>>2)) > 0 ) ? 3 : 0) + offset + cnt;
}

uint32_t x265_tr_quant_get_sig_coeff_group_ctx_inc( const uint32_t* sig_coeff_group_flag, const uint32_t i_cg_pos_x, const uint32_t i_cg_pos_y, int32_t width, int32_t height)
{
	uint32_t i_right = 0;
	uint32_t i_lower = 0;

	width >>= 2;
	height >>= 2;
	if( i_cg_pos_x < width - 1 )
	{
	i_right = (sig_coeff_group_flag[ i_cg_pos_y * width + i_cg_pos_x + 1 ] != 0);
	}
	if (i_cg_pos_y < height - 1 )
	{
	i_lower = (sig_coeff_group_flag[ (i_cg_pos_y  + 1 ) * width + i_cg_pos_x ] != 0);
	}
	return (i_right || i_lower);
}

int x265_tr_quant_init_scaling_list( x265_t* h, x265_tr_quant_t* tr_quant )
{
	for(uint32_t size_id = 0; size_id < SCALING_LIST_SIZE_NUM; size_id++)
	{
		for(uint32_t list_id = 0; list_id < get_scaling_list_num(size_id); list_id++)
		{
			for(uint32_t qp = 0; qp < SCALING_LIST_REM_NUM; qp++)
			{
				CHECKED_MALLOCZERO( tr_quant->quant_coef [size_id][list_id][qp],
									sizeof(int32_t) * get_scaling_list_size(size_id) );

				CHECKED_MALLOCZERO( tr_quant->dequant_coef [size_id][list_id][qp],
									sizeof(int32_t) * get_scaling_list_size(size_id) );
				CHECKED_MALLOCZERO( tr_quant->err_scale [size_id][list_id][qp],
									sizeof(double) * get_scaling_list_size(size_id) );
			}
		}
	}
	// alias list [1] as [3].
	for(uint32_t qp = 0; qp < SCALING_LIST_REM_NUM; qp++)
	{
		tr_quant->quant_coef [SCALING_LIST_32x32][3][qp] = tr_quant->quant_coef [SCALING_LIST_32x32][1][qp];
		tr_quant->dequant_coef [SCALING_LIST_32x32][3][qp] = tr_quant->dequant_coef [SCALING_LIST_32x32][1][qp];
		tr_quant->err_scale [SCALING_LIST_32x32][3][qp] = tr_quant->err_scale [SCALING_LIST_32x32][1][qp];
	}

	return 0;
fail:
	return -1;
}

void x265_tr_quant_destroy_scaling_list( x265_t* h, x265_tr_quant_t* tr_quant )
{
	for(uint32_t size_id = 0; size_id < SCALING_LIST_SIZE_NUM; size_id++)
	{
		for(uint32_t list_id = 0; list_id < get_scaling_list_num(size_id); list_id++)
		{
			for(uint32_t qp = 0; qp < SCALING_LIST_REM_NUM; qp++)
			{
				x265_free(tr_quant->quant_coef   [size_id][list_id][qp]);
				x265_free(tr_quant->dequant_coef [size_id][list_id][qp]);
				x265_free(tr_quant->err_scale    [size_id][list_id][qp]);
			}
		}
	}
}

void x265_tr_quant_set_err_scale_coeff( x265_t* h, uint32_t list,uint32_t size, uint32_t qp )
{
	uint32_t i_log_2_tr_size = h->global.convert_to_bit[ get_scaling_list_size_x(size) ] + 2;
	int32_t bit_depth = (size < SCALING_LIST_32x32 && list != 0 && list != 3) ? h->param.sps.i_bit_depth_c : h->param.sps.i_bit_depth_y;
	int32_t i_transform_shift = X265_MAX_TR_DYNAMIC_RANGE - bit_depth - i_log_2_tr_size;  // Represents scaling through forward transform

	uint32_t i, i_max_num_coeff = get_scaling_list_size(size);
	int32_t *i_quant_coeff;
	double *err_scale;
	i_quant_coeff   = getQuantCoeff(list, qp,size);
	err_scale     = getErrScaleCoeff(list, size, qp);

	double d_err_scale = (double)(1<<X265_SCALE_BITS);                              // Compensate for scaling of bitcount in Lagrange cost function
	d_err_scale = d_err_scale*pow(2.0,-2.0*i_transform_shift);                     // Compensate for scaling through forward transform
	for(i=0;i<i_max_num_coeff;i++)
	{
		err_scale[i] = d_err_scale / i_quant_coeff[i] / i_quant_coeff[i] / (1<<DISTORTION_PRECISION_ADJUSTMENT(2*(bit_depth-8)));
	}
}

void x265_tr_quant_set_flat_scaling_list( x265_t* h )
{
	uint32_t size,list;
	uint32_t qp;

	for(size=0;size<SCALING_LIST_SIZE_NUM;size++)
	{
		for(list = 0; list <  get_scaling_list_num(size); list++)
		{
			for(qp=0;qp<SCALING_LIST_REM_NUM;qp++)
			{
				xsetFlatScalingList(list,size,qp);
				setErrScaleCoeff(list,size,qp);
			}
		}
	}
}

void x265_tr_quant_xset_flat_scaling_list( x265_t* h, uint32_t list, uint32_t size, uint32_t qp )
{
	uint32_t i,num = get_scaling_list_size(size);
	int32_t *quant_coeff;
	int32_t *dequant_coeff;
	int32_t quant_scales = quant_scales[qp];
	int32_t inv_quant_scales = inv_quant_scales[qp]<<4;

	quant_coeff   = getQuantCoeff(list, qp, size);
	dequant_coeff = getDequantCoeff(list, qp, size);

	for(i=0;i<num;i++)
	{
		*quant_coeff++ = quant_scales;
		*dequant_coeff++ = inv_quant_scales;
	}
}

void x265_tr_quant_xset_scaling_list_enc( x265_t* h, x265_scaling_list_t *scaling_list, uint32_t list_id, uint32_t size_id, uint32_t qp )
{
	uint32_t width = get_scaling_list_size_x(size_id);
	uint32_t height = get_scaling_list_size_x(size_id);
	uint32_t ratio = get_scaling_list_size_x(size_id)/min(X265_MAX_MATRIX_SIZE_NUM, (int32_t)get_scaling_list_size_x(size_id));
	int32_t *quant_coeff;
	int32_t *coeff = scaling_list->getScalingListAddress(size_id,list_id);
	quant_coeff   = getQuantCoeff(list_id, qp, size_id);

	processScalingListEnc(	coeff,
							quant_coeff,
							quant_scales[qp]<<4,
							height,
							width,
							ratio,
							min( X265_MAX_MATRIX_SIZE_NUM,
								 (int32_t)get_scaling_list_size_x(size_id) ),
							scaling_list->getScalingListDC(size_id,list_id));
}

void x265_tr_quant_xset_scaling_list_dec( x265_t* h, x265_scaling_list_t *scaling_list, uint32_t list_id, uint32_t size_id, uint32_t qp )
{
	uint32_t width = get_scaling_list_size_x(size_id);
	uint32_t height = get_scaling_list_size_x(size_id);
	uint32_t ratio = get_scaling_list_size_x(size_id)/min(X265_MAX_MATRIX_SIZE_NUM,(int32_t)get_scaling_list_size_x(size_id));
	int32_t *dequant_coeff;
	int32_t *coeff = scaling_list->getScalingListAddress(size_id,list_id);

	dequant_coeff = getDequantCoeff(list_id, qp, size_id);
	processScalingListDec(	coeff,
							dequant_coeff,
							inv_quant_scales[qp],
							height,
							width,
							ratio,
							min( X265_MAX_MATRIX_SIZE_NUM,
								 (int32_t)get_scaling_list_size_x(size_id) ),
							scaling_list->getScalingListDC(size_id,list_id) );
}

void x265_tr_quant_set_scaling_list( x265_t* h, x265_scaling_list_t *scaling_list )
{
	uint32_t size,list;
	uint32_t qp;

	for(size=0;size<SCALING_LIST_SIZE_NUM;size++)
	{
		for(list = 0; list < get_scaling_list_num(size); list++)
		{
			for(qp=0;qp<SCALING_LIST_REM_NUM;qp++)
			{
				xSetScalingListEnc(scaling_list,list,size,qp);
				xSetScalingListDec(scaling_list,list,size,qp);
				setErrScaleCoeff(list,size,qp);
			}
		}
	}
}

void x265_tr_quant_set_scaling_list_dec( x265_t* h, x265_scaling_list_t *scaling_list )
{
	uint32_t size,list;
	uint32_t qp;

	for(size=0;size<SCALING_LIST_SIZE_NUM;size++)
	{
		for(list = 0; list < get_scaling_list_num(size); list++)
		{
			for(qp=0;qp<SCALING_LIST_REM_NUM;qp++)
			{
				xSetScalingListDec(scaling_list,list,size,qp);
			}
		}
	}
}

void x265_tr_quant_process_scaling_list_enc( int32_t *coeff, int32_t *quant_coeff, int32_t quant_scales, uint32_t height, uint32_t width, uint32_t ratio, int32_t sizu_num, uint32_t dc)
{
	int32_t nsqth = (height < width) ? 4: 1; //height ratio for NSQT
	int32_t nsqtw = (width < height) ? 4: 1; //width ratio for NSQT
	for(uint32_t j=0;j<height;j++)
	{
		for(uint32_t i=0;i<width;i++)
		{
			quant_coeff[j*width + i] = quant_scales / coeff[sizu_num * (j * nsqth / ratio) + i * nsqtw /ratio];
		}
	}
	if(ratio > 1)
	{
		quant_coeff[0] = quant_scales / dc;
	}
}

void x265_tr_quant_process_scaling_list_dec( int32_t *coeff, int32_t *dequant_coeff, int32_t inv_quant_scales, uint32_t height, uint32_t width, uint32_t ratio, int32_t sizu_num, uint32_t dc)
{
	for(uint32_t j=0;j<height;j++)
	{
		for(uint32_t i=0;i<width;i++)
		{
			dequant_coeff[j*width + i] = inv_quant_scales * coeff[sizu_num * (j / ratio) + i / ratio];
		}
	}
	if(ratio > 1)
	{
		dequant_coeff[0] = inv_quant_scales * dc;
	}
}

void x265_tr_quant_init_slice_qp_delta( x265_tr_quant_t* tr_quant )
{
	for(int32_t qp=0; qp<=X265_MAX_QP; qp++)
	{
		tr_quant->qp_delta[qp] = qp < 17 ? 0 : 1;
	}
}

#if X265_ADAPTIVE_QP_SELECTION

void x265_tr_quant_init_slice_qp_delta( x265_tr_quant_t* tr_quant )
{
	for(int32_t qp=0; qp<=X265_MAX_QP; qp++)
	{
		tr_quant->qp_delta[qp] = qp < 17 ? 0 : 1;
	}
}

void x265_tr_quant_store_slice_qp_next( x265_tr_quant_t* tr_quant, x265_slice_t* slice )
{
	int32_t i_qp_base = slice->getSliceQpBase();
	int32_t i_slice_qp_used = slice->getSliceQp();
	int32_t i_slice_qp_next;
	double alpha = i_qp_base < 17 ? 0.5 : 1;

	int32_t cnt=0;
	for(int32_t u=1; u<=X265_LEVEL_RANGE; u++)
	{
		cnt += tr_quant->slice_n_samples[u] ;
	}

	if( !tr_quant->b_use_rd_oq )
	{
		i_slice_qp_used = i_qp_base;
		alpha = 0.5;
	}

	if( cnt > 120 )
	{
		double sum = 0;
		int32_t k = 0;
		for(int32_t u=1; u<X265_LEVEL_RANGE; u++)
		{
			sum += u*tr_quant->slice_sum_c[u];
			k += u*u*tr_quant->slice_n_samples[u];
		}

		int32_t v;
		double q[X265_MAX_QP+1] ;
		for(v=0; v<=X265_MAX_QP; v++)
		{
			q[v] = (double)(inv_quant_scales[v%6] * (1<<(v/6)))/64 ;
		}

		double qnext = sum/k * q[i_slice_qp_used] / (1<<X265_ARL_C_PRECISION);

		for(v=0; v<X265_MAX_QP; v++)
		{
			if(qnext < alpha * q[v] + (1 - alpha) * q[v+1] )
			{
				break;
			}
		}
		i_slice_qp_next = Clip3(i_slice_qp_used - 3, i_slice_qp_used + 3, v);
	}
	else
	{
		i_slice_qp_next = i_slice_qp_used;
	}

	tr_quant->qp_delta[i_qp_base] = i_slice_qp_next - i_qp_base;
}

void x265_tr_quant_clearSliceARLCnt( x265_tr_quant_t* tr_quant )
{
	memset(tr_quant->slice_sum_c, 0, sizeof(double)*(X265_LEVEL_RANGE+1));
	memset(tr_quant->slice_n_samples, 0, sizeof(int32_t)*(X265_LEVEL_RANGE+1));
}
#endif

#endif


*/






