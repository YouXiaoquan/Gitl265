

#include "common.h"



void x265_simage_delete ( x265_simage_t *simage )
{
	x265_simage_deinit ( simage ) ;
	x265_free ( simage ) ;
}

x265_simage_t *x265_simage_new ()
{
	x265_simage_t *simage = NULL ;

	CHECKED_MALLOCZERO( simage, sizeof(x265_simage_t) );
	if ( x265_simage_init (simage) )
	{
		goto fail ;
	}

	return simage ;
fail:
	x265_simage_delete ( (void*) simage ) ;
	return NULL ;
}

int x265_simage_init ( x265_simage_t *simage )
{

	return 0 ;
}

void x265_simage_deinit ( x265_simage_t *simage )
{

}

int x265_simage_create ( x265_simage_t *simage, uint32_t i_width, uint32_t i_height )
{
	uint32_t i_total_size = 0 ;

	simage->i_width = i_width ;
	simage->i_height = i_height ;

	simage->i_stride[0] = i_width ;
	simage->i_stride[1] = simage->i_stride[0] / 2 ;
	simage->i_stride[2] = simage->i_stride[1] ;

	i_total_size = simage->i_stride[0] * i_height * 3 / 2 ;
	CHECKED_MALLOCZERO ( simage->buffer, i_total_size * sizeof (spixel) ) ;

	simage->plane[0] = simage->buffer ;
	simage->plane[1] = simage->buffer + simage->i_stride[0] * i_height ;
	simage->plane[2] = simage->plane[1] + simage->i_stride[1] * (i_height >> 1) ;

	return 0 ;

fail:
	x265_simage_destroy ( simage ) ;
	return -1 ;
}

void x265_simage_destroy ( x265_simage_t *simage )
{
	int32_t loop = 0 ;

	x265_free ( simage->buffer ) ;

	simage->buffer = NULL ;
	simage->i_width = 0 ;
	simage->i_height = 0 ;

	for ( loop = 0 ; loop < 3 ; ++ loop )
	{
		simage->i_stride[loop] = 0 ;
		simage->plane[loop] = NULL ;
	}
}

void x265_simage_clear ( x265_simage_t *simage )
{
	memset ( simage->plane[0], 0, simage->i_stride[0] * simage->i_height * sizeof(spixel) ) ;
	memset ( simage->plane[1], 0, simage->i_stride[1] * (simage->i_height>>1) * sizeof(spixel) ) ;
	memset ( simage->plane[2], 0, simage->i_stride[2] * (simage->i_height>>1) * sizeof(spixel) ) ;

}

int x265_simage_create_intra_simage ( x265_simage_t *simage, uint32_t i_width, uint32_t i_height )
{
	uint32_t i_total_size = 0 ;

	simage->i_width = i_width ;
	simage->i_height = i_height ;

	simage->i_stride[0] = i_width + 2 ;
	simage->i_stride[1] = simage->i_stride[0] / 2 ;
	simage->i_stride[2] = simage->i_stride[1] ;

	i_total_size = simage->i_stride[0] * ( i_height + 2 ) * 3 / 2 ;
	CHECKED_MALLOCZERO ( simage->buffer, i_total_size * sizeof (spixel) ) ;

	simage->plane[0] = simage->buffer + simage->i_stride[0] * 2 + 2 ;
	simage->plane[1] = simage->buffer + simage->i_stride[0] * ( i_height + 2 )
						+ simage->i_stride[1] + 1 ;
	simage->plane[2] = simage->plane[1] + simage->i_stride[1] * ((i_height+2) >> 1) ;

	return 0 ;

fail:
	x265_simage_destroy ( simage ) ;
	return -1 ;
}

int32_t x265_simage_get_addr_offset_p4 ( x265_t *h, x265_simage_t *simage,
										uint32_t i_part_unit_idx, uint32_t i_width )
{
	int32_t i_blk_x = 0 ;
	int32_t i_blk_y = 0 ;

	i_blk_x = h->scan.raster_to_pel_x[h->scan.zscan_to_raster[i_part_unit_idx]];
	i_blk_y = h->scan.raster_to_pel_y[h->scan.zscan_to_raster[i_part_unit_idx]];

	return i_blk_x + i_blk_y * i_width;
}

/*
 	int32_t i_blk_x = ( i_trans_unit_idx * i_blk_size ) &  ( i_width - 1 )
			 = ( (i_part_idx_x+i_part_idx_y*(i_width/i_blk_size)) * i_blk_size ) & ( i_width - 1 )
           = ( i_part_idx_x * i_blk_size + i_part_idx_y * i_width ) & ( i_width - 1 )
			 = i_part_idx_x * i_blk_size
	int32_t i_blk_y = ( i_trans_unit_idx * i_blk_size ) &~ ( i_width - 1 )
			 = ( (i_part_idx_x+i_part_idx_y*(i_width/i_blk_size)) * i_blk_size ) &~ ( i_width - 1 )
       	 = ( i_part_idx_x * i_blk_size + i_part_idx_y * i_width ) &~ ( i_width - 1 )
			 = i_part_idx_y * i_width

 */
int32_t x265_simage_get_addr_offset_p5 ( x265_t *h, x265_simage_t *simage,
										uint32_t i_trans_unit_idx, uint32_t i_blk_size, uint32_t i_width )
{
	int32_t i_blk_x = 0 ;
	int32_t i_blk_y = 0 ;

	i_blk_x = ( i_trans_unit_idx * i_blk_size ) &  ( i_width - 1 );
	i_blk_y = ( i_trans_unit_idx * i_blk_size ) &~ ( i_width - 1 );

	return i_blk_x + i_blk_y * i_blk_size ;
}


spixel* x265_simage_get_luma_addr_p2 ( x265_t *h, x265_simage_t* simage )
{
	return simage->plane[0];
}

spixel* x265_simage_get_cb_addr_p2 ( x265_t *h, x265_simage_t* simage )
{
	return simage->plane[1];
}

spixel* x265_simage_get_cr_addr_p2 ( x265_t *h, x265_simage_t* simage )
{
	return simage->plane[2];
}

//  access starting position of simage partition unit buffer
spixel* x265_simage_get_luma_addr_p3 ( x265_t *h, x265_simage_t* simage,
									uint32_t i_part_unit_idx )
{
	return simage->plane[0]
	       + x265_simage_get_addr_offset_p4( h, simage,
											i_part_unit_idx, simage->i_stride[0]  ) ;
}

