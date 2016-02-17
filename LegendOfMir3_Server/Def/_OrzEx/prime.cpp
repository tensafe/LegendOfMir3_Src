

#include "stdafx.h"
#include "prime.h"
#include <math.h>



bool CPrime::IsPrime( int nNumber )
{
	int nSqr = (int) sqrt( nNumber );

	for ( int i = 2; i < nSqr; i++ )
	{
		if ( nNumber % i == 0 )
			return false;
	}

	return true;
}


int CPrime::RoundUp( int nNumber )
{
	while ( IsPrime( nNumber ) == false )
		++nNumber;

	return nNumber;
}


int CPrime::RoundDown( int nNumber )
{
	while ( IsPrime( nNumber ) == false )
		--nNumber;

	return nNumber;
}