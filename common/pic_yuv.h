


#ifndef X265_PIC_YUV_H
#define X265_PIC_YUV_H


typedef struct
{
	pixel*pic_buf_y; ///< Buffer (including margin)
	pixel*pic_buf_u;
	pixel*pic_buf_v;

	pixel*pic_org_y;///< m_apiPicBufY + i_MarginLuma*getStride() + i_MarginLuma
	pixel*pic_org_u;
	pixel*pic_org_v;

	// ------------------------------------------------------------------------------------------------
	//Parameter for general YUV buffer usage
	// ------------------------------------------------------------------------------------------------

	int32_t i_pic_width;///< Width of picture
	int32_t i_pic_height; ///< Height of picture

	int32_t i_cu_width; ///< Width of Coding Unit (CU)
	int32_t i_cu_height;///< Height of Coding Unit (CU)
	int32_t* cu_offset_y;
	int32_t* cu_offset_c;
	int32_t* bu_offset_y;
	int32_t* bu_offset_c;

	int32_t i_luma_margin_x;
	int32_t i_luma_margin_y;
	int32_t i_chroma_margin_x;
	int32_t i_chroma_margin_y;

	int32_t b_is_border_extended;

} x265_pic_yuv_t ;


x265_pic_yuv_t *x265_pic_yuv_new () ;
void x265_pic_yuv_delete ( x265_pic_yuv_t *pic_yuv ) ;

int x265_pic_yuv_init ( x265_pic_yuv_t *pic_yuv );
void x265_pic_yuv_deinit ( x265_pic_yuv_t *pic_yuv );

#endif

