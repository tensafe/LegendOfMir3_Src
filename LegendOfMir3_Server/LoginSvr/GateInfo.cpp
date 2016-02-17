#include "stdafx.h"
#include "../def/dbmgr.h"
#include <stdio.h>

extern CWHList< GAMESERVERINFO* >	g_xGameServerList;
extern char							g_szServerList[1024];

int CompareDBString( char *str1, char *str2 )
{
	ChangeSpaceToNull( str1 );
	ChangeSpaceToNull( str2 );

	return strcmp( str1, str2 );
}

int GetCertification() 
{ 
	static long	g_nCertification = 30;

	InterlockedIncrement(&g_nCertification);

	if (g_nCertification >= 0x7FFFFFFF) 
		g_nCertification = 30; 
	
	return g_nCertification; 
}

/* **************************************************************************************

		Close
		
		PURPOSE : Send packet to login gate server.
		
		NOTE
		
		1. Packet construction : Packet % + client socket handle + / # + packet + ! + $

   ************************************************************************************** */

void CGateInfo::Close()
{
	PLISTNODE		pListNode;
	CUserInfo*		pUserInfo;

	if (xUserInfoList.GetCount())
	{
		pListNode = xUserInfoList.GetHead();

		while (pListNode)
		{
			pUserInfo = xUserInfoList.GetData(pListNode);

			if (pUserInfo)
			{
				delete pUserInfo;
				pUserInfo = NULL;

				pListNode = xUserInfoList.RemoveNode(pListNode);

				continue;
			}

			pListNode = xUserInfoList.GetNext(pListNode);
		}
	}

	closesocket(sock);
	sock = INVALID_SOCKET;
}

/* **************************************************************************************

		SendToGate 
		
		PURPOSE : Send packet to login gate server.
		
		NOTE
		
		1. Packet construction : Packet % + client socket handle + / # + packet + ! + $

   ************************************************************************************** */

void CGateInfo::SendToGate(SOCKET cSock, char *pszPacket)
{
	char	szData[256];
	WSABUF	buf;
	DWORD	dwSendBytes;
	
//	wsprintf(szData, _TEXT("%%%d/#%s!$"), (int)cSock, pszPacket);
	
	int nLen = memlen(pszPacket) - 1;

	szData[0] = '%';
	
	char *pszNext = ValToAnsiStr((int)cSock, &szData[1]);
	
	*pszNext++ = '/';
	*pszNext++ = '#';

	memmove(pszNext, pszPacket, nLen);

	pszNext += nLen;

	*pszNext++ = '!';
	*pszNext++ = '$';
	*pszNext++ = '\0';

	buf.len = pszNext - szData;
	buf.buf = szData;

	WSASend(sock, &buf, 1, &dwSendBytes, 0, NULL, NULL);
}

/* **************************************************************************************

		MakeNewUser
		
		PURPOSE : 
		
		NOTE

   ************************************************************************************** */

