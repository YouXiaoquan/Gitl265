


#ifndef X265_SAMPLE_ADAPTIVE_OFFSET_H
#define X265_SAMPLE_ADAPTIVE_OFFSET_H

#define X265_SAO_MAX_DEPTH 4
#define X265_SAO_BO_BITS 5
#define X265_LUMA_GROUP_NUM (1<<X265_SAO_BO_BITS)
#define X265_MAX_NUM_SAO_OFFSETS 4
#define X265_MAX_NUM_SAO_CLASS 33


typedef struct
{
	int32_t *offset_bo;
	int32_t *offset_bo_y;
	int32_t *offset_bo_cb;
	int32_t *offset_bo_cr;
	int32_t *offset_eo;
	int32_t offset_eo_y[X265_LUMA_GROUP_NUM];
	int32_t offset_eo_cb[X265_LUMA_GROUP_NUM];
	int32_t offset_eo_cr[X265_LUMA_GROUP_NUM];

	int32_t i_pic_width;
	int32_t i_pic_height;
	uint32_t i_max_cu_width;
	uint32_t i_max_cu_height;
	int32_t i_num_cu_in_width;
	int32_t i_num_cu_in_height;
	int32_t i_num_total_parts;
	uint32_t i_num_cu_in_width_mul_two;

	uint32_t i_sao_bit_increase_y;
	uint32_t i_sao_bit_increase_c;  //for chroma
	uint32_t i_qp;

	pixel *clip_table;
	pixel *clip_table_base;
	pixel *luma_table_bo;
	pixel *chroma_clip_table;
	pixel *chroma_clip_table_base;
	pixel *chroma_table_bo;
	int32_t *up_buff_1;
	int32_t *up_buff_2;
	int32_t *up_buff;
	int32_t *swap;
	int32_t b_use_nif;       //!< true for performing non-cross slice boundary ALF
	x265_frame_t* frame_tmp;    //!< temporary picture buffer pointer when non-across slice/tile boundary SAO is enabled

	pixel* tmp_u1;
	pixel* tmp_u2;
	pixel* tmp_l1;
	pixel* tmp_l2;

	pixel* tmp_u1_y;
	pixel* tmp_u2_y;
	pixel* tmp_l1_y;
	pixel* tmp_l2_y;

	pixel* tmp_u1_cb;
	pixel* tmp_u2_cb;
	pixel* tmp_l1_cb;
	pixel* tmp_l2_cb;

	pixel* tmp_u1_cr;
	pixel* tmp_u2_cr;
	pixel* tmp_l1_cr;
	pixel* tmp_l2_cr;


	int32_t i_max_num_offsets_per_pic;
	int32_t b_sao_lcu_boundary;
	int32_t b_sao_lcu_based_optimization;

} x265_sample_adaptive_offset_t ;

typedef struct
{
	uint32_t i_max_depth;
	int32_t num_cul_parts_level[5];
	uint32_t eo_table[9];
	int32_t num_class[MAX_NUM_SAO_TYPE];

} x265_sample_adaptive_offset_class_static_t ;

extern x265_sample_adaptive_offset_class_static_t *p_sample_adaptive_offset_class_static ;

int x265_sample_adaptive_offset_class_static_init () ;
void x265_sample_adaptive_offset_class_static_deinit () ;


void x265_sample_adaptive_offset_x_pcm_restoration(x265_sample_adaptive_offset_t *sample_adaptive_offset,
													x265_frame_t *frame) ;
void x265_sample_adaptive_offset_x_pcm_cu_restoration(x265_t *h,
													x265_sample_adaptive_offset_t *sample_adaptive_offset,
													x265_data_cu_t* cu,
													uint32_t i_abs_zorder_idx,
													uint32_t i_depth ) ;
void x265_sample_adaptive_offset_x_pcm_sample_restoration(x265_t *h,
														x265_sample_adaptive_offset_t *sample_adaptive_offset,
														x265_data_cu_t *cu,
														uint32_t i_abs_zorder_idx,
														uint32_t i_depth,
														enum text_type_e i_text_type ) ;


