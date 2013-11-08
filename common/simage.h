

#ifndef X265_SIMAGE_H
#define X265_SIMAGE_H

typedef struct _x265_image_t x265_image_t ;


typedef struct
{
    int     i_width;     /* width of the picture */
    int     i_height;    /* height of the picture */
    int     i_stride[3]; /* Strides for each plane */
    spixel  *buffer ;
    spixel  *plane[3];   /* Pointers to each plane */
} x265_simage_t;


int x265_simage_init ( x265_simage_t *simage ) ;
void x265_simage_deinit ( x265_simage_t *simage ) ;
x265_simage_t *x265_simage_new () ;
void x265_simage_delete ( x265_simage_t *simage ) ;
int x265_simage_create ( x265_simage_t *simage, uint32_t i_width, uint32_t i_height ) ;
void x265_simage_destroy ( x265_simage_t *simage ) ;

void x265_simage_clear ( x265_simage_t *simage ) ;
int x265_simage_create_intra_simage ( x265_simage_t *simage, uint32_t i_width, uint32_t i_height ) ;
int32_t x265_simage_get_addr_offset_p4 ( x265_t *h, x265_simage_t *simage,
										uint32_t i_part_unit_idx, uint32_t i_width ) ;
int32_t x265_simage_get_addr_offset_p5 ( x265_t *h, x265_simage_t *simage,
										uint32_t i_trans_unit_idx, uint32_t i_blk_size, uint32_t i_width ) ;

spixel* x265_simage_get_luma_addr_p2 ( x265_t *h, x265_simage_t* simage ) ;
spixel* x265_simage_get_cb_addr_p2 ( x265_t *h, x265_simage_t* simage );
spixel* x265_simage_get_cr_addr_p2 ( x265_t *h, x265_simage_t* simage );

//  access starting position of simage partition unit buffer
spixel* x265_simage_get_luma_addr_p3 ( x265_t *h, x265_simage_t* simage, uint32_t i_part_unit_idx ) ;
spixel* x265_simage_get_cb_addr_p3 ( x265_t *h, x265_simage_t* simage, uint32_t i_part_unit_idx ) ;
spixel* x265_simage_get_cr_addr_p3 ( x265_t *h, x265_simage_t* simage, uint32_t i_part_unit_idx ) ;

//  access starting position of simage transform unit buffer
spixel* x265_simage_get_luma_addr_p4( x265_t *h, x265_simage_t* simage, uint32_t i_trans_unit_idx, uint32_t i_blk_size ) ;
spixel* x265_simage_get_cb_addr_p4 ( x265_t *h, x265_simage_t* simage, uint32_t i_trans_unit_idx, uint32_t i_blk_size ) ;
spixel* x265_simage_get_cr_addr_p4 ( x265_t *h, x265_simage_t* simage, uint32_t i_trans_unit_idx, uint32_t i_blk_size ) ;

//  get stride value of simage buffer
uint32_t x265_simage_get_stride( x265_simage_t* simage ) ;
uint32_t x265_simage_get_c_stride( x265_simage_t* simage ) ;

uint32_t x265_simage_get_width( x265_simage_t* simage ) ;
uint32_t x265_simage_get_height( x265_simage_t* simage ) ;
uint32_t x265_simage_get_c_width( x265_simage_t* simage ) ;
uint32_t x265_simage_get_c_height( x265_simage_t* simage ) ;


typedef struct _x265_frame_t x265_frame_t ;

void x265_simage_copy_to_frame ( x265_t *h, x265_simage_t *simage, x265_frame_t* frame_dst, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx ) ;
void x265_simage_copy_to_frame_luma  ( x265_t *h, x265_simage_t *simage, x265_frame_t* frame_dst, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx ) ;
void x265_simage_copy_to_frame_chroma( x265_t *h, x265_simage_t *simage, x265_frame_t* frame_dst, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx ) ;
void x265_simage_copy_from_frame   ( x265_t *h, x265_simage_t *simage, x265_frame_t* pc_frame_src, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx ) ;
void x265_simage_copy_from_frame_luma ( x265_t *h, x265_simage_t *simage, x265_frame_t* pc_frame_src, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx ) ;
void x265_simage_copy_from_frame_chroma( x265_t *h, x265_simage_t *simage, x265_frame_t* pc_frame_src, uint32_t i_cu_addr, uint32_t i_abs_zorder_idx ) ;

void x265_simage_copy_to_part_simage( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_dst, uint32_t i_dst_part_idx ) ;
void x265_simage_copy_to_part_luma( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_dst, uint32_t i_dst_part_idx ) ;
void x265_simage_copy_to_part_chroma( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_dst, uint32_t i_dst_part_idx ) ;

