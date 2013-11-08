


#ifndef X265_RD_COST_H
#define X265_RD_COST_H

#define X265_FIX203 1


typedef struct _x265_rd_cost_t x265_rd_cost_t ;
typedef struct _x265_dist_param_t x265_dist_param_t ;
typedef uint32_t (*x265_fp_dist_func) ( x265_rd_cost_t *rd_cost, x265_dist_param_t* dist_param) ;

#include "rd_cost_weight_prediction.h"

typedef struct _x265_dist_param_t
{
	pixel* org;
	pixel* cur;
	int32_t i_stride_org;
	int32_t i_stride_cur;
	int32_t i_rows;
	int32_t i_cols;
	int32_t i_step;
	x265_fp_dist_func dist_func;
	int32_t i_bit_depth;

	int32_t b_apply_weight;     // whether weithed prediction is used or not
	x265_wp_scaling_param_t *wp_scaling_param;           // weithed prediction scaling parameters for current ref
	uint32_t i_comp;           // uiComp = 0 (luma Y), 1 (chroma U), 2 (chroma V)

#if X265_NS_HAD
	int32_t b_use_ns_had;
#endif

	// (vertical) subsampling shift (for reducing complexity)
	// - 0 = no subsampling, 1 = even rows, 2 = every 4th, etc.
	int32_t i_sub_shift;

} x265_dist_param_t ;


struct _x265_rd_cost_t
{
	x265_rd_cost_weight_prediction_t rd_cost_weight_prediction ;

	// for distortion
	// for distortion
	int32_t i_blk_width ;
	int32_t i_blk_height ;

#if X265_AMP_SAD
	x265_fp_dist_func fp_distort_func[64] ; // [d_func]
#else
	x265_fp_dist_func fp_distort_func[33] ; // [d_func]
#endif

#if X265_WEIGHTED_CHROMA_DISTORTION
	double f_cb_distortion_weight ;
	double f_cr_distortion_weight ;
#endif
	double f_lambda ;
	double f_sqrt_lambda ;
	uint32_t i_lambda_motion_sad ;
	uint32_t i_lambda_motion_sse ;
	double f_frame_lambda ;

  // for motion cost
#if X265_FIX203
	x265_mv_t predictor ;
#else
	uint32_t *component_cost_origin_p ;
	uint32_t *component_cost ;
	uint32_t *ver_cost ;
	uint32_t *hor_cost ;
#endif
	uint32_t i_cost ;
	int32_t i_cost_scale ;
#if !X265_FIX203
	int32_t i_search_limit ;
#endif


	/*
	  Double  calcRdCost  ( UInt   i_bits, UInt   i_distortion, int32_t b_flag = false, DFunc d_func = DF_DEFAULT );
	  Double  calcRdCost64( UInt64 i_bits, UInt64 i_distortion, int32_t b_flag = false, DFunc d_func = DF_DEFAULT );

	  Void    setFrameLambda ( Double dLambda ) { m_dFrameLambda = dLambda; }

	  // Distortion Functions
	  Void    init();

	  Void    setDistParam1( UInt i_blk_width, UInt i_blk_height, DFunc d_func, DistParam& dist_param );
	  Void    setDistParam2( TComPattern* pattern_key, Pel* p_ref_y, Int i_ref_stride,            DistParam& dist_param );
	#if X265_NS_HAD
	  Void    setDistParam3( TComPattern* pattern_key, Pel* p_ref_y, Int i_ref_stride, Int i_step, DistParam& dist_param, int32_t b_hadme=false, int32_t b_use_ns_had=false );
	  Void    setDistParam4( DistParam& dist_param, Int bitDepth, Pel* p1, Int i_stride_1, Pel* p2, Int i_stride_2, Int i_width, Int i_height, int32_t b_hadamard = false, int32_t b_use_ns_had=false );
	#else
	  Void    setDistParam3( TComPattern* pattern_key, Pel* p_ref_y, Int i_ref_stride, Int i_step, DistParam& dist_param, int32_t b_hadme=false );
	  Void    setDistParam4( DistParam& dist_param, Int bitDepth, Pel* p1, Int i_stride_1, Pel* p2, Int i_stride_2, Int i_width, Int i_height, int32_t b_hadamard = false );
	#endif

	  UInt    calcHAD(Int bitDepth, Pel* pi0, Int i_stride_0, Pel* pi1, Int i_stride_1, Int i_width, Int i_height );

	  // for motion cost
	#if X265_!FIX203
	  Void    initRateDistortionModel( Int i_sub_pel_search_limit );
	  Void    xUninit();
	#endif
	  UInt    xGetComponentBits( Int i_val );
	  Void    getMotionCost( int32_t bSad, Int iAdd ) { m_uiCost = (bSad ? m_uiLambdaMotionSAD + iAdd : m_uiLambdaMotionSSE + iAdd); }
	  Void    setPredictor( TComMv& rcMv )
	  {
	#if X265_FIX203
	    m_mvPredictor = rcMv;
	#else
	    m_puiHorCost = m_puiComponentCost - rcMv.getHor();
	    m_puiVerCost = m_puiComponentCost - rcMv.getVer();
	#endif
	  }
	  Void    setCostScale( Int iCostScale )    { m_iCostScale = iCostScale; }
	  __inline UInt getCost( Int x, Int y )
	  {
	#if X265_FIX203
	    return m_uiCost * getBits(x, y) >> 16;
	#else
	    return (( m_uiCost * (m_puiHorCost[ x * (1<<m_iCostScale) ] + m_puiVerCost[ y * (1<<m_iCostScale) ]) ) >> 16);
	#endif
	  }
	  UInt    getBits( Int x, Int y )
	  {
	#if X265_FIX203
	    return xGetComponentBits((x << m_iCostScale) - m_mvPredictor.getHor())
	    +      xGetComponentBits((y << m_iCostScale) - m_mvPredictor.getVer());
	#else
	    return m_puiHorCost[ x * (1<<m_iCostScale)] + m_puiVerCost[ y * (1<<m_iCostScale) ];
	#endif
	  }

	  */

} ;

