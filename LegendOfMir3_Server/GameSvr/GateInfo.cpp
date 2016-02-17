#include "stdafx.h"

void UpdateStatusBarUsers(BOOL fGrow);

CGateInfo::CGateInfo()
{
	m_fDoSending = FALSE;
	
	memset( &OverlappedEx, 0, sizeof( OverlappedEx ) );
}

void CGateInfo::SendGateCheck()
{
	_LPTSENDBUFF lpSendBuff = new _TSENDBUFF;

	if (lpSendBuff)
	{
		_TMSGHEADER		MsgHdr;

		MsgHdr.nCode			= 0xAA55AA55;
		MsgHdr.nSocket			= 0;
		MsgHdr.wUserGateIndex	= 0;
		MsgHdr.wIdent			= GM_CHECKSERVER;
		MsgHdr.wUserListIndex	= 0;
		MsgHdr.wTemp			= 0;
		MsgHdr.nLength			= 0;

		lpSendBuff->nLen = sizeof(_TMSGHEADER);

		memmove(lpSendBuff->szData, (char *)&MsgHdr, sizeof(_TMSGHEADER));
		lpSendBuff->szData[sizeof(_TMSGHEADER) + 1] = '\0';

//		Send(lpSendBuff);
		m_xSendBuffQ.PushQ((BYTE *)lpSendBuff);
	}
}

void CGateInfo::OpenNewUser(char *pszPacket)
{
	int					nIndex;
	_TMSGHEADER			MsgHdr;
	_LPTMSGHEADER		lpMsgHeader;

	nIndex = g_xUserInfoArr.GetFreeKey();

	if (nIndex >= 0)
	{
		CUserInfo * pUserInfo = &g_xUserInfoArr[nIndex];

		pUserInfo->Lock();

		g_xLoginOutUserInfo.AddNewNode(pUserInfo);

		lpMsgHeader = (_LPTMSGHEADER)pszPacket;

		pUserInfo->m_sock			= lpMsgHeader->nSocket;
		pUserInfo->m_pxPlayerObject	= NULL;

		ZeroMemory(pUserInfo->m_szUserID, sizeof(pUserInfo->m_szUserID));
		ZeroMemory(pUserInfo->m_szCharName, sizeof(pUserInfo->m_szCharName));

		pUserInfo->m_nCertification		= 0;
		pUserInfo->m_nClientVersion		= 0;
		pUserInfo->m_nUserGateIndex		= lpMsgHeader->wUserGateIndex;
		pUserInfo->m_nUserServerIndex	= nIndex;
		
		pUserInfo->m_pGateInfo			= this;

		pUserInfo->m_btCurrentMode		= USERMODE_NOTICE;

		pUserInfo->m_lpTHorseRcd		= NULL;

		pUserInfo->m_bEmpty = false;
		pUserInfo->Unlock();

		_LPTSENDBUFF lpSendBuff = new _TSENDBUFF;

		MsgHdr.nCode			= 0xAA55AA55;
		MsgHdr.nSocket			= lpMsgHeader->nSocket;
		MsgHdr.wUserGateIndex	= lpMsgHeader->wUserGateIndex;
		MsgHdr.wIdent			= GM_SERVERUSERINDEX;
		MsgHdr.wUserListIndex	= nIndex;
		MsgHdr.wTemp			= 0;
		MsgHdr.nLength			= 0;

		lpSendBuff->nLen		= sizeof(_TMSGHEADER);
		memmove(lpSendBuff->szData, (char *)&MsgHdr, sizeof(_TMSGHEADER));

		m_xSendBuffQ.PushQ((BYTE *)lpSendBuff);
//		Send(lpSendBuff);
/*		DWORD	dwBytesSends = 0;

		OverlappedEx[1].Buffer[sizeof(_TMSGHEADER)] = '\0';

		OverlappedEx[1].nOvFlag	 = OVERLAPPED_SEND;

		OverlappedEx[1].DataBuf.len = sizeof(_TMSGHEADER);
		OverlappedEx[1].DataBuf.buf = OverlappedEx[1].Buffer;

		WSASend(m_sock, &OverlappedEx[1].DataBuf, 1, &dwBytesSends, 0, (OVERLAPPED *)&OverlappedEx[1], NULL);
		WSASend(m_sock, &OverlappedEx[1].DataBuf, 1, &dwBytesSends, 0, NULL, NULL);
*/
		UpdateStatusBarUsers(TRUE);
	}
}

