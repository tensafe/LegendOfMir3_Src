#include <windows.h>
#include <stdio.h>

static unsigned char Decode6BitMask[5] = { 0xfc, 0xf8, 0xf0, 0xe0, 0xc0 };


typedef struct tag_TDEFAULTMESSAGE
{
	int		nRecog;
	WORD	wIdent;
	WORD	wParam;
	WORD	wTag;
	WORD	wSeries;
} _TDEFAULTMESSAGE, *_LPTDEFAULTMESSAGE;

int memlen(const char *str)
{
    const char *eos = str;

    while(*eos++);

    return((int)(eos - str));
}

/* **************************************************************************************

		Encode/Decode Routine for ANSI character

   ************************************************************************************** */

int WINAPI fnEncode6BitBufA(unsigned char *pszSrc, char *pszDest, int nSrcLen, int nDestLen)
{
	int				nDestPos	= 0;
	int				nRestCount	= 0;
	unsigned char	chMade = 0, chRest = 0;

	for (int i = 0; i < nSrcLen; i++)
	{
		if (nDestPos >= nDestLen) break;
		
		chMade = ((chRest | (pszSrc[i] >> (2 + nRestCount))) & 0x3f);
		chRest = (((pszSrc[i] << (8 - (2 + nRestCount))) >> 2) & 0x3f);

		nRestCount += 2;

		if (nRestCount < 6)
			pszDest[nDestPos++] = chMade + 0x3c;
		else
		{
			if (nDestPos < nDestLen - 1)
			{
				pszDest[nDestPos++]	= chMade + 0x3c;
				pszDest[nDestPos++]	= chRest + 0x3c;
			}
			else
				pszDest[nDestPos++] = chMade + 0x3c;

			nRestCount	= 0;
			chRest		= 0;
		}
	}

	if (nRestCount > 0)
		pszDest[nDestPos++] = chRest + 0x3c;

//	pszDest[nDestPos] = '\0';

	return nDestPos;
}

int  WINAPI fnDecode6BitBufA(char *pszSrc, char *pszDest, int nDestLen)
{
	int				nLen = memlen((const char *)pszSrc) - 1;
	int				nDestPos = 0, nBitPos = 2;
	int				nMadeBit = 0;
	unsigned char	ch, chCode, tmp;

	for (int i = 0; i < nLen; i++)
	{
		if ((pszSrc[i] - 0x3c) >= 0)
			ch = pszSrc[i] - 0x3c;
		else
		{
			nDestPos = 0;
			break;
		}

		if (nDestPos >= nDestLen) break;

		if ((nMadeBit + 6) >= 8)
		{
			chCode = (tmp | ((ch & 0x3f) >> (6 - nBitPos)));
			pszDest[nDestPos++] = chCode;

			nMadeBit = 0;

			if (nBitPos < 6) 
				nBitPos += 2;
			else
			{
				nBitPos = 2;
				continue;
			}
		}

		tmp = ((ch << nBitPos) & Decode6BitMask[nBitPos - 2]);

		nMadeBit += (8 - nBitPos);
	}

//	pszDest[nDestPos] = '\0';

	return nDestPos;
}

int WINAPI fnEncodeMessageA(_LPTDEFAULTMESSAGE lptdm, char *pszBuf, int nLen)
{ return fnEncode6BitBufA((unsigned char *)lptdm, pszBuf, sizeof(_TDEFAULTMESSAGE), nLen); }

void  WINAPI fnDecodeMessageA(_LPTDEFAULTMESSAGE lptdm, char *pszBuf)
	{ fnDecode6BitBufA(pszBuf, (char *)lptdm, sizeof(_TDEFAULTMESSAGE)); }

void main()
{
	_TDEFAULTMESSAGE	DefMsg;
	_TDEFAULTMESSAGE	DefMsg2;
	char			szMsg[256];
	
	ZeroMemory(&DefMsg, sizeof(DefMsg));
	
	DefMsg.wSeries = MAKEWORD(2, 11);
	
	fnEncodeMessageA(&DefMsg, szMsg, sizeof(szMsg));
	
	fnDecodeMessageA(&DefMsg2, szMsg);
	
	printf("%d, %d", HIBYTE(DefMsg2.wSeries), LOBYTE(DefMsg2.wSeries));
	printf("%d\n", sizeof(_TDEFAULTMESSAGE));	
}
