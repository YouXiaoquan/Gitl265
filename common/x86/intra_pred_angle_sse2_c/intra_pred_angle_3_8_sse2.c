


#include "common/common.h"
#include "intra_pred_x86.h"
#include <emmintrin.h>

#define		STORE_ANGLE_MINUS_32(loop_num,loop_size) \
		for ( j = 0 ; j < loop_size ; ++ j ) \
        { \
        	for ( i = loop_num ; i > 0 ; -- i ) \
        	{ \
        		temp_dst = dst + loop_size * (loop_num - i) ; \
    			itmp = _mm_loadu_si128((__m128i const*)(refMain + loop_size * (loop_num-i))); \
        		for ( k = 0 ; k < i ; ++ k ) \
        		{ \
					_mm_storeu_si128((__m128i*)temp_dst, itmp); \
					temp_dst += ((loop_size * i_dst_stride) + loop_size ) ; \
        		} \
        	} \
        	dst += i_dst_stride ; \
        	refMain -- ; \
        } \
\
        for ( i = 0 ; i < loop_num - 1 ; ++ i ) \
        { \
        	for ( j = 0 ; j < loop_size ; ++ j ) \
        	{ \
        		temp_dst = dst ; \
    			itmp = _mm_loadu_si128((__m128i const*)refMain); \
        		for ( k = 0 ; k < loop_num - 1 - i ; ++ k ) \
        		{ \
					_mm_storeu_si128((__m128i*)temp_dst, itmp); \
					temp_dst += ((loop_size * i_dst_stride) + loop_size ) ; \
        		} \
        		dst += i_dst_stride ; \
        		refMain -- ; \
        	} \
        }

#define		STORE_ANGLE_32(loop_num,loop_size) \
        for ( i = 0 ; i < loop_num - 1 ; ++ i ) \
        { \
        	for ( j = 0 ; j < loop_size ; ++ j ) \
        	{ \
        		temp_dst = dst ; \
    			itmp = _mm_loadu_si128((__m128i const*)refMain); \
        		for ( k = 0 ; k <= i ; ++ k ) \
        		{ \
					_mm_storeu_si128((__m128i*)temp_dst, itmp); \
					temp_dst -= ((loop_size * i_dst_stride) - loop_size ) ; \
        		} \
        		dst += i_dst_stride ; \
        		refMain ++ ; \
        	} \
        } \
\
        for ( j = 0 ; j < loop_size ; ++ j ) \
        { \
        	for ( i = loop_num ; i > 0 ; -- i ) \
        	{ \
        		temp_dst = dst + loop_size * (loop_num - i) ; \
    			itmp = _mm_loadu_si128((__m128i const*)(refMain + loop_size * (loop_num-i))); \
        		for ( k = 0 ; k < i ; ++ k ) \
        		{ \
					_mm_storeu_si128((__m128i*)temp_dst, itmp); \
					temp_dst -= ((loop_size * i_dst_stride) - loop_size ) ; \
        		} \
        	} \
        	dst += i_dst_stride ; \
        	refMain ++ ; \
        }

#define GETAP(X, Y) angAP[8 - (X)][(Y)]

x265_pred_angle_help_func x265_pred_angle_help_3_8_sse2[35] =
{
		0,
		0,
		0,
		x265_intra_pred_angle_h_26_3_8_sse2,
		x265_intra_pred_angle_h_21_3_8_sse2,
		x265_intra_pred_angle_h_17_3_8_sse2,
		x265_intra_pred_angle_h_13_3_8_sse2,
		x265_intra_pred_angle_h_9_3_8_sse2,
		x265_intra_pred_angle_h_5_3_8_sse2,
		x265_intra_pred_angle_h_2_3_8_sse2,
		0,
		x265_intra_pred_angle_h_m2_3_8_sse2,
		x265_intra_pred_angle_h_m5_3_8_sse2,
		x265_intra_pred_angle_h_m9_3_8_sse2,
		x265_intra_pred_angle_h_m13_3_8_sse2,
		x265_intra_pred_angle_h_m17_3_8_sse2,
		x265_intra_pred_angle_h_m21_3_8_sse2,
		x265_intra_pred_angle_h_m26_3_8_sse2,
		0,
		x265_intra_pred_angle_v_m26_3_8_sse2,
		x265_intra_pred_angle_v_m21_3_8_sse2,
		x265_intra_pred_angle_v_m17_3_8_sse2,
		x265_intra_pred_angle_v_m13_3_8_sse2,
		x265_intra_pred_angle_v_m9_3_8_sse2,
		x265_intra_pred_angle_v_m5_3_8_sse2,
		x265_intra_pred_angle_v_m2_3_8_sse2,
		0,
		x265_intra_pred_angle_v_2_3_8_sse2,
		x265_intra_pred_angle_v_5_3_8_sse2,
		x265_intra_pred_angle_v_9_3_8_sse2,
		x265_intra_pred_angle_v_13_3_8_sse2,
		x265_intra_pred_angle_v_17_3_8_sse2,
		x265_intra_pred_angle_v_21_3_8_sse2,
		x265_intra_pred_angle_v_26_3_8_sse2,
};


#if HIGH_BIT_DEPTH


#define LOADROW(row, X) { \
        /*tmp.load(refMain + 1 + (X)); */ \
        row = _mm_loadu_si128((__m128i const*)(refMain + 1 + (X))); \
}