void CGateInfo::MakeNewUser(char *pszPacket)
{
	char				szDecodeMsg[256];
	char				szEncodeMsg[32];
	char				*pszID, *pszName, *pszPassword;
	_TDEFAULTMESSAGE	DefMsg;

	fnDecodeMessageA(&DefMsg, pszPacket);

	if (DefMsg.wIdent == CM_ADDNEWUSER)
	{
		int nPos = fnDecode6BitBufA((pszPacket + DEFBLOCKSIZE), szDecodeMsg, sizeof(szDecodeMsg));
		szDecodeMsg[nPos] = '\0';

		pszID		= &szDecodeMsg[0];
		
		pszName		= (char *)memchr(szDecodeMsg, '/', memlen(szDecodeMsg) - 1);
		*pszName = '\0';
		pszName++;

		pszPassword	= (char *)memchr(pszName, '/', memlen(pszName) - 1);
		*pszPassword = '\0';
		pszPassword++;

		if ((memlen(pszID) - 1) || (memlen(pszName) - 1) || (memlen(pszPassword) - 1))
		{
			char szQuery[1024];
			sprintf( szQuery, 
				"INSERT TBL_ACCOUNT( FLD_LOGINID, FLD_PASSWORD, FLD_USERNAME, FLD_CERTIFICATION ) "
				"VALUES( '%s', '%s', '%s', 0 )",
				pszID, pszPassword, pszName );

			CRecordset *pRec = GetDBManager()->CreateRecordset();
			if ( pRec->Execute( szQuery ) && pRec->GetRowCount() )
				fnMakeDefMessageA( &DefMsg, SM_NEWID_SUCCESS, 0, 0, 0, 0 );
			else
				fnMakeDefMessageA( &DefMsg, SM_NEWID_FAIL, 0, 0, 0, 0 );
			GetDBManager()->DestroyRecordset( pRec );
			// -----------------------------------------------------------------------------------
		}
		else
			fnMakeDefMessageA(&DefMsg, SM_NEWID_FAIL, 0, 0, 0, 0);

		fnEncodeMessageA(&DefMsg, szEncodeMsg, sizeof(szEncodeMsg));

		szDecodeMsg[0] = '#';
		memmove(&szDecodeMsg[1], szEncodeMsg, DEFBLOCKSIZE);
		szDecodeMsg[DEFBLOCKSIZE + 1] = '!';
		szDecodeMsg[DEFBLOCKSIZE + 2] = '\0';

		send(sock, szDecodeMsg, DEFBLOCKSIZE + 2, 0); 
	}
}

/* **************************************************************************************

		ReceiveServerMsg
		
		PURPOSE : 
		
		NOTE

   ************************************************************************************** */

void CGateInfo::ReceiveServerMsg(char *pszPacket)
{
	char		*pszPos;
	int			nCertification;
	int			nLen = memlen(pszPacket);

	if (pszPos = (char *)memchr(pszPacket, '/', nLen))
	{
		*pszPos++ = '\0';
		nCertification = AnsiStrToVal(pszPos);

		char szQuery[256];
		sprintf( szQuery, 
			"UPDATE TBL_ACCOUNT SET FLD_CERTIFICATION=%d WHERE FLD_LOGINID='%s'", 
			nCertification, pszPacket );

		CRecordset *pRec = GetDBManager()->CreateRecordset();
		pRec->Execute( szQuery );
		GetDBManager()->DestroyRecordset( pRec );
	}
}

void CGateInfo::ReceiveOpenUser(char *pszPacket)
{
	char	*pszPos;
	int		nSocket;
	int		nLen = memlen(pszPacket);

	if (pszPos = (char *)memchr(pszPacket, '/', nLen))
	{
		nSocket = AnsiStrToVal(pszPacket);

		pszPos++;

		CUserInfo* pUserInfo = new CUserInfo;

		if (pUserInfo)
		{
			MultiByteToWideChar(CP_ACP, 0, pszPacket, -1, pUserInfo->szSockHandle, sizeof(pUserInfo->szSockHandle)/sizeof(TCHAR));
			MultiByteToWideChar(CP_ACP, 0, pszPos, -1, pUserInfo->szAddress, sizeof(pUserInfo->szAddress)/sizeof(TCHAR));

			pUserInfo->sock					= nSocket;
			pUserInfo->nCertification		= 0;
			pUserInfo->nClientVersion		= 0;
			pUserInfo->fSelServerOk			= FALSE;

			ZeroMemory(pUserInfo->szUserID, sizeof(pUserInfo->szUserID));

			xUserInfoList.AddNewNode(pUserInfo);

			InsertLogMsgParam(IDS_OPEN_USER, pUserInfo->szAddress);
		}
	} 
}

/* **************************************************************************************

		ReceiveCloseUser
		
		PURPOSE : 
		
		NOTE

   ************************************************************************************** */

void CGateInfo::ReceiveCloseUser(char *pszPacket)
{
	int nSocket = AnsiStrToVal(pszPacket);
}

/* **************************************************************************************

		ReceiveSendUser
		
		PURPOSE : 
		
		NOTE

   ************************************************************************************** */

