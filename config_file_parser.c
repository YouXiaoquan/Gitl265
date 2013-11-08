

#include "common/common.h"

static int x265_file_log_level;
static const char *x265_file_opt_name[] =
{
	"input-file",
	"bitstrean-file",
	"recon-file",
	"cqmfile",
	"source-width",
	"source-height",
	"lambda-modifier0",
	"lambda-modifier1",
	"lambda-modifier2",
	"lambda-modifier3",
	"lambda-modifier4",
	"lambda-modifier5",
	"lambda-modifier6",
	"lambda-modifier7",
	"frame-rate",
	"frame-skip",
	"conformance-mode",
	"frames-to-be-encoded",
	"conf-left",
	"conf-right",
	"conf-top",
	"conf-bottom",
	"horizontal-padding",
	"vertical-padding",
	"profile",
	"level",
	"tier",
#if X265_L0046_CONSTRAINT_FLAGS
	"progressive-source",
	"interlaced-source",
	"non-packed-source",
	"frame-only",
#endif
	"intra-period",
	"decoding-refresh-type",
	"gop-size",
	"frame1",
	"frame2",
	"frame3",
	"frame4",
	"frame5",
	"frame6",
	"frame7",
	"frame8",
	"frame9",
	"frame10",
	"frame11",
	"frame12",
	"frame13",
	"frame14",
	"frame15",
	"frame16",
	"frame17",
	"frame18",
	"frame19",
	"frame20",
	"frame21",
	"frame22",
	"frame23",
	"frame24",
	"frame25",
	"frame26",
	"frame27",
	"frame28",
	"frame29",
	"frame30",
	"frame31",
	"frame32",
	"frame33",
	"frame34",
	"frame35",
	"frame36",
	"frame37",
	"frame38",
	"frame39",
	"frame40",
	"frame41",
	"frame42",
	"frame43",
	"frame44",
	"frame45",
	"frame46",
	"frame47",
	"frame48",
	"frame49",
	"frame50",
	"frame51",
	"frame52",
	"frame53",
	"frame54",
	"frame55",
	"frame56",
	"frame57",
	"frame58",
	"frame59",
	"frame60",
	"frame61",
	"frame62",
	"frame63",
	"frame64",
	"list-combination",
	"transform-skip",
	"transform-skip-fast",
	"amp",
	"qp",
	"max-delta-qp",
	"delta-qp-rd",
	"max-cu-dqp-depth",
	"cb-qp-offset",
	"cr-qp-offset",
#if X265_ADAPTIVE_QP_SELECTION
	"adaptive-qp-selection",
#endif
	"adaptive-qp",
	"max-qp-adaptation-range",
	"max-cu-width",
	"max-cu-height",
	"max-cu-size",
	"max-partition-depth",
	"quadtree-tu-log2-max-size",
	"quadtree-tu-log2-min-size",
	"quadtree-tu-max-depth-intra",
	"quadtree-tu-max-depth-inter",
	"input-bit-depth",
	"output-bit-depth",
	"internal-bit-depth",
	"input-bit-depthc",
	"output-bit-depthc",
	"internal-bit-depthc",
	"pcm-input-bit-depth-flag",
	"lossless-cu-enabled",
	"sao",
	"max-num-offsets-per-pic",
	"sao-lcu-boundary",
	"sao-lcu-based-optimization",
	"loop-filter-disable",
	"loop-filter-offset-in-pps",
	"loop-filter-beta-offset-div2",
	"loop-filter-tc-offset-div2",
	"deblocking-filter-control-present",
	"pcm-enabled-flag",
	"pcm-log2-max-size",
	"pcm-log2-min-size",
	"pcm-filter-disable-flag",
	"sbac-rd",
	"asr",
	"hadamard-me",
	"rdo-q",
	"rdo-q-ts",

#if X265_L0232_RD_PENALTY
	"rd-penalty",
#endif
	"fast-search",
	"search-range",
	"bipred-search-range",
	"fen",
	"ecu",
	"fdm",
	"cfm",
	"esd",
	"slice-mode",
	"slice-argument",
	"slice-segment-mode",
	"slice-segment-argument",
	"lf-cross-slice-boundary-flag",
	"lf-cross-tile-boundary-flag",
	"uniform-spacing-idc",
	"num-tile-columns-minus1",
	"column-width-array",
	"num-tile-rows-minus1",
	"row-height-array",
	"wave-front-synchro",
	"constrained-intra-pred",
	"sei-decoded-picture-hash",
	"sei-picture-digest",
	"sei-recovery-point",
	"sei-buffering-period",
	"sei-picture-timing",
	"sei-frame-packing",
	"sei-frame-packing-type",
	"sei-frame-packing-id",
	"sei-frame-packing-quincunx",
	"sei-frame-packing-interpretation",
	"sei-display-orientation",
	"sei-temporal-level0-index",
	"sei-gradual-decoding-refresh-info",
	"sei-decoding-unit-info",
	"weighted-pred-p",
	"weighted-pred-b",
	"log2-parallel-merge-level",
	"max-num-merge-cand",
	"tmvp-mode",
	"sign-hide-flag",
#if X265_RATE_CONTROL_LAMBDA_DOMAIN
	"rate-control",
	"target-bitrate",
	"keep-hierarchical-bit",
	"lcu-level-rate-control",
	"rc-lcu-separate-model",
	"initial-qp",
	"rc-force-intra-qp",
#else
	"rate-ctrl",
	"target-bitrate",
	"num-lcu-in-unit",
#endif
	"scaling-list",
	"transquant-bypass-enable-flag",
	"cu-transquant-bypass-flag-value",
	"recalculate-qp-according-to-lambda",
	"strong-intra-smoothing",
	"sei-active-parameter-sets",

#if X265_SIGNAL_BITRATE_PICRATE_IN_VPS
	"bit-rate-pic-rate-max-t-layers",
	"bit-rate-info-present",
	"pic-rate-info-present",
	"avg-bit-rate",
	"max-bit-rate",
	"avg-pic-rate",
	"constant-pic-rate-idc",
#endif

	"vui-parameters-present",
	"aspect-ratio-info-present",
	"aspect-ratio-idc",
	"sar-width",
	"sar-height",
	"overscan-info-present",
	"overscan-appropriate",
	"video-signal-type-present",
	"video-format",
	"video-full-range",
	"colour-description-present",
	"colour-primaries",
	"transfer-charateristics",
	"matrix-coefficients",
	"chroma-loc-info-present",
	"chroma-sample-loc-type-top-field",
	"chroma-sample-loc-type-bottom-field",
	"neutral-chroma-indication",
	"default-display-window-flag",
	"def-disp-win-left-offset",
	"def-disp-win-right-offset",
	"def-disp-win-top-offset",
	"def-disp-win-bottom-offset",
	"frame-field-info-present-flag",
	"poc-proportional-to-timing-flag",
	"num-ticks-poc-diff-one-minus1",
	"bitstream-restriction",
	"tiles-fixed-structure",
	"motion-vectors-over-pic-boundaries",
	"max-bytes-per-pic-denom",
	"max-bits-per-min-cu-denom",
	"log2-max-mv-length-horizontal",
	"log2-max-mv-length-vertical",
};

