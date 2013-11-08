

#include "common.h"



x265_rd_cost_weight_prediction_class_static_t *rd_cost_weight_prediction_class_static ;


x265_rd_cost_weight_prediction_t *x265_rd_cost_weight_prediction_new ()
{
	x265_rd_cost_weight_prediction_t *rd_cost_weight_prediction = NULL ;

	CHECKED_MALLOCZERO( rd_cost_weight_prediction, sizeof(x265_rd_cost_weight_prediction_t) );

	if ( x265_rd_cost_weight_prediction_init ( rd_cost_weight_prediction ) )
	{
		goto fail ;
	}
	return rd_cost_weight_prediction ;
fail:
	x265_rd_cost_weight_prediction_delete ( (void*) rd_cost_weight_prediction ) ;
	return NULL ;
}

void x265_rd_cost_weight_prediction_delete ( x265_rd_cost_weight_prediction_t *rd_cost_weight_prediction )
{
	x265_rd_cost_weight_prediction_deinit ( rd_cost_weight_prediction ) ;
	x265_free ( rd_cost_weight_prediction ) ;
}

int x265_rd_cost_weight_prediction_init ( x265_rd_cost_weight_prediction_t *rd_cost_weight_prediction )
{

	return 0 ;
}

void x265_rd_cost_weight_prediction_deinit ( x265_rd_cost_weight_prediction_t *rd_cost_weight_prediction )
{

}

void x_set_wp_scale
	( x265_rd_cost_weight_prediction_t  *rd_cost_weight_prediction, int32_t i_w0, int32_t i_w1, int32_t i_shift, int32_t i_offset, int32_t i_round)
{

	rd_cost_weight_prediction->i_w0 = i_w0;
	rd_cost_weight_prediction->i_w1 = i_w1;
	rd_cost_weight_prediction->i_shift = i_shift;
	rd_cost_weight_prediction->i_offset = i_offset;
	rd_cost_weight_prediction->i_round = i_round;
	rd_cost_weight_prediction->b_x_set_done = 1 ;
}

uint32_t x_get_sadw( x265_rd_cost_weight_prediction_t  *rd_cost_weight_prediction, x265_dist_param_t* dist_param )
{
	pixel i_pred;
	pixel* org = dist_param->org;
	pixel* cur = dist_param->cur;
	int32_t i_rows = dist_param->i_rows;
	int32_t i_cols = dist_param->i_cols;
	int32_t i_stride_cur = dist_param->i_stride_cur;
	int32_t i_stride_org = dist_param->i_stride_org;

	uint32_t i_comp    = dist_param->i_comp;
	assert(i_comp<3);
	x265_wp_scaling_param_t  *wp_scaling_param    = &(dist_param->wp_scaling_param[i_comp]);
	int32_t w0 = wp_scaling_param->w,
			offset = wp_scaling_param->offset,
			shift   = wp_scaling_param->shift,
			i_round   = wp_scaling_param->i_round;
	uint32_t i_sum = 0;

	for( ; i_rows != 0; i_rows-- )
	{
		for (int32_t n = 0; n < i_cols; n++ )
		{
			i_pred = ( (w0*cur[n] + i_round) >> shift ) + offset ;

			i_sum += abs( org[n] - i_pred );
		}
		org += i_stride_org;
		cur += i_stride_cur;
	}

	dist_param->i_comp = 255;  // reset for _d_e_b_u_g (assert test)

	return i_sum >> X265_DISTORTION_PRECISION_ADJUSTMENT(dist_param->i_bit_depth-8);
}

// --------------------------------------------------------------------------------------------------------------------
// _s_s_e
// --------------------------------------------------------------------------------------------------------------------
/** get weighted _s_s_d cost
 * \param dist_param
 * \returns uint32_t
 */
