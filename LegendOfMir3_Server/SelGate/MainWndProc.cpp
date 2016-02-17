#include "stdafx.h"

LPARAM OnServerSockMsg(WPARAM wParam, LPARAM lParam);
LPARAM OnClientSockMsg(WPARAM wParam, LPARAM lParam);

BOOL	jRegSetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, DWORD dwFlags, LPBYTE pValue, DWORD nValueSize);
BOOL	jRegGetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, LPBYTE pValue);

BOOL CALLBACK ConfigDlgFunc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

VOID WINAPI OnTimerProc(HWND hWnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime);

extern HINSTANCE				g_hInst;

extern HWND						g_hMainWnd;
extern HWND						g_hLogMsgWnd;
extern HWND						g_hToolBar;
extern HWND						g_hStatusBar;

extern HANDLE					g_hMsgThread;

#ifndef _SOCKET_ASYNC_IO
extern HANDLE					g_hIOCP;
extern HANDLE					g_hAcceptThread;
#endif

SOCKET			g_ssock = INVALID_SOCKET;
SOCKADDR_IN		g_saddr;

SOCKET			g_csock = INVALID_SOCKET;
SOCKADDR_IN		g_caddr;

BOOL			g_fTerminated = FALSE;


void SetFontColor()
{
	CHOOSECOLOR color;
	static COLORREF acrCustClr[16]; 

	memset( &color, 0, sizeof( color ) );
	color.hwndOwner		= g_hMainWnd;
	color.lStructSize	= sizeof( color );
	color.Flags			= CC_FULLOPEN | CC_RGBINIT;
	color.rgbResult		= ListView_GetTextColor( g_hLogMsgWnd );
	color.lpCustColors	= acrCustClr;

	if ( !ChooseColor( &color ) )
		return;

	ListView_SetTextColor( g_hLogMsgWnd, color.rgbResult );

	InvalidateRect( g_hLogMsgWnd, NULL, TRUE );
}


void SetBackColor()
{
	CHOOSECOLOR color;
	static COLORREF acrCustClr[16]; 

	memset( &color, 0, sizeof( color ) );
	color.hwndOwner		= g_hMainWnd;
	color.lStructSize	= sizeof( color );
	color.Flags			= CC_FULLOPEN | CC_RGBINIT;
	color.rgbResult		= ListView_GetBkColor( g_hLogMsgWnd );
	color.lpCustColors	= acrCustClr;

	if ( !ChooseColor( &color ) )
		return;

	ListView_SetTextBkColor( g_hLogMsgWnd, color.rgbResult );
	ListView_SetBkColor( g_hLogMsgWnd, color.rgbResult );

	InvalidateRect( g_hLogMsgWnd, NULL, TRUE );
}


void OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		case IDM_STARTSERVICE:
		{
			DWORD	dwIP = 0;
			int		nPort = 0;

			g_fTerminated = FALSE;
		
			if (!jRegGetKey(_SELGATE_SERVER_REGISTRY, _TEXT("LocalPort"), (LPBYTE)&nPort))
				nPort = 7100;

			InitServerSocket(g_ssock, &g_saddr, _IDM_SERVERSOCK_MSG, nPort, 2);

			jRegGetKey(_SELGATE_SERVER_REGISTRY, _TEXT("RemoteIP"), (LPBYTE)&dwIP);

			if (!jRegGetKey(_SELGATE_SERVER_REGISTRY, _TEXT("RemotePort"), (LPBYTE)&nPort))
				nPort = 5100;

			ConnectToServer(g_csock, &g_caddr, _IDM_CLIENTSOCK_MSG, NULL, dwIP, nPort, FD_CONNECT|FD_READ|FD_CLOSE);

			HMENU hMainMenu = GetMenu(g_hMainWnd);
			HMENU hMenu = GetSubMenu(hMainMenu, 0);

			EnableMenuItem(hMenu, IDM_STARTSERVICE, MF_GRAYED|MF_BYCOMMAND);
			EnableMenuItem(hMenu, IDM_STOPSERVICE, MF_ENABLED|MF_BYCOMMAND);

			SendMessage(g_hToolBar, TB_SETSTATE, (WPARAM)IDM_STARTSERVICE, (LPARAM)MAKELONG(TBSTATE_INDETERMINATE, 0));
			SendMessage(g_hToolBar, TB_SETSTATE, (WPARAM)IDM_STOPSERVICE, (LPARAM)MAKELONG(TBSTATE_ENABLED, 0));

			InsertLogMsg(IDS_STARTSERVICE);

			SendMessage(g_hStatusBar, SB_SETTEXT, MAKEWORD(0, 0), (LPARAM)_TEXT("Ready"));

			return;
		}
		case IDM_STOPSERVICE:
		{
			g_fTerminated = TRUE;

/*			if (g_hAcceptThread != INVALID_HANDLE_VALUE)
			{
				TerminateThread(g_hAcceptThread, 0);
				WaitForSingleObject(g_hAcceptThread, INFINITE);
				CloseHandle(g_hAcceptThread);
				g_hAcceptThread = INVALID_HANDLE_VALUE;
			}

			if (g_hMsgThread != INVALID_HANDLE_VALUE)
			{
				TerminateThread(g_hMsgThread, 0);
				WaitForSingleObject(g_hMsgThread, INFINITE);
				CloseHandle(g_hMsgThread);
				g_hMsgThread = INVALID_HANDLE_VALUE;
			}
*/
			ClearSocket(g_ssock);
			ClearSocket(g_csock);

			CloseHandle(g_hIOCP);

			HMENU hMainMenu = GetMenu(g_hMainWnd);
			HMENU hMenu = GetSubMenu(hMainMenu, 0);

			EnableMenuItem(hMenu, IDM_STARTSERVICE, MF_ENABLED|MF_BYCOMMAND);
			EnableMenuItem(hMenu, IDM_STOPSERVICE, MF_GRAYED|MF_BYCOMMAND);

			SendMessage(g_hToolBar, TB_SETSTATE, (WPARAM)IDM_STARTSERVICE, (LPARAM)MAKELONG(TBSTATE_ENABLED, 0));
			SendMessage(g_hToolBar, TB_SETSTATE, (WPARAM)IDM_STOPSERVICE, (LPARAM)MAKELONG(TBSTATE_INDETERMINATE, 0));

			InsertLogMsg(IDS_STOPSERVICE);

			SendMessage(g_hStatusBar, SB_SETTEXT, MAKEWORD(0, 0), (LPARAM)_TEXT("Not Ready"));

			return;
		}
		// ORZ:
		case IDM_FONTCOLOR:
			SetFontColor();
			return;
		case IDM_BACKCOLOR:
			SetBackColor();
			return;
		case IDM_CONFIG:
		{
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_CONFIGDLG), g_hMainWnd, (DLGPROC)ConfigDlgFunc);

			return;
		}
	}
}

