#include "stdafx.h"

BOOL jRegSetKey(LPCSTR pSubKeyName, LPCSTR pValueName, DWORD dwFlags, LPBYTE pValue, DWORD nValueSize);
BOOL jRegGetKey(LPCSTR pSubKeyName, LPCSTR pValueName, LPBYTE pValue);

extern HINSTANCE	g_hInst;
extern HWND			g_hMainWnd;

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

			//
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

/*			TC_ITEM		tie;
			
			tie.mask	= TCIF_TEXT;
			tie.iImage	= -1;
			tie.pszText	= szText;

			LoadString((HINSTANCE)g_hInst, IDS_TAB_LABEL1, szText, sizeof(szText));
			TabCtrl_InsertItem(GetDlgItem(hWndDlg, IDC_CONFIG_TAB), 0, &tie);
*/
			break;
		}
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
