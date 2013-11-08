
#include "common.h"

void x265_pic_sym_offset_print ( x265_t *h )
{
	int32_t i = 0, j = 0 ;
	FILE *file = NULL ;

	file = fopen ( "Offset2", "wb" ) ;
	for ( i = 0 ; i < h->cu.pic.i_height_in_cu ; ++ i )
	{
		for ( j = 0 ; j < h->cu.pic.i_width_in_cu ; ++ j )
		{
			fprintf ( file, "%d ", h->cu.pic.cu_offset_y[i * h->cu.pic.i_width_in_cu + j] ) ;
		}
		fprintf ( file, "\n" ) ;
	}

	for ( i = 0 ; i < h->cu.pic.i_height_in_cu ; ++ i )
	{
		for ( j = 0 ; j < h->cu.pic.i_width_in_cu ; ++ j )
		{
			fprintf ( file, "%d ", h->cu.pic.cu_offset_c[i * h->cu.pic.i_width_in_cu + j] ) ;
		}
		fprintf ( file, "\n" ) ;
	}

	for ( i = 0 ; i < h->cu.pic.i_num_part_in_height ; ++ i )
	{
		for ( j = 0 ; j < h->cu.pic.i_num_part_in_width ; ++ j )
		{
			fprintf ( file, "%d ", h->cu.pic.bu_offset_y[i * h->cu.pic.i_num_part_in_width + j] ) ;
		}
		fprintf ( file, "\n" ) ;
	}

	for ( i = 0 ; i < h->cu.pic.i_num_part_in_height ; ++ i )
	{
		for ( j = 0 ; j < h->cu.pic.i_num_part_in_width ; ++ j )
		{
			fprintf ( file, "%d ", h->cu.pic.bu_offset_c[i * h->cu.pic.i_num_part_in_width + j] ) ;
		}
		fprintf ( file, "\n" ) ;
	}

	fclose ( file ) ;
}


int x265_tile_init ( x265_tile_t *tile )
{
	memset ( tile, 0, sizeof(x265_tile_t) ) ;
	return 0 ;
}

void x265_tile_print ( x265_tile_t *tile, FILE *file )
{
	fprintf ( file, "%d ", tile->i_first_cu_addr ) ;
	fprintf ( file, "%d ", tile->i_tile_width ) ;
	fprintf ( file, "%d ", tile->i_tile_height ) ;
	fprintf ( file, "%d ", tile->i_right_edge_pos_in_cu ) ;
	fprintf ( file, "%d ", tile->i_bottom_edge_pos_in_cu ) ;
}


