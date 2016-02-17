#include "stdafx.h"

LPARAM OnClientSockMsg(WPARAM wParam, LPARAM lParam);
LPARAM OnLogSvrSockMsg(WPARAM wParam, LPARAM lParam);

BOOL	jRegSetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, DWORD dwFlags, LPBYTE pValue, DWORD nValueSize);
BOOL	jRegGetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, LPBYTE pValue);

BOOL	CALLBACK ConfigDlgFunc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

void InitMonsterGenInfo();
void InitMagicInfo();
void InitMonRaceInfo();
void InitStdItemSpecial();
void InitStdItemEtcInfo();
void InitMerchantInfo();
void InitMoveMapEventInfo();
CMapInfo* InitMapInfo(int nServerIndex);

void InitAdminCommandList();
void UnInitAdminCommandList();
void LoadMap(CMapInfo* pMapInfo);

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

CMapInfo* InitDataInDatabase()
{
	int nServerIndex;

	InitMagicInfo();
	InitMonsterGenInfo();
	InitMonRaceInfo();
	InitStdItemSpecial();
	InitStdItemEtcInfo();
	InitMerchantInfo();
	InitMoveMapEventInfo();

	if (!jRegGetKey(_GAME_SERVER_REGISTRY, _TEXT("ServerNumber"), (LPBYTE)&nServerIndex))
		return FALSE;

	return InitMapInfo(nServerIndex);
}

UINT WINAPI InitializingServer(LPVOID lpParameter)
{
	TCHAR		wszPath[128];
	TCHAR		wszFullPath[256];
	DWORD		dwReadLen;

	ZeroMemory(g_szGoldName, sizeof(g_szGoldName));
	LoadString(g_hInst, IDS_GOLD, wszPath, sizeof(wszPath)/sizeof(TCHAR));
	WideCharToMultiByte(CP_ACP, 0, wszPath, -1, g_szGoldName, sizeof(g_szGoldName), NULL, NULL);

	jRegGetKey(_GAME_SERVER_REGISTRY, _TEXT("MapFileLoc"), (LPBYTE)wszPath);

	lstrcpy(wszFullPath, wszPath);
	lstrcat(wszFullPath, _TEXT("\\"));
	lstrcat(wszFullPath, _TEXT("searchTable.tbl"));

	HANDLE hFile = CreateFile(wszFullPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if ( hFile != INVALID_HANDLE_VALUE )
	{
		ReadFile(hFile, &g_SearchTable, sizeof(g_SearchTable), &dwReadLen, NULL);
	 	CloseHandle(hFile);
	}

	CMapInfo* pMapInfo = (CMapInfo*)lpParameter;

	for (int i = 0; i < g_nNumOfMapInfo; i++)
		LoadMap(&pMapInfo[i]);

	delete [] pMapInfo;
	pMapInfo = NULL;

	InitAdminCommandList();

	BYTE	btInstalled;

	if (!jRegGetKey(_GAME_SERVER_REGISTRY, _TEXT("Installed"), (LPBYTE)&btInstalled))
		DialogBox(g_hInst, MAKEINTRESOURCE(IDD_CONFIGDLG), NULL, (DLGPROC)ConfigDlgFunc);

	DWORD	dwIP = 0;
	TCHAR	szPort[24];
	int		nPort = 0;

	jRegGetKey(_GAME_SERVER_REGISTRY, _TEXT("DBServerIP"), (LPBYTE)&dwIP);
	jRegGetKey(_GAME_SERVER_REGISTRY, _TEXT("DBServerPort"), (LPBYTE)&nPort);
	_itow(nPort, szPort, 10);

	ConnectToServer(g_csock, &g_caddr, _IDM_CLIENTSOCK_MSG, NULL, dwIP, nPort, FD_CONNECT|FD_READ|FD_CLOSE);
//	ConnectToServer(g_clsock, &g_claddr, _IDM_LOGSVRSOCK_MSG, NULL, dwIP, 5500, FD_CONNECT|FD_READ|FD_CLOSE);

	return 0L;
}

void UnInitializingServer()
{
	if (g_pMagicInfo) 
	{
		delete [] g_pMagicInfo;
		g_pMagicInfo = NULL;
	}
	
	if (g_pMonGenInfo) 
	{
		delete [] g_pMonGenInfo;
		g_pMonGenInfo = NULL;
	}
	
	if (g_pMonRaceInfo) 
	{
		delete [] g_pMonRaceInfo;
		g_pMonRaceInfo = NULL;
	}

	if (g_pStdItemSpecial) 
	{
		delete [] g_pStdItemSpecial;
		g_pStdItemSpecial = NULL;
	}

	if (g_pStdItemEtc) 
	{
		delete [] g_pStdItemEtc;
		g_pStdItemEtc = NULL;
	}

	if (g_pMerchantInfo) 
	{
		delete [] g_pMerchantInfo;
		g_pMerchantInfo = NULL;
	}

	if (g_pMoveMapEventInfo) 
	{
		delete [] g_pMoveMapEventInfo;
		g_pMoveMapEventInfo = NULL;
	}
}

void OnCommand(WPARAM wParam, LPARAM lParam)
{
	switch (LOWORD(wParam))
	{
		case IDM_STARTSERVICE:
		{
			g_fTerminated = FALSE;

			CMapInfo* pMapInfo = InitDataInDatabase();

			UINT			dwThreadIDForMsg = 0;
			unsigned long	hThreadForMsg = 0;

			hThreadForMsg = _beginthreadex(NULL, 0, InitializingServer, pMapInfo, 0, &dwThreadIDForMsg);

			SwitchMenuItem(TRUE);

			return;
		}
		case IDM_STOPSERVICE:
		{
			g_fTerminated = TRUE;

			SwitchMenuItem(FALSE);
			
			UnInitializingServer();
			UnInitAdminCommandList();
		
			return;
		}
		case IDM_CONFIG:
		{
			DialogBox(g_hInst, MAKEINTRESOURCE(IDD_CONFIGDLG), NULL, (DLGPROC)ConfigDlgFunc);
	
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
		case _IDM_LOGSVRSOCK_MSG:
			return OnLogSvrSockMsg(wParam, lParam);
		case WM_COMMAND:
			OnCommand(wParam, lParam);
			break;
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
				MoveWindow(g_hLogMsgWnd, 0, (rcToolBar.bottom - rcToolBar.top), (rcMain.right - rcMain.left), 
								(rcMain.bottom - rcMain.top) - (rcToolBar.bottom - rcToolBar.top) - (rcStatusBar.bottom - rcStatusBar.top), 
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

			if (MessageBox(g_hMainWnd, szMsg, szTitle, MB_ICONSTOP|MB_YESNO) == IDYES)
			{
				if (SendMessage(g_hToolBar, TB_GETSTATE, (WPARAM)IDM_STARTSERVICE, (LPARAM)0L) == TBSTATE_INDETERMINATE)
					OnCommand(IDM_STOPSERVICE, 0L);

				ClearSocket(g_ssock);

				WSACleanup();

				CoUninitialize();

				PostQuitMessage(0);
			}

			return 0L;
		}
	}

	return (DefWindowProc(hWnd, nMsg, wParam, lParam));
}
