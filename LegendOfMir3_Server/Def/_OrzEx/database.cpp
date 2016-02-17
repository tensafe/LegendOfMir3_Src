

#include "stdafx.h"
#include "database.h"


/*
	Implement CDatabase methods
*/
CDatabase::CDatabase()
{
	m_hEnv		= NULL;
}


CDatabase::~CDatabase()
{
}


bool CDatabase::Init()
{
	if ( SQLAllocHandle( SQL_HANDLE_ENV, SQL_NULL_HANDLE, &m_hEnv ) != SQL_SUCCESS )
	   return false;

   if ( SQLSetEnvAttr( m_hEnv, SQL_ATTR_ODBC_VERSION, (SQLPOINTER) SQL_OV_ODBC3, SQL_IS_INTEGER ) != SQL_SUCCESS )
	   return false;

   return true;
}


void CDatabase::Uninit()
{
	if ( m_hEnv )
	{
		SQLFreeHandle( SQL_HANDLE_ENV, m_hEnv );
		m_hEnv = NULL;
	}
}


void CDatabase::EnumDSN( void (*pfnEnum)( char *pSrcName, char *pSrcDesc ) )
{
	int nResult;
	int nDirection = SQL_FETCH_FIRST;

	char szName[DB_MAXBUF], szDesc[DB_MAXBUF];
	int  nNameLen, nDescLen;

	while ( true )
	{
		nResult = SQLDataSourcesA( m_hEnv, 
								  nDirection, 
								  (byte *) szName, DB_MAXBUF, (short *) &nNameLen,
								  (byte *) szDesc, DB_MAXBUF, (short *) &nDescLen );

		if ( nResult == SQL_ERROR || nResult == SQL_NO_DATA )
			break;

		pfnEnum( szName, szDesc );

		nDirection = SQL_FETCH_NEXT;
	}
}


CConnection * CDatabase::CreateConnection( char *pDSN, char *pID, char *pPassword )
{
	CConnection *pConn = new CConnection;
	if ( pConn == NULL )
		return NULL;

	if ( pConn->Init( m_hEnv, pDSN, pID, pPassword ) == false )
	{
		delete pConn;
		return NULL;
	}

	return pConn;
}


void CDatabase::DestroyConnection( CConnection *pConn )
{
	pConn->Uninit();

	delete pConn;
}


/*
	Implement diagnostic record functions
*/
static void (*g_pfnRecord)( char *pState, int nErrCode, char *pDesc );


void CDatabase::SetDiagRec( void (*pfnRecord)( char *pState, int nErrCode, char *pDesc ) )
{
	g_pfnRecord = pfnRecord;
}


void CDatabase::UnsetDiagRec()
{
	g_pfnRecord = NULL;
}


void CDatabase::DiagRec( int nHandleType, SQLHANDLE hHandle )
{
	if ( g_pfnRecord == NULL )
		return;

	char szState[SQL_SQLSTATE_SIZE + 1];
	char szDesc[SQL_MAX_MESSAGE_LENGTH + 1];
	int  nDescLen;
	int  nNativeError;
	int  nResult;
	int  nRecCnt = 1;

	while ( true )
	{
		nResult = SQLGetDiagRecA( nHandleType, 
								 hHandle, 
								 nRecCnt++, 
								 (byte *) szState, 
								 (long *) &nNativeError, 
								 (byte *) szDesc, 
								 SQL_MAX_MESSAGE_LENGTH,
								 (short *) &nDescLen );
		
		if ( nResult == SQL_NO_DATA || nResult == SQL_ERROR || nResult == SQL_INVALID_HANDLE )
			break;

		g_pfnRecord( szState, nNativeError, szDesc );
	}
}


/*
	Implement CConnection methods
*/
CConnection::CConnection()
{
	m_hDBConn	= NULL;
}


CConnection::~CConnection()
{
}


