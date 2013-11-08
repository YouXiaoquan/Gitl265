



#ifndef X265_RDOQ_X86_H
#define X265_RDOQ_X86_H

void x265_rdoq_0_8_ssse3 (int32_t *p_src_coeff,
							int32_t *p_q_coef,
							int32_t *p_level_double,
							int32_t *p_arl_dst_coeff,
							int32_t *p_max_level,
							int32_t i_q_bits,
							int32_t i_q_bits_c,
							double *p_err_scale,
							double *p_cost_coeff0,
							double *p_block_uncoded_cost ) ;
void x265_rdoq_1_8_ssse3 (int32_t *p_src_coeff,
							int32_t *p_q_coef,
							int32_t *p_level_double,
							int32_t *p_arl_dst_coeff,
							int32_t *p_max_level,
							int32_t i_q_bits,
							int32_t i_q_bits_c,
							double *p_err_scale,
							double *p_cost_coeff0,
							double *p_block_uncoded_cost ) ;
void x265_rdoq_2_8_ssse3 (int32_t *p_src_coeff,
							int32_t *p_q_coef,
							int32_t *p_level_double,
							int32_t *p_arl_dst_coeff,
							int32_t *p_max_level,
							int32_t i_q_bits,
							int32_t i_q_bits_c,
							double *p_err_scale,
							double *p_cost_coeff0,
							double *p_block_uncoded_cost ) ;
void x265_rdoq_3_8_ssse3 (int32_t *p_src_coeff,
							int32_t *p_q_coef,
							int32_t *p_level_double,
							int32_t *p_arl_dst_coeff,
							int32_t *p_max_level,
							int32_t i_q_bits,
							int32_t i_q_bits_c,
							double *p_err_scale,
							double *p_cost_coeff0,
							double *p_block_uncoded_cost ) ;


#endif
