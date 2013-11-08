

#include "common.h"

void x265_frame_delete( x265_t *h, x265_frame_t *frame ) ;


/*
int align_stride( int x, int align, int disalign )
{
    x = ALIGN( x, align );
    if( !(x&(disalign-1)) )
        x += align;
    return x;
}

static int align_plane_size( int x, int disalign )
{
    if( !(x&(disalign-1)) )
        x += 128;
    return x;
}
*/

void destroy_fenc_data ( x265_fenc_data_t *fenc_data )
{

}

int create_fenc_data ( x265_t *h, x265_fenc_data_t *fenc_data )
{

	return 0 ;
}

int init_fenc_data ( x265_t *h, x265_fenc_data_t *fenc_data )
{
	memset ( fenc_data, 0, sizeof(x265_fenc_data_t) ) ;

	if ( create_fenc_data (h, fenc_data) )
	{
		return -1 ;
	}

	return 0 ;

}

void deinit_fenc_data ( x265_fenc_data_t *fenc_data )
{
	destroy_fenc_data ( fenc_data ) ;
}

void destroy_fdec_data ( x265_t *h, x265_fdec_data_t *fdec_data )
{
	int32_t loop = 0 ;

	if ( fdec_data->tmvp_data_cu )
	{
		for ( loop = 0; loop < h->cu.pic.i_num_cus_in_frame ; ++ loop )
		{
			if ( fdec_data->tmvp_data_cu[loop] )
			{
				x265_tmvp_data_cu_destroy ( fdec_data->tmvp_data_cu[loop] ) ;
				x265_tmvp_data_cu_delete ( fdec_data->tmvp_data_cu[loop] ) ;
				fdec_data->tmvp_data_cu[loop] = NULL ;
			}
		}
		x265_free ( fdec_data->tmvp_data_cu ) ;
		fdec_data->tmvp_data_cu = NULL ;
    }

	x265_free ( fdec_data->is_used_as_long_term[1] ) ;
	x265_free ( fdec_data->is_used_as_long_term[0] ) ;
	x265_free ( fdec_data->ref_poc[1] ) ;
	x265_free ( fdec_data->ref_poc[0] ) ;
}

int create_fdec_data ( x265_t *h, x265_fdec_data_t *fdec_data )
{
	int32_t loop = 0 ;

    CHECKED_MALLOCZERO( fdec_data->ref_poc[0],
    					(h->frames.i_reference_and_current + 1) * sizeof(int32_t *) );
    CHECKED_MALLOCZERO( fdec_data->ref_poc[1],
    					(h->frames.i_reference_and_current + 1) * sizeof(int32_t *) );
    CHECKED_MALLOCZERO( fdec_data->is_used_as_long_term[0],
    					(h->frames.i_reference_and_current + 1) * sizeof(int32_t *) );
    CHECKED_MALLOCZERO( fdec_data->is_used_as_long_term[1],
    					(h->frames.i_reference_and_current + 1) * sizeof(int32_t *) );

    CHECKED_MALLOCZERO( fdec_data->tmvp_data_cu,
    					h->cu.pic.i_num_cus_in_frame * sizeof(x265_tmvp_data_cu_t *) );
    for ( loop = 0; loop < h->cu.pic.i_num_cus_in_frame ; ++ loop )
    {
    	fdec_data->tmvp_data_cu[loop] = x265_tmvp_data_cu_new () ;
    	if ( NULL == fdec_data->tmvp_data_cu[loop] )
    	{
    		goto fail ;
    	}
    	if ( x265_tmvp_data_cu_create ( fdec_data->tmvp_data_cu[loop],
    									h->cu.pic.i_num_partitions,
    									h->cu.pic.i_max_cu_width,
    									h->cu.pic.i_max_cu_height,
    									(h->cu.pic.i_max_cu_width >> h->cu.pic.i_total_depth)
    									))
    	{
    		goto fail ;
    	}
    }
	return 0 ;


fail:
	destroy_fdec_data ( h, fdec_data ) ;
	return -1 ;

}

