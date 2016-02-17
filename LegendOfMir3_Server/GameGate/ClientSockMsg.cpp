#include "stdafx.h"

DWORD WINAPI	ThreadFuncForMsg(LPVOID lpParameter);
BOOL			CheckSocketError(LPARAM lParam);
VOID WINAPI		OnTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

UINT WINAPI		ClientWorkerThread(LPVOID lpParameter);

extern SOCKET					g_csock;

extern HWND						g_hMainWnd;
extern HWND						g_hStatusBar;

extern CWHDynamicArray<CSessionInfo>	g_UserInfoArray;


HANDLE							g_hThreadForComm = NULL;
HANDLE							g_hSvrMsgEvnt = NULL;

char							g_szRemainBuff[DATA_BUFSIZE * 2];
int								g_nRemainBuffLen = 0;

//CWHQueue						g_SendToUserQ;

WSAEVENT						g_ClientIoEvent;

void ProcMakeSocketStr(char *lpMsg)
{
//	_LPTSENDBUFF	lpSendUserData = new _TSENDBUFF;
	int				nPos = 0;

	_LPTMSGHEADER	lpMsgHeader = (_LPTMSGHEADER)lpMsg;

	CSessionInfo* pSessionInfo = g_UserInfoArray.GetData(lpMsgHeader->wUserGateIndex);
	
	if (!pSessionInfo)
	{
#ifdef _DEBUG
		_RPT0(_CRT_WARN, "Session Not Found\n");
#endif
		
		return;
	}

	char *pszData = &pSessionInfo->SendBuffer[pSessionInfo->nSendBufferLen];

	if (lpMsgHeader->nLength < 0)
	{
//		wsprintf(szPacket, _TEXT("#%s!"), (lpMsg + sizeof(_TMSGHEADER))); // Walk
		pszData[0]		= '#';
		nPos = abs(lpMsgHeader->nLength);//memlen(lpMsg + sizeof(_TMSGHEADER));
		memmove(&pszData[1], (lpMsg + sizeof(_TMSGHEADER)), nPos);
		pszData[++nPos] = '!';
		pszData[++nPos] = '\0';

//		nPos = lstrlen(szPacket);
//		nPos += 2;
	}
	else
	{
		if (lpMsgHeader->nLength >= sizeof(_TDEFAULTMESSAGE))
		{
			_LPTDEFAULTMESSAGE lpDefMsg = (_LPTDEFAULTMESSAGE)(lpMsg + sizeof(_TMSGHEADER));

			pszData[0] = '#';

			if (lpMsgHeader->nLength > sizeof(_TDEFAULTMESSAGE))
			{
				nPos = fnEncodeMessageA(lpDefMsg, &pszData[1], sizeof(pSessionInfo->SendBuffer) - pSessionInfo->nSendBufferLen - 1);
//				nPos = fnEncodeMessage((char *)lpDefMsg + sizeof(_TDEFAULTMESSAGE) , &szPacket[nPos + 1], sizeof(szPacket) - nPos - 1);
				memmove(&pszData[nPos + 1], (lpMsg + sizeof(_TMSGHEADER) + sizeof(_TDEFAULTMESSAGE)), lpMsgHeader->nLength - sizeof(_TDEFAULTMESSAGE));

				nPos += lpMsgHeader->nLength - sizeof(_TDEFAULTMESSAGE);
			}
			else
			{
				nPos = fnEncodeMessageA(lpDefMsg, &pszData[1], sizeof(pSessionInfo->SendBuffer) - pSessionInfo->nSendBufferLen - 1);
				nPos++;
			}

			pszData[nPos] = '!';
			pszData[++nPos] = '\0';
		}
	}

	pSessionInfo->nSendBufferLen += nPos;

//	lpSendUserData->sock = (SOCKET)lpMsgHeader->nSocket;

//	g_SendToUserQ.PushQ((BYTE *)lpSendUserData);

/*	WSABUF	Buf;
	DWORD	dwBytesSends;

	Buf.len = nPos;
	Buf.buf = szPacket;

	WSASend((SOCKET)lpMsgHeader->nSocket, &Buf, 1, &dwBytesSends, 0, NULL, NULL); */
}