int x265_file_param_parse( x265_param_t *, int *, const char *name, const char *value );



void x265_file_log( const char *name, int i_level, const char *fmt, ... )
{
    if( i_level > x265_file_log_level )
        return;
    char *s_level;
    switch( i_level )
    {
        case X265_LOG_ERROR:
            s_level = "error";
            break;
        case X265_LOG_WARNING:
            s_level = "warning";
            break;
        case X265_LOG_INFO:
            s_level = "info";
            break;
        case X265_LOG_DEBUG:
            s_level = "debug";
            break;
        default:
            s_level = "unknown";
            break;
    }
    fprintf( stderr, "%s [%s]: ", name, s_level );
    va_list arg;
    va_start( arg, fmt );
    vfprintf( stderr, fmt, arg );
    va_end( arg );
}

int x265_get_opt_name_and_value ( const char *line, char **pp_name, char **pp_value )
{
	const char *start = line ;
	const char *option_end = NULL ;
	const char *value_end = NULL ;
	char *name = NULL ;
	char *value = NULL ;

	*pp_name = NULL ;
	*pp_value = NULL ;
	start += strspn ( start, " \t\n\r" ) ;
	if ( ('\0' == *start) || ('#' == *start) )
	{
		return 0 ;
	}
	option_end = strpbrk ( start, ": \t\n\r" ) ;
	name = strndup ( start, option_end - start ) ;
	*pp_name = name ;

	start = option_end + strspn ( option_end, " \t\n\r" ) ;
	if ( ':' != *start )
	{
		free ( name ) ;
		*pp_name = NULL ;
		return 0 ;
	}
	start = start + 1 ;
	start += strspn ( start, " \t\n\r" ) ;
	if ( '\0' == *start )
	{
		free ( name ) ;
		*pp_name = NULL ;
		return 0 ;
	}
	value_end = start ;
	do
	{
		if ( '#' == *value_end )
		{
			-- value_end ;
			break ;
		}
		value_end = strpbrk ( value_end, " \t\n\r" ) ;
		value_end += strspn ( value_end, " \t\n\r" ) ;
	}
	while ( '\0' != *value_end ) ;

	-- value_end ;
	while ( ( *value_end == ' ')
			|| ( *value_end == '\t' )
			|| ( *value_end == '\n' )
			|| ( *value_end == '\r' ) )
	{
		-- value_end ;
	}
	if ( value_end >= start )
	{
		value = strndup ( start, value_end + 1 - start ) ;
		*pp_value = value ;
	}
	else
	{
		free ( name ) ;
		*pp_name = NULL ;
		return 0 ;
	}

	return 1 ;
}

int x265_config_file_parse( x265_param_t *p, const char *config_file_name )
{
	int name_ndex = 0 ;
	char *name = NULL ;
	char *value = NULL ;
	char *line = NULL ;
    int b_error = 0;
    FILE *file = NULL ;
    size_t n = 512 ;

    x265_file_log_level = p->i_log_level ;
    CHECKED_MALLOC ( line, sizeof(char) *n ) ;
    file = fopen ( config_file_name, "r" ) ;
    while ( -1 != getline ( &line, &n, file ) )
    {
        if ( x265_get_opt_name_and_value ( line, &name, &value ) )
        {
        	b_error = x265_file_param_parse ( p, &name_ndex, name, value ) ;
        	if ( X265_PARAM_BAD_VALUE == b_error )
        	{
    			const char *opt_name = x265_file_opt_name[name_ndex];
    			x265_file_log( "x265", X265_LOG_ERROR, "invalid argument: %s = %s\n", opt_name, value );
        		fprintf ( stderr, "X265_PARAM_BAD_VALUE\n") ;
            	free ( name ) ;
            	free ( value ) ;
                free ( line ) ;
                fclose ( file ) ;
                return -1;
        	}
        	else if ( X265_PARAM_BAD_NAME == b_error )
        	{
        		fprintf ( stderr, "X265_PARAM_BAD_NAME:\t%s\n", name ) ;
            	free ( name ) ;
            	free ( value ) ;
                free ( line ) ;
                fclose ( file ) ;
                return b_error;
        	}
        	free ( name ) ;
        	free ( value ) ;
        }
    }
    fclose ( file ) ;

    x265_free ( line ) ;
    return b_error;

fail:
	x265_free ( line ) ;
	return -1 ;
}

static int x265_file_get_string_from_line ( const char *line, char **pp_string, const char **pp_end )
{
	const char *start = line ;
	const char *end = NULL ;
	char *string = NULL ;
	if ( '\0' == *start || '#' == *start )
	{
		return 0 ;
	}
	start += strspn ( start, " \t\n\r" ) ;
	if ( '\0' == *start || '#' == *start )
	{
		return 0 ;
	}
	end = strpbrk ( start, " \t\n\r" ) ;
	if ( 0 == end )
	{
		end = start ;
		while ( 0 != end[0] )
		{
			end ++ ;
		}
	}
	string = strndup ( start, end - start ) ;
	*pp_string = string ;
	*pp_end = end ;

	return 1 ;
}
#define X265_FILE_GET_STRING_FROM_LINE() \
	if ( 0 == x265_file_get_string_from_line ( start, &value, &end ) ) \
	{ \
		b_error = 1 ; \
		*p_error = b_error ; \
		return ; \
	} \
	start = end ;

