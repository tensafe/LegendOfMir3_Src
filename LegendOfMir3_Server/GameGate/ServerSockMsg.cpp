#include "stdafx.h"

extern HWND			g_hStatusBar;
	
extern SOCKET		g_ssock;
extern SOCKET		g_csock;

extern HANDLE		g_hIOCP;

CWHDynamicArray<CSessionInfo>	g_UserInfoArray;
CWHQueue						g_SendToServerQ;

void UpdateStatusBar(BOOL fGrow)
{
	static long	nNumOfCurrSession = 0;

	TCHAR	szText[20];

	(fGrow ? InterlockedIncrement(&nNumOfCurrSession) : InterlockedDecrement(&nNumOfCurrSession));
	
	wsprintf(szText, _TEXT("%d Sessions"), nNumOfCurrSession);

	SendMessage(g_hStatusBar, SB_SETTEXT, MAKEWORD(3, 0), (LPARAM)szText);
}

void SendSocketMsgS (_LPTMSGHEADER lpMsg, int nLen1, char *pszData1, int nLen2, char *pszData2)
{
	char		szBuf[1024];

	WSABUF		Buf;
	DWORD		dwSendBytes;

	memmove(szBuf, lpMsg, sizeof(_TMSGHEADER));

	if (pszData1)
		memmove(&szBuf[sizeof(_TMSGHEADER)], pszData1, nLen1);

	if (pszData2)
		memmove(&szBuf[sizeof(_TMSGHEADER) + nLen1], pszData2, nLen2);

	szBuf[sizeof(_TMSGHEADER) + nLen1 + nLen2] = '\0';

	Buf.len = sizeof(_TMSGHEADER) + nLen1 + nLen2;
	Buf.buf = szBuf;

	WSASend(g_csock, &Buf, 1, &dwSendBytes, 0, NULL, NULL);

//	send(g_csock, szBuf, sizeof(_TMSGHEADER) + nLen1 + nLen2, 0);
//#ifdef _DEBUG
//	_RPT4(_CRT_WARN, "SS:%d %d %d %d\t", lpMsg->wUserGateIndex, lpMsg->wIdent, lpMsg->wUserListIndex, lpMsg->nLength);
//	_RPT1(_CRT_WARN, "%s\n", szBuf);
//#endif
}

void SendSocketMsgS (int nIdent, WORD wIndex, int nSocket, WORD wSrvIndex, int nLen, char *pszData)
{
	_TMSGHEADER	msg;
	char		szBuf[1024];

	WSABUF		Buf;
	DWORD		dwSendBytes;

	msg.nCode			= 0xAA55AA55;
	msg.nSocket			= nSocket;
	msg.wUserGateIndex	= wIndex;
	msg.wIdent			= (WORD)nIdent;
	msg.wUserListIndex	= wSrvIndex;
	msg.nLength			= nLen;

	memmove(szBuf, &msg, sizeof(_TMSGHEADER));

	if (pszData)
		memmove(&szBuf[sizeof(_TMSGHEADER)], pszData, nLen);

	szBuf[sizeof(_TMSGHEADER) + nLen] = '\0';

	Buf.len = sizeof(_TMSGHEADER) + nLen;
	Buf.buf = szBuf;

	WSASend(g_csock, &Buf, 1, &dwSendBytes, 0, NULL, NULL);
//	send(g_csock, szBuf, sizeof(_TMSGHEADER) + nLen, 0);

//#ifdef _DEBUG
//	_RPT4(_CRT_WARN, "SS:%d %d %d %d \t", msg.wUserGateIndex, msg.wIdent, msg.wUserListIndex, msg.nLength);
//	_RPT1(_CRT_WARN, "%s\n", szBuf);
//#endif
}

