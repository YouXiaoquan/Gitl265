


#include "common/common.h"

const uint8_t x265_cabac_range_lps[64][4] =
{
	{ 128, 176, 208, 240},
	{ 128, 167, 197, 227},
	{ 128, 158, 187, 216},
	{ 123, 150, 178, 205},
	{ 116, 142, 169, 195},
	{ 111, 135, 160, 185},
	{ 105, 128, 152, 175},
	{ 100, 122, 144, 166},
	{  95, 116, 137, 158},
	{  90, 110, 130, 150},
	{  85, 104, 123, 142},
	{  81,  99, 117, 135},
	{  77,  94, 111, 128},
	{  73,  89, 105, 122},
	{  69,  85, 100, 116},
	{  66,  80,  95, 110},
	{  62,  76,  90, 104},
	{  59,  72,  86,  99},
	{  56,  69,  81,  94},
	{  53,  65,  77,  89},
	{  51,  62,  73,  85},
	{  48,  59,  69,  80},
	{  46,  56,  66,  76},
	{  43,  53,  63,  72},
	{  41,  50,  59,  69},
	{  39,  48,  56,  65},
	{  37,  45,  54,  62},
	{  35,  43,  51,  59},
	{  33,  41,  48,  56},
	{  32,  39,  46,  53},
	{  30,  37,  43,  50},
	{  29,  35,  41,  48},
	{  27,  33,  39,  45},
	{  26,  31,  37,  43},
	{  24,  30,  35,  41},
	{  23,  28,  33,  39},
	{  22,  27,  32,  37},
	{  21,  26,  30,  35},
	{  20,  24,  29,  33},
	{  19,  23,  27,  31},
	{  18,  22,  26,  30},
	{  17,  21,  25,  28},
	{  16,  20,  23,  27},
	{  15,  19,  22,  25},
	{  14,  18,  21,  24},
	{  14,  17,  20,  23},
	{  13,  16,  19,  22},
	{  12,  15,  18,  21},
	{  12,  14,  17,  20},
	{  11,  14,  16,  19},
	{  11,  13,  15,  18},
	{  10,  12,  15,  17},
	{  10,  12,  14,  16},
	{   9,  11,  13,  15},
	{   9,  11,  12,  14},
	{   8,  10,  12,  14},
	{   8,   9,  11,  13},
	{   7,   9,  11,  12},
	{   7,   9,  10,  12},
	{   7,   8,  10,  11},
	{   6,   8,   9,  11},
	{   6,   7,   9,  10},
	{   6,   7,   8,   9},
	{   2,   2,   2,   2}
};

const uint8_t x265_cabac_renorm_shift[64] =
{
    6,5,4,4,3,3,3,3,2,2,2,2,2,2,2,2,
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
};


x265_enc_bin_cabac_t* enc_bin_cabac_get_enc_bin_cabac ( x265_enc_bin_if_t *enc_bin_if )
{
	x265_enc_bin_cabac_t *enc_bin_cabac = NULL ;

	enc_bin_cabac = (x265_enc_bin_cabac_t*) enc_bin_if ;
	return enc_bin_cabac ;
}

void x265_enc_bin_cabac_set_bins_coded ( x265_enc_bin_cabac_t *enc_bin_cabac, uint32_t i_val )
{
	enc_bin_cabac->i_bins_coded = i_val ;
}

uint32_t x265_enc_bin_cabac_get_bins_coded ( x265_enc_bin_cabac_t *enc_bin_cabac )
{
	return enc_bin_cabac->i_bins_coded ;
}

void x265_enc_bin_cabac_set_bin_counting_enable_flag ( x265_enc_bin_cabac_t *enc_bin_cabac, int32_t b_flag )
{
	enc_bin_cabac->i_bin_count_increment = b_flag ? 1 : 0 ;
}

int32_t x265_enc_bin_cabac_get_bin_counting_enable_flag ( x265_enc_bin_cabac_t *enc_bin_cabac )
{
	return enc_bin_cabac->i_bin_count_increment ;
}


x265_enc_bin_if_t *x265_enc_bin_cabac_new ()
{
	x265_enc_bin_if_t *enc_bin_if = NULL ;

	CHECKED_MALLOCZERO( enc_bin_if, sizeof(x265_enc_bin_cabac_t) );
	if ( x265_enc_bin_cabac_init(enc_bin_if) )
	{
		goto fail ;
	}

	return enc_bin_if ;
fail:
	x265_enc_bin_cabac_delete ( enc_bin_if ) ;
	return NULL ;
}


