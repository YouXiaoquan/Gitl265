

#include "common/common.h"


static const int32_t quant_ts_default4x4[16] =
{
	16,16,16,16,
	16,16,16,16,
	16,16,16,16,
	16,16,16,16
} ;

static const int32_t quant_intra_default8x8[64] =
{
	16,16,16,16,17,18,21,24,
	16,16,16,16,17,19,22,25,
	16,16,17,18,20,22,25,29,
	16,16,18,21,24,27,31,36,
	17,17,20,24,30,35,41,47,
	18,19,22,27,35,44,54,65,
	21,22,25,31,41,54,70,88,
	24,25,29,36,47,65,88,115
} ;

static const int32_t quant_inter_default8x8[64] =
{
	16,16,16,16,17,18,20,24,
	16,16,16,17,18,20,24,25,
	16,16,17,18,20,24,25,28,
	16,17,18,20,24,25,28,33,
	17,18,20,24,25,28,33,41,
	18,20,24,25,28,33,41,54,
	20,24,25,28,33,41,54,71,
	24,25,28,33,41,54,71,91
} ;

static const uint32_t scaling_list_size   [4] = { 16, 64, 256, 1024 } ;
static const uint32_t scaling_list_size_x [4] = { 4, 8, 16, 32 } ;
static const uint32_t scaling_list_num [ SCALING_LIST_SIZE_NUM ] = { 6, 6, 6, 2} ;


static const char matrix_type [4][6][20] =
{
	{
		"INTRA4X4_LUMA",
		"INTRA4X4_CHROMAU",
		"INTRA4X4_CHROMAV",
		"INTER4X4_LUMA",
		"INTER4X4_CHROMAU",
		"INTER4X4_CHROMAV"
	},
	{
		"INTRA8X8_LUMA",
		"INTRA8X8_CHROMAU",
		"INTRA8X8_CHROMAV",
		"INTER8X8_LUMA",
		"INTER8X8_CHROMAU",
		"INTER8X8_CHROMAV"
	},
	{
		"INTRA16X16_LUMA",
		"INTRA16X16_CHROMAU",
		"INTRA16X16_CHROMAV",
		"INTER16X16_LUMA",
		"INTER16X16_CHROMAU",
		"INTER16X16_CHROMAV"
	},
	{
		"INTRA32X32_LUMA",
		"INTER32X32_LUMA",
	},
} ;

static const char matrix_type_dc [4][12][22] =
{
	{
	},
	{
	},
	{
		"INTRA16X16_LUMA_DC",
		"INTRA16X16_CHROMAU_DC",
		"INTRA16X16_CHROMAV_DC",
		"INTER16X16_LUMA_DC",
		"INTER16X16_CHROMAU_DC",
		"INTER16X16_CHROMAV_DC"
	},
	{
		"INTRA32X32_LUMA_DC",
		"INTER32X32_LUMA_DC",
	},
};

void init_reference_picture_set ( x265_reference_picture_set_t *reference_picture_set )
{
	memset ( reference_picture_set, 0, sizeof(x265_reference_picture_set_t) ) ;
}


void print_rps ( x265_reference_picture_set_t * rps, FILE *file )
{
	int32_t loop = 0 ;

	fprintf ( file, "%d ", rps->i_number_of_pictures ) ;
	fprintf ( file, "%d ", rps->i_number_of_negative_pictures ) ;
	fprintf ( file, "%d ", rps->i_number_of_positive_pictures ) ;
	fprintf ( file, "%d ", rps->i_number_of_longterm_pictures ) ;
	fprintf ( file, "%d ", rps->b_inter_rps_prediction ) ;
	fprintf ( file, "%d ", rps->i_delta_ridx_minus1 ) ;
	fprintf ( file, "%d ", rps->i_delta_rps ) ;
	fprintf ( file, "%d ", rps->i_num_ref_idc ) ;
	fprintf ( file, "\n" ) ;

	fprintf ( file, "DeltaPOC: " ) ;
	for ( loop = 0 ; loop < X265_MAX_NUM_REF_PICS ; ++ loop )
	{
		fprintf ( file, "%d ", rps->delta_poc[loop] ) ;
	}
	fprintf ( file, "\n" ) ;

	fprintf ( file, "POC: " ) ;
	for ( loop = 0 ; loop < X265_MAX_NUM_REF_PICS ; ++ loop )
	{
		fprintf ( file, "%d ", rps->poc[loop] ) ;
	}
	fprintf ( file, "\n" ) ;

	fprintf ( file, "Used: " ) ;
	for ( loop = 0 ; loop < X265_MAX_NUM_REF_PICS ; ++ loop )
	{
		fprintf ( file, "%d ", (int)rps->used[loop] ) ;
	}
	fprintf ( file, "\n" ) ;

	fprintf ( file, "RefIdc: " ) ;
	for ( loop = 0 ; loop < X265_MAX_NUM_REF_PICS + 1; ++ loop )
	{
		fprintf ( file, "%d ", rps->ref_idc[loop] ) ;
	}
	fprintf ( file, "\n" ) ;

	fprintf ( file, "CheckLTMSBPresent: " ) ;
	for ( loop = 0 ; loop < X265_MAX_NUM_REF_PICS ; ++ loop )
	{
		fprintf ( file, "%d ", (int)rps->check_lt_msb[loop] ) ;
	}
	fprintf ( file, "\n" ) ;

	fprintf ( file, "POCLSBLT: " ) ;
	for ( loop = 0 ; loop < X265_MAX_NUM_REF_PICS ; ++ loop )
	{
		fprintf ( file, "%d ", rps->poc_lsb_lt[loop] ) ;
	}
	fprintf ( file, "\n" ) ;

	fprintf ( file, "DeltaPocMSBCycleLT: " ) ;
	for ( loop = 0 ; loop < X265_MAX_NUM_REF_PICS ; ++ loop )
	{
		fprintf ( file, "%d ", rps->delta_poc_msb_cycle_lt[loop] ) ;
	}
	fprintf ( file, "\n" ) ;

	fprintf ( file, "DeltaPocMSBPresentFlag: " ) ;
	for ( loop = 0 ; loop < X265_MAX_NUM_REF_PICS ; ++ loop )
	{
		fprintf ( file, "%d ", (int)rps->delta_poc_msb_present_flag[loop] ) ;
	}
	fprintf ( file, "\n" ) ;

}

