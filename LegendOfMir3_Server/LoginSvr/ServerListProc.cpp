#include "stdafx.h"

extern HINSTANCE	g_hInst;
extern HWND			g_hMainWnd;

BOOL jRegGetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, LPBYTE pValue);
LONG jRegEnumKey(LPCTSTR pSubKeyName, int nIndex, LPTSTR lpSubKey, int nSubkeyLen);

BOOL CALLBACK AddSvrListProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

void InitListView(HWND hWndDlg)
{
	LV_ITEM			lvi;
	TCHAR			szText[64];
	UINT			nCount = 0;

	TCHAR			szKey[128];
	TCHAR			szTitle[64];
	DWORD			dwPrivateIP = 0, dwPublicIP = 0;
	UINT			nNumberOfGate = 0;

	lvi.mask		= LVIF_TEXT;
	lvi.iItem		= nCount;
	lvi.iSubItem	= 0;

	ZeroMemory(szText, sizeof(szText));

	while (jRegEnumKey(_LOGIN_SERVERLIST_REGISTRY, nCount, szText, sizeof(szText)) != ERROR_NO_MORE_ITEMS)
	{				
		lvi.pszText = szText;
		ListView_InsertItem(GetDlgItem(hWndDlg, IDC_SERVERINFO_LIST), &lvi);

		lstrcpy(szKey, _LOGIN_SERVERLIST_REGISTRY);
		lstrcat(szKey, _TEXT("\\"));
		lstrcat(szKey, szText);

		jRegGetKey(szKey, _T("Title"), (LPBYTE)szTitle);

		ListView_SetItemText(GetDlgItem(hWndDlg, IDC_SERVERINFO_LIST), nCount, 1, szTitle);

		jRegGetKey(szKey, _T("PrivateIP"), (LPBYTE)dwPrivateIP);
		wsprintf(szText, _TEXT("%d.%d.%d.%d"), FIRST_IPADDRESS(dwPrivateIP), SECOND_IPADDRESS(dwPrivateIP), 
											THIRD_IPADDRESS(dwPrivateIP), FOURTH_IPADDRESS(dwPrivateIP)); 
		ListView_SetItemText(GetDlgItem(hWndDlg, IDC_SERVERINFO_LIST), nCount, 2, szText);

		jRegGetKey(szKey, _T("PublicIP"), (LPBYTE)dwPublicIP);
		wsprintf(szText, _TEXT("%d.%d.%d.%d"), FIRST_IPADDRESS(dwPublicIP), SECOND_IPADDRESS(dwPublicIP), 
											THIRD_IPADDRESS(dwPublicIP), FOURTH_IPADDRESS(dwPublicIP)); 
		ListView_SetItemText(GetDlgItem(hWndDlg, IDC_SERVERINFO_LIST), nCount, 2, szText);

		nCount++;
	}

/*		jRegGetKey(szKey, _T("NumberOfGate"), REG_DWORD, (LPBYTE)&nGateCount, sizeof(UINT));

		jRegSetKey(szKey, szSubKey, REG_SZ, (LPBYTE)szGateList, _tcslen(szGateList));
*/
}

BOOL CALLBACK ServerListProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	static PROPSHEETPAGE *ps;

	switch (uMsg)
	{
		case WM_INITDIALOG:
		{
			ps = (PROPSHEETPAGE *)lParam;

			LV_COLUMN	lvc;
			TCHAR		szText[64];

			lvc.mask	= LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
			lvc.fmt		= LVCFMT_LEFT;
			lvc.cx		= 100;
			lvc.pszText	= szText;

			for (int i = 0; i < 5; i++)
			{
				lvc.iSubItem = i;
				LoadString((HINSTANCE)g_hInst, IDS_CONFLVS_LABEL1 + i, szText, sizeof(szText));
				
				ListView_InsertColumn(GetDlgItem(hWndDlg, IDC_SERVERINFO_LIST), i, &lvc);
			}

			ListView_DeleteAllItems(GetDlgItem(hWndDlg, IDC_SERVERINFO_LIST));

			BYTE	btInstalled;

			if (jRegGetKey(_LOGIN_SERVER_REGISTRY, _T("Installed"), (LPBYTE)&btInstalled))
				InitListView(hWndDlg);
			
			return TRUE;
		}
		case WM_COMMAND:
		{
			case IDC_SVRLIST_ADD:
				DialogBox(g_hInst, MAKEINTRESOURCE(IDD_SVRLIST_ADD), hWndDlg, (DLGPROC)AddSvrListProc);
				break;
		}
	}

	return FALSE;
}
