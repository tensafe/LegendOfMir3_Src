

#pragma once

#include "../def/_orzex/queue.h"
#include "../def/_orzex/syncobj.h"

class CWHQueue : public CQueue< BYTE >, public CIntLock
{
public:
	virtual ~CWHQueue()
	{
		ClearAll();
	}

	bool PushQ( BYTE *lpbtQ )
	{
		Lock();
		bool bRet = Enqueue( lpbtQ );
		Unlock();

		return bRet;
	}

	BYTE * PopQ()
	{
		Lock();
		BYTE *pData = Dequeue();
		Unlock();

		return pData;
	}
};


/*
#define QUEUE_MAX 1000000


class CWHQueue : public CIntLock
{
protected:
	UINT					m_btCount;

	UINT					m_btQPushPos;
	UINT					m_btQPopPos;
	
	BYTE					*m_lpCircularQ[QUEUE_MAX];

//	CRITICAL_SECTION		m_cs;

public:
	CWHQueue();
	~CWHQueue();
	
	BOOL PushQ(BYTE *lpbtQ);
	BYTE *PopQ();
	
	__inline UINT GetCurPushPos() { return m_btQPushPos; }
	__inline UINT GetCurPopPos() { return m_btQPopPos; }

	__inline UINT GetCount() { return m_btCount; }
};
*/