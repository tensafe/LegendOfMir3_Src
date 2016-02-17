#include "stdafx.h"

void UpdateStatusBarUsers(BOOL fGrow);
void SendRDBSocket(int nCertification, char *pszData1, char *pszData2, int nData2Len);

CUserInfo::CUserInfo()
{
	m_bEmpty					= true;

	m_pxPlayerObject			= NULL;
	m_pGateInfo					= NULL;

	m_nNumOfMagics				= 0;
	m_nNumOfGenItems			= 0;
	m_nNumOfItems				= 0;

	m_lpTMagicErgumSkill		= NULL;
	m_lpTMagicBanwolSkill		= NULL;
	m_lpTMagicFireSwordSkill	= NULL;
	m_lpTMagicOneSwordSkill		= NULL;
	m_lpTMagicIlkwangSkill		= NULL;
	m_lpTMagicPowerHitSkill		= NULL;

	m_fLongHitSkill				= FALSE;
	m_fWideHitSkill				= FALSE;
	m_fFireHitSkill				= FALSE;

	m_dwLatestFireHitTime		= 0;
}

bool CUserInfo::IsEmpty()
{
	return m_bEmpty;
}
/*
void CUserInfo::RandomUpgradeItem(WORD wStdMode, _LPTMAKEITEMRCD lpMakeItemRcd)
{
	if (wStdMode == 5 || wStdMode == 6)
		g_pStdItemWeapon[lpMakeItemRcd->nStdIndex - 1].UpgradeRandomItem(lpMakeItemRcd);
	else if (wStdMode == 10 || wStdMode == 11)
		g_pStdItemArmor[lpMakeItemRcd->nStdIndex - 1].UpgradeRandomItem(lpMakeItemRcd);
	else if (wStdMode >= 21 && wStdMode <= 50)
		g_pStdItemAccessory[lpMakeItemRcd->nStdIndex - 1].UpgradeRandomItem(lpMakeItemRcd);
}
*/
void CUserInfo::AddNewItem(char *pszItemName)
{
	int		i;
//	WORD	wStdMode;

	_TMAKEITEMRCD		MakeItemRcd;

	if (m_lpTItemRcd.GetCount() + m_lpTGenItemRcd.GetCount() > USERITEMMAX) return;

	int nLen = memlen(pszItemName) - 1;
	int nCmpLen;

	for (i = 0; i < g_nStdItemSpecial; i++)
	{
		nCmpLen = memlen(g_pStdItemSpecial[i].szName) - 1;

		if (nCmpLen == nLen)
		{
			if (memcmp(g_pStdItemSpecial[i].szName, pszItemName, nLen) == 0)
			{
				ZeroMemory(MakeItemRcd.btValue, sizeof(MakeItemRcd.btValue));

				MakeItemRcd.nStdIndex		= i;

				switch (g_pStdItemSpecial[i].btType)
				{
					case 0:
						MakeItemRcd.szStdType		= 'A';
						break;
					case 1:
						MakeItemRcd.szStdType		= 'B';
						break;
					case 2:
						MakeItemRcd.szStdType		= 'C';
						break;
				}

				MakeItemRcd.nDura			= g_pStdItemSpecial[i].wDuraMax;
				MakeItemRcd.nDuraMax		= g_pStdItemSpecial[i].wDuraMax;

//				wStdMode					= g_pStdItemSpecial[i].wStdMode;

				if (rand() % 10 == 0)
					g_pStdItemSpecial[i].UpgradeRandomItem(MakeItemRcd.btValue, MakeItemRcd.nDura, MakeItemRcd.nDuraMax);

				MakeItemToDB((int)this, m_szUserID, m_szCharName, &MakeItemRcd);
			}
		}
	}

	for (i = 0; i < g_nStdItemEtc; i++)
	{
		nCmpLen = memlen(g_pStdItemEtc[i].szName) - 1;

		if (nCmpLen == nLen)
		{
			if (memcmp(g_pStdItemEtc[i].szName, pszItemName, nLen) == 0)
			{
				_LPTGENERALITEMRCD		lptGenItemRcd = new _TGENERALITEMRCD;

				if (lptGenItemRcd)
				{
//					lptGenItemRcd->szStdType	= g_pStdItemEtc[i].m_btType;
					lptGenItemRcd->nStdIndex	= i;
					lptGenItemRcd->nDura		= g_pStdItemEtc[i].wDuraMax;
					lptGenItemRcd->nDuraMax		= g_pStdItemEtc[i].dwRSource;

					MakeGenItem(lptGenItemRcd);
				}

				return;
			}
		}
	}
}

void CUserInfo::MakeGenItem(_LPTGENERALITEMRCD lptGenItemRcd)
{
	if (m_pxPlayerObject)
	{
		// Make Item on Server
		_TGENITEMRCD		GenItemRcd;

		sprintf(GenItemRcd.szItem, "G%03d%04d%04d", lptGenItemRcd->nStdIndex, lptGenItemRcd->nDura, lptGenItemRcd->nDuraMax);

		memcpy(lptGenItemRcd->szMakeIndex, GenItemRcd.szItem, 12);

		m_lpTGenItemRcd.AddNewNode(lptGenItemRcd);

		_TCLIENTITEMRCD		tClientItemRcd;
		_TDEFAULTMESSAGE	SendDefMsg;
		char				szEncodeMsg[256];

		fnMakeDefMessage(&SendDefMsg, SM_ADDITEM, (int)m_pxPlayerObject, 0, 0, 1);

		// Get Standard General Item (STDITEM_ETC)
		g_pStdItemEtc[lptGenItemRcd->nStdIndex].GetStandardItem(&tClientItemRcd);

		memcpy(tClientItemRcd.szMakeIndex, lptGenItemRcd->szMakeIndex, 12);
		tClientItemRcd.nDura		= lptGenItemRcd->nDura;
		tClientItemRcd.nDuraMax		= lptGenItemRcd->nDuraMax;

		int nIdx = 	fnEncode6BitBufA((unsigned char *)&tClientItemRcd, szEncodeMsg, sizeof(_TCLIENTITEMRCD), sizeof(szEncodeMsg));
		szEncodeMsg[nIdx] = '\0';

		m_pxPlayerObject->SendSocket(&SendDefMsg, szEncodeMsg);
	}
}