void CGateInfo::ReceiveSendUser(char *pszPacket)
{
	char	*pszPos;
	int		nSocket;
	int		nLen = memlen(pszPacket);

	if ((pszPos = (char *)memchr(pszPacket, '/', nLen)))
	{
		nSocket = AnsiStrToVal(pszPacket);

		pszPos++;

		_LPTSENDBUFF lpSendUserData = new _TSENDBUFF;

		lpSendUserData->sock		= (SOCKET)nSocket;

		memmove(lpSendUserData->szData, pszPos, memlen(pszPos));

		g_SendToGateQ.PushQ((BYTE *)lpSendUserData);
	}
}

/* **************************************************************************************

		ProcSelectServer
		
		PURPOSE : 
		
		NOTE

   ************************************************************************************** */

void CGateInfo::ProcSelectServer(SOCKET s, WORD wServerIndex)
{
	_TDEFAULTMESSAGE	DefMsg;
	char				szEncodePacket[128];
	char				szEncodeAllPacket[256];
	char				szEncodeMsg[24];
	char				*pServerIP;
	GAMESERVERINFO		*pServerInfo;

	PLISTNODE pListNode = xUserInfoList.GetHead();

	while (pListNode)
	{
		CUserInfo *pUserInfo = xUserInfoList.GetData(pListNode);

		if (pUserInfo->sock == s)
		{
			if (!pUserInfo->fSelServerOk)
			{
				fnMakeDefMessageA(&DefMsg, SM_SELECTSERVER_OK, 0, pUserInfo->nCertification, 0, 0);		
				int nPos = fnEncodeMessageA(&DefMsg, szEncodeMsg, sizeof(szEncodePacket));
				szEncodeMsg[nPos] = '\0';

				for ( PLISTNODE pNode = g_xGameServerList.GetHead(); pNode; pNode = g_xGameServerList.GetNext( pNode ) )
				{
					pServerInfo = g_xGameServerList.GetData( pNode );
					
					if ( pServerInfo->index == wServerIndex )
					{
						pServerIP = pServerInfo->ip;
						pServerInfo->connCnt++;
						break;
					}
				}

				if ( !pServerIP )
					break;

				pUserInfo->nServerID = wServerIndex;

				int nPos2 = fnEncode6BitBufA((unsigned char *)pServerIP, szEncodePacket, memlen(pServerIP), sizeof(szEncodePacket));
				szEncodePacket[nPos2] = '\0';

				memmove(szEncodeAllPacket, szEncodeMsg, nPos);
				memmove(&szEncodeAllPacket[nPos], szEncodePacket, nPos2);
				szEncodeAllPacket[nPos + nPos2] = '\0';

				SendToGate(s, szEncodeAllPacket);

				pUserInfo->fSelServerOk = TRUE;

				pListNode = xUserInfoList.RemoveNode(pListNode);
			}
		}
		else
			pListNode = xUserInfoList.GetNext(pListNode);
	}
}

/* **************************************************************************************

		ParseUserEntry
		
		PURPOSE : 
		
		NOTE

   ************************************************************************************** */

bool CGateInfo::ParseUserEntry( char *buf, _AUSERENTRYINFO *userInfo )
{
	char seps[] = "\001";
	char *token = strtok( buf, seps );
	int  step   = 0;
	
	__try
	{
		while ( token )
		{
			switch ( step++ )
			{
				case 0: strcpy( userInfo->szLoginID, token );
				case 1: strcpy( userInfo->szPassword, token ); 
				case 2: strcpy( userInfo->szUserName, token );
				case 3: strcpy( userInfo->szSSNo, token );
				case 4: strcpy( userInfo->szBirthDay, token ); 
				case 5: strcpy( userInfo->szZipCode, token );
				case 6: strcpy( userInfo->szAddress1, token ); 
				case 7: strcpy( userInfo->szAddress2, token );
				case 8: strcpy( userInfo->szPhone, token ); 
				case 9: strcpy( userInfo->szMobilePhone, token ); 
				case 10: strcpy( userInfo->szEmail, token ); 
				case 11: strcpy( userInfo->szQuiz, token ); 
				case 12: strcpy( userInfo->szAnswer, token ); 
				case 13: strcpy( userInfo->szQuiz2, token ); 
				case 14: strcpy( userInfo->szAnswer2, token ); 
			}	
			
			token = strtok( NULL, seps );
		}
	}
	__except ( EXCEPTION_EXECUTE_HANDLER )
	{
		return false;
	}
	
	return step >= 15;
}

