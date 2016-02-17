
#define _MAKEITEMINDEX	12

class CMonItem
{
public:
	int			m_nPoint;
	BYTE		m_btItemType;
	WORD		m_wItemIndex;
	WORD		m_wCount;
};

class CMonRaceInfo
{
public:
	int			nIndex;

	char		szMonName[14];
	int			nMonRace;
	int			nAppear;

	BYTE		btUndead;

	BYTE		Level;
	
	WORD		HP;
	WORD		MP;

	WORD		AC;
	WORD		MAC;
	WORD		DC;
	WORD		MC;

	DWORD		dwAttackSpeed;
	DWORD		dwWalkSpeed;

	BYTE		ViewRange;

	WORD		Speed;
	WORD		Hit;

	WORD		wRaceIndex;

	WORD		Exp;

	WORD		wEscape;

	WORD		m_wWater;
	WORD		m_wFire;
	WORD		m_wWind;
	WORD		m_wLight;
	WORD		m_wEarth;

	CWHList<CMonItem*>	m_xMonItemList;
};

class CMonsterGenInfo
{
public:
	char						szMapName[14];

	int							nX;
	int							nY;

	char						szMonName[14];
	int							nMonIndex;

	int							nAreaX;
	int							nAreaY;
	int							nCount;

	DWORD						dwZenTime;
	DWORD						dwStartTime;

	CWHList<CMonsterObject*>	xMonsterObjList;		// MonList

	int							nSmallZenRate;
};

typedef struct tag_TMONSTER
{
	char		szName[14];

	BYTE		btRace;
	BYTE		btRaceImg;
	WORD		wAppr;
	BYTE		btLevel;
	BYTE		btLifeAttrib;
	WORD		wExp;
	WORD		wHP;
	BYTE		btAC;
	BYTE		btMAC;
	BYTE		btDC;
	BYTE		btMaxDC;
	BYTE		btMC;
	BYTE		btSC;
	BYTE		btSpeed;
	BYTE		btHit;
	WORD		wWalkSpeed;
	WORD		wAttackSpeed;
	//Item List
} _TMONSTER, *_LPTMONSTER;

class CMapInfo
{
public:
	char		szMapFileName[14];
	char		szMapName[40];
	DWORD		dwAttribute;

	BYTE		btMapSeries;
	BYTE		btMapSeriesValue;
};

class CMoveMapEventInfo
{
public:
	char		szSMapFileName[14];
	int			nSX;
	int			nSY;
	char		szDMapFileName[14];
	int			nDX;
	int			nDY;

	BOOL		fIsOpen;
};

class CMerchantInfo
{
public:
	char		szMapName[14];
	int			nPosX;
	int			nPosY;
	char		szNPCName[40];
	short		sFace;
	short		sBody;
	short		sGender;
};
