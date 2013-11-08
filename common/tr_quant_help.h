


#ifndef X265_TR_QUANT_HELP_H
#define X265_TR_QUANT_HELP_H

#define			USE_SSE2										1

#define MY_ABS(i_abs_value) \
	( i_abs_value < 0 ? - i_abs_value : i_abs_value )

#if USE_SSE2
	#define x265_tr_quant_store_coeff_sig() \
	__asm__ __volatile__ \
	( \
		"MOVLPD %%XMM7, %0\n" \
		"MOVLPD %%XMM6, %1\n" \
		: \
		: \
		"m"(cost_coeff[ i_scan_pos ]), \
		"m"(cost_sig[ i_scan_pos ]) \
		: \
	) ; \


#define declare_x_get_coded_level1 \
	int32_t i_abs_level1 = 0 ; \
	double f_zero_value = 0.0 ; \
	int32_t err[2] ; \
	int32_t rate[2] ;

#else

#define x265_tr_quant_store_coeff_sig() \
	__asm__ __volatile__ \
	( \
		"FSTPL %0\n" \
		"FSTPL %1\n" \
		: \
		: \
		"m"(cost_coeff[ i_scan_pos ]), \
		"m"(cost_sig[ i_scan_pos ]) \
		: \
	) ; \

#define declare_x_get_coded_level1 \
	int32_t i_abs_level1 = 0 ; \
	int32_t i_err1 = 0 ; \
	int32_t i_rate2 = 0 ; \

#endif

#define declare_x_get_ic_rate_cost1 \
	int32_t i_symbol2 = 0 ; \
	int32_t f_rate2 = 0 ; \
	int32_t i_length2 = 0 ;

#define declare_x_get_ic_rate1 \
	int32_t i_abs_level3 = 0 ; \
	int32_t i_abs_level4 = 0 ; \
	int32_t i_max_vlc3 = 0 ; \
	int32_t b_exp_golomb3 = 0 ; \
	int32_t i_egs3 = 0 ; \
	int32_t i_max3 = 0 ; \
	int32_t i_pref_len3 = 0 ; \
	int32_t i_num_bins3 = 0 ;

#define declare_x_get_rate_last1 \
	uint32_t i_ctx_x4 = 0; \
	uint32_t i_ctx_y4 = 0;



#define x265_tr_quant_x_get_ic_rate_cost2(ic_rate_cost2) \
{ \
	ic_rate_cost2 = 32768 ; \
	if ( i_abs_level1 >= i_base_level ) \
	{ \
		i_symbol2 = i_abs_level1 - i_base_level; \
\
		if (i_symbol2 < X265_IC_RATE_COST_TABLE_LENGTH) \
		{ \
			ic_rate_cost2 += tr_quant->ic_rate_cost[i_go_rice_param][i_symbol2]; \
		} \
		else \
		{ \
			i_length2 = i_go_rice_param; \
			i_symbol2  = i_symbol2 - ( X265_COEF_REMAIN_BIN_REDUCTION << i_go_rice_param); \
			while (i_symbol2 >= (1<<i_length2)) \
			{ \
				i_symbol2 -=  (1<<(i_length2++)); \
			} \
			ic_rate_cost2 += (X265_COEF_REMAIN_BIN_REDUCTION \
						+ i_length2 + 1 - i_go_rice_param + i_length2 ) << 15; \
		} \
		if (i_c1_idx < X265_C1FLAG_NUMBER) \
		{ \
			ic_rate_cost2 += tr_quant->est_bits_sbac->greater_one_bits[ i_one_ctx ][ 1 ]; \
\
			if (i_c2_idx < X265_C2FLAG_NUMBER) \
			{ \
				ic_rate_cost2 += tr_quant->est_bits_sbac->level_abs_bits[ i_abs_ctx ][ 1 ]; \
			} \
		} \
	} \
	else \
	{ \
		if( i_abs_level1 == 1 ) \
		{ \
			ic_rate_cost2 += tr_quant->est_bits_sbac->greater_one_bits[ i_one_ctx ][ 0 ]; \
		} \
		else if( i_abs_level1 == 2 ) \
		{ \
			ic_rate_cost2 += tr_quant->est_bits_sbac->greater_one_bits[ i_one_ctx ][ 1 ]; \
			ic_rate_cost2 += tr_quant->est_bits_sbac->level_abs_bits[ i_abs_ctx ][ 0 ]; \
		} \
		else \
		{ \
			assert (0); \
		} \
	} \
\
}

#define x265_tr_quant_load_zero_to_fpu() \
		__asm__ __volatile__ \
		( \
			"FLDZ \n" \
			: \
			: \
			: \
		) ;

