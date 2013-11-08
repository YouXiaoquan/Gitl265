




#include <emmintrin.h>



#define GETAP(X, Y) angAP[8 - (X)][(Y)]


#define PREDANG_CALCROW_VER(X) { \
        LOADROW(row11L, row11H, GETAP(lookIdx, X)); \
        LOADROW(row12L, row12H, GETAP(lookIdx, X) + 1); \
        CALCROW(row11L, row11H, row11L, row11H, row12L, row12H); \
        /*compress(row11L, row11H).store(dst + ((X)*i_dst_stride));*/ \
        itmp = _mm_packus_epi16(row11L, row11H); \
        _mm_storeu_si128((__m128i*)(dst + ((X)*i_dst_stride)), itmp); \
}

#define PREDANG_CALCROW_HOR(X, rowx) { \
        LOADROW(row11L, row11H, GETAP(lookIdx, (X))); \
        LOADROW(row12L, row12H, GETAP(lookIdx, (X)) + 1); \
        CALCROW(row11L, row11H, row11L, row11H, row12L, row12H); \
        /*rowx = compress(row11L, row11H);*/  \
        rowx = _mm_packus_epi16(row11L, row11H); \
}

// ROWL/H is a Vec8s variable, X is the index in of data to be loaded
#define LOADROW(ROWL, ROWH, X) { \
        /*tmp.load(refMain + 1 + (X)); */ \
        itmp = _mm_loadu_si128((__m128i const*)(refMain + 1 + (X))); \
        /* ROWL = extend_low(tmp);*/  \
        ROWL = _mm_unpacklo_epi8(itmp, _mm_setzero_si128()); \
        /*ROWH = extend_high(tmp);*/  \
        ROWH = _mm_unpackhi_epi8(itmp, _mm_setzero_si128()); \
}

#define CALCROW(RESL, RESH, ROW1L, ROW1H, ROW2L, ROW2H) { \
        /*v_deltaPos += v_ipAngle; \
        v_deltaFract = v_deltaPos & thirty1;*/ \
        v_deltaPos = _mm_add_epi16(v_deltaPos, v_ipAngle); \
        v_deltaFract = _mm_and_si128(v_deltaPos, thirty1); \
        /*RESL = ((thirty2 - v_deltaFract) * ROW1L + (v_deltaFract * ROW2L) + 16) >> 5; \
        RESH = ((thirty2 - v_deltaFract) * ROW1H + (v_deltaFract * ROW2H) + 16) >> 5;*/ \
        it1 = _mm_sub_epi16(thirty2, v_deltaFract); \
        it2 = _mm_mullo_epi16(it1, ROW1L); \
        it3 = _mm_mullo_epi16(v_deltaFract, ROW2L); \
        it2 = _mm_add_epi16(it2, it3); \
        i16 = _mm_set1_epi16(16); \
        it2 = _mm_add_epi16(it2, i16); \
        RESL = _mm_srai_epi16(it2, 5); \
        \
        it2 = _mm_mullo_epi16(it1, ROW1H); \
        it3 = _mm_mullo_epi16(v_deltaFract, ROW2H); \
        it2 = _mm_add_epi16(it2, it3); \
        it2 = _mm_add_epi16(it2, i16); \
        RESH = _mm_srai_epi16(it2, 5); \
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

#define CALC_BLND_8ROWS(R1, R2, R3, R4, R5, R6, R7, R8, X) { \
        PREDANG_CALCROW_HOR(0 + X, R1) \
        PREDANG_CALCROW_HOR(1 + X, R2) \
        PREDANG_CALCROW_HOR(2 + X, R3) \
        PREDANG_CALCROW_HOR(3 + X, R4) \
        PREDANG_CALCROW_HOR(4 + X, R5) \
        PREDANG_CALCROW_HOR(5 + X, R6) \
        PREDANG_CALCROW_HOR(6 + X, R7) \
}

__m128i x265_m8s_min_m8s_int32 (__m128i a, int32_t i )
{
	__m128i b ;

	b = _mm_set1_epi16 (i) ;
	return _mm_min_epi16 (a, b) ;
}

__m128i x265_m8s_max_m8s_int32 (__m128i a, int32_t i )
{
	__m128i b ;

	b = _mm_set1_epi16 (i) ;
	return _mm_max_epi16 (a, b) ;
}