void x265_enc_bin_cabac_delete ( x265_enc_bin_if_t *enc_bin_if )
{
	if ( enc_bin_if )
	{
		x265_enc_bin_cabac_deinit ( enc_bin_if ) ;
		x265_free ( enc_bin_if ) ;
	}
}

int x265_enc_bin_cabac_init ( x265_enc_bin_if_t *enc_bin_if )
{
	x265_enc_bin_cabac_t *enc_bin_cabac = NULL ;

	x265_enc_bin_if_init ( enc_bin_if ) ;
	enc_bin_cabac = (x265_enc_bin_cabac_t*) enc_bin_if ;
	CHECKED_MALLOCZERO( enc_bin_cabac->state, X265_CABAC_STATE_NUMBER * sizeof(uint8_t) );
	CHECKED_MALLOCZERO( enc_bin_cabac->bins_coded, X265_CABAC_STATE_NUMBER * sizeof(uint32_t) );

	enc_bin_if->enc_bin_if_delete = x265_enc_bin_cabac_delete ;
	enc_bin_if->init = enc_bin_cabac_init ;
	enc_bin_if->uninit = enc_bin_cabac_uninit ;
	enc_bin_if->start = enc_bin_cabac_start ;
	enc_bin_if->finish = enc_bin_cabac_finish ;
	enc_bin_if->copy_state = enc_bin_cabac_copy_state ;
	enc_bin_if->flush = enc_bin_cabac_flush ;
	enc_bin_if->reset_bac = enc_bin_cabac_reset_bac ;
	enc_bin_if->encode_pcm_align_bits = enc_bin_cabac_encode_pcm_align_bits ;
	enc_bin_if->x_write_pcm_code = enc_bin_cabac_x_write_pcm_code ;
	enc_bin_if->reset_bits = enc_bin_cabac_reset_bits ;
	enc_bin_if->clear_bits = enc_bin_cabac_clear_bits ;
	enc_bin_if->get_num_written_bits = enc_bin_cabac_get_num_written_bits ;
	enc_bin_if->encode_bin = enc_bin_cabac_encode_bin ;
	enc_bin_if->encode_bin_ep = enc_bin_cabac_encode_bin_ep ;
	enc_bin_if->encode_bins_ep = enc_bin_cabac_encode_bins_ep ;
	enc_bin_if->encode_bin_trm = enc_bin_cabac_encode_bin_trm ;
	enc_bin_if->get_enc_bin_cabac = enc_bin_cabac_get_enc_bin_cabac ;
	enc_bin_if->get_entropy_bits = enc_bin_cabac_get_entropy_bits ;
	enc_bin_if->reset_entropy = enc_bin_cabac_reset_entropy ;
	enc_bin_if->print_cabac_state = enc_bin_cabac_print_cabac_state ;
	enc_bin_if->determine_cabac_init_idx = enc_bin_cabac_determine_cabac_init_idx ;
	enc_bin_if->update_context_table = enc_bin_cabac_update_context_table ;
	enc_bin_if->load_intra_dir_mode_luma = enc_bin_cabac_load_intra_dir_mode_luma ;
	enc_bin_if->x_copy_contexts_from = enc_bin_cabac_x_copy_contexts_from ;


	return 0 ;
fail:
	x265_enc_bin_cabac_deinit ( (void*) enc_bin_if ) ;
	return -1 ;
}

void x265_enc_bin_cabac_deinit ( x265_enc_bin_if_t *enc_bin_if )
{
	x265_enc_bin_cabac_t *enc_bin_cabac = NULL ;

	enc_bin_cabac = (x265_enc_bin_cabac_t *)enc_bin_if ;

	x265_free ( enc_bin_cabac->bins_coded ) ;
	x265_free ( enc_bin_cabac->state ) ;
}


void enc_bin_cabac_init ( x265_enc_bin_if_t *enc_bin_if, x265_bit_if_t *bit_if )
{
	x265_enc_bin_cabac_t *enc_bin_cabac = NULL ;

	enc_bin_cabac = (x265_enc_bin_cabac_t*) enc_bin_if ;
	enc_bin_cabac->bit_if = bit_if ;
}

void enc_bin_cabac_uninit ( x265_enc_bin_if_t *enc_bin_if )
{
	x265_enc_bin_cabac_t *enc_bin_cabac = NULL ;

	enc_bin_cabac = (x265_enc_bin_cabac_t*) enc_bin_if ;
	enc_bin_cabac->bit_if = NULL ;
}