spixel* x265_simage_get_cb_addr_p3 ( x265_t *h, x265_simage_t* simage,
									uint32_t i_part_unit_idx )
{
	return simage->plane[1]
	       + ( x265_simage_get_addr_offset_p4( h, simage,
	    		   	   	   	   	   	   	   	i_part_unit_idx, simage->i_stride[1] ) >> 1 ) ;
}

spixel* x265_simage_get_cr_addr_p3 ( x265_t *h, x265_simage_t* simage,
									uint32_t i_part_unit_idx )
{
	return simage->plane[2]
	       + ( x265_simage_get_addr_offset_p4( h, simage,
	    		   	   	   	   	   	   	   	i_part_unit_idx, simage->i_stride[2] ) >> 1 ) ;
}


//  access starting position of simage transform unit buffer
spixel* x265_simage_get_luma_addr_p4 ( x265_t *h, x265_simage_t* simage,
									uint32_t i_trans_unit_idx, uint32_t i_blk_size )
{
	return simage->plane[0]
	       + x265_simage_get_addr_offset_p5( h, simage,
	    		   	   	   	   	   	   	   i_trans_unit_idx, i_blk_size, simage->i_stride[0] );
}

spixel* x265_simage_get_cb_addr_p4 ( x265_t *h, x265_simage_t* simage,
								uint32_t i_trans_unit_idx, uint32_t i_blk_size )
{
	return simage->plane[1]
	       + x265_simage_get_addr_offset_p5( h, simage,
	    		   	   	   	   	   	   	   i_trans_unit_idx, i_blk_size, simage->i_stride[1] );
}

spixel* x265_simage_get_cr_addr_p4 ( x265_t *h, x265_simage_t* simage,
								uint32_t i_trans_unit_idx, uint32_t i_blk_size )
{
	return simage->plane[2]
	       + x265_simage_get_addr_offset_p5( h, simage,
	    		   	   	   	   	   	   	   i_trans_unit_idx, i_blk_size, simage->i_stride[2] );
}

//  get stride value of simage buffer
uint32_t x265_simage_get_stride ( x265_simage_t* simage )
{
	return simage->i_stride[0] ;
}

uint32_t x265_simage_get_c_stride ( x265_simage_t* simage )
{
	return simage->i_stride[1] ;
}

uint32_t x265_simage_get_width ( x265_simage_t* simage )
{
	return  simage->i_width ;
}

uint32_t x265_simage_get_height ( x265_simage_t* simage )
{
	return  simage->i_height ;
}

uint32_t x265_simage_get_c_width ( x265_simage_t* simage )
{
	return simage->i_width / 2 ;
}

uint32_t x265_simage_get_c_height ( x265_simage_t* simage )
{
	return  simage->i_height / 2 ;
}

void x265_simage_copy_to_frame ( x265_t *h, x265_simage_t *simage, x265_frame_t* frame_dst, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx )
{
	x265_simage_copy_to_frame_luma  ( h, simage, frame_dst, i_cu_addr, i_abs_zorder_idx, i_part_depth, i_part_idx );
	x265_simage_copy_to_frame_chroma( h, simage, frame_dst, i_cu_addr, i_abs_zorder_idx, i_part_depth, i_part_idx );
}

void x265_simage_copy_to_frame_luma  ( x265_t *h, x265_simage_t *simage, x265_frame_t* frame_dst, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx )
{
	int32_t  y, i_width, i_height;
	i_width  = simage->i_width >>i_part_depth;
	i_height = simage->i_height>>i_part_depth;

	spixel* p_src = x265_simage_get_luma_addr_p4 ( h, simage, i_part_idx, i_width);
	pixel* p_dst = x265_frame_get_luma_addr_p4 ( h, frame_dst, i_cu_addr, i_abs_zorder_idx );

	uint32_t  i_src_stride  = simage->i_stride[0];
	uint32_t  i_dst_stride  = frame_dst->i_stride[0];

	for ( y = i_height; y != 0; y-- )
	{
		memcpy( p_dst, p_src, sizeof(spixel)*i_width);
		p_dst += i_dst_stride;
		p_src += i_src_stride;
	}
}

void x265_simage_copy_to_frame_chroma( x265_t *h, x265_simage_t *simage, x265_frame_t* frame_dst, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx )
{
	int32_t  y, i_width, i_height;
	i_width  = (simage->i_width/2) >> i_part_depth;
	i_height = (simage->i_height/2) >> i_part_depth;

	spixel* p_src_u      = x265_simage_get_cb_addr_p4( h, simage, i_part_idx, i_width);
	spixel* p_src_v      = x265_simage_get_cr_addr_p4( h, simage, i_part_idx, i_width);
	pixel* p_dst_u      = x265_frame_get_cb_addr_p4 ( h, frame_dst, i_cu_addr, i_abs_zorder_idx );
	pixel* p_dst_v      = x265_frame_get_cr_addr_p4 ( h, frame_dst, i_cu_addr, i_abs_zorder_idx );

	uint32_t  i_src_stride = simage->i_stride[1];
	uint32_t  i_dst_stride = frame_dst->i_stride[1];
	for ( y = i_height; y != 0; y-- )
	{
		memcpy( p_dst_u, p_src_u, sizeof(spixel)*(i_width) );
		memcpy( p_dst_v, p_src_v, sizeof(spixel)*(i_width) );
		p_src_u += i_src_stride;
		p_src_v += i_src_stride;
		p_dst_u += i_dst_stride;
		p_dst_v += i_dst_stride;
	}
}

void x265_simage_copy_from_frame   ( x265_t *h, x265_simage_t *simage, x265_frame_t* frame_src, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx )
{
	x265_simage_copy_from_frame_luma  ( h, simage, frame_src, i_cu_addr, i_abs_zorder_idx );
	x265_simage_copy_from_frame_chroma( h, simage, frame_src, i_cu_addr, i_abs_zorder_idx );
}

void x265_simage_copy_from_frame_luma ( x265_t *h, x265_simage_t *simage, x265_frame_t* frame_src, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx )
{
	int32_t  y;

	spixel* p_dst     = simage->plane[0];
	pixel* p_src     = x265_frame_get_luma_addr_p4 ( h, frame_src, i_cu_addr, i_abs_zorder_idx );

	uint32_t  i_dst_stride  = simage->i_stride[0];
	uint32_t  i_src_stride  = frame_src->i_stride[0];
	for ( y = simage->i_height; y != 0; y-- )
	{
		memcpy( p_dst, p_src, sizeof(spixel)*simage->i_width);
		p_dst += i_dst_stride;
		p_src += i_src_stride;
	}
}