#define X265_FILE_FREE_AND_CHECK() \
	free ( value ) ; \
	if ( b_error ) \
	{ \
		*p_error = b_error ; \
		return ; \
	}


static int x265_file_atobool( const char *str, int *b_error )
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

static int x265_file_atoi( const char *str, int *b_error )
{
    char *end;
    int v = strtol( str, &end, 0 );
    if( end == str || *end != '\0' )
        *b_error = 1;
    return v;
}

static double x265_file_atof( const char *str, int *b_error )
{
    char *end;
    double v = strtod( str, &end );
    if( end == str || *end != '\0' )
        *b_error = 1;
    return v;
}

#define atobool(str) ( name_was_bool = 1, x265_file_atobool( str, &b_error ) )
#define atoi(str) x265_file_atoi( str, &b_error )
#define atof(str) x265_file_atof( str, &b_error )

void x265_file_parse_gop_entry ( gop_entry_t *gop_entry, const char *line, int *p_error )
{
	const char *start = line ;
	const char *end = NULL ;
	char *value = NULL ;
    int b_error = 0;
    int loop ;

    X265_FILE_GET_STRING_FROM_LINE () ;
    gop_entry->i_slice_type = value[0] ;
    free ( value ) ;

    X265_FILE_GET_STRING_FROM_LINE () ;
    gop_entry->i_poc = atoi ( value ) ;
    X265_FILE_FREE_AND_CHECK () ;

    X265_FILE_GET_STRING_FROM_LINE () ;
    gop_entry->i_qp_Offset = atoi ( value ) ;
    X265_FILE_FREE_AND_CHECK () ;

    X265_FILE_GET_STRING_FROM_LINE () ;
    gop_entry->f_qp_factor = atof ( value ) ;
    X265_FILE_FREE_AND_CHECK () ;

    X265_FILE_GET_STRING_FROM_LINE () ;
    gop_entry->i_tc_offset_div_2 = atoi ( value ) ;
    X265_FILE_FREE_AND_CHECK () ;

    X265_FILE_GET_STRING_FROM_LINE () ;
    gop_entry->i_beta_offset_div_2 = atoi ( value ) ;
    X265_FILE_FREE_AND_CHECK () ;

    X265_FILE_GET_STRING_FROM_LINE () ;
    gop_entry->i_temporal_id = atoi ( value ) ;
    X265_FILE_FREE_AND_CHECK () ;

    X265_FILE_GET_STRING_FROM_LINE () ;
    gop_entry->i_num_ref_pics_active = atoi ( value ) ;
    X265_FILE_FREE_AND_CHECK () ;

    X265_FILE_GET_STRING_FROM_LINE () ;
    gop_entry->i_num_ref_pics = atoi ( value ) ;
    X265_FILE_FREE_AND_CHECK () ;
    for ( loop = 0 ; loop < gop_entry->i_num_ref_pics ; ++ loop )
    {
        X265_FILE_GET_STRING_FROM_LINE () ;
        gop_entry->reference_pics[loop] = atoi ( value ) ;
        X265_FILE_FREE_AND_CHECK () ;
    }

    X265_FILE_GET_STRING_FROM_LINE () ;
    gop_entry->i_inter_rps_prediction = atoi ( value ) ;
    X265_FILE_FREE_AND_CHECK () ;
#if X265_AUTO_INTER_RPS
    if ( 1 == gop_entry->i_inter_rps_prediction )
	{
        X265_FILE_GET_STRING_FROM_LINE () ;
        gop_entry->i_delta_rps = atoi ( value ) ;
        X265_FILE_FREE_AND_CHECK () ;

        X265_FILE_GET_STRING_FROM_LINE () ;
        gop_entry->i_num_ref_idc = atoi ( value ) ;
        X265_FILE_FREE_AND_CHECK () ;

        for ( loop = 0 ; loop < gop_entry->i_num_ref_idc ; ++ loop )
        {
            X265_FILE_GET_STRING_FROM_LINE () ;
            gop_entry->i_ref_idc[loop] = atoi ( value ) ;
            X265_FILE_FREE_AND_CHECK () ;
        }

	}
    else if ( 2 == gop_entry->i_inter_rps_prediction )
	{
        X265_FILE_GET_STRING_FROM_LINE () ;
        gop_entry->i_delta_rps = atoi ( value ) ;
        X265_FILE_FREE_AND_CHECK () ;
	}
#else
    if ( gop_entry->i_inter_rps_prediction )
	{
        X265_FILE_GET_STRING_FROM_LINE () ;
        gop_entry->i_delta_rps = atoi ( value ) ;
        X265_FILE_FREE_AND_CHECK () ;

        X265_FILE_GET_STRING_FROM_LINE () ;
        gop_entry->i_num_ref_idc = atoi ( value ) ;
        X265_FILE_FREE_AND_CHECK () ;

        for ( loop = 0 ; loop < gop_entry->i_num_ref_idc ; ++ loop )
        {
            X265_FILE_GET_STRING_FROM_LINE () ;
            gop_entry->i_ref_idc[loop] = atoi ( value ) ;
            X265_FILE_FREE_AND_CHECK () ;
        }

	}
#endif

}

int x265_file_parse_row_height ( x265_param_t *p, const char *value, int *p_error )
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



int x265_file_parse_column_width ( x265_param_t *p, const char *value, int *p_error )
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

