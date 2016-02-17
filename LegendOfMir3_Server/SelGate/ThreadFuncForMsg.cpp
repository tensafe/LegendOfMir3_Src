#include "stdafx.h"

extern HWND								g_hStatusBar;

extern SOCKET							g_csock;

extern CWHQueue							g_xMsgQueue;
extern CWHList<CSessionInfo*>			g_xSessionList;

extern BOOL								g_fTerminated;

DWORD WINAPI ThreadFuncForMsg(LPVOID lpParameter)
{
	char			szData[DATA_BUFSIZE];
	char			szRemainData[DATA_BUFSIZE];
	char			*pszData;
	char			*pszFirst, *pszEnd;
	int				nCount = 0, nDataPos = 0, nRemainDataLen = 0, nRemain = 0;
	int				nLoop;
	int				nSocket;
	
	FILETIME		ftKernelTimeStart, ftKernelTimeEnd;
	FILETIME		ftUserTimeStart, ftUserTimeEnd;
	FILETIME		ftDummy, ftTotalTimeElapsed;
	__int64			qwKernelTimeElapsed, qwUserTimeElapsed, qwTotalTimeElapsed;
	TCHAR			wszThreadTime[32];

	WSABUF			Buf;
	DWORD			dwSendBytes = 0;

	while (TRUE)
	{
		GetThreadTimes(GetCurrentThread(), &ftDummy, &ftDummy, &ftKernelTimeStart, &ftUserTimeStart);

		if (g_fTerminated)
			return 0;

		nCount		= g_xMsgQueue.GetCount();
		nDataPos	= 0;
		pszFirst	= NULL;

		if (nCount)
		{
			if (nRemainDataLen)
			{
				memmove(szData, szRemainData, nRemainDataLen);
				nDataPos = nRemainDataLen;
			}

			for (nLoop = 0; nLoop < nCount; nLoop++)
			{
				pszData = (char *)g_xMsgQueue.PopQ();

				if (!pszData) continue;

				if (pszFirst)
				{
					nRemain = memlen(pszFirst);

					if (nRemain)
					{
						memmove(szData, pszFirst, nRemain);
						nDataPos = 0;
					}
				}
				else
					nDataPos = 0;

				memmove((szData + nDataPos), pszData, memlen(pszData));

				pszEnd = &szData[0];

				while (TRUE)
				{
					if ((pszFirst = (char *)memchr(pszEnd, '%', memlen(pszEnd))) && (pszEnd = (char *)memchr(pszFirst, '$', memlen(pszFirst))))
					{
						*pszEnd = '\0';

						if (*(pszFirst + 1) == '+')
						{
							if (*(pszFirst + 2) == '-')
								InsertLogMsg(_TEXT("Kick User."));
							else
								SendMessage(g_hStatusBar, SB_SETTEXT, MAKEWORD(2, 0), (LPARAM)_TEXT("Activation"));	// Received keep alive check code from login server 
						}
						else
						{
							if (pszEnd = strchr(pszFirst, '/'))
							{
								pszEnd++;

								if (nSocket = AnsiStrToVal(pszFirst + 1))
								{
									Buf.len	= memlen(pszEnd) - 1;
									Buf.buf	= pszEnd;

									WSASend((SOCKET)nSocket, &Buf, 1, &dwSendBytes, 0, NULL, NULL);
//									send((SOCKET)nSocket, pszEnd, memlen(pszEnd) - 1, 0);
								}
							}
						}
					}
					else
						break;
				} // while loop

				delete [] pszData;
			} // for loop

			if (pszFirst)
			{
				nRemain = memlen(pszFirst);

				if (nRemain)
				{
					memmove(szRemainData, pszFirst, nRemain);
					nRemainDataLen = nRemain;
				}
				else
					nRemainDataLen = 0;
			}
		} // if (nCount)

		GetThreadTimes(GetCurrentThread(), &ftDummy, &ftDummy, &ftKernelTimeEnd, &ftUserTimeEnd);

		qwKernelTimeElapsed = FileTimeToQuadWord(&ftKernelTimeEnd) - FileTimeToQuadWord(&ftKernelTimeStart);
		qwUserTimeElapsed	= FileTimeToQuadWord(&ftUserTimeEnd) - FileTimeToQuadWord(&ftUserTimeStart);
		
		qwTotalTimeElapsed = qwKernelTimeElapsed + qwUserTimeElapsed;

		QuadTimeToFileTime(qwTotalTimeElapsed, &ftTotalTimeElapsed);

		wsprintf(wszThreadTime, _TEXT("%u%u ns"), ftTotalTimeElapsed.dwHighDateTime, ftTotalTimeElapsed.dwLowDateTime);
		
		SendMessage(g_hStatusBar, SB_SETTEXT, MAKEWORD(4, 0), (LPARAM)wszThreadTime);

		SleepEx(1, TRUE);
	}

	return 0;
}