int init_fdec_data ( x265_t *h, x265_fdec_data_t *fdec_data )
{
	memset ( fdec_data, 0, sizeof(x265_fdec_data_t) ) ;

	if ( create_fdec_data (h, fdec_data) )
	{
		return -1 ;
	}

	return 0 ;

}

void deinit_fdec_data ( x265_t *h, x265_fdec_data_t *fdec_data )
{
	destroy_fdec_data ( h, fdec_data ) ;
}

static x265_frame_t *x265_frame_new( x265_t *h, int b_fdec )
{
    x265_frame_t *frame;
    int i_stride, i_width, i_lines, luma_plane_count;
    int i_padv = 0 ;
    int i_padh = 0 ;
	int chroma_padv = 0;
	int chroma_plane_size = 0;
	int loop = 0 ;

    /*
    static int i_frame_dec_allocated = 0 ;
    if ( b_fdec )
    {
    	i_frame_dec_allocated ++ ;
    	fprintf ( stderr, "frame dec allocated = %d\n", i_frame_dec_allocated ) ;
    }
    */

    CHECKED_MALLOCZERO( frame, sizeof(x265_frame_t) );
    i_padh = h->cu.pic.i_max_cu_width + 16 ;
    i_padv = h->cu.pic.i_max_cu_height + 16 ;
    /* allocate frame data (+64 for extra data for me) */
    i_width  = h->param.i_width;
    i_lines  = h->param.i_height;
    i_stride = i_width + 2*i_padh ;

    luma_plane_count = 1;
    frame->i_plane = 3;
	for( loop = 0; loop < luma_plane_count; ++loop )
	{
		frame->i_width[loop] = i_width ;
		frame->i_lines[loop] = i_lines ;
		frame->i_stride[loop] = i_stride ;
	}
	for( loop = luma_plane_count; loop < frame->i_plane; ++loop )
	{
		frame->i_width[loop] = i_width >> 1 ;
		frame->i_lines[loop] = i_lines >> 1 ;
		frame->i_stride[loop] = i_stride >> 1 ;
	}
    frame->i_poc = -1;
    frame->i_pts = -1;
    frame->b_fdec = b_fdec;

	chroma_padv = i_padv >> 1;
	chroma_plane_size = (frame->i_stride[1] * (frame->i_lines[1] + 2*chroma_padv));
	for( loop = luma_plane_count; loop < frame->i_plane; ++loop )
	{
		CHECKED_MALLOCZERO( frame->buffer[loop], chroma_plane_size * sizeof(pixel) );
		frame->plane[loop] = frame->buffer[loop] + frame->i_stride[loop] * chroma_padv + (i_padh>>1) ;
	}


    /* all 4 luma planes allocated together, since the cacheline split code
     * requires them to be in-phase wrt cacheline alignment. */

    for( int p = 0; p < luma_plane_count; p++ )
    {
        int luma_plane_size = frame->i_stride[p] * (frame->i_lines[p] + 2*i_padv) ;
        CHECKED_MALLOCZERO( frame->buffer[p], luma_plane_size * sizeof(pixel) );
        frame->plane[p] = frame->buffer[p] + frame->i_stride[p] * i_padv + i_padh ;
    }

    if ( frame->b_fdec )
    {
        CHECKED_MALLOCZERO( frame->fdec_data, sizeof(x265_fdec_data_t) );
        if ( init_fdec_data ( h, frame->fdec_data ) )
		{
			goto fail ;
		}
    }
    else
    {
        CHECKED_MALLOCZERO( frame->fenc_data, sizeof(x265_fenc_data_t) );
        if ( init_fenc_data ( h, frame->fenc_data ) )
		{
			goto fail ;
		}
    }

    return frame;

fail:
	x265_frame_delete( h, frame );
    return NULL;
}

void x265_frame_delete( x265_t *h, x265_frame_t *frame )
{
    /* Duplicate frames are blank copies of real frames (including pointers),
     * so freeing those pointers would cause a double free later. */
    //static int i_frame_dec_delete = 0 ;

	/*
	if ( frame->b_fdec )
	{
		i_frame_dec_delete ++ ;
		fprintf ( stderr, "frame dec delete = %d\n", i_frame_dec_delete ) ;
	}
	*/

	if ( frame->b_fdec )
	{
		deinit_fdec_data ( h, frame->fdec_data ) ;
		x265_free ( frame->fdec_data ) ;
	}
	else
	{
		deinit_fenc_data ( frame->fenc_data ) ;
		x265_free ( frame->fenc_data ) ;
	}

	for( int i = 0; i < 4; i++ )
	{
		x265_free( frame->buffer[i] );
	}

    x265_free( frame );
}


