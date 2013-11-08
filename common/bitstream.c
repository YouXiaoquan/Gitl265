


#include "common.h"

void bit_if_write_align_one (x265_bit_if_t *bit_if) ;
void bit_if_write_align_zero (x265_bit_if_t *bit_if) ;
void bit_if_store (x265_bit_if_t *bit_if, x265_bit_if_t *dst_bit_if) ;
void x265_bit_if_delete ( x265_bit_if_t *bit_if ) ;

void output_bitstream_write_align_one (x265_bit_if_t *bit_if) ;
void output_bitstream_write_align_zero (x265_bit_if_t *bit_if) ;
void output_bitstream_write ( x265_bit_if_t *bit_if, uint32_t i_bits, int i_count ) ;
void output_bitstream_reset_bits ( x265_bit_if_t *bit_if ) ;
int output_bitstream_get_number_of_written_bits ( x265_bit_if_t *bit_if ) ;
void x265_output_bitstream_delete ( x265_bit_if_t *bit_if ) ;

void x265_bit_counter_delete ( x265_bit_if_t *bit_if ) ;
void bit_counter_write ( x265_bit_if_t *bit_if, uint32_t i_bits, int i_count  ) ;
void bit_counter_reset_bits ( x265_bit_if_t *bit_if ) ;
int bit_counter_get_number_of_written_bits ( x265_bit_if_t *bit_if ) ;
void bit_counter_store (x265_bit_if_t *bit_if, x265_bit_if_t *dst_bit_if) ;

void bs_write( bs_t *s, int i_count, uint32_t i_bits )
{
    if( WORD_SIZE == 8 )
    {
        s->cur_bits = (s->cur_bits << i_count) | i_bits;
        s->i_left -= i_count;
        if( s->i_left <= 32 )
        {
#if WORDS_BIGENDIAN
            M32( s->p ) = s->cur_bits >> (32 - s->i_left);
#else
            M32( s->p ) = endian_fix( s->cur_bits << s->i_left );
#endif
            s->i_left += 32;
            s->p += 4;
        }
    }
    else
    {
        if( i_count < s->i_left )
        {
            s->cur_bits = (s->cur_bits << i_count) | i_bits;
            s->i_left -= i_count;
        }
        else
        {
            i_count -= s->i_left;
            s->cur_bits = (s->cur_bits << s->i_left) | (i_bits >> i_count);
            M32( s->p ) = endian_fix( s->cur_bits );
            s->p += 4;
            s->cur_bits = i_bits;
            s->i_left = 32 - i_count;
        }
    }
}

static uint8_t *x265_nal_escape_c( uint8_t *dst, uint8_t *src, uint8_t *end )
{
    if( src < end )
    {
    	*dst++ = *src++;
    }
    if( src < end )
    {
    	*dst++ = *src++;
    }
    while( src < end )
    {
        if( src[0] <= 0x03 && !dst[-2] && !dst[-1] )
        {
            *dst++ = 0x03;
        }
        *dst++ = *src++;
    }
    return dst;
}

 /****************************************************************************
 * x265_nal_encode:
 ****************************************************************************/
void x265_nal_encode( x265_t *h, uint8_t *dst, x265_nal_t *nal )
{
	uint32_t i_nal_header = 0 ;
    uint8_t *src = nal->p_payload;
    uint8_t *end = nal->p_payload + nal->i_payload;
    uint8_t *orig_dst = dst;

    /*
    for ( uint8_t *start = src ; start < end ; ++ start )
    {
    	fprintf ( stderr, "%02x ", (int)(*start) ) ;
    }
    */

    if( h->param.b_annexb )
	{
		if( nal->b_long_start_code )
		{
			*dst++ = 0x00;
		}
		*dst++ = 0x00;
		*dst++ = 0x00;
		*dst++ = 0x01;
	}
	else /* save room for size later */
	{
		dst += 4;
	}

	i_nal_header = ((nal->i_type & 0x3F) << 9)
					| ((nal->i_reserved_zero6_bits & 0x3F) << 3)
					| ((nal->i_temporal_id + 1) & 0x07 ) ;
    /* nal header */
    *dst++ = ((uint8_t)((i_nal_header >> 8) & 0xFF)) ;
    *dst++ = ((uint8_t)(i_nal_header & 0xFF)) ;

    dst = h->bsf.nal_escape( dst, src, end );
    int size = (dst - orig_dst) - 4;

	/* Write the size header for mp4/etc */
	if( !h->param.b_annexb )
	{
		/* Size doesn't include the size of the header we're writing now. */
		orig_dst[0] = size>>24;
		orig_dst[1] = size>>16;
		orig_dst[2] = size>> 8;
		orig_dst[3] = size>> 0;
	}

    nal->i_payload = size+4;
    nal->p_payload = orig_dst;

}

