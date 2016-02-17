

/*
	Stack

	Date:
		2001/02/05 
*/
#ifndef __ORZ_DATASTRUCTURE_STACK__
#define __ORZ_DATASTRUCTURE_STACK__


#include "list.h"


template< class T >
class CStack : public CList< T >
{
public:
	virtual bool Push( T *pData );
	virtual bool PushHead( T *pData );
	virtual T *  Pop();
};


template< class T >
bool CStack< T >::Push( T * pData )
{
	return CList< T >::Insert( pData );
}


template< class T >
bool CStack< T >::PushHead( T * pData )
{
	return CList< T >::InsertHead( pData );
}


template< class T >
T * CStack< T >::Pop()
{
	if ( IsEmpty() )
		return NULL;

	return RemoveNode( m_pTail );
}


#endif