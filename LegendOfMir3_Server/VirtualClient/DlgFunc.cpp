#include "stdafx.h"
#include "resource.h"

void InsertLogMsg(LPTSTR lpszMsg);

extern HINSTANCE		g_hInst;
extern HWND				g_hMainWnd;

extern SOCKET			g_sock;

void SendPacket(char *lpszPacket, ...)
{
	static int	nCode = 0;
	char		szSendBuff[2048];
	char		*pszPacket;

	if (nCode >= 9)
		nCode = 0;
	else
		nCode++;

	wsprintf(szSendBuff, "#%d%s", nCode, lpszPacket);

	va_list	vaPacket;

	va_start(vaPacket, lpszPacket);

	while (TRUE)
	{
		pszPacket = (char *)va_arg(vaPacket, char *);

		if (!pszPacket)
			break;

		lstrcat(szSendBuff, pszPacket);
	}

	va_end(vaPacket);

	int nLen = lstrlen(szSendBuff);

	szSendBuff[nLen] = '!';
	szSendBuff[nLen + 1] = '\0';

	InsertLogMsg(szSendBuff);

	send(g_sock, szSendBuff, lstrlen(szSendBuff), 0);
}

BOOL CALLBACK LoginFunc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
			CenterDialog(g_hMainWnd, hWndDlg);
			break;
		case WM_COMMAND:
		{
			switch (wParam)
			{
				case IDOK:
				case IDCANCEL:
					return EndDialog(hWndDlg, IDCANCEL);
			}
		}
	}

	return FALSE;
}

BOOL CALLBACK NewUserFunc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
			CenterDialog(g_hMainWnd, hWndDlg);
			break;
		case WM_COMMAND:
		{
			switch (wParam)
			{
				case IDOK:
				case IDCANCEL:
					return EndDialog(hWndDlg, IDCANCEL);
			}
		}
	}

	return FALSE;
}