uint8_t x265_m8s_get_uint8 (__m128i m8s, int32_t i_index )
{
	int16_t dst[8] ;
    _mm_storeu_si128((__m128i*)dst, m8s);            //row0
	return (uint8_t)dst[i_index & 0x0F] ;
}

#define		SET_1_LEFT_0() \
		SET_LEFT_0(0) ;
#define		SET_15_LEFT_0() \
		SET_LEFT_0( 1) ; \
		SET_LEFT_0( 2) ; \
		SET_LEFT_0( 3) ; \
		SET_LEFT_0( 4) ; \
		SET_LEFT_0( 5) ; \
		SET_LEFT_0( 6) ; \
		SET_LEFT_0( 7) ; \
		SET_LEFT_0( 8) ; \
		SET_LEFT_0( 9) ; \
		SET_LEFT_0(10) ; \
		SET_LEFT_0(11) ; \
		SET_LEFT_0(12) ; \
		SET_LEFT_0(13) ; \
		SET_LEFT_0(14) ; \
		SET_LEFT_0(15) ;

#define		SET_16_LEFT_0() \
		SET_1_LEFT_0() ; \
		SET_15_LEFT_0() ;

#define		SET_16_TOP_0() \
		SET_TOP_0() ; \
		SET_TOP_0() ; \
		SET_TOP_0() ; \
		SET_TOP_0() ; \
		SET_TOP_0() ; \
		SET_TOP_0() ; \
		SET_TOP_0() ; \
		SET_TOP_0() ; \
		SET_TOP_0() ; \
		SET_TOP_0() ; \
		SET_TOP_0() ; \
		SET_TOP_0() ; \
		SET_TOP_0() ; \
		SET_TOP_0() ; \
		SET_TOP_0() ; \
		SET_TOP_0() ;

#define		STORE_16_ANGLE_MINUS_32() \
		STORE_ANGLE_MINUS_32() ; \
		STORE_ANGLE_MINUS_32() ; \
		STORE_ANGLE_MINUS_32() ; \
		STORE_ANGLE_MINUS_32() ; \
		STORE_ANGLE_MINUS_32() ; \
		STORE_ANGLE_MINUS_32() ; \
		STORE_ANGLE_MINUS_32() ; \
		STORE_ANGLE_MINUS_32() ; \
		STORE_ANGLE_MINUS_32() ; \
		STORE_ANGLE_MINUS_32() ; \
		STORE_ANGLE_MINUS_32() ; \
		STORE_ANGLE_MINUS_32() ; \
		STORE_ANGLE_MINUS_32() ; \
		STORE_ANGLE_MINUS_32() ; \
		STORE_ANGLE_MINUS_32() ; \
		STORE_ANGLE_MINUS_32() ; \

#define		STORE_16_ANGLE_32() \
		STORE_ANGLE_32() ; \
		STORE_ANGLE_32() ; \
		STORE_ANGLE_32() ; \
		STORE_ANGLE_32() ; \
		STORE_ANGLE_32() ; \
		STORE_ANGLE_32() ; \
		STORE_ANGLE_32() ; \
		STORE_ANGLE_32() ; \
		STORE_ANGLE_32() ; \
		STORE_ANGLE_32() ; \
		STORE_ANGLE_32() ; \
		STORE_ANGLE_32() ; \
		STORE_ANGLE_32() ; \
		STORE_ANGLE_32() ; \
		STORE_ANGLE_32() ; \
		STORE_ANGLE_32() ;

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


