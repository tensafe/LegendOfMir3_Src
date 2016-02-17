#include "stdafx.h"

extern HWND			g_hMainWnd;
extern HWND			g_hStatusBar;

CWHList<CGateInfo*>	g_xGateInfoList;

// **************************************************************************************
//
//			
//
// **************************************************************************************

void UpdateStatusBarGateSession(BOOL fGrow)
{
	static long	nNumOfCurrGateSession = 0;

	TCHAR	szText[20];

	(fGrow ? InterlockedIncrement(&nNumOfCurrGateSession) : InterlockedDecrement(&nNumOfCurrGateSession));
	
	wsprintf(szText, _TEXT("%d Sessions"), nNumOfCurrGateSession);

	SendMessage(g_hStatusBar, SB_SETTEXT, MAKEWORD(4, 0), (LPARAM)szText);
}

BOOL InitGateCommSocket(SOCKET &s, SOCKADDR_IN* addr, UINT nMsgID, int nPort, long lEvent)
{
	if (s == INVALID_SOCKET)
	{
		s = socket(AF_INET, SOCK_STREAM, 0);

		addr->sin_family		= AF_INET;
		addr->sin_port			= htons(nPort);
		addr->sin_addr.s_addr	= htonl(INADDR_ANY);

		if ((bind(s, (const struct sockaddr FAR*)addr, sizeof(SOCKADDR_IN))) == SOCKET_ERROR)
			return FALSE;

		if ((listen(s, 5)) == SOCKET_ERROR)
			return FALSE;

		if ((WSAAsyncSelect(s, g_hMainWnd, nMsgID, lEvent)) == SOCKET_ERROR)
			return FALSE;
	}
	else 
		return FALSE;

	return TRUE;
}


LPARAM OnGateCommSockMsg(WPARAM wParam, LPARAM lParam)
{
	switch (WSAGETSELECTEVENT(lParam))
	{
		case FD_ACCEPT:
		{
			CGateInfo* pGateInfo = new CGateInfo;

			if (pGateInfo)
			{
				pGateInfo->sock = accept(wParam, (struct sockaddr FAR *)NULL, NULL);

				WSAAsyncSelect(pGateInfo->sock, g_hMainWnd, _IDM_GATECOMMSOCK_MSG, FD_READ|FD_CLOSE);
			
				if (g_xGateInfoList.AddNewNode(pGateInfo))
				{
					int zero = 0;
					
					setsockopt(pGateInfo->sock, SOL_SOCKET, SO_SNDBUF, (char *)&zero, sizeof(zero) );

					UpdateStatusBarGateSession(TRUE);
				}
			}

			break;
		}
		case FD_CLOSE:
		{
			UpdateStatusBarGateSession(FALSE);
			break;
		}
		case FD_READ:
		{
			int	 nSocket = 0;
			char szTmp[DATA_BUFSIZE];
			UINT nRecv = 0;

			CGateInfo*	pGateInfo;
			PLISTNODE pListNode = g_xGateInfoList.GetHead();

			while (pListNode)
			{
				pGateInfo = (CGateInfo*)g_xGateInfoList.GetData(pListNode);

				if (pGateInfo->sock == (SOCKET)wParam)
					break;

				pListNode = g_xGateInfoList.GetNext(pListNode);
			}

			nRecv = pGateInfo->Recv();

			if ( nRecv <= 0 )
				break;

			pGateInfo->bufLen += nRecv;

			while ( pGateInfo->HasCompletionPacket() )
			{
				memset( szTmp, 0, sizeof( szTmp ) );
				*(pGateInfo->ExtractPacket( szTmp ) - 1) = '\0';

				switch ( szTmp[1] )
				{
					case '-':
						pGateInfo->SendKeepAlivePacket();
						break;
					case 'A':
						pGateInfo->ReceiveSendUser(&szTmp[2]);
						break;
					case 'O':
						pGateInfo->ReceiveOpenUser(&szTmp[2]);
						break;
					case 'X':
						pGateInfo->ReceiveCloseUser(&szTmp[2]);
						break;
				}
			}

			break;
		}
	}

	return 0L;
}
