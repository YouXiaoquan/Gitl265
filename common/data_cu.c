

#include "common.h"


#if X265_ADAPTIVE_QP_SELECTION

x265_data_cu_class_static_t *p_data_cu_class_static ;

int x265_data_cu_class_static_init ()
{
	CHECKED_MALLOCZERO( p_data_cu_class_static, sizeof(x265_data_cu_class_static_t) );

	return 0 ;

fail:
	x265_data_cu_class_static_deinit () ;
	return -1 ;
}

void x265_data_cu_class_static_deinit ()
{
	x265_free ( p_data_cu_class_static ) ;
}

#endif


int32_t x265_data_cu_get_being_encode ( x265_data_cu_t *cu )
{
	return cu->b_being_encode ;
}

void x265_data_cu_set_being_encode ( x265_data_cu_t *cu, int32_t b_being_encode )
{
	cu->b_being_encode = b_being_encode ;
}

uint32_t x265_data_cu_get_cu_pel_x( x265_data_cu_t *cu )
{
	return cu->i_cu_pel_x ;
}

uint32_t x265_data_cu_get_cu_pel_y( x265_data_cu_t *cu )
{
	return cu->i_cu_pel_y ;
}

x265_pattern_t *x265_data_cu_get_pattern( x265_data_cu_t *cu )
{
	return cu->pattern ;
}

int8_t *x265_data_cu_get_partition_size_p1 ( x265_data_cu_t *cu )
{
	return cu->part_size ;
}

enum part_size_e x265_data_cu_get_partition_size_p2 ( x265_data_cu_t *cu, uint32_t i_idx )
{
	return (enum part_size_e)cu->part_size[i_idx] ;
}

void x265_data_cu_set_partition_size ( x265_data_cu_t *cu,
										uint32_t i_idx,
										enum part_size_e i_part_size )
{
	cu->part_size[i_idx] = i_part_size ;
}


uint8_t *x265_data_cu_get_width_p1 ( x265_data_cu_t *cu )
{
	return cu->width ;
}

uint8_t x265_data_cu_get_width_p2 ( x265_data_cu_t *cu, uint32_t i_idx )
{
	return cu->width[i_idx] ;
}

void x265_data_cu_set_width ( x265_data_cu_t *cu, uint32_t i_idx, uint8_t i_width )
{
	cu->width[i_idx] = i_width ;
}

uint8_t *x265_data_cu_get_height_p1 ( x265_data_cu_t *cu )
{
	return cu->height ;
}

uint8_t x265_data_cu_get_height_p2 ( x265_data_cu_t *cu, uint32_t i_idx )
{
	return cu->height[i_idx] ;
}

void x265_data_cu_set_height ( x265_data_cu_t *cu, uint32_t i_idx, uint8_t i_height )
{
	cu->height[i_idx] = i_height ;
}

void x265_data_cu_set_coded_qp ( x265_data_cu_t* cu, int8_t i_coded_qp )
{
	cu->i_coded_qp = i_coded_qp ;
}

int8_t x265_data_cu_get_coded_qp ( x265_data_cu_t* cu )
{
	return cu->i_coded_qp ;
}

uint8_t *x265_data_cu_get_transform_skip_p2 ( x265_data_cu_t *cu,
											enum text_type_e i_text_type )
{
	return cu->transform_skip[convert_txt_type_to_idx[i_text_type]];
}

uint8_t x265_data_cu_get_transform_skip_p3( x265_data_cu_t *cu,
											uint32_t i_idx,
											enum text_type_e i_text_type )
{
	return cu->transform_skip[convert_txt_type_to_idx[i_text_type]][i_idx];
}


x265_coeff_t *x265_data_cu_get_coeff_y ( x265_data_cu_t *cu )
{
	return cu->tr_coeff_y ;
}

x265_coeff_t *x265_data_cu_get_coeff_cb ( x265_data_cu_t *cu )
{
	return cu->tr_coeff_cb ;
}

x265_coeff_t *x265_data_cu_get_coeff_cr ( x265_data_cu_t *cu )
{
	return cu->tr_coeff_cr ;
}

#if X265_ADAPTIVE_QP_SELECTION

int32_t *x265_data_cu_get_arl_coeff_y ( x265_data_cu_t *cu )
{
	return cu->arl_coeff_y ;
}

int32_t *x265_data_cu_get_arl_coeff_cb ( x265_data_cu_t *cu )
{
	return cu->arl_coeff_cb ;
}

int32_t *x265_data_cu_get_arl_coeff_cr ( x265_data_cu_t *cu )
{
	return cu->arl_coeff_cr ;
}

#endif



pixel *x265_data_cu_get_pcm_sample_y ( x265_data_cu_t *cu )
{
	return cu->ipcm_sample_y ;
}

pixel *x265_data_cu_get_pcm_sample_cb ( x265_data_cu_t *cu )
{
	return cu->ipcm_sample_cb ;
}

pixel *x265_data_cu_get_pcm_sample_cr ( x265_data_cu_t *cu )
{
	return cu->ipcm_sample_cr ;
}


int32_t *x265_data_cu_get_merge_flag_p1 ( x265_data_cu_t *cu )
{
	return cu->merge_flag ;
}

int32_t x265_data_cu_get_merge_flag_p2 ( x265_data_cu_t *cu, uint32_t i_idx )
{
	return cu->merge_flag[i_idx] ;
}

void x265_data_cu_set_merge_flag ( x265_data_cu_t* cu,
									uint32_t i_idx,
									int32_t b_merge_flag )
{
	cu->merge_flag[i_idx] = b_merge_flag ;
}

uint8_t *x265_data_cu_get_merge_index_p1 ( x265_data_cu_t *cu )
{
	return cu->merge_index ;
}

uint8_t x265_data_cu_get_merge_index_p2 ( x265_data_cu_t *cu, uint32_t i_idx )
{
	return cu->merge_index[i_idx] ;
}

void x265_data_cu_set_merge_index( x265_data_cu_t* cu,
									uint32_t i_idx,
									uint32_t i_merge_index )
{
	cu->merge_index[i_idx] = i_merge_index ;
}

#if X265_AMP_MRG

void x265_data_cu_set_merge_amp (x265_data_cu_t* cu, int32_t b_is_merge_amp)
{
	cu->b_is_merge_amp = b_is_merge_amp ;
}

int32_t x265_data_cu_get_merge_amp (x265_data_cu_t* cu)
{
	return cu->b_is_merge_amp ;
}

#endif


uint8_t *x265_data_cu_get_chroma_intra_dir_p1 ( x265_data_cu_t *cu )
{
	return cu->chroma_intra_dir ;
}

uint8_t x265_data_cu_get_chroma_intra_dir_p2 ( x265_data_cu_t *cu, uint32_t i_idx )
{
	return cu->chroma_intra_dir[i_idx] ;
}

void x265_data_cu_set_chroma_intra_dir ( x265_data_cu_t* cu,
										uint32_t i_idx,
										uint8_t i_chroma_intra_dir )
{
	cu->chroma_intra_dir[i_idx] = i_chroma_intra_dir ;
}

int32_t x265_data_cu_get_su_slice_id ( x265_data_cu_t* cu,
										uint32_t i_idx )
{
	return cu->slice_su_map[i_idx] ;
}

int32_t *x265_data_cu_get_slice_su_map( x265_data_cu_t* cu )
{
	return cu->slice_su_map ;
}

void x265_data_cu_set_slice_su_map( x265_data_cu_t* cu, int32_t *slice_su_map )
{
	cu->slice_su_map = slice_su_map ;
}

void x265_data_cu_set_mvp_idx( x265_data_cu_t* cu,
								enum ref_pic_list_e i_ref_pic_list,
								uint32_t i_idx,
								int32_t i_mvp_idx )
{
	cu->mvp_idx[i_ref_pic_list][i_idx] = i_mvp_idx ;
}


int8_t *x265_data_cu_get_mvp_idx_p2 (x265_data_cu_t *cu,
									enum ref_pic_list_e i_ref_pic_list )
{
	return cu->mvp_idx[i_ref_pic_list] ;
}

int32_t x265_data_cu_get_mvp_idx_p3 (x265_data_cu_t *cu,
									enum ref_pic_list_e i_ref_pic_list,
									uint32_t i_idx )
{
	return cu->mvp_idx[i_ref_pic_list][i_idx] ;
}

void x265_data_cu_set_mvp_num( x265_data_cu_t* cu,
								enum ref_pic_list_e i_ref_pic_list,
								uint32_t i_idx,
								int32_t i_mvp_num )
{
	cu->mvp_num[i_ref_pic_list][i_idx] = i_mvp_num ;
}

int8_t *x265_data_cu_get_mvp_num_p2 (x265_data_cu_t *cu,
									enum ref_pic_list_e i_ref_pic_list )
{
	return cu->mvp_num[i_ref_pic_list] ;
}

int32_t x265_data_cu_get_mvp_num_p3 (x265_data_cu_t *cu,
									enum ref_pic_list_e i_ref_pic_list,
									uint32_t i_idx )
{
	return cu->mvp_num[i_ref_pic_list][i_idx] ;
}

void x265_data_cu_get_mv_pred_left ( x265_data_cu_t* cu, x265_mv_t* p_mv_pred )
{
	memcpy ( p_mv_pred, x265_mv_field_get_mv(&cu->mv_field_a), sizeof(x265_mv_t) ) ;
}

void x265_data_cu_get_mv_pred_above ( x265_data_cu_t* cu, x265_mv_t* p_mv_pred )
{
	memcpy ( p_mv_pred, x265_mv_field_get_mv(&cu->mv_field_b), sizeof(x265_mv_t) ) ;
}

void x265_data_cu_get_mv_pred_above_right ( x265_data_cu_t* cu, x265_mv_t* p_mv_pred )
{
	memcpy ( p_mv_pred, x265_mv_field_get_mv(&cu->mv_field_c), sizeof(x265_mv_t) ) ;
}

x265_base_data_cu_t *x265_data_cu_get_cu_above_left( x265_data_cu_t *cu )
{
	return cu->cu_above_left ;
}

x265_base_data_cu_t *x265_data_cu_get_cu_above_right( x265_data_cu_t *cu )
{
	return cu->cu_above_right ;
}


uint32_t *x265_data_cu_get_slice_segment_start_cu_p1 ( x265_data_cu_t *cu )
{
	return cu->slice_segment_start_cu ;
}

uint32_t x265_data_cu_get_slice_segment_start_cu_p2 ( x265_data_cu_t *cu, uint32_t i_idx )
{
	return cu->slice_segment_start_cu[i_idx] ;
}


uint32_t x265_data_cu_get_total_bins( x265_data_cu_t *cu )
{
	return cu->i_total_bins ;
}

double x265_data_cu_get_total_cost( x265_data_cu_t *cu )
{
	return cu->f_total_cost ;
}

uint32_t x265_data_cu_get_total_distortion( x265_data_cu_t *cu )
{
	return cu->i_total_distortion ;
}

uint32_t x265_data_cu_get_total_bits( x265_data_cu_t *cu )
{
	return cu->i_total_bits ;
}

x265_data_cu_t *x265_data_cu_new ()
{
	x265_data_cu_t *cu = NULL ;

	CHECKED_MALLOCZERO( cu, sizeof(x265_data_cu_t) );
	if ( x265_data_cu_init (cu) )
	{
		goto fail ;
	}

	return cu ;
fail:
	x265_data_cu_delete ( (void*) cu ) ;
	return NULL ;
}

void x265_data_cu_delete ( x265_data_cu_t *cu )
{
	x265_data_cu_deinit ( cu ) ;
	x265_free ( cu ) ;
}

int x265_data_cu_init ( x265_data_cu_t *cu )
{
	if ( x265_base_data_cu_init((x265_base_data_cu_t*)cu) )
	{
		goto fail ;
	}

	return 0 ;
fail:
	x265_data_cu_deinit (cu) ;
	return -1 ;
}

void x265_data_cu_deinit ( x265_data_cu_t *cu )
{
	x265_base_data_cu_deinit((x265_base_data_cu_t*)cu) ;
}

int x265_data_cu_create ( x265_data_cu_t *cu,
								uint32_t i_num_partition,
								int32_t i_width,
								int32_t i_height,
								int32_t i_unit_size
#if X265_ADAPTIVE_QP_SELECTION
								, int32_t b_global_rmarl_buffer
#endif
								)
{
	memset ( cu, 0 , sizeof(x265_data_cu_t) ) ;
	if ( x265_base_data_cu_create ( (x265_base_data_cu_t *)cu,
								i_num_partition,
								i_width,
								i_height,
								i_unit_size
#if X265_ADAPTIVE_QP_SELECTION
								, b_global_rmarl_buffer
#endif
								)
								)
	{
		goto fail ;
	}

	cu->i_unit_size = i_unit_size ;

	CHECKED_MALLOCZERO ( cu->width, i_num_partition * sizeof(uint8_t) ) ;
	CHECKED_MALLOCZERO ( cu->height, i_num_partition * sizeof(uint8_t) ) ;
	CHECKED_MALLOCZERO ( cu->part_size, i_num_partition * sizeof(int8_t) ) ;
	memset ( cu->part_size, SIZE_NONE, i_num_partition * sizeof(int8_t) ) ;

	CHECKED_MALLOCZERO ( cu->merge_flag, i_num_partition * sizeof(int32_t) ) ;
	CHECKED_MALLOCZERO ( cu->merge_index, i_num_partition * sizeof(int8_t) ) ;
	CHECKED_MALLOCZERO ( cu->chroma_intra_dir, i_num_partition * sizeof(uint8_t) ) ;

	CHECKED_MALLOCZERO ( cu->transform_skip[0], i_num_partition * sizeof(uint8_t) ) ;
	CHECKED_MALLOCZERO ( cu->transform_skip[1], i_num_partition * sizeof(uint8_t) ) ;
	CHECKED_MALLOCZERO ( cu->transform_skip[2], i_num_partition * sizeof(uint8_t) ) ;

	CHECKED_MALLOCZERO ( cu->mvp_idx[0], i_num_partition * sizeof(int8_t) ) ;
	CHECKED_MALLOCZERO ( cu->mvp_idx[1], i_num_partition * sizeof(int8_t) ) ;
	CHECKED_MALLOCZERO ( cu->mvp_num[0], i_num_partition * sizeof(int8_t) ) ;
	CHECKED_MALLOCZERO ( cu->mvp_num[1], i_num_partition * sizeof(int8_t) ) ;
	memset ( cu->mvp_idx[0], -1, i_num_partition * sizeof(int8_t) ) ;
	memset ( cu->mvp_idx[1], -1, i_num_partition * sizeof(int8_t) ) ;


	CHECKED_MALLOCZERO ( cu->tr_coeff_y, i_width * i_height * sizeof(x265_coeff_t) ) ;
	CHECKED_MALLOCZERO ( cu->tr_coeff_cb, i_width * i_height * sizeof(x265_coeff_t) / 4 ) ;
	CHECKED_MALLOCZERO ( cu->tr_coeff_cr, i_width * i_height * sizeof(x265_coeff_t) / 4 ) ;

	memset ( cu->tr_coeff_y, 0, i_width * i_height * sizeof(x265_coeff_t) ) ;
	memset ( cu->tr_coeff_cb, 0, i_width * i_height * sizeof(x265_coeff_t) / 4 ) ;
	memset ( cu->tr_coeff_cr, 0, i_width * i_height * sizeof(x265_coeff_t) / 4 ) ;

	if ( b_global_rmarl_buffer )
	{
		if ( NULL == p_data_cu_class_static->glb_arl_coeff_y )
		{
			CHECKED_MALLOCZERO ( p_data_cu_class_static->glb_arl_coeff_y, i_width * i_height * sizeof(int32_t) ) ;
			CHECKED_MALLOCZERO ( p_data_cu_class_static->glb_arl_coeff_cb, i_width * i_height * sizeof(int32_t) / 4 ) ;
			CHECKED_MALLOCZERO ( p_data_cu_class_static->glb_arl_coeff_cr, i_width * i_height * sizeof(int32_t) / 4 ) ;
		}
		cu->arl_coeff_y = p_data_cu_class_static->glb_arl_coeff_y ;
		cu->arl_coeff_cb = p_data_cu_class_static->glb_arl_coeff_cb ;
		cu->arl_coeff_cr = p_data_cu_class_static->glb_arl_coeff_cr ;
		cu->b_arl_coeff_is_aliased_allocation = 1 ;
	}
	else
	{
		CHECKED_MALLOCZERO ( cu->arl_coeff_y, i_width * i_height * sizeof(int32_t) ) ;
		CHECKED_MALLOCZERO ( cu->arl_coeff_cb, i_width * i_height * sizeof(int32_t) / 4 ) ;
		CHECKED_MALLOCZERO ( cu->arl_coeff_cr, i_width * i_height * sizeof(int32_t) / 4 ) ;
	}

	CHECKED_MALLOCZERO ( cu->ipcm_sample_y, i_width * i_height * sizeof(pixel) ) ;
	CHECKED_MALLOCZERO ( cu->ipcm_sample_cb, i_width * i_height * sizeof(pixel) / 4 ) ;
	CHECKED_MALLOCZERO ( cu->ipcm_sample_cr, i_width * i_height * sizeof(pixel) / 4 ) ;

	CHECKED_MALLOCZERO ( cu->slice_segment_start_cu, i_num_partition * sizeof(uint32_t) ) ;
	CHECKED_MALLOCZERO ( cu->pattern, sizeof(x265_pattern_t) ) ;

	cu->cu_above_left = NULL ;
	cu->cu_above_right = NULL ;

	return 0 ;

fail:
	x265_data_cu_destroy ( cu ) ;
	return -1 ;
}

void x265_data_cu_destroy ( x265_data_cu_t *cu )
{
	x265_base_data_cu_destroy ( (x265_base_data_cu_t *)cu ) ;

	x265_free ( cu->pattern ) ;

	x265_free ( cu->width ) ;
	x265_free ( cu->height ) ;
	x265_free ( cu->part_size ) ;
	x265_free ( cu->merge_flag ) ;
	x265_free ( cu->merge_index ) ;
	x265_free ( cu->chroma_intra_dir ) ;

	x265_free ( cu->transform_skip[0] ) ;
	x265_free ( cu->transform_skip[1] ) ;
	x265_free ( cu->transform_skip[2] ) ;
	x265_free ( cu->tr_coeff_y ) ;
	x265_free ( cu->tr_coeff_cb ) ;
	x265_free ( cu->tr_coeff_cr ) ;

	if ( !cu->b_arl_coeff_is_aliased_allocation )
	{
		x265_free ( cu->arl_coeff_y ) ;
		x265_free ( cu->arl_coeff_cb ) ;
		x265_free ( cu->arl_coeff_cr ) ;
	}

	x265_free ( p_data_cu_class_static->glb_arl_coeff_y ) ;
	x265_free ( p_data_cu_class_static->glb_arl_coeff_cb ) ;
	x265_free ( p_data_cu_class_static->glb_arl_coeff_cr ) ;

	x265_free ( cu->ipcm_sample_y ) ;
	x265_free ( cu->ipcm_sample_cb ) ;
	x265_free ( cu->ipcm_sample_cr ) ;
	x265_free ( cu->mvp_idx[0] ) ;
	x265_free ( cu->mvp_idx[1] ) ;
	x265_free ( cu->mvp_num[0] ) ;
	x265_free ( cu->mvp_num[1] ) ;

	x265_free ( cu->slice_segment_start_cu ) ;


	cu->pattern = NULL ;

	cu->width = NULL ;
	cu->height = NULL ;
	cu->part_size = NULL ;
	cu->merge_flag = NULL ;
	cu->merge_index = NULL ;
	cu->chroma_intra_dir = NULL ;

	cu->transform_skip[0] = NULL ;
	cu->transform_skip[1] = NULL ;
	cu->transform_skip[2] = NULL ;
	cu->tr_coeff_y = NULL ;
	cu->tr_coeff_cb = NULL ;
	cu->tr_coeff_cr = NULL ;

	if ( !cu->b_arl_coeff_is_aliased_allocation )
	{
		cu->arl_coeff_y = NULL ;
		cu->arl_coeff_cb = NULL ;
		cu->arl_coeff_cr = NULL ;
	}

	p_data_cu_class_static->glb_arl_coeff_y = NULL ;
	p_data_cu_class_static->glb_arl_coeff_cb = NULL ;
	p_data_cu_class_static->glb_arl_coeff_cr = NULL ;

	cu->ipcm_sample_y = NULL ;
	cu->ipcm_sample_cb = NULL ;
	cu->ipcm_sample_cr = NULL ;
	cu->mvp_idx[0] = NULL ;
	cu->mvp_idx[1] = NULL ;
	cu->mvp_num[0] = NULL ;
	cu->mvp_num[1] = NULL ;
	cu->slice_segment_start_cu = NULL ;

	cu->cu_above_left = NULL ;
	cu->cu_above_right = NULL ;
}

void x265_data_cu_init_cu ( x265_t *h, x265_data_cu_t *cu, uint32_t i_cu_addr )
{
	int32_t loop = 0 ;
	int32_t i_part_start_idx = 0 ;
	int32_t i_num_elements = 0 ;
	int32_t i_first_element = 0 ;
	uint32_t i_width_in_cu = 0 ;
	uint32_t i_tmp = 0 ;
	x265_data_cu_t * from = NULL ;

	cu->base_data_cu.i_cu_addr = i_cu_addr ;
	cu->i_cu_pel_x = (i_cu_addr % h->cu.pic.i_width_in_cu) * h->cu.pic.i_max_cu_width ;
	cu->i_cu_pel_y = (i_cu_addr / h->cu.pic.i_width_in_cu) * h->cu.pic.i_max_cu_height ;
	cu->base_data_cu.i_abs_idx_in_lcu = 0 ;
	cu->f_total_cost = X265_MAX_DOUBLE;
	cu->i_total_distortion  = 0;
	cu->i_total_bits = 0;
	cu->i_total_bins = 0;
	cu->base_data_cu.i_num_partition = h->cu.pic.i_num_partitions ;

	for ( loop = 0 ; loop < h->cu.pic.i_num_partitions ; ++ loop )
	{
		if ( h->inverse_cu_order_map[i_cu_addr] * h->cu.pic.i_num_partitions
				+ loop >= h->slice->i_slice_cur_start_cu_addr )
		{
			cu->base_data_cu.slice_start_cu[loop] = h->slice->i_slice_cur_start_cu_addr ;
		}
		else
		{
			cu->base_data_cu.slice_start_cu[loop] = h->data_cu->base_data_cu.slice_start_cu[loop] ;
		}
	}
	for ( loop = 0 ; loop < h->cu.pic.i_num_partitions ; ++ loop )
	{
		if( h->inverse_cu_order_map[i_cu_addr] * h->cu.pic.i_num_partitions
				+ loop >= h->slice->i_slice_segment_cur_start_cu_addr )
		{
			cu->slice_segment_start_cu[loop] = h->slice->i_slice_segment_cur_start_cu_addr ;
		}
		else
		{
			cu->slice_segment_start_cu[loop] = h->data_cu->slice_segment_start_cu[loop] ;
		}
	}

	i_part_start_idx = h->slice->i_slice_segment_cur_start_cu_addr
						- h->inverse_cu_order_map[i_cu_addr] * h->cu.pic.i_num_partitions;

	i_num_elements = X265_MIN( i_part_start_idx, cu->base_data_cu.i_num_partition );
	for ( loop = 0; loop < i_num_elements; ++ loop )
	{
		from = h->data_cu ;
		cu->base_data_cu.skip_flag[loop] = x265_base_data_cu_get_skip_flag_p2 ( (x265_base_data_cu_t*)from,
																				loop );
		cu->part_size[loop] = x265_data_cu_get_partition_size_p2( from, loop );
		cu->base_data_cu.pred_mode[loop] = x265_base_data_cu_get_prediction_mode_p2 ( (x265_base_data_cu_t*)from,
																						loop );

		cu->base_data_cu.cu_transquant_bypass[loop] = x265_base_data_cu_get_cu_transquant_bypass_p2 ( (x265_base_data_cu_t*)from,
																										loop );
		cu->base_data_cu.depth[loop] = x265_base_data_cu_get_depth_p2 ( (x265_base_data_cu_t*)from,
																		loop );
		cu->width[loop] = x265_data_cu_get_width_p2 ( from, loop );
		cu->height[loop] = x265_data_cu_get_height_p2 ( from, loop );
		cu->base_data_cu.tr_idx [loop] = x265_base_data_cu_get_transform_idx_p2 ( (x265_base_data_cu_t*)from,
																				loop );
		cu->transform_skip[0][loop] = x265_data_cu_get_transform_skip_p3 ( from, loop, TEXT_LUMA );
		cu->transform_skip[1][loop] = x265_data_cu_get_transform_skip_p3 ( from, loop, TEXT_CHROMA_U );
		cu->transform_skip[2][loop] = x265_data_cu_get_transform_skip_p3 ( from, loop, TEXT_CHROMA_V );
		cu->mvp_idx[0][loop] = from->mvp_idx[0][loop];
		cu->mvp_idx[1][loop] = from->mvp_idx[1][loop];
		cu->mvp_num[0][loop] = from->mvp_num[0][loop];
		cu->mvp_num[1][loop] = from->mvp_num[1][loop];
		cu->base_data_cu.qp[loop]=from->base_data_cu.qp[loop];
		cu->merge_flag[loop]=from->merge_flag[loop];
		cu->merge_index[loop]=from->merge_index[loop];
		cu->base_data_cu.luma_intra_dir[loop]=from->base_data_cu.luma_intra_dir[loop];
		cu->chroma_intra_dir[loop]=from->chroma_intra_dir[loop];
		cu->base_data_cu.inter_dir[loop]=from->base_data_cu.inter_dir[loop];
		cu->base_data_cu.cbf[0][loop]=from->base_data_cu.cbf[0][loop];
		cu->base_data_cu.cbf[1][loop]=from->base_data_cu.cbf[1][loop];
		cu->base_data_cu.cbf[2][loop]=from->base_data_cu.cbf[2][loop];
		cu->base_data_cu.ipcm_flag[loop] = from->base_data_cu.ipcm_flag[loop];
	}

	i_first_element = X265_MAX ( i_part_start_idx, 0 );
	i_num_elements = cu->base_data_cu.i_num_partition - i_first_element;

	if ( i_num_elements > 0 )
	{
		memset( cu->base_data_cu.skip_flag + i_first_element, 0, i_num_elements * sizeof(int32_t));
		memset( cu->part_size + i_first_element, SIZE_NONE, i_num_elements * sizeof(int8_t));
		memset( cu->base_data_cu.pred_mode + i_first_element, MODE_NONE, i_num_elements * sizeof(int8_t));
		memset( cu->base_data_cu.cu_transquant_bypass + i_first_element, 0, i_num_elements * sizeof(int32_t));
		memset( cu->base_data_cu.depth + i_first_element, 0, i_num_elements * sizeof(uint8_t));
		memset( cu->base_data_cu.tr_idx + i_first_element, 0, i_num_elements * sizeof(uint8_t));
		memset( cu->transform_skip[0] + i_first_element, 0, i_num_elements * sizeof(uint8_t));
		memset( cu->transform_skip[1] + i_first_element, 0, i_num_elements * sizeof(uint8_t));
		memset( cu->transform_skip[2] + i_first_element, 0, i_num_elements * sizeof(uint8_t));
		memset( cu->width + i_first_element, h->cu.pic.i_max_cu_width, i_num_elements * sizeof(uint8_t));
		memset( cu->height + i_first_element, h->cu.pic.i_max_cu_height, i_num_elements * sizeof(uint8_t));
		memset( cu->mvp_idx[0] + i_first_element, -1, i_num_elements * sizeof(int8_t));
		memset( cu->mvp_idx[1] + i_first_element, -1, i_num_elements * sizeof(int8_t));
		memset( cu->mvp_num[0] + i_first_element, -1, i_num_elements * sizeof(int8_t));
		memset( cu->mvp_num[1] + i_first_element, -1, i_num_elements * sizeof(int8_t));
		memset( cu->base_data_cu.qp + i_first_element, h->slice->i_slice_qp, i_num_elements * sizeof(int8_t));
		memset( cu->merge_flag + i_first_element, 0, i_num_elements * sizeof(int32_t));
		memset( cu->merge_index + i_first_element, 0, i_num_elements * sizeof(uint8_t));
		memset( cu->base_data_cu.luma_intra_dir + i_first_element, X265_DC_IDX, i_num_elements * sizeof(uint8_t));
		memset( cu->chroma_intra_dir + i_first_element, 0, i_num_elements * sizeof(uint8_t));
		memset( cu->base_data_cu.inter_dir + i_first_element, 0, i_num_elements * sizeof(uint8_t));
		memset( cu->base_data_cu.cbf[0] + i_first_element, 0, i_num_elements * sizeof(uint8_t));
		memset( cu->base_data_cu.cbf[1] + i_first_element, 0, i_num_elements * sizeof(uint8_t));
		memset( cu->base_data_cu.cbf[2] + i_first_element, 0, i_num_elements * sizeof(uint8_t));
		memset( cu->base_data_cu.ipcm_flag + i_first_element, 0, i_num_elements * sizeof(int32_t));
	}

	i_tmp = h->cu.pic.i_max_cu_width * h->cu.pic.i_max_cu_height;
	if ( 0 >= i_part_start_idx )
	{
		x265_cu_mv_field_clear_mv_field( &cu->base_data_cu.cu_mv_field[0] ) ;
		x265_cu_mv_field_clear_mv_field( &cu->base_data_cu.cu_mv_field[1] ) ;
		memset( cu->tr_coeff_y , 0, sizeof( x265_coeff_t ) * i_tmp );
#if X265_ADAPTIVE_QP_SELECTION
		memset( cu->arl_coeff_y , 0, sizeof( int32_t ) * i_tmp );
#endif
		memset( cu->ipcm_sample_y , 0, sizeof( pixel ) * i_tmp );
		i_tmp  >>= 2;
		memset( cu->tr_coeff_cb, 0, sizeof( x265_coeff_t ) * i_tmp );
		memset( cu->tr_coeff_cr, 0, sizeof( x265_coeff_t ) * i_tmp );
#if X265_ADAPTIVE_QP_SELECTION
		memset( cu->arl_coeff_cb, 0, sizeof( int32_t ) * i_tmp );
		memset( cu->arl_coeff_cr, 0, sizeof( int32_t ) * i_tmp );
#endif
		memset( cu->ipcm_sample_cb , 0, sizeof( pixel ) * i_tmp );
		memset( cu->ipcm_sample_cr , 0, sizeof( pixel ) * i_tmp );
	}
	else
	{
		from = h->data_cu;
		x265_cu_mv_field_copy_from( &cu->base_data_cu.cu_mv_field[0],
									&from->base_data_cu.cu_mv_field[0],
									cu->base_data_cu.i_num_partition,
									0 ) ;
		x265_cu_mv_field_copy_from( &cu->base_data_cu.cu_mv_field[1],
									&from->base_data_cu.cu_mv_field[1],
									cu->base_data_cu.i_num_partition,
									0 ) ;
		for( loop = 0 ; loop < i_tmp ; ++ loop )
		{
			cu->tr_coeff_y[loop]=from->tr_coeff_y[loop];
#if X265_ADAPTIVE_QP_SELECTION
			cu->arl_coeff_y[loop]=from->arl_coeff_y[loop];
#endif
			cu->ipcm_sample_y[loop]=from->ipcm_sample_y[loop];
		}
		for( loop = 0 ; loop < (i_tmp >> 2) ; ++ loop )
		{
			cu->tr_coeff_cb[loop]=from->tr_coeff_cb[loop];
			cu->tr_coeff_cr[loop]=from->tr_coeff_cr[loop];
#if X265_ADAPTIVE_QP_SELECTION
			cu->arl_coeff_cb[loop]=from->arl_coeff_cb[loop];
			cu->arl_coeff_cr[loop]=from->arl_coeff_cr[loop];
#endif
			cu->ipcm_sample_cb[loop]=from->ipcm_sample_cb[loop];
			cu->ipcm_sample_cr[loop]=from->ipcm_sample_cr[loop];
		}
	}

	// setting neighbor cu
	cu->base_data_cu.cu_left = NULL;
	cu->base_data_cu.cu_above = NULL;
	cu->cu_above_left = NULL;
	cu->cu_above_right = NULL;

	i_width_in_cu = h->cu.pic.i_width_in_cu ;
	if ( i_cu_addr % i_width_in_cu )
	{
		cu->base_data_cu.cu_left = x265_pic_sym_get_left_cu(h, i_cu_addr) ;
	}

	if ( i_cu_addr / i_width_in_cu )
	{
		cu->base_data_cu.cu_above = x265_pic_sym_get_above_cu(h, i_cu_addr);
	}

	if ( cu->base_data_cu.cu_left && cu->base_data_cu.cu_above )
	{
		cu->cu_above_left = x265_pic_sym_get_above_left_cu(h, i_cu_addr);
	}

	if ( cu->base_data_cu.cu_above && ( (i_cu_addr%i_width_in_cu) < (i_width_in_cu-1) )  )
	{
		cu->cu_above_right = x265_pic_sym_get_above_right_cu(h, i_cu_addr);
	}

}

