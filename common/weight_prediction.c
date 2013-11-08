

#include "common.h"




spixel weight_bidir_y(x265_t *h,
					int32_t i_w0,
					spixel i_p0,
					int32_t i_w1,
					spixel i_p1,
					int32_t i_round,
					int32_t i_shift,
					int32_t i_offset)
{
	return x265_clip_y(h, ((i_w0 * (i_p0 + X265_IF_INTERNAL_OFFS)
							+ i_w1 * (i_p1 + X265_IF_INTERNAL_OFFS)
							+ i_round + (i_offset << (i_shift - 1))) >> i_shift ) );
}

spixel weight_bidir_c(x265_t *h,
					int32_t i_w0,
					spixel i_p0,
					int32_t i_w1,
					spixel i_p1,
					int32_t i_round,
					int32_t i_shift,
					int32_t i_offset)
{
	return x265_clip_c(h, ((i_w0 * (i_p0 + X265_IF_INTERNAL_OFFS)
							+ i_w1 * (i_p1 + X265_IF_INTERNAL_OFFS)
							+ i_round + (i_offset << (i_shift - 1))) >> i_shift ) );
}

spixel weight_unidir_y(x265_t *h,
					int32_t i_w0,
					spixel i_p0,
					int32_t i_round,
					int32_t i_shift,
					int32_t i_offset)
{
	return x265_clip_y(h, ((i_w0 * (i_p0 + X265_IF_INTERNAL_OFFS)
							+ i_round) >> i_shift ) + i_offset );
}

spixel weight_unidir_c(x265_t *h,
					int32_t i_w0,
					spixel i_p0,
					int32_t i_round,
					int32_t i_shift,
					int32_t i_offset)
{
	return x265_clip_c(h, ((i_w0 * (i_p0 + X265_IF_INTERNAL_OFFS)
							+ i_round) >> i_shift ) + i_offset );
}





void x265_weight_prediction_delete ( x265_weight_prediction_t *weight_prediction )
{
	x265_free ( weight_prediction ) ;
}

x265_weight_prediction_t *x265_weight_prediction_new ()
{
	x265_weight_prediction_t *weight_prediction = NULL ;

	CHECKED_MALLOCZERO( weight_prediction, sizeof(x265_weight_prediction_t) );

	return weight_prediction ;
fail:
	x265_weight_prediction_delete ( (void*) weight_prediction ) ;
	return NULL ;
}

int x265_weight_prediction_init ( x265_weight_prediction_t *weight_prediction )
{

    return 0 ;
}

void x265_weight_prediction_deinit ( x265_weight_prediction_t *weight_prediction )
{

}


