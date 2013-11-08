

#include "common.h"


const uint8_t convert_txt_type_to_idx [4] = { 0, 1, 1, 2 } ;



int32_t x265_raster_address_is_equal_col( int32_t i_addr_a, int32_t i_addr_b, int32_t i_num_units_per_row )
{
	return (( i_addr_a ^ i_addr_b ) &  ( i_num_units_per_row - 1 ) ) == 0;
}

int32_t x265_raster_address_is_equal_row( int32_t i_addr_a, int32_t i_addr_b, int32_t i_num_units_per_row )
{
	return (( i_addr_a ^ i_addr_b ) &~ ( i_num_units_per_row - 1 ) ) == 0;
}

int32_t x265_raster_address_is_equal_row_or_col( int32_t i_addr_a, int32_t i_addr_b, int32_t i_num_units_per_row )
{
	return x265_raster_address_is_equal_col (i_addr_a, i_addr_b, i_num_units_per_row)
			|| x265_raster_address_is_equal_row (i_addr_a, i_addr_b, i_num_units_per_row) ;
}

int32_t x265_raster_address_is_zero_col( int32_t i_addr, int32_t i_num_units_per_row )
{
	// i_addr % i_num_units_per_row == 0
	return ( i_addr & ( i_num_units_per_row - 1 ) ) == 0;
}

int32_t x265_raster_address_is_zero_row( int32_t i_addr, int32_t i_num_units_per_row )
{
	// i_addr / i_num_units_per_row == 0
	return ( i_addr &~ ( i_num_units_per_row - 1 ) ) == 0;
}


int32_t x265_raster_address_less_than_col( int32_t i_addr,
											int32_t i_val,
											int32_t i_num_units_per_row )
{
	return (i_addr & (i_num_units_per_row - 1)) < i_val ;
}

int32_t x265_raster_address_less_than_row( int32_t i_addr,
											int32_t i_val,
											int32_t i_num_units_per_row )
{
	return i_addr < i_val * i_num_units_per_row ;
}

uint32_t x265_base_data_cu_get_total_num_part( x265_base_data_cu_t *base_data_cu )
{
	return base_data_cu->i_num_partition ;
}

uint32_t x265_base_data_cu_get_addr( x265_base_data_cu_t *base_data_cu )
{
	return base_data_cu->i_cu_addr ;
}

uint32_t x265_base_data_cu_get_zorder_idx_in_cu( x265_base_data_cu_t *base_data_cu )
{
	return base_data_cu->i_abs_idx_in_lcu ;
}

x265_base_data_cu_t *x265_base_data_cu_get_cu_left( x265_base_data_cu_t *base_data_cu )
{
	return base_data_cu->cu_left ;
}

x265_base_data_cu_t *x265_base_data_cu_get_cu_above( x265_base_data_cu_t *base_data_cu )
{
	return base_data_cu->cu_above ;
}

int8_t *x265_base_data_cu_get_qp_p1 ( x265_base_data_cu_t *base_data_cu )
{
	return base_data_cu->qp ;
}

int8_t x265_base_data_cu_get_qp_p2 ( x265_base_data_cu_t *base_data_cu, uint32_t i_idx )
{
	return base_data_cu->qp[i_idx] ;
}

int8_t *x265_base_data_cu_get_prediction_mode_p1 ( x265_base_data_cu_t *base_data_cu )
{
	return base_data_cu->pred_mode ;
}

enum pred_mode_e x265_base_data_cu_get_prediction_mode_p2 ( x265_base_data_cu_t *base_data_cu, uint32_t i_idx )
{
	return (enum pred_mode_e)base_data_cu->pred_mode[i_idx] ;
}

uint8_t *x265_base_data_cu_get_depth_p1 ( x265_base_data_cu_t *base_data_cu )
{
	return base_data_cu->depth ;
}

uint8_t x265_base_data_cu_get_depth_p2 ( x265_base_data_cu_t *base_data_cu, uint32_t i_idx )
{
	return base_data_cu->depth[i_idx] ;
}

uint8_t *x265_base_data_cu_get_luma_intra_dir_p1 ( x265_base_data_cu_t *base_data_cu )
{
	return base_data_cu->luma_intra_dir ;
}

uint8_t x265_base_data_cu_get_luma_intra_dir_p2 ( x265_base_data_cu_t *base_data_cu, uint32_t i_idx )
{
	return base_data_cu->luma_intra_dir[i_idx] ;
}

int32_t *x265_base_data_cu_get_skip_flag_p1 ( x265_base_data_cu_t *base_data_cu )
{
	return base_data_cu->skip_flag ;
}

int32_t x265_base_data_cu_get_skip_flag_p2 ( x265_base_data_cu_t *base_data_cu, uint32_t i_idx )
{
	return base_data_cu->skip_flag[i_idx] ;
}

uint8_t *x265_base_data_cu_get_cbf_p2 ( x265_base_data_cu_t *base_data_cu,
										enum text_type_e i_text_type )
{
	return base_data_cu->cbf[convert_txt_type_to_idx[i_text_type]];
}

uint8_t x265_base_data_cu_get_cbf_p3( x265_base_data_cu_t *base_data_cu,
								uint32_t i_idx,
								enum text_type_e i_text_type )
{
	return base_data_cu->cbf[convert_txt_type_to_idx[i_text_type]][i_idx];
}

uint8_t x265_base_data_cu_get_cbf_p4 ( x265_base_data_cu_t *base_data_cu,
										uint32_t i_idx,
										enum text_type_e i_text_type,
										uint32_t i_tr_depth )
{
	return ((x265_base_data_cu_get_cbf_p3(base_data_cu, i_idx, i_text_type )
			>> i_tr_depth) & 0x1) ;
}