void x265_simage_copy_from_frame_chroma( x265_t *h, x265_simage_t *simage, x265_frame_t* frame_src, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx )
{
	int32_t  y;

	spixel* p_dst_u      = simage->plane[1];
	spixel* p_dst_v      = simage->plane[2];
	pixel* p_src_u      = x265_frame_get_cb_addr_p4 ( h, frame_src, i_cu_addr, i_abs_zorder_idx );
	pixel* p_src_v      = x265_frame_get_cr_addr_p4 ( h, frame_src, i_cu_addr, i_abs_zorder_idx );

	uint32_t  i_dst_stride = simage->i_stride[1];
	uint32_t  i_src_stride = frame_src->i_stride[1];
	for ( y = simage->i_height/2; y != 0; y-- )
	{
		memcpy( p_dst_u, p_src_u, sizeof(spixel)*(simage->i_width/2) );
		memcpy( p_dst_v, p_src_v, sizeof(spixel)*(simage->i_width/2) );
		p_src_u += i_src_stride;
		p_src_v += i_src_stride;
		p_dst_u += i_dst_stride;
		p_dst_v += i_dst_stride;
	}
}

void x265_simage_copy_to_part_simage( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_dst, uint32_t i_dst_part_idx )
{
	x265_simage_copy_to_part_luma  ( h, simage, simage_dst, i_dst_part_idx );
	x265_simage_copy_to_part_chroma( h, simage, simage_dst, i_dst_part_idx );
}

void x265_simage_copy_to_part_luma( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_dst, uint32_t i_dst_part_idx )
{
	int32_t  y;

	spixel* p_src     = simage->plane[0];
	spixel* p_dst     = x265_simage_get_luma_addr_p3 ( h, simage_dst, i_dst_part_idx );

	uint32_t  i_src_stride  = simage->i_stride[0];
	uint32_t  i_dst_stride  = simage_dst->i_stride[0];
	for ( y = simage->i_height; y != 0; y-- )
	{
		memcpy( p_dst, p_src, sizeof(spixel)*simage->i_width);
		p_dst += i_dst_stride;
		p_src += i_src_stride;
	}
}

void x265_simage_copy_to_part_chroma( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_dst, uint32_t i_dst_part_idx )
{
	int32_t  y;

	spixel* p_src_u      = simage->plane[1];
	spixel* p_src_v      = simage->plane[2];
	spixel* p_dst_u      = x265_simage_get_cb_addr_p3 ( h, simage_dst, i_dst_part_idx );
	spixel* p_dst_v      = x265_simage_get_cr_addr_p3 ( h, simage_dst, i_dst_part_idx );

	uint32_t  i_src_stride = simage->i_stride[1];
	uint32_t  i_dst_stride = simage_dst->i_stride[1];
	for ( y = simage->i_height/2; y != 0; y-- )
	{
		memcpy( p_dst_u, p_src_u, sizeof(spixel)*(simage->i_width/2) );
		memcpy( p_dst_v, p_src_v, sizeof(spixel)*(simage->i_width/2) );
		p_src_u += i_src_stride;
		p_src_v += i_src_stride;
		p_dst_u += i_dst_stride;
		p_dst_v += i_dst_stride;
	}
}

void x265_simage_copy_part_to_simage( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_dst, uint32_t i_src_part_idx )
{
	x265_simage_copy_part_to_luma  ( h, simage, simage_dst, i_src_part_idx );
	x265_simage_copy_part_to_chroma( h, simage, simage_dst, i_src_part_idx );
}

void x265_simage_copy_part_to_luma( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_dst, uint32_t i_src_part_idx )
{
	int32_t  y;

	spixel* p_src     = x265_simage_get_luma_addr_p3 ( h, simage, i_src_part_idx);
	spixel* p_dst     = x265_simage_get_luma_addr_p3 ( h, simage_dst, 0 );

	uint32_t  i_src_stride  = simage->i_stride[0];
	uint32_t  i_dst_stride  = simage_dst->i_stride[0];

	uint32_t i_height = simage_dst->i_height;
	uint32_t i_width = simage_dst->i_width;

	for ( y = i_height; y != 0; y-- )
	{
		memcpy( p_dst, p_src, sizeof(spixel)*i_width);
		p_dst += i_dst_stride;
		p_src += i_src_stride;
	}
}

void x265_simage_copy_part_to_chroma( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_dst, uint32_t i_src_part_idx )
{
	int32_t  y;

	spixel* p_src_u      = x265_simage_get_cb_addr_p3 ( h, simage,  i_src_part_idx );
	spixel* p_src_v      = x265_simage_get_cr_addr_p3 ( h, simage,  i_src_part_idx );
	spixel* p_dst_u      = x265_simage_get_cb_addr_p3 ( h, simage_dst,  0 );
	spixel* p_dst_v      = x265_simage_get_cr_addr_p3 ( h, simage_dst,  0 );

	uint32_t  i_src_stride = simage->i_stride[1];
	uint32_t  i_dst_stride = simage_dst->i_stride[1];

	uint32_t i_c_height = simage_dst->i_height/2;
	uint32_t i_c_width = simage_dst->i_width/2;

	for ( y = i_c_height; y != 0; y-- )
	{
		memcpy( p_dst_u, p_src_u, sizeof(spixel)*(i_c_width) );
		memcpy( p_dst_v, p_src_v, sizeof(spixel)*(i_c_width) );
		p_src_u += i_src_stride;
		p_src_v += i_src_stride;
		p_dst_u += i_dst_stride;
		p_dst_v += i_dst_stride;
	}
}

void x265_simage_copy_part_to_part_simage_p ( x265_t *h, x265_simage_t *simage, x265_image_t* image_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height )
{
	x265_simage_copy_part_to_part_luma_p   ( h, simage, image_dst, i_part_idx, i_width, i_height );
	x265_simage_copy_part_to_part_chroma_p ( h, simage, image_dst, i_part_idx, i_width>>1, i_height>>1 );
}

