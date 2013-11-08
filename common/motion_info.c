


#include "common.h"


x265_mv_field_t *x265_mv_field_new ()
{
	x265_mv_field_t *mv_field = NULL ;

	CHECKED_MALLOCZERO( mv_field, sizeof(x265_mv_field_t) );
	if ( x265_mv_field_init (mv_field) )
	{
		goto fail ;
	}

	return mv_field ;
fail:
	x265_mv_field_delete ( (void*) mv_field ) ;
	return NULL ;
}

void x265_mv_field_delete ( x265_mv_field_t *mv_field )
{
	x265_mv_field_deinit ( mv_field ) ;
	x265_free ( mv_field ) ;
}

int x265_mv_field_init ( x265_mv_field_t *mv_field )
{

	return 0 ;
}

void x265_mv_field_deinit ( x265_mv_field_t *mv_field )
{

}

x265_mv_t *x265_mv_field_get_mv ( x265_mv_field_t *mv_field )
{
	return &mv_field->mv ;
}


void x265_mv_field_set_mv( x265_mv_field_t *mv_field, x265_mv_t *mv )
{
	memcpy ( &mv_field->mv, mv, sizeof(x265_mv_t)) ;
}

int32_t x265_mv_field_get_ref_idx ( x265_mv_field_t *mv_field )
{
	return mv_field->i_ref_idx ;
}

void x265_mv_field_set_ref_idx( x265_mv_field_t *mv_field, int32_t i_ref_idx )
{
	mv_field->i_ref_idx = i_ref_idx ;
}

void x265_mv_field_set_mv_field( x265_mv_field_t *mv_field,
								x265_mv_t *mv,
								int32_t i_ref_idx )
{
	memcpy ( &mv_field->mv, mv, sizeof(x265_mv_t)) ;
	mv_field->i_ref_idx = i_ref_idx ;
}

x265_cu_mv_field_t *x265_cu_mv_field_new ()
{
	x265_cu_mv_field_t *cu_mv_field = NULL ;

	CHECKED_MALLOCZERO( cu_mv_field, sizeof(x265_cu_mv_field_t) );
	if ( x265_cu_mv_field_init (cu_mv_field) )
	{
		goto fail ;
	}

	return cu_mv_field ;
fail:
	x265_cu_mv_field_delete ( (void*) cu_mv_field ) ;
	return NULL ;
}

void x265_cu_mv_field_delete ( x265_cu_mv_field_t *cu_mv_field )
{
	x265_cu_mv_field_deinit ( cu_mv_field ) ;
	x265_free ( cu_mv_field ) ;
}

int x265_cu_mv_field_init ( x265_cu_mv_field_t *cu_mv_field )
{

	return 0 ;
}

void x265_cu_mv_field_deinit ( x265_cu_mv_field_t *cu_mv_field )
{

}

int x265_cu_mv_field_create ( x265_cu_mv_field_t *cu_mv_field, uint32_t i_num_partition )
{
	memset ( cu_mv_field, 0 , sizeof(x265_cu_mv_field_t) ) ;

	CHECKED_MALLOCZERO ( cu_mv_field->mv, i_num_partition * sizeof(x265_mv_t) ) ;
	CHECKED_MALLOCZERO ( cu_mv_field->mvd, i_num_partition * sizeof(x265_mv_t) ) ;
	CHECKED_MALLOCZERO ( cu_mv_field->ref_idx, i_num_partition * sizeof(int8_t) ) ;

	cu_mv_field->i_num_partition = i_num_partition ;

	return 0 ;

fail:
	x265_cu_mv_field_destroy ( cu_mv_field ) ;
	return -1 ;
}

void x265_cu_mv_field_destroy ( x265_cu_mv_field_t *cu_mv_field )
{
	x265_free ( cu_mv_field->mv ) ;
	x265_free ( cu_mv_field->mvd ) ;
	x265_free ( cu_mv_field->ref_idx ) ;

	cu_mv_field->mv = NULL ;
	cu_mv_field->mvd = NULL ;
	cu_mv_field->ref_idx = NULL ;
	cu_mv_field->i_num_partition = 0 ;
}

x265_mv_t *x265_cu_mv_field_get_mv ( x265_cu_mv_field_t *cu_mv_field, int32_t i_idx )
{
	return &cu_mv_field->mv[i_idx];
}

x265_mv_t *x265_cu_mv_field_get_mvd ( x265_cu_mv_field_t *cu_mv_field, int32_t i_idx )
{
	return &cu_mv_field->mvd[i_idx];
}

