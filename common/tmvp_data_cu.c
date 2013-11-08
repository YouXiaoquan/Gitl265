


#include "common.h"

int x265_tmvp_cu_mv_field_create ( x265_tmvp_cu_mv_field_t *tmvp_cu_mv_field, uint32_t i_num_partition )
{
	memset ( tmvp_cu_mv_field, 0 , sizeof(x265_tmvp_cu_mv_field_t) ) ;

	CHECKED_MALLOCZERO ( tmvp_cu_mv_field->mv, i_num_partition * sizeof(x265_mv_t) ) ;
	CHECKED_MALLOCZERO ( tmvp_cu_mv_field->ref_idx, i_num_partition * sizeof(int8_t) ) ;

	tmvp_cu_mv_field->i_num_partition = i_num_partition ;

	return 0 ;

fail:
	x265_tmvp_cu_mv_field_destroy ( tmvp_cu_mv_field ) ;
	return -1 ;
}

void x265_tmvp_cu_mv_field_destroy ( x265_tmvp_cu_mv_field_t *tmvp_cu_mv_field )
{
	x265_free ( tmvp_cu_mv_field->mv ) ;
	x265_free ( tmvp_cu_mv_field->ref_idx ) ;

	tmvp_cu_mv_field->mv = NULL ;
	tmvp_cu_mv_field->ref_idx = NULL ;
	tmvp_cu_mv_field->i_num_partition = 0 ;
}


void x265_tmvp_cu_mv_field_copy_from_cu_mv_field( x265_tmvp_cu_mv_field_t *tmvp_cu_mv_field,
												x265_cu_mv_field_t *cu_mv_field_src )
{

	memcpy( tmvp_cu_mv_field->mv, cu_mv_field_src->mv, tmvp_cu_mv_field->i_num_partition * sizeof(x265_mv_t)) ;
	memcpy( tmvp_cu_mv_field->ref_idx, cu_mv_field_src->ref_idx, tmvp_cu_mv_field->i_num_partition * sizeof(int8_t)) ;

}

void x265_tmvp_cu_mv_field_compress ( x265_tmvp_cu_mv_field_t *tmvp_cu_mv_field,
										int8_t * pred_mode, int32_t i_scale )
{
	int32_t loop = 0 ;
	int32_t n = 0 ;
	int32_t i_part_idx = 0 ;
	int32_t i_ref_idx = 0;

	enum pred_mode_e i_pred_mode = 0 ;
	x265_mv_t mv ;

	n = i_scale * i_scale ;
	assert( n > 0 && n <= tmvp_cu_mv_field->i_num_partition);

	for ( i_part_idx = 0 ;
			i_part_idx < tmvp_cu_mv_field->i_num_partition;
			i_part_idx += n )
	{
		i_pred_mode = MODE_INTRA;
		i_ref_idx = 0;

		memcpy ( &mv, &tmvp_cu_mv_field->mv[i_part_idx], sizeof(x265_mv_t) ) ;

		i_pred_mode = ((enum pred_mode_e)pred_mode[ i_part_idx ]) ;
		i_ref_idx = tmvp_cu_mv_field->ref_idx[ i_part_idx ];
		for ( loop = 0 ; loop < n ; ++ loop )
		{
			memcpy ( &tmvp_cu_mv_field->mv[i_part_idx + loop], &mv, sizeof(x265_mv_t) ) ;
			pred_mode[ i_part_idx + loop ] = i_pred_mode;
			tmvp_cu_mv_field->ref_idx[ i_part_idx + loop ] = i_ref_idx;
		}
	}
}

x265_mv_t *x265_tmvp_cu_mv_field_get_mv ( x265_tmvp_cu_mv_field_t *tmvp_cu_mv_field, int32_t i_idx )
{
	return &tmvp_cu_mv_field->mv[i_idx];
}

int32_t x265_tmvp_cu_mv_field_get_ref_idx( x265_tmvp_cu_mv_field_t *tmvp_cu_mv_field, int32_t i_idx )
{
	return tmvp_cu_mv_field->ref_idx[i_idx];
}


x265_tmvp_data_cu_t *x265_tmvp_data_cu_new ()
{
	x265_tmvp_data_cu_t *tmvp_data_cu = NULL ;

	CHECKED_MALLOCZERO( tmvp_data_cu, sizeof(x265_tmvp_data_cu_t) );
	if ( x265_tmvp_data_cu_init (tmvp_data_cu) )
	{
		goto fail ;
	}

	return tmvp_data_cu ;
fail:
	x265_tmvp_data_cu_delete ( (void*) tmvp_data_cu ) ;
	return NULL ;
}



void x265_tmvp_data_cu_delete ( x265_tmvp_data_cu_t *tmvp_data_cu )
{
	x265_tmvp_data_cu_deinit ( tmvp_data_cu ) ;
	x265_free ( tmvp_data_cu ) ;
}


int x265_tmvp_data_cu_init ( x265_tmvp_data_cu_t *tmvp_data_cu )
{

	return 0 ;
}