int x265_file_read_bool_string ( const char *value,
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

int x265_file_read_int_string ( const char *value,
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


int x265_file_param_parse( x265_param_t *p, int *name_index, const char *name, const char *value )
{
    char *name_buf = NULL;
    int b_error = 0;
    int name_was_bool;
    int value_was_null = !value;
    int i;
    int i_index = -1 ;

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

    if( (!strncmp( name, "no-", 3 ) && (i = 3)) ||
        (!strncmp( name, "no", 2 ) && (i = 2)) )
    {
        name += i;
        value = atobool(value) ? "false" : "true";
    }
    name_was_bool = 0;

#define OPT(STR) else if( i_index ++, !strcmp( name, STR ) )
#define OPT2(STR0, STR1) else if( i_index ++, !strcmp( name, STR0 ) || !strcmp( name, STR1 ) )

//    printf ( "%s\n", name ) ;
    if (0)
    {
    }
    OPT ( "InputFile" )
    {

    }
    OPT ( "BitstreamFile" )
    {

    }
    OPT ( "ReconFile" )
    {
    	if ( 0 == p->psz_dump_yuv )
    	{
    		p->psz_dump_yuv = strdup ( value ) ;
    	}
    }
    OPT ( "ScalingListFile" )
    {
    	if ( 0 == p->sps.psz_cqm_file )
    	{
    		p->sps.psz_cqm_file = strdup ( value ) ;
    	}
    }
    OPT ( "SourceWidth" )
    {

    }
    OPT ( "SourceHeight" )
    {

    }
    OPT ( "LambdaModifier0" )
    {
    	p->rdo.f_ad_lambda_modifier[0] = atof(value) ;
    }
    OPT ( "LambdaModifier1" )
    {
    	p->rdo.f_ad_lambda_modifier[1] = atof(value) ;
    }
    OPT ( "LambdaModifier2" )
    {
    	p->rdo.f_ad_lambda_modifier[2] = atof(value) ;
    }
    OPT ( "LambdaModifier3" )
    {
    	p->rdo.f_ad_lambda_modifier[3] = atof(value) ;
    }
    OPT ( "LambdaModifier4" )
    {
    	p->rdo.f_ad_lambda_modifier[4] = atof(value) ;
    }
    OPT ( "LambdaModifier5" )
    {
    	p->rdo.f_ad_lambda_modifier[5] = atof(value) ;
    }
    OPT ( "LambdaModifier6" )
    {
    	p->rdo.f_ad_lambda_modifier[6] = atof(value) ;
    }
    OPT ( "LambdaModifier7" )
    {
    	p->rdo.f_ad_lambda_modifier[7] = atof(value) ;
    }
	OPT ( "FrameRate" )
	{
    	p->i_frame_rate = atoi(value) ;
	}
	OPT ( "FrameSkip" )
	{
    	p->i_frame_skip = atoi(value) ;
	}
	OPT ( "ConformanceMode" )
	{
    	p->sps.i_conformance_mode = atoi(value) ;
	}
	OPT ( "FramesToBeEncoded" )
	{
    	p->i_frame_total = atoi(value) ;
	}
	OPT ( "ConfLeft" )
	{
    	p->sps.i_conf_left = atoi(value) ;
	}
	OPT ( "ConfRight" )
	{
    	p->sps.i_conf_right = atoi(value) ;
	}
	OPT ( "ConfTop" )
	{
    	p->sps.i_conf_top = atoi(value) ;
	}
	OPT ( "ConfBottom" )
	{
    	p->sps.i_conf_bottom = atoi(value) ;
	}
	OPT ( "HorizontalPadding" )
	{
    	p->i_pad[0] = atoi(value) ;
	}
	OPT ( "VerticalPadding" )
	{
    	p->i_pad[1] = atoi(value) ;
	}
	OPT ( "Profile" )
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
	OPT ( "Level" )
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
	OPT ( "Tier" )
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
	OPT ( "ProgressiveSource" )
	{
		p->sps.b_progressive_source_flag = atobool ( value ) ;
	}
	OPT ( "InterlacedSource" )
	{
		p->sps.b_interlaced_source_flag = atobool ( value ) ;
	}
	OPT ( "NonPackedSource" )
	{
		p->sps.b_non_packed_constraint_flag = atobool ( value ) ;
	}
	OPT ( "FrameOnly" )
	{
		p->sps.b_frame_only_constraint_flag = atobool ( value ) ;
	}
#endif

	OPT ( "IntraPeriod" )
	{
		p->gop.i_intra_period = atoi ( value ) ;
	}
	OPT ( "DecodingRefreshType" )
	{
		p->gop.i_decoding_refresh_type = atoi ( value ) ;
	}
	OPT ( "GOPSize" )
	{
		p->gop.i_gop_size = atoi ( value ) ;
	}
	OPT ( "Frame1" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[0], value, &b_error ) ;
	}
	OPT ( "Frame2" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[1], value, &b_error ) ;
	}
	OPT ( "Frame3" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[2], value, &b_error ) ;
	}
	OPT ( "Frame4" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[3], value, &b_error ) ;
	}
	OPT ( "Frame5" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[4], value, &b_error ) ;
	}
	OPT ( "Frame6" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[5], value, &b_error ) ;
	}
	OPT ( "Frame7" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[6], value, &b_error ) ;
	}
	OPT ( "Frame8" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[7], value, &b_error ) ;
	}
	OPT ( "Frame9" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[8], value, &b_error ) ;
	}
	OPT ( "Frame10" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[9], value, &b_error ) ;
	}
	OPT ( "Frame11" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[10], value, &b_error ) ;
	}
	OPT ( "Frame12" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[11], value, &b_error ) ;
	}
	OPT ( "Frame13" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[12], value, &b_error ) ;
	}
	OPT ( "Frame14" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[13], value, &b_error ) ;
	}
	OPT ( "Frame15" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[14], value, &b_error ) ;
	}
	OPT ( "Frame16" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[15], value, &b_error ) ;
	}
	OPT ( "Frame17" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[16], value, &b_error ) ;
	}
	OPT ( "Frame18" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[17], value, &b_error ) ;
	}
	OPT ( "Frame19" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[18], value, &b_error ) ;
	}
	OPT ( "Frame20" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[19], value, &b_error ) ;
	}
	OPT ( "Frame21" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[20], value, &b_error ) ;
	}
	OPT ( "Frame22" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[21], value, &b_error ) ;
	}
	OPT ( "Frame23" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[22], value, &b_error ) ;
	}
	OPT ( "Frame24" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[23], value, &b_error ) ;
	}
	OPT ( "Frame25" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[24], value, &b_error ) ;
	}
	OPT ( "Frame26" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[25], value, &b_error ) ;
	}
	OPT ( "Frame27" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[26], value, &b_error ) ;
	}
	OPT ( "Frame28" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[27], value, &b_error ) ;
	}
	OPT ( "Frame29" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[28], value, &b_error ) ;
	}
	OPT ( "Frame30" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[29], value, &b_error ) ;
	}
	OPT ( "Frame31" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[30], value, &b_error ) ;
	}
	OPT ( "Frame32" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[31], value, &b_error ) ;
	}
	OPT ( "Frame33" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[32], value, &b_error ) ;
	}
	OPT ( "Frame34" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[33], value, &b_error ) ;
	}
	OPT ( "Frame35" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[34], value, &b_error ) ;
	}
	OPT ( "Frame36" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[35], value, &b_error ) ;
	}
	OPT ( "Frame37" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[36], value, &b_error ) ;
	}
	OPT ( "Frame38" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[37], value, &b_error ) ;
	}
	OPT ( "Frame39" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[38], value, &b_error ) ;
	}
	OPT ( "Frame40" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[39], value, &b_error ) ;
	}
	OPT ( "Frame41" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[40], value, &b_error ) ;
	}
	OPT ( "Frame42" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[41], value, &b_error ) ;
	}
	OPT ( "Frame43" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[42], value, &b_error ) ;
	}
	OPT ( "Frame44" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[43], value, &b_error ) ;
	}
	OPT ( "Frame45" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[44], value, &b_error ) ;
	}
	OPT ( "Frame46" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[45], value, &b_error ) ;
	}
	OPT ( "Frame47" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[46], value, &b_error ) ;
	}
	OPT ( "Frame48" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[47], value, &b_error ) ;
	}
	OPT ( "Frame49" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[48], value, &b_error ) ;
	}
	OPT ( "Frame50" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[49], value, &b_error ) ;
	}
	OPT ( "Frame51" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[50], value, &b_error ) ;
	}
	OPT ( "Frame52" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[51], value, &b_error ) ;
	}
	OPT ( "Frame53" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[52], value, &b_error ) ;
	}
	OPT ( "Frame54" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[53], value, &b_error ) ;
	}
	OPT ( "Frame55" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[54], value, &b_error ) ;
	}
	OPT ( "Frame56" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[55], value, &b_error ) ;
	}
	OPT ( "Frame57" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[56], value, &b_error ) ;
	}
	OPT ( "Frame58" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[57], value, &b_error ) ;
	}
	OPT ( "Frame59" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[58], value, &b_error ) ;
	}
	OPT ( "Frame60" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[59], value, &b_error ) ;
	}
	OPT ( "Frame61" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[60], value, &b_error ) ;
	}
	OPT ( "Frame62" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[61], value, &b_error ) ;
	}
	OPT ( "Frame63" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[62], value, &b_error ) ;
	}
	OPT ( "Frame64" )
	{
		x265_file_parse_gop_entry ( &p->gop.gop_list[63], value, &b_error ) ;
	}
	OPT ( "ListCombination" )
	{
		p->b_use_list_combination = atobool ( value ) ;
	}
	OPT ( "TransformSkip" )
	{
		p->b_use_transform_skip = atobool ( value ) ;
	}
	OPT ( "TransformSkipFast" )
	{
		p->b_use_transform_skip_fast = atobool ( value ) ;
	}
	OPT ( "AMP" )
	{
		p->b_enable_amp = atobool ( value ) ;
	}
	OPT ( "QP" )
	{
		p->f_qp = atof ( value ) ;
	}
	OPT ( "MaxDeltaQP" )
	{
		p->rdo.i_max_delta_qp = atoi ( value ) ;
	}
	OPT ( "DeltaQpRD" )
	{
		p->rdo.i_delta_qp_rd = atoi ( value ) ;
	}
	OPT ( "MaxCuDQPDepth" )
	{
		p->rdo.i_max_cu_delta_qp_depth = atoi ( value ) ;
	}
	OPT ( "CbQpOffset" )
	{
		p->i_cb_qp_offset = atoi ( value ) ;
	}
	OPT ( "CrQpOffset" )
	{
		p->i_cr_qp_offset = atoi ( value ) ;
	}