int32_t x265_cu_mv_field_get_ref_idx( x265_cu_mv_field_t *cu_mv_field, int32_t i_idx )
{
	return cu_mv_field->ref_idx[i_idx];
}

void x265_cu_mv_field_copy_from_cu_mv_field( x265_cu_mv_field_t *cu_mv_field,
												x265_cu_mv_field_t *cu_mv_field_src )
{
	cu_mv_field->i_num_partition = cu_mv_field_src->i_num_partition ;
	cu_mv_field->amvp_info.i_n = cu_mv_field_src->amvp_info.i_n ;


	memcpy( cu_mv_field->amvp_info.mv_cand,
			cu_mv_field_src->amvp_info.mv_cand,
			X265_AMVP_MAX_NUM_CANDS_MEM * sizeof(x265_mv_t)) ;

	memcpy( cu_mv_field->mv, cu_mv_field_src->mv, cu_mv_field->i_num_partition * sizeof(x265_mv_t)) ;
	memcpy( cu_mv_field->mvd, cu_mv_field_src->mvd, cu_mv_field->i_num_partition * sizeof(x265_mv_t)) ;
	memcpy( cu_mv_field->ref_idx, cu_mv_field_src->ref_idx, cu_mv_field->i_num_partition * sizeof(int8_t)) ;

}

void x265_cu_mv_field_copy_from( x265_cu_mv_field_t *cu_mv_field,
								x265_cu_mv_field_t *cu_mv_field_src,
								int32_t i_num_part_src,
								int32_t i_part_addr_dst )
{
	int32_t i_size_in_mv = 0 ;

	i_size_in_mv = sizeof(x265_mv_t) * i_num_part_src ;

	memcpy( cu_mv_field->mv + i_part_addr_dst,
			cu_mv_field_src->mv,
			i_size_in_mv );
	memcpy( cu_mv_field->mvd + i_part_addr_dst,
			cu_mv_field_src->mvd,
			i_size_in_mv );
	memcpy( cu_mv_field->ref_idx + i_part_addr_dst,
			cu_mv_field_src->ref_idx,
			sizeof(int8_t) * i_num_part_src );

}

void x265_cu_mv_field_copy_to_p3 ( x265_cu_mv_field_t *cu_mv_field,
								x265_cu_mv_field_t *cu_mv_field_dst,
								int32_t i_part_addr_dst )
{
	x265_cu_mv_field_copy_to_p5 ( cu_mv_field,
								cu_mv_field_dst,
								i_part_addr_dst,
								0,
								cu_mv_field->i_num_partition ) ;
}

void x265_cu_mv_field_copy_to_p5 ( x265_cu_mv_field_t *cu_mv_field,
								x265_cu_mv_field_t *cu_mv_field_dst,
								int32_t i_part_addr_dst,
								uint32_t ui_offset,
								uint32_t i_num_part )
{
	int32_t i_size_in_mv = 0 ;
	int32_t i_offset = 0 ;

	i_size_in_mv = sizeof( x265_mv_t ) * i_num_part ;
	i_offset = ui_offset + i_part_addr_dst ;

	memcpy( cu_mv_field_dst->mv + i_offset,
			cu_mv_field->mv + ui_offset,
			i_size_in_mv );
	memcpy( cu_mv_field_dst->mvd + i_offset,
			cu_mv_field->mvd + ui_offset,
			i_size_in_mv );
	memcpy( cu_mv_field_dst->ref_idx + i_offset,
			cu_mv_field->ref_idx + ui_offset,
			sizeof(int8_t) * i_num_part );
}


void x265_cu_mv_field_link_to_with_offset ( x265_cu_mv_field_t *cu_mv_field,
											x265_cu_mv_field_t *cu_mv_field_src,
											int32_t i_offset )
{
	cu_mv_field->mv = cu_mv_field_src->mv + i_offset ;
	cu_mv_field->mvd = cu_mv_field_src->mvd + i_offset ;
	cu_mv_field->ref_idx = cu_mv_field_src->ref_idx + i_offset ;
}

void x265_cu_mv_field_clear_mv_field( x265_cu_mv_field_t *cu_mv_field )
{
	int32_t loop = 0 ;

	for ( loop = 0; loop < cu_mv_field->i_num_partition ; ++ loop )
	{
		x265_mv_set_zero ( &cu_mv_field->mv[loop] ) ;
		x265_mv_set_zero ( &cu_mv_field->mvd[loop] ) ;
	}
	assert( sizeof( *cu_mv_field->ref_idx ) == 1 );
	memset( cu_mv_field->ref_idx,
			X265_NOT_VALID,
			cu_mv_field->i_num_partition * sizeof(*cu_mv_field->ref_idx));
}

