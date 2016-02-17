

/*
	Queue

	Date:
		2001/02/05 
*/
#ifndef __ORZ_DATASTRUCTURE_QUEUE__
#define __ORZ_DATASTRUCTURE_QUEUE__


#include "list.h"


template< class T >
class CQueue : public CList< T >
{
public:
	virtual bool Enqueue( T *pData );
	virtual bool EnqueueHead( T *pData );
	virtual T *  Dequeue();
};


template< class T >
bool CQueue< T >::Enqueue( T * pData )
{
	return CList< T >::Insert( pData );
}


template< class T >
bool CQueue< T >::EnqueueHead( T * pData )
{
	return CList< T >::InsertHead( pData );
}


template< class T >
T * CQueue< T >::Dequeue()
{
	if ( IsEmpty() )
		return NULL;
	else
		return RemoveNode( m_pHead );
}


#endif