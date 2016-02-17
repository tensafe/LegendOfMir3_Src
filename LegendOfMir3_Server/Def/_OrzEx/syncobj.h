

/*
	Synchronization Objects

	Date:
		(Last Updated: 2002/03/06)
*/
#ifndef __ORZ_THREAD_SYNCHRONIZATION_OBJECT__
#define __ORZ_THREAD_SYNCHRONIZATION_OBJECT__


#include <windows.h>


class CCriticalSection 
{
public:
	CRITICAL_SECTION	m_csAccess;

public:
	CCriticalSection();
	virtual ~CCriticalSection();

	void Lock();
	void Unlock();
};


typedef CCriticalSection	CIntLock;	// InterThread Lock


#endif