void x265_tmvp_data_cu_deinit ( x265_tmvp_data_cu_t *tmvp_data_cu )
{

}

int x265_tmvp_data_cu_create ( x265_tmvp_data_cu_t *tmvp_data_cu,
								uint32_t i_num_partition,
								int32_t i_width,
								int32_t i_height,
								int32_t i_unit_size )
{
	memset ( tmvp_data_cu, 0 , sizeof(x265_tmvp_data_cu_t) ) ;

	tmvp_data_cu->i_num_partition = i_num_partition ;
	tmvp_data_cu->i_unit_size = i_unit_size ;
	if ( x265_tmvp_cu_mv_field_create ( &tmvp_data_cu->tmvp_cu_mv_field[0], i_num_partition ) )
	{
		goto fail ;
	}
	if ( x265_tmvp_cu_mv_field_create ( &tmvp_data_cu->tmvp_cu_mv_field[1], i_num_partition ) )
	{
		goto fail ;
	}
	CHECKED_MALLOCZERO ( tmvp_data_cu->part_size, i_num_partition * sizeof(int8_t) ) ;
	memset ( tmvp_data_cu->part_size, SIZE_NONE , i_num_partition * sizeof(int8_t) ) ;
	CHECKED_MALLOCZERO ( tmvp_data_cu->pred_mode, i_num_partition * sizeof(int8_t) ) ;

	return 0 ;

fail:
	x265_tmvp_data_cu_destroy ( tmvp_data_cu ) ;
	return -1 ;
}

void x265_tmvp_data_cu_destroy ( x265_tmvp_data_cu_t *tmvp_data_cu )
{
	x265_free ( tmvp_data_cu->part_size ) ;
	x265_free ( tmvp_data_cu->pred_mode ) ;

	x265_tmvp_cu_mv_field_destroy ( &tmvp_data_cu->tmvp_cu_mv_field[0] ) ;
	x265_tmvp_cu_mv_field_destroy ( &tmvp_data_cu->tmvp_cu_mv_field[1] ) ;

	tmvp_data_cu->part_size = NULL ;
	tmvp_data_cu->pred_mode = NULL ;
	tmvp_data_cu->i_num_partition = 0 ;
	tmvp_data_cu->frame = NULL ;
}

void x265_tmvp_data_cu_load ( x265_t *h,
							x265_tmvp_data_cu_t *tmvp_data_cu,
							x265_data_cu_t *data_cu )
{
	tmvp_data_cu->frame = h->fdec ;

	x265_tmvp_cu_mv_field_copy_from_cu_mv_field ( &tmvp_data_cu->tmvp_cu_mv_field[REF_PIC_LIST_0],
													&data_cu->base_data_cu.cu_mv_field[REF_PIC_LIST_0] ) ;
	x265_tmvp_cu_mv_field_copy_from_cu_mv_field ( &tmvp_data_cu->tmvp_cu_mv_field[REF_PIC_LIST_1],
													&data_cu->base_data_cu.cu_mv_field[REF_PIC_LIST_1] ) ;

	memcpy( tmvp_data_cu->part_size, data_cu->part_size,
			tmvp_data_cu->i_num_partition * sizeof(int8_t)) ;
	memcpy( tmvp_data_cu->pred_mode, data_cu->base_data_cu.pred_mode,
			tmvp_data_cu->i_num_partition * sizeof(int8_t)) ;

}

void x265_tmvp_data_cu_compress_mv ( x265_tmvp_data_cu_t *tmvp_data_cu )
{
	int32_t i_scale_factor = 0 ;

	i_scale_factor = 4 * X265_AMVP_DECIMATION_FACTOR / tmvp_data_cu->i_unit_size ;
	if ( i_scale_factor > 0)
	{
		x265_tmvp_cu_mv_field_compress ( &tmvp_data_cu->tmvp_cu_mv_field[REF_PIC_LIST_0],
										tmvp_data_cu->pred_mode,
										i_scale_factor ) ;
		x265_tmvp_cu_mv_field_compress ( &tmvp_data_cu->tmvp_cu_mv_field[REF_PIC_LIST_1],
										tmvp_data_cu->pred_mode,
										i_scale_factor ) ;
	}
}

x265_tmvp_cu_mv_field_t *x265_tmvp_data_cu_get_tmvp_cu_mv_field( x265_tmvp_data_cu_t *tmvp_data_cu,
																enum ref_pic_list_e i_ref_pic_list )
{
	return &tmvp_data_cu->tmvp_cu_mv_field[i_ref_pic_list] ;
}

enum part_size_e x265_tmvp_data_cu_get_partition_size_p2 (x265_tmvp_data_cu_t *tmvp_data_cu,
															uint32_t i_idx )
{
	return (enum part_size_e)tmvp_data_cu->part_size[i_idx] ;
}

int32_t x265_tmvp_data_cu_is_intra ( x265_tmvp_data_cu_t *tmvp_data_cu,
									uint32_t i_part_idx )
{
	return tmvp_data_cu->pred_mode[ i_part_idx ] == MODE_INTRA ;
}


