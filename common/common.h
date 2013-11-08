

#ifndef X265_COMMON_H
#define X265_COMMON_H

/****************************************************************************
 * Macros
 ****************************************************************************/
#define X265_MIN(a,b) ( (a)<(b) ? (a) : (b) )
#define X265_MAX(a,b) ( (a)>(b) ? (a) : (b) )
#define X265_MIN3(a,b,c) X265_MIN((a),X265_MIN((b),(c)))
#define X265_MAX3(a,b,c) X265_MAX((a),X265_MAX((b),(c)))
#define X265_MIN4(a,b,c,d) X265_MIN((a),X265_MIN3((b),(c),(d)))
#define X265_MAX4(a,b,c,d) X265_MAX((a),X265_MAX3((b),(c),(d)))
#define XCHG(type,a,b) do{ type t = a; a = b; b = t; } while(0)
#define FIX8(f) ((int)(f*(1<<8)+.5))
#define ALIGN(x,a) (((x)+((a)-1))&~((a)-1))

#define CHECKED_MALLOC( var, size )\
do {\
    var = x265_malloc( size );\
    if( !var )\
        goto fail;\
} while( 0 )
#define CHECKED_MALLOCZERO( var, size )\
do {\
    CHECKED_MALLOC( var, size );\
    memset( var, 0, size );\
} while( 0 )
#define NOCHECKED_MALLOCZERO( var, size )\
do {\
    var = x265_malloc( size );\
    memset( var, 0, size );\
} while( 0 )

/****************************************************************************
 * Includes
 ****************************************************************************/
#include "osdep.h"
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>



#ifdef SYS_WINDOWS
char *strndup ( const char *string, int i_string_length ) ;
size_t getline(char **lineptr, size_t *n, FILE *stream) ;
#endif

typedef union { uint16_t i; uint8_t  c[2]; } MAY_ALIAS x265_union16_t;
typedef union { uint32_t i; uint16_t b[2]; uint8_t  c[4]; } MAY_ALIAS x265_union32_t;
typedef union { uint64_t i; uint32_t a[2]; uint16_t b[4]; uint8_t c[8]; } MAY_ALIAS x265_union64_t;
typedef struct { uint64_t i[2]; } x265_uint128_t;
typedef union { x265_uint128_t i; uint64_t a[2]; uint32_t b[4]; uint16_t c[8]; uint8_t d[16]; } MAY_ALIAS x265_union128_t;
#define M16(src) (((x265_union16_t*)(src))->i)
#define M32(src) (((x265_union32_t*)(src))->i)
#define M64(src) (((x265_union64_t*)(src))->i)
#define M128(src) (((x265_union128_t*)(src))->i)
#define M128_ZERO ((x265_uint128_t){{0,0}})
#define CP16(dst,src) M16(dst) = M16(src)
#define CP32(dst,src) M32(dst) = M32(src)
#define CP64(dst,src) M64(dst) = M64(src)
#define CP128(dst,src) M128(dst) = M128(src)


typedef struct _x265_t x265_t ;
#if HIGH_BIT_DEPTH
typedef int16_t pixel ;
#else
typedef uint8_t pixel ;
#endif

typedef int16_t spixel ;
typedef int32_t x265_coeff_t ;     ///< transform coefficient

#include "common_def.h"
#include "type_def.h"
#include "encoder/ratecontrol.h"

typedef struct _x265_frame_t x265_frame_t ;

// log2(LCUSize)
#define X265_MAX_CU_DEPTH 7
// maximum allowable size of CU
#define X265_MAX_CU_SIZE (1<<(X265_MAX_CU_DEPTH))
#define X265_MIN_PU_SIZE 4
// maximum number of SPU in horizontal line
#define X265_MAX_NUM_SPU_W (X265_MAX_CU_SIZE/X265_MIN_PU_SIZE)



typedef struct _x265_base_data_cu_t x265_base_data_cu_t ;
typedef struct _x265_ndbf_block_info_t x265_ndbf_block_info_t ;
typedef struct _x265_data_cu_t x265_data_cu_t ;


#define	PIXEL_TWO_SIZE			0

#include "timer.h"
#include "cpu.h"
#include "simage.h"
#include "image.h"
#include "x265.h"
#include "bitstream.h"
#include "yuv.h"
#include "mv.h"
#include "pattern.h"
#include "set.h"
#include "scan.h"
#include "mc.h"
#include "list.h"
#include "slice.h"
#include "motion_info.h"
#include "base_data_cu.h"
#include "data_cu.h"
#include "tmvp_data_cu.h"
#include "pic_yuv.h"
#include "pic_sym.h"
#include "pic.h"
#include "frame.h"
#include "context_tables.h"
#include "rd_cost.h"
#include "tr_quant.h"
#include "loop_filter.h"
#include "interpolation_filter.h"
#include "weight_prediction.h"
#include "prediction.h"
#include "encoder/enc_top.h"



extern const uint8_t chroma_scale[58];

