


#include "common/common.h"
#include <emmintrin.h>
#include "mb8_x86.h"


#define MB81(R1, R2, R3, R4, R5, R6, R7, R8) { \
		itmp1 = _mm_unpacklo_epi8(R1, _mm_setzero_si128()); \
		itmp2 = _mm_unpackhi_epi8(R1, _mm_setzero_si128()); \
        R1 = _mm_packus_epi16(itmp1, itmp2); \
		itmp1 = _mm_unpacklo_epi8(R2, _mm_setzero_si128()); \
		itmp2 = _mm_unpackhi_epi8(R2, _mm_setzero_si128()); \
        R2 = _mm_packus_epi16(itmp1, itmp2); \
		itmp1 = _mm_unpacklo_epi8(R3, _mm_setzero_si128()); \
		itmp2 = _mm_unpackhi_epi8(R3, _mm_setzero_si128()); \
        R3 = _mm_packus_epi16(itmp1, itmp2); \
		itmp1 = _mm_unpacklo_epi8(R4, _mm_setzero_si128()); \
		itmp2 = _mm_unpackhi_epi8(R4, _mm_setzero_si128()); \
        R4 = _mm_packus_epi16(itmp1, itmp2); \
		itmp1 = _mm_unpacklo_epi8(R5, _mm_setzero_si128()); \
		itmp2 = _mm_unpackhi_epi8(R5, _mm_setzero_si128()); \
        R5 = _mm_packus_epi16(itmp1, itmp2); \
		itmp1 = _mm_unpacklo_epi8(R6, _mm_setzero_si128()); \
		itmp2 = _mm_unpackhi_epi8(R6, _mm_setzero_si128()); \
        R6 = _mm_packus_epi16(itmp1, itmp2); \
		itmp1 = _mm_unpacklo_epi8(R7, _mm_setzero_si128()); \
		itmp2 = _mm_unpackhi_epi8(R7, _mm_setzero_si128()); \
        R7 = _mm_packus_epi16(itmp1, itmp2); \
		itmp1 = _mm_unpacklo_epi8(R8, _mm_setzero_si128()); \
		itmp2 = _mm_unpackhi_epi8(R8, _mm_setzero_si128()); \
        R8 = _mm_packus_epi16(itmp1, itmp2); \
}

#define MB82(R1, R2, R3, R4, R5, R6, R7, R8) { \
        itmp1 = _mm_unpacklo_epi8(R1, R2); \
        itmp2 = _mm_unpackhi_epi8(R1, R2); \
        R1 = itmp1; \
        R2 = itmp2; \
        itmp1 = _mm_unpacklo_epi8(R3, R4); \
        itmp2 = _mm_unpackhi_epi8(R3, R4); \
        R3 = itmp1; \
        R4 = itmp2; \
        itmp1 = _mm_unpacklo_epi8(R5, R6); \
        itmp2 = _mm_unpackhi_epi8(R5, R6); \
        R5 = itmp1; \
        R6 = itmp2; \
        itmp1 = _mm_unpacklo_epi8(R7, R8); \
        itmp2 = _mm_unpackhi_epi8(R7, R8); \
        R7 = itmp1; \
        R8 = itmp2; \
}

#define MB83(R1, R2, R3, R4, R5, R6, R7, R8) { \
		itmp1 = _mm_unpacklo_epi16(R1, R3); \
        itmp2 = _mm_unpackhi_epi16(R1, R3); \
        R1 = itmp1; \
        R3 = itmp2; \
        itmp1 = _mm_unpacklo_epi16(R2, R4); \
        itmp2 = _mm_unpackhi_epi16(R2, R4); \
        R2 = itmp1; \
        R4 = itmp2; \
        itmp1 = _mm_unpacklo_epi16(R5, R7); \
        itmp2 = _mm_unpackhi_epi16(R5, R7); \
        R5 = itmp1; \
        R7 = itmp2; \
        itmp1 = _mm_unpacklo_epi16(R6, R8); \
        itmp2 = _mm_unpackhi_epi16(R6, R8); \
        R6 = itmp1; \
        R8 = itmp2; \
}

