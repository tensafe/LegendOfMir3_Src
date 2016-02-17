


/*
	List

	Date:
		2001/02/05 

	Note: 
		양방향 연결 리스트
*/
#ifndef __ORZ_DATASTRUCTURE_LIST__
#define __ORZ_DATASTRUCTURE_LIST__


template< class T >
class CListNode
{
public:
	T *m_pData;
	CListNode< T > *m_pPrev, *m_pNext;

public:
	CListNode();
	virtual ~CListNode();

	void Clear( bool bDeleteArray );

	T * GetData();
	CListNode< T > * GetPrev();
	CListNode< T > * GetNext();

	operator T * ()				{ return m_pData; }
	operator const T * const ()	{ return (const T *) m_pData; }
};


template< class T >
CListNode< T >::CListNode()
{
	m_pData = NULL;
	m_pPrev = m_pNext = NULL;
}


template< class T >
CListNode< T >::~CListNode()
{
}


template< class T >
void CListNode< T >::Clear( bool bDeleteArray )
{
	if ( bDeleteArray )
		delete[] m_pData;
	else
		delete m_pData;
}


template< class T >
T * CListNode< T >::GetData()
{
	return m_pData;
}


template< class T >
CListNode< T > * CListNode< T >::GetPrev()
{
	return m_pPrev;
}


template< class T >
CListNode< T > * CListNode< T >::GetNext()
{
	return m_pNext;
}



template< class T >
class CList
{
protected:
	CListNode< T > *m_pHead, *m_pTail;

	// Key를 비교할 때 호출되는 함수
	int  (*m_pfnCmp)( void *pArg, T *pFirst, T *pSecond );
	void *m_pArgCmpFunc;

	int  m_nCount;

public:
	CList();
	virtual ~CList();

	void ClearAll( bool bClearData = true, bool bDeleteArray = false );
	void SetCompareFunction( int (*pfnCmp)( void *pArg, T *pFirst, T *pSecond ), void *pArg );

	virtual bool Insert( T *pData );
	virtual bool InsertHead( T *pData );
	virtual bool InsertAt( CListNode< T > *pNode, T *pData );
	virtual T *  Remove( T *pKey );
	virtual T *  RemoveNode( CListNode< T > *pNode );
	virtual T *  Search( T *pKey );

	CListNode< T > * GetHead();
	CListNode< T > * GetTail();
	CListNode< T > * GetPrev( CListNode< T > *pNode );
	CListNode< T > * GetNext( CListNode< T > *pNode );

	int  GetCount();
	bool IsEmpty();
	
	CListNode< T > * operator[]( int nArray );
};


template< class T >
CList< T >::CList()
{
	m_pHead			= NULL;
	m_pTail			= NULL;
	m_pfnCmp		= NULL;
	m_pArgCmpFunc	= NULL;
	m_nCount		= 0;
}


template< class T >
CList< T >::~CList()
{
}


template< class T >
void CList< T >::ClearAll( bool bClearData, bool bDeleteArray )
{
	CListNode< T > *pTemp;

	while ( m_pHead )
	{
		if ( bClearData )
			m_pHead->Clear( bDeleteArray );

		pTemp	= m_pHead;
		m_pHead	= m_pHead->m_pNext;
		if ( m_pHead )
			m_pHead->m_pPrev = NULL;

		delete pTemp;
	}

	
	m_pHead			= NULL;
	m_pTail			= NULL;
	m_pfnCmp		= NULL;
	m_pArgCmpFunc	= NULL;
	m_nCount		= 0;
}


template< class T >
void CList< T >::SetCompareFunction( int (*pfnCmp)( void *pArg, T *pFirst, T *pSecond ), void *pArg )
{
	m_pfnCmp		= pfnCmp;
	m_pArgCmpFunc	= pArg;
}


template< class T >
bool CList< T >::Insert( T *pData )
{
	if ( m_pHead == NULL )
	{
		m_pHead = new CListNode< T >;
		if ( m_pHead == NULL )
			return false;

		m_pHead->m_pData = pData;
		m_pTail = m_pHead;
	}
	else
	{
		CListNode< T > *pNode = new CListNode< T >;
		if ( pNode == NULL )
			return false;

		pNode->m_pData   = pData;
		pNode->m_pNext   = NULL;
		pNode->m_pPrev   = m_pTail;
		m_pTail->m_pNext = pNode;

		m_pTail = pNode;
	}

	++m_nCount;

	return true;
}


