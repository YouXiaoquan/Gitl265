

#include "common.h"


#include <stdarg.h>
#include <stdlib.h>


#if HAVE_MALLOC_H
#include <malloc.h>
#endif


FILE *state_file ;
int64_t i_timer_status_1 ;
int64_t i_timer_status_2 ;

int64_t i_total_time_spend ;
int64_t i_part_time_spend ;

const uint8_t chroma_scale[58] =
{
	0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11,12,13,14,15,16,
	17,18,19,20,21,22,23,24,25,26,27,28,29,29,30,31,32,
	33,33,34,34,35,35,36,36,37,37,38,39,40,41,42,43,44,
	45,46,47,48,49,50,51
} ;

static void x265_log_default( void *, int, const char *, va_list );


#ifdef SYS_WINDOWS
char *strndup ( const char *string, int i_string_length )
{
	char *copy_string = NULL ;

	copy_string = malloc (i_string_length + 1) ;
	memcpy (copy_string, string, i_string_length) ;
	copy_string[i_string_length] = '\0' ;

	return copy_string ;
}

size_t getline(char **lineptr, size_t *n, FILE *stream)
{
	char *bufptr = NULL;
	char *p = bufptr;
	size_t size;
	int c;

	if (lineptr == NULL)
	{
		return -1;
	}
	if (stream == NULL)
	{
		return -1;
	}
	if (n == NULL)
	{
		return -1;
	}
	bufptr = *lineptr;
	size = *n;

	c = fgetc(stream);
	if (c == EOF)
	{
		return -1;
	}
	if (bufptr == NULL)
	{
		bufptr = malloc(128);
		if (bufptr == NULL)
		{
			return -1;
		}
		size = 128;
	}
	p = bufptr;
	while(c != EOF)
	{
		if ((p - bufptr) > (size - 1))
		{
			size = size + 128;
			bufptr = realloc(bufptr, size);
			if (bufptr == NULL)
			{
				return -1;
			}
		}
		*p++ = c;
		if (c == '\n')
		{
			break;
		}
		c = fgetc(stream);
	}

	*p++ = '\0';
	*lineptr = bufptr;
	*n = size;

	return p - bufptr - 1;
}

#endif



void x265_init_gop_entry ( gop_entry_t * p_gop_entry )
{
    memset( (void*)p_gop_entry, 0, sizeof( gop_entry_t ) );

	p_gop_entry ->i_poc = -1 ;
	p_gop_entry ->f_qp_factor = 0 ;
	p_gop_entry ->i_slice_type = 'P' ;
}

/****************************************************************************
 * x265_param_default:
 ****************************************************************************/
void x265_param_default( x265_param_t *param )
{
	int loop ;
    /* */
    memset( (void*)param, 0, sizeof( x265_param_t ) );

    param->cpu = x265_cpu_detect();
    param->i_log_level = X265_LOG_INFO;
    param->pf_log = x265_log_default;

    param->b_annexb = 1 ;
    for ( loop = 0 ; loop < X265_MAX_TLAYER ; ++ loop )
    {
    	param->rdo.f_ad_lambda_modifier[loop] = (double) 1.0 ;
    }

    param->sps.i_profile = PROFILE_NONE ;
    param->sps.i_level_tier = LEVEL_TIER_MAIN ;
    param->sps.i_level = LEVEL_NONE ;

    param->gop.i_intra_period = -1 ;
    param->gop.i_gop_size = 1 ;
    for ( loop = 0 ; loop < X265_MAX_GOP ; ++ loop )
	{
    	x265_init_gop_entry ( param->gop.gop_list + loop ) ;
	}
    param->b_use_list_combination = 1 ;
    param->b_enable_amp = 1 ;

    param->f_qp = 30.0 ;
    param->i_qp = 30 ;
    param->delta_qp = NULL ;

    param->i_qp_adaptation_range = 6 ;

    param->sps.i_max_cu_width = 64 ;
    param->sps.i_max_cu_height = 64 ;
    param->sps.i_max_cu_depth = 4 ;

    param->sps.i_quadtree_tu_log2_max_size = 6 ;
    param->sps.i_quadtree_tu_log2_min_size = 2 ;

    param->sps.i_quadtree_tu_max_depth_intra = 1 ;
    param->sps.i_quadtree_tu_max_depth_inter = 2 ;

    param->i_input_bit_depth_y = 8 ;

    param->b_pcm_input_bit_depth_flag = 1 ;

    param->sps.b_use_sao = 1 ;
    param->sao.i_max_num_offsets_per_pic = 2048 ;
    param->sao.i_sao_lcu_based_optimization = 1 ;

    param->pcm.i_pcm_log2_max_size = 5 ;
    param->pcm.i_pcm_log2_min_size = 3 ;

    param->b_use_sbac_rd = 1 ;
    param->b_use_had_me = 1 ;

    param->rdo.b_use_rdo_q = 1 ;
    param->rdo.b_use_rdo_q_ts = 1 ;

    param->i_fast_search = 1 ;
    param->i_search_range = 96 ;
    param->i_bipred_search_range = 4 ;

    param->b_use_fast_decision_for_merge = 1 ;

    param->deblock.b_lf_cross_slice_boundary_flag = 1 ;
    param->deblock.b_lf_cross_tile_boundary_flag = 1 ;

    param->pps.i_wave_front_substreams = 1 ;

    param->pps.i_log2_parallel_merge_level = 2 ;
    param->i_max_num_merge_cand = 5 ;
    param->sps.i_tmvp_mode_id = 1 ;
    param->pps.i_sign_hide_flag = 1 ;

    param->rc.b_lcu_level_rc = 1 ;
    param->rc.b_use_lcu_separate_model = 1 ;

    param->sps.b_use_strong_intra_smoothing = 1 ;

    param->sps.vui.i_video_format = 5 ;
    param->sps.vui.i_colour_primaries = 2 ;
    param->sps.vui.i_transfer_characteristics = 2 ;
    param->sps.vui.i_matrix_coefficients = 2 ;
    param->sps.vui.i_max_bytes_per_pic_denom = 2 ;
    param->sps.vui.i_max_bits_per_min_cu_denom = 1 ;
    param->sps.vui.i_log2_max_mv_length_horizontal = 15 ;
    param->sps.vui.i_log2_max_mv_length_vertical = 15 ;
}

int x265_param_default_preset( x265_param_t *param )
{
    x265_param_default( param );

    return 0;
}

void x265_param_destroy ( x265_param_t *param )
{
	free (param->psz_dump_yuv) ;
	param->psz_dump_yuv = NULL ;
	free (param->sps.psz_cqm_file) ;
	param->sps.psz_cqm_file = NULL ;

	x265_free (param->pps.row_height) ;
	param->pps.row_height = NULL ;
	x265_free (param->pps.column_width) ;
	param->pps.column_width = NULL ;

#if X265_SIGNAL_BITRATE_PICRATE_IN_VPS
	x265_free (param->vps.bit_rate_info_present_flag) ;
	param->vps.bit_rate_info_present_flag = NULL ;
	x265_free (param->vps.pic_rate_info_present_flag) ;
	param->vps.pic_rate_info_present_flag = NULL ;
	x265_free (param->vps.avg_bit_rate) ;
	param->vps.avg_bit_rate = NULL ;
	x265_free (param->vps.max_bit_rate) ;
	param->vps.max_bit_rate = NULL ;
	x265_free (param->vps.avg_pic_rate) ;
	param->vps.avg_pic_rate = NULL ;
	x265_free (param->vps.constant_pic_rate_idc) ;
	param->vps.constant_pic_rate_idc = NULL ;
#endif
}

static int x265_get_string_from_line ( const char *line, char **pp_string, const char **pp_end )
{
	const char *start = line ;
	const char *end = NULL ;
	char *string = NULL ;
	if ( '\0' == *start || '#' == *start )
	{
		return 0 ;
	}
	start += strspn ( start, ", \t\n\r" ) ;
	if ( '\0' == *start || '#' == *start )
	{
		return 0 ;
	}
	end = strpbrk ( start, ", \t\n\r" ) ;
	string = strndup ( start, end - start ) ;
	*pp_string = string ;
	*pp_end = end ;

	return 1 ;
}
#define X265_GET_STRING_FROM_LINE() \
	if ( 0 == x265_get_string_from_line ( start, &value, &end ) ) \
	{ \
		b_error = 1 ; \
		*p_error = b_error ; \
		return ; \
	} \
	start = end ;