#define MB84(R1, R2, R3, R4, R5, R6, R7, R8) { \
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

#define BLND(R1, R2) { \
		itmp1 = _mm_unpacklo_epi64(R1, R2); \
        itmp2 = _mm_unpackhi_epi64(R1, R2); \
        R1 = itmp1; \
        R2 = itmp2; \
}

void PrintR (__m128i R)
{
	uint8_t array[16] ;

	_mm_storeu_si128((__m128i*)array, R) ;
	for ( int loop = 0 ; loop < 16 ; ++ loop )
	{
		fprintf ( stderr, "(%2d, %2d)  ",
				((array[loop]/8) + 1),
				((array[loop]%8) +1)) ;
	}
	fprintf ( stderr, "\n" ) ;
}

void Print4R(__m128i R1,
			__m128i R2,
			__m128i R3,
			__m128i R4)
{
	PrintR(R1) ;
	PrintR(R2) ;
	PrintR(R3) ;
	PrintR(R4) ;
}

void Print8R(__m128i R1,
			__m128i R2,
			__m128i R3,
			__m128i R4,
			__m128i R5,
			__m128i R6,
			__m128i R7,
			__m128i R8)
{
	PrintR(R1) ;
	PrintR(R2) ;
	PrintR(R3) ;
	PrintR(R4) ;
	PrintR(R5) ;
	PrintR(R6) ;
	PrintR(R7) ;
	PrintR(R8) ;
}

void mb8()
{
	uint8_t array[256] ;

	__m128i r1, r2, r3, r4, r5, r6, r7, r8 ;
	__m128i r9, r10, r11, r12, r13, r14, r15, r16 ;
	__m128i itmp1, itmp2 ;
	for ( int i = 0 ; i < 16 ; ++ i )
	{
		for (int j = 0 ; j < 16 ; ++ j )
		{
			array[16*i+j] = 16*i+j ;
		}
	}

	r1 =  _mm_loadu_si128 ((__m128i*)(array + 16 *  0)) ;
	r2 =  _mm_loadu_si128 ((__m128i*)(array + 16 *  1)) ;
	r3 =  _mm_loadu_si128 ((__m128i*)(array + 16 *  2)) ;
	r4 =  _mm_loadu_si128 ((__m128i*)(array + 16 *  3)) ;
	r5 =  _mm_loadu_si128 ((__m128i*)(array + 16 *  4)) ;
	r6 =  _mm_loadu_si128 ((__m128i*)(array + 16 *  5)) ;
	r7 =  _mm_loadu_si128 ((__m128i*)(array + 16 *  6)) ;
	r8 =  _mm_loadu_si128 ((__m128i*)(array + 16 *  7)) ;

	r9 =  _mm_loadu_si128 ((__m128i*)(array + 16 * 8)) ;
	r10 = _mm_loadu_si128 ((__m128i*)(array + 16 * 9)) ;
	r11 = _mm_loadu_si128 ((__m128i*)(array + 16 * 10)) ;
	r12 = _mm_loadu_si128 ((__m128i*)(array + 16 * 11)) ;
	r13 = _mm_loadu_si128 ((__m128i*)(array + 16 * 12)) ;
	r14 = _mm_loadu_si128 ((__m128i*)(array + 16 * 13)) ;
	r15 = _mm_loadu_si128 ((__m128i*)(array + 16 * 14)) ;
	r16 = _mm_loadu_si128 ((__m128i*)(array + 16 * 15)) ;

	Print8R( r1,  r2,  r3,  r4,  r5,  r6,  r7,  r8) ;
	Print8R( r9, r10, r11, r12, r13, r14, r15, r16) ;
	fprintf ( stderr, "\n" ) ;

	MB81(r1, r2, r3, r4, r5, r6, r7, r8) ;
	MB81( r9, r10, r11, r12, r13, r14, r15, r16) ;
	Print8R( r1,  r2,  r3,  r4,  r5,  r6,  r7,  r8) ;
	Print8R( r9, r10, r11, r12, r13, r14, r15, r16) ;
	fprintf ( stderr, "\n" ) ;

	MB82(r1, r2, r3, r4, r5, r6, r7, r8) ;
	MB82( r9, r10, r11, r12, r13, r14, r15, r16) ;
	Print8R( r1,  r2,  r3,  r4,  r5,  r6,  r7,  r8) ;
	Print8R( r9, r10, r11, r12, r13, r14, r15, r16) ;
	fprintf ( stderr, "\n" ) ;

	MB83(r1, r2, r3, r4, r5, r6, r7, r8) ;
	MB83( r9, r10, r11, r12, r13, r14, r15, r16) ;
	Print8R( r1,  r2,  r3,  r4,  r5,  r6,  r7,  r8) ;
	Print8R( r9, r10, r11, r12, r13, r14, r15, r16) ;
	fprintf ( stderr, "\n" ) ;

	MB84(r1, r2, r3, r4, r5, r6, r7, r8) ;
	MB84( r9, r10, r11, r12, r13, r14, r15, r16) ;
	Print8R( r1,  r2,  r3,  r4,  r5,  r6,  r7,  r8) ;
	Print8R( r9, r10, r11, r12, r13, r14, r15, r16) ;
	fprintf ( stderr, "\n" ) ;


	BLND (r1,  r9) ;
	BLND (r5, r13) ;
	BLND (r3, r11) ;
	BLND (r7, r15) ;
	BLND (r2, r10) ;
	BLND (r6, r14) ;
	BLND (r4, r12) ;
	BLND (r8, r16) ;
	Print8R( r1,  r9,  r5,  r13,  r3,  r11,  r7,  r15) ;
	Print8R( r2, r10,  r6,  r14,  r4,  r12,  r8, r16) ;
	fprintf ( stderr, "\n" ) ;


}