x265_dist_param_t *x265_dist_param_new () ;
void x265_dist_param_delete ( x265_dist_param_t *dist_param ) ;
int x265_dist_param_init ( x265_dist_param_t *dist_param ) ;
void x265_dist_param_deinit ( x265_dist_param_t *dist_param ) ;

x265_rd_cost_t *x265_rd_cost_new () ;
void x265_rd_cost_delete ( x265_rd_cost_t *rd_cost ) ;
int x265_rd_cost_init ( x265_rd_cost_t *rd_cost ) ;
void x265_rd_cost_deinit ( x265_rd_cost_t *rd_cost ) ;


void x265_rd_cost_get_motion_cost(x265_rd_cost_t *rd_cost,
									int32_t b_sad,
									int32_t i_add ) ;

void rd_cost_set_lambda( x265_rd_cost_t *rd_cost, double f_ambda ) ;

void print_rd_cost_offset () ;



void x265_rd_cost_set_predictor( x265_rd_cost_t *rd_cost, x265_mv_t *mv ) ;
uint32_t x265_rd_cost_get_cost_p2 ( x265_rd_cost_t *rd_cost, uint32_t i_bit ) ;
uint32_t x265_rd_cost_get_cost_p3 ( x265_rd_cost_t *rd_cost, int32_t x, int32_t y ) ;
uint32_t x265_rd_cost_get_bits( x265_rd_cost_t *rd_cost, int32_t x, int32_t y ) ;
double x265_rd_cost_calc_rd_cost( x265_rd_cost_t *rd_cost, uint32_t i_bits, uint32_t i_distortion, int32_t b_flag, enum d_func_e d_func ) ;
double x265_rd_cost_calc_rd_cost64( x265_rd_cost_t *rd_cost, uint64_t i_bits, uint64_t i_distortion, int32_t b_flag, enum d_func_e d_func ) ;
#if !X265_FIX203
void rd_cost_init_rate_distortion_model( x265_rd_cost_t *rd_cost, int32_t i_sub_pel_search_limit ) ;
#endif
uint32_t rd_cost_x_get_component_bits( x265_rd_cost_t *rd_cost, int32_t i_val ) ;
//	6
void x265_rd_cost_set_dist_param_p6_1( x265_t *h,
									x265_rd_cost_t *rd_cost,
									uint32_t i_blk_width,
									uint32_t i_blk_height,
									enum d_func_e d_func,
									x265_dist_param_t* dist_param ) ;
//	6
void x265_rd_cost_set_dist_param_p6_2( x265_t *h,
									x265_rd_cost_t *rd_cost,
									x265_pattern_t* pattern_key,
									pixel* p_ref_y,
									int32_t i_ref_stride,
									x265_dist_param_t *dist_param ) ;