uint32_t x_get_ssew( x265_rd_cost_weight_prediction_t  *rd_cost_weight_prediction, x265_dist_param_t* dist_param )
{
  pixel* org   = dist_param->org;
  pixel* cur   = dist_param->cur;
  pixel i_pred;
  int32_t  i_rows   = dist_param->i_rows;
  int32_t  i_cols   = dist_param->i_cols;
  int32_t  i_stride_org = dist_param->i_stride_org;
  int32_t  i_stride_cur = dist_param->i_stride_cur;

  assert( dist_param->i_sub_shift == 0 );

  uint32_t            i_comp    = dist_param->i_comp;
  assert(i_comp<3);
  x265_wp_scaling_param_t  *wp_scaling_param    = &(dist_param->wp_scaling_param[i_comp]);
  int32_t   w0      = wp_scaling_param->w,
        offset  = wp_scaling_param->offset,
        shift   = wp_scaling_param->shift,
        i_round   = wp_scaling_param->i_round;

  uint32_t i_sum = 0;
  uint32_t i_shift = X265_DISTORTION_PRECISION_ADJUSTMENT((dist_param->i_bit_depth-8) << 1);

  int32_t i_temp;

  for( ; i_rows != 0; i_rows-- )
  {
    for (int32_t n = 0; n < i_cols; n++ )
    {
      i_pred = ( (w0*cur[n] + i_round) >> shift ) + offset ;

      i_temp = org[n  ] - i_pred;
      i_sum += ( i_temp * i_temp ) >> i_shift;
    }
    org += i_stride_org;
    cur += i_stride_cur;
  }

  dist_param->i_comp = 255;  // reset for _d_e_b_u_g (assert test)

  return ( i_sum );
}

// --------------------------------------------------------------------------------------------------------------------
// _h_a_d_a_m_a_r_d with step (used in fractional search)
// --------------------------------------------------------------------------------------------------------------------
/** get weighted _hadamard cost for 2x2 block
 * \param *org
 * \param *cur
 * \param i_stride_org
 * \param i_stride_cur
 * \param i_step
 * \returns uint32_t
 */
uint32_t x_calc_hads2x2w( x265_rd_cost_weight_prediction_t  *rd_cost_weight_prediction, pixel *org, pixel *cur, int32_t i_stride_org, int32_t i_stride_cur, int32_t i_step )
{
  int32_t satd = 0, diff[4], m[4];

  assert( rd_cost_weight_prediction->b_x_set_done );
  pixel   i_pred;

  i_pred    = ( (rd_cost_weight_prediction->i_w0*cur[0*i_step             ] + rd_cost_weight_prediction->i_round) >> rd_cost_weight_prediction->i_shift ) + rd_cost_weight_prediction->i_offset ;
  diff[0] = org[0             ] - i_pred;
  i_pred    = ( (rd_cost_weight_prediction->i_w0*cur[1*i_step             ] + rd_cost_weight_prediction->i_round) >> rd_cost_weight_prediction->i_shift ) + rd_cost_weight_prediction->i_offset ;
  diff[1] = org[1             ] - i_pred;
  i_pred    = ( (rd_cost_weight_prediction->i_w0*cur[0*i_step + i_stride_cur] + rd_cost_weight_prediction->i_round) >> rd_cost_weight_prediction->i_shift ) + rd_cost_weight_prediction->i_offset ;
  diff[2] = org[i_stride_org    ] - i_pred;
  i_pred    = ( (rd_cost_weight_prediction->i_w0*cur[1*i_step + i_stride_cur] + rd_cost_weight_prediction->i_round) >> rd_cost_weight_prediction->i_shift ) + rd_cost_weight_prediction->i_offset ;
  diff[3] = org[i_stride_org + 1] - i_pred;

  m[0] = diff[0] + diff[2];
  m[1] = diff[1] + diff[3];
  m[2] = diff[0] - diff[2];
  m[3] = diff[1] - diff[3];

  satd += abs(m[0] + m[1]);
  satd += abs(m[0] - m[1]);
  satd += abs(m[2] + m[3]);
  satd += abs(m[2] - m[3]);

  return satd;
}

/** get weighted _hadamard cost for 4x4 block
 * \param *org
 * \param *cur
 * \param i_stride_org
 * \param i_stride_cur
 * \param i_step
 * \returns uint32_t
 */
