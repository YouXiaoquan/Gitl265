

#include "common.h"


x265_sample_adaptive_offset_class_static_t *p_sample_adaptive_offset_class_static ;


/*
 * const Int TComSampleAdaptiveOffset::m_aiNumCulPartsLevel[5] =
{
  1,   //level 0
  5,   //level 1
  21,  //level 2
  85,  //level 3
  341, //level 4
};

const UInt TComSampleAdaptiveOffset::m_auiEoTable[9] =
{
  1, //0
  2, //1
  0, //2
  3, //3
  4, //4
  0, //5
  0, //6
  0, //7
  0
};

const Int TComSampleAdaptiveOffset::m_iNumClass[MAX_NUM_SAO_TYPE] =
{
  SAO_EO_LEN,
  SAO_EO_LEN,
  SAO_EO_LEN,
  SAO_EO_LEN,
  SAO_BO_LEN
};

const UInt TComSampleAdaptiveOffset::m_uiMaxDepth = SAO_MAX_DEPTH;
 */
int x265_sample_adaptive_offset_class_static_init ()
{
	CHECKED_MALLOCZERO( p_sample_adaptive_offset_class_static, sizeof(x265_sample_adaptive_offset_class_static_t) );

	p_sample_adaptive_offset_class_static->num_cul_parts_level[0] = 1 ;
	p_sample_adaptive_offset_class_static->num_cul_parts_level[1] = 5 ;
	p_sample_adaptive_offset_class_static->num_cul_parts_level[2] = 21 ;
	p_sample_adaptive_offset_class_static->num_cul_parts_level[3] = 85 ;
	p_sample_adaptive_offset_class_static->num_cul_parts_level[4] = 341 ;

	p_sample_adaptive_offset_class_static->eo_table[0] = 1 ;
	p_sample_adaptive_offset_class_static->eo_table[1] = 2 ;
	p_sample_adaptive_offset_class_static->eo_table[2] = 0 ;
	p_sample_adaptive_offset_class_static->eo_table[3] = 3 ;
	p_sample_adaptive_offset_class_static->eo_table[4] = 4 ;
	p_sample_adaptive_offset_class_static->eo_table[5] = 0 ;
	p_sample_adaptive_offset_class_static->eo_table[6] = 0 ;
	p_sample_adaptive_offset_class_static->eo_table[7] = 0 ;
	p_sample_adaptive_offset_class_static->eo_table[8] = 0 ;

	p_sample_adaptive_offset_class_static->num_class[0] = SAO_EO_LEN ;
	p_sample_adaptive_offset_class_static->num_class[1] = SAO_EO_LEN ;
	p_sample_adaptive_offset_class_static->num_class[2] = SAO_EO_LEN ;
	p_sample_adaptive_offset_class_static->num_class[3] = SAO_EO_LEN ;
	p_sample_adaptive_offset_class_static->num_class[4] = SAO_BO_LEN ;

	p_sample_adaptive_offset_class_static->i_max_depth = X265_SAO_MAX_DEPTH ;
	return 0 ;

fail:
	x265_sample_adaptive_offset_class_static_deinit () ;
	return -1 ;
}

void x265_sample_adaptive_offset_class_static_deinit ()
{
	x265_free ( p_sample_adaptive_offset_class_static ) ;
}


void x265_sample_adaptive_offset_set_sao_lcu_boundary(x265_sample_adaptive_offset_t *sample_adaptive_offset,
														int32_t b_val)
{
	sample_adaptive_offset->b_sao_lcu_boundary = b_val;
}

int32_t x265_sample_adaptive_offset_get_sao_lcu_boundary(x265_sample_adaptive_offset_t *sample_adaptive_offset)
{
	return sample_adaptive_offset->b_sao_lcu_boundary;
}

void x265_sample_adaptive_offset_set_sao_lcu_based_optimization(x265_sample_adaptive_offset_t *sample_adaptive_offset,
																int32_t b_val)
{
	sample_adaptive_offset->b_sao_lcu_based_optimization = b_val;
}


int32_t x265_sample_adaptive_offset_get_sao_lcu_based_optimization(x265_sample_adaptive_offset_t *sample_adaptive_offset)
{
	return sample_adaptive_offset->b_sao_lcu_based_optimization;
}



x265_sample_adaptive_offset_t *x265_sample_adaptive_offset_new ()
{
	x265_sample_adaptive_offset_t *sample_adaptive_offset = NULL ;

	CHECKED_MALLOCZERO( sample_adaptive_offset, sizeof(x265_sample_adaptive_offset_t) );

	return sample_adaptive_offset ;
fail:
	x265_sample_adaptive_offset_delete ( (void*) sample_adaptive_offset ) ;
	return NULL ;
}

void x265_sample_adaptive_offset_delete ( x265_sample_adaptive_offset_t *sample_adaptive_offset )
{
	x265_free ( sample_adaptive_offset ) ;
}

int x265_sample_adaptive_offset_init ( x265_sample_adaptive_offset_t *sample_adaptive_offset )
{
	sample_adaptive_offset->offset_bo = NULL ;
	sample_adaptive_offset->offset_bo_y = NULL ;
	sample_adaptive_offset->offset_bo_cb = NULL ;
	sample_adaptive_offset->offset_bo_cr = NULL ;
	sample_adaptive_offset->offset_eo = 0 ;
	sample_adaptive_offset->i_pic_width = 0 ;
	sample_adaptive_offset->i_pic_height = 0 ;
	sample_adaptive_offset->i_max_cu_width = 0 ;
	sample_adaptive_offset->i_max_cu_height = 0 ;
	sample_adaptive_offset->i_num_cu_in_width = 0 ;
	sample_adaptive_offset->i_num_cu_in_height = 0 ;
	sample_adaptive_offset->i_num_total_parts = 0 ;
	sample_adaptive_offset->i_num_cu_in_width_mul_two  = 0 ;
	sample_adaptive_offset->i_sao_bit_increase_y = 0 ;
	sample_adaptive_offset->i_sao_bit_increase_c = 0 ;
	sample_adaptive_offset->i_qp = 0 ;
	sample_adaptive_offset->clip_table = NULL ;
	sample_adaptive_offset->clip_table_base = NULL ;
	sample_adaptive_offset->luma_table_bo = NULL ;
	sample_adaptive_offset->chroma_clip_table = NULL ;
	sample_adaptive_offset->chroma_clip_table_base = NULL ;
	sample_adaptive_offset->chroma_table_bo = NULL ;
	sample_adaptive_offset->up_buff_1 = NULL ;
	sample_adaptive_offset->up_buff_2 = NULL ;
	sample_adaptive_offset->up_buff = NULL ;
	sample_adaptive_offset->swap = NULL ;
	sample_adaptive_offset->b_use_nif = 0 ;
	sample_adaptive_offset->frame_tmp = NULL ;
	sample_adaptive_offset->tmp_u1 = NULL ;
	sample_adaptive_offset->tmp_u2 = NULL ;
	sample_adaptive_offset->tmp_l1 = NULL ;
	sample_adaptive_offset->tmp_l2 = NULL ;
	sample_adaptive_offset->tmp_u1_y = NULL ;
	sample_adaptive_offset->tmp_u2_y = NULL ;
	sample_adaptive_offset->tmp_l1_y = NULL ;
	sample_adaptive_offset->tmp_l2_y = NULL ;
	sample_adaptive_offset->tmp_u1_cb = NULL ;
	sample_adaptive_offset->tmp_u2_cb = NULL ;
	sample_adaptive_offset->tmp_l1_cb = NULL ;
	sample_adaptive_offset->tmp_l2_cb = NULL ;
	sample_adaptive_offset->tmp_u1_cr = NULL ;
	sample_adaptive_offset->tmp_u2_cr = NULL ;
	sample_adaptive_offset->tmp_l1_cr = NULL ;
	sample_adaptive_offset->tmp_l2_cr = NULL ;
	sample_adaptive_offset->i_max_num_offsets_per_pic = 0 ;
	sample_adaptive_offset->b_sao_lcu_boundary = 0 ;
	sample_adaptive_offset->b_sao_lcu_based_optimization = 0 ;

	return 0 ;
}