void xPredIntraAng16x16(pixel *left,
						pixel *top,
						pixel* dst,
						int i_dst_stride,
						int i_chroma_idx,
						int i_mode )
{
    int k;
    int blkSize        = 16;

    // Map the mode index to main prediction direction and angle
    assert(i_mode > 1); //no planar and dc
    int32_t modeHor       = (i_mode < 18);
    int32_t modeVer       = !modeHor;
    int intraPredAngle = modeVer ? (int)i_mode - X265_VER_IDX : modeHor ? -((int)i_mode - X265_HOR_IDX) : 0;
    int lookIdx = intraPredAngle;
    int absAng         = abs(intraPredAngle);
    int signAng        = intraPredAngle < 0 ? -1 : 1;

    // Set bitshifts and scale the angle parameter to block size
    int angTable[9]    = { 0,    2,    5,   9,  13,  17,  21,  26,  32 };
    int invAngTable[9] = { 0, 4096, 1638, 910, 630, 482, 390, 315, 256 }; // (256 * 32) / Angle
    int invAngle       = invAngTable[absAng];
    absAng             = angTable[absAng];
    intraPredAngle     = signAng * absAng;

    // Do angular predictions

    pixel* refMain;
    pixel* temp_ref_main;
    pixel* refSide;
    pixel* temp_dst;

    // Initialise the Main and Left reference array.
    if (intraPredAngle < 0)
    {
        refMain = (modeVer ? top-1 : left-1);     // + (blkSize - 1);
        refSide = (modeVer ? left-1 : top-1);     // + (blkSize - 1);

        // Extend the Main reference to the left.
        int invAngleSum    = 128;     // rounding for (shift by 8)
        if (intraPredAngle != -32)
            for (k = -1; k > blkSize * intraPredAngle >> 5; k--)
            {
                invAngleSum += invAngle;
                refMain[k] = refSide[invAngleSum >> 8];
            }
    }
    else
    {
        refMain = modeVer ? top-1 : left-1;
        refSide = modeVer ? left-1  : top-1;
    }

    if (intraPredAngle == 0)  // Exactly hotizontal/vertical angles
    {
        if (modeHor)
        {
        	__m128i tmp1;

#define		SET_LEFT_0(num) \
			tmp1 = _mm_set1_epi8(temp_ref_main[num]); \
        	_mm_storeu_si128((__m128i*)temp_dst, tmp1); \
        	temp_dst += i_dst_stride ;

        	temp_ref_main = refMain + 1;
        	temp_dst = dst ;

            if (10 == i_mode && 0 == i_chroma_idx)
            {
            	__m128i v_side_0; // refSide[0] value in a vector
            	__m128i v_temp16;

            	v_side_0 = _mm_set1_epi16(refSide[0]);
            	v_temp16 = _mm_loadu_si128(((__m128i const*)(refSide + 1)));
            	__m128i v_side;
                v_side =  _mm_unpacklo_epi8(v_temp16,_mm_setzero_si128());

                __m128i row01, row02, ref ;
                ref = _mm_set1_epi16(refMain[1]);
                v_side = _mm_sub_epi16(v_side, v_side_0);
                v_side = _mm_srai_epi16(v_side, 1);
                row01 = _mm_add_epi16(ref, v_side);
                row01 = x265_m8s_min_m8s_int32(x265_m8s_max_m8s_int32(row01, 0),
                								(1 << 8) - 1);

                v_side =  _mm_unpackhi_epi8(v_temp16,_mm_setzero_si128());
                v_side = _mm_sub_epi16(v_side, v_side_0);
                v_side = _mm_srai_epi16(v_side, 1);
                row02 = _mm_add_epi16(ref, v_side);
                row02 = x265_m8s_min_m8s_int32(x265_m8s_max_m8s_int32(row02, 0),
                								(1 << 8) - 1);

                tmp1 = _mm_packus_epi16(row01, row02);
                _mm_storeu_si128((__m128i*)dst, tmp1);            //row0
            	temp_dst += i_dst_stride ;
            }
            else
            {
            	SET_1_LEFT_0() ;
            }

            SET_15_LEFT_0() ;
#undef SET_LEFT_0

        }
        else
        {
        	temp_dst = dst ;

            __m128i v_main;
//            v_main.load(refMain + 1);
            v_main = _mm_loadu_si128((__m128i const*)(refMain + 1));
#define SET_TOP_0() \
			_mm_storeu_si128((__m128i*)temp_dst, v_main); \
			temp_dst += i_dst_stride ;

            SET_16_TOP_0() ;
#undef SET_TOP_0
            if (26 == i_mode && 0 == i_chroma_idx)
            {
                temp_dst = dst ;

#define STORE_LEFT_0(num) \
                temp_dst[0] = x265_m8s_get_uint8 (row0, num) ; \
                temp_dst += i_dst_stride ;

            	__m128i v_side_0; // refSide[0] value in a vector
            	__m128i v_temp16;

            	v_side_0 = _mm_set1_epi16(refSide[0]);
            	v_temp16 = _mm_loadu_si128(((__m128i const*)(refSide + 1)));
            	__m128i v_side;
                v_side =  _mm_unpacklo_epi8(v_temp16,_mm_setzero_si128());

                __m128i row0, ref ;
                ref = _mm_set1_epi16(refMain[1]);
                v_side = _mm_sub_epi16(v_side, v_side_0);
                v_side = _mm_srai_epi16(v_side, 1);
                row0 = _mm_add_epi16(ref, v_side);
                row0 = x265_m8s_min_m8s_int32(x265_m8s_max_m8s_int32(row0, 0),
                								(1 << 8) - 1);
                STORE_LEFT_0(0) ;
                STORE_LEFT_0(1) ;
                STORE_LEFT_0(2) ;
                STORE_LEFT_0(3) ;
                STORE_LEFT_0(4) ;
                STORE_LEFT_0(5) ;
                STORE_LEFT_0(6) ;
                STORE_LEFT_0(7) ;

                v_side =  _mm_unpackhi_epi8(v_temp16,_mm_setzero_si128());
                v_side = _mm_sub_epi16(v_side, v_side_0);
                v_side = _mm_srai_epi16(v_side, 1);
                row0 = _mm_add_epi16(ref, v_side);
                row0 = x265_m8s_min_m8s_int32(x265_m8s_max_m8s_int32(row0, 0),
                								(1 << 8) - 1);

                STORE_LEFT_0(0) ;
                STORE_LEFT_0(1) ;
                STORE_LEFT_0(2) ;
                STORE_LEFT_0(3) ;
                STORE_LEFT_0(4) ;
                STORE_LEFT_0(5) ;
                STORE_LEFT_0(6) ;
                STORE_LEFT_0(7) ;

#undef STORE_LEFT_0
            }

        }
    }
    else if (intraPredAngle == -32)
    {
/*        Vec16uc v_refSide;
        v_refSide.load(refSide);
        v_refSide = permute16uc<15, 14, 13, 12, 11, 10, 9, 8, 7, 6, 5, 4, 3, 2, 1, 0>(v_refSide);
        pixel refMain0 = refMain[0];
*/
        for (int loop = -1 ; loop > -16 ; -- loop )
        {
        	refMain[loop] = refSide[-loop] ;
        }
//        v_refSide.store(refMain - 15);
//        refMain[0] = refMain0;

//        Vec16uc tmp;
        __m128i itmp;
//        tmp.load(refMain);        //-1,0,1,2
//        tmp.store(dst);
#define		STORE_ANGLE_MINUS_32() \
        itmp = _mm_loadu_si128((__m128i const*)refMain--); \
        _mm_storeu_si128((__m128i*)dst, itmp); \
        dst += i_dst_stride

        STORE_16_ANGLE_MINUS_32() ;

#undef STORE_ANGLE_MINUS_32

/*
        tmp.load(--refMain);
        dst += i_dst_stride;
        tmp.store(dst);
        ... 14 times more
*/
        return;
    }
    else if (intraPredAngle == 32)
    {
        __m128i itmp;
        refMain += 2;

//        tmp.load(refMain++);
//        tmp.store(dst);

#define		STORE_ANGLE_32() \
        itmp = _mm_loadu_si128((__m128i const*)refMain++); \
        _mm_storeu_si128((__m128i*)dst, itmp); \
        dst += i_dst_stride;

        STORE_16_ANGLE_32() ;

#undef STORE_ANGLE_32


        return;
    }
    else
    {
        if (modeHor)
        {
        	__m128i row11L, row12L, row11H, row12H;
        	__m128i v_deltaFract, v_deltaPos, thirty2, thirty1, v_ipAngle;
        	__m128i R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, R13, R14, R15, R16;

        	thirty2 = _mm_set1_epi16(32) ;
        	thirty1 = _mm_set1_epi16(31) ;
        	v_deltaPos = _mm_set1_epi16(0) ;
            v_ipAngle = _mm_set1_epi16(intraPredAngle);
            __m128i itmp, itmp1, itmp2, it1, it2, it3, i16;
//            MB16;
            CALC_FIRST_BLND_ROWS(0) ;
        }
        else
        {
        	__m128i row11L, row12L, row11H, row12H;
        	__m128i v_deltaFract, v_deltaPos, thirty2, thirty1, v_ipAngle;
        	thirty2 = _mm_set1_epi16(32) ;
        	thirty1 = _mm_set1_epi16(31) ;
        	v_deltaPos = _mm_set1_epi16(0) ;
            v_ipAngle = _mm_set1_epi16(intraPredAngle);
            __m128i itmp, it1, it2, it3, i16;

            PREDANG_CALCROW_VER(0);
            PREDANG_CALCROW_VER(1);
            PREDANG_CALCROW_VER(2);
            PREDANG_CALCROW_VER(3);
            PREDANG_CALCROW_VER(4);
            PREDANG_CALCROW_VER(5);
            PREDANG_CALCROW_VER(6);
            PREDANG_CALCROW_VER(7);
            PREDANG_CALCROW_VER(8);
            PREDANG_CALCROW_VER(9);
            PREDANG_CALCROW_VER(10);
            PREDANG_CALCROW_VER(11);
            PREDANG_CALCROW_VER(12);
            PREDANG_CALCROW_VER(13);
            PREDANG_CALCROW_VER(14);
            PREDANG_CALCROW_VER(15);
        }
    }
}


