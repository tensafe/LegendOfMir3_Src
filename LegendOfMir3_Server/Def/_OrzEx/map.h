

/*
	Map

	Date:
		2001/02/05 

	Note: 
		해쉬 테이블을 이용한 맵 클래스
		Division Hash 함수를 사용하며 각 버킷에 대해서 이진 탐색을 수행한다.
		Key는 문자열(char *)을 사용한다.

	Usage:
		InitHashTable로 필요한 메모리를 할당하고 UninitHashTable로 메모리를 해제한다.
		사용하기 전에 반드시 SetGetKeyFunction을 호출하여야 한다.
*/
#ifndef __ORZ_DATASTRUCTURE_MAP__
#define __ORZ_DATASTRUCTURE_MAP__



#include "bstree.h"
#include "prime.h"


// 해쉬 테이블을 초기화할 때 사용되는 플래그
#define IHT_UNTOUCH		1	// 지정된 크기를 그대로 사용한다.
#define IHT_ROUNDUP		2	// 지정된 크기와 가장 가까운 소수(큰값)를 사용한다.
#define IHT_ROUNDDOWN	3	// 지정된 크기와 가장 가까운 소수(작은값)를 사용한다.


/*
	CMap 클래스 정의

	Note: T의 포인터를 사용한다.
*/
template< class T >
class CMap
{
protected:
	CBsTree< T > *m_pHashTable;

	int  m_nDemandSize;
	int  m_nRealSize;
	int  m_nCount;

	char * (*m_pfnGetKey)( T *pData ); 

public:
	CMap();
	virtual ~CMap();

	virtual bool InitHashTable( int nDemandSize, int nFlags = IHT_UNTOUCH );
	virtual void UninitHashTable( bool bDeleteData = true, bool bDeleteArray = false );

	virtual void SetGetKeyFunction( char * (*pfnGetKey)( T * ) );

	virtual bool Insert( T *pData );
	virtual T *  Remove( T *pKey );
	virtual T *  Search( T *pKey );
	virtual T *  SearchKey( char *pKey );
	
	virtual int  GetCount();
	virtual bool IsEmpty();

	virtual int  GetHashKey( T *pData, bool bUseCallbackFunc = true );

protected:
	static int __cbCmpString( void *pArg, T *pFirst, T *pSecond );
	static int __cbCmpKeyString( void *pArg, T *pData, char *pKey );
};




/*
	구현
*/
template< class T >
CMap< T >::CMap()
{
	m_pHashTable	= NULL;
	m_nDemandSize	= 0;
	m_nRealSize		= 0;
	m_nCount		= 0;
	m_pfnGetKey		= NULL;
}


template< class T >
CMap< T >::~CMap()
{
}


template< class T >
bool CMap< T >::InitHashTable( int nDemandSize, int nFlags )
{
	m_nDemandSize = nDemandSize;
	
	// 실제 버킷 크기를 구한다.
	switch ( nFlags )
	{
	case IHT_UNTOUCH:
		m_nRealSize = nFlags;
		break;

	case IHT_ROUNDUP:
		m_nRealSize = CPrime::RoundUp( nDemandSize );
		break;

	case IHT_ROUNDDOWN:
		m_nRealSize = CPrime::RoundDown( nDemandSize );
		break;
	}

	// 노드 배열 할당
	m_pHashTable = new CBsTree< T >[ m_nRealSize ];
	if ( m_pHashTable )
	{
		for ( int i = 0; i < m_nRealSize; i++ )
		{
			m_pHashTable[i].SetCompareFunction( __cbCmpString, this );
			m_pHashTable[i].SetCompareStringFunction( __cbCmpKeyString, this );
		}
	}

	return m_pHashTable ? true : false;
}


template< class T >
void CMap< T >::UninitHashTable( bool bDeleteData, bool bDeleteArray )
{
	if ( m_pHashTable )
	{
		for ( int i = 0; i < m_nRealSize; i++ )
			m_pHashTable[i].ClearAll( bDeleteData, bDeleteArray );

		delete[] m_pHashTable;
		m_pHashTable = NULL;
	}
}



template< class T >
void CMap< T >::SetGetKeyFunction( char * (*pfnGetKey)( T * ) )
{
	m_pfnGetKey = pfnGetKey;
}


template< class T >
bool CMap< T >::Insert( T *pData )
{
	if ( m_pHashTable[ GetHashKey( pData ) ].Insert( pData ) )
	{
		++m_nCount;
		return true;
	}

	return false;
}



template< class T >
T * CMap< T >::Remove( T *pKey )
{
	T *pData = m_pHashTable[ GetHashKey( pKey ) ].Remove( pKey );
	if ( pData )
		--m_nCount;

	return pData;
}


template< class T >
T * CMap< T >::Search( T *pKey )
{
	return m_pHashTable[ GetHashKey( pKey ) ].Search( pKey );
}


template< class T >
T * CMap< T >::SearchKey( char *pKey )
{
	return m_pHashTable[ GetHashKey( (T *) pKey, false ) ].SearchKeyString( pKey );
}


template< class T >
int CMap< T >::GetCount()
{
	return m_nCount;
}


template< class T >
bool CMap< T >::IsEmpty()
{
	return m_nCount == 0;
}


template< class T >
int CMap< T >::GetHashKey( T *pData, bool bUseCallbackFunc )
{
	int  nKeyLen = 0;
	char *pKey = bUseCallbackFunc ? m_pfnGetKey( pData ) : (char *) pData;

	while ( *pKey )
		nKeyLen += (unsigned char) *pKey++;

	return nKeyLen % m_nRealSize;
}


template< class T >
int CMap< T >::__cbCmpString( void *pArg, T *pFirst, T *pSecond )
{
	return strcmp( ((CMap< T > *) pArg)->m_pfnGetKey( pFirst ), ((CMap< T > *) pArg)->m_pfnGetKey( pSecond ) );
}


template< class T >
int CMap< T >::__cbCmpKeyString( void *pArg, T *pData, char *pString )
{
	return strcmp( ((CMap< T > *) pArg)->m_pfnGetKey( pData ), pString );
}


#endif


