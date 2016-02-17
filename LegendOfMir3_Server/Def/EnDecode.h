
#ifndef _LEGENDOFMIR_ENDECODE
#define _LEGENDOFMIR_ENDECODE

#define _DEFBLOCKSIZE		16

typedef struct tag_TDEFAULTMESSAGE
{
	int		nRecog;
	WORD	wIdent;
	WORD	wParam;
	WORD	wTag;
	WORD	wSeries;
} _TDEFAULTMESSAGE, *_LPTDEFAULTMESSAGE;

typedef struct tag_TMSGHEADER
{
	int		nCode;
	int		nSocket;
	WORD	wUserGateIndex;
	WORD	wIdent;
	WORD	wUserListIndex;
	WORD	wTemp;
	int		nLength;
} _TMSGHEADER, *_LPTMSGHEADER;

/*typedef struct tagTGateToSvrHeader
{
	char	szPrefix;				// Always is '%'
	char	szID;					// Identifier.
	BYTE	btGateIndex;			// Gate server index.
	int		nSocket;				// Socket(owner of msg) in gate server.
	WORD	wDataLength;			// Length of msg that following header.
} TGateToSvrHeader, *LPTGateToSvrHeader; 

#define GTS_HEADER_SIZE		sizeof(TGateToSvrHeader) */

//void WINAPI fnMakeDefMessage(_LPTDEFAULTMESSAGE lptdm, WORD wIdent, int nRecog, WORD wParam, WORD wTag, WORD wSeries);
#ifdef _UNICODE
#define fnEncode6BitBuf	fnEncode6BitBufW
#define fnDecode6BitBuf fnDecode6BitBufW
#define fnEncodeMessage fnEncodeMessageW
#define fnDecodeMessage fnDecodeMessageW
#define fnMakeDefMessage fnMakeDefMessageW
#else
#define fnEncode6BitBuf	fnEncode6BitBufA
#define fnDecode6BitBuf fnDecode6BitBufA
#define fnEncodeMessage fnEncodeMessageA
#define fnDecodeMessage fnDecodeMessageA
#define fnMakeDefMessage fnMakeDefMessageA
#endif

int  WINAPI fnEncode6BitBufW(unsigned char *pszSrc, TCHAR *pszDest, int nSrcLen, int nDestLen);
int  WINAPI fnDecode6BitBufW(TCHAR *pwszSrc, char *pszDest, int nDestLen);
int  WINAPI fnEncodeMessageW(_LPTDEFAULTMESSAGE lptdm, TCHAR *pszBuf, int nLen);
__inline void  WINAPI fnDecodeMessageW(_LPTDEFAULTMESSAGE lptdm, TCHAR *pszBuf)
	{ fnDecode6BitBufW(pszBuf, (char *)lptdm, sizeof(_TDEFAULTMESSAGE)); }

__inline void WINAPI fnMakeDefMessageW(_LPTDEFAULTMESSAGE lptdm, WORD wIdent, int nRecog, WORD wParam, WORD wTag, WORD wSeries)
	{ lptdm->wIdent	= wIdent; lptdm->nRecog	= nRecog; lptdm->wParam	= wParam; lptdm->wTag = wTag; lptdm->wSeries = wSeries; }

int  WINAPI fnEncode6BitBufA(unsigned char *pszSrc, char *pszDest, int nSrcLen, int nDestLen);
int  WINAPI fnDecode6BitBufA(char *pwszSrc, char *pszDest, int nDestLen);
int  WINAPI fnEncodeMessageA(_LPTDEFAULTMESSAGE lptdm, char *pszBuf, int nLen);
__inline void  WINAPI fnDecodeMessageA(_LPTDEFAULTMESSAGE lptdm, char *pszBuf)
	{ fnDecode6BitBufA(pszBuf, (char *)lptdm, sizeof(_TDEFAULTMESSAGE)); }

__inline void WINAPI fnMakeDefMessageA(_LPTDEFAULTMESSAGE lptdm, WORD wIdent, int nRecog, WORD wParam, WORD wTag, WORD wSeries)
	{ lptdm->wIdent	= wIdent; lptdm->nRecog	= nRecog; lptdm->wParam	= wParam; lptdm->wTag = wTag; lptdm->wSeries = wSeries; }

#endif //_LEGENDOFMIR_ENDECODE