void x265_cu_mv_field_compress ( x265_cu_mv_field_t *cu_mv_field,
								int8_t * pred_mode, int32_t i_scale )
{
	int32_t loop = 0 ;
	int32_t n = 0 ;
	int32_t i_part_idx = 0 ;
	int32_t i_ref_idx = 0;

	enum pred_mode_e i_pred_mode = 0 ;
	x265_mv_t mv ;

	n = i_scale * i_scale ;
	assert( n > 0 && n <= cu_mv_field->i_num_partition);

	for ( i_part_idx = 0 ;
			i_part_idx < cu_mv_field->i_num_partition;
			i_part_idx += n )
	{
		i_pred_mode = MODE_INTRA;
		i_ref_idx = 0;

		memcpy ( &mv, &cu_mv_field->mv[i_part_idx], sizeof(x265_mv_t) ) ;

		i_pred_mode = ((enum pred_mode_e)pred_mode[ i_part_idx ]) ;
		i_ref_idx = cu_mv_field->ref_idx[ i_part_idx ];
		for ( loop = 0 ; loop < n ; ++ loop )
		{
			memcpy ( &cu_mv_field->mv[i_part_idx + loop], &mv, sizeof(x265_mv_t) ) ;
			pred_mode[ i_part_idx + loop ] = i_pred_mode;
			cu_mv_field->ref_idx[ i_part_idx + loop ] = i_ref_idx;
		}
	}
}



