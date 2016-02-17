

#include "stdafx.h"
#include "dbmgr.h"
#include <stdio.h>


static CDBManager s_dbManager;


CDBManager::CDBManager()
{
	m_dbMain.SetDiagRec( __cbDBMsg );

	m_dbMain.Init();
}


CDBManager::~CDBManager()
{
	if ( m_pConn )
		m_dbMain.DestroyConnection( m_pConn );

	m_dbMain.Uninit();
}


bool CDBManager::Init( void	(*pfnLog)( LPTSTR pMsg ), char *pDSN, char *pID, char *pPassword )
{
	m_pfnLog = pfnLog;
	m_pConn  = m_dbMain.CreateConnection( pDSN, pID, pPassword );

	return m_pConn ? true : false;
}


CRecordset * CDBManager::CreateRecordset()
{
	Lock();
	
	return m_pConn->CreateRecordset();
}


void CDBManager::DestroyRecordset( CRecordset *pRec )
{
	m_pConn->DestroyRecordset( pRec );

	Unlock();
}


void CDBManager::__cbDBMsg( char *pState, int nNativeCode, char *pDesc )
{
	static TCHAR szState[256], szDesc[2048];
	static TCHAR szMsg[2048];

	MultiByteToWideChar( CP_ACP, 0, pState, -1, szState, sizeof( szState ) / sizeof( TCHAR ) );
	MultiByteToWideChar( CP_ACP, 0, pDesc, -1, szDesc, sizeof( szDesc ) / sizeof( TCHAR ) );
	
	wsprintf( szMsg, _T("ODBC MsgID: %s(%d)"), szState, nNativeCode );
	GetDBManager()->m_pfnLog( szMsg );

	wsprintf( szMsg, _T("%s"), szDesc );
	GetDBManager()->m_pfnLog( szMsg );
}




CDBManager * GetDBManager()
{
	return &s_dbManager;
}




CQueryManager::CQueryManager()
{
}


CQueryManager::~CQueryManager()
{
	m_listItem.ClearAll();
}


void CQueryManager::StartUpdateQuery( char *pTable, char *pCondition )
{
	m_listItem.ClearAll();

	strcpy( m_szTable, pTable );
	strcpy( m_szCondition, pCondition );
}


char * CQueryManager::GetUpdateQuery()
{
	sprintf( m_szQuery, "UPDATE %s SET ", m_szTable );
	
	CListNode< ITEM > *pNode = m_listItem.GetHead();
	if ( !pNode )
		return NULL;
	ITEM *pItem = pNode->GetData();
	strcat( m_szQuery, pItem->szName );
	strcat( m_szQuery, "=" );
	if ( pItem->bString )
	{
		strcat( m_szQuery, "'" );
		strcat( m_szQuery, pItem->szValue );
		strcat( m_szQuery, "'" );
	}
	else
		strcat( m_szQuery, pItem->szValue );

	for ( pNode = pNode->GetNext(); pNode; pNode = pNode->GetNext() )
	{
		pItem = pNode->GetData();
		strcat( m_szQuery, ", " );
		strcat( m_szQuery, pItem->szName );
		strcat( m_szQuery, "=" );
		if ( pItem->bString )
		{
			strcat( m_szQuery, "'" );
			strcat( m_szQuery, pItem->szValue );
			strcat( m_szQuery, "'" );
		}
		else
			strcat( m_szQuery, pItem->szValue );
	}

	strcat( m_szQuery, " WHERE " );
	strcat( m_szQuery, m_szCondition );

	return m_szQuery;
}


void CQueryManager::StartInsertQuery( char *pTable )
{
	m_listItem.ClearAll();

	strcpy( m_szTable, pTable );
}


char * CQueryManager::GetInsertQuery()
{
	sprintf( m_szQuery, "INSERT %s(", m_szTable );

	CListNode< ITEM > *pNode = m_listItem.GetHead();
	if ( !pNode )
		return NULL;

	ITEM *pItem = pNode->GetData();
	strcat( m_szQuery, pItem->szName );
	
	for ( pNode = pNode->GetNext(); pNode; pNode = pNode->GetNext() )
	{
		pItem = pNode->GetData();
		strcat( m_szQuery, ", " );
		strcat( m_szQuery, pItem->szName );
	}

	strcat( m_szQuery, ") VALUES( " );

	pNode = m_listItem.GetHead();
	pItem = pNode->GetData();

	if ( pItem->bString )
	{
		strcat( m_szQuery, "'" );
		strcat( m_szQuery, pItem->szValue );
		strcat( m_szQuery, "'" );
	}
	else
		strcat( m_szQuery, pItem->szValue );

	for ( pNode = pNode->GetNext(); pNode; pNode = pNode->GetNext() )
	{
		pItem = pNode->GetData();

		strcat( m_szQuery, ", " );
		
		if ( pItem->bString )
		{
			strcat( m_szQuery, "'" );
			strcat( m_szQuery, pItem->szValue );
			strcat( m_szQuery, "'" );
		}
		else
			strcat( m_szQuery, pItem->szValue );
	}

	strcat( m_szQuery, " )" );

	return m_szQuery;
}


void CQueryManager::InsertItem( char *pName, char *pValue, int nValueLen, bool bString )
{
	ITEM *pItem = new ITEM;
	if ( !pItem )
		return;

	strcpy( pItem->szName, pName );
	strncpy( pItem->szValue, pValue, nValueLen );
	pItem->szValue[nValueLen] = '\0';
	pItem->bString = bString;

	m_listItem.Insert( pItem );
}


void CQueryManager::InsertItem( char *pName, TCHAR *pValue, bool bString )
{
	ITEM *pItem = new ITEM;
	if ( !pItem )
		return;

	strcpy( pItem->szName, pName );
	WideCharToMultiByte( CP_ACP, 0, pValue, -1, pItem->szValue, sizeof( pItem->szValue ), 0, 0 );
	pItem->bString = bString;

	m_listItem.Insert( pItem );
}


void CQueryManager::InsertItem( char *pName, char *pValue, bool bString )
{
	ITEM *pItem = new ITEM;
	if ( !pItem )
		return;

	strcpy( pItem->szName, pName );
	strcpy( pItem->szValue, pValue );
	pItem->bString = bString;

	m_listItem.Insert( pItem );
}

void CQueryManager::InsertItem( char *pName, BYTE pValue )
{
	ITEM *pItem = new ITEM;
	if ( !pItem )
		return;

	strcpy( pItem->szName, pName );

	pItem->szValue[0] = pValue;
	pItem->szValue[1] = '\0';
	
	pItem->bString = true;

	m_listItem.Insert( pItem );
}


void CQueryManager::InsertItem( char *pName, int nValue, bool bString )
{
	ITEM *pItem = new ITEM;
	if ( !pItem )
		return;

	strcpy( pItem->szName, pName );
	itoa( nValue, pItem->szValue, 10 );
	pItem->bString = bString;

	m_listItem.Insert( pItem );
}