int	CUserInfo::GetDressFeature()
{
	if (m_lpTItemRcd.GetCount())
	{
		PLISTNODE pListNode = m_lpTItemRcd.GetHead();

		while (pListNode)
		{
			_LPTUSERITEMRCD lpTItemRcd = m_lpTItemRcd.GetData(pListNode);
		
			if (memcmp(m_THumanRcd.szTakeItem[U_DRESS], lpTItemRcd->szMakeIndex, _MAKEITEMINDEX) == 0)
				return g_pStdItemSpecial[lpTItemRcd->nStdIndex].wShape;

			pListNode = m_lpTItemRcd.GetNext(pListNode);
		}
	}

	return 0;
}

int	CUserInfo::GetWeaponFeature()
{
	if (m_lpTItemRcd.GetCount())
	{
		PLISTNODE pListNode = m_lpTItemRcd.GetHead();

		while (pListNode)
		{
			_LPTUSERITEMRCD lpTItemRcd = m_lpTItemRcd.GetData(pListNode);
		
			if (memcmp(m_THumanRcd.szTakeItem[U_WEAPON], lpTItemRcd->szMakeIndex, _MAKEITEMINDEX) == 0)
				return g_pStdItemSpecial[lpTItemRcd->nStdIndex].wShape;

			pListNode = m_lpTItemRcd.GetNext(pListNode);
		}
	}

	return 0;
}

_LPTUSERITEMRCD	CUserInfo::GetItem(char *pszMakeIndex)
{
	if (m_lpTItemRcd.GetCount())
	{
		PLISTNODE pListNode = m_lpTItemRcd.GetHead();

		while (pListNode)
		{
			_LPTUSERITEMRCD lpTItemRcd = m_lpTItemRcd.GetData(pListNode);
		
			if (memcmp(pszMakeIndex, lpTItemRcd->szMakeIndex, _MAKEITEMINDEX) == 0)
				return lpTItemRcd;

			pListNode = m_lpTItemRcd.GetNext(pListNode);
		}
	}

	return NULL;
}

_LPTGENERALITEMRCD CUserInfo::GetUseGenItem(char *pszMakeItemID)
{
	if (m_lpTGenItemRcd.GetCount())
	{
		PLISTNODE pListNode = m_lpTGenItemRcd.GetHead();

		while (pListNode)
		{
			_LPTGENERALITEMRCD lpTItemRcd = m_lpTGenItemRcd.GetData(pListNode);
		
			if (memcmp(pszMakeItemID, lpTItemRcd->szMakeIndex, _MAKEITEMINDEX) == 0)
				return lpTItemRcd;

			pListNode = m_lpTGenItemRcd.GetNext(pListNode);
		}
	}

	return NULL;
}

BOOL CUserInfo::IsMyMagic(int nIndex)
{
	if (m_lpTMagicRcd.GetCount())
	{
		_LPTHUMANMAGICRCD	lptUserMagicRcd;

		PLISTNODE pListNode = m_lpTMagicRcd.GetHead();

		while (pListNode)
		{
			lptUserMagicRcd = m_lpTMagicRcd.GetData(pListNode);

			if (lptUserMagicRcd->btMagicID == nIndex)
				return TRUE;

			pListNode = m_lpTMagicRcd.GetNext(pListNode);
		}
	}

	return FALSE;
}

int CUserInfo::EncodeMyMagicToServer(char *pszEncodeMsg, int nBuffSize, int& nPos)
{
	int	nCnt = 0;

	if (m_lpTMagicRcd.GetCount())
	{
		_LPTHUMANMAGICRCD	lptUserMagicRcd;

		PLISTNODE pListNode = m_lpTMagicRcd.GetHead();

		while (pListNode)
		{
			lptUserMagicRcd = m_lpTMagicRcd.GetData(pListNode);

			if (lptUserMagicRcd)
			{
				nPos +=	fnEncode6BitBufA((unsigned char *)lptUserMagicRcd, pszEncodeMsg + nPos, sizeof(_THUMANMAGICRCD), nBuffSize - nPos);

				*(pszEncodeMsg + nPos) = '/';

				nPos++;
				nCnt++;
			}

			pListNode = m_lpTMagicRcd.GetNext(pListNode);
		}
	}

	return nCnt;
}

