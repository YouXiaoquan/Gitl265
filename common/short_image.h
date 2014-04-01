

#ifndef X265_SHORT_IMAGE_H
#define X265_SHORT_IMAGE_H

typedef struct _x265_image_t x265_image_t ;


typedef struct
{
    int     i_width;     /* width of the picture */
    int     i_height;    /* height of the picture */
    int     i_stride[3]; /* Strides for each plane */
    short_pixel  *buffer ;
    short_pixel  *plane[3];   /* Pointers to each plane */
} x265_short_image_t;


int x265_short_image_init ( x265_short_image_t *short_image ) ;
void x265_short_image_deinit ( x265_short_image_t *short_image ) ;
x265_short_image_t *x265_short_image_new () ;
void x265_short_image_delete ( x265_short_image_t *short_image ) ;
int x265_short_image_create ( x265_short_image_t *short_image, uint32_t i_width, uint32_t i_height ) ;
void x265_short_image_destroy ( x265_short_image_t *short_image ) ;

void x265_short_image_clear ( x265_short_image_t *short_image ) ;
int x265_short_image_create_intra_short_image ( x265_short_image_t *short_image, uint32_t i_width, uint32_t i_height ) ;
int32_t x265_short_image_get_addr_offset_p4 ( x265_t *h, x265_short_image_t *short_image,
										uint32_t i_part_unit_idx, uint32_t i_width ) ;
int32_t x265_short_image_get_addr_offset_p5 ( x265_t *h, x265_short_image_t *short_image,
										uint32_t i_trans_unit_idx, uint32_t i_blk_size, uint32_t i_width ) ;

short_pixel* x265_short_image_get_luma_addr_p2 ( x265_t *h, x265_short_image_t* short_image ) ;
short_pixel* x265_short_image_get_cb_addr_p2 ( x265_t *h, x265_short_image_t* short_image );
short_pixel* x265_short_image_get_cr_addr_p2 ( x265_t *h, x265_short_image_t* short_image );

//  access starting position of short_image partition unit buffer
short_pixel* x265_short_image_get_luma_addr_p3 ( x265_t *h, x265_short_image_t* short_image, uint32_t i_part_unit_idx ) ;
short_pixel* x265_short_image_get_cb_addr_p3 ( x265_t *h, x265_short_image_t* short_image, uint32_t i_part_unit_idx ) ;
short_pixel* x265_short_image_get_cr_addr_p3 ( x265_t *h, x265_short_image_t* short_image, uint32_t i_part_unit_idx ) ;

//  access starting position of short_image transform unit buffer
short_pixel* x265_short_image_get_luma_addr_p4( x265_t *h, x265_short_image_t* short_image, uint32_t i_trans_unit_idx, uint32_t i_blk_size ) ;
short_pixel* x265_short_image_get_cb_addr_p4 ( x265_t *h, x265_short_image_t* short_image, uint32_t i_trans_unit_idx, uint32_t i_blk_size ) ;
short_pixel* x265_short_image_get_cr_addr_p4 ( x265_t *h, x265_short_image_t* short_image, uint32_t i_trans_unit_idx, uint32_t i_blk_size ) ;

//  get stride value of short_image buffer
uint32_t x265_short_image_get_stride( x265_short_image_t* short_image ) ;
uint32_t x265_short_image_get_c_stride( x265_short_image_t* short_image ) ;

uint32_t x265_short_image_get_width( x265_short_image_t* short_image ) ;
uint32_t x265_short_image_get_height( x265_short_image_t* short_image ) ;
uint32_t x265_short_image_get_c_width( x265_short_image_t* short_image ) ;
uint32_t x265_short_image_get_c_height( x265_short_image_t* short_image ) ;


typedef struct _x265_frame_t x265_frame_t ;

void x265_short_image_copy_to_frame ( x265_t *h, x265_short_image_t *short_image, x265_frame_t* frame_dst, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx ) ;
void x265_short_image_copy_to_frame_luma  ( x265_t *h, x265_short_image_t *short_image, x265_frame_t* frame_dst, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx ) ;
void x265_short_image_copy_to_frame_chroma( x265_t *h, x265_short_image_t *short_image, x265_frame_t* frame_dst, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx ) ;
void x265_short_image_copy_from_frame   ( x265_t *h, x265_short_image_t *short_image, x265_frame_t* pc_frame_src, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx ) ;
void x265_short_image_copy_from_frame_luma ( x265_t *h, x265_short_image_t *short_image, x265_frame_t* pc_frame_src, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx ) ;
void x265_short_image_copy_from_frame_chroma( x265_t *h, x265_short_image_t *short_image, x265_frame_t* pc_frame_src, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx ) ;

void x265_short_image_copy_to_part_short_image( x265_t *h, x265_short_image_t *short_image, x265_short_image_t* short_image_dst, uint32_t i_dst_part_idx ) ;
void x265_short_image_copy_to_part_luma( x265_t *h, x265_short_image_t *short_image, x265_short_image_t* short_image_dst, uint32_t i_dst_part_idx ) ;
void x265_short_image_copy_to_part_chroma( x265_t *h, x265_short_image_t *short_image, x265_short_image_t* short_image_dst, uint32_t i_dst_part_idx ) ;

