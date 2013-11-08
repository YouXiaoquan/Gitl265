

#include "common/common.h"


void init_sig_last_scan8x8 ( x265_scan_t *scan )
{
	scan->sig_last_scan_8x8[0][0] = 0 ;
	scan->sig_last_scan_8x8[0][1] = 2 ;
	scan->sig_last_scan_8x8[0][2] = 1 ;
	scan->sig_last_scan_8x8[0][3] = 3 ;

	scan->sig_last_scan_8x8[1][0] = 0 ;
	scan->sig_last_scan_8x8[1][1] = 1 ;
	scan->sig_last_scan_8x8[1][2] = 2 ;
	scan->sig_last_scan_8x8[1][3] = 3 ;

	scan->sig_last_scan_8x8[2][0] = 0 ;
	scan->sig_last_scan_8x8[2][1] = 2 ;
	scan->sig_last_scan_8x8[2][2] = 1 ;
	scan->sig_last_scan_8x8[2][3] = 3 ;

}

void init_sig_last_scan ( x265_scan_t *scan,
						x265_t *h,
						uint32_t *buff_d,
						uint32_t *buff_h,
						uint32_t *buff_v,
						int32_t i_width,
						int32_t i_height )
{
	uint32_t i_next_scan_pos = 0;
	uint32_t *buff_temp = NULL ;
	uint32_t i_scan_line = 0 ;
    int32_t i_prim_dim = 0 ;
    int32_t i_scnd_dim = 0 ;
    uint32_t i_num_blk_side = 0 ;
    uint32_t i_num_blks = 0 ;
    uint32_t i_log2_blk = 0 ;
    uint32_t i_blk = 0 ;
    uint32_t i_init_blk_pos = 0 ;
    uint32_t i_offset_x = 0, i_offset_y = 0, i_offset = 0, i_offset_d = 0, i_offset_scan = 0 ;
    uint32_t i_cnt = 0 ;
    int32_t i_blk_x = 0, i_blk_y = 0, x = 0, y = 0 ;
	const uint32_t i_num_scan_pos = i_width * i_width ;

	init_sig_last_scan8x8 ( scan ) ;
	if( i_width < 16 )
	{
		buff_temp = buff_d ;
		if( i_width == 8 )
		{
			buff_temp = h->scan.sig_last_scan_cg_32x32 ;
		}
		for( i_scan_line = 0 ; i_next_scan_pos < i_num_scan_pos ; ++ i_scan_line )
		{
			i_prim_dim = i_scan_line ;
		    i_scnd_dim = 0 ;
		    while ( i_prim_dim >= i_width )
		    {
		    	++ i_scnd_dim ;
		    	-- i_prim_dim ;
		    }
		    while( i_prim_dim >= 0 && i_scnd_dim < i_width )
		    {
		    	buff_temp[ i_next_scan_pos ] = i_prim_dim * i_width + i_scnd_dim ;
		    	++ i_next_scan_pos ;
		    	++ i_scnd_dim ;
		    	-- i_prim_dim ;
		    }
		}
	}
	if( i_width > 4 )
	{
		i_num_blk_side = i_width >> 2 ;
		i_num_blks = i_num_blk_side * i_num_blk_side ;
		//	log2(i_num_blk_side)-1
		i_log2_blk = h->global.convert_to_bit [i_num_blk_side] + 1 ;
		for( i_blk = 0; i_blk < i_num_blks ; ++ i_blk )
		{
			i_next_scan_pos = 0 ;
			//	scan->sig_last_scan[ SCAN_DIAG ][ i_log2_blk ] =
			//	scan->sig_last_scan[ SCAN_DIAG ][ log2(i_num_blk_side)-1 ]
			//	(1 << log2(i_num_blk_side)-1+1) = i_num_blk_side
			i_init_blk_pos = scan->sig_last_scan[SCAN_DIAG][i_log2_blk][i_blk] ;
			if( i_width == 32 )
			{
				i_init_blk_pos = scan->sig_last_scan_cg_32x32[ i_blk ] ;
			}
			i_offset_y    = i_init_blk_pos / i_num_blk_side ;
			i_offset_x    = i_init_blk_pos - i_offset_y * i_num_blk_side ;
			i_offset_d    = 4 * ( i_offset_x + i_offset_y * i_width ) ;
			i_offset_scan = 16 * i_blk;
			for( i_scan_line = 0 ; i_next_scan_pos < 16 ; ++ i_scan_line )
			{
				i_prim_dim = i_scan_line ;
				i_scnd_dim = 0;
				while( i_prim_dim >= 4 )
				{
					++ i_scnd_dim ;
					-- i_prim_dim ;
				}
				while( i_prim_dim >= 0 && i_scnd_dim < 4 )
				{
					buff_d[i_next_scan_pos + i_offset_scan] = i_prim_dim * i_width + i_scnd_dim
																+ i_offset_d;
					++ i_next_scan_pos ;
					++ i_scnd_dim ;
					-- i_prim_dim ;
				}
			}
		}
	}

	i_cnt = 0 ;
	if( i_width > 2 )
	{
		i_num_blk_side = i_width >> 2;
		for ( i_blk_y = 0 ; i_blk_y < i_num_blk_side ; ++ i_blk_y )
		{
			for ( i_blk_x = 0 ; i_blk_x < i_num_blk_side ; ++ i_blk_x )
			{
				i_offset = i_blk_y * 4 * i_width + i_blk_x * 4;
				for ( y = 0 ; y < 4 ; ++ y )
				{
					for ( x = 0 ; x < 4 ; ++ x )
					{
						buff_h[i_cnt] = y*i_width + x + i_offset;
						i_cnt ++;
					}
				}
			}
		}

		i_cnt = 0;
		for ( i_blk_x = 0 ; i_blk_x < i_num_blk_side ; ++ i_blk_x )
		{
			for ( i_blk_y = 0 ; i_blk_y < i_num_blk_side ; ++ i_blk_y )
			{
				i_offset = i_blk_y * 4 * i_width + i_blk_x * 4 ;
				for ( x = 0 ; x < 4 ; ++ x )
				{
					for ( y = 0 ; y < 4 ; ++ y )
					{
						buff_v [ i_cnt ] = y * i_width + x + i_offset;
						i_cnt ++;
					}
				}
			}
		}
	}
	else
	{
		for( y = 0 ; y < i_height; ++ y )
		{
			for( x = 0 ; x < i_width; ++ x )
			{
				buff_h[i_cnt] = y * i_width + x ;
				i_cnt ++;
			}
		}

		i_cnt = 0;
		for( x = 0 ; x < i_width; ++ x )
		{
			for( y = 0 ; y < i_height; ++ y )
			{
				buff_v[i_cnt] = y * i_width + x ;
				i_cnt ++;
			}
		}
	}
}

