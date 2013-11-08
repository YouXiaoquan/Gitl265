


#ifndef X265_FRAME_H
#define X265_FRAME_H

typedef struct
{

} x265_fenc_data_t ;

typedef struct
{
	int32_t b_is_border_extended ;
    int32_t b_is_long_term ;
    int32_t b_used_by_curr ;
	int32_t b_temporal_layer_non_reference_flag ;
    int32_t i_t_layer ;
    int32_t b_reference; /* number of threads using this frame (not necessarily the number of pointers) */
    int32_t *is_used_as_long_term[2];
    int32_t *ref_poc[2];
    x265_tmvp_data_cu_t **tmvp_data_cu ;
} x265_fdec_data_t ;

struct _x265_frame_t
{
    /* */
    int i_poc;
    int64_t i_pts;
    int64_t i_dts;
    uint8_t b_fdec;
    int     i_plane;
    int     i_stride[3];
    int     i_width[3];
    int     i_lines[3];
    pixel *plane[3];
    pixel *buffer[4];

    x265_fenc_data_t *fenc_data ;
    x265_fdec_data_t *fdec_data ;

} ;

void x265_frame_write ( x265_t *h, x265_frame_t *frame ) ;
void x265_frame_compress_motion ( x265_t *h, x265_frame_t *frame ) ;

void x265_frame_expand_border( x265_t *h, x265_frame_t *frame ) ;
int           x265_frame_copy_picture( x265_t *h, x265_frame_t *dst, x265_picture_t *src );
void x265_frame_push_unused( x265_t *h, x265_frame_t *frame ) ;
x265_frame_t *x265_frame_pop_unused( x265_t *h, int b_fdec );
void x265_frame_buffering ( x265_t *h, x265_frame_t *frame ) ;
x265_frame_t * x265_frame_buffering_pop ( x265_t *h ) ;
int x265_frame_list_size ( x265_frame_t **list ) ;
x265_frame_t *x265_frame_get_from_poc ( x265_frame_t **list, int32_t i_poc ) ;
x265_frame_t *x265_frame_get_long_term_from_poc ( x265_frame_t **list, int32_t i_poc, int32_t i_bits_for_poc ) ;
x265_frame_t *x265_frame_pop_from_poc ( x265_frame_t **list, int32_t i_poc ) ;
x265_frame_t *x265_frame_pop_from_position ( x265_frame_t **list, int32_t i_position ) ;
void x265_frame_delete_list( x265_t *h, x265_frame_t **list ) ;

int32_t x265_frame_get_width ( x265_frame_t *frame )  ;
int32_t x265_frame_get_height ( x265_frame_t *frame )  ;

int32_t x265_frame_get_stride ( x265_frame_t *frame ) ;
int32_t x265_frame_get_c_stride ( x265_frame_t *frame )  ;

int32_t x265_frame_get_luma_margin ( x265_t *h, x265_frame_t *frame ) ;
int32_t x265_frame_get_chroma_margin ( x265_t *h, x265_frame_t *frame ) ;

// ------------------------------------------------------------------------------------------------
//  access function for picture buffer
// ------------------------------------------------------------------------------------------------

//  access starting position of picture buffer with margin
pixel* x265_frame_get_buf_y ( x265_frame_t *frame ) ;
pixel* x265_frame_get_buf_u ( x265_frame_t *frame ) ;
pixel* x265_frame_get_buf_v ( x265_frame_t *frame ) ;

pixel* x265_frame_get_luma_addr_p2 ( x265_t *h, x265_frame_t *frame ) ;
pixel* x265_frame_get_cb_addr_p2 ( x265_t *h, x265_frame_t *frame ) ;
pixel* x265_frame_get_cr_addr_p2 ( x265_t *h, x265_frame_t *frame ) ;

//  access starting position of original picture for specific coding unit (cu) or partition unit (pu)
pixel* x265_frame_get_luma_addr_p3 ( x265_t *h, x265_frame_t *frame, int32_t i_cu_addr ) ;
pixel* x265_frame_get_cb_addr_p3 ( x265_t *h, x265_frame_t *frame, int32_t i_cu_addr ) ;
pixel* x265_frame_get_cr_addr_p3 ( x265_t *h,  x265_frame_t *frame, int32_t i_cu_addr ) ;

pixel* x265_frame_get_luma_addr_p4 ( x265_t *h, x265_frame_t *frame, int32_t i_cu_addr, int32_t ui_abs_zorder_idx ) ;
pixel* x265_frame_get_cb_addr_p4 ( x265_t *h,  x265_frame_t *frame, int32_t i_cu_addr, int32_t ui_abs_zorder_idx ) ;
pixel* x265_frame_get_cr_addr_p4 ( x265_t *h,  x265_frame_t *frame, int32_t i_cu_addr, int32_t ui_abs_zorder_idx ) ;


void x265_frame_copy_to_frame ( x265_t *h, x265_frame_t *frame, x265_frame_t *frame_dst ) ;
void x265_frame_copy_to_frame_luma ( x265_t *h, x265_frame_t *frame, x265_frame_t *frame_dst ) ;
void x265_frame_copy_to_frame_cb ( x265_t *h, x265_frame_t *frame, x265_frame_t *frame_dst ) ;
void x265_frame_copy_to_frame_cr ( x265_t *h, x265_frame_t *frame, x265_frame_t *frame_dst ) ;



void x265_frame_dump ( x265_t* h, x265_frame_t *frame, const char *file_name, int32_t b_add ) ;

#endif


