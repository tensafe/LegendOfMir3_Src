

#ifndef __ORZ_STRING__
#define __ORZ_STRING__


#include <string.h>
#include <ctype.h>


/*
	string support class (binary string)
*/
class bstr
{
public:
	char *ptr;
	int  size;

public:
	bstr();
	bstr( bstr &str );
	bstr( char *str );
	virtual ~bstr();

	bool alloc ( int len );
	bool expand( int len );
	bool assign( char *str, int len );
	bool assign( char *str );
	void cleanup();

	bool isassign();
	int  length();

	operator char * ()				{ return ptr; }
	operator const char * const ()	{ return (const char *) ptr; }

	char * operator =  ( char *str );
	char * operator =  ( bstr &str );
	char * operator =  ( int  num  );
	bool   operator == ( char *str );
	bool   operator != ( char *str );
	bstr & operator += ( char *str );
	bstr & operator += ( int  num  );
	friend bstr operator + ( bstr &str1, bstr &str2 );
	friend bstr operator + ( bstr &str1, char *str2 );
	friend bstr operator + ( char *str1, bstr &str2 );
};


/*
	C runtime function plus++
*/
char * _memstr( char *buf, int buf_len, char *str );	
char * _memistr( char *buf, int buf_len, char *str );


/*
	string manipulation
*/
bool _isspace( char c );
int  _ltrim( char *str );
int  _rtrim( char *str );
int  _trim( char *str );
int  _linecopy( char *str, char *buf );
int  _linecopy( bstr *str, char *buf );
bool _pickstring( char *str, char sep, int index, char *buf, int buf_len );


#endif
