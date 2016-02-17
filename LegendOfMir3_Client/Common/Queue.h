#define QUEUE_MAX 100

class CWHQueue
{
protected:
	UINT					m_btCount;

	UINT					m_btQPushPos;
	UINT					m_btQPopPos;
	
	BYTE					*m_lpCircularQ[QUEUE_MAX];

	CRITICAL_SECTION		m_cs;

public:
	CWHQueue();
	~CWHQueue();
	
	BOOL PushQ(BYTE *lpbtQ);
	BYTE *PopQ();
	
	__inline UINT GetCurPushPos() { return m_btQPushPos; }
	__inline UINT GetCurPopPos() { return m_btQPopPos; }

	__inline UINT GetCount() { return m_btCount; }
};