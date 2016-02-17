#include "stdafx.h"

extern HWND					g_hStatusBar;
extern HANDLE				g_hIOCP;
extern SOCKET				g_ssock;

extern BOOL					g_fTerminated;

CWHList<CServerInfo*>		g_xServerList;

CWHQueue					g_DBMsgQ;
CWHQueue					g_ServerMsgQ;

void UpdateStatusBarSession(BOOL fGrow)
{
	static long	nNumOfCurrSession = 0;

	TCHAR	szText[20];

	(fGrow ? InterlockedIncrement(&nNumOfCurrSession) : InterlockedDecrement(&nNumOfCurrSession));
	
	wsprintf(szText, _TEXT("%d Sessions"), nNumOfCurrSession);

	SendMessage(g_hStatusBar, SB_SETTEXT, MAKEWORD(3, 0), (LPARAM)szText);
}

DWORD WINAPI AcceptThread(LPVOID lpParameter)
{
	int					nLen = sizeof(SOCKADDR_IN);

	SOCKET				Accept;
	SOCKADDR_IN			Address;

	DWORD				dwRecvBytes;
	DWORD				dwFlags;

	TCHAR				szGateIP[16];

	while (TRUE)
	{
		Accept = accept(g_ssock, (struct sockaddr FAR *)&Address, &nLen);

		if (g_fTerminated) return 0L;

		CServerInfo* pServerInfo = (CServerInfo*)GlobalAlloc(GPTR, sizeof(CServerInfo));

		if (pServerInfo)
		{
			pServerInfo->m_sock	= Accept;

			CreateIoCompletionPort((HANDLE)pServerInfo->m_sock, g_hIOCP, (DWORD)pServerInfo, 0);

			if (g_xServerList.AddNewNode(pServerInfo))
			{
				int zero = 0;
				
				setsockopt(pServerInfo->m_sock, SOL_SOCKET, SO_SNDBUF, (char *)&zero, sizeof(zero));

				ZeroMemory(&(pServerInfo->Overlapped), sizeof(OVERLAPPED));

				pServerInfo->DataBuf.len	= DATA_BUFSIZE;
				pServerInfo->DataBuf.buf	= pServerInfo->Buffer;
				pServerInfo->nOvFlag		= OVERLAPPED_RECV;

				dwFlags = 0;

				WSARecv(pServerInfo->m_sock, &(pServerInfo->DataBuf), 1, &dwRecvBytes, &dwFlags, &(pServerInfo->Overlapped), NULL);

				UpdateStatusBarSession(TRUE);

				_stprintf(szGateIP, _T("%d.%d.%d.%d"), Address.sin_addr.s_net, Address.sin_addr.s_host, 
															Address.sin_addr.s_lh, Address.sin_addr.s_impno);

				InsertLogMsgParam(IDS_ACCEPT_GATESERVER, szGateIP, LOGPARAM_STR);
			}
		}
	}

	return 0;
}