uint32_t x_calc_hads4x4w( x265_rd_cost_weight_prediction_t  *rd_cost_weight_prediction, pixel *org, pixel *cur, int32_t i_stride_org, int32_t i_stride_cur, int32_t i_step )
{
  int32_t k, satd = 0, diff[16], m[16], d[16];

  assert( rd_cost_weight_prediction->b_x_set_done );
  pixel   i_pred;

  for( k = 0; k < 16; k+=4 )
  {
    i_pred      = ( (rd_cost_weight_prediction->i_w0*cur[0*i_step] + rd_cost_weight_prediction->i_round) >> rd_cost_weight_prediction->i_shift ) + rd_cost_weight_prediction->i_offset ;
    diff[k+0] = org[0] - i_pred;
    i_pred      = ( (rd_cost_weight_prediction->i_w0*cur[1*i_step] + rd_cost_weight_prediction->i_round) >> rd_cost_weight_prediction->i_shift ) + rd_cost_weight_prediction->i_offset ;
    diff[k+1] = org[1] - i_pred;
    i_pred      = ( (rd_cost_weight_prediction->i_w0*cur[2*i_step] + rd_cost_weight_prediction->i_round) >> rd_cost_weight_prediction->i_shift ) + rd_cost_weight_prediction->i_offset ;
    diff[k+2] = org[2] - i_pred;
    i_pred      = ( (rd_cost_weight_prediction->i_w0*cur[3*i_step] + rd_cost_weight_prediction->i_round) >> rd_cost_weight_prediction->i_shift ) + rd_cost_weight_prediction->i_offset ;
    diff[k+3] = org[3] - i_pred;

    cur += i_stride_cur;
    org += i_stride_org;
  }

  /*===== hadamard transform =====*/
  m[ 0] = diff[ 0] + diff[12];
  m[ 1] = diff[ 1] + diff[13];
  m[ 2] = diff[ 2] + diff[14];
  m[ 3] = diff[ 3] + diff[15];
  m[ 4] = diff[ 4] + diff[ 8];
  m[ 5] = diff[ 5] + diff[ 9];
  m[ 6] = diff[ 6] + diff[10];
  m[ 7] = diff[ 7] + diff[11];
  m[ 8] = diff[ 4] - diff[ 8];
  m[ 9] = diff[ 5] - diff[ 9];
  m[10] = diff[ 6] - diff[10];
  m[11] = diff[ 7] - diff[11];
  m[12] = diff[ 0] - diff[12];
  m[13] = diff[ 1] - diff[13];
  m[14] = diff[ 2] - diff[14];
  m[15] = diff[ 3] - diff[15];

  d[ 0] = m[ 0] + m[ 4];
  d[ 1] = m[ 1] + m[ 5];
  d[ 2] = m[ 2] + m[ 6];
  d[ 3] = m[ 3] + m[ 7];
  d[ 4] = m[ 8] + m[12];
  d[ 5] = m[ 9] + m[13];
  d[ 6] = m[10] + m[14];
  d[ 7] = m[11] + m[15];
  d[ 8] = m[ 0] - m[ 4];
  d[ 9] = m[ 1] - m[ 5];
  d[10] = m[ 2] - m[ 6];
  d[11] = m[ 3] - m[ 7];
  d[12] = m[12] - m[ 8];
  d[13] = m[13] - m[ 9];
  d[14] = m[14] - m[10];
  d[15] = m[15] - m[11];

  m[ 0] = d[ 0] + d[ 3];
  m[ 1] = d[ 1] + d[ 2];
  m[ 2] = d[ 1] - d[ 2];
  m[ 3] = d[ 0] - d[ 3];
  m[ 4] = d[ 4] + d[ 7];
  m[ 5] = d[ 5] + d[ 6];
  m[ 6] = d[ 5] - d[ 6];
  m[ 7] = d[ 4] - d[ 7];
  m[ 8] = d[ 8] + d[11];
  m[ 9] = d[ 9] + d[10];
  m[10] = d[ 9] - d[10];
  m[11] = d[ 8] - d[11];
  m[12] = d[12] + d[15];
  m[13] = d[13] + d[14];
  m[14] = d[13] - d[14];
  m[15] = d[12] - d[15];

  d[ 0] = m[ 0] + m[ 1];
  d[ 1] = m[ 0] - m[ 1];
  d[ 2] = m[ 2] + m[ 3];
  d[ 3] = m[ 3] - m[ 2];
  d[ 4] = m[ 4] + m[ 5];
  d[ 5] = m[ 4] - m[ 5];
  d[ 6] = m[ 6] + m[ 7];
  d[ 7] = m[ 7] - m[ 6];
  d[ 8] = m[ 8] + m[ 9];
  d[ 9] = m[ 8] - m[ 9];
  d[10] = m[10] + m[11];
  d[11] = m[11] - m[10];
  d[12] = m[12] + m[13];
  d[13] = m[12] - m[13];
  d[14] = m[14] + m[15];
  d[15] = m[15] - m[14];

  for (k=0; k<16; ++k)
  {
    satd += abs(d[k]);
  }
  satd = ((satd+1)>>1);

  return satd;
}