void x265_cu_mv_field_set_all_mv (x265_cu_mv_field_t *cu_mv_field,
									const x265_mv_t *mv,
									enum part_size_e i_part_size,
									int32_t i_part_addr,
									uint32_t i_depth,
									int32_t i_part_idx)
{
	int32_t loop = 0;
	int32_t i_num_elements = 0;
	int32_t i_curr_part_num_q = 0 ;
	x265_mv_t *p_mv = NULL ;
	x265_mv_t *p_mv1 = NULL ;
	x265_mv_t *p_mv2 = NULL ;
	x265_mv_t *p_mv3 = NULL ;
	x265_mv_t *p_mv4 = NULL ;

	p_mv = cu_mv_field->mv ;
	p_mv += i_part_addr;
	i_num_elements = cu_mv_field->i_num_partition >> ( 2 * i_depth );

	switch( i_part_size )
	{
	case SIZE_2Nx2N:
    	for ( loop = 0; loop < i_num_elements; ++ loop )
    	{
    		memcpy(&p_mv[loop], mv, sizeof(x265_mv_t));
    	}
    	break;

    case SIZE_2NxN:
    	i_num_elements >>= 1;
    	for ( loop = 0; loop < i_num_elements; ++ loop )
    	{
    		memcpy(&p_mv[loop], mv, sizeof(x265_mv_t));
    	}
    	break;

    case SIZE_Nx2N:
    	i_num_elements >>= 2;
    	for ( loop = 0; loop < i_num_elements; ++ loop )
    	{
    		memcpy(&p_mv[loop], mv, sizeof(x265_mv_t));
    		memcpy(&p_mv[loop + i_num_elements * 2], mv, sizeof(x265_mv_t));
    	}
    	break;

    case SIZE_NxN:
    	i_num_elements >>= 2;
    	for ( loop = 0; loop < i_num_elements; ++ loop )
    	{
    		memcpy(&p_mv[loop], mv, sizeof(x265_mv_t));
    	}
    	break;
    case SIZE_2NxnU:
    {
    	i_curr_part_num_q = i_num_elements>>2;
    	if( i_part_idx == 0 )
    	{
    		p_mv1 = p_mv;
    		p_mv2 = p_mv + i_curr_part_num_q;
    		for (loop = 0; loop < (i_curr_part_num_q>>1); loop++)
    		{
    			memcpy(&p_mv1[loop], mv, sizeof(x265_mv_t));
        		memcpy(&p_mv2[loop], mv, sizeof(x265_mv_t));
    		}
    	}
    	else
    	{
    		p_mv1 = p_mv;
    		for (loop = 0; loop < (i_curr_part_num_q>>1); loop++)
    		{
    			memcpy(&p_mv1[loop], mv, sizeof(x265_mv_t));
    		}
    		p_mv1 = p_mv + i_curr_part_num_q;
    		for (loop = 0; loop < ( (i_curr_part_num_q>>1) + (i_curr_part_num_q<<1) ); loop++)
    		{
    			memcpy(&p_mv1[loop], mv, sizeof(x265_mv_t));
    		}
    	}
    	break;
    }
    case SIZE_2NxnD:
    {
    	i_curr_part_num_q = i_num_elements>>2;
    	if( i_part_idx == 0 )
    	{
    		p_mv1  = p_mv;
    		for (loop = 0; loop < ( (i_curr_part_num_q>>1) + (i_curr_part_num_q<<1) ); loop++)
    		{
    			memcpy(&p_mv1[loop], mv, sizeof(x265_mv_t));
    		}
    		p_mv1 = p_mv + ( i_num_elements - i_curr_part_num_q );
    		for (loop = 0; loop < (i_curr_part_num_q>>1); loop++)
    		{
    			memcpy(&p_mv1[loop], mv, sizeof(x265_mv_t));
    		}
    	}
    	else
    	{
    		p_mv1 = p_mv;
    		p_mv2 = p_mv + i_curr_part_num_q;
    		for (loop = 0; loop < (i_curr_part_num_q>>1); loop++)
    		{
    			memcpy(&p_mv1[loop], mv, sizeof(x265_mv_t));
    			memcpy(&p_mv2[loop], mv, sizeof(x265_mv_t));
    		}
    	}
    	break;
    }
    case SIZE_nLx2N:
    {
    	i_curr_part_num_q = i_num_elements>>2;
    	if( i_part_idx == 0 )
    	{
    		p_mv1 = p_mv;
    		p_mv2 = p_mv + (i_curr_part_num_q<<1);
    		p_mv3 = p_mv + (i_curr_part_num_q>>1);
    		p_mv4 = p_mv + (i_curr_part_num_q<<1) + (i_curr_part_num_q>>1);

    		for (loop = 0; loop < (i_curr_part_num_q>>2); loop++)
    		{
    			memcpy(&p_mv1[loop], mv, sizeof(x265_mv_t));
    			memcpy(&p_mv2[loop], mv, sizeof(x265_mv_t));
    			memcpy(&p_mv3[loop], mv, sizeof(x265_mv_t));
    			memcpy(&p_mv4[loop], mv, sizeof(x265_mv_t));
    		}
    	}
    	else
    	{
    		p_mv1 = p_mv;
    		p_mv2 = p_mv + (i_curr_part_num_q<<1);
    		for (loop = 0; loop < (i_curr_part_num_q>>2); loop++)
    		{
    			memcpy(&p_mv1[loop], mv, sizeof(x265_mv_t));
    			memcpy(&p_mv2[loop], mv, sizeof(x265_mv_t));
    		}

    		p_mv1 = p_mv + (i_curr_part_num_q>>1);
    		p_mv2 = p_mv + (i_curr_part_num_q<<1) + (i_curr_part_num_q>>1);
    		for (loop = 0; loop < ( (i_curr_part_num_q>>2) + i_curr_part_num_q ); loop++)
    		{
    			memcpy(&p_mv1[loop], mv, sizeof(x265_mv_t));
    			memcpy(&p_mv2[loop], mv, sizeof(x265_mv_t));
    		}
    	}
    	break;
    }
    case SIZE_nRx2N:
    {
    	i_curr_part_num_q = i_num_elements>>2;
    	if( i_part_idx == 0 )
    	{
    		p_mv1 = p_mv;
    		p_mv2 = p_mv + (i_curr_part_num_q<<1);
    		for (loop = 0; loop < ( (i_curr_part_num_q>>2) + i_curr_part_num_q ); loop++)
    		{
    			memcpy(&p_mv1[loop], mv, sizeof(x265_mv_t));
    			memcpy(&p_mv2[loop], mv, sizeof(x265_mv_t));
    		}

    		p_mv1 = p_mv + i_curr_part_num_q + (i_curr_part_num_q>>1);
    		p_mv2 = p_mv + i_num_elements - i_curr_part_num_q + (i_curr_part_num_q>>1);
    		for (loop = 0; loop < (i_curr_part_num_q>>2); loop++)
    		{
    			memcpy(&p_mv1[loop], mv, sizeof(x265_mv_t));
    			memcpy(&p_mv2[loop], mv, sizeof(x265_mv_t));
    		}
    	}
    	else
    	{
    		p_mv1 = p_mv;
    		p_mv2 = p_mv + (i_curr_part_num_q>>1);
    		p_mv3 = p_mv + (i_curr_part_num_q<<1);
    		p_mv4 = p_mv + (i_curr_part_num_q<<1) + (i_curr_part_num_q>>1);
    		for (loop = 0; loop < (i_curr_part_num_q>>2); loop++)
    		{
    			memcpy(&p_mv1[loop], mv, sizeof(x265_mv_t));
    			memcpy(&p_mv2[loop], mv, sizeof(x265_mv_t));
    			memcpy(&p_mv3[loop], mv, sizeof(x265_mv_t));
    			memcpy(&p_mv4[loop], mv, sizeof(x265_mv_t));
    		}
    	}
    	break;
    }
    default:
    {
    	assert(0);
    	break;
	}
	}
}


