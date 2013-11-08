

#ifndef X265_INTRA_PRED_H
#define X265_INTRA_PRED_H

#define DECLARE_ONE_PRED_FUNCTION(name,mode_id,extent) \
	void x265_intra_pred_mode_##mode_id##_##name##_##extent(pixel *left, \
														pixel *top, \
														pixel* dst, \
														int i_dst_stride, \
														int i_chroma_idx, \
														int32_t i_bit_depth)
#define	P(name,mode_id,extent) \
	x265_intra_pred_mode_##mode_id##_##name##_##extent,

#define INITIALIZE_35_PRED_FUNCTION(n,e) \
x265_intra_pred_func x265_intra_pred_##n##_##e[35] = \
{ \
	0, \
	0, \
	P(n,2, e) \
	P(n,3, e) \
	P(n,4, e) \
	P(n,5, e) \
	P(n,6, e) \
	P(n,7, e) \
	P(n,8, e) \
	P(n,9, e) \
	P(n,10,e) \
	P(n,11,e) \
	P(n,12,e) \
	P(n,13,e) \
	P(n,14,e) \
	P(n,15,e) \
	P(n,16,e) \
	P(n,17,e) \
	P(n,18,e) \
	P(n,19,e) \
	P(n,20,e) \
	P(n,21,e) \
	P(n,22,e) \
	P(n,23,e) \
	P(n,24,e) \
	P(n,25,e) \
	P(n,26,e) \
	P(n,27,e) \
	P(n,28,e) \
	P(n,29,e) \
	P(n,30,e) \
	P(n,31,e) \
	P(n,32,e) \
	P(n,33,e) \
	P(n,34,e) \
	} ;

#define DECLARE_35_PRED_FUNCTION(name,extent) \
		DECLARE_ONE_PRED_FUNCTION(name, 0,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name, 1,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name, 2,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name, 3,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name, 4,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name, 5,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name, 6,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name, 7,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name, 8,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name, 9,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name,10,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name,11,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name,12,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name,13,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name,14,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name,15,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name,16,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name,17,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name,18,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name,19,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name,20,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name,21,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name,22,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name,23,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name,24,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name,25,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name,26,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name,27,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name,28,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name,29,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name,30,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name,31,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name,32,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name,33,extent) ; \
		DECLARE_ONE_PRED_FUNCTION(name,34,extent) ; \

#define DECLARE_ALL_PRED_FUNCTION(extent) \
		DECLARE_35_PRED_FUNCTION(0_8,extent) \
		DECLARE_35_PRED_FUNCTION(1_8,extent) \
		DECLARE_35_PRED_FUNCTION(2_8,extent) \
		DECLARE_35_PRED_FUNCTION(3_8,extent) \
		DECLARE_35_PRED_FUNCTION(4_8,extent) \

#define X265_INTRA_PRED_INITIALIZE(intra_pred_func,index,name,extent) \
		intra_pred_func[index][ 0] = x265_intra_pred_mode_0_##name##_##extent ;\
		intra_pred_func[index][ 1] = x265_intra_pred_mode_1_##name##_##extent ;\
		intra_pred_func[index][ 2] = x265_intra_pred_mode_2_##name##_##extent ;\
		intra_pred_func[index][ 3] = x265_intra_pred_mode_3_##name##_##extent ;\
		intra_pred_func[index][ 4] = x265_intra_pred_mode_4_##name##_##extent ;\
		intra_pred_func[index][ 5] = x265_intra_pred_mode_5_##name##_##extent ;\
		intra_pred_func[index][ 6] = x265_intra_pred_mode_6_##name##_##extent ;\
		intra_pred_func[index][ 7] = x265_intra_pred_mode_7_##name##_##extent ;\
		intra_pred_func[index][ 8] = x265_intra_pred_mode_8_##name##_##extent ;\
		intra_pred_func[index][ 9] = x265_intra_pred_mode_9_##name##_##extent ;\
		intra_pred_func[index][10] = x265_intra_pred_mode_10_##name##_##extent ;\
		intra_pred_func[index][11] = x265_intra_pred_mode_11_##name##_##extent ;\
		intra_pred_func[index][12] = x265_intra_pred_mode_12_##name##_##extent ;\
		intra_pred_func[index][13] = x265_intra_pred_mode_13_##name##_##extent ;\
		intra_pred_func[index][14] = x265_intra_pred_mode_14_##name##_##extent ;\
		intra_pred_func[index][15] = x265_intra_pred_mode_15_##name##_##extent ;\
		intra_pred_func[index][16] = x265_intra_pred_mode_16_##name##_##extent ;\
		intra_pred_func[index][17] = x265_intra_pred_mode_17_##name##_##extent ;\
		intra_pred_func[index][18] = x265_intra_pred_mode_18_##name##_##extent ;\
		intra_pred_func[index][19] = x265_intra_pred_mode_19_##name##_##extent ;\
		intra_pred_func[index][20] = x265_intra_pred_mode_20_##name##_##extent ;\
		intra_pred_func[index][21] = x265_intra_pred_mode_21_##name##_##extent ;\
		intra_pred_func[index][22] = x265_intra_pred_mode_22_##name##_##extent ;\
		intra_pred_func[index][23] = x265_intra_pred_mode_23_##name##_##extent ;\
		intra_pred_func[index][24] = x265_intra_pred_mode_24_##name##_##extent ;\
		intra_pred_func[index][25] = x265_intra_pred_mode_25_##name##_##extent ;\
		intra_pred_func[index][26] = x265_intra_pred_mode_26_##name##_##extent ;\
		intra_pred_func[index][27] = x265_intra_pred_mode_27_##name##_##extent ;\
		intra_pred_func[index][28] = x265_intra_pred_mode_28_##name##_##extent ;\
		intra_pred_func[index][29] = x265_intra_pred_mode_29_##name##_##extent ;\
		intra_pred_func[index][30] = x265_intra_pred_mode_30_##name##_##extent ;\
		intra_pred_func[index][31] = x265_intra_pred_mode_31_##name##_##extent ;\
		intra_pred_func[index][32] = x265_intra_pred_mode_32_##name##_##extent ;\
		intra_pred_func[index][33] = x265_intra_pred_mode_33_##name##_##extent ;\
		intra_pred_func[index][34] = x265_intra_pred_mode_34_##name##_##extent ;\