#define x265_tr_quant_load_zero_to_xmm() \
		__asm__ __volatile__ \
		( \
			"MOVLPD %0, %%XMM6\n" \
			: \
			: \
			"m"(f_zero_value) \
			: \
		) ; \

#define x265_tr_quant_calc_cost1(x_err,x_rate) \
		__asm__ __volatile__ \
		( \
			"CVTDQ2PD (%0), %%XMM7\n" \
			"CVTDQ2PD (%1), %%XMM3\n" \
			"MULSD %%XMM7, %%XMM7\n" \
			"MOVLPD %3, %%XMM2\n" \
			"MULSD %2, %%XMM7\n" \
			"MULSD %%XMM3, %%XMM2\n" \
			"ADDSD %%XMM2, %%XMM7\n" \
			: \
			: \
			"a"(err), \
			"b"(rate), \
			"m"(f_temp), \
			"m"(tr_quant->f_lambda) \
			: \
		) ;

#define x265_tr_quant_calc_cost2(x_err,x_rate) \
		__asm__ __volatile__ \
		( \
			"MOVLPD %2, %%XMM0\n" \
			"MOVLPD %3, %%XMM2\n" \
			"SHUFPD $0, %%XMM0, %%XMM0\n" \
			"CVTDQ2PD (%0), %%XMM7\n" \
			"SHUFPD $0, %%XMM2, %%XMM2\n" \
			"CVTDQ2PD (%1), %%XMM3\n" \
			"MULPD %%XMM7, %%XMM7\n" \
			"MULPD %%XMM0, %%XMM7\n" \
			"MULPD %%XMM2, %%XMM3\n" \
			"ADDPD %%XMM3, %%XMM7\n" \
			: \
			: \
			"a"(err), \
			"b"(rate), \
			"m"(f_temp), \
			"m"(tr_quant->f_lambda) \
			: \
		) ; \

#if USE_SSE2


#define x265_tr_quant_x_get_coded_level1() \
{ \
	if ( i_max_abs_level > 1 ) \
	{ \
\
		x265_tr_quant_load_zero_to_xmm() ; \
		for( i_abs_level1 = i_max_abs_level; \
				i_abs_level1 >= i_max_abs_level - 1 ; \
				i_abs_level1-- ) \
		{ \
			x265_tr_quant_x_get_ic_rate_cost2 (rate[i_max_abs_level-i_abs_level1]) ; \
			err[i_max_abs_level-i_abs_level1] = i_level_double - ( i_abs_level1 << i_q_bits ) ; \
		} \
		x265_tr_quant_calc_cost2(i_err1,f_rate2) ; \
\
		__asm__ __volatile__ \
		( \
			"MOVDQA %%XMM7, %%XMM0\n" \
			"SHUFPD $3, %%XMM7, %%XMM7\n" \
			"MOV %0, %%edx\n" \
			"SUB $1, %%edx\n" \
			"COMISD %%XMM0, %%XMM7\n" \
			"JB x265_tr_quant_x_find_coded_level1_label1\n" \
			"MOVQ %%XMM0, %%XMM7\n" \
			"ADD $1, %%edx\n" \
			"x265_tr_quant_x_find_coded_level1_label1:\n" \
			"MOV %%edx, %2\n" \
			"MOVLPD %1, %%XMM0\n" \
			"ADDSD %%XMM7, %%XMM0\n" \
			"MOVLPD %%XMM0, %1\n" \
			: \
			: \
			"m"(i_max_abs_level), \
			"m"(f_base_cost), \
			"m"(i_level) \
			: \
			"edx" \
		) ; \
\
	} \
	else \
	{ \
		x265_tr_quant_load_zero_to_xmm() ; \
		rate[0] = 32768 + tr_quant->est_bits_sbac->greater_one_bits[ i_one_ctx ][ 0 ]; \
		err[0] = i_level_double - ( 1 << i_q_bits ) ; \
		x265_tr_quant_calc_cost1(i_err1,i_rate2) ; \
\
		__asm__ __volatile__ \
		( \
			"MOVLPD %0, %%XMM0\n" \
			"ADDSD %%XMM7, %%XMM0\n" \
			"MOVLPD %%XMM0, %0\n" \
			"MOV $1, %%edx\n" \
			"MOV %%edx, %1\n" \
			: \
			: \
			"m"(f_base_cost), \
			"m"(i_level) \
			: \
			"edx"\
		) ; \
\
	\
	} \
\
}

