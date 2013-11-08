



#ifndef X265_IP_H
#define X265_IP_H

#define	OUTPUT_DEBUG								1

#define	DECLARE_IP_FILTER_FUNC(dir,chroma,src_type,src_type_name,dst_type,dst_type_name,frac,extent) \
	void x265_ip_filter_##dir##_##chroma##_##src_type_name##_##dst_type_name##_##frac##_##extent(\
														src_type *ref, \
														int32_t i_ref_stride, \
														dst_type *dst, \
														int32_t i_dst_stride, \
														int32_t i_width, \
														int32_t i_height, \
														int32_t i_bit_depth, \
														int32_t *dst_int ) ; \

#define	DECLARE_ALL_IP_FILTER_LUMA_FUNC(dir,src_type,src_type_name,dst_type,dst_type_name,extent) \
	DECLARE_IP_FILTER_FUNC(dir,luma,src_type,src_type_name,dst_type,dst_type_name,1,extent) \
	DECLARE_IP_FILTER_FUNC(dir,luma,src_type,src_type_name,dst_type,dst_type_name,2,extent) \
	DECLARE_IP_FILTER_FUNC(dir,luma,src_type,src_type_name,dst_type,dst_type_name,3,extent) \

#define	DECLARE_ALL_IP_FILTER_CHROMA_FUNC(dir,src_type,src_type_name,dst_type,dst_type_name,extent) \
	DECLARE_IP_FILTER_FUNC(dir,chroma,src_type,src_type_name,dst_type,dst_type_name,1,extent) \
	DECLARE_IP_FILTER_FUNC(dir,chroma,src_type,src_type_name,dst_type,dst_type_name,2,extent) \
	DECLARE_IP_FILTER_FUNC(dir,chroma,src_type,src_type_name,dst_type,dst_type_name,3,extent) \
	DECLARE_IP_FILTER_FUNC(dir,chroma,src_type,src_type_name,dst_type,dst_type_name,4,extent) \
	DECLARE_IP_FILTER_FUNC(dir,chroma,src_type,src_type_name,dst_type,dst_type_name,5,extent) \
	DECLARE_IP_FILTER_FUNC(dir,chroma,src_type,src_type_name,dst_type,dst_type_name,6,extent) \
	DECLARE_IP_FILTER_FUNC(dir,chroma,src_type,src_type_name,dst_type,dst_type_name,7,extent) \


typedef void (*x265_ip_filter_p_p_func)(pixel *ref,
										int32_t i_ref_stride,
										pixel *dst,
										int32_t i_dst_stride,
										int32_t i_width,
										int32_t i_height,
										int32_t i_bit_depth
#if OUTPUT_DEBUG
										,int32_t *dst_int
#endif
													) ;

typedef void (*x265_ip_filter_p_s_func)(pixel *ref,
										int32_t i_ref_stride,
										spixel *dst,
										int32_t i_dst_stride,
										int32_t i_width,
										int32_t i_height,
										int32_t i_bit_depth
#if OUTPUT_DEBUG
										,int32_t *dst_int
#endif
													) ;

typedef void (*x265_ip_filter_s_p_func)(spixel *ref,
										int32_t i_ref_stride,
										pixel *dst,
										int32_t i_dst_stride,
										int32_t i_width,
										int32_t i_height,
										int32_t i_bit_depth
#if OUTPUT_DEBUG
										,int32_t *dst_int
#endif
													) ;
typedef void (*x265_ip_filter_s_s_func)(spixel *ref,
										int32_t i_ref_stride,
										spixel *dst,
										int32_t i_dst_stride,
										int32_t i_width,
										int32_t i_height,
										int32_t i_bit_depth
#if OUTPUT_DEBUG
										,int32_t *dst_int
#endif
													) ;


typedef struct
{
	x265_ip_filter_p_p_func ip_filter_hor_luma_p_p[4] ;
	x265_ip_filter_p_s_func ip_filter_hor_luma_p_s[4] ;
	x265_ip_filter_p_p_func ip_filter_ver_luma_p_p[4] ;
	x265_ip_filter_p_s_func ip_filter_ver_luma_p_s[4] ;
	x265_ip_filter_s_p_func ip_filter_ver_luma_s_p[4] ;
	x265_ip_filter_s_s_func ip_filter_ver_luma_s_s[4] ;
	x265_ip_filter_p_p_func ip_filter_hor_chroma_p_p[8] ;
	x265_ip_filter_p_s_func ip_filter_hor_chroma_p_s[8] ;
	x265_ip_filter_p_p_func ip_filter_ver_chroma_p_p[8] ;
	x265_ip_filter_p_s_func ip_filter_ver_chroma_p_s[8] ;
	x265_ip_filter_s_p_func ip_filter_ver_chroma_s_p[8] ;
	x265_ip_filter_s_s_func ip_filter_ver_chroma_s_s[8] ;

} x265_ip_t ;


