#include "stdafx.h"

BOOL jRegSetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, DWORD dwFlags, LPBYTE pValue, DWORD nValueSize);
BOOL jRegGetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, LPBYTE pValue);

extern HWND		g_hMainWnd;

BOOL CALLBACK AddSvrListProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			CenterDialog(g_hMainWnd, hWndDlg);

			break;
		}
		case WM_COMMAND:
		{
			switch (wParam)
			{
				case IDC_ADD_GATELIST:
				{

					break;
				}
				case IDC_REMOVE_GATELIST:
				{
					int nSel = SendMessage(GetDlgItem(hWndDlg, IDC_GATELIST), LB_GETCURSEL, (WPARAM)0, (LPARAM)0L);

					if (nSel != LB_ERR)
						SendMessage(GetDlgItem(hWndDlg, IDC_GATELIST), LB_DELETESTRING, (WPARAM)nSel, (LPARAM)0L);

					break;
				}
				case IDOK:
				{
					TCHAR	szKey[64];
					TCHAR	szSubKey[32];
					TCHAR	szTitle[64];
					TCHAR	szGateList[32];
					DWORD	dwPrivateIP = 0, dwPublicIP = 0;
					UINT	nGateCount = 0;

					ZeroMemory(szKey, sizeof(szKey));
					ZeroMemory(szSubKey, sizeof(szSubKey));
					ZeroMemory(szTitle, sizeof(szTitle));
					ZeroMemory(szGateList, sizeof(szGateList));

					GetWindowText(GetDlgItem(hWndDlg, IDC_TITLE), szTitle, sizeof(szTitle));

					if (lstrlen(szTitle))
					{
						lstrcpy(szKey, _LOGIN_SERVER_REGISTRY);
						lstrcat(szKey, _T("\\Servers\\"));
						lstrcat(szKey, szTitle);

						jRegSetKey(szKey, _TEXT("Title"), REG_SZ, (LPBYTE)szTitle, _tcslen(szTitle) * sizeof(TCHAR));
						
						if (SendMessage(GetDlgItem(hWndDlg, IDC_IPADDRESS1), IPM_ISBLANK, (WPARAM)0, (LPARAM)0L) ||
							SendMessage(GetDlgItem(hWndDlg, IDC_IPADDRESS2), IPM_ISBLANK, (WPARAM)0, (LPARAM)0L))
						{
							MessageBox(hWndDlg, _TEXT("ERROR"), _T("Configuration"), MB_OK|MB_ICONSTOP);
							return FALSE;
						}

						SendMessage(GetDlgItem(hWndDlg, IDC_IPADDRESS1), IPM_GETADDRESS, (WPARAM)0, (LPARAM)(LPDWORD)&dwPrivateIP);
						SendMessage(GetDlgItem(hWndDlg, IDC_IPADDRESS2), IPM_GETADDRESS, (WPARAM)0, (LPARAM)(LPDWORD)&dwPublicIP);

						jRegSetKey(szKey, _TEXT("PrivateIP"), REG_DWORD, (LPBYTE)&dwPrivateIP, sizeof(DWORD));
						jRegSetKey(szKey, _TEXT("PublicIP"), REG_DWORD, (LPBYTE)&dwPublicIP, sizeof(DWORD));

						nGateCount = SendMessage(GetDlgItem(hWndDlg, IDC_GATELIST), LB_GETCOUNT, (WPARAM)0, (LPARAM)(LPDWORD)0L);

						jRegSetKey(szKey, _TEXT("NumberOfGate"), REG_DWORD, (LPBYTE)&nGateCount, sizeof(UINT));

						if (SendMessage(GetDlgItem(hWndDlg, IDC_IPADDRESS3), IPM_ISBLANK, (WPARAM)0, (LPARAM)0L) == 0)
						{
							DWORD	dwIP = 0;
							TCHAR	szPort[10];

							ZeroMemory(szPort, sizeof(szPort));

							SendMessage(GetDlgItem(hWndDlg, IDC_IPADDRESS3), IPM_GETADDRESS, (WPARAM)0, (LPARAM)(LPDWORD)&dwIP);
							
							GetWindowText(GetDlgItem(hWndDlg, IDC_PORT), szPort, sizeof(szPort));

							wsprintf(szGateList, _TEXT("%d.%d.%d.%d:%s"), FIRST_IPADDRESS(dwIP), SECOND_IPADDRESS(dwIP), 
															THIRD_IPADDRESS(dwIP), FOURTH_IPADDRESS(dwIP), szPort);

							jRegSetKey(szKey, _TEXT("Gate"), REG_SZ, (LPBYTE)szGateList, _tcslen(szGateList) * sizeof(TCHAR));
						}
					}
				}
				case IDCANCEL:
					return EndDialog(hWndDlg, IDCANCEL);
			}
		}
	}

	return FALSE;
}
