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
#include <tchar.h>
#ifdef _DEBUG
#include <crtdbg.h>
#endif
#include <stdio.h>
#include <commctrl.h>
#include <time.h>
#include <process.h>
#include <stdlib.h>

#include "resource.h"
#include "..\AdminCmd\resource.h"

#define ROUND(f)	(int)(f + 0.5)
#define _MAX(i, j)	(i > j ? i : j)
#define _MIN(i, j)	(i > j ? j : i)

// **************************************************************************************

#define _STATUS_HEIGHT				10
#define _NUMOFMAX_STATUS_PARTS		6

#define	MAXLEVEL					51

#define _GAME_SERVER_CLASS			_TEXT("MirGameServerClass")
#define _GAME_SERVER_TITLE			_TEXT("Game Server - Legend of Mir II")
#define _GAME_SERVER_REGISTRY		_TEXT("Software\\LegendOfMir\\GameServer")

#define _IDM_SERVERSOCK_MSG			WM_USER + 1000
#define _IDM_CLIENTSOCK_MSG			_IDM_SERVERSOCK_MSG + 1
#define _IDM_LOGSVRSOCK_MSG			_IDM_SERVERSOCK_MSG + 2

#define _IDW_TOOLBAR				_IDM_SERVERSOCK_MSG + 3
#define _IDW_STATUSBAR				_IDM_SERVERSOCK_MSG + 4

#define _ID_TIMER_CONNECTSERVER		_IDM_SERVERSOCK_MSG + 5
#define _ID_TIMER_PROCESSUSERHUMAN	_IDM_SERVERSOCK_MSG + 6

typedef struct tag_TSEARCHTABLE
{
	char	x;
	char	y;
} _TSEARCHTABLE, *_LPTSEARCHTABLE;

#define _MAX_SEARCHTABLE			960

#define _ITEM_ACTION_CREATE			1
#define _ITEM_ACTION_THROW			2
#define _ITEM_ACTION_PICKUP			3
#define _ITEM_ACTION_UPDATE			4

// **************************************************************************************

#include "..\Def\EnDecode.h"
#include "..\Def\Protocol.h"
#include "..\Def\ServerSockHandler.h"
#include "..\Def\Queue.h"
#include "..\Def\List.h"
#include "..\Def\StaticArray.h"
#include "..\Def\Misc.h"

#include "..\Def\_orzEx/database.h"
#include "..\Def\_orzEx/syncobj.h"
#include "..\Def\_orzEx/list.h"

class CCharObject;
class CAnimalObject;
class CObjectAddAbility;
class CEvent;

class CMonRaceInfo;

#include "MagicHandler.h"
#include "ItemHandler.h"
#include "ObjectEngine.h"
#include "ObjectPlayer.h"
#include "ObjectScripter.h"
#include "ObjectPlayerClone.h"
#include "ObjectNPC.h"
#include "ObjectMonster.h"
#include "Tables.h"
#include "DataHandler.h"
#include "GameSvr.h"
#include "FrontEngine.h"
#include "ObjectEvent.h"

#include "global.h"

#pragma comment (lib, "ws2_32.lib")
#pragma comment (lib, "comctl32.lib")

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)