int x265_pic_sym_init ( x265_t *h, x265_param_t *param )
{
	int32_t loop = 0 ;
	int32_t i = 0, j = 0 ;
    int i_stride = 0, i_width = 0 ;
    int i_padh = 0 ;
    uint32_t i_cummulative_tile_width = 0 ;
    uint32_t i_cummulative_tile_height = 0 ;
    int32_t p = 0 ;
    uint32_t i_enc_cu_addr = 0 ;

    h->cu.pic.i_max_cu_width = h->param.sps.i_max_cu_width ;
    h->cu.pic.i_max_cu_height = h->param.sps.i_max_cu_height ;

    h->cu.pic.i_add_cu_depth = 0 ;
    while( (h->param.sps.i_max_cu_width >> h->param.sps.i_max_cu_depth)
    		> (1 << (h->param.sps.i_quadtree_tu_log2_min_size + h->cu.pic.i_add_cu_depth))
    		)
    {
    	h->cu.pic.i_add_cu_depth ++ ;
    }

    h->param.sps.i_max_cu_depth += h->cu.pic.i_add_cu_depth ;
    h->cu.pic.i_add_cu_depth ++ ;
    h->cu.pic.i_total_depth = h->param.sps.i_max_cu_depth ;
    h->cu.pic.i_bit_depth_y = h->param.sps.i_bit_depth_y ;
    h->cu.pic.i_bit_depth_c = h->param.sps.i_bit_depth_c ;
    h->cu.pic.i_pcm_bit_depth_luma = h->param.b_pcm_input_bit_depth_flag ?
    								h->param.i_input_bit_depth_y :
    								h->param.sps.i_bit_depth_y ;
    h->cu.pic.i_pcm_bit_depth_chroma = h->param.b_pcm_input_bit_depth_flag ?
    								h->param.i_input_bit_depth_c :
    								h->param.sps.i_bit_depth_c ;

    h->cu.pic.i_num_partitions = (1 << (h->cu.pic.i_total_depth << 1)) ;
    h->cu.pic.i_min_cu_width = (h->cu.pic.i_max_cu_width >> h->cu.pic.i_total_depth) ;
    h->cu.pic.i_min_cu_height = (h->cu.pic.i_max_cu_height >> h->cu.pic.i_total_depth) ;
    h->cu.pic.i_num_part_in_width = h->cu.pic.i_max_cu_width / h->cu.pic.i_min_cu_width ;
    h->cu.pic.i_num_part_in_height = h->cu.pic.i_max_cu_height / h->cu.pic.i_min_cu_height ;
    h->cu.pic.i_width_in_cu = (h->param.i_width % h->cu.pic.i_max_cu_width)
								? h->param.i_width / h->cu.pic.i_max_cu_width + 1
								: h->param.i_width / h->cu.pic.i_max_cu_width ;
    h->cu.pic.i_height_in_cu = (h->param.i_height % h->cu.pic.i_max_cu_height)
								? h->param.i_height / h->cu.pic.i_max_cu_height + 1
								: h->param.i_height / h->cu.pic.i_max_cu_height ;
    h->cu.pic.i_num_cus_in_frame = h->cu.pic.i_width_in_cu * h->cu.pic.i_height_in_cu ;
    h->cu.pic.i_width_in_cu_mul_two = h->cu.pic.i_width_in_cu * 2 ;

    CHECKED_MALLOCZERO( h->cu.pic.cu_offset_y,
    					h->cu.pic.i_num_cus_in_frame * sizeof(int32_t) );
    CHECKED_MALLOCZERO( h->cu.pic.cu_offset_c,
    					h->cu.pic.i_num_cus_in_frame * sizeof(int32_t) );
    CHECKED_MALLOCZERO( h->cu.pic.bu_offset_y,
    					h->cu.pic.i_num_partitions * sizeof(int32_t) );
    CHECKED_MALLOCZERO( h->cu.pic.bu_offset_c,
    					h->cu.pic.i_num_partitions * sizeof(int32_t) );

    i_width  = h->param.i_width;
    i_padh = h->cu.pic.i_max_cu_width + 16 ;
    i_stride = i_width + 2*i_padh ;
    for ( i = 0 ; i < h->cu.pic.i_height_in_cu ; ++ i )
    {
    	for ( j = 0 ; j < h->cu.pic.i_width_in_cu ; ++ j )
    	{
    		h->cu.pic.cu_offset_y[i * h->cu.pic.i_width_in_cu + j]
    		   = i_stride * i * h->cu.pic.i_max_cu_height + j * h->cu.pic.i_max_cu_width ;
    		h->cu.pic.cu_offset_c[i * h->cu.pic.i_width_in_cu + j]
    		   = (i_stride>>1) * i * (h->cu.pic.i_max_cu_height / 2)
    		   	   + j * (h->cu.pic.i_max_cu_width>>1) ;
    	}
    }
    for ( i = 0 ; i < h->cu.pic.i_num_part_in_height ; ++ i )
    {
    	for ( j = 0 ; j < h->cu.pic.i_num_part_in_width ; ++ j )
    	{
    		h->cu.pic.bu_offset_y[i * h->cu.pic.i_num_part_in_width + j]
    		   = i_stride * i * h->cu.pic.i_min_cu_height + j * h->cu.pic.i_min_cu_width ;
    		h->cu.pic.bu_offset_c[i * h->cu.pic.i_num_part_in_width + j]
    		   = (i_stride>>1) * i * (h->cu.pic.i_min_cu_height / 2 )
    		     + j * (h->cu.pic.i_min_cu_width>>1) ;
    	}
    }
    //	x265_pic_sym_offset_print ( h ) ;

    h->slice = x265_slice_new ( h ) ;
    if ( NULL == h->slice )
    {
    	goto fail ;
    }
    if ( h->param.sps.b_use_sao )
    {
    	CHECKED_MALLOCZERO( h->sao_param, sizeof(x265_sao_param_t) );
    	if ( x265_sample_adaptive_offset_alloc_sao_param ( h,
    														(x265_sample_adaptive_offset_t*)&h->enc_sao,
    														h->sao_param ) )
    	{
    		goto fail ;
    	}
    }

	h->image_up_line_of_intra = x265_image_new () ;
	if ( NULL == h->image_up_line_of_intra )
	{
		goto fail ;
	}
	if ( x265_image_create_intra_image ( h->image_up_line_of_intra,
										h->cu.pic.i_width_in_cu * h->cu.pic.i_max_cu_width,
										2 )
										)
	{
		goto fail ;
	}
	h->image_bottom_line_of_intra = x265_image_new () ;
	if ( NULL == h->image_bottom_line_of_intra )
	{
		goto fail ;
	}
	if ( x265_image_create_intra_image ( h->image_bottom_line_of_intra,
										h->cu.pic.i_width_in_cu * h->cu.pic.i_max_cu_width,
										2 )
										)
	{
		goto fail ;
	}

	h->image_left_line_of_intra = x265_image_new () ;
	if ( NULL == h->image_left_line_of_intra )
	{
		goto fail ;
	}
	if ( x265_image_create ( h->image_left_line_of_intra,
							2, h->cu.pic.i_max_cu_height )
							)
	{
		goto fail ;
	}

	h->image_intra = x265_image_new () ;
	if ( NULL == h->image_intra )
	{
		goto fail ;
	}
	if ( x265_image_create_intra_image ( h->image_intra,
										h->cu.pic.i_max_cu_width * 2,
										h->cu.pic.i_max_cu_height )
										)
	{
		goto fail ;
	}

	CHECKED_MALLOCZERO( h->neighbour_cus,
						h->cu.pic.i_width_in_cu_mul_two * sizeof(x265_base_data_cu_t*) );
	for ( loop = 0 ; loop < h->cu.pic.i_width_in_cu_mul_two ; ++ loop )
	{
		h->neighbour_cus[loop] = x265_base_data_cu_new () ;
		if ( NULL == h->neighbour_cus[loop] )
		{
			goto fail ;
		}
		if ( x265_base_data_cu_create ( h->neighbour_cus[loop],
										h->cu.pic.i_num_partitions,
										h->cu.pic.i_max_cu_width,
										h->cu.pic.i_max_cu_height,
										h->cu.pic.i_max_cu_width >> h->cu.pic.i_total_depth
#if X265_ADAPTIVE_QP_SELECTION
										, 1
#endif
										)
										)
		{
			goto fail ;
		}
	}
	h->data_cu = x265_data_cu_new () ;
	if ( NULL == h->data_cu )
	{
		goto fail ;
	}
	if ( x265_data_cu_create ( h->data_cu,
								h->cu.pic.i_num_partitions,
								h->cu.pic.i_max_cu_width,
								h->cu.pic.i_max_cu_height,
								h->cu.pic.i_max_cu_width >> h->cu.pic.i_total_depth
#if X265_ADAPTIVE_QP_SELECTION
								, 1
#endif
								)
								)
	{
		goto fail ;
	}

    /*

  h->neighbour_cus = new TComBaseDataCU*[h->cu.pic.i_width_in_cuMulTwo] ;

  for ( i = 0 ; i < h->cu.pic.i_width_in_cuMulTwo ; ++ i )
  {
	h->neighbour_cus[i] = new TComBaseDataCU () ;
	h->neighbour_cus[i]->create( m_uiNumPartitions, m_uiMaxCUWidth, m_uiMaxCUHeight,
									m_uiMaxCUWidth >> m_uhTotalDepth
#if ADAPTIVE_QP_SELECTION
									, true
#endif
) ;
  }
  m_pcCU = new TComDataCU () ;
  m_pcCU->create( m_uiNumPartitions, m_uiMaxCUWidth, m_uiMaxCUHeight,
		  	  	  m_uiMaxCUWidth >> m_uhTotalDepth
#if ADAPTIVE_QP_SELECTION
    , true
#endif
    );

     *
     */

    CHECKED_MALLOCZERO( h->cu_order_map,
    					(h->cu.pic.i_num_cus_in_frame+1) * sizeof(uint32_t) );
    CHECKED_MALLOCZERO( h->tile_idx_map,
    					h->cu.pic.i_num_cus_in_frame * sizeof(uint32_t) );
    CHECKED_MALLOCZERO( h->inverse_cu_order_map,
    					(h->cu.pic.i_num_cus_in_frame+1) * sizeof(uint32_t) );

    //create the TComTileArray
    x265_pic_sym_x_create_tile_array ( h ) ;

    if( 1 == h->pps[0].i_uniform_spacing_flag )
    {
    	//set the width for each tile
    	for ( j = 0 ; j < h->pps[0].i_num_rows_minus1 + 1 ; ++ j )
    	{
    		for ( p = 0; p < h->pps[0].i_num_columns_minus1 + 1 ; ++ p )
    		{
    			h->tile[j * (h->pps[0].i_num_columns_minus1+1) + p ]->
    					i_tile_width = (p+1)*h->cu.pic.i_width_in_cu/(h->pps[0].i_num_columns_minus1+1)
    							- (p*h->cu.pic.i_width_in_cu)/(h->pps[0].i_num_columns_minus1+1) ;
    		}
    	}

    	//set the height for each tile
    	for ( j = 0 ; j < h->pps[0].i_num_columns_minus1 + 1 ; ++ j )
    	{
    		for ( p = 0; p < h->pps[0].i_num_rows_minus1 + 1 ; ++ p )
    		{
    			h->tile[ p * (h->pps[0].i_num_columns_minus1+1) + j ]->
    					i_tile_height = (p+1)*h->cu.pic.i_height_in_cu/(h->pps[0].i_num_rows_minus1+1)
    							- (p*h->cu.pic.i_height_in_cu)/(h->pps[0].i_num_rows_minus1+1) ;
    		}
    	}
    }
    else
    {
      //set the width for each tile
    	for ( j = 0 ; j < h->pps[0].i_num_rows_minus1 + 1 ; ++ j )
    	{
            i_cummulative_tile_width = 0;
    	    for ( p = 0; p < h->pps[0].i_num_columns_minus1 ; ++ p )
    	    {
    	    	h->tile[ j * (h->pps[0].i_num_columns_minus1+1) + p ]->i_tile_width
    	    		= h->pps[0].column_width[p] ;
    	    	i_cummulative_tile_width += h->pps[0].column_width[p] ;
    	    }
    	    h->tile[j * (h->pps[0].i_num_columns_minus1+1) + p]->i_tile_width
    	    		= h->cu.pic.i_width_in_cu - i_cummulative_tile_width ;
    	}

    	//set the height for each tile
    	for ( j = 0 ; j < h->pps[0].i_num_columns_minus1 + 1 ; ++ j )
    	{
            i_cummulative_tile_height = 0;
    		for ( p = 0; p < h->pps[0].i_num_rows_minus1 ; ++ p )
    		{
    			h->tile[ p * (h->pps[0].i_num_columns_minus1+1) + j ]->i_tile_height
    				= h->pps[0].row_height[p] ;
    			i_cummulative_tile_height += h->pps[0].row_height[p];
    		}
    		h->tile[p * (h->pps[0].i_num_columns_minus1+1) + j]->i_tile_height
    			= h->cu.pic.i_height_in_cu - i_cummulative_tile_height ;
    	}
    }
    //intialize each tile of the current picture
    x265_pic_sym_x_init_tiles ( h ) ;

    //generate the Coding Order Map and Inverse Coding Order Map
    for ( p = 0, i_enc_cu_addr = 0 ;
    		p < h->cu.pic.i_num_cus_in_frame ;
    		++ p,
    		i_enc_cu_addr = x265_pic_sym_x_calculate_nxt_cu_addr ( h, i_enc_cu_addr))
    {
    	h->cu_order_map[p] = i_enc_cu_addr ;
    	h->inverse_cu_order_map[i_enc_cu_addr] = p ;
    }
	h->cu_order_map[h->cu.pic.i_num_cus_in_frame]
	                                 = h->cu.pic.i_num_cus_in_frame ;
	h->inverse_cu_order_map[h->cu.pic.i_num_cus_in_frame]
	                                 = h->cu.pic.i_num_cus_in_frame ;

	//	x265_pic_sym_print ( h ) ;

    return 0 ;

fail:
	x265_pic_sym_deinit ( h ) ;
	return -1 ;
}


