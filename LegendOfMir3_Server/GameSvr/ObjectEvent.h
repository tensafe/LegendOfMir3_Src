
#define ET_DIGOUTZOMBI    1  //좀비가 땅파고 나온 흔적
#define ET_MINE           2  //광석이 매장되어 있음
#define ET_PILESTONES     3  //돌무더기
#define ET_HOLYCURTAIN    4  //결계
#define ET_FIRE           5
#define ET_SCULPEICE      6  //주마왕의 돌깨진 조각

class CEvent
{
public:
	BOOL			m_fVisible;
	BOOL			m_fActive;

	int				m_nCheck;
	CMirMap*		m_pMap;
	int				m_nX;
	int				m_nY;
	int				m_nEventType;
	int				m_nEventParam;

	DWORD			m_dwOpenStartTime;
	DWORD			m_dwContinueTime;
	DWORD			m_dwCloseTime;

	BOOL			m_fClosed;

	int				m_nDamage;

	CCharObject*	m_pOwnObject;

	DWORD			m_dwRunStart;
	DWORD			m_dwRunTick;

	void Create(CMirMap* pMap, int nX, int nY, DWORD dwETime, int nType, BOOL fVisible);
	void Close();
	virtual void Run();
};

class CFireBurnEvent : public CEvent
{
protected:
	DWORD			m_dwTickTime;
public:

	CFireBurnEvent(int nX, int nY, DWORD dwETime, CCharObject* pCharObject, int nDamage)
	{
		Create(nX, nY, dwETime, pCharObject, nDamage);
	}

	void	Create(int nX, int nY, DWORD dwETime, CCharObject* pCharObject, int nDamage);
	virtual void	Run();
};

class CHolyCurtainEvent : public CEvent
{
public:
	CHolyCurtainEvent(int nX, int nY, DWORD dwETime, CCharObject* pCharObject)
	{
		CEvent::Create(pCharObject->m_pMap, nX, nY, dwETime, ET_HOLYCURTAIN, TRUE);
	}
};
