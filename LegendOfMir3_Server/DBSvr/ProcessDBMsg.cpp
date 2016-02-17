#include "stdafx.h"
#include "processdbmsg.h"
#include <stdio.h>
#include "../def/dbmgr.h"

extern CWHQueue					g_DBMsgQ;

extern BOOL						g_fTerminated;

extern CWHList< GAMESERVERINFO * >	g_xGameServerList;

void SendSocket(SOCKET s, int nCertification, char *pszData1, char *pszData2, int nData2Len)
{
	int		nLen = nData2Len + DEFBLOCKSIZE + 6;
	LONG	lCert = MAKELONG(nCertification ^ 0xAA, nLen);
	char	szCert[24];
	char	szEncodeMsg[8096];

	int nCertLen = fnEncode6BitBufA((unsigned char *)&lCert, szCert, sizeof(LONG), sizeof(szCert));

	szEncodeMsg[0] = '#';
	char *pszPos = ValToAnsiStr(nCertification, &szEncodeMsg[1]);

	*pszPos++ = '/';

	memmove(pszPos, pszData1, DEFBLOCKSIZE);
	pszPos += DEFBLOCKSIZE;
	memmove(pszPos, pszData2, nData2Len);
	pszPos += nData2Len;
	memmove(pszPos, szCert, nCertLen);

	pszPos		+= nCertLen;

	*pszPos++	= '!';
	*pszPos		= '\0';

	int nSendLen = memlen(szEncodeMsg) - 1;

	send(s, szEncodeMsg, nSendLen, 0);
}

int GetHorseRcd(char *szName, _LPTHORSERCD lpTHorseRcd)
{
	char szQuery[1024];
	sprintf( szQuery, "SELECT * FROM TBL_CHARACTER_HORSE WHERE FLD_CHARNAME='%s'", szName );

	CRecordset *pRec = GetDBManager()->CreateRecordset();
	if ( pRec->Execute( szQuery ) && pRec->Fetch() )
	{
		strcpy( lpTHorseRcd->szHorseIndex, pRec->Get( "FLD_HORSEINDEX" ) );
		lpTHorseRcd->btHorseType = atoi( pRec->Get( "FLD_HORSETYPE" ) );
	}
	else
	{
		GetDBManager()->DestroyRecordset( pRec );
		return 0;
	}
	GetDBManager()->DestroyRecordset( pRec );

	return 1;
}

void GetHumanGenItemRcd(char *szName, CWHList<_LPTGENITEMRCD>	*pxUserGenItemRcdList)
{
	char szQuery[128];

	sprintf( szQuery, "SELECT * FROM TBL_CHARACTER_GENITEM WHERE FLD_CHARNAME='%s'", szName );

	CRecordset *pRec = GetDBManager()->CreateRecordset();
	
	if ( pRec->Execute( szQuery ) )
	{		
		while ( pRec->Fetch() )
		{
			_LPTGENITEMRCD pItemRcd = new _TGENITEMRCD;

			if ( pItemRcd )
			{
				strcpy( pItemRcd->szItem, pRec->Get( "FLD_ITEMINDEX" ) );
				pxUserGenItemRcdList->AddNewNode( pItemRcd );
			}
		}
	}

	GetDBManager()->DestroyRecordset( pRec );
}

void GetHumanMagicRcd(char *szName, CWHList<_LPTHUMANMAGICRCD>	*pxUserMagicRcdList)
{
	char szQuery[1024];

	sprintf( szQuery, "SELECT * FROM TBL_CHARACTER_MAGIC WHERE FLD_CHARNAME='%s'", szName );

	CRecordset *pRec = GetDBManager()->CreateRecordset();
	
	if (pRec->Execute( szQuery ))
	{
		while (pRec->Fetch())
		{
			_LPTHUMANMAGICRCD	lptUserMagicRcd = new _THUMANMAGICRCD;
			
			if (lptUserMagicRcd)
			{
				lptUserMagicRcd->btMagicID	= atoi( pRec->Get( "FLD_MAGICID" ) );
				lptUserMagicRcd->btLevel	= atoi( pRec->Get( "FLD_LEVEL" ) );
				lptUserMagicRcd->btUseKey	= atoi( pRec->Get( "FLD_USEKEY" ) );
				lptUserMagicRcd->nCurrTrain	= atoi( pRec->Get( "FLD_CURRTRAIN" ) );
				
				pxUserMagicRcdList->AddNewNode(lptUserMagicRcd);
			}
		}
	}

	GetDBManager()->DestroyRecordset( pRec );
}

