#ifndef _GFUN_H_
#define _GFUN_H_



// 여러 Class에서 쓸수 있는 함수및 구조체를 공용으로 모아놓은 파일이다.



typedef struct tagCHAT
{
	DWORD	dwFontColor;
	DWORD	dwBackColor;
	CHAR	pszChat[MAX_PATH];
}CHAT, *LPCHAT;

typedef struct tagTEXTUREFILE
{
	WORD					wFileIdx;
	WORD					wImgIdx;
	LPDIRECTDRAWSURFACE7	lpddsTextr;
}TEXTUREFILE, *LPTEXTUREFILE;




DWORD GetChatColor(BYTE bIdx);

DWORD GetUserNameColor(BYTE bIdx);

#endif // _GFUN_H_