/** initialize prediction data with enabling sub-lcu-level delta base_data_cu.qp
*\param  i_base_data_cu.depth  base_data_cu.depth of the current cu
*\param  base_data_cu.qp     base_data_cu.qp for the current cu
*- set cu width and cu height according to base_data_cu.depth
*- set base_data_cu.qp value according to input base_data_cu.qp
*- set last-coded base_data_cu.qp value according to input last-coded base_data_cu.qp
*/
void x265_data_cu_init_est_data( x265_t *h,
								x265_data_cu_t* cu,
								uint32_t i_depth,
								int32_t i_qp )
{
	uint32_t loop = 0 ;
	uint8_t i_width = 0 ;
	uint8_t i_height = 0 ;
	uint32_t i_tmp = 0 ;

	cu->f_total_cost = X265_MAX_DOUBLE ;
	cu->i_total_distortion = 0;
	cu->i_total_bits = 0;
	cu->i_total_bins = 0;

	i_width  = h->cu.pic.i_max_cu_width  >> i_depth;
	i_height = h->cu.pic.i_max_cu_height >> i_depth;

	for ( loop = 0; loop < cu->base_data_cu.i_num_partition; ++ loop )
	{
		if(h->inverse_cu_order_map[cu->base_data_cu.i_cu_addr]
		   * h->cu.pic.i_num_partitions + cu->base_data_cu.i_abs_idx_in_lcu
		   + loop >= h->slice->i_slice_segment_cur_start_cu_addr)
		{
			cu->mvp_idx[0][loop] = -1;
			cu->mvp_idx[1][loop] = -1;
			cu->mvp_num[0][loop] = -1;
			cu->mvp_num[1][loop] = -1;
			cu->base_data_cu.depth [loop] = i_depth;
			cu->width  [loop] = i_width;
			cu->height [loop] = i_height;
			cu->base_data_cu.tr_idx  [loop] = 0;
			cu->transform_skip[0][loop] = 0;
			cu->transform_skip[1][loop] = 0;
			cu->transform_skip[2][loop] = 0;
			cu->base_data_cu.skip_flag[loop]   = 0;
			cu->part_size[loop] = SIZE_NONE;
			cu->base_data_cu.pred_mode[loop] = MODE_NONE;
			cu->base_data_cu.cu_transquant_bypass[loop] = 0;
			cu->base_data_cu.ipcm_flag[loop] = 0;
			cu->base_data_cu.qp[loop] = i_qp;
			cu->merge_flag[loop] = 0;
			cu->merge_index[loop] = 0;
			cu->base_data_cu.luma_intra_dir[loop] = X265_DC_IDX;
			cu->chroma_intra_dir[loop] = 0;
			cu->base_data_cu.inter_dir[loop] = 0;
			cu->base_data_cu.cbf[0][loop] = 0;
			cu->base_data_cu.cbf[1][loop] = 0;
			cu->base_data_cu.cbf[2][loop] = 0;
		}
	}

	i_tmp = i_width * i_height;

	if(h->inverse_cu_order_map[cu->base_data_cu.i_cu_addr]
	   * h->cu.pic.i_num_partitions + cu->base_data_cu.i_abs_idx_in_lcu
	   >= h->slice->i_slice_segment_cur_start_cu_addr)
	{
		x265_cu_mv_field_clear_mv_field( &cu->base_data_cu.cu_mv_field[0] ) ;
		x265_cu_mv_field_clear_mv_field( &cu->base_data_cu.cu_mv_field[1] ) ;
		i_tmp = i_width * i_height;

		memset( cu->tr_coeff_y, 0, i_tmp * sizeof(x265_coeff_t));
#if X265_ADAPTIVE_QP_SELECTION
		memset( cu->arl_coeff_y, 0, i_tmp * sizeof(int32_t));
#endif
		memset( cu->ipcm_sample_y, 0, i_tmp * sizeof(pixel));

		i_tmp>>=2;
		memset( cu->tr_coeff_cb, 0, i_tmp * sizeof(x265_coeff_t));
		memset( cu->tr_coeff_cr, 0, i_tmp * sizeof(x265_coeff_t));
#if X265_ADAPTIVE_QP_SELECTION
		memset( cu->arl_coeff_cb, 0, i_tmp * sizeof(int32_t));
		memset( cu->arl_coeff_cr, 0, i_tmp * sizeof(int32_t));
#endif
		memset( cu->ipcm_sample_cb, 0, i_tmp * sizeof(pixel));
		memset( cu->ipcm_sample_cr, 0, i_tmp * sizeof(pixel));
	}
}


// initialize sub partition
void x265_data_cu_init_sub_cu( x265_t *h,
								x265_data_cu_t* cu,
								x265_data_cu_t* cu_src,
								uint32_t i_part_unit_idx,
								uint32_t i_depth,
								int32_t i_qp )
{
	uint32_t loop = 0 ;
	uint32_t i_part_offset = 0 ;
	uint32_t i_num_partition = 0 ;
	uint8_t i_width = 0 ;
	uint8_t i_height = 0 ;
	uint32_t i_tmp = 0 ;
	uint32_t i_max_cu_width = 0;
	uint32_t i_max_cu_height = 0 ;
	int32_t i_minui = 0 ;
	uint32_t i_coff_offset = 0 ;
	x265_data_cu_t * big_cu = NULL ;

	assert( i_part_unit_idx < 4 );
	i_part_offset = (x265_base_data_cu_get_total_num_part((x265_base_data_cu_t*)cu_src) >> 2)
					* i_part_unit_idx ;

	cu->base_data_cu.i_cu_addr = x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu_src);
	cu->base_data_cu.i_abs_idx_in_lcu = x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)cu_src) + i_part_offset ;

	cu->i_cu_pel_x = x265_data_cu_get_cu_pel_x(cu_src) + ( h->cu.pic.i_max_cu_width >> i_depth) * (i_part_unit_idx &  1);
	cu->i_cu_pel_y = x265_data_cu_get_cu_pel_y(cu_src) + ( h->cu.pic.i_max_cu_height >> i_depth) *(i_part_unit_idx >> 1);

	cu->f_total_cost = X265_MAX_DOUBLE ;
	cu->i_total_distortion = 0;
	cu->i_total_bits = 0;
	cu->i_total_bins = 0;
	cu->base_data_cu.i_num_partition = x265_base_data_cu_get_total_num_part((x265_base_data_cu_t*)cu_src) >> 2 ;

	i_num_partition = cu->base_data_cu.i_num_partition ;
	memset( cu->base_data_cu.qp, i_qp, sizeof(int8_t) * i_num_partition );

	memset( cu->merge_flag, 0, sizeof(int32_t) * i_num_partition );
	memset( cu->merge_index, 0, sizeof(uint8_t) * i_num_partition );
	memset( cu->base_data_cu.luma_intra_dir, X265_DC_IDX, sizeof(uint8_t) * i_num_partition );
	memset( cu->chroma_intra_dir,  0, sizeof(uint8_t) * i_num_partition );
	memset( cu->base_data_cu.inter_dir, 0, sizeof(uint8_t) * i_num_partition );
	memset( cu->base_data_cu.tr_idx, 0, sizeof(uint8_t) * i_num_partition );
	memset( cu->transform_skip[0], 0, sizeof(uint8_t) * i_num_partition );
	memset( cu->transform_skip[1], 0, sizeof(uint8_t) * i_num_partition );
	memset( cu->transform_skip[2], 0, sizeof(uint8_t) * i_num_partition );
	memset( cu->base_data_cu.cbf[0], 0, sizeof(uint8_t) * i_num_partition );
	memset( cu->base_data_cu.cbf[1], 0, sizeof(uint8_t) * i_num_partition );
	memset( cu->base_data_cu.cbf[2], 0, sizeof(uint8_t) * i_num_partition );
	memset( cu->base_data_cu.depth, i_depth, sizeof(uint8_t) * i_num_partition );

	i_width = h->cu.pic.i_max_cu_width  >> i_depth;
	i_height = h->cu.pic.i_max_cu_height >> i_depth;
	memset( cu->width, i_width,  sizeof(uint8_t) * i_num_partition );
	memset( cu->height, i_height, sizeof(uint8_t) * i_num_partition );
	memset( cu->base_data_cu.ipcm_flag, 0, sizeof(int32_t) * i_num_partition );
	for ( loop = 0 ; loop < i_num_partition ; ++ loop)
	{
		cu->base_data_cu.skip_flag[loop]   = 0;
		cu->part_size[loop] = SIZE_NONE;
		cu->base_data_cu.pred_mode[loop] = MODE_NONE;
		cu->base_data_cu.cu_transquant_bypass[loop] = 0;
		cu->mvp_idx[0][loop] = -1;
		cu->mvp_idx[1][loop] = -1;
		cu->mvp_num[0][loop] = -1;
		cu->mvp_num[1][loop] = -1;
		if(h->inverse_cu_order_map[cu->base_data_cu.i_cu_addr]
		   * h->cu.pic.i_num_partitions + cu->base_data_cu.i_abs_idx_in_lcu
		   + loop<h->slice->i_slice_segment_cur_start_cu_addr)
		{
			cu->mvp_idx[0][loop] = cu_src->mvp_idx[0][i_part_offset + loop];
			cu->mvp_idx[1][loop] = cu_src->mvp_idx[1][i_part_offset + loop];
			cu->mvp_num[0][loop] = cu_src->mvp_num[0][i_part_offset + loop];
			cu->mvp_num[1][loop] = cu_src->mvp_num[1][i_part_offset + loop];
			cu->base_data_cu.depth  [loop] = x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu_src, i_part_offset+loop);
			cu->width  [loop] = x265_data_cu_get_width_p2(cu_src, i_part_offset+loop);
			cu->height  [loop] = x265_data_cu_get_height_p2(cu_src, i_part_offset+loop);
			cu->base_data_cu.tr_idx  [loop] = x265_base_data_cu_get_transform_idx_p2((x265_base_data_cu_t*)cu_src, i_part_offset+loop);
			cu->transform_skip[0][loop] = x265_data_cu_get_transform_skip_p3(cu_src, i_part_offset+loop, TEXT_LUMA);
			cu->transform_skip[1][loop] = x265_data_cu_get_transform_skip_p3(cu_src, i_part_offset+loop, TEXT_CHROMA_U);
			cu->transform_skip[2][loop] = x265_data_cu_get_transform_skip_p3(cu_src, i_part_offset+loop, TEXT_CHROMA_V);
			cu->base_data_cu.skip_flag[loop] = x265_base_data_cu_get_skip_flag_p2((x265_base_data_cu_t*)cu_src, i_part_offset+loop);
			cu->part_size[loop] = x265_data_cu_get_partition_size_p2 (cu_src, i_part_offset+loop);
			cu->base_data_cu.pred_mode[loop] = x265_base_data_cu_get_prediction_mode_p2((x265_base_data_cu_t*)cu_src, i_part_offset+loop);
			cu->base_data_cu.cu_transquant_bypass[loop] = x265_base_data_cu_get_cu_transquant_bypass_p2((x265_base_data_cu_t*)cu_src, i_part_offset+loop);
			cu->base_data_cu.ipcm_flag[loop] = cu_src->base_data_cu.ipcm_flag[i_part_offset+loop];
			cu->base_data_cu.qp[loop] = cu_src->base_data_cu.qp[i_part_offset+loop];
			cu->merge_flag[loop] = cu_src->merge_flag[i_part_offset+loop];
			cu->merge_index[loop] = cu_src->merge_index[i_part_offset+loop];
			cu->base_data_cu.luma_intra_dir[loop] = cu_src->base_data_cu.luma_intra_dir[i_part_offset+loop];
			cu->chroma_intra_dir[loop] = cu_src->chroma_intra_dir[i_part_offset+loop];
			cu->base_data_cu.inter_dir[loop] = cu_src->base_data_cu.inter_dir[i_part_offset+loop];
			cu->base_data_cu.cbf[0][loop] = cu_src->base_data_cu.cbf[0][i_part_offset+loop];
			cu->base_data_cu.cbf[1][loop] = cu_src->base_data_cu.cbf[1][i_part_offset+loop];
			cu->base_data_cu.cbf[2][loop] = cu_src->base_data_cu.cbf[2][i_part_offset+loop];
		}
	}
	i_tmp = i_width * i_height;
	memset( cu->tr_coeff_y, 0, sizeof(x265_coeff_t)*i_tmp );
#if X265_ADAPTIVE_QP_SELECTION
	memset( cu->arl_coeff_y, 0, sizeof(int32_t)*i_tmp );
#endif
	memset( cu->ipcm_sample_y, 0, sizeof(pixel) * i_tmp );
	i_tmp >>= 2;
	memset( cu->tr_coeff_cb, 0, sizeof(x265_coeff_t)*i_tmp );
	memset( cu->tr_coeff_cr, 0, sizeof(x265_coeff_t)*i_tmp );
#if X265_ADAPTIVE_QP_SELECTION
	memset( cu->arl_coeff_cb, 0, sizeof(int32_t)*i_tmp );
	memset( cu->arl_coeff_cr, 0, sizeof(int32_t)*i_tmp );
#endif
	memset( cu->ipcm_sample_cb , 0, sizeof(pixel) * i_tmp );
	memset( cu->ipcm_sample_cr , 0, sizeof(pixel) * i_tmp );
	x265_cu_mv_field_clear_mv_field( &cu->base_data_cu.cu_mv_field[0] ) ;
	x265_cu_mv_field_clear_mv_field( &cu->base_data_cu.cu_mv_field[1] ) ;

	if(h->inverse_cu_order_map[cu->base_data_cu.i_cu_addr]
	    * h->cu.pic.i_num_partitions + cu->base_data_cu.i_abs_idx_in_lcu
		< h->slice->i_slice_segment_cur_start_cu_addr )
	{
		// part of this cu contains data from an older slice. now copy in that data.
		i_max_cu_width = h->sps[0].i_max_cu_width;
		i_max_cu_height = h->sps[0].i_max_cu_height;
		big_cu = h->data_cu ;
		i_minui = i_part_offset;
		i_minui = - i_minui;
		x265_cu_mv_field_copy_to_p5 ( &cu->base_data_cu.cu_mv_field[0],
										x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu_src,
																			REF_PIC_LIST_0),
										i_minui,
										i_part_offset,
										cu->base_data_cu.i_num_partition ) ;
		x265_cu_mv_field_copy_to_p5 ( &cu->base_data_cu.cu_mv_field[1],
										x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu_src,
																			REF_PIC_LIST_1),
										i_minui,
										i_part_offset,
										cu->base_data_cu.i_num_partition ) ;
		i_coff_offset = i_max_cu_width * i_max_cu_height * cu->base_data_cu.i_abs_idx_in_lcu
						/ h->cu.pic.i_num_partitions ;
		i_tmp = i_width * i_height;
		for ( loop = 0 ; loop < i_tmp ; ++ loop )
		{
			cu->tr_coeff_y[loop] = big_cu->tr_coeff_y[i_coff_offset + loop];
#if X265_ADAPTIVE_QP_SELECTION
			cu->arl_coeff_y[loop] = big_cu->arl_coeff_y[i_coff_offset + loop];
#endif
			cu->ipcm_sample_y[loop] = big_cu->ipcm_sample_y[i_coff_offset + loop];
		}
		i_tmp >>= 2;
		i_coff_offset >>= 2;
		for ( loop = 0 ; loop < i_tmp ; ++ loop )
		{
			cu->tr_coeff_cr[loop] = big_cu->tr_coeff_cr[i_coff_offset + loop];
			cu->tr_coeff_cb[loop] = big_cu->tr_coeff_cb[i_coff_offset + loop];
#if X265_ADAPTIVE_QP_SELECTION
			cu->arl_coeff_cr[loop] = big_cu->arl_coeff_cr[i_coff_offset + loop];
			cu->arl_coeff_cb[loop] = big_cu->arl_coeff_cb[i_coff_offset + loop];
#endif
			cu->ipcm_sample_cb[loop] = big_cu->ipcm_sample_cb[i_coff_offset + loop];
			cu->ipcm_sample_cr[loop] = big_cu->ipcm_sample_cr[i_coff_offset + loop];
		}
	}

	cu->base_data_cu.cu_left = cu_src->base_data_cu.cu_left;
	cu->base_data_cu.cu_above = cu_src->base_data_cu.cu_above;
	cu->cu_above_left = cu_src->cu_above_left;
	cu->cu_above_right = cu_src->cu_above_right;

	memcpy(cu->base_data_cu.slice_start_cu,
			cu_src->base_data_cu.slice_start_cu + i_part_offset,
			sizeof(uint32_t) * cu->base_data_cu.i_num_partition);
	memcpy(cu->slice_segment_start_cu,
			cu_src->slice_segment_start_cu + i_part_offset,
			sizeof(uint32_t) * cu->base_data_cu.i_num_partition);
}

void x265_data_cu_set_outside_cu_part( x265_t *h,
										x265_data_cu_t* cu,
										uint32_t i_abs_part_idx,
										uint32_t i_depth )
{
	uint32_t i_num_partition = 0 ;
	uint8_t i_width = 0 ;
	uint8_t i_height = 0 ;

	i_num_partition = cu->base_data_cu.i_num_partition >> (i_depth << 1);
	i_width  = h->cu.pic.i_max_cu_width  >> i_depth;
	i_height = h->cu.pic.i_max_cu_height >> i_depth;

	memset( cu->base_data_cu.depth + i_abs_part_idx, i_depth, sizeof(uint8_t ) * i_num_partition );
	memset( cu->width + i_abs_part_idx, i_width, sizeof(uint8_t ) * i_num_partition );
	memset( cu->height + i_abs_part_idx, i_height, sizeof(uint8_t ) * i_num_partition );
}

// --------------------------------------------------------------------------------------------------------------------
// copy
// --------------------------------------------------------------------------------------------------------------------

void x265_data_cu_copy_sub_cu( x265_t *h,
								x265_data_cu_t* cu,
								x265_data_cu_t* cu_src,
								uint32_t i_abs_part_idx,
								uint32_t i_depth )
{
	uint32_t i_part = i_abs_part_idx;
	uint32_t i_width = 0 ;
	uint32_t i_height = 0 ;
	uint32_t i_tmp = i_width*i_height;
	uint32_t i_max_cu_width = 0 ;
	uint32_t i_max_cu_height = 0 ;
	uint32_t i_coff_offset = 0 ;

	i_part = i_abs_part_idx;

	cu->base_data_cu.i_cu_addr = x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu_src);
	cu->base_data_cu.i_abs_idx_in_lcu = x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)cu_src) ;

	cu->i_cu_pel_x = x265_data_cu_get_cu_pel_x(cu_src) + h->scan.raster_to_pel_x[h->scan.zscan_to_raster[i_abs_part_idx]];
	cu->i_cu_pel_y = x265_data_cu_get_cu_pel_y(cu_src) + h->scan.raster_to_pel_y[h->scan.zscan_to_raster[i_abs_part_idx]];

	i_width = h->cu.pic.i_max_cu_width  >> i_depth;
	i_height = h->cu.pic.i_max_cu_height >> i_depth;

	cu->base_data_cu.skip_flag = x265_base_data_cu_get_skip_flag_p1((x265_base_data_cu_t*)cu_src) + i_part;

	cu->base_data_cu.qp = x265_base_data_cu_get_qp_p1((x265_base_data_cu_t*)cu_src) + i_part;
	cu->part_size = x265_data_cu_get_partition_size_p1(cu_src) + i_part;
	cu->base_data_cu.pred_mode = x265_base_data_cu_get_prediction_mode_p1((x265_base_data_cu_t*)cu_src)  + i_part;
	cu->base_data_cu.cu_transquant_bypass  = x265_base_data_cu_get_cu_transquant_bypass_p1((x265_base_data_cu_t*)cu_src) + i_part;

	cu->merge_flag = x265_data_cu_get_merge_flag_p1(cu_src) + i_part;
	cu->merge_index = x265_data_cu_get_merge_index_p1(cu_src) + i_part;

	cu->base_data_cu.luma_intra_dir = x265_base_data_cu_get_luma_intra_dir_p1((x265_base_data_cu_t*)cu_src) + i_part;
	cu->chroma_intra_dir = x265_data_cu_get_chroma_intra_dir_p1(cu_src) + i_part;
	cu->base_data_cu.inter_dir = x265_base_data_cu_get_inter_dir_p1((x265_base_data_cu_t*)cu_src) + i_part;
	cu->base_data_cu.tr_idx = x265_base_data_cu_get_transform_idx_p1((x265_base_data_cu_t*)cu_src) + i_part;
	cu->transform_skip[0] = x265_data_cu_get_transform_skip_p2(cu_src, TEXT_LUMA) + i_part;
	cu->transform_skip[1] = x265_data_cu_get_transform_skip_p2(cu_src, TEXT_CHROMA_U) + i_part;
	cu->transform_skip[2] = x265_data_cu_get_transform_skip_p2(cu_src, TEXT_CHROMA_V) + i_part;

	cu->base_data_cu.cbf[0] = x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu_src, TEXT_LUMA) + i_part;
	cu->base_data_cu.cbf[1] = x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu_src, TEXT_CHROMA_U) + i_part;
	cu->base_data_cu.cbf[2] = x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu_src, TEXT_CHROMA_V) + i_part;

	cu->base_data_cu.depth = x265_base_data_cu_get_depth_p1((x265_base_data_cu_t*)cu_src) + i_part;
	cu->width = x265_data_cu_get_width_p1(cu_src) + i_part;
	cu->height = x265_data_cu_get_height_p1(cu_src) + i_part;

	cu->mvp_idx[0] = x265_data_cu_get_mvp_idx_p2(cu_src, REF_PIC_LIST_0) + i_part;
	cu->mvp_idx[1] = x265_data_cu_get_mvp_idx_p2(cu_src, REF_PIC_LIST_1) + i_part;
	cu->mvp_num[0] = x265_data_cu_get_mvp_num_p2(cu_src, REF_PIC_LIST_0) + i_part;
	cu->mvp_num[1] = x265_data_cu_get_mvp_num_p2(cu_src, REF_PIC_LIST_1) + i_part;

	cu->base_data_cu.ipcm_flag = x265_base_data_cu_get_ipcm_flag_p1((x265_base_data_cu_t*)cu_src) + i_part;

	cu->base_data_cu.cu_left = x265_base_data_cu_get_cu_left((x265_base_data_cu_t*)cu_src) ;
	cu->base_data_cu.cu_above = x265_base_data_cu_get_cu_above((x265_base_data_cu_t*)cu_src) ;
	cu->cu_above_left = x265_data_cu_get_cu_above_left(cu_src);
	cu->cu_above_right = x265_data_cu_get_cu_above_right(cu_src);

	i_tmp = i_width*i_height;
	i_max_cu_width = h->sps[0].i_max_cu_width ;
	i_max_cu_height = h->sps[0].i_max_cu_height ;

	i_coff_offset = i_max_cu_width * i_max_cu_height * i_abs_part_idx
					/ h->cu.pic.i_num_partitions ;

	cu->tr_coeff_y = x265_data_cu_get_coeff_y(cu_src) + i_coff_offset;
#if X265_ADAPTIVE_QP_SELECTION
	cu->arl_coeff_y = x265_data_cu_get_arl_coeff_y(cu_src) + i_coff_offset;
#endif
	cu->ipcm_sample_y = x265_data_cu_get_pcm_sample_y(cu_src) + i_coff_offset;

	i_tmp >>= 2;
	i_coff_offset >>= 2;
	cu->tr_coeff_cb = x265_data_cu_get_coeff_cb(cu_src) + i_coff_offset;
	cu->tr_coeff_cr = x265_data_cu_get_coeff_cr(cu_src) + i_coff_offset;
#if X265_ADAPTIVE_QP_SELECTION
	cu->arl_coeff_cb = x265_data_cu_get_arl_coeff_cb(cu_src) + i_coff_offset;
	cu->arl_coeff_cr = x265_data_cu_get_arl_coeff_cr(cu_src) + i_coff_offset;