#define STORE_VECTOR(R) \
        _mm_storeu_si128((__m128i*)dst, R); \
        dst += i_dst_stride;

#define MB8(R1, R2, R3, R4, R5, R6, R7, R8) { \
        itmp1 = _mm_unpacklo_epi16(R1, R2); \
        itmp2 = _mm_unpackhi_epi16(R1, R2); \
        R1 = itmp1; \
        R2 = itmp2; \
        itmp1 = _mm_unpacklo_epi16(R3, R4); \
        itmp2 = _mm_unpackhi_epi16(R3, R4); \
        R3 = itmp1; \
        R4 = itmp2; \
        itmp1 = _mm_unpacklo_epi32(R1, R3); \
        itmp2 = _mm_unpackhi_epi32(R1, R3); \
        R1 = itmp1; \
        R3 = itmp2; \
        itmp1 = _mm_unpacklo_epi32(R2, R4); \
        itmp2 = _mm_unpackhi_epi32(R2, R4); \
        R2 = itmp1; \
        R4 = itmp2; \
        itmp1 = _mm_unpacklo_epi16(R5, R6); \
        itmp2 = _mm_unpackhi_epi16(R5, R6); \
        R5 = itmp1; \
        R6 = itmp2; \
        itmp1 = _mm_unpacklo_epi16(R7, R8); \
        itmp2 = _mm_unpackhi_epi16(R7, R8); \
        R7 = itmp1; \
        R8 = itmp2; \
        itmp1 = _mm_unpacklo_epi32(R5, R7); \
        itmp2 = _mm_unpackhi_epi32(R5, R7); \
        R5 = itmp1; \
        R7 = itmp2; \
        itmp1 = _mm_unpacklo_epi32(R6, R8); \
        itmp2 = _mm_unpackhi_epi32(R6, R8); \
        R6 = itmp1; \
        R8 = itmp2; \
        itmp1 = _mm_unpacklo_epi64(R1, R5); \
        itmp2 = _mm_unpackhi_epi64(R1, R5); \
        R1 = itmp1; \
        R5 = itmp2; \
  \
        itmp1 = _mm_unpacklo_epi64(R2, R6); \
        itmp2 = _mm_unpackhi_epi64(R2, R6); \
        R2 = itmp1; \
        R6 = itmp2; \
  \
        itmp1 = _mm_unpacklo_epi64(R3, R7); \
        itmp2 = _mm_unpackhi_epi64(R3, R7); \
        R3 = itmp1; \
        R7 = itmp2; \
  \
        itmp1 = _mm_unpacklo_epi64(R4, R8); \
        itmp2 = _mm_unpackhi_epi64(R4, R8); \
        R4 = itmp1; \
        R8 = itmp2; \
}


#define BLND2_2(R1, R2) { \
        /*tmp1 = blend2q<0, 2>((Vec2q)R1, (Vec2q)R2); \
        tmp2 = blend2q<1, 3>((Vec2q)R1, (Vec2q)R2);*/ \
        itmp1 = _mm_unpacklo_epi64(R1, R2); \
        itmp2 = _mm_unpackhi_epi64(R1, R2); \
        /*tmp1.store(dst); */ \
        _mm_storeu_si128((__m128i*)dst, itmp1); \
        dst += i_dst_stride; \
        /*tmp2.store(dst);*/ \
        _mm_storeu_si128((__m128i*)dst, itmp2); \
        dst += i_dst_stride; \
}

#define PREDANG_CALCROW_VER(X) { \
        v_deltaPos = _mm_add_epi16(v_deltaPos, v_ipAngle); \
        v_deltaFract = _mm_and_si128(v_deltaPos, thirty1); \
        it1 = _mm_sub_epi16(thirty2, v_deltaFract); \
        i16 = _mm_set1_epi16(16); \
        for ( j = 0 ; j < 4 ; ++ j ) \
        { \
        	row1 = _mm_loadu_si128((__m128i const*)(refMain + 1 + (angAP[8 - (lookIdx)][(X)] + 8 * j))); \
        	row2 = _mm_loadu_si128((__m128i const*)(refMain + 1 + (angAP[8 - (lookIdx)][(X)] + 8 * j + 1))); \
        	\
			it2 = _mm_mullo_epi16(it1, row1); \
			it3 = _mm_mullo_epi16(v_deltaFract, row2); \
			it2 = _mm_add_epi16(it2, it3); \
			it2 = _mm_add_epi16(it2, i16); \
			itmp = _mm_srai_epi16(it2, 5); \
			_mm_storeu_si128((__m128i*)(dst + ((X)*i_dst_stride) + 8 * j), itmp); \
        } \
}

#define PREDANG_CALCROW_VER_MODE2(X) { \
        v_deltaPos = _mm_add_epi16(v_deltaPos, v_ipAngle); \
        v_deltaFract = _mm_and_si128(v_deltaPos, thirty1); \
        it1 = _mm_sub_epi16(thirty2, v_deltaFract); \
        i16 = _mm_set1_epi16(16); \
        it2 = _mm_mullo_epi16(it1, row1); \
        it3 = _mm_mullo_epi16(v_deltaFract, row2); \
        it2 = _mm_add_epi16(it2, it3); \
        it2 = _mm_add_epi16(it2, i16); \
        itmp = _mm_srai_epi16(it2, 5); \
        _mm_storeu_si128((__m128i*)(dst + ((X)*i_dst_stride)), itmp); \
}