void sort_delta_poc ( x265_reference_picture_set_t *reference_picture_set )
{
	int i = 0, j = 0 ;
	int32_t i_delta_poc = 0 ;
	int32_t b_used = 0 ;
	int32_t i_temp = 0 ;
	int32_t num_neg_pics = 0 ;

	// sort in increasing order (smallest first)
	for( i = 1 ; i < reference_picture_set->i_number_of_pictures ; ++ i )
	{
		i_delta_poc = reference_picture_set->delta_poc[i] ;
		b_used = reference_picture_set->used[i] ;
		for ( j = i - 1 ; j >= 0 ; -- j )
		{
			i_temp = reference_picture_set->delta_poc[j] ;
			if ( i_delta_poc < i_temp )
			{
				reference_picture_set->delta_poc[j+1] = i_temp ;
				reference_picture_set->used[j+1] = reference_picture_set->used[j] ;
				reference_picture_set->delta_poc[j] = i_delta_poc ;
				reference_picture_set->used[j] = b_used ;
			}
		}
	}

	// flip the negative values to largest first
	num_neg_pics = reference_picture_set->i_number_of_negative_pictures ;
	for( i = 0, j = num_neg_pics - 1 ; i < num_neg_pics >> 1 ; ++ i, --j  )
	{
		i_delta_poc = reference_picture_set->delta_poc[i];
		b_used = reference_picture_set->used[i];
		reference_picture_set->delta_poc[i] = reference_picture_set->delta_poc[j] ;
		reference_picture_set->used[i] = reference_picture_set->used[j] ;
		reference_picture_set->delta_poc[j] = i_delta_poc ;
		reference_picture_set->used[j] = b_used ;
	}
}

/** Prints the deltaPOC and RefIdc (if available) values in the RPS.
 *  A "*" is added to the deltaPOC value if it is Used bu current.
 * \returns Void
 */
void print_delta_poc ( x265_reference_picture_set_t *reference_picture_set )
{
	int loop = 0 ;
	printf("DeltaPOC = { ");
	for( loop = 0 ; loop < reference_picture_set->i_number_of_pictures ; ++ loop )
	{
		printf("%d%s ", reference_picture_set->delta_poc[loop],
				(reference_picture_set->used[loop] == 1) ? "*" : "" ) ;
	}
	if ( reference_picture_set->b_inter_rps_prediction )
	{
		printf("}, RefIdc = { ");
		for( loop = 0 ; loop < reference_picture_set->i_num_ref_idc ; ++ loop )
		{
			printf("%d ", reference_picture_set->ref_idc[loop] ) ;
		}
	}
	printf("}\n");
}

void destroy_rps_list ( x265_rps_list_t *rps_list )
{
	x265_free ( rps_list->reference_picture_sets );
	rps_list->reference_picture_sets = NULL ;
	rps_list->i_number_of_reference_picture_sets = 0 ;
}