bool CConnection::Init( SQLHENV hEnv, char *pDSN, char *pID, char *pPassword )
{
	int nResult;

	nResult = SQLAllocHandle( SQL_HANDLE_DBC, hEnv, &m_hDBConn );
	if ( nResult != SQL_SUCCESS )
	{
		CDatabase::DiagRec( SQL_HANDLE_ENV, hEnv );
		return false;
	}

	nResult = SQLConnectA( m_hDBConn, 
						  (byte *) pDSN, SQL_NTS, 
						  (byte *) pID, SQL_NTS, 
						  (byte *) pPassword, SQL_NTS );
	if ( nResult != SQL_SUCCESS )
	{
		CDatabase::DiagRec( SQL_HANDLE_DBC, m_hDBConn );

		if ( nResult != SQL_SUCCESS_WITH_INFO )
			return false;
	}

	return true;
}


void CConnection::Uninit()
{
	if ( m_hDBConn )
	{
		SQLDisconnect( m_hDBConn );
		SQLFreeHandle( SQL_HANDLE_DBC, m_hDBConn );
		m_hDBConn = NULL;
	}
}


CRecordset * CConnection::CreateRecordset()
{
	CRecordset *pRec = new CRecordset;
	if ( pRec == NULL )
		return NULL;

	if ( pRec->Init( m_hDBConn ) == false )
	{
		delete pRec;
		return NULL;
	}

	return pRec;
}


void CConnection::DestroyRecordset( CRecordset *pRec )
{
	pRec->Uninit();
	delete pRec;
}


/*
	Implement CRecordset methods
*/
CRecordset::CRecordset()
{
	m_hStmt		= NULL;

	m_nRowCount	= 0;
	m_nCols		= 0;

	m_pColInfo	= NULL;
	m_pColData	= NULL;
}


CRecordset::~CRecordset()
{
}


bool CRecordset::Init( SQLHDBC hDBConn )
{
	if ( SQLAllocHandle( SQL_HANDLE_STMT, hDBConn, &m_hStmt ) != SQL_SUCCESS )
	{
		CDatabase::DiagRec( SQL_HANDLE_STMT, m_hStmt );
		return false;
	}

	return true;
}


void CRecordset::Uninit()
{
	if ( m_pColInfo )
	{
		delete[] m_pColInfo;
		m_pColInfo = NULL;
	}
	
	if ( m_pColData )
	{
		delete[] m_pColData;
		m_pColData = NULL;
	}

	if ( m_hStmt )
	{
		SQLFreeHandle( SQL_HANDLE_STMT, m_hStmt );
		m_hStmt = NULL;
	}
}