#define PREDANG_CALCROW_HOR(X, rowx) { \
        row1 = _mm_loadu_si128((__m128i const*)(refMain + 1 + (angAP[8 - (lookIdx)][((X))]))); \
  \
        row2 = _mm_loadu_si128((__m128i const*)(refMain + 1 + (angAP[8 - (lookIdx)][((X))] + 1))); \
  \
        v_deltaPos = _mm_add_epi16(v_deltaPos, v_ipAngle); \
        v_deltaFract = _mm_and_si128(v_deltaPos, thirty1); \
        it1 = _mm_sub_epi16(thirty2, v_deltaFract); \
        i16 = _mm_set1_epi16(16); \
        it2 = _mm_mullo_epi16(it1, row1); \
        it3 = _mm_mullo_epi16(v_deltaFract, row2); \
        it2 = _mm_add_epi16(it2, it3); \
        it2 = _mm_add_epi16(it2, i16); \
        rowx = _mm_srai_epi16(it2, 5); \
}

#define PREDANG_CALCROW_HOR_MODE2(rowx) { \
        v_deltaPos = _mm_add_epi16(v_deltaPos, v_ipAngle); \
        v_deltaFract = _mm_and_si128(v_deltaPos, thirty1); \
        it1 = _mm_sub_epi16(thirty2, v_deltaFract); \
        i16 = _mm_set1_epi16(16); \
        it2 = _mm_mullo_epi16(it1, row1); \
        it3 = _mm_mullo_epi16(v_deltaFract, row2); \
        it2 = _mm_add_epi16(it2, it3); \
        it2 = _mm_add_epi16(it2, i16); \
        rowx = _mm_srai_epi16(it2, 5); \
}


#define CALC_BLND_8ROWS(R1, R2, R3, R4, R5, R6, R7, R8, X) { \
        PREDANG_CALCROW_HOR(0 + X, R1) \
        PREDANG_CALCROW_HOR(1 + X, R2) \
        PREDANG_CALCROW_HOR(2 + X, R3) \
        PREDANG_CALCROW_HOR(3 + X, R4) \
        PREDANG_CALCROW_HOR(4 + X, R5) \
        PREDANG_CALCROW_HOR(5 + X, R6) \
        PREDANG_CALCROW_HOR(6 + X, R7) \
}

#define CALC_BLND_8ROWS_MODE2(R1, R2, R3, R4, R5, R6, R7, R8) { \
        PREDANG_CALCROW_HOR_MODE2(R1) \
        PREDANG_CALCROW_HOR_MODE2(R2) \
        PREDANG_CALCROW_HOR_MODE2(R3) \
        PREDANG_CALCROW_HOR_MODE2(R4) \
        PREDANG_CALCROW_HOR_MODE2(R5) \
        PREDANG_CALCROW_HOR_MODE2(R6) \
        PREDANG_CALCROW_HOR_MODE2(R7) \
}

#define CALC_BLND_ROWS_MODE2(row) \
        CALC_BLND_8ROWS_MODE2(R1, R2, R3, R4, R5, R6, R7, R8) \
        PREDANG_CALCROW_HOR_MODE2(R8) \
        MB8(R1, R2, R3, R4, R5, R6, R7, R8) \
		STORE_VECTOR(R1) \
		STORE_VECTOR(R5) \
		STORE_VECTOR(R3) \
		STORE_VECTOR(R7) \
		STORE_VECTOR(R2) \
		STORE_VECTOR(R6) \
		STORE_VECTOR(R4) \
		STORE_VECTOR(R8) \
        CALC_BLND_8ROWS_MODE2(R1, R2, R3, R4, R5, R6, R7, R8) \
        v_deltaPos = _mm_add_epi16(v_deltaPos, v_ipAngle); \
        R8 = row; \
        MB8(R1, R2, R3, R4, R5, R6, R7, R8) \
		STORE_VECTOR(R1) \
		STORE_VECTOR(R5) \
		STORE_VECTOR(R3) \
		STORE_VECTOR(R7) \
		STORE_VECTOR(R2) \
		STORE_VECTOR(R6) \
		STORE_VECTOR(R4) \
		STORE_VECTOR(R8) \

#define CALC_FIRST_BLND_ROWS(start) \
		CALC_BLND_8ROWS(R1, R2, R3, R4, R5, R6, R7, R8, start) \
		PREDANG_CALCROW_HOR(7 + start, R8) ; \
		MB8(R1, R2, R3, R4, R5, R6, R7, R8) ; \
		STORE_VECTOR(R1) \
		STORE_VECTOR(R5) \
		STORE_VECTOR(R3) \
		STORE_VECTOR(R7) \
		STORE_VECTOR(R2) \
		STORE_VECTOR(R6) \
		STORE_VECTOR(R4) \
		STORE_VECTOR(R8) \