uint8_t *x265_base_data_cu_get_transform_idx_p1 ( x265_base_data_cu_t *base_data_cu )
{
	return base_data_cu->tr_idx ;
}

uint8_t x265_base_data_cu_get_transform_idx_p2 ( x265_base_data_cu_t *base_data_cu, uint32_t i_idx )
{
	return base_data_cu->tr_idx[i_idx] ;
}

int32_t *x265_base_data_cu_get_ipcm_flag_p1 ( x265_base_data_cu_t *base_data_cu )
{
	return base_data_cu->ipcm_flag ;
}

int32_t x265_base_data_cu_get_ipcm_flag_p2 ( x265_base_data_cu_t *base_data_cu, uint32_t i_idx )
{
	return base_data_cu->ipcm_flag[i_idx] ;
}

int32_t *x265_base_data_cu_get_cu_transquant_bypass_p1 ( x265_base_data_cu_t *base_data_cu )
{
	return base_data_cu->cu_transquant_bypass ;
}

int32_t x265_base_data_cu_get_cu_transquant_bypass_p2 ( x265_base_data_cu_t *base_data_cu, uint32_t i_idx )
{
	return base_data_cu->cu_transquant_bypass[i_idx] ;
}

uint8_t *x265_base_data_cu_get_inter_dir_p1 ( x265_base_data_cu_t *base_data_cu )
{
	return base_data_cu->inter_dir ;
}

uint8_t x265_base_data_cu_get_inter_dir_p2 ( x265_base_data_cu_t *base_data_cu, uint32_t i_idx )
{
	return base_data_cu->inter_dir[i_idx] ;
}

uint32_t *x265_base_data_cu_get_slice_start_cu_p1 ( x265_base_data_cu_t *base_data_cu )
{
	return base_data_cu->slice_start_cu ;
}

uint32_t x265_base_data_cu_get_slice_start_cu_p2 ( x265_base_data_cu_t *base_data_cu, uint32_t i_idx )
{
	return base_data_cu->slice_start_cu[i_idx] ;
}

x265_cu_mv_field_t *x265_base_data_cu_get_cu_mv_field( x265_base_data_cu_t *base_data_cu,
														enum ref_pic_list_e i_ref_pic_list )
{
	return &base_data_cu->cu_mv_field[i_ref_pic_list] ;
}

uint8_t x265_base_data_cu_get_qt_root_cbf ( x265_base_data_cu_t *base_data_cu, uint32_t i_idx )
{
	return x265_base_data_cu_get_cbf_p4 ( base_data_cu, i_idx, TEXT_LUMA, 0 )
			|| x265_base_data_cu_get_cbf_p4 ( base_data_cu, i_idx, TEXT_CHROMA_U, 0 )
			|| x265_base_data_cu_get_cbf_p4 ( base_data_cu, i_idx, TEXT_CHROMA_V, 0 );
}

int32_t x265_base_data_cu_is_intra ( x265_base_data_cu_t *base_data_cu,
									uint32_t i_part_idx )
{
	return base_data_cu->pred_mode[ i_part_idx ] == MODE_INTRA ;
}

void x265_base_data_set_cbf ( x265_base_data_cu_t *base_data_cu,
								uint32_t i_idx,
								enum text_type_e i_text_type,
								uint8_t i_cbf )
{
	base_data_cu->cbf[convert_txt_type_to_idx[i_text_type]][i_idx] = i_cbf ;
}

void x265_base_data_cu_set_depth ( x265_base_data_cu_t *base_data_cu,
									uint32_t i_idx,
									uint8_t i_depth )
{
	base_data_cu->depth[i_idx] = i_depth ;
}


void x265_base_data_cu_set_prediction_mode ( x265_base_data_cu_t *base_data_cu,
											uint32_t i_idx,
											enum pred_mode_e i_pred_mode)
{
	base_data_cu->pred_mode[i_idx] = i_pred_mode ;
}

void x265_base_data_cu_set_skip_flag ( x265_base_data_cu_t *base_data_cu,
										uint32_t i_idx,
										int32_t b_skip)
{
	base_data_cu->depth[i_idx] = b_skip ;
}

void x265_base_data_cu_set_qp ( x265_base_data_cu_t *base_data_cu,
								uint32_t i_idx,
								int8_t i_value )
{
	base_data_cu->qp[i_idx] = i_value ;
}

void x265_base_data_cu_set_luma_intra_dir ( x265_base_data_cu_t *base_data_cu,
											uint32_t i_idx,
											uint8_t i_dir )
{
	base_data_cu->luma_intra_dir[i_idx] = i_dir ;
}

void x265_base_data_cu_set_inter_dir ( x265_base_data_cu_t *base_data_cu,
										uint32_t i_idx,
										uint8_t i_dir )
{
	base_data_cu->inter_dir[i_idx] = i_dir ;
}

void x265_base_data_cu_set_ipcm_flag( x265_base_data_cu_t* base_data_cu,
										uint32_t i_idx,
										int32_t b_ipcm_flag )
{
	base_data_cu->ipcm_flag[i_idx] = b_ipcm_flag ;
}



x265_ndbf_block_info_t *x265_ndbf_block_info_new ()
{
	x265_ndbf_block_info_t *ndbf_block_info = NULL ;

	CHECKED_MALLOCZERO( ndbf_block_info, sizeof(x265_ndbf_block_info_t) );
	if ( x265_ndbf_block_info_init (ndbf_block_info) )
	{
		goto fail ;
	}

	return ndbf_block_info ;
fail:
	x265_ndbf_block_info_delete ( (void*) ndbf_block_info ) ;
	return NULL ;
}