#define X265_FREE_AND_CHECK() \
	free ( value ) ; \
	if ( b_error ) \
	{ \
		*p_error = b_error ; \
		return ; \
	}

static int x265_atobool( const char *str, int *b_error )
{
    if( !strcmp(str, "1") ||
        !strcmp(str, "true") ||
        !strcmp(str, "yes") )
    {
        return 1;
    }
    if( !strcmp(str, "0") ||
        !strcmp(str, "false") ||
        !strcmp(str, "no") )
    {
        return 0;
    }
    *b_error = 1;
    return 0;
}

static int x265_atoi( const char *str, int *b_error )
{
    char *end;
    int v = strtol( str, &end, 0 );
    if( end == str || *end != '\0' )
        *b_error = 1;
    return v;
}

static double x265_atof( const char *str, int *b_error )
{
    char *end;
    double v = strtod( str, &end );
    if( end == str || *end != '\0' )
        *b_error = 1;
    return v;
}

#define atobool(str) ( name_was_bool = 1, x265_atobool( str, &b_error ) )
#define atoi(str) x265_atoi( str, &b_error )
#define atof(str) x265_atof( str, &b_error )

void x265_parse_gop_entry ( gop_entry_t *gop_entry, const char *line, int *p_error )
{
	const char *start = line ;
	const char *end = NULL ;
	char *value = NULL ;
    int b_error = 0;
    int loop ;

    X265_GET_STRING_FROM_LINE () ;
    gop_entry->i_slice_type = value[0] ;
    free ( value ) ;

    X265_GET_STRING_FROM_LINE () ;
    gop_entry->i_poc = atoi ( value ) ;
    X265_FREE_AND_CHECK () ;

    X265_GET_STRING_FROM_LINE () ;
    gop_entry->i_qp_Offset = atoi ( value ) ;
    X265_FREE_AND_CHECK () ;

    X265_GET_STRING_FROM_LINE () ;
    gop_entry->f_qp_factor = atof ( value ) ;
    X265_FREE_AND_CHECK () ;

    X265_GET_STRING_FROM_LINE () ;
    gop_entry->i_tc_offset_div_2 = atoi ( value ) ;
    X265_FREE_AND_CHECK () ;

    X265_GET_STRING_FROM_LINE () ;
    gop_entry->i_beta_offset_div_2 = atoi ( value ) ;
    X265_FREE_AND_CHECK () ;

    X265_GET_STRING_FROM_LINE () ;
    gop_entry->i_temporal_id = atoi ( value ) ;
    X265_FREE_AND_CHECK () ;

    X265_GET_STRING_FROM_LINE () ;
    gop_entry->i_num_ref_pics_active = atoi ( value ) ;
    X265_FREE_AND_CHECK () ;

    X265_GET_STRING_FROM_LINE () ;
    gop_entry->i_num_ref_pics = atoi ( value ) ;
    X265_FREE_AND_CHECK () ;
    for ( loop = 0 ; loop < gop_entry->i_num_ref_pics ; ++ loop )
    {
        X265_GET_STRING_FROM_LINE () ;
        gop_entry->reference_pics[loop] = atoi ( value ) ;
        X265_FREE_AND_CHECK () ;
    }

    X265_GET_STRING_FROM_LINE () ;
    gop_entry->i_inter_rps_prediction = atoi ( value ) ;
    X265_FREE_AND_CHECK () ;
#if X265_AUTO_INTER_RPS
    if ( 1 == gop_entry->i_inter_rps_prediction )
	{
        X265_GET_STRING_FROM_LINE () ;
        gop_entry->i_delta_rps = atoi ( value ) ;
        X265_FREE_AND_CHECK () ;

        X265_GET_STRING_FROM_LINE () ;
        gop_entry->i_num_ref_idc = atoi ( value ) ;
        X265_FREE_AND_CHECK () ;

        for ( loop = 0 ; loop < gop_entry->i_num_ref_idc ; ++ loop )
        {
            X265_GET_STRING_FROM_LINE () ;
            gop_entry->i_ref_idc[loop] = atoi ( value ) ;
            X265_FREE_AND_CHECK () ;
        }

	}
    else if ( 2 == gop_entry->i_inter_rps_prediction )
	{
        X265_GET_STRING_FROM_LINE () ;
        gop_entry->i_delta_rps = atoi ( value ) ;
        X265_FREE_AND_CHECK () ;
	}
#else
    if ( gop_entry->i_inter_rps_prediction )
	{
        X265_GET_STRING_FROM_LINE () ;
        gop_entry->i_delta_rps = atoi ( value ) ;
        X265_FREE_AND_CHECK () ;

        X265_GET_STRING_FROM_LINE () ;
        gop_entry->i_num_ref_idc = atoi ( value ) ;
        X265_FREE_AND_CHECK () ;

        for ( loop = 0 ; loop < gop_entry->i_num_ref_idc ; ++ loop )
        {
            X265_GET_STRING_FROM_LINE () ;
            gop_entry->i_ref_idc[loop] = atoi ( value ) ;
            X265_FREE_AND_CHECK () ;
        }

	}
#endif

}

int x265_parse_row_height ( x265_param_t *p, const char *value, int *p_error )
{
	int b_error = 0 ;
	char *line = NULL ;
    char *row_height = NULL ;
    int loop = 0 ;

	line = strdup ( value ) ;
	if ( NULL == line )
	{
		goto fail ;
	}

	if( 0 == p->pps.i_uniform_spacing_idr && p->pps.i_num_rows_minus_1 > 0 )
	{
		CHECKED_MALLOCZERO ( p->pps.row_height,
						p->pps.i_num_rows_minus_1 * sizeof (uint32_t) ) ;
	    row_height = strtok ( line, " ,-" ) ;

	    while ( NULL != line )
	    {
	    	if( loop >= p->pps.i_num_rows_minus_1 )
	    	{
	    		fprintf( stderr, "The number of rows whose height are defined is larger than the allowed number of rows.\n" );
	    		goto fail ;
	    	}
	    	p->pps.row_height[loop] = atoi( row_height ) ;
	    	row_height = strtok(NULL, " ,-") ;
	    	++ loop ;
	    }
	    if( loop < p->pps.i_num_rows_minus_1 )
	    {
	    	fprintf( stderr, "The height of some rows is not defined.\n" ) ;
	    	goto fail ;
	    }
	}
	else
	{
		p->pps.row_height = NULL;
	}

	*p_error = b_error ;
	if ( line )
	{
		free ( line ) ;
	}
	return 0 ;

fail:
	b_error = 1 ;
	*p_error = b_error ;
	if ( line )
	{
		free ( line ) ;
	}
	return -1 ;

}



int x265_parse_column_width ( x265_param_t *p, const char *value, int *p_error )
{
	int b_error = 0 ;
	char *line = NULL ;
    char *column_width = NULL ;
    int loop = 0 ;

	line = strdup ( value ) ;
	if ( NULL == line )
	{
		goto fail ;
	}

	if( 0 == p->pps.i_uniform_spacing_idr && p->pps.i_num_columns_minus_1 > 0 )
	{
		CHECKED_MALLOCZERO ( p->pps.column_width,
						p->pps.i_num_columns_minus_1 * sizeof (uint32_t) ) ;
	    column_width = strtok ( line, " ,-" ) ;

	    while ( NULL != line )
	    {
	    	if( loop >= p->pps.i_num_columns_minus_1 )
	    	{
	    		fprintf( stderr, "The number of columns whose width are defined is larger than the allowed number of columns.\n" );
	    		goto fail ;
	    	}
	    	p->pps.column_width[loop] = atoi( column_width ) ;
	    	column_width = strtok(NULL, " ,-") ;
	    	++ loop ;
	    }
	    if( loop < p->pps.i_num_columns_minus_1 )
	    {
	    	fprintf( stderr, "The width of some columns is not defined.\n" ) ;
	    	goto fail ;
	    }
	}
	else
	{
		p->pps.column_width = NULL;
	}

	*p_error = b_error ;
	if ( line )
	{
		free ( line ) ;
	}
	return 0 ;

fail:
	b_error = 1 ;
	*p_error = b_error ;
	if ( line )
	{
		free ( line ) ;
	}
	return -1 ;
}

