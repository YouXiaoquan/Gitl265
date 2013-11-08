

#include "common.h"








void x265_weight_prediction_delete ( x265_weight_prediction_t *weight_prediction )
{
	x265_free ( weight_prediction ) ;
}

x265_weight_prediction_t *x265_weight_prediction_new ()
{
	x265_weight_prediction_t *weight_prediction = NULL ;

	CHECKED_MALLOCZERO( weight_prediction, sizeof(x265_weight_prediction_t) );

	return weight_prediction ;
fail:
	x265_weight_prediction_delete ( (void*) weight_prediction ) ;
	return NULL ;
}

int x265_weight_prediction_init ( x265_weight_prediction_t *weight_prediction )
{

    return 0 ;
}

void x265_weight_prediction_deinit ( x265_weight_prediction_t *weight_prediction )
{

}

//=======================================================
//  getWpScaling()
//=======================================================
/** derivation of wp tables
 * \param TComDataCU* pcCU
 * \param iRefIdx0
 * \param iRefIdx1
 * \param wpScalingParam *&wp0
 * \param wpScalingParam *&wp1
 * \param ibdi
 * \returns Void
 */
void x265_weight_prediction_gget_wp_scaling( x265_data_cu_t* pc_cu,
											Int iRefIdx0, Int iRefIdx1, wpScalingParam *&wp0, wpScalingParam *&wp1)
{
  TComSlice*      pcSlice       = pcCU->getSlice();
  TComPPS*        pps           = pcCU->getSlice()->getPPS();
  Bool            wpBiPred = pps->getWPBiPred();
  wpScalingParam* pwp;
  Bool            bBiDir        = (iRefIdx0>=0 && iRefIdx1>=0);
  Bool            bUniDir       = !bBiDir;

  if ( bUniDir || wpBiPred )
  { // explicit --------------------
    if ( iRefIdx0 >= 0 )
    {
      pcSlice->getWpScaling(REF_PIC_LIST_0, iRefIdx0, wp0);
    }
    if ( iRefIdx1 >= 0 )
    {
      pcSlice->getWpScaling(REF_PIC_LIST_1, iRefIdx1, wp1);
    }
  }
  else
  {
    assert(0);
  }

  if ( iRefIdx0 < 0 )
  {
    wp0 = NULL;
  }
  if ( iRefIdx1 < 0 )
  {
    wp1 = NULL;
  }

  if ( bBiDir )
  { // Bi-Dir case
    for ( Int yuv=0 ; yuv<3 ; yuv++ )
    {
      Int bitDepth = yuv ? g_bitDepthC : g_bitDepthY;
      wp0[yuv].w      = wp0[yuv].iWeight;
      wp0[yuv].o      = wp0[yuv].iOffset * (1 << (bitDepth-8));
      wp1[yuv].w      = wp1[yuv].iWeight;
      wp1[yuv].o      = wp1[yuv].iOffset * (1 << (bitDepth-8));
      wp0[yuv].offset = wp0[yuv].o + wp1[yuv].o;
      wp0[yuv].shift  = wp0[yuv].uiLog2WeightDenom + 1;
      wp0[yuv].round  = (1 << wp0[yuv].uiLog2WeightDenom);
      wp1[yuv].offset = wp0[yuv].offset;
      wp1[yuv].shift  = wp0[yuv].shift;
      wp1[yuv].round  = wp0[yuv].round;
    }
  }
  else
  {  // Unidir
    pwp = (iRefIdx0>=0) ? wp0 : wp1 ;
    for ( Int yuv=0 ; yuv<3 ; yuv++ )
    {
      Int bitDepth = yuv ? g_bitDepthC : g_bitDepthY;
      pwp[yuv].w      = pwp[yuv].iWeight;
      pwp[yuv].offset = pwp[yuv].iOffset * (1 << (bitDepth-8));
      pwp[yuv].shift  = pwp[yuv].uiLog2WeightDenom;
      pwp[yuv].round  = (pwp[yuv].uiLog2WeightDenom>=1) ? (1 << (pwp[yuv].uiLog2WeightDenom-1)) : (0);
    }
  }
}


