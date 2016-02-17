class CReadyUserInfo
{
public:
//	char		m_szUserID[10];
//	char		m_szCharName[14];
	BOOL		m_fStartNew;
//	int			m_nCertification;
//	int			m_nApprovalMode;
//	int			m_nClientVersion;

//	SOCKET		m_s;

//	int			m_nUserGateIndex;

//	CGateInfo*	m_pGateInfo;
	CUserInfo*	m_pUserInfo;

	DWORD		m_dwReadyStartTime;
	BOOL		m_fClosed;
};

class CReadyUserInfo2
{
public:
	BOOL		m_fStartNew;
	DWORD		m_dwReadyStartTime;

	int			m_nNumOfGenItem;
	int			m_nNumOfItem;
	int			m_nNumOfMagic;

	_THUMANRCD	m_THumanRcd;

	char		pszData[8096];
};

void LoadPlayer(CUserInfo* pUserInfo);
BOOL LoadPlayer(CReadyUserInfo2* pReadyUserInfo, CUserInfo* pUserInfo);