#undef PREDANG_CALCROW_VER
#undef PREDANG_CALCROW_HOR
#undef LOADROW
#undef CALCROW
#undef BLND2_16
#undef BLND2_2
#undef BLND2_4
#undef MB4


#define PREDANG_CALCROW_VER(X) { \
        v_deltaPos = _mm_add_epi16(v_deltaPos, v_ipAngle); \
        v_deltaFract = _mm_and_si128(v_deltaPos, thirty1); \
        itmp = _mm_loadu_si128((__m128i const*)(refMain + 1 + (angAP[8 - (lookIdx)][(X)]))); \
        row11L = _mm_unpacklo_epi8(itmp, _mm_setzero_si128()); \
        row11H = _mm_unpackhi_epi8(itmp, _mm_setzero_si128()); \
  \
        itmp = _mm_loadu_si128((__m128i const*)(refMain + 1 + (angAP[8 - (lookIdx)][(X)] + 1))); \
        row12L = _mm_unpacklo_epi8(itmp, _mm_setzero_si128()); \
        row12H = _mm_unpackhi_epi8(itmp, _mm_setzero_si128()); \
  \
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
        itmp = _mm_packus_epi16(row11L, row11H); \
        _mm_storeu_si128((__m128i*)(dst + ((X)*i_dst_stride)), itmp); \
        itmp = _mm_loadu_si128((__m128i const*)(refMain + 1 + (angAP[8 - (lookIdx)][(X)] + 16))); \
        row11L = _mm_unpacklo_epi8(itmp, _mm_setzero_si128()); \
        row11H = _mm_unpackhi_epi8(itmp, _mm_setzero_si128()); \
  \
        itmp = _mm_loadu_si128((__m128i const*)(refMain + 1 + (angAP[8 - (lookIdx)][(X)] + 17))); \
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
        _mm_storeu_si128((__m128i*)(dst + ((X)*i_dst_stride) + 16), itmp); \
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

