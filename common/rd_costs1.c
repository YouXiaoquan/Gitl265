
#include "common.h"

  // for distortion
  Int                     m_iBlkWidth;
  Int                     m_iBlkHeight;

#if X265_AMP_SAD
  FpDistFunc              m_afpDistortFunc[64]; // [eDFunc]
#else
  FpDistFunc              m_afpDistortFunc[33]; // [eDFunc]
#endif

#if X265_WEIGHTED_CHROMA_DISTORTION
  Double                  m_cbDistortionWeight;
  Double                  m_crDistortionWeight;
#endif
  Double                  m_dLambda;
  Double                  m_sqrtLambda;
  UInt                    m_uiLambdaMotionSAD;
  UInt                    m_uiLambdaMotionSSE;
  Double                  m_dFrameLambda;

  // for motion cost
#if X265_FIX203
  TComMv                  m_mvPredictor;
#else
  UInt*                   m_puiComponentCostOriginP;
  UInt*                   m_puiComponentCost;
  UInt*                   m_puiVerCost;
  UInt*                   m_puiHorCost;
#endif
  UInt                    m_uiCost;
  Int                     m_iCostScale;
#if X265_!FIX203
  Int                     m_iSearchLimit;
#endif