void GetHumanItemRcd(char *szName, CWHList<_LPTUSERITEMRCD>	*pxUserItemRcdList)
{
	char szQuery[128];

	sprintf( szQuery, "SELECT * FROM TBL_CHARACTER_ITEM WHERE FLD_CHARNAME='%s'", szName );

	CRecordset *pRec = GetDBManager()->CreateRecordset();
	
	if ( pRec->Execute( szQuery ) )
	{
		while ( pRec->Fetch() )
		{
			_LPTUSERITEMRCD	pItem = new _TUSERITEMRCD;
			
			pItem->szMakeIndex[0] = *(pRec->Get( "FLD_STDTYPE" ));
			memmove( &pItem->szMakeIndex[1], pRec->Get( "FLD_MAKEDATE" ), 6 );
			memmove( &pItem->szMakeIndex[7], pRec->Get( "FLD_MAKEINDEX" ), 5 );
			
			pItem->nStdIndex	= atoi( pRec->Get( "FLD_STDINDEX" ) );
			pItem->nDura		= atoi( pRec->Get( "FLD_DURA" ) );
			pItem->nDuraMax		= atoi( pRec->Get( "FLD_DURAMAX" ) );
			pItem->btValue[0]	= atoi( pRec->Get( "FLD_VALUE1" ) );
			pItem->btValue[1]	= atoi( pRec->Get( "FLD_VALUE2" ) );
			pItem->btValue[2]	= atoi( pRec->Get( "FLD_VALUE3" ) );
			pItem->btValue[3]	= atoi( pRec->Get( "FLD_VALUE4" ) );
			pItem->btValue[4]	= atoi( pRec->Get( "FLD_VALUE5" ) );
			pItem->btValue[5]	= atoi( pRec->Get( "FLD_VALUE6" ) );
			pItem->btValue[6]	= atoi( pRec->Get( "FLD_VALUE7" ) );
			pItem->btValue[7]	= atoi( pRec->Get( "FLD_VALUE8" ) );
			pItem->btValue[8]	= atoi( pRec->Get( "FLD_VALUE9" ) );
			pItem->btValue[9]	= atoi( pRec->Get( "FLD_VALUE10" ) );
			pItem->btValue[10]	= atoi( pRec->Get( "FLD_VALUE11" ) );
			pItem->btValue[11]	= atoi( pRec->Get( "FLD_VALUE12" ) );
			pItem->btValue[12]	= atoi( pRec->Get( "FLD_VALUE13" ) );

			ZeroMemory(pItem->szPrefixName, sizeof(pItem->szPrefixName));
			strcpy( pItem->szPrefixName, pRec->Get( "FLD_PREFIXNAME") );

			pxUserItemRcdList->AddNewNode(pItem);
		}
	}
	GetDBManager()->DestroyRecordset( pRec );
}