#define CALC_SECOND_BLND_ROWS(start) \
		CALC_BLND_8ROWS(R1, R2, R3, R4, R5, R6, R7, R8, start) \
        v_deltaPos = _mm_add_epi16(v_deltaPos, v_ipAngle); \
		R8 = _mm_loadu_si128((__m128i const*)(refMain + 1 + GETAP(lookIdx, start+7))); \
		MB8(R1, R2, R3, R4, R5, R6, R7, R8) \
		STORE_VECTOR(R1) \
		STORE_VECTOR(R5) \
		STORE_VECTOR(R3) \
		STORE_VECTOR(R7) \
		STORE_VECTOR(R2) \
		STORE_VECTOR(R6) \
		STORE_VECTOR(R4) \
		STORE_VECTOR(R8) \

#define CALC_FIRST_BLND_ROWS_MODE2() \
        CALC_BLND_8ROWS_MODE2(R1, R2, R3, R4, R5, R6, R7, R8) \
        PREDANG_CALCROW_HOR_MODE2(R8) \
        MB8(R1, R2, R3, R4, R5, R6, R7, R8) \
		STORE_VECTOR(R1) \
		STORE_VECTOR(R5) \
		STORE_VECTOR(R3) \
		STORE_VECTOR(R7) \
		STORE_VECTOR(R2) \
		STORE_VECTOR(R6) \
		STORE_VECTOR(R4) \
		STORE_VECTOR(R8) \

#define CALC_SECOND_BLND_ROWS_MODE2(row) \
        CALC_BLND_8ROWS_MODE2(R1, R2, R3, R4, R5, R6, R7, R8) \
        v_deltaPos = _mm_add_epi16(v_deltaPos, v_ipAngle); \
        R8 = row; \
        MB8(R1, R2, R3, R4, R5, R6, R7, R8) \
		STORE_VECTOR(R1) \
		STORE_VECTOR(R5) \
		STORE_VECTOR(R3) \
		STORE_VECTOR(R7) \
		STORE_VECTOR(R2) \
		STORE_VECTOR(R6) \
		STORE_VECTOR(R4) \
		STORE_VECTOR(R8) \

void x265_intra_pred_angle_3_8_sse2(pixel *left,
									pixel *top,
									pixel* dst,
									int i_dst_stride,
									int i_chroma_idx,
									int i_mode)
{
    int blkSize = 32;

    // Map the mode index to main prediction direction and angle
    assert(i_mode > 1); //no planar and dc
    int32_t modeHor       = (i_mode < 18);
    int32_t modeVer       = !modeHor;
    int intraPredAngle = modeVer ? (int)i_mode - X265_VER_IDX : modeHor ? -((int)i_mode - X265_HOR_IDX) : 0;
    int absAng         = abs(intraPredAngle);
    int signAng        = intraPredAngle < 0 ? -1 : 1;

    // Set bitshifts and scale the angle parameter to block size
    int angTable[9]    = { 0,    2,    5,   9,  13,  17,  21,  26,  32 };
    int invAngTable[9] = { 0, 4096, 1638, 910, 630, 482, 390, 315, 256 }; // (256 * 32) / Angle
    int invAngle       = invAngTable[absAng];
    absAng             = angTable[absAng];
    intraPredAngle     = signAng * absAng;

    // Do angular predictions

    pixel ref_array[65];
    pixel* refMain;
    pixel* temp_ref_main;
    pixel* refSide;
    pixel* temp_dst;
    int32_t loop = 0 ;
    int32_t i = 0, j = 0, k = 0 ;

    // Initialise the Main and Left reference array.
    if (intraPredAngle < 0)
    {
        refMain = ref_array+32 ;
        if (modeVer)
        {
        	memcpy(refMain, top-1, sizeof(pixel)*33) ;
        }
        else
        {
        	memcpy(refMain, left-1, sizeof(pixel)*33) ;
        }

        refSide = (modeVer ? left-1 : top-1);     // + (blkSize - 1);

        // Extend the Main reference to the left.
        int invAngleSum    = 128;     // rounding for (shift by 8)
        if (intraPredAngle != -32)
        {
        	for (k = -1; k > blkSize * intraPredAngle >> 5; k--)
            {
                invAngleSum += invAngle;
                refMain[k] = refSide[invAngleSum >> 8];
            }
        }
    }
    else
    {
        refMain = modeVer ? top-1 : left-1;
        refSide = modeVer ? left-1  : top-1;
    }

    // bfilter will always be true for blocksize 8
    if (intraPredAngle == 0)  // Exactly hotizontal/vertical angles
    {
        if (modeHor)
        {
        	__m128i tmp1 ;

        	temp_dst = dst ;
        	temp_ref_main = refMain + 1 ;
        	for ( i = 0 ; i < 4 ; ++ i )
        	{
				for ( j = 0 ; j < 8 ; ++ j )
				{
					tmp1 = _mm_set1_epi16(temp_ref_main[j]);
					for ( k = 0 ; k < 4 ; ++ k )
					{
						_mm_storeu_si128((__m128i*)(temp_dst+8*k), tmp1);
					}
					temp_dst += i_dst_stride ;
				}
				temp_ref_main += 8 ;
        	}
        }
        else
        {
            __m128i v_main;

            for ( i = 0 ; i < 4 ; ++ i )
            {
            	temp_dst = dst + 8 * i ;
            	v_main = _mm_loadu_si128((__m128i const*)(refMain + 1 + 8 * i ));
            	for ( j = 0 ; j < 32 ; ++ j )
            	{
            		_mm_storeu_si128((__m128i*)temp_dst, v_main);
            		temp_dst += i_dst_stride ;
            	}
            }
        }
    }
    else if (intraPredAngle == -32)
    {
        for ( loop = -1 ; loop > -32 ; -- loop )
        {
        	refMain[loop] = refSide[-loop] ;
        }

        x265_intra_pred_angle_m32_3_8_sse2 (refMain, dst, i_dst_stride) ;
    }
    else if (intraPredAngle == 32)
    {
        refMain += 2;

        x265_intra_pred_angle_32_3_8_sse2 (refMain, dst, i_dst_stride) ;
    }
    else
    {
        x265_pred_angle_help_3_8_sse2[i_mode] ( refMain + 1, dst, i_dst_stride ) ;
        return;
    }
}