int CUserInfo::EncodeMyMagic(char *pszEncodeMsg, int nBuffSize, int& nPos)
{
	int	nCnt = 0;

	if (m_lpTMagicRcd.GetCount())
	{
		_LPTHUMANMAGICRCD	lptUserMagicRcd;
		_TCLIENTMAGICRCD	tClientMagicRcd;

		PLISTNODE pListNode = m_lpTMagicRcd.GetHead();

		while (pListNode)
		{
			lptUserMagicRcd = m_lpTMagicRcd.GetData(pListNode);

			if (lptUserMagicRcd)
			{
				tClientMagicRcd.btMagicID	= lptUserMagicRcd->btMagicID;
				tClientMagicRcd.btLevel		= lptUserMagicRcd->btLevel;
				tClientMagicRcd.btUseKey	= lptUserMagicRcd->btUseKey;
				tClientMagicRcd.nCurrTrain	= lptUserMagicRcd->nCurrTrain;

				tClientMagicRcd.tStdMagic.btEffect		= (BYTE)g_pMagicInfo[lptUserMagicRcd->btMagicID].sEffect;
				tClientMagicRcd.tStdMagic.btEffectType	= (BYTE)g_pMagicInfo[lptUserMagicRcd->btMagicID].sEffectType;
				memcpy(&tClientMagicRcd.tStdMagic.sNeed, g_pMagicInfo[lptUserMagicRcd->btMagicID].sNeed, sizeof(tClientMagicRcd.tStdMagic.sNeed));
				memcpy(&tClientMagicRcd.tStdMagic.nTrain, g_pMagicInfo[lptUserMagicRcd->btMagicID].nTrain, sizeof(tClientMagicRcd.tStdMagic.nTrain));
				tClientMagicRcd.tStdMagic.btDefSpell	= (BYTE)g_pMagicInfo[lptUserMagicRcd->btMagicID].sDefSpell;
				tClientMagicRcd.tStdMagic.nDelayTime	= g_pMagicInfo[lptUserMagicRcd->btMagicID].sDelay;

				nPos +=	fnEncode6BitBufA((unsigned char *)&tClientMagicRcd, pszEncodeMsg + nPos, sizeof(_TCLIENTMAGICRCD), nBuffSize - nPos);

				*(pszEncodeMsg + nPos) = '/';

				nPos++;
				nCnt++;
			}

			pListNode = m_lpTMagicRcd.GetNext(pListNode);
		}
	}

	return nCnt;
}

void CUserInfo::RemoveGenItem(_LPTGENERALITEMRCD lptItemRcd)
{
	if (m_lpTGenItemRcd.GetCount())
	{
		_LPTGENERALITEMRCD	lptGenItemRcd;

		PLISTNODE pListNode = m_lpTGenItemRcd.GetHead();

		while (pListNode)
		{
			lptGenItemRcd = m_lpTGenItemRcd.GetData(pListNode);

			if (lptGenItemRcd == lptItemRcd)
			{
				m_lpTGenItemRcd.RemoveNode(pListNode);
				delete lptGenItemRcd;
				lptGenItemRcd = NULL;
				return;
			}

			pListNode = m_lpTGenItemRcd.GetNext(pListNode);
		}
	}
}

int CUserInfo::EncodeGenItem(char *pszEncodeMsg, int nBuffSize, int& nPos)
{
	int	nCnt = 0;

	if (m_lpTGenItemRcd.GetCount())
	{
		_LPTGENERALITEMRCD	lptGenItemRcd;
		_TGENITEMRCD		GenItemRcd;
		TCHAR				wszItem[13];

		PLISTNODE pListNode = m_lpTGenItemRcd.GetHead();

		while (pListNode)
		{
			lptGenItemRcd = m_lpTGenItemRcd.GetData(pListNode);

			if (lptGenItemRcd)
			{
				wsprintf(wszItem, _TEXT("G%03d%04d%04d"), lptGenItemRcd->nStdIndex, lptGenItemRcd->nDura, lptGenItemRcd->nDuraMax);
				WideCharToMultiByte(CP_ACP, 0, wszItem, -1, GenItemRcd.szItem, sizeof(GenItemRcd.szItem), NULL, NULL);

				nPos +=	fnEncode6BitBufA((unsigned char *)&GenItemRcd, pszEncodeMsg + nPos, sizeof(_TGENITEMRCD), nBuffSize - nPos);

				*(pszEncodeMsg + nPos) = '/';

				nPos++;
				nCnt++;
			}

			pListNode = m_lpTGenItemRcd.GetNext(pListNode);
		}
	}

	return nCnt;
}

void CUserInfo::AllowLongHitSkill(BOOL fFlag)
{
	TCHAR	szText[64];
	char	szMsg[64];

	m_fLongHitSkill = fFlag;

	if (m_fLongHitSkill)
		LoadString(g_hInst, IDS_ALLOW_LONGHIT, szText, sizeof(szText)/sizeof(TCHAR));
	else
		LoadString(g_hInst, IDS_NOTALLOW_LONGHIT, szText, sizeof(szText)/sizeof(TCHAR));

	WideCharToMultiByte(CP_ACP, 0, szText, -1, szMsg, sizeof(szMsg), NULL, NULL);
	
	m_pxPlayerObject->SysMsg(szMsg, 1);
}

void CUserInfo::AllowWideHitSkill(BOOL fFlag)
{
	TCHAR	szText[64];
	char	szMsg[64];

	m_fWideHitSkill = fFlag;

	if (m_fWideHitSkill)
		LoadString(g_hInst, IDS_ALLOW_WIDEHIT, szText, sizeof(szText)/sizeof(TCHAR));
	else
		LoadString(g_hInst, IDS_NOTALLOW_WIDEHIT, szText, sizeof(szText)/sizeof(TCHAR));

	WideCharToMultiByte(CP_ACP, 0, szText, -1, szMsg, sizeof(szMsg), NULL, NULL);
	
	m_pxPlayerObject->SysMsg(szMsg, 1);
}