void x265_pic_sym_deinit ( x265_t *h )
{
	int32_t loop = 0 ;

	x265_pic_sym_x_destroy_tile_array ( h ) ;

	x265_free ( h->inverse_cu_order_map ) ;
	x265_free ( h->tile_idx_map ) ;
	x265_free ( h->cu_order_map ) ;

	if ( h->data_cu )
	{
		x265_data_cu_destroy ( h->data_cu ) ;
		x265_free ( h->data_cu ) ;
		h->data_cu = NULL ;
	}
	if ( h->neighbour_cus )
	{
		for ( loop = 0 ; loop < h->cu.pic.i_width_in_cu_mul_two ; ++ loop )
		{
			if ( h->neighbour_cus[loop] )
			{
				x265_base_data_cu_destroy ( h->neighbour_cus[loop] ) ;
				x265_free ( h->neighbour_cus[loop] ) ;
				h->neighbour_cus[loop] = NULL ;
			}
		}
		x265_free ( h->neighbour_cus ) ;
		h->neighbour_cus = NULL ;
	}
	if ( h->image_intra )
	{
		x265_image_destroy ( h->image_intra ) ;
		x265_free ( h->image_intra ) ;
		h->image_intra = NULL ;
	}
	if ( h->image_left_line_of_intra )
	{
		x265_image_destroy ( h->image_left_line_of_intra ) ;
		x265_free ( h->image_left_line_of_intra ) ;
		h->image_left_line_of_intra = NULL ;
	}
	if ( h->image_bottom_line_of_intra )
	{
		x265_image_destroy ( h->image_bottom_line_of_intra ) ;
		x265_free ( h->image_bottom_line_of_intra ) ;
		h->image_bottom_line_of_intra = NULL ;
	}
	if ( h->image_up_line_of_intra )
	{
		x265_image_destroy ( h->image_up_line_of_intra ) ;
		x265_free ( h->image_up_line_of_intra ) ;
		h->image_up_line_of_intra = NULL ;
	}
	if ( h->sao_param )
	{
		x265_sample_adaptive_offset_free_sao_param ( (x265_sample_adaptive_offset_t*)&h->enc_sao,
													h->sao_param ) ;
		x265_free ( h->sao_param ) ;
		h->sao_param = NULL ;
	}
	x265_slice_delete ( h->slice ) ;
	x265_free ( h->cu.pic.bu_offset_c ) ;
	x265_free ( h->cu.pic.bu_offset_y ) ;
	x265_free ( h->cu.pic.cu_offset_c ) ;
	x265_free ( h->cu.pic.cu_offset_y ) ;

	h->cu_order_map = NULL ;
	h->tile_idx_map = NULL ;
	h->inverse_cu_order_map = NULL ;
	h->slice = NULL ;
	h->cu.pic.cu_offset_y = NULL ;
	h->cu.pic.cu_offset_c = NULL ;
	h->cu.pic.bu_offset_y = NULL ;
	h->cu.pic.bu_offset_c = NULL ;
}