int x265_read_bool_string ( const char *value,
							int *p_error,
							int i_num_entries,
							int32_t** pp_member_array,
							const char *element_name )
{
	int b_error = 0 ;
	char *line = NULL ;
	char *temp_array = NULL ;
    int loop = 0 ;
    int32_t *member_array = NULL ;

	line = strdup ( value ) ;
	if ( i_num_entries )
	{
		temp_array = strtok ( line, " ,-" ) ;
		CHECKED_MALLOCZERO ( member_array, i_num_entries * sizeof (int32_t) ) ;
		while( NULL != temp_array )
		{
			if( loop >= i_num_entries )
			{
				fprintf( stderr, "The number of %s defined is larger than the allowed number\n",
						element_name );
				goto fail ;
			}
			assert( ( 0 == atoi ( temp_array ) ) || ( 1 == atoi ( temp_array ) ) ) ;
			member_array[loop] = atoi(temp_array) ;
			temp_array = strtok(NULL, " ,-");
			++ loop ;
		}
		if( loop < i_num_entries )
		{
			fprintf( stderr, "Some %s are not defined\n", element_name );
			goto fail ;
		}
	}
	else
	{
		member_array = NULL;
	}

	*p_error = b_error ;
	if ( line )
	{
		free ( line ) ;
	}
	*pp_member_array = member_array ;
	return 0 ;

fail:
	b_error = 1 ;
	*p_error = b_error ;
	if ( line )
	{
		free ( line ) ;
	}
	x265_free ( member_array ) ;
	return -1 ;
}

int x265_read_int_string ( const char *value,
						int *p_error,
						int i_num_entries,
						int32_t** pp_member_array,
						const char *element_name )
{
	int b_error = 0 ;
	char *line = NULL ;
	char *temp_array = NULL ;
    int loop = 0 ;
    int32_t *member_array = NULL ;

	line = strdup ( value ) ;
	if ( i_num_entries )
	{
		temp_array = strtok ( line, " ,-" ) ;
		CHECKED_MALLOCZERO ( member_array, i_num_entries * sizeof (int32_t) ) ;
		while( NULL != temp_array )
		{
			if( loop >= i_num_entries )
			{
				fprintf( stderr, "The number of %s defined is larger than the allowed number\n",
						element_name );
				goto fail ;
			}
			assert( ( 0 == atoi ( temp_array ) ) || ( 1 == atoi ( temp_array ) ) ) ;
			member_array[loop] = atoi(temp_array) ;
			temp_array = strtok(NULL, " ,-");
			++ loop ;
		}
		if( loop < i_num_entries )
		{
			fprintf( stderr, "Some %s are not defined\n", element_name );
			goto fail ;
		}
	}
	else
	{
		member_array = NULL;
	}

	*p_error = b_error ;
	if ( line )
	{
		free ( line ) ;
	}
	*pp_member_array = member_array ;
	return 0 ;

fail:
	b_error = 1 ;
	*p_error = b_error ;
	if ( line )
	{
		free ( line ) ;
	}
	x265_free ( member_array ) ;
	return -1 ;
}