template< class T >
bool CList< T >::InsertHead( T *pData )
{
	if ( m_pHead == NULL )
	{
		m_pHead = new CListNode< T >;
		if ( m_pHead == NULL )
			return false;

		m_pHead->m_pData = pData;
		m_pTail = m_pHead;
	}
	else
	{
		CListNode< T > *pNode = new CListNode< T >;
		if ( pNode == NULL )
			return false;

		pNode->m_pData = pData;
		pNode->m_pNext = m_pHead;

		m_pHead->m_pPrev = pNode;
		m_pHead = pNode;
	}

	++m_nCount;

	return true;
}


template< class T >
bool CList< T >::InsertAt( CListNode< T > *pNode, T *pData )
{
	CListNode< T > *pNew = new CListNode< T >;
	if ( pNew == NULL )
		return false;
	
	pNew->m_pData = pData;
	pNew->m_pPrev = pNode;
	pNew->m_pNext = pNode->m_pNext;

	if ( pNode->m_pNext )
		pNode->m_pNext->m_pPrev = pNew;
	pNode->m_pNext = pNew;

	if ( pNew->m_pNext == NULL )
		m_pTail = pNew;

	return true;
}


template< class T >
T * CList< T >::Remove( T *pKey )
{
	for ( CListNode< T > *pTemp = m_pHead; pTemp; pTemp = pTemp->m_pNext )
	{
		if ( m_pfnCmp( m_pArgCmpFunc, pKey, pTemp->m_pData ) == 0 )
		{
			pKey = pTemp->m_pData; 

			if ( m_pHead == pTemp )
			{
				m_pHead = m_pHead->m_pNext;

				if ( m_pHead )
					m_pHead->m_pPrev = NULL;
				else
					m_pTail = NULL;
			}
			else
			{
				if ( pTemp->m_pPrev )
					pTemp->m_pPrev->m_pNext = pTemp->m_pNext;

				if ( pTemp->m_pNext )
					pTemp->m_pNext->m_pPrev = pTemp->m_pPrev;
				else
					m_pTail = pTemp->m_pPrev;
			}

			delete pTemp;
			--m_nCount;

			return pKey;
		}
	}

	return NULL;
}


template< class T >
T * CList< T >::RemoveNode( CListNode< T > *pNode )
{
	T *pData = pNode->m_pData;

	if ( m_pHead == pNode )
	{
		m_pHead = pNode->m_pNext;
		if ( m_pHead )
			m_pHead->m_pPrev = NULL;
		else
			m_pTail = NULL;
	}
	else
	{
		if ( pNode->m_pPrev )
			pNode->m_pPrev->m_pNext = pNode->m_pNext;

		if ( pNode->m_pNext )
			pNode->m_pNext->m_pPrev = pNode->m_pPrev;
		else
			m_pTail = pNode->m_pPrev;
	}

	delete pNode;
	--m_nCount;

	return pData;
}


template< class T >
T * CList< T >::Search( T *pKey )
{
	for ( CListNode< T > *pTemp = m_pHead; pTemp; pTemp = pTemp->m_pNext )
	{
		if ( m_pfnCmp( m_pArgCmpFunc, pKey, pTemp->m_pData ) == 0 )
			return pTemp->m_pData; 
	}

	return NULL;
}


template< class T >
CListNode< T > * CList< T >::GetHead()
{
	return m_pHead;
}


template< class T >
CListNode< T > * CList< T >::GetTail()
{
	return m_pTail;
}


template< class T >
CListNode< T > * CList< T >::GetPrev( CListNode< T > *pNode )
{
	return pNode->GetPrev();
}


template< class T >
CListNode< T > * CList< T >::GetNext( CListNode< T > *pNode )
{
	return pNode->GetNext();
}


template< class T >
int CList< T >::GetCount()
{
	return m_nCount;
}


template< class T >
bool CList< T >::IsEmpty()
{
	return m_nCount == 0;
}


template< class T >
CListNode< T > * CList< T >::operator []( int nArray )
{
	for ( CListNode< T > *pTemp = m_pHead; pTemp; pTemp = pTemp->GetNext(), nArray-- )
	{
		if ( nArray == 0 )
			return pTemp;
	}
	
	return NULL;
}


#endif