void bs_add_stream ( bs_t *s, bs_t *bs_src )
{
	int32_t i_bytes_left = 0 ;
	int32_t i_bits_left = 0 ;
	int32_t loop = 0 ;
	uint8_t *p_curr = NULL ;

	for ( p_curr = bs_src->p_start ; p_curr < bs_src->p ; ++ p_curr )
	{
		bs_write ( s, 8, p_curr[0] ) ;
	}
	i_bytes_left = ((WORD_SIZE * 8 - bs_src->i_left) >> 3) ;
	i_bits_left = (WORD_SIZE * 8 - bs_src->i_left) - (i_bytes_left << 3) ;
	for ( loop = i_bytes_left - 1 ; loop >= 0 ; -- loop )
	{
		bs_write ( s,
				8,
				(bs_src->cur_bits >> (loop * 8 + i_bits_left)) & 0xFF
				) ;
	}
	bs_write ( s, i_bits_left, bs_src->cur_bits & ((1 << i_bits_left) - 1 ) ) ;
}

int bs_get_number_of_written_bits ( bs_t *s )
{
	return ( 8 * (s->p - s->p_start) + (WORD_SIZE*8) - s->i_left );
}



//	( bs_src->cur_bits >> ((loop - 1) * 8 + i_bits_left) )


void x265_bitstream_init( int cpu, x265_bitstream_function_t *pf )
{
    pf->nal_escape = x265_nal_escape_c;
}

void x265_bit_if_delete ( x265_bit_if_t *bit_if )
{

}

void bit_if_write_align_one (x265_bit_if_t *bit_if)
{

}

void bit_if_write_align_zero (x265_bit_if_t *bit_if)
{

}

void bit_if_store (x265_bit_if_t *bit_if, x265_bit_if_t *dst_bit_if)
{

}

int x265_bit_if_init ( x265_bit_if_t * bit_if )
{
	memset ( bit_if, 0, sizeof (x265_bit_if_t) ) ;
	bit_if->bit_if_delete = x265_bit_if_delete ;
	bit_if->write_alignOne = bit_if_write_align_one ;
	bit_if->write_align_zero = bit_if_write_align_zero ;
	bit_if->store = bit_if_store ;

	return 0 ;
}

x265_bit_if_t *x265_output_bitstream_new ()
{
	x265_bit_if_t *bit_if = NULL ;

	CHECKED_MALLOCZERO( bit_if, sizeof(x265_output_bitstream_t) );
	if ( x265_output_bitstream_init (bit_if) )
	{
		goto fail ;
	}
	return bit_if ;
fail:
	x265_output_bitstream_delete ( bit_if ) ;
	return NULL ;
}

void x265_output_bitstream_delete ( x265_bit_if_t * bit_if )
{
	x265_output_bitstream_t *output_bitstream = NULL ;

	if ( bit_if )
	{
		output_bitstream = (x265_output_bitstream_t*) bit_if ;
		x265_output_bitstream_deinit ( (x265_bit_if_t*)output_bitstream ) ;
		x265_free ( output_bitstream ) ;
	}
}