void x265_ndbf_block_info_delete ( x265_ndbf_block_info_t *ndbf_block_info )
{
	x265_ndbf_block_info_deinit ( ndbf_block_info ) ;
	x265_free ( ndbf_block_info ) ;
}

int x265_ndbf_block_info_init ( x265_ndbf_block_info_t *ndbf_block_info )
{

	return 0 ;
}

void x265_ndbf_block_info_deinit ( x265_ndbf_block_info_t *ndbf_block_info )
{

}

void x265_ndbf_block_info_copy_from ( x265_ndbf_block_info_t *ndbf_block_info,
										x265_ndbf_block_info_t *src_ndbf_block_info )
{
	ndbf_block_info->i_tile_id = src_ndbf_block_info->i_tile_id ;
	ndbf_block_info->i_slice_id = src_ndbf_block_info->i_slice_id ;
	ndbf_block_info->i_start_su = src_ndbf_block_info->i_start_su ;
	ndbf_block_info->i_end_su = src_ndbf_block_info->i_end_su ;
	ndbf_block_info->i_width_su = src_ndbf_block_info->i_width_su ;
	ndbf_block_info->i_height_su = src_ndbf_block_info->i_height_su ;
	ndbf_block_info->i_pos_x = src_ndbf_block_info->i_pos_x ;
	ndbf_block_info->i_pos_y = src_ndbf_block_info->i_pos_y ;
	ndbf_block_info->i_width = src_ndbf_block_info->i_width ;
	ndbf_block_info->i_height = src_ndbf_block_info->i_height ;

	memcpy ( ndbf_block_info->is_border_available,
			src_ndbf_block_info->is_border_available,
			sizeof(int32_t) * NUM_SGU_BORDER ) ;
	ndbf_block_info->b_all_borders_available = src_ndbf_block_info->b_all_borders_available ;
}



x265_base_data_cu_t *x265_base_data_cu_new ()
{
	x265_base_data_cu_t *base_data_cu = NULL ;

	CHECKED_MALLOCZERO( base_data_cu, sizeof(x265_base_data_cu_t) );
	if ( x265_base_data_cu_init (base_data_cu) )
	{
		goto fail ;
	}

	return base_data_cu ;
fail:
	x265_base_data_cu_delete ( (void*) base_data_cu ) ;
	return NULL ;
}

void x265_base_data_cu_delete ( x265_base_data_cu_t *base_data_cu )
{
	x265_base_data_cu_deinit ( base_data_cu ) ;
	x265_free ( base_data_cu ) ;
}

int x265_base_data_cu_init ( x265_base_data_cu_t *base_data_cu )
{
	int32_t loop = 0 ;

	for ( loop = 0 ; loop < 2 ; ++ loop )
	{
		if ( x265_cu_mv_field_init(&base_data_cu->cu_mv_field[loop]) )
		{
			goto fail ;
		}
	}

	return 0 ;
fail :
	x265_base_data_cu_deinit (base_data_cu) ;
	return -1 ;
}

void x265_base_data_cu_deinit ( x265_base_data_cu_t *base_data_cu )
{
	int32_t loop = 0 ;

	for ( loop = 0 ; loop < 2 ; ++ loop )
	{
		x265_cu_mv_field_deinit(&base_data_cu->cu_mv_field[loop]) ;
	}
}

int x265_base_data_cu_create ( x265_base_data_cu_t *base_data_cu,
								uint32_t i_num_partition,
								int32_t i_width,
								int32_t i_height,
								int32_t i_unit_size
#if X265_ADAPTIVE_QP_SELECTION
								, int32_t b_global_rmarl_buffer
#endif
								)
{
	memset ( base_data_cu, 0 , sizeof(x265_base_data_cu_t) ) ;

	base_data_cu->cu_above = NULL ;
	base_data_cu->cu_left = NULL ;

	CHECKED_MALLOCZERO ( base_data_cu->slice_start_cu, i_num_partition * sizeof(uint32_t) ) ;

	CHECKED_MALLOCZERO ( base_data_cu->pred_mode, i_num_partition * sizeof(int8_t) ) ;
	CHECKED_MALLOCZERO ( base_data_cu->depth, i_num_partition * sizeof(uint8_t) ) ;
	CHECKED_MALLOCZERO ( base_data_cu->qp, i_num_partition * sizeof(int8_t) ) ;
	CHECKED_MALLOCZERO ( base_data_cu->inter_dir, i_num_partition * sizeof(uint8_t) ) ;
	CHECKED_MALLOCZERO ( base_data_cu->luma_intra_dir, i_num_partition * sizeof(uint8_t) ) ;

	CHECKED_MALLOCZERO ( base_data_cu->skip_flag, i_num_partition * sizeof(int32_t) ) ;
	CHECKED_MALLOCZERO ( base_data_cu->cbf[0], i_num_partition * sizeof(uint8_t) ) ;
	CHECKED_MALLOCZERO ( base_data_cu->cbf[1], i_num_partition * sizeof(uint8_t) ) ;
	CHECKED_MALLOCZERO ( base_data_cu->cbf[2], i_num_partition * sizeof(uint8_t) ) ;

	CHECKED_MALLOCZERO ( base_data_cu->tr_idx, i_num_partition * sizeof(uint8_t) ) ;
	CHECKED_MALLOCZERO ( base_data_cu->ipcm_flag, i_num_partition * sizeof(int32_t) ) ;
	CHECKED_MALLOCZERO ( base_data_cu->cu_transquant_bypass, i_num_partition * sizeof(uint32_t) ) ;

	if ( x265_cu_mv_field_create ( &base_data_cu->cu_mv_field[0], i_num_partition ) )
	{
		goto fail ;
	}
	if ( x265_cu_mv_field_create ( &base_data_cu->cu_mv_field[1], i_num_partition ) )
	{
		goto fail ;
	}

	base_data_cu->i_num_partition = i_num_partition ;

	return 0 ;

fail:
	x265_base_data_cu_destroy ( base_data_cu ) ;
	return -1 ;
}