void x265_cu_mv_field_set_all_mvd (x265_cu_mv_field_t *cu_mv_field,
									const x265_mv_t *mvd,
									enum part_size_e i_part_size,
									int32_t i_part_addr,
									uint32_t i_depth,
									int32_t i_part_idx)
{
	int32_t loop = 0;
	int32_t i_num_elements = 0;
	int32_t i_curr_part_num_q = 0 ;
	x265_mv_t *p_mvd = NULL ;
	x265_mv_t *p_mvd1 = NULL ;
	x265_mv_t *p_mvd2 = NULL ;
	x265_mv_t *p_mvd3 = NULL ;
	x265_mv_t *p_mvd4 = NULL ;

	p_mvd = cu_mv_field->mvd ;
	p_mvd += i_part_addr;
	i_num_elements = cu_mv_field->i_num_partition >> ( 2 * i_depth );

	switch( i_part_size )
	{
	case SIZE_2Nx2N:
    	for ( loop = 0; loop < i_num_elements; ++ loop )
    	{
    		memcpy(&p_mvd[loop], mvd, sizeof(x265_mv_t));
    	}
    	break;

    case SIZE_2NxN:
    	i_num_elements >>= 1;
    	for ( loop = 0; loop < i_num_elements; ++ loop )
    	{
    		memcpy(&p_mvd[loop], mvd, sizeof(x265_mv_t));
    	}
    	break;

    case SIZE_Nx2N:
    	i_num_elements >>= 2;
    	for ( loop = 0; loop < i_num_elements; ++ loop )
    	{
    		memcpy(&p_mvd[loop], mvd, sizeof(x265_mv_t));
    		memcpy(&p_mvd[loop + i_num_elements * 2], mvd, sizeof(x265_mv_t));
    	}
    	break;

    case SIZE_NxN:
    	i_num_elements >>= 2;
    	for ( loop = 0; loop < i_num_elements; ++ loop )
    	{
    		memcpy(&p_mvd[loop], mvd, sizeof(x265_mv_t));
    	}
    	break;
    case SIZE_2NxnU:
    {
    	i_curr_part_num_q = i_num_elements>>2;
    	if( i_part_idx == 0 )
    	{
    		p_mvd1 = p_mvd;
    		p_mvd2 = p_mvd + i_curr_part_num_q;
    		for (loop = 0; loop < (i_curr_part_num_q>>1); loop++)
    		{
    			memcpy(&p_mvd1[loop], mvd, sizeof(x265_mv_t));
        		memcpy(&p_mvd2[loop], mvd, sizeof(x265_mv_t));
    		}
    	}
    	else
    	{
    		p_mvd1 = p_mvd;
    		for (loop = 0; loop < (i_curr_part_num_q>>1); loop++)
    		{
    			memcpy(&p_mvd1[loop], mvd, sizeof(x265_mv_t));
    		}
    		p_mvd1 = p_mvd + i_curr_part_num_q;
    		for (loop = 0; loop < ( (i_curr_part_num_q>>1) + (i_curr_part_num_q<<1) ); loop++)
    		{
    			memcpy(&p_mvd1[loop], mvd, sizeof(x265_mv_t));
    		}
    	}
    	break;
    }
    case SIZE_2NxnD:
    {
    	i_curr_part_num_q = i_num_elements>>2;
    	if( i_part_idx == 0 )
    	{
    		p_mvd1  = p_mvd;
    		for (loop = 0; loop < ( (i_curr_part_num_q>>1) + (i_curr_part_num_q<<1) ); loop++)
    		{
    			memcpy(&p_mvd1[loop], mvd, sizeof(x265_mv_t));
    		}
    		p_mvd1 = p_mvd + ( i_num_elements - i_curr_part_num_q );
    		for (loop = 0; loop < (i_curr_part_num_q>>1); loop++)
    		{
    			memcpy(&p_mvd1[loop], mvd, sizeof(x265_mv_t));
    		}
    	}
    	else
    	{
    		p_mvd1 = p_mvd;
    		p_mvd2 = p_mvd + i_curr_part_num_q;
    		for (loop = 0; loop < (i_curr_part_num_q>>1); loop++)
    		{
    			memcpy(&p_mvd1[loop], mvd, sizeof(x265_mv_t));
    			memcpy(&p_mvd2[loop], mvd, sizeof(x265_mv_t));
    		}
    	}
    	break;
    }
    case SIZE_nLx2N:
    {
    	i_curr_part_num_q = i_num_elements>>2;
    	if( i_part_idx == 0 )
    	{
    		p_mvd1 = p_mvd;
    		p_mvd2 = p_mvd + (i_curr_part_num_q<<1);
    		p_mvd3 = p_mvd + (i_curr_part_num_q>>1);
    		p_mvd4 = p_mvd + (i_curr_part_num_q<<1) + (i_curr_part_num_q>>1);

    		for (loop = 0; loop < (i_curr_part_num_q>>2); loop++)
    		{
    			memcpy(&p_mvd1[loop], mvd, sizeof(x265_mv_t));
    			memcpy(&p_mvd2[loop], mvd, sizeof(x265_mv_t));
    			memcpy(&p_mvd3[loop], mvd, sizeof(x265_mv_t));
    			memcpy(&p_mvd4[loop], mvd, sizeof(x265_mv_t));
    		}
    	}
    	else
    	{
    		p_mvd1 = p_mvd;
    		p_mvd2 = p_mvd + (i_curr_part_num_q<<1);
    		for (loop = 0; loop < (i_curr_part_num_q>>2); loop++)
    		{
    			memcpy(&p_mvd1[loop], mvd, sizeof(x265_mv_t));
    			memcpy(&p_mvd2[loop], mvd, sizeof(x265_mv_t));
    		}

    		p_mvd1 = p_mvd + (i_curr_part_num_q>>1);
    		p_mvd2 = p_mvd + (i_curr_part_num_q<<1) + (i_curr_part_num_q>>1);
    		for (loop = 0; loop < ( (i_curr_part_num_q>>2) + i_curr_part_num_q ); loop++)
    		{
    			memcpy(&p_mvd1[loop], mvd, sizeof(x265_mv_t));
    			memcpy(&p_mvd2[loop], mvd, sizeof(x265_mv_t));
    		}
    	}
    	break;
    }
    case SIZE_nRx2N:
    {
    	i_curr_part_num_q = i_num_elements>>2;
    	if( i_part_idx == 0 )
    	{
    		p_mvd1 = p_mvd;
    		p_mvd2 = p_mvd + (i_curr_part_num_q<<1);
    		for (loop = 0; loop < ( (i_curr_part_num_q>>2) + i_curr_part_num_q ); loop++)
    		{
    			memcpy(&p_mvd1[loop], mvd, sizeof(x265_mv_t));
    			memcpy(&p_mvd2[loop], mvd, sizeof(x265_mv_t));
    		}

    		p_mvd1 = p_mvd + i_curr_part_num_q + (i_curr_part_num_q>>1);
    		p_mvd2 = p_mvd + i_num_elements - i_curr_part_num_q + (i_curr_part_num_q>>1);
    		for (loop = 0; loop < (i_curr_part_num_q>>2); loop++)
    		{
    			memcpy(&p_mvd1[loop], mvd, sizeof(x265_mv_t));
    			memcpy(&p_mvd2[loop], mvd, sizeof(x265_mv_t));
    		}
    	}
    	else
    	{
    		p_mvd1 = p_mvd;
    		p_mvd2 = p_mvd + (i_curr_part_num_q>>1);
    		p_mvd3 = p_mvd + (i_curr_part_num_q<<1);
    		p_mvd4 = p_mvd + (i_curr_part_num_q<<1) + (i_curr_part_num_q>>1);
    		for (loop = 0; loop < (i_curr_part_num_q>>2); loop++)
    		{
    			memcpy(&p_mvd1[loop], mvd, sizeof(x265_mv_t));
    			memcpy(&p_mvd2[loop], mvd, sizeof(x265_mv_t));
    			memcpy(&p_mvd3[loop], mvd, sizeof(x265_mv_t));
    			memcpy(&p_mvd4[loop], mvd, sizeof(x265_mv_t));
    		}
    	}
    	break;
    }
    default:
    {
    	assert(0);
    	break;
	}
	}
}