void enc_bin_cabac_start ( x265_enc_bin_if_t *enc_bin_if )
{
	x265_enc_bin_cabac_t *enc_bin_cabac = NULL ;

	enc_bin_cabac = (x265_enc_bin_cabac_t*) enc_bin_if ;
	enc_bin_cabac->i_low = 0 ;
	enc_bin_cabac->i_range = 510 ;
	enc_bin_cabac->i_queue = -9 ;
	enc_bin_cabac->i_num_buffered_bytes = 0 ;
	enc_bin_cabac->i_buffered_byte = 0xFF ;
}


void enc_bin_cabac_finish ( x265_enc_bin_if_t *enc_bin_if )
{
	int out = 0 ;
    int carry = 0 ;
	x265_enc_bin_cabac_t *enc_bin_cabac = NULL ;

	enc_bin_cabac = (x265_enc_bin_cabac_t*) enc_bin_if ;
	out = enc_bin_cabac->i_low >> (enc_bin_cabac->i_queue+10);
    carry = out >> 8;
    if ( carry )
    {
    	enc_bin_cabac->bit_if->write ( enc_bin_cabac->bit_if,
    										enc_bin_cabac->i_buffered_byte + 1, 8 ) ;
    	while ( enc_bin_cabac->i_num_buffered_bytes > 1 )
    	{
        	enc_bin_cabac->bit_if->write ( enc_bin_cabac->bit_if, 0, 8 ) ;
    		-- enc_bin_cabac->i_num_buffered_bytes ;
    	}
    	enc_bin_cabac->i_low &= (1 << (enc_bin_cabac->i_queue + 18)) - 1;
    }
    else
    {
    	if ( enc_bin_cabac->i_num_buffered_bytes > 0 )
    	{
    		enc_bin_cabac->bit_if->write ( enc_bin_cabac->bit_if,
    											enc_bin_cabac->i_buffered_byte, 8 ) ;
    	}
    	while ( enc_bin_cabac->i_num_buffered_bytes > 1 )
    	{
        	enc_bin_cabac->bit_if->write ( enc_bin_cabac->bit_if, 0xFF, 8 ) ;
    		-- enc_bin_cabac->i_num_buffered_bytes ;
    	}
    }
	enc_bin_cabac->bit_if->write ( enc_bin_cabac->bit_if,
										enc_bin_cabac->i_low >> 8,
										(enc_bin_cabac->i_queue+10) ) ;

}

void enc_bin_cabac_flush ( x265_enc_bin_if_t *enc_bin_if )
{
	x265_enc_bin_cabac_t *enc_bin_cabac = NULL ;

	enc_bin_cabac = (x265_enc_bin_cabac_t*) enc_bin_if ;

	enc_bin_cabac_encode_bin_trm ( enc_bin_if, 1 ) ;
	enc_bin_cabac_finish ( enc_bin_if ) ;
	enc_bin_cabac->bit_if->write ( enc_bin_cabac->bit_if, 1, 1) ;
	enc_bin_cabac->bit_if->write_align_zero ( enc_bin_cabac->bit_if ) ;
	enc_bin_cabac_start ( enc_bin_if ) ;
}


void enc_bin_cabac_reset_bac ( x265_enc_bin_if_t *enc_bin_if )
{
	enc_bin_cabac_start ( enc_bin_if ) ;
}


void enc_bin_cabac_encode_pcm_align_bits ( x265_enc_bin_if_t *enc_bin_if )
{
	x265_enc_bin_cabac_t *enc_bin_cabac = NULL ;

	enc_bin_cabac = (x265_enc_bin_cabac_t*) enc_bin_if ;

	enc_bin_cabac->bit_if->write ( enc_bin_cabac->bit_if, 1, 1) ;
	enc_bin_cabac->bit_if->write_align_zero ( enc_bin_cabac->bit_if ) ;
}

void enc_bin_cabac_x_write_pcm_code ( x265_enc_bin_if_t *enc_bin_if, uint32_t i_code, uint32_t i_length )
{
	x265_enc_bin_cabac_t *enc_bin_cabac = NULL ;

	enc_bin_cabac = (x265_enc_bin_cabac_t*) enc_bin_if ;

	enc_bin_cabac->bit_if->write ( enc_bin_cabac->bit_if, i_code, i_length ) ;
}