void x265_pic_sym_print ( x265_t *h )
{
	int32_t loop = 0 ;
	FILE *file = NULL ;
	static int32_t b_first_print_pic_sym = 1 ;
	static int32_t i_pic_sym_number = 1 ;

	if ( b_first_print_pic_sym )
	{
		file = fopen ( "PicSym2", "wb" ) ;
		b_first_print_pic_sym = 0 ;
	}
	else
	{
		file = fopen ( "PicSym2", "r+b" ) ;
		fseek ( file, 0, SEEK_END ) ;
	}

	fprintf ( file, "PicSym%d:\n", i_pic_sym_number ) ;
	i_pic_sym_number ++ ;

	fprintf ( file, "%d ", h->cu.pic.i_width_in_cu ) ;
	fprintf ( file, "%d ", h->cu.pic.i_height_in_cu ) ;
	fprintf ( file, "%d ", h->cu.pic.i_width_in_cu_mul_two ) ;

	fprintf ( file, "%d ", h->cu.pic.i_max_cu_width ) ;
	fprintf ( file, "%d ", h->cu.pic.i_max_cu_height ) ;
	fprintf ( file, "%d ", h->cu.pic.i_min_cu_width ) ;
	fprintf ( file, "%d ", h->cu.pic.i_min_cu_height ) ;

	fprintf ( file, "%d ", h->cu.pic.i_total_depth ) ;
	fprintf ( file, "%d ", h->cu.pic.i_num_partitions ) ;
	fprintf ( file, "%d ", h->cu.pic.i_num_part_in_width ) ;
	fprintf ( file, "%d ", h->cu.pic.i_num_part_in_height ) ;
	fprintf ( file, "%d ", h->cu.pic.i_num_cus_in_frame ) ;

	fprintf ( file, "%d ", h->cu.pic.i_add_cu_depth ) ;

	fprintf ( file, "\n" ) ;

	for ( loop = 0 ;
			loop < ( h->param.pps.i_num_columns_minus_1 + 1 ) * ( h->param.pps.i_num_rows_minus_1 + 1 ) ;
			++ loop )
	{
		fprintf ( file, "Tile%d:\n", loop ) ;

		x265_tile_print ( h->tile[loop], file ) ;
		fprintf ( file, "\n" ) ;
	}

	for( loop = 0 ; loop < h->cu.pic.i_num_cus_in_frame + 1 ; ++ loop )
	{
		fprintf ( file, "%d ", h->cu_order_map[loop] ) ;
	}
	fprintf ( file, "\n" ) ;

	for( loop = 0 ; loop < h->cu.pic.i_num_cus_in_frame ; ++ loop )
	{
		fprintf ( file, "%d ", h->tile_idx_map[loop] ) ;
	}
	fprintf ( file, "\n" ) ;

	for( loop = 0 ; loop < h->cu.pic.i_num_cus_in_frame + 1 ; ++ loop )
	{
		fprintf ( file, "%d ", h->inverse_cu_order_map[loop] ) ;
	}
	fprintf ( file, "\n" ) ;


	fclose ( file ) ;
}


