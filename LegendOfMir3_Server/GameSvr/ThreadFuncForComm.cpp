#include "stdafx.h"

BOOL	jRegGetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, LPBYTE pValue);

VOID WINAPI OnTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime)
{
	switch (idEvent)
	{
		case _ID_TIMER_CONNECTSERVER:
		{
			if (g_csock == INVALID_SOCKET)
			{
				DWORD	dwIP = 0;
				int		nPort = 0;
				TCHAR	szPort[24];

				InsertLogMsg(IDS_APPLY_RECONNECT);

				jRegGetKey(_GAME_SERVER_REGISTRY, _TEXT("DBServerIP"), (LPBYTE)&dwIP);
				jRegGetKey(_GAME_SERVER_REGISTRY, _TEXT("DBServerPort"), (LPBYTE)&nPort);
				_itow(nPort, szPort, 10);

				ConnectToServer(g_csock, &g_caddr, _IDM_CLIENTSOCK_MSG, NULL, dwIP, nPort, FD_CONNECT|FD_READ|FD_CLOSE);
			}

			break;
		}
	}
}