void x265_weight_prediction_add_weight_bi(x265_t *h,
										x265_weight_prediction_t *weight_prediction,
										x265_simage_t *p_image_src0,
										x265_simage_t *p_image_src1,
										uint32_t i_part_unit_idx,
										uint32_t i_width,
										uint32_t i_height,
										x265_wp_scaling_param_t *wp0,
										x265_wp_scaling_param_t *wp1,
										x265_image_t *p_image_dst,
										int32_t b_round )
{
	int32_t x, y;
	spixel *p_src_y0 = NULL;
	spixel *p_src_u0 = NULL;
	spixel *p_src_v0 = NULL;
	spixel *p_src_y1 = NULL;
	spixel *p_src_u1 = NULL;
	spixel *p_src_v1 = NULL;
	pixel *p_dst_y = NULL;
	pixel *p_dst_u = NULL;
	pixel *p_dst_v = NULL;
	// luma : --------------------------------------------
	int32_t i_w0 = 0;
	int32_t i_offset = 0;
	int32_t i_shift_num = 0;
	int32_t i_shift = 0;
	int32_t i_round = 0;
	int32_t i_w1 = 0;
	uint32_t i_src0_stride = 0;
	uint32_t i_src1_stride = 0;
	uint32_t i_dst_stride = 0;


	p_src_y0 = x265_simage_get_luma_addr_p3(h, p_image_src0, i_part_unit_idx );
	p_src_u0 = x265_simage_get_cb_addr_p3(h, p_image_src0, i_part_unit_idx );
	p_src_v0 = x265_simage_get_cr_addr_p3(h, p_image_src0, i_part_unit_idx );
	p_src_y1 = x265_simage_get_luma_addr_p3(h, p_image_src1, i_part_unit_idx );
	p_src_u1 = x265_simage_get_cb_addr_p3(h, p_image_src1, i_part_unit_idx );
	p_src_v1 = x265_simage_get_cr_addr_p3(h, p_image_src1, i_part_unit_idx );
	p_dst_y = x265_image_get_luma_addr_p3(h, p_image_dst, i_part_unit_idx );
	p_dst_u = x265_image_get_cb_addr_p3(h, p_image_dst, i_part_unit_idx );
	p_dst_v = x265_image_get_cr_addr_p3(h, p_image_dst, i_part_unit_idx );

	// luma : --------------------------------------------
	i_w0 = wp0[0].w;
	i_offset = wp0[0].i_offset;
	i_shift_num = X265_IF_INTERNAL_PREC - h->param.sps.i_bit_depth_y;
	i_shift = wp0[0].shift + i_shift_num;
	i_round = i_shift ? (1 << (i_shift - 1)) * b_round:0;
	i_w1 = wp1[0].w;

	i_src0_stride = x265_simage_get_stride(p_image_src0);
	i_src1_stride = x265_simage_get_stride(p_image_src1);
	i_dst_stride  = x265_image_get_stride(p_image_dst);

	for ( y = i_height-1; y >= 0; y-- )
	{
		for ( x = i_width-1; x >= 0; )
		{
			// note: luma min width is 4
			p_dst_y[x] = weight_bidir_y(h,
										i_w0,
										p_src_y0[x],
										i_w1,
										p_src_y1[x],
										i_round,
										i_shift,
										i_offset);
			x--;
			p_dst_y[x] = weight_bidir_y(h,
										i_w0,
										p_src_y0[x],
										i_w1,
										p_src_y1[x],
										i_round,
										i_shift,
										i_offset);
			x--;
			p_dst_y[x] = weight_bidir_y(h,
										i_w0,
										p_src_y0[x],
										i_w1,
										p_src_y1[x],
										i_round,
										i_shift,
										i_offset);
			x--;
			p_dst_y[x] = weight_bidir_y(h,
										i_w0,
										p_src_y0[x],
										i_w1,
										p_src_y1[x],
										i_round,
										i_shift,
										i_offset);
			x--;
		}
		p_src_y0 += i_src0_stride;
		p_src_y1 += i_src1_stride;
		p_dst_y  += i_dst_stride;
	}


	// chroma u : --------------------------------------------
	i_w0 = wp0[1].w;
	i_offset = wp0[1].i_offset;
	i_shift_num = X265_IF_INTERNAL_PREC - h->param.sps.i_bit_depth_c;
	i_shift = wp0[1].shift + i_shift_num;
	i_round = i_shift ? (1 << (i_shift - 1)) : 0;
	i_w1 = wp1[1].w;

	i_src0_stride = x265_simage_get_c_stride(p_image_src0);
	i_src1_stride = x265_simage_get_c_stride(p_image_src1);
	i_dst_stride = x265_image_get_c_stride(p_image_dst);

	i_width  >>=1;
	i_height >>=1;

	for ( y = i_height-1; y >= 0; y-- )
	{
		for ( x = i_width-1; x >= 0; )
		{
			// note: chroma min width is 2
			p_dst_u[x] = weight_bidir_c(h,
										i_w0,
										p_src_u0[x],
										i_w1,
										p_src_u1[x],
										i_round,
										i_shift,
										i_offset);
			x--;
			p_dst_u[x] = weight_bidir_c(h,
										i_w0,
										p_src_u0[x],
										i_w1,
										p_src_u1[x],
										i_round,
										i_shift,
										i_offset);
			x--;
		}
		p_src_u0 += i_src0_stride;
		p_src_u1 += i_src1_stride;
		p_dst_u  += i_dst_stride;
	}

	// chroma v : --------------------------------------------
	i_w0 = wp0[2].w;
	i_offset = wp0[2].i_offset;
	i_shift = wp0[2].shift + i_shift_num;
	i_round = i_shift ? (1 << (i_shift - 1)) : 0;
	i_w1 = wp1[2].w;

	for ( y = i_height-1; y >= 0; y-- )
	{
		for ( x = i_width-1; x >= 0; )
		{
			// note: chroma min width is 2
			p_dst_v[x] = weight_bidir_c(h,
										i_w0,
										p_src_v0[x],
										i_w1,
										p_src_v1[x],
										i_round,
										i_shift,
										i_offset);
			x--;
			p_dst_v[x] = weight_bidir_c(h,
										i_w0,
										p_src_v0[x],
										i_w1,
										p_src_v1[x],
										i_round,
										i_shift,
										i_offset);
			x--;
		}
		p_src_v0 += i_src0_stride;
		p_src_v1 += i_src1_stride;
		p_dst_v  += i_dst_stride;
	}
}