#endif
	cu->ipcm_sample_cb = x265_data_cu_get_pcm_sample_cb(cu_src) + i_coff_offset;
	cu->ipcm_sample_cr = x265_data_cu_get_pcm_sample_cr(cu_src) + i_coff_offset;

	x265_cu_mv_field_link_to_with_offset ( &cu->base_data_cu.cu_mv_field[0],
											x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu_src,
																				REF_PIC_LIST_0),
											i_part ) ;
	x265_cu_mv_field_link_to_with_offset ( &cu->base_data_cu.cu_mv_field[1],
											x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu_src,
																				REF_PIC_LIST_1),
											i_part ) ;
	memcpy(cu->base_data_cu.slice_start_cu,
			cu_src->base_data_cu.slice_start_cu + i_part,
			sizeof(uint32_t) * cu->base_data_cu.i_num_partition);
	memcpy(cu->slice_segment_start_cu,
			cu_src->slice_segment_start_cu + i_part,
			sizeof(uint32_t) * cu->base_data_cu.i_num_partition);
}

// copy inter prediction info from the biggest cu
void x265_data_cu_copy_inter_pred_info_from ( x265_t *h,
											x265_data_cu_t* cu,
											x265_data_cu_t* cu_src,
											uint32_t i_abs_part_idx,
											enum ref_pic_list_e i_ref_pic_list )
{
	int32_t i_rast_part_idx = 0 ;

	cu->base_data_cu.i_cu_addr = x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu_src);
	cu->base_data_cu.i_abs_idx_in_lcu      = i_abs_part_idx;

	i_rast_part_idx = h->scan.zscan_to_raster[i_abs_part_idx];
	cu->i_cu_pel_x = x265_data_cu_get_cu_pel_x(cu_src) + h->cu.pic.i_min_cu_width * ( i_rast_part_idx % h->cu.pic.i_num_part_in_width );
	cu->i_cu_pel_y = x265_data_cu_get_cu_pel_y(cu_src) + h->cu.pic.i_min_cu_height * ( i_rast_part_idx / h->cu.pic.i_num_part_in_width );

	cu->base_data_cu.cu_left = x265_base_data_cu_get_cu_left((x265_base_data_cu_t*)cu_src) ;
	cu->base_data_cu.cu_above = x265_base_data_cu_get_cu_above((x265_base_data_cu_t*)cu_src) ;
	cu->cu_above_left = x265_data_cu_get_cu_above_left(cu_src);
	cu->cu_above_right = x265_data_cu_get_cu_above_right(cu_src);

	cu->base_data_cu.skip_flag = x265_base_data_cu_get_skip_flag_p1((x265_base_data_cu_t*)cu_src) + i_abs_part_idx;

	cu->part_size = x265_data_cu_get_partition_size_p1(cu_src) + i_abs_part_idx;
	cu->base_data_cu.pred_mode = x265_base_data_cu_get_prediction_mode_p1((x265_base_data_cu_t*)cu_src) + i_abs_part_idx;
	cu->base_data_cu.cu_transquant_bypass = x265_base_data_cu_get_cu_transquant_bypass_p1((x265_base_data_cu_t*)cu_src) + i_abs_part_idx;
	cu->base_data_cu.inter_dir = x265_base_data_cu_get_inter_dir_p1((x265_base_data_cu_t*)cu_src) + i_abs_part_idx;

	cu->base_data_cu.depth = x265_base_data_cu_get_depth_p1((x265_base_data_cu_t*)cu_src) + i_abs_part_idx;
	cu->width = x265_data_cu_get_width_p1(cu_src) + i_abs_part_idx;
	cu->height = x265_data_cu_get_height_p1(cu_src) + i_abs_part_idx;

	cu->merge_flag = x265_data_cu_get_merge_flag_p1(cu_src) + i_abs_part_idx;
	cu->merge_index = x265_data_cu_get_merge_index_p1(cu_src) + i_abs_part_idx;

	cu->mvp_idx[i_ref_pic_list] = x265_data_cu_get_mvp_idx_p2(cu_src, i_ref_pic_list) + i_abs_part_idx;
	cu->mvp_num[i_ref_pic_list] = x265_data_cu_get_mvp_num_p2(cu_src, i_ref_pic_list) + i_abs_part_idx;

	x265_cu_mv_field_link_to_with_offset ( &cu->base_data_cu.cu_mv_field[i_ref_pic_list],
											x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu_src,
																				i_ref_pic_list ),
											i_abs_part_idx );

	memcpy(cu->base_data_cu.slice_start_cu,
			cu_src->base_data_cu.slice_start_cu + i_abs_part_idx,
			sizeof(uint32_t) * cu->base_data_cu.i_num_partition);
	memcpy(cu->slice_segment_start_cu,
			cu_src->slice_segment_start_cu + i_abs_part_idx,
			sizeof(uint32_t) * cu->base_data_cu.i_num_partition);
}

// copy small cu to bigger cu.
// one of quarter parts overwritten by predicted sub part.
void x265_data_cu_copy_part_from ( x265_t *h,
								x265_data_cu_t* cu,
								x265_data_cu_t* cu_src,
								uint32_t i_part_unit_idx,
								uint32_t i_depth )
{
	uint32_t i_offset = 0 ;
	uint32_t i_num_partition = 0  ;
	uint32_t i_tmp  = 0 ;
	uint32_t i_tmp2 = 0 ;

	assert( i_part_unit_idx < 4 );

	cu->f_total_cost += x265_data_cu_get_total_cost(cu_src);
	cu->i_total_distortion += x265_data_cu_get_total_distortion(cu_src);
	cu->i_total_bits += x265_data_cu_get_total_bits(cu_src);

	i_offset = x265_base_data_cu_get_total_num_part((x265_base_data_cu_t*)cu_src)
						* i_part_unit_idx;
	i_num_partition = x265_base_data_cu_get_total_num_part((x265_base_data_cu_t*)cu_src) ;

	memcpy( cu->base_data_cu.skip_flag + i_offset, x265_base_data_cu_get_skip_flag_p1((x265_base_data_cu_t*)cu_src), sizeof(int32_t) * i_num_partition );
	memcpy( cu->base_data_cu.qp + i_offset, x265_base_data_cu_get_qp_p1((x265_base_data_cu_t*)cu_src), sizeof(int8_t) * i_num_partition );
	memcpy( cu->part_size + i_offset, x265_data_cu_get_partition_size_p1(cu_src), sizeof(int8_t) * i_num_partition );
	memcpy( cu->base_data_cu.pred_mode + i_offset, x265_base_data_cu_get_prediction_mode_p1((x265_base_data_cu_t*)cu_src), sizeof(int8_t) * i_num_partition );
	memcpy( cu->base_data_cu.cu_transquant_bypass + i_offset, x265_base_data_cu_get_cu_transquant_bypass_p1((x265_base_data_cu_t*)cu_src), sizeof(int32_t) * i_num_partition );
	memcpy( cu->merge_flag + i_offset, x265_data_cu_get_merge_flag_p1(cu_src), sizeof(int32_t) * i_num_partition );
	memcpy( cu->merge_index + i_offset, x265_data_cu_get_merge_index_p1(cu_src), sizeof(uint8_t) * i_num_partition );
	memcpy( cu->base_data_cu.luma_intra_dir + i_offset, x265_base_data_cu_get_luma_intra_dir_p1((x265_base_data_cu_t*)cu_src), sizeof(uint8_t) * i_num_partition );
	memcpy( cu->chroma_intra_dir + i_offset, x265_data_cu_get_chroma_intra_dir_p1(cu_src), sizeof(uint8_t) * i_num_partition );
	memcpy( cu->base_data_cu.inter_dir + i_offset, x265_base_data_cu_get_inter_dir_p1((x265_base_data_cu_t*)cu_src), sizeof(uint8_t) * i_num_partition );
	memcpy( cu->base_data_cu.tr_idx + i_offset, x265_base_data_cu_get_transform_idx_p1((x265_base_data_cu_t*)cu_src), sizeof(uint8_t) * i_num_partition );
	memcpy( cu->transform_skip[0] + i_offset, x265_data_cu_get_transform_skip_p2(cu_src, TEXT_LUMA), sizeof(uint8_t) * i_num_partition );
	memcpy( cu->transform_skip[1] + i_offset, x265_data_cu_get_transform_skip_p2(cu_src, TEXT_CHROMA_U), sizeof(uint8_t) * i_num_partition );
	memcpy( cu->transform_skip[2] + i_offset, x265_data_cu_get_transform_skip_p2(cu_src, TEXT_CHROMA_V), sizeof(uint8_t) * i_num_partition );
	memcpy( cu->base_data_cu.cbf[0] + i_offset, x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu_src, TEXT_LUMA), sizeof(uint8_t) * i_num_partition );
	memcpy( cu->base_data_cu.cbf[1] + i_offset, x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu_src, TEXT_CHROMA_U), sizeof(uint8_t) * i_num_partition );
	memcpy( cu->base_data_cu.cbf[2] + i_offset, x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu_src, TEXT_CHROMA_V), sizeof(uint8_t) * i_num_partition );
	memcpy( cu->base_data_cu.depth + i_offset, x265_base_data_cu_get_depth_p1((x265_base_data_cu_t*)cu_src), sizeof(uint8_t) * i_num_partition );
	memcpy( cu->width + i_offset, x265_data_cu_get_width_p1(cu_src), sizeof(uint8_t) * i_num_partition );
	memcpy( cu->height + i_offset, x265_data_cu_get_height_p1(cu_src), sizeof(uint8_t) * i_num_partition );
	memcpy( cu->mvp_idx[0] + i_offset, x265_data_cu_get_mvp_idx_p2(cu_src, REF_PIC_LIST_0), sizeof(int8_t) * i_num_partition );
	memcpy( cu->mvp_idx[1] + i_offset, x265_data_cu_get_mvp_idx_p2(cu_src, REF_PIC_LIST_1), sizeof(int8_t) * i_num_partition );
	memcpy( cu->mvp_num[0] + i_offset, x265_data_cu_get_mvp_num_p2(cu_src, REF_PIC_LIST_0), sizeof(int8_t) * i_num_partition );
	memcpy( cu->mvp_num[1] + i_offset, x265_data_cu_get_mvp_num_p2(cu_src, REF_PIC_LIST_1), sizeof(int8_t) * i_num_partition );
	memcpy( cu->base_data_cu.ipcm_flag + i_offset, x265_base_data_cu_get_ipcm_flag_p1((x265_base_data_cu_t*)cu_src), sizeof(int32_t) * i_num_partition );

	cu->base_data_cu.cu_left = x265_base_data_cu_get_cu_left((x265_base_data_cu_t*)cu_src) ;
	cu->base_data_cu.cu_above = x265_base_data_cu_get_cu_above((x265_base_data_cu_t*)cu_src) ;
	cu->cu_above_left = x265_data_cu_get_cu_above_left(cu_src);
	cu->cu_above_right = x265_data_cu_get_cu_above_right(cu_src);

	x265_cu_mv_field_copy_from (&cu->base_data_cu.cu_mv_field[0],
								x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu_src,
																	REF_PIC_LIST_0),
								x265_base_data_cu_get_total_num_part((x265_base_data_cu_t*)cu_src),
								i_offset );
	x265_cu_mv_field_copy_from (&cu->base_data_cu.cu_mv_field[1],
								x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu_src,
																	REF_PIC_LIST_1),
								x265_base_data_cu_get_total_num_part((x265_base_data_cu_t*)cu_src),
								i_offset );

	i_tmp  = h->cu.pic.i_max_cu_width*h->cu.pic.i_max_cu_height >> (i_depth << 1);
	i_tmp2 = i_part_unit_idx * i_tmp;
	memcpy( cu->tr_coeff_y  + i_tmp2, x265_data_cu_get_coeff_y(cu_src),  sizeof(x265_coeff_t)*i_tmp );
#if X265_ADAPTIVE_QP_SELECTION
	memcpy( cu->arl_coeff_y  + i_tmp2, x265_data_cu_get_arl_coeff_y(cu_src), sizeof(int32_t)*i_tmp );
#endif
	memcpy( cu->ipcm_sample_y + i_tmp2, x265_data_cu_get_pcm_sample_y(cu_src), sizeof(pixel) * i_tmp );

	i_tmp >>= 2; i_tmp2>>= 2;
	memcpy( cu->tr_coeff_cb + i_tmp2, x265_data_cu_get_coeff_cb(cu_src), sizeof(x265_coeff_t)*i_tmp );
	memcpy( cu->tr_coeff_cr + i_tmp2, x265_data_cu_get_coeff_cr(cu_src), sizeof(x265_coeff_t)*i_tmp );
#if X265_ADAPTIVE_QP_SELECTION
	memcpy( cu->arl_coeff_cb + i_tmp2, x265_data_cu_get_arl_coeff_cb(cu_src), sizeof(int32_t)*i_tmp );
	memcpy( cu->arl_coeff_cr + i_tmp2, x265_data_cu_get_arl_coeff_cr(cu_src), sizeof(int32_t)*i_tmp );
#endif
	memcpy( cu->ipcm_sample_cb + i_tmp2, x265_data_cu_get_pcm_sample_cb(cu_src), sizeof(pixel) * i_tmp );
	memcpy( cu->ipcm_sample_cr + i_tmp2, x265_data_cu_get_pcm_sample_cr(cu_src), sizeof(pixel) * i_tmp );
	cu->i_total_bins += x265_data_cu_get_total_cost(cu_src);
	memcpy( cu->base_data_cu.slice_start_cu + i_offset,
			x265_base_data_cu_get_slice_start_cu_p1((x265_base_data_cu_t*)cu_src),
			sizeof( uint32_t ) * i_num_partition  );
	memcpy( cu->slice_segment_start_cu + i_offset,
			x265_data_cu_get_slice_segment_start_cu_p1(cu_src), sizeof( uint32_t ) * i_num_partition  );
}

// copy current predicted part to a cu in picture.
// it is used to predict for next part
void x265_data_cu_copy_to_pic_p3( x265_t *h, x265_data_cu_t* cu, uint8_t i_depth )
{
	uint32_t i_abs_idx_in_lcu = 0 ;
	uint32_t i_num_partition = 0 ;
	uint32_t i_tmp = 0 ;
	uint32_t i_tmp2 = 0 ;
	x265_data_cu_t* cu_dst = NULL ;

	cu_dst = h->data_cu;
	i_abs_idx_in_lcu = cu->base_data_cu.i_abs_idx_in_lcu ;
	i_num_partition = cu->base_data_cu.i_num_partition ;

	cu_dst->f_total_cost = cu->f_total_cost;
	cu_dst->i_total_distortion = cu->i_total_distortion;
	cu_dst->i_total_bits = cu->i_total_bits;

	memcpy( x265_base_data_cu_get_skip_flag_p1((x265_base_data_cu_t*)cu_dst)
			+ i_abs_idx_in_lcu,
			cu->base_data_cu.skip_flag,
			sizeof(int32_t) * i_num_partition );
	memcpy( x265_base_data_cu_get_qp_p1((x265_base_data_cu_t*)cu_dst)
			+ i_abs_idx_in_lcu,
			cu->base_data_cu.qp,
			sizeof(int8_t) * i_num_partition  );
	memcpy( x265_data_cu_get_partition_size_p1(cu_dst)
			+ i_abs_idx_in_lcu,
			cu->part_size,
			sizeof(int8_t) * i_num_partition );
	memcpy( x265_base_data_cu_get_prediction_mode_p1((x265_base_data_cu_t*)cu_dst)
			+ i_abs_idx_in_lcu,
			cu->base_data_cu.pred_mode,
			sizeof(int8_t) * i_num_partition );
	memcpy( x265_base_data_cu_get_cu_transquant_bypass_p1((x265_base_data_cu_t*)cu_dst)
			+ i_abs_idx_in_lcu,
			cu->base_data_cu.cu_transquant_bypass,
			sizeof(int32_t) * i_num_partition );
	memcpy( x265_data_cu_get_merge_flag_p1(cu_dst)
			+ i_abs_idx_in_lcu,
			cu->merge_flag,
			sizeof(int32_t) * i_num_partition );
	memcpy( x265_data_cu_get_merge_index_p1(cu_dst)
			+ i_abs_idx_in_lcu,
			cu->merge_index,
			sizeof(uint8_t) * i_num_partition );
	memcpy( x265_base_data_cu_get_luma_intra_dir_p1((x265_base_data_cu_t*)cu_dst)
			+ i_abs_idx_in_lcu,
			cu->base_data_cu.luma_intra_dir,
			sizeof(uint8_t) * i_num_partition );
	memcpy( x265_data_cu_get_chroma_intra_dir_p1(cu_dst)
			+ i_abs_idx_in_lcu,
			cu->chroma_intra_dir,
			sizeof(uint8_t) * i_num_partition );
	memcpy( x265_base_data_cu_get_inter_dir_p1((x265_base_data_cu_t*)cu_dst)
			+ i_abs_idx_in_lcu,
			cu->base_data_cu.inter_dir,
			sizeof(uint8_t) * i_num_partition );
	memcpy( x265_base_data_cu_get_transform_idx_p1((x265_base_data_cu_t*)cu_dst)
			+ i_abs_idx_in_lcu,
			cu->base_data_cu.tr_idx,
			sizeof(uint8_t) * i_num_partition );
	memcpy( x265_data_cu_get_transform_skip_p2(cu_dst, TEXT_LUMA)
			+ i_abs_idx_in_lcu,
			cu->transform_skip[0],
			sizeof(uint8_t) * i_num_partition );
	memcpy( x265_data_cu_get_transform_skip_p2(cu_dst, TEXT_CHROMA_U)
			+ i_abs_idx_in_lcu,
			cu->transform_skip[1],
			sizeof(uint8_t) * i_num_partition );
	memcpy( x265_data_cu_get_transform_skip_p2(cu_dst, TEXT_CHROMA_V)
			+ i_abs_idx_in_lcu,
			cu->transform_skip[2],
			sizeof(uint8_t) * i_num_partition );
	memcpy( x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu_dst, TEXT_LUMA)
			+ i_abs_idx_in_lcu,
			cu->base_data_cu.cbf[0],
			sizeof(uint8_t) * i_num_partition );
	memcpy( x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu_dst, TEXT_CHROMA_U)
			+ i_abs_idx_in_lcu,
			cu->base_data_cu.cbf[1],
			sizeof(uint8_t) * i_num_partition );
	memcpy( x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu_dst, TEXT_CHROMA_V)
			+ i_abs_idx_in_lcu,
			cu->base_data_cu.cbf[2],
			sizeof(uint8_t) * i_num_partition );
	memcpy( x265_base_data_cu_get_depth_p1((x265_base_data_cu_t*)cu_dst)
			+ i_abs_idx_in_lcu,
			cu->base_data_cu.depth,
			sizeof(uint8_t) * i_num_partition );
	memcpy( x265_data_cu_get_width_p1(cu_dst)
			+ i_abs_idx_in_lcu,
			cu->width,
			sizeof(uint8_t) * i_num_partition );
	memcpy( x265_data_cu_get_height_p1(cu_dst)
			+ i_abs_idx_in_lcu,
			cu->height,
			sizeof(uint8_t) * i_num_partition );
	memcpy( x265_data_cu_get_mvp_idx_p2(cu_dst, REF_PIC_LIST_0)
			+ i_abs_idx_in_lcu,
			cu->mvp_idx[0],
			sizeof(int8_t) * i_num_partition );
	memcpy( x265_data_cu_get_mvp_idx_p2(cu_dst, REF_PIC_LIST_1)
			+ i_abs_idx_in_lcu,
			cu->mvp_idx[1],
			sizeof(int8_t) * i_num_partition );
	memcpy( x265_data_cu_get_mvp_num_p2(cu_dst, REF_PIC_LIST_0)
			+ i_abs_idx_in_lcu,
			cu->mvp_num[0],
			sizeof(int8_t) * i_num_partition );
	memcpy( x265_data_cu_get_mvp_num_p2(cu_dst, REF_PIC_LIST_1)
			+ i_abs_idx_in_lcu,
			cu->mvp_num[1],
			sizeof(int8_t) * i_num_partition );

	x265_cu_mv_field_copy_to_p3 ( &cu->base_data_cu.cu_mv_field[0],
									x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu_dst,
																		REF_PIC_LIST_0),
									cu->base_data_cu.i_abs_idx_in_lcu );
	x265_cu_mv_field_copy_to_p3 ( &cu->base_data_cu.cu_mv_field[1],
									x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu_dst,
																		REF_PIC_LIST_1),
									i_abs_idx_in_lcu );


	memcpy( x265_base_data_cu_get_ipcm_flag_p1((x265_base_data_cu_t*)cu_dst)
			+ i_abs_idx_in_lcu,
			cu->base_data_cu.ipcm_flag,
			sizeof(int32_t) * i_num_partition );

	i_tmp  = (h->cu.pic.i_max_cu_width * h->cu.pic.i_max_cu_height) >> (i_depth << 1) ;
	i_tmp2 = i_abs_idx_in_lcu * h->cu.pic.i_min_cu_width * h->cu.pic.i_min_cu_height ;
	memcpy( x265_data_cu_get_coeff_y(cu_dst) + i_tmp2, cu->tr_coeff_y,  sizeof(x265_coeff_t)*i_tmp  );
#if X265_ADAPTIVE_QP_SELECTION
	memcpy( x265_data_cu_get_arl_coeff_y(cu_dst)  + i_tmp2, cu->arl_coeff_y,  sizeof(int32_t)*i_tmp  );
#endif
	memcpy( x265_data_cu_get_pcm_sample_y(cu_dst) + i_tmp2 , cu->ipcm_sample_y, sizeof(pixel)*i_tmp );

	i_tmp >>= 2; i_tmp2 >>= 2;
	memcpy( x265_data_cu_get_coeff_cb(cu_dst) + i_tmp2, cu->tr_coeff_cb, sizeof(x265_coeff_t)*i_tmp  );
	memcpy( x265_data_cu_get_coeff_cr(cu_dst) + i_tmp2, cu->tr_coeff_cr, sizeof(x265_coeff_t)*i_tmp  );
#if X265_ADAPTIVE_QP_SELECTION
	memcpy( x265_data_cu_get_arl_coeff_cb(cu_dst) + i_tmp2, cu->arl_coeff_cb, sizeof(int32_t)*i_tmp  );
	memcpy( x265_data_cu_get_arl_coeff_cr(cu_dst) + i_tmp2, cu->arl_coeff_cr, sizeof(int32_t)*i_tmp  );
#endif
	memcpy( x265_data_cu_get_pcm_sample_cb(cu_dst) + i_tmp2 , cu->ipcm_sample_cb, sizeof( pixel ) * i_tmp );
	memcpy( x265_data_cu_get_pcm_sample_cr(cu_dst) + i_tmp2 , cu->ipcm_sample_cr, sizeof( pixel ) * i_tmp );
	cu_dst->i_total_bins = cu->i_total_bins;
	memcpy( cu_dst->base_data_cu.slice_start_cu + i_abs_idx_in_lcu,
			cu->base_data_cu.slice_start_cu,
			sizeof(uint32_t) * i_num_partition  );
	memcpy( cu_dst->slice_segment_start_cu + i_abs_idx_in_lcu,
			cu->slice_segment_start_cu,
			sizeof(uint32_t) * i_num_partition  );
}

void x265_data_cu_copy_to_pic_p5 ( x265_t *h,
								x265_data_cu_t* cu,
								uint8_t i_depth,
								uint32_t i_part_idx,
								uint32_t i_part_depth )
{
	x265_data_cu_t *cu_dst = NULL ;
	uint32_t i_q_num_part = 0 ;
	uint32_t i_part_start = 0 ;
	uint32_t i_part_offset = 0 ;
	uint32_t i_tmp = 0 ;
	uint32_t i_tmp2 = 0 ;

	cu_dst = h->data_cu ;
	i_q_num_part = cu->base_data_cu.i_num_partition >> (i_part_depth << 1) ;
	i_part_start = i_part_idx * i_q_num_part ;
	i_part_offset = cu->base_data_cu.i_abs_idx_in_lcu + i_part_start;

	cu_dst->f_total_cost = cu->f_total_cost;
	cu_dst->i_total_distortion = cu->i_total_distortion;
	cu_dst->i_total_bits = cu->i_total_bits;

	memcpy( x265_base_data_cu_get_skip_flag_p1((x265_base_data_cu_t*)cu_dst)
			+ i_part_offset,
			cu->base_data_cu.skip_flag,
			sizeof(int32_t) * i_q_num_part );
	memcpy( x265_base_data_cu_get_qp_p1((x265_base_data_cu_t*)cu_dst)
			+ i_part_offset,
			cu->base_data_cu.qp,
			sizeof(int8_t) * i_q_num_part );
	memcpy( x265_data_cu_get_partition_size_p1(cu_dst)
			+ i_part_offset,
			cu->part_size,
			sizeof(int8_t) * i_q_num_part );
	memcpy( x265_base_data_cu_get_prediction_mode_p1((x265_base_data_cu_t*)cu_dst)
			+ i_part_offset,
			cu->base_data_cu.pred_mode,
			sizeof(int8_t) * i_q_num_part );
	memcpy( x265_base_data_cu_get_cu_transquant_bypass_p1((x265_base_data_cu_t*)cu_dst)
			+ i_part_offset,
			cu->base_data_cu.cu_transquant_bypass,
			sizeof(int32_t) * i_q_num_part );
	memcpy( x265_data_cu_get_merge_flag_p1(cu_dst)
			+ i_part_offset,
			cu->merge_flag,
			sizeof(int32_t) * i_q_num_part );
	memcpy( x265_data_cu_get_merge_index_p1(cu_dst)
			+ i_part_offset,
			cu->merge_index,
			sizeof(uint8_t) * i_q_num_part );
	memcpy( x265_base_data_cu_get_luma_intra_dir_p1((x265_base_data_cu_t*)cu_dst)
			+ i_part_offset,
			cu->base_data_cu.luma_intra_dir,
			sizeof(uint8_t) * i_q_num_part );
	memcpy( x265_data_cu_get_chroma_intra_dir_p1(cu_dst)
			+ i_part_offset,
			cu->chroma_intra_dir,
			sizeof(uint8_t) * i_q_num_part );
	memcpy( x265_base_data_cu_get_inter_dir_p1((x265_base_data_cu_t*)cu_dst)
			+ i_part_offset,
			cu->base_data_cu.inter_dir,
			sizeof(uint8_t) * i_q_num_part );
	memcpy( x265_base_data_cu_get_transform_idx_p1((x265_base_data_cu_t*)cu_dst)
			+ i_part_offset,
			cu->base_data_cu.tr_idx,
			sizeof(uint8_t) * i_q_num_part );
	memcpy( x265_data_cu_get_transform_skip_p2(cu_dst, TEXT_LUMA)
			+ i_part_offset,
			cu->transform_skip[0],
			sizeof(uint8_t) * i_q_num_part );
	memcpy( x265_data_cu_get_transform_skip_p2(cu_dst, TEXT_CHROMA_U)
			+ i_part_offset,
			cu->transform_skip[1],
			sizeof(uint8_t) * i_q_num_part );
	memcpy( x265_data_cu_get_transform_skip_p2(cu_dst, TEXT_CHROMA_V)
			+ i_part_offset,
			cu->transform_skip[2],
			sizeof(uint8_t) * i_q_num_part );
	memcpy( x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu_dst, TEXT_LUMA)
			+ i_part_offset,
			cu->base_data_cu.cbf[0],
			sizeof(uint8_t) * i_q_num_part );
	memcpy( x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu_dst, TEXT_CHROMA_U) + i_part_offset,
			cu->base_data_cu.cbf[1],
			sizeof(uint8_t) * i_q_num_part );
	memcpy( x265_base_data_cu_get_cbf_p2((x265_base_data_cu_t*)cu_dst, TEXT_CHROMA_V)
			+ i_part_offset,
			cu->base_data_cu.cbf[2],
			sizeof(uint8_t) * i_q_num_part );
	memcpy( x265_base_data_cu_get_depth_p1((x265_base_data_cu_t*)cu_dst)
			+ i_part_offset,
			cu->base_data_cu.depth,
			sizeof(uint8_t) * i_q_num_part );
	memcpy( x265_data_cu_get_width_p1(cu_dst)
			+ i_part_offset,
			cu->width,
			sizeof(uint8_t) * i_q_num_part );
	memcpy( x265_data_cu_get_height_p1(cu_dst)
			+ i_part_offset,
			cu->height,
			sizeof(uint8_t) * i_q_num_part );
	memcpy( x265_data_cu_get_mvp_idx_p2(cu_dst, REF_PIC_LIST_0)
			+ i_part_offset,
			cu->mvp_idx[0],
			sizeof(int8_t) * i_q_num_part );
	memcpy( x265_data_cu_get_mvp_idx_p2(cu_dst, REF_PIC_LIST_1)
			+ i_part_offset,
			cu->mvp_idx[1],
			sizeof(int8_t) * i_q_num_part );
	memcpy( x265_data_cu_get_mvp_num_p2(cu_dst, REF_PIC_LIST_0)
			+ i_part_offset,
			cu->mvp_num[0],
			sizeof(int8_t) * i_q_num_part );
	memcpy( x265_data_cu_get_mvp_num_p2(cu_dst, REF_PIC_LIST_1)
			+ i_part_offset,
			cu->mvp_num[1],
			sizeof(int8_t) * i_q_num_part );

	x265_cu_mv_field_copy_to_p5 ( &cu->base_data_cu.cu_mv_field[0],
									x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu_dst,
																		REF_PIC_LIST_0),
									cu->base_data_cu.i_abs_idx_in_lcu,
									i_part_start,
									i_q_num_part );
	x265_cu_mv_field_copy_to_p5 ( &cu->base_data_cu.cu_mv_field[1],
									x265_base_data_cu_get_cu_mv_field((x265_base_data_cu_t*)cu_dst,
																		REF_PIC_LIST_1),
									cu->base_data_cu.i_abs_idx_in_lcu,
									i_part_start,
									i_q_num_part );

	memcpy( x265_base_data_cu_get_ipcm_flag_p1((x265_base_data_cu_t*)cu_dst)
			+ i_part_offset,
			cu->base_data_cu.ipcm_flag,
			sizeof(int32_t) * i_q_num_part );

	i_tmp = (h->cu.pic.i_max_cu_width * h->cu.pic.i_max_cu_height)
			>> ((i_depth + i_part_depth) << 1) ;
	i_tmp2 = i_part_offset * h->cu.pic.i_min_cu_width * h->cu.pic.i_min_cu_height ;
	memcpy( x265_data_cu_get_coeff_y(cu_dst)  + i_tmp2, cu->tr_coeff_y,  sizeof(x265_coeff_t)*i_tmp  );