/* log */
void x265_log( x265_t *h, int i_level, const char *psz_fmt, ... );


spixel x265_clip_y( x265_t *h, spixel v ) ;
spixel x265_clip_c( x265_t *h, spixel v ) ;

static ALWAYS_INLINE spixel x265_clip3_pixel( spixel v, spixel i_min, spixel i_max )
{
    return ( (v < i_min) ? i_min : (v > i_max) ? i_max : v );
}

static ALWAYS_INLINE int16_t x265_clip3_int16( int16_t v, int16_t i_min, int16_t i_max )
{
    return ( (v < i_min) ? i_min : (v > i_max) ? i_max : v );
}

static ALWAYS_INLINE int32_t x265_clip3_int32( int32_t v, int32_t i_min, int32_t i_max )
{
    return ( (v < i_min) ? i_min : (v > i_max) ? i_max : v );
}

static ALWAYS_INLINE int64_t x265_clip3_int64( int64_t v, int64_t i_min, int64_t i_max )
{
    return ( (v < i_min) ? i_min : (v > i_max) ? i_max : v );
}

static ALWAYS_INLINE double x265_clip3_double( double v, double i_min, double i_max )
{
    return ( (v < i_min) ? i_min : (v > i_max) ? i_max : v );
}

/****************************************************************************
 * General functions
 ****************************************************************************/
/* x265_malloc : will do or emulate a memalign
 * you have to use x265_free for buffers allocated with x265_malloc */
void *x265_malloc( int );
void  x265_free( void * );

void x265_nal_start( x265_t *h, int i_type) ;
int x265_nal_check_buffer( x265_t *h ) ;
int x265_nal_end( x265_t *h ) ;
void x265_slice_header_write( x265_t *h, bs_t *s, x265_slice_t *slice ) ;
int32_t x265_encoder_frame_end( x265_t *h,
							x265_nal_t **pp_nal,
							int *pi_nal,
							x265_picture_t *pic_out ) ;
int x265_encoder_encapsulate_nals( x265_t *h, int start ) ;

struct _x265_t
{
    /* encoder parameters */
    x265_param_t    param;

    x265_vps_t vps[1] ;
    x265_sps_t sps[1] ;
    x265_pps_t pps[1] ;


    struct
    {
    	struct
    	{
    		int32_t i_total_depth ;			///< max. depth
    		int32_t i_num_partitions ;
    		int32_t i_max_cu_width ;
    		int32_t i_max_cu_height ;
    		int32_t i_min_cu_width ;
    		int32_t i_min_cu_height ;
    		int32_t i_num_part_in_width ;
    		int32_t i_num_part_in_height ;
    		int32_t i_width_in_cu ;
    		int32_t i_height_in_cu ;
    		int32_t i_num_cus_in_frame ;
    		int32_t i_width_in_cu_mul_two ;
    		int32_t i_add_cu_depth ;
    		int32_t i_bit_depth_y ;
    		int32_t i_bit_depth_c ;
    		int32_t i_pcm_bit_depth_luma ;
    		int32_t i_pcm_bit_depth_chroma ;

    		int32_t *cu_offset_y ;
    		int32_t *cu_offset_c ;
    		int32_t *bu_offset_y ;
    		int32_t *bu_offset_c ;
    	} pic ;

    } cu ;

    struct
	{
		int i_nal;
		int i_nals_allocated;
		x265_nal_t *nal;
		int i_bitstream;    /* size of p_bitstream */
		uint8_t *p_bitstream;   /* will hold data for all nal */
		bs_t bs;
	} out;

	struct
	{
		int8_t *convert_to_bit ;
	} global ;

    struct
	{
    	int32_t i_frame_buf_num ;
    	int32_t i_reference_and_current ;
		/* Frames to be encoded (whose types have been decided) */
		x265_frame_t **current;
		x265_frame_t **buffered;
		/* frames used for reference + sentinels */
		x265_frame_t **reference;
		/* Unused frames: 0 = fenc, 1 = fdec */
		x265_frame_t **unused[2];

	} frames;
    x265_frame_t *fenc ;
    x265_frame_t *fdec ;

	struct
	{
		int32_t i_gop_size ;
		int32_t i_gop_id ;
		int32_t i_gop_encoded ;
		int32_t i_poc_last ;
		int32_t i_num_frame_rcvd ;
		int32_t i_num_all_pic_coded ;
		int32_t i_frames_to_be_encoded ;
		int32_t i_num_substreams ;
		int32_t i_max_ref_pic_num ;

	} enc_top ;

	//!< _scaling_list class pointer
	x265_scaling_list_t scaling_list ;
	x265_tr_quant_t tr_quant;
	x265_loop_filter_t loop_filter;

	x265_enc_search_t enc_search ;
	x265_enc_entropy_t *pc_enc_entropy ;
	x265_enc_cavlc_t *pc_enc_cavlc ;
	x265_enc_sample_adaptive_offset_t enc_sao;
	x265_enc_entropy_t enc_entropy;