void x265_sample_adaptive_offset_deinit ( x265_sample_adaptive_offset_t *sample_adaptive_offset )
{

}

int x265_sample_adaptive_offset_create ( x265_t *h, x265_sample_adaptive_offset_t *sample_adaptive_offset )
{
	int32_t loop = 0;
	uint32_t i_pixel_range_y = 0;
	uint32_t i_bo_range_shift_y = 0;
	uint32_t i_pixel_range_c = 0;
	uint32_t i_bo_range_shift_c = 0;
	uint32_t i_max_y = 0;
	uint32_t i_min_y = 0;
	int32_t i_c_range_ext = 0;
	uint32_t i_max_c = 0;
	uint32_t i_min_c = 0;
	int32_t i_c_range_ext_c = 0;




	sample_adaptive_offset->i_pic_width = h->param.i_width ;
	sample_adaptive_offset->i_pic_height = h->param.i_height ;
	sample_adaptive_offset->i_max_cu_width = h->param.sps.i_max_cu_width ;
	sample_adaptive_offset->i_max_cu_height = h->param.sps.i_max_cu_height ;

	sample_adaptive_offset->i_num_cu_in_width = sample_adaptive_offset->i_pic_width / sample_adaptive_offset->i_max_cu_width ;
	sample_adaptive_offset->i_num_cu_in_width += (sample_adaptive_offset->i_pic_width % sample_adaptive_offset->i_max_cu_width) ? 1 : 0 ;

	sample_adaptive_offset->i_num_cu_in_height = sample_adaptive_offset->i_pic_height / sample_adaptive_offset->i_max_cu_height ;
	sample_adaptive_offset->i_num_cu_in_height += (sample_adaptive_offset->i_pic_height % sample_adaptive_offset->i_max_cu_height) ? 1 : 0 ;

	sample_adaptive_offset->i_num_cu_in_width_mul_two = sample_adaptive_offset->i_num_cu_in_width * 2 ;
	sample_adaptive_offset->i_num_total_parts = 3 ;


	i_pixel_range_y = 1 << h->param.sps.i_bit_depth_y;
	i_bo_range_shift_y = h->param.sps.i_bit_depth_y - X265_SAO_BO_BITS;

	CHECKED_MALLOCZERO(sample_adaptive_offset->luma_table_bo,
						sizeof(pixel) * i_pixel_range_y);
	for (loop=0; loop < i_pixel_range_y; ++loop )
	{
		sample_adaptive_offset->luma_table_bo[loop] = 1 + (loop >> i_bo_range_shift_y);
	}

	i_pixel_range_c = 1 << h->param.sps.i_bit_depth_c;
	i_bo_range_shift_c = h->param.sps.i_bit_depth_c - X265_SAO_BO_BITS;

	CHECKED_MALLOCZERO(sample_adaptive_offset->chroma_table_bo,
						sizeof(pixel) * i_pixel_range_c);
	for (loop=0; loop < i_pixel_range_c; ++loop )
	{
		sample_adaptive_offset->chroma_table_bo[loop] = 1 + (loop >> i_bo_range_shift_c);
	}
	CHECKED_MALLOCZERO(sample_adaptive_offset->up_buff_1,
						sizeof(int32_t) * (sample_adaptive_offset->i_pic_width + 2));
	CHECKED_MALLOCZERO(sample_adaptive_offset->up_buff_2,
						sizeof(int32_t) * (sample_adaptive_offset->i_pic_width + 2));
	CHECKED_MALLOCZERO(sample_adaptive_offset->up_buff,
						sizeof(int32_t) * (sample_adaptive_offset->i_pic_width + 2));
	sample_adaptive_offset->up_buff_1 ++;
	sample_adaptive_offset->up_buff_2 ++;
	sample_adaptive_offset->up_buff ++;


	i_max_y  = (1 << h->param.sps.i_bit_depth_y) - 1;;
	i_min_y  = 0;
	i_c_range_ext = i_max_y>>1;

	CHECKED_MALLOCZERO(sample_adaptive_offset->clip_table_base,
						sizeof(pixel) * (i_max_y + 2 * i_c_range_ext));
	CHECKED_MALLOCZERO(sample_adaptive_offset->offset_bo_y,
						sizeof(int32_t) * i_max_y + 2 * i_c_range_ext);

	for( loop = 0 ; loop < (i_min_y + i_c_range_ext) ; ++loop )
	{
		sample_adaptive_offset->clip_table_base[loop] = i_min_y;
	}

	for( loop = i_min_y + i_c_range_ext ; loop < (i_max_y + i_c_range_ext) ; ++ loop)
	{
		sample_adaptive_offset->clip_table_base[loop] = loop - i_c_range_ext;
	}

	for( loop = i_max_y + i_c_range_ext ; loop < (i_max_y + 2*i_c_range_ext) ; ++loop )
	{
		sample_adaptive_offset->clip_table_base[loop] = i_max_y;
	}

	sample_adaptive_offset->clip_table = &(sample_adaptive_offset->clip_table_base[i_c_range_ext]);

	i_max_c  = (1 << h->param.sps.i_bit_depth_c) - 1;
	i_min_c  = 0;
	i_c_range_ext_c = i_max_c >> 1;

	CHECKED_MALLOCZERO(sample_adaptive_offset->chroma_clip_table_base,
						sizeof(pixel) * (i_max_c + 2 * i_c_range_ext_c));
	CHECKED_MALLOCZERO(sample_adaptive_offset->offset_bo_cb,
						sizeof(int32_t) * (i_max_c + 2 * i_c_range_ext_c));
	CHECKED_MALLOCZERO(sample_adaptive_offset->offset_bo_cr,
						sizeof(int32_t) * (i_max_c + 2 * i_c_range_ext_c));

	for( loop = 0 ; loop < (i_min_c+i_c_range_ext_c) ; ++loop )
	{
		sample_adaptive_offset->chroma_clip_table_base[loop] = i_min_c;
	}

	for( loop = i_min_c + i_c_range_ext_c ; loop < (i_max_c + i_c_range_ext_c) ; ++loop )
	{
		sample_adaptive_offset->chroma_clip_table_base[loop] = loop - i_c_range_ext_c;
	}

	for( loop = i_max_c + i_c_range_ext_c ; loop < (i_max_c + 2 * i_c_range_ext_c) ; ++ loop)
	{
		sample_adaptive_offset->chroma_clip_table_base[loop] = i_max_c;
	}

	sample_adaptive_offset->chroma_clip_table = &(sample_adaptive_offset->chroma_clip_table_base[i_c_range_ext_c]);


	CHECKED_MALLOCZERO(sample_adaptive_offset->tmp_l1_y,
						sizeof(pixel) * (sample_adaptive_offset->i_max_cu_height + 1));
	CHECKED_MALLOCZERO(sample_adaptive_offset->tmp_l2_y,
						sizeof(pixel) * (sample_adaptive_offset->i_max_cu_height + 1));
	CHECKED_MALLOCZERO(sample_adaptive_offset->tmp_u1_y,
						sizeof(pixel) * (sample_adaptive_offset->i_max_cu_width * sample_adaptive_offset->i_num_cu_in_width));
	CHECKED_MALLOCZERO(sample_adaptive_offset->tmp_u2_y,
						sizeof(pixel) * (sample_adaptive_offset->i_max_cu_width * sample_adaptive_offset->i_num_cu_in_width));

	CHECKED_MALLOCZERO(sample_adaptive_offset->tmp_l1_cb,
						sizeof(pixel) * (sample_adaptive_offset->i_max_cu_height + 1));
	CHECKED_MALLOCZERO(sample_adaptive_offset->tmp_l2_cb,
						sizeof(pixel) * (sample_adaptive_offset->i_max_cu_height + 1));
	CHECKED_MALLOCZERO(sample_adaptive_offset->tmp_u1_cb,
						sizeof(pixel) * (sample_adaptive_offset->i_max_cu_width * sample_adaptive_offset->i_num_cu_in_width));
	CHECKED_MALLOCZERO(sample_adaptive_offset->tmp_u2_cb,
						sizeof(pixel) * (sample_adaptive_offset->i_max_cu_width * sample_adaptive_offset->i_num_cu_in_width));

	CHECKED_MALLOCZERO(sample_adaptive_offset->tmp_l1_cr,
						sizeof(pixel) * (sample_adaptive_offset->i_max_cu_height + 1));
	CHECKED_MALLOCZERO(sample_adaptive_offset->tmp_l2_cr,
						sizeof(pixel) * (sample_adaptive_offset->i_max_cu_height + 1));
	CHECKED_MALLOCZERO(sample_adaptive_offset->tmp_u1_cr,
						sizeof(pixel) * (sample_adaptive_offset->i_max_cu_width * sample_adaptive_offset->i_num_cu_in_width));
	CHECKED_MALLOCZERO(sample_adaptive_offset->tmp_u2_cr,
						sizeof(pixel) * (sample_adaptive_offset->i_max_cu_width * sample_adaptive_offset->i_num_cu_in_width));

	return 0 ;

fail:
	x265_sample_adaptive_offset_destroy(h, sample_adaptive_offset) ;
	return -1 ;
}