int create_rps_list ( x265_rps_list_t *rps_list, int i_number_of_reference_picture_sets )
{
	int loop = 0 ;

	rps_list->i_number_of_reference_picture_sets = i_number_of_reference_picture_sets ;
	CHECKED_MALLOCZERO ( rps_list->reference_picture_sets,
						rps_list->i_number_of_reference_picture_sets * sizeof(x265_reference_picture_set_t) );

	for ( loop = 0 ; loop < rps_list->i_number_of_reference_picture_sets ; ++ loop )
	{
		init_reference_picture_set ( &rps_list->reference_picture_sets[loop] ) ;
	}
	return 0 ;

fail:
	destroy_rps_list ( rps_list ) ;
	return -1 ;
}


void init_rps_list ( x265_rps_list_t *rps_list )
{
	memset ( rps_list, 0, sizeof(x265_rps_list_t) ) ;
}

void deinit_rps_list ( x265_rps_list_t *rps_list )
{
	destroy_rps_list ( rps_list ) ;
}

void destroy_scaling_list ( x265_scaling_list_t *scaling_list )
{
	int i_size_id = 0 ;
	int i_list_id = 0 ;
	for( i_size_id = 0 ; i_size_id < SCALING_LIST_SIZE_NUM ; ++ i_size_id )
	{
		for( i_list_id = 0; i_list_id < scaling_list_num[i_size_id] ; ++ i_list_id )
		{
			x265_free ( scaling_list->scaling_list_coef[i_size_id][i_list_id] ) ;
		}
	}
}

int create_scaling_list ( x265_scaling_list_t *scaling_list )
{
	int i_size_id = 0 ;
	int i_list_id = 0 ;
	for( i_size_id = 0 ; i_size_id < SCALING_LIST_SIZE_NUM ; ++ i_size_id )
	{
		for( i_list_id = 0; i_list_id < scaling_list_num[i_size_id] ; ++ i_list_id )
		{
			CHECKED_MALLOCZERO ( scaling_list->scaling_list_coef[i_size_id][i_list_id],
					X265_MIN ( X265_MAX_MATRIX_COEF_NUM, scaling_list_size[i_size_id] )
					* sizeof ( int32_t ) ) ;
		}
	}
	// copy address for 32x32
	scaling_list->scaling_list_coef[SCALING_LIST_32x32][3] =
			scaling_list->scaling_list_coef[SCALING_LIST_32x32][1];
	return 0 ;

fail:
	destroy_scaling_list ( scaling_list ) ;
	return -1 ;
}

int init_scaling_list ( x265_scaling_list_t *scaling_list )
{
	memset ( scaling_list, 0, sizeof(x265_scaling_list_t) ) ;
	if ( create_scaling_list (scaling_list) )
	{
		return -1 ;
	}
	return 0 ;
}

void deinit_scaling_list ( x265_scaling_list_t *scaling_list )
{
	destroy_scaling_list (scaling_list) ;
	memset ( scaling_list, 0, sizeof(x265_scaling_list_t) ) ;
}

const int32_t *get_scaling_list_default_address ( uint32_t i_size_id, uint32_t i_list_id )
{
	const int32_t *src = 0;
	switch(i_size_id)
	{
	case SCALING_LIST_4x4:
		src = quant_ts_default4x4 ;
		break;
	case SCALING_LIST_8x8:
		src = (i_list_id<3) ? quant_intra_default8x8 : quant_inter_default8x8;
		break;
	case SCALING_LIST_16x16:
		src = (i_list_id<3) ? quant_intra_default8x8 : quant_inter_default8x8;
		break;
	case SCALING_LIST_32x32:
		src = (i_list_id<1) ? quant_intra_default8x8 : quant_inter_default8x8;
		break;
	default:
		assert (0) ;
		src = NULL;
	break;
	}
	return src;
}

void process_ref_matrix( x265_scaling_list_t *scaling_list,
						uint32_t i_size_id,
						uint32_t i_list_id,
						uint32_t i_ref_list_id )
{
	memcpy ( scaling_list->scaling_list_coef[i_size_id][i_list_id],
			((i_list_id == i_ref_list_id) ?
			get_scaling_list_default_address(i_size_id, i_ref_list_id)
			: scaling_list->scaling_list_coef[i_size_id][i_ref_list_id]),
			  sizeof(int32_t) * X265_MIN(X265_MAX_MATRIX_COEF_NUM, scaling_list_size[i_size_id]));
}