void x265_ip_initialize ( x265_ip_t *ip, unsigned int cpu ) ;

DECLARE_ALL_IP_FILTER_LUMA_FUNC(hor,pixel,p,pixel,p,c)
DECLARE_ALL_IP_FILTER_LUMA_FUNC(hor,pixel,p,pixel,p,ssse3)
DECLARE_ALL_IP_FILTER_LUMA_FUNC(hor,pixel,p,pixel,p,cmp)
DECLARE_ALL_IP_FILTER_CHROMA_FUNC(hor,pixel,p,pixel,p,c)
DECLARE_ALL_IP_FILTER_CHROMA_FUNC(hor,pixel,p,pixel,p,ssse3)
DECLARE_ALL_IP_FILTER_CHROMA_FUNC(hor,pixel,p,pixel,p,cmp)

DECLARE_ALL_IP_FILTER_LUMA_FUNC(hor,pixel,p,spixel,s,c)
DECLARE_ALL_IP_FILTER_LUMA_FUNC(hor,pixel,p,spixel,s,ssse3)
DECLARE_ALL_IP_FILTER_LUMA_FUNC(hor,pixel,p,spixel,s,cmp)
DECLARE_ALL_IP_FILTER_CHROMA_FUNC(hor,pixel,p,spixel,s,c)
DECLARE_ALL_IP_FILTER_CHROMA_FUNC(hor,pixel,p,spixel,s,ssse3)
DECLARE_ALL_IP_FILTER_CHROMA_FUNC(hor,pixel,p,spixel,s,cmp)


DECLARE_ALL_IP_FILTER_LUMA_FUNC(ver,pixel,p,pixel,p,c)
DECLARE_ALL_IP_FILTER_LUMA_FUNC(ver,pixel,p,pixel,p,ssse3)
DECLARE_ALL_IP_FILTER_LUMA_FUNC(ver,pixel,p,pixel,p,cmp)
DECLARE_ALL_IP_FILTER_CHROMA_FUNC(ver,pixel,p,pixel,p,c)
DECLARE_ALL_IP_FILTER_CHROMA_FUNC(ver,pixel,p,pixel,p,ssse3)
DECLARE_ALL_IP_FILTER_CHROMA_FUNC(ver,pixel,p,pixel,p,cmp)

DECLARE_ALL_IP_FILTER_LUMA_FUNC(ver,pixel,p,spixel,s,c)
DECLARE_ALL_IP_FILTER_LUMA_FUNC(ver,pixel,p,spixel,s,ssse3)
DECLARE_ALL_IP_FILTER_LUMA_FUNC(ver,pixel,p,spixel,s,cmp)
DECLARE_ALL_IP_FILTER_CHROMA_FUNC(ver,pixel,p,spixel,s,c)
DECLARE_ALL_IP_FILTER_CHROMA_FUNC(ver,pixel,p,spixel,s,ssse3)
DECLARE_ALL_IP_FILTER_CHROMA_FUNC(ver,pixel,p,spixel,s,cmp)

DECLARE_ALL_IP_FILTER_LUMA_FUNC(ver,spixel,s,pixel,p,c)
DECLARE_ALL_IP_FILTER_LUMA_FUNC(ver,spixel,s,pixel,p,ssse3)
DECLARE_ALL_IP_FILTER_LUMA_FUNC(ver,spixel,s,pixel,p,cmp)
DECLARE_ALL_IP_FILTER_CHROMA_FUNC(ver,spixel,s,pixel,p,c)
DECLARE_ALL_IP_FILTER_CHROMA_FUNC(ver,spixel,s,pixel,p,ssse3)
DECLARE_ALL_IP_FILTER_CHROMA_FUNC(ver,spixel,s,pixel,p,cmp)

DECLARE_ALL_IP_FILTER_LUMA_FUNC(ver,spixel,s,spixel,s,c)
DECLARE_ALL_IP_FILTER_LUMA_FUNC(ver,spixel,s,spixel,s,ssse3)
DECLARE_ALL_IP_FILTER_LUMA_FUNC(ver,spixel,s,spixel,s,cmp)
DECLARE_ALL_IP_FILTER_CHROMA_FUNC(ver,spixel,s,spixel,s,c)
DECLARE_ALL_IP_FILTER_CHROMA_FUNC(ver,spixel,s,spixel,s,ssse3)
DECLARE_ALL_IP_FILTER_CHROMA_FUNC(ver,spixel,s,spixel,s,cmp)



#endif