void x265_simage_copy_part_to_part_luma_p ( x265_t *h, x265_simage_t *simage, x265_image_t* image_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height )
{
	spixel* p_src =           x265_simage_get_luma_addr_p3 ( h, simage, i_part_idx);
	pixel* p_dst =           x265_image_get_luma_addr_p3(h, image_dst,i_part_idx);

	uint32_t  i_src_stride = simage->i_stride[0];
	uint32_t  i_dst_stride = image_dst->i_stride[0];
	for ( uint32_t y = i_height; y != 0; y-- )
	{
		for ( uint32_t x = 0 ; x < i_width ; ++ x )
		{
			p_dst[x] = p_src[x];
		}
		p_src += i_src_stride;
		p_dst += i_dst_stride;
	}
}

void x265_simage_copy_part_to_part_chroma_p( x265_t *h, x265_simage_t *simage, x265_image_t* image_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height )
{
	spixel*  p_src_u =           x265_simage_get_cb_addr_p3 ( h, simage, i_part_idx);
	spixel*  p_src_v =           x265_simage_get_cr_addr_p3 ( h, simage, i_part_idx);
	pixel*  p_dst_u = x265_image_get_cb_addr_p3 ( h, image_dst, i_part_idx);
	pixel*  p_dst_v = x265_image_get_cr_addr_p3 ( h, image_dst, i_part_idx);

	uint32_t   i_src_stride = simage->i_stride[1];
	uint32_t   i_dst_stride = image_dst->i_stride[1];
	for ( uint32_t y = i_height; y != 0; y-- )
	{
		for ( uint32_t x = 0 ; x < i_width ; ++ x )
		{
			p_dst_u[x] = p_src_u[x];
			p_dst_v[x] = p_src_v[x];
		}
		p_src_u += i_src_stride;
		p_src_v += i_src_stride;
		p_dst_u += i_dst_stride;
		p_dst_v += i_dst_stride;
	}
}

void x265_simage_copy_part_to_part_simage_s ( x265_t *h, x265_simage_t *simage, x265_simage_t* image_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height )
{
	x265_simage_copy_part_to_part_luma_s   ( h, simage, image_dst, i_part_idx, i_width, i_height );
	x265_simage_copy_part_to_part_chroma_s ( h, simage, image_dst, i_part_idx, i_width>>1, i_height>>1 );
}

void x265_simage_copy_part_to_part_luma_s ( x265_t *h, x265_simage_t *simage, x265_simage_t* image_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height )
{
	spixel* p_src =           x265_simage_get_luma_addr_p3 ( h, simage, i_part_idx);
	spixel* p_dst =           x265_simage_get_luma_addr_p3(h, image_dst,i_part_idx);

	uint32_t  i_src_stride = simage->i_stride[0];
	uint32_t  i_dst_stride = image_dst->i_stride[0];
	for ( uint32_t y = i_height; y != 0; y-- )
	{
		memcpy( p_dst, p_src, i_width * sizeof(spixel) );
		p_src += i_src_stride;
		p_dst += i_dst_stride;
	}
}

void x265_simage_copy_part_to_part_chroma_s( x265_t *h, x265_simage_t *simage, x265_simage_t* image_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height )
{
	spixel*  p_src_u =           x265_simage_get_cb_addr_p3 ( h, simage, i_part_idx);
	spixel*  p_src_v =           x265_simage_get_cr_addr_p3 ( h, simage, i_part_idx);
	spixel*  p_dst_u = x265_simage_get_cb_addr_p3 ( h, image_dst, i_part_idx);
	spixel*  p_dst_v = x265_simage_get_cr_addr_p3 ( h, image_dst, i_part_idx);

	uint32_t   i_src_stride = simage->i_stride[1];
	uint32_t   i_dst_stride = image_dst->i_stride[1];
	for ( uint32_t y = i_height; y != 0; y-- )
	{
		memcpy( p_dst_u, p_src_u, i_width * sizeof(spixel) );
		memcpy( p_dst_v, p_src_v, i_width * sizeof(spixel) );
		p_src_u += i_src_stride;
		p_src_v += i_src_stride;
		p_dst_u += i_dst_stride;
		p_dst_v += i_dst_stride;
	}
}

void x265_simage_copy_part_to_part_chroma_p7( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height, uint32_t chroma_id)
{
	if(chroma_id == 0)
	{
		spixel*  p_src_u =           x265_simage_get_cb_addr_p3 ( h, simage, i_part_idx);
		spixel*  p_dst_u = x265_simage_get_cb_addr_p3 ( h, simage_dst, i_part_idx);
		if( p_src_u == p_dst_u)
		{
			return ;
		}
		uint32_t   i_src_stride = simage->i_stride[1];
		uint32_t   i_dst_stride = simage_dst->i_stride[1];
		for ( uint32_t y = i_height; y != 0; y-- )
		{
			memcpy( p_dst_u, p_src_u, i_width * sizeof(spixel) );
			p_src_u += i_src_stride;
			p_dst_u += i_dst_stride;
		}
	}
	else if (chroma_id == 1)
	{
		spixel*  p_src_v =           x265_simage_get_cr_addr_p3 ( h, simage, i_part_idx);
		spixel*  p_dst_v = x265_simage_get_cr_addr_p3 ( h, simage_dst, i_part_idx);
		if( p_src_v == p_dst_v)
		{
			return;
		}
		uint32_t   i_src_stride = simage->i_stride[1];
		uint32_t   i_dst_stride = simage_dst->i_stride[1];
		for ( uint32_t y = i_height; y != 0; y-- )
		{
			memcpy( p_dst_v, p_src_v, i_width * sizeof(spixel) );
			p_src_v += i_src_stride;
			p_dst_v += i_dst_stride;
		}
	}
	else
	{
		spixel*  p_src_u =           x265_simage_get_cb_addr_p3 ( h, simage, i_part_idx);
		spixel*  p_src_v =           x265_simage_get_cr_addr_p3 ( h, simage, i_part_idx);
		spixel*  p_dst_u = x265_simage_get_cb_addr_p3 ( h, simage_dst, i_part_idx);
		spixel*  p_dst_v = x265_simage_get_cr_addr_p3 ( h, simage_dst, i_part_idx);

		if( p_src_u == p_dst_u && p_src_v == p_dst_v)
		{
			//th not a good idea
			//th best would be to fix the caller
			return ;
		}
		uint32_t   i_src_stride = simage->i_stride[1];
		uint32_t   i_dst_stride = simage_dst->i_stride[1];
		for ( uint32_t y = i_height; y != 0; y-- )
		{
			memcpy( p_dst_u, p_src_u, i_width * sizeof(spixel) );
			memcpy( p_dst_v, p_src_v, i_width * sizeof(spixel) );
			p_src_u += i_src_stride;
			p_src_v += i_src_stride;
			p_dst_u += i_dst_stride;
			p_dst_v += i_dst_stride;
		}
	}
}

