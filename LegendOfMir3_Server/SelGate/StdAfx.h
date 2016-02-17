// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
#define AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#define UNICODE
#define _UNICODE

#include <winsock2.h>
#include <windows.h>
#include <process.h>
#include <stdio.h>
#include <tchar.h>
#ifdef _DEBUG
#include <crtdbg.h>
#endif

#pragma warning( disable : 4146 )
#pragma warning(disable:4786)
#include <commctrl.h>
#include <time.h>

#include "resource.h"

#include "..\Def\EnDecode.h"
#include "..\Def\ServerSockHandler.h"
#include "..\Def\Queue.h"
#include "..\Def\Misc.h"
#include "..\Def\List.h"

#include "SelGate.h"

#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "comctl32.lib")

// **************************************************************************************

#define _BMP_CX						16
#define _BMP_CY						16

#define _STATUS_HEIGHT				10
#define _NUMOFMAX_STATUS_PARTS		4

#define _SELGATE_SERVER_CLASS		_T("SelGateServerClass")
#define _SELGATE_SERVER_TITLE		_T("Legend of Mir II - Select Gate Server")
#define _SELGATE_SERVER_REGISTRY	_T("Software\\LegendOfMir\\SelGate")

#define _IDM_SERVERSOCK_MSG			WM_USER + 1000
#define _IDM_CLIENTSOCK_MSG			_IDM_SERVERSOCK_MSG + 1

#define _IDW_TOOLBAR				_IDM_SERVERSOCK_MSG + 2
#define _IDW_STATUSBAR				_IDM_SERVERSOCK_MSG + 3

#define _ID_TIMER_KEEPALIVE			_IDM_SERVERSOCK_MSG + 4
#define _ID_TIMER_CONNECTSERVER		_IDM_SERVERSOCK_MSG + 5

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