int parse_scaling_list ( x265_scaling_list_t *scaling_list, const char *filename)
{
	FILE *fh;
	char line[1024];
	uint32_t i_size_id = 0 ;
	uint32_t i_list_id = 0 ;

	int loop = 0 ;
	int32_t i_size = 0 ;
	int32_t *src = 0 ;
	int32_t i_data ;
	char *ret;
	int32_t i_return_value ;

	if((fh = fopen(filename,"r")) == NULL)
	{
		printf("can't open file %s :: set Default Matrix\n", filename);
		return -1 ;
	}

	for(i_size_id = 0; i_size_id < SCALING_LIST_SIZE_NUM; i_size_id++)
	{
		i_size = X265_MIN ( X265_MAX_MATRIX_COEF_NUM, scaling_list_size[i_size_id] ) ;
		for( i_list_id = 0; i_list_id < scaling_list_num[i_size_id] ; ++ i_list_id )
		{
			src = scaling_list->scaling_list_coef[i_size_id][i_list_id] ;

			fseek(fh,0,0);
			do
			{
				ret = fgets(line, 1024, fh) ;
				if ( (ret == NULL)
					|| (strstr(line, matrix_type[i_size_id][i_list_id]) == NULL && feof(fh)))
				{
					printf("Error: can't read Matrix :: set Default Matrix\n") ;
					return -1 ;
				}
			}
			while (strstr(line, matrix_type[i_size_id][i_list_id]) == NULL) ;

			for ( loop = 0 ; loop < i_size ; ++ loop )
			{
				i_return_value = fscanf ( fh, "%d,", &i_data ) ;
				if (i_return_value !=1 )
				{
					printf("Error: can't read Matrix :: set Default Matrix\n");
					return -1 ;
				}
				src[loop] = i_data;
			}
			//set DC value for default matrix check
			scaling_list->scaling_list_dc[i_size_id][i_list_id] = src[0] ;

			if ( i_size_id > SCALING_LIST_8x8 )
			{
				fseek(fh,0,0);
				do
				{
					ret = fgets(line, 1024, fh);
					if ( (ret == NULL)
						||(strstr(line, matrix_type_dc[i_size_id][i_list_id]) == NULL && feof(fh)))
					{
						printf("Error: can't read DC :: set Default Matrix\n");
						return -1;
					}
				}
				while (strstr(line, matrix_type_dc[i_size_id][i_list_id]) == NULL);
				i_return_value = fscanf(fh, "%d,", &i_data);
				if (i_return_value != 1)
				{
					printf("Error: can't read Matrix :: set Default Matrix\n");
					return -1 ;
				}
				//overwrite DC value when size of matrix is larger than 16x16
				scaling_list->scaling_list_dc[i_size_id][i_list_id] = i_data ;
			}
		}
	}
	fclose(fh);
	return 0;
}

void process_default_marix ( x265_scaling_list_t *scaling_list,
							uint32_t i_size_id,
							uint32_t i_list_id)
{
	memcpy(scaling_list->scaling_list_coef[i_size_id][i_list_id],
			get_scaling_list_default_address(i_size_id,i_list_id),
			sizeof(int32_t) * X265_MIN(X265_MAX_MATRIX_COEF_NUM, scaling_list_size[i_size_id]));
	scaling_list->scaling_list_dc[i_size_id][i_list_id] = SCALING_LIST_DC ;
}


int check_default_scaling_list ( x265_scaling_list_t *scaling_list )
{
	uint32_t i_default_counter = 0;
	uint32_t i_size_id = 0 ;
	uint32_t i_list_id = 0 ;

	for(i_size_id = 0; i_size_id < SCALING_LIST_SIZE_NUM; i_size_id++)
	{
		for( i_list_id = 0; i_list_id < scaling_list_num[i_size_id] ; ++ i_list_id )
		{
			if( ! memcmp ( scaling_list->scaling_list_coef[i_size_id][i_list_id],
							get_scaling_list_default_address ( i_size_id, i_list_id ),
							sizeof(int32_t) * X265_MIN(X265_MAX_MATRIX_COEF_NUM, scaling_list_size[i_size_id]))
				&& ((i_size_id < SCALING_LIST_16x16)
						// check DC value
					|| (scaling_list->scaling_list_dc[i_size_id][i_list_id] == 16))
					)
			{
				i_default_counter++;
			}
		}
	}
	// -4 for 32x32
	return (i_default_counter == (SCALING_LIST_NUM * SCALING_LIST_SIZE_NUM - 4)) ? 0 : 1 ;
}

/** check DC value of matrix for default matrix signaling
 */
void check_dc_of_matrix ( x265_scaling_list_t *scaling_list )
{
	uint32_t i_size_id = 0 ;
	uint32_t i_list_id = 0 ;
	for ( i_size_id = 0; i_size_id < SCALING_LIST_SIZE_NUM ; ++ i_size_id )
	{
		for ( i_list_id = 0; i_list_id < scaling_list_num[i_size_id]; ++ i_list_id )
		{
			//check default matrix?
			if ( 0 == scaling_list->scaling_list_dc[i_size_id][i_list_id] )
			{
				process_default_marix ( scaling_list, i_size_id, i_list_id ) ;
			}
		}
	}
}

