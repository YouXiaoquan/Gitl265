


#include "common/common.h"
#include "timer_test.h"

int32_t test_instruction_timer_one_time()
{
	int64_t i_start_time = 0 ;
	int64_t i_end_time = 0 ;
	int32_t i_time_spend = 0 ;

	x265_test_instruction_timer(&i_start_time, &i_end_time) ;
	i_time_spend = ((int32_t)(i_end_time - i_start_time)) ;

	return i_time_spend ;
}

void test_instruction_timer()
{
	int32_t i_total_time_spend = 0 ;
	int32_t i_time_spend = 0 ;
	int32_t loop = 0 ;

	for ( loop = 0 ; loop < 256 ; ++ loop )
	{
		i_time_spend = test_instruction_timer_one_time() ;
		fprintf ( stderr, "%d\n", i_time_spend ) ;
		i_total_time_spend += i_time_spend ;
	}

	i_total_time_spend = (i_total_time_spend + 128) >> 7 ;
	fprintf ( stderr, "%d\n", i_total_time_spend ) ;
}