void enc_bin_cabac_copy_state ( x265_enc_bin_if_t *src_enc_bin_if,
								x265_enc_bin_if_t *dst_enc_bin_if )
{

	x265_enc_bin_cabac_t *src_enc_bin_cabac = NULL ;
	x265_enc_bin_cabac_t *dst_enc_bin_cabac = NULL ;

	src_enc_bin_cabac = (x265_enc_bin_cabac_t*) src_enc_bin_if ;
	dst_enc_bin_cabac = (x265_enc_bin_cabac_t*) dst_enc_bin_if ;


	dst_enc_bin_cabac->i_low = src_enc_bin_cabac->i_low ;
	dst_enc_bin_cabac->i_range = src_enc_bin_cabac->i_range ;
	dst_enc_bin_cabac->i_queue = src_enc_bin_cabac->i_queue ;
	dst_enc_bin_cabac->i_num_buffered_bytes = src_enc_bin_cabac->i_num_buffered_bytes ;
	dst_enc_bin_cabac->i_buffered_byte = src_enc_bin_cabac->i_buffered_byte ;
	dst_enc_bin_cabac->i_frac_bits = src_enc_bin_cabac->i_frac_bits ;
	memcpy( dst_enc_bin_cabac->state, src_enc_bin_cabac->state,
			X265_CABAC_STATE_NUMBER * sizeof( uint8_t ) );
	memcpy( dst_enc_bin_cabac->bins_coded, src_enc_bin_cabac->bins_coded,
			X265_CABAC_STATE_NUMBER * sizeof( uint32_t ) );
}



void enc_bin_cabac_reset_bits ( x265_enc_bin_if_t *enc_bin_if )
{
	x265_enc_bin_cabac_t *enc_bin_cabac = NULL ;

	enc_bin_cabac = (x265_enc_bin_cabac_t*) enc_bin_if ;

	enc_bin_cabac->i_low = 0 ;
	enc_bin_cabac->i_queue = -9 ;
	enc_bin_cabac->i_num_buffered_bytes = 0 ;
	enc_bin_cabac->i_buffered_byte = 0xFF ;
	enc_bin_cabac->i_frac_bits &= 32767 ;
	if ( enc_bin_cabac->i_bin_count_increment )
	{
		enc_bin_cabac->i_bins_coded = 0 ;
	}
}

void enc_bin_cabac_clear_bits ( x265_enc_bin_if_t *enc_bin_if )
{
	x265_enc_bin_cabac_t *enc_bin_cabac = NULL ;

	enc_bin_cabac = (x265_enc_bin_cabac_t*) enc_bin_if ;

	enc_bin_cabac->i_low = 0 ;
	enc_bin_cabac->i_queue = -9 ;
	enc_bin_cabac->i_num_buffered_bytes = 0 ;
	enc_bin_cabac->i_buffered_byte = 0xFF ;
	enc_bin_cabac->i_frac_bits = 0 ;
	if ( enc_bin_cabac->i_bin_count_increment )
	{
		enc_bin_cabac->i_bins_coded = 0 ;
	}
}

uint32_t enc_bin_cabac_get_num_written_bits ( x265_enc_bin_if_t *enc_bin_if )
{
	x265_enc_bin_cabac_t *enc_bin_cabac = NULL ;

	enc_bin_cabac = (x265_enc_bin_cabac_t*) enc_bin_if ;

	return enc_bin_cabac->bit_if->get_number_of_written_bits (enc_bin_cabac->bit_if)
			+ 8 * enc_bin_cabac->i_num_buffered_bytes + enc_bin_cabac->i_queue + 9 ;
}

void enc_bin_cabac_encode_bin ( x265_enc_bin_if_t *enc_bin_if, int32_t b, int32_t i_ctx )
{
    int i_state = 0 ;
    int i_range_lps = 0 ;
	x265_enc_bin_cabac_t *enc_bin_cabac = NULL ;

	enc_bin_cabac = (x265_enc_bin_cabac_t*) enc_bin_if ;

	enc_bin_cabac->i_bins_coded += enc_bin_cabac->i_bin_count_increment ;
	enc_bin_cabac->bins_coded[i_ctx] = 1 ;
    i_state = enc_bin_cabac->state[i_ctx];
    i_range_lps = x265_cabac_range_lps[i_state>>1][(enc_bin_cabac->i_range>>6)&0x03];
    enc_bin_cabac->i_range -= i_range_lps;
    if( b != (i_state & 1) )
    {
        enc_bin_cabac->i_low += enc_bin_cabac->i_range;
        enc_bin_cabac->i_range = i_range_lps;
    }
    enc_bin_cabac->state[i_ctx] = x265_cabac_transition[i_state][b];
    x265_cabac_encode_renorm( enc_bin_cabac );
}

void enc_bin_cabac_encode_bin_ep ( x265_enc_bin_if_t *enc_bin_if, int32_t b )
{
	x265_enc_bin_cabac_t *enc_bin_cabac = NULL ;

	enc_bin_cabac = (x265_enc_bin_cabac_t*) enc_bin_if ;

	enc_bin_cabac->i_bins_coded += enc_bin_cabac->i_bin_count_increment ;
    enc_bin_cabac->i_low <<= 1;
    enc_bin_cabac->i_low += ( -b & enc_bin_cabac->i_range ) ;
    enc_bin_cabac->i_queue += 1;
    x265_cabac_putbyte( enc_bin_cabac );
}

