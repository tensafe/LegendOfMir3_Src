

/*
	Stream Buffer

	Date:
		2001/10/23

	Note:
		Queue와 비슷한 개념으로 노드 단위가 아닌 스트림으로 처리한다.
*/
#ifndef __ORZ_DATASTRUCTURE_STREAM_BUFFER__
#define __ORZ_DATASTRUCTURE_STREAM_BUFFER__


#include <stdio.h>


template< class T >
class CStreamBuffer
{
public:
	T * m_pBuf;
	int m_nBufLen;
	int m_nUseLen;
	
public:
	CStreamBuffer();
	~CStreamBuffer();

	void ClearAll();

	T *  Buffer()				{ return m_pBuf; }
	int  Length()				{ return m_nUseLen; }
	T	 operator[]( int nPos )	{ return m_pBuf[ nPos ]; }
	operator T * ()				{ return m_pBuf; }
	operator const T * () const	{ return (const T *) m_pBuf; }

	bool Expand( int nBufLen );
	bool Append( T *pBuf, int nBufLen );
	void Remove( int nBufLen );
};


template< class T >
CStreamBuffer< T >::CStreamBuffer()
{
	m_pBuf		= NULL;
	m_nBufLen	= 0;
	m_nUseLen	= 0;
}


template< class T >
CStreamBuffer< T >::~CStreamBuffer()
{
	ClearAll();
}


template< class T >
void CStreamBuffer< T >::ClearAll()
{
	if ( m_pBuf )
	{
		free( m_pBuf );
		m_pBuf = NULL;
		m_nBufLen = 0;
		m_nUseLen = 0;
	}
}


template< class T >
bool CStreamBuffer< T >::Expand( int nBufLen )
{
	m_pBuf = (T *) realloc( m_pBuf, sizeof( T ) * (m_nBufLen + nBufLen) );
	if ( !m_pBuf && (m_nBufLen + nBufLen) > 0 )
		return false;

	m_nBufLen += nBufLen;

	return true;
}


template< class T >
bool CStreamBuffer< T >::Append( T *pBuf, int nBufLen )
{	
	if ( m_nBufLen < m_nUseLen + nBufLen )
	{
		if ( Expand( m_nUseLen + nBufLen - m_nBufLen ) == false )
			return false;
	}

	memcpy( m_pBuf + m_nUseLen, pBuf, nBufLen );
	m_nUseLen += nBufLen;
	
	return true;
}


template< class T >
void CStreamBuffer< T >::Remove( int nBufLen )
{
	memmove( m_pBuf, m_pBuf + nBufLen, sizeof( T ) * (m_nUseLen - nBufLen) );

	Expand( -nBufLen );

	m_nUseLen -= nBufLen;
}


#endif