BOOL CUserInfo::AllowFireHitSkill()
{
	TCHAR	szText[128];
	char	szMsg[128];

	if (GetTickCount() - m_dwLatestFireHitTime > 10 * 1000)
	{
		m_fFireHitSkill			= TRUE;
		m_dwLatestFireHitTime	= GetTickCount();

		LoadString(g_hInst, IDS_ALLOW_FIREHIT, szText, sizeof(szText)/sizeof(TCHAR));
		WideCharToMultiByte(CP_ACP, 0, szText, -1, szMsg, sizeof(szMsg), NULL, NULL);
		
		m_pxPlayerObject->SysMsg(szMsg, 1);

		return TRUE;
	}
	else
	{
		LoadString(g_hInst, IDS_NOTALLOW_FIREHIT, szText, sizeof(szText)/sizeof(TCHAR));
		WideCharToMultiByte(CP_ACP, 0, szText, -1, szMsg, sizeof(szMsg), NULL, NULL);
		
		m_pxPlayerObject->SysMsg(szMsg, 0);
	}

	return FALSE;
}

int CUserInfo::GetGenItemStdIdx(char *pszMakeIndex)
{
	char	szStdIdx[5];

	ZeroMemory(szStdIdx, sizeof(szStdIdx));
	memmove(szStdIdx, &pszMakeIndex[1], 3);

	return AnsiStrToVal(szStdIdx);
}

int CUserInfo::GetGenItemVal1(char *pszMakeIndex)
{
	char	szVal1[5];

	ZeroMemory(szVal1, sizeof(szVal1));
	memmove(szVal1, &pszMakeIndex[4], 4);

	return AnsiStrToVal(szVal1);
}

int CUserInfo::GetGenItemVal2(char *pszMakeIndex)
{
	char	szVal2[5];

	ZeroMemory(szVal2, sizeof(szVal2));
	memmove(szVal2, &pszMakeIndex[8], 4);

	return AnsiStrToVal(szVal2);
}

int CUserInfo::CalcBagWeight()
{
	int nWeight = 0;

	if (m_lpTItemRcd.GetCount())
	{
		PLISTNODE pListNode = m_lpTItemRcd.GetHead();

		while (pListNode)
		{
			_LPTUSERITEMRCD lpTItemRcd = m_lpTItemRcd.GetData(pListNode);

			if (lpTItemRcd)
				nWeight += g_pStdItemSpecial[lpTItemRcd->nStdIndex].wWeight;

			pListNode = m_lpTItemRcd.GetNext(pListNode);
		}
	}

	if (m_lpTGenItemRcd.GetCount())
	{
		_LPTGENERALITEMRCD	lptGenItemRcd;

		PLISTNODE pListNode = m_lpTGenItemRcd.GetHead();

		while (pListNode)
		{
			lptGenItemRcd = m_lpTGenItemRcd.GetData(pListNode);

			if (lptGenItemRcd)
				nWeight += g_pStdItemEtc[lptGenItemRcd->nStdIndex].wWeight;

			pListNode = m_lpTGenItemRcd.GetNext(pListNode);
		}
	}

	return nWeight;
}

int CUserInfo::CalcWearWeightEx(int nIndex)
{
	_LPTUSERITEMRCD lptItemRcd;
	int				nWeight = 0;

	for (int i = 0; i < 8; i++)
	{
		if ((i != nIndex) && (i != U_WEAPON) && (i != U_RIGHTHAND))
		{
			lptItemRcd = GetUseItem(i);

			if (lptItemRcd)
				nWeight += g_pStdItemSpecial[lptItemRcd->nStdIndex].wWeight;
		}
	}

	return nWeight;
}

BOOL CUserInfo::UserDropItem(int nItemIndex, char *pszMakeIndex)
{
	if (!m_pxPlayerObject) return FALSE;

	if (m_lpTItemRcd.GetCount())
	{
		PLISTNODE pListNode = m_lpTItemRcd.GetHead();

		while (pListNode)
		{
			_LPTUSERITEMRCD lpTItemRcd = m_lpTItemRcd.GetData(pListNode);
		
			if (memcmp(pszMakeIndex, lpTItemRcd->szMakeIndex, _MAKEITEMINDEX) == 0)
			{
				if (m_pxPlayerObject->DropItemDown(lpTItemRcd, 2, FALSE))
				{
					m_pxPlayerObject->UpdateItemToDB(lpTItemRcd, _ITEM_ACTION_THROW);

//					delete lpTItemRcd;
					m_lpTItemRcd.RemoveNode(pListNode);

					m_pxPlayerObject->WeightChanged();

					return TRUE;
				}
			}

			pListNode = m_lpTItemRcd.GetNext(pListNode);
		}
	}

	return FALSE;
}

BOOL CUserInfo::UserDropGenItem(int nItemIndex, char *pszMakeIndex)
{
	if (!m_pxPlayerObject) return FALSE;

	if (m_lpTGenItemRcd.GetCount())
	{
		PLISTNODE pListNode = m_lpTGenItemRcd.GetHead();

		while (pListNode)
		{
			_LPTGENERALITEMRCD lpTItemRcd = m_lpTGenItemRcd.GetData(pListNode);
		
			if (memcmp(pszMakeIndex, lpTItemRcd->szMakeIndex, _MAKEITEMINDEX) == 0)
			{
				if (m_pxPlayerObject->DropItemDown((_LPTUSERITEMRCD)lpTItemRcd, 2, TRUE))
				{
//					delete lpTItemRcd;
					m_lpTGenItemRcd.RemoveNode(pListNode);

					m_pxPlayerObject->WeightChanged();

					return TRUE;
				}
			}

			pListNode = m_lpTGenItemRcd.GetNext(pListNode);
		}
	}

	return FALSE;
}