BOOL GetHumanRcd(char	*szName, _LPTHUMANRCD lptHumanRcd, _LPTLOADHUMAN lpLoadHuman)
{
	char szQuery[1024];

	sprintf( szQuery, "SELECT * FROM TBL_CHARACTER WHERE FLD_CHARNAME='%s'", szName );

	CRecordset *pRec = GetDBManager()->CreateRecordset();

	if ( pRec->Execute( szQuery ) && pRec->Fetch() )
	{
		strcpy(lptHumanRcd->szUserID, pRec->Get( "FLD_LOGINID" ) );
		ChangeSpaceToNull(lptHumanRcd->szUserID);
		strcpy(lptHumanRcd->szCharName, pRec->Get( "FLD_CHARNAME" ) );
		ChangeSpaceToNull(lptHumanRcd->szCharName);

		lptHumanRcd->btJob		= (BYTE)atoi( pRec->Get( "FLD_JOB" ) );
		lptHumanRcd->btGender	= (BYTE)atoi( pRec->Get( "FLD_GENDER" ) );
		lptHumanRcd->szLevel	= (BYTE)atoi( pRec->Get( "FLD_LEVEL" ) );
		lptHumanRcd->nDirection	= (BYTE)atoi( pRec->Get( "FLD_DIRECTION" ) );
		lptHumanRcd->nCX		= atoi( pRec->Get( "FLD_CX" ) );
		lptHumanRcd->nCY		= atoi( pRec->Get( "FLD_CY" ) );

		lptHumanRcd->btAttackMode	= atoi( pRec->Get( "FLD_ATTACKMODE" ) );
		lptHumanRcd->nExp			= atoi( pRec->Get( "FLD_EXP" ) );

		strcpy( lptHumanRcd->szMapName, pRec->Get( "FLD_MAPNAME" ) );
		ChangeSpaceToNull( lptHumanRcd->szMapName );

		lptHumanRcd->dwGold		= atoi( pRec->Get( "FLD_GOLD" ) );
		lptHumanRcd->szHair		= atoi( pRec->Get( "FLD_HAIR" ) );

		memmove( lptHumanRcd->szTakeItem[0], pRec->Get( "FLD_DRESS_ID" ), 12 );
		memmove( lptHumanRcd->szTakeItem[1], pRec->Get( "FLD_WEAPON_ID" ), 12 );
		memmove( lptHumanRcd->szTakeItem[2], pRec->Get( "FLD_LEFTHAND_ID" ), 12 );
		memmove( lptHumanRcd->szTakeItem[3], pRec->Get( "FLD_RIGHTHAND_ID" ), 12 );
		memmove( lptHumanRcd->szTakeItem[4], pRec->Get( "FLD_HELMET_ID" ), 12 );
		memmove( lptHumanRcd->szTakeItem[5], pRec->Get( "FLD_NECKLACE_ID" ), 12 );
		memmove( lptHumanRcd->szTakeItem[6], pRec->Get( "FLD_ARMRINGL_ID" ), 12 );
		memmove( lptHumanRcd->szTakeItem[7], pRec->Get( "FLD_ARMRINGR_ID" ), 12 );
		memmove( lptHumanRcd->szTakeItem[8], pRec->Get( "FLD_RINGL_ID" ), 12 );
		memmove( lptHumanRcd->szTakeItem[9], pRec->Get( "FLD_RINGR_ID" ), 12 );

		lptHumanRcd->fIsAdmin	= (BYTE)*pRec->Get( "FLD_ISADMIN" );
	}
	else
	{
		GetDBManager()->DestroyRecordset( pRec );
		return FALSE;
	}
	
	GetDBManager()->DestroyRecordset( pRec );

	return TRUE;
}