int xPrintSigLastScan ( FILE *file, uint32_t *sig_last_scan, int32_t i_width )
{
	int32_t loop = 0 ;
	int32_t i_count = 0 ;
	uint32_t *invert_sig_last_scan = NULL ;
	int32_t x = 0, y = 0 ;
	int32_t i_index = 0 ;

	i_count = i_width * i_width ;
	CHECKED_MALLOCZERO ( invert_sig_last_scan, i_count * sizeof (uint32_t) ) ;

	for ( loop = 0 ; loop < i_count ; ++ loop )
	{
		invert_sig_last_scan[sig_last_scan[loop]] = loop ;
	}

	for ( y = 0 ; y < i_width ; ++ y )
	{
		for ( x = 0 ; x < i_width ; ++ x )
		{
			fprintf ( file, "%d ", invert_sig_last_scan[i_index++] ) ;
		}
		fprintf ( file, "\n" ) ;
	}

	x265_free ( invert_sig_last_scan ) ;
	return 0 ;

fail:
	x265_free ( invert_sig_last_scan ) ;
	return -1 ;
}

void print_sig_last_scan ( x265_scan_t *scan )
{
	int32_t i = 0, j = 0 ;
	FILE *file = NULL ;

	file = fopen ( "SigLastScan2", "wb" ) ;

	for ( i = 0 ; i < X265_MAX_CU_DEPTH ; ++ i )
	{
		for ( j = 0 ; j < 3 ; ++ j )
		{
			xPrintSigLastScan ( file, scan->sig_last_scan[j][i], (1 << ( i + 1 ))) ;
		}
	}
	xPrintSigLastScan ( file, scan->sig_last_scan_cg_32x32, 8) ;


	fclose ( file ) ;
}