// ROWL/H is a Vec8s variable, X is the index in of data to be loaded
#define LOADROW(ROWL, ROWH, X) { \
/*        tmp.load(refMain + 1 + (X)); \
        ROWL = extend_low(tmp); \
        ROWH = extend_high(tmp); */\
        itmp = _mm_loadu_si128((__m128i const*)(refMain + 1 + (X))); \
        ROWL = _mm_unpacklo_epi8(itmp, _mm_setzero_si128()); \
        ROWH = _mm_unpackhi_epi8(itmp, _mm_setzero_si128()); \
}

#define BLND2_2(R1, R2) { \
/*        tmp1 = blend2q<0, 2>((Vec2q)R1, (Vec2q)R2); \
        tmp2 = blend2q<1, 3>((Vec2q)R1, (Vec2q)R2); \
        tmp1.store(dst);   dst += i_dst_stride; \
        tmp2.store(dst);   dst += i_dst_stride; */\
        itmp1 = _mm_unpacklo_epi64(R1, R2); \
        itmp2 = _mm_unpackhi_epi64(R1, R2); \
        _mm_storeu_si128((__m128i*)dst, itmp1); \
        dst += i_dst_stride; \
        _mm_storeu_si128((__m128i*)dst, itmp2); \
        dst += i_dst_stride; \
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

#define CALC_SECOND_BLND_ROWS(start) \
		CALC_BLND_8ROWS(R1, R2, R3, R4, R5, R6, R7, R8, start) \
		PREDANG_CALCROW_HOR(7 + start, R8) \
		MB8(R1, R2, R3, R4, R5, R6, R7, R8) \
		CALC_BLND_8ROWS(R9, R10, R11, R12, R13, R14, R15, R16, start+8) \
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