bool CRecordset::Execute( char *pQuery )
{
	int nResult;
	int nCount;

	nResult = SQLExecDirectA( m_hStmt, (byte *) pQuery, SQL_NTS );
	if ( nResult != SQL_SUCCESS )
	{
		CDatabase::DiagRec( SQL_HANDLE_STMT, m_hStmt );

		if ( nResult != SQL_SUCCESS_WITH_INFO )
			return false;
	}

	m_nCols = 0;
	nResult = SQLNumResultCols( m_hStmt, (short *) &m_nCols );
	if ( nResult != SQL_SUCCESS && nResult != SQL_SUCCESS_WITH_INFO )
	{
		CDatabase::DiagRec( SQL_HANDLE_STMT, m_hStmt );
		return false;
	}
	
	// If cols is 0, the statement probably was a non-SELECT simply return.
	if ( m_nCols == 0 )
	{
		m_nRowCount = 0;
		SQLRowCount( m_hStmt, (long *) &m_nRowCount );
		return true;
	}

	m_pColInfo = new CColumnInfo[ m_nCols ];
	if ( m_pColInfo == NULL )
		return false;

	for ( nCount = 0; nCount < m_nCols; nCount++ )
	{
		nResult = SQLDescribeColA( m_hStmt, 
								  nCount + 1, 
								  (byte *) m_pColInfo[nCount].szColName, DB_MAXBUF,
								  (short *) &m_pColInfo[nCount].nColNameSize,
								  (short *) &m_pColInfo[nCount].nColType,
								  (ULONG *) &m_pColInfo[nCount].nColSize,
								  (short *) &m_pColInfo[nCount].nAllowDecimalDigit,
								  (short *) &m_pColInfo[nCount].nAllowNull );	
		
		if ( nResult != SQL_SUCCESS )
		{
			CDatabase::DiagRec( SQL_HANDLE_STMT, m_hStmt );

			if ( nResult != SQL_SUCCESS_WITH_INFO )
				return false;
		}
	}

	m_pColData = new CColumnData[ m_nCols ];
	if ( m_pColData == NULL )
		return false;

	for ( nCount = 0; nCount < m_nCols; nCount++ )
	{
		if ( m_pColData[nCount].AllocMemory( m_pColInfo[nCount].nColSize + 1 ) == false )
			return false;

		nResult = SQLBindCol( m_hStmt, 
							  nCount + 1, 
							  SQL_C_CHAR,
							  m_pColData[nCount].pData,
							  m_pColInfo[nCount].nColSize + 1,
							  (long *) &m_pColData[nCount].nDataSize );

		if ( nResult != SQL_SUCCESS )
		{
			CDatabase::DiagRec( SQL_HANDLE_STMT, m_hStmt );

			if ( nResult != SQL_SUCCESS_WITH_INFO )
				return false;
		}
	}

	return true;
}


bool CRecordset::Fetch()
{
	int nResult;

	nResult = SQLFetch( m_hStmt );
	if ( nResult != SQL_SUCCESS )
	{
		CDatabase::DiagRec( SQL_HANDLE_STMT, m_hStmt );

		if ( nResult != SQL_SUCCESS_WITH_INFO )
			return false;
	}

	return true;
}


int CRecordset::GetRowCount()
{
	return m_nRowCount;
}


int CRecordset::GetCols()
{
	return m_nCols;
}


char * CRecordset::Get( char *pColName )
{
	for ( int nCount = 0; nCount < m_nCols; nCount++ )
	{
		if ( stricmp( m_pColInfo[nCount].szColName, pColName ) == 0 )
			return m_pColData[nCount].pData;
	}

	return NULL;
}


char * CRecordset::Get( int nCol )
{
	if ( nCol < 0 || nCol >= m_nCols )
		return NULL;

	return m_pColData[ nCol ].pData;
}


CRecordset::CColumnInfo * CRecordset::GetColInfo( char *pColName )
{
	for ( int nCount = 0; nCount < m_nCols; nCount++ )
	{
		if ( stricmp( m_pColInfo[nCount].szColName, pColName ) == 0 )
			return &m_pColInfo[nCount];
	}

	return NULL;
}


CRecordset::CColumnInfo * CRecordset::GetColInfo( int nCol )
{
	if ( nCol < 0 || nCol >= m_nCols )
		return NULL;
	
	return &m_pColInfo[nCol];
}


/*
	Implement CRecordset::CColumnInfo methods
*/
CRecordset::CColumnInfo::CColumnInfo()
{
	nColNameSize		= 0;
	szColName[0]		= '\0';
	nColType			= 0;
	nColSize			= 0;
	nAllowDecimalDigit	= 0;
	nAllowNull			= 0;
}


CRecordset::CColumnInfo::~CColumnInfo()
{
}


/*
	Implement CRecordset::CColumnData methods
*/
CRecordset::CColumnData::CColumnData()
{
	pData		= NULL;
	nDataSize	= 0;
}


CRecordset::CColumnData::~CColumnData()
{
	if ( pData )
		delete[] pData;
}


bool CRecordset::CColumnData::AllocMemory( int nSize )
{
	pData = new char[ nSize ];
	if ( pData == NULL )
		return false;

	memset( pData, 0, nSize );

	return true;
}