void GetLoadHumanRcd(CServerInfo* pServerInfo, _LPTLOADHUMAN lpLoadHuman, int nRecog)
{
	if (!pServerInfo) return;

	_TDEFAULTMESSAGE	DefMsg;

	char				szEncodeMsg1[24];

	_THUMANRCD					tHumanRcd;
	CWHList<_LPTUSERITEMRCD>	xUserItemRcdList;
	CWHList<_LPTHUMANMAGICRCD>	xUserMagicRcdList;
	CWHList<_LPTGENITEMRCD>		xUserGenItemRcdList;
	
	_THORSERCD					tHorseRcd;
	int							nHorse = 0;
	
	char						szEncodeMsg2[8096];
	
	if (GetHumanRcd(lpLoadHuman->szCharName, &tHumanRcd, lpLoadHuman))		// Fetch Character Data
	{
		GetHumanItemRcd(lpLoadHuman->szCharName, &xUserItemRcdList);		// Fetch Item Data
		GetHumanMagicRcd(lpLoadHuman->szCharName, &xUserMagicRcdList);		// Fetch Magic Data
		GetHumanGenItemRcd(lpLoadHuman->szCharName, &xUserGenItemRcdList);	// Fetch General Item Data
		
		int nCount		= xUserItemRcdList.GetCount();
		int nMagicCount = xUserMagicRcdList.GetCount();
		int nItemCount	= xUserGenItemRcdList.GetCount();
		
		if (nCount)
			nHorse = GetHorseRcd(lpLoadHuman->szCharName, &tHorseRcd);	// Fetch Horse Data

		if (nRecog)
			fnMakeDefMessageA(&DefMsg, DBR_LOADHUMANRCD, nRecog, MAKEWORD(nCount, nItemCount), nHorse, nMagicCount);
		else
			fnMakeDefMessageA(&DefMsg, DBR_LOADHUMANRCD2, nRecog, MAKEWORD(nCount, nItemCount), nHorse, nMagicCount);

		fnEncodeMessageA(&DefMsg, szEncodeMsg1, sizeof(szEncodeMsg1));
		
		int nPos = fnEncode6BitBufA((unsigned char *)&tHumanRcd, szEncodeMsg2, sizeof(_THUMANRCD), sizeof(szEncodeMsg2));
		
		if (nItemCount)
		{
			PLISTNODE pListNode = xUserGenItemRcdList.GetHead();
			
			while (pListNode)
			{
				_LPTGENITEMRCD	lptItemRcd = xUserGenItemRcdList.GetData(pListNode);
				
				if (lptItemRcd)
				{
					nPos += fnEncode6BitBufA((unsigned char *)lptItemRcd, &szEncodeMsg2[nPos], sizeof(_TGENITEMRCD), sizeof(szEncodeMsg2) - nPos);
					
					pListNode = xUserGenItemRcdList.RemoveNode(pListNode);
					delete lptItemRcd;
				}
				else 
					pListNode = xUserGenItemRcdList.GetNext(pListNode);
			}
		}
		
		if (nMagicCount)
		{
			PLISTNODE pListNode = xUserMagicRcdList.GetHead();
			
			while (pListNode)
			{
				_LPTHUMANMAGICRCD	lptUserMagicRcd = xUserMagicRcdList.GetData(pListNode);
				
				if (lptUserMagicRcd)
				{
					nPos += fnEncode6BitBufA((unsigned char *)lptUserMagicRcd, &szEncodeMsg2[nPos], sizeof(_THUMANMAGICRCD), sizeof(szEncodeMsg2) - nPos);
					
					pListNode = xUserMagicRcdList.RemoveNode(pListNode);
					delete lptUserMagicRcd;
				}
				else 
					pListNode = xUserMagicRcdList.GetNext(pListNode);
			}
		}
		
		if (nCount)
		{
			PLISTNODE pListNode = xUserItemRcdList.GetHead();
			
			while (pListNode)
			{
				_LPTUSERITEMRCD	lptUserItemRcd = xUserItemRcdList.GetData(pListNode);
				
				if (lptUserItemRcd)
				{
					nPos += fnEncode6BitBufA((unsigned char *)lptUserItemRcd, &szEncodeMsg2[nPos], sizeof(_TUSERITEMRCD), sizeof(szEncodeMsg2) - nPos);
					
					pListNode = xUserItemRcdList.RemoveNode(pListNode);
					delete lptUserItemRcd;
				}
				else 
					pListNode = xUserItemRcdList.GetNext(pListNode);
			}
			
			if (nHorse)
				nPos += fnEncode6BitBufA((unsigned char *)&tHorseRcd, &szEncodeMsg2[nPos], sizeof(_THORSERCD), sizeof(szEncodeMsg2) - nPos);
		}
		
		szEncodeMsg2[nPos] = '\0';
		
		SendSocket(pServerInfo->m_sock, lpLoadHuman->nCertification, szEncodeMsg1, szEncodeMsg2, nPos);
	}
	else
	{
		fnMakeDefMessageA(&DefMsg, DBR_FAIL, nRecog, 0, 0, 0);
		fnEncodeMessageA(&DefMsg, szEncodeMsg1, sizeof(szEncodeMsg1));
		
		SendSocket(pServerInfo->m_sock, lpLoadHuman->nCertification, szEncodeMsg1, "Test", 4);
	}
}