#undef LOADROW
#undef STORE_VECTOR
#undef MB8
#undef BLND2_2
#undef PREDANG_CALCROW_VER
#undef PREDANG_CALCROW_VER_MODE2
#undef PREDANG_CALCROW_HOR
#undef PREDANG_CALCROW_HOR_MODE2
#undef CALC_BLND_8ROWS
#undef CALC_BLND_8ROWS_MODE2
#undef CALC_BLND_ROWS_MODE2
#undef CALC_FIRST_BLND_ROWS
#undef CALC_SECOND_BLND_ROWS
#undef CALC_FIRST_BLND_ROWS_MODE2
#undef CALC_SECOND_BLND_ROWS_MODE2

#else


#define LOADROW(ROWL, ROWH, X) { \
        /*tmp.load(refMain + 1 + (X)); */ \
        itmp = _mm_loadu_si128((__m128i const*)(refMain + 1 + (X))); \
        /* ROWL = extend_low(tmp);*/  \
        ROWL = _mm_unpacklo_epi8(itmp, _mm_setzero_si128()); \
        /*ROWH = extend_high(tmp);*/  \
        ROWH = _mm_unpackhi_epi8(itmp, _mm_setzero_si128()); \
}

#define BLND2_2(R1, R2) { \
        /*tmp1 = blend2q<0, 2>((Vec2q)R1, (Vec2q)R2); \
        tmp2 = blend2q<1, 3>((Vec2q)R1, (Vec2q)R2);*/ \
        itmp1 = _mm_unpacklo_epi64(R1, R2); \
        itmp2 = _mm_unpackhi_epi64(R1, R2); \
        /*tmp1.store(dst); */ \
        _mm_storeu_si128((__m128i*)dst, itmp1); \
        dst += i_dst_stride; \
        /*tmp2.store(dst);*/ \
        _mm_storeu_si128((__m128i*)dst, itmp2); \
        dst += i_dst_stride; \
}

#define MB8(R1, R2, R3, R4, R5, R6, R7, R8) { \
        itmp1 = _mm_unpacklo_epi8(R1, R2); \
        itmp2 = _mm_unpackhi_epi8(R1, R2); \
        R1 = itmp1; \
        R2 = itmp2; \
        itmp1 = _mm_unpacklo_epi8(R3, R4); \
        itmp2 = _mm_unpackhi_epi8(R3, R4); \
        R3 = itmp1; \
        R4 = itmp2; \
        itmp1 = _mm_unpacklo_epi16(R1, R3); \
        itmp2 = _mm_unpackhi_epi16(R1, R3); \
        R1 = itmp1; \
        R3 = itmp2; \
        itmp1 = _mm_unpacklo_epi16(R2, R4); \
        itmp2 = _mm_unpackhi_epi16(R2, R4); \
        R2 = itmp1; \
        R4 = itmp2; \
        itmp1 = _mm_unpacklo_epi8(R5, R6); \
        itmp2 = _mm_unpackhi_epi8(R5, R6); \
        R5 = itmp1; \
        R6 = itmp2; \
        itmp1 = _mm_unpacklo_epi8(R7, R8); \
        itmp2 = _mm_unpackhi_epi8(R7, R8); \
        R7 = itmp1; \
        R8 = itmp2; \
        itmp1 = _mm_unpacklo_epi16(R5, R7); \
        itmp2 = _mm_unpackhi_epi16(R5, R7); \
        R5 = itmp1; \
        R7 = itmp2; \
        itmp1 = _mm_unpacklo_epi16(R6, R8); \
        itmp2 = _mm_unpackhi_epi16(R6, R8); \
        R6 = itmp1; \
        R8 = itmp2; \
        itmp1 = _mm_unpacklo_epi32(R1, R5); \
        itmp2 = _mm_unpackhi_epi32(R1, R5); \
        R1 = itmp1; \
        R5 = itmp2; \
  \
        itmp1 = _mm_unpacklo_epi32(R2, R6); \
        itmp2 = _mm_unpackhi_epi32(R2, R6); \
        R2 = itmp1; \
        R6 = itmp2; \
  \
        itmp1 = _mm_unpacklo_epi32(R3, R7); \
        itmp2 = _mm_unpackhi_epi32(R3, R7); \
        R3 = itmp1; \
        R7 = itmp2; \
  \
        itmp1 = _mm_unpacklo_epi32(R4, R8); \
        itmp2 = _mm_unpackhi_epi32(R4, R8); \
        R4 = itmp1; \
        R8 = itmp2; \
}


