#include "StdAfx.h"



CWHWilImageData::CWHWilImageData()
{
	Init();
}


CWHWilImageData::~CWHWilImageData()
{
	Destroy();
}


VOID CWHWilImageData::Init()
{
	m_bIsMemMapped		= FALSE;
	m_bIsCompressed		= FALSE;
	m_pbStartData		= NULL;	

	ZeroMemory(&m_stWixImgaeInfo, sizeof(WIXIMAGEINFO));

	ZeroMemory(&m_stNewWixImgaeInfo, sizeof(WIXIMAGEINFO));

	ZeroMemory(&m_szWilFileName, sizeof(MAX_PATH));

	m_nCurrImageIdx		= -1;								// 초기 세팅치가 설정되지 않은상태.
	m_pbCurrImage		= NULL;
	m_lpstCurrWilImageInfo = NULL;

	m_lpstNewCurrWilImageInfo = NULL;
}


VOID CWHWilImageData::Destroy()
{
	if ( m_pbStartData ) 
	{
		if ( m_bIsMemMapped )
		{
			UnmapViewOfFile(m_pbStartData);
			m_pbStartData = NULL;
		}
		else
		{
			SAFE_DELETE(m_pbStartData);
		}
	}

	SAFE_DELETE(m_stWixImgaeInfo.pnPosition);
	ZeroMemory(&m_stWixImgaeInfo, sizeof(WIXIMAGEINFO));

	SAFE_DELETE(m_stNewWixImgaeInfo.pnPosition);
	ZeroMemory(&m_stNewWixImgaeInfo, sizeof(NEWWIXIMAGEINFO));

	Init();
}