void x265_sample_adaptive_offset_destroy ( x265_t *h,
											x265_sample_adaptive_offset_t *sample_adaptive_offset )
{
	x265_free (sample_adaptive_offset->tmp_u2_cr) ;
	sample_adaptive_offset->tmp_u2_cr = NULL ;
	x265_free (sample_adaptive_offset->tmp_u1_cr) ;
	sample_adaptive_offset->tmp_u1_cr = NULL ;
	x265_free (sample_adaptive_offset->tmp_l2_cr) ;
	sample_adaptive_offset->tmp_l2_cr = NULL ;
	x265_free (sample_adaptive_offset->tmp_l1_cr) ;
	sample_adaptive_offset->tmp_l1_cr = NULL ;

	x265_free (sample_adaptive_offset->tmp_u2_cb) ;
	sample_adaptive_offset->tmp_u2_cb = NULL ;
	x265_free (sample_adaptive_offset->tmp_u1_cb) ;
	sample_adaptive_offset->tmp_u1_cb = NULL ;
	x265_free (sample_adaptive_offset->tmp_l2_cb) ;
	sample_adaptive_offset->tmp_l2_cb = NULL ;
	x265_free (sample_adaptive_offset->tmp_l1_cb) ;
	sample_adaptive_offset->tmp_l1_cb = NULL ;

	x265_free (sample_adaptive_offset->tmp_u2_y) ;
	sample_adaptive_offset->tmp_u2_y = NULL ;
	x265_free (sample_adaptive_offset->tmp_u1_y) ;
	sample_adaptive_offset->tmp_u1_y = NULL ;
	x265_free (sample_adaptive_offset->tmp_l2_y) ;
	sample_adaptive_offset->tmp_l2_y = NULL ;
	x265_free (sample_adaptive_offset->tmp_l1_y) ;
	sample_adaptive_offset->tmp_l1_y = NULL ;

	x265_free (sample_adaptive_offset->offset_bo_cr) ;
	sample_adaptive_offset->offset_bo_cr = NULL ;
	x265_free (sample_adaptive_offset->offset_bo_cb) ;
	sample_adaptive_offset->offset_bo_cb = NULL ;
	x265_free (sample_adaptive_offset->chroma_clip_table_base) ;
	sample_adaptive_offset->chroma_clip_table_base = NULL ;
	sample_adaptive_offset->chroma_clip_table = NULL ;

	x265_free (sample_adaptive_offset->offset_bo_y) ;
	sample_adaptive_offset->offset_bo_y = NULL ;
	x265_free (sample_adaptive_offset->clip_table_base) ;
	sample_adaptive_offset->clip_table_base = NULL ;
	sample_adaptive_offset->clip_table = NULL ;

	if (sample_adaptive_offset->up_buff)
	{
		sample_adaptive_offset->up_buff -- ;
		x265_free (sample_adaptive_offset->up_buff) ;
		sample_adaptive_offset->up_buff = NULL ;
	}
	if (sample_adaptive_offset->up_buff_2)
	{
		sample_adaptive_offset->up_buff_2 -- ;
		x265_free (sample_adaptive_offset->up_buff_2) ;
		sample_adaptive_offset->up_buff_2 = NULL ;
	}

	if (sample_adaptive_offset->up_buff_1)
	{
		sample_adaptive_offset->up_buff_1 -- ;
		x265_free (sample_adaptive_offset->up_buff_1) ;
		sample_adaptive_offset->up_buff_1 = NULL ;
	}

	x265_free (sample_adaptive_offset->chroma_table_bo) ;
	sample_adaptive_offset->chroma_table_bo = NULL ;
	x265_free (sample_adaptive_offset->luma_table_bo) ;
	sample_adaptive_offset->luma_table_bo = NULL ;
}

int x265_sample_adaptive_offset_alloc_sao_param ( x265_t *h,
													x265_sample_adaptive_offset_t *sample_adaptive_offset,
													x265_sao_param_t *sao_param )
{
	memset ( sao_param, 0, sizeof (x265_sao_param_t) ) ;

	sao_param->i_num_cu_in_width = h->cu.pic.i_width_in_cu ;
	sao_param->i_num_cu_in_height = h->cu.pic.i_height_in_cu ;
	sao_param->i_num_cu_in_width_mul_two = h->cu.pic.i_width_in_cu_mul_two ;

	CHECKED_MALLOCZERO ( sao_param->sao_lcu_param[0],
						sao_param->i_num_cu_in_width_mul_two * sizeof (x265_sao_lcu_param_t) ) ;
	CHECKED_MALLOCZERO ( sao_param->sao_lcu_param[1],
						sao_param->i_num_cu_in_width_mul_two * sizeof (x265_sao_lcu_param_t) ) ;
	CHECKED_MALLOCZERO ( sao_param->sao_lcu_param[2],
						sao_param->i_num_cu_in_width_mul_two * sizeof (x265_sao_lcu_param_t) ) ;

	return 0 ;

fail:
	x265_sample_adaptive_offset_free_sao_param ( sample_adaptive_offset, sao_param ) ;
	return -1 ;
}


