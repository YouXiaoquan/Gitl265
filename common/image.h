

#ifndef X265_IMAGE_H
#define X265_IMAGE_H



struct _x265_image_t
{
    int     i_width;     /* width of the picture */
    int     i_height;    /* height of the picture */
    int     i_stride[3]; /* Strides for each plane */
    pixel  *buffer ;
    pixel  *plane[3];   /* Pointers to each plane */
} ;

int x265_image_init ( x265_image_t *image ) ;
void x265_image_deinit ( x265_image_t *image ) ;
x265_image_t *x265_image_new () ;
void x265_image_delete ( x265_image_t *image ) ;
int x265_image_create ( x265_image_t *image, uint32_t i_width, uint32_t i_height ) ;
void x265_image_destroy ( x265_image_t *image ) ;

void x265_image_clear ( x265_image_t *image ) ;
int x265_image_create_intra_image ( x265_image_t *image, uint32_t i_width, uint32_t i_height ) ;
int32_t x265_image_get_addr_offset_p4 ( x265_t *h, x265_image_t *image,
										uint32_t i_part_unit_idx, uint32_t i_width ) ;
int32_t x265_image_get_addr_offset_p5 ( x265_t *h, x265_image_t *image,
										uint32_t i_trans_unit_idx, uint32_t i_blk_size, uint32_t i_width ) ;

pixel* x265_image_get_luma_addr_p2 ( x265_t *h, x265_image_t* image ) ;
pixel* x265_image_get_cb_addr_p2 ( x265_t *h, x265_image_t* image );
pixel* x265_image_get_cr_addr_p2 ( x265_t *h, x265_image_t* image );

//  access starting position of image partition unit buffer
pixel* x265_image_get_luma_addr_p3 ( x265_t *h, x265_image_t* image, uint32_t i_part_unit_idx ) ;
pixel* x265_image_get_cb_addr_p3 ( x265_t *h, x265_image_t* image, uint32_t i_part_unit_idx ) ;
pixel* x265_image_get_cr_addr_p3 ( x265_t *h, x265_image_t* image, uint32_t i_part_unit_idx ) ;

//  access starting position of image transform unit buffer
pixel* x265_image_get_luma_addr_p4( x265_t *h, x265_image_t* image, uint32_t i_trans_unit_idx, uint32_t i_blk_size ) ;
pixel* x265_image_get_cb_addr_p4 ( x265_t *h, x265_image_t* image, uint32_t i_trans_unit_idx, uint32_t i_blk_size ) ;
pixel* x265_image_get_cr_addr_p4 ( x265_t *h, x265_image_t* image, uint32_t i_trans_unit_idx, uint32_t i_blk_size ) ;

//  get stride value of image buffer
uint32_t x265_image_get_stride( x265_image_t* image ) ;
uint32_t x265_image_get_c_stride( x265_image_t* image ) ;

uint32_t x265_image_get_width( x265_image_t* image ) ;
uint32_t x265_image_get_height( x265_image_t* image ) ;
uint32_t x265_image_get_c_width( x265_image_t* image ) ;
uint32_t x265_image_get_c_height( x265_image_t* image ) ;


typedef struct _x265_frame_t x265_frame_t ;

void x265_image_copy_to_frame ( x265_t *h, x265_image_t *image, x265_frame_t* frame_dst, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx ) ;
void x265_image_copy_to_frame_luma  ( x265_t *h, x265_image_t *image, x265_frame_t* frame_dst, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx ) ;
void x265_image_copy_to_frame_chroma( x265_t *h, x265_image_t *image, x265_frame_t* frame_dst, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx ) ;
void x265_image_copy_from_frame   ( x265_t *h, x265_image_t *image, x265_frame_t* pc_frame_src, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx ) ;
void x265_image_copy_from_frame_luma ( x265_t *h, x265_image_t *image, x265_frame_t* pc_frame_src, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx ) ;
void x265_image_copy_from_frame_chroma( x265_t *h, x265_image_t *image, x265_frame_t* pc_frame_src, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx ) ;

void x265_image_copy_to_part_image( x265_t *h, x265_image_t *image, x265_image_t* image_dst, uint32_t i_dst_part_idx ) ;
void x265_image_copy_to_part_luma( x265_t *h, x265_image_t *image, x265_image_t* image_dst, uint32_t i_dst_part_idx ) ;
void x265_image_copy_to_part_chroma( x265_t *h, x265_image_t *image, x265_image_t* image_dst, uint32_t i_dst_part_idx ) ;

