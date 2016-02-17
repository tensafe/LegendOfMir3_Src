

/*
	Date:
		2002/03/06 (Last Updated: 2002/03/08)
*/
#pragma once


#include "_orzEx/database.h"
#include "_orzEx/syncobj.h"
#include "_orzEx/list.h"


class CDBManager : public CIntLock
{
public:
	CDatabase	m_dbMain;
	CConnection	*m_pConn;

	void (*m_pfnLog)( LPTSTR pMsg );

public:
	CDBManager();
	~CDBManager();

	bool Init( void	(*pfnLog)( LPTSTR pMsg ), char *pDSN, char *pID, char *pPassword );

	CRecordset * CreateRecordset();
	void DestroyRecordset( CRecordset *pRec );

protected:
	static void __cbDBMsg( char *pState, int nNativeCode, char *pDesc );
};


CDBManager * GetDBManager();


// 쿼리문이 너무 복잡할 때 사용한다.
// 주의: 최대 버퍼크기=4096
class CQueryManager
{
protected:
	struct ITEM
	{
		char szName[256];
		char szValue[256];
		bool bString;
	};

	char			m_szQuery[4096];
	char			m_szTable[256];
	char			m_szCondition[1024];
	CList< ITEM >	m_listItem;

public:
	CQueryManager();
	virtual ~CQueryManager();

	void StartUpdateQuery( char *pTable, char *pCondition );
	char *GetUpdateQuery();
	void StartInsertQuery( char *pTable );
	char *GetInsertQuery();

	void InsertItem( char *pName, char  *pValue, int  nValueLen, bool bString = true );
	void InsertItem( char *pName, char  *pValue, bool bString = true );
	void InsertItem( char *pName, TCHAR *pValue, bool bString = true );
	void InsertItem( char *pName, int    nValue, bool bString = false );
	void InsertItem( char *pName, BYTE   pValue );
};