#define x265_tr_quant_x_get_coded_level2() \
{ \
	if( i_max_abs_level == 0 ) \
	{ \
		i_level = 0 ; \
        __asm__ __volatile__ \
        ( \
			"CVTDQ2PD (%0), %%XMM6\n" \
        	"MULSD %1, %%XMM6\n" \
        	"MOVLPD %3, %%XMM7\n" \
        	"ADDSD %%XMM6, %%XMM7\n" \
        	"MOVLPD %2, %%XMM0\n" \
        	"ADDSD %%XMM7, %%XMM0\n" \
        	"MOVLPD %%XMM0, %2\n" \
        	: \
        	: \
            "a"(&tr_quant->est_bits_sbac->significant_bits[ctx_sig_array[i_blk_pos]][0]), \
            "m"(tr_quant->f_lambda), \
            "m"(f_base_cost), \
            "m"(cost_coeff0[ i_blk_pos ]) \
            : \
        ) ; \
\
	} \
	else if( i_max_abs_level == 1 ) \
	{ \
        __asm__ __volatile__ \
        ( \
			"CVTDQ2PD (%1), %%XMM4\n" \
			"MOVDQA %%XMM4, %%XMM6\n" \
			"SHUFPD $3, %%XMM6, %%XMM6\n" \
        	"MOVLPD (%0), %%XMM0\n" \
        	"MULSD %%XMM0, %%XMM4\n" \
        	"MULSD %%XMM0, %%XMM6\n" \
        	"MOVLPD (%2), %%XMM5\n" \
        	"ADDSD %%XMM4, %%XMM5\n" \
        	: \
        	: \
            "a"(&tr_quant->f_lambda), \
            "b"(&tr_quant->est_bits_sbac->significant_bits[ctx_sig_array[i_blk_pos]][0]), \
			"c"(&cost_coeff0[ i_blk_pos ]) \
            : \
        ) ; \
\
        i_abs_level1 = 1 ; \
		x265_tr_quant_x_get_ic_rate_cost2 (rate[0]) ; \
		err[0] = i_level_double - ( 1 << i_q_bits ) ; \
		x265_tr_quant_calc_cost1(i_err1,f_rate2) ; \
\
		__asm__ __volatile__ \
		( \
			"MOV $1, %%edx\n" \
        	"ADDSD %%XMM6, %%XMM7\n" \
			"COMISD %%XMM5, %%XMM7\n" \
			"JB x265_tr_quant_x_find_coded_level1_label3\n" \
        	"MOVQ %%XMM4, %%XMM6\n" \
        	"MOVQ %%XMM5, %%XMM7\n" \
			"SUB $1, %%edx\n" \
			"x265_tr_quant_x_find_coded_level1_label3:\n" \
			"MOV %%edx, %0\n" \
			"MOVLPD %1, %%XMM0\n" \
			"ADDSD %%XMM7, %%XMM0\n" \
			"MOVLPD %%XMM0, %1\n" \
			: \
			: \
			"m"(i_level), \
			"m"(f_base_cost) \
			: \
			"edx" \
		) ; \
\
	} \
	else if( i_max_abs_level == 2 ) \
	{ \
        __asm__ __volatile__ \
        ( \
			"CVTDQ2PD (%1), %%XMM4\n" \
			"MOVDQA %%XMM4, %%XMM6\n" \
			"SHUFPD $3, %%XMM6, %%XMM6\n" \
        	"MOVLPD (%0), %%XMM0\n" \
        	"MULSD %%XMM0, %%XMM4\n" \
        	"MULSD %%XMM0, %%XMM6\n" \
        	"MOVLPD (%2), %%XMM5\n" \
        	"ADDSD %%XMM4, %%XMM5\n" \
        	: \
        	: \
            "a"(&tr_quant->f_lambda), \
            "b"(&tr_quant->est_bits_sbac->significant_bits[ctx_sig_array[i_blk_pos]][0]), \
			"c"(&cost_coeff0[ i_blk_pos ]) \
            : \
        ) ; \
\
		for( i_abs_level1 = i_max_abs_level; \
				i_abs_level1 >= i_max_abs_level - 1 ; \
				i_abs_level1-- ) \
		{ \
			x265_tr_quant_x_get_ic_rate_cost2 (rate[i_max_abs_level-i_abs_level1]) ; \
			err[i_max_abs_level-i_abs_level1] = i_level_double - ( i_abs_level1 << i_q_bits ) ; \
		} \
		x265_tr_quant_calc_cost2(i_err1,f_rate2) ; \
	\
		__asm__ __volatile__ \
		( \
			"MOV %0, %%edx\n" \
			"SUB $1, %%edx\n" \
			"MOVDQA %%XMM7, %%XMM0\n" \
			"SHUFPD $3, %%XMM7, %%XMM7\n" \
			"COMISD %%XMM0, %%XMM7\n" \
			"JB x265_tr_quant_x_find_coded_level1_label4\n" \
			"MOVQ %%XMM0, %%XMM7\n" \
			"ADD $1, %%edx\n" \
			"x265_tr_quant_x_find_coded_level1_label4:\n" \
        	"ADDSD %%XMM6, %%XMM7\n" \
			"COMISD %%XMM5, %%XMM7\n" \
			"JB x265_tr_quant_x_find_coded_level1_label5\n" \
        	"MOVQ %%XMM4, %%XMM6\n" \
        	"MOVQ %%XMM5, %%XMM7\n" \
			"MOV $0, %%edx\n" \
			"x265_tr_quant_x_find_coded_level1_label5:\n" \
			"MOV %%edx, %1\n" \
			"MOVLPD %2, %%XMM0\n" \
			"ADDSD %%XMM7, %%XMM0\n" \
			"MOVLPD %%XMM0, %2\n" \
			: \
			: \
			"m"(i_max_abs_level), \
			"m"(i_level), \
			"m"(f_base_cost) \
			: \
			"edx" \
		) ; \
	} \
	else \
	{ \
		cost_coeff[ i_scan_pos ] = X265_MAX_DOUBLE; \
		__asm__ __volatile__ \
		( \
			"CVTDQ2PD (%1), %%XMM6\n" \
        	"MULSD (%0), %%XMM6\n" \
			: \
			: \
			"a"(&tr_quant->f_lambda), \
			"b"(&tr_quant->est_bits_sbac->significant_bits[ctx_sig_array[i_blk_pos]][1]) \
			: \
		) ; \
\
		for( i_abs_level1 = i_max_abs_level; \
				i_abs_level1 >= i_max_abs_level - 1 ; \
				i_abs_level1-- ) \
		{ \
			x265_tr_quant_x_get_ic_rate_cost2 (rate[i_max_abs_level-i_abs_level1]) ; \
			err[i_max_abs_level-i_abs_level1] = i_level_double - ( i_abs_level1 << i_q_bits ) ; \
		} \
		x265_tr_quant_calc_cost2(i_err1,f_rate2) ; \
\
		__asm__ __volatile__ \
		( \
			"MOVDQA %%XMM7, %%XMM0\n" \
			"SHUFPD $3, %%XMM7, %%XMM7\n" \
			"MOV %0, %%edx\n" \
			"SUB $1, %%edx\n" \
			"COMISD %%XMM0, %%XMM7\n" \
			"JB x265_tr_quant_x_find_coded_level1_label2\n" \
			"MOVQ %%XMM0, %%XMM7\n" \
			"ADD $1, %%edx\n" \
			"x265_tr_quant_x_find_coded_level1_label2:\n" \
        	"ADDSD %%XMM6, %%XMM7\n" \
			"MOV %%edx, %2\n" \
			"MOVLPD %1, %%XMM0\n" \
			"ADDSD %%XMM7, %%XMM0\n" \
			"MOVLPD %%XMM0, %1\n" \
			: \
			: \
			"m"(i_max_abs_level), \
			"m"(f_base_cost), \
			"m"(i_level) \
			: \
			"edx" \
		) ; \
	} \
\
	\
}

