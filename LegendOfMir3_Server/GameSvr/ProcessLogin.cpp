#include "stdafx.h"

CMirMap* GetMap(char *pszMapName);

void SendRDBSocket(int nCertification, char *pszData1, char *pszData2, int nData2Len)
{
	char	szEncodeMsg[8192];
	char	szEncode[24];
	int		nLen = nData2Len + DEFBLOCKSIZE + 6;
	LONG	lCert = MAKELONG(nCertification ^ 0xAA, nLen);
	
	int nCertLen = fnEncode6BitBufA((unsigned char *)&lCert, szEncode, sizeof(LONG), sizeof(szEncode));

	// Make Packet
	szEncodeMsg[0] = '#';
	char *pszPos = ValToAnsiStr(nCertification, &szEncodeMsg[1]);

	*pszPos++ = '/';

	memmove(pszPos, pszData1, DEFBLOCKSIZE);
	pszPos += DEFBLOCKSIZE;
	memmove(pszPos, pszData2, nData2Len);
	pszPos += nData2Len;
	memmove(pszPos, szEncode, nCertLen);

	pszPos		+= nCertLen;

	*pszPos++	= '!';
	*pszPos		= '\0';

	send(g_csock, szEncodeMsg, memlen(szEncodeMsg) - 1, 0);
}

void LoadHumanFromDB(int nReadyUserInfo, char *pszUserID, char *pszCharName, char *pszUserAddr, int nCertification)
{
	_TDEFAULTMESSAGE	Def;
	_TLOADHUMAN			tLoadHuman;
	char				szEncodeMsg1[24];
	char				szEncodeMsg2[128];

	fnMakeDefMessage(&Def, DB_LOADHUMANRCD, nReadyUserInfo, 0, 0, 0);

	memcpy(tLoadHuman.szUserID, pszUserID, memlen(pszUserID));
	memcpy(tLoadHuman.szCharName, pszCharName, memlen(pszCharName));
	memcpy(tLoadHuman.szUserAddr, pszUserAddr, memlen(pszUserAddr));
	tLoadHuman.nCertification	= nCertification;
	
	int nPos = fnEncodeMessageA(&Def, szEncodeMsg1, sizeof(szEncodeMsg1));
	szEncodeMsg1[nPos] = '\0';
	nPos = fnEncode6BitBufA((unsigned char *)&tLoadHuman, szEncodeMsg2, sizeof(_TLOADHUMAN), sizeof(szEncodeMsg2));
	szEncodeMsg2[nPos] = '\0';

	SendRDBSocket(nCertification, szEncodeMsg1, szEncodeMsg2, nPos);
}

void MakeItemToDB(int nReadyUserInfo, char *pszUserId, char *pszCharName, _LPTMAKEITEMRCD lpMakeItemRcd)
{
	_TDEFAULTMESSAGE	Def;
	_TLOADHUMAN			tLoadHuman;
	char				szEncodeMsg1[24];
	char				szEncodeMsg2[128];

	fnMakeDefMessage(&Def, DB_MAKEITEMRCD, nReadyUserInfo, 0, 0, 0);

	memcpy(tLoadHuman.szUserID, pszUserId, memlen(pszUserId));
	memcpy(tLoadHuman.szCharName, pszCharName, memlen(pszCharName));
	
	int nPos = fnEncodeMessageA(&Def, szEncodeMsg1, sizeof(szEncodeMsg1));
	szEncodeMsg1[nPos] = '\0';
	nPos = fnEncode6BitBufA((unsigned char *)&tLoadHuman, szEncodeMsg2, sizeof(_TLOADHUMAN), sizeof(szEncodeMsg2));
	nPos += fnEncode6BitBufA((unsigned char *)lpMakeItemRcd, &szEncodeMsg2[nPos], sizeof(_TMAKEITEMRCD), sizeof(szEncodeMsg2) - nPos);
	szEncodeMsg2[nPos] = '\0';

	SendRDBSocket(2, szEncodeMsg1, szEncodeMsg2, nPos);
}