#if X265_ADAPTIVE_QP_SELECTION
	memcpy( x265_data_cu_get_arl_coeff_y(cu_dst) + i_tmp2, cu->arl_coeff_y,  sizeof(int32_t)*i_tmp  );
#endif

	memcpy( x265_data_cu_get_pcm_sample_y(cu_dst) + i_tmp2 , cu->ipcm_sample_y, sizeof( pixel ) * i_tmp );

	i_tmp >>= 2; i_tmp2 >>= 2;
	memcpy( x265_data_cu_get_coeff_cb(cu_dst) + i_tmp2, cu->tr_coeff_cb, sizeof(x265_coeff_t)*i_tmp  );
	memcpy( x265_data_cu_get_coeff_cr(cu_dst) + i_tmp2, cu->tr_coeff_cr, sizeof(x265_coeff_t)*i_tmp  );
#if X265_ADAPTIVE_QP_SELECTION
	memcpy( x265_data_cu_get_arl_coeff_cb(cu_dst) + i_tmp2, cu->arl_coeff_cb, sizeof(int32_t)*i_tmp  );
	memcpy( x265_data_cu_get_arl_coeff_cr(cu_dst) + i_tmp2, cu->arl_coeff_cr, sizeof(int32_t)*i_tmp  );
#endif

	memcpy( x265_data_cu_get_pcm_sample_cb(cu_dst) + i_tmp2 , cu->ipcm_sample_cb, sizeof( pixel ) * i_tmp );
	memcpy( x265_data_cu_get_pcm_sample_cr(cu_dst) + i_tmp2 , cu->ipcm_sample_cr, sizeof( pixel ) * i_tmp );
	cu_dst->i_total_bins = cu->i_total_bins;
	memcpy( cu_dst->base_data_cu.slice_start_cu + i_part_offset,
			cu->base_data_cu.slice_start_cu,
			sizeof(uint32_t) * i_q_num_part );
	memcpy( cu_dst->slice_segment_start_cu + i_part_offset,
			cu->slice_segment_start_cu,
			sizeof(uint32_t) * i_q_num_part  );
}

x265_base_data_cu_t* x265_data_cu_get_pu_above_left ( x265_t *h,
													x265_data_cu_t *cu,
													uint32_t *p_al_part_unit_idx,
													uint32_t i_curr_part_unit_idx,
													int32_t b_enforce_slice_restriction )
{
	uint32_t i_abs_part_idx = 0 ;
	uint32_t i_abs_zorder_cu_idx = 0 ;
	uint32_t i_num_part_in_cu_width = 0 ;

	i_abs_part_idx = h->scan.zscan_to_raster[i_curr_part_unit_idx];
	i_abs_zorder_cu_idx = h->scan.zscan_to_raster[cu->base_data_cu.i_abs_idx_in_lcu];
	i_num_part_in_cu_width = h->cu.pic.i_num_part_in_width;

	if ( !x265_raster_address_is_zero_col( i_abs_part_idx, i_num_part_in_cu_width ) )
	{
		if ( !x265_raster_address_is_zero_row( i_abs_part_idx, i_num_part_in_cu_width ) )
		{
			*p_al_part_unit_idx = h->scan.raster_to_zscan[ i_abs_part_idx - i_num_part_in_cu_width - 1 ];
			if ( x265_raster_address_is_equal_row_or_col( i_abs_part_idx, i_abs_zorder_cu_idx, i_num_part_in_cu_width ) )
			{
				return (x265_base_data_cu_t*)h->data_cu;
			}
			else
			{
				*p_al_part_unit_idx -= cu->base_data_cu.i_abs_idx_in_lcu;
				return (x265_base_data_cu_t*)cu;
			}
		}
		*p_al_part_unit_idx = h->scan.raster_to_zscan[ i_abs_part_idx + h->cu.pic.i_num_partitions - i_num_part_in_cu_width - 1 ];
		if (( b_enforce_slice_restriction &&
				(cu->base_data_cu.cu_above == NULL
				|| x265_base_data_cu_get_scu_addr(h, cu->base_data_cu.cu_above) + *p_al_part_unit_idx
				< x265_base_data_cu_get_slice_start_cu_p2((x265_base_data_cu_t*)h->data_cu, i_curr_part_unit_idx)
				|| (h->tile_idx_map[x265_base_data_cu_get_addr(cu->base_data_cu.cu_above)]
					!= h->tile_idx_map[x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu)]
					)
				)
			)
		)
		{
			return NULL;
		}
		return cu->base_data_cu.cu_above;
	}

	if ( !x265_raster_address_is_zero_row( i_abs_part_idx, i_num_part_in_cu_width ) )
	{
		*p_al_part_unit_idx = h->scan.raster_to_zscan[ i_abs_part_idx - 1 ];
		if (( b_enforce_slice_restriction &&
				(cu->base_data_cu.cu_left == NULL
				|| x265_base_data_cu_get_scu_addr(h, cu->base_data_cu.cu_left) + *p_al_part_unit_idx
				< x265_base_data_cu_get_slice_start_cu_p2((x265_base_data_cu_t*)h->data_cu, i_curr_part_unit_idx)
				|| (h->tile_idx_map[x265_base_data_cu_get_addr(cu->base_data_cu.cu_left)]
				 != h->tile_idx_map[x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu)]
					)
				)
			)
		)
		{
			return NULL;
		}
		return cu->base_data_cu.cu_left;
	}

	*p_al_part_unit_idx = h->scan.raster_to_zscan[h->cu.pic.i_num_partitions - 1];
	if (( b_enforce_slice_restriction &&
			(cu->cu_above_left == NULL
			|| x265_base_data_cu_get_scu_addr(h, cu->cu_above_left) + *p_al_part_unit_idx
			< x265_base_data_cu_get_slice_start_cu_p2((x265_base_data_cu_t*)h->data_cu, i_curr_part_unit_idx)
			|| (h->tile_idx_map[x265_base_data_cu_get_addr(cu->cu_above_left)]
			 != h->tile_idx_map[x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu)]
				)
			)
		)
	)
	{
		return NULL;
	}
	return cu->cu_above_left;
}

x265_base_data_cu_t* x265_data_cu_get_pu_above_right ( x265_t *h,
													x265_data_cu_t *cu,
													uint32_t *p_ar_part_unit_idx,
													uint32_t i_curr_part_unit_idx,
													int32_t b_enforce_slice_restriction )
{
	uint32_t i_abs_part_idx_rt = 0 ;
	uint32_t i_abs_zorder_cu_idx = 0 ;
	uint32_t i_num_part_in_cu_width = 0 ;

	i_abs_part_idx_rt = h->scan.zscan_to_raster[i_curr_part_unit_idx];
	i_abs_zorder_cu_idx = h->scan.zscan_to_raster[cu->base_data_cu.i_abs_idx_in_lcu]
	                      + cu->width[0] / h->cu.pic.i_min_cu_width - 1;
	i_num_part_in_cu_width = h->cu.pic.i_num_part_in_width;
	if( ( x265_data_cu_get_cu_pel_x(h->data_cu)
			+ h->scan.raster_to_pel_x[i_abs_part_idx_rt]
			+ h->cu.pic.i_min_cu_width ) >= h->sps[0].i_pic_width_in_luma_samples )
	{
		*p_ar_part_unit_idx = X265_MAX_UINT;
		return NULL;
	}

	if ( x265_raster_address_less_than_col( i_abs_part_idx_rt, i_num_part_in_cu_width - 1, i_num_part_in_cu_width ) )
	{
		if ( !x265_raster_address_is_zero_row( i_abs_part_idx_rt, i_num_part_in_cu_width ) )
		{
			if ( i_curr_part_unit_idx > h->scan.raster_to_zscan[ i_abs_part_idx_rt - i_num_part_in_cu_width + 1 ] )
			{
				*p_ar_part_unit_idx = h->scan.raster_to_zscan[ i_abs_part_idx_rt - i_num_part_in_cu_width + 1 ];
				if ( x265_raster_address_is_equal_row_or_col( i_abs_part_idx_rt, i_abs_zorder_cu_idx, i_num_part_in_cu_width ) )
				{
					return (x265_base_data_cu_t*)h->data_cu;
				}
				else
				{
					*p_ar_part_unit_idx -= cu->base_data_cu.i_abs_idx_in_lcu;
					return (x265_base_data_cu_t*)cu;
				}
			}
			*p_ar_part_unit_idx = X265_MAX_UINT;
			return NULL;
		}
		*p_ar_part_unit_idx = h->scan.raster_to_zscan[i_abs_part_idx_rt
		                                              + h->cu.pic.i_num_partitions
		                                              - i_num_part_in_cu_width + 1 ];
		if (( b_enforce_slice_restriction &&
				(cu->base_data_cu.cu_above == NULL
				|| x265_base_data_cu_get_scu_addr(h, cu->base_data_cu.cu_above) + *p_ar_part_unit_idx
				< x265_base_data_cu_get_slice_start_cu_p2((x265_base_data_cu_t*)h->data_cu, i_curr_part_unit_idx)
				|| (h->tile_idx_map[x265_base_data_cu_get_addr(cu->base_data_cu.cu_above)]
				 != h->tile_idx_map[x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu)]
					)
				)
			)
		)
		{
			return NULL;
		}
		return cu->base_data_cu.cu_above;
	}

	if ( !x265_raster_address_is_zero_row( i_abs_part_idx_rt, i_num_part_in_cu_width ) )
	{
		*p_ar_part_unit_idx = X265_MAX_UINT;
		return NULL;
	}

	*p_ar_part_unit_idx = h->scan.raster_to_zscan[h->cu.pic.i_num_partitions
	                                               - i_num_part_in_cu_width];
	if (( b_enforce_slice_restriction &&
			(cu->cu_above_right == NULL
			|| h->inverse_cu_order_map[x265_base_data_cu_get_addr(cu->cu_above_right)]
			> h->inverse_cu_order_map[x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu)]
			|| x265_base_data_cu_get_scu_addr(h, cu->cu_above_right) + *p_ar_part_unit_idx
			< x265_base_data_cu_get_slice_start_cu_p2((x265_base_data_cu_t*)h->data_cu, i_curr_part_unit_idx)
			|| (h->tile_idx_map[x265_base_data_cu_get_addr(cu->cu_above_right)]
			 != h->tile_idx_map[x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu)]
				)
			)
		)
	)
	{
		return NULL;
	}
	return cu->cu_above_right;
}

x265_base_data_cu_t* x265_data_cu_get_pu_below_left ( x265_t *h,
													x265_data_cu_t *cu,
													uint32_t *p_bl_part_unit_idx,
													uint32_t i_curr_part_unit_idx,
													int32_t b_enforce_slice_restriction )
{
	uint32_t i_abs_part_idx_lb = 0 ;
	uint32_t i_abs_zorder_cu_idx_lb = 0 ;
	uint32_t i_num_part_in_cu_width = 0 ;

	i_abs_part_idx_lb = h->scan.zscan_to_raster[i_curr_part_unit_idx];
	i_abs_zorder_cu_idx_lb = h->scan.zscan_to_raster[cu->base_data_cu.i_abs_idx_in_lcu]
	                         + (cu->height[0] / h->cu.pic.i_min_cu_height - 1)
	                         * h->cu.pic.i_num_part_in_width;
	i_num_part_in_cu_width = h->cu.pic.i_num_part_in_width;
	if( ( x265_data_cu_get_cu_pel_y(h->data_cu)
			+ h->scan.raster_to_pel_y[i_abs_part_idx_lb]
			+ h->cu.pic.i_min_cu_height )
			>= h->sps[0].i_pic_height_in_luma_samples )
	{
		*p_bl_part_unit_idx = X265_MAX_UINT;
		return NULL;
	}

	if ( x265_raster_address_less_than_row( i_abs_part_idx_lb,
											h->cu.pic.i_num_part_in_height - 1,
											i_num_part_in_cu_width ) )
	{
		if ( !x265_raster_address_is_zero_col( i_abs_part_idx_lb, i_num_part_in_cu_width ) )
		{
			if ( i_curr_part_unit_idx > h->scan.raster_to_zscan[ i_abs_part_idx_lb + i_num_part_in_cu_width - 1 ] )
			{
				*p_bl_part_unit_idx = h->scan.raster_to_zscan[ i_abs_part_idx_lb + i_num_part_in_cu_width - 1 ];
				if ( x265_raster_address_is_equal_row_or_col( i_abs_part_idx_lb, i_abs_zorder_cu_idx_lb, i_num_part_in_cu_width ) )
				{
					return (x265_base_data_cu_t*)h->data_cu;
				}
				else
				{
					*p_bl_part_unit_idx -= cu->base_data_cu.i_abs_idx_in_lcu;
					return (x265_base_data_cu_t*)cu;
				}
			}
			*p_bl_part_unit_idx = X265_MAX_UINT;
			return NULL;
		}
		*p_bl_part_unit_idx = h->scan.raster_to_zscan[ i_abs_part_idx_lb + i_num_part_in_cu_width*2 - 1 ];
		if (( b_enforce_slice_restriction &&
				(cu->base_data_cu.cu_left == NULL
				|| x265_base_data_cu_get_scu_addr(h, cu->base_data_cu.cu_left) + *p_bl_part_unit_idx
				< x265_base_data_cu_get_slice_start_cu_p2((x265_base_data_cu_t*)h->data_cu, i_curr_part_unit_idx)
				|| (h->tile_idx_map[x265_base_data_cu_get_addr(cu->base_data_cu.cu_left)]
				 != h->tile_idx_map[x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu)]
					)
				)
			)
		)
		{
			return NULL;
		}
		return cu->base_data_cu.cu_left;
	}

	*p_bl_part_unit_idx = X265_MAX_UINT;
	return NULL;
}

x265_base_data_cu_t* x265_data_cu_get_pu_below_left_adi ( x265_t *h,
														x265_data_cu_t *cu,
														uint32_t *p_bl_part_unit_idx,
														uint32_t i_curr_part_unit_idx,
														uint32_t i_part_unit_offset,
														int32_t b_enforce_slice_restriction )
{
	uint32_t i_abs_part_idx_lb = 0 ;
	uint32_t i_abs_zorder_cu_idx_lb = 0 ;
	uint32_t i_num_part_in_cu_width = 0 ;

	i_abs_part_idx_lb = h->scan.zscan_to_raster[i_curr_part_unit_idx];
	i_abs_zorder_cu_idx_lb = h->scan.zscan_to_raster[cu->base_data_cu.i_abs_idx_in_lcu]
	                         + ((cu->height[0] / h->cu.pic.i_min_cu_height) - 1)
	                         * h->cu.pic.i_num_part_in_width;
	i_num_part_in_cu_width = h->cu.pic.i_num_part_in_width;
	if( ( x265_data_cu_get_cu_pel_y(h->data_cu)
			+ h->scan.raster_to_pel_y[i_abs_part_idx_lb]
			+ (h->cu.pic.i_min_cu_height * i_part_unit_offset))
			>= h->sps[0].i_pic_height_in_luma_samples )
	{
		*p_bl_part_unit_idx = X265_MAX_UINT;
		return NULL;
	}

	if ( x265_raster_address_less_than_row( i_abs_part_idx_lb,
											h->cu.pic.i_num_part_in_height - i_part_unit_offset,
											i_num_part_in_cu_width ) )
	{
		if ( !x265_raster_address_is_zero_col( i_abs_part_idx_lb, i_num_part_in_cu_width ) )
		{
			if ( i_curr_part_unit_idx > h->scan.raster_to_zscan[ i_abs_part_idx_lb + i_part_unit_offset * i_num_part_in_cu_width - 1 ] )
			{
				*p_bl_part_unit_idx = h->scan.raster_to_zscan[ i_abs_part_idx_lb + i_part_unit_offset * i_num_part_in_cu_width - 1 ];
				if ( x265_raster_address_is_equal_row_or_col( i_abs_part_idx_lb, i_abs_zorder_cu_idx_lb, i_num_part_in_cu_width ) )
				{
					return (x265_base_data_cu_t*)h->data_cu;
				}
				else
				{
					*p_bl_part_unit_idx -= cu->base_data_cu.i_abs_idx_in_lcu;
					return (x265_base_data_cu_t*)cu;
				}
			}
			*p_bl_part_unit_idx = X265_MAX_UINT;
			return NULL;
		}
		*p_bl_part_unit_idx = h->scan.raster_to_zscan[ i_abs_part_idx_lb + (1+i_part_unit_offset) * i_num_part_in_cu_width - 1 ];
		if (( b_enforce_slice_restriction &&
				(cu->base_data_cu.cu_left == NULL
				|| x265_base_data_cu_get_scu_addr(h, cu->base_data_cu.cu_left) + *p_bl_part_unit_idx
				< x265_base_data_cu_get_slice_start_cu_p2((x265_base_data_cu_t*)h->data_cu, i_curr_part_unit_idx)
				|| (h->tile_idx_map[x265_base_data_cu_get_addr(cu->base_data_cu.cu_left)]
				 != h->tile_idx_map[x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu)]
					)
				)
			)
		)
		{
			return NULL;
		}
		return cu->base_data_cu.cu_left;
	}

	*p_bl_part_unit_idx = X265_MAX_UINT;
	return NULL;
}

x265_base_data_cu_t* x265_data_cu_get_pu_above_right_adi ( x265_t *h,
														x265_data_cu_t *cu,
														uint32_t *p_ar_part_unit_idx,
														uint32_t i_curr_part_unit_idx,
														uint32_t i_part_unit_offset,
														int32_t b_enforce_slice_restriction )
{
	uint32_t i_abs_part_idx_rt = 0 ;
	uint32_t i_abs_zorder_cu_idx = 0 ;
	uint32_t i_num_part_in_cu_width = 0 ;

	i_abs_part_idx_rt = h->scan.zscan_to_raster[i_curr_part_unit_idx];
	i_abs_zorder_cu_idx = h->scan.zscan_to_raster[ cu->base_data_cu.i_abs_idx_in_lcu ]
	                      + (cu->width[0] / h->cu.pic.i_min_cu_width) - 1;
	i_num_part_in_cu_width = h->cu.pic.i_num_part_in_width;

	if( ( x265_data_cu_get_cu_pel_x(h->data_cu)
			+ h->scan.raster_to_pel_x[i_abs_part_idx_rt]
			+ (h->cu.pic.i_min_cu_height * i_part_unit_offset))
			>= h->sps[0].i_pic_width_in_luma_samples )
	{
		*p_ar_part_unit_idx = X265_MAX_UINT;
		return NULL;
	}

	if ( x265_raster_address_less_than_col( i_abs_part_idx_rt, i_num_part_in_cu_width - i_part_unit_offset, i_num_part_in_cu_width ) )
	{
		if ( !x265_raster_address_is_zero_row( i_abs_part_idx_rt, i_num_part_in_cu_width ) )
		{
			if ( i_curr_part_unit_idx > h->scan.raster_to_zscan[ i_abs_part_idx_rt - i_num_part_in_cu_width + i_part_unit_offset ] )
			{
				*p_ar_part_unit_idx = h->scan.raster_to_zscan[ i_abs_part_idx_rt - i_num_part_in_cu_width + i_part_unit_offset ];
				if ( x265_raster_address_is_equal_row_or_col( i_abs_part_idx_rt, i_abs_zorder_cu_idx, i_num_part_in_cu_width ) )
				{
					return (x265_base_data_cu_t*)h->data_cu;
				}
				else
				{
					*p_ar_part_unit_idx -= cu->base_data_cu.i_abs_idx_in_lcu;
					return (x265_base_data_cu_t*)cu;
				}
			}
			*p_ar_part_unit_idx = X265_MAX_UINT;
			return NULL;
		}
		*p_ar_part_unit_idx = h->scan.raster_to_zscan[ i_abs_part_idx_rt
		                                               + h->cu.pic.i_num_partitions
		                                               - i_num_part_in_cu_width
		                                               + i_part_unit_offset ];
		if (( b_enforce_slice_restriction &&
				(cu->base_data_cu.cu_above == NULL
				|| x265_base_data_cu_get_scu_addr(h, cu->base_data_cu.cu_above) + *p_ar_part_unit_idx
				< x265_base_data_cu_get_slice_start_cu_p2((x265_base_data_cu_t*)h->data_cu, i_curr_part_unit_idx)
				|| (h->tile_idx_map[x265_base_data_cu_get_addr(cu->base_data_cu.cu_above)]
				 != h->tile_idx_map[x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu)]
					)
				)
			)
		)
		{
			return NULL;
		}
		return cu->base_data_cu.cu_above;
	}

	if ( !x265_raster_address_is_zero_row( i_abs_part_idx_rt, i_num_part_in_cu_width ) )
	{
		*p_ar_part_unit_idx = X265_MAX_UINT;
		return NULL;
	}

	*p_ar_part_unit_idx = h->scan.raster_to_zscan[h->cu.pic.i_num_partitions
	                                              - i_num_part_in_cu_width
	                                              + i_part_unit_offset - 1 ];
	if (( b_enforce_slice_restriction &&
			(cu->cu_above_right == NULL
			|| h->inverse_cu_order_map[x265_base_data_cu_get_addr(cu->cu_above_right)]
			> h->inverse_cu_order_map[x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu)]
			|| x265_base_data_cu_get_scu_addr(h, cu->cu_above_right) + *p_ar_part_unit_idx
			< x265_base_data_cu_get_slice_start_cu_p2((x265_base_data_cu_t*)h->data_cu, i_curr_part_unit_idx)
			|| (h->tile_idx_map[x265_base_data_cu_get_addr(cu->cu_above_right)]
			 != h->tile_idx_map[x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu)]
				)
			)
		)
	)
	{
		return NULL;
	}
	return cu->cu_above_right;
}

/** Get left QpMinCu
*\param   uiLPartUnitIdx
*\param   i_curr_abs_idx_in_lcu
*\returns TComDataCU*   point of TComDataCU of left QpMinCu
*/
x265_data_cu_t *x265_data_cu_get_qp_min_cu_left ( x265_t *h,
												x265_data_cu_t *cu,
												uint32_t *p_part_unit_idx,
												uint32_t i_curr_abs_idx_in_lcu )
{
	uint32_t i_num_part_in_cu_width = 0 ;
	uint32_t i_abs_zorder_qp_min_cu_idx = 0 ;
	uint32_t i_abs_rorder_qp_min_cu_idx = 0 ;


	i_num_part_in_cu_width = h->cu.pic.i_width_in_cu ;
	i_abs_zorder_qp_min_cu_idx = (i_curr_abs_idx_in_lcu >> ((h->cu.pic.i_total_depth - h->pps[0].i_max_cu_dqp_depth)<<1))<<((h->cu.pic.i_total_depth - h->pps[0].i_max_cu_dqp_depth)<<1);
	i_abs_rorder_qp_min_cu_idx = h->scan.zscan_to_raster[i_abs_zorder_qp_min_cu_idx];

	// check for left LCU boundary
	if ( x265_raster_address_is_zero_col(i_abs_rorder_qp_min_cu_idx, i_num_part_in_cu_width) )
	{
		return NULL;
	}

	// get index of left-CU relative to top-left corner of current quantization group
	*p_part_unit_idx = h->scan.raster_to_zscan[i_abs_rorder_qp_min_cu_idx - 1];

	// return pointer to current LCU
	return h->data_cu;
}

/** Get Above QpMinCu
*\param   aPartUnitIdx
*\param   currAbsIdxInLCU
*\returns TComDataCU*   point of TComDataCU of above QpMinCu
*/
x265_data_cu_t *x265_data_cu_get_qp_min_cu_above ( x265_t *h,
													x265_data_cu_t *cu,
													uint32_t *p_part_unit_idx,
													uint32_t i_curr_abs_idx_in_lcu )
{
	uint32_t i_num_part_in_cu_width = 0 ;
	uint32_t i_abs_zorder_qp_min_cu_idx = 0 ;
	uint32_t i_abs_rorder_qp_min_cu_idx = 0 ;


	i_num_part_in_cu_width = h->cu.pic.i_width_in_cu ;
	i_abs_zorder_qp_min_cu_idx = (i_curr_abs_idx_in_lcu >> ((h->cu.pic.i_total_depth - h->pps[0].i_max_cu_dqp_depth)<<1))<<((h->cu.pic.i_total_depth - h->pps[0].i_max_cu_dqp_depth)<<1);
	i_abs_rorder_qp_min_cu_idx = h->scan.zscan_to_raster[i_abs_zorder_qp_min_cu_idx];

	// check for top LCU boundary
	if ( x265_raster_address_is_zero_row( i_abs_rorder_qp_min_cu_idx, i_num_part_in_cu_width) )
	{
		return NULL;
	}

	// get index of top-CU relative to top-left corner of current quantization group
	*p_part_unit_idx = h->scan.raster_to_zscan[i_abs_rorder_qp_min_cu_idx - i_num_part_in_cu_width];

	// return pointer to current LCU
	return h->data_cu ;
}

int8_t x265_data_cu_get_ref_qp( x265_t *h, x265_data_cu_t *cu, uint32_t i_curr_abs_idx_in_lcu )
{
	x265_data_cu_t *cu_left = NULL ;
	x265_data_cu_t *cu_above = NULL ;
	uint32_t i_left_part_idx = 0 ;
	uint32_t i_above_part_idx = 0;

	cu_left = x265_data_cu_get_qp_min_cu_left ( h,
												cu,
												&i_left_part_idx,
												cu->base_data_cu.i_abs_idx_in_lcu + i_curr_abs_idx_in_lcu ) ;
	cu_above = x265_data_cu_get_qp_min_cu_above ( h,
												cu,
												&i_above_part_idx,
												cu->base_data_cu.i_abs_idx_in_lcu + i_curr_abs_idx_in_lcu ) ;
	return (((cu_left ? x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu_left, i_left_part_idx)
					: x265_base_data_cu_get_last_coded_qp(h, (x265_base_data_cu_t*)cu, i_curr_abs_idx_in_lcu ))
			+ (cu_above ? x265_base_data_cu_get_qp_p2((x265_base_data_cu_t*)cu_above, i_above_part_idx)
					: x265_base_data_cu_get_last_coded_qp(h, (x265_base_data_cu_t*)cu, i_curr_abs_idx_in_lcu ))
					  + 1) >> 1);
}