void x265_frame_write ( x265_t *h, x265_frame_t *frame )
{
	static int32_t b_first_x265_frame_write = 1 ;
	int32_t i_num_of_bytes_to_write = 0;
    int i_padv = 0 ;
	FILE *file = NULL ;

	i_padv = h->cu.pic.i_max_cu_height + 16 ;
	if ( b_first_x265_frame_write )
	{
		file = fopen ( "YuvPic2", "wb" ) ;
		b_first_x265_frame_write = 0 ;
	}
	else
	{
		file = fopen ( "YuvPic2", "r+b" ) ;
		fseek ( file, 0, SEEK_END ) ;
	}

	i_num_of_bytes_to_write = frame->i_stride[0]
						* ( frame->i_lines[0] + (i_padv  <<1))
						* sizeof(pixel) ;

	fwrite ( frame->buffer[0], 1, i_num_of_bytes_to_write, file ) ;
	i_num_of_bytes_to_write = frame->i_stride[1]
						* (frame->i_lines[1] + ((i_padv>>1)<<1))
						* sizeof(pixel) ;
	fwrite ( frame->buffer[1], 1, i_num_of_bytes_to_write, file ) ;
	fwrite ( frame->buffer[2], 1, i_num_of_bytes_to_write, file ) ;
	fclose ( file ) ;

}

int x265_frame_copy_picture( x265_t *h, x265_frame_t *dst, x265_picture_t *src )
{
	int32_t loop = 0 ;
	int32_t luma_plane_count = 0 ;
    dst->i_pts = src->i_pts;

    luma_plane_count = 1 ;

    for ( loop = 0 ; loop < luma_plane_count ; ++loop )
    {
    	h->mc.plane_copy( dst->plane[loop], dst->i_stride[loop], src->img.plane[loop],
    					src->img.i_stride[loop], h->param.i_width, h->param.i_height );
    }

    for ( loop = 1 ; loop < dst->i_plane ; ++loop )
    {
    	h->mc.plane_copy( dst->plane[loop], dst->i_stride[loop], src->img.plane[loop],
    					src->img.i_stride[loop], h->param.i_width>>1, h->param.i_height>>1 );
    }

    //	x265_frame_write ( h, dst ) ;
    return 0;
}

void x265_frame_compress_motion ( x265_t *h, x265_frame_t *frame )
{
	uint32_t i_cu_addr = 0 ;

	for ( i_cu_addr = 0 ;
			i_cu_addr < h->cu.pic.i_height_in_cu * h->cu.pic.i_width_in_cu ;
			++ i_cu_addr )
	{
		x265_tmvp_data_cu_compress_mv ( frame->fdec_data->tmvp_data_cu[i_cu_addr] ) ;
	}
}


static void ALWAYS_INLINE pixel_memset( pixel *dst, pixel *src, int len, int size )
{
    uint8_t *dstp = (uint8_t*)dst;
    uint32_t v1 = *src;
    uint32_t v2 = size == 1 ? v1 + (v1 <<  8) : M16( src );
    uint32_t v4 = size <= 2 ? v2 + (v2 << 16) : M32( src );
    int i = 0;
    len *= size;

    /* Align the input pointer if it isn't already */
    if( (intptr_t)dstp & (WORD_SIZE - 1) )
    {
        if( size <= 2 && ((intptr_t)dstp & 3) )
        {
            if( size == 1 && ((intptr_t)dstp & 1) )
                dstp[i++] = v1;
            if( (intptr_t)dstp & 2 )
            {
                M16( dstp+i ) = v2;
                i += 2;
            }
        }
        if( WORD_SIZE == 8 && ((intptr_t)dstp & 4 ))
        {
            M32( dstp+i ) = v4;
            i += 4;
        }
    }

    /* Main copy loop */
    if( WORD_SIZE == 8 )
    {
        uint64_t v8 = v4 + ((uint64_t)v4<<32);
        for( ; i < len - 7; i+=8 )
            M64( dstp+i ) = v8;
    }
    for( ; i < len - 3; i+=4 )
        M32( dstp+i ) = v4;

    /* Finish up the last few bytes */
    if( size <= 2 )
    {
        if( i < len - 1 )
        {
            M16( dstp+i ) = v2;
            i += 2;
        }
        if( size == 1 && i != len )
            dstp[i] = v1;
    }
}