void x265_weight_prediction_add_weight_uni(x265_t *h,
											x265_weight_prediction_t *weight_prediction,
											x265_simage_t *p_image_src0,
											uint32_t i_part_unit_idx,
											uint32_t i_width,
											uint32_t i_height,
											x265_wp_scaling_param_t *wp0,
											x265_image_t* p_image_dst )
{
	int32_t x, y;
	spixel *p_src_y0 = NULL;
	spixel *p_src_u0 = NULL;
	spixel *p_src_v0 = NULL;
	pixel *p_dst_y = NULL;
	pixel *p_dst_u = NULL;
	pixel *p_dst_v = NULL;
	// luma : --------------------------------------------
	int32_t i_w0 = 0;
	int32_t i_offset = 0;
	int32_t i_shift_num = 0;
	int32_t i_shift = 0;
	int32_t i_round = 0;
	uint32_t i_src0_stride = 0;
	uint32_t i_dst_stride = 0;


	p_src_y0 = x265_simage_get_luma_addr_p3(h, p_image_src0, i_part_unit_idx );
	p_src_u0 = x265_simage_get_cb_addr_p3(h, p_image_src0, i_part_unit_idx );
	p_src_v0 = x265_simage_get_cr_addr_p3(h, p_image_src0, i_part_unit_idx );
	p_dst_y = x265_image_get_luma_addr_p3(h, p_image_dst, i_part_unit_idx );
	p_dst_u = x265_image_get_cb_addr_p3(h, p_image_dst, i_part_unit_idx );
	p_dst_v = x265_image_get_cr_addr_p3(h, p_image_dst, i_part_unit_idx );

	// luma : --------------------------------------------
	i_w0 = wp0[0].w;
	i_offset  = wp0[0].i_offset;
	i_shift_num = X265_IF_INTERNAL_PREC - h->param.sps.i_bit_depth_y;
	i_shift = wp0[0].shift + i_shift_num;
	i_round = i_shift ? (1 << (i_shift - 1)) : 0;
	i_src0_stride = x265_simage_get_stride(p_image_src0);
	i_dst_stride = x265_image_get_stride(p_image_dst);

	for ( y = i_height-1; y >= 0; y-- )
	{
		for ( x = i_width-1; x >= 0; )
		{
			// note: luma min width is 4
			p_dst_y[x] = weight_unidir_y(h,
										i_w0,
										p_src_y0[x],
										i_round,
										i_shift,
										i_offset);
			x--;
			p_dst_y[x] = weight_unidir_y(h,
										i_w0,
										p_src_y0[x],
										i_round,
										i_shift,
										i_offset);
			x--;
			p_dst_y[x] = weight_unidir_y(h,
										i_w0,
										p_src_y0[x],
										i_round,
										i_shift,
										i_offset);
			x--;
			p_dst_y[x] = weight_unidir_y(h,
										i_w0,
										p_src_y0[x],
										i_round,
										i_shift,
										i_offset);
			x--;
		}
		p_src_y0 += i_src0_stride;
		p_dst_y  += i_dst_stride;
	}

	// chroma u : --------------------------------------------
	i_w0 = wp0[1].w;
	i_offset  = wp0[1].i_offset;
	i_shift_num = X265_IF_INTERNAL_PREC - h->param.sps.i_bit_depth_c;
	i_shift = wp0[1].shift + i_shift_num;
	i_round = i_shift ? (1 << (i_shift - 1)) : 0;

	i_src0_stride = x265_simage_get_c_stride(p_image_src0);
	i_dst_stride = x265_image_get_c_stride(p_image_dst);

	i_width  >>=1;
	i_height >>=1;

	for ( y = i_height-1; y >= 0; y-- )
	{
		for ( x = i_width-1; x >= 0; )
		{
			// note: chroma min width is 2
			p_dst_u[x] = weight_unidir_c(h,
										i_w0,
										p_src_u0[x],
										i_round,
										i_shift,
										i_offset);
			x--;
			p_dst_u[x] = weight_unidir_c(h,
										i_w0,
										p_src_u0[x],
										i_round,
										i_shift,
										i_offset);
			x--;
		}
		p_src_u0 += i_src0_stride;
		p_dst_u  += i_dst_stride;
	}

	// chroma v : --------------------------------------------
	i_w0 = wp0[2].w;
	i_offset = wp0[2].i_offset;
	i_shift = wp0[2].shift + i_shift_num;
	i_round = i_shift ? (1 << (i_shift - 1)) : 0;

	for ( y = i_height-1; y >= 0; y-- )
	{
		for ( x = i_width-1; x >= 0; )
		{
			// note: chroma min width is 2
			p_dst_v[x] = weight_unidir_c(h,
										i_w0,
										p_src_v0[x],
										i_round,
										i_shift,
										i_offset);
			x--;
			p_dst_v[x] = weight_unidir_c(h,
										i_w0,
										p_src_v0[x],
										i_round,
										i_shift,
										i_offset);
			x--;
		}
		p_src_v0 += i_src0_stride;
		p_dst_v += i_dst_stride;
	}
}




