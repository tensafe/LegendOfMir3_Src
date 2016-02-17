

/*
	IndexMap

	Date:
		2001/02/05

	Note:
		순회(Traverse) 속도를 증가시키기 위해 내부적으로 List를 사용한다.
		물론, 인덱스를 사용하기 때문에 삽입/삭제시 좀더 많은 CPU Time과 메모리가 필요하다.
		리스트의 Key 정렬은 하지 않는다.
*/
#ifndef __ORZ_DATASTRUCTURE_INDEX_MAP__
#define __ORZ_DATASTRUCTURE_INDEX_MAP__


#include "list.h"
#include "map.h"


template< class T >
class CIndexMap : public CMap< T >
{
protected:
	CList< T > m_Index;

public:
	CIndexMap();
	virtual ~CIndexMap();

	virtual bool InitHashTable( int nDemandSize, int nFlags = IHT_UNTOUCH );
	virtual void UninitHashTable( bool bDeleteData = true, bool bDeleteArray = false );

	bool Insert( T *pData );
	T *  Remove( T *pKey );

	CListNode< T > * GetHead();
	CListNode< T > * GetNext( CListNode< T > *pNode );
};


template< class T >
CIndexMap< T >::CIndexMap()
{
}


template< class T >
CIndexMap< T >::~CIndexMap()
{
}


template< class T >
bool CIndexMap< T >::InitHashTable( int nDemandSize, int nFlags )
{
	m_Index.SetCompareFunction( __cbCmpString, this );

	return CMap< T >::InitHashTable( nDemandSize, nFlags );
}


template< class T >
void CIndexMap< T >::UninitHashTable( bool bDeleteData, bool bDeleteArray )
{
	m_Index.ClearAll( false );

	CMap< T >::UninitHashTable( bDeleteData, bDeleteArray );
}


template< class T >
bool CIndexMap< T >::Insert( T *pData )
{
	if ( CMap< T >::Insert( pData ) )
		return m_Index.Insert( pData );

	return false;
}


template< class T >
T * CIndexMap< T >::Remove( T *pKey )
{
	if ( CMap< T >::Remove( pKey ) )
		return m_Index.Remove( pKey );

	return NULL;
}


template< class T >
CListNode< T > * CIndexMap< T >::GetHead()
{
	return m_Index.GetHead();
}


template< class T >
CListNode< T > * CIndexMap< T >::GetNext( CListNode< T > *pNode )
{
	return m_Index.GetNext( pNode );
}


#endif