int x265_param_parse( x265_param_t *p, const char *name, const char *value )
{
    char *name_buf = NULL;
    int b_error = 0;
    int name_was_bool;
    int value_was_null = !value;
    int i;

    if( !name )
    {
        return X265_PARAM_BAD_NAME;
    }
    if( !value )
    {
        value = "true";
    }

    if( value[0] == '=' )
    {
        value++;
    }

    if( strchr( name, '_' ) ) // s/_/-/g
    {
        char *c;
        name_buf = strdup(name);
        while( (c = strchr( name_buf, '_' )) )
        {
            *c = '-';
        }
        name = name_buf;
    }

    if( !strcmp( name, "non-packed-source" ) )
	{
		p->sps.b_non_packed_constraint_flag = atobool ( value ) ;
		goto exit_func ;
	}

    if( (!strncmp( name, "no-", 3 ) && (i = 3)) ||
        (!strncmp( name, "no", 2 ) && (i = 2)) )
    {
        name += i;
        value = atobool(value) ? "false" : "true";
    }
    name_was_bool = 0;

#define OPT(STR) else if( !strcmp( name, STR ) )
#define OPT2(STR0, STR1) else if( !strcmp( name, STR0 ) || !strcmp( name, STR1 ) )

    if (0)
    {
    	;
    }
    OPT ( "dump-yuv" )
    {
    	if ( 0 == p->psz_dump_yuv )
		{
			p->psz_dump_yuv = strdup ( value ) ;
		}
    }
    OPT ( "cqmfile" )
    {
    	if ( 0 == p->sps.psz_cqm_file )
    	{
    		p->sps.psz_cqm_file = strdup ( value ) ;
    	}
    }
    OPT ( "lambda-modifier0" )
    {
    	p->rdo.f_ad_lambda_modifier[0] = atof(value) ;
    }
    OPT ( "lambda-modifier1" )
    {
    	p->rdo.f_ad_lambda_modifier[1] = atof(value) ;
    }
    OPT ( "lambda-modifier2" )
    {
    	p->rdo.f_ad_lambda_modifier[2] = atof(value) ;
    }
    OPT ( "lambda-modifier3" )
    {
    	p->rdo.f_ad_lambda_modifier[3] = atof(value) ;
    }
    OPT ( "lambda-modifier4" )
    {
    	p->rdo.f_ad_lambda_modifier[4] = atof(value) ;
    }
    OPT ( "lambda-modifier5" )
    {
    	p->rdo.f_ad_lambda_modifier[5] = atof(value) ;
    }
    OPT ( "lambda-modifier6" )
    {
    	p->rdo.f_ad_lambda_modifier[6] = atof(value) ;
    }
    OPT ( "lambda-modifier7" )
    {
    	p->rdo.f_ad_lambda_modifier[7] = atof(value) ;
    }
	OPT ( "frame-rate" )
	{
    	p->i_frame_rate = atoi(value) ;
	}
	OPT ( "frame-skip" )
	{
    	p->i_frame_skip = atoi(value) ;
	}
	OPT ( "conformance-mode" )
	{
    	p->sps.i_conformance_mode = atoi(value) ;
	}
	OPT ( "frames-to-be-encoded" )
	{
    	p->i_frame_total = atoi(value) ;
	}
	OPT ( "conf-left" )
	{
    	p->sps.i_conf_left = atoi(value) ;
	}
	OPT ( "conf-right" )
	{
    	p->sps.i_conf_right = atoi(value) ;
	}
	OPT ( "conf-top" )
	{
    	p->sps.i_conf_top = atoi(value) ;
	}
	OPT ( "conf-bottom" )
	{
    	p->sps.i_conf_bottom = atoi(value) ;
	}
	OPT ( "horizontal-padding" )
	{
    	p->i_pad[0] = atoi(value) ;
	}
	OPT ( "vertical-padding" )
	{
    	p->i_pad[1] = atoi(value) ;
	}
	OPT ( "profile" )
	{
		if ( ! strcmp ( value, "none") )
		{
			p->sps.i_profile = PROFILE_NONE ;
		}
		else if ( ! strcmp ( value, "main") )
		{
			p->sps.i_profile = PROFILE_MAIN ;
		}
		else if ( ! strcmp ( value, "main10") )
		{
			p->sps.i_profile = PROFILE_MAIN10 ;
		}
		if ( ! strcmp ( value, "main_still_picture") )
		{
			p->sps.i_profile = PROFILE_MAINSTILLPICTURE ;
		}
	}
	OPT ( "level" )
	{
		if ( ! strcmp ( value, "none") )
		{
			p->sps.i_level = LEVEL_NONE ;
		}
		else if ( ! strcmp ( value, "level1") )
		{
			p->sps.i_level = LEVEL_LEVEL1 ;
		}
		else if ( ! strcmp ( value, "level2") )
		{
			p->sps.i_level = LEVEL_LEVEL2 ;
		}
		else if ( ! strcmp ( value, "level2-1") )
		{
			p->sps.i_level = LEVEL_LEVEL2_1 ;
		}
		else if ( ! strcmp ( value, "level3") )
		{
			p->sps.i_level = LEVEL_LEVEL3 ;
		}
		else if ( ! strcmp ( value, "level3-1") )
		{
			p->sps.i_level = LEVEL_LEVEL3_1 ;
		}
		else if ( ! strcmp ( value, "level4") )
		{
			p->sps.i_level = LEVEL_LEVEL4 ;
		}
		else if ( ! strcmp ( value, "level4-1") )
		{
			p->sps.i_level = LEVEL_LEVEL4_1 ;
		}
		else if ( ! strcmp ( value, "level5") )
		{
			p->sps.i_level = LEVEL_LEVEL5 ;
		}
		else if ( ! strcmp ( value, "level5-1") )
		{
			p->sps.i_level = LEVEL_LEVEL5_1 ;
		}
		else if ( ! strcmp ( value, "level5-2") )
		{
			p->sps.i_level = LEVEL_LEVEL5_2 ;
		}
		else if ( ! strcmp ( value, "level6") )
		{
			p->sps.i_level = LEVEL_LEVEL6 ;
		}
		else if ( ! strcmp ( value, "level6-1") )
		{
			p->sps.i_level = LEVEL_LEVEL6_1 ;
		}
		else if ( ! strcmp ( value, "level6-2") )
		{
			p->sps.i_level = LEVEL_LEVEL6_2 ;
		}
	}
	OPT ( "tier" )
	{
		if ( ! strcmp ( value, "main") )
		{
			p->sps.i_level_tier = LEVEL_TIER_MAIN ;
		}
		else if ( ! strcmp ( value, "high") )
		{
			p->sps.i_level_tier = LEVEL_TIER_HIGH ;
		}
	}
#if X265_L0046_CONSTRAINT_FLAGS
	OPT ( "progressive-source" )
	{
		p->sps.b_progressive_source_flag = atobool ( value ) ;
	}
	OPT ( "interlaced-source" )
	{
		p->sps.b_interlaced_source_flag = atobool ( value ) ;
	}
	OPT ( "frame-only" )
	{
		p->sps.b_frame_only_constraint_flag = atobool ( value ) ;
	}
#endif

	OPT ( "intra-period" )
	{
		p->gop.i_intra_period = atoi ( value ) ;
	}
	OPT ( "decoding-refresh-type" )
	{
		p->gop.i_decoding_refresh_type = atoi ( value ) ;
	}
	OPT ( "gop-size" )
	{
		p->gop.i_gop_size = atoi ( value ) ;
	}
	OPT ( "frame1" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[0], value, &b_error ) ;
	}
	OPT ( "frame2" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[1], value, &b_error ) ;
	}
	OPT ( "frame3" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[2], value, &b_error ) ;
	}
	OPT ( "frame4" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[3], value, &b_error ) ;
	}
	OPT ( "frame5" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[4], value, &b_error ) ;
	}
	OPT ( "frame6" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[5], value, &b_error ) ;
	}
	OPT ( "frame7" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[6], value, &b_error ) ;
	}
	OPT ( "frame8" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[7], value, &b_error ) ;
	}
	OPT ( "frame9" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[8], value, &b_error ) ;
	}
	OPT ( "frame10" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[9], value, &b_error ) ;
	}
	OPT ( "frame11" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[10], value, &b_error ) ;
	}
	OPT ( "frame12" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[11], value, &b_error ) ;
	}
	OPT ( "frame13" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[12], value, &b_error ) ;
	}
	OPT ( "frame14" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[13], value, &b_error ) ;
	}
	OPT ( "frame15" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[14], value, &b_error ) ;
	}
	OPT ( "frame16" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[15], value, &b_error ) ;
	}
	OPT ( "frame17" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[16], value, &b_error ) ;
	}
	OPT ( "frame18" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[17], value, &b_error ) ;
	}
	OPT ( "frame19" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[18], value, &b_error ) ;
	}
	OPT ( "frame20" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[19], value, &b_error ) ;
	}
	OPT ( "frame21" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[20], value, &b_error ) ;
	}
	OPT ( "frame22" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[21], value, &b_error ) ;
	}
	OPT ( "frame23" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[22], value, &b_error ) ;
	}
	OPT ( "frame24" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[23], value, &b_error ) ;
	}
	OPT ( "frame25" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[24], value, &b_error ) ;
	}
	OPT ( "frame26" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[25], value, &b_error ) ;
	}
	OPT ( "frame27" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[26], value, &b_error ) ;
	}
	OPT ( "frame28" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[27], value, &b_error ) ;
	}
	OPT ( "frame29" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[28], value, &b_error ) ;
	}
	OPT ( "frame30" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[29], value, &b_error ) ;
	}
	OPT ( "frame31" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[30], value, &b_error ) ;
	}
	OPT ( "frame32" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[31], value, &b_error ) ;
	}
	OPT ( "frame33" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[32], value, &b_error ) ;
	}
	OPT ( "frame34" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[33], value, &b_error ) ;
	}
	OPT ( "frame35" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[34], value, &b_error ) ;
	}
	OPT ( "frame36" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[35], value, &b_error ) ;
	}
	OPT ( "frame37" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[36], value, &b_error ) ;
	}
	OPT ( "frame38" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[37], value, &b_error ) ;
	}
	OPT ( "frame39" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[38], value, &b_error ) ;
	}
	OPT ( "frame40" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[39], value, &b_error ) ;
	}
	OPT ( "frame41" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[40], value, &b_error ) ;
	}
	OPT ( "frame42" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[41], value, &b_error ) ;
	}
	OPT ( "frame43" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[42], value, &b_error ) ;
	}
	OPT ( "frame44" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[43], value, &b_error ) ;
	}
	OPT ( "frame45" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[44], value, &b_error ) ;
	}
	OPT ( "frame46" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[45], value, &b_error ) ;
	}
	OPT ( "frame47" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[46], value, &b_error ) ;
	}
	OPT ( "frame48" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[47], value, &b_error ) ;
	}
	OPT ( "frame49" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[48], value, &b_error ) ;
	}
	OPT ( "frame50" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[49], value, &b_error ) ;
	}
	OPT ( "frame51" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[50], value, &b_error ) ;
	}
	OPT ( "frame52" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[51], value, &b_error ) ;
	}
	OPT ( "frame53" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[52], value, &b_error ) ;
	}
	OPT ( "frame54" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[53], value, &b_error ) ;
	}
	OPT ( "frame55" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[54], value, &b_error ) ;
	}
	OPT ( "frame56" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[55], value, &b_error ) ;
	}
	OPT ( "frame57" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[56], value, &b_error ) ;
	}
	OPT ( "frame58" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[57], value, &b_error ) ;
	}
	OPT ( "frame59" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[58], value, &b_error ) ;
	}
	OPT ( "frame60" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[59], value, &b_error ) ;
	}
	OPT ( "frame61" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[60], value, &b_error ) ;
	}
	OPT ( "frame62" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[61], value, &b_error ) ;
	}
	OPT ( "frame63" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[62], value, &b_error ) ;
	}
	OPT ( "frame64" )
	{
		x265_parse_gop_entry ( &p->gop.gop_list[63], value, &b_error ) ;
	}
	OPT ( "list-combination" )
	{
		p->b_use_list_combination = atobool ( value ) ;
	}
	OPT ( "transform-skip" )
	{
		p->b_use_transform_skip = atobool ( value ) ;
	}
	OPT ( "transform-skip-fast" )
	{
		p->b_use_transform_skip_fast = atobool ( value ) ;
	}
	OPT ( "amp" )
	{
		p->b_enable_amp = atobool ( value ) ;
	}
	OPT ( "qp" )
	{
		p->f_qp = atof ( value ) ;
	}
	OPT ( "max-delta-qp" )
	{
		p->rdo.i_max_delta_qp = atoi ( value ) ;
	}
	OPT ( "delta-qp-rd" )
	{
		p->rdo.i_delta_qp_rd = atoi ( value ) ;
	}
	OPT ( "max-cu-dqp-depth" )
	{
		p->rdo.i_max_cu_delta_qp_depth = atoi ( value ) ;
	}
	OPT ( "cb-qp-offset" )
	{
		p->i_cb_qp_offset = atoi ( value ) ;
	}
	OPT ( "cr-qp-offset" )
	{
		p->i_cr_qp_offset = atoi ( value ) ;
	}
