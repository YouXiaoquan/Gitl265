

#include "common.h"


void x265_uint_list_delete ( x265_uint_list_t *uint_list )
{
	if ( uint_list )
	{
		if ( uint_list->list )
		{
			x265_free ( uint_list->list ) ;
		}
		x265_free ( uint_list ) ;
	}
}

uint32_t x265_uint_list_reallocate ( x265_uint_list_t *uint_list )
{
	uint32_t i_max_size = 0 ;
	uint32_t *list = NULL ;

	if ( 0 == uint_list->i_max_size )
	{
		i_max_size = 1 ;
	}
	else
	{
		i_max_size = uint_list->i_max_size * 2 ;
	}
    CHECKED_MALLOCZERO( list, i_max_size * sizeof(uint32_t) );
    if ( uint_list->i_length > 0 )
    {
    	memcpy ( list, uint_list->list, uint_list->i_length * sizeof(uint32_t) ) ;
    }
    x265_free ( uint_list->list ) ;
    uint_list->list = list ;
    uint_list->i_max_size = i_max_size ;

    return 0 ;
fail:
	x265_free ( list ) ;
	return -1 ;
}


x265_uint_list_t * x265_uint_list_new ()
{
	x265_uint_list_t *uint_list = NULL ;

    CHECKED_MALLOCZERO( uint_list, sizeof(x265_uint_list_t) );
    return uint_list ;

fail:
	return 0 ;
}

void x265_uint_list_push_back ( x265_uint_list_t *uint_list, uint32_t i_element )
{
	if ( uint_list->i_length == uint_list->i_max_size )
	{
		if ( ! x265_uint_list_reallocate ( uint_list ) )
		{
			uint_list->list[uint_list->i_length] = i_element ;
			uint_list->i_length ++ ;
		}
	}
	else
	{
		uint_list->list[uint_list->i_length] = i_element ;
		uint_list->i_length ++ ;
	}
}

void x265_uint_list_clear ( x265_uint_list_t *uint_list )
{
	if ( uint_list->list )
	{
		x265_free ( uint_list->list ) ;
	}
	uint_list->i_length = 0 ;
	uint_list->i_max_size = 0 ;
	uint_list->list = NULL ;
}

int x265_uint_list_get_element ( x265_uint_list_t *uint_list, uint32_t i_index )
{
	return uint_list->list[i_index] ;
}

uint32_t x265_uint_list_size ( x265_uint_list_t *uint_list )
{
	return uint_list->i_length ;
}