void x265_pic_sym_load_prediction_data ( x265_t *h, uint32_t i_cu_addr )
{
	int32_t i_up_width = 0 ;

	if ( i_cu_addr % h->cu.pic.i_width_in_cu == h->cu.pic.i_width_in_cu - 1 )
	{
		i_up_width = h->cu.pic.i_max_cu_width ;
	}
	else
	{
		i_up_width = h->cu.pic.i_max_cu_width * 2 ;
	}

	x265_image_copy_up_from_image ( h,
									h->image_intra,
									h->image_up_line_of_intra,
									i_cu_addr % h->cu.pic.i_width_in_cu,
									i_up_width ) ;
	x265_image_copy_left_from_image ( h,
									h->image_intra,
									h->image_left_line_of_intra ) ;
}

void x265_pic_sym_store_prediction_data ( x265_t *h, uint32_t i_cu_addr )
{
	x265_image_t * yuv_swap_line_of_intra = NULL ;

	x265_image_copy_right_from_frame ( h,
										h->image_left_line_of_intra,
										h->fdec,
										i_cu_addr ) ;
	x265_image_copy_bottom_from_frame ( h,
										h->image_bottom_line_of_intra,
										h->fdec,
										i_cu_addr % h->cu.pic.i_width_in_cu,
										i_cu_addr ) ;

	if ( x265_pic_sym_is_tile_right_edge( h, i_cu_addr ) )
	{
		yuv_swap_line_of_intra = h->image_bottom_line_of_intra ;
		h->image_bottom_line_of_intra = h->image_up_line_of_intra ;
		h->image_up_line_of_intra = yuv_swap_line_of_intra ;
	}

	x265_base_data_cu_load ( h->neighbour_cus[i_cu_addr%h->cu.pic.i_width_in_cu_mul_two],
							(x265_base_data_cu_t*) h->data_cu ) ;
}

x265_image_t *x265_pic_sym_get_image_intra ( x265_t *h )
{
	return h->image_intra ;
}

void x265_pic_sym_swap_cu_buffer ( x265_t *h )
{

}

x265_slice_t*  x265_pic_sym_get_slice( x265_t *h, uint32_t i )
{
	return h->slice ;
}

uint32_t x265_pic_sym_get_frame_width_in_cu( x265_t *h )
{
	return h->cu.pic.i_width_in_cu ;
}