#if X265_ADAPTIVE_QP_SELECTION
	OPT ( "AdaptiveQpSelection" )
	{
		p->b_use_adapt_qp_select = atobool ( value ) ;
	}
#endif
	OPT ( "AdaptiveQP" )
	{
		p->b_use_adaptive_qp = atobool ( value ) ;
	}
	OPT ( "MaxQPAdaptationRange" )
	{
		p->i_qp_adaptation_range = atoi ( value ) ;
	}
	OPT ( "MaxCUWidth" )
	{
		p->sps.i_max_cu_width = atoi ( value ) ;
	}
	OPT ( "MaxCUHeight" )
	{
		p->sps.i_max_cu_height = atoi ( value ) ;
	}
	OPT ( "MaxCUSize" )
	{
		p->sps.i_max_cu_width = atoi ( value ) ;
		p->sps.i_max_cu_height = atoi ( value ) ;
	}
	OPT ( "MaxPartitionDepth" )
	{
		p->sps.i_max_cu_depth = atoi ( value ) ;
	}
	OPT ( "QuadtreeTULog2MaxSize" )
	{
		p->sps.i_quadtree_tu_log2_max_size = atoi ( value ) ;
	}
	OPT ( "QuadtreeTULog2MinSize" )
	{
		p->sps.i_quadtree_tu_log2_min_size = atoi ( value ) ;
	}
	OPT ( "QuadtreeTUMaxDepthIntra" )
	{
		p->sps.i_quadtree_tu_max_depth_intra = atoi ( value ) ;
	}
	OPT ( "QuadtreeTUMaxDepthInter" )
	{
		p->sps.i_quadtree_tu_max_depth_inter = atoi ( value ) ;
	}
	OPT ( "InputBitDepth" )
	{
		p->i_input_bit_depth_y = atoi ( value ) ;
	}
	OPT ( "OutputBitDepth" )
	{
		p->i_output_bit_depth_y = atoi ( value ) ;
	}
	OPT ( "InternalBitDepth" )
	{
		p->sps.i_bit_depth_y = atoi ( value ) ;
	}
	OPT ( "InputBitDepthC" )
	{
		p->i_input_bit_depth_c = atoi ( value ) ;
	}
	OPT ( "OutputBitDepthC" )
	{
		p->i_output_bit_depth_c = atoi ( value ) ;
	}
	OPT ( "InternalBitDepthC" )
	{
		p->sps.i_bit_depth_c = atoi ( value ) ;
	}
	OPT ( "PCMInputBitDepthFlag" )
	{
		p->b_pcm_input_bit_depth_flag = atobool ( value ) ;
	}
	OPT ( "LosslessCuEnabled" )
	{
		p->sps.b_use_lossless = atobool ( value ) ;
	}
	OPT ( "SAO" )
	{
		p->sps.b_use_sao = atobool ( value ) ;
	}
	OPT ( "MaxNumOffsetsPerPic" )
	{
		p->sao.i_max_num_offsets_per_pic = atoi ( value ) ;
	}
	OPT ( "SAOLcuBoundary" )
	{
		p->sao.i_sao_lcu_boundary = atoi ( value ) ;
	}
	OPT ( "SAOLcuBasedOptimization" )
	{
		p->sao.i_sao_lcu_based_optimization = atoi ( value ) ;
	}
	OPT ( "LoopFilterDisable" )
	{
		p->sps.b_loop_filter_disable = atobool ( value ) ;
	}
	OPT ( "LoopFilterOffsetInPPS" )
	{
		p->deblock.i_loop_filter_offset_in_pps = atoi ( value ) ;
	}
	OPT ( "LoopFilterBetaOffset-div2" )
	{
		p->deblock.i_loop_filter_beta_offset_div_2 = atoi ( value ) ;
	}
	OPT ( "LoopFilterTcOffset-div2" )
	{
		p->deblock.i_loop_filter_tc_offset_div_2 = atoi ( value ) ;
	}
	OPT ( "DeblockingFilterControlPresent" )
	{
		p->deblock.i_deblocking_filter_control_present = atoi ( value ) ;
	}
	OPT ( "PCMEnabledFlag" )
	{
		p->sps.b_use_pcm = atobool ( value ) ;
	}
	OPT ( "PCMLog2MaxSize" )
	{
		p->pcm.i_pcm_log2_max_size = atoi ( value ) ;
	}
	OPT ( "PCMLog2MinSize" )
	{
		p->pcm.i_pcm_log2_min_size = atoi ( value ) ;
	}
	OPT ( "PCMFilterDisableFlag" )
	{
		p->pcm.b_pcm_filter_disable_flag = atobool ( value ) ;
	}
	OPT ( "SBACRD" )
	{
		p->b_use_sbac_rd = atobool ( value ) ;
	}
	OPT ( "ASR" )
	{
		p->b_use_asr = atobool ( value ) ;
	}
	OPT ( "HadamardME" )
	{
		p->b_use_had_me = atobool ( value ) ;
	}
	OPT ( "RDOQ" )
	{
		p->rdo.b_use_rdo_q = atobool ( value ) ;
	}
	OPT ( "RDOQTS" )
	{
		p->rdo.b_use_rdo_q_ts = atobool ( value ) ;
	}

