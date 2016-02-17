#include "stdafx.h"

CWHQueue PacketQ;

CClientSocket::CClientSocket()
{
	m_sockClient = INVALID_SOCKET;
	m_pxDefProc	 = NULL;

	m_nSync = 1;
}


CClientSocket::~CClientSocket()
{
	m_sockClient = INVALID_SOCKET;
	m_pxDefProc	 = NULL;
}


unsigned long CClientSocket::GetINetAddr(CHAR* pszDotAddress)
{
	if ( inet_addr(pszDotAddress) == -1L )
	{
		struct hostent* pHostEnt;

		pHostEnt = gethostbyname(pszDotAddress);
		return *(unsigned long*)pHostEnt->h_addr_list[0];
	}
	
	return inet_addr(pszDotAddress);
}


BOOL CClientSocket::ConnectToServer(HWND hWnd, CHAR* szID, INT nPort)
{
	SOCKADDR_IN		sockAddr;
	CHAR strError[256]; 

	if ( m_sockClient == INVALID_SOCKET )
	{
		m_sockClient = socket(AF_INET, SOCK_STREAM, 0);

		if ( m_sockClient == INVALID_SOCKET )
		{
			wsprintf(strError, "socket() generated error %d\n", WSAGetLastError());
			OutputDebugString(strError);
			return FALSE;
		}

		sockAddr.sin_family		 = AF_INET;
		sockAddr.sin_port		 = htons( nPort );
		sockAddr.sin_addr.s_addr = inet_addr(szID);

		if ( WSAAsyncSelect(m_sockClient, hWnd, ID_SOCKCLIENT_EVENT_MSG, FD_CONNECT|FD_READ|FD_CLOSE ) == SOCKET_ERROR)
		{
			wsprintf(strError, "WSAAsyncSelect() generated error %d\n", WSAGetLastError());
			OutputDebugString(strError);
			return FALSE;
		}

		if ( connect(m_sockClient, (const struct sockaddr FAR*)&sockAddr, sizeof(sockAddr)) == SOCKET_ERROR )
		{
			switch ( WSAGetLastError() )
			{
				case WSAEWOULDBLOCK:
					OutputDebugString( "This socket is Non-Blocking Mode" );
					break;
			}
		}

		m_nSync = 1;
	}

	return TRUE;
}


BOOL CClientSocket::DisconnectToServer()
{
	if ( m_sockClient != INVALID_SOCKET )
	{
		closesocket(m_sockClient);
		m_sockClient = INVALID_SOCKET;

		return TRUE;
	}

	return FALSE;
}

/*
VOID CClientSocket::SendToServer(CHAR *pszPacket)
{
	CHAR				szPacket[1024];
	static INT nCode = 1;

	wsprintf(szPacket, "#%d%s!", nCode, pszPacket);

	if (nCode >= 9) nCode = 1;
	else
		nCode++;

	send(m_sockClient, szPacket, lstrlen(szPacket), 0);
}
*/

VOID CClientSocket::SendActMsg(WORD wIdent, INT nX, INT nY, INT nDir)
{
	fnMakeDefMessage(&DefMsg, wIdent, MAKELONG(nX, nY), 0, nDir, 0);
	SendPacket(&DefMsg, NULL);
}

VOID CClientSocket::SendHitMsg(WORD wIdent, INT nX, INT nY, INT nDir, WORD wHitStyle)
{
	fnMakeDefMessage(&DefMsg, wIdent, MAKELONG(nX, nY), 0, nDir, 0);
	SendPacket(&DefMsg, NULL);
}

VOID CClientSocket::SendRideMsg(WORD wIdent, INT nDir, BOOL bRide)
{
	// nDir  = wParam;
	// bRide = lParam1;
	fnMakeDefMessage(&DefMsg, wIdent, 0, nDir, bRide, 0);
	SendPacket(&DefMsg, NULL);
}


