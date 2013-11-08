

#include "common.h"



void x265_image_delete ( x265_image_t *image )
{
	x265_image_deinit ( image ) ;
	x265_free ( image ) ;
}

x265_image_t *x265_image_new ()
{
	x265_image_t *image = NULL ;

	CHECKED_MALLOCZERO( image, sizeof(x265_image_t) );
	if ( x265_image_init (image) )
	{
		goto fail ;
	}

	return image ;
fail:
	x265_image_delete ( (void*) image ) ;
	return NULL ;
}

int x265_image_init ( x265_image_t *image )
{

	return 0 ;
}

void x265_image_deinit ( x265_image_t *image )
{

}

int x265_image_create ( x265_image_t *image, uint32_t i_width, uint32_t i_height )
{
	uint32_t i_total_size = 0 ;

	image->i_width = i_width ;
	image->i_height = i_height ;

	image->i_stride[0] = i_width ;
	image->i_stride[1] = image->i_stride[0] / 2 ;
	image->i_stride[2] = image->i_stride[1] ;

	i_total_size = image->i_stride[0] * i_height * 3 / 2 ;
	CHECKED_MALLOCZERO ( image->buffer, i_total_size * sizeof (pixel) ) ;

	image->plane[0] = image->buffer ;
	image->plane[1] = image->buffer + image->i_stride[0] * i_height ;
	image->plane[2] = image->plane[1] + image->i_stride[1] * (i_height >> 1) ;

	return 0 ;

fail:
	x265_image_destroy ( image ) ;
	return -1 ;
}

void x265_image_destroy ( x265_image_t *image )
{
	int32_t loop = 0 ;

	x265_free ( image->buffer ) ;

	image->buffer = NULL ;
	image->i_width = 0 ;
	image->i_height = 0 ;

	for ( loop = 0 ; loop < 3 ; ++ loop )
	{
		image->i_stride[loop] = 0 ;
		image->plane[loop] = NULL ;
	}
}

void x265_image_clear ( x265_image_t *image )
{
	memset ( image->plane[0], 0, image->i_stride[0] * image->i_height * sizeof(pixel) ) ;
	memset ( image->plane[1], 0, image->i_stride[1] * (image->i_height>>1) * sizeof(pixel) ) ;
	memset ( image->plane[2], 0, image->i_stride[2] * (image->i_height>>1) * sizeof(pixel) ) ;

}

int x265_image_create_intra_image ( x265_image_t *image, uint32_t i_width, uint32_t i_height )
{
	uint32_t i_total_size = 0 ;

	image->i_width = i_width ;
	image->i_height = i_height ;

	image->i_stride[0] = i_width + 2 ;
	image->i_stride[1] = image->i_stride[0] / 2 ;
	image->i_stride[2] = image->i_stride[1] ;

	i_total_size = image->i_stride[0] * ( i_height + 2 ) * 3 / 2 ;
	CHECKED_MALLOCZERO ( image->buffer, i_total_size * sizeof (pixel) ) ;

	image->plane[0] = image->buffer + image->i_stride[0] * 2 + 2 ;
	image->plane[1] = image->buffer + image->i_stride[0] * ( i_height + 2 )
						+ image->i_stride[1] + 1 ;
	image->plane[2] = image->plane[1] + image->i_stride[1] * ((i_height+2) >> 1) ;

	return 0 ;

fail:
	x265_image_destroy ( image ) ;
	return -1 ;
}