void x265_sample_adaptive_offset_free_sao_param ( x265_sample_adaptive_offset_t *sample_adaptive_offset,
												x265_sao_param_t *sao_param )
{
	x265_free ( sao_param->sao_lcu_param[0] ) ;
	x265_free ( sao_param->sao_lcu_param[1] ) ;
	x265_free ( sao_param->sao_lcu_param[2] ) ;

	sao_param->sao_lcu_param[0] = NULL ;
	sao_param->sao_lcu_param[1] = NULL ;
	sao_param->sao_lcu_param[2] = NULL ;
}

void x265_sample_adaptive_offset_reset_sao_param(x265_sample_adaptive_offset_t *sample_adaptive_offset,
												x265_sao_param_t *sao_param)
{
	int32_t loop = 0;
	int32_t i_num_componet = 0;

	i_num_componet = 3;
	for(loop = 0 ; loop < i_num_componet; ++ loop )
	{
		if (loop < 2)
		{
			sao_param->b_sao_flag[loop] = 0;
		}
		sao_param->b_one_unit_flag[0]   = 0;
		sao_param->b_one_unit_flag[1]   = 0;
		sao_param->b_one_unit_flag[2]   = 0;
		x265_sample_adaptive_offset_reset_lcu_part(sample_adaptive_offset, sao_param->sao_lcu_param[0]);
		x265_sample_adaptive_offset_reset_lcu_part(sample_adaptive_offset, sao_param->sao_lcu_param[1]);
		x265_sample_adaptive_offset_reset_lcu_part(sample_adaptive_offset, sao_param->sao_lcu_param[2]);
	}
}

/** get the sign of input variable
 * \param   x
 */
int32_t x265_sample_adaptive_offset_x_sign(int32_t x)
{
	return ((x >> 31) | ((int32_t)( (((uint32_t) -x)) >> 31)));
}

/** sample adaptive offset process for one lcu crossing lcu boundary
 * \param   i_addr, i_sao_type, i_y_cb_cr
 */