void x265_data_cu_get_allowed_chroma_dir( x265_data_cu_t* cu,
										uint32_t i_abs_part_idx,
										uint32_t* mode_list )
{
	int32_t loop = 0 ;
	uint32_t i_luma_mode = 0 ;

	mode_list[0] = X265_PLANAR_IDX;
	mode_list[1] = X265_VER_IDX;
	mode_list[2] = X265_HOR_IDX;
	mode_list[3] = X265_DC_IDX;
	mode_list[4] = X265_DM_CHROMA_IDX;

	i_luma_mode = x265_base_data_cu_get_luma_intra_dir_p2((x265_base_data_cu_t*)cu, i_abs_part_idx );

	for( loop = 0; loop < X265_NUM_CHROMA_MODE - 1; ++ loop )
	{
		if( i_luma_mode == mode_list[loop] )
		{
			mode_list[loop] = 34; // ver+8 mode
			break;
		}
	}
}


int32_t x265_data_cu_get_intra_dir_luma_predictor ( x265_t *h,
													x265_data_cu_t *cu,
													uint32_t i_abs_part_idx,
													int32_t* intra_dir_pred,
													int32_t* pi_mode )
{
	x265_base_data_cu_t* temp_cu;
	uint32_t        i_temp_part_idx;
	int32_t         i_left_intra_dir, i_above_intra_dir;
	int32_t         i_pred_num = 0;

	// get intra direction of left pu
	temp_cu = x265_base_data_cu_get_pu_left( h,
											(x265_base_data_cu_t*)cu,
											&i_temp_part_idx,
											cu->base_data_cu.i_abs_idx_in_lcu + i_abs_part_idx,
											1, 1 ) ;

	i_left_intra_dir  = temp_cu ? ( x265_base_data_cu_is_intra(temp_cu, i_temp_part_idx ) ? x265_base_data_cu_get_luma_intra_dir_p2(temp_cu, i_temp_part_idx ) : X265_DC_IDX ) : X265_DC_IDX;

	// get intra direction of above pu
	temp_cu = x265_base_data_cu_get_pu_above( h,
											(x265_base_data_cu_t*)cu,
											&i_temp_part_idx,
											cu->base_data_cu.i_abs_idx_in_lcu + i_abs_part_idx,
											1, 1, 1 );

	i_above_intra_dir = temp_cu ? ( x265_base_data_cu_is_intra(temp_cu, i_temp_part_idx ) ? x265_base_data_cu_get_luma_intra_dir_p2(temp_cu, i_temp_part_idx ) : X265_DC_IDX ) : X265_DC_IDX;

	i_pred_num = 3;
	if(i_left_intra_dir == i_above_intra_dir)
	{
		if( pi_mode )
		{
			*pi_mode = 1;
		}

		if (i_left_intra_dir > 1) // angular modes
		{
			intra_dir_pred[0] = i_left_intra_dir;
			intra_dir_pred[1] = ((i_left_intra_dir + 29) % 32) + 2;
			intra_dir_pred[2] = ((i_left_intra_dir - 1 ) % 32) + 2;
		}
		else //non-angular
		{
			intra_dir_pred[0] = X265_PLANAR_IDX;
			intra_dir_pred[1] = X265_DC_IDX;
			intra_dir_pred[2] = X265_VER_IDX;
		}
	}
	else
	{
		if( pi_mode )
		{
			*pi_mode = 2;
		}
		intra_dir_pred[0] = i_left_intra_dir;
		intra_dir_pred[1] = i_above_intra_dir;

		if (i_left_intra_dir && i_above_intra_dir ) //both modes are non-planar
		{
			intra_dir_pred[2] = X265_PLANAR_IDX;
		}
		else
		{
			intra_dir_pred[2] =  (i_left_intra_dir+i_above_intra_dir)<2? X265_VER_IDX : X265_DC_IDX;
		}
	}

	return i_pred_num;
}

uint32_t x265_data_cu_get_ctx_split_flag( x265_t *h, x265_data_cu_t* cu,
										uint32_t i_abs_part_idx, uint32_t i_depth )
{
	x265_base_data_cu_t* temp_cu;
	uint32_t        i_temp_part_idx;
	uint32_t        i_ctx;
	// get left split flag
	temp_cu = x265_base_data_cu_get_pu_left( h,
											(x265_base_data_cu_t*)cu,
											&i_temp_part_idx,
											cu->base_data_cu.i_abs_idx_in_lcu + i_abs_part_idx,
											1, 1 ) ;
	i_ctx  = ( temp_cu ) ? ( ( x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)temp_cu, i_temp_part_idx ) > i_depth ) ? 1 : 0 ) : 0;

	// get above split flag
	temp_cu = x265_base_data_cu_get_pu_above( h,
											(x265_base_data_cu_t*)cu,
											&i_temp_part_idx,
											cu->base_data_cu.i_abs_idx_in_lcu + i_abs_part_idx,
											1, 0, 1 );
	i_ctx += ( temp_cu ) ? ( ( x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)temp_cu, i_temp_part_idx ) > i_depth ) ? 1 : 0 ) : 0;

	return i_ctx;
}

uint32_t x265_data_cu_get_ctx_qt_cbf( x265_data_cu_t* cu,
									enum text_type_e i_text_type,
									uint32_t i_tr_depth )
{
	uint32_t i_ctx = 0 ;
	if( i_text_type )
	{
		return i_tr_depth;
	}

	i_ctx = ( i_tr_depth == 0 ? 1 : 0 );
	return i_ctx;
}

uint32_t x265_data_cu_get_quadtree_tu_log2_min_size_in_cu( x265_t *h,
															x265_data_cu_t *cu,
															uint32_t i_abs_part_idx )
{
	uint32_t i_log2_cb_size = 0 ;
	enum part_size_e i_part_size = 0 ;
	uint32_t i_quadtree_tu_max_depth = 0 ;
	int32_t b_intra_split_flag = 0 ;
	int32_t b_inter_split_flag = 0 ;
	uint32_t i_log2_min_tu_size_in_cu = 0;

	i_log2_cb_size = h->global.convert_to_bit[x265_data_cu_get_width_p2(cu, i_abs_part_idx )] + 2;
	i_part_size  = x265_data_cu_get_partition_size_p2( cu, i_abs_part_idx );
	i_quadtree_tu_max_depth = x265_base_data_cu_get_prediction_mode_p2((x265_base_data_cu_t*)cu, i_abs_part_idx ) == MODE_INTRA
							? h->sps[0].i_quadtree_tu_max_depth_intra
									: h->sps[0].i_quadtree_tu_max_depth_inter;
	b_intra_split_flag = ( x265_base_data_cu_get_prediction_mode_p2((x265_base_data_cu_t*)cu, i_abs_part_idx ) == MODE_INTRA
						&& i_part_size == SIZE_NxN ) ? 1 : 0;
	b_inter_split_flag = ((i_quadtree_tu_max_depth == 1)
						&& (x265_base_data_cu_get_prediction_mode_p2((x265_base_data_cu_t*)cu, i_abs_part_idx ) == MODE_INTER)
						&& ( i_part_size != SIZE_2Nx2N) );

	if (i_log2_cb_size < (h->sps[0].i_quadtree_tu_log2_min_size + i_quadtree_tu_max_depth - 1 + b_inter_split_flag + b_intra_split_flag) )
	{
		// when fully making use of signaled tumax_depth + inter/b_intra_split_flag, resulting luma tb size is < quadtree_t_ulog2min_size
		i_log2_min_tu_size_in_cu = h->sps[0].i_quadtree_tu_log2_min_size;
	}
	else
	{
		// when fully making use of signaled tumax_depth + inter/b_intra_split_flag, resulting luma tb size is still >= quadtree_t_ulog2min_size
		i_log2_min_tu_size_in_cu = i_log2_cb_size - ( i_quadtree_tu_max_depth - 1 + b_inter_split_flag + b_intra_split_flag); // stop when trafo_depth == hierarchy_depth = split_flag
		if ( i_log2_min_tu_size_in_cu > h->sps[0].i_quadtree_tu_log2_max_size)
		{
			// when fully making use of signaled tumax_depth + inter/b_intra_split_flag, resulting luma tb size is still > quadtree_t_ulog2max_size
			i_log2_min_tu_size_in_cu = h->sps[0].i_quadtree_tu_log2_max_size ;
		}
	}
	return i_log2_min_tu_size_in_cu;
}

uint32_t x265_data_cu_get_ctx_skip_flag( x265_t *h,
										x265_data_cu_t* cu,
										uint32_t i_abs_part_idx )
{
	x265_base_data_cu_t* temp_cu;
	uint32_t        i_temp_part_idx;
	uint32_t        i_ctx = 0;

	temp_cu = x265_base_data_cu_get_pu_left( h,
											(x265_base_data_cu_t*)cu,
											&i_temp_part_idx,
											cu->base_data_cu.i_abs_idx_in_lcu + i_abs_part_idx,
											1, 1 ) ;
	i_ctx = ( temp_cu ) ? x265_base_data_cu_is_skipped(temp_cu, i_temp_part_idx ) : 0;
	temp_cu = x265_base_data_cu_get_pu_above( h,
											(x265_base_data_cu_t*)cu,
											&i_temp_part_idx,
											cu->base_data_cu.i_abs_idx_in_lcu + i_abs_part_idx,
											1, 0, 1 );
	i_ctx += ( temp_cu ) ? x265_base_data_cu_is_skipped(temp_cu, i_temp_part_idx ) : 0;

	return i_ctx;
}


uint32_t x265_data_cu_get_ctx_inter_dir( x265_data_cu_t* cu, uint32_t i_abs_part_idx )
{
	return x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_part_idx ) ;
}

void x265_data_cu_set_part_size_sub_parts ( x265_t *h,
											x265_data_cu_t *cu,
											enum part_size_e i_part_size,
											uint32_t i_abs_part_idx,
											uint32_t i_depth )
{
	assert( sizeof( *cu->part_size) == 1 );
	memset( cu->part_size + i_abs_part_idx,
			i_part_size,
			h->cu.pic.i_num_partitions >> ( 2 * i_depth ) );

}

void x265_data_cu_set_cbf_sub_parts_p7_2 ( x265_t *h,
										x265_data_cu_t* cu,
										uint32_t i_cbf,
										enum text_type_e i_text_type,
										uint32_t i_abs_part_idx,
										uint32_t i_part_idx,
										uint32_t i_depth)
{
	x265_data_cu_set_sub_part_uint8 (h,
									cu,
									i_cbf,
									cu->base_data_cu.cbf[convert_txt_type_to_idx[i_text_type]],
									i_abs_part_idx,
									i_depth,
									i_part_idx) ;
}

void x265_data_cu_set_cbf_sub_parts_p7_1 ( x265_t *h,
										x265_data_cu_t* cu,
										uint32_t i_cbf_y,
										uint32_t i_cbf_u,
										uint32_t i_cbf_v,
										uint32_t i_abs_part_idx,
										uint32_t i_depth )
{
	uint32_t i_curr_part_numb = 0 ;

	i_curr_part_numb = h->cu.pic.i_num_partitions >> (i_depth << 1);
	memset( cu->base_data_cu.cbf[0] + i_abs_part_idx,
			i_cbf_y,
			sizeof( uint8_t ) * i_curr_part_numb );
	memset( cu->base_data_cu.cbf[1] + i_abs_part_idx,
			i_cbf_u,
			sizeof( uint8_t ) * i_curr_part_numb );
	memset( cu->base_data_cu.cbf[2] + i_abs_part_idx,
			i_cbf_v,
			sizeof( uint8_t ) * i_curr_part_numb );
}

void x265_data_cu_set_cbf_sub_parts_p6 ( x265_t *h,
										x265_data_cu_t* cu,
										uint32_t i_cbf,
										enum text_type_e i_text_type,
										uint32_t i_abs_part_idx,
										uint32_t i_depth)
{
	uint32_t i_curr_part_numb = 0 ;

	i_curr_part_numb = h->cu.pic.i_num_partitions >> (i_depth << 1);
	memset( cu->base_data_cu.cbf[convert_txt_type_to_idx[i_text_type]] + i_abs_part_idx,
			i_cbf,
			sizeof( uint8_t ) * i_curr_part_numb );

}

void x265_data_cu_set_sub_part_int8 ( x265_t *h,
									x265_data_cu_t* cu,
									int8_t i_parameter,
									int8_t *p_base_lcu,
									uint32_t i_cu_addr,
									uint32_t i_cu_depth,
									uint32_t i_pu_idx )
{
	uint32_t i_curr_part_num_q = 0 ;
	assert( sizeof(int8_t) == 1 ); // Using memset() works only for types of size 1

	i_curr_part_num_q = (h->cu.pic.i_num_partitions >> (2 * i_cu_depth)) >> 2;
	switch ( cu->part_size[ i_cu_addr ] )
	{
	case SIZE_2Nx2N:
		memset( p_base_lcu + i_cu_addr, i_parameter, 4 * i_curr_part_num_q );
		break;
	case SIZE_2NxN:
		memset( p_base_lcu + i_cu_addr, i_parameter, 2 * i_curr_part_num_q );
		break;
	case SIZE_Nx2N:
		memset( p_base_lcu + i_cu_addr, i_parameter, i_curr_part_num_q );
		memset( p_base_lcu + i_cu_addr + 2 * i_curr_part_num_q, i_parameter, i_curr_part_num_q );
		break;
	case SIZE_NxN:
		memset( p_base_lcu + i_cu_addr, i_parameter, i_curr_part_num_q );
		break;
	case SIZE_2NxnU:
		if ( i_pu_idx == 0 )
		{
			memset( p_base_lcu + i_cu_addr, i_parameter, (i_curr_part_num_q >> 1) );
			memset( p_base_lcu + i_cu_addr + i_curr_part_num_q, i_parameter, (i_curr_part_num_q >> 1) );
		}
		else if ( i_pu_idx == 1 )
		{
			memset( p_base_lcu + i_cu_addr, i_parameter, (i_curr_part_num_q >> 1) );
			memset( p_base_lcu + i_cu_addr + i_curr_part_num_q, i_parameter, ((i_curr_part_num_q >> 1) + (i_curr_part_num_q << 1)) );
		}
		else
		{
			assert(0);
		}
		break;
	case SIZE_2NxnD:
		if ( i_pu_idx == 0 )
		{
			memset( p_base_lcu + i_cu_addr, i_parameter, ((i_curr_part_num_q << 1) + (i_curr_part_num_q >> 1)) );
			memset( p_base_lcu + i_cu_addr + (i_curr_part_num_q << 1) + i_curr_part_num_q, i_parameter, (i_curr_part_num_q >> 1) );
		}
		else if ( i_pu_idx == 1 )
		{
			memset( p_base_lcu + i_cu_addr, i_parameter, (i_curr_part_num_q >> 1) );
			memset( p_base_lcu + i_cu_addr + i_curr_part_num_q, i_parameter, (i_curr_part_num_q >> 1) );
		}
		else
		{
			assert(0);
		}
		break;
	case SIZE_nLx2N:
		if ( i_pu_idx == 0 )
		{
			memset( p_base_lcu + i_cu_addr, i_parameter, (i_curr_part_num_q >> 2) );
			memset( p_base_lcu + i_cu_addr + (i_curr_part_num_q >> 1), i_parameter, (i_curr_part_num_q >> 2) );
			memset( p_base_lcu + i_cu_addr + (i_curr_part_num_q << 1), i_parameter, (i_curr_part_num_q >> 2) );
			memset( p_base_lcu + i_cu_addr + (i_curr_part_num_q << 1) + (i_curr_part_num_q >> 1), i_parameter, (i_curr_part_num_q >> 2) );
		}
		else if ( i_pu_idx == 1 )
		{
			memset( p_base_lcu + i_cu_addr, i_parameter, (i_curr_part_num_q >> 2) );
			memset( p_base_lcu + i_cu_addr + (i_curr_part_num_q >> 1), i_parameter, (i_curr_part_num_q + (i_curr_part_num_q >> 2)) );
			memset( p_base_lcu + i_cu_addr + (i_curr_part_num_q << 1), i_parameter, (i_curr_part_num_q >> 2) );
			memset( p_base_lcu + i_cu_addr + (i_curr_part_num_q << 1) + (i_curr_part_num_q >> 1), i_parameter, (i_curr_part_num_q + (i_curr_part_num_q >> 2)) );
		}
		else
		{
			assert(0);
		}
		break;
	case SIZE_nRx2N:
		if ( i_pu_idx == 0 )
		{
			memset( p_base_lcu + i_cu_addr, i_parameter, (i_curr_part_num_q + (i_curr_part_num_q >> 2)) );
			memset( p_base_lcu + i_cu_addr + i_curr_part_num_q + (i_curr_part_num_q >> 1), i_parameter, (i_curr_part_num_q >> 2) );
			memset( p_base_lcu + i_cu_addr + (i_curr_part_num_q << 1), i_parameter, (i_curr_part_num_q + (i_curr_part_num_q >> 2)) );
			memset( p_base_lcu + i_cu_addr + (i_curr_part_num_q << 1) + i_curr_part_num_q + (i_curr_part_num_q >> 1), i_parameter, (i_curr_part_num_q >> 2) );
		}
		else if ( i_pu_idx == 1 )
		{
			memset( p_base_lcu + i_cu_addr, i_parameter, (i_curr_part_num_q >> 2) );
			memset( p_base_lcu + i_cu_addr + (i_curr_part_num_q >> 1), i_parameter, (i_curr_part_num_q >> 2) );
			memset( p_base_lcu + i_cu_addr + (i_curr_part_num_q << 1), i_parameter, (i_curr_part_num_q >> 2) );
			memset( p_base_lcu + i_cu_addr + (i_curr_part_num_q << 1) + (i_curr_part_num_q >> 1), i_parameter, (i_curr_part_num_q >> 2) );
		}
		else
		{
			assert(0);
		}
		break;
	default:
		assert( 0 );
		break ;
	}
}

void x265_data_cu_set_sub_part_uint8 ( x265_t *h,
									x265_data_cu_t* cu,
									uint8_t i_parameter,
									uint8_t *p_base_lcu,
									uint32_t i_cu_addr,
									uint32_t i_cu_depth,
									uint32_t i_pu_idx )
{
	uint32_t i_curr_part_num_q = 0 ;
	assert( sizeof(uint8_t) == 1 ); // Using memset() works only for types of size 1

	i_curr_part_num_q = (h->cu.pic.i_num_partitions >> (2 * i_cu_depth)) >> 2;
	switch ( cu->part_size[ i_cu_addr ] )
	{
	case SIZE_2Nx2N:
		memset( p_base_lcu + i_cu_addr, i_parameter, 4 * i_curr_part_num_q );
		break;
	case SIZE_2NxN:
		memset( p_base_lcu + i_cu_addr, i_parameter, 2 * i_curr_part_num_q );
		break;
	case SIZE_Nx2N:
		memset( p_base_lcu + i_cu_addr, i_parameter, i_curr_part_num_q );
		memset( p_base_lcu + i_cu_addr + 2 * i_curr_part_num_q, i_parameter, i_curr_part_num_q );
		break;
	case SIZE_NxN:
		memset( p_base_lcu + i_cu_addr, i_parameter, i_curr_part_num_q );
		break;
	case SIZE_2NxnU:
		if ( i_pu_idx == 0 )
		{
			memset( p_base_lcu + i_cu_addr, i_parameter, (i_curr_part_num_q >> 1) );
			memset( p_base_lcu + i_cu_addr + i_curr_part_num_q, i_parameter, (i_curr_part_num_q >> 1) );
		}
		else if ( i_pu_idx == 1 )
		{
			memset( p_base_lcu + i_cu_addr, i_parameter, (i_curr_part_num_q >> 1) );
			memset( p_base_lcu + i_cu_addr + i_curr_part_num_q, i_parameter, ((i_curr_part_num_q >> 1) + (i_curr_part_num_q << 1)) );
		}
		else
		{
			assert(0);
		}
		break;
	case SIZE_2NxnD:
		if ( i_pu_idx == 0 )
		{
			memset( p_base_lcu + i_cu_addr, i_parameter, ((i_curr_part_num_q << 1) + (i_curr_part_num_q >> 1)) );
			memset( p_base_lcu + i_cu_addr + (i_curr_part_num_q << 1) + i_curr_part_num_q, i_parameter, (i_curr_part_num_q >> 1) );
		}
		else if ( i_pu_idx == 1 )
		{
			memset( p_base_lcu + i_cu_addr, i_parameter, (i_curr_part_num_q >> 1) );
			memset( p_base_lcu + i_cu_addr + i_curr_part_num_q, i_parameter, (i_curr_part_num_q >> 1) );
		}
		else
		{
			assert(0);
		}
		break;
	case SIZE_nLx2N:
		if ( i_pu_idx == 0 )
		{
			memset( p_base_lcu + i_cu_addr, i_parameter, (i_curr_part_num_q >> 2) );
			memset( p_base_lcu + i_cu_addr + (i_curr_part_num_q >> 1), i_parameter, (i_curr_part_num_q >> 2) );
			memset( p_base_lcu + i_cu_addr + (i_curr_part_num_q << 1), i_parameter, (i_curr_part_num_q >> 2) );
			memset( p_base_lcu + i_cu_addr + (i_curr_part_num_q << 1) + (i_curr_part_num_q >> 1), i_parameter, (i_curr_part_num_q >> 2) );
		}
		else if ( i_pu_idx == 1 )
		{
			memset( p_base_lcu + i_cu_addr, i_parameter, (i_curr_part_num_q >> 2) );
			memset( p_base_lcu + i_cu_addr + (i_curr_part_num_q >> 1), i_parameter, (i_curr_part_num_q + (i_curr_part_num_q >> 2)) );
			memset( p_base_lcu + i_cu_addr + (i_curr_part_num_q << 1), i_parameter, (i_curr_part_num_q >> 2) );
			memset( p_base_lcu + i_cu_addr + (i_curr_part_num_q << 1) + (i_curr_part_num_q >> 1), i_parameter, (i_curr_part_num_q + (i_curr_part_num_q >> 2)) );
		}
		else
		{
			assert(0);
		}
		break;
	case SIZE_nRx2N:
		if ( i_pu_idx == 0 )
		{
			memset( p_base_lcu + i_cu_addr, i_parameter, (i_curr_part_num_q + (i_curr_part_num_q >> 2)) );
			memset( p_base_lcu + i_cu_addr + i_curr_part_num_q + (i_curr_part_num_q >> 1), i_parameter, (i_curr_part_num_q >> 2) );
			memset( p_base_lcu + i_cu_addr + (i_curr_part_num_q << 1), i_parameter, (i_curr_part_num_q + (i_curr_part_num_q >> 2)) );
			memset( p_base_lcu + i_cu_addr + (i_curr_part_num_q << 1) + i_curr_part_num_q + (i_curr_part_num_q >> 1), i_parameter, (i_curr_part_num_q >> 2) );
		}
		else if ( i_pu_idx == 1 )
		{
			memset( p_base_lcu + i_cu_addr, i_parameter, (i_curr_part_num_q >> 2) );
			memset( p_base_lcu + i_cu_addr + (i_curr_part_num_q >> 1), i_parameter, (i_curr_part_num_q >> 2) );
			memset( p_base_lcu + i_cu_addr + (i_curr_part_num_q << 1), i_parameter, (i_curr_part_num_q >> 2) );
			memset( p_base_lcu + i_cu_addr + (i_curr_part_num_q << 1) + (i_curr_part_num_q >> 1), i_parameter, (i_curr_part_num_q >> 2) );
		}
		else
		{
			assert(0);
		}
		break;
	default:
		assert( 0 );
		break ;
	}
}

void x265_data_cu_set_sub_part_int32 ( x265_t *h,
									x265_data_cu_t* cu,
									int32_t i_parameter,
									int32_t *p_base_lcu,
									uint32_t i_cu_addr,
									uint32_t i_cu_depth,
									uint32_t i_pu_idx )
{
	uint32_t loop = 0 ;
	uint32_t i_length = 0 ;
	uint32_t i_curr_part_num_q = 0 ;
	int32_t *p_curr = NULL ;

	i_curr_part_num_q = (h->cu.pic.i_num_partitions >> (2 * i_cu_depth)) >> 2;
	switch ( cu->part_size[ i_cu_addr ] )
	{
	case SIZE_2Nx2N:
		p_curr = p_base_lcu + i_cu_addr ;
		i_length = 4 * i_curr_part_num_q ;
		for ( loop = 0 ; loop < i_length ; ++ loop )
		{
			p_curr[loop] = i_parameter ;
		}
		break;
	case SIZE_2NxN:
		p_curr = p_base_lcu + i_cu_addr ;
		i_length = 2 * i_curr_part_num_q ;
		for ( loop = 0 ; loop < i_length ; ++ loop )
		{
			p_curr[loop] = i_parameter ;
		}
		break;
	case SIZE_Nx2N:
		p_curr = p_base_lcu + i_cu_addr ;
		i_length = i_curr_part_num_q ;
		for ( loop = 0 ; loop < i_length ; ++ loop )
		{
			p_curr[loop] = i_parameter ;
		}
		p_curr = p_base_lcu + i_cu_addr + 2 * i_curr_part_num_q ;
		i_length = i_curr_part_num_q ;
		for ( loop = 0 ; loop < i_length ; ++ loop )
		{
			p_curr[loop] = i_parameter ;
		}
		break;
	case SIZE_NxN:
		p_curr = p_base_lcu + i_cu_addr ;
		i_length = i_curr_part_num_q ;
		for ( loop = 0 ; loop < i_length ; ++ loop )
		{
			p_curr[loop] = i_parameter ;
		}
		break;
	case SIZE_2NxnU:
		if ( i_pu_idx == 0 )
		{
			p_curr = p_base_lcu + i_cu_addr ;
			i_length = (i_curr_part_num_q >> 1) ;
			for ( loop = 0 ; loop < i_length ; ++ loop )
			{
				p_curr[loop] = i_parameter ;
			}
			p_curr = p_base_lcu + i_cu_addr + i_curr_part_num_q ;
			i_length = (i_curr_part_num_q >> 1) ;
			for ( loop = 0 ; loop < i_length ; ++ loop )
			{
				p_curr[loop] = i_parameter ;
			}
		}
		else if ( i_pu_idx == 1 )
		{
			p_curr = p_base_lcu + i_cu_addr ;
			i_length = (i_curr_part_num_q >> 1) ;
			for ( loop = 0 ; loop < i_length ; ++ loop )
			{
				p_curr[loop] = i_parameter ;
			}
			p_curr = p_base_lcu + i_cu_addr + i_curr_part_num_q ;
			i_length = (i_curr_part_num_q >> 1) + (i_curr_part_num_q << 1);
			for ( loop = 0 ; loop < i_length ; ++ loop )
			{
				p_curr[loop] = i_parameter ;
			}
		}
		else
		{
			assert(0);
		}
		break;
	case SIZE_2NxnD:
		if ( i_pu_idx == 0 )
		{
			p_curr = p_base_lcu + i_cu_addr ;
			i_length = (i_curr_part_num_q >> 1) + (i_curr_part_num_q << 1);
			for ( loop = 0 ; loop < i_length ; ++ loop )
			{
				p_curr[loop] = i_parameter ;
			}
			p_curr = p_base_lcu + i_cu_addr + (i_curr_part_num_q << 1) + i_curr_part_num_q ;
			i_length = (i_curr_part_num_q >> 1) ;
			for ( loop = 0 ; loop < i_length ; ++ loop )
			{
				p_curr[loop] = i_parameter ;
			}
		}
		else if ( i_pu_idx == 1 )
		{
			p_curr = p_base_lcu + i_cu_addr ;
			i_length = (i_curr_part_num_q >> 1) ;
			for ( loop = 0 ; loop < i_length ; ++ loop )
			{
				p_curr[loop] = i_parameter ;
			}
			p_curr = p_base_lcu + i_cu_addr + i_curr_part_num_q ;
			i_length = (i_curr_part_num_q >> 1) ;
			for ( loop = 0 ; loop < i_length ; ++ loop )
			{
				p_curr[loop] = i_parameter ;
			}
		}
		else
		{
			assert(0);
		}
		break;
	case SIZE_nLx2N:
		if ( i_pu_idx == 0 )
		{
			p_curr = p_base_lcu + i_cu_addr ;
			i_length = (i_curr_part_num_q >> 2) ;
			for ( loop = 0 ; loop < i_length ; ++ loop )
			{
				p_curr[loop] = i_parameter ;
			}
			p_curr = p_base_lcu + i_cu_addr + (i_curr_part_num_q >> 1);
			i_length = (i_curr_part_num_q >> 2) ;
			for ( loop = 0 ; loop < i_length ; ++ loop )
			{
				p_curr[loop] = i_parameter ;
			}
			p_curr = p_base_lcu + i_cu_addr + (i_curr_part_num_q << 1);
			i_length = (i_curr_part_num_q >> 2) ;
			for ( loop = 0 ; loop < i_length ; ++ loop )
			{
				p_curr[loop] = i_parameter ;
			}
			p_curr = p_base_lcu + i_cu_addr + (i_curr_part_num_q << 1) + (i_curr_part_num_q >> 1);
			i_length = (i_curr_part_num_q >> 2) ;
			for ( loop = 0 ; loop < i_length ; ++ loop )
			{
				p_curr[loop] = i_parameter ;
			}
		}
		else if ( i_pu_idx == 1 )
		{
			p_curr = p_base_lcu + i_cu_addr ;
			i_length = (i_curr_part_num_q >> 2) ;
			for ( loop = 0 ; loop < i_length ; ++ loop )
			{
				p_curr[loop] = i_parameter ;
			}
			p_curr = p_base_lcu + i_cu_addr + (i_curr_part_num_q >> 1);
			i_length = i_curr_part_num_q + (i_curr_part_num_q >> 2) ;
			for ( loop = 0 ; loop < i_length ; ++ loop )
			{
				p_curr[loop] = i_parameter ;
			}
			p_curr = p_base_lcu + i_cu_addr + (i_curr_part_num_q << 1);
			i_length = (i_curr_part_num_q >> 2) ;
			for ( loop = 0 ; loop < i_length ; ++ loop )
			{
				p_curr[loop] = i_parameter ;
			}
			p_curr = p_base_lcu + i_cu_addr + (i_curr_part_num_q << 1) + (i_curr_part_num_q >> 1);
			i_length = i_curr_part_num_q + (i_curr_part_num_q >> 2) ;
			for ( loop = 0 ; loop < i_length ; ++ loop )
			{
				p_curr[loop] = i_parameter ;
			}
		}
		else
		{
			assert(0);
		}
		break;
	case SIZE_nRx2N:
		if ( i_pu_idx == 0 )
		{
			p_curr = p_base_lcu + i_cu_addr ;
			i_length = i_curr_part_num_q + (i_curr_part_num_q >> 2) ;
			for ( loop = 0 ; loop < i_length ; ++ loop )
			{
				p_curr[loop] = i_parameter ;
			}
			p_curr = p_base_lcu + i_cu_addr + i_curr_part_num_q + (i_curr_part_num_q >> 1);
			i_length = (i_curr_part_num_q >> 2) ;
			for ( loop = 0 ; loop < i_length ; ++ loop )
			{
				p_curr[loop] = i_parameter ;
			}
			p_curr = p_base_lcu + i_cu_addr + (i_curr_part_num_q << 1);
			i_length = i_curr_part_num_q + (i_curr_part_num_q >> 2) ;
			for ( loop = 0 ; loop < i_length ; ++ loop )
			{
				p_curr[loop] = i_parameter ;
			}
			p_curr = p_base_lcu + i_cu_addr + (i_curr_part_num_q << 1) + i_curr_part_num_q + (i_curr_part_num_q >> 1);
			i_length = (i_curr_part_num_q >> 2) ;
			for ( loop = 0 ; loop < i_length ; ++ loop )
			{
				p_curr[loop] = i_parameter ;
			}
		}
		else if ( i_pu_idx == 1 )
		{
			p_curr = p_base_lcu + i_cu_addr ;
			i_length = (i_curr_part_num_q >> 2) ;
			for ( loop = 0 ; loop < i_length ; ++ loop )
			{
				p_curr[loop] = i_parameter ;
			}
			p_curr = p_base_lcu + i_cu_addr + (i_curr_part_num_q >> 1);
			i_length = (i_curr_part_num_q >> 2) ;
			for ( loop = 0 ; loop < i_length ; ++ loop )
			{
				p_curr[loop] = i_parameter ;
			}
			p_curr = p_base_lcu + i_cu_addr + (i_curr_part_num_q << 1);
			i_length = (i_curr_part_num_q >> 2) ;
			for ( loop = 0 ; loop < i_length ; ++ loop )
			{
				p_curr[loop] = i_parameter ;
			}
			p_curr = p_base_lcu + i_cu_addr + (i_curr_part_num_q << 1) + (i_curr_part_num_q >> 1);
			i_length = (i_curr_part_num_q >> 2) ;
			for ( loop = 0 ; loop < i_length ; ++ loop )
			{
				p_curr[loop] = i_parameter ;
			}
		}
		else
		{
			assert(0);
		}
		break;
	default:
		assert( 0 );
		break ;
	}
}