#if X265_L0232_RD_PENALTY
	OPT ( "RDpenalty" )
	{
		p->rdo.i_rd_penalty = atoi ( value ) ;
	}
#endif
	OPT ( "FastSearch" )
	{
		p->i_fast_search = atoi ( value ) ;
	}
	OPT ( "SearchRange" )
	{
		p->i_search_range = atoi ( value ) ;
	}
	OPT ( "BipredSearchRange" )
	{
		p->i_bipred_search_range = atoi ( value ) ;
	}
	OPT ( "FEN" )
	{
		p->b_use_fast_enc = atobool ( value ) ;
	}
	OPT ( "ECU" )
	{
		p->b_use_early_cu = atobool ( value ) ;
	}
	OPT ( "FDM" )
	{
		p->b_use_fast_decision_for_merge = atobool ( value ) ;
	}
	OPT ( "CFM" )
	{
		p->b_use_cbf_fast_mode = atobool ( value ) ;
	}
	OPT ( "ESD" )
	{
		p->b_use_early_skip_detection = atobool ( value ) ;
	}
	OPT ( "SliceMode" )
	{
		p->i_slice_mode = atoi ( value ) ;
	}
	OPT ( "SliceArgument" )
	{
		p->i_slice_argument = atoi ( value ) ;
	}
	OPT ( "SliceSegmentMode" )
	{
		p->i_slice_segment_mode = atoi ( value ) ;
	}
	OPT ( "SliceSegmentArgument" )
	{
		p->i_slice_segment_argument = atoi ( value ) ;
	}
	OPT ( "LFCrossSliceBoundaryFlag" )
	{
		p->deblock.b_lf_cross_slice_boundary_flag = atobool ( value ) ;
	}
	OPT ( "LFCrossTileBoundaryFlag" )
	{
		p->deblock.b_lf_cross_tile_boundary_flag = atobool ( value ) ;
	}
	OPT ( "UniformSpacingIdc" )
	{
		p->pps.i_uniform_spacing_idr = atoi ( value ) ;
	}
	OPT ( "NumTileColumnsMinus1" )
	{
		p->pps.i_num_columns_minus_1 = atoi ( value ) ;
	}
	OPT ( "ColumnWidthArray" )
	{
		x265_file_parse_column_width ( p, value, &b_error ) ;
	}
	OPT ( "NumTileRowsMinus1" )
	{
		p->pps.i_num_rows_minus_1 = atoi ( value ) ;
	}
	OPT ( "RowHeightArray" )
	{
		x265_file_parse_row_height ( p, value, &b_error ) ;
	}
	OPT ( "WaveFrontSynchro" )
	{
		p->pps.i_wave_front_synchro = atoi ( value ) ;
	}
	OPT ( "ConstrainedIntraPred" )
	{
		p->b_use_constrained_intra_pred = atoi ( value ) ;
	}
	OPT ( "SEIDecodedPictureHash")
	{
		p->sei.i_decoded_picture_hash_sei_enabled = atoi ( value ) ;
	}
	OPT ( "SEIpictureDigest" )
	{
		p->sei.i_decoded_picture_hash_sei_enabled = atoi ( value ) ;
	}
	OPT ( "SEIRecoveryPoint" )
	{
		p->sei.i_recovery_point_sei_enabled = atoi ( value ) ;
	}
	OPT ( "SEIBufferingPeriod" )
	{
		p->sei.i_buffering_period_sei_enabled = atoi ( value ) ;
	}
	OPT ( "SEIPictureTiming" )
	{
		p->sei.i_picture_timing_sei_enabled = atoi ( value ) ;
	}
	OPT ( "SEIFramePacking" )
	{
		p->sei.i_framePacking_sei_enabled = atoi ( value ) ;
	}
	OPT ( "SEIFramePackingType" )
	{
		p->sei.i_framePacking_sei_type = atoi ( value ) ;
	}
	OPT ( "SEIFramePackingId" )
	{
		p->sei.i_framePacking_sei_id = atoi ( value ) ;
	}
	OPT ( "SEIFramePackingQuincunx" )
	{
		p->sei.i_framePacking_sei_quincunx = atoi ( value ) ;
	}
	OPT ( "SEIFramePackingInterpretation" )
	{
		p->sei.i_framePacking_sei_interpretation = atoi ( value ) ;
	}
	OPT ( "SEIDisplayOrientation" )
	{
		p->sei.i_display_orientation_sei_Angle = atoi ( value ) ;
	}
	OPT ( "SEITemporalLevel0Index" )
	{
		p->sei.i_temporal_level0_index_sei_enabled = atoi ( value ) ;
	}
	OPT ( "SEIGradualDecodingRefreshInfo" )
	{
		p->sei.i_gradual_decoding_refresh_info_enabled = atoi ( value ) ;
	}
	OPT ( "SEIDecodingUnitInfo" )
	{
		p->sei.i_decoding_unit_info_sei_enabled = atoi ( value ) ;
	}
	OPT ( "WeightedPredP" )
	{
		p->pps.b_use_weighted_pred = atobool ( value ) ;
	}
	OPT ( "WeightedPredB" )
	{
		p->pps.b_use_weighted_bi_pred = atobool ( value ) ;
	}
	OPT ( "Log2ParallelMergeLevel" )
	{
		p->pps.i_log2_parallel_merge_level = atoi ( value ) ;
	}
	OPT ( "MaxNumMergeCand" )
	{
		p->i_max_num_merge_cand = atoi ( value ) ;
	}
	OPT ( "TMVPMode" )
	{
		p->sps.i_tmvp_mode_id = atoi ( value ) ;
	}
	OPT ( "SignHideFlag" )
	{
		p->pps.i_sign_hide_flag = atoi ( value ) ;
	}