VOID CClientSocket::SendRunLogin(CHAR* szLoginID, CHAR* szCharName, INT nCertification, INT VersionNum)
{
	CHAR				szEncodeMsg[512];
	CHAR				szLoginInfo[512];
	CHAR				szPacket[512];

	wsprintf(szLoginInfo, "**%s/%s/%d/%d/1", szLoginID, szCharName, nCertification, VersionNum);

	int nPos = fnEncode6BitBuf((BYTE*)szLoginInfo, szEncodeMsg, lstrlen(szLoginInfo), sizeof(szEncodeMsg));
	szEncodeMsg[nPos] = '\0';

	((m_nSync >= 9) ? m_nSync = 1 : m_nSync++);
	wsprintf(szPacket, "#%d%s!", m_nSync, szEncodeMsg);

	send(m_sockClient, szPacket, strlen(szPacket), 0);
}

BOOL CClientSocket::CheckSocketError(LPARAM lParam)
{
	switch ( WSAGETSELECTERROR(lParam) )
	{
		case WSANOTINITIALISED:
//			MessageBox(NULL, "A successful WSAStartup must occur before using this function.", "", MB_OK);
			return FALSE;
		case WSAENETDOWN:
//			MessageBox(NULL, "The network subsystem has failed.", "", MB_OK);
			return FALSE;
		case WSAEADDRINUSE:
//			MessageBox(NULL, "The socket's local address is already in use and the socket was not marked to allow address reuse with SO_REUSEADDR. This error usually occurs when executing bind, but could be delayed until this function if the bind was to a partially wild-card address (involving ADDR_ANY) and if a specific address needs to be committed at the time of this function.", "", MB_OK);
			return FALSE;
		case WSAEINTR:
//			MessageBox(NULL, "The (blocking) Windows Socket 1.1 call was canceled through WSACancelBlockingCall.", "", MB_OK);
			return FALSE;
		case WSAEINPROGRESS:
//			MessageBox(NULL, "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function.", "", MB_OK);
			return FALSE;
		case WSAEALREADY:
//			MessageBox(NULL, "A nonblocking connect call is in progress on the specified socket. Note In order to preserve backward compatibility, this error is reported as WSAEINVAL to Windows Sockets 1.1 applications that link to either WINSOCK.DLL or WSOCK32.DLL.", "", MB_OK);
			return FALSE;
		case WSAEADDRNOTAVAIL:
//			MessageBox(NULL, "The remote address is not a valid address (such as ADDR_ANY).", "", MB_OK);
			return FALSE;
		case WSAEAFNOSUPPORT:
//			MessageBox(NULL, "Addresses in the specified family cannot be used with this socket.", "", MB_OK);
			return FALSE;
		case WSAECONNREFUSED:			// Can't Connect Server.
			return FALSE;
		case WSAETIMEDOUT:				// Time Out
		{
//			MessageBox(NULL, "Addresses in the specified family cannot be used with this socket.", "", MB_OK);
			return FALSE;
		}
		case WSAEFAULT:
//			MessageBox(NULL, "The name or the namelen parameter is not a valid part of the user address space, the namelen parameter is too small, or the name parameter contains incorrect address format for the associated address family.", "", MB_OK);
			return FALSE;
		case WSAEINVAL:
//			MessageBox(NULL, "The parameter s is a listening socket, or the destination address specified is not consistent with that of the constrained group the socket belongs to.", "", MB_OK);
			return FALSE;
		case WSAEISCONN:
//			MessageBox(NULL, "The socket is already connected (connection-oriented sockets only).", "", MB_OK);
			return FALSE;
		case WSAENETUNREACH:
//			MessageBox(NULL, "The network cannot be reached from this host at this time.", "", MB_OK);
			return FALSE;
		case WSAENOBUFS:
//			MessageBox(NULL, "No buffer space is available. The socket cannot be connected.", "", MB_OK);
			return FALSE;
		case WSAENOTSOCK:
//			MessageBox(NULL, "The descriptor is not a socket.", "", MB_OK);
			return FALSE;
		case WSAEWOULDBLOCK:
//			MessageBox(NULL, "The socket is marked as nonblocking and the connection cannot be completed immediately.", "", MB_OK);
			return FALSE;
		case WSAEACCES:
//			MessageBox(NULL, "Attempt to connect datagram socket to broadcast address failed because setsockopt option SO_BROADCAST is not enabled.", "", MB_OK);
			return FALSE;
	}

	return TRUE;
}


