// Patch.cpp: implementation of the CPatch class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Patch.h"

#define PATCH_SECTION			"PATCH_FTP"
#define PATCH_VERSION			"LastUpdate"
#define PATCH_SETVER_NAME		"FTPSerVerIP"
#define PATCH_SERVER_PORT		"FTPSerVerPort"
#define PATCH_USER_ID			"UserID"
#define PATCH_USER_PASS			"PassWord"

#define IDB_PATCH_BASE                  109
#define IDB_PATCH_COVER                 110

#define DEFAULT_PATCH_SERVER	"192.168.0.202" //"wemade.nefficient.co.kr"
#define DEFAULT_PATCH_PORT		INTERNET_DEFAULT_FTP_PORT
#define DEFAULT_PATCH_UID		"anonymous"
#define DEFAULT_PATCH_PASS		NULL
#define DEFAULT_LAST_PATCH_DATE	"20020120"
#define DEFAULT_VERSION			"20011219"
#define DEFAULT_REG_PATH		"SOFTWARE\\WEMADE Entertainment\\미르의전설2ei"

#define PATCH_INI_FILE_NAME		"\\Mir2Patch.ini"
#define PATCHED_LIST_FILE		"Patch.lst"
#define PATCH_LIST_FILE_LIST	"Mir2Patch.lst"
#define PATCH_LAST_DATE			"Last Patch Date"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////


CPatch::CPatch()
{
	char	m_szPatchServerIP[16];
	char	FullPathFileName[MAX_PATH];

	m_TheAmount		=	0;
	m_AnAmount		=	0;
	m_PatchState	=	0;
	m_HInet			=	NULL;
	m_HFileFtp		=	NULL;
	m_HFtp			=	NULL;

	GetCurrentDirectory(MAX_PATH,FullPathFileName);
 	strcat(FullPathFileName,PATCH_INI_FILE_NAME);

	m_Server_Port	=	DEFAULT_PATCH_PORT;

						GetPrivateProfileString(PATCH_SECTION,PATCH_SETVER_NAME,DEFAULT_PATCH_SERVER
												,m_szPatchServerIP,16,FullPathFileName);

	m_Server_Port	=	GetPrivateProfileInt(PATCH_SECTION,PATCH_SERVER_PORT,m_Server_Port,FullPathFileName);
						GetPrivateProfileString(PATCH_SECTION,PATCH_USER_ID,DEFAULT_PATCH_UID
												,m_UID,11,FullPathFileName);
						GetPrivateProfileString(PATCH_SECTION,PATCH_USER_PASS,DEFAULT_PATCH_PASS
												,m_PASS,11,FullPathFileName);

	m_HInet = OpenInet("MIR2Patch");
	m_HFtp	= ConnectInet(m_HInet,m_szPatchServerIP,m_Server_Port,m_UID,m_PASS);
}

CPatch::~CPatch()
{
	for(int i = 0 ; i< m_FACount;i++)
		delete m_ppFilesData[i];

	if ( m_HFtp )
	{
		InternetCloseHandle(m_HFtp);
		m_HFtp = NULL;
	}

	if ( m_HInet)
	{
		InternetCloseHandle(m_HInet);
		m_HInet = NULL;
	}

	if(m_xBmp[0]!=NULL) DeleteObject(m_xBmp[0]);
	if(m_xBmp[1]!=NULL) DeleteObject(m_xBmp[1]);
}


short CPatch::Progress(long Total, long Current)
{

	if( Total < Current ) return 0;

	return (short((Current * 100.0) / Total));
}

HINTERNET CPatch::OpenInet(char* name)
{
	HINTERNET	tHInet;
	tHInet = InternetOpen(name,INTERNET_OPEN_TYPE_PRECONFIG,NULL,NULL,0);
	if(tHInet == NULL)
	{
		// Error Handle
	}
	return tHInet;
}

