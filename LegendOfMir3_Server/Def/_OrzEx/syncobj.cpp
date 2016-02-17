

#include "stdafx.h"
#include "syncobj.h"
#include "error.h"


CCriticalSection::CCriticalSection()
{
	InitializeCriticalSection( &m_csAccess );
}


CCriticalSection::~CCriticalSection()
{
	DeleteCriticalSection( &m_csAccess );
}


void CCriticalSection::Lock()
{
	EnterCriticalSection( &m_csAccess );
}


void CCriticalSection::Unlock()
{
	LeaveCriticalSection( &m_csAccess );
}