int check_pred_mode ( x265_scaling_list_t *scaling_list,
					uint32_t i_size_id, uint32_t i_list_id )
{
	int32_t i_pred_list_id = 0 ;
	for( i_pred_list_id = (int32_t)i_list_id ; i_pred_list_id >= 0 ; -- i_pred_list_id )
	{
		// check value of matrix
		if( !memcmp( scaling_list->scaling_list_coef[i_size_id][i_list_id],
					((i_list_id == i_pred_list_id) ?
					get_scaling_list_default_address (i_size_id, i_pred_list_id) :
					scaling_list->scaling_list_coef[i_size_id][i_pred_list_id]),
					sizeof(int32_t) * X265_MIN(X265_MAX_MATRIX_COEF_NUM, scaling_list_size[i_size_id])
					)
		&& ((i_size_id < SCALING_LIST_16x16)
		// check DC value
			|| (scaling_list->scaling_list_dc[i_size_id][i_list_id]
			== scaling_list->scaling_list_dc[i_size_id][i_pred_list_id])))
		{
			scaling_list->ref_matrix_id[i_size_id][i_list_id] = i_pred_list_id ;
			return 0 ;
		}
	}
	return 1;
}

int x265_scaling_list_get_coef_num ( uint32_t i_size_id )
{
	return X265_MIN(X265_MAX_MATRIX_COEF_NUM, scaling_list_size[i_size_id]) ;
}

int x265_scaling_list_get_scaling_list_num ( uint32_t i_size_id )
{
	return scaling_list_num[i_size_id] ;
}

int x265_scaling_list_get_scaling_list_size ( uint32_t i_size_id )
{
	return scaling_list_size[i_size_id] ;
}

int x265_scaling_list_get_scaling_list_size_x ( uint32_t i_size_id )
{
	return scaling_list_size_x[i_size_id] ;
}

void init_profile_tier_level ( x265_profile_tier_level_t *profile_tier_level )
{
	memset ( profile_tier_level, 0, sizeof(x265_profile_tier_level_t) ) ;
}

void init_ptl ( x265_ptl_t *ptl )
{
	int loop = 0 ;

	memset ( ptl, 0, sizeof(x265_ptl_t) ) ;
	init_profile_tier_level ( &ptl->p_general_ptl ) ;
	for ( loop = 0 ; loop < 6 ; ++ loop )
	{
		init_profile_tier_level ( &ptl->sub_layer_ptl[loop] ) ;
	}
}

#if X265_SIGNAL_BITRATE_PICRATE_IN_VPS

void init_bit_rate_pic_rate_info ( x265_bit_rate_pic_rate_info_t *bit_rate_pic_rate_info )
{
	memset ( bit_rate_pic_rate_info, 0, sizeof(x265_bit_rate_pic_rate_info_t) ) ;
}

#endif

void init_hrd_sub_layer_info ( x265_hrd_sub_layer_info_t *hrd_sub_layer_info )
{
	memset ( hrd_sub_layer_info, 0, sizeof(x265_hrd_sub_layer_info_t) ) ;
}

void init_hrd ( x265_hrd_t *hrd )
{
	int loop = 0 ;
	memset ( hrd, 0, sizeof(x265_hrd_t) ) ;
#if !X265_L0043_TIMING_INFO
	hrd->i_num_units_in_tick = 1001 ;
	hrd->i_time_scale = 60000 ;
#endif

	for ( loop = 0 ; loop < X265_MAX_TLAYER ; ++ loop )
	{
		init_hrd_sub_layer_info ( &hrd->hrd[loop] ) ;
	}
}


void initiming_info ( x265_timing_info_t *timing_info )
{
	memset ( timing_info, 0, sizeof(x265_timing_info_t) ) ;
	timing_info->i_num_units_in_tick = 1001 ;
	timing_info->i_time_scale = 60000 ;
}


void destroy_hrd_param_buffer( x265_vps_t *vps )
{
	x265_free ( vps->hrd_parameters ) ;
	x265_free ( vps->hrd_op_set_idx ) ;
	x265_free ( vps->cprms_present_flag ) ;
}

int create_hrd_param_buffer( x265_vps_t *vps )
{
	int loop = 0 ;

	CHECKED_MALLOCZERO ( vps->hrd_parameters,
						vps->i_num_hrd_parameters * sizeof (x265_hrd_t) ) ;
	CHECKED_MALLOCZERO ( vps->hrd_op_set_idx,
						vps->i_num_hrd_parameters * sizeof (uint32_t) ) ;
	CHECKED_MALLOCZERO ( vps->cprms_present_flag,
						vps->i_num_hrd_parameters * sizeof (int32_t) ) ;
	for ( loop = 0 ; loop < vps->i_num_hrd_parameters ; ++ loop )
	{
		init_hrd ( & vps->hrd_parameters[loop] ) ;
	}

	return 0 ;

fail:
	destroy_hrd_param_buffer ( vps ) ;
	return -1 ;
}