#define x265_tr_quant_add_rd_stats_f_sig_cost() \
		__asm__ __volatile__ \
		( \
			"MOVLPD %0, %%XMM0\n" \
			"ADDSD %%XMM6, %%XMM0\n" \
			"MOVLPD %%XMM0, %0\n" \
			: \
			: \
			"m"(rd_stats.f_sig_cost) \
			: \
		) ; \

#define x265_tr_quant_add_rd_stats_f_coded_leveland_dist() \
		__asm__ __volatile__ \
		( \
			"MOVLPD %0, %%XMM0\n" \
			"ADDSD %%XMM7, %%XMM0\n" \
			"SUBSD %%XMM6, %%XMM0\n" \
			"MOVLPD %%XMM0, %0\n" \
			: \
			: \
			"m"(rd_stats.f_coded_leveland_dist) \
			: \
		) ; \


#define x265_tr_quant_add_rd_stats_cost() \
		__asm__ __volatile__ \
		( \
			"MOVLPD %3, %%XMM3\n" \
			"MOVLPD %2, %%XMM2\n" \
			"MOVLPD %1, %%XMM1\n" \
			"MOVLPD (%0), %%XMM0\n" \
			"ADDSD %%XMM1, %%XMM0\n" \
			"SUBSD %%XMM2, %%XMM0\n" \
			"SUBSD %%XMM3, %%XMM0\n" \
			"MOVLPD %%XMM0, (%0)\n" \
			: \
			: \
			"a"(&f_cost_zero_cg), \
			"m"(rd_stats.f_uncoded_dist), \
			"m"(rd_stats.f_coded_leveland_dist), \
			"m"(rd_stats.f_sig_cost) \
			: \
		) ;

