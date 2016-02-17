

#include "stdafx.h"
#include "util.h"


int _hextodec( char *hex, int len )
{
	int dec  = 0;
	int base = 1;
	int index;

	for ( int i = len - 1; i >= 0; i-- )
	{
		if ( hex[i] >= '0' && hex[i] <= '9' )
			index = hex[i] - '0';
		else
			index = hex[i] - 'A' + 10;

		dec += index * base;
		base *= 16;
	}

	return dec;
}


// 16진수 정수를 16진수 문자로 변환시킨다.
static char _hexdectochar( char num )
{
	if ( num < 10 )
		return num + '0';
	
	return (num - 10) + 'A';
}


void _dectohex( int dec, char *hex, int len )
{
	int index = 0;

	while ( index < len )
	{
		hex[index++] = _hexdectochar( dec % 16 );
		dec = dec / 16;

		if ( dec == 1 )
		{
			if ( index < len )
				hex[index++] = _hexdectochar( dec );
			break;
		}
		else if ( dec == 0 )
		{
			if ( index < len )
				hex[index++] = '0';
			break;
		}
	}

	// 거꾸로 저장된 16진수 문자열을 뒤집는다.
	for ( int i = 0; i < index / 2; i++ )
		_swap( hex[i], hex[index - i - 1] );
}