void init_vps ( x265_vps_t *vps )
{
	memset ( vps, 0, sizeof(x265_vps_t) ) ;

	init_ptl ( & vps->ptl );
#if X265_SIGNAL_BITRATE_PICRATE_IN_VPS
	init_bit_rate_pic_rate_info ( &vps->b_bit_rate_pic_rate_info ) ;
#endif

#if X265_L0043_TIMING_INFO
	initiming_info ( & vps->timing_info );
#endif

	vps->i_max_t_layers = 1 ;
	vps->i_max_layers = 1 ;
}

void deinit_vps ( x265_vps_t *vps )
{
	destroy_hrd_param_buffer ( vps ) ;
}

void init_window ( x265_window_t *window )
{
	memset ( window, 0, sizeof(x265_window_t) ) ;
}

void init_ref_pic_list_modification ( x265_ref_pic_list_modification_t *ref_pic_list_modification )
{
	memset ( ref_pic_list_modification, 0, sizeof(x265_ref_pic_list_modification_t) ) ;
}

void print_ref_pic_list_modification ( x265_ref_pic_list_modification_t *ref_pic_list_modification,
										FILE *file )
{
	int32_t loop = 0 ;

	fprintf ( file, "%d ", ref_pic_list_modification->b_ref_pic_list_modification_flag_l0 ) ;
	fprintf ( file, "%d ", ref_pic_list_modification->b_ref_pic_list_modification_flag_l1 ) ;
	fprintf ( file, "\n" ) ;

	for ( loop = 0 ; loop < 32 ; ++ loop )
	{
		fprintf ( file, "%d ", ref_pic_list_modification->ref_pic_set_idx_l0[loop] ) ;
	}
	fprintf ( file, "\n" ) ;
	for ( loop = 0 ; loop < 32 ; ++ loop )
	{
		fprintf ( file, "%d ", ref_pic_list_modification->ref_pic_set_idx_l1[loop] ) ;
	}
	fprintf ( file, "\n" ) ;

}



void set_window ( x265_window_t *window,
				int32_t i_offset_left,
				int32_t i_offset_right,
				int32_t i_offset_top,
				int32_t i_offset_bottom )
{
	window->b_enabled_flag = 1 ;
	window->i_win_left_offset = i_offset_left ;
	window->i_win_right_offset = i_offset_right ;
	window->i_win_top_offset = i_offset_top ;
	window->i_win_bottom_offset = i_offset_bottom ;
}

void init_vui ( x265_vui_t *vui )
{
	memset ( vui, 0, sizeof(x265_vui_t) ) ;

	vui->i_video_format = 5 ;
	vui->i_colour_primaries = 2 ;
	vui->i_transfer_characteristics = 2 ;
	vui->i_matrix_coefficients = 2 ;
	vui->b_motion_vectors_over_pic_boundaries_flag = 1 ;
	vui->b_restricted_ref_pic_lists_flag = 1 ;
	vui->i_max_bytes_per_pic_denom = 2 ;
	vui->i_max_bits_per_min_cu_denom = 1 ;
	vui->i_log2_max_mv_length_horizontal = 15 ;
	vui->i_log2_max_mv_length_vertical = 15 ;

	init_hrd ( &vui->hrd_parameters ) ;
	init_window ( &vui->default_display_window ) ;
#if X265_L0043_TIMING_INFO
	initiming_info ( & vui->timing_info );
#endif

}

void destroy_sps ( x265_sps_t *sps )
{
	deinit_rps_list ( &sps->rps_list ) ;
}

int init_sps ( x265_sps_t *sps )
{
	memset ( sps, 0, sizeof(x265_sps_t) ) ;

	init_window ( &sps->conformance_window ) ;
	init_vui ( &sps->vui_parameters ) ;
	init_ptl ( &sps->ptl ) ;
	init_rps_list ( &sps->rps_list ) ;

	sps->i_chroma_format_idc = CHROMA_420 ;
	sps->i_max_t_layers = 1 ;
	sps->i_pic_width_in_luma_samples = 352 ;
	sps->i_pic_height_in_luma_samples = 288 ;
	sps->i_max_cu_width = 32 ;
	sps->i_max_cu_height = 32 ;
	sps->i_max_cu_depth = 3 ;
	sps->i_max_tr_depth = 1 ;
	sps->i_pcm_log2_max_size = 5 ;
	sps->i_pcm_log2_min_size = 7 ;

	sps->i_bit_depth_y = 8 ;
	sps->i_bit_depth_c = 8 ;
	sps->i_pcm_bit_depth_luma = 8 ;
	sps->i_pcm_bit_depth_chroma = 8 ;
	sps->i_bits_for_poc = 8 ;

	sps->i_max_tr_size = 32 ;

	return 0 ;

}

