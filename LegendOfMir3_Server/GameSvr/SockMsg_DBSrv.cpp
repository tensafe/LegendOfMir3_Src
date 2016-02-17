#include "stdafx.h"

VOID WINAPI OnTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

UINT WINAPI ProcessLogin(LPVOID lpParameter);
UINT WINAPI ProcessUserHuman(LPVOID lpParameter);
UINT WINAPI ProcessMonster(LPVOID lpParameter);
UINT WINAPI ProcessNPC(LPVOID lpParameter);

BOOL	jRegGetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, LPBYTE pValue);

char	g_szRemainBuff[DATA_BUFSIZE];
int		g_nRemainBuffLen = 0;

void ProcReceiveBuffer(char *pszPacket, int nRecv)
{
	int					nBuffLen = nRecv;
	char				szBuff[DATA_BUFSIZE];
	char				*pszFirst, *pszEnd = &szBuff[0], *pszDevide;
	_TDEFAULTMESSAGE	DefMsg;
	_TDEFAULTMESSAGE	SendDefMsg;
	int					nCertification;
	WORD				w1, w2;
	LONG				lValid;
	int					nPos;
	char				szCC[32];
	char				szEncodeMsg[256];

	CReadyUserInfo		*pReadUserInfo;
	CUserInfo			*pUserInfo;

	_TCLIENTITEMRCD		tClientItemRcd;

	if (g_nRemainBuffLen)
		memmove(szBuff, g_szRemainBuff, g_nRemainBuffLen);

	memmove(&szBuff[g_nRemainBuffLen], pszPacket, nBuffLen + 1);

	nBuffLen += g_nRemainBuffLen;

	while (nBuffLen)
	{
		if ((pszFirst = (char *)memchr(pszEnd, '#', nBuffLen)) && (pszEnd = (char *)memchr(pszFirst, '!', nBuffLen)))
		{
			*pszEnd++ = '\0';

			if (pszDevide = (char *)memchr(pszFirst, '/', pszEnd - pszFirst))
			{
				*pszDevide++ = '\0';

				nCertification = AnsiStrToVal(pszFirst + 1);

				w1 = nCertification	^ 0xAA;
				w2 = memlen(pszDevide) - 1;
				lValid = MAKELONG(w1, w2);
				
				nPos = fnEncode6BitBufA((unsigned char *)&lValid, szCC, sizeof(LONG), sizeof(szCC));
				szCC[nPos] = '\0';

				if (memcmp((pszEnd - nPos - 1), szCC, nPos) == 0)
				{
					fnDecodeMessageA(&DefMsg, pszDevide);

					*(pszEnd - nPos - 1) = '\0';

					switch (DefMsg.wIdent)
					{
						case DBR_LOADHUMANRCD2:
						{
							CReadyUserInfo2*	pReadyUserInfo2 = new CReadyUserInfo2;

							pReadyUserInfo2->m_dwReadyStartTime = GetTickCount();
							pReadyUserInfo2->m_nNumOfGenItem	= HIBYTE(DefMsg.wParam);
							pReadyUserInfo2->m_nNumOfMagic		= DefMsg.wSeries;
							pReadyUserInfo2->m_nNumOfItem		= LOBYTE(DefMsg.wParam);

							fnDecode6BitBufA((pszDevide + DEFBLOCKSIZE), (char *)&pReadyUserInfo2->m_THumanRcd, sizeof(_THUMANRCD));
							
							memmove(pReadyUserInfo2->pszData, pszDevide + DEFBLOCKSIZE + HUMANRCDBLOCKSIZE, memlen(pszDevide + DEFBLOCKSIZE + HUMANRCDBLOCKSIZE));

							g_xReadyUserInfoList2.AddNewNode(pReadyUserInfo2);

							break;
						}
						case DBR_LOADHUMANRCD:
						{
							pReadUserInfo = (CReadyUserInfo *)DefMsg.nRecog;

							nPos = fnDecode6BitBufA((pszDevide + DEFBLOCKSIZE), (char *)&pReadUserInfo->m_pUserInfo->m_THumanRcd, sizeof(_THUMANRCD));

							char *pszData;

							pszData = (pszDevide + DEFBLOCKSIZE + HUMANRCDBLOCKSIZE);

							if (HIBYTE(DefMsg.wParam))		// Decode General Item
							{
								pReadUserInfo->m_pUserInfo->m_nNumOfGenItems = HIBYTE(DefMsg.wParam);

								_TGENITEMRCD	GenItemRcd;
								char			szVal[5];						

								for (int i = 0; i < HIBYTE(DefMsg.wParam); i++)
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

										pReadUserInfo->m_pUserInfo->m_lpTGenItemRcd.AddNewNode(lptGenItemRcd);
								
										pszData += GENITEMRCDBLOCKSIZE;
									}
								}
							}								  

							if (DefMsg.wSeries)		// Decode Magic
							{
								pReadUserInfo->m_pUserInfo->m_nNumOfMagics = DefMsg.wSeries;

								for (int i = 0; i < DefMsg.wSeries; i++)
								{
									_LPTHUMANMAGICRCD lpHumanMagicRcd = new _THUMANMAGICRCD;

									if (lpHumanMagicRcd)
									{
										fnDecode6BitBufA(pszData, (char *)lpHumanMagicRcd, sizeof(_THUMANMAGICRCD));
										pReadUserInfo->m_pUserInfo->m_lpTMagicRcd.AddNewNode(lpHumanMagicRcd);
										pszData += MAGICRCDBLOCKSIZE;

//#ifdef _DEBUG
//										_RPT3(_CRT_WARN, "%d - %d - %d\n", lpHumanMagicRcd->btLevel, lpHumanMagicRcd->btUseKey, lpHumanMagicRcd->nCurrTrain);
//#endif
									}
								}
							}								  

							if (LOBYTE(DefMsg.wParam))		// Decode Item
							{
								pReadUserInfo->m_pUserInfo->m_nNumOfItems = LOBYTE(DefMsg.wParam);
		//						pReadUserInfo->m_pUserInfo->m_lpTItemRcd = (_LPTUSERITEMRCD)GlobalAlloc(GPTR, sizeof(_TUSERITEMRCD) * DefMsg.wParam);

								for (int i = 0; i < LOBYTE(DefMsg.wParam); i++)
								{
									_LPTUSERITEMRCD lpTItemRcd = new _TUSERITEMRCD;

									if (lpTItemRcd)
									{
										fnDecode6BitBufA(pszData, (char *)lpTItemRcd, sizeof(_TUSERITEMRCD));
										pReadUserInfo->m_pUserInfo->m_lpTItemRcd.AddNewNode(lpTItemRcd);
										pszData += ITEMRCDBLOCKSIZE;
									}
								}
							}

							if (DefMsg.wTag)		// Decode Horse
							{
								if (!pReadUserInfo->m_pUserInfo->m_lpTHorseRcd)
									pReadUserInfo->m_pUserInfo->m_lpTHorseRcd = new _THORSERCD;

								fnDecode6BitBufA(pszData, (char *)pReadUserInfo->m_pUserInfo->m_lpTHorseRcd, sizeof(_THORSERCD));
							}

							if (pReadUserInfo->m_pUserInfo->m_pxPlayerObject)
							{
								pReadUserInfo->m_pUserInfo->m_pxPlayerObject->MakeFeature();
								pReadUserInfo->m_pUserInfo->m_pxPlayerObject->Initialize();
								pReadUserInfo->m_pUserInfo->m_pxPlayerObject->m_fIsAlive	= TRUE;

								pReadUserInfo->m_pUserInfo->m_btCurrentMode	= USERMODE_PLAYGAME;
							}

							break;
						}
						case DBR_MAKEITEMRCD:
						{
							pUserInfo = (CUserInfo *)DefMsg.nRecog;

							_LPTUSERITEMRCD lpTItemRcd = new _TUSERITEMRCD;

							if (lpTItemRcd)
							{
								nPos = fnDecode6BitBufA((pszDevide + DEFBLOCKSIZE), (char *)lpTItemRcd, sizeof(_TUSERITEMRCD));
								
								pUserInfo->m_lpTItemRcd.AddNewNode(lpTItemRcd);

								fnMakeDefMessage(&SendDefMsg, SM_ADDITEM, (int)pUserInfo->m_pxPlayerObject, 0, 0, 1);

								if (lpTItemRcd->szMakeIndex[0] != 'G')
								{
									g_pStdItemSpecial[lpTItemRcd->nStdIndex].GetStandardItem(&tClientItemRcd);
									g_pStdItemSpecial[lpTItemRcd->nStdIndex].GetUpgradeStdItem(&tClientItemRcd, lpTItemRcd);
								}

								memcpy(tClientItemRcd.szMakeIndex, lpTItemRcd->szMakeIndex, 12);
								
								tClientItemRcd.nDura		= lpTItemRcd->nDura;
								tClientItemRcd.nDuraMax		= lpTItemRcd->nDuraMax;

								nPos = 	fnEncode6BitBufA((unsigned char *)&tClientItemRcd, szEncodeMsg, sizeof(_TCLIENTITEMRCD), sizeof(szEncodeMsg));
								szEncodeMsg[nPos] = '\0';

								pUserInfo->m_pxPlayerObject->SendSocket(&SendDefMsg, szEncodeMsg);
							}

							break;
						}
						case DBR_MAKEITEMRCD2:
						{
							CPlayerObject* pPlayerObject = (CPlayerObject *)DefMsg.nRecog;
							
							pUserInfo = pPlayerObject->m_pUserInfo;
							
							_LPTUSERITEMRCD lpTItemRcd = new _TUSERITEMRCD;

							if (lpTItemRcd)
							{
								nPos = fnDecode6BitBufA((pszDevide + DEFBLOCKSIZE), (char *)lpTItemRcd, sizeof(_TUSERITEMRCD));
								
								pUserInfo->m_lpTItemRcd.AddNewNode(lpTItemRcd);

								fnMakeDefMessage(&SendDefMsg, SM_ADDITEM, (int)pUserInfo->m_pxPlayerObject, 0, 0, 1);

								if (lpTItemRcd->szMakeIndex[0] != 'G')
								{
									g_pStdItemSpecial[lpTItemRcd->nStdIndex].GetStandardItem(&tClientItemRcd);
									g_pStdItemSpecial[lpTItemRcd->nStdIndex].GetUpgradeStdItem(&tClientItemRcd, lpTItemRcd);
								}

								memcpy(tClientItemRcd.szMakeIndex, lpTItemRcd->szMakeIndex, 12);
								
								tClientItemRcd.nDura		= lpTItemRcd->nDura;
								tClientItemRcd.nDuraMax		= lpTItemRcd->nDuraMax;

								nPos = 	fnEncode6BitBufA((unsigned char *)&tClientItemRcd, szEncodeMsg, sizeof(_TCLIENTITEMRCD), sizeof(szEncodeMsg));
								szEncodeMsg[nPos] = '\0';

								pUserInfo->m_pxPlayerObject->SendSocket(&SendDefMsg, szEncodeMsg);
							}

							break;
						}
					}
				}
			}

			nBuffLen -= (pszEnd - pszFirst);
		}
		else
			break;
	} // while

	if (pszFirst && (nBuffLen > 0))
	{
		memmove(g_szRemainBuff, pszFirst, nBuffLen);
		g_nRemainBuffLen = nBuffLen;
#ifdef _DEBUG
	_RPT2(_CRT_WARN, "ProcReceiveBuffer:%d, %s\n", g_nRemainBuffLen, pszFirst);
#endif
	}
	else
		g_nRemainBuffLen = 0;
}