void x265_data_cu_set_merge_flag_sub_parts( x265_t *h,
											x265_data_cu_t* cu,
											int32_t b_merge_flag,
											uint32_t i_abs_part_idx,
											uint32_t i_part_idx,
											uint32_t i_depth )
{
	x265_data_cu_set_sub_part_int32 (h,
									cu,
									b_merge_flag,
									cu->merge_flag,
									i_abs_part_idx,
									i_depth,
									i_part_idx) ;
}

void x265_data_cu_set_merge_index_sub_parts ( x265_t *h,
											x265_data_cu_t* cu,
											uint32_t i_merge_index,
											uint32_t i_abs_part_idx,
											uint32_t i_part_idx,
											uint32_t i_depth )
{
	x265_data_cu_set_sub_part_uint8 (h,
									cu,
									i_merge_index,
									cu->merge_index,
									i_abs_part_idx,
									i_depth,
									i_part_idx) ;
}

void x265_data_cu_set_size_sub_parts( x265_t *h,
									x265_data_cu_t* cu,
									uint32_t i_width,
									uint32_t i_height,
									uint32_t i_abs_part_idx,
									uint32_t i_depth )
{
	uint32_t i_curr_part_numb = 0 ;

	i_curr_part_numb = h->cu.pic.i_num_partitions >> (i_depth << 1);
	memset( cu->width + i_abs_part_idx, i_width, sizeof( uint8_t ) * i_curr_part_numb );
	memset( cu->height + i_abs_part_idx, i_height, sizeof( uint8_t ) * i_curr_part_numb );
}

void x265_data_cu_set_chrom_intra_dir_sub_parts ( x265_t *h,
												x265_data_cu_t* cu,
												uint32_t i_dir,
												uint32_t i_abs_part_idx,
												uint32_t i_depth )
{
	uint32_t i_curr_part_numb = 0 ;

	i_curr_part_numb = h->cu.pic.i_num_partitions >> (i_depth << 1);
	memset( cu->chroma_intra_dir + i_abs_part_idx, i_dir, sizeof( uint8_t ) * i_curr_part_numb );
}

void x265_data_cu_set_inter_dir_sub_parts ( x265_t *h,
											x265_data_cu_t* cu,
											uint32_t i_dir,
											uint32_t i_abs_part_idx,
											uint32_t i_part_idx,
											uint32_t i_depth )
{
	x265_data_cu_set_sub_part_uint8 (h,
									cu,
									i_dir,
									cu->base_data_cu.inter_dir,
									i_abs_part_idx,
									i_depth,
									i_part_idx) ;
}

void x265_data_cu_set_mvp_idx_sub_parts ( x265_t *h,
										x265_data_cu_t* cu,
										int32_t i_mvp_idx,
										enum ref_pic_list_e i_ref_pic_list,
										uint32_t i_abs_part_idx,
										uint32_t i_part_idx,
										uint32_t i_depth )
{
	x265_data_cu_set_sub_part_int8 (h,
									cu,
									i_mvp_idx,
									cu->mvp_idx[i_ref_pic_list],
									i_abs_part_idx,
									i_depth,
									i_part_idx) ;
}

void x265_data_cu_set_mvp_num_sub_parts ( x265_t *h,
										x265_data_cu_t* cu,
										int32_t i_mvp_num,
										enum ref_pic_list_e i_ref_pic_list,
										uint32_t i_abs_part_idx,
										uint32_t i_part_idx,
										uint32_t i_depth )
{
	x265_data_cu_set_sub_part_int8 (h,
									cu,
									i_mvp_num,
									cu->mvp_num[i_ref_pic_list],
									i_abs_part_idx,
									i_depth,
									i_part_idx) ;
}

void x265_data_cu_set_transform_skip_sub_parts_p7( x265_t *h,
												x265_data_cu_t* cu,
												uint32_t b_use_transform_skip_y,
												uint32_t b_use_transform_skip_u,
												uint32_t b_use_transform_skip_v,
												uint32_t i_abs_part_idx,
												uint32_t i_depth )
{
	uint32_t i_curr_part_numb = 0 ;

	i_curr_part_numb = h->cu.pic.i_num_partitions >> (i_depth << 1);
	memset( cu->transform_skip[0] + i_abs_part_idx,
			b_use_transform_skip_y,
			sizeof( uint8_t ) * i_curr_part_numb );
	memset( cu->transform_skip[1] + i_abs_part_idx,
			b_use_transform_skip_u,
			sizeof( uint8_t ) * i_curr_part_numb );
	memset( cu->transform_skip[2] + i_abs_part_idx,
			b_use_transform_skip_v,
			sizeof( uint8_t ) * i_curr_part_numb );
}

void x265_data_cu_set_transform_skip_sub_parts_p6( x265_t *h,
												x265_data_cu_t* cu,
												uint32_t b_use_transform_skip,
												enum text_type_e i_text_type,
												uint32_t i_abs_part_idx,
												uint32_t i_depth)
{
	uint32_t i_curr_part_numb = 0 ;

	i_curr_part_numb = h->cu.pic.i_num_partitions >> (i_depth << 1);
	memset( cu->transform_skip[convert_txt_type_to_idx[i_text_type]] + i_abs_part_idx,
			b_use_transform_skip,
			sizeof( uint8_t ) * i_curr_part_numb );
}

uint8_t x265_data_cu_get_num_part_inter( x265_data_cu_t* cu )
{
	uint8_t i_num_part = 0;

	switch ( cu->part_size[0] )
	{
	case SIZE_2Nx2N:    i_num_part = 1; break;
	case SIZE_2NxN:     i_num_part = 2; break;
	case SIZE_Nx2N:     i_num_part = 2; break;
	case SIZE_NxN:      i_num_part = 4; break;
	case SIZE_2NxnU:    i_num_part = 2; break;
	case SIZE_2NxnD:    i_num_part = 2; break;
	case SIZE_nLx2N:    i_num_part = 2; break;
	case SIZE_nRx2N:    i_num_part = 2; break;
	default:            assert (0);   break;
	}

	return  i_num_part;
}


void x265_data_cu_get_part_index_and_size ( x265_data_cu_t* cu,
											uint32_t i_part_idx,
											uint32_t* p_part_addr,
											int32_t* p_width,
											int32_t* p_height )
{
	switch ( cu->part_size[0] )
	{
	case SIZE_2NxN:
		*p_width = x265_data_cu_get_width_p2(cu, 0);
		*p_height = x265_data_cu_get_height_p2(cu, 0) >> 1;
		*p_part_addr = ( i_part_idx == 0 ) ? 0 : cu->base_data_cu.i_num_partition >> 1;
		break;
	case SIZE_Nx2N:
		*p_width = x265_data_cu_get_width_p2(cu, 0) >> 1;
		*p_height = x265_data_cu_get_height_p2(cu, 0);
		*p_part_addr = ( i_part_idx == 0 ) ? 0 : cu->base_data_cu.i_num_partition >> 2;
		break;
	case SIZE_NxN:
		*p_width = x265_data_cu_get_width_p2(cu, 0) >> 1;
		*p_height = x265_data_cu_get_height_p2(cu, 0) >> 1;
		*p_part_addr = ( cu->base_data_cu.i_num_partition >> 2 ) * i_part_idx;
		break;
	case SIZE_2NxnU:
		*p_width     = x265_data_cu_get_width_p2(cu, 0);
		*p_height    = ( i_part_idx == 0 ) ?  x265_data_cu_get_height_p2(cu, 0) >> 2 : ( x265_data_cu_get_height_p2(cu, 0) >> 2 ) + ( x265_data_cu_get_height_p2(cu, 0) >> 1 );
		*p_part_addr = ( i_part_idx == 0 ) ? 0 : cu->base_data_cu.i_num_partition >> 3;
		break;
	case SIZE_2NxnD:
		*p_width     = x265_data_cu_get_width_p2(cu, 0);
		*p_height    = ( i_part_idx == 0 ) ?  ( x265_data_cu_get_height_p2(cu, 0) >> 2 ) + ( x265_data_cu_get_height_p2(cu, 0) >> 1 ) : x265_data_cu_get_height_p2(cu, 0) >> 2;
		*p_part_addr = ( i_part_idx == 0 ) ? 0 : (cu->base_data_cu.i_num_partition >> 1) + (cu->base_data_cu.i_num_partition >> 3);
		break;
	case SIZE_nLx2N:
		*p_width     = ( i_part_idx == 0 ) ? x265_data_cu_get_width_p2(cu, 0) >> 2 : ( x265_data_cu_get_width_p2(cu, 0) >> 2 ) + ( x265_data_cu_get_width_p2(cu, 0) >> 1 );
		*p_height    = x265_data_cu_get_height_p2(cu, 0);
		*p_part_addr = ( i_part_idx == 0 ) ? 0 : cu->base_data_cu.i_num_partition >> 4;
		break;
	case SIZE_nRx2N:
		*p_width     = ( i_part_idx == 0 ) ? ( x265_data_cu_get_width_p2(cu, 0) >> 2 ) + ( x265_data_cu_get_width_p2(cu, 0) >> 1 ) : x265_data_cu_get_width_p2(cu, 0) >> 2;
		*p_height    = x265_data_cu_get_height_p2(cu, 0);
		*p_part_addr = ( i_part_idx == 0 ) ? 0 : (cu->base_data_cu.i_num_partition >> 2) + (cu->base_data_cu.i_num_partition >> 4);
		break;
	default:
		assert ( cu->part_size[0] == SIZE_2Nx2N );
		*p_width = x265_data_cu_get_width_p2(cu, 0);
		*p_height = x265_data_cu_get_height_p2(cu, 0);
		*p_part_addr = 0;
		break;
	}
}

void x265_data_cu_derive_left_right_top_idx_general( x265_t *h,
													x265_data_cu_t* cu,
													uint32_t i_abs_part_idx,
													uint32_t i_part_idx,
													uint32_t* p_part_idx_lt,
													uint32_t* p_part_idx_rt )
{
	*p_part_idx_lt = cu->base_data_cu.i_abs_idx_in_lcu + i_abs_part_idx;
	uint32_t i_pu_width = 0;

	switch ( cu->part_size[i_abs_part_idx] )
	{
	case SIZE_2Nx2N: i_pu_width = cu->width[i_abs_part_idx];  break;
	case SIZE_2NxN:  i_pu_width = cu->width[i_abs_part_idx];   break;
	case SIZE_Nx2N:  i_pu_width = cu->width[i_abs_part_idx]  >> 1;  break;
	case SIZE_NxN:   i_pu_width = cu->width[i_abs_part_idx]  >> 1; break;
	case SIZE_2NxnU:   i_pu_width = cu->width[i_abs_part_idx]; break;
	case SIZE_2NxnD:   i_pu_width = cu->width[i_abs_part_idx]; break;
	case SIZE_nLx2N:
		if ( i_part_idx == 0 )
		{
			i_pu_width = cu->width[i_abs_part_idx]  >> 2;
		}
		else if ( i_part_idx == 1 )
		{
			i_pu_width = (cu->width[i_abs_part_idx]  >> 1) + (cu->width[i_abs_part_idx]  >> 2);
		}
		else
		{
			assert(0);
		}
		break;
	case SIZE_nRx2N:
		if ( i_part_idx == 0 )
		{
			i_pu_width = (cu->width[i_abs_part_idx]  >> 1) + (cu->width[i_abs_part_idx]  >> 2);
		}
		else if ( i_part_idx == 1 )
		{
			i_pu_width = cu->width[i_abs_part_idx]  >> 2;
		}
		else
		{
			assert(0);
		}
		break;
	default:
		assert (0);
		break;
	}

	*p_part_idx_rt = h->scan.raster_to_zscan [h->scan.zscan_to_raster[ *p_part_idx_lt ]
	                                          + i_pu_width / h->cu.pic.i_min_cu_width - 1 ];
}

void x265_data_cu_derive_left_bottom_idx_general( x265_t *h,
													x265_data_cu_t* cu,
													uint32_t i_abs_part_idx,
													uint32_t i_part_idx,
													uint32_t* p_part_idx_lb )
{
	uint32_t i_pu_height = 0;
	switch ( cu->part_size[i_abs_part_idx] )
	{
	case SIZE_2Nx2N: i_pu_height = cu->height[i_abs_part_idx];    break;
	case SIZE_2NxN:  i_pu_height = cu->height[i_abs_part_idx] >> 1;    break;
	case SIZE_Nx2N:  i_pu_height = cu->height[i_abs_part_idx];  break;
	case SIZE_NxN:   i_pu_height = cu->height[i_abs_part_idx] >> 1;    break;
	case SIZE_2NxnU:
		if ( i_part_idx == 0 )
		{
			i_pu_height = cu->height[i_abs_part_idx] >> 2;
		}
		else if ( i_part_idx == 1 )
		{
			i_pu_height = (cu->height[i_abs_part_idx] >> 1) + (cu->height[i_abs_part_idx] >> 2);
		}
		else
		{
			assert(0);
		}
		break;
	case SIZE_2NxnD:
		if ( i_part_idx == 0 )
		{
			i_pu_height = (cu->height[i_abs_part_idx] >> 1) + (cu->height[i_abs_part_idx] >> 2);
		}
		else if ( i_part_idx == 1 )
		{
			i_pu_height = cu->height[i_abs_part_idx] >> 2;
		}
		else
		{
			assert(0);
		}
		break;
	case SIZE_nLx2N: i_pu_height = cu->height[i_abs_part_idx];  break;
	case SIZE_nRx2N: i_pu_height = cu->height[i_abs_part_idx];  break;
	default:
		assert (0);
		break;
	}

	*p_part_idx_lb = h->scan.raster_to_zscan
						[h->scan.zscan_to_raster[cu->base_data_cu.i_abs_idx_in_lcu + i_abs_part_idx ] +
						 ((i_pu_height / h->cu.pic.i_min_cu_height) - 1)
						 * h->cu.pic.i_num_part_in_width];
}

void x265_data_cu_derive_left_right_top_idx( x265_t *h,
											x265_data_cu_t* cu,
											uint32_t i_part_idx,
											uint32_t* p_part_idx_lt,
											uint32_t* p_part_idx_rt )
{
	*p_part_idx_lt = cu->base_data_cu.i_abs_idx_in_lcu;
	*p_part_idx_rt = h->scan.raster_to_zscan [h->scan.zscan_to_raster[*p_part_idx_lt]
	                                          + cu->width[0] / h->cu.pic.i_min_cu_width - 1];

	switch ( cu->part_size[0] )
	{
	case SIZE_2Nx2N:                                                                                                                                break;
	case SIZE_2NxN:
		*p_part_idx_lt += ( i_part_idx == 0 ) ? 0 : cu->base_data_cu.i_num_partition >> 1;
		*p_part_idx_rt += ( i_part_idx == 0 ) ? 0 : cu->base_data_cu.i_num_partition >> 1;
		break;
	case SIZE_Nx2N:
		*p_part_idx_lt += ( i_part_idx == 0 ) ? 0 : cu->base_data_cu.i_num_partition >> 2;
		*p_part_idx_rt -= ( i_part_idx == 1 ) ? 0 : cu->base_data_cu.i_num_partition >> 2;
		break;
	case SIZE_NxN:
		*p_part_idx_lt += ( cu->base_data_cu.i_num_partition >> 2 ) * i_part_idx;
		*p_part_idx_rt +=  ( cu->base_data_cu.i_num_partition >> 2 ) * ( i_part_idx - 1 );
		break;
	case SIZE_2NxnU:
		*p_part_idx_lt += ( i_part_idx == 0 )? 0 : cu->base_data_cu.i_num_partition >> 3;
		*p_part_idx_rt += ( i_part_idx == 0 )? 0 : cu->base_data_cu.i_num_partition >> 3;
		break;
	case SIZE_2NxnD:
		*p_part_idx_lt += ( i_part_idx == 0 )? 0 : ( cu->base_data_cu.i_num_partition >> 1 ) + ( cu->base_data_cu.i_num_partition >> 3 );
		*p_part_idx_rt += ( i_part_idx == 0 )? 0 : ( cu->base_data_cu.i_num_partition >> 1 ) + ( cu->base_data_cu.i_num_partition >> 3 );
		break;
	case SIZE_nLx2N:
		*p_part_idx_lt += ( i_part_idx == 0 )? 0 : cu->base_data_cu.i_num_partition >> 4;
		*p_part_idx_rt -= ( i_part_idx == 1 )? 0 : ( cu->base_data_cu.i_num_partition >> 2 ) + ( cu->base_data_cu.i_num_partition >> 4 );
		break;
	case SIZE_nRx2N:
		*p_part_idx_lt += ( i_part_idx == 0 )? 0 : ( cu->base_data_cu.i_num_partition >> 2 ) + ( cu->base_data_cu.i_num_partition >> 4 );
		*p_part_idx_rt -= ( i_part_idx == 1 )? 0 : cu->base_data_cu.i_num_partition >> 4;
		break;
	default:
		assert (0);
		break;
	}

}

void x265_data_cu_derive_left_bottom_idx( x265_t *h,
										x265_data_cu_t* cu,
										uint32_t i_part_idx,
										uint32_t* p_part_idx_lb )
{
	*p_part_idx_lb = h->scan.raster_to_zscan[
	                 h->scan.zscan_to_raster[cu->base_data_cu.i_abs_idx_in_lcu ]
	                 + (((cu->height[0] / h->cu.pic.i_min_cu_height)>>1) - 1)
	                 * h->cu.pic.i_num_part_in_width];

	switch ( cu->part_size[0] )
	{
    case SIZE_2Nx2N:
      *p_part_idx_lb += cu->base_data_cu.i_num_partition >> 1;
      break;
    case SIZE_2NxN:
      *p_part_idx_lb += ( i_part_idx == 0 ) ? 0 : cu->base_data_cu.i_num_partition >> 1;
      break;
    case SIZE_Nx2N:
      *p_part_idx_lb += ( i_part_idx == 0 ) ? cu->base_data_cu.i_num_partition >> 1 : (cu->base_data_cu.i_num_partition >> 2)*3;
      break;
    case SIZE_NxN:
      *p_part_idx_lb += ( cu->base_data_cu.i_num_partition >> 2 ) * i_part_idx;
      break;
    case SIZE_2NxnU:
      *p_part_idx_lb += ( i_part_idx == 0 ) ? -((int32_t)cu->base_data_cu.i_num_partition >> 3) : cu->base_data_cu.i_num_partition >> 1;
      break;
    case SIZE_2NxnD:
      *p_part_idx_lb += ( i_part_idx == 0 ) ? (cu->base_data_cu.i_num_partition >> 2) + (cu->base_data_cu.i_num_partition >> 3): cu->base_data_cu.i_num_partition >> 1;
      break;
    case SIZE_nLx2N:
      *p_part_idx_lb += ( i_part_idx == 0 ) ? cu->base_data_cu.i_num_partition >> 1 : (cu->base_data_cu.i_num_partition >> 1) + (cu->base_data_cu.i_num_partition >> 4);
      break;
    case SIZE_nRx2N:
      *p_part_idx_lb += ( i_part_idx == 0 ) ? cu->base_data_cu.i_num_partition >> 1 : (cu->base_data_cu.i_num_partition >> 1) + (cu->base_data_cu.i_num_partition >> 2) + (cu->base_data_cu.i_num_partition >> 4);
      break;
    default:
      assert (0);
      break;
	}
}

void x265_data_cu_derive_right_bottom_idx ( x265_t *h,
											x265_data_cu_t* cu,
											uint32_t i_part_idx,
											uint32_t* p_part_idx_rb )
{
	*p_part_idx_rb = h->scan.raster_to_zscan[
	                 h->scan.zscan_to_raster[cu->base_data_cu.i_abs_idx_in_lcu]
	                 + (((cu->height[0] / h->cu.pic.i_min_cu_height) >> 1) - 1)
	                 * h->cu.pic.i_num_part_in_width
	                 +  cu->width[0] / h->cu.pic.i_min_cu_width - 1];

	switch ( cu->part_size[0] )
	{
	case SIZE_2Nx2N:
		*p_part_idx_rb += cu->base_data_cu.i_num_partition >> 1;
		break;
	case SIZE_2NxN:
		*p_part_idx_rb += ( i_part_idx == 0 )? 0 : cu->base_data_cu.i_num_partition >> 1;
		break;
	case SIZE_Nx2N:
		*p_part_idx_rb += ( i_part_idx == 0 )? cu->base_data_cu.i_num_partition >> 2 : (cu->base_data_cu.i_num_partition >> 1);
		break;
	case SIZE_NxN:
		*p_part_idx_rb += ( cu->base_data_cu.i_num_partition >> 2 ) * ( i_part_idx - 1 );
		break;
	case SIZE_2NxnU:
		*p_part_idx_rb += ( i_part_idx == 0 ) ? -((int32_t)cu->base_data_cu.i_num_partition >> 3) : cu->base_data_cu.i_num_partition >> 1;
		break;
	case SIZE_2NxnD:
		*p_part_idx_rb += ( i_part_idx == 0 ) ? (cu->base_data_cu.i_num_partition >> 2) + (cu->base_data_cu.i_num_partition >> 3): cu->base_data_cu.i_num_partition >> 1;
		break;
	case SIZE_nLx2N:
		*p_part_idx_rb += ( i_part_idx == 0 ) ? (cu->base_data_cu.i_num_partition >> 3) + (cu->base_data_cu.i_num_partition >> 4): cu->base_data_cu.i_num_partition >> 1;
		break;
	case SIZE_nRx2N:
		*p_part_idx_rb += ( i_part_idx == 0 ) ? (cu->base_data_cu.i_num_partition >> 2) + (cu->base_data_cu.i_num_partition >> 3) + (cu->base_data_cu.i_num_partition >> 4) : cu->base_data_cu.i_num_partition >> 1;
		break;
	default:
		assert (0);
		break;
	}
}

void x265_data_cu_derive_left_right_top_idx_adi( x265_t *h,
												x265_data_cu_t* cu,
												uint32_t* p_part_idx_lt,
												uint32_t* p_part_idx_rt,
												uint32_t i_part_offset,
												uint32_t i_part_depth )
{
	  uint32_t i_num_part_in_width = (cu->width[0] / h->cu.pic.i_min_cu_width) >> i_part_depth;
	  *p_part_idx_lt = cu->base_data_cu.i_abs_idx_in_lcu + i_part_offset;
	  *p_part_idx_rt = h->scan.raster_to_zscan[h->scan.zscan_to_raster[*p_part_idx_lt] + i_num_part_in_width - 1 ];
}

void x265_data_cu_derive_left_bottom_idx_adi( x265_t *h,
											x265_data_cu_t* cu,
											uint32_t* p_part_idx_lb,
											uint32_t i_part_offset,
											uint32_t i_part_depth )
{
	  uint32_t i_abs_idx;
	  uint32_t i_min_cu_width, i_width_in_min_cus;

	  i_min_cu_width = h->cu.pic.i_min_cu_width ;
	  i_width_in_min_cus = (x265_data_cu_get_width_p2(cu, 0) / i_min_cu_width) >> i_part_depth;
	  i_abs_idx = x265_base_data_cu_get_zorder_idx_in_cu((x265_base_data_cu_t*)cu) + i_part_offset
			  	  + (cu->base_data_cu.i_num_partition >> (i_part_depth << 1)) - 1;
	  i_abs_idx = h->scan.zscan_to_raster[i_abs_idx]-(i_width_in_min_cus-1);
	  *p_part_idx_lb = h->scan.raster_to_zscan[i_abs_idx];
}

