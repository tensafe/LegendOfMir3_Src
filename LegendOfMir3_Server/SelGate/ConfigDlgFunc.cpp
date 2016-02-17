#include "stdafx.h"

BOOL jRegSetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, DWORD dwFlags, LPBYTE pValue, DWORD nValueSize);
BOOL jRegGetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, LPBYTE pValue);

extern HWND		g_hMainWnd;

BOOL CALLBACK ConfigDlgFunc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			RECT rcMainWnd, rcDlg;

			GetWindowRect(g_hMainWnd, &rcMainWnd);
			GetWindowRect(hWndDlg, &rcDlg);

			MoveWindow(hWndDlg, rcMainWnd.left + (((rcMainWnd.right - rcMainWnd.left) - (rcDlg.right - rcDlg.left)) / 2), 
						rcMainWnd.top + (((rcMainWnd.bottom - rcMainWnd.top) - (rcDlg.bottom - rcDlg.top)) / 2), 
						(rcDlg.right - rcDlg.left), (rcDlg.bottom - rcDlg.top), FALSE);

			SendMessage(GetDlgItem(hWndDlg, IDC_LOGINSVR_PORT), EM_LIMITTEXT, (WPARAM)5, (LPARAM)0L);
			SendMessage(GetDlgItem(hWndDlg, IDC_LOCALPORT), EM_LIMITTEXT, (WPARAM)5, (LPARAM)0L);

			DWORD	dwIP = 0;
			TCHAR	szPort[24];
			int		nPort = 0;

			jRegGetKey(_SELGATE_SERVER_REGISTRY, _TEXT("RemoteIP"), (LPBYTE)&dwIP);
			SendMessage(GetDlgItem(hWndDlg, IDC_LOGINSVR_IP), IPM_SETADDRESS, (WPARAM)0, (LPARAM)(DWORD)dwIP);

			jRegGetKey(_SELGATE_SERVER_REGISTRY, _TEXT("RemotePort"), (LPBYTE)&nPort);
			_itow(nPort, szPort, 10);
			SetWindowText(GetDlgItem(hWndDlg, IDC_LOGINSVR_PORT), szPort);
			
			jRegGetKey(_SELGATE_SERVER_REGISTRY, _TEXT("LocalPort"), (LPBYTE)&nPort);
			_itow(nPort, szPort, 10);
			SetWindowText(GetDlgItem(hWndDlg, IDC_LOCALPORT), szPort);

			break;
		}
		case WM_COMMAND:
		{
			switch (wParam)
			{
				case IDOK:
				{
					DWORD	dwIP = 0;
					TCHAR	szPort[24];
					int		nRemotePort = 0, nLocalPort = 0;
					BYTE	btInstalled = 1;

					jRegSetKey(_SELGATE_SERVER_REGISTRY, _TEXT("Installed"), REG_BINARY, (LPBYTE)&btInstalled, sizeof(BYTE));

					SendMessage(GetDlgItem(hWndDlg, IDC_LOGINSVR_IP), IPM_GETADDRESS, (WPARAM)0, (LPARAM)(LPDWORD)&dwIP);
					
					jRegSetKey(_SELGATE_SERVER_REGISTRY, _TEXT("RemoteIP"), REG_DWORD, (LPBYTE)&dwIP, sizeof(DWORD));

					GetWindowText(GetDlgItem(hWndDlg, IDC_LOGINSVR_PORT), szPort, sizeof(szPort));
					nRemotePort = _wtoi(szPort);
					GetWindowText(GetDlgItem(hWndDlg, IDC_LOCALPORT), szPort, sizeof(szPort));
					nLocalPort = _wtoi(szPort);

					jRegSetKey(_SELGATE_SERVER_REGISTRY, _TEXT("RemotePort"), REG_DWORD, (LPBYTE)&nRemotePort, sizeof(DWORD));
					jRegSetKey(_SELGATE_SERVER_REGISTRY, _TEXT("LocalPort"), REG_DWORD, (LPBYTE)&nLocalPort, sizeof(DWORD));
				}
				case IDCANCEL:
					return EndDialog(hWndDlg, IDCANCEL);
			}
		}
	}

	return FALSE;
}
