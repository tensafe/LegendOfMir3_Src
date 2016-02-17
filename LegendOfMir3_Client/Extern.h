#ifndef _EXTERN_H
#define _EXTERN_H

extern CWHDXGraphicWindow	g_xMainWnd;
extern CLoginProcess		g_xLoginProc;
extern CCharacterProcess	g_xChrSelProc;
extern CGameProcess			g_xGameProc;
extern CSprite				g_xSpriteInfo;
extern CChatEdit			g_xChatEditBox;
extern CClientSocket		g_xClientSocket;
extern CMirSound			g_xSound;

extern BYTE					g_bProcState;
extern INT					g_nCertifyCode;			// Global Certification Code
extern char					g_szUserID[20];			// Global User ID
extern char					g_szServerIP[16];		// Global Current Connection Server IP
extern INT					g_nServerPort;			// Global Current Connection Server Port
extern char					g_szCharName[20];		// Global Charector Name
extern INT					g_nVersion;				// Global Client Program Version Number

#endif // _EXTERN_H