#define PREDANG_CALCROW_VER(X) { \
        v_deltaPos = _mm_add_epi16(v_deltaPos, v_ipAngle); \
        v_deltaFract = _mm_and_si128(v_deltaPos, thirty1); \
        it1 = _mm_sub_epi16(thirty2, v_deltaFract); \
        i16 = _mm_set1_epi16(16); \
        for ( j = 0 ; j < 2 ; ++ j ) \
        { \
        	itmp = _mm_loadu_si128((__m128i const*)(refMain + 1 + (angAP[8 - (lookIdx)][(X)] + 16 * j))); \
			row11L = _mm_unpacklo_epi8(itmp, _mm_setzero_si128()); \
			row11H = _mm_unpackhi_epi8(itmp, _mm_setzero_si128()); \
  \
			itmp = _mm_loadu_si128((__m128i const*)(refMain + 1 + (angAP[8 - (lookIdx)][(X)] + 16 * j + 1))); \
			row12L = _mm_unpacklo_epi8(itmp, _mm_setzero_si128()); \
			row12H = _mm_unpackhi_epi8(itmp, _mm_setzero_si128()); \
  \
			it2 = _mm_mullo_epi16(it1, row11L); \
			it3 = _mm_mullo_epi16(v_deltaFract, row12L); \
			it2 = _mm_add_epi16(it2, it3); \
			it2 = _mm_add_epi16(it2, i16); \
			row11L = _mm_srai_epi16(it2, 5); \
			it2 = _mm_mullo_epi16(it1, row11H); \
			it3 = _mm_mullo_epi16(v_deltaFract, row12H); \
			it2 = _mm_add_epi16(it2, it3); \
			it2 = _mm_add_epi16(it2, i16); \
			row11H = _mm_srai_epi16(it2, 5); \
  \
  	  	  	itmp = _mm_packus_epi16(row11L, row11H); \
  	  	  	_mm_storeu_si128((__m128i*)(dst + ((X)*i_dst_stride) + 16 * j), itmp); \
        } \
}

#define PREDANG_CALCROW_VER_MODE2(X) { \
        v_deltaPos = _mm_add_epi16(v_deltaPos, v_ipAngle); \
        v_deltaFract = _mm_and_si128(v_deltaPos, thirty1); \
        it1 = _mm_sub_epi16(thirty2, v_deltaFract); \
        i16 = _mm_set1_epi16(16); \
        it2 = _mm_mullo_epi16(it1, row11); \
        it3 = _mm_mullo_epi16(v_deltaFract, row21); \
        it2 = _mm_add_epi16(it2, it3); \
        it2 = _mm_add_epi16(it2, i16); \
        res1 = _mm_srai_epi16(it2, 5); \
        it2 = _mm_mullo_epi16(it1, row12); \
        it3 = _mm_mullo_epi16(v_deltaFract, row22); \
        it2 = _mm_add_epi16(it2, it3); \
        it2 = _mm_add_epi16(it2, i16); \
        res2 = _mm_srai_epi16(it2, 5); \
  \
        itmp = _mm_packus_epi16(res1, res2); \
        _mm_storeu_si128((__m128i*)(dst + ((X)*i_dst_stride)), itmp); \
        it2 = _mm_mullo_epi16(it1, row13); \
        it3 = _mm_mullo_epi16(v_deltaFract, row23); \
        it2 = _mm_add_epi16(it2, it3); \
        it2 = _mm_add_epi16(it2, i16); \
        res1 = _mm_srai_epi16(it2, 5); \
        it2 = _mm_mullo_epi16(it1, row14); \
        it3 = _mm_mullo_epi16(v_deltaFract, row24); \
        it2 = _mm_add_epi16(it2, it3); \
        it2 = _mm_add_epi16(it2, i16); \
        res2 = _mm_srai_epi16(it2, 5); \
  \
        itmp = _mm_packus_epi16(res1, res2); \
        _mm_storeu_si128((__m128i*)(dst + ((X)*i_dst_stride) + 16), itmp); \
}

#define PREDANG_CALCROW_HOR(X, rowx) { \
        itmp = _mm_loadu_si128((__m128i const*)(refMain + 1 + (angAP[8 - (lookIdx)][((X))]))); \
        row11L = _mm_unpacklo_epi8(itmp, _mm_setzero_si128()); \
        row11H = _mm_unpackhi_epi8(itmp, _mm_setzero_si128()); \
  \
        itmp = _mm_loadu_si128((__m128i const*)(refMain + 1 + (angAP[8 - (lookIdx)][((X))] + 1))); \
        row12L = _mm_unpacklo_epi8(itmp, _mm_setzero_si128()); \
        row12H = _mm_unpackhi_epi8(itmp, _mm_setzero_si128()); \
  \
        v_deltaPos = _mm_add_epi16(v_deltaPos, v_ipAngle); \
        v_deltaFract = _mm_and_si128(v_deltaPos, thirty1); \
        it1 = _mm_sub_epi16(thirty2, v_deltaFract); \
        i16 = _mm_set1_epi16(16); \
        it2 = _mm_mullo_epi16(it1, row11L); \
        it3 = _mm_mullo_epi16(v_deltaFract, row12L); \
        it2 = _mm_add_epi16(it2, it3); \
        it2 = _mm_add_epi16(it2, i16); \
        row11L = _mm_srai_epi16(it2, 5); \
        it2 = _mm_mullo_epi16(it1, row11H); \
        it3 = _mm_mullo_epi16(v_deltaFract, row12H); \
        it2 = _mm_add_epi16(it2, it3); \
        it2 = _mm_add_epi16(it2, i16); \
        row11H = _mm_srai_epi16(it2, 5); \
  \
        rowx = _mm_packus_epi16(row11L, row11H); \
}

