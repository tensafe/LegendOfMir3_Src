#include <windows.h>
#include <stdio.h>
#include "..\Def\Protocol.h"

int WINAPI fnEncode6BitBufA(unsigned char *pszSrc, char *pszDest, int nSrcLen, int nDestLen)
{
	int             nDestPos    = 0;
	int             nRestCount  = 0;
	unsigned char   chMade = 0, chRest = 0;

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
				pszDest[nDestPos++] = chMade + 0x3c;
				pszDest[nDestPos++] = chRest + 0x3c;
			}
			else
				pszDest[nDestPos++] = chMade + 0x3c;

			nRestCount  = 0;
			chRest      = 0;
		}
	}

	if (nRestCount > 0)
		pszDest[nDestPos++] = chRest + 0x3c;

//  pszDest[nDestPos] = '\0';

	return nDestPos;
}

void main()
{
//  _TUSERITEMRCD   t;
//    _TGENITEMRCD  t;
//    _THUMANMAGICRCD t;        
	_THUMANRCD  t;  
//  _TCHARDESC t;    
//    _THUMANMAGICRCD t;
	
	char        szEncode[1024];
	
	int nPos = fnEncode6BitBufA((unsigned char *)&t, szEncode, sizeof(t), sizeof(szEncode));
	
	printf("%d\n", nPos);
}