void mb81()
{
	uint8_t array[256] ;

	__m128i r1, r2, r3, r4, r5, r6, r7, r8 ;
	__m128i itmp1, itmp2 ;
	for ( int i = 0 ; i < 8 ; i += 2 )
	{
		for (int j = 0 ; j < 8 ; ++ j )
		{
			array[8*i+j] = 8*i+j ;
			array[8*i+j+8] = 8*i+j+8 ;
		}
	}

	r1 =  _mm_loadu_si128 ((__m128i*)(array + 16 *  0)) ;
	r2 =  _mm_loadu_si128 ((__m128i*)(array + 16 *  1)) ;
	r3 =  _mm_loadu_si128 ((__m128i*)(array + 16 *  2)) ;
	r4 =  _mm_loadu_si128 ((__m128i*)(array + 16 *  3)) ;
	r5 =  _mm_loadu_si128 ((__m128i*)(array + 16 *  4)) ;
	r6 =  _mm_loadu_si128 ((__m128i*)(array + 16 *  5)) ;
	r7 =  _mm_loadu_si128 ((__m128i*)(array + 16 *  6)) ;
	r8 =  _mm_loadu_si128 ((__m128i*)(array + 16 *  7)) ;


	Print4R( r1,  r2,  r3,  r4) ;
	fprintf ( stderr, "\n" ) ;

	MB82(r1, r2, r3, r4, r5, r6, r7, r8) ;
	Print4R( r1,  r2,  r3,  r4) ;
	fprintf ( stderr, "\n" ) ;

	MB83(r1, r2, r3, r4, r5, r6, r7, r8) ;
	Print4R( r1,  r2,  r3,  r4) ;
	fprintf ( stderr, "\n" ) ;

	MB82(r1, r2, r3, r4, r5, r6, r7, r8) ;
	Print4R( r1,  r2,  r3,  r4) ;
	fprintf ( stderr, "\n" ) ;





}

