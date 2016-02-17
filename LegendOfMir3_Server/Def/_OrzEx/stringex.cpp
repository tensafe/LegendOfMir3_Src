

#include "stdafx.h"
#include "stringex.h"
#include "datatype.h"
#include "error.h"
#include <windows.h>
#include <malloc.h>


/*
	string support class
*/
bstr::bstr()
{
	ptr		= NULL;
	size	= 0;
};


bstr::bstr( bstr &str )
{	
	ptr		= NULL;
	size	= 0;

	if ( assign( str.ptr ) == false )
		throw CError( "bstr::bstr 메모리 할당 실패" );
}


bstr::bstr( char *str )
{
	ptr		= NULL;
	size	= 0;

	if ( assign( str ) == false )
		throw CError( "bstr::bstr 메모리 할당 실패" );
}


bstr::~bstr()
{
	cleanup();
}


bool bstr::alloc( int len )
{
	// cleanup previous assigned data
	cleanup();

	size = len;
	ptr = new char[ size ];
	
	return ptr ? true : false;
}


bool bstr::expand( int len )
{
	size += len;
	ptr = (char *) realloc( ptr, size );

	return ptr ? true : false;
}


bool bstr::assign( char *str, int len )
{
	// cleanup previous assigned data
	cleanup();

	if ( alloc( len + 1 ) == false )
		return false;

	memcpy( ptr, str, len );
	ptr[len] = NULL;

	return true;
}


bool bstr::assign( char *str )
{
	return assign( str, strlen( str ) );	
}


void bstr::cleanup()
{
	if ( ptr )
	{
		delete[] ptr;
		ptr = NULL;
	}
	
	size	= 0;
}


bool bstr::isassign()
{
	return ptr ? true : false;
}


int bstr::length()
{
	// include NULL space
	return size;
}


char * bstr::operator = ( char *str )
{
	return assign( str ) ? ptr : NULL;
}


char * bstr::operator = ( bstr &str )
{
	return assign( str ) ? ptr : NULL;
}


char * bstr::operator = ( int num )
{
	char temp[12];
	_itoa( num, temp, 10 );

	return assign( temp ) ? ptr : NULL;
}


bool bstr::operator == (char *str )
{
	return strcmp( ptr, str ) ? false : true;
}


bool bstr::operator != ( char *str )
{
	return strcmp( ptr, str ) ? true : false;
}


bstr & bstr::operator += ( char *str )
{
	if ( !size )
	{
		assign( str );
		return *this;
	}

	expand( strlen( str ) );
	strcat( ptr, str );

	return *this;
}



bstr & bstr::operator += ( int num )
{
	char temp[12]; // it's enough to store 32bit decimal either signed or unsigned
	_itoa( num, temp, 10 );
	
	return this->operator += ( temp );
}


bstr operator + ( bstr &str1, bstr &str2 )
{
	bstr temp( str1 );
	temp += str2;
	return temp;
}


bstr operator + ( bstr &str1, char *str2 )
{
	bstr temp( str1 );
	temp += str2;
	return temp;
}


bstr operator + ( char *str1, bstr &str2 )
{
	bstr temp( str1 );
	temp += str2;	
	return temp;
}




char * _memstr( char *buf, int buf_len, char *str )
{
	int str_len = strlen( str );
	int str_idx = 0;

	for ( int i = 0; i < buf_len; i++ )
	{
		if ( str_idx == str_len )
			return (char *)(buf + i - str_len);

		if ( buf[i] == str[str_idx++] )
			continue;

		str_idx = 0;
		if ( buf[i] == str[str_idx] )
			++str_idx;
	}

	if ( str_idx == str_len )
		return (char *)(buf + i - str_len);

	return NULL;
}


char * _memistr( char *buf, int buf_len, char *str )
{
	int str_len = strlen( str );
	int str_idx = 0;

	for ( int i = 0; i < buf_len; i++ )
	{
		if ( str_idx == str_len )
			return (char *)(buf + i - str_len);

		if ( toupper( (byte) buf[i] ) == toupper( (byte) str[str_idx++] ) )
			continue;

		str_idx = 0;
		if ( toupper( (byte) buf[i] ) == toupper( (byte) str[str_idx] ) )
			++str_idx;
	}

	if ( str_idx == str_len )
		return (char *)(buf + i - str_len);

	return NULL;
}


bool _isspace( char c )
{
	return (c == ' ' || c == '\t' || c == '\r' || c == '\n');
}


int _ltrim( char *str )
{
	int str_len = strlen( str );
	int cur_len = 0;

	while ( cur_len < str_len )
	{
		if ( _isspace( str[cur_len] ) )
		{
			cur_len++;
			continue;
		}
			
		strcpy( str, &str[cur_len] );
		break;
	}

	return cur_len;
}


int _rtrim( char *str )
{
	int str_len = 0;
	int cur_len = 0;

	while ( true )
	{
		str_len = strlen( str );

		if ( str_len && _isspace( str[str_len - 1] ) )
		{
			str[str_len - 1] = '\0';
			cur_len++;
		}
		else
		{
			break;
		}
	}

	return cur_len;
}


int _trim( char *str )
{
	return _ltrim( str ) + _rtrim( str );
}


int _linecopy( char *str, char *buf )
{
	char *next;
	int  line_len = 0;
	
	if ( (next = strstr( buf, "\r\n" )) || (next = strchr( buf, '\n' )) )
		line_len = next - buf;
	else
		line_len = strlen( buf );

	memcpy( str, buf, line_len );
	str[line_len] = '\0';
	
	return line_len;
}


int _linecopy( bstr *str, char *buf )
{
	char *next;
	int  line_len = 0;
	
	if ( (next = strstr( buf, "\r\n" )) || (next = strchr( buf, '\n' )) )
		line_len = next - buf;
	else
		line_len = strlen( buf );

	str->assign( buf, line_len );
	
	return line_len;
}


bool _pickstring( char *str, char sep, int index, char *buf, int buf_len )
{
	char *pos = str, *next;
	int  cur_index = 0;

	while ( cur_index <= index )
	{
		next = strchr( pos, sep );
		
		if ( cur_index == index )
		{
			if ( next )
			{
				if ( next - pos >= buf_len )
					return false;

				memcpy( buf, pos, next - pos );
				buf[next - pos] = 0;
				return true;
			}

			if ( strlen( pos ) >= (unsigned) buf_len )
				return false;

			strcpy( buf, pos );
			return true;
		}

		if ( !next )
			break;

		++cur_index;
		pos = next + sizeof( sep );
	}

	return false;
}