int x265_output_bitstream_init ( x265_bit_if_t *bit_if )
{
	x265_output_bitstream_t *output_bitstream = NULL ;

	if ( x265_bit_if_init ( bit_if ) )
	{
		goto fail ;
	}
	output_bitstream = (x265_output_bitstream_t*) bit_if ;
	output_bitstream->i_bitstream = BITSTREAM_MAX_SIZE ;
	CHECKED_MALLOCZERO( output_bitstream->p_bitstream,
						sizeof(uint8_t) * output_bitstream->i_bitstream );
	bs_init ( &output_bitstream->bs,
			output_bitstream->p_bitstream,
			output_bitstream->i_bitstream ) ;
	bit_if->bit_if_delete = x265_output_bitstream_delete ;
	bit_if->write_alignOne = output_bitstream_write_align_one ;
	bit_if->write_align_zero = output_bitstream_write_align_zero ;
	bit_if->write = output_bitstream_write ;
	bit_if->reset_bits = output_bitstream_reset_bits ;
	bit_if->get_number_of_written_bits = output_bitstream_get_number_of_written_bits ;

	return 0 ;
fail:
	x265_output_bitstream_deinit ( bit_if ) ;
	return -1 ;
}

void x265_output_bitstream_deinit ( x265_bit_if_t *bit_if )
{
	x265_output_bitstream_t *output_bitstream = NULL ;

	output_bitstream = (x265_output_bitstream_t *)bit_if ;
	x265_free ( output_bitstream->p_bitstream ) ;
}

void x265_output_bitstream_clear ( x265_output_bitstream_t *output_bitstream )
{
	bs_init ( &output_bitstream->bs,
			output_bitstream->p_bitstream,
			output_bitstream->i_bitstream ) ;
}

void x265_output_bitstream_write_byte_alignment ( x265_output_bitstream_t *output_bitstream )
{
	output_bitstream->write ( (x265_bit_if_t*)output_bitstream, 1, 1 ) ;
	output_bitstream->write_align_zero ( (x265_bit_if_t*)output_bitstream ) ;
}

void x265_output_bitstream_add_stream ( x265_output_bitstream_t *output_bitstream,
										x265_output_bitstream_t *output_bitstream_src )
{
	bs_add_stream ( &output_bitstream->bs, &output_bitstream_src->bs ) ;
}

void x265_output_bitstream_print ( bs_t *s, const char *filename )
{
	int32_t loop = 0 ;
	FILE *file = NULL ;
	int32_t i_bytes_left = 0 ;
	int32_t i_bits_left = 0 ;
	int32_t i_index = 0 ;
	uint8_t *p_curr = NULL ;
	static int32_t b_first_print_output_bitstream = 1 ;
	static int32_t i_output_bitstream_number = 1 ;

	if ( b_first_print_output_bitstream )
	{
		file = fopen ( filename, "wb" ) ;
		b_first_print_output_bitstream = 0 ;
	}
	else
	{
		file = fopen ( filename, "r+b" ) ;
		fseek ( file, 0, SEEK_END ) ;
	}

	fprintf ( file, "SliceHeader%d:\n", i_output_bitstream_number ) ;
	i_output_bitstream_number ++ ;




	for ( p_curr = s->p_start ; p_curr < s->p ; ++ p_curr )
	{
		fprintf ( file, "%02X ", p_curr[0] ) ;
		i_index ++ ;
		if ( i_index % 8 == 0 )
		{
			fprintf ( file, "\n" ) ;
		}
	}
	i_bytes_left = ((WORD_SIZE * 8 - s->i_left) >> 3) ;
	i_bits_left = (WORD_SIZE * 8 - s->i_left) - (i_bytes_left << 3) ;
	for ( loop = i_bytes_left - 1 ; loop >= 0 ; -- loop )
	{
		fprintf ( file, "%02X ",
				(s->cur_bits >> (loop * 8 + i_bits_left)) & 0xFF ) ;
		i_index ++ ;
		if ( i_index % 8 == 0 )
		{
			fprintf ( file, "\n" ) ;
		}
	}
	if ( i_bits_left > 0 )
	{
		fprintf ( file, "%X ",
				s->cur_bits & ((1 << i_bits_left) - 1 ) ) ;
	}
	fprintf ( file, "\n" ) ;

	fclose ( file ) ;
}

void output_bitstream_write_align_one (x265_bit_if_t *bit_if)
{
	x265_output_bitstream_t *output_bitstream = NULL ;
	uint32_t i_num_bits = 0 ;

	output_bitstream = (x265_output_bitstream_t *)bit_if ;

	i_num_bits = ( output_bitstream->bs.i_left ) & 0x07 ;
	bs_write ( &output_bitstream->bs, i_num_bits, (1 << i_num_bits) - 1 ) ;

}