uint32_t x265_pic_sym_get_frame_height_in_cu( x265_t *h )
{
	return h->cu.pic.i_height_in_cu ;
}

uint32_t x265_pic_sym_get_min_cu_width( x265_t *h )
{
	return h->cu.pic.i_min_cu_width;
}

uint32_t x265_pic_sym_get_min_cu_height( x265_t *h )
{
	return h->cu.pic.i_min_cu_height ;
}

uint32_t x265_pic_sym_get_number_of_cus_in_frame( x265_t *h )
{
	return h->cu.pic.i_num_cus_in_frame ;
}


x265_base_data_cu_t* x265_pic_sym_get_curr_cu ( x265_t *h, uint32_t i_cu_addr)
{
	return h->neighbour_cus[i_cu_addr % h->cu.pic.i_width_in_cu_mul_two] ;
}

x265_base_data_cu_t* x265_pic_sym_get_left_cu ( x265_t *h, uint32_t i_cu_addr)
{
	uint32_t i_left_cu_addr = 0 ;

	i_left_cu_addr = i_cu_addr - 1 ;
	return h->neighbour_cus[i_left_cu_addr % h->cu.pic.i_width_in_cu_mul_two] ;
}

x265_base_data_cu_t* x265_pic_sym_get_above_cu ( x265_t *h, uint32_t i_cu_addr)
{
	uint32_t i_above_cu_addr = 0 ;

	i_above_cu_addr = i_cu_addr - h->cu.pic.i_width_in_cu ;
	return h->neighbour_cus[i_above_cu_addr % h->cu.pic.i_width_in_cu_mul_two] ;

}

x265_base_data_cu_t* x265_pic_sym_get_above_left_cu ( x265_t *h, uint32_t i_cu_addr)
{
	uint32_t i_above_left_cu_addr = 0 ;

	i_above_left_cu_addr = i_cu_addr - h->cu.pic.i_width_in_cu - 1 ;
	return h->neighbour_cus[i_above_left_cu_addr % h->cu.pic.i_width_in_cu_mul_two] ;

}

x265_base_data_cu_t* x265_pic_sym_get_above_right_cu ( x265_t *h, uint32_t i_cu_addr)
{
	uint32_t i_above_right_cu_addr = 0 ;

	i_above_right_cu_addr = i_cu_addr - h->cu.pic.i_width_in_cu + 1 ;
	return h->neighbour_cus[i_above_right_cu_addr % h->cu.pic.i_width_in_cu_mul_two] ;

}

x265_data_cu_t *x265_pic_sym_get_cu ( x265_t *h )
{
	return h->data_cu ;
}

uint32_t x265_pic_sym_get_num_partition( x265_t *h )
{
	return h->cu.pic.i_num_partitions ;
}

uint32_t x265_pic_sym_get_num_part_in_width( x265_t *h )
{
	return h->cu.pic.i_num_part_in_width ;
}

uint32_t x265_pic_sym_get_num_part_in_height( x265_t *h )
{
	return h->cu.pic.i_num_part_in_height ;
}

void x265_pic_sym_set_num_columns_minus1( x265_t *h, int32_t i )
{

}

int32_t x265_pic_sym_get_num_columns_minus1( x265_t *h )
{
	return h->pps[0].i_num_columns_minus1 ;
}

void x265_pic_sym_set_num_rows_minus1( x265_t *h, int32_t i )
{

}

int32_t x265_pic_sym_get_num_rows_minus1( x265_t *h )
{
	return h->pps[0].i_num_rows_minus1 ;
}

int32_t x265_pic_sym_get_num_tiles( x265_t *h )
{
	return (h->pps[0].i_num_columns_minus1 + 1) * (h->pps[0].i_num_rows_minus1 + 1) ;
}

x265_tile_t *x265_pic_sym_get_tile  ( x265_t *h, uint32_t i_tile_idx )
{
	return h->tile[i_tile_idx] ;
}

void x265_pic_sym_set_cu_order_map ( x265_t *h, int32_t i_enc_cu_order, int32_t i_cu_addr )
{
	h->cu_order_map[i_enc_cu_order] = i_cu_addr ;
}

uint32_t x265_pic_sym_get_cu_order_map( x265_t *h, int32_t i_enc_cu_order )
{
	return h->cu_order_map[i_enc_cu_order] ;
}

uint32_t x265_pic_sym_get_tile_idx_map( x265_t *h, int32_t i_cu_addr )
{
	return h->tile_idx_map[i_cu_addr] ;
}

void x265_pic_sym_set_inverse_cu_order_map( x265_t *h, int32_t i_cu_addr, int32_t i_enc_cu_order )
{
	h->inverse_cu_order_map[i_cu_addr] = i_enc_cu_order ;
}

uint32_t x265_pic_sym_get_inverse_cu_order_map( x265_t *h, int32_t i_cu_addr )
{
	return h->inverse_cu_order_map[i_cu_addr] ;
}