VOID CClientSocket::SendQueryMsg(WORD wIdent)
{
	fnMakeDefMessage(&DefMsg, wIdent, 0, 0, 0, 0);
	SendPacket(&DefMsg, NULL);
}


VOID CClientSocket::SendTakeOnOffItem(WORD wIdent, BYTE bEquipNum, CHAR* szItemName, INT nItemIdx)
{
	fnMakeDefMessage(&DefMsg, wIdent, nItemIdx, bEquipNum, 0, 0);
	SendPacket(&DefMsg, szItemName);
}


VOID CClientSocket::SendQueryName(INT nRecog, INT nX, INT nY)
{
	fnMakeDefMessage(&DefMsg, CM_QUERYUSERNAME, nRecog, nX, nY, 0);
	SendPacket(&DefMsg, NULL);
}


VOID CClientSocket::SendNPCClick(INT nRecog)
{
	fnMakeDefMessage(&DefMsg, CM_CLICKNPC, nRecog, 0, 0, 0);
	SendPacket(&DefMsg, NULL);
}


VOID CClientSocket::SendMagicKeyChange(INT nKeyNum, INT nMagicID)
{
	fnMakeDefMessage(&DefMsg, CM_MAGICKEYCHANGE, nMagicID, nKeyNum, 0, 0);
	SendPacket(&DefMsg, NULL);
}


VOID CClientSocket::SendItemIndex(WORD wIdent, INT nItemIndex, CHAR* pszItemName)
{
	fnMakeDefMessage(&DefMsg, wIdent, nItemIndex, 0, 0, 0);
	SendPacket(&DefMsg, pszItemName);
}

VOID CClientSocket::SendPickUp(INT nX, INT nY)
{
	fnMakeDefMessage(&DefMsg, CM_PICKUP, 0, nX, nY, 0);
	SendPacket(&DefMsg, NULL);
}

VOID CClientSocket::SendButchAnimal(INT nX, INT nY, INT nDir, INT nActorID)
{
	fnMakeDefMessage(&DefMsg, CM_BUTCH, nActorID, nX, nY, nDir);
	SendPacket(&DefMsg, NULL);
}

LRESULT CClientSocket::OnSocketMessage(WPARAM wParam, LPARAM lParam)
{
	if ( !CheckSocketError(lParam) )
	{
		PostQuitMessage(NULL);
		return FALSE;
	}

	switch ( WSAGETSELECTEVENT(lParam) )
	{
		case FD_CONNECT:
		{
			ZeroMemory(m_szBuff, sizeof(m_szBuff));
			m_nCurRecv = 0;

			if (m_pxDefProc)
				m_pxDefProc->OnConnectToServer();

			break;
		}
		case FD_CLOSE:
		{
			DisconnectToServer();
			
			break;
		}
		case FD_READ:
		{
			int		nRecv, nCurRecv = 0;

			if (g_bProcState == _GAME_PROC)
			{
				ioctlsocket((SOCKET)wParam, FIONREAD, (u_long *)&nRecv);

				char* pszPacket = new char[nRecv + 1];

				while (nRecv > nCurRecv)
					nCurRecv	+= recv((SOCKET)wParam, &pszPacket[nCurRecv], nRecv - nCurRecv, 0);

				pszPacket[nCurRecv] = '\0';

#ifdef _DEBUG
				_RPT1(_CRT_WARN, "RECV:%s\n", pszPacket);
#endif
				PacketQ.PushQ((BYTE*)pszPacket);
			}
			else
			{
				ioctlsocket((SOCKET)wParam, FIONREAD, (u_long *)&nRecv);
				
				while (nRecv > nCurRecv)
				{
					nCurRecv	+= recv((SOCKET)wParam, &m_szBuff[m_nCurRecv], nRecv - nCurRecv, 0);
					m_nCurRecv	+= nCurRecv;
				}

				m_szBuff[m_nCurRecv] = '\0';

				if (m_pxDefProc)
				{
					char *pszRemain = m_pxDefProc->OnMessageReceive(m_szBuff);

					if (pszRemain)
					{
						m_nCurRecv = strlen(pszRemain);
						memcpy(m_szBuff, pszRemain, m_nCurRecv);
					}
					else
					{
						m_nCurRecv = 0;
						ZeroMemory(m_szBuff, sizeof(m_szBuff));
					}
				}
			}

			break;
		}
	}

	return 0L;
}

