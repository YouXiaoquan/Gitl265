

#ifndef X265_TMVP_DATA_CU_H
#define X265_TMVP_DATA_CU_H

typedef struct
{
	x265_mv_t* mv ;
	int8_t* ref_idx ;
	uint32_t i_num_partition ;

} x265_tmvp_cu_mv_field_t ;

typedef struct _x265_tmvp_data_cu_t x265_tmvp_data_cu_t ;

struct _x265_tmvp_data_cu_t
{
	x265_frame_t *frame ;
	x265_tmvp_cu_mv_field_t tmvp_cu_mv_field[2];     ///< array of motion vectors
	int8_t* part_size;         ///< array of partition sizes
	int8_t* pred_mode;         ///< array of prediction modes
	uint32_t i_num_partition;     ///< total number of minimum partitions in a CU
	int32_t i_unit_size;           ///< size of a "minimum partition"

};

int x265_tmvp_cu_mv_field_create ( x265_tmvp_cu_mv_field_t *tmvp_cu_mv_field, uint32_t i_num_partition ) ;
void x265_tmvp_cu_mv_field_destroy ( x265_tmvp_cu_mv_field_t *tmvp_cu_mv_field ) ;
void x265_tmvp_cu_mv_field_compress ( x265_tmvp_cu_mv_field_t *tmvp_cu_mv_field,
										int8_t * pred_mode, int32_t i_scale ) ;
x265_mv_t *x265_tmvp_cu_mv_field_get_mv ( x265_tmvp_cu_mv_field_t *tmvp_cu_mv_field, int32_t i_idx ) ;
int32_t x265_tmvp_cu_mv_field_get_ref_idx( x265_tmvp_cu_mv_field_t *tmvp_cu_mv_field, int32_t i_idx ) ;

x265_tmvp_data_cu_t *x265_tmvp_data_cu_new () ;
void x265_tmvp_data_cu_delete ( x265_tmvp_data_cu_t *tmvp_data_cu ) ;
int x265_tmvp_data_cu_init ( x265_tmvp_data_cu_t *tmvp_data_cu ) ;
void x265_tmvp_data_cu_deinit ( x265_tmvp_data_cu_t *tmvp_data_cu ) ;
int x265_tmvp_data_cu_create ( x265_tmvp_data_cu_t *tmvp_data_cu,
								uint32_t i_num_partition,
								int32_t i_width,
								int32_t i_height,
								int32_t i_unit_size ) ;
void x265_tmvp_data_cu_destroy ( x265_tmvp_data_cu_t *tmvp_data_cu ) ;
void x265_tmvp_data_cu_load ( x265_t *h,
							x265_tmvp_data_cu_t *tmvp_data_cu,
							x265_data_cu_t *data_cu ) ;
void x265_tmvp_cu_mv_field_copy_from_cu_mv_field( x265_tmvp_cu_mv_field_t *tmvp_cu_mv_field,
												x265_cu_mv_field_t *cu_mv_field_src ) ;
void x265_tmvp_data_cu_compress_mv ( x265_tmvp_data_cu_t *tmvp_data_cu ) ;

x265_tmvp_cu_mv_field_t *x265_tmvp_data_cu_get_tmvp_cu_mv_field( x265_tmvp_data_cu_t *tmvp_data_cu,
																enum ref_pic_list_e i_ref_pic_list ) ;

enum part_size_e x265_tmvp_data_cu_get_partition_size_p2 (x265_tmvp_data_cu_t *tmvp_data_cu,
															uint32_t i_idx ) ;
int32_t x265_tmvp_data_cu_is_intra ( x265_tmvp_data_cu_t *tmvp_data_cu,
									uint32_t i_part_idx ) ;


#endif



