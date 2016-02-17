

/*
	Prime Class

	Date:
		2001/02/05 (Last Updated: 2002/03/06)

	Note:
		소수(Prime Number)와 관련된 함수 집합
*/
#ifndef __ORZ_ALGORITHM_PRIME__
#define __ORZ_ALGORITHM_PRIME__


class CPrime
{
public:
	static bool IsPrime( int nNumber );
	static int  RoundUp( int nNumber );
	static int  RoundDown( int nNumber );
};


#endif