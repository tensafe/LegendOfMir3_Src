

/*
	Date:
		2002/03/06

	Note:
		Modified Version for Precompiler and Unicode
*/
#ifndef __ORZ_DATABASE__
#define __ORZ_DATABASE__


#include <windows.h>
#include <sql.h>
#include <sqlext.h>


#define DB_MAXBUF	256


class CConnection;
class CRecordset;


/*
	define CDatabase 
*/
class CDatabase
{
protected:
	SQLHENV		m_hEnv;

public:
	CDatabase();
	virtual ~CDatabase();

	bool Init();
	void Uninit();

	void EnumDSN( void (*pfnEnum)( char *pSrcName, char *pSrcDesc ) );

	CConnection * CreateConnection( char *pDSN, char *pID, char *pPassword );
	void DestroyConnection( CConnection *pConn );

public:
	static void SetDiagRec( void (*pfnRecord)( char *pState, int nErrCode, char *pDesc ) );
	static void UnsetDiagRec();
	static void DiagRec( int nHandleType, SQLHANDLE hHandle );
};


/*
	define CConnection 
*/
class CConnection
{
protected:
	SQLHDBC		m_hDBConn;

private:
	CConnection();
	virtual ~CConnection();

	bool Init( SQLHENV hEnv, char *pDSN, char *pID, char *pPassword );
	void Uninit();

public:
	friend CDatabase;

	CRecordset * CreateRecordset();
	void DestroyRecordset( CRecordset *pRec );
};


/*
	define CRecordset 
*/
class CRecordset
{
protected:
	SQLHSTMT	m_hStmt;

	int			m_nRowCount;
	int			m_nCols;

	class CColumnInfo
	{
	public:
		int  nColNameSize;			// 컬럼 이름 크기
		char szColName[DB_MAXBUF];	// 컬럼 이름
		int  nColType;				// 컬럼 타입
		int  nColSize;				// 컬럼 크기
		int  nAllowDecimalDigit;	// 10진수 변경 허용 여부
		int  nAllowNull;			// 널 허용 여부

	public:
		CColumnInfo();
		~CColumnInfo();

	} *m_pColInfo;

	class CColumnData
	{
	public:
		char *pData;				// 데이터
		int  nDataSize;				// 순수 데이터 크기 (할당된 메모리 크기가 아님)

	public:
		CColumnData();
		~CColumnData();

		bool AllocMemory( int nSize );

	} *m_pColData;

private:
	CRecordset();
	virtual ~CRecordset();

	bool Init( SQLHDBC hDBConn );
	void Uninit();

public:
	friend CConnection;
	
	bool Execute( char *pQuery );
	bool Fetch();

	int  GetRowCount();
	int  GetCols();
	char * Get( char *pColName );
	char * Get( int nCol );
	CColumnInfo * GetColInfo( char *pColName );
	CColumnInfo * GetColInfo( int nCol );
};


#endif