BOOL CUserInfo::IncGold(int nGold)
{
	if (m_THumanRcd.dwGold + nGold <= BAGGOLD)
	{
		m_THumanRcd.dwGold  += nGold;
		return TRUE;
	}

	return FALSE;
}

BOOL CUserInfo::DecGold(int nGold)
{
	if (m_THumanRcd.dwGold - nGold >= 0)
	{
		m_THumanRcd.dwGold  -= nGold;
		return TRUE;
	}

	return FALSE;
}

_LPTHUMANMAGICRCD CUserInfo::GetMagicRcdByKey(int nKey)
{
	PLISTNODE pListNode = m_lpTMagicRcd.GetHead();

	while (pListNode)
	{
		_LPTHUMANMAGICRCD	lptUserMagicRcd = m_lpTMagicRcd.GetData(pListNode);

		if (lptUserMagicRcd)
		{
			if (lptUserMagicRcd->btUseKey == nKey)
				return lptUserMagicRcd;
		}

		pListNode = m_lpTMagicRcd.GetNext(pListNode);
	}

	return NULL;
}

_LPTHUMANMAGICRCD CUserInfo::GetMagicRcdByID(int nID)
{
	PLISTNODE pListNode = m_lpTMagicRcd.GetHead();

	while (pListNode)
	{
		_LPTHUMANMAGICRCD	lptUserMagicRcd = m_lpTMagicRcd.GetData(pListNode);

		if (lptUserMagicRcd)
		{
			if (lptUserMagicRcd->btMagicID == nID)
				return lptUserMagicRcd;
		}

		pListNode = m_lpTMagicRcd.GetNext(pListNode);
	}

	return NULL;
}

_LPTGENERALITEMRCD CUserInfo::CanUseBujuk()
{
	_LPTGENERALITEMRCD lptItem = GetUseGenItem(m_THumanRcd.szTakeItem[U_ARMRINGL]);

	if (lptItem)
	{
		if (lptItem->szMakeIndex[0] == 'G')
		{
			int nIndex = GetGenItemStdIdx(lptItem->szMakeIndex);

			if (g_pStdItemEtc[nIndex].wStdMode == 25 || g_pStdItemEtc[nIndex].wStdMode == 5)
				return lptItem;
		}
	}

	return NULL;
}

