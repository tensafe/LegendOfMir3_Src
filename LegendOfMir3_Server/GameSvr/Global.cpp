#include "stdafx.h"

// **************************************************************************************
//
//			Global Variables Definition
//
// **************************************************************************************

HINSTANCE		g_hInst			= NULL;			// Application instance
HWND			g_hMainWnd		= NULL;			// Main window handle
HWND			g_hLogMsgWnd	= NULL;
HWND			g_hToolBar		= NULL;

HWND			g_hStatusBar	= NULL;
int				g_nStatusPartsWidths[_NUMOFMAX_STATUS_PARTS] = { 10, 10, 10, -1 };
#define BTNS_SEP TBSTYLE_SEP // 0x0001 
TBBUTTON tbButtons[] = 
{
	{ 0, IDM_STARTSERVICE,	TBSTATE_ENABLED,	TBSTYLE_BUTTON, 0L, 0},
	{ 1, IDM_STOPSERVICE,	TBSTATE_ENABLED,	TBSTYLE_BUTTON, 0L, 0},
	{ 0, 0,					0,					BTNS_SEP,		0L, 0},	
	{ 2, IDM_SETFONTCOLOR,	TBSTATE_ENABLED,	TBSTYLE_BUTTON, 0L, 0},
	{ 3, IDM_SETBKGCOLOR,	TBSTATE_ENABLED,	TBSTYLE_BUTTON, 0L, 0}
};

CDatabase						g_MirDB;
CConnection						*g_pConnCommon;
CConnection						*g_pConnGame;

CWHList<char *>					g_xAdminCommandList;
CWHList<char *>					g_xUserCommandList;

CWHList<CEvent*>				g_xEventList;
CWHList<CEvent*>				g_xEventCloseList;

CWHList<CHolySeizeInfo*>		g_xHolySeizeList;

CWHList<CUserInfo*>				g_xLoginOutUserInfo;
CWHList<CReadyUserInfo*>		g_xReadyUserInfoList;
CWHList<CReadyUserInfo2*>		g_xReadyUserInfoList2;
CWHList<CMerchantObject*>		g_xMerchantObjList;
BOOL							g_fInitMerchant = FALSE;
CWHList<CNPCObject*>			g_xNPCObjList;
CWHList<CUserInfo*>				g_xReadyList;

CWHList<CGateInfo*>				g_xGateList;
CStaticArray<CUserInfo>			g_xUserInfoArr( 10000 );
CStaticArray<CPlayerObject>		g_xPlayerObjectArr( 10000 );
CWHList<CUserInfo*>				g_xUserInfoList;

int								g_nMirDayTime = 1;


// General Standard Data
CWHList<CMirMap*>			g_xMirMapList;					// Map List
CMoveMapEventInfo*			g_pMoveMapEventInfo = NULL;		// Map Event List
int							g_nNumOfMoveMapEventInfo = 0;
CMagicInfo*					g_pMagicInfo = NULL;			// Magic List
int							g_nNumOfMagicInfo = 0;
CMonsterGenInfo*			g_pMonGenInfo = NULL;			// Monster Gen List
int							g_nNumOfMonGenInfo = 0;
CMonRaceInfo*				g_pMonRaceInfo = NULL;			// Monster List
int							g_nNumOfMonRaceInfo = 0;
CStdItemSpecial*			g_pStdItemSpecial = NULL;		// Standard Item List
int							g_nStdItemSpecial = 0;
CStdItem*					g_pStdItemEtc = NULL;			// Standard General Item list
int							g_nStdItemEtc = 0;
CMerchantInfo*				g_pMerchantInfo = NULL;			// Merchant List
int							g_nNumOfMurchantInfo = 0;
int							g_nNumOfMapInfo;

BOOL						g_fTerminated = FALSE;

SOCKET						g_ssock = INVALID_SOCKET;
SOCKADDR_IN					g_saddr;

SOCKET						g_csock = INVALID_SOCKET;
SOCKADDR_IN					g_caddr;

SOCKET						g_clsock = INVALID_SOCKET;
SOCKADDR_IN					g_claddr;

_TSEARCHTABLE				g_SearchTable[_MAX_SEARCHTABLE];

char						g_szGoldName[8];

DWORD NEEDEXPS[MAXLEVEL] = {
								100,			//1
								200,			//2
								300,			//3
								400,			//4
								600,			//5
								900,			//6
								1200,			//7
								1700,			//8
								2500,			//9
								6000,			//10
								8000,			//11
								10000,			//12
								15000,			//13
								30000,			//14
								40000,			//15
								50000,			//16
								70000,			//17
								100000,			//18
								120000,			//19
								140000,			//20
								250000,			//21
								300000,			//22
								350000,			//23
								400000,			//24
								500000,			//25
								700000,			//26
								1000000,		//27
								1400000,		//28
								1800000,		//29
								2000000,		//30
								2400000,		//31
								2800000,		//32
								3200000,		//33
								3600000,		//34
								4000000,		//35
								4800000,		//36
								5600000,		//37
								8200000,		//38
								9000000,		//39
								12000000,		//40
								16000000,		//41
								30000000,		//42
								50000000,		//43
								80000000,		//44
								120000000,		//45
								480000000,		//46
								1000000000,		//47
								3000000000,		//48
								3500000000,		//49
								4000000000,		//50
								4000000000		//51
								};

HANDLE							g_hThreadForComm = NULL;
HANDLE							g_hSvrMsgEvnt = NULL;

CWHList<CScripterObject*>		g_xScripterList;
