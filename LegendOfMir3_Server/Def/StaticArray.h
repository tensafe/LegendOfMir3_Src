

#pragma once





template< class T >
class CStaticArray
{
public:
	// 클래스 T는 아래 인터페이스를 구현하여야 한다.
	interface IArrayData
	{
		virtual bool IsEmpty() = 0;
	};

public:
	T *  m_pElements;
	int  m_nSize;
	int  m_nCursor;	

public:
	CStaticArray( int nSize );
	virtual ~CStaticArray();

	int  GetFreeKey();
	T &  operator []( int nIndex );
};


template< class T >
CStaticArray< T >::CStaticArray( int nSize )
{
	m_pElements	= new T[ nSize ];
	m_nSize		= nSize;
	m_nCursor	= 0;
}


template< class T >
CStaticArray< T >::~CStaticArray()
{
	if ( m_pElements )
		delete[] m_pElements;
}


template< class T >
int CStaticArray< T >::GetFreeKey()
{
	int nFind;

	for ( nFind = m_nCursor; nFind < m_nSize; nFind++ )
	{
		if ( m_pElements[nFind].IsEmpty() )
		{
			m_nCursor++;
			return nFind;
		}
	}

	for ( nFind = 0; nFind < m_nCursor; nFind++ )
	{
		if ( m_pElements[nFind].IsEmpty() )
		{
			m_nCursor++;
			return nFind;
		}
	}

	return -1;
}


template< class T >
T & CStaticArray< T >::operator []( int nIndex )
{
	return m_pElements[ nIndex ];
}