//=======================================================
//  get_wp_scaling()
//=======================================================
/** derivation of wp tables
 * \param x265_data_c_ut* pc_c_u
 * \param *p_ref_idx0
 * \param *p_ref_idx1
 * \param x265_wp_scaling_param_t *&wp0
 * \param x265_wp_scaling_param_t *&wp1
 * \param ibdi
 * \returns void
 */
void x265_weight_prediction_get_wp_scaling( x265_t *h,
											x265_weight_prediction_t *weight_prediction,
											x265_data_cu_t* cu,
											int32_t *p_ref_idx0,
											int32_t *p_ref_idx1,
											x265_wp_scaling_param_t **pp_wp0,
											x265_wp_scaling_param_t **pp_wp1 )
{
	x265_slice_t * pc_slice = h->slice ;
	x265_pps_t* pps = &h->pps[0] ;
	int32_t b_wp_bi_pred = pps->b_use_weighted_bi_pred ;
	x265_wp_scaling_param_t* pwp;
	int32_t b_bi_dir = (*p_ref_idx0>=0 && *p_ref_idx1>=0);
	int32_t b_uni_dir = !b_bi_dir;

	if ( b_uni_dir || b_wp_bi_pred )
	{
		// explicit --------------------
		if ( *p_ref_idx0 >= 0 )
		{
			*pp_wp0 = pc_slice->weight_pred_table[REF_PIC_LIST_0][*p_ref_idx0] ;
		}
		if ( *p_ref_idx1 >= 0 )
		{
			*pp_wp1 = pc_slice->weight_pred_table[REF_PIC_LIST_1][*p_ref_idx1] ;
		}
	}
	else
	{
		assert(0);
	}

	if ( *p_ref_idx0 < 0 )
	{
		*pp_wp0 = NULL;
	}
	if ( *p_ref_idx1 < 0 )
	{
		*pp_wp1 = NULL;
	}

	if ( b_bi_dir )
	{
		// bi-dir case
		for ( int32_t yuv=0 ; yuv<3 ; yuv++ )
		{
			int32_t bit_depth = yuv ? h->param.sps.i_bit_depth_c : h->param.sps.i_bit_depth_y;
			(*pp_wp0)[yuv].w      = (*pp_wp0)[yuv].i_weight;
			(*pp_wp0)[yuv].o      = (*pp_wp0)[yuv].i_offset * (1 << (bit_depth-8));
			(*pp_wp1)[yuv].w      = (*pp_wp1)[yuv].i_weight;
			(*pp_wp1)[yuv].o      = (*pp_wp1)[yuv].i_offset * (1 << (bit_depth-8));
			(*pp_wp0)[yuv].offset = (*pp_wp0)[yuv].o + (*pp_wp1)[yuv].o;
			(*pp_wp0)[yuv].shift  = (*pp_wp0)[yuv].i_log2_weight_denom + 1;
			(*pp_wp0)[yuv].i_round  = (1 << (*pp_wp0)[yuv].i_log2_weight_denom);
			(*pp_wp1)[yuv].offset = (*pp_wp0)[yuv].offset;
			(*pp_wp1)[yuv].shift  = (*pp_wp0)[yuv].shift;
			(*pp_wp1)[yuv].i_round  = (*pp_wp0)[yuv].i_round;
		}
	}
	else
	{
		// unidir
		pwp = (*p_ref_idx0>=0) ? *pp_wp0 : *pp_wp1 ;
		for ( int32_t yuv=0 ; yuv<3 ; yuv++ )
		{
			int32_t bit_depth = yuv ? h->param.sps.i_bit_depth_c : h->param.sps.i_bit_depth_y;
			pwp[yuv].w      = pwp[yuv].i_weight;
			pwp[yuv].offset = pwp[yuv].i_offset * (1 << (bit_depth-8));
			pwp[yuv].shift  = pwp[yuv].i_log2_weight_denom;
			pwp[yuv].i_round  = (pwp[yuv].i_log2_weight_denom>=1) ? (1 << (pwp[yuv].i_log2_weight_denom-1)) : (0);
		}
	}
}