void enc_bin_cabac_encode_bins_ep ( x265_enc_bin_if_t *enc_bin_if, int32_t i_bin_values, uint32_t i_num_bins )
{
    int i = 0, k = 0 ;
	x265_enc_bin_cabac_t *enc_bin_cabac = NULL ;

	if ( i_num_bins > 0 )
	{
		enc_bin_cabac = (x265_enc_bin_cabac_t*) enc_bin_if ;
		enc_bin_cabac->i_bins_coded += (i_num_bins & (-enc_bin_cabac->i_bin_count_increment)) ;
		k = i_num_bins ;
		i = ((k-1)&7)+1;
		do {
			k -= i;
			enc_bin_cabac->i_low <<= i;
			enc_bin_cabac->i_low += ((i_bin_values>>k)&0xff) * enc_bin_cabac->i_range;
			enc_bin_cabac->i_queue += i;
			x265_cabac_putbyte( enc_bin_cabac );
			i = 8;
		} while( k > 0 );
	}
}

/**
 * \brief Encode terminating bin
 *
 * \param binValue bin value
 */
void enc_bin_cabac_encode_bin_trm ( x265_enc_bin_if_t *enc_bin_if, int32_t b )
{
	x265_enc_bin_cabac_t *enc_bin_cabac = NULL ;

	enc_bin_cabac = (x265_enc_bin_cabac_t*) enc_bin_if ;
	enc_bin_cabac->i_bins_coded += enc_bin_cabac->i_bin_count_increment ;
	enc_bin_cabac->i_range -= 2 ;
	if ( b )
	{
		enc_bin_cabac->i_low += enc_bin_cabac->i_range ;
		enc_bin_cabac->i_low <<= 7 ;
		enc_bin_cabac->i_range = ( 2 << 7 ) ;
		enc_bin_cabac->i_queue += 7 ;
	}
	else if ( enc_bin_cabac->i_range >= 256 )
	{
		return ;
	}
	else
	{
		enc_bin_cabac->i_low <<= 1 ;
		enc_bin_cabac->i_range <<= 1 ;
		enc_bin_cabac->i_queue ++ ;
	}
	x265_cabac_putbyte ( enc_bin_cabac ) ;
}

void enc_bin_cabac_print_cabac_state ( x265_enc_bin_if_t *enc_bin_if )
{
	int32_t loop = 0 ;
	FILE *file = NULL ;
	static int32_t b_first_print_cabac_state = 1 ;
	x265_enc_bin_cabac_t *enc_bin_cabac = NULL ;

	enc_bin_cabac = (x265_enc_bin_cabac_t*) enc_bin_if ;

	if ( b_first_print_cabac_state )
	{
		file = fopen ( "CabacState2", "wb" ) ;
		b_first_print_cabac_state = 0 ;
	}
	else
	{
		file = fopen ( "CabacState2", "r+b" ) ;
		fseek ( file, 0, SEEK_END ) ;
	}

	for ( loop = 0 ; loop < X265_CABAC_STATE_NUMBER ; ++ loop )
	{
		fprintf ( file, "%d\n", enc_bin_cabac->state[loop] ) ;
	}

	fclose (file) ;
}

void x265_cabac_encode_renorm( x265_enc_bin_cabac_t *enc_bin_cabac )
{
    int shift = x265_cabac_renorm_shift[enc_bin_cabac->i_range>>3];
    enc_bin_cabac->i_range <<= shift;
    enc_bin_cabac->i_low   <<= shift;
    enc_bin_cabac->i_queue  += shift;
    x265_cabac_putbyte( enc_bin_cabac );
}