void x265_base_data_cu_destroy ( x265_base_data_cu_t *base_data_cu )
{
	base_data_cu->cu_above = NULL ;
	base_data_cu->cu_left = NULL ;
	x265_free ( base_data_cu->slice_start_cu ) ;

	x265_free ( base_data_cu->pred_mode ) ;
	x265_free ( base_data_cu->depth ) ;
	x265_free ( base_data_cu->qp ) ;
	x265_free ( base_data_cu->inter_dir ) ;
	x265_free ( base_data_cu->luma_intra_dir ) ;

	x265_free ( base_data_cu->skip_flag ) ;
	x265_free ( base_data_cu->cbf[0] ) ;
	x265_free ( base_data_cu->cbf[1] ) ;
	x265_free ( base_data_cu->cbf[2] ) ;

	x265_free ( base_data_cu->tr_idx ) ;
	x265_free ( base_data_cu->ipcm_flag ) ;
	x265_free ( base_data_cu->cu_transquant_bypass ) ;

	x265_cu_mv_field_destroy ( &base_data_cu->cu_mv_field[0] ) ;
	x265_cu_mv_field_destroy ( &base_data_cu->cu_mv_field[1] ) ;

	base_data_cu->slice_start_cu = NULL ;

	base_data_cu->pred_mode = NULL ;
	base_data_cu->depth = NULL ;
	base_data_cu->qp = NULL ;
	base_data_cu->inter_dir = NULL ;
	base_data_cu->luma_intra_dir = NULL ;

	base_data_cu->skip_flag = NULL ;
	base_data_cu->cbf[0] = NULL ;
	base_data_cu->cbf[1] = NULL ;
	base_data_cu->cbf[2] = NULL ;

	base_data_cu->tr_idx = NULL ;
	base_data_cu->ipcm_flag = NULL ;
	base_data_cu->cu_transquant_bypass = NULL ;

	base_data_cu->i_num_partition = 0 ;
}

void x265_base_data_cu_load ( x265_base_data_cu_t *base_data_cu,
							x265_base_data_cu_t *base_data_cu_src )
{
	base_data_cu->cu_left = base_data_cu_src->cu_left ;
	base_data_cu->cu_above = base_data_cu_src->cu_above ;

	memcpy ( base_data_cu->slice_start_cu,
			base_data_cu_src->slice_start_cu,
			base_data_cu->i_num_partition * sizeof(uint32_t)) ;

	memcpy ( base_data_cu->pred_mode,
			base_data_cu_src->pred_mode,
			base_data_cu->i_num_partition * sizeof(int8_t)) ;
	memcpy ( base_data_cu->depth,
			base_data_cu_src->depth,
			base_data_cu->i_num_partition * sizeof(uint8_t)) ;
	memcpy ( base_data_cu->qp,
			base_data_cu_src->qp,
			base_data_cu->i_num_partition * sizeof(int8_t)) ;
	memcpy ( base_data_cu->inter_dir,
			base_data_cu_src->inter_dir,
			base_data_cu->i_num_partition * sizeof(uint8_t)) ;
	memcpy ( base_data_cu->luma_intra_dir,
			base_data_cu_src->luma_intra_dir,
			base_data_cu->i_num_partition * sizeof(uint8_t)) ;

	memcpy ( base_data_cu->skip_flag,
			base_data_cu_src->skip_flag,
			base_data_cu->i_num_partition * sizeof(int32_t)) ;
	memcpy ( base_data_cu->cbf[0],
			base_data_cu_src->cbf[0],
			base_data_cu->i_num_partition * sizeof(uint8_t)) ;
	memcpy ( base_data_cu->cbf[1],
			base_data_cu_src->cbf[1],
			base_data_cu->i_num_partition * sizeof(uint8_t)) ;
	memcpy ( base_data_cu->cbf[2],
			base_data_cu_src->cbf[2],
			base_data_cu->i_num_partition * sizeof(uint8_t)) ;

	memcpy ( base_data_cu->tr_idx,
			base_data_cu_src->tr_idx,
			base_data_cu->i_num_partition * sizeof(uint8_t)) ;
	memcpy ( base_data_cu->ipcm_flag,
			base_data_cu_src->ipcm_flag,
			base_data_cu->i_num_partition * sizeof(int32_t)) ;
	memcpy ( base_data_cu->cu_transquant_bypass,
			base_data_cu_src->cu_transquant_bypass,
			base_data_cu->i_num_partition * sizeof(int32_t)) ;

	x265_cu_mv_field_copy_from_cu_mv_field ( &base_data_cu->cu_mv_field[REF_PIC_LIST_0],
											&base_data_cu_src->cu_mv_field[REF_PIC_LIST_0] ) ;
	x265_cu_mv_field_copy_from_cu_mv_field ( &base_data_cu->cu_mv_field[REF_PIC_LIST_1],
											&base_data_cu_src->cu_mv_field[REF_PIC_LIST_1] ) ;

	base_data_cu->i_cu_addr = base_data_cu_src->i_cu_addr ;
	base_data_cu->i_abs_idx_in_lcu = base_data_cu_src->i_abs_idx_in_lcu ;
}