VOID CClientSocket::SendPacket(_TDEFAULTMESSAGE* lpDefMsg, char *pszData)
{
	fnEncodeMessage(lpDefMsg, m_szEncodeDefMsg, sizeof(m_szEncodeDefMsg));

	((m_nSync >= 9) ? m_nSync = 1 : m_nSync++);

	if (pszData)
	{
		fnEncode6BitBuf((unsigned char *)pszData, m_szEncodeBody, strlen(pszData), sizeof(m_szEncodeBody));
		sprintf(m_szPacket, "#%d%s%s!", m_nSync, m_szEncodeDefMsg, m_szEncodeBody);
	}
	else
		sprintf(m_szPacket, "#%d%s!", m_nSync, m_szEncodeDefMsg);

	send(m_sockClient, m_szPacket, strlen(m_szPacket), 0);
}

// **************************************************************************************
//
//		For Login Process
//
// **************************************************************************************

VOID CClientSocket::OnLogin(char *pszID, char *pszPassword)
{
	char				szPacket[64];
	
	fnMakeDefMessage(&DefMsg, CM_IDPASSWORD, 0, 0, 0, 0);
	wsprintf(szPacket, "%s/%s", pszID, pszPassword);

	SendPacket(&DefMsg, szPacket);
}

VOID CClientSocket::OnSelectServer(char *pszServerName)
{
	fnMakeDefMessage(&DefMsg, CM_SELECTSERVER, 0, 0, 0, 0);
	SendPacket(&DefMsg, "ktest");
}

VOID CClientSocket::OnChangePassword(char *pszID, char *pszOldPassword, char *pszNewPassword)
{
	char				szPacket[64];
	char				cDivider;

	cDivider = 0x09;
	fnMakeDefMessage(&DefMsg, CM_CHANGEPASSWORD, 0, 0, 0, 0);

	wsprintf(szPacket, "%s%c%s%c%s", pszID, cDivider, pszOldPassword, cDivider, pszNewPassword);

	SendPacket(&DefMsg, szPacket);
}

VOID CClientSocket::OnNewAccount(char *pszNewAccount)
{
	fnMakeDefMessage(&DefMsg, CM_ADDNEWUSER, 0, 0, 0, 0);
	SendPacket(&DefMsg, pszNewAccount);
}

// **************************************************************************************
//
//		For Character Select Process
//
// **************************************************************************************

VOID CClientSocket::OnQueryChar()
{
	char szPacket[32];

	fnMakeDefMessage(&DefMsg, CM_QUERYCHR, 0, 0, 0, 0);

	wsprintf(szPacket, "%s/%d", g_szUserID, g_nCertifyCode);

	SendPacket(&DefMsg, szPacket);
}

VOID CClientSocket::OnNewChar( char *pszId, int nHair, int nJob, int nSex )
{
	char szPacket[64];

	fnMakeDefMessage(&DefMsg, CM_NEWCHR, 0, 0, 0, 0);

	wsprintf( szPacket, "%s/%d/%d/%d", pszId, nHair, nJob, nSex );

	SendPacket(&DefMsg, szPacket);

}

VOID CClientSocket::OnDelChar( char *pszCharName)
{
	fnMakeDefMessage(&DefMsg, CM_DELCHR, 0, 0, 0, 0);
	SendPacket(&DefMsg, pszCharName);
}