static void ALWAYS_INLINE plane_expand_border( pixel *pix, int i_stride, int i_width, int i_height, int i_padh, int i_padv, int b_pad_top, int b_pad_bottom )
{
#define PPIXEL(x, y) ( pix + (x) + (y)*i_stride )
    for( int y = 0; y < i_height; y++ )
    {
        /* left band */
        pixel_memset( PPIXEL(-i_padh, y), PPIXEL(0, y), i_padh, sizeof(pixel) );
        /* right band */
        pixel_memset( PPIXEL(i_width, y), PPIXEL(i_width-1, y), i_padh, sizeof(pixel) );
    }
    /* upper band */
    if( b_pad_top )
        for( int y = 0; y < i_padv; y++ )
            memcpy( PPIXEL(-i_padh, -y-1), PPIXEL(-i_padh, 0), (i_width+2*i_padh) * sizeof(pixel) );
    /* lower band */
    if( b_pad_bottom )
        for( int y = 0; y < i_padv; y++ )
            memcpy( PPIXEL(-i_padh, i_height+y), PPIXEL(-i_padh, i_height-1), (i_width+2*i_padh) * sizeof(pixel) );
#undef PPIXEL
}

void x265_frame_expand_border( x265_t *h, x265_frame_t *frame )
{
    int pad_top = 1 ;
    int pad_bot = 1 ;
    int padh = 0;
    int padv = 0 ;
    int loop = 0 ;

    if ( ! frame->fdec_data->b_is_border_extended )
    {
		padh = h->cu.pic.i_max_cu_width + 16;
		padv = h->cu.pic.i_max_cu_height + 16 ;

		for( loop = 0; loop < 1; ++loop )
		{
			plane_expand_border( frame->plane[loop], frame->i_stride[loop],
								frame->i_width[loop], frame->i_lines[loop],
								padh, padv, pad_top, pad_bot );
		}

		padh >>= 1 ;
		padv >>= 1 ;
		for( loop = 1; loop < frame->i_plane; loop++ )
		{
			plane_expand_border( frame->plane[loop], frame->i_stride[loop],
								frame->i_width[loop], frame->i_lines[loop],
								padh, padv, pad_top, pad_bot );
		}
		frame->fdec_data->b_is_border_extended = 1 ;
    }
}


/* list operators */

void x265_frame_push( x265_frame_t **list, x265_frame_t *frame )
{
    int i = 0;
    while( list[i] ) i++;
    list[i] = frame;
}

x265_frame_t *x265_frame_pop( x265_frame_t **list )
{
    x265_frame_t *frame;
    int i = 0;
    assert( list[0] );
    while( list[i+1] ) i++;
    frame = list[i];
    list[i] = NULL;
    return frame;
}

void x265_frame_unshift( x265_frame_t **list, x265_frame_t *frame )
{
    int i = 0;
    while( list[i] ) i++;
    while( i-- )
        list[i+1] = list[i];
    list[0] = frame;
}

x265_frame_t *x265_frame_shift( x265_frame_t **list )
{
    x265_frame_t *frame = list[0];
    int i;
    for( i = 0; list[i]; i++ )
        list[i] = list[i+1];
    assert(frame);
    return frame;
}

void x265_frame_push_unused( x265_t *h, x265_frame_t *frame )
{
	x265_frame_push( h->frames.unused[frame->b_fdec], frame );
}