#if X265_ADAPTIVE_QP_SELECTION
	OPT ( "adaptive-qp-selection" )
	{
		p->b_use_adapt_qp_select = atobool ( value ) ;
	}
#endif
	OPT ( "adaptive-qp" )
	{
		p->b_use_adaptive_qp = atobool ( value ) ;
	}
	OPT ( "max-qp-adaptation-range" )
	{
		p->i_qp_adaptation_range = atoi ( value ) ;
	}
	OPT ( "max-cu-width" )
	{
		p->sps.i_max_cu_width = atoi ( value ) ;
	}
	OPT ( "max-cu-height" )
	{
		p->sps.i_max_cu_height = atoi ( value ) ;
	}
	OPT ( "max-cu-size" )
	{
		p->sps.i_max_cu_width = atoi ( value ) ;
		p->sps.i_max_cu_height = atoi ( value ) ;
	}
	OPT ( "max-partition-depth" )
	{
		p->sps.i_max_cu_depth = atoi ( value ) ;
	}
	OPT ( "quadtree-tu-log2-max-size" )
	{
		p->sps.i_quadtree_tu_log2_max_size = atoi ( value ) ;
	}
	OPT ( "quadtree-tu-log2-min-size" )
	{
		p->sps.i_quadtree_tu_log2_min_size = atoi ( value ) ;
	}
	OPT ( "quadtree-tu-max-depth-intra" )
	{
		p->sps.i_quadtree_tu_max_depth_intra = atoi ( value ) ;
	}
	OPT ( "quadtree-tu-max-depth-inter" )
	{
		p->sps.i_quadtree_tu_max_depth_inter = atoi ( value ) ;
	}
	OPT ( "input-bit-depth" )
	{
		p->i_input_bit_depth_y = atoi ( value ) ;
	}
	OPT ( "output-bit-depth" )
	{
		p->i_output_bit_depth_y = atoi ( value ) ;
	}
	OPT ( "internal-bit-depth" )
	{
		p->sps.i_bit_depth_y = atoi ( value ) ;
	}
	OPT ( "input-bit-depthc" )
	{
		p->i_input_bit_depth_c = atoi ( value ) ;
	}
	OPT ( "output-bit-depthc" )
	{
		p->i_output_bit_depth_c = atoi ( value ) ;
	}
	OPT ( "internal-bit-depthc" )
	{
		p->sps.i_bit_depth_c = atoi ( value ) ;
	}
	OPT ( "pcm-input-bit-depth-flag" )
	{
		p->b_pcm_input_bit_depth_flag = atobool ( value ) ;
	}
	OPT ( "lossless-cu-enabled" )
	{
		p->sps.b_use_lossless = atobool ( value ) ;
	}
	OPT ( "sao" )
	{
		p->sps.b_use_sao = atobool ( value ) ;
	}
	OPT ( "max-num-offsets-per-pic" )
	{
		p->sao.i_max_num_offsets_per_pic = atoi ( value ) ;
	}
	OPT ( "sao-lcu-boundary" )
	{
		p->sao.i_sao_lcu_boundary = atoi ( value ) ;
	}
	OPT ( "sao-lcu-based-optimization" )
	{
		p->sao.i_sao_lcu_based_optimization = atoi ( value ) ;
	}
	OPT ( "loop-filter-disable" )
	{
		p->sps.b_loop_filter_disable = atobool ( value ) ;
	}
	OPT ( "loop-filter-offset-in-pps" )
	{
		p->deblock.i_loop_filter_offset_in_pps = atoi ( value ) ;
	}
	OPT ( "loop-filter-beta-offset-div2" )
	{
		p->deblock.i_loop_filter_beta_offset_div_2 = atoi ( value ) ;
	}
	OPT ( "loop-filter-tc-offset-div2" )
	{
		p->deblock.i_loop_filter_tc_offset_div_2 = atoi ( value ) ;
	}
	OPT ( "deblocking-filter-control-present" )
	{
		p->deblock.i_deblocking_filter_control_present = atoi ( value ) ;
	}
	OPT ( "pcm-enabled-flag" )
	{
		p->sps.b_use_pcm = atobool ( value ) ;
	}
	OPT ( "pcm-log2-max-size" )
	{
		p->pcm.i_pcm_log2_max_size = atoi ( value ) ;
	}
	OPT ( "pcm-log2-min-size" )
	{
		p->pcm.i_pcm_log2_min_size = atoi ( value ) ;
	}
	OPT ( "pcm-filter-disable-flag" )
	{
		p->pcm.b_pcm_filter_disable_flag = atobool ( value ) ;
	}
	OPT ( "sbac-rd" )
	{
		p->b_use_sbac_rd = atobool ( value ) ;
	}
	OPT ( "asr" )
	{
		p->b_use_asr = atobool ( value ) ;
	}
	OPT ( "hadamard-me" )
	{
		p->b_use_had_me = atobool ( value ) ;
	}
	OPT ( "rdo-q" )
	{
		p->rdo.b_use_rdo_q = atobool ( value ) ;
	}
	OPT ( "rdo-q-ts" )
	{
		p->rdo.b_use_rdo_q_ts = atobool ( value ) ;
	}

#if X265_L0232_RD_PENALTY
	OPT ( "rd-penalty" )
	{
		p->rdo.i_rd_penalty = atoi ( value ) ;
	}