VOID CClientSocket::OnSelChar( char *pszCharName)
{
	char szPacket[32];

	fnMakeDefMessage(&DefMsg, CM_SELCHR, 0, 0, 0, 0);

	wsprintf( szPacket, "%s/%s", g_szUserID, pszCharName );

	SendPacket(&DefMsg, szPacket);
}



// **************************************************************************************
//
//		For Game Playing
//
// **************************************************************************************

void CClientSocket::SendNoticeOK()
{
	fnMakeDefMessage(&DefMsg, CM_LOGINNOTICEOK, 0, 0, 0, 0);
	SendPacket(&DefMsg, NULL);
}

VOID CClientSocket::SendSay(CHAR *pszSay)
{
	fnMakeDefMessage(&DefMsg, CM_SAY, 0, 0, 0, 0);
	SendPacket(&DefMsg, pszSay);
}

VOID CClientSocket::SendSpellMsg(INT nMagicID, INT nTargetX, INT nTargetY, INT nTargetID)
{
//wParam  : ¸¶¹ý ID(wtag)
//lParam1 : Å¸°ÙX
//lParam2 : Å¸°ÙY
//lParam3 : Å¸°Ù ¿ÀºêÁ§Æ® id(wTag)
//lParam4 : ...(wSeries)
	fnMakeDefMessage(&DefMsg, CM_SPELL, MAKELONG(nTargetX, nTargetY), LOWORD(nTargetID), nMagicID, HIWORD(nTargetID));
	SendPacket(&DefMsg, NULL);
}

VOID CClientSocket::SendOpenDoor(INT nX, INT nY, INT nIdx)
{
	fnMakeDefMessage(&DefMsg, CM_OPENDOOR, nIdx, nX, nY, 0);
	SendPacket(&DefMsg, NULL);
}

void CClientSocket::SendNPCMessage(WORD wIdent, DWORD dwNpcId, INT reg, char *pszMsg)
{
	fnMakeDefMessage(&DefMsg, wIdent, dwNpcId, 0, 0, reg);
	SendPacket(&DefMsg, pszMsg);
}

void CClientSocket::SendGetDetailItem(DWORD dwNpcId, INT nIndex, CHAR* szItemName)
{
	fnMakeDefMessage(&DefMsg, CM_USERGETDETAILITEM, dwNpcId,nIndex,0,0);
	SendPacket(&DefMsg, szItemName);
}

void CClientSocket::SendBuyItem(DWORD dwNpcId, CHAR* szItemName, CHAR* szItemIndex)
{
	fnMakeDefMessage(&DefMsg, CM_USERBUYITEM, dwNpcId, 0, 0, 0);
	SendPacket(&DefMsg, szItemName);
}

// *** For Group
void CClientSocket::SendCreateGroup(char *pszWho)
{
	fnMakeDefMessage(&DefMsg, CM_CREATEGROUP, 0, 0, 0, 0);
	SendPacket(&DefMsg, pszWho);
}

void CClientSocket::SendGroupMode(BOOL fFlag)
{
	if (fFlag)
		fnMakeDefMessage(&DefMsg, CM_GROUPMODE, 0, 1, 0, 0); // On
	else
		fnMakeDefMessage(&DefMsg, CM_GROUPMODE, 0, 0, 0, 0); // Off

	SendPacket(&DefMsg, NULL);
}

void CClientSocket::SendAddGroupMember(char *pszWho)
{
	fnMakeDefMessage(&DefMsg, CM_ADDGROUPMEMBER, 0, 0, 0, 0);
	SendPacket(&DefMsg, pszWho);
}

void CClientSocket::SendDelGroupMember(char *pszWho)
{
	fnMakeDefMessage(&DefMsg, CM_DELGROUPMEMBER, 0, 0, 0, 0);
	SendPacket(&DefMsg, pszWho);
}

// *** For Guild
void CClientSocket::SendGuildHome()
{
	fnMakeDefMessage(&DefMsg, CM_GUILDHOME, 0, 0, 0, 0);
	SendPacket(&DefMsg, NULL);
}