x265_frame_t *x265_frame_pop_unused( x265_t *h, int b_fdec )
{
    x265_frame_t *frame;
    if( h->frames.unused[b_fdec][0] )
    {
        frame = x265_frame_pop( h->frames.unused[b_fdec] );
    }
    else
    {
        frame = x265_frame_new( h, b_fdec );
    }
    if( !frame )
    {
        return NULL;
    }

    return frame;
}

void x265_frame_buffering ( x265_t *h, x265_frame_t *frame )
{
	int i_insert_point = 0 ;
	int i_shift_pos = 0 ;

	while ( h->frames.buffered[i_insert_point]
	        && h->frames.buffered[i_insert_point]->i_pts < frame->i_pts )
	{
		i_insert_point ++ ;
	}

	i_shift_pos = i_insert_point ;
	while ( h->frames.buffered[i_shift_pos ] )
	{
		++ i_shift_pos ;
	}
	while ( i_shift_pos > i_insert_point )
	{
		h->frames.buffered[i_shift_pos] = h->frames.buffered[i_shift_pos - 1] ;
		--i_shift_pos ;
	}

	h->frames.buffered[i_insert_point] = frame ;
	h->frames.i_frame_buf_num ++ ;
}

x265_frame_t * x265_frame_buffering_pop ( x265_t *h )
{
	x265_frame_t *frame = NULL ;
	int i_index = 1 ;

	frame = h->frames.buffered[0] ;
	while ( h->frames.buffered[i_index] )
	{
		h->frames.buffered[i_index-1] = h->frames.buffered[i_index] ;
		i_index ++ ;
	}
	h->frames.buffered[i_index-1] = NULL ;

	return frame ;
}

int x265_frame_list_size ( x265_frame_t **list )
{
	int i_size = 0 ;

	while ( list[i_size] )
	{
		i_size ++ ;
	}
	return i_size ;
}

x265_frame_t *x265_frame_get_from_poc ( x265_frame_t **list, int32_t i_poc )
{
	int i_index = 0 ;
	while ( list[i_index] )
	{
		if ( i_poc == list[i_index]->i_poc )
		{
			return list[i_index] ;
		}
		i_index ++ ;
	}

	return NULL ;
}

x265_frame_t *x265_frame_get_long_term_from_poc ( x265_frame_t **list, int32_t i_poc, int32_t i_bits_for_poc )
{
	int i_index = 0 ;
	x265_frame_t *frame = NULL ;
	x265_frame_t *st_pic = NULL ;

	frame = list[i_index] ;
	st_pic = frame ;
	while ( list[i_index] )
	{
	    if ( ( list[i_index]->i_poc % i_bits_for_poc ) == ( i_poc % ( 1 << i_bits_for_poc ) ) )
	    {
	    	if ( frame->fdec_data->b_is_long_term )
	    	{
	    		return frame ;
	    	}
	    	else
	    	{
	    		st_pic = frame ;
	    	}
	    	break;
	    }
		i_index ++ ;
	}
	return st_pic ;
}


x265_frame_t *x265_frame_pop_from_poc ( x265_frame_t **list, int32_t i_poc )
{
	int i_index = 0 ;
	x265_frame_t *frame = NULL ;
	while ( list[i_index] )
	{
		if ( i_poc == list[i_index]->i_poc )
		{
			frame = list[i_index] ;
			i_index ++ ;
			while ( list[i_index] )
			{
				list[i_index-1] = list[i_index] ;
				i_index ++ ;
			}
			list[i_index-1] = NULL ;
			return frame ;
		}
		i_index ++ ;
	}

	return NULL ;
}

x265_frame_t *x265_frame_pop_from_position ( x265_frame_t **list, int32_t i_position )
{
	int i_index = 0 ;
	x265_frame_t *frame = NULL ;

	frame = list[i_position] ;
	i_index = i_position + 1 ;
	while ( list[i_index] )
	{
		list[i_index-1] = list[i_index] ;
		i_index ++ ;
	}
	list[i_index-1] = NULL ;
	return frame ;
}

void x265_frame_delete_list( x265_t *h, x265_frame_t **list )
{
    int i = 0;
    if( !list )
        return;
    while( list[i] )
        x265_frame_delete( h, list[i++] );
    x265_free( list );
}