void ProcReceiveBuffer(char *pszPacket, int nRecv)
{
	int				nLen = nRecv;
	int				nNext = 0;
	char			szBuff[DATA_BUFSIZE];
	char			*pszData = &szBuff[0];
	_LPTMSGHEADER	lpMsgHeader;

	if (g_nRemainBuffLen > 0)
		memmove(szBuff, g_szRemainBuff, g_nRemainBuffLen);

	memmove(&szBuff[g_nRemainBuffLen], pszPacket, nLen + 1);

	nLen += g_nRemainBuffLen;

	while (nLen >= sizeof(_TMSGHEADER))
	{
		lpMsgHeader = (_LPTMSGHEADER)pszData;

		if (nLen < (int)(sizeof(_TMSGHEADER) + lpMsgHeader->nLength)) break;

		if (lpMsgHeader->nCode == 0xAA55AA55)
		{
			switch (lpMsgHeader->wIdent)
			{
				case GM_CHECKSERVER:
					SendMessage(g_hStatusBar, SB_SETTEXT, MAKEWORD(2, 0), (LPARAM)_TEXT("Activation"));	// Received keep alive check code from game server 
					break;
				case GM_SERVERUSERINDEX:
				{
					CSessionInfo* pSessionInfo = g_UserInfoArray.GetData(lpMsgHeader->wUserGateIndex);

					if (pSessionInfo)
						pSessionInfo->nServerUserIndex = lpMsgHeader->wUserListIndex;

					break;
				}
				case GM_RECEIVE_OK:
					SendSocketMsgS(GM_RECEIVE_OK, 0, 0, 0, 0, NULL);
					break;
				case GM_DATA:
					ProcMakeSocketStr(pszData);
					break;
				case GM_TEST:
					break;
			}

			pszData += sizeof(_TMSGHEADER) + abs(lpMsgHeader->nLength);
			nLen -= sizeof(_TMSGHEADER) + abs(lpMsgHeader->nLength);
		}
		else
		{
			pszData++;
			nLen--;
		}
	} // while

	if (nLen > 0)
	{
		memmove(g_szRemainBuff, pszData, nLen);
		g_nRemainBuffLen = nLen;
#ifdef _DEBUG
	_RPT2(_CRT_WARN, "REMAIN:%d, %s\n", g_nRemainBuffLen, pszData);
#endif
	}
	else
	{
		g_nRemainBuffLen = 0;
	}
}

/*
BOOL InitServerThreadForComm()
{
	DWORD	dwThreadIDForComm = 0;

	if (!g_hSvrMsgEvnt)
		g_hSvrMsgEvnt = CreateEvent(NULL, FALSE, FALSE, NULL);

	if (!g_hThreadForComm)
	{
		g_hThreadForComm	= CreateThread(NULL, 0, ThreadFuncForComm,	NULL, 0, &dwThreadIDForComm);

		if (g_hThreadForComm)
			return TRUE;
	}

	return FALSE;
}
*/
BOOL InitServerThreadForMsg()
{
	DWORD	dwThreadIDForMsg = 0;

	HANDLE hThreadForMsg	= CreateThread(NULL, 0, ThreadFuncForMsg,	NULL, 0, &dwThreadIDForMsg);

	if (hThreadForMsg)
	{
		CloseHandle(hThreadForMsg);
	
		return TRUE;
	}

	return FALSE;
}