void x265_cu_mv_field_set_all_ref_idx (x265_cu_mv_field_t *cu_mv_field,
									const int8_t i_ref_idx,
									enum part_size_e i_part_size,
									int32_t i_part_addr,
									uint32_t i_depth,
									int32_t i_part_idx)
{
	int32_t loop = 0;
	int32_t i_num_elements = 0;
	int32_t i_curr_part_num_q = 0 ;
	int8_t *p_ref_idx = NULL ;
	int8_t *p_ref_idx1 = NULL ;
	int8_t *p_ref_idx2 = NULL ;
	int8_t *p_ref_idx3 = NULL ;
	int8_t *p_ref_idx4 = NULL ;

	p_ref_idx = cu_mv_field->ref_idx ;
	p_ref_idx += i_part_addr;
	i_num_elements = cu_mv_field->i_num_partition >> ( 2 * i_depth );

	switch( i_part_size )
	{
	case SIZE_2Nx2N:
    	for ( loop = 0; loop < i_num_elements; ++ loop )
    	{
    		p_ref_idx[loop] = i_ref_idx;
    	}
    	break;

    case SIZE_2NxN:
    	i_num_elements >>= 1;
    	for ( loop = 0; loop < i_num_elements; ++ loop )
    	{
    		p_ref_idx[loop] = i_ref_idx;
    	}
    	break;

    case SIZE_Nx2N:
    	i_num_elements >>= 2;
    	for ( loop = 0; loop < i_num_elements; ++ loop )
    	{
    		p_ref_idx[loop] = i_ref_idx;
    		p_ref_idx[loop + i_num_elements * 2] = i_ref_idx;
    	}
    	break;

    case SIZE_NxN:
    	i_num_elements >>= 2;
    	for ( loop = 0; loop < i_num_elements; ++ loop )
    	{
    		p_ref_idx[loop] = i_ref_idx;
    	}
    	break;
    case SIZE_2NxnU:
    {
    	i_curr_part_num_q = i_num_elements>>2;
    	if( i_part_idx == 0 )
    	{
    		p_ref_idx1 = p_ref_idx;
    		p_ref_idx2 = p_ref_idx + i_curr_part_num_q;
    		for (loop = 0; loop < (i_curr_part_num_q>>1); loop++)
    		{
    			p_ref_idx1[loop] = i_ref_idx;
        		p_ref_idx2[loop] = i_ref_idx;
    		}
    	}
    	else
    	{
    		p_ref_idx1 = p_ref_idx;
    		for (loop = 0; loop < (i_curr_part_num_q>>1); loop++)
    		{
    			p_ref_idx1[loop] = i_ref_idx;
    		}
    		p_ref_idx1 = p_ref_idx + i_curr_part_num_q;
    		for (loop = 0; loop < ( (i_curr_part_num_q>>1) + (i_curr_part_num_q<<1) ); loop++)
    		{
    			p_ref_idx1[loop] = i_ref_idx;
    		}
    	}
    	break;
    }
    case SIZE_2NxnD:
    {
    	i_curr_part_num_q = i_num_elements>>2;
    	if( i_part_idx == 0 )
    	{
    		p_ref_idx1  = p_ref_idx;
    		for (loop = 0; loop < ( (i_curr_part_num_q>>1) + (i_curr_part_num_q<<1) ); loop++)
    		{
    			p_ref_idx1[loop] = i_ref_idx;
    		}
    		p_ref_idx1 = p_ref_idx + ( i_num_elements - i_curr_part_num_q );
    		for (loop = 0; loop < (i_curr_part_num_q>>1); loop++)
    		{
    			p_ref_idx1[loop] = i_ref_idx;
    		}
    	}
    	else
    	{
    		p_ref_idx1 = p_ref_idx;
    		p_ref_idx2 = p_ref_idx + i_curr_part_num_q;
    		for (loop = 0; loop < (i_curr_part_num_q>>1); loop++)
    		{
    			p_ref_idx1[loop] = i_ref_idx;
    			p_ref_idx2[loop] = i_ref_idx;
    		}
    	}
    	break;
    }
    case SIZE_nLx2N:
    {
    	i_curr_part_num_q = i_num_elements>>2;
    	if( i_part_idx == 0 )
    	{
    		p_ref_idx1 = p_ref_idx;
    		p_ref_idx2 = p_ref_idx + (i_curr_part_num_q<<1);
    		p_ref_idx3 = p_ref_idx + (i_curr_part_num_q>>1);
    		p_ref_idx4 = p_ref_idx + (i_curr_part_num_q<<1) + (i_curr_part_num_q>>1);

    		for (loop = 0; loop < (i_curr_part_num_q>>2); loop++)
    		{
    			p_ref_idx1[loop] = i_ref_idx;
    			p_ref_idx2[loop] = i_ref_idx;
    			p_ref_idx3[loop] = i_ref_idx;
    			p_ref_idx4[loop] = i_ref_idx;
    		}
    	}
    	else
    	{
    		p_ref_idx1 = p_ref_idx;
    		p_ref_idx2 = p_ref_idx + (i_curr_part_num_q<<1);
    		for (loop = 0; loop < (i_curr_part_num_q>>2); loop++)
    		{
    			p_ref_idx1[loop] = i_ref_idx;
    			p_ref_idx2[loop] = i_ref_idx;
    		}

    		p_ref_idx1 = p_ref_idx + (i_curr_part_num_q>>1);
    		p_ref_idx2 = p_ref_idx + (i_curr_part_num_q<<1) + (i_curr_part_num_q>>1);
    		for (loop = 0; loop < ( (i_curr_part_num_q>>2) + i_curr_part_num_q ); loop++)
    		{
    			p_ref_idx1[loop] = i_ref_idx;
    			p_ref_idx2[loop] = i_ref_idx;
    		}
    	}
    	break;
    }
    case SIZE_nRx2N:
    {
    	i_curr_part_num_q = i_num_elements>>2;
    	if( i_part_idx == 0 )
    	{
    		p_ref_idx1 = p_ref_idx;
    		p_ref_idx2 = p_ref_idx + (i_curr_part_num_q<<1);
    		for (loop = 0; loop < ( (i_curr_part_num_q>>2) + i_curr_part_num_q ); loop++)
    		{
    			p_ref_idx1[loop] = i_ref_idx;
    			p_ref_idx2[loop] = i_ref_idx;
    		}

    		p_ref_idx1 = p_ref_idx + i_curr_part_num_q + (i_curr_part_num_q>>1);
    		p_ref_idx2 = p_ref_idx + i_num_elements - i_curr_part_num_q + (i_curr_part_num_q>>1);
    		for (loop = 0; loop < (i_curr_part_num_q>>2); loop++)
    		{
    			p_ref_idx1[loop] = i_ref_idx;
    			p_ref_idx2[loop] = i_ref_idx;
    		}
    	}
    	else
    	{
    		p_ref_idx1 = p_ref_idx;
    		p_ref_idx2 = p_ref_idx + (i_curr_part_num_q>>1);
    		p_ref_idx3 = p_ref_idx + (i_curr_part_num_q<<1);
    		p_ref_idx4 = p_ref_idx + (i_curr_part_num_q<<1) + (i_curr_part_num_q>>1);
    		for (loop = 0; loop < (i_curr_part_num_q>>2); loop++)
    		{
    			p_ref_idx1[loop] = i_ref_idx;
    			p_ref_idx2[loop] = i_ref_idx;
    			p_ref_idx3[loop] = i_ref_idx;
    			p_ref_idx4[loop] = i_ref_idx;
    		}
    	}
    	break;
    }
    default:
    {
    	assert(0);
    	break;
	}
	}
}