#if X265_RATE_CONTROL_LAMBDA_DOMAIN
	OPT ( "RateControl" )
	{
		p->rc.b_enable_rate_control = atobool ( value ) ;
	}
	OPT ( "TargetBitrate" )
	{
		p->rc.i_target_bitrate = atoi ( value ) ;
	}
	OPT ( "KeepHierarchicalBit" )
	{
		p->rc.b_keep_hierarchical_bit = atobool ( value ) ;
	}
	OPT ( "LCULevelRateControl" )
	{
		p->rc.b_lcu_level_rc = atobool ( value ) ;
	}
	OPT ( "RCLCUSeparateModel" )
	{
		p->rc.b_use_lcu_separate_model = atobool ( value ) ;
	}
	OPT ( "InitialQP" )
	{
		p->rc.i_initial_qp = atoi ( value ) ;
	}
	OPT ( "RCForceIntraQP" )
	{
		p->rc.b_force_intra_qp = atobool ( value ) ;
	}
#else
	OPT ( "RateCtrl" )
	{
		p->b_enable_rate_ctrl = atobool ( value ) ;
	}
	OPT ( "TargetBitrate" )
	{
		p->i_target_bitrate = atoi ( value ) ;
	}
	OPT ( "NumLcuInUnit" )
	{
		p->i_num_lcu_in_unit = atoi ( value ) ;
	}
#endif
	OPT ( "ScalingList" )
	{
		p->sps.i_use_scaling_list_id = atoi ( value ) ;
	}
	OPT ( "TransquantBypassEnableFlag" )
	{
		p->pps.b_transquant_bypass_enable_flag = atobool ( value ) ;
	}
	OPT ( "CUTransquantBypassFlagValue" )
	{
		p->b_cu_transquant_bypass_flag_value = atobool ( value ) ;
	}
	OPT ( "RecalculateQPAccordingToLambda" )
	{
		p->b_recalculate_qp_according_to_lambda = atobool ( value ) ;
	}
	OPT ( "StrongIntraSmoothing" )
	{
		p->sps.b_use_strong_intra_smoothing = atobool ( value ) ;
	}
	OPT ( "SEIActiveParameterSets" )
	{
		p->sei.i_active_parameter_sets_sei_enabled = atoi ( value ) ;
	}