/* **************************************************************************************

		ProcAddUser
		
		PURPOSE : 
		
		NOTE

   ************************************************************************************** */

void CGateInfo::ProcAddUser(SOCKET s, char *pszData)
{
	char				szEntryInfo[2048];
	_AUSERENTRYINFO		UserEntryInfo;
	_TDEFAULTMESSAGE	DefMsg;
	char				szEncodePacket[64];

	int len = fnDecode6BitBufA(pszData, (char *)&szEntryInfo, sizeof(szEntryInfo));
	szEntryInfo[len] = '\0';

	if ( !ParseUserEntry( szEntryInfo, &UserEntryInfo ) )
		fnMakeDefMessageA(&DefMsg, SM_NEWID_FAIL, 0, 0, 0, 0);
	else
	{	
		char szQuery[1024];
		sprintf( szQuery, 
			"SELECT * FROM TBL_ACCOUNT WHERE FLD_LOGINID='%s'",
			UserEntryInfo.szLoginID );

		CRecordset *pRec = GetDBManager()->CreateRecordset();

		if (!pRec->Execute( szQuery ))
			fnMakeDefMessageA(&DefMsg, SM_NEWID_FAIL, 0, 0, 0, 0);

		if ( pRec->Fetch() )
			fnMakeDefMessageA(&DefMsg, SM_NEWID_FAIL, 0, 0, 0, 0);
		else
		{
			GetDBManager()->DestroyRecordset( pRec );

			pRec = GetDBManager()->CreateRecordset();

			sprintf( szQuery, 
				"INSERT TBL_ACCOUNT(FLD_LOGINID, FLD_PASSWORD, FLD_USERNAME, FLD_CERTIFICATION) "
				"VALUES( '%s', '%s', '%s', 0 )",
				UserEntryInfo.szLoginID, 
				UserEntryInfo.szPassword, 
				UserEntryInfo.szUserName );

			pRec->Execute( szQuery );

			sprintf( szQuery,
				"INSERT TBL_ACCOUNTADD(FLD_LOGINID, FLD_SSNO, FLD_BIRTHDAY, FLD_ADDRESS1, FLD_ADDRESS2, "
				                      "FLD_PHONE, FLD_MOBILEPHONE, FLD_EMAIL, FLD_QUIZ1, FLD_ANSWER1, FLD_QUIZ2, FLD_ANSWER2) "
				"VALUES( '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s', '%s' )",
				UserEntryInfo.szLoginID,
				UserEntryInfo.szSSNo,
				UserEntryInfo.szBirthDay,
				UserEntryInfo.szAddress1,
				UserEntryInfo.szAddress2,
				UserEntryInfo.szPhone,
				UserEntryInfo.szMobilePhone,
				UserEntryInfo.szEmail,
				UserEntryInfo.szQuiz,
				UserEntryInfo.szAnswer,
				UserEntryInfo.szQuiz2,
				UserEntryInfo.szAnswer2 );

			pRec->Execute( szQuery );
		}

		GetDBManager()->DestroyRecordset( pRec );
				
		fnMakeDefMessageA(&DefMsg, SM_NEWID_SUCCESS, 0, 0, 0, 0);
	
		TCHAR szID[32];
		MultiByteToWideChar( CP_ACP, 0, UserEntryInfo.szLoginID, -1, szID, sizeof( szID ) / sizeof( TCHAR ) );
		InsertLogMsgParam(IDS_COMPLETENEWUSER, szID);
	}		
	
	fnEncodeMessageA(&DefMsg, szEncodePacket, sizeof(szEncodePacket));
	SendToGate(s, szEncodePacket);
}