void x265_cu_mv_field_set_all_mv_field_p6(x265_cu_mv_field_t *cu_mv_field,
										x265_mv_field_t *mv_field,
										enum part_size_e i_part_size,
										int32_t i_part_addr,
										uint32_t i_depth,
										int32_t i_part_idx )
{
	x265_cu_mv_field_set_all_mv(cu_mv_field,
								x265_mv_field_get_mv(mv_field),
								i_part_size,
								i_part_addr,
								i_depth,
								i_part_idx );
	x265_cu_mv_field_set_all_ref_idx(cu_mv_field,
									x265_mv_field_get_ref_idx(mv_field),
									i_part_size,
									i_part_addr,
									i_depth,
									i_part_idx );

}


void x265_cu_mv_field_set_all_mv_field_p7(x265_cu_mv_field_t *cu_mv_field,
										x265_mv_t *mv,
										int8_t i_ref_idx,
										enum part_size_e i_part_size,
										int32_t i_part_addr,
										uint32_t i_depth,
										int32_t i_part_idx )
{
	x265_cu_mv_field_set_all_mv(cu_mv_field,
								mv,
								i_part_size,
								i_part_addr,
								i_depth,
								i_part_idx );
	x265_cu_mv_field_set_all_ref_idx(cu_mv_field,
									i_ref_idx,
									i_part_size,
									i_part_addr,
									i_depth,
									i_part_idx );

}