void deinit_sps ( x265_sps_t *sps )
{
	destroy_sps ( sps ) ;
}

void sps_create_rps_list ( x265_sps_t *sps, int i_num_rps )
{
	destroy_rps_list ( &sps->rps_list ) ;
	create_rps_list ( &sps->rps_list, i_num_rps ) ;
}

void set_hrd_parameters( x265_sps_t *sps,
						uint32_t i_frame_rate,
						uint32_t i_num_du,
						uint32_t i_bit_rate,
						int32_t b_random_access )
{
	x265_vui_t *vui = NULL ;
	x265_hrd_t *hrd = NULL ;
	x265_timing_info_t *timing_info = NULL ;
	int32_t b_rate_cnt = 0 ;
	int32_t i = 0, j = 0 ;
	uint32_t i_birate_value = 0, i_cpb_size_value = 0;
	uint32_t i_du_cpb_size_value = 0;
#if X265_L0363_DU_BIT_RATE
	uint32_t i_du_bit_rate_value = 0;
#endif


	if( ! sps->b_vui_parameters_present_flag )
	{
		return;
	}

	vui = & sps->vui_parameters ;
	hrd = & vui->hrd_parameters ;

#if X265_L0043_TIMING_INFO
	timing_info = & vui->timing_info ;
	timing_info->b_timing_info_present_flag = 1 ;
	switch( i_frame_rate )
	{
	case 24:
		timing_info->i_num_units_in_tick = 1125000 ;
		timing_info->i_time_scale = 27000000 ;
		break;
	case 25:
		timing_info->i_num_units_in_tick = 1080000 ;
		timing_info->i_time_scale = 27000000 ;
		break;
	case 30:
		timing_info->i_num_units_in_tick = 900900 ;
		timing_info->i_time_scale = 27000000 ;
		break;
	case 50:
		timing_info->i_num_units_in_tick = 540000 ;
		timing_info->i_time_scale = 27000000 ;
		break;
	case 60:
		timing_info->i_num_units_in_tick = 450450 ;
		timing_info->i_time_scale = 27000000 ;
		break;
	default:
		timing_info->i_num_units_in_tick = 1001 ;
		timing_info->i_time_scale = 60000 ;
		break;
	}
#else
	hrd->b_timing_info_present_flag = 1 ;
	switch( i_frame_rate )
	{
	case 24:
		hrd->i_num_units_in_tick = 1125000 ;
		hrd->i_time_scale = 27000000 ;
		break;
	case 25:
		hrd->i_num_units_in_tick = 1080000 ;
		hrd->i_time_scale = 27000000 ;
		break;
	case 30:
		hrd->i_num_units_in_tick = 900900 ;
		hrd->i_time_scale = 27000000 ;
		break;
	case 50:
		hrd->i_num_units_in_tick = 540000 ;
		hrd->i_time_scale = 27000000 ;
		break;
	case 60:
		hrd->i_num_units_in_tick = 450450 ;
		hrd->i_time_scale = 27000000 ;
		break;
	default:
		hrd->i_num_units_in_tick = 1001 ;
		hrd->i_time_scale = 60000 ;
		break;
	}
#endif

	b_rate_cnt = ( i_bit_rate > 0 );
	hrd->b_nal_hrd_parameters_present_flag = b_rate_cnt ;
	hrd->b_vcl_hrd_parameters_present_flag = b_rate_cnt;

	hrd->b_sub_pic_cpb_params_present_flag = ( i_num_du > 1 ) ;

	if( hrd->b_sub_pic_cpb_params_present_flag )
	{
		hrd->i_tick_divisor_minus2 = 100 - 2 ;
		// 8-bit precision ( plus 1 for last DU in AU )
		hrd->i_du_cpb_removal_delay_length_minus1 = 7 ;
		hrd->b_sub_pic_cpb_params_in_pic_timing_sei_flag = 1 ;
#if X265_L0044_DU_DPB_OUTPUT_DELAY_HRD
		// With sub-clock tick factor of 100, at least 7 bits to have the same value as AU dpb delay
		hrd->i_dpb_output_delay_du_length_minus1 = 5 + 7 ;
#endif
	}
	else
	{
		hrd->b_sub_pic_cpb_params_in_pic_timing_sei_flag= 0 ;
	}

	// in units of 2~( 6 + 4 ) = 1,024 bps
	hrd->i_bit_rate_scale = 4 ;
	// in units of 2~( 4 + 4 ) = 1,024 bit
	hrd->i_cpb_size_scale = 6 ;
	// in units of 2~( 4 + 4 ) = 1,024 bit
	hrd->i_du_cpb_size_scale = 6 ;

    // assuming 0.5 sec, log2( 90,000 * 0.5 ) = 16-bit
	hrd->i_initial_cpb_removal_delay_length_minus1 = 15 ;
	if( b_random_access )
	{
        // 32 = 2^5 (plus 1)
		hrd->i_cpb_removal_delay_length_minus1 = 5;
        // 32 + 3 = 2^6
		hrd->i_dpb_output_delay_length_minus1 = 5 ;
	}
	else
	{
        // max. 2^10
		hrd->i_cpb_removal_delay_length_minus1 = 9 ;
        // max. 2^10
		hrd->i_dpb_output_delay_length_minus1 = 9;
	}

/*
   Note: only the case of "vps_max_temporal_layers_minus1 = 0" is supported.
*/
	for( i = 0; i < X265_MAX_TLAYER; i ++ )
	{
		hrd->hrd[i].b_fixed_pic_rate_flag = 1 ;
		hrd->hrd[i].i_pic_duration_in_tc_minus1 = 0 ;
		hrd->hrd[i].b_low_delay_hrd_flag = 0 ;
		hrd->hrd[i].i_cpb_cnt_minus1 = 0 ;

		i_birate_value  = i_bit_rate;
		// 1 second
		i_cpb_size_value = i_bit_rate;
		i_du_cpb_size_value = i_bit_rate / i_num_du ;
#if X265_L0363_DU_BIT_RATE
		i_du_bit_rate_value = i_bit_rate;
#endif
		for( j = 0; j < ( hrd->hrd[i].i_cpb_cnt_minus1 + 1 ); j ++ )
		{
			hrd->hrd[i].bit_rate_value_minus1[j][0] = i_birate_value  - 1 ;
			hrd->hrd[i].cpb_size_value[j][0] = i_cpb_size_value - 1 ;
			hrd->hrd[i].du_cpb_size_value[j][0] = i_du_cpb_size_value - 1 ;
			hrd->hrd[i].cbr_flag[j][0] = (j == 0) ? 1 : 0 ;

			hrd->hrd[i].bit_rate_value_minus1[j][1] = i_birate_value  - 1 ;
			hrd->hrd[i].cpb_size_value[j][1] = i_cpb_size_value - 1 ;
			hrd->hrd[i].du_cpb_size_value[j][1] = i_du_cpb_size_value - 1 ;
#if X265_L0363_DU_BIT_RATE
			hrd->hrd[i].du_bit_rate_value[j][1] = i_du_bit_rate_value - 1 ;
#endif
			hrd->hrd[i].cbr_flag[j][1] = (j == 0) ? 1 : 0 ;
		}
	}
}