#endif
	OPT ( "fast-search" )
	{
		p->i_fast_search = atoi ( value ) ;
	}
	OPT ( "search-range" )
	{
		p->i_search_range = atoi ( value ) ;
	}
	OPT ( "bipred-search-range" )
	{
		p->i_bipred_search_range = atoi ( value ) ;
	}
	OPT ( "fen" )
	{
		p->b_use_fast_enc = atobool ( value ) ;
	}
	OPT ( "ecu" )
	{
		p->b_use_early_cu = atobool ( value ) ;
	}
	OPT ( "fdm" )
	{
		p->b_use_fast_decision_for_merge = atobool ( value ) ;
	}
	OPT ( "cfm" )
	{
		p->b_use_cbf_fast_mode = atobool ( value ) ;
	}
	OPT ( "esd" )
	{
		p->b_use_early_skip_detection = atobool ( value ) ;
	}
	OPT ( "slice-mode" )
	{
		p->i_slice_mode = atoi ( value ) ;
	}
	OPT ( "slice-argument" )
	{
		p->i_slice_argument = atoi ( value ) ;
	}
	OPT ( "slice-segment-mode" )
	{
		p->i_slice_segment_mode = atoi ( value ) ;
	}
	OPT ( "slice-segment-argument" )
	{
		p->i_slice_segment_argument = atoi ( value ) ;
	}
	OPT ( "lf-cross-slice-boundary-flag" )
	{
		p->deblock.b_lf_cross_slice_boundary_flag = atobool ( value ) ;
	}
	OPT ( "lf-cross-tile-boundary-flag" )
	{
		p->deblock.b_lf_cross_tile_boundary_flag = atobool ( value ) ;
	}
	OPT ( "uniform-spacing-idc" )
	{
		p->pps.i_uniform_spacing_idr = atoi ( value ) ;
	}
	OPT ( "num-tile-columns-minus1" )
	{
		p->pps.i_num_columns_minus_1 = atoi ( value ) ;
	}
	OPT ( "column-width-array" )
	{
		x265_parse_column_width ( p, value, &b_error ) ;
	}
	OPT ( "num-tile-rows-minus1" )
	{
		p->pps.i_num_rows_minus_1 = atoi ( value ) ;
	}
	OPT ( "row-height-array" )
	{
		x265_parse_row_height ( p, value, &b_error ) ;
	}
	OPT ( "wave-front-synchro" )
	{
		p->pps.i_wave_front_synchro = atoi ( value ) ;
	}
	OPT ( "constrained-intra-pred" )
	{
		p->b_use_constrained_intra_pred = atoi ( value ) ;
	}
	OPT ( "sei-decoded-picture-hash")
	{
		p->sei.i_decoded_picture_hash_sei_enabled = atoi ( value ) ;
	}
	OPT ( "sei-picture-digest" )
	{
		p->sei.i_decoded_picture_hash_sei_enabled = atoi ( value ) ;
	}
	OPT ( "sei-recovery-point" )
	{
		p->sei.i_recovery_point_sei_enabled = atoi ( value ) ;
	}
	OPT ( "sei-buffering-period" )
	{
		p->sei.i_buffering_period_sei_enabled = atoi ( value ) ;
	}
	OPT ( "sei-picture-timing" )
	{
		p->sei.i_picture_timing_sei_enabled = atoi ( value ) ;
	}
	OPT ( "sei-frame-packing" )
	{
		p->sei.i_framePacking_sei_enabled = atoi ( value ) ;
	}
	OPT ( "sei-frame-packing-type" )
	{
		p->sei.i_framePacking_sei_type = atoi ( value ) ;
	}
	OPT ( "sei-frame-packing-id" )
	{
		p->sei.i_framePacking_sei_id = atoi ( value ) ;
	}
	OPT ( "sei-frame-packing-quincunx" )
	{
		p->sei.i_framePacking_sei_quincunx = atoi ( value ) ;
	}
	OPT ( "sei-frame-packing-interpretation" )
	{
		p->sei.i_framePacking_sei_interpretation = atoi ( value ) ;
	}
	OPT ( "sei-display-orientation" )
	{
		p->sei.i_display_orientation_sei_Angle = atoi ( value ) ;
	}
	OPT ( "sei-temporal-level0-index" )
	{
		p->sei.i_temporal_level0_index_sei_enabled = atoi ( value ) ;
	}
	OPT ( "sei-gradual-decoding-refresh-info" )
	{
		p->sei.i_gradual_decoding_refresh_info_enabled = atoi ( value ) ;
	}
	OPT ( "sei-decoding-unit-info" )
	{
		p->sei.i_decoding_unit_info_sei_enabled = atoi ( value ) ;
	}
	OPT ( "weighted-pred-p" )
	{
		p->pps.b_use_weighted_pred = atobool ( value ) ;
	}
	OPT ( "weighted-pred-b" )
	{
		p->pps.b_use_weighted_bi_pred = atobool ( value ) ;
	}
	OPT ( "log2-parallel-merge-level" )
	{
		p->pps.i_log2_parallel_merge_level = atoi ( value ) ;
	}
	OPT ( "max-num-merge-cand" )
	{
		p->i_max_num_merge_cand = atoi ( value ) ;
	}
	OPT ( "tmvp-mode" )
	{
		p->sps.i_tmvp_mode_id = atoi ( value ) ;
	}
	OPT ( "sign-hide-flag" )
	{
		p->pps.i_sign_hide_flag = atoi ( value ) ;
	}
#if X265_RATE_CONTROL_LAMBDA_DOMAIN


	OPT ( "rate-control" )
	{
		p->rc.b_enable_rate_control = atobool ( value ) ;
	}
	OPT ( "target-bitrate" )
	{
		p->rc.i_target_bitrate = atoi ( value ) ;
	}
	OPT ( "keep-hierarchical-bit" )
	{
		p->rc.b_keep_hierarchical_bit = atobool ( value ) ;
	}
	OPT ( "lcu-level-rate-control" )
	{
		p->rc.b_lcu_level_rc = atobool ( value ) ;
	}
	OPT ( "rc-lcu-separate-model" )
	{
		p->rc.b_use_lcu_separate_model = atobool ( value ) ;
	}
	OPT ( "initial-qp" )
	{
		p->rc.i_initial_qp = atoi ( value ) ;
	}
	OPT ( "rc-force-intra-qp" )
	{
		p->rc.b_force_intra_qp = atobool ( value ) ;
	}
#else
	OPT ( "rate-ctrl" )
	{
		p->b_enable_rate_ctrl = atobool ( value ) ;
	}
	OPT ( "target-bitrate" )
	{
		p->i_target_bitrate = atoi ( value ) ;
	}
	OPT ( "num-lcu-in-unit" )
	{
		p->i_num_lcu_in_unit = atoi ( value ) ;
	}
#endif
	OPT ( "scaling-list" )
	{
		p->sps.i_use_scaling_list_id = atoi ( value ) ;
	}
	OPT ( "transquant-bypass-enable-flag" )
	{
		p->pps.b_transquant_bypass_enable_flag = atobool ( value ) ;
	}
	OPT ( "cu-transquant-bypass-flag-value" )
	{
		p->b_cu_transquant_bypass_flag_value = atobool ( value ) ;
	}
	OPT ( "recalculate-qp-according-to-lambda" )
	{
		p->b_recalculate_qp_according_to_lambda = atobool ( value ) ;
	}
	OPT ( "strong-intra-smoothing" )
	{
		p->sps.b_use_strong_intra_smoothing = atobool ( value ) ;
	}
	OPT ( "sei-active-parameter-sets" )
	{
		p->sei.i_active_parameter_sets_sei_enabled = atoi ( value ) ;
	}

#if X265_SIGNAL_BITRATE_PICRATE_IN_VPS
	OPT ( "bit-rate-pic-rate-max-t-layers" )
	{
		p->vps.i_bit_rate_pic_rate_max_t_layers = atoi ( value ) ;
	}
	OPT ( "bit-rate-info-present" )
	{
		x265_read_bool_string ( value, &b_error, p->vps.i_bit_rate_pic_rate_max_t_layers,
								&p->vps.bit_rate_info_present_flag,
								"bit rate info. present flag" ) ;
	}
	OPT ( "pic-rate-info-present" )
	{
		x265_read_bool_string ( value, &b_error, p->vps.i_bit_rate_pic_rate_max_t_layers,
								&p->vps.pic_rate_info_present_flag,
								"bit rate info. present flag" ) ;
	}
	OPT ( "avg-bit-rate" )
	{
		x265_read_int_string ( value, &b_error, p->vps.i_bit_rate_pic_rate_max_t_layers,
								&p->vps.avg_bit_rate,
								"avg. bit rate" ) ;
	}
	OPT ( "max-bit-rate" )
	{
		x265_read_int_string ( value, &b_error, p->vps.i_bit_rate_pic_rate_max_t_layers,
								&p->vps.max_bit_rate,
								"max. bit rate" ) ;
	}
	OPT ( "avg-pic-rate" )
	{
		x265_read_int_string ( value, &b_error, p->vps.i_bit_rate_pic_rate_max_t_layers,
								&p->vps.avg_pic_rate,
								"avg. pic rate" ) ;
	}
	OPT ( "constant-pic-rate-idc" )
	{
		x265_read_int_string ( value, &b_error, p->vps.i_bit_rate_pic_rate_max_t_layers,
								&p->vps.constant_pic_rate_idc,
								"constant pic rate Idc" ) ;
	}
