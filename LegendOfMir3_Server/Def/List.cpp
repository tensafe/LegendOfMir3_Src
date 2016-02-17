//
//  Euyheon's Linked List Class
//
//  All written by Euy-heon, Jeong.
//  Copyright(C) 1999-2001 Euy-heon, Jeong. All rights reserved.
//
//	Win32 Version : Compiled by Microsoft Visual C++ 6.0
//
//	Problems & History
//	------------------

#include "stdafx.h"

template<class T> CWHList<T>::CWHList()
{				  
	m_lpHead = NULL;
	m_lpTail = NULL;

	m_nCount = 0;

	InitializeCriticalSection(&m_cs);
}

template<class T> CWHList<T>::~CWHList()
{
	Clear();

	DeleteCriticalSection(&m_cs);
}

template<class T> BOOL CWHList<T>::AddNewNode(T lpData)
{
	BOOL fRet = FALSE;

	EnterCriticalSection(&m_cs);

	__try
	{
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
	}
	__finally
	{
		LeaveCriticalSection(&m_cs);
	}

	return fRet;
}

template<class T> void CWHList<T>::Clear()
{
	EnterCriticalSection(&m_cs);

	LPLINKEDLIST	lpNode	= m_lpHead;

	__try
	{
		while (lpNode)
			lpNode = RemoveNode(lpNode);
	}
	__finally
	{
		LeaveCriticalSection(&m_cs);
	}
}

/*template<class T> LPLINKEDLIST CWHList<T>::RemoveNode(LPLINKEDLIST lpList)
{
    LPLINKEDLIST prev = NULL, next;

	EnterCriticalSection(&m_cs);

	__try
	{
		next = lpList->next;
		prev = lpList->prev;

		if (prev) prev->next = next;
		else m_lpHead = next;

		if (next) next->prev = prev;
		else m_lpTail = prev;

		GlobalFree(lpList);

		m_nCount--;
	}
	__finally
	{
		LeaveCriticalSection(&m_cs);
	}

	return prev;
}
*/
template <class T>BOOL CWHList<T>::RemoveNodeByKey(SOCKET s)
{
	EnterCriticalSection(&m_cs);

    LPLINKEDLIST	prev	= NULL, next;
	LPLINKEDLIST	lpNode	= m_lpHead;
	BOOL			fFlag	= FALSE;

	__try
	{
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

				m_nCount--;

				fFlag = TRUE;

				break;
			}

			lpNode = lpNode->next;
		}
	}
	__finally
	{
		LeaveCriticalSection(&m_cs);
	}

	return fFlag;
}

template <class T>BOOL CWHList<T>::RemoveNodeByData(T lpData)
{
//	EnterCriticalSection(&m_cs);

    LPLINKEDLIST	prev	= NULL, next;
	LPLINKEDLIST	lpNode	= m_lpHead;
	BOOL			fFlag	= FALSE;

	__try
	{
		while (lpNode)
		{
			if ((CSessionInfo*)lpNode->lpData == lpData)
			{
				next = lpNode->next;
				prev = lpNode->prev;

				if (prev) prev->next = next;
				else m_lpHead = next;

				if (next) next->prev = prev;
				else m_lpTail = prev;

				GlobalFree(lpNode);

				m_nCount--;

				fFlag = TRUE;

				break;
			}

			lpNode = lpNode->next;
		}
	}
	__finally
	{
//		LeaveCriticalSection(&m_cs);
	}

	return fFlag;
}
/*
LPLINKEDLIST CWHList::FindNode(LPLINKEDLIST lpList)
{
	EnterCriticalSection(&m_cs);

	LPLINKEDLIST lpNode = m_lpHead;

	while (lpNode)
	{
		if (lpNode == lpList)
			return lpNode;

		lpNode = lpNode->next;
	}

	LeaveCriticalSection(&m_cs);

	return NULL;
}
*/
template <class T>T CWHList<T>::FindDataByKey(SOCKET s)
{
	LPLINKEDLIST lpNode = m_lpHead;

//	EnterCriticalSection(&m_cs);

	while (lpNode)
	{
		if (((CSessionInfo*)lpNode->lpData)->sock == s)
			return (CSessionInfo*)lpNode->lpData;

		lpNode = lpNode->next;
	}

//	LeaveCriticalSection(&m_cs);

	return NULL;
}