void CUserInfo::ProcessUserMessage(char *pszPacket)
{
	_LPTDEFAULTMESSAGE lpDefMsg = (_LPTDEFAULTMESSAGE)pszPacket;

	if (m_pxPlayerObject->m_fIsCapture)
	{
		if (m_pxPlayerObject->m_hCaptureFile)
		{
			DWORD	dwWrite = 0;

			fprintf(m_pxPlayerObject->m_hCaptureFile, "%d, %d, %d, %d, %d\r\n", lpDefMsg->wIdent, lpDefMsg->nRecog, lpDefMsg->wParam, lpDefMsg->wTag, lpDefMsg->wSeries);

			switch (lpDefMsg->wIdent)
			{
				case CM_SAY:
				case CM_PICKUP:
				{
					char szDecodeMsg[512];
					int nPos = fnDecode6BitBufA(pszPacket + sizeof(_TDEFAULTMESSAGE), szDecodeMsg, sizeof(szDecodeMsg));
					szDecodeMsg[nPos] = '\0';

					fprintf(m_pxPlayerObject->m_hCaptureFile, "%s\r\n", szDecodeMsg);
				}
			}
		}
	}

	switch (lpDefMsg->wIdent)
	{
		case CM_HIT:
		case CM_POWERHIT:
		case CM_LONGHIT:
		case CM_WIDEHIT:
		case CM_HEAVYHIT:
		case CM_BIGHIT:
		case CM_FIREHIT:
			m_pxPlayerObject->AddProcess(m_pxPlayerObject, lpDefMsg->wIdent, lpDefMsg->wTag, LOWORD(lpDefMsg->nRecog), HIWORD(lpDefMsg->nRecog), lpDefMsg->wParam, NULL);
			break;
		case CM_TURN:
		case CM_WALK:
		case CM_RUN:
		case CM_SITDOWN:
		case CM_RIDE:
			m_pxPlayerObject->AddProcess(m_pxPlayerObject, lpDefMsg->wIdent, lpDefMsg->wTag, LOWORD(lpDefMsg->nRecog), HIWORD(lpDefMsg->nRecog), NULL);
			break;
		case CM_SAY:
			m_pxPlayerObject->AddProcess(m_pxPlayerObject, lpDefMsg->wIdent, 0, 0, 0, 0, pszPacket + sizeof(_TDEFAULTMESSAGE));
			break;
		case CM_SPELL:
			m_pxPlayerObject->AddProcess(m_pxPlayerObject, lpDefMsg->wIdent, 
														lpDefMsg->wTag/*MagicID*/, LOWORD(lpDefMsg->nRecog)/*TargetX*/, HIWORD(lpDefMsg->nRecog)/*TargetY*/, 
														MAKELONG(lpDefMsg->wSeries, lpDefMsg->wParam)/*TargetObj*/, NULL);
			break;
		case CM_TAKEONITEM:
		case CM_TAKEOFFITEM:
			m_pxPlayerObject->AddProcess(m_pxPlayerObject, lpDefMsg->wIdent, lpDefMsg->wSeries, lpDefMsg->nRecog, lpDefMsg->wParam, lpDefMsg->wTag, pszPacket + sizeof(_TDEFAULTMESSAGE));
			break;
        case CM_QUERYUSERNAME:
			m_pxPlayerObject->AddProcess(m_pxPlayerObject, lpDefMsg->wIdent, 0, lpDefMsg->nRecog, lpDefMsg->wParam, lpDefMsg->wTag, NULL);
			break;
		case CM_EAT:
		case CM_DROPITEM:
			m_pxPlayerObject->AddProcess(m_pxPlayerObject, lpDefMsg->wIdent, lpDefMsg->wSeries, 0, 0, 0, pszPacket + sizeof(_TDEFAULTMESSAGE));
			break;
		case CM_PICKUP:
			m_pxPlayerObject->AddProcess(m_pxPlayerObject, lpDefMsg->wIdent, lpDefMsg->wSeries, lpDefMsg->nRecog, lpDefMsg->wTag, lpDefMsg->wParam);
			break;
		default:
			m_pxPlayerObject->AddProcess(m_pxPlayerObject, lpDefMsg->wIdent, lpDefMsg->wSeries, lpDefMsg->nRecog, lpDefMsg->wTag, NULL);
	}
/*	case CM_QUERYUSERNAME:
            begin
               hum.SendMsg (hum, pmsg.Ident, 0, pmsg.Recog, pmsg.Param{x}, pmsg.Tag{y}, '');
            end;

         //string 파라메터가 있음.
	case CM_DROPITEM:
	case CM_TAKEOFFITEM:
	case CM_EXCHGTAKEONITEM:
	case CM_MERCHANTDLGSELECT:
	case CM_MERCHANTQUERYSELLPRICE:
	case CM_MERCHANTQUERYREPAIRCOST:
	case CM_USERSELLITEM:
	case CM_USERREPAIRITEM:
	case CM_USERSTORAGEITEM:
	case CM_USERBUYITEM:
	case CM_USERGETDETAILITEM:
	case CM_CREATEGROUP:
	case CM_ADDGROUPMEMBER:
	case CM_DELGROUPMEMBER:
	case CM_DEALTRY:
	case CM_DEALADDITEM:
	case CM_DEALDELITEM:
	case CM_USERTAKEBACKSTORAGEITEM:
	case CM_USERMAKEDRUGITEM:
	case CM_GUILDADDMEMBER:
	case CM_GUILDDELMEMBER:
	case CM_GUILDUPDATENOTICE:
	case CM_GUILDUPDATERANKINFO:
         :
            begin
               hum.SendMsg (hum, pmsg.Ident, pmsg.Series, pmsg.Recog, pmsg.Param, pmsg.Tag, DecodeString(body));
            end;
	case CM_ADJUST_BONUS:
         :
            begin
               hum.SendMsg (hum, pmsg.Ident, pmsg.Series, pmsg.Recog, pmsg.Param, pmsg.Tag, body);
            end;
         else
            hum.SendMsg (hum, pmsg.Ident, pmsg.Series, pmsg.Recog, pmsg.Param, pmsg.Tag, '');
      end;
	}
/*
	char				szEncodeMsg[256];

	switch (lpDefMsg->wIdent)
	{
		case CM_TURN:
		{
			if (pUserInfo->m_pxPlayerObject->TurnXY(lpDefMsg->wTag, LOWORD(lpDefMsg->nRecog), HIWORD(lpDefMsg->nRecog)))
				memmove(szEncodeMsg, _MSG_GOOD, sizeof(_MSG_GOOD));
			else
				memmove(szEncodeMsg, _MSG_FAIL, sizeof(_MSG_FAIL));

			ValToAnsiStr(GetTickCount(), &szEncodeMsg[sizeof(_MSG_GOOD) - 1]);
			pUserInfo->m_pxPlayerObject->SendSocket(NULL, szEncodeMsg);
			break;
		}
		case CM_WALK:
		{
			if (pUserInfo->m_pxPlayerObject->WalkTo(lpDefMsg->wTag))
				memmove(szEncodeMsg, _MSG_GOOD, sizeof(_MSG_GOOD));
			else
				memmove(szEncodeMsg, _MSG_FAIL, sizeof(_MSG_FAIL));

			ValToAnsiStr(GetTickCount(), &szEncodeMsg[sizeof(_MSG_GOOD) - 1]);
			pUserInfo->m_pxPlayerObject->SendSocket(NULL, szEncodeMsg);
			break;
		}
		case CM_RUN:
		{
			if (pUserInfo->m_pxPlayerObject->RunTo(lpDefMsg->wTag))
				memmove(szEncodeMsg, _MSG_GOOD, sizeof(_MSG_GOOD));
			else
				memmove(szEncodeMsg, _MSG_FAIL, sizeof(_MSG_FAIL));

			ValToAnsiStr(GetTickCount(), &szEncodeMsg[sizeof(_MSG_GOOD) - 1]);
			pUserInfo->m_pxPlayerObject->SendSocket(NULL, szEncodeMsg);
			break;
		}
/*		case CM_HIT:
		{
			if (HitXY(lpProcessMsg->wIdent, lpProcessMsg->lParam1, lpProcessMsg->lParam2, lpProcessMsg->wParam))
				memmove(szEncodeMsg, _MSG_GOOD, sizeof(_MSG_GOOD));
			else
				memmove(szEncodeMsg, _MSG_FAIL, sizeof(_MSG_FAIL));

			ValToAnsiStr(GetTickCount(), &szEncodeMsg[sizeof(_MSG_GOOD) - 1]);
			SendSocket(NULL, szEncodeMsg);
			break;
		}
		case CM_RIDE:
		{
			if (DoRideHorse(lpProcessMsg->wParam, lpProcessMsg->lParam1))
				memmove(szEncodeMsg, _MSG_GOOD, sizeof(_MSG_GOOD));
			else
				memmove(szEncodeMsg, _MSG_FAIL, sizeof(_MSG_FAIL));

			ValToAnsiStr(GetTickCount(), &szEncodeMsg[sizeof(_MSG_GOOD) - 1]);
			SendSocket(NULL, szEncodeMsg);

			break;
		}
		case CM_SAY:
		{
			if (lpProcessMsg->pszData)
				ProcessForUserSaid(lpProcessMsg);
			break;
		}
		case CM_SPELL:
		{
			if (SpellXY(lpProcessMsg->wParam, lpProcessMsg->lParam1, lpProcessMsg->lParam2, lpProcessMsg->lParam3))
				memmove(szEncodeMsg, _MSG_GOOD, sizeof(_MSG_GOOD));
			else
				memmove(szEncodeMsg, _MSG_FAIL, sizeof(_MSG_FAIL));

			ValToAnsiStr(GetTickCount(), &szEncodeMsg[sizeof(_MSG_GOOD) - 1]);
			SendSocket(NULL, szEncodeMsg);

			break;
		}
		case CM_TAKEONITEM:
		{
			lpProcessMsg->pCharObject->m_tFeature.btGender	= lpProcessMsg->wParam;
			lpProcessMsg->pCharObject->m_tFeature.btWear	= HIBYTE(lpProcessMsg->lParam1);
			lpProcessMsg->pCharObject->m_tFeature.btHair	= LOBYTE(lpProcessMsg->lParam1);
			lpProcessMsg->pCharObject->m_tFeature.btWeapon	= lpProcessMsg->lParam2;

			//AddRefMsg(
			break;
		}
	} */
}

