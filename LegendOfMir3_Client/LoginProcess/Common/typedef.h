#ifndef	_LOGIN_TYPE_DEF
#define _LOGIN_TYPE_DEF


typedef struct	tagServerList
{
	char Caption[20];
	char Name[20];
}ServerList,* PServerList;

typedef struct tagIMAGEINFO
{
	UINT	nImgIdx;
	LONG	Left;
	LONG	Top;
}	_LOGINIMAGEINFO,* _PLOGINIMAGEINFO;

typedef struct tagSERVERBUTTON
{
	RECT		rcBtn;
	CHAR		szCaption[20];
	CHAR		szName[20];
	COLORREF	dwTxtColor;
	COLORREF	dwBkColor;
	INT			nAlpha;
} _SERVERBUTTON,*	_PSERVERBUTTON,	**_PPSERVERBUTTON;

typedef struct tagINPUTSTATE
{
	UINT	nID;
	LONG	Left;
	LONG	Top;
	LONG	Width;
	LONG	Height;
	INT		nHelptxt;
	INT		nSize;
	CHAR	szData[MAX_PATH];		
} INPUTSTATE;


#endif //	_LOGIN_TYPE_DEF