void x265_short_image_copy_part_to_short_image( x265_t *h, x265_short_image_t *short_image, x265_short_image_t* short_image_dst, uint32_t i_src_part_idx ) ;
void x265_short_image_copy_part_to_luma( x265_t *h, x265_short_image_t *short_image, x265_short_image_t* short_image_dst, uint32_t i_src_part_idx ) ;
void x265_short_image_copy_part_to_chroma( x265_t *h, x265_short_image_t *short_image, x265_short_image_t* short_image_dst, uint32_t i_src_part_idx ) ;
void x265_short_image_copy_part_to_part_short_image_p ( x265_t *h, x265_short_image_t *short_image, x265_image_t* image_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height ) ;
void x265_short_image_copy_part_to_part_luma_p ( x265_t *h, x265_short_image_t *short_image, x265_image_t* image_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height ) ;
void x265_short_image_copy_part_to_part_chroma_p( x265_t *h, x265_short_image_t *short_image, x265_image_t* image_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height ) ;
void x265_short_image_copy_part_to_part_short_image_s ( x265_t *h, x265_short_image_t *short_image, x265_short_image_t* image_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height ) ;
void x265_short_image_copy_part_to_part_luma_s ( x265_t *h, x265_short_image_t *short_image, x265_short_image_t* image_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height ) ;
void x265_short_image_copy_part_to_part_chroma_s( x265_t *h, x265_short_image_t *short_image, x265_short_image_t* image_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height ) ;
void x265_short_image_copy_part_to_part_chroma_p7( x265_t *h, x265_short_image_t *short_image, x265_short_image_t* short_image_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height, uint32_t chroma_id) ;


void x265_short_image_add_clip( x265_t *h, x265_short_image_t *short_image, x265_short_image_t* short_image_src0, x265_short_image_t* short_image_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size ) ;
void x265_short_image_add_clip_luma( x265_t *h, x265_short_image_t *short_image, x265_short_image_t* short_image_src0, x265_short_image_t* short_image_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size ) ;
void x265_short_image_add_clip_chroma( x265_t *h, x265_short_image_t *short_image, x265_short_image_t* short_image_src0, x265_short_image_t* short_image_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size ) ;
void x265_short_image_subtract( x265_t *h, x265_short_image_t *short_image, x265_image_t* short_image_src0, x265_image_t* short_image_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size ) ;
void x265_short_image_subtract_luma( x265_t *h, x265_short_image_t *short_image, x265_image_t* short_image_src0, x265_image_t* short_image_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size ) ;
void x265_short_image_subtract_chroma( x265_t *h, x265_short_image_t *short_image, x265_image_t* short_image_src0, x265_image_t* short_image_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size ) ;
void x265_short_image_add_avg( x265_t *h, x265_short_image_t *short_image, x265_short_image_t* short_image_src0, x265_short_image_t* short_image_src1, uint32_t i_part_unit_idx, uint32_t i_width, uint32_t i_height ) ;
void x265_short_image_remove_high_freq( x265_t *h, x265_short_image_t *short_image, x265_short_image_t* short_image_src, uint32_t i_part_idx, uint32_t i_widht, uint32_t i_height ) ;


void x265_short_image_copy_up_from_short_image ( x265_t *h, x265_short_image_t *short_image, x265_short_image_t* short_image_src, uint32_t i_cu_addr_x, int32_t i_width ) ;
void x265_short_image_copy_up_from_short_image_luma ( x265_t *h, x265_short_image_t *short_image, x265_short_image_t* short_image_src, uint32_t i_cu_addr_x, int32_t i_width ) ;
void x265_short_image_copy_up_from_short_image_chroma ( x265_t *h, x265_short_image_t *short_image, x265_short_image_t* short_image_src, uint32_t i_cu_addr_x, int32_t i_width ) ;

void x265_short_image_copy_left_from_short_image ( x265_t *h, x265_short_image_t *short_image, x265_short_image_t* short_image_src ) ;
void x265_short_image_copy_left_from_short_image_luma ( x265_t *h, x265_short_image_t *short_image, x265_short_image_t* short_image_src ) ;
void x265_short_image_copy_left_from_short_image_chroma ( x265_t *h, x265_short_image_t *short_image, x265_short_image_t* short_image_src ) ;

void x265_short_image_copy_right_from_frame ( x265_t *h, x265_short_image_t *short_image, x265_frame_t* frame_src, uint32_t i_cu_addr ) ;
void x265_short_image_copy_right_from_frame_luma ( x265_t *h, x265_short_image_t *short_image, x265_frame_t* frame_src, uint32_t i_cu_addr ) ;
void x265_short_image_copy_right_from_frame_chroma ( x265_t *h, x265_short_image_t *short_image, x265_frame_t* frame_src, uint32_t i_cu_addr ) ;
void x265_short_image_copy_bottom_from_frame ( x265_t *h, x265_short_image_t *short_image, x265_frame_t* frame_src, uint32_t i_cu_addr_x, uint32_t i_cu_addr ) ;
void x265_short_image_copy_bottom_from_frame_luma ( x265_t *h, x265_short_image_t *short_image, x265_frame_t* frame_src, uint32_t i_cu_addr_x, uint32_t i_cu_addr ) ;
void x265_short_image_copy_bottom_from_frame_chroma ( x265_t *h, x265_short_image_t *short_image, x265_frame_t* frame_src, uint32_t i_cu_addr_x, uint32_t i_cu_addr ) ;

void x265_short_image_copy_to_short_image ( x265_t *h, x265_short_image_t *short_image, x265_short_image_t* short_image_dst, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx ) ;
void x265_short_image_copy_to_short_image_luma ( x265_t *h, x265_short_image_t *short_image, x265_short_image_t* short_image_dst, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx ) ;
void x265_short_image_copy_to_short_image_chroma ( x265_t *h, x265_short_image_t *short_image, x265_short_image_t* short_image_dst, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx ) ;

#endif