void print_zscan_to_raster ( x265_scan_t *scan )
{
	int32_t i, j, k = 0;
	FILE *file = NULL;

	file = fopen( "ZscanToRaster2", "wb") ;

	assert(file) ;

	for (i = 0; i < X265_MAX_NUM_SPU_W / 2; ++i)
	{
		for (j = 0; j < X265_MAX_NUM_SPU_W / 2; ++j)
			fprintf ( file, "%d ", scan->zscan_to_raster[k++] ) ;
		fprintf ( file, "\n" ) ;
	}

	fclose (file) ;
}

void print_raster_to_zscan ( x265_scan_t *scan )
{
	int32_t i, j, k = 0;
	FILE *file = NULL;

	file = fopen( "RasterToZscan2", "wb") ;

	assert(file) ;

	for (i = 0; i < X265_MAX_NUM_SPU_W / 2; ++i)
	{
		for (j = 0; j < X265_MAX_NUM_SPU_W / 2; ++j)
			fprintf ( file, "%d ", scan->raster_to_zscan[k++] ) ;
		fprintf ( file, "\n" ) ;
	}

	fclose (file) ;
}

void init_zscan_to_raster ( int32_t iMaxDepth,
							int32_t iDepth,
							uint32_t uiStartVal,
							uint32_t** rpuiCurrIdx )
{
	int32_t iStride = 1 << ( iMaxDepth - 1 ) ;
	int32_t iStep = iStride >> iDepth;
	if ( iDepth == iMaxDepth )
	{
		(*rpuiCurrIdx)[0] = uiStartVal;
		(*rpuiCurrIdx)++;
	}
	else
	{
		init_zscan_to_raster( iMaxDepth, iDepth+1, uiStartVal,                     rpuiCurrIdx ) ;
		init_zscan_to_raster( iMaxDepth, iDepth+1, uiStartVal+iStep,               rpuiCurrIdx ) ;
		init_zscan_to_raster( iMaxDepth, iDepth+1, uiStartVal+iStep*iStride,       rpuiCurrIdx ) ;
		init_zscan_to_raster( iMaxDepth, iDepth+1, uiStartVal+iStep*iStride+iStep, rpuiCurrIdx ) ;
	}
}

void init_raster_to_zscan ( x265_t *h,
							x265_scan_t *scan )
{
	uint32_t loop = 0 ;

	for ( loop = 0; loop < h->cu.pic.i_num_partitions ; ++ loop )
	{
		scan->raster_to_zscan[ scan->zscan_to_raster[loop] ] = loop;
	}
}

void print_raster_to_pel_xy ( x265_t *h, x265_scan_t *scan )
{
	int32_t i_index = 0 ;
	int32_t i = 0, j = 0 ;

	FILE *file = NULL ;

	file = fopen ( "RasterToPelXY2", "wb" ) ;
	i_index = 0 ;
	for ( i = 0 ; i < h->cu.pic.i_num_part_in_width ; ++ i )
	{
		for ( j = 0 ; j < h->cu.pic.i_num_part_in_height ; ++ j )
		{
			fprintf ( file, "%d ", scan->raster_to_pel_x[i_index] ) ;
			++ i_index ;
		}
		fprintf ( file, "\n" ) ;
	}
	i_index = 0 ;
	for ( i = 0 ; i < h->cu.pic.i_num_part_in_width ; ++ i )
	{
		for ( j = 0 ; j < h->cu.pic.i_num_part_in_height ; ++ j )
		{
			fprintf ( file, "%d ", scan->raster_to_pel_y[i_index] ) ;
			++ i_index ;
		}
		fprintf ( file, "\n" ) ;
	}

	fclose ( file ) ;
}

void init_raster_to_pel_xy ( x265_t *h, x265_scan_t *scan )
{
	uint32_t loop = 0 ;

	uint32_t *temp_x = scan->raster_to_pel_x ;
	uint32_t *temp_y = scan->raster_to_pel_y ;

	temp_x[0] = 0;
	temp_x++;
	for ( loop = 1 ; loop < h->cu.pic.i_num_part_in_width ; ++ loop )
	{
		temp_x[0] = temp_x[-1] + h->cu.pic.i_min_cu_width ;
		temp_x++ ;
	}
	for ( loop = 1; loop < h->cu.pic.i_num_part_in_height ; ++ loop )
	{
		memcpy(temp_x,
				temp_x - h->cu.pic.i_num_part_in_width,
				sizeof(uint32_t) * h->cu.pic.i_num_part_in_width ) ;
		temp_x += h->cu.pic.i_num_part_in_width ;
	}

	for ( loop = 1; loop < h->cu.pic.i_num_partitions ; ++ loop )
	{
		temp_y[loop] = ( loop / h->cu.pic.i_num_part_in_width ) * h->cu.pic.i_min_cu_width ;
	}
	//	print_raster_to_pel_xy ( h, scan ) ;
}