void x265_cabac_putbyte( x265_enc_bin_cabac_t *enc_bin_cabac )
{
    if( enc_bin_cabac->i_queue >= 0 )
    {
        int out = enc_bin_cabac->i_low >> (enc_bin_cabac->i_queue+10);
        enc_bin_cabac->i_low &= (0x400<<enc_bin_cabac->i_queue)-1;
        enc_bin_cabac->i_queue -= 8;

        if( (out & 0xff) == 0xff )
            enc_bin_cabac->i_num_buffered_bytes++;
        else
        {
        	if ( enc_bin_cabac->i_num_buffered_bytes > 0 )
        	{
				int carry = out >> 8;
				int bytes = enc_bin_cabac->i_buffered_byte + carry ;

				enc_bin_cabac->i_buffered_byte = ( out & 0xFF ) ;
				enc_bin_cabac->bit_if->write ( enc_bin_cabac->bit_if,
													bytes,
													8 ) ;

				bytes = ( 0xFF + carry ) & 0xFF ;
				// this can't modify before the beginning of the stream because
				// that would correspond to a probability > 1.
				// it will write before the beginning of the stream, which is ok
				// because a slice header always comes before cabac data.
				// this can't carry beyond the one byte, because any 0xff bytes
				// are in bytes_outstanding and thus not written yet.
				while( enc_bin_cabac->i_num_buffered_bytes > 1 )
				{
					enc_bin_cabac->bit_if->write ( enc_bin_cabac->bit_if,
														bytes,
														8 ) ;
					--enc_bin_cabac->i_num_buffered_bytes;
				}
        	}
        	else
        	{
        		enc_bin_cabac->i_buffered_byte = out ;
        		enc_bin_cabac->i_num_buffered_bytes = 1 ;
        	}
        }
    }
}

int32_t enc_bin_cabac_get_entropy_bits ( x265_enc_bin_if_t *enc_bin_if, int32_t b, int32_t i_ctx )
{
	x265_enc_bin_cabac_t *enc_bin_cabac = NULL ;

	enc_bin_cabac = (x265_enc_bin_cabac_t*) enc_bin_if ;
	return x265_entropy_bits[enc_bin_cabac->state[i_ctx] ^ b];
}

void enc_bin_cabac_reset_entropy ( x265_t *h,
									x265_enc_bin_if_t *enc_bin_if,
									enum slice_type_e i_slice_type,
									int32_t i_slice_qp )
{
	int32_t loop = 0 ;
	int32_t i_qp = 0 ;
	int32_t i_slope = 0 ;
	int32_t i_offset = 0 ;
	int32_t i_init_state = 0 ;
	int32_t b_mp_state = 0 ;

	uint32_t i_enc_cabac_table_idx = 0 ;
	const uint8_t *cabac_context_init = NULL ;
	x265_enc_bin_cabac_t *enc_bin_cabac = NULL ;

	enc_bin_cabac = (x265_enc_bin_cabac_t*) enc_bin_if ;

	i_enc_cabac_table_idx = h->pps[0].i_enc_cabac_table_idx ;
	if ( I_SLICE != i_slice_type
		&& (B_SLICE == i_enc_cabac_table_idx || P_SLICE == i_enc_cabac_table_idx )
		&& h->pps[0].b_cabac_init_present_flag )
	{
		i_slice_type = (enum slice_type_e) i_enc_cabac_table_idx ;
	}

	if ( I_SLICE == i_slice_type )
	{
		cabac_context_init = x265_cabac_context_init_I ;
	}
	else if ( P_SLICE == i_slice_type )
	{
		cabac_context_init = x265_cabac_context_init_PB1 ;
	}
	else if ( B_SLICE == i_slice_type )
	{
		cabac_context_init = x265_cabac_context_init_PB2 ;
	}

	for ( loop = 0 ; loop < X265_CABAC_STATE_NUMBER ; ++ loop )
	{
		i_qp = x265_clip3_int32 ( i_slice_qp, 0, 51 ) ;
		i_slope = ( cabac_context_init[loop] >> 4 ) * 5 - 45 ;
		i_offset = ( ( cabac_context_init[loop] & 15 ) << 3 ) - 16 ;
		i_init_state = X265_MIN ( X265_MAX ( 1, ( ( ( i_slope * i_qp ) >> 4 ) + i_offset ) ), 126 );
		b_mp_state = ( i_init_state >= 64 ) ;
		enc_bin_cabac->state[loop] = ((b_mp_state
											? (i_init_state - 64)
											: (63 - i_init_state)) <<1) + b_mp_state ;
		enc_bin_cabac->bins_coded[loop] = 0 ;
	}
}