LPARAM OnClientSockMsg(WPARAM wParam, LPARAM lParam)
{
	switch (WSAGETSELECTEVENT(lParam))
	{
		case FD_CONNECT:
		{
			if (CheckSocketError(lParam))
			{
				if (InitServerThreadForMsg())
				{
					g_nRemainBuffLen = 0;

					KillTimer(g_hMainWnd, _ID_TIMER_CONNECTSERVER);
					
					SetTimer(g_hMainWnd, _ID_TIMER_KEEPALIVE, 50000, (TIMERPROC)OnTimerProc);
//					SetTimer(g_hMainWnd, _ID_TIMER_KEEPALIVE, 1000, (TIMERPROC)OnTimerProc);

					InsertLogMsg(IDS_CONNECT_LOGINSERVER);
					SendMessage(g_hStatusBar, SB_SETTEXT, MAKEWORD(1, 0), (LPARAM)_TEXT("Connected"));

					//
					UINT			dwThreadIDForMsg = 0;
					unsigned long	hThreadForMsg = 0;

					g_ClientIoEvent = WSACreateEvent();

					hThreadForMsg = _beginthreadex(NULL, 0, ClientWorkerThread, NULL, 0, &dwThreadIDForMsg);
				}
			}
			else
			{
				closesocket(g_csock);
				g_csock = INVALID_SOCKET;

				SetTimer(g_hMainWnd, _ID_TIMER_CONNECTSERVER, 10000, (TIMERPROC)OnTimerProc);
			}

			break;
		}
/*		case FD_CLOSE:
		{
			closesocket(g_csock);
			g_csock = INVALID_SOCKET;

			KillTimer(g_hMainWnd, _ID_TIMER_KEEPALIVE);
			
			SetTimer(g_hMainWnd, _ID_TIMER_CONNECTSERVER, 10000, (TIMERPROC)OnTimerProc);

			InsertLogMsg(IDS_DISCONNECT_LOGINSERVER);
			SendMessage(g_hStatusBar, SB_SETTEXT, MAKEWORD(1, 0), (LPARAM)_TEXT("Not Connected"));

			break;
		}
		case FD_READ:
		{
			char	szPacket[1024];
	
			int nRecv = recv((SOCKET)wParam, szPacket, sizeof(szPacket), 0);

			szPacket[nRecv] = '\0';

			ProcReceiveBuffer(szPacket, nRecv);

			break;
		}
*/	}

	return 0L;
}

UINT WINAPI		ClientWorkerThread(LPVOID lpParameter)
{
	_TOVERLAPPEDEX		ClientOverlapped;
	DWORD				dwIndex;
	DWORD				dwBytesTransferred;
	DWORD				dwFlags;
	DWORD				dwRecvBytes;
		
//	char				*pszPos;
//	int					nSocket;

	ZeroMemory(&ClientOverlapped.Overlapped, sizeof(WSAOVERLAPPED));

	ClientOverlapped.Overlapped.hEvent = g_ClientIoEvent;

	ClientOverlapped.DataBuf.len	= DATA_BUFSIZE;
	ClientOverlapped.DataBuf.buf	= &ClientOverlapped.Buffer[0];

	dwFlags = 0;

	if (WSARecv(g_csock, &(ClientOverlapped.DataBuf), 1, &dwRecvBytes, &dwFlags, &(ClientOverlapped.Overlapped), NULL) == SOCKET_ERROR)
	{
		if (WSAGetLastError() != ERROR_IO_PENDING)
			return 0;		
	}

	while (TRUE)
	{
		dwIndex = WSAWaitForMultipleEvents(1, &g_ClientIoEvent, FALSE, WSA_INFINITE, FALSE);

		WSAResetEvent(g_ClientIoEvent);

		WSAGetOverlappedResult(g_csock, &ClientOverlapped.Overlapped, &dwBytesTransferred, FALSE, &dwFlags);

		if (dwBytesTransferred == 0)
			break;

		ClientOverlapped.DataBuf.buf[dwBytesTransferred] = '\0';

/*#ifdef _DEBUG
		_RPT1(_CRT_WARN, "%d:", dwBytesTransferred);

		for (int i = 0; i < dwBytesTransferred; i++)
			_RPT1(_CRT_WARN, "%c", ClientOverlapped.DataBuf.buf[i]);
		_RPT0(_CRT_WARN, "\n");
#endif*/

		ProcReceiveBuffer(ClientOverlapped.DataBuf.buf, dwBytesTransferred);

		dwFlags = 0;

		ZeroMemory(&(ClientOverlapped.Overlapped), sizeof(OVERLAPPED));

		ClientOverlapped.DataBuf.len = DATA_BUFSIZE;
		ClientOverlapped.DataBuf.buf = &ClientOverlapped.Buffer[0];

		ClientOverlapped.Overlapped.hEvent = g_ClientIoEvent;

		if (WSARecv(g_csock, &(ClientOverlapped.DataBuf), 1, &dwRecvBytes, &dwFlags, &(ClientOverlapped.Overlapped), NULL) == SOCKET_ERROR)
		{
			if (WSAGetLastError() != ERROR_IO_PENDING)
				break;
		}
	}

	return 0L;
}