int x265_scan_init ( x265_scan_t *scan, x265_t *h )
{
	int32_t i = 0, j = 0 ;
	int32_t loop = 0, i_length = 0 ;
	uint32_t *p_temp = NULL ;

	memset ( scan, 0, sizeof (x265_scan_t) ) ;
	i_length = 2 ;

	CHECKED_MALLOCZERO( scan->sig_last_scan_8x8, 3 * sizeof(uint32_t*) ) ;
	for ( loop = 0 ; loop < 3 ; ++ loop )
	{
		CHECKED_MALLOCZERO( scan->sig_last_scan_8x8[loop], 4 * sizeof(uint32_t) ) ;
	}
	for ( i = 0 ; i < X265_MAX_CU_DEPTH ; ++ i )
	{
		for ( j = 0 ; j <= SCAN_VER ; ++ j )
		{
			CHECKED_MALLOCZERO( scan->sig_last_scan[j][i],
								i_length * i_length * sizeof(uint32_t) ) ;
		}
		i_length <<= 1 ;
	}
	CHECKED_MALLOCZERO( scan->sig_last_scan_cg_32x32, 64 * sizeof(uint32_t) ) ;

	i_length = 2 ;
	for ( loop = 0 ; loop < X265_MAX_CU_DEPTH ; ++ loop )
	{
		init_sig_last_scan ( &h->scan, h,
							scan->sig_last_scan[0][loop],
							scan->sig_last_scan[1][loop],
							scan->sig_last_scan[2][loop],
							i_length, i_length ) ;

		i_length <<= 1 ;
	}

	CHECKED_MALLOCZERO( scan->zscan_to_raster,
						X265_MAX_NUM_SPU_W * X265_MAX_NUM_SPU_W * sizeof(uint32_t) ) ;
	p_temp = &h->scan.zscan_to_raster[0] ;
	init_zscan_to_raster ( h->cu.pic.i_total_depth + 1, 1, 0, &p_temp ) ;

	CHECKED_MALLOCZERO( scan->raster_to_zscan,
						X265_MAX_NUM_SPU_W * X265_MAX_NUM_SPU_W * sizeof(uint32_t)) ;
	CHECKED_MALLOCZERO( scan->raster_to_pel_x,
						X265_MAX_NUM_SPU_W * X265_MAX_NUM_SPU_W * sizeof(uint32_t)) ;
	CHECKED_MALLOCZERO( scan->raster_to_pel_y,
						X265_MAX_NUM_SPU_W * X265_MAX_NUM_SPU_W * sizeof(uint32_t)) ;
	init_raster_to_zscan ( h, scan ) ;
	init_raster_to_pel_xy ( h, scan ) ;

	//	print_sig_last_scan ( &h->scan ) ;
	//	print_zscan_to_raster ( &h->scan ) ;
	//	print_raster_to_zscan ( &h->scan ) ;
	//	print_raster_to_pel_xy ( h, &h->scan ) ;

	return 0 ;

fail:
	x265_scan_deinit ( scan ) ;
	return -1 ;
}


void x265_scan_deinit ( x265_scan_t *scan )
{
	int32_t loop = 0 ;
	int32_t i = 0, j = 0 ;

	x265_free ( scan->raster_to_pel_y ) ;
	x265_free ( scan->raster_to_pel_x ) ;
	x265_free ( scan->raster_to_zscan ) ;
	x265_free ( scan->zscan_to_raster ) ;

	x265_free ( scan->sig_last_scan_cg_32x32 ) ;
	for ( i = 0 ; i < X265_MAX_CU_DEPTH ; ++ i )
	{
		for ( j = 0 ; j <= SCAN_VER ; ++ j )
		{
			x265_free ( scan->sig_last_scan[j][i] ) ;
		}
	}

	if (scan->sig_last_scan_8x8)
	{
		for ( loop = 0 ; loop < 3 ; ++ loop )
		{
			x265_free (scan->sig_last_scan_8x8[loop]) ;
			scan->sig_last_scan_8x8[loop] = NULL ;
		}
		x265_free (scan->sig_last_scan_8x8) ;
		scan->sig_last_scan_8x8 = NULL ;
	}

}