#endif

	OPT ( "vui-parameters-present" )
	{
		p->sps.b_vui_parameters_present_flag = atobool ( value ) ;
	}
	OPT ( "aspect-ratio-info-present" )
	{
		p->sps.vui.b_aspec_ratio_info_present_flag = atobool ( value ) ;
	}
	OPT ( "aspect-ratio-idc" )
	{
		p->sps.vui.i_aspect_ratio_idc = atoi ( value ) ;
	}
	OPT ( "sar-width" )
	{
		p->sps.vui.i_sar_width = atoi ( value ) ;
	}
	OPT ( "sar-height" )
	{
		p->sps.vui.i_sar_height = atoi ( value ) ;
	}
	OPT ( "overscan-info-present" )
	{
		p->sps.vui.b_overscan_info_present_flag = atobool ( value ) ;
	}
	OPT ( "overscan-appropriate" )
	{
		p->sps.vui.b_overscan_appropriate_flag = atobool ( value ) ;
	}
	OPT ( "video-signal-type-present" )
	{
		p->sps.vui.b_video_signal_type_present_flag = atobool ( value ) ;
	}
	OPT ( "video-format" )
	{
		p->sps.vui.i_video_format = atoi ( value ) ;
	}
	OPT ( "video-full-range" )
	{
		p->sps.vui.b_video_full_range_flag = atobool ( value ) ;
	}
	OPT ( "colour-description-present" )
	{
		p->sps.vui.b_colour_description_present_flag = atobool ( value ) ;
	}
	OPT ( "colour-primaries" )
	{
		p->sps.vui.i_colour_primaries = atoi ( value ) ;
	}
	OPT ( "transfer-charateristics" )
	{
		p->sps.vui.i_transfer_characteristics = atoi ( value ) ;
	}
	OPT ( "matrix-coefficients" )
	{
		p->sps.vui.i_matrix_coefficients = atoi ( value ) ;
	}
	OPT ( "chroma-loc-info-present" )
	{
		p->sps.vui.b_chroma_loc_info_present_flag = atobool ( value ) ;
	}
	OPT ( "chroma-sample-loc-type-top-field" )
	{
		p->sps.vui.i_chroma_sample_loc_type_top_field = atoi ( value ) ;
	}
	OPT ( "chroma-sample-loc-type-bottom-field" )
	{
		p->sps.vui.i_chroma_sample_loc_type_bottom_field = atoi ( value ) ;
	}
	OPT ( "neutral-chroma-indication" )
	{
		p->sps.vui.b_neutral_chroma_indication_flag = atobool ( value ) ;
	}
	OPT ( "default-display-window-flag" )
	{
		p->sps.vui.b_default_display_window_flag = atobool ( value ) ;
	}
	OPT ( "def-disp-win-left-offset" )
	{
		p->sps.vui.i_def_disp_win_left_offset = atoi ( value ) ;
	}
	OPT ( "def-disp-win-right-offset" )
	{
		p->sps.vui.i_def_disp_win_right_offset = atoi ( value ) ;
	}
	OPT ( "def-disp-win-top-offset" )
	{
		p->sps.vui.i_def_disp_win_top_offset = atoi ( value ) ;
	}
	OPT ( "def-disp-win-bottom-offset" )
	{
		p->sps.vui.i_def_disp_win_bottom_offset = atoi ( value ) ;
	}
	OPT ( "frame-field-info-present-flag" )
	{
		p->sps.vui.b_frame_field_info_present_flag = atobool ( value ) ;
	}
	OPT ( "poc-proportional-to-timing-flag" )
	{
		p->sps.vui.b_poc_proportional_to_timing_flag = atobool ( value ) ;
	}
	OPT ( "num-ticks-poc-diff-one-minus1" )
	{
		p->sps.vui.i_num_ticks_poc_diff_one_minus1 = atoi ( value ) ;
	}
	OPT ( "bitstream-restriction" )
	{
		p->sps.vui.b_bitstream_restriction_flag = atobool ( value ) ;
	}
	OPT ( "tiles-fixed-structure" )
	{
		p->sps.vui.b_tiles_fixed_structure_flag = atobool ( value ) ;
	}
	OPT ( "motion-vectors-over-pic-boundaries" )
	{
		p->sps.vui.b_motion_vectors_over_pic_boundaries_flag = atobool ( value ) ;
	}
	OPT ( "max-bytes-per-pic-denom" )
	{
		p->sps.vui.i_max_bytes_per_pic_denom = atoi ( value ) ;
	}
	OPT ( "max-bits-per-min-cu-denom" )
	{
		p->sps.vui.i_max_bits_per_min_cu_denom = atoi ( value ) ;
	}
	OPT ( "log2-max-mv-length-horizontal" )
	{
		p->sps.vui.i_log2_max_mv_length_horizontal = atoi ( value ) ;
	}
	OPT ( "log2-max-mv-length-vertical" )
	{
		p->sps.vui.i_log2_max_mv_length_vertical = atoi ( value ) ;
	}
	else
    {
        return X265_PARAM_BAD_NAME;
    }
#undef OPT
#undef OPT2
#undef atobool
#undef atoi
#undef atof


exit_func:
    if( name_buf )
    {
        free( name_buf );
    }

    b_error |= value_was_null && !name_was_bool;
    return b_error ? X265_PARAM_BAD_VALUE : 0;
}

void x265_picture_init( x265_picture_t *pic )
{
    memset( pic, 0, sizeof( x265_picture_t ) );
}


void x265_log( x265_t *h, int i_level, const char *psz_fmt, ... )
{
    if( !h || i_level <= h->param.i_log_level )
    {
        va_list arg;
        va_start( arg, psz_fmt );
        if( !h )
            x265_log_default( NULL, i_level, psz_fmt, arg );
        else
            h->param.pf_log( h->param.p_log_private, i_level, psz_fmt, arg );
        va_end( arg );
    }
}

static void x265_log_default( void *p_unused, int i_level, const char *psz_fmt, va_list arg )
{
    char *psz_prefix;
    switch( i_level )
    {
        case X265_LOG_ERROR:
            psz_prefix = "error";
            break;
        case X265_LOG_WARNING:
            psz_prefix = "warning";
            break;
        case X265_LOG_INFO:
            psz_prefix = "info";
            break;
        case X265_LOG_DEBUG:
            psz_prefix = "debug";
            break;
        default:
            psz_prefix = "unknown";
            break;
    }
    fprintf( stderr, "x265 [%s]: ", psz_prefix );
    vfprintf( stderr, psz_fmt, arg );
}

void *x265_malloc( int i_size )
{
    uint8_t *align_buf = NULL;
#if SYS_MACOSX || (SYS_WINDOWS && ARCH_X86_64)
    /* Mac OS X and Win x64 always returns 16 byte aligned memory */
    align_buf = malloc( i_size );
#elif HAVE_MALLOC_H
    align_buf = memalign( 16, i_size );
#else
    uint8_t *buf = malloc( i_size + 15 + sizeof(void **) );
    if( buf )
    {
        align_buf = buf + 15 + sizeof(void **);
        align_buf -= (intptr_t) align_buf & 15;
        *( (void **) ( align_buf - sizeof(void **) ) ) = buf;
    }
#endif
    if( !align_buf )
        x265_log( NULL, X265_LOG_ERROR, "malloc of size %d failed\n", i_size );
    return align_buf;
}

/****************************************************************************
 * x265_free:
 ****************************************************************************/
void x265_free( void *p )
{
    if( p )
    {
#if HAVE_MALLOC_H || SYS_MACOSX || (SYS_WINDOWS && ARCH_X86_64)
        free( p );
#else
        free( *( ( ( void **) p ) - 1 ) );
#endif
    }
}

void print_double ( double f_value, FILE *file )
{
	int32_t loop = 0 ;
	uint8_t *memory_content = NULL ;

	fprintf ( file, "( " ) ;
	fprintf ( file, "%f ", f_value ) ;

	memory_content = (uint8_t*) (&f_value) ;
	for ( loop = 0 ; loop < sizeof (double) ; ++ loop )
	{
		fprintf ( file, "%02X ", memory_content[loop] ) ;
	}
	fprintf ( file, ")" ) ;
}

void open_state_file ()
{
	state_file = fopen ( "State2", "wb" ) ;
}

void close_state_file ()
{
	fclose (state_file) ;
}


void print_int_state (int32_t i_state)
{
	/*
	static int ww = 0 ;

	ww ++ ;
	if ( ww == 25 )
	{
		fprintf ( stderr, "Good\n" ) ;
	}
	*/




	static int ww = 0 ;
	++ ww ;
	if ( 3039 == ww )
	{
		fprintf ( stderr, "Good\n" ) ;
	}



	fprintf ( state_file, "%d \n", i_state ) ;
	fflush ( state_file ) ;
}

void print_line ()
{
	fprintf ( state_file, "\n" ) ;
	//	fflush ( state_file ) ;
}




void print_int64_state (int64_t i64_state)
{
	int32_t i_lower = 0 ;
	int32_t i_higher = 0 ;

	i_higher = (int32_t)((i64_state >> 32) & 0xFFFFFFFF) ;
	i_lower = (int32_t)(i64_state & 0xFFFFFFFF) ;

	fprintf ( state_file, "( %8X %8X )\n", i_higher, i_lower ) ;
	//fflush ( state_file ) ;

}