void CUserInfo::Operate()
{
	if (GetTickCount() - m_pxPlayerObject->m_dwSearchTime >= m_pxPlayerObject->m_dwSearchTick)
	{
		m_pxPlayerObject->m_dwSearchTime = GetTickCount();
		m_pxPlayerObject->SearchViewRange();

		if (m_pxPlayerObject->m_btBright != g_nMirDayTime)
		{
			m_pxPlayerObject->m_btBright = g_nMirDayTime;
			m_pxPlayerObject->AddProcess(m_pxPlayerObject, RM_DAYCHANGING, 0, 0, 0, 0, NULL);
		}
	}

	m_pxPlayerObject->Operate();
}

void CUserInfo::CloseUserHuman()
{
	Lock();

	if (m_pxPlayerObject)
	{
		m_THumanRcd.nCX	= m_pxPlayerObject->m_nCurrX;
		m_THumanRcd.nCY	= m_pxPlayerObject->m_nCurrY;

		if (m_pxPlayerObject->m_nDirection >= 0 && m_pxPlayerObject->m_nDirection <= 7)
			m_THumanRcd.nDirection	= m_pxPlayerObject->m_nDirection;
		else
			m_THumanRcd.nDirection	= 4;

		if (m_pxPlayerObject->m_pMap)
		{
			ZeroMemory(m_THumanRcd.szMapName, sizeof(m_THumanRcd.szMapName));
			memcpy(m_THumanRcd.szMapName, m_pxPlayerObject->m_pMap->m_szMapName, memlen(m_pxPlayerObject->m_pMap->m_szMapName));
		}

		m_THumanRcd.szLevel = m_pxPlayerObject->m_Ability.Level;
		m_THumanRcd.nExp	= m_pxPlayerObject->m_Ability.Exp;
		m_THumanRcd.nExp	= m_pxPlayerObject->m_Ability.Exp;

		// Save Player Information Data
		SaveHumanToDB("192.168.0.200");
	}

	if (m_pxPlayerObject)
	{										  
		PLISTNODE pVisibleObjListNode = m_pxPlayerObject->m_xVisibleObjectList.GetHead();

		while (pVisibleObjListNode)
		{
			CVisibleObject* pVisibleObject = m_pxPlayerObject->m_xVisibleObjectList.GetData(pVisibleObjListNode);

			if (pVisibleObject) 
			{
				delete pVisibleObject;
				pVisibleObject = NULL;
			}

			pVisibleObjListNode = m_pxPlayerObject->m_xVisibleObjectList.RemoveNode(pVisibleObjListNode);
		}

		m_pxPlayerObject->m_pMap->RemoveObject(m_pxPlayerObject->m_nCurrX,m_pxPlayerObject->m_nCurrY,
												OS_MOVINGOBJECT, m_pxPlayerObject);
		
		m_pxPlayerObject->m_fIsAlive = FALSE;
	}

	PLISTNODE pListNode;

	// Remove User Item
	if (m_lpTItemRcd.GetCount())
	{
		pListNode = m_lpTItemRcd.GetHead();

		while (pListNode)
		{
			_LPTUSERITEMRCD lpUserItemRcd = m_lpTItemRcd.GetData(pListNode);

			if (lpUserItemRcd) 
			{
				delete lpUserItemRcd;
				lpUserItemRcd = NULL;
			}

			pListNode = m_lpTItemRcd.RemoveNode(pListNode);
		}
	}

	// Remove General User Item
	if (m_lpTGenItemRcd.GetCount())
	{
		pListNode = m_lpTGenItemRcd.GetHead();

		while (pListNode)
		{
			_LPTGENERALITEMRCD lpGenItemRcd = m_lpTGenItemRcd.GetData(pListNode);

			if (lpGenItemRcd) 
			{
				delete lpGenItemRcd;
				lpGenItemRcd = NULL;
			}

			pListNode = m_lpTGenItemRcd.RemoveNode(pListNode);
		}
	}

	// Remove User Magic Information
	if (m_lpTMagicRcd.GetCount())
	{
		pListNode = m_lpTMagicRcd.GetHead();

		while (pListNode)
		{
			_LPTHUMANMAGICRCD lpMagicRcd = m_lpTMagicRcd.GetData(pListNode);

			if (lpMagicRcd) 
			{
				delete lpMagicRcd;
				lpMagicRcd = NULL;
			}

			pListNode = m_lpTMagicRcd.RemoveNode(pListNode);
		}
	}

	if (m_pxPlayerObject)
		CloseAccount(m_szUserID, m_nCertification);

	m_bEmpty = true;
	Unlock();

	UpdateStatusBarUsers(FALSE);
}