#if X265_NS_HAD
//	9
void x265_rd_cost_set_dist_param_p9 ( x265_t *h,
									x265_rd_cost_t *rd_cost,
									x265_pattern_t* pattern_key,
									pixel* p_ref_y,
									int32_t i_ref_stride,
									int32_t i_step,
									x265_dist_param_t *dist_param,
									int32_t b_hadme,
									int32_t b_use_ns_had ) ;
//	11
void x265_rd_cost_set_dist_param_p11( x265_t *h,
									x265_rd_cost_t *rd_cost,
									x265_dist_param_t* dist_param,
									pixel* p1,
									int32_t i_stride_1,
									pixel* p2,
									int32_t i_stride_2,
									int32_t i_width,
									int32_t i_height,
									int32_t b_hadamard,
									int32_t b_use_ns_had ) ;
#else
//	8
void x265_rd_cost_set_dist_param_p8( x265_t *h,
									x265_rd_cost_t *rd_cost,
									x265_pattern_t* pattern_key,
									pixel* p_ref_y,
									int32_t i_ref_stride,
									int32_t i_step,
									x265_dist_param_t *dist_param,
									int32_t b_hadme ) ;
//	11
void x265_rd_cost_set_dist_param_p11( x265_t *h,
									x265_rd_cost_t *rd_cost,
									x265_dist_param_t *dist_param,
									int32_t i_bit_depth,
									pixel* p1,
									int32_t i_stride_1,
									pixel* p2,
									int32_t i_stride_2,
									int32_t i_width,
									int32_t i_height,
									int32_t b_hadamard ) ;
#endif

uint32_t x265_rd_cost_calc_had(x265_rd_cost_t *rd_cost,
								int32_t i_bit_depth,
								pixel* p_0,
								int32_t i_stride_0,
								pixel* p_1,
								int32_t i_stride_1,
								int32_t i_width,
								int32_t i_height ) ;
#if X265_WEIGHTED_CHROMA_DISTORTION
uint32_t x265_rd_cost_get_dist_part(x265_t *h, x265_rd_cost_t *rd_cost, int32_t i_bit_depth, pixel* cur, int32_t i_cur_stride,  pixel* org, int32_t i_org_stride, uint32_t i_blk_width, uint32_t i_blk_height, enum text_type_e e_text, enum d_func_e d_func) ;
#else
uint32_t x265_rd_cost_get_dist_part(x265_t *h, x265_rd_cost_t *rd_cost, int32_t i_bit_depth, pixel* cur, int32_t i_cur_stride,  pixel* org, int32_t i_org_stride, uint32_t i_blk_width, uint32_t i_blk_height, enum d_func_e d_func ) ;
#endif

#if X265_WEIGHTED_CHROMA_DISTORTION
uint32_t x265_rd_cost_get_sse_s_s(x265_t *h, x265_rd_cost_t *rd_cost, int32_t i_bit_depth, spixel* cur, int32_t i_cur_stride,  spixel* org, int32_t i_org_stride, uint32_t i_blk_width, uint32_t i_blk_height, enum text_type_e e_text, enum d_func_e d_func) ;
#else
uint32_t x265_rd_cost_get_sse_s_s(x265_t *h, x265_rd_cost_t *rd_cost, int32_t i_bit_depth, spixel* cur, int32_t i_cur_stride,  spixel* org, int32_t i_org_stride, uint32_t i_blk_width, uint32_t i_blk_height, enum d_func_e d_func ) ;
#endif

#if X265_WEIGHTED_CHROMA_DISTORTION
uint32_t x265_rd_cost_get_sse_p_s(x265_t *h, x265_rd_cost_t *rd_cost, int32_t i_bit_depth, pixel* cur, int32_t i_cur_stride,  spixel* org, int32_t i_org_stride, uint32_t i_blk_width, uint32_t i_blk_height, enum text_type_e e_text, enum d_func_e d_func) ;
#else
uint32_t x265_rd_cost_get_sse_p_s(x265_t *h, x265_rd_cost_t *rd_cost, int32_t i_bit_depth, pixel* cur, int32_t i_cur_stride,  spixel* org, int32_t i_org_stride, uint32_t i_blk_width, uint32_t i_blk_height, enum d_func_e d_func ) ;
#endif

#if X265_RATE_CONTROL_LAMBDA_DOMAIN
uint32_t x265_rd_cost_get_sad_part(x265_rd_cost_t *rd_cost,
									int32_t i_bit_depth,
									pixel* cur,
									int32_t cur_stride,
									pixel* org,
									int32_t org_stride,
									uint32_t width,
									uint32_t height ) ;
