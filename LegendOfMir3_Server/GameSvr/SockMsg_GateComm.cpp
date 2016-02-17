#include "stdafx.h"

void UpdateStatusBarSession(BOOL fGrow)
{
	static long	nNumOfCurrSession = 0;

	TCHAR	szText[20];

	(fGrow ? InterlockedIncrement(&nNumOfCurrSession) : InterlockedDecrement(&nNumOfCurrSession));
	
	wsprintf(szText, _TEXT("%d Sessions"), nNumOfCurrSession);

	SendMessage(g_hStatusBar, SB_SETTEXT, MAKEWORD(1, 0), (LPARAM)szText);
}

void UpdateStatusBarUsers(BOOL fGrow)
{
	static long	nNumOfUsers = 0;

	TCHAR	szText[20];

	(fGrow ? InterlockedIncrement(&nNumOfUsers) : InterlockedDecrement(&nNumOfUsers));
	
	wsprintf(szText, _TEXT("%d Users"), nNumOfUsers);

	SendMessage(g_hStatusBar, SB_SETTEXT, MAKEWORD(2, 0), (LPARAM)szText);
}

DWORD WINAPI AcceptThread(LPVOID lpParameter)
{
	int					nLen = sizeof(SOCKADDR_IN);

	SOCKET				Accept;
	SOCKADDR_IN			Address;

	TCHAR				szGateIP[16];

	while (TRUE)
	{
		nLen	= sizeof( Address );
		Accept	= WSAAccept( g_ssock, (SOCKADDR *) &Address, &nLen, NULL, 0 );

		if (g_fTerminated) return 0L;

		CGateInfo* pGateInfo = new CGateInfo;

		if (pGateInfo)
		{
			pGateInfo->m_sock			= Accept;

			CreateIoCompletionPort((HANDLE)pGateInfo->m_sock, g_hIOCP, (DWORD)pGateInfo, 0);

			if (g_xGateList.AddNewNode(pGateInfo))
			{
				int zero = 0;
				
				setsockopt(pGateInfo->m_sock, SOL_SOCKET, SO_SNDBUF, (char *)&zero, sizeof(zero) );

				ZeroMemory(&(pGateInfo->OverlappedEx), sizeof(OVERLAPPED) * 2);

				pGateInfo->OverlappedEx[1].nOvFlag		= OVERLAPPED_SEND;

				pGateInfo->Recv();

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
	CGateInfo*			pGateInfo;
	LPOVERLAPPEDEX		lpOverlapped;
	_LPTMSGHEADER		pMsgHeader;
	char				completionPacket[DATA_BUFSIZE];

	while (TRUE)
	{
		GetQueuedCompletionStatus((HANDLE)CompletionPortID, &dwBytesTransferred, (LPDWORD)&pGateInfo, (LPOVERLAPPED *)&lpOverlapped, INFINITE);
		
		if (g_fTerminated) return 0L;

		if (dwBytesTransferred == 0)
		{
			if (g_xUserInfoList.GetCount())
			{
				PLISTNODE pListNode = g_xUserInfoList.GetHead();

				while (pListNode)
				{
					CUserInfo *pUserInfo = g_xUserInfoList.GetData(pListNode);

					if (pUserInfo->m_pGateInfo == pGateInfo)
					{
						pUserInfo->Lock();
						pUserInfo->m_bEmpty = true;
						pUserInfo->Unlock();
						
						// °íÃÄ¾ß µÊ
						if (pUserInfo->m_pxPlayerObject)
							pUserInfo->m_pxPlayerObject->m_pMap->RemoveObject(pUserInfo->m_pxPlayerObject->m_nCurrX,
																				pUserInfo->m_pxPlayerObject->m_nCurrY,
																				OS_MOVINGOBJECT,
																				pUserInfo->m_pxPlayerObject);
						
						pListNode = g_xUserInfoList.RemoveNode(pListNode);

						UpdateStatusBarUsers(FALSE);
					}
					else
						pListNode = g_xUserInfoList.GetNext(pListNode);
				}
			}

			closesocket(pGateInfo->m_sock);
			g_xGateList.RemoveNodeByData(pGateInfo);

			if (pGateInfo) delete pGateInfo;
			
			continue;
		}

		if (lpOverlapped->nOvFlag == OVERLAPPED_RECV)
		{
			static DWORD nLastTick = GetTickCount();
			static DWORD nBytes = 0;

			nBytes += dwBytesTransferred;

			if ( GetTickCount() - nLastTick >= 1000)
			{
				TCHAR buf[256];
				wsprintf( buf, _T("R: %d bytes/sec"), nBytes );

				nLastTick = GetTickCount();
				nBytes = 0;

				SendMessage(g_hStatusBar, SB_SETTEXT, MAKEWORD(3, 0), (LPARAM)buf);
			}	

			pGateInfo->OverlappedEx[0].bufLen += dwBytesTransferred;

			while ( pGateInfo->HasCompletionPacket() )
			{
				*(pGateInfo->ExtractPacket( completionPacket )) = '\0';

				pMsgHeader = (_LPTMSGHEADER) completionPacket;

				if ( pMsgHeader->nCode != 0xAA55AA55 )
					continue;

				switch ( pMsgHeader->wIdent )
				{
					case GM_OPEN:
					{
						pGateInfo->OpenNewUser( completionPacket );
						break;
					}
					case GM_CLOSE:
					{
						CUserInfo *pUserInfo = &g_xUserInfoArr[ pMsgHeader->wUserListIndex ];

						if (pUserInfo)
						{
							pUserInfo->m_btCurrentMode = USERMODE_LOGOFF;
							g_xLoginOutUserInfo.AddNewNode(pUserInfo);
						}

						break;
					}
					case GM_CHECKCLIENT:
					{
						pGateInfo->SendGateCheck();
						break;
					}
					case GM_RECEIVE_OK:
					{
						break;
					}
					case GM_DATA:
					{
						CUserInfo *pUserInfo = &g_xUserInfoArr[ pMsgHeader->wUserListIndex ];

						if ( !pUserInfo->IsEmpty() )
						{
							if ( pUserInfo->m_btCurrentMode == USERMODE_PLAYGAME)
							{
								if ( pMsgHeader->nSocket == pUserInfo->m_sock )
									pUserInfo->ProcessUserMessage(completionPacket + sizeof( _TMSGHEADER ) );
							}
							else
							{
								pUserInfo->Lock();
								pUserInfo->DoClientCertification( completionPacket + sizeof( _TMSGHEADER ) + sizeof(_TDEFAULTMESSAGE) );
								pUserInfo->Unlock();
							}
						}

						break;
					}
				} // switch
			} // while
	
			if ( pGateInfo->Recv() == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING )
				InsertLogMsg( _T("WSARecv() failed") );
		} // if (OVERLAPPED_RECV)
		else if (lpOverlapped->nOvFlag == OVERLAPPED_SEND)
		{
			static DWORD nLastTick = GetTickCount();
			static DWORD nBytes = 0;

			nBytes += dwBytesTransferred;

			if ( GetTickCount() - nLastTick >= 1000)
			{
				TCHAR buf[256];
				wsprintf( buf, _T("S: %d bytes/sec"), nBytes );

				nLastTick = GetTickCount();
				nBytes = 0;

				SendMessage(g_hStatusBar, SB_SETTEXT, MAKEWORD(4, 0), (LPARAM)buf);
			}	
/*			fprintf( fp, "sended\r\n" );

			pGateInfo->m_fDoSending = FALSE;
			
			if ( pGateInfo->Send( NULL ) == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING )
				InsertLogMsg( _T("WSASend() failed") );	*/
		}
	}

	return 0;
}