/** get weighted _hadamard cost for 8x8 block
 * \param *org
 * \param *cur
 * \param i_stride_org
 * \param i_stride_cur
 * \param i_step
 * \returns uint32_t
 */
uint32_t x_calc_hads8x8w( x265_rd_cost_weight_prediction_t  *rd_cost_weight_prediction, pixel *org, pixel *cur, int32_t i_stride_org, int32_t i_stride_cur, int32_t i_step )
{
  int32_t k, i, j, jj, sad=0;
  int32_t diff[64], m1[8][8], m2[8][8], m3[8][8];
  int32_t i_step2 = i_step<<1;
  int32_t i_step3 = i_step2 + i_step;
  int32_t i_step4 = i_step3 + i_step;
  int32_t i_step5 = i_step4 + i_step;
  int32_t i_step6 = i_step5 + i_step;
  int32_t i_step7 = i_step6 + i_step;

  assert( rd_cost_weight_prediction->b_x_set_done );
  pixel   i_pred;

  for( k = 0; k < 64; k+=8 )
  {
    i_pred      = ( (rd_cost_weight_prediction->i_w0*cur[     0] + rd_cost_weight_prediction->i_round) >> rd_cost_weight_prediction->i_shift ) + rd_cost_weight_prediction->i_offset ;
    diff[k+0] = org[0] - i_pred;
    i_pred      = ( (rd_cost_weight_prediction->i_w0*cur[i_step ] + rd_cost_weight_prediction->i_round) >> rd_cost_weight_prediction->i_shift ) + rd_cost_weight_prediction->i_offset ;
    diff[k+1] = org[1] - i_pred;
    i_pred      = ( (rd_cost_weight_prediction->i_w0*cur[i_step2] + rd_cost_weight_prediction->i_round) >> rd_cost_weight_prediction->i_shift ) + rd_cost_weight_prediction->i_offset ;
    diff[k+2] = org[2] - i_pred;
    i_pred      = ( (rd_cost_weight_prediction->i_w0*cur[i_step3] + rd_cost_weight_prediction->i_round) >> rd_cost_weight_prediction->i_shift ) + rd_cost_weight_prediction->i_offset ;
    diff[k+3] = org[3] - i_pred;
    i_pred      = ( (rd_cost_weight_prediction->i_w0*cur[i_step4] + rd_cost_weight_prediction->i_round) >> rd_cost_weight_prediction->i_shift ) + rd_cost_weight_prediction->i_offset ;
    diff[k+4] = org[4] - i_pred;
    i_pred      = ( (rd_cost_weight_prediction->i_w0*cur[i_step5] + rd_cost_weight_prediction->i_round) >> rd_cost_weight_prediction->i_shift ) + rd_cost_weight_prediction->i_offset ;
    diff[k+5] = org[5] - i_pred;
    i_pred      = ( (rd_cost_weight_prediction->i_w0*cur[i_step6] + rd_cost_weight_prediction->i_round) >> rd_cost_weight_prediction->i_shift ) + rd_cost_weight_prediction->i_offset ;
    diff[k+6] = org[6] - i_pred;
    i_pred      = ( (rd_cost_weight_prediction->i_w0*cur[i_step7] + rd_cost_weight_prediction->i_round) >> rd_cost_weight_prediction->i_shift ) + rd_cost_weight_prediction->i_offset ;
    diff[k+7] = org[7] - i_pred;

    cur += i_stride_cur;
    org += i_stride_org;
  }

  //horizontal
  for (j=0; j < 8; j++)
  {
    jj = j << 3;
    m2[j][0] = diff[jj  ] + diff[jj+4];
    m2[j][1] = diff[jj+1] + diff[jj+5];
    m2[j][2] = diff[jj+2] + diff[jj+6];
    m2[j][3] = diff[jj+3] + diff[jj+7];
    m2[j][4] = diff[jj  ] - diff[jj+4];
    m2[j][5] = diff[jj+1] - diff[jj+5];
    m2[j][6] = diff[jj+2] - diff[jj+6];
    m2[j][7] = diff[jj+3] - diff[jj+7];

    m1[j][0] = m2[j][0] + m2[j][2];
    m1[j][1] = m2[j][1] + m2[j][3];
    m1[j][2] = m2[j][0] - m2[j][2];
    m1[j][3] = m2[j][1] - m2[j][3];
    m1[j][4] = m2[j][4] + m2[j][6];
    m1[j][5] = m2[j][5] + m2[j][7];
    m1[j][6] = m2[j][4] - m2[j][6];
    m1[j][7] = m2[j][5] - m2[j][7];

    m2[j][0] = m1[j][0] + m1[j][1];
    m2[j][1] = m1[j][0] - m1[j][1];
    m2[j][2] = m1[j][2] + m1[j][3];
    m2[j][3] = m1[j][2] - m1[j][3];
    m2[j][4] = m1[j][4] + m1[j][5];
    m2[j][5] = m1[j][4] - m1[j][5];
    m2[j][6] = m1[j][6] + m1[j][7];
    m2[j][7] = m1[j][6] - m1[j][7];
  }

  //vertical
  for (i=0; i < 8; i++)
  {
    m3[0][i] = m2[0][i] + m2[4][i];
    m3[1][i] = m2[1][i] + m2[5][i];
    m3[2][i] = m2[2][i] + m2[6][i];
    m3[3][i] = m2[3][i] + m2[7][i];
    m3[4][i] = m2[0][i] - m2[4][i];
    m3[5][i] = m2[1][i] - m2[5][i];
    m3[6][i] = m2[2][i] - m2[6][i];
    m3[7][i] = m2[3][i] - m2[7][i];

    m1[0][i] = m3[0][i] + m3[2][i];
    m1[1][i] = m3[1][i] + m3[3][i];
    m1[2][i] = m3[0][i] - m3[2][i];
    m1[3][i] = m3[1][i] - m3[3][i];
    m1[4][i] = m3[4][i] + m3[6][i];
    m1[5][i] = m3[5][i] + m3[7][i];
    m1[6][i] = m3[4][i] - m3[6][i];
    m1[7][i] = m3[5][i] - m3[7][i];

    m2[0][i] = m1[0][i] + m1[1][i];
    m2[1][i] = m1[0][i] - m1[1][i];
    m2[2][i] = m1[2][i] + m1[3][i];
    m2[3][i] = m1[2][i] - m1[3][i];
    m2[4][i] = m1[4][i] + m1[5][i];
    m2[5][i] = m1[4][i] - m1[5][i];
    m2[6][i] = m1[6][i] + m1[7][i];
    m2[7][i] = m1[6][i] - m1[7][i];
  }

  for (j=0; j < 8; j++)
  {
    for (i=0; i < 8; i++)
    {
      sad += (abs(m2[j][i]));
    }
  }

  sad=((sad+2)>>2);

  return sad;
}