#endif




uint32_t x265_rd_cost_x_get_sad( x265_rd_cost_t *rd_cost, x265_dist_param_t* dist_param) ;
uint32_t x265_rd_cost_x_get_sad4( x265_rd_cost_t *rd_cost,  x265_dist_param_t* dist_param ) ;
uint32_t x265_rd_cost_x_get_sad8( x265_rd_cost_t *rd_cost,  x265_dist_param_t* dist_param ) ;
uint32_t x265_rd_cost_x_get_sad16( x265_rd_cost_t *rd_cost,  x265_dist_param_t* dist_param ) ;
#if X265_AMP_SAD
uint32_t x265_rd_cost_x_get_sad12( x265_rd_cost_t *rd_cost,  x265_dist_param_t* dist_param ) ;
#endif
uint32_t x265_rd_cost_x_get_sad16N( x265_rd_cost_t *rd_cost,  x265_dist_param_t* dist_param ) ;
uint32_t x265_rd_cost_x_get_sad32( x265_rd_cost_t *rd_cost,  x265_dist_param_t* dist_param ) ;
#if X265_AMP_SAD
uint32_t x265_rd_cost_x_get_sad24( x265_rd_cost_t *rd_cost,  x265_dist_param_t* dist_param ) ;
#endif
uint32_t x265_rd_cost_x_get_sad64( x265_rd_cost_t *rd_cost,  x265_dist_param_t* dist_param ) ;
#if X265_AMP_SAD
uint32_t x265_rd_cost_x_get_sad48( x265_rd_cost_t *rd_cost, x265_dist_param_t* dist_param ) ;
#endif
uint32_t x265_rd_cost_x_get_sse( x265_rd_cost_t *rd_cost, x265_dist_param_t* x265_dt_param ) ;
uint32_t x265_rd_cost_x_get_sse4( x265_rd_cost_t *rd_cost, x265_dist_param_t* x265_dt_param ) ;
uint32_t x265_rd_cost_x_get_sse8( x265_rd_cost_t *rd_cost, x265_dist_param_t* x265_dt_param ) ;
uint32_t x265_rd_cost_x_get_sse16( x265_rd_cost_t *rd_cost, x265_dist_param_t* x265_dt_param ) ;
uint32_t x265_rd_cost_x_get_sse16N( x265_rd_cost_t *rd_cost, x265_dist_param_t* x265_dt_param ) ;
uint32_t x265_rd_cost_x_get_sse32( x265_rd_cost_t *rd_cost, x265_dist_param_t* x265_dt_param ) ;
uint32_t x265_rd_cost_x_get_sse64( x265_rd_cost_t *rd_cost, x265_dist_param_t* x265_dt_param ) ;
uint32_t x265_rd_cost_x_calc_hads_2x2( x265_rd_cost_t *rd_cost, pixel *i_org, pixel *i_cur, int32_t i_stride_org, int32_t i_stride_cur, int32_t i_step ) ;
uint32_t x265_rd_cost_x_calc_hads_4x4( x265_rd_cost_t *rd_cost, pixel *i_org, pixel *i_cur, int32_t i_stride_org, int32_t i_stride_cur, int32_t i_step ) ;
uint32_t x265_rd_cost_x_calc_hads_8x8( x265_rd_cost_t *rd_cost, pixel *i_org, pixel *i_cur, int32_t i_stride_org, int32_t i_stride_cur, int32_t i_step ) ;
#if X265_NS_HAD
uint32_t x265_rd_cost_x_calc_hads_16x4( x265_rd_cost_t *rd_cost, pixel *i_org, pixel *i_cur, int32_t i_stride_org, int32_t i_stride_cur, int32_t i_step ) ;
uint32_t x265_rd_cost_x_calc_hads_4x16( x265_rd_cost_t *rd_cost, pixel *i_org, pixel *i_cur, int32_t i_stride_org, int32_t i_stride_cur, int32_t i_step ) ;
#endif
uint32_t x265_rd_cost_x_get_hads4( x265_rd_cost_t *rd_cost, x265_dist_param_t* x265_dt_param ) ;
uint32_t x265_rd_cost_x_get_hads8( x265_rd_cost_t *rd_cost, x265_dist_param_t* x265_dt_param ) ;
uint32_t x265_rd_cost_x_get_hads( x265_rd_cost_t *rd_cost, x265_dist_param_t* x265_dt_param ) ;


#endif
