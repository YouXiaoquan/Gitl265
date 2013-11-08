


#ifndef X265_PIC_SYM_H
#define X265_PIC_SYM_H


typedef struct
{
	uint32_t i_tile_width;
	uint32_t i_tile_height;
	uint32_t i_right_edge_pos_in_cu;
	uint32_t i_bottom_edge_pos_in_cu;
	uint32_t i_first_cu_addr;
} x265_tile_t ;

void x265_pic_sym_offset_print ( x265_t *h ) ;

int x265_tile_init ( x265_tile_t *tile ) ;
void x265_tile_print ( x265_tile_t *tile, FILE *file ) ;

int x265_pic_sym_init ( x265_t *h, x265_param_t *param ) ;
void x265_pic_sym_deinit ( x265_t *h ) ;
void x265_pic_sym_print ( x265_t *h ) ;


void x265_pic_sym_load_prediction_data ( x265_t *h, uint32_t i_cu_addr ) ;
void x265_pic_sym_store_prediction_data ( x265_t *h, uint32_t i_cu_addr ) ;

x265_image_t *x265_pic_sym_get_image_intra ( x265_t *h ) ;

void x265_pic_sym_swap_cu_buffer ( x265_t *h ) ;

x265_slice_t*  x265_pic_sym_get_slice( x265_t *h, uint32_t i ) ;
uint32_t x265_pic_sym_get_frame_width_in_cu( x265_t *h ) ;
uint32_t x265_pic_sym_get_frame_height_in_cu( x265_t *h ) ;
uint32_t x265_pic_sym_get_min_cu_width( x265_t *h ) ;
uint32_t x265_pic_sym_get_min_cu_height( x265_t *h ) ;
uint32_t x265_pic_sym_get_number_of_cus_in_frame( x265_t *h ) ;

x265_base_data_cu_t* x265_pic_sym_get_curr_cu ( x265_t *h, uint32_t i_cu_addr) ;
x265_base_data_cu_t* x265_pic_sym_get_left_cu ( x265_t *h, uint32_t i_cu_addr) ;
x265_base_data_cu_t* x265_pic_sym_get_above_cu ( x265_t *h, uint32_t i_cu_addr) ;
x265_base_data_cu_t* x265_pic_sym_get_above_left_cu ( x265_t *h, uint32_t i_cu_addr) ;
x265_base_data_cu_t* x265_pic_sym_get_above_right_cu ( x265_t *h, uint32_t i_cu_addr) ;
x265_data_cu_t *x265_pic_sym_get_cu ( x265_t *h ) ;

uint32_t x265_pic_sym_get_num_partition( x265_t *h ) ;
uint32_t x265_pic_sym_get_num_part_in_width( x265_t *h ) ;
uint32_t x265_pic_sym_get_num_part_in_height( x265_t *h ) ;
void x265_pic_sym_set_num_columns_minus1( x265_t *h, int32_t i ) ;
int32_t x265_pic_sym_get_num_columns_minus1( x265_t *h ) ;
void x265_pic_sym_set_num_rows_minus1( x265_t *h, int32_t i ) ;
int32_t x265_pic_sym_get_num_rows_minus1( x265_t *h ) ;
int32_t x265_pic_sym_get_num_tiles( x265_t *h ) ;
void x265_pic_sym_set_cu_order_map ( x265_t *h, int32_t i_enc_cu_order, int32_t i_cu_addr ) ;
uint32_t x265_pic_sym_get_cu_order_map( x265_t *h, int32_t i_enc_cu_order ) ;
uint32_t x265_pic_sym_get_tile_idx_map( x265_t *h, int32_t i_cu_addr ) ;
void x265_pic_sym_set_inverse_cu_order_map( x265_t *h, int32_t i_cu_addr, int32_t i_enc_cu_order ) ;
uint32_t x265_pic_sym_get_inverse_cu_order_map( x265_t *h, int32_t i_cu_addr ) ;
uint32_t x265_pic_sym_get_pic_scu_enc_order( x265_t *h, uint32_t i_scu_addr ) ;
uint32_t x265_pic_sym_get_pic_scu_addr( x265_t *h, uint32_t i_scu_enc_order ) ;
int x265_pic_sym_x_create_tile_array ( x265_t *h ) ;
void x265_pic_sym_x_destroy_tile_array ( x265_t *h ) ;
void x265_pic_sym_x_init_tiles( x265_t *h ) ;
uint32_t x265_pic_sym_x_calculate_nxt_cu_addr ( x265_t *h, uint32_t i_curr_cu_addr ) ;

x265_sao_param_t *x265_pic_sym_get_sao_param( x265_t *h ) ;

int32_t x265_pic_sym_is_pic_left_edge ( x265_t *h, uint32_t i_cu_addr ) ;
int32_t x265_pic_sym_is_pic_right_edge ( x265_t *h, uint32_t i_cu_addr ) ;
int32_t x265_pic_sym_is_pic_top_edge ( x265_t *h, uint32_t i_cu_addr ) ;
int32_t x265_pic_sym_is_pic_bottom_edge ( x265_t *h, uint32_t i_cu_addr ) ;
int32_t x265_pic_sym_is_tile_left_edge ( x265_t *h, uint32_t i_cu_addr ) ;
int32_t x265_pic_sym_is_tile_right_edge ( x265_t *h, uint32_t i_cu_addr ) ;
int32_t x265_pic_sym_is_tile_top_edge ( x265_t *h, uint32_t i_cu_addr ) ;
int32_t x265_pic_sym_is_tile_bottom_edge ( x265_t *h, uint32_t i_cu_addr ) ;





#endif