void print_double_state (double f_state)
{
	int32_t loop = 0 ;
	uint8_t *memory_content = NULL ;




	/*
	static int ww = 0 ;

	ww ++ ;
	if ( ww == 4159 )
	{
		fprintf ( stderr, "Good\n" ) ;
	}
	*/






	fprintf ( state_file, "( %f ", f_state ) ;

	memory_content = (uint8_t*) (&f_state) ;
	for ( loop = 0 ; loop < sizeof (double) ; ++ loop )
	{
		fprintf ( state_file, "%02X ", memory_content[loop] ) ;
	}
	fprintf ( state_file, ")\n" ) ;

	//	fflush ( state_file ) ;

}

void reset_timer_status ()
{
	i_timer_status_1 = 0 ;
	i_timer_status_2 = 0 ;

	i_total_time_spend = 0 ;
	i_part_time_spend = 0 ;
}

void print_timer_status ()
{
	double f_timer_status_1 = 0.0 ;
	double f_timer_status_2 = 0.0 ;
	double f_total_time_spend = 0.0 ;
	double f_part_time_spend = 0.0 ;
	double f_ratio = 0.0 ;

	if ( 0 != i_timer_status_2 )
	{
		f_timer_status_1 = ((double)i_timer_status_1) ;
		f_timer_status_2 = ((double)i_timer_status_2) ;
		f_ratio = f_timer_status_1 / f_timer_status_2 ;

		fprintf ( stderr, "Ratio = %f\n", f_ratio ) ;
	}

	if ( 0 != i_total_time_spend )
	{
		f_total_time_spend = ((double)i_total_time_spend) ;
		f_part_time_spend = ((double)i_part_time_spend) ;
		f_ratio = f_part_time_spend / f_total_time_spend ;
		f_ratio = f_ratio * 100.0 ;

		fprintf ( stderr, "Percent = %f \n", f_ratio ) ;
	}
}

void add_timer_status_1 ( int64_t i_timer_status )
{
	i_timer_status_1 = i_timer_status_1 + i_timer_status ;
}

void add_timer_status_2 ( int64_t i_timer_status )
{
	i_timer_status_2 = i_timer_status_2 + i_timer_status ;
}


/*
		int64_t i_start_time = 0 ;
		int64_t i_end_time = 0 ;

		i_start_time = x265_get_timer_state () ;

		i_end_time = x265_get_timer_state () ;
		add_part_time ( i_end_time - i_start_time ) ;
 */
void add_total_time ( int64_t i_total_time )
{
	i_total_time_spend = i_total_time_spend + i_total_time ;
}

void add_part_time ( int64_t i_part_time )
{
	i_part_time_spend = i_part_time_spend + i_part_time ;
}


int32_t memory_compare (pixel *src1,
						int32_t i_src_stride1,
						pixel *src2,
						int32_t i_src_stride2,
						int32_t i_width,
						int32_t i_height )
{
	int32_t loop = 0 ;

	for ( loop = 0 ; loop < i_height ; ++ loop )
	{
		if ( memcmp ( src1, src2, sizeof(pixel) * i_width ) )
		{
			/**/
			fprintf ( stderr, "Line %d Error!\n", loop ) ;
			for ( int k = 0 ; k < i_width ; ++ k )
			{
				fprintf ( stderr, "%d ", src1[k] ) ;
			}
			fprintf ( stderr, "\n" ) ;
			for ( int k = 0 ; k < i_width ; ++ k )
			{
				fprintf ( stderr, "%d ", src2[k] ) ;
			}
			fprintf ( stderr, "\n" ) ;
			return 1 ;
		}
		src1 += i_src_stride1 ;
		src2 += i_src_stride2 ;
	}

	return 0 ;
}

int32_t memory_compare_p (pixel *src1,
						int32_t i_src_stride1,
						pixel *src2,
						int32_t i_src_stride2,
						int32_t i_width,
						int32_t i_height )
{
	int32_t loop = 0 ;

	for ( loop = 0 ; loop < i_height ; ++ loop )
	{
		if ( memcmp ( src1, src2, sizeof(pixel) * i_width ) )
		{
			/**/
			fprintf ( stderr, "Line %d Error!\n", loop ) ;
			for ( int k = 0 ; k < i_width ; ++ k )
			{
				fprintf ( stderr, "%d ", src1[k] ) ;
			}
			fprintf ( stderr, "\n" ) ;
			for ( int k = 0 ; k < i_width ; ++ k )
			{
				fprintf ( stderr, "%d ", src2[k] ) ;
			}
			fprintf ( stderr, "\n" ) ;
			return 1 ;
		}
		src1 += i_src_stride1 ;
		src2 += i_src_stride2 ;
	}

	return 0 ;
}

int32_t memory_compare_s (spixel *src1,
						int32_t i_src_stride1,
						spixel *src2,
						int32_t i_src_stride2,
						int32_t i_width,
						int32_t i_height )
{
	int32_t loop = 0 ;

	for ( loop = 0 ; loop < i_height ; ++ loop )
	{
		if ( memcmp ( src1, src2, sizeof(spixel) * i_width ) )
		{
			/**/
			fprintf ( stderr, "Line %d Error!\n", loop ) ;
			for ( int k = 0 ; k < i_width ; ++ k )
			{
				fprintf ( stderr, "%d ", src1[k] ) ;
			}
			fprintf ( stderr, "\n" ) ;
			for ( int k = 0 ; k < i_width ; ++ k )
			{
				fprintf ( stderr, "%d ", src2[k] ) ;
			}
			fprintf ( stderr, "\n" ) ;
			return 1 ;
		}
		src1 += i_src_stride1 ;
		src2 += i_src_stride2 ;
	}

	return 0 ;
}

int32_t memory_compare_int16 (int16_t *src1,
							int32_t i_src_stride1,
							int16_t *src2,
							int32_t i_src_stride2,
							int32_t i_width,
							int32_t i_height )
{
	int32_t loop = 0 ;

	for ( loop = 0 ; loop < i_height ; ++ loop )
	{
		if ( memcmp ( src1, src2, sizeof(int16_t) * i_width ) )
		{
			/**/
			fprintf ( stderr, "Error Line:\t%d\n", loop ) ;
			for ( int k = 0 ; k < i_width ; ++ k )
			{
				fprintf ( stderr, "%d ", src1[k] ) ;
			}
			fprintf ( stderr, "\n" ) ;
			for ( int k = 0 ; k < i_width ; ++ k )
			{
				fprintf ( stderr, "%d ", src2[k] ) ;
			}
			fprintf ( stderr, "\n" ) ;
			return 1 ;
		}
		src1 += i_src_stride1 ;
		src2 += i_src_stride2 ;
	}

	return 0 ;
}

int32_t memory_compare_int32 (int32_t *src1,
							int32_t i_src_stride1,
							int32_t *src2,
							int32_t i_src_stride2,
							int32_t i_width,
							int32_t i_height )
{
	int32_t loop = 0 ;

	for ( loop = 0 ; loop < i_height ; ++ loop )
	{
		if ( memcmp ( src1, src2, sizeof(int32_t) * i_width ) )
		{
			/**/
			fprintf ( stderr, "Error Line: %d\n", loop ) ;
			for ( int k = 0 ; k < i_width ; ++ k )
			{
				fprintf ( stderr, "%d ", src1[k] ) ;
			}
			fprintf ( stderr, "\n" ) ;
			for ( int k = 0 ; k < i_width ; ++ k )
			{
				fprintf ( stderr, "%d ", src2[k] ) ;
			}
			fprintf ( stderr, "\n" ) ;
			return 1 ;
		}
		src1 += i_src_stride1 ;
		src2 += i_src_stride2 ;
	}

	return 0 ;
}



spixel x265_clip_y( x265_t *h, spixel v )
{
    return ( (v < 0) ? 0 : (v > ((1<<h->param.sps.i_bit_depth_y)-1)) ? ((1<<h->param.sps.i_bit_depth_y)-1) : v );
}

spixel x265_clip_c( x265_t *h, spixel v )
{
    return ( (v < 0) ? 0 : (v > ((1<<h->param.sps.i_bit_depth_c)-1)) ? ((1<<h->param.sps.i_bit_depth_c)-1) : v );
}