/* **************************************************************************************

		ProcLogin
		
		PURPOSE : 
		
		NOTE

   ************************************************************************************** */

void CGateInfo::ProcLogin(SOCKET s, char *pszData)
{
	char				szIDPassword[32];
	char				*pszID, *pszPassword;
	char				szEncodePacket[64];
	_TDEFAULTMESSAGE	DefMsg;
	int					nPos;
	char				szQuery[256];

	if (memlen(pszData) - 1 <= 0) return;
	
	PLISTNODE pListNode = xUserInfoList.GetHead();

	while (pListNode)
	{
		CUserInfo *pUserInfo = xUserInfoList.GetData(pListNode);

		if (pUserInfo->sock == s)
		{
			int nDecodeLen = fnDecode6BitBufA(pszData, szIDPassword, sizeof(szIDPassword));
			szIDPassword[nDecodeLen] = '\0';

			pszID		= &szIDPassword[0];

			if (pszPassword	= (char *)memchr(szIDPassword, '/', sizeof(szIDPassword)))
			{
				*pszPassword = '\0';
				pszPassword++;

				sprintf( szQuery, "SELECT * FROM TBL_ACCOUNT WHERE FLD_LOGINID='%s'", pszID );

				CRecordset *pRec = GetDBManager()->CreateRecordset();

				if ( !pRec->Execute( szQuery ) || !pRec->Fetch() )
					fnMakeDefMessageA( &DefMsg, SM_ID_NOTFOUND, 0, 0, 0, 0 );
				else if ( CompareDBString( pszPassword, pRec->Get( "FLD_PASSWORD" ) ) != 0 )
					fnMakeDefMessageA( &DefMsg, SM_PASSWD_FAIL, 0, 0, 0, 0 );
				else
				{
					int nCertCode = atoi( pRec->Get( "FLD_CERTIFICATION" ) );
		/*
					if ( nCertCode > 0 && nCertCode < 30 )
						fnMakeDefMessageA(&DefMsg, SM_CERTIFICATION_FAIL, (nCertCode + 1), 0, 0, 0);
					else if ( nCertCode >= 30 )
						fnMakeDefMessageA(&DefMsg, SM_CERTIFICATION_FAIL, 1, 0, 0, 0);
					else*/
					{
						char szEncodeServerList[512];
						char szEncodeAllPacket[1024];
						
						fnMakeDefMessageA(&DefMsg, SM_PASSOK_SELECTSERVER, 0, 1, 0, 0);		
						nPos = fnEncodeMessageA(&DefMsg, szEncodePacket, sizeof(szEncodePacket));
						szEncodePacket[nPos] = '\0';
						
						int nPos2 = fnEncode6BitBufA((unsigned char *)g_szServerList, szEncodeServerList, memlen(g_szServerList), sizeof(szEncodeServerList));
						szEncodeServerList[nPos2] = '\0';
						
						memmove(szEncodeAllPacket, szEncodePacket, nPos);
						memmove(&szEncodeAllPacket[nPos], szEncodeServerList, memlen(szEncodeServerList));
						
						SendToGate(s, szEncodeAllPacket);
										
						GetDBManager()->DestroyRecordset( pRec );

						pUserInfo->nCertification = GetCertification();

		//				pRec = GetDBManager()->CreateRecordset();
		//				sprintf( szQuery, 
		//					"UPDATE TBL_ACCOUNT SET FLD_CERTIFICATION=%d WHERE FLD_LOGINID='%s'",
		//					GetCertification(), pszID );
		//				pRec->Execute( szQuery );
						
		//				GetDBManager()->DestroyRecordset( pRec );

						return;
					}
				}

				GetDBManager()->DestroyRecordset( pRec );

				nPos = fnEncodeMessageA(&DefMsg, szEncodePacket, sizeof(szEncodePacket));
				szEncodePacket[nPos] = '\0';

				SendToGate(s, szEncodePacket);
			}
		}

		pListNode = xUserInfoList.GetNext(pListNode);
	}
}