void x265_data_cu_get_inter_merge_candidates( x265_t *h,
											x265_data_cu_t* cu,
											uint32_t i_abs_part_idx,
											uint32_t i_pu_idx,
											x265_mv_field_t* p_mv_field_neighbours,
											uint8_t* p_inter_dir_neighbours,
											int32_t* p_num_valid_merge_cand,
											int32_t i_mrg_cand_idx )
{
	uint32_t loop = 0 ;
	uint32_t i_abs_part_addr = 0 ;
	int32_t i_x_p, i_y_p, i_psw, i_psh ;
	int32_t i_count = 0;
	uint32_t i_part_idx_lt, i_part_idx_rt, i_part_idx_lb ;
	uint32_t i_left_part_idx = 0;
	uint32_t i_above_part_idx = 0;
	uint32_t i_above_right_part_idx = 0;
	uint32_t i_left_bottom_part_idx = 0;
	uint32_t i_above_left_part_idx = 0;
	int32_t b_is_available_a1 = 0 ;
	int32_t b_is_available_b1 = 0 ;
	int32_t b_is_available_b0 = 0 ;
	int32_t b_is_available_a0 = 0;
	int32_t b_is_available_b2 = 0;
	uint32_t i_part_idx_rb;
	int32_t i_lcu_idx = 0 ;
	uint32_t i_abs_part_idx_tmp = 0 ;
	uint32_t i_num_part_in_cu_width = 0 ;
	int32_t i_ref_idx;
	int32_t b_exist_mv = 0;
	uint32_t i_part_idx_center;
	uint32_t i_cur_lcu_idx = 0 ;
	int32_t dir = 0;
	uint32_t i_array_addr = 0 ;
	uint32_t i_cutoff = 0 ;
	int32_t i_idx = 0 ;
	int32_t i = 0 ;
	int32_t j = 0 ;
	int32_t i_ref_pocl0 = 0 ;
	int32_t i_ref_pocl1 = 0 ;
	int32_t i_num_ref_idx = 0 ;
	int32_t i_r = 0;
	int32_t i_refcnt = 0;
	int32_t cand_is_inter[X265_MRG_MAX_NUM_CANDS] ;
	uint32_t priority_list0[12] = {0 , 1, 0, 2, 1, 2, 0, 3, 1, 3, 2, 3};
	uint32_t priority_list1[12] = {1 , 0, 2, 0, 2, 1, 3, 0, 3, 1, 3, 2};
	x265_mv_t col_mv;
	x265_mv_t zero_mv ;
	x265_base_data_cu_t* cu_left = NULL;
	x265_base_data_cu_t* cu_above = NULL;
	x265_base_data_cu_t* cu_above_right = 0;
	x265_base_data_cu_t* cu_left_bottom = 0;
	x265_base_data_cu_t* cu_above_left = 0;
	enum part_size_e i_cur_ps = 0 ;

	x265_mv_set_zero (&zero_mv) ;
	i_abs_part_addr = cu->base_data_cu.i_abs_idx_in_lcu + i_abs_part_idx;
	for( loop = 0; loop < h->slice->i_max_num_merge_cand ; ++loop )
	{
		cand_is_inter[loop] = 0;
		x265_mv_field_set_ref_idx (&p_mv_field_neighbours[(loop << 1)], X265_NOT_VALID);
		x265_mv_field_set_ref_idx (&p_mv_field_neighbours[(loop << 1) + 1], X265_NOT_VALID);
	}
	*p_num_valid_merge_cand = h->slice->i_max_num_merge_cand;
	// compute the location of the current pu

	x265_data_cu_get_part_position( cu, i_pu_idx, &i_x_p, &i_y_p, &i_psw, &i_psh) ;

	i_cur_ps = x265_data_cu_get_partition_size_p2( cu, i_abs_part_idx );
	x265_data_cu_derive_left_right_top_idx_general(h, cu, i_abs_part_idx, i_pu_idx, &i_part_idx_lt, &i_part_idx_rt );
	x265_data_cu_derive_left_bottom_idx_general(h, cu, i_abs_part_idx, i_pu_idx, &i_part_idx_lb );


	//	left
	cu_left = x265_base_data_cu_get_pu_left(h,
											(x265_base_data_cu_t*)cu,
											&i_left_part_idx,
											i_part_idx_lb, 1, 1);
	b_is_available_a1 = cu_left &&
		x265_base_data_cu_is_diff_mer(h, cu_left, i_x_p - 1, i_y_p + i_psh - 1, i_x_p, i_y_p) &&
		!( i_pu_idx == 1 && (i_cur_ps == SIZE_Nx2N || i_cur_ps == SIZE_nLx2N || i_cur_ps == SIZE_nRx2N) ) &&
		!x265_base_data_cu_is_intra(cu_left, i_left_part_idx ) ;
	if ( b_is_available_a1 )
	{
		cand_is_inter[i_count] = 1;
		// get _inter dir
		p_inter_dir_neighbours[i_count] = x265_base_data_cu_get_inter_dir_p2(cu_left, i_left_part_idx );
		// get mv from left
		x265_base_data_cu_get_mv_field(cu_left, cu_left, i_left_part_idx, REF_PIC_LIST_0, &p_mv_field_neighbours[i_count<<1] );
		if ( B_SLICE == h->slice->i_slice_type )
		{
			x265_base_data_cu_get_mv_field(cu_left, cu_left, i_left_part_idx, REF_PIC_LIST_1, &p_mv_field_neighbours[(i_count<<1)+1] );
		}
		if ( i_mrg_cand_idx == i_count )
		{
			return;
		}
		i_count++;
	}
	// early termination
	if (i_count == h->slice->i_max_num_merge_cand )
	{
		return;
	}

	// above
	cu_above = x265_base_data_cu_get_pu_above(h,
											(x265_base_data_cu_t*)cu,
											&i_above_part_idx,
											i_part_idx_rt, 1, 0, 1 );
	b_is_available_b1 = cu_above &&
			x265_base_data_cu_is_diff_mer(h, cu_above, i_x_p + i_psw - 1, i_y_p - 1, i_x_p, i_y_p) &&
		!( i_pu_idx == 1 && (i_cur_ps == SIZE_2NxN || i_cur_ps == SIZE_2NxnU || i_cur_ps == SIZE_2NxnD) ) &&
		!x265_base_data_cu_is_intra(cu_above, i_above_part_idx );
	if ( b_is_available_b1
			&& (!b_is_available_a1
			|| !x265_base_data_cu_has_equal_motion(cu_left, i_left_part_idx, cu_above, i_above_part_idx)))
	{
		cand_is_inter[i_count] = 1;
		// get _inter dir
		p_inter_dir_neighbours[i_count] = x265_base_data_cu_get_inter_dir_p2(cu_above, i_above_part_idx );
		// get mv from left
		x265_base_data_cu_get_mv_field(cu_above, cu_above, i_above_part_idx, REF_PIC_LIST_0, &p_mv_field_neighbours[i_count<<1] );
		if ( B_SLICE == h->slice->i_slice_type )
		{
			x265_base_data_cu_get_mv_field(cu_above, cu_above, i_above_part_idx, REF_PIC_LIST_1, &p_mv_field_neighbours[(i_count<<1)+1] );
		}
		if ( i_mrg_cand_idx == i_count )
		{
			return;
		}

		i_count++;
	}
	// early termination
	if (i_count == h->slice->i_max_num_merge_cand )
	{
		return;
	}

	// above right
	cu_above_right = x265_data_cu_get_pu_above_right(h, cu,
													&i_above_right_part_idx,
													i_part_idx_rt,
													1 );
	b_is_available_b0 = cu_above_right &&
		x265_base_data_cu_is_diff_mer(h, cu_above_right, i_x_p + i_psw, i_y_p - 1, i_x_p, i_y_p) &&
		!x265_base_data_cu_is_intra(cu_above_right, i_above_right_part_idx );
	if ( b_is_available_b0
			&& ( !b_is_available_b1
			|| !x265_base_data_cu_has_equal_motion(cu_above, i_above_part_idx, cu_above_right, i_above_right_part_idx )))
	{
		cand_is_inter[i_count] = 1;
		// get _inter dir
		p_inter_dir_neighbours[i_count] = x265_base_data_cu_get_inter_dir_p2(cu_above_right, i_above_right_part_idx );
		// get mv from left
		x265_base_data_cu_get_mv_field(cu_above_right, cu_above_right, i_above_right_part_idx, REF_PIC_LIST_0, &p_mv_field_neighbours[i_count << 1] );
		if ( B_SLICE == h->slice->i_slice_type )
		{
			x265_base_data_cu_get_mv_field(cu_above_right, cu_above_right, i_above_right_part_idx, REF_PIC_LIST_1, &p_mv_field_neighbours[(i_count << 1) + 1] );
		}
		if ( i_mrg_cand_idx == i_count )
		{
			return;
		}
		i_count++;
	}
	// early termination
	if (i_count == h->slice->i_max_num_merge_cand )
	{
		return;
	}

	//left bottom
	cu_left_bottom = x265_data_cu_get_pu_below_left(h, cu,
													&i_left_bottom_part_idx,
													i_part_idx_lb, 1 );
	b_is_available_a0 = cu_left_bottom &&
		x265_base_data_cu_is_diff_mer(h, cu_left_bottom, i_x_p - 1, i_y_p + i_psh, i_x_p, i_y_p) &&
		!x265_base_data_cu_is_intra(cu_left_bottom, i_left_bottom_part_idx ) ;
	if ( b_is_available_a0
			&& ( !b_is_available_a1
			|| !x265_base_data_cu_has_equal_motion(cu_left, i_left_part_idx, cu_left_bottom, i_left_bottom_part_idx )))
	{
		cand_is_inter[i_count] = 1;
		// get _inter dir
		p_inter_dir_neighbours[i_count] = x265_base_data_cu_get_inter_dir_p2(cu_left_bottom, i_left_bottom_part_idx );
		// get mv from left
		x265_base_data_cu_get_mv_field(cu_left_bottom, cu_left_bottom, i_left_bottom_part_idx, REF_PIC_LIST_0, &p_mv_field_neighbours[i_count << 1] );
		if ( B_SLICE == h->slice->i_slice_type )
		{
			x265_base_data_cu_get_mv_field(cu_left_bottom, cu_left_bottom, i_left_bottom_part_idx, REF_PIC_LIST_1, &p_mv_field_neighbours[(i_count << 1) + 1] );
		}
		if ( i_mrg_cand_idx == i_count )
		{
			return;
		}
		i_count++;
	}
	// early termination
	if (i_count == h->slice->i_max_num_merge_cand )
	{
		return;
	}

	// above left
	if( i_count < 4 )
	{
		cu_above_left = x265_data_cu_get_pu_above_left(h, cu,
														&i_above_left_part_idx,
														i_abs_part_addr, 1 );
		b_is_available_b2 = cu_above_left &&
			x265_base_data_cu_is_diff_mer(h, cu_above_left, i_x_p - 1, i_y_p - 1, i_x_p, i_y_p) &&
			!x265_base_data_cu_is_intra(cu_above_left, i_above_left_part_idx );
		if ( b_is_available_b2
				&& ( !b_is_available_a1
					|| !x265_base_data_cu_has_equal_motion(cu_left, i_left_part_idx, cu_above_left, i_above_left_part_idx ) )
				&& ( !b_is_available_b1
					|| !x265_base_data_cu_has_equal_motion(cu_above, i_above_part_idx, cu_above_left, i_above_left_part_idx ) ) )
		{
			cand_is_inter[i_count] = 1;
			// get _inter dir
			p_inter_dir_neighbours[i_count] = x265_base_data_cu_get_inter_dir_p2(cu_above_left, i_above_left_part_idx );
			// get mv from left
			x265_base_data_cu_get_mv_field(cu_above_left, cu_above_left, i_above_left_part_idx, REF_PIC_LIST_0, &p_mv_field_neighbours[i_count << 1] );
			if ( B_SLICE == h->slice->i_slice_type )
			{
				x265_base_data_cu_get_mv_field(cu_above_left, cu_above_left, i_above_left_part_idx, REF_PIC_LIST_1, &p_mv_field_neighbours[(i_count << 1) + 1] );
			}
			if ( i_mrg_cand_idx == i_count )
			{
				return;
			}
			i_count++;
		}
	}
	// early termination
	if (i_count == h->slice->i_max_num_merge_cand )
	{
		return;
	}

	if ( h->slice->b_enable_tmvp_flag )
	{
		i_lcu_idx = x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu);
		x265_data_cu_derive_right_bottom_idx( h, cu, i_pu_idx, &i_part_idx_rb );
		i_abs_part_idx_tmp = h->scan.zscan_to_raster[i_part_idx_rb];
		i_num_part_in_cu_width = h->cu.pic.i_num_part_in_width;
		if ( ( x265_data_cu_get_cu_pel_x(h->data_cu)
				+ h->scan.raster_to_pel_x[i_abs_part_idx_tmp]
				+ h->cu.pic.i_min_cu_width )
				>= h->sps[0].i_pic_width_in_luma_samples )  // image boundary check
		{
			i_lcu_idx = -1;
		}
		else if ( ( x265_data_cu_get_cu_pel_y(h->data_cu)
				+ h->scan.raster_to_pel_y[i_abs_part_idx_tmp]
				+ h->cu.pic.i_min_cu_height )
				>= h->sps[0].i_pic_height_in_luma_samples )
		{
			i_lcu_idx = -1;
		}
		else
		{
			if ( ( i_abs_part_idx_tmp % i_num_part_in_cu_width < i_num_part_in_cu_width - 1 ) &&           // is not at the last column of lcu
					( i_abs_part_idx_tmp / i_num_part_in_cu_width < h->cu.pic.i_num_part_in_height - 1 ) ) // is not at the last row    of lcu
			{
				i_abs_part_addr = h->scan.raster_to_zscan[ i_abs_part_idx_tmp + i_num_part_in_cu_width + 1 ];
				i_lcu_idx = x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu);
			}
			else if ( i_abs_part_idx_tmp % i_num_part_in_cu_width < i_num_part_in_cu_width - 1 )           // is not at the last column of lcu but is last row of lcu
			{
				i_abs_part_addr = h->scan.raster_to_zscan[ (i_abs_part_idx_tmp + i_num_part_in_cu_width + 1)
				                                           % h->cu.pic.i_num_partitions ];
				i_lcu_idx = -1 ;
			}
			else if ( i_abs_part_idx_tmp / i_num_part_in_cu_width < h->cu.pic.i_num_part_in_height - 1 ) // is not at the last row of lcu but is last column of lcu
			{
				i_abs_part_addr = h->scan.raster_to_zscan[ i_abs_part_idx_tmp + 1 ];
				i_lcu_idx = x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu) + 1;
			}
			else //is the right bottom corner of lcu
			{
				i_abs_part_addr = 0;
				i_lcu_idx = -1 ;
			}
		}

		i_ref_idx = 0;
		i_cur_lcu_idx = x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu) ;
		i_array_addr = i_count;
		x265_data_cu_x_derive_center_idx(h, cu, i_pu_idx, &i_part_idx_center );
		b_exist_mv = i_lcu_idx >= 0 && x265_data_cu_x_get_col_mvp( h, cu, REF_PIC_LIST_0, i_lcu_idx, i_abs_part_addr, &col_mv, &i_ref_idx );
		if( b_exist_mv == 0 )
		{
			b_exist_mv = x265_data_cu_x_get_col_mvp(h, cu, REF_PIC_LIST_0, i_cur_lcu_idx, i_part_idx_center, &col_mv, &i_ref_idx );
		}
		if( b_exist_mv )
		{
			dir |= 1;
			x265_mv_field_set_mv_field(&p_mv_field_neighbours[2 * i_array_addr],
										&col_mv, i_ref_idx );
		}

		if ( B_SLICE == h->slice->i_slice_type )
		{
			b_exist_mv = i_lcu_idx >= 0 && x265_data_cu_x_get_col_mvp(h, cu, REF_PIC_LIST_1, i_lcu_idx, i_abs_part_addr, &col_mv, &i_ref_idx);
			if( b_exist_mv == 0 )
			{
				b_exist_mv = x265_data_cu_x_get_col_mvp(h, cu, REF_PIC_LIST_1, i_cur_lcu_idx, i_part_idx_center, &col_mv, &i_ref_idx );
			}
			if( b_exist_mv )
			{
				dir |= 2;
				x265_mv_field_set_mv_field(&p_mv_field_neighbours[2 * i_array_addr + 1],
											&col_mv, i_ref_idx );
			}
		}

		if (dir != 0)
		{
			p_inter_dir_neighbours[i_array_addr] = dir;
			cand_is_inter[i_array_addr] = 1;

			if ( i_mrg_cand_idx == i_count )
			{
				return;
			}
			i_count++;
		}
	}
	// early termination
	if (i_count == h->slice->i_max_num_merge_cand)
	{
		return;
	}

	i_array_addr = i_count;
	i_cutoff = i_array_addr;

	if ( B_SLICE == h->slice->i_slice_type )
	{
		for ( i_idx=0;
			i_idx < i_cutoff * (i_cutoff-1) && i_array_addr!= h->slice->i_max_num_merge_cand;
			i_idx++)
		{
			i = priority_list0[i_idx];
			j = priority_list1[i_idx];
			if ( cand_is_inter[i] && cand_is_inter[j]
			    && (p_inter_dir_neighbours[i]&0x1) && (p_inter_dir_neighbours[j]&0x2))
			{
				cand_is_inter[i_array_addr] = 1;
				p_inter_dir_neighbours[i_array_addr] = 3;

				// get mv from cand[i] and cand[j]
				x265_mv_field_set_mv_field(&p_mv_field_neighbours[i_array_addr << 1],
											x265_mv_field_get_mv(&p_mv_field_neighbours[i<<1]),
											x265_mv_field_get_ref_idx(&p_mv_field_neighbours[i<<1]));
				x265_mv_field_set_mv_field(&p_mv_field_neighbours[( i_array_addr << 1 ) + 1],
											x265_mv_field_get_mv(&p_mv_field_neighbours[(j<<1)+1]),
											x265_mv_field_get_ref_idx(&p_mv_field_neighbours[(j<<1)+1]));

				i_ref_pocl0 = h->fdec->fdec_data->ref_poc[REF_PIC_LIST_0][x265_mv_field_get_ref_idx(&p_mv_field_neighbours[(i_array_addr<<1)])] ;
				i_ref_pocl1 = h->fdec->fdec_data->ref_poc[REF_PIC_LIST_1][x265_mv_field_get_ref_idx(&p_mv_field_neighbours[(i_array_addr<<1)+1])];
				if (i_ref_pocl0 == i_ref_pocl1
						&& x265_mv_is_equal_mv (
								x265_mv_field_get_mv(&p_mv_field_neighbours[(i_array_addr<<1)]),
								x265_mv_field_get_mv(&p_mv_field_neighbours[(i_array_addr<<1)+1]))
					)
				{
					cand_is_inter[i_array_addr] = 0;
				}
				else
				{
					i_array_addr++;
				}
			}
		}
	}
	// early termination
	if (i_array_addr == h->slice->i_max_num_merge_cand)
	{
		return;
	}

	i_num_ref_idx = (B_SLICE == h->slice->i_slice_type)
					? X265_MIN(h->i_ref[REF_PIC_LIST_0], h->i_ref[REF_PIC_LIST_1])
					: h->i_ref[REF_PIC_LIST_0];
	while (i_array_addr < h->slice->i_max_num_merge_cand)
	{
		cand_is_inter[i_array_addr] = 1;
		p_inter_dir_neighbours[i_array_addr] = 1;
		x265_mv_field_set_mv_field(&p_mv_field_neighbours[i_array_addr << 1],
									&zero_mv, i_r);

		if ( B_SLICE == h->slice->i_slice_type )
		{
			p_inter_dir_neighbours[i_array_addr] = 3;
			x265_mv_field_set_mv_field(&p_mv_field_neighbours[(i_array_addr << 1) + 1],
										&zero_mv, i_r);
		}
		i_array_addr++;
		if ( i_refcnt == i_num_ref_idx - 1 )
		{
			i_r = 0;
		}
		else
		{
			++ i_r;
			++ i_refcnt;
		}
	}

	*p_num_valid_merge_cand = i_array_addr;
}

void x265_data_cu_get_part_position( x265_data_cu_t* cu,
									uint32_t i_part_idx,
									int32_t* p_x_p,
									int32_t* p_y_p,
									int32_t* p_psw,
									int32_t* p_psh)
{
	uint32_t col = cu->i_cu_pel_x;
	uint32_t row = cu->i_cu_pel_y;

	switch ( cu->part_size[0] )
	{
	case SIZE_2NxN:
		*p_psw = x265_data_cu_get_width_p2(cu, 0);
		*p_psh = x265_data_cu_get_height_p2(cu, 0) >> 1;
		*p_x_p   = col;
		*p_y_p   = (i_part_idx == 0)? row: row + *p_psh;
		break;
	case SIZE_Nx2N:
		*p_psw = x265_data_cu_get_width_p2(cu, 0) >> 1;
		*p_psh = x265_data_cu_get_height_p2(cu, 0);
		*p_x_p   = (i_part_idx == 0)? col: col + *p_psw;
		*p_y_p   = row;
		break;
	case SIZE_NxN:
		*p_psw = x265_data_cu_get_width_p2(cu, 0) >> 1;
		*p_psh = x265_data_cu_get_height_p2(cu, 0) >> 1;
		*p_x_p   = col + (i_part_idx*0x1)**p_psw;
		*p_y_p   = row + (i_part_idx>>1)**p_psh;
		break;
	case SIZE_2NxnU:
		*p_psw = x265_data_cu_get_width_p2(cu, 0);
		*p_psh = ( i_part_idx == 0 ) ?  x265_data_cu_get_height_p2(cu, 0) >> 2 : ( x265_data_cu_get_height_p2(cu, 0) >> 2 ) + ( x265_data_cu_get_height_p2(cu, 0) >> 1 );
		*p_x_p   = col;
		*p_y_p   = (i_part_idx ==0)? row: row + x265_data_cu_get_height_p2(cu, 0) - *p_psh;

		break;
	case SIZE_2NxnD:
		*p_psw = x265_data_cu_get_width_p2(cu, 0);
		*p_psh = ( i_part_idx == 0 ) ?  ( x265_data_cu_get_height_p2(cu, 0) >> 2 ) + ( x265_data_cu_get_height_p2(cu, 0) >> 1 ) : x265_data_cu_get_height_p2(cu, 0) >> 2;
		*p_x_p   = col;
		*p_y_p   = (i_part_idx ==0)? row: row + x265_data_cu_get_height_p2(cu, 0) - *p_psh;
		break;
	case SIZE_nLx2N:
		*p_psw = ( i_part_idx == 0 ) ? x265_data_cu_get_width_p2(cu, 0) >> 2 : ( x265_data_cu_get_width_p2(cu, 0) >> 2 ) + ( x265_data_cu_get_width_p2(cu, 0) >> 1 );
		*p_psh = x265_data_cu_get_height_p2(cu, 0);
		*p_x_p   = (i_part_idx ==0)? col: col + x265_data_cu_get_width_p2(cu, 0) - *p_psw;
		*p_y_p   = row;
		break;
	case SIZE_nRx2N:
		*p_psw = ( i_part_idx == 0 ) ? ( x265_data_cu_get_width_p2(cu, 0) >> 2 ) + ( x265_data_cu_get_width_p2(cu, 0) >> 1 ) : x265_data_cu_get_width_p2(cu, 0) >> 2;
		*p_psh = x265_data_cu_get_height_p2(cu, 0);
		*p_x_p   = (i_part_idx ==0)? col: col + x265_data_cu_get_width_p2(cu, 0) - *p_psw;
		*p_y_p   = row;
		break;
	default:
		assert ( cu->part_size[0] == SIZE_2Nx2N );
		*p_psw = x265_data_cu_get_width_p2(cu, 0);
		*p_psh = x265_data_cu_get_height_p2(cu, 0);
		*p_x_p   = col ;
		*p_y_p   = row ;

		break;
	}
}

void x265_data_cu_fill_mvp_cand( x265_t* h,
								x265_data_cu_t* cu,
								uint32_t i_part_idx,
								uint32_t i_part_addr,
								enum ref_pic_list_e i_ref_pic_list,
								int32_t i_ref_idx,
								x265_amvp_info_t* p_amvp_info )
{
	int32_t b_added_smvp = 0;
	//-- get spatial mv
	uint32_t i_part_idx_lt, i_part_idx_rt, i_part_idx_lb;
	uint32_t i_num_part_in_cu_width = 0 ;
	uint32_t idx;
	int32_t b_added = 0;
	int32_t i_ref_idx_col = 0;
	uint32_t i_part_idx_rb;
	uint32_t i_abs_part_idx;
	uint32_t i_abs_part_addr;
	int32_t i_lcu_idx = 0;
	uint32_t i_part_idx_center;
	uint32_t i_cur_lcu_idx = 0 ;
	x265_mv_t col_mv;
	x265_base_data_cu_t* tmp_cu = NULL;

	p_amvp_info->i_n = 0;
	if (i_ref_idx < 0)
	{
		return;
	}

	i_num_part_in_cu_width = h->cu.pic.i_num_part_in_width ;

	x265_data_cu_derive_left_right_top_idx(h, cu, i_part_idx, &i_part_idx_lt, &i_part_idx_rt );
	x265_data_cu_derive_left_bottom_idx(h, cu, i_part_idx, &i_part_idx_lb );

	tmp_cu = x265_data_cu_get_pu_below_left(h, cu, &idx, i_part_idx_lb, 1);
	b_added_smvp = (tmp_cu != NULL)
					&& (x265_base_data_cu_get_prediction_mode_p2(tmp_cu, idx) != MODE_INTRA);

	if (!b_added_smvp)
	{
		tmp_cu = x265_base_data_cu_get_pu_left(h,
												(x265_base_data_cu_t*)cu,
												&idx,
												i_part_idx_lb, 1, 1);
		b_added_smvp = (tmp_cu != NULL)
						&& (x265_base_data_cu_get_prediction_mode_p2(tmp_cu, idx) != MODE_INTRA);
	}

	// left predictor search
	b_added = x265_data_cu_x_add_mvp_cand(h, cu, p_amvp_info, i_ref_pic_list, i_ref_idx, i_part_idx_lb, MD_BELOW_LEFT);
	if (!b_added)
	{
		b_added = x265_data_cu_x_add_mvp_cand(h, cu, p_amvp_info, i_ref_pic_list, i_ref_idx, i_part_idx_lb, MD_LEFT );
	}

	if(!b_added)
	{
		b_added = x265_data_cu_x_add_mvp_cand_order(h, cu, p_amvp_info, i_ref_pic_list, i_ref_idx, i_part_idx_lb, MD_BELOW_LEFT);
		if (!b_added)
		{
			b_added = x265_data_cu_x_add_mvp_cand_order(h, cu, p_amvp_info, i_ref_pic_list, i_ref_idx, i_part_idx_lb, MD_LEFT );
		}
	}
	// above predictor search
	b_added = x265_data_cu_x_add_mvp_cand(h, cu, p_amvp_info, i_ref_pic_list, i_ref_idx, i_part_idx_rt, MD_ABOVE_RIGHT);

	if (!b_added)
	{
		b_added = x265_data_cu_x_add_mvp_cand(h, cu, p_amvp_info, i_ref_pic_list, i_ref_idx, i_part_idx_rt, MD_ABOVE);
	}

	if(!b_added)
	{
		b_added = x265_data_cu_x_add_mvp_cand(h, cu, p_amvp_info, i_ref_pic_list, i_ref_idx, i_part_idx_lt, MD_ABOVE_LEFT);
	}
	b_added = b_added_smvp;
	if (p_amvp_info->i_n==2)
	{
		b_added = 1;
	}

	if(!b_added)
	{
		b_added = x265_data_cu_x_add_mvp_cand_order(h, cu, p_amvp_info, i_ref_pic_list, i_ref_idx, i_part_idx_rt, MD_ABOVE_RIGHT);
		if (!b_added)
		{
			b_added = x265_data_cu_x_add_mvp_cand_order(h, cu, p_amvp_info, i_ref_pic_list, i_ref_idx, i_part_idx_rt, MD_ABOVE);
		}

		if(!b_added)
		{
			b_added = x265_data_cu_x_add_mvp_cand_order(h, cu, p_amvp_info, i_ref_pic_list, i_ref_idx, i_part_idx_lt, MD_ABOVE_LEFT);
		}
	}

	if ( p_amvp_info->i_n == 2 )
	{
		if (x265_mv_is_equal_mv(&p_amvp_info->mv_cand[ 0 ], &p_amvp_info->mv_cand[ 1 ]))
		{
			p_amvp_info->i_n = 1;
		}
	}

	if ( h->slice->b_enable_tmvp_flag )
	{
		// get temporal motion predictor
		i_ref_idx_col = i_ref_idx;
		i_lcu_idx = x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu) ;

		x265_data_cu_derive_right_bottom_idx( h, cu, i_part_idx, &i_part_idx_rb );
		i_abs_part_addr = cu->base_data_cu.i_abs_idx_in_lcu + i_part_addr;

		//----  co-located right_bottom temporal predictor (h) ---//
		i_abs_part_idx = h->scan.zscan_to_raster[i_part_idx_rb];
		if ( ( x265_data_cu_get_cu_pel_x(h->data_cu)
				+ h->scan.raster_to_pel_x[i_abs_part_idx]
				+ h->cu.pic.i_min_cu_width )
				>= h->sps[0].i_pic_width_in_luma_samples )  // image boundary check
		{
			i_lcu_idx = -1;
		}
		else if ( ( x265_data_cu_get_cu_pel_y(h->data_cu)
				+ h->scan.raster_to_pel_y[i_abs_part_idx]
				+ h->cu.pic.i_min_cu_height )
				>= h->sps[0].i_pic_height_in_luma_samples )
		{
			i_lcu_idx = -1;
		}
		else
		{
			if ( ( i_abs_part_idx % i_num_part_in_cu_width < i_num_part_in_cu_width - 1 )           // is not at the last column of lcu
					&& ( i_abs_part_idx / i_num_part_in_cu_width < h->cu.pic.i_num_part_in_height - 1 ) ) // is not at the last row    of lcu
			{
				i_abs_part_addr = h->scan.raster_to_zscan[ i_abs_part_idx + i_num_part_in_cu_width + 1 ];
				i_lcu_idx = x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu);
			}
			else if ( i_abs_part_idx % i_num_part_in_cu_width < i_num_part_in_cu_width - 1 )           // is not at the last column of lcu but is last row of lcu
			{
				i_abs_part_addr = h->scan.raster_to_zscan[ (i_abs_part_idx + i_num_part_in_cu_width + 1)
				                                           % h->cu.pic.i_num_partitions ];
				i_lcu_idx      = -1 ;
			}
			else if ( i_abs_part_idx / i_num_part_in_cu_width < h->cu.pic.i_num_part_in_height - 1 ) // is not at the last row of lcu but is last column of lcu
			{
				i_abs_part_addr = h->scan.raster_to_zscan[ i_abs_part_idx + 1 ];
				i_lcu_idx = x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu) + 1;
			}
			else //is the right bottom corner of lcu
			{
				i_abs_part_addr = 0;
				i_lcu_idx      = -1 ;
			}
		}
		if ( i_lcu_idx >= 0 && x265_data_cu_x_get_col_mvp(h, cu, i_ref_pic_list, i_lcu_idx, i_abs_part_addr, &col_mv, &i_ref_idx_col ) )
		{
			memcpy(&p_amvp_info->mv_cand[p_amvp_info->i_n++],
					&col_mv, sizeof(x265_mv_t) );
		}
		else
		{
			i_cur_lcu_idx = x265_base_data_cu_get_addr((x265_base_data_cu_t*)cu);
			x265_data_cu_x_derive_center_idx(h, cu, i_part_idx, &i_part_idx_center);
			if (x265_data_cu_x_get_col_mvp(h, cu, i_ref_pic_list, i_cur_lcu_idx, i_part_idx_center,  &col_mv, &i_ref_idx_col ))
			{
				memcpy(&p_amvp_info->mv_cand[p_amvp_info->i_n++],
						&col_mv, sizeof(x265_mv_t) );
			}
		}
		//----  co-located right_bottom temporal predictor  ---//
	}

	if (p_amvp_info->i_n > X265_AMVP_MAX_NUM_CANDS )
	{
		p_amvp_info->i_n = X265_AMVP_MAX_NUM_CANDS;
	}
	while (p_amvp_info->i_n < X265_AMVP_MAX_NUM_CANDS)
	{
		x265_mv_set_zero(&p_amvp_info->mv_cand[p_amvp_info->i_n]);
		p_amvp_info->i_n++;
	}
	return ;
}