int32_t x265_image_get_addr_offset_p4 ( x265_t *h, x265_image_t *image,
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
int32_t x265_image_get_addr_offset_p5 ( x265_t *h, x265_image_t *image,
										uint32_t i_trans_unit_idx, uint32_t i_blk_size, uint32_t i_width )
{
	int32_t i_blk_x = 0 ;
	int32_t i_blk_y = 0 ;

	i_blk_x = ( i_trans_unit_idx * i_blk_size ) &  ( i_width - 1 );
	i_blk_y = ( i_trans_unit_idx * i_blk_size ) &~ ( i_width - 1 );

	return i_blk_x + i_blk_y * i_blk_size ;
}


pixel* x265_image_get_luma_addr_p2 ( x265_t *h, x265_image_t* image )
{
	return image->plane[0];
}

pixel* x265_image_get_cb_addr_p2 ( x265_t *h, x265_image_t* image )
{
	return image->plane[1];
}

pixel* x265_image_get_cr_addr_p2 ( x265_t *h, x265_image_t* image )
{
	return image->plane[2];
}

//  access starting position of image partition unit buffer
pixel* x265_image_get_luma_addr_p3 ( x265_t *h, x265_image_t* image,
									uint32_t i_part_unit_idx )
{
	return image->plane[0]
	       + x265_image_get_addr_offset_p4( h, image,
											i_part_unit_idx, image->i_stride[0]  ) ;
}

pixel* x265_image_get_cb_addr_p3 ( x265_t *h, x265_image_t* image,
									uint32_t i_part_unit_idx )
{
	return image->plane[1]
	       + ( x265_image_get_addr_offset_p4( h, image,
	    		   	   	   	   	   	   	   	i_part_unit_idx, image->i_stride[1] ) >> 1 ) ;
}

pixel* x265_image_get_cr_addr_p3 ( x265_t *h, x265_image_t* image,
									uint32_t i_part_unit_idx )
{
	return image->plane[2]
	       + ( x265_image_get_addr_offset_p4( h, image,
	    		   	   	   	   	   	   	   	i_part_unit_idx, image->i_stride[2] ) >> 1 ) ;
}


//  access starting position of image transform unit buffer
pixel* x265_image_get_luma_addr_p4 ( x265_t *h, x265_image_t* image,
									uint32_t i_trans_unit_idx, uint32_t i_blk_size )
{
	return image->plane[0]
	       + x265_image_get_addr_offset_p5( h, image,
	    		   	   	   	   	   	   	   i_trans_unit_idx, i_blk_size, image->i_stride[0] );
}

pixel* x265_image_get_cb_addr_p4 ( x265_t *h, x265_image_t* image,
								uint32_t i_trans_unit_idx, uint32_t i_blk_size )
{
	return image->plane[1]
	       + x265_image_get_addr_offset_p5( h, image,
	    		   	   	   	   	   	   	   i_trans_unit_idx, i_blk_size, image->i_stride[1] );
}

pixel* x265_image_get_cr_addr_p4 ( x265_t *h, x265_image_t* image,
								uint32_t i_trans_unit_idx, uint32_t i_blk_size )
{
	return image->plane[2]
	       + x265_image_get_addr_offset_p5( h, image,
	    		   	   	   	   	   	   	   i_trans_unit_idx, i_blk_size, image->i_stride[2] );
}

//  get stride value of image buffer
uint32_t x265_image_get_stride ( x265_image_t* image )
{
	return image->i_stride[0] ;
}

uint32_t x265_image_get_c_stride ( x265_image_t* image )
{
	return image->i_stride[1] ;
}

uint32_t x265_image_get_width ( x265_image_t* image )
{
	return  image->i_width ;
}

uint32_t x265_image_get_height ( x265_image_t* image )
{
	return  image->i_height ;
}

uint32_t x265_image_get_c_width ( x265_image_t* image )
{
	return image->i_width / 2 ;
}

uint32_t x265_image_get_c_height ( x265_image_t* image )
{
	return  image->i_height / 2 ;
}

void x265_image_copy_to_frame ( x265_t *h, x265_image_t *image, x265_frame_t* frame_dst, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx )
{
	x265_image_copy_to_frame_luma  ( h, image, frame_dst, i_cu_addr, i_abs_zorder_idx, i_part_depth, i_part_idx );
	x265_image_copy_to_frame_chroma( h, image, frame_dst, i_cu_addr, i_abs_zorder_idx, i_part_depth, i_part_idx );
}

void x265_image_copy_to_frame_luma  ( x265_t *h, x265_image_t *image, x265_frame_t* frame_dst, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx )
{
	int32_t  y, i_width, i_height;
	i_width  = image->i_width >>i_part_depth;
	i_height = image->i_height>>i_part_depth;

	pixel* p_src = x265_image_get_luma_addr_p4 ( h, image, i_part_idx, i_width);
	pixel* p_dst = x265_frame_get_luma_addr_p4 ( h, frame_dst, i_cu_addr, i_abs_zorder_idx );

	uint32_t  i_src_stride  = image->i_stride[0];
	uint32_t  i_dst_stride  = frame_dst->i_stride[0];

	for ( y = i_height; y != 0; y-- )
	{
		memcpy( p_dst, p_src, sizeof(pixel)*i_width);
		p_dst += i_dst_stride;
		p_src += i_src_stride;
	}
}

void x265_image_copy_to_frame_chroma( x265_t *h, x265_image_t *image, x265_frame_t* frame_dst, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx )
{
	int32_t  y, i_width, i_height;
	i_width  = (image->i_width/2) >> i_part_depth;
	i_height = (image->i_height/2) >> i_part_depth;

	pixel* p_src_u      = x265_image_get_cb_addr_p4( h, image, i_part_idx, i_width);
	pixel* p_src_v      = x265_image_get_cr_addr_p4( h, image, i_part_idx, i_width);
	pixel* p_dst_u      = x265_frame_get_cb_addr_p4 ( h, frame_dst, i_cu_addr, i_abs_zorder_idx );
	pixel* p_dst_v      = x265_frame_get_cr_addr_p4 ( h, frame_dst, i_cu_addr, i_abs_zorder_idx );

	uint32_t  i_src_stride = image->i_stride[1];
	uint32_t  i_dst_stride = frame_dst->i_stride[1];
	for ( y = i_height; y != 0; y-- )
	{
		memcpy( p_dst_u, p_src_u, sizeof(pixel)*(i_width) );
		memcpy( p_dst_v, p_src_v, sizeof(pixel)*(i_width) );
		p_src_u += i_src_stride;
		p_src_v += i_src_stride;
		p_dst_u += i_dst_stride;
		p_dst_v += i_dst_stride;
	}
}

void x265_image_copy_from_frame   ( x265_t *h, x265_image_t *image, x265_frame_t* frame_src, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx )
{
	x265_image_copy_from_frame_luma  ( h, image, frame_src, i_cu_addr, i_abs_zorder_idx );
	x265_image_copy_from_frame_chroma( h, image, frame_src, i_cu_addr, i_abs_zorder_idx );
}

void x265_image_copy_from_frame_luma ( x265_t *h, x265_image_t *image, x265_frame_t* frame_src, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx )
{
	int32_t  y;

	pixel* p_dst     = image->plane[0];
	pixel* p_src     = x265_frame_get_luma_addr_p4 ( h, frame_src, i_cu_addr, i_abs_zorder_idx );

	uint32_t  i_dst_stride  = image->i_stride[0];
	uint32_t  i_src_stride  = frame_src->i_stride[0];
	for ( y = image->i_height; y != 0; y-- )
	{
		memcpy( p_dst, p_src, sizeof(pixel)*image->i_width);
		p_dst += i_dst_stride;
		p_src += i_src_stride;
	}
}

void x265_image_copy_from_frame_chroma( x265_t *h, x265_image_t *image, x265_frame_t* frame_src, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx )
{
	int32_t  y;

	pixel* p_dst_u      = image->plane[1];
	pixel* p_dst_v      = image->plane[2];
	pixel* p_src_u      = x265_frame_get_cb_addr_p4 ( h, frame_src, i_cu_addr, i_abs_zorder_idx );
	pixel* p_src_v      = x265_frame_get_cr_addr_p4 ( h, frame_src, i_cu_addr, i_abs_zorder_idx );

	uint32_t  i_dst_stride = image->i_stride[1];
	uint32_t  i_src_stride = frame_src->i_stride[1];
	for ( y = image->i_height/2; y != 0; y-- )
	{
		memcpy( p_dst_u, p_src_u, sizeof(pixel)*(image->i_width/2) );
		memcpy( p_dst_v, p_src_v, sizeof(pixel)*(image->i_width/2) );
		p_src_u += i_src_stride;
		p_src_v += i_src_stride;
		p_dst_u += i_dst_stride;
		p_dst_v += i_dst_stride;
	}
}

void x265_image_copy_to_part_image( x265_t *h, x265_image_t *image, x265_image_t* image_dst, uint32_t i_dst_part_idx )
{
	x265_image_copy_to_part_luma  ( h, image, image_dst, i_dst_part_idx );
	x265_image_copy_to_part_chroma( h, image, image_dst, i_dst_part_idx );
}

void x265_image_copy_to_part_luma( x265_t *h, x265_image_t *image, x265_image_t* image_dst, uint32_t i_dst_part_idx )
{
	int32_t  y;

	pixel* p_src     = image->plane[0];
	pixel* p_dst     = x265_image_get_luma_addr_p3 ( h, image_dst, i_dst_part_idx );

	uint32_t  i_src_stride  = image->i_stride[0];
	uint32_t  i_dst_stride  = image_dst->i_stride[0];
	for ( y = image->i_height; y != 0; y-- )
	{
		memcpy( p_dst, p_src, sizeof(pixel)*image->i_width);
		p_dst += i_dst_stride;
		p_src += i_src_stride;
	}
}

void x265_image_copy_to_part_chroma( x265_t *h, x265_image_t *image, x265_image_t* image_dst, uint32_t i_dst_part_idx )
{
	int32_t  y;

	pixel* p_src_u      = image->plane[1];
	pixel* p_src_v      = image->plane[2];
	pixel* p_dst_u      = x265_image_get_cb_addr_p3 ( h, image_dst, i_dst_part_idx );
	pixel* p_dst_v      = x265_image_get_cr_addr_p3 ( h, image_dst, i_dst_part_idx );

	uint32_t  i_src_stride = image->i_stride[1];
	uint32_t  i_dst_stride = image_dst->i_stride[1];
	for ( y = image->i_height/2; y != 0; y-- )
	{
		memcpy( p_dst_u, p_src_u, sizeof(pixel)*(image->i_width/2) );
		memcpy( p_dst_v, p_src_v, sizeof(pixel)*(image->i_width/2) );
		p_src_u += i_src_stride;
		p_src_v += i_src_stride;
		p_dst_u += i_dst_stride;
		p_dst_v += i_dst_stride;
	}
}

void x265_image_copy_part_to_image( x265_t *h, x265_image_t *image, x265_image_t* image_dst, uint32_t i_src_part_idx )
{
	x265_image_copy_part_to_luma  ( h, image, image_dst, i_src_part_idx );
	x265_image_copy_part_to_chroma( h, image, image_dst, i_src_part_idx );
}

void x265_image_copy_part_to_luma( x265_t *h, x265_image_t *image, x265_image_t* image_dst, uint32_t i_src_part_idx )
{
	int32_t  y;

	pixel* p_src     = x265_image_get_luma_addr_p3 ( h, image, i_src_part_idx);
	pixel* p_dst     = x265_image_get_luma_addr_p3 ( h, image_dst, 0 );

	uint32_t  i_src_stride  = image->i_stride[0];
	uint32_t  i_dst_stride  = image_dst->i_stride[0];

	uint32_t i_height = image_dst->i_height;
	uint32_t i_width = image_dst->i_width;

	for ( y = i_height; y != 0; y-- )
	{
		memcpy( p_dst, p_src, sizeof(pixel)*i_width);
		p_dst += i_dst_stride;
		p_src += i_src_stride;
	}
}

void x265_image_copy_part_to_chroma( x265_t *h, x265_image_t *image, x265_image_t* image_dst, uint32_t i_src_part_idx )
{
	int32_t  y;

	pixel* p_src_u      = x265_image_get_cb_addr_p3 ( h, image,  i_src_part_idx );
	pixel* p_src_v      = x265_image_get_cr_addr_p3 ( h, image,  i_src_part_idx );
	pixel* p_dst_u      = x265_image_get_cb_addr_p3 ( h, image_dst,  0 );
	pixel* p_dst_v      = x265_image_get_cr_addr_p3 ( h, image_dst,  0 );

	uint32_t  i_src_stride = image->i_stride[1];
	uint32_t  i_dst_stride = image_dst->i_stride[1];

	uint32_t i_c_height = image_dst->i_height/2;
	uint32_t i_c_width = image_dst->i_width/2;

	for ( y = i_c_height; y != 0; y-- )
	{
		memcpy( p_dst_u, p_src_u, sizeof(pixel)*(i_c_width) );
		memcpy( p_dst_v, p_src_v, sizeof(pixel)*(i_c_width) );
		p_src_u += i_src_stride;
		p_src_v += i_src_stride;
		p_dst_u += i_dst_stride;
		p_dst_v += i_dst_stride;
	}
}

void x265_image_copy_part_to_part_image ( x265_t *h, x265_image_t *image, x265_image_t* image_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height )
{
	x265_image_copy_part_to_part_luma   ( h, image, image_dst, i_part_idx, i_width, i_height );
	x265_image_copy_part_to_part_chroma ( h, image, image_dst, i_part_idx, i_width>>1, i_height>>1 );
}

void x265_image_copy_part_to_part_luma ( x265_t *h, x265_image_t *image, x265_image_t* image_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height )
{
	pixel* p_src =           x265_image_get_luma_addr_p3 ( h, image, i_part_idx);
	pixel* p_dst =           x265_image_get_luma_addr_p3(h, image_dst,i_part_idx);
	if( p_src == p_dst )
	{
		//th not a good idea
		//th best would be to fix the caller
		return ;
	}

	uint32_t  i_src_stride = image->i_stride[0];
	uint32_t  i_dst_stride = image_dst->i_stride[0];
	for ( uint32_t y = i_height; y != 0; y-- )
	{
		memcpy( p_dst, p_src, i_width * sizeof(pixel) );
		p_src += i_src_stride;
		p_dst += i_dst_stride;
	}
}

void x265_image_copy_part_to_part_chroma( x265_t *h, x265_image_t *image, x265_image_t* image_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height )
{
	pixel*  p_src_u =           x265_image_get_cb_addr_p3 ( h, image, i_part_idx);
	pixel*  p_src_v =           x265_image_get_cr_addr_p3 ( h, image, i_part_idx);
	pixel*  p_dst_u = x265_image_get_cb_addr_p3 ( h, image_dst, i_part_idx);
	pixel*  p_dst_v = x265_image_get_cr_addr_p3 ( h, image_dst, i_part_idx);

	if( p_src_u == p_dst_u && p_src_v == p_dst_v)
	{
		//th not a good idea
		//th best would be to fix the caller
		return ;
	}

	uint32_t   i_src_stride = image->i_stride[1];
	uint32_t   i_dst_stride = image_dst->i_stride[1];
	for ( uint32_t y = i_height; y != 0; y-- )
	{
		memcpy( p_dst_u, p_src_u, i_width * sizeof(pixel) );
		memcpy( p_dst_v, p_src_v, i_width * sizeof(pixel) );
		p_src_u += i_src_stride;
		p_src_v += i_src_stride;
		p_dst_u += i_dst_stride;
		p_dst_v += i_dst_stride;
	}
}

void x265_image_copy_part_to_part_chroma_p7( x265_t *h, x265_image_t *image, x265_image_t* image_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height, uint32_t chroma_id)
{
	if(chroma_id == 0)
	{
		pixel*  p_src_u =           x265_image_get_cb_addr_p3 ( h, image, i_part_idx);
		pixel*  p_dst_u = x265_image_get_cb_addr_p3 ( h, image_dst, i_part_idx);
		if( p_src_u == p_dst_u)
		{
			return ;
		}
		uint32_t   i_src_stride = image->i_stride[1];
		uint32_t   i_dst_stride = image_dst->i_stride[1];
		for ( uint32_t y = i_height; y != 0; y-- )
		{
			memcpy( p_dst_u, p_src_u, i_width * sizeof(pixel) );
			p_src_u += i_src_stride;
			p_dst_u += i_dst_stride;
		}
	}
	else if (chroma_id == 1)
	{
		pixel*  p_src_v =           x265_image_get_cr_addr_p3 ( h, image, i_part_idx);
		pixel*  p_dst_v = x265_image_get_cr_addr_p3 ( h, image_dst, i_part_idx);
		if( p_src_v == p_dst_v)
		{
			return;
		}
		uint32_t   i_src_stride = image->i_stride[1];
		uint32_t   i_dst_stride = image_dst->i_stride[1];
		for ( uint32_t y = i_height; y != 0; y-- )
		{
			memcpy( p_dst_v, p_src_v, i_width * sizeof(pixel) );
			p_src_v += i_src_stride;
			p_dst_v += i_dst_stride;
		}
	}
	else
	{
		pixel*  p_src_u =           x265_image_get_cb_addr_p3 ( h, image, i_part_idx);
		pixel*  p_src_v =           x265_image_get_cr_addr_p3 ( h, image, i_part_idx);
		pixel*  p_dst_u = x265_image_get_cb_addr_p3 ( h, image_dst, i_part_idx);
		pixel*  p_dst_v = x265_image_get_cr_addr_p3 ( h, image_dst, i_part_idx);

		if( p_src_u == p_dst_u && p_src_v == p_dst_v)
		{
			//th not a good idea
			//th best would be to fix the caller
			return ;
		}
		uint32_t   i_src_stride = image->i_stride[1];
		uint32_t   i_dst_stride = image_dst->i_stride[1];
		for ( uint32_t y = i_height; y != 0; y-- )
		{
			memcpy( p_dst_u, p_src_u, i_width * sizeof(pixel) );
			memcpy( p_dst_v, p_src_v, i_width * sizeof(pixel) );
			p_src_u += i_src_stride;
			p_src_v += i_src_stride;
			p_dst_u += i_dst_stride;
			p_dst_v += i_dst_stride;
		}
	}
}

void x265_image_add_clip( x265_t *h, x265_image_t *image, x265_image_t* image_src0, x265_simage_t* image_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size )
{
	x265_image_add_clip_luma   ( h, image, image_src0, image_src1, i_tr_unit_idx, i_part_size     );
	x265_image_add_clip_chroma ( h, image, image_src0, image_src1, i_tr_unit_idx, i_part_size>>1  );
}

void x265_image_add_clip_luma( x265_t *h, x265_image_t *image, x265_image_t* image_src0, x265_simage_t* image_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size )
{
	int32_t x, y;

	pixel* p_src0 = x265_image_get_luma_addr_p4 ( h, image_src0, i_tr_unit_idx, i_part_size );
	spixel* p_src1 = x265_simage_get_luma_addr_p4 ( h, image_src1, i_tr_unit_idx, i_part_size );
	pixel* p_dst  = x265_image_get_luma_addr_p4 ( h, image, i_tr_unit_idx, i_part_size );

	uint32_t i_src0stride = image_src0->i_stride[0];
	uint32_t i_src1stride = image_src1->i_stride[0];
	uint32_t i_dst_stride = image->i_stride[0];
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

void x265_image_add_clip_chroma( x265_t *h, x265_image_t *image, x265_image_t* image_src0, x265_simage_t* image_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size )
{
	int32_t x, y;

	pixel* p_src_u0 = x265_image_get_cb_addr_p4 ( h, image_src0, i_tr_unit_idx, i_part_size );
	spixel* p_src_u1 = x265_simage_get_cb_addr_p4 ( h, image_src1, i_tr_unit_idx, i_part_size );
	pixel* p_src_v0 = x265_image_get_cr_addr_p4 ( h, image_src0, i_tr_unit_idx, i_part_size );
	spixel* p_src_v1 = x265_simage_get_cr_addr_p4 ( h, image_src1, i_tr_unit_idx, i_part_size );
	pixel* p_dst_u = x265_image_get_cb_addr_p4 ( h, image, i_tr_unit_idx, i_part_size );
	pixel* p_dst_v = x265_image_get_cr_addr_p4 ( h, image, i_tr_unit_idx, i_part_size );

	uint32_t  i_src0stride = image_src0->i_stride[1];
	uint32_t  i_src1stride = image_src1->i_stride[1];
	uint32_t  i_dst_stride = image->i_stride[1];
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

void x265_image_subtract( x265_t *h, x265_image_t *image, x265_image_t* image_src0, x265_image_t* image_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size )
{
	x265_image_subtract_luma  ( h, image, image_src0, image_src1,  i_tr_unit_idx, i_part_size    );
	x265_image_subtract_chroma( h, image, image_src0, image_src1,  i_tr_unit_idx, i_part_size>>1 );
}

void x265_image_subtract_luma( x265_t *h, x265_image_t *image, x265_image_t* image_src0, x265_image_t* image_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size )
{
	int32_t x, y;

	pixel* p_src0 = x265_image_get_luma_addr_p4 ( h, image_src0, i_tr_unit_idx, i_part_size );
	pixel* p_src1 = x265_image_get_luma_addr_p4 ( h, image_src1, i_tr_unit_idx, i_part_size );
	pixel* p_dst  = x265_image_get_luma_addr_p4 ( h, image, i_tr_unit_idx, i_part_size );

	uint32_t i_src0stride = image_src0->i_stride[0];
	uint32_t i_src1stride = image_src1->i_stride[0];
	uint32_t i_dst_stride = image->i_stride[0];
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

void x265_image_subtract_chroma( x265_t *h, x265_image_t *image, x265_image_t* image_src0, x265_image_t* image_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size )
{
	int32_t x, y;

	pixel* p_src_u0 = x265_image_get_cb_addr_p4 ( h, image_src0, i_tr_unit_idx, i_part_size );
	pixel* p_src_u1 = x265_image_get_cb_addr_p4 ( h, image_src1, i_tr_unit_idx, i_part_size );
	pixel* p_src_v0 = x265_image_get_cr_addr_p4 ( h, image_src0, i_tr_unit_idx, i_part_size );
	pixel* p_src_v1 = x265_image_get_cr_addr_p4 ( h, image_src1, i_tr_unit_idx, i_part_size );
	pixel* p_dst_u = x265_image_get_cb_addr_p4 ( h, image, i_tr_unit_idx, i_part_size );
	pixel* p_dst_v = x265_image_get_cr_addr_p4 ( h, image, i_tr_unit_idx, i_part_size );

	uint32_t  i_src0stride = image_src0->i_stride[1];
	uint32_t  i_src1stride = image_src1->i_stride[1];
	uint32_t  i_dst_stride = image->i_stride[1];
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

void x265_image_add_avg( x265_t *h, x265_image_t *image, x265_simage_t* image_src0, x265_simage_t* image_src1, uint32_t i_part_unit_idx, uint32_t i_width, uint32_t i_height )
{
	int32_t x, y;

	spixel* p_src_y0  = x265_simage_get_luma_addr_p3 ( h, image_src0, i_part_unit_idx );
	spixel* p_src_u0  = x265_simage_get_cb_addr_p3 ( h, image_src0, i_part_unit_idx );
	spixel* p_src_v0  = x265_simage_get_cr_addr_p3 ( h, image_src0, i_part_unit_idx );

	spixel* p_src_y1  = x265_simage_get_luma_addr_p3 ( h, image_src1,  i_part_unit_idx );
	spixel* p_src_u1  = x265_simage_get_cb_addr_p3 ( h, image_src1, i_part_unit_idx );
	spixel* p_src_v1  = x265_simage_get_cr_addr_p3 ( h, image_src1, i_part_unit_idx );

	pixel* p_dst_y   = x265_image_get_luma_addr_p3 ( h, image, i_part_unit_idx );
	pixel* p_dst_u   = x265_image_get_cb_addr_p3 ( h, image, i_part_unit_idx );
	pixel* p_dst_v   = x265_image_get_cr_addr_p3 ( h, image, i_part_unit_idx );

	uint32_t  i_src0stride = image_src0->i_stride[0];
	uint32_t  i_src1stride = image_src1->i_stride[0];
	uint32_t  i_dst_stride = image->i_stride[0];
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

	i_src0stride = image_src0->i_stride[1];
	i_src1stride = image_src1->i_stride[1];
	i_dst_stride  = image->i_stride[1];

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

void x265_image_remove_high_freq( x265_t *h, x265_image_t *image, x265_image_t* image_src, uint32_t i_part_idx, uint32_t i_widht, uint32_t i_height )
{
	int32_t x, y;

	pixel* p_src  = x265_image_get_luma_addr_p3 ( h, image_src, i_part_idx ) ;
	pixel* p_src_u = x265_image_get_cb_addr_p3 ( h, image_src, i_part_idx ) ;
	pixel* p_src_v = x265_image_get_cr_addr_p3 ( h, image_src, i_part_idx ) ;

	pixel* p_dst  = x265_image_get_luma_addr_p3 ( h, image, i_part_idx ) ;
	pixel* p_dst_u = x265_image_get_cb_addr_p3 ( h, image, i_part_idx ) ;
	pixel* p_dst_v = x265_image_get_cr_addr_p3 ( h, image, i_part_idx ) ;

	int32_t  i_src_stride = image_src->i_stride[0];
	int32_t  i_dst_stride = image->i_stride[0];

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

	i_src_stride = image_src->i_stride[1];
	i_dst_stride = image->i_stride[1];

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


void x265_image_copy_up_from_image ( x265_t *h, x265_image_t *image, x265_image_t* image_src, uint32_t i_cu_addr_x, int32_t i_width )
{
	x265_image_copy_up_from_image_luma ( h, image, image_src, i_cu_addr_x, i_width ) ;
	x265_image_copy_up_from_image_chroma ( h, image, image_src, i_cu_addr_x, i_width ) ;
}

void x265_image_copy_up_from_image_luma ( x265_t *h, x265_image_t *image, x265_image_t* image_src, uint32_t i_cu_addr_x, int32_t i_width )
{
	pixel* p_src = NULL, *p_dst = NULL ;

	p_src = x265_image_get_luma_addr_p2 ( h, image_src ) + h->cu.pic.i_max_cu_width * i_cu_addr_x - 1 ;
	p_dst = x265_image_get_luma_addr_p2 ( h, image ) - image->i_stride[0] - 1;
	memcpy( p_dst, p_src, sizeof(pixel) * (i_width + 1) );
}

void x265_image_copy_up_from_image_chroma ( x265_t *h, x265_image_t *image, x265_image_t* image_src, uint32_t i_cu_addr_x, int32_t i_width )
{
	pixel* p_src = NULL, *p_dst = NULL ;

	p_src = x265_image_get_cb_addr_p2 ( h, image_src ) + (h->cu.pic.i_max_cu_width>>1) * i_cu_addr_x - 1 ;
	p_dst = x265_image_get_cb_addr_p2 ( h, image ) - image->i_stride[1] - 1;
	memcpy( p_dst, p_src, sizeof(pixel) * ((i_width>>1) + 1) );

	p_src = x265_image_get_cr_addr_p2 ( h, image_src ) + (h->cu.pic.i_max_cu_width>>1) * i_cu_addr_x - 1 ;
	p_dst = x265_image_get_cr_addr_p2 ( h, image ) - image->i_stride[1] - 1;
	memcpy( p_dst, p_src, sizeof(pixel) * ((i_width>>1) + 1) );

}

void x265_image_copy_left_from_image ( x265_t *h, x265_image_t *image, x265_image_t* image_src )
{
	x265_image_copy_left_from_image_luma ( h, image, image_src ) ;
	x265_image_copy_left_from_image_chroma ( h, image, image_src ) ;
}

void x265_image_copy_left_from_image_luma ( x265_t *h, x265_image_t *image, x265_image_t* image_src )
{
	int32_t y = 0, i_height = 0 ;

	pixel* p_src = x265_image_get_luma_addr_p2 ( h, image_src ) ;
	pixel* p_dst = x265_image_get_luma_addr_p2 ( h, image )  - 1 ;

	uint32_t  i_src_stride = image_src->i_stride[0] ;
	uint32_t  i_dst_stride = image->i_stride[0] ;

	i_height = h->cu.pic.i_max_cu_height ;
	for ( y = i_height; y != 0; y-- )
	{
		p_dst[0] = p_src[0] ;
		p_dst += i_dst_stride;
		p_src += i_src_stride;
	}
}

void x265_image_copy_left_from_image_chroma ( x265_t *h, x265_image_t *image, x265_image_t* image_src )
{
	int32_t  y = 0, i_height = 0 ;
	pixel* p_src_u = x265_image_get_cb_addr_p2 ( h, image_src ) ;
	pixel* p_src_v = x265_image_get_cr_addr_p2 ( h, image_src ) ;
	pixel* p_dst_u = x265_image_get_cb_addr_p2 ( h, image )  - 1 ;
	pixel* p_dst_v = x265_image_get_cr_addr_p2 ( h, image )  - 1 ;
	uint32_t  i_src_stride = image_src->i_stride[1];
	uint32_t  i_dst_stride = image->i_stride[1];

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


void x265_image_copy_right_from_frame ( x265_t *h, x265_image_t *image, x265_frame_t* frame_src, uint32_t i_cu_addr )
{
	x265_image_copy_right_from_frame_luma ( h, image, frame_src, i_cu_addr ) ;
	x265_image_copy_right_from_frame_chroma ( h, image, frame_src, i_cu_addr ) ;

}

void x265_image_copy_right_from_frame_luma ( x265_t *h, x265_image_t *image, x265_frame_t* frame_src, uint32_t i_cu_addr )
{
	int32_t y = 0, i_height = 0 ;

	pixel* p_src = x265_frame_get_luma_addr_p3 ( h, frame_src, i_cu_addr ) + h->cu.pic.i_max_cu_width - 1;
	pixel* p_dst = x265_image_get_luma_addr_p2 ( h, image ) ;

	uint32_t  i_src_stride = frame_src->i_stride[0] ;
	uint32_t  i_dst_stride = image->i_stride[0] ;

	i_height = h->cu.pic.i_max_cu_height ;
	for ( y = i_height; y != 0; y-- )
	{
		p_dst[0] = p_src[0] ;
		p_dst += i_dst_stride;
		p_src += i_src_stride;
	}
}

void x265_image_copy_right_from_frame_chroma ( x265_t *h, x265_image_t *image, x265_frame_t* frame_src, uint32_t i_cu_addr )
{
	int32_t  y = 0, i_height = 0 ;
	pixel* p_src_u = x265_frame_get_cb_addr_p3 ( h, frame_src, i_cu_addr ) + (h->cu.pic.i_max_cu_width>>1) - 1 ;
	pixel* p_src_v = x265_frame_get_cr_addr_p3 ( h, frame_src, i_cu_addr ) + (h->cu.pic.i_max_cu_width>>1) - 1 ;
	pixel* p_dst_u = x265_image_get_cb_addr_p2 ( h, image ) ;
	pixel* p_dst_v = x265_image_get_cr_addr_p2 ( h, image ) ;
	uint32_t  i_src_stride = frame_src->i_stride[1];
	uint32_t  i_dst_stride = image->i_stride[1];

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

void x265_image_copy_bottom_from_frame ( x265_t *h, x265_image_t *image, x265_frame_t* frame_src, uint32_t i_cu_addr_x, uint32_t i_cu_addr )
{
	x265_image_copy_bottom_from_frame_luma ( h, image, frame_src, i_cu_addr_x, i_cu_addr ) ;
	x265_image_copy_bottom_from_frame_chroma ( h, image, frame_src, i_cu_addr_x, i_cu_addr ) ;
}

void x265_image_copy_bottom_from_frame_luma ( x265_t *h, x265_image_t *image, x265_frame_t* frame_src, uint32_t i_cu_addr_x, uint32_t i_cu_addr )
{
	pixel* p_src = NULL, *p_dst = NULL ;
	int32_t i_src_stride = 0 ;

	i_src_stride = frame_src->i_stride[0] ;
	p_src = x265_frame_get_luma_addr_p3 ( h, frame_src, i_cu_addr )
			+ (h->cu.pic.i_max_cu_height - 1) * i_src_stride ;
	p_dst = x265_image_get_luma_addr_p2 ( h, image ) + h->cu.pic.i_max_cu_width * i_cu_addr_x ;
	memcpy( p_dst, p_src, sizeof(pixel) * h->cu.pic.i_max_cu_width );
}

void x265_image_copy_bottom_from_frame_chroma ( x265_t *h, x265_image_t *image, x265_frame_t* frame_src, uint32_t i_cu_addr_x, uint32_t i_cu_addr )
{
	pixel* p_src = NULL, *p_dst = NULL ;
	int32_t i_src_c_stride = 0 ;

	i_src_c_stride = frame_src->i_stride[1] ;

	p_src = x265_frame_get_cb_addr_p3 ( h, frame_src, i_cu_addr )
			+ ((h->cu.pic.i_max_cu_height>>1) - 1) * i_src_c_stride ;
	p_dst = x265_image_get_cb_addr_p2 ( h, image ) + (h->cu.pic.i_max_cu_width>>1) * i_cu_addr_x ;
	memcpy( p_dst, p_src, sizeof(pixel) * (h->cu.pic.i_max_cu_width>>1) );
	p_src = x265_frame_get_cr_addr_p3 ( h, frame_src, i_cu_addr )
			+ ((h->cu.pic.i_max_cu_height>>1) - 1) * i_src_c_stride ;
	p_dst = x265_image_get_cr_addr_p2 ( h, image ) + (h->cu.pic.i_max_cu_width>>1) * i_cu_addr_x ;
	memcpy( p_dst, p_src, sizeof(pixel) * (h->cu.pic.i_max_cu_width>>1) );

}


void x265_image_copy_to_image ( x265_t *h, x265_image_t *image, x265_image_t* image_dst, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx )
{
	x265_image_copy_to_image_luma ( h, image, image_dst, i_abs_zorder_idx, i_part_depth, i_part_idx ) ;
	x265_image_copy_to_image_chroma ( h, image, image_dst, i_abs_zorder_idx, i_part_depth, i_part_idx ) ;
}

void x265_image_copy_to_image_luma ( x265_t *h, x265_image_t *image, x265_image_t* image_dst, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx )
{
	int32_t  y, i_width, i_height;
	i_width  = image->i_width >>i_part_depth;
	i_height = image->i_height>>i_part_depth;

	pixel* p_src     = x265_image_get_luma_addr_p4 ( h, image, i_part_idx, i_width);
	pixel* p_dst     = x265_image_get_luma_addr_p3 ( h, image_dst, i_abs_zorder_idx );

	uint32_t  i_src_stride  = image->i_stride[0];
	uint32_t  i_dst_stride  = image_dst->i_stride[0];

	for ( y = i_height; y != 0; y-- )
	{
		memcpy( p_dst, p_src, sizeof(pixel)*i_width);
		p_dst += i_dst_stride;
		p_src += i_src_stride;
	}
}

void x265_image_copy_to_image_chroma ( x265_t *h, x265_image_t *image, x265_image_t* image_dst, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx )
{
	int32_t  y, i_width, i_height;
	i_width  = (image->i_width/2)>>i_part_depth;
	i_height = (image->i_height/2)>>i_part_depth;

	pixel* p_src_u      = x265_image_get_cb_addr_p4 ( h, image, i_part_idx, i_width);
	pixel* p_src_v      = x265_image_get_cr_addr_p4 ( h, image, i_part_idx, i_width);
	pixel* p_dst_u      = x265_image_get_cb_addr_p3 ( h, image_dst, i_abs_zorder_idx );
	pixel* p_dst_v      = x265_image_get_cr_addr_p3 ( h, image_dst, i_abs_zorder_idx );

	uint32_t  i_src_stride = image->i_stride[1];
	uint32_t  i_dst_stride = image_dst->i_stride[1];
	for ( y = i_height; y != 0; y-- )
	{
		memcpy( p_dst_u, p_src_u, sizeof(pixel)*(i_width) );
		memcpy( p_dst_v, p_src_v, sizeof(pixel)*(i_width) );
		p_src_u += i_src_stride;
		p_src_v += i_src_stride;
		p_dst_u += i_dst_stride;
		p_dst_v += i_dst_stride;
	}

}