char *SaveHumanMagicRcd(char *pszUserID, char *pszCharName, char *pszEncodeRcd, int nCount)
{
	char szTmp[1024];

	// Delete Magic Data
	CRecordset *pRec = GetDBManager()->CreateRecordset();
	sprintf(szTmp, "DELETE FROM TBL_CHARACTER_MAGIC WHERE FLD_CHARNAME = '%s'", pszCharName);
	pRec->Execute(szTmp);
	GetDBManager()->DestroyRecordset( pRec );

	// Update Magic Data
	char *pszEncode = pszEncodeRcd;
	_THUMANMAGICRCD	tUserMagicRcd;

	for (int i = 0; i < nCount; i++)
	{
		if ( memlen( pszEncode ) >= MAGICRCDBLOCKSIZE )
		{
			pRec = GetDBManager()->CreateRecordset();

			fnDecode6BitBufA( pszEncode, (char *) &tUserMagicRcd, sizeof( _THUMANMAGICRCD ) );

			sprintf(szTmp, "INSERT TBL_CHARACTER_MAGIC(FLD_LOGINID, FLD_CHARNAME, FLD_MAGICID, FLD_LEVEL, FLD_USEKEY, FLD_CURRTRAIN) VALUES "
							"( '%s', '%s', %d, %d, %d, %d )",
							pszUserID, pszCharName, tUserMagicRcd.btMagicID, tUserMagicRcd.btLevel, tUserMagicRcd.btUseKey, tUserMagicRcd.nCurrTrain);
			
			if ( !pRec->Execute( szTmp ) || pRec->GetRowCount() <= 0 )
				InsertLogMsg(_T("SaveHumanMagicRcd 갱신 오류"));
			
			GetDBManager()->DestroyRecordset( pRec );

			pszEncode += MAGICRCDBLOCKSIZE + 1;
		}
	}
		
	return pszEncode;
}

void SaveGenItemRcd(char *pszUserID, char *pszCharName, char *pszEncodeRcd, int nCount)
{
	char szTmp[1024];

	// Delete Magic Data
	CRecordset *pRec = GetDBManager()->CreateRecordset();
	sprintf(szTmp, "DELETE FROM TBL_CHARACTER_GENITEM WHERE FLD_CHARNAME = '%s'", pszCharName);
	pRec->Execute(szTmp);
	GetDBManager()->DestroyRecordset( pRec );

	// Update General Item Data
	sprintf( szTmp, "FLD_CHARNAME='%s'", pszCharName );

	CQueryManager query;

	char *pszEncode = pszEncodeRcd;
	_TGENITEMRCD tItemRcd;

	for (int i = 0; i < nCount; i++)
	{
		while ( memlen( pszEncode ) >= GENITEMRCDBLOCKSIZE )
		{
			pRec = GetDBManager()->CreateRecordset();
		
			ZeroMemory(&tItemRcd, sizeof(_TGENITEMRCD));

			fnDecode6BitBufA( pszEncode, (char *) &tItemRcd, sizeof( _TGENITEMRCD ) );

			sprintf(szTmp, "INSERT TBL_CHARACTER_GENITEM (FLD_LOGINID, FLD_CHARNAME, FLD_ITEMINDEX) VALUES "
							"( '%s', '%s', '%s' )", pszUserID, pszCharName, tItemRcd.szItem);

			if ( !pRec->Execute( szTmp ) || pRec->GetRowCount() <= 0 )
				InsertLogMsg(_T("SaveGenItemRcd 갱신 오류"));

			GetDBManager()->DestroyRecordset( pRec );

			pszEncode += (GENITEMRCDBLOCKSIZE + 1);
		}
	}
}

BOOL SaveHumanRcd(CServerInfo* pServerInfo, _LPTLOADHUMAN lpLoadHuman, _LPTHUMANRCD lptHumanRcd, int nRecog)
{
	char szSQL[1024];
	char szTakeItem[10][13];

	ZeroMemory(szTakeItem, sizeof(szTakeItem));

	for (int i = 0; i < 10; i++)
		memmove(szTakeItem[i], lptHumanRcd->szTakeItem[i], 12);

	sprintf(szSQL, "UPDATE TBL_CHARACTER SET FLD_JOB=%d, FLD_GENDER=%d, FLD_LEVEL=%d, FLD_DIRECTION=%d, FLD_CX=%d, FLD_CY=%d, "
						"FLD_MAPNAME='%s', FLD_GOLD=%d, FLD_HAIR=%d, FLD_DRESS_ID='%s', FLD_WEAPON_ID='%s', "
						"FLD_LEFTHAND_ID='%s', FLD_RIGHTHAND_ID='%s', FLD_HELMET_ID='%s', FLD_NECKLACE_ID='%s', "
						"FLD_ARMRINGL_ID='%s', FLD_ARMRINGR_ID='%s', FLD_RINGL_ID='%s', FLD_RINGR_ID='%s', FLD_EXP=%d "
						"WHERE FLD_CHARNAME='%s'",
						lptHumanRcd->btJob, lptHumanRcd->btGender, lptHumanRcd->szLevel, lptHumanRcd->nDirection,
						lptHumanRcd->nCX, lptHumanRcd->nCY, lptHumanRcd->szMapName, lptHumanRcd->dwGold,
						lptHumanRcd->szHair, szTakeItem[0], szTakeItem[1], szTakeItem[2], szTakeItem[3], szTakeItem[4], 
						szTakeItem[5], szTakeItem[6], szTakeItem[7], szTakeItem[8], szTakeItem[9], lptHumanRcd->nExp,
						lpLoadHuman->szCharName);

	CRecordset *pRec = GetDBManager()->CreateRecordset();

	if (!pRec->Execute( szSQL ) || pRec->GetRowCount() <= 0 )
	{
		InsertLogMsg(_T("SaveHumanRcd 갱신 오류"));
		GetDBManager()->DestroyRecordset( pRec );
		return FALSE;		
	}
	
	GetDBManager()->DestroyRecordset( pRec );

	return TRUE;
}