#if X265_SIGNAL_BITRATE_PICRATE_IN_VPS
	OPT ( "BitRatePicRateMaxTLayers" )
	{
		p->vps.i_bit_rate_pic_rate_max_t_layers = atoi ( value ) ;
	}
	OPT ( "BitRateInfoPresent" )
	{
		x265_file_read_bool_string ( value, &b_error, p->vps.i_bit_rate_pic_rate_max_t_layers,
									&p->vps.bit_rate_info_present_flag,
									"bit rate info. present flag" ) ;
	}
	OPT ( "PicRateInfoPresent" )
	{
		x265_file_read_bool_string ( value, &b_error, p->vps.i_bit_rate_pic_rate_max_t_layers,
									&p->vps.pic_rate_info_present_flag,
									"bit rate info. present flag" ) ;
	}
	OPT ( "AvgBitRate" )
	{
		x265_file_read_int_string ( value, &b_error, p->vps.i_bit_rate_pic_rate_max_t_layers,
									&p->vps.avg_bit_rate,
									"avg. bit rate" ) ;
	}
	OPT ( "MaxBitRate" )
	{
		x265_file_read_int_string ( value, &b_error, p->vps.i_bit_rate_pic_rate_max_t_layers,
									&p->vps.max_bit_rate,
									"max. bit rate" ) ;
	}
	OPT ( "AvgPicRate" )
	{
		x265_file_read_int_string ( value, &b_error, p->vps.i_bit_rate_pic_rate_max_t_layers,
									&p->vps.avg_pic_rate,
									"avg. pic rate" ) ;
	}
	OPT ( "ConstantPicRateIdc" )
	{
		x265_file_read_int_string ( value, &b_error, p->vps.i_bit_rate_pic_rate_max_t_layers,
									&p->vps.constant_pic_rate_idc,
									"constant pic rate Idc" ) ;
	}
#endif

	OPT ( "VuiParametersPresent" )
	{
		p->sps.b_vui_parameters_present_flag = atobool ( value ) ;
	}
	OPT ( "AspectRatioInfoPresent" )
	{
		p->sps.vui.b_aspec_ratio_info_present_flag = atobool ( value ) ;
	}
	OPT ( "AspectRatioIdc" )
	{
		p->sps.vui.i_aspect_ratio_idc = atoi ( value ) ;
	}
	OPT ( "SarWidth" )
	{
		p->sps.vui.i_sar_width = atoi ( value ) ;
	}
	OPT ( "SarHeight" )
	{
		p->sps.vui.i_sar_height = atoi ( value ) ;
	}
	OPT ( "OverscanInfoPresent" )
	{
		p->sps.vui.b_overscan_info_present_flag = atobool ( value ) ;
	}
	OPT ( "OverscanAppropriate" )
	{
		p->sps.vui.b_overscan_appropriate_flag = atobool ( value ) ;
	}
	OPT ( "VideoSignalTypePresent" )
	{
		p->sps.vui.b_video_signal_type_present_flag = atobool ( value ) ;
	}
	OPT ( "VideoFormat" )
	{
		p->sps.vui.i_video_format = atoi ( value ) ;
	}
	OPT ( "VideoFullRange" )
	{
		p->sps.vui.b_video_full_range_flag = atobool ( value ) ;
	}
	OPT ( "ColourDescriptionPresent" )
	{
		p->sps.vui.b_colour_description_present_flag = atobool ( value ) ;
	}
	OPT ( "ColourPrimaries" )
	{
		p->sps.vui.i_colour_primaries = atoi ( value ) ;
	}
	OPT ( "TransferCharateristics" )
	{
		p->sps.vui.i_transfer_characteristics = atoi ( value ) ;
	}
	OPT ( "MatrixCoefficients" )
	{
		p->sps.vui.i_matrix_coefficients = atoi ( value ) ;
	}
	OPT ( "ChromaLocInfoPresent" )
	{
		p->sps.vui.b_chroma_loc_info_present_flag = atobool ( value ) ;
	}
	OPT ( "ChromaSampleLocTypeTopField" )
	{
		p->sps.vui.i_chroma_sample_loc_type_top_field = atoi ( value ) ;
	}
	OPT ( "ChromaSampleLocTypeBottomField" )
	{
		p->sps.vui.i_chroma_sample_loc_type_bottom_field = atoi ( value ) ;
	}
	OPT ( "NeutralChromaIndication" )
	{
		p->sps.vui.b_neutral_chroma_indication_flag = atobool ( value ) ;
	}
	OPT ( "DefaultDisplayWindowFlag" )
	{
		p->sps.vui.b_default_display_window_flag = atobool ( value ) ;
	}
	OPT ( "DefDispWinLeftOffset" )
	{
		p->sps.vui.i_def_disp_win_left_offset = atoi ( value ) ;
	}
	OPT ( "DefDispWinRightOffset" )
	{
		p->sps.vui.i_def_disp_win_right_offset = atoi ( value ) ;
	}
	OPT ( "DefDispWinTopOffset" )
	{
		p->sps.vui.i_def_disp_win_top_offset = atoi ( value ) ;
	}
	OPT ( "DefDispWinBottomOffset" )
	{
		p->sps.vui.i_def_disp_win_bottom_offset = atoi ( value ) ;
	}
	OPT ( "FrameFieldInfoPresentFlag" )
	{
		p->sps.vui.b_frame_field_info_present_flag = atobool ( value ) ;
	}
	OPT ( "PocProportionalToTimingFlag" )
	{
		p->sps.vui.b_poc_proportional_to_timing_flag = atobool ( value ) ;
	}
	OPT ( "NumTicksPocDiffOneMinus1" )
	{
		p->sps.vui.i_num_ticks_poc_diff_one_minus1 = atoi ( value ) ;
	}
	OPT ( "BitstreamRestriction" )
	{
		p->sps.vui.b_bitstream_restriction_flag = atobool ( value ) ;
	}
	OPT ( "TilesFixedStructure" )
	{
		p->sps.vui.b_tiles_fixed_structure_flag = atobool ( value ) ;
	}
	OPT ( "MotionVectorsOverPicBoundaries" )
	{
		p->sps.vui.b_motion_vectors_over_pic_boundaries_flag = atobool ( value ) ;
	}
	OPT ( "MaxBytesPerPicDenom" )
	{
		p->sps.vui.i_max_bytes_per_pic_denom = atoi ( value ) ;
	}
	OPT ( "MaxBitsPerMinCuDenom" )
	{
		p->sps.vui.i_max_bits_per_min_cu_denom = atoi ( value ) ;
	}
	OPT ( "Log2MaxMvLengthHorizontal" )
	{
		p->sps.vui.i_log2_max_mv_length_horizontal = atoi ( value ) ;
	}
	OPT ( "Log2MaxMvLengthVertical" )
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

    *name_index = i_index ;
    if( name_buf )
    {
        free( name_buf );
    }

    b_error |= value_was_null && !name_was_bool;
    return b_error ? X265_PARAM_BAD_VALUE : 0;
}