#define X265_INTRA_PRED_INITIALIZE_ALL(intra_pred_func,extent) \
		X265_INTRA_PRED_INITIALIZE(intra_pred_func,0,0_8,extent) \
		X265_INTRA_PRED_INITIALIZE(intra_pred_func,1,1_8,extent) \
		X265_INTRA_PRED_INITIALIZE(intra_pred_func,2,2_8,extent) \
		X265_INTRA_PRED_INITIALIZE(intra_pred_func,3,3_8,extent) \
		X265_INTRA_PRED_INITIALIZE(intra_pred_func,4,4_8,extent) \

#define DEFINE_5_INTRA_PRED_PLANAR_FUNCTION_C(extent) \
		DEFINE_ONE_INTRA_PRED_PLANAR_FUNCTION_C(0_8,extent, 4,2) \
		DEFINE_ONE_INTRA_PRED_PLANAR_FUNCTION_C(1_8,extent, 8,3) \
		DEFINE_ONE_INTRA_PRED_PLANAR_FUNCTION_C(2_8,extent,16,4) \
		DEFINE_ONE_INTRA_PRED_PLANAR_FUNCTION_C(3_8,extent,32,5) \
		DEFINE_ONE_INTRA_PRED_PLANAR_FUNCTION_C(4_8,extent,64,6) \

#define DEFINE_5_INTRA_PRED_DC_FUNCTION_C(extent) \
		DEFINE_ONE_INTRA_PRED_DC_FUNCTION_0_2_C(0_8,extent, 4,2) \
		DEFINE_ONE_INTRA_PRED_DC_FUNCTION_0_2_C(1_8,extent, 8,3) \
		DEFINE_ONE_INTRA_PRED_DC_FUNCTION_0_2_C(2_8,extent,16,4) \
		DEFINE_ONE_INTRA_PRED_DC_FUNCTION_3_4_C(3_8,extent,32,5) \
		DEFINE_ONE_INTRA_PRED_DC_FUNCTION_3_4_C(4_8,extent,64,6) \


#define DEFINE_33_INTRA_PRED_MODE_FUNCTION_C(name,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name, 2,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name, 3,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name, 4,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name, 5,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name, 6,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name, 7,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name, 8,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name, 9,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name,10,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name,11,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name,12,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name,13,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name,14,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name,15,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name,16,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name,17,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name,18,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name,19,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name,20,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name,21,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name,22,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name,23,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name,24,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name,25,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name,26,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name,27,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name,28,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name,29,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name,30,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name,31,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name,32,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name,33,extent,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_C(name,34,extent,blk_size) \

#define DEFINE_35_INTRA_PRED_MODE_FUNCTION_CMP(name,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name, 0,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name, 1,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name, 2,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name, 3,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name, 4,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name, 5,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name, 6,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name, 7,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name, 8,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name, 9,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name,10,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name,11,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name,12,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name,13,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name,14,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name,15,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name,16,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name,17,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name,18,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name,19,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name,20,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name,21,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name,22,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name,23,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name,24,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name,25,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name,26,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name,27,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name,28,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name,29,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name,30,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name,31,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name,32,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name,33,extent,extent1,extent2,blk_size) \
		DEFINE_ONE_INTRA_PRED_MODE_FUNCTION_CMP(name,34,extent,extent1,extent2,blk_size) \


typedef void (*x265_intra_pred_func)(pixel *left,
									pixel *top,
									pixel* dst,
									int i_dst_stride,
									int i_chroma_idx,
									int32_t i_bit_depth) ;

typedef struct _x265_intra_pred_t x265_intra_pred_t ;
struct _x265_intra_pred_t
{
	x265_intra_pred_func intra_pred_func[5][35] ;
};

DECLARE_ALL_PRED_FUNCTION(c)
DECLARE_ALL_PRED_FUNCTION(cmp)
DECLARE_ALL_PRED_FUNCTION(ssse3)


void x265_intra_pred_initialize ( x265_intra_pred_t *ip, unsigned int cpu ) ;


#endif