BOOL MakeNewItem(CServerInfo* pServerInfo, _LPTLOADHUMAN lpHumanLoad, _LPTMAKEITEMRCD lpMakeItemRcd, int nRecog)
{
	static char  g_szYesterDay[24];
	static UINT  g_nItemIndexCnt = 0;

	CRecordset *pRec;

	char szQuery[1024];
	char szDate[24];
	GetDate( szDate );

//	if (strcmp(szDate, g_szYesterDay) != 0)
//	{
		sprintf( szQuery, 
			"SELECT * FROM TBL_CHARACTER_ITEM WHERE FLD_STDTYPE = '%c' AND "
			"FLD_MAKEDATE = '%s' ORDER BY FLD_MAKEINDEX DESC",
			lpMakeItemRcd->szStdType, szDate );
		
		pRec = GetDBManager()->CreateRecordset();
		if ( pRec->Execute( szQuery ) && pRec->Fetch() )
		{
			g_nItemIndexCnt = atoi( pRec->Get( "FLD_MAKEINDEX" ) ) + 1;
		}
		GetDBManager()->DestroyRecordset( pRec );

		strcpy( g_szYesterDay, szDate );
//	}
//	else
//		g_nItemIndexCnt++;

	char szUserID[32];
	char szCharName[32];

	if (lpHumanLoad)
	{
		strcpy(szUserID, lpHumanLoad->szUserID);
		strcpy(szCharName, lpHumanLoad->szCharName);
	}
	else
	{
		strcpy(szUserID, "0");
		strcpy(szCharName, "WEMADE");
	}

	sprintf(szQuery, "INSERT TBL_CHARACTER_ITEM (FLD_LOGINID, FLD_CHARNAME, FLD_STDTYPE, FLD_MAKEDATE, FLD_MAKEINDEX, "
						"FLD_STDINDEX, FLD_DURA, FLD_DURAMAX, FLD_VALUE1, FLD_VALUE2, FLD_VALUE3, FLD_VALUE4, FLD_VALUE5, "
						"FLD_VALUE6, FLD_VALUE7, FLD_VALUE8, FLD_VALUE9, FLD_VALUE10, FLD_VALUE11, FLD_VALUE12, FLD_VALUE13, "
						"FLD_VALUE14, FLD_LASTOWNER, FLD_LASTACTION, FLD_PREFIXNAME) "
						"VALUES( '%s', '%s', '%c', '%s', '%05d', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', %d, '%s' )", 
						szUserID, szCharName, lpMakeItemRcd->szStdType, g_szYesterDay, g_nItemIndexCnt,
						lpMakeItemRcd->nStdIndex, lpMakeItemRcd->nDura,	lpMakeItemRcd->nDuraMax, 
						lpMakeItemRcd->btValue[0], lpMakeItemRcd->btValue[1], lpMakeItemRcd->btValue[2], lpMakeItemRcd->btValue[3], 
						lpMakeItemRcd->btValue[5], lpMakeItemRcd->btValue[5], lpMakeItemRcd->btValue[6], lpMakeItemRcd->btValue[7], 
						lpMakeItemRcd->btValue[8], lpMakeItemRcd->btValue[9], lpMakeItemRcd->btValue[10], lpMakeItemRcd->btValue[11], 
						lpMakeItemRcd->btValue[12], lpMakeItemRcd->btValue[13], szCharName, 1, "");
	
	pRec = GetDBManager()->CreateRecordset();
	if ( !pRec->Execute( szQuery ) || pRec->GetRowCount() <= 0 )
	{
		GetDBManager()->DestroyRecordset( pRec );
		return FALSE;
	}
	GetDBManager()->DestroyRecordset( pRec );
	
	if ( pServerInfo )
	{
		// Send Packet to Game Server
		_TDEFAULTMESSAGE	DefMsg;
		_TUSERITEMRCD		UserItemRcd;
		
		char				szEncodeMsg1[24];
		char				szEncodeMsg2[128];
		int					nPos = 0;
		
		if (lpHumanLoad)
			fnMakeDefMessageA(&DefMsg, DBR_MAKEITEMRCD, nRecog, 0, 0, 0);
		else
			fnMakeDefMessageA(&DefMsg, DBR_MAKEITEMRCD2, nRecog, 0, 0, 0);

		fnEncodeMessageA(&DefMsg, szEncodeMsg1, sizeof(szEncodeMsg1));
		
		UserItemRcd.szMakeIndex[0] = lpMakeItemRcd->szStdType;
		
		memcpy( &UserItemRcd.szMakeIndex[1], g_szYesterDay, 6 );
		sprintf( &UserItemRcd.szMakeIndex[7], "%d", g_nItemIndexCnt );
		
		memcpy(&UserItemRcd.nStdIndex, &lpMakeItemRcd->nStdIndex, sizeof(_TUSERITEMRCD) - sizeof(UserItemRcd.szMakeIndex));
		
		nPos = fnEncode6BitBufA((unsigned char *)&UserItemRcd, szEncodeMsg2, sizeof(_TUSERITEMRCD), sizeof(szEncodeMsg2));
		
		SendSocket(pServerInfo->m_sock, 2, szEncodeMsg1, szEncodeMsg2, nPos);		
	}

	return TRUE;
}

