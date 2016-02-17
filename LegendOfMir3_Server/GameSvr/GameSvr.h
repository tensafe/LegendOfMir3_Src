

#pragma once

#include "../def/staticArray.h"
#include "../def/_orzex/syncobj.h"

#define _RUNGATE_STATUS_DISCONNECTED		0
#define _RUNGATE_STATUS_GOOD				1
#define _RUNGATE_STATUS_HEAVYTRAFFIC		2

#define LOGPARAM_STR						1
#define LOGPARAM_INT						2

#define _NUM_OF_MAXUSER						5000

#define HAM_ALL								0
#define HAM_PEACE							1
#define HAM_GROUP							2
#define HAM_GUILD							3
#define HAM_PKATTACK						4

#define BAGGOLD								5000000

#define USERMODE_PLAYGAME					1
#define USERMODE_LOGIN						2
#define USERMODE_LOGOFF						3
#define USERMODE_NOTICE						4

void MakeItemToDB(int nReadyUserInfo, char *pszUserId, char *pszCharName, _LPTMAKEITEMRCD lpMakeItemRcd);

typedef struct tagOVERLAPPEDEX
{
	OVERLAPPED				Overlapped;
	INT						nOvFlag;
	WSABUF					DataBuf;
	CHAR					Buffer[DATA_BUFSIZE * 2];
	int						bufLen;
} OVERLAPPEDEX, *LPOVERLAPPEDEX;

typedef struct tag_TSENDBUFF
{
	int				nLen;
	char			szData[DATA_BUFSIZE];
}_TSENDBUFF, *_LPTSENDBUFF;

class CGateInfo
{
public:
	SOCKET					m_sock;

	BOOL					m_fDoSending;
	CWHQueue				m_xSendBuffQ;
	OVERLAPPEDEX			OverlappedEx[2];
	
public:
	CGateInfo();

	void	SendGateCheck();
	void	OpenNewUser(char *pszPacket);

	void	xSend();
	int		Send(_LPTSENDBUFF lpSendBuff);
	int		Recv();
	bool	HasCompletionPacket();
	char*	ExtractPacket( char *pPacket );
};

class CUserInfo	: public CIntLock, CStaticArray< CUserInfo >::IArrayData
{
public:
	bool							m_bEmpty;

	int								m_sock;

	char							m_szUserID[12];
	char							m_szCharName[14];

	int								m_nUserGateIndex;
	int								m_nUserServerIndex;
	
	int								m_nCertification;
	int								m_nClientVersion;

	CPlayerObject*					m_pxPlayerObject;
	CGateInfo*						m_pGateInfo;

	BYTE							m_btCurrentMode;

	_THUMANRCD						m_THumanRcd;
	int								m_nNumOfMagics;
	int								m_nNumOfGenItems;
	int								m_nNumOfItems;

	CWHList<_LPTGENERALITEMRCD>		m_lpTGenItemRcd;
	CWHList<_LPTHUMANMAGICRCD>		m_lpTMagicRcd;
	CWHList<_LPTUSERITEMRCD>		m_lpTItemRcd;

	_LPTHUMANMAGICRCD				m_lpTMagicErgumSkill;
	BOOL							m_fLongHitSkill;
	_LPTHUMANMAGICRCD				m_lpTMagicBanwolSkill;
	BOOL							m_fWideHitSkill;
	_LPTHUMANMAGICRCD				m_lpTMagicFireSwordSkill;
	BOOL							m_fFireHitSkill;
	DWORD							m_dwLatestFireHitTime;
	
	_LPTHUMANMAGICRCD				m_lpTMagicOneSwordSkill;
	_LPTHUMANMAGICRCD				m_lpTMagicIlkwangSkill;

	_LPTHUMANMAGICRCD				m_lpTMagicPowerHitSkill;
	BOOL							m_fPowerHitSkill;

	BYTE							m_btAttackSkillCount;
	BYTE							m_btAttackSkillPointCount;

	_LPTHORSERCD					m_lpTHorseRcd;

public:
	CUserInfo();
	bool IsEmpty();

	void							CloseAccount(char *pszName, int nCertification);
	void							SaveHumanToDB(char *pszUserAddr);
	void							CloseUserHuman();

	void							DoClientCertification(char *pszPacket);

	void							Operate();

	_LPTUSERITEMRCD					GetItem(char *pszMakeIndex);
	__inline _LPTUSERITEMRCD		GetUseItem(int nPosition) { return GetItem(m_THumanRcd.szTakeItem[nPosition]); }
	
