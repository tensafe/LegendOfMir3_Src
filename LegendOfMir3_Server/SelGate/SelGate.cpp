// SelGate.cpp : Defines the entry point for the application.
//

#include "stdafx.h"

// **************************************************************************************

BOOL	InitApplication(HANDLE hInstance);
BOOL	InitInstance(HANDLE hInstance, int nCmdShow);
LPARAM	APIENTRY MainWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

BOOL	jRegSetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, DWORD dwFlags, LPBYTE pValue, DWORD nValueSize);
BOOL	jRegGetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, LPBYTE pValue);

BOOL	CALLBACK ConfigDlgFunc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

// **************************************************************************************
//
//			Global Variables Definition
//
// **************************************************************************************

HINSTANCE		g_hInst = NULL;				// Application instance
HWND			g_hMainWnd = NULL;			// Main window handle
HWND			g_hLogMsgWnd = NULL;
HWND			g_hToolBar = NULL;
HWND			g_hStatusBar = NULL;

static WSADATA	g_wsd;

//list<CUserInfo> TUserInfoList;
//CUserInfo TUserInfoList[1500];

TBBUTTON tbButtons[] = 
{
	{ 0, IDM_STARTSERVICE,	TBSTATE_ENABLED,	TBSTYLE_BUTTON, 0L, 0},
	{ 1, IDM_STOPSERVICE,	TBSTATE_ENABLED,	TBSTYLE_BUTTON, 0L, 0}
};

int				g_nStatusPartsWidths[_NUMOFMAX_STATUS_PARTS] = { 10, 10, 10, -1 };

// **************************************************************************************

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;

//	if (CheckAvailableIOCP())
//	{
		if (!InitApplication(hInstance))
			return (FALSE);

		if (!InitInstance(hInstance, nCmdShow))
			return (FALSE);

		while (GetMessage(&msg, NULL, 0, 0))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
/*	}
	else
	{
		TCHAR szMsg[1024];

		LoadString(hInstance, IDS_NOTWINNT, szMsg, sizeof(szMsg));
		MessageBox(NULL, szMsg, _LOGINGATE_SERVER_TITLE, MB_OK|MB_ICONINFORMATION);
		
		return -1;
	}
*/
    return (msg.wParam);
}

// **************************************************************************************
//
//			
//
// **************************************************************************************

BOOL InitApplication(HANDLE hInstance)
{
    WNDCLASS  wc;

    wc.style            = CS_GLOBALCLASS|CS_HREDRAW|CS_VREDRAW;
    wc.lpfnWndProc      = (WNDPROC)MainWndProc;
    wc.cbClsExtra       = 0;
    wc.cbWndExtra       = 0;
    wc.hIcon            = LoadIcon((HINSTANCE)hInstance, MAKEINTRESOURCE(IDI_MIR2));
    wc.hInstance        = (HINSTANCE)hInstance;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName     = MAKEINTRESOURCE(IDR_MAINMENU);
    wc.lpszClassName    = _SELGATE_SERVER_CLASS;

	return RegisterClass(&wc);
}

// **************************************************************************************
//
//			
//
// **************************************************************************************

