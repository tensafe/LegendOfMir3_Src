

/*
	Priority Queue

	Date:
		2002/02/25

	Note:
		힙을 이용한 우선순위 큐

	Usage:
		사용 전 호출 함수: SetCompareFunction
		사용 후 호출 함수: ClearAll
		리셋: ClearAll -> ResetVector
*/
#ifndef __ORZ_DATASTRUCTURE_PRIORITY_QUEUE__
#define __ORZ_DATASTRUCTURE_PRIORITY_QUEUE__


#include "vector.h"


template< class T >
class CPriorityQueue
{
protected:
	CVector< T >	m_vector;
	int				m_nCount;

	int  (*m_pfnCmp)( void *pArg, T *pFirst, T *pSecond );
	void *m_pArgCmpFunc;

public:
	CPriorityQueue( int nCapacity, int nIncrease );
	virtual ~CPriorityQueue();

	void ClearAll( bool bClearData = true, bool bDeleteArray = false );
	bool ResetVector();

	void SetCompareFunction( int (*pfnCmp)( void *pArg, T *pFirst, T *pSecond ), void *pArg );

	virtual bool Enqueue( T *pData );
	virtual T *  Dequeue();
	virtual T *  DequeueIndex( int nIndex );
	virtual int  GetCount();

	inline	T *& operator[]( int nIndex );

protected:
	virtual void UpHeap( int nCount, int nNode );
	virtual void DownHeap( int nCount, int nNode );
	virtual void Sort();
};


template< class T >
CPriorityQueue< T >::CPriorityQueue( int nCapacity, int nIncrease )
: m_vector( nCapacity, nIncrease ), m_nCount( 1 )
{
	m_vector[0] = NULL;
}


template< class T >
CPriorityQueue< T >::~CPriorityQueue()
{
}


template< class T >
void CPriorityQueue< T >::ClearAll( bool bClearData, bool bDeleteArray )
{
	m_vector.ClearAll( bClearData, bDeleteArray );
}


template< class T >
bool CPriorityQueue< T >::ResetVector()
{
	return m_vector.Create();
}


template< class T >
void CPriorityQueue< T >::SetCompareFunction( int (*pfnCmp)( void *pArg, T *pFirst, T *pSecond ), void *pArg )
{
	m_pfnCmp		= pfnCmp;
	m_pArgCmpFunc	= pArg;
}


template< class T >
bool CPriorityQueue< T >::Enqueue( T *pData )
{
	if ( m_vector.GetCapacity() <= m_nCount )
	{
		if ( m_vector.Expand() == false )
			return false;
	}

	m_vector[m_nCount] = pData;

	UpHeap( m_nCount, m_nCount++ );

	return true;
}


template< class T >
T * CPriorityQueue< T >::Dequeue()
{
	if ( GetCount() <= 0 )
		return NULL;

	T * pData	= m_vector[1];
	m_vector[1]	= m_vector[--m_nCount];

	DownHeap( m_nCount, 1 ); // 1 is root index

	m_vector[m_nCount] = NULL;

	return pData;
}


template< class T >
T * CPriorityQueue< T >::DequeueIndex( int nIndex )
{
	if ( GetCount() <= nIndex )
		return NULL;

	T *pData = m_vector[nIndex + 1];	
	memmove( &m_vector[nIndex + 1], &m_vector[nIndex + 2], sizeof( T * ) * (m_nCount-- - (nIndex + 1)) );

	Sort();

	m_vector[m_nCount] = NULL;

	return pData;
}


template< class T >
int CPriorityQueue< T >::GetCount()
{
	// ignore dummy node
	return m_nCount - 1;
}


template< class T >
T *& CPriorityQueue< T >::operator[]( int nIndex )
{
	// skip dummy node
	return m_vector[nIndex + 1];
}


template< class T >
void CPriorityQueue< T >::UpHeap( int nCount, int nNode )
{
	T * pData = m_vector[nNode];

	while ( nNode > 1 && // ignore dummy node
		    m_pfnCmp( m_pArgCmpFunc, m_vector[nNode >> 1], pData ) < 0 )
	{
		m_vector[nNode] = m_vector[nNode >> 1];
		nNode >>= 1;
	}

	m_vector[nNode] = pData;
}


template< class T >
void CPriorityQueue< T >::DownHeap( int nCount, int nNode )
{
	T * pData = m_vector[nNode];
	int nChild;

	while ( nNode <= nCount >> 1 )
	{
		nChild = nNode << 1; // left child

		if ( nChild < nCount && m_pfnCmp( m_pArgCmpFunc, m_vector[nChild], m_vector[nChild + 1] ) < 0 )
			nChild++; // right child

		if ( m_pfnCmp( m_pArgCmpFunc, pData, m_vector[nChild] ) >= 0 )
			break;

		m_vector[nNode] = m_vector[nChild];
		nNode = nChild;
	}

	m_vector[nNode] = pData;
}


template< class T >
void CPriorityQueue< T >::Sort()
{
	int nCount = m_nCount - 1;

	for ( int nNode = nCount >> 1; nNode >= 1; nNode-- )
		DownHeap( nCount, nNode );
}


#endif