void x265_sample_adaptive_offset_process_sao_cu_org(x265_t *h,
													x265_sample_adaptive_offset_t *sample_adaptive_offset,
													uint32_t i_cu_addr,
													int32_t i_sao_type,
													int32_t i_y_cb_cr)
{
	int32_t x = 0, y = 0;
	int32_t i_stride = 0;
	int32_t i_lcu_width = 0;
	int32_t i_lcu_height = 0;
	uint32_t i_l_pel_x = 0;
	uint32_t i_t_pel_y = 0;
	uint32_t i_r_pel_x = 0;
	uint32_t i_b_pel_y = 0;
	int32_t i_sign_left = 0;
	int32_t i_sign_right = 0;
	int32_t i_sign_down = 0;
	int32_t i_sign_down1 = 0;
	int32_t i_sign_down2 = 0;
	uint32_t i_edge_type = 0;
	int32_t i_pic_width_tmp = 0;
	int32_t i_pic_height_tmp = 0;
	int32_t i_start_x = 0;
	int32_t i_start_y = 0;
	int32_t i_end_x = 0;
	int32_t i_end_y = 0;
	int32_t i_is_chroma = 0;
	int32_t i_shift = 0;
	int32_t i_cu_height_tmp = 0;
	pixel *p_rec = NULL;
	pixel *p_tmp_l_swap = NULL;
	pixel *p_tmp_l = NULL;
	pixel *p_tmp_u = NULL;
	pixel *p_clip_tbl = NULL;
	int32_t *p_offset_bo = NULL;

	i_lcu_width = sample_adaptive_offset->i_max_cu_width;
	i_lcu_height = sample_adaptive_offset->i_max_cu_height;
	i_l_pel_x = x265_base_data_cu_get_cu_pel_x_by_cu_addr(h, i_cu_addr);
	i_t_pel_y = x265_base_data_cu_get_cu_pel_y_by_cu_addr(h, i_cu_addr);
	i_is_chroma = (i_y_cb_cr!=0)? 1:0;

	i_pic_width_tmp  = sample_adaptive_offset->i_pic_width  >> i_is_chroma;
	i_pic_height_tmp = sample_adaptive_offset->i_pic_height >> i_is_chroma;
	i_lcu_width     = i_lcu_width    >> i_is_chroma;
	i_lcu_height    = i_lcu_height   >> i_is_chroma;
	i_l_pel_x       = i_l_pel_x      >> i_is_chroma;
	i_t_pel_y       = i_t_pel_y      >> i_is_chroma;
	i_r_pel_x       = i_l_pel_x + i_lcu_width  ;
	i_b_pel_y       = i_t_pel_y + i_lcu_height ;
	i_r_pel_x       = i_r_pel_x > i_pic_width_tmp  ? i_pic_width_tmp  : i_r_pel_x;
	i_b_pel_y       = i_b_pel_y > i_pic_height_tmp ? i_pic_height_tmp : i_b_pel_y;
	i_lcu_width     = i_r_pel_x - i_l_pel_x;
	i_lcu_height    = i_b_pel_y - i_t_pel_y;

	if (i_y_cb_cr == 0)
	{
		p_rec       = x265_frame_get_luma_addr_p3(h, h->fdec, i_cu_addr);
		i_stride    = h->fdec->i_stride[0];
	}
	else if (i_y_cb_cr == 1)
	{
		p_rec       = x265_frame_get_cb_addr_p3(h, h->fdec, i_cu_addr);
		i_stride    = h->fdec->i_stride[1];
	}
	else
	{
		p_rec       = x265_frame_get_cr_addr_p3(h, h->fdec, i_cu_addr);
		i_stride    = h->fdec->i_stride[1];
	}

	//   if (i_sao_type!=sao_bo_0 || i_sao_type!=sao_bo_1)
	{
		i_cu_height_tmp = (sample_adaptive_offset->i_max_cu_height >> i_is_chroma);
		i_shift = (sample_adaptive_offset->i_max_cu_width>> i_is_chroma)-1;
		for (int32_t i=0;i<i_cu_height_tmp+1;i++)
		{
		  sample_adaptive_offset->tmp_l2[i] = p_rec[i_shift];
		  p_rec += i_stride;
		}
		p_rec -= (i_stride*(i_cu_height_tmp+1));

		p_tmp_l = sample_adaptive_offset->tmp_l1;
		p_tmp_u = &(sample_adaptive_offset->tmp_u1[i_l_pel_x]);
	}

	p_clip_tbl = (i_y_cb_cr==0)? sample_adaptive_offset->clip_table:sample_adaptive_offset->chroma_clip_table;
	p_offset_bo = sample_adaptive_offset->offset_bo;

	switch (i_sao_type)
	{
	case SAO_EO_0: // dir: -
    {
    	i_start_x = (i_l_pel_x == 0) ? 1 : 0;
		i_end_x   = (i_r_pel_x == i_pic_width_tmp) ? i_lcu_width-1 : i_lcu_width;
		for (y=0; y<i_lcu_height; y++)
		{
			i_sign_left = x265_sample_adaptive_offset_x_sign(p_rec[i_start_x] - p_tmp_l[y]);
			for (x=i_start_x; x< i_end_x; x++)
			{
				i_sign_right =  x265_sample_adaptive_offset_x_sign(p_rec[x] - p_rec[x+1]);
				i_edge_type =  i_sign_right + i_sign_left + 2;
				i_sign_left  = -i_sign_right;

				p_rec[x] = p_clip_tbl[p_rec[x] + sample_adaptive_offset->offset_eo[i_edge_type]];
			}
			p_rec += i_stride;
		}
		break;
    }
	case SAO_EO_1: // dir: |
    {
    	i_start_y = (i_t_pel_y == 0) ? 1 : 0;
    	i_end_y   = (i_b_pel_y == i_pic_height_tmp) ? i_lcu_height-1 : i_lcu_height;
    	if (i_t_pel_y == 0)
    	{
    		p_rec += i_stride;
    	}
    	for (x=0; x< i_lcu_width; x++)
    	{
    		sample_adaptive_offset->up_buff_1[x] = x265_sample_adaptive_offset_x_sign(p_rec[x] - p_tmp_u[x]);
    	}
    	for (y=i_start_y; y<i_end_y; y++)
    	{
    		for (x=0; x<i_lcu_width; x++)
    		{
    			i_sign_down  = x265_sample_adaptive_offset_x_sign(p_rec[x] - p_rec[x+i_stride]);
    			i_edge_type = i_sign_down + sample_adaptive_offset->up_buff_1[x] + 2;
    			sample_adaptive_offset->up_buff_1[x]= -i_sign_down;

    			p_rec[x] = p_clip_tbl[p_rec[x] + sample_adaptive_offset->offset_eo[i_edge_type]];
    		}
    		p_rec += i_stride;
    	}
    	break;
    }
	case SAO_EO_2: // dir: 135
    {
    	i_start_x = (i_l_pel_x == 0)            ? 1 : 0;
    	i_end_x   = (i_r_pel_x == i_pic_width_tmp) ? i_lcu_width-1 : i_lcu_width;

    	i_start_y = (i_t_pel_y == 0) ?             1 : 0;
    	i_end_y   = (i_b_pel_y == i_pic_height_tmp) ? i_lcu_height-1 : i_lcu_height;

    	if (i_t_pel_y == 0)
    	{
    		p_rec += i_stride;
    	}

    	for (x=i_start_x; x<i_end_x; x++)
    	{
    		sample_adaptive_offset->up_buff_1[x] = x265_sample_adaptive_offset_x_sign(p_rec[x] - p_tmp_u[x-1]);
    	}
    	for (y=i_start_y; y<i_end_y; y++)
    	{
    		i_sign_down2 = x265_sample_adaptive_offset_x_sign(p_rec[i_stride+i_start_x] - p_tmp_l[y]);
    		for (x=i_start_x; x<i_end_x; x++)
    		{
    			i_sign_down1      =  x265_sample_adaptive_offset_x_sign(p_rec[x] - p_rec[x+i_stride+1]) ;
				i_edge_type      =  i_sign_down1 + sample_adaptive_offset->up_buff_1[x] + 2;
				sample_adaptive_offset->up_buff[x+1] = -i_sign_down1;
				p_rec[x] = p_clip_tbl[p_rec[x] + sample_adaptive_offset->offset_eo[i_edge_type]];
    		}
    		sample_adaptive_offset->up_buff[i_start_x] = i_sign_down2;

    		sample_adaptive_offset->swap     = sample_adaptive_offset->up_buff_1;
    		sample_adaptive_offset->up_buff_1 = sample_adaptive_offset->up_buff;
    		sample_adaptive_offset->up_buff = sample_adaptive_offset->swap;

    		p_rec += i_stride;
    	}
    	break;
    }
	case SAO_EO_3: // dir: 45
	{
		i_start_x = (i_l_pel_x == 0) ? 1 : 0;
		i_end_x   = (i_r_pel_x == i_pic_width_tmp) ? i_lcu_width-1 : i_lcu_width;

		i_start_y = (i_t_pel_y == 0) ? 1 : 0;
		i_end_y   = (i_b_pel_y == i_pic_height_tmp) ? i_lcu_height-1 : i_lcu_height;

		if (i_start_y == 1)
		{
			p_rec += i_stride;
		}

		for (x=i_start_x-1; x<i_end_x; x++)
		{
			sample_adaptive_offset->up_buff_1[x] = x265_sample_adaptive_offset_x_sign(p_rec[x] - p_tmp_u[x+1]);
		}
		for (y=i_start_y; y<i_end_y; y++)
		{
			x=i_start_x;
			i_sign_down1      =  x265_sample_adaptive_offset_x_sign(p_rec[x] - p_tmp_l[y+1]) ;
			i_edge_type      =  i_sign_down1 + sample_adaptive_offset->up_buff_1[x] + 2;
			sample_adaptive_offset->up_buff_1[x-1] = -i_sign_down1;
			p_rec[x] = p_clip_tbl[p_rec[x] + sample_adaptive_offset->offset_eo[i_edge_type]];
			for (x=i_start_x+1; x<i_end_x; x++)
			{
				i_sign_down1      =  x265_sample_adaptive_offset_x_sign(p_rec[x] - p_rec[x+i_stride-1]) ;
				i_edge_type      =  i_sign_down1 + sample_adaptive_offset->up_buff_1[x] + 2;
				sample_adaptive_offset->up_buff_1[x-1] = -i_sign_down1;
				p_rec[x] = p_clip_tbl[p_rec[x] + sample_adaptive_offset->offset_eo[i_edge_type]];
			}
			sample_adaptive_offset->up_buff_1[i_end_x-1] = x265_sample_adaptive_offset_x_sign(p_rec[i_end_x-1 + i_stride] - p_rec[i_end_x]);

			p_rec += i_stride;
		}
		break;
	}
	case SAO_BO:
	{
		for (y=0; y<i_lcu_height; y++)
		{
			for (x=0; x<i_lcu_width; x++)
			{
				p_rec[x] = p_offset_bo[p_rec[x]];
			}
			p_rec += i_stride;
		}
		break;
	}
	default: break;
	}

	//   if (i_sao_type!=sao_bo_0 || i_sao_type!=sao_bo_1)
	if ( 0 == i_y_cb_cr )
	{
		p_tmp_l_swap = sample_adaptive_offset->tmp_l1_y;
		sample_adaptive_offset->tmp_l1_y  = sample_adaptive_offset->tmp_l2_y;
		sample_adaptive_offset->tmp_l2_y  = p_tmp_l_swap;
	}
	else if ( 1 == i_y_cb_cr )
	{
		p_tmp_l_swap = sample_adaptive_offset->tmp_l1_cb;
		sample_adaptive_offset->tmp_l1_cb  = sample_adaptive_offset->tmp_l2_cb;
		sample_adaptive_offset->tmp_l2_cb  = p_tmp_l_swap;
	}
	else
	{
		p_tmp_l_swap = sample_adaptive_offset->tmp_l1_cr;
		sample_adaptive_offset->tmp_l1_cr  = sample_adaptive_offset->tmp_l2_cr;
		sample_adaptive_offset->tmp_l2_cr  = p_tmp_l_swap;
	}
}


pixel* x265_sample_adaptive_offset_get_frame_addr(x265_t *h,
													x265_sample_adaptive_offset_t *sample_adaptive_offset,
													x265_frame_t *frame,
													int32_t i_y_cb_cr,
													int32_t i_cu_addr)
{
	switch (i_y_cb_cr)
	{
	case 0:
		return x265_frame_get_luma_addr_p3(h, frame, i_cu_addr);
		break;
	case 1:
		return x265_frame_get_cb_addr_p3(h, frame, i_cu_addr);
		break;
	case 2:
		return x265_frame_get_cr_addr_p3(h, frame, i_cu_addr);
		break;
	default:
		return NULL;
		break;
	}

	return 0;
}