HINTERNET CPatch::ConnectInet(HINTERNET hInet, char *sIP, WORD lPort, char *pID, char *pPass)
{
	DWORD		LenStr;
	DWORD		dwErrorCode;
	char		szErrorBuffer[512];
	HINTERNET	thInet;

	thInet = InternetConnect( hInet,sIP,lPort,pID, pPass, INTERNET_SERVICE_FTP, INTERNET_FLAG_PASSIVE,0);
	if ( thInet == NULL )
	{
		LenStr =0;
		dwErrorCode =0;
		dwErrorCode = GetLastError();
		InternetGetLastResponseInfo(&dwErrorCode,szErrorBuffer,&LenStr);
	}
	return thInet;
}


BOOL	CPatch::WillBePatch(VOID)
{
	int				CmpResult;
	int				BufferSize;
	char			Version[8];
	char			szTemp[MAX_PATH];
	char			FileBuffer[4100];
	DWORD			dwReadLen;
	DWORD			dwErrNum;
	FILE*			HFile;
	FileListHeader	FLHeader;

	BufferSize=0;
	dwReadLen = 0;
	BufferSize = 4100;
	ZeroMemory(FileBuffer,4100);

	m_hFile = NULL;
	m_hFile = CreateFile (PATCH_LIST_FILE_LIST, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);

	if((dwErrNum=GetLastError())==ERROR_FILE_EXISTS)
	{
		// 화일이 존재하면 다시 받아라
		DeleteFile(PATCH_LIST_FILE_LIST);
		m_hFile = CreateFile (PATCH_LIST_FILE_LIST, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
	}

	// Make Update Directory from FTP
	m_szPatchDate[8] = '\0';
	strcpy(szTemp,m_szPatchDate);
	strcat(szTemp,"\\");
	strcat(szTemp,PATCH_LIST_FILE_LIST);
	m_HFileFtp = FtpOpenFile( m_HFtp,szTemp, GENERIC_READ, FTP_TRANSFER_TYPE_BINARY, 0);

	while(1)						// 화일 받는 부분.
	{
		if (InternetReadFile( m_HFileFtp, FileBuffer, (DWORD)BufferSize, &dwReadLen) != FALSE)
		{
			if ( (int)dwReadLen != BufferSize )
			{
				BufferSize = (int)dwReadLen;
				WriteFile( m_hFile, FileBuffer, (DWORD)BufferSize, &dwReadLen, NULL);
				break;
			}
			else
			{
				WriteFile( m_hFile, FileBuffer, (DWORD)BufferSize, &dwReadLen, NULL);
			}
		}
		else 
		{
			break;
		}
	}

	CloseHandle(m_hFile);
	InternetCloseHandle(m_HFileFtp);

	m_PatchState	=	0;
	m_hFile			=	NULL;
	m_HFileFtp		=	NULL;

	if(!jRegGetKey(_T(DEFAULT_REG_PATH), _T(PATCH_LAST_DATE), (LPBYTE)Version))
	{
		strcpy(Version,DEFAULT_LAST_PATCH_DATE);	// Default Last Patch Date
	}

	ZeroMemory(FLHeader.VersionDate,sizeof(FLHeader.VersionDate));

	if((HFile = fopen(PATCH_LIST_FILE_LIST,"r")) == NULL)	return FALSE;		

	fread(FLHeader.VersionDate,sizeof(char),sizeof(FLHeader.VersionDate),HFile);

	fclose(HFile);

	FLHeader.VersionDate[sizeof(FLHeader.VersionDate)] = NULL;

	CmpResult=strncmp(FLHeader.VersionDate, Version,8);	

	if(CmpResult>0)
	{
		return TRUE;
		// Patch 받아야 함
	}
	else
	{
		if(CmpResult==0)
		{
			// Patch 받을 필요 없음
			return FALSE;
		}
		else
		{
			// 현제 가지고 있는것이 서버에서 지원하는거 보다 최근 버전이다 ㅡ.ㅡ?
			return FALSE;
		}
	}
}


HRESULT CPatch::GetFtpFiles(VOID)	// 여기에서 File List를 분석해서 GetFile을 계속 돌려줄것  return -1l : Error  0: End 1: Continue
{
	static int		Pos=0;
	char			szTemp[MAX_PATH];
	char			szDirStr[MAX_PATH];
	char			szFileName[MAX_PATH];

	UnCompressedFileNode* FileNode;
// 여기에서 Last Patch Date를 얻어야 한다.
	if(m_bEndPatch != TRUE)
	{
		if(m_PatchState == 3)
		{
			m_bEndPatch = TRUE;
			return 0l;
		}
		else
		{
			if((m_PatchState == 2) || (m_PatchState == 0)) 
			{
				Pos ++;
				if(m_PatchState == 2)
				{
					if(m_hFile != NULL)   
					{
						CloseHandle(m_hFile);
						m_hFile = NULL;
					}

	  				if(m_HFileFtp != NULL) 
					{
						InternetCloseHandle(m_HFileFtp);
						m_HFileFtp=NULL;
					}

					if(Pos > m_FACount)
					{
						m_PatchState = 0;
						m_bPatched   = TRUE;
						m_bEndPatch  = TRUE;
						return 0l;
					}
				}

				FileNode = GetFileNode(Pos - 1);
				ZeroMemory(szFileName, MAX_PATH);
				ZeroMemory(szTemp, MAX_PATH);

				DWORD dwErrNum;
				ZeroMemory(szDirStr, MAX_PATH);

				if(strcmp(FileNode->szFileName, "Mir2Patch.exe") == 0)	// Patch 화일 일 경우
				{
					DeleteFile(FileNode->szFileName);
					if(!jRegGetKey(_T(DEFAULT_REG_PATH), _T("setup path"), (LPBYTE)szDirStr))
					{
						ZeroMemory(szDirStr, MAX_PATH);
						GetCurrentDirectory(MAX_PATH, szDirStr);
						szDirStr[strlen(szDirStr)] ='\\';
						szDirStr[strlen(szDirStr)] = NULL;
					}
					for ( INT nCnt = strlen(szDirStr); nCnt >= 0; nCnt--)
					{
						if ( szDirStr[nCnt] == '\\' )
						{
							szDirStr[nCnt+1] = NULL;
							break;
						}
					}					
				}
				else
				{
					strcpy(szDirStr, m_UpdatePath);
				}

				strcpy(szFileName, FileNode->szFileName);
				strcpy(szTemp, szDirStr);
				strcat(szTemp, szFileName);
					
				m_hFile = CreateFile (szTemp, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
				if((dwErrNum=GetLastError()) == ERROR_FILE_EXISTS)
				{
					// 화일이 이미 존재 할 경우 그 화일의 크기를 확인
					// 업데이트 하려는 화일보다 작은 화일의 경우
					// 재대로 업데이트 되지 않은 화일 이므로 다시 업데이트를 받는다.
					m_hFile = CreateFile (szTemp, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
					DWORD High_FileSize;
					DWORD Low_FileSize;
					High_FileSize = 0;
					Low_FileSize = 0;
					Low_FileSize = GetFileSize(m_hFile, &High_FileSize);
					CloseHandle(m_hFile);
					m_hFile = NULL;
					if(Low_FileSize <= (DWORD)FileNode->lFileSize)
					{
						BOOL bResult = DeleteFile(szTemp);
						if (bResult)
						{
							//	Success
			//					MessageBox(NULL,"File Delete & ReCreate","Message",MB_OK); 
							m_hFile = CreateFile (szTemp, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, NULL);
						}
						else
						{
			//					MessageBox(NULL,"File Delete Fail","Message",MB_OK); 
							//	Fail 
							return -1l;
						}
					}
				}
				strcpy(szTemp, m_szPatchDate);
				strcat(szTemp, "\\");
				strcat(szTemp, szFileName);
				m_HFileFtp = FtpOpenFile(m_HFtp, szTemp, GENERIC_READ, FTP_TRANSFER_TYPE_BINARY, 0);
			}
			else
			{
				FileNode = GetFileNode(Pos - 1);
			}
			strcpy(szTemp, m_szPatchDate);
			strcat(szTemp, "\\");
			strcat(szTemp, szFileName);
			GetFile(szTemp);		// 화일 계속 받기 ^^;
		}
	}
	else
	{
		// End Patch
		if(m_HFileFtp != NULL) 
		{
			InternetCloseHandle(m_HFileFtp);
			m_HFileFtp = NULL;
		}
		m_bPatched = TRUE;
		return 0l;
	}
	return 1l;
}


HRESULT CPatch::GetFile(char* nName)
{
	static long CurrentSize = 0;
	int			BufferSize;
	DWORD		dwBytes;
	HANDLE		hFind;
	char		FileBuffer[4100];

	hFind		= NULL;
	BufferSize	= 4100;

	ZeroMemory(FileBuffer, 4100);
	if (InternetReadFile( m_HFileFtp, FileBuffer, (DWORD)BufferSize, &dwBytes) != FALSE)
	{
		m_AnAmount += dwBytes;

		if ( (int)dwBytes != BufferSize )
		{
			BufferSize = (int)dwBytes;
			WriteFile( m_hFile, FileBuffer, (DWORD)BufferSize, &dwBytes, NULL);
			m_PatchState	=	2;
		}
		else
		{
			WriteFile( m_hFile, FileBuffer, (DWORD)BufferSize, &dwBytes, NULL);
			m_PatchState	=	1;
		}
	}
	else
	{
		m_PatchState	=	2;
	}
	return 0L;
}


VOID CPatch::RenderSence(INT nLoopTime)
{
	if ( FAILED(g_xMainWnd.Present()) )
	{
		g_xMainWnd.RestoreSurfaces();
	}
}


VOID CPatch::DrawProgressImage(VOID)
{
	long	lSpace;
	double	dSpace;
	int		Height;
	int		Width;
	RECT	tRect;
	HDC		hdc;
	HDC		MemDC,MemDC2;
	HBITMAP OldBitmap1;
	HBITMAP OldBitmap2;
	BITMAP	bit;
	PAINTSTRUCT PaintStruct;

	BeginPaint(g_xMainWnd.GetSafehWnd(),&PaintStruct);
	GetClientRect(g_xMainWnd.GetSafehWnd(),&tRect);

	hdc		= GetDC(g_xMainWnd.GetSafehWnd());
	MemDC	= CreateCompatibleDC(hdc);
	MemDC2	= CreateCompatibleDC(hdc);

	OldBitmap1	= (HBITMAP)SelectObject(MemDC, m_xBmp[0]);
	OldBitmap2	= (HBITMAP)SelectObject(MemDC2, m_xBmp[1]);

	GetObject(m_xBmp[0],sizeof(BITMAP), &bit);

	Width	= bit.bmWidth;
	Height	= bit.bmHeight;
	dSpace	= (Height / 100.0) * (100 - Progress(m_TheAmount, m_AnAmount % (m_TheAmount + 1)));
	lSpace	= (long)dSpace;

	if(lSpace < 0) lSpace = 0;


	BitBlt(MemDC, 0, lSpace, tRect.right, tRect.bottom, MemDC2, 0, lSpace, SRCCOPY);
	BitBlt(hdc, 0, 0, tRect.right, tRect.bottom, MemDC, 0, 0, SRCCOPY);

	SelectObject(MemDC, OldBitmap1);
	SelectObject(MemDC2, OldBitmap2);

	DeleteDC(MemDC);
	DeleteDC(MemDC2);
	EndPaint(g_xMainWnd.GetSafehWnd(), &PaintStruct);
	InvalidateRect(g_xMainWnd.GetSafehWnd(), &tRect, FALSE);	
}

VOID CPatch::InitPatch(VOID)
{
	int				Width,Height;
	LONG			Left,Top;
	LONG			tWidth,tHeight;
	RECT			rcWork;
    DWORD			dwFrameWidth;
    DWORD			dwFrameHeight;
	DWORD			dwMenuHeight;
	FILE*			HFile;
	char			szDirStr[MAX_PATH];
	char			szFileName[MAX_PATH];
	char			szTemp[MAX_PATH];
	FileListHeader	FLHeader;


	m_ppFilesData	= NULL;
	m_bEndPatch		= TRUE;
	m_bPatched		= FALSE;
	m_PatchState	= 0;	// Start
	Width			= 479;
	Height			= 359;
	dwMenuHeight	= 0;
	dwFrameHeight   = 0;
	dwFrameWidth    = 0;

	// 바탕화면의 RECT를 구한다.
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0);

	// 현제 설정된 Window의 Rect를 보존
	tWidth =g_xMainWnd.m_rcWindow.right;
	tHeight = g_xMainWnd.m_rcWindow.bottom;

	// Window의 (x,y)좌표 얻기
	Left = (rcWork.right - Width) / 2;
	Top =  (rcWork.bottom - Height) / 2;

	//Window의 시작 좌표를 설정
	g_xMainWnd.m_rcWindow.left  = Left + dwFrameWidth;
	g_xMainWnd.m_rcWindow.top	= Top + dwFrameHeight + dwMenuHeight;

	// 바뀌게될 window의 Width와 Height
	Width = Width + dwFrameWidth * 2;
	Height = Height + dwFrameHeight * 2 + dwMenuHeight;

	SetWindowPos(g_xMainWnd.GetSafehWnd(), HWND_TOP, Left, Top, Width, Height, NULL);

	ShowWindow(g_xMainWnd.GetSafehWnd(), SW_SHOW);

	g_xMainWnd.m_rcWindow.right		= 800 + Left + dwFrameWidth;
	g_xMainWnd.m_rcWindow.bottom	= 600 + Top + dwFrameHeight + dwMenuHeight;

	if(CheckPatchDate())	// Patch Date List Check
	{
		if(WillBePatch())	// Patch Data List Check
		{
			// 2 개의 Patch Check 가 모두 성공해야만 가능하다.
			m_bEndPatch = FALSE;
			// 페치가 있을때만, 이미지를 Convert 한다.
			ZeroMemory((BYTE*)&FLHeader,sizeof(FileListHeader));

			HFile = NULL;

			if((HFile =fopen(PATCH_LIST_FILE_LIST,"r"))==NULL)
			{
				m_bPatched	= FALSE;
				m_bEndPatch	= TRUE;
				fclose(HFile);
				return;
			};

			fseek(HFile, 0, SEEK_SET);
			fscanf(HFile, "%s",FLHeader.VersionDate);
			fscanf(HFile, "%d",&m_FACount);
			fscanf(HFile, "%ld",&m_TheAmount);

			if(!jRegGetKey(_T(DEFAULT_REG_PATH), _T("setup path"), (LPBYTE)szDirStr))
			{
				ZeroMemory(szDirStr,MAX_PATH);
				GetCurrentDirectory(MAX_PATH,szDirStr);
				szDirStr[strlen(szDirStr)] = '\\';
				szDirStr[strlen(szDirStr)] = NULL;
			}

			for ( INT nCnt = strlen(szDirStr); nCnt >= 0; nCnt--)
			{
				if ( szDirStr[nCnt] == '\\' )
				{
					szDirStr[nCnt+1] = NULL;
					break;
				}
			}

			// Update 할 Directory 를 생성 
			strcat(szDirStr, "UpDate\\");			// Update Directory를 생성
			strcpy(m_UpdatePath, szDirStr);			// Update Path를 가지고 있을것 
			CreateDirectory(szDirStr, NULL);

			// Patch 정보를 얻는다.
			m_ppFilesData = new UnCompressedFileNode* [m_FACount];

			ZeroMemory(szFileName,MAX_PATH);
			ZeroMemory(szTemp,MAX_PATH);
			// 개개 화일 정보 읽기
			for(int i = 0; i< m_FACount ;i ++)
			{
				m_ppFilesData[i] = new UnCompressedFileNode;
				fscanf(HFile,"%s", m_ppFilesData[i]->szFileName);
				fscanf(HFile,"%d", &m_ppFilesData[i]->lDirectory);
				fscanf(HFile,"%s", m_ppFilesData[i]->szTargetFileName);
				fscanf(HFile,"%d", &m_ppFilesData[i]->ActionMode);
				fscanf(HFile,"%d", &m_ppFilesData[i]->Position);
				fscanf(HFile,"%d", &m_ppFilesData[i]->lFileSize);
				fscanf(HFile,"%d", &m_ppFilesData[i]->lCount);
			}

			fclose(HFile);
			m_hLib = LoadLibrary("Pbmp.dll"); // Load Dll
			if(m_hLib!=NULL)
			{
				m_xBmp[0] = LoadBitmap(m_hLib, MAKEINTRESOURCE(IDB_PATCH_BASE)); 
				m_xBmp[1] = LoadBitmap(m_hLib, MAKEINTRESOURCE(IDB_PATCH_COVER)); 
				FreeLibrary(m_hLib);
				m_hLib = NULL;
			}
			else
			{
				m_xBmp[0] = NULL;
				m_xBmp[1] = NULL;
			}
		}
	}
}

BOOL CPatch::CheckPatchDate(VOID)
{
	int		CmpResult;
	int		BufferSize;
	char	ReadDate[9];
	char	LastUpdateDate[9];
	char	FileBuffer[4100];
	DWORD	dwReadLen;
	DWORD	dwErrNum;
	FILE*	HFile;

	dwReadLen	= 0;
	BufferSize	= 2048;
	ZeroMemory(FileBuffer, 4100);
	m_hFile = NULL;

	m_hFile = CreateFile (PATCHED_LIST_FILE, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL,NULL);

	if((dwErrNum=GetLastError())==ERROR_FILE_EXISTS)
	{
		// 화일이 존재하면 다시 받아라
		DeleteFile(PATCHED_LIST_FILE);
		m_hFile = CreateFile (PATCHED_LIST_FILE, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL,NULL);
	}

	m_HFileFtp = FtpOpenFile( m_HFtp, PATCHED_LIST_FILE, GENERIC_READ, FTP_TRANSFER_TYPE_BINARY, 0);

	while(1)						// 화일 받는 부분.
	{
		if (InternetReadFile( m_HFileFtp, FileBuffer, (DWORD)BufferSize, &dwReadLen) != FALSE)
		{
			if ( (int)dwReadLen != BufferSize )
			{
				BufferSize = (int)dwReadLen;
				WriteFile( m_hFile, FileBuffer, (DWORD)BufferSize, &dwReadLen, NULL);
				break;
			}
			else
			{
				WriteFile( m_hFile, FileBuffer, (DWORD)BufferSize, &dwReadLen, NULL);
			}
		}
		else 
		{
			break;
		}
	}
	CloseHandle(m_hFile);
	InternetCloseHandle(m_HFileFtp);

	m_hFile = NULL;
	m_HFileFtp = NULL;
	m_PatchState = 0;

	// Get Last Update Date From Reg

	if(!jRegGetKey(_T(DEFAULT_REG_PATH), _T(PATCH_LAST_DATE), (LPBYTE)LastUpdateDate))
	{
		strcpy(LastUpdateDate,DEFAULT_LAST_PATCH_DATE);	// Default Last Patch Date
	}

	if((HFile = fopen(PATCHED_LIST_FILE,"r")) == NULL)	return FALSE;
	fseek(HFile, 0L, SEEK_SET );
	while(!feof( HFile ))
	{
		ZeroMemory(ReadDate, 9);
		fread( ReadDate, sizeof( char ) * 9, 1, HFile );
//		fscanf(HFile,"%8s",ReadDate);
		// 읽었으니까 비교헤서 처리하자 ^^
		ReadDate[8]=NULL;

		CmpResult=strncmp(ReadDate, LastUpdateDate, 8);

		if(CmpResult > 0)
		{
			// m_szPatchDate에 해당 날짜가 있음.
			fclose(HFile);
			strcpy(m_szPatchDate, ReadDate);
			return TRUE;
			// Patch 받아야 함
		}
		else
		{
			if(CmpResult == 0)
			{
				// Patch 받을 필요 없음 다음 날짜 확인
				continue;
			}
			else
			{
				// 현제 가지고 있는것이 서버에서 지원하는거 보다 최근 버전이다 ㅡ.ㅡ?
				// 다음 날짜 확인 ㅡㅡ; 
				continue;
			}
		}		
	}
	return FALSE;
}

UnCompressedFileNode* CPatch::GetFileNode(int Pos)
{
	return m_ppFilesData[Pos];
}