#define x265_tr_quant_x_get_rate_last1(pos_x4,pos_y4) \
{ \
	i_ctx_x4 = group_idx[pos_x4]; \
	i_ctx_y4 = group_idx[pos_y4]; \
	f_rate2 = tr_quant->est_bits_sbac->last_x_bits[ i_ctx_x4 ] \
	         + tr_quant->est_bits_sbac->last_y_bits[ i_ctx_y4 ]; \
\
	if( i_ctx_x4 > 3 ) \
	{ \
		f_rate2 += (((i_ctx_x4-2)>>1) << 15); \
	} \
	if( i_ctx_y4 > 3 ) \
	{ \
		f_rate2 += (((i_ctx_y4-2)>>1) << 15); \
	} \
	__asm__ __volatile__ \
	( \
		"MOVLPD %4, %%XMM3\n" \
		"MOVLPD (%3), %%XMM2\n" \
		"CVTDQ2PD (%2), %%XMM1\n" \
		"MOVLPD (%1), %%XMM0\n" \
		"MULSD %%XMM1, %%XMM0\n" \
		"ADDSD %%XMM2, %%XMM0\n" \
		"SUBSD %%XMM3, %%XMM0\n" \
		"MOVLPD %%XMM0, (%0)\n" \
		: \
		: \
		"a"(&f_total_cost), \
		"b"(&tr_quant->f_lambda), \
		"c"(&f_rate2), \
		"d"(&f_base_cost), \
		"m"(cost_sig[ i_scan_pos ]) \
		: \
	) ; \
	\
}

#else


#define x265_tr_quant_x_get_coded_level1() \
{ \
	if ( i_max_abs_level > 1 ) \
	{ \
\
		x265_tr_quant_load_zero_to_fpu() ; \
		for( i_abs_level1 = i_max_abs_level; \
				i_abs_level1 >= i_max_abs_level - 1 ; \
				i_abs_level1-- ) \
		{ \
			x265_tr_quant_x_get_ic_rate_cost2 (f_rate2) ; \
			i_err1 = i_level_double - ( i_abs_level1 << i_q_bits ) ; \
			x265_tr_quant_calc_cost(i_err1,f_rate2) ; \
		} \
	\
		__asm__ __volatile__ \
		( \
			"MOV %0, %%edx\n" \
			"SUB $1, %%edx\n" \
			"FUCOMI %%ST(1)\n" \
			"JB x265_tr_quant_x_find_coded_level1_label1\n" \
			"FSTP %%ST(0)\n" \
			"ADD $1, %%edx\n" \
			"JMP x265_tr_quant_x_find_coded_level1_exit1\n" \
			"x265_tr_quant_x_find_coded_level1_label1:\n" \
			"FSTP %%ST(1)\n" \
			"x265_tr_quant_x_find_coded_level1_exit1:\n" \
			"MOV %%edx, %1\n" \
			"FLDL %2\n" \
			"FADD %%ST(1), %%ST(0)\n" \
			"FSTPL %2\n" \
			: \
			: \
			"m"(i_max_abs_level), \
			"m"(i_level), \
			"m"(f_base_cost) \
			: \
			"edx" \
		) ; \
\
	} \
	else \
	{ \
		x265_tr_quant_load_zero_to_fpu() ; \
		i_rate2 = 32768 + tr_quant->est_bits_sbac->greater_one_bits[ i_one_ctx ][ 0 ]; \
		i_err1 = i_level_double - ( 1 << i_q_bits ) ; \
		x265_tr_quant_calc_cost(i_err1,i_rate2) ; \
\
		__asm__ __volatile__ \
		( \
			"FLDL %0\n" \
			"FADD %%ST(1), %%ST(0)\n" \
			"FSTPL %0\n" \
			"MOV $1, %%edx\n" \
			"MOV %%edx, %1\n" \
			: \
			: \
			"m"(f_base_cost), \
			"m"(i_level) \
			: \
			"edx"\
		) ; \
\
	\
	} \
\
}