//UINT WINAPI AcceptThread(LPVOID lpParameter)
DWORD WINAPI AcceptThread(LPVOID lpParameter)
{
	int							nLen = sizeof(SOCKADDR_IN);
	char						szMsg[64];
	TCHAR						szAddress[24];

	int							nCvtLen;

	SOCKET						Accept;
	SOCKADDR_IN					Address;

//	DWORD						dwRecvBytes;
//	DWORD						dwFlags;

	while (TRUE)
	{
		Accept = accept(g_ssock, (struct sockaddr FAR *)&Address, &nLen);

		if (g_fTerminated)
			return 0;

		int nIndex = g_UserInfoArray.GetAvailablePosition();

		if (nIndex >= 0)
		{
			CSessionInfo* pNewSessionInfo = g_UserInfoArray.GetEmptyElement(nIndex);

			pNewSessionInfo->sock				= Accept;
			pNewSessionInfo->nGateIndex			= nIndex;
			// Initializing Session Information
			pNewSessionInfo->nCrackWarningLevel	= 0;
			pNewSessionInfo->fLoginCode			= TRUE;

			pNewSessionInfo->nSendBufferLen		= 0;

			CreateIoCompletionPort((HANDLE)Accept, g_hIOCP, (DWORD)pNewSessionInfo, 0);

			pNewSessionInfo->Recv();

			UpdateStatusBar(TRUE);

			// Make packet and send to login server.
			wsprintf(szAddress, _TEXT("%d.%d.%d.%d"), Address.sin_addr.s_net, Address.sin_addr.s_host, 
															Address.sin_addr.s_lh, Address.sin_addr.s_impno);

			nCvtLen = WideCharToMultiByte(CP_ACP, 0, szAddress, -1, szMsg, sizeof(szMsg), NULL, NULL);

			SendSocketMsgS(GM_OPEN, pNewSessionInfo->nGateIndex, (int)pNewSessionInfo->sock, 0, nCvtLen + 1, szMsg);
		}
	}

	return 0;
}

void CloseSession(CSessionInfo* pSessionInfo)
{
	g_UserInfoArray.SetEmptyElement(pSessionInfo->nGateIndex, pSessionInfo);

	closesocket(pSessionInfo->sock);

	UpdateStatusBar(FALSE);
}

DWORD WINAPI ServerWorkerThread(LPVOID CompletionPortID)
{
	DWORD					dwBytesTransferred;
//	DWORD					dwFlags;
//	DWORD					dwRecvBytes;
//	DWORD					dwSendBytes;

	CSessionInfo*			pSessionInfo;
	LPOVERLAPPED			lpOverlapped;
//	char					*pszFirst, *pszEnd;

//	CHAR					szBuff[DATA_BUFSIZE + 1];
//	INT						nBuffLen;

	while (TRUE)
	{
		GetQueuedCompletionStatus((HANDLE)CompletionPortID, &dwBytesTransferred, (LPDWORD)&pSessionInfo, 
										(LPOVERLAPPED *)&lpOverlapped, INFINITE);

		if (dwBytesTransferred == 0)
		{
			SendSocketMsgS(GM_CLOSE, pSessionInfo->nGateIndex, pSessionInfo->sock, pSessionInfo->nServerUserIndex, 0, NULL);
			CloseSession(pSessionInfo);
			continue;
		}

		if (pSessionInfo->nOvFlag == OVERLAPPED_RECV)
		{
			pSessionInfo->bufLen += dwBytesTransferred;

			while ( pSessionInfo->HasCompletionPacket() )
			{
				_LPTSENDBUFF pSendData = new _TSENDBUFF;

				if ( !pSendData )
					break;

				pSendData->sock				= pSessionInfo->sock;
				pSendData->nGateIndex		= pSessionInfo->nGateIndex;

				*(pSessionInfo->ExtractPacket( pSendData->szData )) = '\0';

				g_SendToServerQ.Lock();
				if ( !g_SendToServerQ.PushQ( (BYTE *) pSendData ) )
				{
					InsertLogMsg( _TEXT("PushQ() failed") );
					delete pSendData;
				}
				g_SendToServerQ.Unlock();
			}

			if ( pSessionInfo->Recv() == SOCKET_ERROR && WSAGetLastError() != ERROR_IO_PENDING )
			{
				InsertLogMsg(_TEXT("WSARecv() failed"));
				CloseSession(pSessionInfo);
				continue;
			}
		}
	}

	return 0L;
}