int32_t x265_data_cu_is_bipred_restriction( x265_data_cu_t* cu, uint32_t i_pu_idx )
{
	int32_t width = 0;
	int32_t height = 0;
	uint32_t part_addr;

	x265_data_cu_get_part_index_and_size(cu, i_pu_idx, &part_addr, &width, &height );
	if ( x265_data_cu_get_width_p2(cu, 0) == 8 && (width < 8 || height < 8) )
	{
		return 1;
	}
	return 0;
}

void x265_data_cu_clip_mv (x265_t* h, x265_data_cu_t* cu, x265_mv_t* p_mv )
{
	int32_t i_mv_shift = 2;
	int32_t i_offset = 8;
	int32_t i_hor_max = ( h->sps[0].i_pic_width_in_luma_samples + i_offset - cu->i_cu_pel_x - 1 ) << i_mv_shift;
	int32_t i_hor_min = ( -(int32_t)h->cu.pic.i_max_cu_width - i_offset - (int32_t)cu->i_cu_pel_x + 1 ) << i_mv_shift;

	int32_t i_ver_max = ( h->sps[0].i_pic_height_in_luma_samples + i_offset - cu->i_cu_pel_y - 1 ) << i_mv_shift;
	int32_t i_ver_min = ( -(int32_t)h->cu.pic.i_max_cu_height - i_offset - (int32_t)cu->i_cu_pel_y + 1 ) << i_mv_shift;

	p_mv->i_hor = X265_MIN(i_hor_max, X265_MAX(i_hor_min, p_mv->i_hor)) ;
	p_mv->i_ver = X265_MIN(i_ver_max, X265_MAX(i_ver_min, p_mv->i_ver)) ;
}

uint32_t x265_data_cu_get_intra_size_idx ( x265_data_cu_t* cu, uint32_t i_abs_part_idx )
{
	uint32_t i_shift = ( (cu->base_data_cu.tr_idx[i_abs_part_idx]==0)
						&& (cu->part_size[i_abs_part_idx]==SIZE_NxN) )
							? cu->base_data_cu.tr_idx[i_abs_part_idx] + 1
							: cu->base_data_cu.tr_idx[i_abs_part_idx];
	i_shift = ( cu->part_size[i_abs_part_idx]==SIZE_NxN ? 1 : 0 );

	uint8_t i_width = cu->width[i_abs_part_idx]>>i_shift;
	uint32_t  i_cnt = 0;
	while( i_width )
	{
		i_cnt++;
		i_width>>=1;
	}
	i_cnt -= 2;
	return i_cnt > 6 ? 6 : i_cnt;
}

void x265_data_cu_clear_cbf( x265_t* h,
							x265_data_cu_t* cu,
							uint32_t i_idx,
							enum text_type_e i_text_type,
							uint32_t i_num_parts )
{
	memset( &cu->base_data_cu.cbf[convert_txt_type_to_idx[i_text_type]][i_idx],
			0,
			sizeof(uint8_t) * i_num_parts);
}

// ====================================================================================================================
// protected member functions
// ====================================================================================================================

int32_t x265_data_cu_x_add_mvp_cand( x265_t *h,
									x265_data_cu_t* cu,
									x265_amvp_info_t* p_amp_info,
									enum ref_pic_list_e i_ref_pic_list,
									int32_t i_ref_idx,
									uint32_t i_part_unit_idx,
									enum mvp_dir_e i_mvp_dir )
{
	x265_base_data_cu_t* tmp_cu = NULL;
	uint32_t i_idx;
	x265_mv_t *p_mv_pred = NULL ;
	enum ref_pic_list_e i_ref_pic_list_2nd = 0 ;
	int32_t i_curr_ref_poc = 0;
	int32_t i_neib_ref_poc;

	switch( i_mvp_dir )
	{
	case MD_LEFT:
	{
		tmp_cu = x265_base_data_cu_get_pu_left( h,
												(x265_base_data_cu_t*)cu,
												&i_idx,
												i_part_unit_idx,
												1, 1 ) ;
		break;
	}
	case MD_ABOVE:
	{
		tmp_cu = x265_base_data_cu_get_pu_above( h,
												(x265_base_data_cu_t*)cu,
												&i_idx,
												i_part_unit_idx,
												1, 0, 1 );
		break;
	}
	case MD_ABOVE_RIGHT:
	{
		tmp_cu = x265_data_cu_get_pu_above_right(h, cu,
												&i_idx,
												i_part_unit_idx,
												1 );
		break;
	}
	case MD_BELOW_LEFT:
	{
		tmp_cu = x265_data_cu_get_pu_below_left(h, cu,
												&i_idx,
												i_part_unit_idx, 1 );
		break;
	}
	case MD_ABOVE_LEFT:
	{
		tmp_cu = x265_data_cu_get_pu_above_left(h, cu,
												&i_idx,
												i_part_unit_idx, 1 );

		break;
	}
	default:
	{
		break;
	}
	}

	if ( tmp_cu == NULL )
	{
		return 0;
	}

#if X265_L0363_MVP_POC
	if ( x265_cu_mv_field_get_ref_idx(
			x265_base_data_cu_get_cu_mv_field(tmp_cu, i_ref_pic_list),
			i_idx) >= 0
			&& h->fref[i_ref_pic_list][i_ref_idx]->i_poc
			== h->fdec->fdec_data->ref_poc[i_ref_pic_list]
			   [x265_cu_mv_field_get_ref_idx(
						x265_base_data_cu_get_cu_mv_field(tmp_cu, i_ref_pic_list),
						i_idx)
			   ]
		)
#else
	if(x265_slice_is_equal_ref (h->slice,
								i_ref_pic_list,
								x265_cu_mv_field_get_ref_idx(
										x265_base_data_cu_get_cu_mv_field(tmp_cu, i_ref_pic_list),
										i_idx),
										i_ref_idx ))
#endif
	{
		p_mv_pred = x265_cu_mv_field_get_mv(
								x265_base_data_cu_get_cu_mv_field(tmp_cu, i_ref_pic_list),
								i_idx) ;
		memcpy(&p_amp_info->mv_cand[p_amp_info->i_n++],
				p_mv_pred, sizeof(x265_mv_t));
		return 1;
	}

	i_ref_pic_list_2nd = REF_PIC_LIST_0;
	if( i_ref_pic_list == REF_PIC_LIST_0 )
	{
		i_ref_pic_list_2nd = REF_PIC_LIST_1;
	}
	else if ( i_ref_pic_list == REF_PIC_LIST_1)
	{
		i_ref_pic_list_2nd = REF_PIC_LIST_0;
	}


	i_curr_ref_poc = h->fref[i_ref_pic_list][i_ref_idx]->i_poc ;
	if( x265_cu_mv_field_get_ref_idx(
			x265_base_data_cu_get_cu_mv_field(tmp_cu, i_ref_pic_list_2nd),
			i_idx) >= 0 )
	{
		i_neib_ref_poc = h->fdec->fdec_data->ref_poc[i_ref_pic_list_2nd]
		                        [x265_cu_mv_field_get_ref_idx(
		                        		x265_base_data_cu_get_cu_mv_field(tmp_cu, i_ref_pic_list_2nd),
		                        		i_idx)
		                         ] ;
		if( i_neib_ref_poc == i_curr_ref_poc ) // same reference frame but diff list//
		{
			p_mv_pred = x265_cu_mv_field_get_mv(
									x265_base_data_cu_get_cu_mv_field(tmp_cu, i_ref_pic_list_2nd),
									i_idx) ;
			memcpy(&p_amp_info->mv_cand[p_amp_info->i_n++],
					p_mv_pred, sizeof(x265_mv_t));
			return 1;
		}
	}
	return 0;
}

/**
 * \param p_info
 * \param e_ref_pic_list
 * \param i_ref_idx
 * \param i_part_unit_idx
 * \param e_dir
 * \returns bool
 */
int32_t x265_data_cu_x_add_mvp_cand_order( x265_t *h,
											x265_data_cu_t* cu,
											x265_amvp_info_t* p_amp_info,
											enum ref_pic_list_e i_ref_pic_list,
											int32_t i_ref_idx,
											uint32_t i_part_unit_idx,
											enum mvp_dir_e i_mvp_dir )
{
	x265_base_data_cu_t* tmp_cu = NULL;
	uint32_t i_idx;
	enum ref_pic_list_e i_ref_pic_list_2nd = 0 ;
	int32_t i_curr_poc = 0;
	int32_t i_curr_ref_poc = 0;
	int32_t i_neib_poc = i_curr_poc;
	int32_t i_neib_ref_poc;
	int32_t i_scale = 0 ;
	int32_t b_is_curr_ref_long_term = 0;
	int32_t b_is_neib_ref_long_term = 0;
	x265_mv_t mv ;
	x265_mv_t scale_mv ;
	x265_mv_t *p_mv_pred = NULL ;

	switch( i_mvp_dir )
	{
	case MD_LEFT:
	{
		tmp_cu = x265_base_data_cu_get_pu_left( h,
												(x265_base_data_cu_t*)cu,
												&i_idx,
												i_part_unit_idx,
												1, 1 ) ;
		break;
	}
	case MD_ABOVE:
	{
		tmp_cu = x265_base_data_cu_get_pu_above( h,
												(x265_base_data_cu_t*)cu,
												&i_idx,
												i_part_unit_idx,
												1, 0, 1 );
		break;
	}
	case MD_ABOVE_RIGHT:
	{
		tmp_cu = x265_data_cu_get_pu_above_right(h, cu,
												&i_idx,
												i_part_unit_idx,
												1 );
		break;
	}
	case MD_BELOW_LEFT:
	{
		tmp_cu = x265_data_cu_get_pu_below_left(h, cu,
												&i_idx,
												i_part_unit_idx, 1 );
		break;
	}
	case MD_ABOVE_LEFT:
	{
		tmp_cu = x265_data_cu_get_pu_above_left(h, cu,
												&i_idx,
												i_part_unit_idx, 1 );

		break;
	}
	default:
	{
		break;
	}
	}

	if ( tmp_cu == NULL )
	{
		return 0;
	}

	i_ref_pic_list_2nd = REF_PIC_LIST_0;
	if( i_ref_pic_list == REF_PIC_LIST_0 )
	{
		i_ref_pic_list_2nd = REF_PIC_LIST_1;
	}
	else if ( i_ref_pic_list == REF_PIC_LIST_1)
	{
		i_ref_pic_list_2nd = REF_PIC_LIST_0;
	}

	i_curr_poc = h->fdec->i_poc;
	i_curr_ref_poc = h->fref[i_ref_pic_list][i_ref_idx]->i_poc ;
	i_neib_poc = i_curr_poc;

	b_is_curr_ref_long_term = h->fref[i_ref_pic_list][i_ref_idx]->fdec_data->b_is_long_term;
	//---------------  v1 (end) ------------------//
	if( x265_cu_mv_field_get_ref_idx(
			x265_base_data_cu_get_cu_mv_field(tmp_cu, i_ref_pic_list),
			i_idx) >= 0 )
	{
		i_neib_ref_poc = h->fref[i_ref_pic_list][
		                         x265_cu_mv_field_get_ref_idx(
		                        		 x265_base_data_cu_get_cu_mv_field(tmp_cu, i_ref_pic_list),
		                        		 i_idx)
		                         ]->i_poc;
		p_mv_pred = x265_cu_mv_field_get_mv(
								x265_base_data_cu_get_cu_mv_field(tmp_cu, i_ref_pic_list),
								i_idx) ;
		b_is_neib_ref_long_term = h->fref[i_ref_pic_list][
		                          x265_cu_mv_field_get_ref_idx(
		                        		  x265_base_data_cu_get_cu_mv_field(tmp_cu, i_ref_pic_list),
		                        		  i_idx)
		                        		  ]->fdec_data->b_is_long_term ;
		if ( b_is_curr_ref_long_term == b_is_neib_ref_long_term )
		{
			if ( b_is_curr_ref_long_term || b_is_neib_ref_long_term )
			{
				memcpy(&mv, p_mv_pred, sizeof(x265_mv_t));
			}
			else
			{
				i_scale = x265_data_cu_x_get_dist_scale_factor(cu, i_curr_poc, i_curr_ref_poc, i_neib_poc, i_neib_ref_poc );
				if ( i_scale == 4096 )
				{
					memcpy(&mv, p_mv_pred, sizeof(x265_mv_t));
				}
				else
				{
					x265_mv_scale_mv(p_mv_pred, &scale_mv, i_scale) ;
					memcpy(&mv, &scale_mv, sizeof(x265_mv_t));
				}
			}
			memcpy(&p_amp_info->mv_cand[p_amp_info->i_n++],
					&mv, sizeof(x265_mv_t));
			return 1;
		}
	}
	//---------------------- v2(end) --------------------//
	if( x265_cu_mv_field_get_ref_idx(
			x265_base_data_cu_get_cu_mv_field(tmp_cu, i_ref_pic_list_2nd),
			i_idx) >= 0 )
	{
		i_neib_ref_poc = h->fref[i_ref_pic_list_2nd][
		                         x265_cu_mv_field_get_ref_idx(
		                        		 x265_base_data_cu_get_cu_mv_field(tmp_cu, i_ref_pic_list_2nd),
		                        		 i_idx)
		                         ]->i_poc;
		p_mv_pred = x265_cu_mv_field_get_mv(
								x265_base_data_cu_get_cu_mv_field(tmp_cu, i_ref_pic_list_2nd),
								i_idx) ;
		b_is_neib_ref_long_term = h->fref[i_ref_pic_list_2nd][
		                          x265_cu_mv_field_get_ref_idx(
		                        		  x265_base_data_cu_get_cu_mv_field(tmp_cu, i_ref_pic_list_2nd),
		                        		  i_idx)
		                        		  ]->fdec_data->b_is_long_term ;

		if ( b_is_curr_ref_long_term == b_is_neib_ref_long_term )
		{
			if ( b_is_curr_ref_long_term || b_is_neib_ref_long_term )
			{
				memcpy(&mv, p_mv_pred, sizeof(x265_mv_t));
			}
			else
			{
				i_scale = x265_data_cu_x_get_dist_scale_factor(cu, i_curr_poc, i_curr_ref_poc, i_neib_poc, i_neib_ref_poc );
				if ( i_scale == 4096 )
				{
					memcpy(&mv, p_mv_pred, sizeof(x265_mv_t));
				}
				else
				{
					x265_mv_scale_mv(p_mv_pred, &scale_mv, i_scale) ;
					memcpy(&mv, &scale_mv, sizeof(x265_mv_t));
				}
			}
			memcpy(&p_amp_info->mv_cand[p_amp_info->i_n++],
					&mv, sizeof(x265_mv_t));
			return 1;
		}
	}
	//---------------------- v3(end) --------------------//
	return 0;
}

int32_t x265_data_cu_x_get_col_mvp( x265_t *h,
									x265_data_cu_t* cu,
									enum ref_pic_list_e i_ref_pic_list,
									int32_t i_cu_addr,
									int32_t i_part_unit_idx,
									x265_mv_t* p_mv,
									int32_t* p_ref_idx )
{
	uint32_t i_abs_part_addr = 0 ;
	enum ref_pic_list_e i_col_ref_pic_list = 0 ;
	int32_t i_col_poc = 0 ;
	int32_t i_col_ref_poc = 0 ;
	int32_t i_curr_poc = 0 ;
	int32_t i_curr_ref_poc = 0 ;
	int32_t i_scale = 0 ;
	int32_t i_col_ref_idx = 0 ;
	int32_t b_is_curr_ref_long_term = 0 ;
	int32_t b_is_col_ref_long_term = 0 ;
	x265_mv_t col_mv;
	x265_mv_t scale_mv ;
	x265_frame_t *p_col_frame = 0 ;
	x265_tmvp_data_cu_t *p_col_cu = NULL ;

	i_abs_part_addr = i_part_unit_idx;
	// use coldir.
	p_col_frame = h->fref[(enum ref_pic_list_e)((B_SLICE == h->slice->i_slice_type)
							? 1 - h->slice->i_col_from_l0_flag : 0 )]
						 [h->slice->i_col_ref_idx] ;
	p_col_cu = p_col_frame->fdec_data->tmvp_data_cu[i_cu_addr];
	if ( p_col_cu->frame == NULL
		|| x265_tmvp_data_cu_get_partition_size_p2(p_col_cu ,i_part_unit_idx) == SIZE_NONE )
	{
		return 0;
	}
	i_curr_poc = h->fdec->i_poc ;
	i_curr_ref_poc = h->fref[i_ref_pic_list][*p_ref_idx]->i_poc ;
	i_col_poc = p_col_cu->frame->i_poc ;

	if (x265_tmvp_data_cu_is_intra(p_col_cu, i_abs_part_addr))
	{
		return 0;
	}

	i_col_ref_pic_list = h->slice->b_check_ldc
							? i_ref_pic_list
							: (enum ref_pic_list_e)(h->slice->i_col_from_l0_flag);
	i_col_ref_idx = x265_tmvp_cu_mv_field_get_ref_idx(
						x265_tmvp_data_cu_get_tmvp_cu_mv_field(p_col_cu, (enum ref_pic_list_e)i_col_ref_pic_list),
						i_abs_part_addr) ;
	if (i_col_ref_idx < 0 )
	{
		i_col_ref_pic_list = (enum ref_pic_list_e)(1 - i_col_ref_pic_list);
		i_col_ref_idx = x265_tmvp_cu_mv_field_get_ref_idx(
							x265_tmvp_data_cu_get_tmvp_cu_mv_field(p_col_cu, (enum ref_pic_list_e)i_col_ref_pic_list),
							i_abs_part_addr) ;
		if (i_col_ref_idx < 0 )
		{
			return 0;
		}
	}

	// scale the vector.
	i_col_ref_poc = p_col_cu->frame->fdec_data->ref_poc[i_col_ref_pic_list][i_col_ref_idx] ;
	memcpy(&col_mv,
			x265_tmvp_cu_mv_field_get_mv(
									x265_tmvp_data_cu_get_tmvp_cu_mv_field(p_col_cu, i_col_ref_pic_list),
									i_abs_part_addr),
			sizeof(x265_mv_t)
			);

	i_curr_ref_poc = h->fref[i_ref_pic_list][*p_ref_idx]->i_poc ;
	b_is_curr_ref_long_term = h->fref[i_ref_pic_list][*p_ref_idx]->fdec_data->b_is_long_term;
	b_is_col_ref_long_term = p_col_cu->frame->fdec_data->is_used_as_long_term[i_col_ref_pic_list][i_col_ref_idx] ;

	if ( b_is_curr_ref_long_term != b_is_col_ref_long_term )
	{
		return 0;
	}

	if ( b_is_curr_ref_long_term || b_is_col_ref_long_term )
	{
		memcpy(p_mv, &col_mv, sizeof(x265_mv_t));
	}
	else
	{
		i_scale = x265_data_cu_x_get_dist_scale_factor(cu, i_curr_poc, i_curr_ref_poc, i_col_poc, i_col_ref_poc);
		if ( i_scale == 4096 )
		{
			memcpy(p_mv, &col_mv, sizeof(x265_mv_t));
		}
		else
		{
			x265_mv_scale_mv(&col_mv, &scale_mv, i_scale) ;
			memcpy(p_mv, &scale_mv, sizeof(x265_mv_t));
		}
	}

	//	print_int_state (p_mv->i_hor) ;

	return 1;
}

uint32_t x265_data_cu_x_get_mvd_bits( x265_data_cu_t* cu, x265_mv_t *mvd )
{
	return ( x265_data_cu_x_get_component_bits(cu, mvd->i_hor)
			+ x265_data_cu_x_get_component_bits(cu, mvd->i_ver));
}

uint32_t x265_data_cu_x_get_component_bits ( x265_data_cu_t* cu, int32_t i_val )
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


int32_t x265_data_cu_x_get_dist_scale_factor ( x265_data_cu_t* cu,
												int32_t i_curr_poc,
												int32_t i_curr_ref_poc,
												int32_t i_col_poc,
												int32_t i_col_ref_poc )
{
	int32_t i_diff_poc_d = 0 ;
	int32_t i_diff_poc_b = 0 ;
	int32_t i_t_db = 0 ;
	int32_t i_t_dd = 0 ;
	int32_t i_x = 0 ;
	int32_t i_scale = 0 ;

	i_diff_poc_d = i_col_poc - i_col_ref_poc;
	i_diff_poc_b = i_curr_poc - i_curr_ref_poc;
	if( i_diff_poc_d == i_diff_poc_b )
	{
		return 4096;
	}

	i_t_db = x265_clip3_int32( i_diff_poc_b, -128, 127 );
	i_t_dd = x265_clip3_int32( i_diff_poc_d, -128, 127 );
	i_x = (0x4000 + abs(i_t_dd/2)) / i_t_dd;
	i_scale = x265_clip3_int32( (i_t_db * i_x + 32) >> 6, -4096, 4095 );
	return i_scale;
}

void x265_data_cu_x_derive_center_idx( x265_t* h,
										x265_data_cu_t* cu,
										uint32_t i_part_idx,
										uint32_t* p_part_idx_center )
{
	uint32_t i_part_addr;
	int32_t  i_part_width;
	int32_t  i_part_height;
	x265_data_cu_get_part_index_and_size(cu, i_part_idx, &i_part_addr, &i_part_width, &i_part_height);

	*p_part_idx_center = cu->base_data_cu.i_abs_idx_in_lcu + i_part_addr; // partition origin.
	*p_part_idx_center = h->scan.raster_to_zscan[h->scan.zscan_to_raster[*p_part_idx_center]
						 + ( i_part_height / h->cu.pic.i_min_cu_height )
						 / 2 * h->cu.pic.i_num_part_in_width
						 + ( i_part_width / h->cu.pic.i_min_cu_width ) / 2] ;
}

void x265_data_cu_compress_mv ( x265_data_cu_t *cu )
{
	int32_t i_scale_factor = 0 ;

	i_scale_factor = 4 * X265_AMVP_DECIMATION_FACTOR / cu->i_unit_size ;
	if ( i_scale_factor > 0)
	{
		x265_cu_mv_field_compress ( &cu->base_data_cu.cu_mv_field[REF_PIC_LIST_0],
									cu->base_data_cu.pred_mode,
									i_scale_factor ) ;
		x265_cu_mv_field_compress ( &cu->base_data_cu.cu_mv_field[REF_PIC_LIST_1],
									cu->base_data_cu.pred_mode,
									i_scale_factor ) ;
	}
}

uint32_t x265_data_cu_get_coef_scan_idx( x265_t *h,
										x265_data_cu_t *cu,
										uint32_t i_abs_part_idx,
										uint32_t i_width,
										int32_t b_is_luma,
										int32_t b_is_intra )
{
	uint32_t i_ctx_idx = 0;
	uint32_t i_scan_idx = 0;
	uint32_t i_dir_mode = 0;
	uint32_t i_depth = 0 ;
	uint32_t i_num_parts = 0 ;

	if ( !b_is_intra )
	{
		i_scan_idx = SCAN_DIAG;
		return i_scan_idx;
	}

	switch(i_width)
	{
		case  2: i_ctx_idx = 6; break;
		case  4: i_ctx_idx = 5; break;
		case  8: i_ctx_idx = 4; break;
		case 16: i_ctx_idx = 3; break;
		case 32: i_ctx_idx = 2; break;
		case 64: i_ctx_idx = 1; break;
		default: i_ctx_idx = 0; break;
	}

	if ( b_is_luma )
	{
		i_dir_mode = x265_base_data_cu_get_luma_intra_dir_p2((x265_base_data_cu_t*)cu, i_abs_part_idx);
		i_scan_idx = SCAN_DIAG;
		if (i_ctx_idx >3 && i_ctx_idx < 6) //if multiple scans supported for transform size
		{
			i_scan_idx = abs((int32_t) i_dir_mode - X265_VER_IDX) < 5 ? SCAN_HOR : (abs((int32_t)i_dir_mode - X265_HOR_IDX) < 5 ? SCAN_VER : SCAN_DIAG);
		}
	}
	else
	{
		i_dir_mode = x265_data_cu_get_chroma_intra_dir_p2(cu, i_abs_part_idx);
		if( i_dir_mode == X265_DM_CHROMA_IDX )
		{
			// get number of partitions in current cu
			i_depth = x265_base_data_cu_get_depth_p2((x265_base_data_cu_t*)cu, i_abs_part_idx);
			i_num_parts = h->cu.pic.i_num_partitions >> (2 * i_depth);

			// get luma mode from upper-left corner of current cu
			i_dir_mode = x265_base_data_cu_get_luma_intra_dir_p2((x265_base_data_cu_t*)cu, (i_abs_part_idx / i_num_parts) * i_num_parts);
		}
		i_scan_idx = SCAN_DIAG;
		if (i_ctx_idx >4 && i_ctx_idx < 7) //if multiple scans supported for transform size
		{
			i_scan_idx = abs((int32_t) i_dir_mode - X265_VER_IDX) < 5 ? SCAN_HOR : (abs((int32_t)i_dir_mode - X265_HOR_IDX) < 5 ? SCAN_VER : SCAN_DIAG);
		}
	}

	return i_scan_idx;
}