// **************************************************************************************
//
//			
//
// **************************************************************************************

LPARAM APIENTRY MainWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)
{
	switch (nMsg)
	{
#ifdef _SOCKET_ASYNC_IO
		case _IDM_SERVERSOCK_MSG:
			return OnServerSockMsg(wParam, lParam);
#endif
		case _IDM_CLIENTSOCK_MSG:
			return OnClientSockMsg(wParam, lParam);
		case WM_COMMAND:
			OnCommand(wParam, lParam);
			break;
		case WM_ERASEBKGND:
			return 0; // ORZ:
		case WM_SIZE:
		{
			if (g_hToolBar && g_hMainWnd && g_hStatusBar) 
			{
				RECT rcToolBar, rcMain, rcStatusBar;

				GetWindowRect(g_hToolBar, &rcToolBar);
				GetClientRect(g_hMainWnd, &rcMain);
				GetWindowRect(g_hStatusBar, &rcStatusBar);

				MoveWindow(g_hToolBar, 0, 0, LOWORD(lParam), (rcToolBar.bottom - rcToolBar.top), TRUE);
				MoveWindow(g_hStatusBar, 0, rcMain.bottom - (rcStatusBar.bottom - rcStatusBar.top), 
								LOWORD(lParam), (rcStatusBar.bottom - rcStatusBar.top), TRUE);
				MoveWindow(g_hLogMsgWnd, 0, (rcToolBar.bottom - rcToolBar.top) - 2, (rcMain.right - rcMain.left), 
								(rcMain.bottom - rcMain.top) - (rcToolBar.bottom - rcToolBar.top) - (rcStatusBar.bottom - rcStatusBar.top) + 2, 
								TRUE);

				int	nStatusPartsWidths[_NUMOFMAX_STATUS_PARTS];
				int nCnt = 0;

				for (int i = _NUMOFMAX_STATUS_PARTS - 1; i >= 0 ; i--)
					nStatusPartsWidths[nCnt++] = (rcStatusBar.right - rcStatusBar.left) - (90 * i);

				SendMessage(g_hStatusBar, SB_SETPARTS, _NUMOFMAX_STATUS_PARTS, (LPARAM)nStatusPartsWidths);
			}

			break;
		}
		case WM_CLOSE:
		{
			TCHAR	szMsg[128];
			TCHAR	szTitle[128];

			LoadString(g_hInst, IDS_PROGRAM_QUIT, szMsg, sizeof(szMsg));
			LoadString(g_hInst, IDS_PROGRAM_TITLE, szTitle, sizeof(szTitle));

			if (MessageBox(g_hMainWnd, szMsg, szTitle, MB_ICONINFORMATION|MB_YESNO) == IDYES)
			{
				if (SendMessage(g_hToolBar, TB_GETSTATE, (WPARAM)IDM_STARTSERVICE, (LPARAM)0L) == TBSTATE_INDETERMINATE)
					OnCommand(IDM_STOPSERVICE, 0L);

				WSACleanup();

				PostQuitMessage(0);
			}

			return 0L;
		}
	}

	return (DefWindowProc(hWnd, nMsg, wParam, lParam));
}