void xPredIntraAng32x32(pixel *left,
						pixel *top,
						pixel* dst,
						int i_dst_stride,
						int i_chroma_idx,
						int i_mode)
{
    int k;
    int blkSize = 32;

    // Map the mode index to main prediction direction and angle
    assert(i_mode > 1); //no planar and dc
    int32_t modeHor       = (i_mode < 18);
    int32_t modeVer       = !modeHor;
    int intraPredAngle = modeVer ? (int)i_mode - X265_VER_IDX : modeHor ? -((int)i_mode - X265_HOR_IDX) : 0;
    int lookIdx = intraPredAngle;
    int absAng         = abs(intraPredAngle);
    int signAng        = intraPredAngle < 0 ? -1 : 1;

    // Set bitshifts and scale the angle parameter to block size
    int angTable[9]    = { 0,    2,    5,   9,  13,  17,  21,  26,  32 };
    int invAngTable[9] = { 0, 4096, 1638, 910, 630, 482, 390, 315, 256 }; // (256 * 32) / Angle
    int invAngle       = invAngTable[absAng];
    absAng             = angTable[absAng];
    intraPredAngle     = signAng * absAng;

    // Do angular predictions

    pixel* refMain;
    pixel* temp_ref_main;
    pixel* refSide;
    pixel* temp_dst;
    pixel* dst1 ;
    pixel* dst2 ;
    pixel* dst3 ;

    // Initialise the Main and Left reference array.
    if (intraPredAngle < 0)
    {
        refMain = (modeVer ? top-1: left-1);     // + (blkSize - 1);
        refSide = (modeVer ? left-1 : top-1);     // + (blkSize - 1);

        // Extend the Main reference to the left.
        int invAngleSum    = 128;     // rounding for (shift by 8)
        if (intraPredAngle != -32)
            for (k = -1; k > blkSize * intraPredAngle >> 5; k--)
            {
                invAngleSum += invAngle;
                refMain[k] = refSide[invAngleSum >> 8];
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
#define		SET_LEFT_0(num) \
			tmp1 = _mm_set1_epi8(temp_ref_main[num]); \
        	_mm_storeu_si128((__m128i*)temp_dst, tmp1); \
        	_mm_storeu_si128((__m128i*)(temp_dst+16), tmp1); \
        	temp_dst += i_dst_stride ;

        	__m128i tmp1 ;

        	temp_dst = dst ;

        	temp_ref_main = refMain + 1 ;
        	SET_16_LEFT_0() ;

        	temp_ref_main = refMain + 17 ;
        	SET_16_LEFT_0() ;
#undef SET_LEFT_0
        }
        else
        {
            __m128i v_main;

#define SET_TOP_0() \
			_mm_storeu_si128((__m128i*)temp_dst, v_main); \
			temp_dst += i_dst_stride ;

            temp_dst = dst ;
            v_main = _mm_loadu_si128((__m128i const*)(refMain + 1));
            SET_16_TOP_0() ;
            SET_16_TOP_0() ;

            temp_dst = dst + 16;
            v_main = _mm_loadu_si128((__m128i const*)(refMain + 17));
            SET_16_TOP_0() ;
            SET_16_TOP_0() ;
#undef SET_TOP_0
        }
    }
    else if (intraPredAngle == -32)
    {
        for (int loop = -1 ; loop > -32 ; -- loop )
        {
        	refMain[loop] = refSide[-loop] ;
        }

        __m128i itmp;

        dst1 = dst ;
        dst2 = dst + (16 * i_dst_stride) + 16 ;
        dst3 = (dst + 16) ;

#define		STORE_ANGLE_MINUS_32() \
        itmp = _mm_loadu_si128((__m128i const*)refMain); \
        _mm_storeu_si128((__m128i*)dst1, itmp); \
        _mm_storeu_si128((__m128i*)dst2, itmp); \
        itmp = _mm_loadu_si128((__m128i const*)(refMain + 16)); \
        _mm_storeu_si128((__m128i*)dst3, itmp); \
        dst1 += i_dst_stride; \
        dst2 += i_dst_stride; \
        dst3 += i_dst_stride; \
        refMain--;

        STORE_16_ANGLE_MINUS_32() ;

#undef STORE_ANGLE_MINUS_32

#define		STORE_ANGLE_MINUS_32() \
        itmp = _mm_loadu_si128((__m128i const*)refMain); \
        _mm_storeu_si128((__m128i*)dst1, itmp); \
        dst1 += i_dst_stride; \
        refMain--;

        STORE_16_ANGLE_MINUS_32() ;

#undef STORE_ANGLE_MINUS_32

        return;
    }
    else if (intraPredAngle == 32)
    {
        __m128i itmp;
        refMain += 2;

        dst1 = dst ;

#define		STORE_ANGLE_32() \
        itmp = _mm_loadu_si128((__m128i const*)refMain); \
        _mm_storeu_si128((__m128i*)dst1, itmp); \
        dst1 += i_dst_stride; \
        refMain++;

        STORE_16_ANGLE_32() ;
#undef STORE_ANGLE_32


        dst2 = dst1 - (16 * i_dst_stride) + 16 ;
        dst3 = dst1 + 16 ;
#define		STORE_ANGLE_32() \
        itmp = _mm_loadu_si128((__m128i const*)refMain); \
        refMain++; \
        _mm_storeu_si128((__m128i*)dst1, itmp); \
        _mm_storeu_si128((__m128i*)dst2, itmp); \
        itmp = _mm_loadu_si128((__m128i const*)(refMain + 15)); \
        _mm_storeu_si128((__m128i*)dst3, itmp); \
        dst1 += i_dst_stride; \
        dst2 += i_dst_stride; \
        dst3 += i_dst_stride;


        STORE_16_ANGLE_32() ;
#undef STORE_ANGLE_32



        return;
    }
    else
    {
        if (modeHor)
        {
            __m128i row11L, row12L, row11H, row12H, res1, res2;
            __m128i v_deltaFract, v_deltaPos, thirty2, thirty1, v_ipAngle;
            __m128i R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, R12, R13, R14, R15, R16;

            pixel * original_pDst = dst;
            v_deltaPos = _mm_setzero_si128(); //v_deltaPos = 0;
            v_ipAngle = _mm_set1_epi16(intraPredAngle);
            thirty2 = _mm_set1_epi16(32);
            thirty1 = _mm_set1_epi16(31);
            __m128i itmp, itmp1, itmp2, it1, it2, it3, i16;

            switch (intraPredAngle)
            {
            case -2:
                LOADROW(row11L, row11H, -1)
                LOADROW(row12L, row12H,  0)
                CALC_BLND_ROWS_MODE2(row11L, row11H);

                row12L = row11L;
                row12H = row11H;
                LOADROW(row11L, row11H, -2)
                dst = original_pDst + 16;
                CALC_BLND_ROWS_MODE2(row11L, row11H);

                refMain += 16;
                v_deltaPos = _mm_setzero_si128();

                LOADROW(row11L, row11H, -1)
                LOADROW(row12L, row12H,  0)
                dst = original_pDst + (16 * i_dst_stride);
                CALC_BLND_ROWS_MODE2(row11L, row11H);

                row12L = row11L;
                row12H = row11H;
                LOADROW(row11L, row11H, -2)
                dst = original_pDst + (16 * i_dst_stride) + 16;
                CALC_BLND_ROWS_MODE2(row11L, row11H);
                return;

            case  2:
                LOADROW(row11L, row11H, 0)
                LOADROW(row12L, row12H, 1)
                CALC_BLND_ROWS_MODE2(row12L, row12H);

                row11L = row12L;
                row11H = row12H;
                LOADROW(row12L, row12H, 2)
                dst = original_pDst + 16;
                CALC_BLND_ROWS_MODE2(row12L, row12H);

                refMain += 16;
                v_deltaPos = _mm_setzero_si128();

                LOADROW(row11L, row11H, 0)
                LOADROW(row12L, row12H, 1)
                dst = original_pDst + (16 * i_dst_stride);
                CALC_BLND_ROWS_MODE2(row12L, row12H);

                row11L = row12L;
                row11H = row12H;
                LOADROW(row12L, row12H, 2)
                dst = original_pDst + (16 * i_dst_stride) + 16;
                CALC_BLND_ROWS_MODE2(row12L, row12H);
                return;
            }

            CALC_FIRST_BLND_ROWS(0) ;
            dst = original_pDst + 16;
            CALC_SECOND_BLND_ROWS(16) ;

            refMain += 16;
            v_deltaPos = _mm_setzero_si128();

            dst = original_pDst + (16 * i_dst_stride);
            CALC_FIRST_BLND_ROWS(0) ;
            dst = original_pDst + (16 * i_dst_stride) + 16;
            CALC_SECOND_BLND_ROWS(16) ;
        }
        else
        {
            __m128i row11L, row12L, row11H, row12H;
            __m128i v_deltaFract, v_deltaPos, thirty2, thirty1, v_ipAngle;
            __m128i row11, row12, row13, row14, row21, row22, row23, row24;
            __m128i res1, res2;

            v_deltaPos = _mm_setzero_si128(); //v_deltaPos = 0;
            v_ipAngle = _mm_set1_epi16(intraPredAngle);
            thirty2 = _mm_set1_epi16(32);
            thirty1 = _mm_set1_epi16(31);
            __m128i itmp, it1, it2, it3, i16;

            switch (intraPredAngle)
            {
            case -2:
                LOADROW(row11, row12, -1)
                LOADROW(row21, row22,  0)
                LOADROW(row13, row14, 15)
                LOADROW(row23, row24, 16)
                for (int i = 0; i <= 14; i++)
                {
                    PREDANG_CALCROW_VER_MODE2(i);
                }

                //deltaFract == 0 for 16th row
                v_deltaPos = _mm_add_epi16(v_deltaPos, v_ipAngle);
                v_deltaFract = _mm_and_si128(v_deltaPos, thirty1);
                itmp = _mm_packus_epi16(row11, row12);
                _mm_storeu_si128((__m128i*)(dst + ((15) * i_dst_stride)), itmp);
                itmp = _mm_packus_epi16(row13, row14);
                _mm_storeu_si128((__m128i*)(dst + ((15) * i_dst_stride) + 16), itmp);

                row21 = row11;
                row22 = row12;
                row23 = row13;
                row24 = row14;

                LOADROW(row11, row12, -2)
                LOADROW(row13, row14, 14)
                for (int i = 16; i <= 30; i++)
                {
                    PREDANG_CALCROW_VER_MODE2(i);
                }

                itmp = _mm_packus_epi16(row11, row12);
                _mm_storeu_si128((__m128i*)(dst + ((31) * i_dst_stride)), itmp);
                itmp = _mm_packus_epi16(row13, row14);
                _mm_storeu_si128((__m128i*)(dst + ((31) * i_dst_stride) + 16), itmp);

                return;

            case  2:

                LOADROW(row11, row12, 0)
                LOADROW(row21, row22, 1)
                LOADROW(row13, row14, 16)
                LOADROW(row23, row24, 17)
                for (int i = 0; i <= 14; i++)
                {
                    PREDANG_CALCROW_VER_MODE2(i);
                }

                //deltaFract == 0 for 16th row

                v_deltaPos = _mm_add_epi16(v_deltaPos, v_ipAngle);
                v_deltaFract = _mm_and_si128(v_deltaPos, thirty1);
                itmp = _mm_packus_epi16(row21, row22);
                _mm_storeu_si128((__m128i*)(dst + ((15) * i_dst_stride)), itmp);
                itmp = _mm_packus_epi16(row23, row24);
                _mm_storeu_si128((__m128i*)(dst + ((15) * i_dst_stride) + 16), itmp);

                row11 = row21;
                row12 = row22;
                row13 = row23;
                row14 = row24;

                LOADROW(row21, row22, 2)
                LOADROW(row23, row24, 18)
                for (int i = 16; i <= 30; i++)
                {
                    PREDANG_CALCROW_VER_MODE2(i);
                }

                itmp = _mm_packus_epi16(row21, row22);
                _mm_storeu_si128((__m128i*)(dst + ((31) * i_dst_stride)), itmp);
                itmp = _mm_packus_epi16(row23, row24);
                _mm_storeu_si128((__m128i*)(dst + ((31) * i_dst_stride) + 16), itmp);

                return;
            }

            for (int i = 0; i <= 30; i++)
            {
                PREDANG_CALCROW_VER(i);
            }

            itmp = _mm_loadu_si128((__m128i const*)(refMain + 1 + GETAP(lookIdx, 31)));
            _mm_storeu_si128((__m128i*)(dst + ((31) * i_dst_stride)), itmp);
            itmp = _mm_loadu_si128((__m128i const*)(refMain + 17 + GETAP(lookIdx, 31)));
            _mm_storeu_si128((__m128i*)(dst + ((31) * i_dst_stride) + 16), itmp);
        }
    }
}

#undef SET_1_LEFT_0
#undef SET_15_LEFT_0
#undef SET_16_LEFT_0
#undef STORE_16_ANGLE_MINUS_32
#undef STORE_16_ANGLE_32
#undef CALC_FIRST_BLND_ROWS
