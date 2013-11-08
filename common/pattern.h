


#ifndef X265_PATTERN_H
#define X265_PATTERN_H


typedef struct
{
	int32_t i_offset_left;
	int32_t i_offset_above;
	pixel* pattern_origin;

	int32_t i_roi_width;
	int32_t i_roi_height;
	int32_t i_pattern_stride;

} x265_pattern_param_t ;

typedef struct
{
	uint8_t intra_filter[5];

} x265_pattern_class_static_t ;

typedef struct
{
	x265_pattern_param_t  pattern_y ;
	x265_pattern_param_t  pattern_cb ;
	x265_pattern_param_t  pattern_cr ;

} x265_pattern_t ;


extern x265_pattern_class_static_t *p_pattern_class_static ;



x265_pattern_param_t *x265_pattern_param_new () ;
void x265_pattern_param_delete ( x265_pattern_param_t *pattern_param ) ;

int x265_pattern_param_init ( x265_pattern_param_t *pattern_param );
void x265_pattern_param_deinit ( x265_pattern_param_t *pattern_param );



/// return starting position of buffer
pixel *x265_pattern_param_get_pattern_origin ( x265_pattern_param_t *pattern_param ) ;

/// return starting position of ROI (ROI = &pattern[AboveOffset][LeftOffset])
pixel *x265_pattern_param_get_roi_origin ( x265_pattern_param_t *pattern_param ) ;


/// set parameters from Pel buffer for accessing neighbouring pixels
void x265_pattern_param_set_pattern_param_pel ( x265_pattern_param_t *pattern_param,
												pixel *p_texture,
												int32_t i_roi_width,
												int32_t i_roi_height,
												int32_t i_stride,
												int32_t i_offset_left,
												int32_t i_offset_above );

/// set parameters of one color component from CU data for accessing neighbouring pixels
void x265_pattern_param_set_pattern_param_cu ( x265_t *h,
												x265_pattern_param_t *pattern_param,
												x265_data_cu_t *cu,
												uint8_t i_comp,
												uint8_t i_roi_width,
												uint8_t i_roi_height,
												int32_t i_offset_left,
												int32_t i_offset_above,
												uint32_t i_abs_part_idx );

int x265_pattern_class_static_init () ;
void x265_pattern_class_static_deinit () ;



x265_pattern_t *x265_pattern_new () ;
void x265_pattern_delete ( x265_pattern_t *pattern ) ;



int x265_pattern_init ( x265_pattern_t *pattern );
void x265_pattern_deinit ( x265_pattern_t *pattern );


pixel *x265_pattern_get_roi_y ( x265_pattern_t *pattern ) ;
int32_t x265_pattern_get_roi_width ( x265_pattern_t *pattern ) ;
int32_t x265_pattern_get_roi_height ( x265_pattern_t *pattern ) ;
int32_t x265_pattern_get_pattern_l_stride ( x265_pattern_t *pattern ) ;


pixel *x265_pattern_get_adi_org_buf ( x265_pattern_t *pattern,
										int32_t i_cu_width,
										int32_t i_cu_height,
										pixel *adi_buf,
										int32_t i_stride ) ;
pixel *x265_pattern_get_adi_cb_buf ( x265_pattern_t *pattern,
										int32_t i_cu_width,
										int32_t i_cu_height,
										pixel *adi_buf,
										int32_t i_stride ) ;
pixel *x265_pattern_get_adi_cr_buf ( x265_pattern_t *pattern,
										int32_t i_cu_width,
										int32_t i_cu_height,
										pixel *adi_buf,
										int32_t i_stride ) ;

pixel* x265_pattern_get_predictor_ptr(x265_pattern_t *pattern,
										uint32_t i_dir_mode,
										uint32_t i_log2_blk_size,
										pixel *adi_buf,
										int32_t i_stride ) ;


void x265_pattern_init_pattern_p9 (x265_pattern_t *pattern,
								pixel *p_y,
								pixel *p_cb,
								pixel *p_cr,
								int32_t i_roi_width,
								int32_t i_roi_height,
								int32_t i_stride,
								int32_t i_offset_left,
								int32_t i_offset_above ) ;

void x265_pattern_init_pattern_p5(x265_t *h,
								x265_pattern_t *pattern,
								x265_data_cu_t *cu,
								uint32_t i_part_depth,
								uint32_t i_abs_part_idx ) ;

void x265_pattern_init_adi_pattern(x265_t *h,
									x265_pattern_t *pattern,
									x265_data_cu_t *cu,
									x265_image_t *p_image,
									uint32_t i_zorder_idx_in_part,
									uint32_t i_part_depth,
									pixel *adi_buf,
									int32_t i_org_buf_stride,
									int32_t i_org_buf_height,
									int32_t *p_above,
									int32_t *p_left,
									int32_t b_l_mmode ) ;

void x265_pattern_init_adi_pattern_chroma ( x265_t *h,
										x265_pattern_t *pattern,
										x265_data_cu_t *cu,
										x265_image_t *p_image,
										uint32_t i_zorder_idx_in_part,
										uint32_t i_part_depth,
										pixel *adi_buf,
										int32_t i_org_buf_stride,
										int32_t i_org_buf_height,
										int32_t *p_above,
										int32_t *p_left ) ;

void x265_pattern_fill_reference_samples(x265_pattern_t *pattern,
										int32_t bit_depth,
										pixel *p_roi_origin,
										pixel *p_adi_temp,
										int32_t i_stride,
										int32_t *p_neighbor_flags,
										int32_t i_num_intra_neighbor,
										int32_t i_unit_size,
										int32_t i_num_units_in_cu,
										int32_t i_total_units,
										uint32_t i_cu_width,
										uint32_t i_cu_height,
										uint32_t i_width,
										uint32_t i_height,
										int32_t i_pic_stride,
										int32_t b_l_mmode ) ;
int32_t x265_pattern_is_above_left_available(x265_t *h,
											x265_pattern_t *pattern,
											x265_data_cu_t *cu,
											uint32_t i_part_idx_lt ) ;
int32_t x265_pattern_is_above_available(x265_t *h,
										x265_pattern_t *pattern,
										x265_data_cu_t* cu,
										uint32_t i_part_idx_lt,
										uint32_t i_part_idx_rt,
										int32_t *p_valid_flags ) ;
int32_t x265_pattern_is_left_available(x265_t *h,
										x265_pattern_t *pattern,
										x265_data_cu_t *cu,
										uint32_t i_part_idx_lt,
										uint32_t i_part_idx_lb,
										int32_t *p_valid_flags ) ;
int32_t x265_pattern_is_above_right_available(x265_t *h,
												x265_pattern_t *pattern,
												x265_data_cu_t *cu,
												uint32_t i_part_idx_lt,
												uint32_t i_part_idx_rt,
												int32_t *p_valid_flags ) ;
int32_t x265_pattern_is_below_left_available(x265_t *h,
											x265_pattern_t *pattern,
											x265_data_cu_t *cu,
											uint32_t i_part_idx_lt,
											uint32_t i_part_idx_lb,
											int32_t *p_valid_flags ) ;






#endif

