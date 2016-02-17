
// ORZ: 중복 include 방지
#pragma once

typedef VOID *	PLISTNODE;

template <class T> class CWHList
{
public:
	typedef struct tagLINKEDLIST
	{
		T						lpData;
		struct tagLINKEDLIST	*prev;
		struct tagLINKEDLIST	*next;
	} LINKEDLIST, FAR *LPLINKEDLIST;

	LPLINKEDLIST		m_lpHead;
	LPLINKEDLIST		m_lpTail;

	UINT				m_nCount;

//	CRITICAL_SECTION	m_cs;

public:
	CWHList();
	~CWHList();

	BOOL			AddNewNode(T lpData);
	void			Clear();
	PLISTNODE		RemoveNode(LPLINKEDLIST lpListNode);
	BOOL			RemoveNodeByKey(SOCKET s);
	PLISTNODE		RemoveNodeByData(T lpData);
	void			*FindNode(LPLINKEDLIST lpList);
	PLISTNODE		FindData(T t);

	__inline PLISTNODE		GetHead()	
	{ 
		if (m_lpHead) 
			return (PLISTNODE)m_lpHead; 
	
		return NULL; 
	}
	
	__inline PLISTNODE		GetNext(PLISTNODE pListNode) { if (pListNode) return ((LPLINKEDLIST)pListNode)->next; return NULL; }
	__inline PLISTNODE		RemoveNode(PLISTNODE pListNode) { if (pListNode) return RemoveNode((LPLINKEDLIST)pListNode); return NULL; }
	__inline UINT			GetCount()	{ return m_nCount; }
	__inline T				GetData(PLISTNODE pListNode) { if (pListNode) return ((LPLINKEDLIST)pListNode)->lpData; return NULL; }
};

template<class T> CWHList<T>::CWHList()
{				  
	m_lpHead = NULL;
	m_lpTail = NULL;

	m_nCount = 0;

//	InitializeCriticalSection(&m_cs);
}

template<class T> CWHList<T>::~CWHList()
{
	Clear();
  
//	DeleteCriticalSection(&m_cs);
}

template<class T> BOOL CWHList<T>::AddNewNode(T lpData)
{
	BOOL fRet = FALSE;

//	EnterCriticalSection(&m_cs);

	LPLINKEDLIST lpCD = (LPLINKEDLIST)GlobalAlloc(GPTR, sizeof(LINKEDLIST));

	lpCD->lpData = lpData;

	if (!m_lpHead)
	{
		m_lpHead = lpCD;
		lpCD->prev = (LPLINKEDLIST)NULL;
	}
	else
	{
		m_lpTail->next = lpCD;
		lpCD->prev = m_lpTail;
	}

	lpCD->next = (LPLINKEDLIST)NULL;
	m_lpTail = lpCD;

	m_nCount++;

	fRet = TRUE;
//		LeaveCriticalSection(&m_cs);

	return fRet;
}

template<class T> void CWHList<T>::Clear()
{
//	EnterCriticalSection(&m_cs);

	LPLINKEDLIST	lpNode	= m_lpHead;

	while (lpNode)
		lpNode = (LPLINKEDLIST)RemoveNode(lpNode);
//		LeaveCriticalSection(&m_cs);
}

template<class T> PLISTNODE CWHList<T>::RemoveNode(LPLINKEDLIST lpList)
{
    LPLINKEDLIST prev = NULL, next;

//	EnterCriticalSection(&m_cs);

	next = lpList->next;
	prev = lpList->prev;

	if (prev) prev->next = next;
	else m_lpHead = next;

	if (next) next->prev = prev;
	else m_lpTail = prev;

	GlobalFree(lpList);
	lpList = NULL;

	m_nCount--;
//		LeaveCriticalSection(&m_cs);

	return (PLISTNODE)next;
}

template <class T>BOOL CWHList<T>::RemoveNodeByKey(SOCKET s)
{
//	EnterCriticalSection(&m_cs);

    LPLINKEDLIST	prev	= NULL, next;
	LPLINKEDLIST	lpNode	= m_lpHead;
	BOOL			fFlag	= FALSE;

	while (lpNode)
	{
		if (((CSessionInfo*)lpNode->lpData)->sock == s)
		{
			next = lpNode->next;
			prev = lpNode->prev;

			if (prev) prev->next = next;
			else m_lpHead = next;

			if (next) next->prev = prev;
			else m_lpTail = prev;

			GlobalFree(lpNode);
			lpNode = NULL;

			m_nCount--;

			fFlag = TRUE;

			break;
		}

		lpNode = lpNode->next;
	}
//		LeaveCriticalSection(&m_cs);

	return fFlag;
}

template <class T>PLISTNODE CWHList<T>::RemoveNodeByData(T lpData)
{
//	EnterCriticalSection(&m_cs);

    LPLINKEDLIST	prev	= NULL, next;
	LPLINKEDLIST	lpNode	= m_lpHead;

	__try
	{
		while (lpNode)
		{
			if ((T)lpNode->lpData == lpData)
			{
				next = lpNode->next;
				prev = lpNode->prev;

				if (prev) prev->next = next;
				else m_lpHead = next;

				if (next) next->prev = prev;
				else m_lpTail = prev;

				GlobalFree(lpNode);
				lpNode = NULL;

				m_nCount--;

				break;
			}

			lpNode = lpNode->next;
		}
	}
	__finally
	{
//		LeaveCriticalSection(&m_cs);
	}

	return (PLISTNODE)next;
}

template <class T> void *CWHList<T>::FindNode(LPLINKEDLIST lpList)
{
//	EnterCriticalSection(&m_cs);

	LPLINKEDLIST lpNode = m_lpHead;

	while (lpNode)
	{
		if (lpNode == lpList)
		{
			// ORZ: 코드 추가
//			LeaveCriticalSection( &m_cs );
			return lpNode;
		}

		lpNode = lpNode->next;
	}

//	LeaveCriticalSection(&m_cs);

	return NULL;
}

template <class T>PLISTNODE CWHList<T>::FindData(T t)
{
	LPLINKEDLIST lpNode = m_lpHead;

//	EnterCriticalSection(&m_cs);

	while (lpNode)
	{
		if (((T)lpNode->lpData) == t)
		{
			// ORZ: 코드 추가
//			LeaveCriticalSection( &m_cs );
			return (PLISTNODE)lpNode;
		}

		lpNode = lpNode->next;
	}

//	LeaveCriticalSection(&m_cs);

	return (PLISTNODE) NULL;
}