void x265_sample_adaptive_offset_process_cu(x265_t *h,
											x265_sample_adaptive_offset_t *sample_adaptive_offset,
											x265_sao_lcu_param_t *sao_lcu_param,
											uint32_t i_cu_addr,
											int32_t b_one_unit_flag,
											int32_t y_cb_cr)
{
	pixel *p_rec = NULL;
	pixel *tmp_u_swap = NULL;
	int32_t pic_width_tmp = 0;
	int32_t i = 0;
	uint32_t edge_type = 0;
	pixel *p_luma_table = NULL;
	pixel *p_clip_table = NULL;
	int32_t *p_offset_bo = NULL;
	int32_t type_idx = 0;
	int32_t offset[X265_LUMA_GROUP_NUM+1];
	int32_t idx_x = 0;
	int32_t idx_y = 0;
	int32_t frame_width_in_cu = 0;
	int32_t stride = 0;
	int32_t is_chroma = 0;
	int32_t b_merge_left_flag = 0;
	int32_t sao_bit_increase = 0;
	int32_t bit_depth = 0 ;

	if (y_cb_cr == 0)
	{
		p_rec        = x265_frame_get_luma_addr_p3(h, h->fdec, i_cu_addr);
		pic_width_tmp = sample_adaptive_offset->i_pic_width;
		sample_adaptive_offset->tmp_u1 = sample_adaptive_offset->tmp_u1_y ;
		sample_adaptive_offset->tmp_u2 = sample_adaptive_offset->tmp_u2_y ;
		sample_adaptive_offset->tmp_l1 = sample_adaptive_offset->tmp_l1_y ;
		sample_adaptive_offset->tmp_l2 = sample_adaptive_offset->tmp_l2_y ;
		sample_adaptive_offset->offset_eo = sample_adaptive_offset->offset_eo_y ;
		sample_adaptive_offset->offset_bo = sample_adaptive_offset->offset_bo_y ;
	}
	else if (y_cb_cr == 1)
	{
		p_rec        = x265_frame_get_cb_addr_p3(h, h->fdec, i_cu_addr);
		pic_width_tmp = sample_adaptive_offset->i_pic_width>>1;
		sample_adaptive_offset->tmp_u1 = sample_adaptive_offset->tmp_u1_cb ;
		sample_adaptive_offset->tmp_u2 = sample_adaptive_offset->tmp_u2_cb ;
		sample_adaptive_offset->tmp_l1 = sample_adaptive_offset->tmp_l1_cb ;
		sample_adaptive_offset->tmp_l2 = sample_adaptive_offset->tmp_l2_cb ;
		sample_adaptive_offset->offset_eo = sample_adaptive_offset->offset_eo_cb ;
		sample_adaptive_offset->offset_bo = sample_adaptive_offset->offset_bo_cb ;
	}
	else
	{
		p_rec        = x265_frame_get_cr_addr_p3(h, h->fdec, i_cu_addr);
		pic_width_tmp = sample_adaptive_offset->i_pic_width>>1;
		sample_adaptive_offset->tmp_u1 = sample_adaptive_offset->tmp_u1_cr ;
		sample_adaptive_offset->tmp_u2 = sample_adaptive_offset->tmp_u2_cr ;
		sample_adaptive_offset->tmp_l1 = sample_adaptive_offset->tmp_l1_cr ;
		sample_adaptive_offset->tmp_l2 = sample_adaptive_offset->tmp_l2_cr ;
		sample_adaptive_offset->offset_eo = sample_adaptive_offset->offset_eo_cr ;
		sample_adaptive_offset->offset_bo = sample_adaptive_offset->offset_bo_cr ;
	}

	if ( 0 == i_cu_addr )
	{
		memcpy(sample_adaptive_offset->tmp_u1, p_rec, sizeof(pixel)*pic_width_tmp);
	}

	frame_width_in_cu = h->cu.pic.i_width_in_cu;
	is_chroma = (y_cb_cr == 0) ? 0:1;
	sao_bit_increase = (y_cb_cr == 0) ? sample_adaptive_offset->i_sao_bit_increase_y
										: sample_adaptive_offset->i_sao_bit_increase_c;

	p_offset_bo = sample_adaptive_offset->offset_bo ;

	offset[0] = 0;
	idx_y = i_cu_addr / frame_width_in_cu ;
	idx_x = i_cu_addr % frame_width_in_cu ;

	if (y_cb_cr == 0)
	{
		p_rec  = x265_frame_get_luma_addr_p3(h, h->fdec, idx_y * frame_width_in_cu);
		stride = h->fdec->i_stride[0];
		pic_width_tmp = sample_adaptive_offset->i_pic_width;
	}
	else if (y_cb_cr == 1)
	{
		p_rec  = x265_frame_get_cb_addr_p3(h, h->fdec, idx_y * frame_width_in_cu);
		stride = h->fdec->i_stride[1];
		pic_width_tmp = sample_adaptive_offset->i_pic_width>>1;
	}
	else
	{
		p_rec  = x265_frame_get_cr_addr_p3(h, h->fdec, idx_y * frame_width_in_cu);
		stride = h->fdec->i_stride[1];
		pic_width_tmp = sample_adaptive_offset->i_pic_width>>1;
	}

	if ( 0 == idx_x )
	{
		//     p_rec += i_stride*(sample_adaptive_offset->i_max_cu_height-1);
		for (i=0;i<(sample_adaptive_offset->i_max_cu_height>>is_chroma)+1;i++)
		{
		  sample_adaptive_offset->tmp_l1[i] = p_rec[0];
		  p_rec+=stride;
		}
		p_rec-= ((sample_adaptive_offset->i_max_cu_height>>is_chroma)+1) * stride ;
	}
	if (y_cb_cr == 0)
	{
		memcpy(sample_adaptive_offset->tmp_u2 + idx_x*h->cu.pic.i_max_cu_width,
				x265_frame_get_luma_addr_p3(h, h->fdec, i_cu_addr)
				+ ((sample_adaptive_offset->i_max_cu_height-1)*stride),
				sizeof(pixel)*h->cu.pic.i_max_cu_width);
	}
	else if (y_cb_cr == 1)
	{
		memcpy(sample_adaptive_offset->tmp_u2 + idx_x*(h->cu.pic.i_max_cu_width>>is_chroma),
				x265_frame_get_cb_addr_p3(h, h->fdec, i_cu_addr)
				+ (((sample_adaptive_offset->i_max_cu_height>>is_chroma)-1)*stride),
			sizeof(pixel)*(h->cu.pic.i_max_cu_width>>is_chroma));
	}
	else
	{
		memcpy(sample_adaptive_offset->tmp_u2 + idx_x*(h->cu.pic.i_max_cu_width>>is_chroma),
				x265_frame_get_cr_addr_p3(h, h->fdec, i_cu_addr)
				+ (((sample_adaptive_offset->i_max_cu_height>>is_chroma)-1)*stride),
				sizeof(pixel)*(h->cu.pic.i_max_cu_width>>is_chroma));
	}


	if (b_one_unit_flag)
	{
		type_idx = sao_lcu_param[0].i_type_idx;
		b_merge_left_flag = (i_cu_addr == 0)? 0:1;
	}
	else
	{
		type_idx = sao_lcu_param[i_cu_addr%sample_adaptive_offset->i_num_cu_in_width_mul_two].i_type_idx;
		b_merge_left_flag = sao_lcu_param[i_cu_addr%sample_adaptive_offset->i_num_cu_in_width_mul_two].b_merge_left_flag;
	}
	if (type_idx>=0)
	{
		if (!b_merge_left_flag)
		{

			if (type_idx == SAO_BO)
			{
				for (i=0; i<SAO_MAX_BO_CLASSES+1;i++)
	            {
					offset[i] = 0;
	            }
	            for (i=0; i<sao_lcu_param[i_cu_addr%sample_adaptive_offset->i_num_cu_in_width_mul_two].i_length; i++)
	            {
	            	offset[(sao_lcu_param[i_cu_addr%sample_adaptive_offset->i_num_cu_in_width_mul_two].i_sub_type_idx
	            					+ i ) % SAO_MAX_BO_CLASSES+1]
	            	       = sao_lcu_param[i_cu_addr%sample_adaptive_offset->i_num_cu_in_width_mul_two].offset[i] << sao_bit_increase;
	            }

	            p_luma_table = (y_cb_cr==0) ? sample_adaptive_offset->luma_table_bo
	            							: sample_adaptive_offset->chroma_table_bo;
	            p_clip_table = (y_cb_cr==0) ? sample_adaptive_offset->clip_table
	            							: sample_adaptive_offset->chroma_clip_table;

	            bit_depth = (y_cb_cr == 0) ? h->param.sps.i_bit_depth_y
	            							: h->param.sps.i_bit_depth_c;
	            for (i=0;i<(1<<bit_depth);i++)
	            {
	            	p_offset_bo[i] = p_clip_table[i + offset[p_luma_table[i]]];
	            }

			}
			if (type_idx == SAO_EO_0 || type_idx == SAO_EO_1
					|| type_idx == SAO_EO_2 || type_idx == SAO_EO_3)
			{
				for (i=0;i<sao_lcu_param[i_cu_addr%sample_adaptive_offset->i_num_cu_in_width_mul_two].i_length;i++)
	            {
					offset[i+1] = sao_lcu_param[i_cu_addr%sample_adaptive_offset->i_num_cu_in_width_mul_two].offset[i] << sao_bit_increase;
	            }
	            for (edge_type=0;edge_type<6;edge_type++)
	            {
	            	sample_adaptive_offset->offset_eo[edge_type]= offset[p_sample_adaptive_offset_class_static->eo_table[edge_type]];
	            }
			}
		}
		x265_sample_adaptive_offset_process_sao_cu_org(h,
														sample_adaptive_offset,
														i_cu_addr,
														type_idx,
														y_cb_cr);
	}
	else
	{
		if (idx_x != (frame_width_in_cu-1))
		{
			if (y_cb_cr == 0)
			{
				p_rec  = x265_frame_get_luma_addr_p3(h, h->fdec, i_cu_addr);
	            stride = h->fdec->i_stride[0];
			}
			else if (y_cb_cr == 1)
			{
	            p_rec  = x265_frame_get_cb_addr_p3(h, h->fdec, i_cu_addr);
	            stride = h->fdec->i_stride[1];
			}
			else
			{
				p_rec  = x265_frame_get_cr_addr_p3(h, h->fdec, i_cu_addr);
	            stride = h->fdec->i_stride[1];
			}
			int32_t width_shift = sample_adaptive_offset->i_max_cu_width>>is_chroma;
			for (i=0;i<(sample_adaptive_offset->i_max_cu_height>>is_chroma)+1;i++)
			{
				sample_adaptive_offset->tmp_l1[i] = p_rec[width_shift-1];
	            p_rec+=stride;
			}
		}
	}

	if ( idx_x== frame_width_in_cu - 1 )
	{
		if ( 0 == y_cb_cr )
		{
			tmp_u_swap = sample_adaptive_offset->tmp_u1_y;
			sample_adaptive_offset->tmp_u1_y = sample_adaptive_offset->tmp_u2_y;
			sample_adaptive_offset->tmp_u2_y = tmp_u_swap;
		}
		else if ( 1 == y_cb_cr )
		{
			tmp_u_swap = sample_adaptive_offset->tmp_u1_cb;
			sample_adaptive_offset->tmp_u1_cb = sample_adaptive_offset->tmp_u2_cb;
			sample_adaptive_offset->tmp_u2_cb = tmp_u_swap;
		}
		else
		{
			tmp_u_swap = sample_adaptive_offset->tmp_u1_cr;
			sample_adaptive_offset->tmp_u1_cr = sample_adaptive_offset->tmp_u2_cr;
			sample_adaptive_offset->tmp_u2_cr = tmp_u_swap;
		}
	}
}