void CUserInfo::SaveHumanToDB(char *pszUserAddr)
{
	_TDEFAULTMESSAGE	Def;
	_TLOADHUMAN			tLoadHuman;
	char				szEncodeMsg1[24];
	char				szEncodeMsg2[8192];

	fnMakeDefMessage(&Def, DB_SAVEHUMANRCD, (int)this, m_lpTMagicRcd.GetCount(), m_lpTGenItemRcd.GetCount(), 0);

	memcpy(tLoadHuman.szUserID, m_szUserID, memlen(m_szUserID));
	memcpy(tLoadHuman.szCharName, m_szCharName, memlen(m_szCharName));
	memcpy(tLoadHuman.szUserAddr, pszUserAddr, memlen(pszUserAddr));
	tLoadHuman.nCertification	= m_nCertification;
	
	int nPos = fnEncodeMessageA(&Def, szEncodeMsg1, sizeof(szEncodeMsg1));
	szEncodeMsg1[nPos] = '\0';
	nPos = fnEncode6BitBufA((unsigned char *)&tLoadHuman, szEncodeMsg2, sizeof(_TLOADHUMAN), sizeof(szEncodeMsg2));
	nPos += fnEncode6BitBufA((unsigned char *)&m_THumanRcd, &szEncodeMsg2[nPos], sizeof(_THUMANRCD), sizeof(szEncodeMsg2) - nPos);

	// BEGIN:Add Magic Information
	EncodeMyMagicToServer(szEncodeMsg2, sizeof(szEncodeMsg2) - nPos, nPos);
	// END

	// BEGIN:Add General Item Information
	EncodeGenItem(szEncodeMsg2, sizeof(szEncodeMsg2) - nPos, nPos);
	// END

	szEncodeMsg2[nPos] = '\0';

	SendRDBSocket(tLoadHuman.nCertification, szEncodeMsg1, szEncodeMsg2, nPos);
}

void CUserInfo::CloseAccount(char *pszName, int nCertification)
{
	char	szMsg[256];
	int		nLen = memlen(pszName) - 1;

	szMsg[0] = '%';
	szMsg[1] = 'S';

	memcpy(&szMsg[2], pszName, nLen);

	szMsg[nLen + 2] = '/';

	char *pszPos = ValToAnsiStr(nCertification, &szMsg[nLen + 3]);

	*pszPos++	= '$';
	*pszPos		= '\0';

	send(g_clsock, szMsg, memlen(szMsg) - 1, 0);
}

void CUserInfo::DoClientCertification(char *pszPacket)
{
	char szDecodePacket[64];
	char *pszDecodePacket = &szDecodePacket[0];
	char *pszPos;

	if (m_btCurrentMode == USERMODE_NOTICE)
	{
		int nLen = memlen(pszPacket);

		if (pszPos = (char *)memchr(pszPacket, '!', nLen))
		{
			*pszPos = '\0';

			//             uid  chr	 cer  ver  startnew
			// pszPacket **SSSS/SSSS/SSSS/SSSS/1
			nLen = fnDecode6BitBufA(pszPacket + 2, szDecodePacket, sizeof(szDecodePacket));
			szDecodePacket[nLen] = '\0';

			if (*pszDecodePacket == '*' && *(pszDecodePacket + 1) == '*')
			{
				pszDecodePacket += 2;

				if (!(pszPos = (char *)memchr(pszDecodePacket, '/', nLen))) return;

				*pszPos++ = '\0';
				nLen -= (pszPos - pszDecodePacket);
				memmove(m_szUserID, pszDecodePacket, (pszPos - pszDecodePacket));
				pszDecodePacket = pszPos;

				if (!(pszPos = (char *)memchr(pszDecodePacket, '/', nLen))) return;

				*pszPos++ = '\0';
				nLen -= (pszPos - pszDecodePacket);
				memmove(m_szCharName, pszDecodePacket, (pszPos - pszDecodePacket));
				pszDecodePacket = pszPos;

				if (!(pszPos = (char *)memchr(pszDecodePacket, '/', nLen))) return;

				*pszPos++ = '\0';
				nLen -= (pszPos - pszDecodePacket);
				m_nCertification = AnsiStrToVal(pszDecodePacket);
				pszDecodePacket = pszPos;

				if (!(pszPos = (char *)memchr(pszDecodePacket, '/', nLen))) return;

				*pszPos++ = '\0';
				nLen -= (pszPos - pszDecodePacket);
				m_nClientVersion = AnsiStrToVal(pszDecodePacket);
				pszDecodePacket = pszPos;

				m_btCurrentMode = USERMODE_LOGIN;
//				(*pszDecodePacket == '0' ? StartNew = TRUE : StartNew = FALSE);

				// INSERT:Check pay bill.

//				if (pUserInfo->m_nCertification >= 30)
//				{

//					LoadPlayer(pUserInfo);
//				}
	/*				else
				{
					// INSERT:Close User
				} */
			}
		}
	}
}