void output_bitstream_write_align_zero (x265_bit_if_t *bit_if)
{
	x265_output_bitstream_t *output_bitstream = NULL ;

	output_bitstream = (x265_output_bitstream_t *)bit_if ;
	bs_flush ( &output_bitstream->bs ) ;
}

void output_bitstream_write ( x265_bit_if_t *bit_if, uint32_t i_bits, int i_count  )
{
	x265_output_bitstream_t *output_bitstream = NULL ;

	output_bitstream = (x265_output_bitstream_t *)bit_if ;
	bs_write ( &output_bitstream->bs, i_count, i_bits ) ;
}

void output_bitstream_reset_bits ( x265_bit_if_t *bit_if )
{
	x265_output_bitstream_t *output_bitstream = NULL ;

	output_bitstream = (x265_output_bitstream_t *)bit_if ;
	bs_init ( &output_bitstream->bs,
			output_bitstream->p_bitstream,
			output_bitstream->i_bitstream ) ;

}

int output_bitstream_get_number_of_written_bits ( x265_bit_if_t *bit_if )
{
	x265_output_bitstream_t *output_bitstream = NULL ;

	output_bitstream = (x265_output_bitstream_t *)bit_if ;

	return ( 8 * (output_bitstream->bs.p
				- output_bitstream->bs.p_start) + (WORD_SIZE*8)
			- output_bitstream->bs.i_left );
}




x265_bit_if_t *x265_bit_counter_new ()
{
	x265_bit_if_t *bit_if = NULL ;

	CHECKED_MALLOCZERO( bit_if, sizeof(x265_bit_counter_t) );
	if ( x265_bit_counter_init (bit_if) )
	{
		goto fail ;
	}
	return bit_if ;
fail:
	x265_bit_counter_delete ( bit_if ) ;
	return NULL ;
}

void x265_bit_counter_delete ( x265_bit_if_t * bit_if )
{
	x265_bit_counter_t *bit_counter = NULL ;

	if ( bit_if )
	{
		x265_bit_counter_deinit ( bit_if ) ;
		bit_counter = (x265_bit_counter_t *) bit_if ;
		x265_free ( bit_counter ) ;
	}
}

int x265_bit_counter_init ( x265_bit_if_t *bit_if )
{
	if ( x265_bit_if_init ( bit_if ) )
	{
		goto fail ;
	}
	bit_if->bit_if_delete = x265_bit_counter_delete ;
	bit_if->write = bit_counter_write ;
	bit_if->reset_bits = bit_counter_reset_bits ;
	bit_if->get_number_of_written_bits = bit_counter_get_number_of_written_bits ;
	bit_if->store = bit_counter_store ;

	return 0 ;
fail:
	x265_bit_counter_deinit ( bit_if ) ;
	return -1 ;
}

void x265_bit_counter_deinit ( x265_bit_if_t *bit_if )
{

}

void bit_counter_write ( x265_bit_if_t *bit_if, uint32_t i_bits, int i_count  )
{
	x265_bit_counter_t *bit_counter = NULL ;

	bit_counter = (x265_bit_counter_t *)bit_if ;
	bit_counter->i_bit_counter += i_count ;
}

void bit_counter_reset_bits ( x265_bit_if_t *bit_if )
{
	x265_bit_counter_t *bit_counter = NULL ;

	bit_counter = (x265_bit_counter_t *)bit_if ;
	bit_counter->i_bit_counter = 0 ;

}

int bit_counter_get_number_of_written_bits ( x265_bit_if_t *bit_if )
{
	x265_bit_counter_t *bit_counter = NULL ;

	bit_counter = (x265_bit_counter_t *)bit_if ;

	return bit_counter->i_bit_counter ;
}

void bit_counter_store (x265_bit_if_t *bit_if, x265_bit_if_t *dst_bit_if)
{
	x265_bit_counter_t *bit_counter = NULL ;
	x265_bit_counter_t *dst_bit_counter = NULL ;

	bit_counter = (x265_bit_counter_t *)bit_if ;
	dst_bit_counter = (x265_bit_counter_t *)dst_bit_if ;

	dst_bit_counter->i_bit_counter = bit_counter->i_bit_counter ;
}
