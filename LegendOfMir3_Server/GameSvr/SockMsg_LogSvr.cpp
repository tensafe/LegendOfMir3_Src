#include "stdafx.h"

BOOL	jRegGetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, LPBYTE pValue);

LPARAM OnLogSvrSockMsg(WPARAM wParam, LPARAM lParam)
{
	switch (WSAGETSELECTEVENT(lParam))
	{
		case FD_CONNECT:
		{
			break;
		}
		case FD_CLOSE:
		{
			closesocket(g_clsock);
			g_clsock = INVALID_SOCKET;

			break;
		}
		case FD_READ:
		{
			char	szPacket[1024];
	
			int nRecv = recv((SOCKET)wParam, szPacket, sizeof(szPacket), 0);

			szPacket[nRecv] = '\0';

			break;
		}
	}

	return 0L;
}