void x265_image_copy_part_to_image( x265_t *h, x265_image_t *image, x265_image_t* image_dst, uint32_t i_src_part_idx ) ;
void x265_image_copy_part_to_luma( x265_t *h, x265_image_t *image, x265_image_t* image_dst, uint32_t i_src_part_idx ) ;
void x265_image_copy_part_to_chroma( x265_t *h, x265_image_t *image, x265_image_t* image_dst, uint32_t i_src_part_idx ) ;
void x265_image_copy_part_to_part_image ( x265_t *h, x265_image_t *image, x265_image_t* image_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height ) ;
void x265_image_copy_part_to_part_luma ( x265_t *h, x265_image_t *image, x265_image_t* image_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height ) ;
void x265_image_copy_part_to_part_chroma( x265_t *h, x265_image_t *image, x265_image_t* image_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height ) ;
void x265_image_copy_part_to_part_chroma_p7( x265_t *h, x265_image_t *image, x265_image_t* image_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height, uint32_t chroma_id) ;


void x265_image_add_clip( x265_t *h, x265_image_t *image, x265_image_t* image_src0, x265_simage_t* image_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size ) ;
void x265_image_add_clip_luma( x265_t *h, x265_image_t *image, x265_image_t* image_src0, x265_simage_t* image_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size ) ;
void x265_image_add_clip_chroma( x265_t *h, x265_image_t *image, x265_image_t* image_src0, x265_simage_t* image_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size ) ;
void x265_image_subtract( x265_t *h, x265_image_t *image, x265_image_t* image_src0, x265_image_t* image_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size ) ;
void x265_image_subtract_luma( x265_t *h, x265_image_t *image, x265_image_t* image_src0, x265_image_t* image_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size ) ;
void x265_image_subtract_chroma( x265_t *h, x265_image_t *image, x265_image_t* image_src0, x265_image_t* image_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size ) ;
void x265_image_add_avg( x265_t *h, x265_image_t *image, x265_simage_t* image_src0, x265_simage_t* image_src1, uint32_t i_part_unit_idx, uint32_t i_width, uint32_t i_height ) ;
void x265_image_remove_high_freq( x265_t *h, x265_image_t *image, x265_image_t* image_src, uint32_t i_part_idx, uint32_t i_widht, uint32_t i_height ) ;


void x265_image_copy_up_from_image ( x265_t *h, x265_image_t *image, x265_image_t* image_src, uint32_t i_cu_addr_x, int32_t i_width ) ;
void x265_image_copy_up_from_image_luma ( x265_t *h, x265_image_t *image, x265_image_t* image_src, uint32_t i_cu_addr_x, int32_t i_width ) ;
void x265_image_copy_up_from_image_chroma ( x265_t *h, x265_image_t *image, x265_image_t* image_src, uint32_t i_cu_addr_x, int32_t i_width ) ;

void x265_image_copy_left_from_image ( x265_t *h, x265_image_t *image, x265_image_t* image_src ) ;
void x265_image_copy_left_from_image_luma ( x265_t *h, x265_image_t *image, x265_image_t* image_src ) ;
void x265_image_copy_left_from_image_chroma ( x265_t *h, x265_image_t *image, x265_image_t* image_src ) ;

void x265_image_copy_right_from_frame ( x265_t *h, x265_image_t *image, x265_frame_t* frame_src, uint32_t i_cu_addr ) ;
void x265_image_copy_right_from_frame_luma ( x265_t *h, x265_image_t *image, x265_frame_t* frame_src, uint32_t i_cu_addr ) ;
void x265_image_copy_right_from_frame_chroma ( x265_t *h, x265_image_t *image, x265_frame_t* frame_src, uint32_t i_cu_addr ) ;
void x265_image_copy_bottom_from_frame ( x265_t *h, x265_image_t *image, x265_frame_t* frame_src, uint32_t i_cu_addr_x, uint32_t i_cu_addr ) ;
void x265_image_copy_bottom_from_frame_luma ( x265_t *h, x265_image_t *image, x265_frame_t* frame_src, uint32_t i_cu_addr_x, uint32_t i_cu_addr ) ;
void x265_image_copy_bottom_from_frame_chroma ( x265_t *h, x265_image_t *image, x265_frame_t* frame_src, uint32_t i_cu_addr_x, uint32_t i_cu_addr ) ;

void x265_image_copy_to_image ( x265_t *h, x265_image_t *image, x265_image_t* image_dst, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx ) ;
void x265_image_copy_to_image_luma ( x265_t *h, x265_image_t *image, x265_image_t* image_dst, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx ) ;
void x265_image_copy_to_image_chroma ( x265_t *h, x265_image_t *image, x265_image_t* image_dst, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx ) ;

#endif