/** reset sao lcu part
 * \param sao_lcu_param
 */
void x265_sample_adaptive_offset_reset_lcu_part(x265_sample_adaptive_offset_t *sample_adaptive_offset,
												x265_sao_lcu_param_t *sao_lcu_param)
{
	int32_t i,j;
	for (i=0;i<sample_adaptive_offset->i_num_cu_in_width*2;i++)
	{
		sao_lcu_param[i].b_merge_up_flag     =  1;
		sao_lcu_param[i].b_merge_left_flag =  0;
		sao_lcu_param[i].i_part_idx   =  0;
		sao_lcu_param[i].i_type_idx      = -1;
		for (j=0;j<X265_MAX_NUM_SAO_OFFSETS;j++)
		{
			sao_lcu_param[i].offset[j] = 0;
		}
		sao_lcu_param[i].i_sub_type_idx = 0;
	}
}



void x265_sample_adaptive_offset_reset_sao_unit(x265_sample_adaptive_offset_t *sample_adaptive_offset,
												x265_sao_lcu_param_t *sao_unit)
{
	int32_t loop = 0 ;

	sao_unit->i_part_idx       = 0;
	sao_unit->i_part_idx_tmp    = 0;
	sao_unit->b_merge_left_flag = 0;
	sao_unit->b_merge_up_flag   = 0;
	sao_unit->i_type_idx       = -1;
	sao_unit->i_length        = 0;
	sao_unit->i_sub_type_idx    = 0;

	for ( loop = 0 ; loop < 4 ; ++ loop)
	{
		sao_unit->offset[loop] = 0;
	}
}

void x265_sample_adaptive_offset_copy_sao_unit(x265_sample_adaptive_offset_t *sample_adaptive_offset,
												x265_sao_lcu_param_t *sao_unit_dst,
												x265_sao_lcu_param_t *sao_unit_src )
{
	int32_t loop = 0 ;

	sao_unit_dst->b_merge_left_flag = sao_unit_src->b_merge_left_flag;
	sao_unit_dst->b_merge_up_flag   = sao_unit_src->b_merge_up_flag;
	sao_unit_dst->i_type_idx       = sao_unit_src->i_type_idx;
	sao_unit_dst->i_length        = sao_unit_src->i_length;

	sao_unit_dst->i_sub_type_idx  = sao_unit_src->i_sub_type_idx;
	for ( loop = 0 ; loop < 4 ; loop++ )
	{
		sao_unit_dst->offset[loop] = sao_unit_src->offset[loop];
	}
}