uint32_t x265_base_data_cu_get_cu_pel_x_by_cu_addr ( x265_t *h, uint32_t i_cu_addr )
{
	return (i_cu_addr % h->cu.pic.i_width_in_cu) * h->cu.pic.i_max_cu_width ;
}

uint32_t x265_base_data_cu_get_cu_pel_y_by_cu_addr ( x265_t *h, uint32_t i_cu_addr )
{
	return (i_cu_addr / h->cu.pic.i_width_in_cu) * h->cu.pic.i_max_cu_height ;
}

int32_t x265_base_data_cu_get_last_valid_part_idx ( x265_base_data_cu_t *base_data_cu,
													int32_t i_abs_part_idx )
{
	uint32_t i_depth = 0 ;
	int32_t i_last_valid_part_idx = 0 ;

	i_last_valid_part_idx = i_abs_part_idx - 1 ;
	while ( i_last_valid_part_idx >= 0
       && x265_base_data_cu_get_prediction_mode_p2 ( base_data_cu, i_last_valid_part_idx ) == MODE_NONE )
	{
		i_depth = x265_base_data_cu_get_depth_p2 ( base_data_cu, i_last_valid_part_idx );
		i_last_valid_part_idx -= base_data_cu->i_num_partition>> (i_depth<<1);
	}
	return i_last_valid_part_idx;
}

int32_t x265_base_data_cu_is_diff_mer ( x265_t *h,
										x265_base_data_cu_t *base_data_cu,
										int32_t i_x_n,
										int32_t i_y_n,
										int32_t i_x_p,
										int32_t i_y_p )
{
	uint32_t i_plevel = 0 ;

	i_plevel = h->pps[0].i_log2_parallel_merge_level_minus2 + 2 ;
	if ((i_x_n >> i_plevel)!= (i_x_p >> i_plevel))
	{
		return 1 ;
	}
	if ((i_y_n >> i_plevel)!= (i_y_p >> i_plevel))
	{
		return 1;
	}

	return 0 ;
}

uint32_t x265_base_data_cu_get_scu_addr( x265_t *h, x265_base_data_cu_t *base_data_cu )
{
	return h->inverse_cu_order_map[base_data_cu->i_cu_addr]
	       * (1 << (h->sps[0].i_max_cu_depth << 1)) + base_data_cu->i_abs_idx_in_lcu ;
}

int32_t x265_base_data_cu_is_lossless_coded ( x265_t *h,
											x265_base_data_cu_t *base_data_cu,
											uint32_t i_abs_part_idx )
{
	return (h->pps[0].b_transquant_bypass_enable_flag
			&& x265_base_data_cu_get_cu_transquant_bypass_p2( base_data_cu, i_abs_part_idx) ) ;
}

x265_base_data_cu_t* x265_base_data_cu_get_pu_left( x265_t *h,
												x265_base_data_cu_t* base_data_cu,
												uint32_t* p_left_part_unit_idx,
												uint32_t i_curr_part_unit_idx,
												int32_t b_enforce_slice_restriction,
												int32_t b_enforce_tile_restriction )
{
	uint32_t i_abs_part_idx = h->scan.zscan_to_raster[i_curr_part_unit_idx];
	uint32_t i_abs_zorder_cu_idx = h->scan.zscan_to_raster[base_data_cu->i_abs_idx_in_lcu];
	uint32_t i_num_part_in_cu_width = h->cu.pic.i_num_part_in_width ;

	if ( !x265_raster_address_is_zero_col( i_abs_part_idx, i_num_part_in_cu_width ) )
	{
		*p_left_part_unit_idx = h->scan.raster_to_zscan[ i_abs_part_idx - 1 ];
		if ( x265_raster_address_is_equal_col( i_abs_part_idx, i_abs_zorder_cu_idx, i_num_part_in_cu_width ) )
		{
			return (x265_base_data_cu_t*)h->data_cu;
		}
		else
		{
			*p_left_part_unit_idx -= base_data_cu->i_abs_idx_in_lcu;
			return base_data_cu;
		}
	}

	*p_left_part_unit_idx = h->scan.raster_to_zscan[ i_abs_part_idx + i_num_part_in_cu_width - 1 ];


	if ((b_enforce_slice_restriction
			&& (base_data_cu->cu_left==NULL
				|| x265_base_data_cu_get_scu_addr(h,base_data_cu->cu_left)
				+ *p_left_part_unit_idx
				< x265_base_data_cu_get_slice_start_cu_p2((x265_base_data_cu_t*)h->data_cu, i_curr_part_unit_idx)))
		||
		(b_enforce_tile_restriction
				&& ( base_data_cu->cu_left==NULL
				|| (h->tile_idx_map[x265_base_data_cu_get_addr(base_data_cu->cu_left)]
				    != h->tile_idx_map[x265_base_data_cu_get_addr(base_data_cu)]) )
			)
		)
	{
		return NULL;
	}
	return base_data_cu->cu_left;
}