#define x265_tr_quant_x_get_coded_level2() \
{ \
	if( i_max_abs_level == 0 ) \
	{ \
		i_level = 0 ; \
        __asm__ __volatile__ \
        ( \
            "FILDL (%1)\n" \
            "FLDL (%2)\n" \
        	"FMULP %%ST(0), %%ST(1)\n" \
            "FLDL (%3)\n" \
        	"FADD %%ST(1), %%ST(0)\n" \
			"FLDL (%0)\n" \
			"FADD %%ST(1), %%ST(0)\n" \
			"FSTPL (%0)\n" \
        	: \
        	: \
            "a"(&f_base_cost), \
            "b"(&tr_quant->est_bits_sbac->significant_bits[ctx_sig_array[i_blk_pos]][0]), \
            "c"(&tr_quant->f_lambda), \
            "d"(&cost_coeff0[ i_blk_pos ]) \
            : \
        ) ; \
\
	} \
	else if( i_max_abs_level == 1 ) \
	{ \
        __asm__ __volatile__ \
        ( \
            "FILDL (%1)\n" \
            "FLDL (%0)\n" \
        	"FMULP %%ST(0), %%ST(1)\n" \
            "FLDL (%3)\n" \
        	"FADD %%ST(1), %%ST(0)\n" \
        	"FILDL (%2)\n" \
            "FLDL (%0)\n" \
        	"FMULP %%ST(0), %%ST(1)\n" \
        	: \
        	: \
            "a"(&tr_quant->f_lambda), \
            "b"(&tr_quant->est_bits_sbac->significant_bits[ctx_sig_array[i_blk_pos]][0]), \
            "c"(&tr_quant->est_bits_sbac->significant_bits[ctx_sig_array[i_blk_pos]][1]), \
			"d"(&cost_coeff0[ i_blk_pos ]) \
            : \
        ) ; \
\
        i_abs_level1 = 1 ; \
		x265_tr_quant_x_get_ic_rate_cost2 (f_rate2) ; \
		i_err1 = i_level_double - ( 1 << i_q_bits ) ; \
		x265_tr_quant_calc_cost(i_err1,f_rate2) ; \
\
		__asm__ __volatile__ \
		( \
			"MOV $1, %%edx\n" \
			"FADD %%ST(1), %%ST(0)\n" \
			"FUCOMI %%ST(2)\n" \
			"JB x265_tr_quant_x_find_coded_level1_label3\n" \
			"FSTP %%ST(0)\n" \
			"FSTP %%ST(0)\n" \
			"SUB $1, %%edx\n" \
			"JMP x265_tr_quant_x_find_coded_level1_exit3\n" \
			"x265_tr_quant_x_find_coded_level1_label3:\n" \
			"FSTP %%ST(2)\n" \
			"FSTP %%ST(2)\n" \
			"x265_tr_quant_x_find_coded_level1_exit3:\n" \
			"MOV %%edx, %0\n" \
			"FLDL %1\n" \
			"FADD %%ST(1), %%ST(0)\n" \
			"FSTPL %1\n" \
			: \
			: \
			"m"(i_level), \
			"m"(f_base_cost) \
			: \
			"edx" \
		) ; \
\
	} \
	else if( i_max_abs_level == 2 ) \
	{ \
        __asm__ __volatile__ \
        ( \
            "FILDL (%1)\n" \
            "FLDL (%0)\n" \
        	"FMULP %%ST(0), %%ST(1)\n" \
            "FLDL (%3)\n" \
        	"FADD %%ST(1), %%ST(0)\n" \
        	"FILDL (%2)\n" \
            "FLDL (%0)\n" \
        	"FMULP %%ST(0), %%ST(1)\n" \
        	: \
        	: \
            "a"(&tr_quant->f_lambda), \
            "b"(&tr_quant->est_bits_sbac->significant_bits[ctx_sig_array[i_blk_pos]][0]), \
            "c"(&tr_quant->est_bits_sbac->significant_bits[ctx_sig_array[i_blk_pos]][1]), \
			"d"(&cost_coeff0[ i_blk_pos ]) \
            : \
        ) ; \
		for( i_abs_level1 = i_max_abs_level; \
				i_abs_level1 >= i_max_abs_level - 1 ; \
				i_abs_level1-- ) \
		{ \
			x265_tr_quant_x_get_ic_rate_cost2 (f_rate2) ; \
			i_err1 = i_level_double - ( i_abs_level1 << i_q_bits ) ; \
			x265_tr_quant_calc_cost(i_err1,f_rate2) ; \
		} \
	\
		__asm__ __volatile__ \
		( \
			"MOV %0, %%edx\n" \
			"SUB $1, %%edx\n" \
			"FUCOMI %%ST(1)\n" \
			"JB x265_tr_quant_x_find_coded_level1_label4\n" \
			"FSTP %%ST(0)\n" \
			"ADD $1, %%edx\n" \
			"JMP x265_tr_quant_x_find_coded_level1_exit4\n" \
			"x265_tr_quant_x_find_coded_level1_label4:\n" \
			"FSTP %%ST(1)\n" \
			"x265_tr_quant_x_find_coded_level1_exit4:\n" \
			"FADD %%ST(1), %%ST(0)\n" \
			"FUCOMI %%ST(2)\n" \
			"JB x265_tr_quant_x_find_coded_level1_label5\n" \
			"FSTP %%ST(0)\n" \
			"FSTP %%ST(0)\n" \
			"MOV $0, %%edx\n" \
			"JMP x265_tr_quant_x_find_coded_level1_exit5\n" \
			"x265_tr_quant_x_find_coded_level1_label5:\n" \
			"FSTP %%ST(2)\n" \
			"FSTP %%ST(2)\n" \
			"x265_tr_quant_x_find_coded_level1_exit5:\n" \
			"MOV %%edx, %1\n" \
			"FLDL %2\n" \
			"FADD %%ST(1), %%ST(0)\n" \
			"FSTPL %2\n" \
			: \
			: \
			"m"(i_max_abs_level), \
			"m"(i_level), \
			"m"(f_base_cost) \
			: \
			"edx" \
		) ; \
	} \
	else \
	{ \
        __asm__ __volatile__ \
        ( \
            "FILDL (%0)\n" \
            "FLDL (%1)\n" \
        	"FMULP %%ST(0), %%ST(1)\n" \
        	: \
        	: \
            "a"(&tr_quant->est_bits_sbac->significant_bits[ctx_sig_array[i_blk_pos]][1]), \
            "b"(&tr_quant->f_lambda) \
            : \
        ) ; \
		for( i_abs_level1 = i_max_abs_level; \
				i_abs_level1 >= i_max_abs_level - 1 ; \
				i_abs_level1-- ) \
		{ \
			x265_tr_quant_x_get_ic_rate_cost2 (f_rate2) ; \
			i_err1 = i_level_double - ( i_abs_level1 << i_q_bits ) ; \
			x265_tr_quant_calc_cost(i_err1,f_rate2) ; \
		} \
	\
		__asm__ __volatile__ \
		( \
			"MOV %0, %%edx\n" \
			"SUB $1, %%edx\n" \
			"FUCOMI %%ST(1)\n" \
			"JB x265_tr_quant_x_find_coded_level1_label2\n" \
			"FSTP %%ST(0)\n" \
			"ADD $1, %%edx\n" \
			"JMP x265_tr_quant_x_find_coded_level1_exit2\n" \
			"x265_tr_quant_x_find_coded_level1_label2:\n" \
			"FSTP %%ST(1)\n" \
			"x265_tr_quant_x_find_coded_level1_exit2:\n" \
			"FADD %%ST(1), %%ST(0)\n" \
			"MOV %%edx, %1\n" \
			"FLDL %2\n" \
			"FADD %%ST(1), %%ST(0)\n" \
			"FSTPL %2\n" \
			: \
			: \
			"m"(i_max_abs_level), \
			"m"(i_level), \
			"m"(f_base_cost) \
			: \
			"edx" \
		) ; \
	} \
\
	\
}