#define PREDANG_CALCROW_HOR_MODE2(rowx) { \
        v_deltaPos = _mm_add_epi16(v_deltaPos, v_ipAngle); \
        v_deltaFract = _mm_and_si128(v_deltaPos, thirty1); \
        it1 = _mm_sub_epi16(thirty2, v_deltaFract); \
        i16 = _mm_set1_epi16(16); \
        it2 = _mm_mullo_epi16(it1, row11L); \
        it3 = _mm_mullo_epi16(v_deltaFract, row12L); \
        it2 = _mm_add_epi16(it2, it3); \
        it2 = _mm_add_epi16(it2, i16); \
        res1 = _mm_srai_epi16(it2, 5); \
        it2 = _mm_mullo_epi16(it1, row11H); \
        it3 = _mm_mullo_epi16(v_deltaFract, row12H); \
        it2 = _mm_add_epi16(it2, it3); \
        it2 = _mm_add_epi16(it2, i16); \
        res2 = _mm_srai_epi16(it2, 5); \
  \
        rowx = _mm_packus_epi16(res1, res2); \
}


#define CALC_BLND_8ROWS(R1, R2, R3, R4, R5, R6, R7, R8, X) { \
        PREDANG_CALCROW_HOR(0 + X, R1) \
        PREDANG_CALCROW_HOR(1 + X, R2) \
        PREDANG_CALCROW_HOR(2 + X, R3) \
        PREDANG_CALCROW_HOR(3 + X, R4) \
        PREDANG_CALCROW_HOR(4 + X, R5) \
        PREDANG_CALCROW_HOR(5 + X, R6) \
        PREDANG_CALCROW_HOR(6 + X, R7) \
}

#define CALC_BLND_8ROWS_MODE2(R1, R2, R3, R4, R5, R6, R7, R8) { \
        PREDANG_CALCROW_HOR_MODE2(R1) \
        PREDANG_CALCROW_HOR_MODE2(R2) \
        PREDANG_CALCROW_HOR_MODE2(R3) \
        PREDANG_CALCROW_HOR_MODE2(R4) \
        PREDANG_CALCROW_HOR_MODE2(R5) \
        PREDANG_CALCROW_HOR_MODE2(R6) \
        PREDANG_CALCROW_HOR_MODE2(R7) \
}

#define CALC_BLND_ROWS_MODE2(rowL,rowH) \
        CALC_BLND_8ROWS_MODE2(R1, R2, R3, R4, R5, R6, R7, R8) \
        PREDANG_CALCROW_HOR_MODE2(R8) \
        MB8(R1, R2, R3, R4, R5, R6, R7, R8) \
        CALC_BLND_8ROWS_MODE2(R9, R10, R11, R12, R13, R14, R15, R16) \
        v_deltaPos = _mm_add_epi16(v_deltaPos, v_ipAngle); \
        R16 = _mm_packus_epi16(rowL, rowH); \
        MB8(R9, R10, R11, R12, R13, R14, R15, R16) \
        BLND2_2(R1, R9) \
        BLND2_2(R5, R13) \
        BLND2_2(R3, R11) \
        BLND2_2(R7, R15) \
        BLND2_2(R2, R10) \
        BLND2_2(R6, R14) \
        BLND2_2(R4, R12) \
        BLND2_2(R8, R16)

#define CALC_FIRST_BLND_ROWS(start) \
		CALC_BLND_8ROWS(R1, R2, R3, R4, R5, R6, R7, R8, start) \
		PREDANG_CALCROW_HOR(7 + start, R8) ; \
		MB8(R1, R2, R3, R4, R5, R6, R7, R8) ; \
		CALC_BLND_8ROWS(R9, R10, R11, R12, R13, R14, R15, R16, start+8) \
		PREDANG_CALCROW_HOR(15 + start, R16) ; \
		MB8(R9, R10, R11, R12, R13, R14, R15, R16) ; \
		BLND2_2(R1, R9) \
		BLND2_2(R5, R13) \
		BLND2_2(R3, R11) \
		BLND2_2(R7, R15) \
		BLND2_2(R2, R10) \
		BLND2_2(R6, R14) \
		BLND2_2(R4, R12) \
		BLND2_2(R8, R16)

#define CALC_SECOND_BLND_ROWS(start) \
		CALC_BLND_8ROWS(R1, R2, R3, R4, R5, R6, R7, R8, start) \
		PREDANG_CALCROW_HOR(7 + start, R8) \
		MB8(R1, R2, R3, R4, R5, R6, R7, R8) \
		CALC_BLND_8ROWS(R9, R10, R11, R12, R13, R14, R15, R16, start+8) \
        v_deltaPos = _mm_add_epi16(v_deltaPos, v_ipAngle); \
		R16 = _mm_loadu_si128((__m128i const*)(refMain + 1 + GETAP(lookIdx, start+15))); \
		MB8(R9, R10, R11, R12, R13, R14, R15, R16) \
		BLND2_2(R1, R9) \
		BLND2_2(R5, R13) \
		BLND2_2(R3, R11) \
		BLND2_2(R7, R15) \
		BLND2_2(R2, R10) \
		BLND2_2(R6, R14) \
		BLND2_2(R4, R12) \
		BLND2_2(R8, R16)

