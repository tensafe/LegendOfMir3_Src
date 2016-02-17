#include "stdafx.h"
#include <stdio.h>
#include "../def/dbmgr.h"


BOOL			jRegSetKey(LPCSTR pSubKeyName, LPCSTR pValueName, DWORD dwFlags, LPBYTE pValue, DWORD nValueSize);
BOOL			jRegGetKey(LPCSTR pSubKeyName, LPCSTR pValueName, LPBYTE pValue);

BOOL CALLBACK	ConfigDlgFunc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
BOOL CALLBACK	ServerListProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

LPARAM			OnGateCommSockMsg(WPARAM wParam, LPARAM lParam);

BOOL			InitServerThreadForMsg();


extern CWHList< GAMESERVERINFO * >	g_xGameServerList;
extern CWHList< GATESERVERINFO * >	g_xGateServerList;
extern char							g_szServerList[1024];

extern HINSTANCE		g_hInst;

extern HWND				g_hMainWnd;
extern HWND				g_hLogMsgWnd;
extern HWND				g_hToolBar;
extern HWND				g_hStatusBar;

extern unsigned long	g_hThreadForMsg;

SOCKET					g_gcSock = INVALID_SOCKET;
SOCKADDR_IN				g_gcAddr;

BOOL					g_fTerminated = FALSE;

void SwitchMenuItem(BOOL fFlag)
{
	HMENU hMainMenu = GetMenu(g_hMainWnd);
	HMENU hMenu = GetSubMenu(hMainMenu, 0);

	if (fFlag)
	{
		EnableMenuItem(hMenu, IDM_STARTSERVICE, MF_GRAYED|MF_BYCOMMAND);
		EnableMenuItem(hMenu, IDM_STOPSERVICE, MF_ENABLED|MF_BYCOMMAND);

		SendMessage(g_hToolBar, TB_SETSTATE, (WPARAM)IDM_STARTSERVICE, (LPARAM)MAKELONG(TBSTATE_INDETERMINATE, 0));
		SendMessage(g_hToolBar, TB_SETSTATE, (WPARAM)IDM_STOPSERVICE, (LPARAM)MAKELONG(TBSTATE_ENABLED, 0));

		InsertLogMsg(IDS_STARTSERVICE);
	}
	else
	{
		EnableMenuItem(hMenu, IDM_STARTSERVICE, MF_ENABLED|MF_BYCOMMAND);
		EnableMenuItem(hMenu, IDM_STOPSERVICE, MF_GRAYED|MF_BYCOMMAND);

		SendMessage(g_hToolBar, TB_SETSTATE, (WPARAM)IDM_STARTSERVICE, (LPARAM)MAKELONG(TBSTATE_ENABLED, 0));
		SendMessage(g_hToolBar, TB_SETSTATE, (WPARAM)IDM_STOPSERVICE, (LPARAM)MAKELONG(TBSTATE_INDETERMINATE, 0));

		InsertLogMsg(IDS_STOPSERVICE);
	}
}

void CreateConfigProperties()
{
	TCHAR			szText[64];

	PROPSHEETPAGE	psp[1];
	PROPSHEETHEADER	psh;

	LoadString((HINSTANCE)g_hInst, IDS_TAB_LABEL1, szText, sizeof(szText));

	psp[0].dwSize		= sizeof(PROPSHEETPAGE);
	psp[0].dwFlags		= 0; //PSP_USETITLE;
	psp[0].hInstance	= g_hInst;
	psp[0].pszTemplate	= MAKEINTRESOURCE(IDD_CONFIGDLG_SERVERLIST);
	psp[0].pszIcon		= NULL;
	psp[0].pfnDlgProc	= ServerListProc;
	psp[0].pszTitle		= szText;
	psp[0].lParam		= 0;

	psh.dwSize		= sizeof(PROPSHEETHEADER);
	psh.dwFlags		= PSH_PROPSHEETPAGE | PSH_NOAPPLYNOW;
	psh.hwndParent	= g_hMainWnd;
	psh.hInstance	= g_hInst;
	psh.pszIcon		= NULL;
	psh.pszCaption	= (LPTSTR)_T("Configuration");
	psh.nPages		= sizeof(psp) / sizeof(PROPSHEETPAGE);
	psh.ppsp		= (LPCPROPSHEETPAGE)&psp;

	PropertySheet(&psh);
}

UINT WINAPI LoadAccountRecords(LPVOID lpParameter)
{
	InsertLogMsg(IDS_LOADACCOUNTRECORDS);

	CRecordset *pRec = GetDBManager()->CreateRecordset();
	pRec->Execute( "UPDATE TBL_ACCOUNT SET FLD_CERTIFICATION=0 WHERE FLD_CERTIFICATION >= 30" );
	GetDBManager()->DestroyRecordset( pRec );
	// ----------------------------------------------------------------------------------------

	GAMESERVERINFO *pServerInfo;

	pRec = GetDBManager()->CreateRecordset();
	if ( pRec->Execute( "SELECT * FROM TBL_SERVERINFO" ) )
	{
		while ( pRec->Fetch() )
		{			
			pServerInfo = new GAMESERVERINFO;
			if ( !pServerInfo )
				break;

			pServerInfo->index = atoi( pRec->Get( "FLD_SERVERIDX" ) );
			strcpy( pServerInfo->name, pRec->Get( "FLD_SERVERNAME" ) );
			strcpy( pServerInfo->ip,   pRec->Get( "FLD_SERVERIP" ) );
			pServerInfo->connCnt = 0;

			g_xGameServerList.AddNewNode( pServerInfo );
		}
	}
	GetDBManager()->DestroyRecordset( pRec );

	char szTmp[64];
	for ( PLISTNODE pNode = g_xGameServerList.GetHead(); pNode; pNode = g_xGameServerList.GetNext( pNode ) )
	{
		pServerInfo = g_xGameServerList.GetData( pNode );
		
		sprintf( szTmp, "%d,%s,", pServerInfo->index, pServerInfo->name );
		strcat( g_szServerList, szTmp );
	}
	// ----------------------------------------------------------------------------------------

	InitServerThreadForMsg();

	if (InitServerSocket(g_gcSock, &g_gcAddr, _IDM_GATECOMMSOCK_MSG, 5500, 1))
		SwitchMenuItem(TRUE);

	return 0L;
}

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
			g_fTerminated = FALSE;
			
			UINT			dwThreadIDForMsg = 0;
			unsigned long	hThreadForMsg = 0;
			
			hThreadForMsg = _beginthreadex(NULL, 0, LoadAccountRecords, NULL, 0, &dwThreadIDForMsg);
			
			return;
		}
		case IDM_STOPSERVICE:
		{
			ClearSocket(g_gcSock);

			SwitchMenuItem(FALSE);

			return;
		}
		case IDM_FONTCOLOR:
			SetFontColor();
			return;
		case IDM_BACKCOLOR:
			SetBackColor();
			return;
		case IDM_CONFIG:
		{
			CreateConfigProperties();
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
		case WM_COMMAND:
			OnCommand(wParam, lParam);
			break;
		case WM_ERASEBKGND:
			return 0; // ORZ: ±ôºý°Å¸®´Â °Í Á¦°Å
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
//				ClearServerSocket();
//				DisconnectFromServer();

				OnCommand(IDM_STOPSERVICE, 0L);

				CoUninitialize();

				WSACleanup();

				PostQuitMessage(0);
			}

			return 0L;
		}
	}

	return (DefWindowProc(hWnd, nMsg, wParam, lParam));
}