void enc_bin_cabac_determine_cabac_init_idx ( x265_t *h,
											x265_enc_bin_if_t *enc_bin_if,
											enum slice_type_e i_slice_type,
											int32_t i_slice_qp )
{
	uint32_t i_idx = 0 ;
	uint32_t loop = 0 ;
	uint32_t i_best_cost = 0 ;
	uint32_t i_cur_cost = 0 ;
	const uint8_t *cabac_context_inits[2] ;
	enum slice_type_e i_best_slice_type = 0 ;
	enum slice_type_e i_cur_slice_type = 0 ;
	enum slice_type_e slice_type_choices[2] ;

	x265_enc_bin_cabac_t *enc_bin_cabac = NULL ;

	enc_bin_cabac = (x265_enc_bin_cabac_t*) enc_bin_if ;
	cabac_context_inits[0] = x265_cabac_context_init_PB2 ;
	cabac_context_inits[1] = x265_cabac_context_init_PB1 ;

	if ( I_SLICE != i_slice_type )
	{
		slice_type_choices[0] = B_SLICE ;
		slice_type_choices[1] = P_SLICE ;
		i_best_cost = X265_MAX_UINT ;
		i_best_slice_type = slice_type_choices[0] ;

		for ( i_idx = 0 ; i_idx < 2 ; ++ i_idx )
		{
			i_cur_cost = 0 ;
			i_cur_slice_type = slice_type_choices[i_idx] ;
			for ( loop = 0 ; loop < X265_CABAC_STATE_NUMBER ; ++ loop )
			{
				i_cur_cost += enc_bin_cabac_calc_cost ( enc_bin_cabac, i_slice_qp,
														cabac_context_inits[i_idx][loop], loop ) ;
			}
			if ( i_cur_cost < i_best_cost )
			{
				i_best_cost = i_cur_cost ;
				i_best_slice_type = i_cur_slice_type ;
			}
		}
		h->pps[0].i_enc_cabac_table_idx = i_best_slice_type ;
	}
	else
	{
		h->pps[0].i_enc_cabac_table_idx = I_SLICE ;
	}
}

void enc_bin_cabac_update_context_table( x265_enc_bin_if_t *enc_bin_if,
										enum slice_type_e i_slice_type,
										int32_t i_slice_qp )
{
	int32_t loop = 0 ;
	int32_t i_qp = 0 ;
	int32_t i_slope = 0 ;
	int32_t i_offset = 0 ;
	int32_t i_init_state = 0 ;
	int32_t b_mp_state = 0 ;

	const uint8_t *cabac_context_init = NULL ;
	x265_enc_bin_cabac_t *enc_bin_cabac = NULL ;

	enc_bin_cabac = (x265_enc_bin_cabac_t*) enc_bin_if ;


	if ( I_SLICE == i_slice_type )
	{
		cabac_context_init = x265_cabac_context_init_I ;
	}
	else if ( P_SLICE == i_slice_type )
	{
		cabac_context_init = x265_cabac_context_init_PB1 ;
	}
	else if ( B_SLICE == i_slice_type )
	{
		cabac_context_init = x265_cabac_context_init_PB2 ;
	}

	for ( loop = 0 ; loop < X265_CABAC_STATE_NUMBER ; ++ loop )
	{
		i_qp = x265_clip3_int32 ( i_slice_qp, 0, 51 ) ;
		i_slope = ( cabac_context_init[loop] >> 4 ) * 5 - 45 ;
		i_offset = ( ( cabac_context_init[loop] & 15 ) << 3 ) - 16 ;
		i_init_state = X265_MIN ( X265_MAX ( 1, ( ( ( i_slope * i_qp ) >> 4 ) + i_offset ) ), 126 );
		b_mp_state = ( i_init_state >= 64 ) ;
		enc_bin_cabac->state[loop] = ((b_mp_state ? (i_init_state - 64)
										: (63 - i_init_state)) <<1) + b_mp_state ;
		enc_bin_cabac->bins_coded[loop] = 0 ;
	}
}

void enc_bin_cabac_load_intra_dir_mode_luma ( x265_enc_bin_if_t *enc_bin_if,
											x265_enc_bin_if_t *src_enc_bin_if )
{

	x265_enc_bin_cabac_t *src_enc_bin_cabac = NULL ;
	x265_enc_bin_cabac_t *dst_enc_bin_cabac = NULL ;

	src_enc_bin_cabac = (x265_enc_bin_cabac_t*) src_enc_bin_if ;
	dst_enc_bin_cabac = (x265_enc_bin_cabac_t*) enc_bin_if ;

	dst_enc_bin_cabac->i_low = src_enc_bin_cabac->i_low ;
	dst_enc_bin_cabac->i_range = src_enc_bin_cabac->i_range ;
	dst_enc_bin_cabac->i_queue = src_enc_bin_cabac->i_queue ;
	dst_enc_bin_cabac->i_num_buffered_bytes = src_enc_bin_cabac->i_num_buffered_bytes ;
	dst_enc_bin_cabac->i_buffered_byte = src_enc_bin_cabac->i_buffered_byte ;
	dst_enc_bin_cabac->i_frac_bits = src_enc_bin_cabac->i_frac_bits ;

	dst_enc_bin_cabac->state[X265_CU_INTRA_PRED_START_IDX] = src_enc_bin_cabac->state[X265_CU_INTRA_PRED_START_IDX] ;
	dst_enc_bin_cabac->bins_coded[X265_CU_INTRA_PRED_START_IDX] = src_enc_bin_cabac->bins_coded[X265_CU_INTRA_PRED_START_IDX] ;
}