void x265_simage_add_clip( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_src0, x265_simage_t* simage_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size )
{
	x265_simage_add_clip_luma   ( h, simage, simage_src0, simage_src1, i_tr_unit_idx, i_part_size     );
	x265_simage_add_clip_chroma ( h, simage, simage_src0, simage_src1, i_tr_unit_idx, i_part_size>>1  );
}

void x265_simage_add_clip_luma( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_src0, x265_simage_t* simage_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size )
{
	int32_t x, y;

	spixel* p_src0 = x265_simage_get_luma_addr_p4 ( h, simage_src0, i_tr_unit_idx, i_part_size );
	spixel* p_src1 = x265_simage_get_luma_addr_p4 ( h, simage_src1, i_tr_unit_idx, i_part_size );
	spixel* p_dst  = x265_simage_get_luma_addr_p4 ( h, simage, i_tr_unit_idx, i_part_size );

	uint32_t i_src0stride = simage_src0->i_stride[0];
	uint32_t i_src1stride = simage_src1->i_stride[0];
	uint32_t i_dst_stride = simage->i_stride[0];
	for ( y = i_part_size-1; y >= 0; y-- )
	{
		for ( x = i_part_size-1; x >= 0; x-- )
		{
			p_dst[x] = x265_clip_y( h, p_src0[x] + p_src1[x] );
		}
		p_src0 += i_src0stride;
		p_src1 += i_src1stride;
		p_dst  += i_dst_stride;
	}
}

void x265_simage_add_clip_chroma( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_src0, x265_simage_t* simage_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size )
{
	int32_t x, y;

	spixel* p_src_u0 = x265_simage_get_cb_addr_p4 ( h, simage_src0, i_tr_unit_idx, i_part_size );
	spixel* p_src_u1 = x265_simage_get_cb_addr_p4 ( h, simage_src1, i_tr_unit_idx, i_part_size );
	spixel* p_src_v0 = x265_simage_get_cr_addr_p4 ( h, simage_src0, i_tr_unit_idx, i_part_size );
	spixel* p_src_v1 = x265_simage_get_cr_addr_p4 ( h, simage_src1, i_tr_unit_idx, i_part_size );
	spixel* p_dst_u = x265_simage_get_cb_addr_p4 ( h, simage, i_tr_unit_idx, i_part_size );
	spixel* p_dst_v = x265_simage_get_cr_addr_p4 ( h, simage, i_tr_unit_idx, i_part_size );

	uint32_t  i_src0stride = simage_src0->i_stride[1];
	uint32_t  i_src1stride = simage_src1->i_stride[1];
	uint32_t  i_dst_stride = simage->i_stride[1];
	for ( y = i_part_size-1; y >= 0; y-- )
	{
		for ( x = i_part_size-1; x >= 0; x-- )
		{
			p_dst_u[x] = x265_clip_c( h, p_src_u0[x] + p_src_u1[x] );
			p_dst_v[x] = x265_clip_c( h, p_src_v0[x] + p_src_v1[x] );
		}

		p_src_u0 += i_src0stride;
		p_src_u1 += i_src1stride;
		p_src_v0 += i_src0stride;
		p_src_v1 += i_src1stride;
		p_dst_u  += i_dst_stride;
		p_dst_v  += i_dst_stride;
	}
}

void x265_simage_subtract( x265_t *h, x265_simage_t *simage, x265_image_t* simage_src0, x265_image_t* simage_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size )
{
	x265_simage_subtract_luma  ( h, simage, simage_src0, simage_src1,  i_tr_unit_idx, i_part_size    );
	x265_simage_subtract_chroma( h, simage, simage_src0, simage_src1,  i_tr_unit_idx, i_part_size>>1 );
}

void x265_simage_subtract_luma( x265_t *h, x265_simage_t *simage, x265_image_t* simage_src0, x265_image_t* simage_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size )
{
	int32_t x, y;

	pixel* p_src0 = x265_image_get_luma_addr_p4 ( h, simage_src0, i_tr_unit_idx, i_part_size );
	pixel* p_src1 = x265_image_get_luma_addr_p4 ( h, simage_src1, i_tr_unit_idx, i_part_size );
	spixel* p_dst  = x265_simage_get_luma_addr_p4 ( h, simage, i_tr_unit_idx, i_part_size );

	uint32_t i_src0stride = simage_src0->i_stride[0];
	uint32_t i_src1stride = simage_src1->i_stride[0];
	uint32_t i_dst_stride = simage->i_stride[0];
	for ( y = i_part_size-1; y >= 0; y-- )
	{
		for ( x = i_part_size-1; x >= 0; x-- )
		{
			p_dst[x] = p_src0[x] - p_src1[x];
		}
		p_src0 += i_src0stride;
		p_src1 += i_src1stride;
		p_dst  += i_dst_stride;
	}
}

void x265_simage_subtract_chroma( x265_t *h, x265_simage_t *simage, x265_image_t* simage_src0, x265_image_t* simage_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size )
{
	int32_t x, y;

	pixel* p_src_u0 = x265_image_get_cb_addr_p4 ( h, simage_src0, i_tr_unit_idx, i_part_size );
	pixel* p_src_u1 = x265_image_get_cb_addr_p4 ( h, simage_src1, i_tr_unit_idx, i_part_size );
	pixel* p_src_v0 = x265_image_get_cr_addr_p4 ( h, simage_src0, i_tr_unit_idx, i_part_size );
	pixel* p_src_v1 = x265_image_get_cr_addr_p4 ( h, simage_src1, i_tr_unit_idx, i_part_size );
	spixel* p_dst_u = x265_simage_get_cb_addr_p4 ( h, simage, i_tr_unit_idx, i_part_size );
	spixel* p_dst_v = x265_simage_get_cr_addr_p4 ( h, simage, i_tr_unit_idx, i_part_size );

	uint32_t  i_src0stride = simage_src0->i_stride[1];
	uint32_t  i_src1stride = simage_src1->i_stride[1];
	uint32_t  i_dst_stride = simage->i_stride[1];
	for ( y = i_part_size-1; y >= 0; y-- )
	{
		for ( x = i_part_size-1; x >= 0; x-- )
		{
			p_dst_u[x] = p_src_u0[x] - p_src_u1[x];
			p_dst_v[x] = p_src_v0[x] - p_src_v1[x];
		}
		p_src_u0 += i_src0stride;
		p_src_u1 += i_src1stride;
		p_src_v0 += i_src0stride;
		p_src_v1 += i_src1stride;
		p_dst_u  += i_dst_stride;
		p_dst_v  += i_dst_stride;
	}
}