DWORD WINAPI ServerWorkerThread(LPVOID CompletionPortID)
{
	DWORD				dwBytesTransferred;

	CServerInfo*		pServerInfo;

	LPOVERLAPPED		lpOverlapped;

	DWORD				dwFlags;
	DWORD				dwRecvBytes;

	char				szBuff[DATA_BUFSIZE];
	int					nBuffLen;
	char				*pszFirst, *pszEnd, *pszDivide;

	LONG				lValid;
	WORD				w1, w2;
	char				szCC[32];
	int					nPos;

	while (TRUE)
	{
		GetQueuedCompletionStatus((HANDLE)CompletionPortID, &dwBytesTransferred, (LPDWORD)&pServerInfo, (LPOVERLAPPED *)&lpOverlapped, INFINITE);
		
		if (g_fTerminated) return 0L;

		if (dwBytesTransferred == 0)
		{
			closesocket(pServerInfo->m_sock);
			if (pServerInfo) GlobalFree(pServerInfo);

			UpdateStatusBarSession(FALSE);
			
			continue;
		}

		if (pServerInfo->nOvFlag == OVERLAPPED_RECV)
		{
			if (pServerInfo->nRemainBuffLen)
			{
				memmove(szBuff, pServerInfo->RemainBuff, pServerInfo->nRemainBuffLen); // Copy previous remain data in szBuff
				memmove(&szBuff[pServerInfo->nRemainBuffLen], pServerInfo->Buffer, dwBytesTransferred + 1); // Append new received data in szBuff

				nBuffLen = pServerInfo->nRemainBuffLen + dwBytesTransferred;

				pszEnd = &szBuff[0];
			}
			else
			{
				pszEnd		= &pServerInfo->Buffer[0];
				nBuffLen	= dwBytesTransferred;
			}

			while (nBuffLen)
			{
				if ((pszFirst = (char *)memchr(pszEnd, '#', nBuffLen)) && (pszEnd = (char *)memchr(pszFirst, '!', nBuffLen)))
				{
					*pszEnd++ = '\0';

					if (pszDivide = (char *)memchr(pszFirst, '/', pszEnd - pszFirst))
					{
						*pszDivide++ = '\0';

						_LPTSENDBUFF lpSendUserData = new _TSENDBUFF;

						if (lpSendUserData)
						{
							lpSendUserData->nCertification = AnsiStrToVal(pszFirst + 1);

							w1 = lpSendUserData->nCertification	^ 0xAA;
							w2 = memlen(pszDivide) - 1;
							lValid = MAKELONG(w1, w2);
							
							nPos = fnEncode6BitBufA((unsigned char *)&lValid, szCC, sizeof(LONG), sizeof(szCC));
							szCC[nPos] = '\0';

							if (memcmp((pszEnd - nPos - 1), szCC, nPos) == 0)
							{
								fnDecodeMessageA(&lpSendUserData->DefMsg, pszDivide);

								*(pszEnd - nPos - 1) = '\0';

								if (lpSendUserData->DefMsg.wIdent >= 100 && lpSendUserData->DefMsg.wIdent <= 200)
								{
									lpSendUserData->pServerInfo = pServerInfo;

									switch (lpSendUserData->DefMsg.wIdent)
									{
										case DB_MAKEITEMRCD2:
										{
											lpSendUserData->lpbtAddData = new BYTE[sizeof(_TMAKEITEMRCD)];
											fnDecode6BitBufA((pszDivide + DEFBLOCKSIZE), (char *)lpSendUserData->lpbtAddData, sizeof(_TMAKEITEMRCD));

											lpSendUserData->lpbtAddData2 = NULL;

											break;
										}
										default:
										{
											nPos = fnDecode6BitBufA((pszDivide + DEFBLOCKSIZE), (char *)&lpSendUserData->HumanLoad, sizeof(lpSendUserData->HumanLoad));
				
											switch (lpSendUserData->DefMsg.wIdent)
											{
												case DB_MAKEITEMRCD:
												{
													lpSendUserData->lpbtAddData = new BYTE[sizeof(_TMAKEITEMRCD)];
													fnDecode6BitBufA((pszDivide + DEFBLOCKSIZE + 75), (char *)lpSendUserData->lpbtAddData, sizeof(_TMAKEITEMRCD));

													lpSendUserData->lpbtAddData2 = NULL;
													break;
												}
												case DB_SAVEHUMANRCD:
												{
													lpSendUserData->lpbtAddData = new BYTE[sizeof(_THUMANRCD)];
													fnDecode6BitBufA((pszDivide + DEFBLOCKSIZE + 75), (char *)lpSendUserData->lpbtAddData, sizeof(_THUMANRCD));

													int nRemainLen = (int)(pszEnd - pszFirst) - (DEFBLOCKSIZE + 75 + HUMANRCDBLOCKSIZE);

													lpSendUserData->lpbtAddData2 = new BYTE[nRemainLen + 1];

													memcpy(lpSendUserData->lpbtAddData2, (pszDivide + DEFBLOCKSIZE + 75 + HUMANRCDBLOCKSIZE), nRemainLen);
													lpSendUserData->lpbtAddData2[nRemainLen] = '\0';

													break;
												}
												default:
													lpSendUserData->lpbtAddData		= NULL;
													lpSendUserData->lpbtAddData2	= NULL;
											}

											break;
										}
									}
									
									g_DBMsgQ.PushQ((BYTE *)lpSendUserData);
								}
								else
									g_ServerMsgQ.PushQ((BYTE *)lpSendUserData);
							}
							else
								delete lpSendUserData;
						}
					}
					
					nBuffLen -= (pszEnd - pszFirst);
				}
				else
					break;
			} // while

			if (pszFirst && nBuffLen)
			{
				memmove(pServerInfo->RemainBuff, pszFirst, nBuffLen);
				pServerInfo->nRemainBuffLen = nBuffLen;
#ifdef _DEBUG
				_RPT2(_CRT_WARN, "Remain Packet : %d, %s\n", nBuffLen, pszFirst);
#endif
			}
			else
				pServerInfo->nRemainBuffLen = 0;

			// Set next overlapped Process
			dwFlags = 0;

			ZeroMemory(&(pServerInfo->Overlapped), sizeof(OVERLAPPED));

			pServerInfo->DataBuf.len	= DATA_BUFSIZE;
			pServerInfo->DataBuf.buf	= pServerInfo->Buffer;
			pServerInfo->nOvFlag		= OVERLAPPED_RECV;

			if (WSARecv(pServerInfo->m_sock, &(pServerInfo->DataBuf), 1, &dwRecvBytes, &dwFlags, &(pServerInfo->Overlapped), NULL) == SOCKET_ERROR)
			{
				if (WSAGetLastError() != ERROR_IO_PENDING)
				{
					InsertLogMsg(_T("WSARecv() failed"));
					
//					CloseSession(pServerInfo);
					continue;
				}
			}
		} // if (OVERLAPPED_RECV)
	}

	return 0;
}