	x265_enc_cavlc_t enc_cavlc ;
	x265_enc_sbac_t enc_sbac ;

	x265_enc_bin_cabac_t enc_bin_cabac;
	x265_enc_slice_t enc_slice ;
	x265_enc_cu_t enc_cu;

	x265_bit_counter_t bit_counter;
	x265_rd_cost_t rd_cost;
	x265_enc_sbac_t rd_go_on_sbac_coder;

#if X265_FAST_BIT_EST
	x265_enc_bin_cabac_counter_t rd_go_on_bin_coder_cabac;
#else
	x265_enc_bin_cabac_t rd_go_on_bin_coder_cabac;
#endif

	x265_enc_preanalyzer_t enc_preanalyzer;
	x265_enc_rate_ctrl_t enc_rate_ctrl;

	x265_enc_sbac_t **pc_sbac_coders ;
	x265_enc_bin_cabac_t **pc_bin_coder_cabacs ;
	x265_enc_sbac_t **pc_rd_go_on_sbac_coders ;
	x265_enc_bin_cabac_t **pc_rd_go_on_bin_coders_cabac ;
	x265_bit_counter_t **pc_bit_counters ;
	x265_rd_cost_t **rd_costs ;
	x265_enc_sbac_t ****ppppc_rd_sbac_coders ;
	x265_enc_bin_cabac_t ****ppppc_bin_coders_cabac ;
	x265_enc_sbac_t ***pppc_rd_sbac_coder ;

#if X265_FAST_BIT_EST
	x265_enc_bin_cabac_counter_t ***pppc_bin_coder_cabac ;
#else
	x265_enc_bin_cabac_t ***pppc_bin_coder_cabac ;
#endif

	x265_enc_gop_t enc_gop ;

    x265_slice_t *slice ;
    x265_tile_t ** tile ;
    uint32_t *cu_order_map ;
    uint32_t *tile_idx_map ;
    uint32_t *inverse_cu_order_map ;

    x265_image_t *image_up_line_of_intra ;
    x265_image_t *image_bottom_line_of_intra ;
    x265_image_t *image_left_line_of_intra ;
    x265_image_t *image_intra ;
    x265_base_data_cu_t **neighbour_cus ;
    x265_data_cu_t *data_cu ;
    x265_sao_param_t * sao_param;

    x265_ref_pic_list_modification_t ref_pic_list_modification ;
    int i_ref[3];
    int32_t ref_idx_of_lc[2][X265_MAX_NUM_REF_LC];
    int32_t list_id_from_idx_of_lc[X265_MAX_NUM_REF_LC];
    int32_t ref_idx_from_idx_of_lc[X265_MAX_NUM_REF_LC];
    int32_t ref_idx_of_l1_from_ref_idx_of_l0[X265_MAX_NUM_REF_LC];
    int32_t ref_idx_of_l0_from_ref_idx_of_l1[X265_MAX_NUM_REF_LC];
    int32_t b_ref_pic_list_modification_flag_lc ;
    int32_t b_ref_pic_list_combination_flag ;
    x265_frame_t **fref[2];


    uint8_t *nal_buffer ;
    int nal_buffer_size ;

    x265_scan_t scan ;
    x265_mc_functions_t   mc;
    x265_bitstream_function_t bsf ;

} ;

void print_double ( double f_value, FILE *file ) ;

void open_state_file () ;
void close_state_file () ;
void print_int_state (int32_t i_state) ;
void print_line () ;
void print_int64_state (int64_t i64_state) ;
void print_double_state (double f_state) ;

void reset_timer_status () ;
void print_timer_status () ;
void add_timer_status_1 ( int64_t i_timer_status ) ;
void add_timer_status_2 ( int64_t i_timer_status ) ;
void add_total_time ( int64_t i_total_time ) ;
void add_part_time ( int64_t i_part_time ) ;
int32_t memory_compare (pixel *src1,
						int32_t i_src_stride1,
						pixel *src2,
						int32_t i_src_stride2,
						int32_t i_width,
						int32_t i_height ) ;
int32_t memory_compare_p(pixel *src1,
						int32_t i_src_stride1,
						pixel *src2,
						int32_t i_src_stride2,
						int32_t i_width,
						int32_t i_height ) ;
int32_t memory_compare_s (spixel *src1,
						int32_t i_src_stride1,
						spixel *src2,
						int32_t i_src_stride2,
						int32_t i_width,
						int32_t i_height ) ;
int32_t memory_compare_int16 (int16_t *src1,
							int32_t i_src_stride1,
							int16_t *src2,
							int32_t i_src_stride2,
							int32_t i_width,
							int32_t i_height ) ;
int32_t memory_compare_int32 (int32_t *src1,
							int32_t i_src_stride1,
							int32_t *src2,
							int32_t i_src_stride2,
							int32_t i_width,
							int32_t i_height ) ;

#endif