void x265_simage_add_avg( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_src0, x265_simage_t* simage_src1, uint32_t i_part_unit_idx, uint32_t i_width, uint32_t i_height )
{
	int32_t x, y;

	spixel* p_src_y0  = x265_simage_get_luma_addr_p3 ( h, simage_src0, i_part_unit_idx );
	spixel* p_src_u0  = x265_simage_get_cb_addr_p3 ( h, simage_src0, i_part_unit_idx );
	spixel* p_src_v0  = x265_simage_get_cr_addr_p3 ( h, simage_src0, i_part_unit_idx );

	spixel* p_src_y1  = x265_simage_get_luma_addr_p3 ( h, simage_src1,  i_part_unit_idx );
	spixel* p_src_u1  = x265_simage_get_cb_addr_p3 ( h, simage_src1, i_part_unit_idx );
	spixel* p_src_v1  = x265_simage_get_cr_addr_p3 ( h, simage_src1, i_part_unit_idx );

	spixel* p_dst_y   = x265_simage_get_luma_addr_p3 ( h, simage, i_part_unit_idx );
	spixel* p_dst_u   = x265_simage_get_cb_addr_p3 ( h, simage, i_part_unit_idx );
	spixel* p_dst_v   = x265_simage_get_cr_addr_p3 ( h, simage, i_part_unit_idx );

	uint32_t  i_src0stride = simage_src0->i_stride[0];
	uint32_t  i_src1stride = simage_src1->i_stride[0];
	uint32_t  i_dst_stride = simage->i_stride[0];
	int32_t shift_num = X265_IF_INTERNAL_PREC + 1 - h->param.sps.i_bit_depth_y ;
	int32_t offset = ( 1 << ( shift_num - 1 ) ) + 2 * X265_IF_INTERNAL_OFFS;

	for ( y = 0; y < i_height; y++ )
	{
		for ( x = 0; x < i_width; x += 4 )
		{
			p_dst_y[ x + 0 ] = x265_clip_y ( h, ( p_src_y0[ x + 0 ] + p_src_y1[ x + 0 ] + offset ) >> shift_num );
			p_dst_y[ x + 1 ] = x265_clip_y ( h, ( p_src_y0[ x + 1 ] + p_src_y1[ x + 1 ] + offset ) >> shift_num );
			p_dst_y[ x + 2 ] = x265_clip_y ( h, ( p_src_y0[ x + 2 ] + p_src_y1[ x + 2 ] + offset ) >> shift_num );
			p_dst_y[ x + 3 ] = x265_clip_y ( h, ( p_src_y0[ x + 3 ] + p_src_y1[ x + 3 ] + offset ) >> shift_num );
		}
		p_src_y0 += i_src0stride;
		p_src_y1 += i_src1stride;
		p_dst_y  += i_dst_stride;
	}

	shift_num = X265_IF_INTERNAL_PREC + 1 - h->param.sps.i_bit_depth_c ;
	offset = ( 1 << ( shift_num - 1 ) ) + 2 * X265_IF_INTERNAL_OFFS ;

	i_src0stride = simage_src0->i_stride[1];
	i_src1stride = simage_src1->i_stride[1];
	i_dst_stride  = simage->i_stride[1];

	i_width  >>=1;
	i_height >>=1;

	for ( y = i_height-1; y >= 0; y-- )
	{
		for ( x = i_width-1; x >= 0; )
		{
			// note: chroma min width is 2
			p_dst_u[x] = x265_clip_c ( h, (p_src_u0[x] + p_src_u1[x] + offset) >> shift_num);
			p_dst_v[x] = x265_clip_c ( h, (p_src_v0[x] + p_src_v1[x] + offset) >> shift_num); x--;
			p_dst_u[x] = x265_clip_c ( h, (p_src_u0[x] + p_src_u1[x] + offset) >> shift_num);
			p_dst_v[x] = x265_clip_c ( h, (p_src_v0[x] + p_src_v1[x] + offset) >> shift_num); x--;
		}

		p_src_u0 += i_src0stride;
		p_src_u1 += i_src1stride;
		p_src_v0 += i_src0stride;
		p_src_v1 += i_src1stride;
		p_dst_u  += i_dst_stride;
		p_dst_v  += i_dst_stride;
	}
}

void x265_simage_remove_high_freq( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_src, uint32_t i_part_idx, uint32_t i_widht, uint32_t i_height )
{
	int32_t x, y;

	spixel* p_src  = x265_simage_get_luma_addr_p3 ( h, simage_src, i_part_idx ) ;
	spixel* p_src_u = x265_simage_get_cb_addr_p3 ( h, simage_src, i_part_idx ) ;
	spixel* p_src_v = x265_simage_get_cr_addr_p3 ( h, simage_src, i_part_idx ) ;

	spixel* p_dst  = x265_simage_get_luma_addr_p3 ( h, simage, i_part_idx ) ;
	spixel* p_dst_u = x265_simage_get_cb_addr_p3 ( h, simage, i_part_idx ) ;
	spixel* p_dst_v = x265_simage_get_cr_addr_p3 ( h, simage, i_part_idx ) ;

	int32_t  i_src_stride = simage_src->i_stride[0];
	int32_t  i_dst_stride = simage->i_stride[0];

	for ( y = i_height-1; y >= 0; y-- )
	{
		for ( x = i_widht-1; x >= 0; x-- )
		{
#if X265_DISABLING_CLIP_FOR_BIPREDME
			p_dst[x ] = (p_dst[x ]<<1) - p_src[x ] ;
#else
			p_dst[x ] = x265_clip_y(h, (p_dst[x ]<<1) - p_src[x ] );
#endif
		}
		p_src += i_src_stride;
		p_dst += i_dst_stride;
	}

	i_src_stride = simage_src->i_stride[1];
	i_dst_stride = simage->i_stride[1];

	i_height >>= 1;
	i_widht  >>= 1;

	for ( y = i_height-1; y >= 0; y-- )
	{
		for ( x = i_widht-1; x >= 0; x-- )
		{
#if X265_DISABLING_CLIP_FOR_BIPREDME
			p_dst_u[x ] = (p_dst_u[x ]<<1) - p_src_u[x ] ;
			p_dst_v[x ] = (p_dst_v[x ]<<1) - p_src_v[x ] ;
#else
			p_dst_u[x ] = x265_clip_c(h, (p_dst_u[x ]<<1) - p_src_u[x ] );
			p_dst_v[x ] = x265_clip_c(h, (p_dst_v[x ]<<1) - p_src_v[x ] );
#endif
		}
		p_src_u += i_src_stride;
		p_src_v += i_src_stride;
		p_dst_u += i_dst_stride;
		p_dst_v += i_dst_stride;
	}
}