#define x265_tr_quant_add_rd_stats_f_sig_cost() \
		__asm__ __volatile__ \
		( \
			"FLDL %0\n" \
			"FADD %%ST(2), %%ST(0)\n" \
			"FSTPL %0\n" \
			: \
			: \
			"m"(rd_stats.f_sig_cost) \
			: \
		) ; \

#define x265_tr_quant_add_rd_stats_f_coded_leveland_dist() \
		__asm__ __volatile__ \
		( \
			"FLDL %0\n" \
			"FADD %%ST(1), %%ST(0)\n" \
			"FSUB %%ST(2), %%ST(0)\n" \
			"FSTPL %0\n" \
			: \
			: \
			"m"(rd_stats.f_coded_leveland_dist) \
			: \
		) ; \

#define x265_tr_quant_add_rd_stats_cost() \
		__asm__ __volatile__ \
		( \
			"FLDL %3\n" \
			"FLDL %2\n" \
			"FLDL %1\n" \
			"FLDL (%0)\n" \
			"FADDP %%ST(0), %%ST(1)\n" \
			"FSUBP %%ST(0), %%ST(1)\n" \
			"FSUBP %%ST(0), %%ST(1)\n" \
			"FSTPL (%0)\n" \
			: \
			: \
			"a"(&f_cost_zero_cg), \
			"m"(rd_stats.f_uncoded_dist), \
			"m"(rd_stats.f_coded_leveland_dist), \
			"m"(rd_stats.f_sig_cost) \
			: \
		) ;