uint32_t x265_pic_sym_get_pic_scu_enc_order( x265_t *h, uint32_t i_scu_addr )
{
	return h->inverse_cu_order_map[i_scu_addr/h->cu.pic.i_num_partitions] * h->cu.pic.i_num_partitions
			+ i_scu_addr % h->cu.pic.i_num_partitions ;
}

uint32_t x265_pic_sym_get_pic_scu_addr( x265_t *h, uint32_t i_scu_enc_order )
{
	return h->cu_order_map[i_scu_enc_order/h->cu.pic.i_num_partitions] * h->cu.pic.i_num_partitions
			+ i_scu_enc_order % h->cu.pic.i_num_partitions;
}

int x265_pic_sym_x_create_tile_array ( x265_t *h )
{
	int32_t loop = 0 ;

    CHECKED_MALLOCZERO( h->tile,
    					(h->pps[0].i_num_columns_minus1 + 1) * (h->pps[0].i_num_rows_minus1 + 1)
    					* sizeof(x265_tile_t*) );
	for ( loop = 0 ;
			loop < (h->pps[0].i_num_columns_minus1 + 1) * (h->pps[0].i_num_rows_minus1 + 1) ;
			++ loop )
	{
        CHECKED_MALLOCZERO( h->tile[loop], sizeof(x265_tile_t) );
        if ( x265_tile_init (h->tile[loop] ) )
        {
        	goto fail ;
        }
	}

	return 0 ;
fail:
	x265_pic_sym_x_destroy_tile_array ( h ) ;
	return -1 ;
}

void x265_pic_sym_x_destroy_tile_array ( x265_t *h )
{
	int32_t loop = 0 ;

	if ( h->tile )
	{
		for ( loop = 0 ;
				loop < (h->pps[0].i_num_columns_minus1 + 1) * (h->pps[0].i_num_rows_minus1 + 1) ;
				++ loop )
		{
			x265_free ( h->tile[loop] ) ;
		}
		x265_free ( h->tile ) ;
		h->tile = NULL ;
	}
}

void x265_pic_sym_x_init_tiles ( x265_t *h )
{
	uint32_t i_tile_idx = 0 ;
	uint32_t i_column_idx = 0;
	uint32_t i_row_idx = 0;
	uint32_t i_right_edge_pos_in_cu = 0 ;
	uint32_t i_bottom_edge_pos_in_cu= 0 ;
	int32_t i = 0 , j = 0 ;

	//initialize each tile of the current picture
	for( i_row_idx = 0; i_row_idx < h->pps[0].i_num_rows_minus1 + 1; i_row_idx++ )
	{
		for( i_column_idx = 0; i_column_idx < h->pps[0].i_num_columns_minus1 + 1; i_column_idx++ )
		{
			i_tile_idx = i_row_idx * (h->pps[0].i_num_columns_minus1 + 1) + i_column_idx;

			//initialize the RightEdgePosInCU for each tile
			i_right_edge_pos_in_cu = 0;
			for( i=0; i <= i_column_idx; i++ )
			{
				i_right_edge_pos_in_cu += h->tile[i_row_idx * (h->pps[0].i_num_columns_minus1 + 1) + i]->i_tile_width;
			}
			h->tile[i_tile_idx]->i_right_edge_pos_in_cu = i_right_edge_pos_in_cu - 1 ;

			//initialize the BottomEdgePosInCU for each tile
			i_bottom_edge_pos_in_cu = 0;
			for( i=0; i <= i_row_idx; i++ )
			{
				i_bottom_edge_pos_in_cu += h->tile[i * (h->pps[0].i_num_columns_minus1 + 1) + i_column_idx]->i_tile_height ;
			}
			h->tile[i_tile_idx]->i_bottom_edge_pos_in_cu = i_bottom_edge_pos_in_cu - 1 ;

			//initialize the FirstCUAddr for each tile
			h->tile[i_tile_idx]->i_first_cu_addr
				= (h->tile[i_tile_idx]->i_bottom_edge_pos_in_cu
					- h->tile[i_tile_idx]->i_tile_height +1)
					* h->cu.pic.i_width_in_cu
					+ (h->tile[i_tile_idx]->i_right_edge_pos_in_cu
					- h->tile[i_tile_idx]->i_tile_width + 1) ;
		}
	}

	//initialize the TileIdxMap
	for( i = 0 ; i < h->cu.pic.i_num_cus_in_frame ; ++ i )
	{
		for ( j = 0 ; j < h->pps[0].i_num_columns_minus1 + 1; ++ j )
		{
			if(i % h->cu.pic.i_width_in_cu  <= h->tile[j]->i_right_edge_pos_in_cu)
			{
				i_column_idx = j;
				j = h->pps[0].i_num_columns_minus1 + 1;
			}
		}
		for(j=0; j < h->pps[0].i_num_rows_minus1 + 1; j++)
		{
			if(i/h->cu.pic.i_width_in_cu  <= h->tile[j*(h->pps[0].i_num_columns_minus1 + 1)]->i_bottom_edge_pos_in_cu)
			{
				i_row_idx = j;
				j = h->pps[0].i_num_rows_minus1 + 1;
			}
		}
		h->tile_idx_map[i] = i_row_idx * (h->pps[0].i_num_columns_minus1 + 1) + i_column_idx;
	}
}

