// VirtualClient.cpp : Defines the entry point for the application.
//
#include "stdafx.h"

#include "resource.h"

// **************************************************************************************

BOOL	InitApplication(HANDLE hInstance);
BOOL	InitInstance(HANDLE hInstance, int nCmdShow);
LPARAM	APIENTRY MainWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam);

BOOL	ConnectServer(SOCKET &s, SOCKADDR_IN* addr, UINT nMsgID, LPCTSTR lpServerIP, DWORD dwIP, int nPort, long lEvent);

LRESULT OnSockMsg(WPARAM wParam, LPARAM lParam);

// **************************************************************************************
//
//			Global Variables Definition
//
// **************************************************************************************

HINSTANCE		g_hInst = NULL;				// Application instance
HWND			g_hMainWnd = NULL;			// Main window handle
HWND			g_hLogMsgWnd = NULL;

static WSADATA	g_wsd;

char			szLvsLable[3][10] = { "Date", "Time", "Message" };
char			g_szServerIP[15];

SOCKET			g_sock = INVALID_SOCKET;
SOCKADDR_IN		g_addr;

int				g_nCode = 0;

// **************************************************************************************

int AddNewLogMsg()
{
	LV_ITEM		lvi;
	TCHAR		szText[64];

	int nCount = ListView_GetItemCount(g_hLogMsgWnd);

	if (nCount >= 100)
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

void InsertLogMsg(LPTSTR lpszMsg)
{
	int nCount = AddNewLogMsg();

	ListView_SetItemText(g_hLogMsgWnd, nCount, 2, lpszMsg);

#ifdef _DEBUG
	_RPT1(_CRT_WARN, "%s\n", lpszMsg);
#endif
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    MSG msg;

	lstrcpy(g_szServerIP, lpCmdLine);

	if (!InitApplication(hInstance))
		return (FALSE);

	if (!InitInstance(hInstance, nCmdShow))
		return (FALSE);

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

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
    wc.hIcon            = NULL;
    wc.hInstance        = (HINSTANCE)hInstance;
    wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground    = (HBRUSH)(COLOR_WINDOW + 1);
    wc.lpszMenuName     = MAKEINTRESOURCE(IDR_MAINMENU);
    wc.lpszClassName    = _T("VirtualClient");

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
	icex.dwICC = ICC_LISTVIEW_CLASSES | ICC_BAR_CLASSES | ICC_INTERNET_CLASSES | ICC_TAB_CLASSES;

	InitCommonControlsEx(&icex);

    g_hMainWnd = CreateWindowEx(0, _T("VirtualClient"), _T("미르의 전설 2 - 가상 클라이언트"), 
							WS_OVERLAPPEDWINDOW|WS_VISIBLE,
							CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,                 
							NULL, NULL, (HINSTANCE)hInstance, NULL);

	RECT rcMainWnd;

	GetClientRect(g_hMainWnd, &rcMainWnd);

    g_hLogMsgWnd = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, "", 
							WS_CHILD|WS_VISIBLE|WS_BORDER|LVS_REPORT|LVS_EDITLABELS,
							0, 0, (rcMainWnd.right - rcMainWnd.left), 
							(rcMainWnd.bottom - rcMainWnd.top),
							g_hMainWnd, NULL, (HINSTANCE)hInstance, NULL);

	ListView_SetExtendedListViewStyleEx(g_hLogMsgWnd, 0, LVS_EX_FULLROWSELECT);

	LV_COLUMN	lvc;
	TCHAR		szText[64];

	lvc.mask	= LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.fmt		= LVCFMT_LEFT;
	lvc.cx		= 150;
	lvc.pszText	= szText;

	for (int i = 0; i < 3; i++)
	{
		lvc.iSubItem	= i;
		lvc.pszText		= szLvsLable[i];
		
		ListView_InsertColumn(g_hLogMsgWnd, i, &lvc);
	}

	ShowWindow(g_hMainWnd, SW_SHOW);
	UpdateWindow(g_hMainWnd);

	if (WSAStartup(MAKEWORD(2, 2), &g_wsd) != 0)
		return (FALSE);

	ConnectServer(g_sock, &g_addr, _IDM_SOCKMSG, g_szServerIP, 0, 7000, FD_CONNECT|FD_READ|FD_CLOSE);

	return TRUE;
}

BOOL CALLBACK LoginFunc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK NewUserFunc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

LPARAM APIENTRY MainWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
		case _IDM_SOCKMSG:
			return OnSockMsg(wParam, lParam);
/*		case _IDM_CLIENTSOCK_MSG:
			return OnClientSockMsg(wParam, lParam);
*/		case WM_COMMAND:
		{
			switch (LOWORD(wParam))
			{
				case IDM_CM_IDPASSWORD:
					DialogBox(g_hInst, MAKEINTRESOURCE(IDD_LOGIN), NULL, (DLGPROC)LoginFunc);
					break;
				case IDM_CM_ADDUSER:
					DialogBox(g_hInst, MAKEINTRESOURCE(IDD_NEWUSER), NULL, (DLGPROC)NewUserFunc);
					break;
			}

			break;
		}
		case WM_SIZE:
		{
			RECT rcMain;

			GetClientRect(g_hMainWnd, &rcMain);

			MoveWindow(g_hLogMsgWnd, 0, 0, (rcMain.right - rcMain.left), (rcMain.bottom - rcMain.top), TRUE);

			break;
		}
		case WM_CLOSE:
		{
			WSACleanup();

			PostQuitMessage(0);

			return 0L;
		}
	}

	return (DefWindowProc(hWnd, nMsg, wParam, lParam));
}