void CGateInfo::xSend()
{
	if (m_xSendBuffQ.GetCount())
	{
		DWORD	dwBytesSends = 0;
		int		nPos = 0;

		_LPTSENDBUFF lpSendBuff = (_LPTSENDBUFF)m_xSendBuffQ.PopQ();

		while (lpSendBuff)
		{
			memmove(&OverlappedEx[1].Buffer[nPos], lpSendBuff->szData, lpSendBuff->nLen);
			nPos += lpSendBuff->nLen;

			delete lpSendBuff;
			lpSendBuff = NULL;

			if (nPos >= 8192)
				break;

			lpSendBuff = (_LPTSENDBUFF)m_xSendBuffQ.PopQ();
		}

		if (nPos)
		{
			memset( &OverlappedEx[1].Overlapped, 0, sizeof( OVERLAPPED ) );
			OverlappedEx[1].nOvFlag		= OVERLAPPED_SEND;
			OverlappedEx[1].DataBuf.len	= nPos;
			OverlappedEx[1].DataBuf.buf	= OverlappedEx[1].Buffer;

			WSASend(
				m_sock, 
				&OverlappedEx[1].DataBuf, 
				1, 
				&dwBytesSends, 
				0, 
				(OVERLAPPED *) &OverlappedEx[1], 
//				NULL, 
				NULL
				);
		}
	}
}

int CGateInfo::Send(_LPTSENDBUFF lpSendBuff)
{
	DWORD	dwBytesSends = 0;
	int		nPos = 0;				 	
	int		nRet = 0;
	
	if ( lpSendBuff )
		m_xSendBuffQ.PushQ((BYTE *)lpSendBuff);

	if (m_fDoSending)
	{			  
		return 0;	   
	}
	
	_LPTSENDBUFF lpSBuff = (_LPTSENDBUFF)m_xSendBuffQ.PopQ();

	if ( !lpSBuff )	 	
	{			  
		return 0;	   
	}


	while (lpSBuff)
	{
		memmove(&OverlappedEx[1].Buffer[nPos], lpSBuff->szData, lpSBuff->nLen);
		nPos += lpSBuff->nLen;

		delete lpSBuff;

		if (nPos >= 4096)
			break;

		lpSBuff = (_LPTSENDBUFF)m_xSendBuffQ.PopQ();
	}

	if ( nPos )
	{
		memset( &OverlappedEx[1].Overlapped, 0, sizeof( OVERLAPPED ) );
		
		OverlappedEx[1].nOvFlag		= OVERLAPPED_SEND;
		OverlappedEx[1].DataBuf.len	= nPos;
		OverlappedEx[1].DataBuf.buf	= OverlappedEx[1].Buffer;

		int nRet =WSASend(m_sock, &OverlappedEx[1].DataBuf, 1, 
			&dwBytesSends, 0, (OVERLAPPED *) &OverlappedEx[1], NULL);

		m_fDoSending = TRUE;
	}

	return nRet;
}

int CGateInfo::Recv()
{
	DWORD nRecvBytes = 0, nFlags = 0;

	OverlappedEx[0].nOvFlag		= OVERLAPPED_RECV;
	OverlappedEx[0].DataBuf.len = DATA_BUFSIZE - OverlappedEx[0].bufLen;
	OverlappedEx[0].DataBuf.buf = OverlappedEx[0].Buffer + OverlappedEx[0].bufLen;

	memset( &OverlappedEx[0].Overlapped, 0, sizeof( OverlappedEx[0].Overlapped ) );

	return WSARecv( m_sock, &OverlappedEx[0].DataBuf, 1, &nRecvBytes, &nFlags, &OverlappedEx[0].Overlapped, 0 );
}

bool CGateInfo::HasCompletionPacket()
{
	if ( OverlappedEx[0].bufLen < sizeof( _TMSGHEADER ) )
		return false;

	_LPTMSGHEADER pMsgHeader = (_LPTMSGHEADER) &OverlappedEx[0].Buffer;

	return (unsigned) OverlappedEx[0].bufLen >= sizeof( _TMSGHEADER ) + pMsgHeader->nLength;
}

char * CGateInfo::ExtractPacket( char *pPacket )
{
	int packetLen = sizeof( _TMSGHEADER ) + ((_LPTMSGHEADER) &OverlappedEx[0].Buffer)->nLength;

	memcpy( pPacket, OverlappedEx[0].Buffer, packetLen );

	memmove( OverlappedEx[0].Buffer, OverlappedEx[0].Buffer + packetLen, DATA_BUFSIZE - packetLen );
	OverlappedEx[0].bufLen -= packetLen;

	return pPacket + packetLen;
}