void x265_simage_copy_up_from_simage ( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_src, uint32_t i_cu_addr_x, int32_t i_width )
{
	x265_simage_copy_up_from_simage_luma ( h, simage, simage_src, i_cu_addr_x, i_width ) ;
	x265_simage_copy_up_from_simage_chroma ( h, simage, simage_src, i_cu_addr_x, i_width ) ;
}

void x265_simage_copy_up_from_simage_luma ( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_src, uint32_t i_cu_addr_x, int32_t i_width )
{
	spixel* p_src = NULL, *p_dst = NULL ;

	p_src = x265_simage_get_luma_addr_p2 ( h, simage_src ) + h->cu.pic.i_max_cu_width * i_cu_addr_x - 1 ;
	p_dst = x265_simage_get_luma_addr_p2 ( h, simage ) - simage->i_stride[0] - 1;
	memcpy( p_dst, p_src, sizeof(spixel) * (i_width + 1) );
}

void x265_simage_copy_up_from_simage_chroma ( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_src, uint32_t i_cu_addr_x, int32_t i_width )
{
	spixel* p_src = NULL, *p_dst = NULL ;

	p_src = x265_simage_get_cb_addr_p2 ( h, simage_src ) + (h->cu.pic.i_max_cu_width>>1) * i_cu_addr_x - 1 ;
	p_dst = x265_simage_get_cb_addr_p2 ( h, simage ) - simage->i_stride[1] - 1;
	memcpy( p_dst, p_src, sizeof(spixel) * ((i_width>>1) + 1) );

	p_src = x265_simage_get_cr_addr_p2 ( h, simage_src ) + (h->cu.pic.i_max_cu_width>>1) * i_cu_addr_x - 1 ;
	p_dst = x265_simage_get_cr_addr_p2 ( h, simage ) - simage->i_stride[1] - 1;
	memcpy( p_dst, p_src, sizeof(spixel) * ((i_width>>1) + 1) );

}

void x265_simage_copy_left_from_simage ( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_src )
{
	x265_simage_copy_left_from_simage_luma ( h, simage, simage_src ) ;
	x265_simage_copy_left_from_simage_chroma ( h, simage, simage_src ) ;
}

void x265_simage_copy_left_from_simage_luma ( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_src )
{
	int32_t y = 0, i_height = 0 ;

	spixel* p_src = x265_simage_get_luma_addr_p2 ( h, simage_src ) ;
	spixel* p_dst = x265_simage_get_luma_addr_p2 ( h, simage )  - 1 ;

	uint32_t  i_src_stride = simage_src->i_stride[0] ;
	uint32_t  i_dst_stride = simage->i_stride[0] ;

	i_height = h->cu.pic.i_max_cu_height ;
	for ( y = i_height; y != 0; y-- )
	{
		p_dst[0] = p_src[0] ;
		p_dst += i_dst_stride;
		p_src += i_src_stride;
	}
}

void x265_simage_copy_left_from_simage_chroma ( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_src )
{
	int32_t  y = 0, i_height = 0 ;
	spixel* p_src_u = x265_simage_get_cb_addr_p2 ( h, simage_src ) ;
	spixel* p_src_v = x265_simage_get_cr_addr_p2 ( h, simage_src ) ;
	spixel* p_dst_u = x265_simage_get_cb_addr_p2 ( h, simage )  - 1 ;
	spixel* p_dst_v = x265_simage_get_cr_addr_p2 ( h, simage )  - 1 ;
	uint32_t  i_src_stride = simage_src->i_stride[1];
	uint32_t  i_dst_stride = simage->i_stride[1];

	i_height = h->cu.pic.i_max_cu_height >> 1 ;
	for ( y = i_height; y != 0; y-- )
	{
		p_dst_u[0] = p_src_u[0] ;
		p_dst_v[0] = p_src_v[0] ;
		p_src_u += i_src_stride;
		p_src_v += i_src_stride;
		p_dst_u += i_dst_stride;
		p_dst_v += i_dst_stride;
	}
}


void x265_simage_copy_right_from_frame ( x265_t *h, x265_simage_t *simage, x265_frame_t* frame_src, uint32_t i_cu_addr )
{
	x265_simage_copy_right_from_frame_luma ( h, simage, frame_src, i_cu_addr ) ;
	x265_simage_copy_right_from_frame_chroma ( h, simage, frame_src, i_cu_addr ) ;

}

void x265_simage_copy_right_from_frame_luma ( x265_t *h, x265_simage_t *simage, x265_frame_t* frame_src, uint32_t i_cu_addr )
{
	int32_t y = 0, i_height = 0 ;

	pixel* p_src = x265_frame_get_luma_addr_p3 ( h, frame_src, i_cu_addr ) + h->cu.pic.i_max_cu_width - 1;
	spixel* p_dst = x265_simage_get_luma_addr_p2 ( h, simage ) ;

	uint32_t  i_src_stride = frame_src->i_stride[0] ;
	uint32_t  i_dst_stride = simage->i_stride[0] ;

	i_height = h->cu.pic.i_max_cu_height ;
	for ( y = i_height; y != 0; y-- )
	{
		p_dst[0] = p_src[0] ;
		p_dst += i_dst_stride;
		p_src += i_src_stride;
	}
}