uint32_t x265_pic_sym_x_calculate_nxt_cu_addr ( x265_t *h, uint32_t i_curr_cu_addr )
{
	uint32_t i_next_cu_addr;
	uint32_t i_tile_idx;

	//get the tile index for the current LCU
	i_tile_idx = h->tile_idx_map[i_curr_cu_addr] ;

	//get the raster scan address for the next LCU
	if( i_curr_cu_addr % h->cu.pic.i_width_in_cu == h->tile[i_tile_idx]->i_right_edge_pos_in_cu
			&& i_curr_cu_addr / h->cu.pic.i_width_in_cu == h->tile[i_tile_idx]->i_bottom_edge_pos_in_cu )
		//the current LCU is the last LCU of the tile
	{
		if ( i_tile_idx == (h->pps[0].i_num_columns_minus1 + 1) * (h->pps[0].i_num_rows_minus1 + 1) - 1 )
		{
			i_next_cu_addr = h->cu.pic.i_num_cus_in_frame;
		}
		else
		{
			i_next_cu_addr = h->tile[i_tile_idx+1]->i_first_cu_addr;
		}
	}
	else //the current LCU is not the last LCU of the tile
	{
		if( i_curr_cu_addr % h->cu.pic.i_width_in_cu == h->tile[i_tile_idx]->i_right_edge_pos_in_cu )  //the current LCU is on the rightmost edge of the tile
		{
			i_next_cu_addr = i_curr_cu_addr + h->cu.pic.i_width_in_cu - h->tile[i_tile_idx]->i_tile_width + 1;
		}
		else
		{
			i_next_cu_addr = i_curr_cu_addr + 1;
		}
	}

	return i_next_cu_addr;
}

x265_sao_param_t *x265_pic_sym_get_sao_param( x265_t *h )
{
	return h->sao_param ;
}

int32_t x265_pic_sym_is_pic_left_edge ( x265_t *h, uint32_t i_cu_addr )
{
	if ( i_cu_addr % h->cu.pic.i_width_in_cu == 0 )
	{
		return 1 ;
	}
	return 0 ;
}

int32_t x265_pic_sym_is_pic_right_edge ( x265_t *h, uint32_t i_cu_addr )
{
	if ( i_cu_addr % h->cu.pic.i_width_in_cu == h->cu.pic.i_width_in_cu - 1 )
	{
		return 1 ;
	}
	return 0 ;
}

int32_t x265_pic_sym_is_pic_top_edge ( x265_t *h, uint32_t i_cu_addr )
{
	if ( i_cu_addr / h->cu.pic.i_width_in_cu == 0 )
	{
		return 1 ;
	}
	return 0 ;
}

int32_t x265_pic_sym_is_pic_bottom_edge ( x265_t *h, uint32_t i_cu_addr )
{
	if ( i_cu_addr / h->cu.pic.i_width_in_cu == h->cu.pic.i_height_in_cu - 1 )
	{
		return 1 ;
	}
	return 0 ;
}

int32_t x265_pic_sym_is_tile_left_edge ( x265_t *h, uint32_t i_cu_addr )
{
	uint32_t i_tile_idx;

	i_tile_idx = h->tile_idx_map[i_cu_addr] ;
	if ( (i_cu_addr % h->cu.pic.i_width_in_cu ) ==
		(h->tile[i_tile_idx]->i_right_edge_pos_in_cu + 1
		- h->tile[i_tile_idx]->i_tile_width )
		)
	{
		return 1 ;
	}

	return 0 ;

}

int32_t x265_pic_sym_is_tile_right_edge ( x265_t *h, uint32_t i_cu_addr )
{
	uint32_t i_tile_idx;

	i_tile_idx = h->tile_idx_map[i_cu_addr] ;
	if ( (i_cu_addr % h->cu.pic.i_width_in_cu ) ==
		h->tile[i_tile_idx]->i_right_edge_pos_in_cu
		)
	{
		return 1 ;
	}

	return 0 ;

}

int32_t x265_pic_sym_is_tile_top_edge ( x265_t *h, uint32_t i_cu_addr )
{
	uint32_t i_tile_idx;

	i_tile_idx = h->tile_idx_map[i_cu_addr] ;
	if ( (i_cu_addr / h->cu.pic.i_width_in_cu ) ==
		(h->tile[i_tile_idx]->i_bottom_edge_pos_in_cu + 1
		- h->tile[i_tile_idx]->i_tile_height )
		)
	{
		return 1 ;
	}

	return 0 ;

}


int32_t x265_pic_sym_is_tile_bottom_edge ( x265_t *h, uint32_t i_cu_addr )
{
	uint32_t i_tile_idx;

	i_tile_idx = h->tile_idx_map[i_cu_addr] ;
	if ( (i_cu_addr / h->cu.pic.i_width_in_cu ) ==
		h->tile[i_tile_idx]->i_bottom_edge_pos_in_cu
		)
	{
		return 1 ;
	}

	return 0 ;

}