	_LPTGENERALITEMRCD				GetUseGenItem(char *pszMakeItemID);
	void							RemoveGenItem(_LPTGENERALITEMRCD lptGenItemRcd);

	int								EncodeMyMagicToServer(char *pszEncodeMsg, int nBuffSize, int& nPos);
	int								EncodeMyMagic(char *pszEncodeMsg, int nBuffSize, int& nPos);
	int								EncodeGenItem(char *pszEncodeMsg, int nBuffSize, int& nPos);

	int								GetWeaponFeature();
	int								GetDressFeature();

	void							AddNewItem(char *pszItemName);
	
	void							MakeGenItem(_LPTGENERALITEMRCD lpItemRcd);

	BOOL							IsMyMagic(int nIndex);

	void							AllowLongHitSkill(BOOL fFlag);
	void							AllowWideHitSkill(BOOL fFlag);
	BOOL							AllowFireHitSkill();

	int								CalcBagWeight();
	int								CalcWearWeightEx(int nIndex);

	int								GetGenItemStdIdx(char *pszMakeIndex);
	int								GetGenItemVal1(char *pszMakeIndex);
	int								GetGenItemVal2(char *pszMakeIndex);

	BOOL							UserDropItem(int nItemIndex, char *pszMakeIndex);
	BOOL							UserDropGenItem(int nItemIndex, char *pszMakeIndex);

	BOOL							IncGold(int nGold);
	BOOL							DecGold(int nGold);

	_LPTHUMANMAGICRCD				GetMagicRcdByID(int nID);
	_LPTHUMANMAGICRCD				GetMagicRcdByKey(int nKey);

	_LPTGENERALITEMRCD				CanUseBujuk();

	void							SetGenItemVal1(int nVal);

	void							ProcessUserMessage(char *pszPacket);

	__inline	_LPTUSERITEMRCD		GetWeapon() { return GetUseItem(U_WEAPON); }
	__inline	_LPTUSERITEMRCD		SetWeapon(char *pszMakeIndex) { memmove(m_THumanRcd.szTakeItem[U_WEAPON], pszMakeIndex, _MAKEITEMINDEX); }
	__inline	void				SetEmptyWeapon() { ZeroMemory(m_THumanRcd.szTakeItem[U_WEAPON], _MAKEITEMINDEX); m_THumanRcd.szTakeItem[U_WEAPON][0] = '0'; }
	__inline	void				SetEmptyDress() { ZeroMemory(m_THumanRcd.szTakeItem[U_DRESS], _MAKEITEMINDEX); m_THumanRcd.szTakeItem[U_DRESS][0] = '0'; }
	__inline	void				SetEmptyUseItem(int i) { ZeroMemory(m_THumanRcd.szTakeItem[i], _MAKEITEMINDEX); m_THumanRcd.szTakeItem[i][0] = '0'; }
	__inline	_LPTUSERITEMRCD		GetDress() { return GetUseItem(U_DRESS); }
	__inline	_LPTUSERITEMRCD		GetAccessory(int nPosition) { return GetUseItem(nPosition); }
	__inline	_LPTGENERALITEMRCD	GetRightHand() { return GetUseGenItem(m_THumanRcd.szTakeItem[U_RIGHTHAND]); }

	__inline	BYTE				GetGenderFeature() { return m_THumanRcd.btGender; }
	__inline	char				GetHairFeature() { return m_THumanRcd.szHair; }
	__inline	BYTE				GetHorseFeatureEx() { if (m_lpTHorseRcd) return m_lpTHorseRcd->btHorseType; else return 0; }
	__inline	int					GetJob() { return m_THumanRcd.btJob; }
	__inline	void				SetJob(int nJob) { if (nJob >= 0 && nJob < 3) m_THumanRcd.btJob = (BYTE)nJob; }

	__inline	int					GetAttackMode() { return (int)m_THumanRcd.btAttackMode; }

	__inline	int					GetMagicID(char *pszMakeIndex) { return GetGenItemVal2(pszMakeIndex); }

	__inline	BOOL				IsEnoughBag() { if (m_lpTItemRcd.GetCount() + m_lpTGenItemRcd.GetCount() >= USERITEMMAX) return FALSE; return TRUE; }

	__inline	DWORD				GetGold() { return m_THumanRcd.dwGold; }

};

void InsertLogMsg(UINT nID);
void InsertLogMsg(LPTSTR lpszMsg);
void InsertLogPacket(char *pszPacket);
void InsertLogMsgParam(UINT nID, void *pParam, BYTE btFlags);