void x265_simage_copy_right_from_frame_chroma ( x265_t *h, x265_simage_t *simage, x265_frame_t* frame_src, uint32_t i_cu_addr )
{
	int32_t  y = 0, i_height = 0 ;
	pixel* p_src_u = x265_frame_get_cb_addr_p3 ( h, frame_src, i_cu_addr ) + (h->cu.pic.i_max_cu_width>>1) - 1 ;
	pixel* p_src_v = x265_frame_get_cr_addr_p3 ( h, frame_src, i_cu_addr ) + (h->cu.pic.i_max_cu_width>>1) - 1 ;
	spixel* p_dst_u = x265_simage_get_cb_addr_p2 ( h, simage ) ;
	spixel* p_dst_v = x265_simage_get_cr_addr_p2 ( h, simage ) ;
	uint32_t  i_src_stride = frame_src->i_stride[1];
	uint32_t  i_dst_stride = simage->i_stride[1];

	i_height = h->cu.pic.i_max_cu_height >> 1 ;
	for ( y = i_height; y != 0; y-- )
	{
		p_dst_u[0] = p_src_u[0] ;
		p_dst_v[0] = p_src_v[0] ;
		p_src_u += i_src_stride;
		p_src_v += i_src_stride;
		p_dst_u += i_dst_stride;
		p_dst_v += i_dst_stride;
	}
}

void x265_simage_copy_bottom_from_frame ( x265_t *h, x265_simage_t *simage, x265_frame_t* frame_src, uint32_t i_cu_addr_x, uint32_t i_cu_addr )
{
	x265_simage_copy_bottom_from_frame_luma ( h, simage, frame_src, i_cu_addr_x, i_cu_addr ) ;
	x265_simage_copy_bottom_from_frame_chroma ( h, simage, frame_src, i_cu_addr_x, i_cu_addr ) ;
}

void x265_simage_copy_bottom_from_frame_luma ( x265_t *h, x265_simage_t *simage, x265_frame_t* frame_src, uint32_t i_cu_addr_x, uint32_t i_cu_addr )
{
	pixel* p_src = NULL ;
	spixel *p_dst = NULL ;
	int32_t i_src_stride = 0 ;

	i_src_stride = frame_src->i_stride[0] ;
	p_src = x265_frame_get_luma_addr_p3 ( h, frame_src, i_cu_addr )
			+ (h->cu.pic.i_max_cu_height - 1) * i_src_stride ;
	p_dst = x265_simage_get_luma_addr_p2 ( h, simage ) + h->cu.pic.i_max_cu_width * i_cu_addr_x ;
	memcpy( p_dst, p_src, sizeof(spixel) * h->cu.pic.i_max_cu_width );
}

void x265_simage_copy_bottom_from_frame_chroma ( x265_t *h, x265_simage_t *simage, x265_frame_t* frame_src, uint32_t i_cu_addr_x, uint32_t i_cu_addr )
{
	pixel* p_src = NULL ;
	spixel *p_dst = NULL ;
	int32_t i_src_c_stride = 0 ;

	i_src_c_stride = frame_src->i_stride[1] ;

	p_src = x265_frame_get_cb_addr_p3 ( h, frame_src, i_cu_addr )
			+ ((h->cu.pic.i_max_cu_height>>1) - 1) * i_src_c_stride ;
	p_dst = x265_simage_get_cb_addr_p2 ( h, simage ) + (h->cu.pic.i_max_cu_width>>1) * i_cu_addr_x ;
	memcpy( p_dst, p_src, sizeof(spixel) * (h->cu.pic.i_max_cu_width>>1) );
	p_src = x265_frame_get_cr_addr_p3 ( h, frame_src, i_cu_addr )
			+ ((h->cu.pic.i_max_cu_height>>1) - 1) * i_src_c_stride ;
	p_dst = x265_simage_get_cr_addr_p2 ( h, simage ) + (h->cu.pic.i_max_cu_width>>1) * i_cu_addr_x ;
	memcpy( p_dst, p_src, sizeof(spixel) * (h->cu.pic.i_max_cu_width>>1) );

}


void x265_simage_copy_to_simage ( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_dst, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx )
{
	x265_simage_copy_to_simage_luma ( h, simage, simage_dst, i_abs_zorder_idx, i_part_depth, i_part_idx ) ;
	x265_simage_copy_to_simage_chroma ( h, simage, simage_dst, i_abs_zorder_idx, i_part_depth, i_part_idx ) ;
}

void x265_simage_copy_to_simage_luma ( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_dst, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx )
{
	int32_t  y, i_width, i_height;
	i_width  = simage->i_width >>i_part_depth;
	i_height = simage->i_height>>i_part_depth;

	spixel* p_src     = x265_simage_get_luma_addr_p4 ( h, simage, i_part_idx, i_width);
	spixel* p_dst     = x265_simage_get_luma_addr_p3 ( h, simage_dst, i_abs_zorder_idx );

	uint32_t  i_src_stride  = simage->i_stride[0];
	uint32_t  i_dst_stride  = simage_dst->i_stride[0];

	for ( y = i_height; y != 0; y-- )
	{
		memcpy( p_dst, p_src, sizeof(spixel)*i_width);
		p_dst += i_dst_stride;
		p_src += i_src_stride;
	}
}

void x265_simage_copy_to_simage_chroma ( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_dst, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx )
{
	int32_t  y, i_width, i_height;
	i_width  = (simage->i_width/2)>>i_part_depth;
	i_height = (simage->i_height/2)>>i_part_depth;

	spixel* p_src_u      = x265_simage_get_cb_addr_p4 ( h, simage, i_part_idx, i_width);
	spixel* p_src_v      = x265_simage_get_cr_addr_p4 ( h, simage, i_part_idx, i_width);
	spixel* p_dst_u      = x265_simage_get_cb_addr_p3 ( h, simage_dst, i_abs_zorder_idx );
	spixel* p_dst_v      = x265_simage_get_cr_addr_p3 ( h, simage_dst, i_abs_zorder_idx );

	uint32_t  i_src_stride = simage->i_stride[1];
	uint32_t  i_dst_stride = simage_dst->i_stride[1];
	for ( y = i_height; y != 0; y-- )
	{
		memcpy( p_dst_u, p_src_u, sizeof(spixel)*(i_width) );
		memcpy( p_dst_v, p_src_v, sizeof(spixel)*(i_width) );
		p_src_u += i_src_stride;
		p_src_v += i_src_stride;
		p_dst_u += i_dst_stride;
		p_dst_v += i_dst_stride;
	}

}