void enc_bin_cabac_x_copy_contexts_from ( x265_enc_bin_if_t *enc_bin_if,
											x265_enc_bin_if_t *src_enc_bin_if )
{

	x265_enc_bin_cabac_t *src_enc_bin_cabac = NULL ;
	x265_enc_bin_cabac_t *dst_enc_bin_cabac = NULL ;

	src_enc_bin_cabac = (x265_enc_bin_cabac_t*) src_enc_bin_if ;
	dst_enc_bin_cabac = (x265_enc_bin_cabac_t*) enc_bin_if ;

	memcpy( dst_enc_bin_cabac->state, src_enc_bin_cabac->state,
			X265_CABAC_STATE_NUMBER * sizeof( uint8_t ) );
	memcpy( dst_enc_bin_cabac->bins_coded, src_enc_bin_cabac->bins_coded,
			X265_CABAC_STATE_NUMBER * sizeof( uint32_t ) );
}

uint32_t enc_bin_cabac_calc_cost ( x265_enc_bin_cabac_t *enc_bin_cabac,
					int32_t i_slice_qp, uint8_t i_init_value, int32_t i_ctx )
{
	uint8_t i_state = 0 ;
	uint32_t i_cost = 0;
	double f_prob_lps = 0.0 ;
	double f_prob0 = 0.0, f_prob1 = 0.0 ;
	static double state_to_prob_lps[] = {0.50000000, 0.47460857, 0.45050660, 0.42762859, 0.40591239, 0.38529900, 0.36573242, 0.34715948, 0.32952974, 0.31279528, 0.29691064, 0.28183267, 0.26752040, 0.25393496, 0.24103941, 0.22879875, 0.21717969, 0.20615069, 0.19568177, 0.18574449, 0.17631186, 0.16735824, 0.15885931, 0.15079198, 0.14313433, 0.13586556, 0.12896592, 0.12241667, 0.11620000, 0.11029903, 0.10469773, 0.09938088, 0.09433404, 0.08954349, 0.08499621, 0.08067986, 0.07658271, 0.07269362, 0.06900203, 0.06549791, 0.06217174, 0.05901448, 0.05601756, 0.05317283, 0.05047256, 0.04790942, 0.04547644, 0.04316702, 0.04097487, 0.03889405, 0.03691890, 0.03504406, 0.03326442, 0.03157516, 0.02997168, 0.02844963, 0.02700488, 0.02563349, 0.02433175, 0.02309612, 0.02192323, 0.02080991, 0.01975312, 0.01875000};

	i_state = x265_enc_bin_if_get_state ( i_slice_qp, i_init_value ) ;

	// Map the 64 CABAC states to their corresponding probability values

	f_prob_lps = state_to_prob_lps[(enc_bin_cabac->state[i_ctx] >> 1)] ;
	if ( 1 == ( enc_bin_cabac->state[i_ctx] & 1) )
	{
		f_prob0 = f_prob_lps ;
		f_prob1 = 1.0 - f_prob0 ;
	}
	else
	{
		f_prob1 = f_prob_lps ;
		f_prob0 = 1.0 - f_prob1 ;
	}

    if ( enc_bin_cabac->bins_coded[i_ctx] )
    {
    	i_cost += (uint32_t) (f_prob0 * x265_entropy_bits[i_state^0]
    	                      + f_prob1 * x265_entropy_bits[i_state^1] );
    }

    return i_cost ;
}

void x265_enc_bin_cabac_print_offset ()
{
	FILE *file = stderr ;

	// 20
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->init ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->uninit ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->start ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->finish ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->copy_state ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->flush ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->reset_bac ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->encode_pcm_align_bits ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->x_write_pcm_code ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->reset_bits ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->clear_bits ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->get_num_written_bits ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->encode_bin ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->encode_bin_ep ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->encode_bins_ep ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->encode_bin_trm ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->get_enc_bin_cabac ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->enc_bin_if_delete ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->reset_entropy ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->determine_cabac_init_idx ) ;

	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->bit_if ) ;

	// 10
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->i_bins_coded ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->i_bin_count_increment ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->state ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->bins_coded ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->i_low ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->i_range ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->i_queue ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->i_num_buffered_bytes ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->i_buffered_byte ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_enc_bin_cabac_t*)0)->i_frac_bits ) ;

	fprintf ( file, "%d\n", sizeof(x265_enc_bin_cabac_t) ) ;
}