/** get weighted _hadamard cost
 * \param *dist_param
 * \returns uint32_t
 */
uint32_t x_get_hads4w( x265_rd_cost_weight_prediction_t  *rd_cost_weight_prediction, x265_dist_param_t* dist_param )
{
  pixel* org   = dist_param->org;
  pixel* cur   = dist_param->cur;
  int32_t  i_rows   = dist_param->i_rows;
  int32_t  i_stride_cur = dist_param->i_stride_cur;
  int32_t  i_stride_org = dist_param->i_stride_org;
  int32_t  i_step  = dist_param->i_step;
  int32_t  y;
  int32_t  i_offset_org = i_stride_org<<2;
  int32_t  i_offset_cur = i_stride_cur<<2;

  uint32_t i_sum = 0;

  for ( y=0; y<i_rows; y+= 4 )
  {
    i_sum += x_calc_hads4x4w( rd_cost_weight_prediction, org, cur, i_stride_org, i_stride_cur, i_step );
    org += i_offset_org;
    cur += i_offset_cur;
  }

  return i_sum >> X265_DISTORTION_PRECISION_ADJUSTMENT(dist_param->i_bit_depth-8);
}

/** get weighted _hadamard cost
 * \param *dist_param
 * \returns uint32_t
 */
uint32_t x_get_hads8w( x265_rd_cost_weight_prediction_t  *rd_cost_weight_prediction, x265_dist_param_t* dist_param )
{
  pixel* org   = dist_param->org;
  pixel* cur   = dist_param->cur;
  int32_t  i_rows   = dist_param->i_rows;
  int32_t  i_stride_cur = dist_param->i_stride_cur;
  int32_t  i_stride_org = dist_param->i_stride_org;
  int32_t  i_step  = dist_param->i_step;
  int32_t  y;

  uint32_t i_sum = 0;

  if ( i_rows == 4 )
  {
    i_sum += x_calc_hads4x4w( rd_cost_weight_prediction, org+0, cur        , i_stride_org, i_stride_cur, i_step );
    i_sum += x_calc_hads4x4w( rd_cost_weight_prediction, org+4, cur+4*i_step, i_stride_org, i_stride_cur, i_step );
  }
  else
  {
    int32_t  i_offset_org = i_stride_org<<3;
    int32_t  i_offset_cur = i_stride_cur<<3;
    for ( y=0; y<i_rows; y+= 8 )
    {
      i_sum += x_calc_hads8x8w( rd_cost_weight_prediction, org, cur, i_stride_org, i_stride_cur, i_step );
      org += i_offset_org;
      cur += i_offset_cur;
    }
  }

  return i_sum >> X265_DISTORTION_PRECISION_ADJUSTMENT(dist_param->i_bit_depth-8);
}

