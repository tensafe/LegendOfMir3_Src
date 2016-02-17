#include "stdafx.h"

extern HWND						g_hStatusBar;

extern SOCKET					g_csock;
extern HANDLE					g_hSvrMsgEvnt;

extern CWHQueue					g_SendToUserQ;
extern CWHQueue					g_SendToServerQ;

extern CWHDynamicArray<CSessionInfo>			g_UserInfoArray;

CWHAbusive						g_xAbusive;

DWORD WINAPI ThreadFuncForMsg(LPVOID lpParameter)
{
	int							nCount;
	_TDEFAULTMESSAGE			DefMsg;
	int							nBodyLen;
	_TMSGHEADER					MsgHdr;
	char						szDecodeSay[256];
	char						szEncodeSay[256];
	char						*pData;
	CSessionInfo*				pSessionInfo;
	DWORD						dwBytesSends;

	int							nPos;

	UINT						nIdentity = 0;
	MsgHdr.nCode	= 0xAA55AA55;
	MsgHdr.wIdent	= GM_DATA;

//	g_xAbusive.LoadAbusiveList();
	
	while(TRUE)
	{
		//Send packet to Game server.
		g_SendToServerQ.Lock();
		nCount = g_SendToServerQ.GetCount();

		if (nCount)
		{
			for (int nLoop = 0; nLoop < nCount; nLoop++)
			{
				_LPTSENDBUFF pSendBuff = (_LPTSENDBUFF)g_SendToServerQ.PopQ();

				if (pSendBuff)
				{
					if (pSessionInfo = g_UserInfoArray.GetData(pSendBuff->nGateIndex))
					{
						MsgHdr.nSocket			= (int)pSendBuff->sock;
						MsgHdr.wUserGateIndex	= pSendBuff->nGateIndex;
						MsgHdr.wUserListIndex	= pSessionInfo->nServerUserIndex;

						if (pSessionInfo->fLoginCode)
						{
							pSessionInfo->fLoginCode = FALSE;

//							MsgHdr.nLength = (memlen(pSendBuff->szData) - 1);
//							SendSocketMsgS (&MsgHdr, MsgHdr.nLength, , 0, NULL);					

							MsgHdr.nLength = sizeof(_TDEFAULTMESSAGE) + (memlen(pSendBuff->szData) - 1);

							SendSocketMsgS (&MsgHdr, sizeof(_TDEFAULTMESSAGE), (char *)&DefMsg, (memlen(pSendBuff->szData) - 1), pSendBuff->szData);
						}
						else
						{
							if (*pSendBuff->szData == '#')
							{
								if (memlen(pSendBuff->szData) > _DEFBLOCKSIZE + 2)
								{
									if (*(pSendBuff->szData + _DEFBLOCKSIZE + 2) == '!')	// 3[#? !]
									{
										*(pSendBuff->szData + _DEFBLOCKSIZE + 2) = '\0';
										fnDecodeMessageA(&DefMsg, (pSendBuff->szData + 2));		// 2[#?] ? = Code
		
										MsgHdr.nLength = sizeof(_TDEFAULTMESSAGE);
										SendSocketMsgS(&MsgHdr, sizeof(_TDEFAULTMESSAGE), (char *)&DefMsg, 0, NULL);
									}
									else
									{
										nBodyLen = memlen(pSendBuff->szData + _DEFBLOCKSIZE + 2) - 2;	// 2 = '!\0'

										pData = pSendBuff->szData + _DEFBLOCKSIZE + 2;				
										pData[nBodyLen] = '\0';

										fnDecodeMessageA(&DefMsg, (pSendBuff->szData + 2));		// 2[#?] ? = Code

										if (DefMsg.wIdent == CM_SAY)
										{
											nPos = fnDecode6BitBufA(pData, szDecodeSay, sizeof(szDecodeSay));
											szDecodeSay[nPos] = '\0';
											g_xAbusive.ChangeAbusiveText(szDecodeSay);
											nPos = fnEncode6BitBufA((unsigned char *)szDecodeSay, szEncodeSay, memlen(szDecodeSay) - 1, sizeof(szEncodeSay));
											szEncodeSay[nPos] = '\0';

											pData = &szEncodeSay[0];
										}

										MsgHdr.nLength = sizeof(_TDEFAULTMESSAGE) + nBodyLen;

										SendSocketMsgS (&MsgHdr, sizeof(_TDEFAULTMESSAGE), (char *)&DefMsg, nBodyLen, pData);
									}
								} // if (memlen(pSendBuff->szData) > _DEFBLOCKSIZE + 2)
							} // if '#'
						}
					}

					delete pSendBuff;
				}
			}// for nLoop
		}
		g_SendToServerQ.Unlock();

		//Send Packet to Client
		for (int nLoop = 0; nLoop < _MAX_USER_ARRAY; nLoop++)
		{
			pSessionInfo = g_UserInfoArray.GetData(nLoop);

			if (pSessionInfo)
			{
				if (pSessionInfo->nSendBufferLen)
				{
					WSABUF	Buf;

					Buf.len = memlen(pSessionInfo->SendBuffer) - 1;
					Buf.buf = pSessionInfo->SendBuffer;

					WSASend(pSessionInfo->sock, &Buf, 1, &dwBytesSends, 0, NULL, NULL);

					if (dwBytesSends < Buf.len)
						_RPT2(_CRT_WARN, "%d:%s\n", memlen(pSessionInfo->SendBuffer) - 1, pSessionInfo->SendBuffer);

/*#ifdef _DEBUG
					_RPT3(_CRT_WARN, "(%d)%d:%s\n", pSessionInfo->sock, memlen(pSessionInfo->SendBuffer) - 1, pSessionInfo->SendBuffer);
#endif*/
					pSessionInfo->nSendBufferLen = 0;
				}
			}
		}

		SleepEx(1, TRUE);
	}

	return 0;
}