int32_t x265_frame_get_width ( x265_frame_t *frame )
{
	return frame->i_width[0] ;
}

int32_t x265_frame_get_height ( x265_frame_t *frame )
{
	return frame->i_lines[0];
}

int32_t x265_frame_get_stride ( x265_frame_t *frame )
{
	return frame->i_stride[0] ;
}

int32_t x265_frame_get_c_stride ( x265_frame_t *frame )
{
	return frame->i_stride[1] ;
}

int32_t x265_frame_get_luma_margin ( x265_t *h, x265_frame_t *frame )
{
	return h->cu.pic.i_max_cu_width + 16 ;
}

int32_t x265_frame_get_chroma_margin ( x265_t *h, x265_frame_t *frame )
{
	return ((h->cu.pic.i_max_cu_width + 16) >> 1);
}

pixel* x265_frame_get_buf_y ( x265_frame_t *frame )
{
	return  frame->buffer[0];
}

pixel* x265_frame_get_buf_u ( x265_frame_t *frame )
{
	return frame->buffer[1] ;
}

pixel* x265_frame_get_buf_v ( x265_frame_t *frame )
{
	return frame->buffer[2] ;
}

pixel* x265_frame_get_luma_addr_p2 ( x265_t *h, x265_frame_t *frame )
{
	return frame->plane[0];
}

pixel* x265_frame_get_cb_addr_p2 ( x265_t *h, x265_frame_t *frame )
{
	return frame->plane[1];
}

pixel* x265_frame_get_cr_addr_p2 ( x265_t *h, x265_frame_t *frame )
{
	return frame->plane[2];
}

pixel* x265_frame_get_luma_addr_p3 ( x265_t *h, x265_frame_t *frame, int32_t i_cu_addr )
{
	return frame->plane[0]+ h->cu.pic.cu_offset_y[ i_cu_addr ] ;
}

pixel* x265_frame_get_cb_addr_p3 ( x265_t *h, x265_frame_t *frame, int32_t i_cu_addr )
{
	return frame->plane[1] + h->cu.pic.cu_offset_c[ i_cu_addr ];
}

pixel* x265_frame_get_cr_addr_p3 ( x265_t *h,  x265_frame_t *frame, int32_t i_cu_addr )
{
	return frame->plane[2] + h->cu.pic.cu_offset_c[ i_cu_addr ];
}

pixel* x265_frame_get_luma_addr_p4 ( x265_t *h, x265_frame_t *frame, int32_t i_cu_addr, int32_t ui_abs_zorder_idx )
{
	return frame->plane[0] + h->cu.pic.cu_offset_y[i_cu_addr]
	       + h->cu.pic.bu_offset_y[h->scan.zscan_to_raster[ui_abs_zorder_idx]];
}

pixel* x265_frame_get_cb_addr_p4 ( x265_t *h, x265_frame_t *frame, int32_t i_cu_addr, int32_t ui_abs_zorder_idx )
{
	return frame->plane[1] + h->cu.pic.cu_offset_c[i_cu_addr]
	       + h->cu.pic.bu_offset_c[h->scan.zscan_to_raster[ui_abs_zorder_idx]];
}

pixel* x265_frame_get_cr_addr_p4 ( x265_t *h, x265_frame_t *frame, int32_t i_cu_addr, int32_t ui_abs_zorder_idx )
{
	return frame->plane[2] + h->cu.pic.cu_offset_c[i_cu_addr]
	       + h->cu.pic.bu_offset_c[h->scan.zscan_to_raster[ui_abs_zorder_idx]];
}

void x265_frame_copy_to_frame ( x265_t *h, x265_frame_t *frame, x265_frame_t *frame_dst )
{
	assert( frame->i_width[0]  == frame_dst->i_width[0]  );
	assert( frame->i_lines[0] == frame_dst->i_lines[0] );

	memcpy ( frame_dst->buffer[0], frame->buffer[0],
			sizeof (pixel) * frame->i_stride[0] *
			( frame->i_lines[0] + ((h->cu.pic.i_max_cu_height+16) << 1)) ) ;
	memcpy ( frame_dst->buffer[1], frame->buffer[1],
			sizeof (pixel) * frame->i_stride[1] *
			( frame->i_lines[1] + (h->cu.pic.i_max_cu_height+16)) ) ;
	memcpy ( frame_dst->buffer[2], frame->buffer[2],
			sizeof (pixel) * frame->i_stride[2] *
			( frame->i_lines[2] + (h->cu.pic.i_max_cu_height+16)) ) ;

}

