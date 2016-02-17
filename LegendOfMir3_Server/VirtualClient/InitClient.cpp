#include "stdafx.h"

void InsertLogMsg(LPTSTR lpszMsg);

extern HINSTANCE		g_hInst;
extern HWND				g_hMainWnd;

BOOL ConnectServer(SOCKET &s, SOCKADDR_IN* addr, UINT nMsgID, LPCTSTR lpServerIP, DWORD dwIP, int nPort, long lEvent)
{
	if (s != INVALID_SOCKET)
	{
		closesocket(s);
		s = INVALID_SOCKET;
	}

	s = socket(AF_INET, SOCK_STREAM, 0);

	addr->sin_family	= AF_INET;
	addr->sin_port		= htons(nPort);
	
	if (lpServerIP)
		addr->sin_addr.s_addr	= inet_addr(lpServerIP);
	else
	{
		DWORD dwReverseIP = 0;

		dwReverseIP = (LOBYTE(LOWORD(dwIP)) << 24) | (HIBYTE(LOWORD(dwIP)) << 16) | (LOBYTE(HIWORD(dwIP)) << 8) | (HIBYTE(HIWORD(dwIP)));

		addr->sin_addr.s_addr	= dwReverseIP;
	}

	if (WSAAsyncSelect(s, g_hMainWnd, nMsgID, lEvent) == SOCKET_ERROR)
		return FALSE;

	if (connect(s, (const struct sockaddr FAR*)addr, sizeof(SOCKADDR_IN)) == SOCKET_ERROR)
	{
		if (WSAGetLastError() == WSAEWOULDBLOCK)
			return TRUE;
	}

	return FALSE;
}

LRESULT OnSockMsg(WPARAM wParam, LPARAM lParam)
{
	switch (WSAGETSELECTEVENT(lParam))
	{
		case FD_CONNECT:
		{
			InsertLogMsg(_T("로그인 게이트 서버와 접속 되었습니다."));

			break;
		}
		case FD_CLOSE:
		{
			break;
		}
		case FD_READ:
		{
			char	szMsg[4096];

			int nLen = recv((SOCKET)wParam, szMsg, sizeof(szMsg), 0);

			szMsg[nLen] = '\0';

			InsertLogMsg(szMsg);

			break;
		}
	}

	return 0;
}
