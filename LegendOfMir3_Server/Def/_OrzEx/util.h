

/*
	Utility Functions

	Date:
		2001/02/15
*/
#ifndef __ORZ_MISC_UTIL__
#define __ORZ_MISC_UTIL__


/*
	Useful Macros
*/
#define _abs(a)		(((a) < (0)) ? (-a) : (b))
#define _swap(a, b)	((a) ^= (b) ^= (a) ^= (b))


/*
	Number System Converting Functions
*/
int  _hextodec( char *hex, int len );
void _dectohex( int dec, char *hex, int len );


/*
	Number Maniplulation Functions
*/
template< class TV, class TM > 
inline TV _rounddown( TV value, TM multiple )
{	
	return (value / multiple) * multiple;
}

template< class TV, class TM > 
inline TV _roundup( TV value, TM multiple )
{
	return _rounddown( value, multiple ) + ((value % multiple) > 0 ? multiple : 0);
}


#endif
