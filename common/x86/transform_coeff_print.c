



void print_partial_butterfly_inverse_32_coeff ()
{
	int32_t i = 0 ;
	int32_t j = 0 ;
	int32_t k = 0 ;

#define print_one_partial_butterfly_inverse_32_coeff(a,b) \
	for ( i = 0 ; i < 2 ; ++ i ) \
	{ \
		fprintf ( stderr, "\t\t\t\t\t\t\t\t\tDW\t\t\t") ; \
		for ( j = 0 ; j < 3 ; ++ j ) \
		{ \
			fprintf ( stderr, "%3d, %3d, ", t8[a][k], t8[b][k] ) ; \
		} \
		fprintf ( stderr, "%3d, %3d\n", t8[a][k], t8[b][k] ) ; \
	} \


#define print_one_line_partial_butterfly_inverse_32_coeff(a,b) \
	for ( k = 0 ; k < 8 ; ++ k ) \
	{ \
		print_one_partial_butterfly_inverse_32_coeff(a, b) ; \
	} \


	print_one_line_partial_butterfly_inverse_32_coeff( 0,  4) ;
	print_one_line_partial_butterfly_inverse_32_coeff( 1,  7) ;
	print_one_line_partial_butterfly_inverse_32_coeff( 2,  6) ;
	print_one_line_partial_butterfly_inverse_32_coeff( 3,  5) ;
//	print_one_line_partial_butterfly_inverse_32_coeff( 8, 24) ;
//	print_one_line_partial_butterfly_inverse_32_coeff( 9, 23) ;
//	print_one_line_partial_butterfly_inverse_32_coeff(10, 22) ;
//	print_one_line_partial_butterfly_inverse_32_coeff(11, 21) ;
//	print_one_line_partial_butterfly_inverse_32_coeff(12, 20) ;
//	print_one_line_partial_butterfly_inverse_32_coeff(13, 19) ;
//	print_one_line_partial_butterfly_inverse_32_coeff(14, 18) ;
//	print_one_line_partial_butterfly_inverse_32_coeff(15, 17) ;



	exit(0) ;

}




void print_ont_line_t32( int32_t i_line_number )
{
	int32_t loop = 0 ;

	for ( loop = 0 ; loop < 31 ; ++ loop )
	{
		fprintf ( stderr, "%3d, ", t32[i_line_number][loop] ) ;
	}
	fprintf ( stderr, "%3d\n", t32[i_line_number][31] ) ;
}


void print_two_line_t32( int32_t i_line_number1, int32_t i_line_number2 )
{
	print_ont_line_t32(i_line_number1) ;
	print_ont_line_t32(i_line_number2) ;

	fprintf ( stderr, "\n" ) ;
}


void print_one_line_t32(int32_t k, int32_t ii, int32_t jj )
{
	int32_t i = 0 ;
	int32_t j = 0 ;

	for ( i = 0 ; i < 1 ; ++ i )
	{
		fprintf ( stderr, "\t\t\t\t\t\t\t\t\tDW\t\t\t" ) ;
		for ( j = 0 ; j < 3 ; ++ j )
		{
			fprintf ( stderr, "%3d, %3d, ", t8[k][ii], t8[k][jj] ) ;
		}
		fprintf ( stderr, "%3d, %3d\n", t8[k][ii], t8[k][jj] ) ;
	}
}

void print_8_1_t32(int k)
{
	for ( int loop = 0 ; loop < 2 ; ++ loop )
	{
		print_one_line_t32(k ,0,  1) ;
	}
}

void print_8_2_t32(int k)
{
	for ( int loop = 0 ; loop < 2 ; ++ loop )
	{
		print_one_line_t32(k ,0,  1) ;
	}
}

void print_4_t32(int k)
{
	for ( int loop = 0 ; loop < 1 ; ++ loop )
	{
		print_one_line_t32(k ,0,  3) ;
		print_one_line_t32(k ,1,  2) ;
	}
}

void print_2_t32(int k)
{
	print_one_line_t32(k ,0,  7) ;
	print_one_line_t32(k ,1,  6) ;
	print_one_line_t32(k ,2,  5) ;
	print_one_line_t32(k ,3,  4) ;
}

void print_all_t32()
{
	/*
	for ( int i = 0 ; i < 8 ; ++ i )
	{
		fprintf ( stderr, "\t\t\t\t\t\t\t\t\tDW\t\t\t" ) ;
		for ( int j = 0 ; j < 7 ; ++ j )
		{
			fprintf ( stderr, "%3d, ", t8[i][j] ) ;
		}
		fprintf ( stderr, "%3d\n", t8[i][7] ) ;
	}
	print_8_1_t32(0) ;
	print_2_t32(1) ;
	print_4_t32(2) ;
	print_2_t32(3) ;
	print_8_2_t32(4) ;
	print_2_t32(5) ;
	print_4_t32(6) ;
	print_2_t32(7) ;
	print_8_1_t32(8) ;
	print_2_t32(9) ;
	print_4_t32(10) ;
	print_2_t32(11) ;
	print_8_2_t32(12) ;
	print_2_t32(13) ;
	print_4_t32(14) ;
	print_2_t32(15) ;

	print_2_t32(13) ;
	print_4_t32(14) ;
	print_2_t32(15) ;
	print_8_1_t32(16) ;
	print_2_t32(17) ;
	print_4_t32(18) ;
	print_2_t32(19) ;
	print_8_2_t32(20) ;
	print_2_t32(21) ;
	print_4_t32(22) ;
	print_2_t32(23) ;
	print_8_1_t32(24) ;
	print_2_t32(25) ;
	print_4_t32(26) ;
	print_2_t32(27) ;
	print_8_2_t32(28) ;
	print_2_t32(29) ;
	print_4_t32(30) ;
	print_2_t32(31) ;
	*/

	for ( int i = 0 ; i < 8 ; ++ i )
	{
		for ( int j = 0 ; j < 4 ; ++ j )
		{
			fprintf ( stderr, "\t\t\t\t\t\t\t\t\tDW\t\t\t" ) ;
			for ( int loop = 0 ; loop < 3 ; ++ loop )
			{
				fprintf ( stderr, "%3d, %3d, ", t8[i][2*j], t8[i][2*j+1] ) ;
			}
			fprintf ( stderr, "%3d, %3d\n", t8[i][2*j], t8[i][2*j+1] ) ;
		}
	}


	exit (0) ;
}