BOOL LoadPlayer(CReadyUserInfo2* pReadyUserInfo, CUserInfo* pUserInfo)
{
	int nIndex = g_xPlayerObjectArr.GetFreeKey();

	if (nIndex >= 0)
	{
		pUserInfo->Lock();
		pUserInfo->m_btCurrentMode	= USERMODE_PLAYGAME;
		pUserInfo->m_pxPlayerObject = &g_xPlayerObjectArr[nIndex];
		
		memcpy(&pUserInfo->m_THumanRcd, &pReadyUserInfo->m_THumanRcd, sizeof(pUserInfo->m_THumanRcd));

		pUserInfo->Unlock();

		pUserInfo->m_pxPlayerObject->Lock();
		pUserInfo->m_pxPlayerObject->m_bEmpty = false;
		pUserInfo->m_pxPlayerObject->m_nArrIndex = nIndex;
		pUserInfo->m_pxPlayerObject->m_pUserInfo = pUserInfo;
		
		char* pszData = &pReadyUserInfo->pszData[0];

		if (pReadyUserInfo->m_nNumOfGenItem)		// Decode General Item
		{
			pUserInfo->m_nNumOfGenItems = pReadyUserInfo->m_nNumOfGenItem;

			_TGENITEMRCD	GenItemRcd;
			char			szVal[5];						

			for (int i = 0; i < pReadyUserInfo->m_nNumOfGenItem; i++)
			{
				_LPTGENERALITEMRCD lptGenItemRcd = new _TGENERALITEMRCD;

				if (lptGenItemRcd)
				{
					fnDecode6BitBufA(pszData, (char *)&GenItemRcd, sizeof(_TGENITEMRCD));
					
					memcpy(lptGenItemRcd->szMakeIndex, GenItemRcd.szItem, 12);

					ZeroMemory(szVal, sizeof(szVal));

					memcpy(szVal, &lptGenItemRcd->szMakeIndex[1], 3);
					lptGenItemRcd->nStdIndex	= AnsiStrToVal(szVal);
					memcpy(szVal, &lptGenItemRcd->szMakeIndex[4], 4);
					lptGenItemRcd->nDura		= AnsiStrToVal(szVal);
					memcpy(szVal, &lptGenItemRcd->szMakeIndex[8], 4);
					lptGenItemRcd->nDuraMax		= AnsiStrToVal(szVal);

					pUserInfo->m_lpTGenItemRcd.AddNewNode(lptGenItemRcd);
			
					pszData += GENITEMRCDBLOCKSIZE;
				}
			}
		}								  

		if (pReadyUserInfo->m_nNumOfMagic)		// Decode Magic
		{
			pUserInfo->m_nNumOfMagics = pReadyUserInfo->m_nNumOfMagic;

			for (int i = 0; i < pReadyUserInfo->m_nNumOfMagic; i++)
			{
				_LPTHUMANMAGICRCD lpHumanMagicRcd = new _THUMANMAGICRCD;

				if (lpHumanMagicRcd)
				{
					fnDecode6BitBufA(pszData, (char *)lpHumanMagicRcd, sizeof(_THUMANMAGICRCD));
					pUserInfo->m_lpTMagicRcd.AddNewNode(lpHumanMagicRcd);
					pszData += MAGICRCDBLOCKSIZE;
				}
			}
		}								  

		if (pReadyUserInfo->m_nNumOfItem)		// Decode Item
		{
			pUserInfo->m_nNumOfItems = pReadyUserInfo->m_nNumOfItem;

			for (int i = 0; i < pReadyUserInfo->m_nNumOfItem; i++)
			{
				_LPTUSERITEMRCD lpTItemRcd = new _TUSERITEMRCD;

				if (lpTItemRcd)
				{
					fnDecode6BitBufA(pszData, (char *)lpTItemRcd, sizeof(_TUSERITEMRCD));
					pUserInfo->m_lpTItemRcd.AddNewNode(lpTItemRcd);
					pszData += ITEMRCDBLOCKSIZE;
				}
			}
		}

/*		if (DefMsg.wTag)		// Decode Horse
		{
			if (!pReadUserInfo->m_pUserInfo->m_lpTHorseRcd)
				pReadUserInfo->m_pUserInfo->m_lpTHorseRcd = new _THORSERCD;

			fnDecode6BitBufA(pszData, (char *)pReadUserInfo->m_pUserInfo->m_lpTHorseRcd, sizeof(_THORSERCD));
		}
*/
		pUserInfo->m_pxPlayerObject->MakeFeature();
		pUserInfo->m_pxPlayerObject->Initialize();
		pUserInfo->m_pxPlayerObject->m_fIsAlive	= TRUE;
	
		pUserInfo->m_pxPlayerObject->Unlock();

		g_xUserInfoList.AddNewNode(pUserInfo);

		return TRUE;
	}

	return FALSE;
}