x265_sample_adaptive_offset_t *x265_sample_adaptive_offset_new () ;
void x265_sample_adaptive_offset_delete ( x265_sample_adaptive_offset_t *sample_adaptive_offset ) ;
int x265_sample_adaptive_offset_init ( x265_sample_adaptive_offset_t *sample_adaptive_offset ) ;
void x265_sample_adaptive_offset_deinit ( x265_sample_adaptive_offset_t *sample_adaptive_offset ) ;
int x265_sample_adaptive_offset_create ( x265_t *h, x265_sample_adaptive_offset_t *sample_adaptive_offset ) ;
void x265_sample_adaptive_offset_destroy ( x265_t *h, x265_sample_adaptive_offset_t *sample_adaptive_offset ) ;

int x265_sample_adaptive_offset_alloc_sao_param ( x265_t *h,
													x265_sample_adaptive_offset_t *sample_adaptive_offset,
													x265_sao_param_t *sao_param ) ;
void x265_sample_adaptive_offset_reset_sao_param(x265_sample_adaptive_offset_t *sample_adaptive_offset,
												x265_sao_param_t *sao_param) ;
void x265_sample_adaptive_offset_free_sao_param ( x265_sample_adaptive_offset_t *sample_adaptive_offset,
												x265_sao_param_t *sao_param ) ;

pixel* x265_sample_adaptive_offset_get_frame_addr(x265_t *h,
													x265_sample_adaptive_offset_t *sample_adaptive_offset,
													x265_frame_t *frame,
													int32_t i_y_cb_cr,
													int32_t i_cu_addr) ;
void x265_sample_adaptive_offset_process_sao_cu_org(x265_t *h,
													x265_sample_adaptive_offset_t *sample_adaptive_offset,
													uint32_t i_cu_addr,
													int32_t i_sao_type,
													int32_t i_y_cb_cr) ;
void x265_sample_adaptive_offset_reset_lcu_part(x265_sample_adaptive_offset_t *sample_adaptive_offset,
												x265_sao_lcu_param_t *sao_lcu_param) ;
void x265_sample_adaptive_offset_process_cu(x265_t *h,
											x265_sample_adaptive_offset_t *sample_adaptive_offset,
											x265_sao_lcu_param_t *sao_lcu_param,
											uint32_t i_cu_addr,
											int32_t b_one_unit_flag,
											int32_t y_cb_cr) ;

void x265_sample_adaptive_offset_reset_lcu_part(x265_sample_adaptive_offset_t *sample_adaptive_offset,
												x265_sao_lcu_param_t *sao_lcu_param) ;

void x265_sample_adaptive_offset_set_sao_lcu_boundary(x265_sample_adaptive_offset_t *sample_adaptive_offset,
														int32_t b_val) ;
int32_t x265_sample_adaptive_offset_get_sao_lcu_boundary(x265_sample_adaptive_offset_t *sample_adaptive_offset) ;
void x265_sample_adaptive_offset_set_sao_lcu_based_optimization(x265_sample_adaptive_offset_t *sample_adaptive_offset,
																int32_t b_val) ;
int32_t x265_sample_adaptive_offset_get_sao_lcu_based_optimization(x265_sample_adaptive_offset_t *sample_adaptive_offset) ;


void x265_sample_adaptive_offset_reset_sao_unit(x265_sample_adaptive_offset_t *sample_adaptive_offset,
												x265_sao_lcu_param_t *sao_unit) ;
void x265_sample_adaptive_offset_copy_sao_unit(x265_sample_adaptive_offset_t *sample_adaptive_offset,
												x265_sao_lcu_param_t *sao_unit_dst,
												x265_sao_lcu_param_t *sao_unit_src ) ;
void x265_sample_adaptive_offset_pcm_lf_disable_process(x265_sample_adaptive_offset_t *sample_adaptive_offset,
														x265_frame_t *frame) ;

#endif