x265_base_data_cu_t* x265_base_data_cu_get_pu_above( x265_t *h,
													x265_base_data_cu_t* base_data_cu,
													uint32_t* p_above_part_unit_idx,
													uint32_t i_curr_part_unit_idx,
													int32_t b_enforce_slice_restriction,
													int32_t b_planar_at_lcu_boundary ,
													int32_t b_enforce_tile_restriction )
{
	uint32_t i_abs_part_idx = h->scan.zscan_to_raster[i_curr_part_unit_idx];
	uint32_t i_abs_zorder_cu_idx = h->scan.zscan_to_raster[base_data_cu->i_abs_idx_in_lcu];
	uint32_t i_num_part_in_cu_width = h->cu.pic.i_num_part_in_width ;

	if ( !x265_raster_address_is_zero_row( i_abs_part_idx, i_num_part_in_cu_width ) )
	{
		*p_above_part_unit_idx = h->scan.raster_to_zscan[ i_abs_part_idx - i_num_part_in_cu_width ];
		if ( x265_raster_address_is_equal_row( i_abs_part_idx, i_abs_zorder_cu_idx, i_num_part_in_cu_width ) )
		{
			return (x265_base_data_cu_t*)h->data_cu ;
		}
		else
		{
			*p_above_part_unit_idx -= base_data_cu->i_abs_idx_in_lcu;
			return base_data_cu;
		}
	}

	if(b_planar_at_lcu_boundary)
	{
		return NULL;
	}

	*p_above_part_unit_idx = h->scan.raster_to_zscan[ i_abs_part_idx + h->cu.pic.i_num_partitions - i_num_part_in_cu_width ];

	if ( (b_enforce_slice_restriction
			&& (base_data_cu->cu_above==NULL
				|| x265_base_data_cu_get_scu_addr(h,base_data_cu->cu_above)
				+ *p_above_part_unit_idx
				< x265_base_data_cu_get_slice_start_cu_p2((x265_base_data_cu_t*)h->data_cu, i_curr_part_unit_idx)))
		||
		(b_enforce_tile_restriction
				&& (base_data_cu->cu_above==NULL
				|| (h->tile_idx_map[x265_base_data_cu_get_addr(base_data_cu->cu_above)]
					!= h->tile_idx_map[x265_base_data_cu_get_addr(base_data_cu)]) )
			)
		)
	{
		return NULL;
	}
	return base_data_cu->cu_above;
}

int32_t x265_base_data_cu_has_equal_motion ( x265_base_data_cu_t *base_data_cu,
											uint32_t i_abs_part_idx,
											x265_base_data_cu_t *base_data_cu_cand,
											uint32_t i_cand_abs_part_idx )
{
	uint32_t i_ref_list_idx = 0 ;

	if ( x265_base_data_cu_get_inter_dir_p2(base_data_cu, i_abs_part_idx)
		!= x265_base_data_cu_get_inter_dir_p2(base_data_cu_cand, i_cand_abs_part_idx) )
	{
		return 0 ;
	}

	for ( i_ref_list_idx = 0 ; i_ref_list_idx < 2 ; ++ i_ref_list_idx )
	{
		if ( x265_base_data_cu_get_inter_dir_p2(base_data_cu, i_abs_part_idx) & ( 1 << i_ref_list_idx ) )
	    {
			if ( (!x265_mv_is_equal_mv(
					x265_cu_mv_field_get_mv(
							x265_base_data_cu_get_cu_mv_field(base_data_cu,
															(enum ref_pic_list_e)i_ref_list_idx
															),
											i_abs_part_idx
											),
					x265_cu_mv_field_get_mv(
							x265_base_data_cu_get_cu_mv_field(base_data_cu_cand,
															(enum ref_pic_list_e)i_ref_list_idx
															),
											i_cand_abs_part_idx
											)
										)
					)
					||
				(x265_cu_mv_field_get_ref_idx(
						x265_base_data_cu_get_cu_mv_field(base_data_cu,
														(enum ref_pic_list_e)i_ref_list_idx
														),
													i_abs_part_idx
													) !=
				x265_cu_mv_field_get_ref_idx(
						x265_base_data_cu_get_cu_mv_field(base_data_cu_cand,
														(enum ref_pic_list_e)i_ref_list_idx
														),
													i_cand_abs_part_idx
													)
										)

				)
			{
				return 0 ;
			}
	    }
	}

	return 1 ;
}


int x265_base_data_cu_get_mv_field ( x265_base_data_cu_t *base_data_cu,
										x265_base_data_cu_t *base_data_cu_src,
										uint32_t i_abs_part_idx,
										enum ref_pic_list_e i_ref_pic_list,
										x265_mv_field_t *mv_field )
{
	x265_mv_t *zero_mv = NULL ;
	x265_cu_mv_field_t *cu_mv_field = NULL;

	if ( base_data_cu_src == NULL )  // OUT OF BOUNDARY
	{
		zero_mv = x265_mv_new () ;
		if ( NULL == zero_mv )
		{
			goto fail ;
		}
		x265_mv_set_zero ( zero_mv ) ;
		x265_mv_field_set_mv_field ( mv_field, zero_mv, X265_NOT_VALID );
		x265_mv_delete ( zero_mv ) ;
		zero_mv = NULL ;
		return 0 ;
	}

	cu_mv_field = x265_base_data_cu_get_cu_mv_field ( base_data_cu, i_ref_pic_list) ;
	x265_mv_field_set_mv_field ( mv_field,
								x265_cu_mv_field_get_mv(cu_mv_field, i_abs_part_idx),
								x265_cu_mv_field_get_ref_idx(cu_mv_field, i_abs_part_idx));
	return 0 ;

fail:

	x265_mv_delete ( zero_mv ) ;
	zero_mv = NULL ;
	return -1 ;
}



int32_t x265_base_data_cu_is_skipped ( x265_base_data_cu_t *base_data_cu, uint32_t i_part_idx )
{
	return (x265_base_data_cu_get_skip_flag_p2(base_data_cu, i_part_idx)) ;
}