void x265_weight_prediction_x_weighted_prediction_bi(x265_t *h,
													x265_weight_prediction_t *weight_prediction,
													x265_data_cu_t *cu,
													x265_simage_t *p_image_src0,
													x265_simage_t *p_image_src1,
													int32_t i_ref_idx0,
													int32_t i_ref_idx1,
													uint32_t i_part_idx,
													int32_t i_width,
													int32_t i_height,
													x265_image_t *p_image_dst )
{
	x265_wp_scaling_param_t *pwp0, *pwp1;
	x265_pps_t *pps = &h->pps[0];

	assert( pps->b_use_weighted_bi_pred);

	x265_weight_prediction_get_wp_scaling(h,
										weight_prediction,
										cu,
										&i_ref_idx0,
										&i_ref_idx1,
										&pwp0,
										&pwp1);

	if( i_ref_idx0 >= 0 && i_ref_idx1 >= 0 )
	{
		x265_weight_prediction_add_weight_bi(h,
											weight_prediction,
											p_image_src0,
											p_image_src1,
											i_part_idx,
											i_width,
											i_height,
											pwp0,
											pwp1,
											p_image_dst,
											1 );
	}
	else if ( i_ref_idx0 >= 0 && i_ref_idx1 <  0 )
	{
		x265_weight_prediction_add_weight_uni(h,
											weight_prediction,
											p_image_src0,
											i_part_idx,
											i_width,
											i_height,
											pwp0,
											p_image_dst );
	}
	else if ( i_ref_idx0 <  0 && i_ref_idx1 >= 0 )
	{
		x265_weight_prediction_add_weight_uni(h,
											weight_prediction,
											p_image_src1,
											i_part_idx,
											i_width,
											i_height,
											pwp1,
											p_image_dst );
	}
	else
	{
		assert (0);
	}
}


void x265_weight_prediction_x_weighted_prediction_uni(x265_t *h,
													x265_weight_prediction_t *weight_prediction,
													x265_data_cu_t *cu,
													x265_simage_t *p_image_src,
													uint32_t i_part_addr,
													int32_t i_width,
													int32_t i_height,
													enum ref_pic_list_e i_ref_pic_list,
													x265_image_t **pp_image_pred,
													int32_t i_ref_idx)
{
	int32_t i_ref_idx1 = 0 ;
	x265_wp_scaling_param_t *pwp, *pwp_tmp;

	i_ref_idx1 = -1 ;
	if ( i_ref_idx < 0 )
	{
		i_ref_idx   = x265_cu_mv_field_get_ref_idx(
				x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu,
													i_ref_pic_list ),
													i_part_addr );
	}
	assert (i_ref_idx >= 0);

	if ( i_ref_pic_list == REF_PIC_LIST_0 )
	{
		x265_weight_prediction_get_wp_scaling(h,
											weight_prediction,
											cu,
											&i_ref_idx,
											&i_ref_idx1,
											&pwp,
											&pwp_tmp);
	}
	else
	{
		x265_weight_prediction_get_wp_scaling(h,
											weight_prediction,
											cu,
											&i_ref_idx1,
											&i_ref_idx,
											&pwp_tmp,
											&pwp);
	}
	x265_weight_prediction_add_weight_uni(h,
										weight_prediction,
										p_image_src,
										i_part_addr,
										i_width,
										i_height,
										pwp,
										*pp_image_pred );
}



