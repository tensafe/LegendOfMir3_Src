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

#define _WM_USER_MSG_INPUTTED		WM_USER + 1005  // 메시지가 커맨드일경우 wParam값은 TRUE로 세팅된다.

#define _LOGIN_PROC		0
#define _SVR_SEL_PROC	1
#define _CHAR_SEL_PROC	2
#define _GAME_PROC		3

#ifndef WM_MOUSEWHEEL
#define WM_MOUSEWHEEL WM_MOUSELAST+1 
#endif

#pragma comment(lib, "WS2_32.lib")
#pragma comment(lib,"Vfw32.lib")			// AVI
#pragma comment(lib, "strmbase.lib")		// DirectShow
#pragma comment(lib, "quartz.lib")			// DirectShow

#include <string>
#include <control.h>
#include <vfw.h>

#include "Resource.h"
#include ".\WindHorn\WHEngine.h"
#include "Define.h"
#include "GFun.h"

#include ".\Sound\TypeDef.h"
#include ".\Sound\Sound.h"
#include ".\Sound\SBuffer.h"
#include ".\Sound\BMMP3.h"
#include ".\Sound\MirSound.h"
#include ".\avi\avi.h"

#include ".\Common\EnDecode.h"
#include ".\Common\DLinkedList.h"
#include ".\Common\DblList.h"
#include ".\GameProcess\GameBtn.h"
#include ".\Common\MsgBox.h"
#include ".\Common\MirMsgBox.h"
#include ".\Common\MirMsgBox3D.h"
#include ".\Common\NoticeBox.h"
#include ".\Common\ChatEditBox.h"
#include ".\Common\ClientSocket.h"
#include ".\Common\ImageHandler.h"
#include ".\Common\Protocol.h"
#include ".\Common\MirButton.h"
#include ".\Common\Queue.h"
#include ".\GameProcess\SprDfn.h"
#include ".\GameProcess\Particle.h"

#include ".\GameProcess\ClientSysMsg.h"
#include ".\GameProcess\Item.h"
#include ".\GameProcess\WeatherEffect.h"
#include ".\GameProcess\MapHandler.h"
//#include ".\GameProcess\GameBtn.h"
#include ".\GameProcess\GameWnd.h"
#include ".\GameProcess\InventoryWnd.h"
#include ".\GameProcess\StatusWnd.h"
#include ".\GameProcess\ExchangeWnd.h"
#include ".\GameProcess\GuildMasterWnd.h"
#include ".\GameProcess\GuildWnd.h"
#include ".\GameProcess\GroupWnd.h"
#include ".\GameProcess\GroupPopWnd.h"
#include ".\GameProcess\ChatPopWnd.h"
#include ".\GameProcess\QuestWnd.h"
#include ".\GameProcess\OptionWnd.h"
#include ".\GameProcess\HorseWnd.h"

#include ".\Common\Typedeftxt.h"
#include ".\Common\Button.h"
#include ".\Common\NPCTxtAnalysis.h"
#include ".\GameProcess\NpcWnd.h"
#include ".\GameProcess\StoreWnd.h"

#include ".\GameProcess\LightFog.h"
#include ".\GameProcess\Actor.h"
#include ".\GameProcess\Magic.h"
#include ".\GameProcess\Interface.h"
#include ".\GameProcess\GameProc.h"

#include ".\LoginProcess\Common\typedef.h"
#include ".\LoginProcess\Common\define.h"
#include ".\LoginProcess\Common\BMWnd.h"
#include ".\LoginProcess\Intro\loginavi.h"
#include ".\LoginProcess\Login\Login.h"
#include ".\LoginProcess\New Account\NewAccount.h"
#include ".\LoginProcess\Select Srv\SelectSrv.h"
#include ".\LoginProcess\Patch.h"
#include ".\LoginProcess\LoginProcess.h"

#include ".\CharSelectProcess\common\define.h"
#include ".\CharSelectProcess\BackGround\BackGround.h"
#include ".\CharSelectProcess\CreateChr\CreateChr.h"
#include ".\CharSelectProcess\SelectChr\SelectChr.h"
#include ".\CharSelectProcess\CharacterProcess.h"


#include "Extern.h"

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__A9DB83DB_A9FD_11D0_BFD1_444553540000__INCLUDED_)