void destroy_pps ( x265_pps_t *pps )
{
	x265_free ( pps->row_height ) ;
	x265_free ( pps->column_width ) ;
}


int init_pps ( x265_pps_t *pps )
{
	memset ( pps, 0, sizeof(x265_pps_t) ) ;

	pps->i_num_ref_idx_l0_default_active = 1 ;
	pps->i_num_ref_idx_l1_default_active = 1 ;
	pps->b_loop_filter_across_tiles_enabled_flag = 1 ;
	pps->i_num_substreams = 1 ;
	pps->i_enc_cabac_table_idx = I_SLICE ;

	return 0 ;
}


void deinit_pps ( x265_pps_t *pps )
{
	destroy_pps ( pps ) ;
}


int set_column_width ( x265_pps_t *pps, uint32_t* column_width )
{
	int32_t loop = 0 ;
	if( pps->i_uniform_spacing_flag == 0 && pps->i_num_columns_minus1 > 0 )
	{
		CHECKED_MALLOCZERO ( pps->column_width,
							pps->i_num_columns_minus1 * sizeof(uint32_t) );

		for( loop = 0 ; loop < pps->i_num_columns_minus1 ; ++ loop )
		{
			pps->column_width[loop] = column_width[loop];
		}
	}

	return 0 ;

fail:
	x265_free ( pps->column_width ) ;
	return -1 ;

}

int set_row_height ( x265_pps_t *pps, uint32_t* row_height )
{
	int32_t loop = 0 ;
	if( pps->i_uniform_spacing_flag == 0 && pps->i_num_rows_minus1 > 0 )
    {
		CHECKED_MALLOCZERO ( pps->row_height,
							pps->i_num_rows_minus1 * sizeof(uint32_t) );

		for( loop = 0 ; loop < pps->i_num_rows_minus1 ; ++ loop )
		{
			pps->row_height[loop] = row_height[loop];
		}
    }

	return 0 ;

fail:
	x265_free ( pps->row_height ) ;
	return -1 ;
}