void x265_frame_copy_to_frame_luma ( x265_t *h, x265_frame_t *frame, x265_frame_t *frame_dst )
{
	assert( frame->i_width[0] == frame_dst->i_width[0]  );
	assert( frame->i_lines[0] == frame_dst->i_lines[0] );

	memcpy ( frame_dst->buffer[0], frame->buffer[0],
			sizeof (pixel) * frame->i_stride[0] *
			( frame->i_lines[0] + ((h->cu.pic.i_max_cu_height+16) << 1)) ) ;
}

void x265_frame_copy_to_frame_cb ( x265_t *h, x265_frame_t *frame, x265_frame_t *frame_dst )
{
	assert( frame->i_width[0]  == frame_dst->i_width[0]  );
	assert( frame->i_lines[0] == frame_dst->i_lines[0] );

	memcpy ( frame_dst->buffer[1], frame->buffer[1],
			sizeof (pixel) * frame->i_stride[1] *
			( frame->i_lines[1] + (h->cu.pic.i_max_cu_height+16)) ) ;
}

void x265_frame_copy_to_frame_cr ( x265_t *h, x265_frame_t *frame, x265_frame_t *frame_dst )
{
	assert( frame->i_width[0]  == frame_dst->i_width[0]  );
	assert( frame->i_lines[0] == frame_dst->i_lines[0] );

	memcpy ( frame_dst->buffer[2], frame->buffer[2],
			sizeof (pixel) * frame->i_stride[2] *
			( frame->i_lines[2] + (h->cu.pic.i_max_cu_height+16)) ) ;
}

void x265_frame_dump ( x265_t* h, x265_frame_t *frame, const char *file_name, int32_t b_add )
{
	int32_t shift = h->cu.pic.i_bit_depth_y-8;
	int32_t offset = (shift>0)?(1<<(shift-1)):0;
	int32_t x = 0, y = 0;
	uint8_t uc = 0 ;
	pixel *src = NULL ;
	FILE* file = NULL ;

	if (!b_add)
	{
		file = fopen (file_name, "wb");
	}
	else
	{
		file = fopen (file_name, "ab");
	}

	shift = h->cu.pic.i_bit_depth_y-8;
	offset = (shift>0)?(1<<(shift-1)):0;
	src = x265_frame_get_luma_addr_p2 (h, frame ) ;

	for ( y = 0; y < frame->i_lines[0]; y++ )
	{
		for ( x = 0; x < frame->i_width[0]; x++ )
		{
			uc = (uint8_t)x265_clip3_pixel ( (src[x]+offset)>>shift, 0, 255);

			fwrite( &uc, sizeof(uint8_t), 1, file );
		}
		src += frame->i_stride[0];
	}

	shift = h->cu.pic.i_bit_depth_c-8;
	offset = (shift>0)?(1<<(shift-1)):0;
	src = x265_frame_get_cb_addr_p2 (h, frame ) ;

	for ( y = 0; y < frame->i_lines[1]; y++ )
	{
		for ( x = 0; x < frame->i_width[1]; x++ )
		{
			uc = (uint8_t)x265_clip3_pixel( (src[x]+offset)>>shift, 0, 255 );
			fwrite( &uc, sizeof(uint8_t), 1, file );
		}
		src += frame->i_stride[1];
	}

	src = x265_frame_get_cr_addr_p2 (h, frame ) ;
	for ( y = 0; y < frame->i_lines[2]; y++ )
	{
		for ( x = 0; x < frame->i_width[2]; x++ )
		{
			uc = (uint8_t)x265_clip3_pixel( (src[x]+offset)>>shift, 0, 255 );
			fwrite( &uc, sizeof(uint8_t), 1, file );
		}
		src += frame->i_stride[2];
	}

	fclose(file);
}