void LoadPlayer(CUserInfo* pUserInfo)
{
	CReadyUserInfo*		pReadyUserInfo = new CReadyUserInfo;

	if (pReadyUserInfo)
	{
		pReadyUserInfo->m_pUserInfo			= pUserInfo;

		int nIndex = g_xPlayerObjectArr.GetFreeKey();

		if (nIndex >= 0)
		{
			pUserInfo->m_pxPlayerObject = &g_xPlayerObjectArr[nIndex];
			pUserInfo->m_pxPlayerObject->Lock();
			pUserInfo->m_pxPlayerObject->m_bEmpty		= false;
			pUserInfo->m_pxPlayerObject->m_nArrIndex	= nIndex;
			pUserInfo->m_pxPlayerObject->m_pUserInfo	= pUserInfo;

			LoadHumanFromDB((int)pReadyUserInfo, pUserInfo->m_szUserID, pUserInfo->m_szCharName, "192.168.0.200", pUserInfo->m_nCertification);
		   	pUserInfo->m_pxPlayerObject->Unlock();

			g_xReadyUserInfoList.AddNewNode(pReadyUserInfo);
		}
	}
}

UINT WINAPI ProcessLogin(LPVOID lpParameter)
{
	PLISTNODE			pReadyListNode = NULL;
	CReadyUserInfo2*	pReadyUserInfo = NULL;
	BOOL				fRemoveFlag = FALSE;

	while (TRUE)
	{
		if (g_fTerminated) return 0L;

		if (g_xLoginOutUserInfo.GetCount())
		{
			PLISTNODE pListNode = g_xLoginOutUserInfo.GetHead();

			while (pListNode)
			{
				CUserInfo *pUserInfo = g_xLoginOutUserInfo.GetData(pListNode);

				if (pUserInfo)
				{
					pUserInfo->Lock();

					switch (pUserInfo->m_btCurrentMode)
					{
						case USERMODE_LOGOFF:
						{
							pUserInfo->CloseUserHuman();

							pListNode = g_xLoginOutUserInfo.RemoveNode(pListNode);

							continue;
						}
						case USERMODE_LOGIN:
						{
							pReadyListNode = g_xReadyUserInfoList2.GetHead();

							while (pReadyListNode)
							{
								pReadyUserInfo = g_xReadyUserInfoList2.GetData(pReadyListNode);

								if ((strcmp(pReadyUserInfo->m_THumanRcd.szUserID, pUserInfo->m_szUserID) == 0) &&
									(strcmp(pReadyUserInfo->m_THumanRcd.szCharName, pUserInfo->m_szCharName) == 0))
								{
									if (LoadPlayer(pReadyUserInfo, pUserInfo))
									{
										delete pReadyUserInfo;
										pReadyUserInfo = NULL;

										g_xReadyUserInfoList2.RemoveNode(pReadyListNode);
									
										fRemoveFlag = TRUE;

										break;
									}
								}

								pListNode = g_xReadyUserInfoList2.GetNext(pReadyListNode);
							}

							break;
						}
						case USERMODE_NOTICE:
						{
							break;
						}
					}

					pUserInfo->Unlock();
				}

				if (fRemoveFlag)
				{
					pListNode = g_xLoginOutUserInfo.RemoveNode(pListNode);
					fRemoveFlag = FALSE;
				}
				else
					pListNode = g_xLoginOutUserInfo.GetNext(pListNode);
			}
		}

		SleepEx(1, TRUE);
	}

	return 0L;
}