UINT WINAPI ProcessDBMsg(LPVOID lpParameter)
{
	while (TRUE)
	{
		if (g_fTerminated) return 0L;

		_LPTSENDBUFF pSendBuff = (_LPTSENDBUFF)g_DBMsgQ.PopQ();

		if (pSendBuff)
		{
			switch (pSendBuff->DefMsg.wIdent)
			{
				case DB_LOADHUMANRCD:
					GetLoadHumanRcd(pSendBuff->pServerInfo, &pSendBuff->HumanLoad, pSendBuff->DefMsg.nRecog);
					break;
				case DB_SAVEHUMANRCD:
				{
					SaveHumanRcd(pSendBuff->pServerInfo, &pSendBuff->HumanLoad, (_LPTHUMANRCD)pSendBuff->lpbtAddData, pSendBuff->DefMsg.nRecog);
					char *pszData = SaveHumanMagicRcd(pSendBuff->HumanLoad.szUserID, pSendBuff->HumanLoad.szCharName, (char *)pSendBuff->lpbtAddData2, pSendBuff->DefMsg.wParam);
					SaveGenItemRcd(pSendBuff->HumanLoad.szUserID, pSendBuff->HumanLoad.szCharName, pszData, pSendBuff->DefMsg.wTag);
					break;
				}
				case DB_MAKEITEMRCD:
					MakeNewItem(pSendBuff->pServerInfo, &pSendBuff->HumanLoad, (_LPTMAKEITEMRCD)pSendBuff->lpbtAddData, pSendBuff->DefMsg.nRecog);
					break;
				case DB_MAKEITEMRCD2:
				{
					_LPTUSERITEMRCD lptUserItemRcd = (_LPTUSERITEMRCD)pSendBuff->lpbtAddData;
					_TMAKEITEMRCD	tMakeItemRcd;

					tMakeItemRcd.szStdType	= lptUserItemRcd->szMakeIndex[0];
					tMakeItemRcd.nStdIndex	= lptUserItemRcd->nStdIndex;
					tMakeItemRcd.nDura		= lptUserItemRcd->nDura;
					tMakeItemRcd.nDuraMax	= lptUserItemRcd->nDuraMax;
					memcpy(tMakeItemRcd.btValue, lptUserItemRcd->btValue, sizeof(lptUserItemRcd->btValue));

					MakeNewItem(pSendBuff->pServerInfo, NULL, &tMakeItemRcd, pSendBuff->DefMsg.nRecog);
					break;
				}
			} // switch

			delete pSendBuff;
		} // if (pSendBuff)

		SleepEx(1, TRUE);
	}

	return 0L;
}