void x265_simage_copy_part_to_simage( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_dst, uint32_t i_src_part_idx ) ;
void x265_simage_copy_part_to_luma( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_dst, uint32_t i_src_part_idx ) ;
void x265_simage_copy_part_to_chroma( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_dst, uint32_t i_src_part_idx ) ;
void x265_simage_copy_part_to_part_simage_p ( x265_t *h, x265_simage_t *simage, x265_image_t* image_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height ) ;
void x265_simage_copy_part_to_part_luma_p ( x265_t *h, x265_simage_t *simage, x265_image_t* image_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height ) ;
void x265_simage_copy_part_to_part_chroma_p( x265_t *h, x265_simage_t *simage, x265_image_t* image_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height ) ;
void x265_simage_copy_part_to_part_simage_s ( x265_t *h, x265_simage_t *simage, x265_simage_t* image_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height ) ;
void x265_simage_copy_part_to_part_luma_s ( x265_t *h, x265_simage_t *simage, x265_simage_t* image_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height ) ;
void x265_simage_copy_part_to_part_chroma_s( x265_t *h, x265_simage_t *simage, x265_simage_t* image_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height ) ;
void x265_simage_copy_part_to_part_chroma_p7( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_dst, uint32_t i_part_idx, uint32_t i_width, uint32_t i_height, uint32_t chroma_id) ;


void x265_simage_add_clip( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_src0, x265_simage_t* simage_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size ) ;
void x265_simage_add_clip_luma( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_src0, x265_simage_t* simage_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size ) ;
void x265_simage_add_clip_chroma( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_src0, x265_simage_t* simage_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size ) ;
void x265_simage_subtract( x265_t *h, x265_simage_t *simage, x265_image_t* simage_src0, x265_image_t* simage_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size ) ;
void x265_simage_subtract_luma( x265_t *h, x265_simage_t *simage, x265_image_t* simage_src0, x265_image_t* simage_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size ) ;
void x265_simage_subtract_chroma( x265_t *h, x265_simage_t *simage, x265_image_t* simage_src0, x265_image_t* simage_src1, uint32_t i_tr_unit_idx, uint32_t i_part_size ) ;
void x265_simage_add_avg( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_src0, x265_simage_t* simage_src1, uint32_t i_part_unit_idx, uint32_t i_width, uint32_t i_height ) ;
void x265_simage_remove_high_freq( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_src, uint32_t i_part_idx, uint32_t i_widht, uint32_t i_height ) ;


void x265_simage_copy_up_from_simage ( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_src, uint32_t i_cu_addr_x, int32_t i_width ) ;
void x265_simage_copy_up_from_simage_luma ( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_src, uint32_t i_cu_addr_x, int32_t i_width ) ;
void x265_simage_copy_up_from_simage_chroma ( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_src, uint32_t i_cu_addr_x, int32_t i_width ) ;

void x265_simage_copy_left_from_simage ( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_src ) ;
void x265_simage_copy_left_from_simage_luma ( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_src ) ;
void x265_simage_copy_left_from_simage_chroma ( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_src ) ;

void x265_simage_copy_right_from_frame ( x265_t *h, x265_simage_t *simage, x265_frame_t* frame_src, uint32_t i_cu_addr ) ;
void x265_simage_copy_right_from_frame_luma ( x265_t *h, x265_simage_t *simage, x265_frame_t* frame_src, uint32_t i_cu_addr ) ;
void x265_simage_copy_right_from_frame_chroma ( x265_t *h, x265_simage_t *simage, x265_frame_t* frame_src, uint32_t i_cu_addr ) ;
void x265_simage_copy_bottom_from_frame ( x265_t *h, x265_simage_t *simage, x265_frame_t* frame_src, uint32_t i_cu_addr_x, uint32_t i_cu_addr ) ;
void x265_simage_copy_bottom_from_frame_luma ( x265_t *h, x265_simage_t *simage, x265_frame_t* frame_src, uint32_t i_cu_addr_x, uint32_t i_cu_addr ) ;
void x265_simage_copy_bottom_from_frame_chroma ( x265_t *h, x265_simage_t *simage, x265_frame_t* frame_src, uint32_t i_cu_addr_x, uint32_t i_cu_addr ) ;

void x265_simage_copy_to_simage ( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_dst, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx ) ;
void x265_simage_copy_to_simage_luma ( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_dst, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx ) ;
void x265_simage_copy_to_simage_chroma ( x265_t *h, x265_simage_t *simage, x265_simage_t* simage_dst, uint32_t i_abs_zorder_idx, uint32_t i_part_depth, uint32_t i_part_idx ) ;

#endif


