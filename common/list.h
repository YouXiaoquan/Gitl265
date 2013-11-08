

#ifndef X265_LIST_H
#define X265_LIST_H


typedef struct
{
	uint32_t i_length ;
	uint32_t i_max_size ;
	uint32_t *list ;

} x265_uint_list_t;



x265_uint_list_t * x265_uint_list_new () ;
void x265_uint_list_delete ( x265_uint_list_t *uint_list ) ;
void x265_uint_list_push_back ( x265_uint_list_t *uint_list, uint32_t i_element ) ;
void x265_uint_list_clear ( x265_uint_list_t *uint_list ) ;
int x265_uint_list_get_element ( x265_uint_list_t *uint_list, uint32_t i_index ) ;
uint32_t x265_uint_list_size ( x265_uint_list_t *uint_list ) ;




#endif


