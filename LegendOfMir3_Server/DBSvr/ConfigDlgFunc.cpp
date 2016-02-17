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

			TCHAR	szDatabase[256];

			ZeroMemory(szDatabase, sizeof(szDatabase));

			jRegGetKey(_DB_SERVER_REGISTRY, _TEXT("Device"), (LPBYTE)szDatabase);
			SetWindowText(GetDlgItem(hWndDlg, IDC_DBMS_DEVICE), szDatabase);

			break;
		}
		case WM_COMMAND:
		{
			switch (wParam)
			{
				case IDOK:
				{
					TCHAR	szDatabase[256];

					BYTE	btInstalled = 1;

					jRegSetKey(_DB_SERVER_REGISTRY, _TEXT("Installed"), REG_BINARY, (LPBYTE)&btInstalled, sizeof(BYTE));

					GetWindowText(GetDlgItem(hWndDlg, IDC_DBMS_DEVICE), szDatabase, sizeof(szDatabase));
					jRegSetKey(_DB_SERVER_REGISTRY, _TEXT("Device"), REG_SZ, (LPBYTE)szDatabase, sizeof(szDatabase)/sizeof(TCHAR));
				}
				case IDCANCEL:
					return EndDialog(hWndDlg, IDCANCEL);
			}

			break;
		}
	}

	return FALSE;
}