#define x265_tr_quant_x_get_rate_last1(pos_x4,pos_y4) \
{ \
	i_ctx_x4 = group_idx[pos_x4]; \
	i_ctx_y4 = group_idx[pos_y4]; \
	f_rate2 = tr_quant->est_bits_sbac->last_x_bits[ i_ctx_x4 ] \
	         + tr_quant->est_bits_sbac->last_y_bits[ i_ctx_y4 ]; \
\
	if( i_ctx_x4 > 3 ) \
	{ \
		f_rate2 += (((i_ctx_x4-2)>>1) << 15); \
	} \
	if( i_ctx_y4 > 3 ) \
	{ \
		f_rate2 += (((i_ctx_y4-2)>>1) << 15); \
	} \
	__asm__ __volatile__ \
	( \
		"FLDL %4\n" \
		"FLDL (%3)\n" \
		"FILDL (%2)\n" \
		"FLDL (%1)\n" \
		"FMULP %%ST(0), %%ST(1)\n" \
		"FADDP %%ST(0), %%ST(1)\n" \
		"FSUBP %%ST(0), %%ST(1)\n" \
		"FSTPL (%0)\n" \
		: \
		: \
		"a"(&f_total_cost), \
		"b"(&tr_quant->f_lambda), \
		"c"(&f_rate2), \
		"d"(&f_base_cost), \
		"m"(cost_sig[ i_scan_pos ]) \
		: \
	) ; \
	\
}

#endif


#define x265_tr_quant_x_get_ic_rate1(rate_to_get) \
		rate_to_get = 0 ; \
		i_base_level = (c1_idx_array[i_blk_pos] < X265_C1FLAG_NUMBER) ? (2 + (c2_idx_array[i_blk_pos] < X265_C2FLAG_NUMBER)) : 1; \
		if ( i_abs_level3 >= i_base_level ) \
		{ \
			i_symbol2 = i_abs_level3 - i_base_level; \
			i_max_vlc3 = go_rice_range[ go_rice_param_array[i_blk_pos] ]; \
			b_exp_golomb3 = ( i_symbol2 > i_max_vlc3 ); \
\
			if( b_exp_golomb3 ) \
			{ \
				i_abs_level4 = i_symbol2 - i_max_vlc3; \
				i_egs3 = 1; \
				for( i_max3 = 2; i_abs_level4 >= i_max3; i_max3 <<= 1, i_egs3 += 2 ); \
				{ \
					\
				} \
				rate_to_get += i_egs3 << 15; \
				i_symbol2 = X265_MIN ( i_symbol2, ( i_max_vlc3 + 1 ) ); \
			} \
\
			i_pref_len3 = (uint16_t)( i_symbol2 >> go_rice_param_array[i_blk_pos] ) + 1; \
	    	i_num_bins3 = X265_MIN( i_pref_len3, go_rice_prefix_len[ go_rice_param_array[i_blk_pos] ] ) + go_rice_param_array[i_blk_pos]; \
\
	    	rate_to_get += i_num_bins3 << 15; \
\
	    	if (c1_idx_array[i_blk_pos] < X265_C1FLAG_NUMBER) \
	    	{ \
	    		rate_to_get += tr_quant->est_bits_sbac->greater_one_bits[ one_ctx_array[i_blk_pos] ][ 1 ]; \
\
	    		if (c2_idx_array[i_blk_pos] < X265_C2FLAG_NUMBER) \
	    		{ \
	    			rate_to_get += tr_quant->est_bits_sbac->level_abs_bits[ abs_ctx_array[i_blk_pos] ][ 1 ]; \
	    		} \
	    	} \
		} \
		else \
		{ \
			if( i_abs_level3 == 0 ) \
			{ \
				rate_to_get = 0; \
			} \
			else if( i_abs_level3 == 1 ) \
			{ \
				rate_to_get += tr_quant->est_bits_sbac->greater_one_bits[ one_ctx_array[i_blk_pos] ][ 0 ]; \
			} \
			else if( i_abs_level3 == 2 ) \
			{ \
				rate_to_get += tr_quant->est_bits_sbac->greater_one_bits[ one_ctx_array[i_blk_pos] ][ 1 ]; \
				rate_to_get += tr_quant->est_bits_sbac->level_abs_bits[ abs_ctx_array[i_blk_pos] ][ 0 ]; \
			} \
			else \
			{ \
				assert(0); \
			} \
		} \


#endif

