
#ifndef X265_BS_H
#define X265_BS_H

#define			BITSTREAM_MAX_SIZE				1000000

typedef struct bs_s
{
    uint8_t *p_start;
    uint8_t *p;
    uint8_t *p_end;

    uintptr_t cur_bits;
    int i_left;    /* i_count number of available bits */
    int i_bits_encoded; /* RD only */
} bs_t;


typedef struct
{
    uint8_t *(*nal_escape) ( uint8_t *dst, uint8_t *src, uint8_t *end );
} x265_bitstream_function_t;

typedef struct _x265_bit_if_t x265_bit_if_t ;

#define X265_BIT_IF_FUNC \
	void  (*bit_if_delete) ( x265_bit_if_t *bit_if ) ; \
	void (*write_alignOne) (x265_bit_if_t *bit_if) ; \
	void (*write_align_zero) (x265_bit_if_t *bit_if) ; \
	void (*store) (x265_bit_if_t *bit_if, x265_bit_if_t *dst_bit_if) ; \
	void (*write) ( x265_bit_if_t *bit_if, uint32_t i_bits, int i_count ) ; \
	void (*reset_bits) ( x265_bit_if_t *bit_if ) ; \
	int (*get_number_of_written_bits) ( x265_bit_if_t *bit_if ) ;

struct _x265_bit_if_t
{
	X265_BIT_IF_FUNC ;
};


typedef struct
{
	X265_BIT_IF_FUNC ;
	bs_t bs ;
	int i_bitstream ;
	uint8_t *p_bitstream ;

} x265_output_bitstream_t ;

typedef struct
{
	X265_BIT_IF_FUNC ;
	int i_bit_counter ;

} x265_bit_counter_t ;


void x265_bitstream_init( int cpu, x265_bitstream_function_t *pf );
x265_bit_if_t *x265_output_bitstream_new () ;
void x265_output_bitstream_delete ( x265_bit_if_t *bit_if ) ;
int x265_output_bitstream_init ( x265_bit_if_t *bit_if ) ;
void x265_output_bitstream_deinit ( x265_bit_if_t *bit_if ) ;
void x265_output_bitstream_clear ( x265_output_bitstream_t *output_bitstream ) ;
x265_bit_if_t *x265_bit_counter_new () ;
void x265_bit_counter_delete ( x265_bit_if_t *bit_if ) ;
int x265_bit_counter_init ( x265_bit_if_t *bit_if ) ;
void x265_bit_counter_deinit ( x265_bit_if_t *bit_if ) ;
void x265_output_bitstream_write_byte_alignment ( x265_output_bitstream_t *output_bitstream ) ;
void x265_output_bitstream_add_stream ( x265_output_bitstream_t *output_bitstream,
										x265_output_bitstream_t *output_bitstream_src ) ;
void x265_output_bitstream_print ( bs_t *s, const char *filename ) ;


static inline void bs_init( bs_t *s, void *p_data, int i_data )
{
    int offset = ((intptr_t)p_data & 3);
    s->p       = s->p_start = (uint8_t*)p_data - offset;
    s->p_end   = (uint8_t*)p_data + i_data;
    s->i_left  = (WORD_SIZE - offset)*8;
    s->cur_bits = endian_fix32( M32(s->p) );
    s->cur_bits >>= (4-offset)*8;
}
static inline int bs_pos( bs_t *s )
{
    return( 8 * (s->p - s->p_start) + (WORD_SIZE*8) - s->i_left );
}

/* Write the rest of cur_bits to the bitstream; results in a bitstream no longer 32-bit aligned. */
//	byte align
static inline void bs_flush( bs_t *s )
{
    M32( s->p ) = endian_fix32( s->cur_bits << (s->i_left&31) );
    //	(left>>3) = left-(left&0x07)
    //	s->p += WORD_SIZE - [ left-(left&0x07) ]
    s->p += WORD_SIZE - (s->i_left >> 3);
    s->i_left = WORD_SIZE*8;
}
/* The inverse of bs_flush: prepare the bitstream to be written to again. */
//	four byte align
static inline void bs_realign( bs_t *s )
{
    int offset = ((intptr_t)s->p & 3);
    if( offset )
    {
        s->p       = (uint8_t*)s->p - offset;
        s->i_left  = (WORD_SIZE - offset)*8;
        s->cur_bits = endian_fix32( M32(s->p) );
        s->cur_bits >>= (4-offset)*8;
    }
}

void bs_write( bs_t *s, int i_count, uint32_t i_bits ) ;

/* Special case to eliminate branch in normal bs_write. */
/* Golomb never writes an even-size code, so this is only used in slice headers. */
static inline void bs_write32( bs_t *s, uint32_t i_bits )
{
    bs_write( s, 16, i_bits >> 16 );
    bs_write( s, 16, i_bits );
}

static inline void bs_write1( bs_t *s, uint32_t i_bit )
{
    s->cur_bits <<= 1;
    s->cur_bits |= i_bit;
    s->i_left--;
    if( s->i_left == WORD_SIZE*8-32 )
    {
        M32( s->p ) = endian_fix32( s->cur_bits );
        s->p += 4;
        s->i_left = WORD_SIZE*8;
    }
}

static const uint8_t x264_ue_size_tab[256] =
{
     1, 1, 3, 3, 5, 5, 5, 5, 7, 7, 7, 7, 7, 7, 7, 7,
     9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9,
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
    11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,11,
    13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
    13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
    13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
    13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,13,
    15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
    15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,15,
};

static inline void bs_write_ue_big( bs_t *s, unsigned int val )
{
    int size = 0;
    int tmp = ++val;
    if( tmp >= 0x10000 )
    {
        size = 32;
        tmp >>= 16;
    }
    if( tmp >= 0x100 )
    {
        size += 16;
        tmp >>= 8;
    }
    size += x264_ue_size_tab[tmp];
    bs_write( s, size>>1, 0 );
    bs_write( s, (size>>1)+1, val );
}

/* Only works on values under 255. */
static inline void bs_write_ue( bs_t *s, int val )
{
    bs_write( s, x264_ue_size_tab[val+1], val+1 );
}


static inline void bs_write_se( bs_t *s, int val )
{
    int size = 0;
    /* Faster than (val <= 0 ? -val*2+1 : val*2) */
    /* 4 instructions on x86, 3 on ARM */
    int tmp = 1 - val*2;
    if( tmp < 0 ) tmp = val*2;
    val = tmp;

    if( tmp >= 0x100 )
    {
        size = 16;
        tmp >>= 8;
    }
    size += x264_ue_size_tab[tmp];
    bs_write( s, size, val );
}

void bs_add_stream ( bs_t *s, bs_t *bs_src ) ;
int bs_get_number_of_written_bits ( bs_t *s ) ;

#endif