BOOL InitInstance(HANDLE hInstance, int nCmdShow)
{
	g_hInst = (HINSTANCE)hInstance;
	
	INITCOMMONCONTROLSEX	icex;

	icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
	icex.dwICC = ICC_LISTVIEW_CLASSES | ICC_BAR_CLASSES | ICC_INTERNET_CLASSES;

	InitCommonControlsEx(&icex);

    g_hMainWnd = CreateWindowEx(0, _SELGATE_SERVER_CLASS, _SELGATE_SERVER_TITLE, 
							WS_OVERLAPPEDWINDOW|WS_VISIBLE,
							CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,                 
							NULL, NULL, (HINSTANCE)hInstance, NULL);

	g_hToolBar = CreateToolbarEx(g_hMainWnd, WS_CHILD|CCS_TOP|WS_VISIBLE|WS_BORDER,
									_IDW_TOOLBAR, sizeof(tbButtons) / sizeof(TBBUTTON), (HINSTANCE)hInstance, IDB_TOOLBAR,
									(LPCTBBUTTON)&tbButtons, sizeof(tbButtons) / sizeof(TBBUTTON),
									_BMP_CX, _BMP_CY, _BMP_CX, _BMP_CY, sizeof(TBBUTTON));

	RECT rcMainWnd, rcToolBar, rcStatusBar;

	GetClientRect(g_hMainWnd, &rcMainWnd);
	GetWindowRect(g_hToolBar, &rcToolBar);

	g_hStatusBar = CreateWindowEx(0L, STATUSCLASSNAME, _T(""), WS_CHILD|WS_BORDER|WS_VISIBLE|SBS_SIZEGRIP,
									0, rcMainWnd.bottom - _STATUS_HEIGHT, (rcMainWnd.right - rcMainWnd.left), _STATUS_HEIGHT, g_hMainWnd, (HMENU)_IDW_STATUSBAR, g_hInst, NULL);

	int	nStatusPartsWidths[_NUMOFMAX_STATUS_PARTS];
	int nCnt = 0;
	
	for (int i = _NUMOFMAX_STATUS_PARTS - 1; i >= 0; i--)
		nStatusPartsWidths[nCnt++] = (rcMainWnd.right - rcMainWnd.left) - (90 * i);

	SendMessage(g_hStatusBar, SB_SETPARTS, _NUMOFMAX_STATUS_PARTS, (LPARAM)nStatusPartsWidths);
	SendMessage(g_hStatusBar, SB_SETTEXT, MAKEWORD(1, 0), (LPARAM)_T("Not Connected"));

	GetWindowRect(g_hStatusBar, &rcStatusBar);

    g_hLogMsgWnd = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, _TEXT(""), 
							WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_REPORT|LVS_EDITLABELS,
							0, (rcToolBar.bottom - rcToolBar.top) - 2, (rcMainWnd.right - rcMainWnd.left), 
							(rcMainWnd.bottom - rcMainWnd.top) - (rcToolBar.bottom - rcToolBar.top) - (rcStatusBar.bottom - rcStatusBar.top) + 2,
							g_hMainWnd, NULL, (HINSTANCE)hInstance, NULL);

	ListView_SetExtendedListViewStyleEx(g_hLogMsgWnd, 0, LVS_EX_FULLROWSELECT);

	LV_COLUMN	lvc;
	TCHAR		szText[64];

	lvc.mask	= LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt		= LVCFMT_LEFT;
	lvc.cx		= 100;
	lvc.pszText	= szText;

	for (i = 0; i < 3; i++)
	{
		lvc.iSubItem = i;
		LoadString((HINSTANCE)hInstance, IDS_LVS_LABEL1 + i, szText, sizeof(szText));
		
		ListView_InsertColumn(g_hLogMsgWnd, i, &lvc);
	}

	ListView_SetColumnWidth( g_hLogMsgWnd, 2, 500 );

	SendMessage(g_hToolBar, TB_SETSTATE, (WPARAM)IDM_STOPSERVICE, (LPARAM)MAKELONG(TBSTATE_INDETERMINATE, 0));

	ShowWindow(g_hMainWnd, SW_SHOW);
	UpdateWindow(g_hMainWnd);

	if (WSAStartup(MAKEWORD(2, 2), &g_wsd) != 0)
		return (FALSE);

	//
	BYTE	btInstalled;

	if (!jRegGetKey(_SELGATE_SERVER_REGISTRY, _TEXT("Installed"), (LPBYTE)&btInstalled))
		DialogBox(g_hInst, MAKEINTRESOURCE(IDD_CONFIGDLG), NULL, (DLGPROC)ConfigDlgFunc);

	return TRUE;
}

// **************************************************************************************
//
//			
//
// **************************************************************************************

int AddNewLogMsg()
{
	LV_ITEM		lvi;
	TCHAR		szText[64];

	int nCount = ListView_GetItemCount(g_hLogMsgWnd);

	if (nCount >= 50)
	{
		ListView_DeleteItem(g_hLogMsgWnd, 0);
		nCount--;
	}

	lvi.mask		= LVIF_TEXT;
	lvi.iItem		= nCount;
	lvi.iSubItem	= 0;
	
	_tstrdate(szText);

	lvi.pszText = szText;
	
	ListView_InsertItem(g_hLogMsgWnd, &lvi);

	_tstrtime(szText);

	ListView_SetItemText(g_hLogMsgWnd, nCount, 1, szText);

	return nCount;
}

void InsertLogMsg(UINT nID)
{
	TCHAR	szText[256];

	int nCount = AddNewLogMsg();

	LoadString(g_hInst, nID, szText, sizeof(szText));

	ListView_SetItemText(g_hLogMsgWnd, nCount, 2, szText);
	ListView_Scroll(g_hLogMsgWnd, 0, 8);
}

void InsertLogMsg(LPTSTR lpszMsg)
{
	int nCount = AddNewLogMsg();

	ListView_SetItemText(g_hLogMsgWnd, nCount, 2, lpszMsg);
	ListView_Scroll(g_hLogMsgWnd, 0, 8);
}