int8_t x265_base_data_cu_get_last_coded_qp ( x265_t *h,
											x265_base_data_cu_t *base_data_cu,
											uint32_t i_abs_part_idx )
{
	uint32_t i_qu_part_idx_mask = 0 ;
	uint32_t i_last_valid_part_idx = 0 ;
	uint32_t i_last_cu_addr = 0 ;


	i_last_valid_part_idx = x265_base_data_cu_get_last_valid_part_idx( base_data_cu,
																		i_abs_part_idx & i_qu_part_idx_mask );

	i_qu_part_idx_mask = ~((1<<((h->cu.pic.i_total_depth - h->pps[0].i_max_cu_dqp_depth )<< 1)) - 1) ;
	if ( i_abs_part_idx < base_data_cu->i_num_partition
			&& (x265_base_data_cu_get_scu_addr(h, base_data_cu)
				+ i_last_valid_part_idx
				< x265_base_data_cu_get_slice_start_cu_p2( base_data_cu, base_data_cu->i_abs_idx_in_lcu + i_abs_part_idx)))
	{
		return h->slice->i_slice_qp ;
	}
	else if ( i_last_valid_part_idx >= 0 )
	{
		return x265_base_data_cu_get_qp_p2 ( base_data_cu, i_last_valid_part_idx );
	}
	else
	{
		if ( x265_base_data_cu_get_zorder_idx_in_cu(base_data_cu) > 0 )
		{
			return x265_base_data_cu_get_last_coded_qp ( h,
														(x265_base_data_cu_t*)h->data_cu,
														x265_base_data_cu_get_zorder_idx_in_cu(base_data_cu) ) ;
		}
		else if ( h->inverse_cu_order_map[x265_base_data_cu_get_addr(base_data_cu)] > 0
				&& h->tile_idx_map[x265_base_data_cu_get_addr(base_data_cu)]
				== h->tile_idx_map[h->cu_order_map[
				   h->inverse_cu_order_map[x265_base_data_cu_get_addr(base_data_cu)]-1]
				   ]
				&& !( h->pps[0].b_entropy_coding_sync_enabled_flag
						&& x265_base_data_cu_get_addr(base_data_cu) % h->cu.pic.i_width_in_cu == 0 ) )
		{
			i_last_cu_addr = h->cu_order_map[
			              	h->inverse_cu_order_map[x265_base_data_cu_get_addr(base_data_cu)]-1] ; ;
			if ( ( i_last_cu_addr / h->cu.pic.i_width_in_cu )
					== (x265_base_data_cu_get_addr(base_data_cu) / h->cu.pic.i_width_in_cu) )
			{
				return x265_base_data_cu_get_last_coded_qp (h,
															x265_pic_sym_get_left_cu(h, x265_base_data_cu_get_addr(base_data_cu)),
															h->cu.pic.i_num_partitions
															);
			}
			else if ( ( i_last_cu_addr / h->cu.pic.i_width_in_cu )
					== (x265_base_data_cu_get_addr(base_data_cu) / h->cu.pic.i_width_in_cu - 1) )
			{
				return x265_base_data_cu_get_last_coded_qp (h,
															x265_pic_sym_get_curr_cu(h, i_last_cu_addr),
															h->cu.pic.i_num_partitions
															);
			}
		}
		else
		{
			return h->slice->i_slice_qp ;
		}
	}

	return 0 ;
}

void x265_base_data_cu_set_cu_transquant_bypass_sub_parts ( x265_t *h,
															x265_base_data_cu_t *base_data_cu,
															int32_t b_flag,
															uint32_t i_abs_part_idx,
															uint32_t i_depth )
{
	int32_t loop = 0 ;
	int32_t i_num_partitions = 0 ;
	int32_t *p_curr = NULL ;

	i_num_partitions = h->cu.pic.i_num_partitions >> ( 2 * i_depth ) ;
	p_curr = base_data_cu->cu_transquant_bypass + i_abs_part_idx ;
	for ( loop = 0 ; loop < i_num_partitions ; ++ loop )
	{
		p_curr[loop] = b_flag ;
	}
}

void x265_base_data_cu_set_depth_sub_parts ( x265_t *h,
											x265_base_data_cu_t *base_data_cu,
											uint32_t i_depth,
											uint32_t i_abs_part_idx )
{
	uint32_t i_curr_part_numb = 0 ;

	i_curr_part_numb = h->cu.pic.i_num_partitions >> (i_depth << 1) ;
	memset( base_data_cu->depth + i_abs_part_idx,
			i_depth,
			sizeof(uint8_t) * i_curr_part_numb );
}

int32_t x265_base_data_cu_is_first_abs_zorder_idx_in_depth ( x265_t *h,
															x265_base_data_cu_t *base_data_cu,
															uint32_t i_abs_part_idx,
															uint32_t i_depth )
{
	uint32_t i_part_numb = 0 ;

	i_part_numb = h->cu.pic.i_num_partitions >> (i_depth << 1);
	return (((base_data_cu->i_abs_idx_in_lcu + i_abs_part_idx)% i_part_numb) == 0);
}

void x265_base_data_cu_set_skip_flag_sub_parts ( x265_t *h,
												x265_base_data_cu_t *base_data_cu,
												int32_t b_skip,
												uint32_t i_abs_part_idx,
												uint32_t i_depth )
{
	int32_t loop = 0 ;
	int32_t i_num_partitions = 0 ;
	int32_t *p_curr = NULL ;

	i_num_partitions = h->cu.pic.i_num_partitions >> ( 2 * i_depth ) ;
	p_curr = base_data_cu->skip_flag + i_abs_part_idx ;
	for ( loop = 0 ; loop < i_num_partitions ; ++ loop )
	{
		p_curr[loop] = b_skip ;
	}
}