VOID WINAPI OnProcessUserHuman(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

LPARAM OnClientSockMsg(WPARAM wParam, LPARAM lParam)
{
	switch (WSAGETSELECTEVENT(lParam))
	{
		case FD_CONNECT:
		{
			if (CheckSocketError(lParam))
			{
				InsertLogMsg(IDS_CONNECT_DBSERVER);

				KillTimer(g_hMainWnd, _ID_TIMER_CONNECTSERVER);

				UINT			dwThreadIDForMsg = 0;
				unsigned long	hThreadForMsg = 0;

//				SetTimer(g_hMainWnd, _ID_TIMER_PROCESSUSERHUMAN, 1, (TIMERPROC)OnProcessUserHuman);
				if (hThreadForMsg = _beginthreadex(NULL, 0, ProcessLogin, NULL, 0, &dwThreadIDForMsg))
				{
//					SetThreadPriority((HANDLE)hThreadForMsg, THREAD_PRIORITY_HIGHEST);
					hThreadForMsg = _beginthreadex(NULL, 0, ProcessUserHuman, NULL, 0, &dwThreadIDForMsg);
					hThreadForMsg = _beginthreadex(NULL, 0, ProcessMonster, NULL, 0, &dwThreadIDForMsg);
					hThreadForMsg = _beginthreadex(NULL, 0, ProcessNPC, NULL, 0, &dwThreadIDForMsg);
				}

				int nPort;

				if (!jRegGetKey(_GAME_SERVER_REGISTRY, _TEXT("LocalPort"), (LPBYTE)&nPort))
					nPort = 5000;

				InitServerSocket(g_ssock, &g_saddr, _IDM_SERVERSOCK_MSG, nPort, 1);

				InsertLogMsg(IDS_STARTSERVICE);
				SendMessage(g_hStatusBar, SB_SETTEXT, MAKEWORD(0, 0), (LPARAM)_T("Ready"));
			}
			else
			{
				closesocket(g_csock);
				g_csock = INVALID_SOCKET;

				SetTimer(g_hMainWnd, _ID_TIMER_CONNECTSERVER, 10000, (TIMERPROC)OnTimerProc);
			}

			break;
		}
		case FD_CLOSE:
		{
			closesocket(g_csock);
			g_csock = INVALID_SOCKET;

			InsertLogMsg(IDS_DISCONNECT_DBSERVER);

			break;
		}
		case FD_READ:
		{
			char	szPacket[8096];
	
			int nRecv = recv((SOCKET)wParam, szPacket, sizeof(szPacket), 0);

#ifdef _DEBUG
			_RPT1(_CRT_WARN, "ClientSockMsg[FD_READ]:%d\n", nRecv);
#endif
			szPacket[nRecv] = '\0';

			ProcReceiveBuffer(szPacket, nRecv);

			break;
		}
	}

	return 0L;
}