/** pcm lf disable process.
 * \param pc_pic picture (x265_pic)t pointer
 * \returns void
 *
 * \note replace filtered sample values of pcm mode blocks with the transmitted and reconstructed ones.
 */
void x265_sample_adaptive_offset_pcm_lf_disable_process(x265_sample_adaptive_offset_t *sample_adaptive_offset,
														x265_frame_t *frame)
{
	x265_sample_adaptive_offset_x_pcm_restoration(sample_adaptive_offset, frame);
}

/** picture-level pcm restoration.
 * \param pc_pic picture (x265_pic)t pointer
 * \returns void
 */
void x265_sample_adaptive_offset_x_pcm_restoration(x265_sample_adaptive_offset_t *sample_adaptive_offset,
													x265_frame_t *frame)
{

}

/** pcm cu restoration.
 * \param cu pointer to current cu
 * \param i_abs_part_idx part index
 * \param i_depth cu depth
 * \returns void
 */
void x265_sample_adaptive_offset_x_pcm_cu_restoration(x265_t *h,
													x265_sample_adaptive_offset_t *sample_adaptive_offset,
													x265_data_cu_t* cu,
													uint32_t i_abs_zorder_idx,
													uint32_t i_depth )
{
	uint32_t i_cur_num_parts = 0;
	uint32_t i_q_num_parts = 0;
	uint32_t i_part_idx = 0;
	uint32_t i_l_pel_x = 0;
	uint32_t i_t_pel_y = 0;

	i_cur_num_parts = h->cu.pic.i_num_partitions >> (i_depth<<1);
	i_q_num_parts   = i_cur_num_parts >> 2;


	// go to sub-cu
	if( x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_zorder_idx) > i_depth )
	{
		for ( i_part_idx = 0; i_part_idx < 4; i_part_idx++, i_abs_zorder_idx += i_q_num_parts )
		{
			i_l_pel_x = x265_data_cu_get_cu_pel_x(cu) + h->scan.raster_to_pel_x[h->scan.zscan_to_raster[i_abs_zorder_idx]];
			i_t_pel_y = x265_data_cu_get_cu_pel_y(cu) + h->scan.raster_to_pel_y[h->scan.zscan_to_raster[i_abs_zorder_idx]];
			if(( i_l_pel_x < h->sps[0].i_pic_width_in_luma_samples)
					&& (i_t_pel_y < h->sps[0].i_pic_height_in_luma_samples))
			{
				x265_sample_adaptive_offset_x_pcm_cu_restoration(h,
																sample_adaptive_offset,
																cu,
																i_abs_zorder_idx,
																i_depth + 1 );
			}
		}
		return;
	}

	// restore pcm samples
	if ((x265_base_data_cu_get_ipcm_flag_p2((x265_base_data_cu_t*)cu, i_abs_zorder_idx)
			&& h->sps[0].b_pcm_filter_disable_flag )
			|| x265_base_data_cu_is_lossless_coded(h, (x265_base_data_cu_t*)cu, i_abs_zorder_idx))
	{
		x265_sample_adaptive_offset_x_pcm_sample_restoration(h,
															sample_adaptive_offset,
															cu,
															i_abs_zorder_idx,
															i_depth,
															TEXT_LUMA );
		x265_sample_adaptive_offset_x_pcm_sample_restoration(h,
															sample_adaptive_offset,
															cu,
															i_abs_zorder_idx,
															i_depth,
															TEXT_CHROMA_U);
		x265_sample_adaptive_offset_x_pcm_sample_restoration(h,
															sample_adaptive_offset,
															cu,
															i_abs_zorder_idx,
															i_depth,
															TEXT_CHROMA_V);
	}
}

/** pcm sample restoration.
 * \param cu pointer to current cu
 * \param i_abs_part_idx part index
 * \param i_depth cu depth
 * \param tt_text texture component type
 * \returns void
 */
void x265_sample_adaptive_offset_x_pcm_sample_restoration(x265_t *h,
														x265_sample_adaptive_offset_t *sample_adaptive_offset,
														x265_data_cu_t *cu,
														uint32_t i_abs_zorder_idx,
														uint32_t i_depth,
														enum text_type_e i_text_type )
{
	uint32_t i_stride = 0;
	uint32_t i_width = 0;
	uint32_t i_height = 0;
	uint32_t i_pcm_left_shift_bit = 0;
	uint32_t i_x = 0, i_y = 0;
	uint32_t i_min_coeff_size = 0;
	uint32_t i_luma_offset = 0;
	uint32_t i_chroma_offset = 0;
	pixel* p_src = NULL;
	pixel* p_pcm = NULL;

	i_min_coeff_size = h->cu.pic.i_min_cu_width * h->cu.pic.i_min_cu_height ;
	i_luma_offset = i_min_coeff_size * i_abs_zorder_idx;
	i_chroma_offset = i_luma_offset >> 2;

	if( i_text_type == TEXT_LUMA )
	{
		p_src = x265_frame_get_luma_addr_p4(h,
											h->fdec,
											x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu),
											i_abs_zorder_idx);
		p_pcm = x265_data_cu_get_pcm_sample_y(cu) + i_luma_offset;
		i_stride = h->fdec->i_stride[0];
		i_width  = (h->cu.pic.i_max_cu_width >> i_depth);
		i_height = (h->cu.pic.i_max_cu_height >> i_depth);
		if ( x265_base_data_cu_is_lossless_coded(h, (x265_base_data_cu_t*)cu, i_abs_zorder_idx)
				&& !x265_base_data_cu_get_ipcm_flag_p2((x265_base_data_cu_t*)cu, i_abs_zorder_idx) )
		{
			i_pcm_left_shift_bit = 0;
		}
		else
		{
			i_pcm_left_shift_bit = h->param.sps.i_bit_depth_y - h->sps[0].i_pcm_bit_depth_luma ;
		}
	}
	else
	{
		if( i_text_type == TEXT_CHROMA_U )
		{
			p_src = x265_frame_get_cb_addr_p4(h,
												h->fdec,
												x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu),
												i_abs_zorder_idx);
			p_pcm = x265_data_cu_get_pcm_sample_cb(cu) + i_chroma_offset;
		}
		else
		{
			p_src = x265_frame_get_cr_addr_p4(h,
												h->fdec,
												x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu),
												i_abs_zorder_idx);
			p_pcm = x265_data_cu_get_pcm_sample_cr(cu) + i_chroma_offset;
		}

		i_stride = h->fdec->i_stride[1];
		i_width  = ((h->cu.pic.i_max_cu_width >> i_depth)/2);
		i_height = ((h->cu.pic.i_max_cu_width >> i_depth)/2);
		if ( x265_base_data_cu_is_lossless_coded(h, (x265_base_data_cu_t*)cu, i_abs_zorder_idx)
				&& !x265_base_data_cu_get_ipcm_flag_p2((x265_base_data_cu_t*)cu, i_abs_zorder_idx) )
		{
			i_pcm_left_shift_bit = 0;
		}
		else
		{
			i_pcm_left_shift_bit = h->param.sps.i_bit_depth_c - h->sps[0].i_pcm_bit_depth_chroma;
		}
	}

	for( i_y = 0; i_y < i_height; i_y++ )
	{
		for( i_x = 0; i_x < i_width; i_x++ )
		{
			p_src[i_x] = (p_pcm[i_x] << i_pcm_left_shift_bit);
		}
		p_pcm += i_width;
		p_src += i_stride;
	}
}

//! \}