void x265_base_data_cu_set_pred_mode_sub_parts ( x265_t *h,
												x265_base_data_cu_t *base_data_cu,
												enum pred_mode_e i_pred_mode,
												uint32_t i_abs_part_idx,
												uint32_t i_depth )
{
	int32_t loop = 0 ;
	int32_t i_num_partitions = 0 ;
	int8_t *p_curr = NULL ;

	i_num_partitions = h->cu.pic.i_num_partitions >> ( 2 * i_depth ) ;
	p_curr = base_data_cu->pred_mode + i_abs_part_idx ;
	for ( loop = 0 ; loop < i_num_partitions ; ++ loop )
	{
		p_curr[loop] = i_pred_mode ;
	}
}

void x265_base_data_cu_set_qp_sub_cus( x265_t *h,
								x265_base_data_cu_t* cu,
								int32_t i_qp,
								x265_base_data_cu_t* base_data_cu,
								uint32_t i_abs_part_idx,
								uint32_t i_depth,
								int32_t* p_found_non_zero_cbf )
{
	uint32_t i_curr_part_numb = 0 ;
	uint32_t i_curr_part_num_q = 0 ;
	uint32_t i_part_unit_idx = 0 ;

	i_curr_part_numb = h->cu.pic.i_num_partitions >> (i_depth << 1);
	i_curr_part_num_q = i_curr_part_numb >> 2;

	if(!(*p_found_non_zero_cbf))
	{
		if(x265_base_data_cu_get_depth_p2(base_data_cu, i_abs_part_idx) > i_depth)
		{
			for ( i_part_unit_idx = 0; i_part_unit_idx < 4; i_part_unit_idx++ )
			{
				x265_base_data_cu_set_qp_sub_cus( h,
												cu,
												i_qp,
												base_data_cu,
												i_abs_part_idx + i_part_unit_idx * i_curr_part_num_q,
												i_depth + 1,
												p_found_non_zero_cbf );
			}
		}
		else
		{
			if( x265_base_data_cu_get_cbf_p3(base_data_cu, i_abs_part_idx, TEXT_LUMA )
				|| x265_base_data_cu_get_cbf_p3(base_data_cu, i_abs_part_idx, TEXT_CHROMA_U )
				|| x265_base_data_cu_get_cbf_p3(base_data_cu, i_abs_part_idx, TEXT_CHROMA_V ) )
			{
				*p_found_non_zero_cbf = 1;
			}
			else
			{
				x265_base_data_cu_set_qp_sub_parts( h, cu, i_qp, i_abs_part_idx, i_depth ) ;
			}
		}
	}
}

void x265_base_data_cu_set_qp_sub_parts( x265_t *h,
									x265_base_data_cu_t *base_data_cu,
									int32_t i_qp,
									uint32_t i_abs_part_idx,
									uint32_t i_depth )
{
	uint32_t i_scu_idx = 0 ;
	uint32_t i_curr_part_numb = 0 ;
	x265_slice_t *slice = NULL ;

	i_curr_part_numb = h->cu.pic.i_num_partitions >> (i_depth << 1);
	slice = h->slice ;

	for( i_scu_idx = i_abs_part_idx;
			i_scu_idx < i_abs_part_idx + i_curr_part_numb;
			i_scu_idx++)
	{
		if( x265_data_cu_get_slice_segment_start_cu_p2(h->data_cu,
			i_scu_idx + x265_base_data_cu_get_zorder_idx_in_cu(base_data_cu))
				== slice->i_slice_segment_cur_start_cu_addr )
		{
			base_data_cu->qp[i_scu_idx] = i_qp;
		}
	}
}

void x265_base_data_cu_set_luma_intra_dir_sub_parts ( x265_t *h,
														x265_base_data_cu_t *base_data_cu,
														uint32_t i_dir,
														uint32_t i_abs_part_idx,
														uint32_t i_depth )
{
	uint32_t i_curr_part_numb = 0 ;

	i_curr_part_numb = h->cu.pic.i_num_partitions >> (i_depth << 1) ;
	memset( base_data_cu->luma_intra_dir + i_abs_part_idx,
			i_dir,
			sizeof(uint8_t) * i_curr_part_numb );

}

void x265_base_data_cu_set_tr_idx_sub_parts ( x265_t *h,
											x265_base_data_cu_t *base_data_cu,
											uint32_t i_tr_idx,
											uint32_t i_abs_part_idx,
											uint32_t i_depth )
{
	uint32_t i_curr_part_numb = 0 ;

	i_curr_part_numb = h->cu.pic.i_num_partitions >> (i_depth << 1) ;
	memset( base_data_cu->tr_idx + i_abs_part_idx,
			i_tr_idx,
			sizeof(uint8_t) * i_curr_part_numb );

}

void x265_base_data_cu_set_ipcm_flag_sub_parts ( x265_t *h,
												x265_base_data_cu_t* base_data_cu,
												int32_t b_pcm_flag,
												uint32_t i_abs_part_idx,
												uint32_t i_depth)
{
	uint32_t i_curr_part_numb = 0 ;

	i_curr_part_numb = h->cu.pic.i_num_partitions >> (i_depth << 1) ;
	memset( base_data_cu->ipcm_flag + i_abs_part_idx,
			b_pcm_flag,
			sizeof(uint8_t) * i_curr_part_numb );

}