BOOL CWHWilImageData::Load(CHAR* szWilFile, BOOL bIsMemMapped, BOOL bComp)
{
	m_bIsMemMapped	= bIsMemMapped;
	m_bIsCompressed	= bComp;

	HANDLE	hWilFile;
	HANDLE	hWixFile;
	HANDLE	hWilMappedFile;
	DWORD	dwReadLen;
	CHAR	szWixFile[MAX_PATH];
	CHAR	szMsg[MAX_PATH];

	strcpy(szWixFile, szWilFile);
	strcpy(szWixFile+strlen(szWixFile)-3, "wix");
	strcpy(m_szWilFileName, szWilFile);

	hWixFile= CreateFile(szWixFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if ( hWixFile )
	{
		ReadFile(hWixFile, &m_stWixImgaeInfo, sizeof(WIXIMAGEINFO)-sizeof(INT*), &dwReadLen, NULL);

		m_stWixImgaeInfo.pnPosition = new INT[m_stWixImgaeInfo.nIndexCount];
		ReadFile(hWixFile, m_stWixImgaeInfo.pnPosition, sizeof(INT)*m_stWixImgaeInfo.nIndexCount, &dwReadLen, NULL);

		hWilFile = CreateFile(szWilFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

		if ( hWilFile != INVALID_HANDLE_VALUE )
		{
			// 메모리맵형태일때.
			if ( m_bIsMemMapped )
			{
				hWilMappedFile = CreateFileMapping(hWilFile, NULL, PAGE_READONLY, NULL, NULL, NULL);

				m_pbStartData = (BYTE*)MapViewOfFile(hWilMappedFile, FILE_MAP_READ, NULL, NULL, NULL);

				if ( !m_pbStartData )
				{
					// Debug
					MessageBox(NULL, "CreateFileMapping Failed", "Mir2EX :: Error", MB_ICONERROR | MB_OK);
					CloseHandle(hWilFile);
					CloseHandle(hWixFile);
					return FALSE;
				}
				CloseHandle(hWilMappedFile);
			}
			else
			{
				DWORD dwFileSize;
				dwFileSize = GetFileSize(hWilFile, NULL);
				m_pbStartData = new BYTE[dwFileSize];
				ReadFile(hWilFile, m_pbStartData, dwFileSize, &dwReadLen, NULL);
			}

			SetIndex(0);						// 0번 인덱스로 세팅한다.
			CloseHandle(hWilFile);
			CloseHandle(hWixFile);
			return TRUE;
		}

		wsprintf(szMsg, "Cannot find %s file", szWixFile);
		// Debug
		MessageBox(NULL, szMsg, "Mir2EX :: Error", MB_ICONERROR | MB_OK);
		CloseHandle(hWixFile);
		return FALSE;
	}

	wsprintf(szMsg, "Cannot find %s file", szWixFile);
	// Debug
	MessageBox(NULL, szMsg, "Mir2EX :: Error", MB_ICONERROR | MB_OK);

	return FALSE;

}



VOID CWHWilImageData::SetIndex(INT nIndex)
{
	if ( m_nCurrImageIdx == nIndex )	return;				// 새로 Setting되는 Index가 변화가 없다면.

	m_nCurrImageIdx = nIndex;

	BYTE* pbTmpData = (BYTE*)(m_pbStartData + m_stWixImgaeInfo.pnPosition[nIndex]);

	m_lpstCurrWilImageInfo = (LPWILIMAGEINFO)pbTmpData;

	if ( m_bIsCompressed )
		m_pbCurrImage = (BYTE*)(pbTmpData + sizeof(WILIMAGEINFO));
	else
		m_pbCurrImage = (BYTE*)(pbTmpData + sizeof(WILIMAGEINFO) - sizeof(DWORD));
}


BOOL CWHWilImageData::NewSetIndex(DWORD dwIndex)
{
//	if ( m_nCurrImageIdx == nIndex || !m_stNewWixImgaeInfo.pnPosition[nIndex] )	return;				// 새로 Setting되는 Index가 변화가 없다면.

	m_nCurrImageIdx = dwIndex;

	if ( m_stNewWixImgaeInfo.pnPosition[dwIndex] != 0 && dwIndex >= 0 && (INT)dwIndex < m_stNewWixImgaeInfo.nIndexCount )
	{
		BYTE* pbTmpData = (BYTE*)(m_pbStartData + m_stNewWixImgaeInfo.pnPosition[dwIndex]);

		m_lpstNewCurrWilImageInfo = (LPNEWWILIMAGEINFO)pbTmpData;

		if ( m_bIsCompressed )
			m_pbCurrImage = (BYTE*)(pbTmpData + sizeof(NEWWILIMAGEINFO));
		else
			m_pbCurrImage = (BYTE*)(pbTmpData + sizeof(NEWWILIMAGEINFO) - sizeof(DWORD));

		return TRUE;
	}
	return FALSE;
}


BOOL CWHWilImageData::NewLoad(CHAR* szWilFile, BOOL bIsMemMapped, BOOL bComp)
{
	m_bIsMemMapped	= bIsMemMapped;
	m_bIsCompressed	= bComp;

	HANDLE	hWilFile;
	HANDLE	hWixFile;
	HANDLE	hWilMappedFile;
	DWORD	dwReadLen;
	CHAR	szWixFile[MAX_PATH];
	CHAR	szMsg[MAX_PATH];

	strcpy(szWixFile, szWilFile);
	strcpy(szWixFile+strlen(szWixFile)-3, "wix");
	strcpy(m_szWilFileName, szWilFile);

	hWixFile= CreateFile(szWixFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if ( hWixFile )
	{
		ReadFile(hWixFile, &m_stNewWixImgaeInfo, sizeof(NEWWIXIMAGEINFO)-sizeof(INT*), &dwReadLen, NULL);

		m_stNewWixImgaeInfo.pnPosition = new INT[m_stNewWixImgaeInfo.nIndexCount];
		ReadFile(hWixFile, m_stNewWixImgaeInfo.pnPosition, sizeof(INT)*m_stNewWixImgaeInfo.nIndexCount, &dwReadLen, NULL);

		hWilFile = CreateFile(szWilFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL );

		if ( hWilFile != INVALID_HANDLE_VALUE )
		{
			// 메모리맵형태일때.
			if ( m_bIsMemMapped )
			{
				hWilMappedFile = CreateFileMapping(hWilFile, NULL, PAGE_READONLY, NULL, NULL, NULL);

				m_pbStartData = (BYTE*)MapViewOfFile(hWilMappedFile, FILE_MAP_READ, NULL, NULL, NULL);

				if ( !m_pbStartData )
				{
					// Debug
					MessageBox(NULL, "CreateFileMapping Failed", "Mir2EX :: Error", MB_ICONERROR | MB_OK);
					CloseHandle(hWilFile);
					CloseHandle(hWixFile);
					return FALSE;
				}
				CloseHandle(hWilMappedFile);
			}
			else
			{
				DWORD dwFileSize;
				dwFileSize = GetFileSize(hWilFile, NULL);
				m_pbStartData = new BYTE[dwFileSize];
				ReadFile(hWilFile, m_pbStartData, dwFileSize, &dwReadLen, NULL);
			}

			NewSetIndex(0);						// 0번 인덱스로 세팅한다.
			CloseHandle(hWilFile);
			return TRUE;
		}

		wsprintf(szMsg, "Cannot find %s file", szWixFile);
		// Debug
		MessageBox(NULL, szMsg, "Mir2EX :: Error", MB_ICONERROR | MB_OK);
		CloseHandle(hWixFile);
		return FALSE;
	}

	wsprintf(szMsg, "Cannot find %s file", szWixFile);
	// Debug
	MessageBox(NULL, szMsg, "Mir2EX :: Error", MB_ICONERROR | MB_OK);

	return FALSE;

}
