
#include "common.h"


#if _x265__a_m_p__s_a_d
  _fp_dist_func              m_afp_distort_func[64]; // [e_d_func]
#else
  _fp_dist_func              m_afp_distort_func[33]; // [e_d_func]
#endif

#if _x265__w_e_i_g_h_t_e_d__c_h_r_o_m_a__d_i_s_t_o_r_t_i_o_n
  double m_cb_distortion_weight;
  double m_cr_distortion_weight;
#endif
  double m_d_lambda;
  double m_sqrt_lambda;
  uint32_t i_lambda_motion_s_a_d;
  uint32_t i_lambda_motion_s_s_e;
  double m_d_frame_lambda;

  // for motion cost
#if _x265__f_i_x203
  _t_com_mv                  m_mv_predictor;
#else
  uint32_t m_pi_component_cost_origin_p;
  uint32_t m_pi_component_cost;
  uint32_t m_pi_ver_cost;
  uint32_t m_pi_hor_cost;
#endif
  uint32_t i_cost;
  int32_t i_cost_scale;
#if _x265_!_f_i_x203
  int32_t i_search_limit;
#endif