/** get weighted _hadamard cost
 * \param *dist_param
 * \returns uint32_t
 */
uint32_t x_get_hadsw( x265_rd_cost_weight_prediction_t  *rd_cost_weight_prediction, x265_dist_param_t* dist_param )
{
  pixel* org   = dist_param->org;
  pixel* cur   = dist_param->cur;
  int32_t  i_rows   = dist_param->i_rows;
  int32_t  i_cols   = dist_param->i_cols;
  int32_t  i_stride_cur = dist_param->i_stride_cur;
  int32_t  i_stride_org = dist_param->i_stride_org;
  int32_t  i_step  = dist_param->i_step;

  int32_t  x, y;

  uint32_t            i_comp    = dist_param->i_comp;
  assert(i_comp<3);
  x265_wp_scaling_param_t  *wp_scaling_param    = &(dist_param->wp_scaling_param[i_comp]);

	x_set_wp_scale(rd_cost_weight_prediction, wp_scaling_param->w, 0, wp_scaling_param->shift, wp_scaling_param->offset, wp_scaling_param->i_round);

  uint32_t i_sum = 0;

  if( ( i_rows % 8 == 0) && (i_cols % 8 == 0) )
  {
    int32_t  i_offset_org = i_stride_org<<3;
    int32_t  i_offset_cur = i_stride_cur<<3;
    for ( y=0; y<i_rows; y+= 8 )
    {
      for ( x=0; x<i_cols; x+= 8 )
      {
        i_sum += x_calc_hads8x8w( rd_cost_weight_prediction, &org[x], &cur[x*i_step], i_stride_org, i_stride_cur, i_step );
      }
      org += i_offset_org;
      cur += i_offset_cur;
    }
  }
  else if( ( i_rows % 4 == 0) && (i_cols % 4 == 0) )
  {
    int32_t  i_offset_org = i_stride_org<<2;
    int32_t  i_offset_cur = i_stride_cur<<2;

    for ( y=0; y<i_rows; y+= 4 )
    {
      for ( x=0; x<i_cols; x+= 4 )
      {
        i_sum += x_calc_hads4x4w( rd_cost_weight_prediction, &org[x], &cur[x*i_step], i_stride_org, i_stride_cur, i_step );
      }
      org += i_offset_org;
      cur += i_offset_cur;
    }
  }
  else
  {
    for ( y=0; y<i_rows; y+=2 )
    {
      for ( x=0; x<i_cols; x+=2 )
      {
        i_sum += x_calc_hads2x2w( rd_cost_weight_prediction, &org[x], &cur[x*i_step], i_stride_org, i_stride_cur, i_step );
      }
      org += i_stride_org;
      cur += i_stride_cur;
    }
  }

  rd_cost_weight_prediction->b_x_set_done  = 0;

  return i_sum >> X265_DISTORTION_PRECISION_ADJUSTMENT(dist_param->i_bit_depth-8);
}

void print_rd_cost_weight_prediction_offset ()
{
	FILE *file = stderr ;

	fprintf ( file, "%d\n", (uint32_t)&((x265_rd_cost_weight_prediction_t*)0)->i_w0 ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_rd_cost_weight_prediction_t*)0)->i_w1 ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_rd_cost_weight_prediction_t*)0)->i_shift ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_rd_cost_weight_prediction_t*)0)->i_offset ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_rd_cost_weight_prediction_t*)0)->i_round ) ;
	fprintf ( file, "%d\n", (uint32_t)&((x265_rd_cost_weight_prediction_t*)0)->b_x_set_done ) ;;

	fprintf ( file, "%d\n", sizeof(x265_rd_cost_weight_prediction_t) ) ;
}