void x265_intra_pred_angle_3_8_sse2(pixel *left,
									pixel *top,
									pixel* dst,
									int i_dst_stride,
									int i_chroma_idx,
									int i_mode)
{
    int blkSize = 32;

    // Map the mode index to main prediction direction and angle
    assert(i_mode > 1); //no planar and dc
    int32_t modeHor       = (i_mode < 18);
    int32_t modeVer       = !modeHor;
    int intraPredAngle = modeVer ? (int)i_mode - X265_VER_IDX : modeHor ? -((int)i_mode - X265_HOR_IDX) : 0;
    int absAng         = abs(intraPredAngle);
    int signAng        = intraPredAngle < 0 ? -1 : 1;

    // Set bitshifts and scale the angle parameter to block size
    int angTable[9]    = { 0,    2,    5,   9,  13,  17,  21,  26,  32 };
    int invAngTable[9] = { 0, 4096, 1638, 910, 630, 482, 390, 315, 256 }; // (256 * 32) / Angle
    int invAngle       = invAngTable[absAng];
    absAng             = angTable[absAng];
    intraPredAngle     = signAng * absAng;

    // Do angular predictions

    pixel ref_array[65];
    pixel* refMain;
    pixel* temp_ref_main;
    pixel* refSide;
    pixel* temp_dst;
    int32_t loop = 0 ;
    int32_t i = 0, j = 0, k ;

    // Initialise the Main and Left reference array.
    if (intraPredAngle < 0)
    {
        refMain = ref_array+32 ;
        if (modeVer)
        {
        	memcpy(refMain, top-1, sizeof(pixel)*33) ;
        }
        else
        {
        	memcpy(refMain, left-1, sizeof(pixel)*33) ;
        }

        refSide = (modeVer ? left-1 : top-1);     // + (blkSize - 1);

        // Extend the Main reference to the left.
        int invAngleSum    = 128;     // rounding for (shift by 8)
        if (intraPredAngle != -32)
        {
        	for (k = -1; k > blkSize * intraPredAngle >> 5; k--)
            {
                invAngleSum += invAngle;
                refMain[k] = refSide[invAngleSum >> 8];
            }
        }
    }
    else
    {
        refMain = modeVer ? top-1 : left-1;
        refSide = modeVer ? left-1  : top-1;
    }

    // bfilter will always be true for blocksize 8
    if (intraPredAngle == 0)  // Exactly hotizontal/vertical angles
    {
        if (modeHor)
        {
        	__m128i tmp1 ;

        	temp_dst = dst ;
        	temp_ref_main = refMain + 1 ;
        	for ( i = 0 ; i < 2 ; ++ i )
        	{
				for ( j = 0 ; j < 16 ; ++ j )
				{
					tmp1 = _mm_set1_epi8(temp_ref_main[j]);
					for ( k = 0 ; k < 2 ; ++ k )
					{
						_mm_storeu_si128((__m128i*)(temp_dst+16 * k), tmp1);
					}
					temp_dst += i_dst_stride ;
				}
				temp_ref_main += 16 ;
        	}
        }
        else
        {
            __m128i v_main;


            for ( i = 0 ; i < 2 ; ++ i )
            {
            	temp_dst = dst + 16 * i ;
            	v_main = _mm_loadu_si128((__m128i const*)(refMain + 1 + 16 * i ));
            	for ( j = 0 ; j < 32 ; ++ j )
            	{
            		_mm_storeu_si128((__m128i*)temp_dst, v_main);
            		temp_dst += i_dst_stride ;
            	}
            }
        }
    }
    else if (intraPredAngle == -32)
    {
        for ( loop = -1 ; loop > -32 ; -- loop )
        {
        	refMain[loop] = refSide[-loop] ;
        }
        x265_intra_pred_angle_m32_3_8_sse2 (refMain, dst, i_dst_stride) ;
        return;
    }
    else if (intraPredAngle == 32)
    {
        refMain += 2;
        x265_intra_pred_angle_32_3_8_sse2 (refMain, dst, i_dst_stride) ;
        return;
    }
    else
    {
        x265_pred_angle_help_3_8_sse2[i_mode] ( refMain + 1, dst, i_dst_stride ) ;
    }
}


#undef LOADROW
#undef BLND2_2
#undef MB8
#undef PREDANG_CALCROW_VER
#undef PREDANG_CALCROW_VER_MODE2
#undef PREDANG_CALCROW_HOR
#undef PREDANG_CALCROW_HOR_MODE2
#undef CALC_BLND_8ROWS
#undef CALC_BLND_8ROWS_MODE2
#undef CALC_BLND_ROWS_MODE2
#undef CALC_FIRST_BLND_ROWS
#undef CALC_SECOND_BLND_ROWS

#endif



#undef STORE_ANGLE_MINUS_32
#undef	STORE_ANGLE_32
#undef GETAP

