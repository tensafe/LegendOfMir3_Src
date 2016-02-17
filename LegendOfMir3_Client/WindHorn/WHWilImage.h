/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/



#ifndef _WILIMAGEHANDLER_
#define	_WILIMAGEHANDLER_



/******************************************************************************************************************

	CWilImageData Class Declaration

   윌이미지를 메모리에 로딩하거나 파일 메모리맵형태로 원하는 위치로 세팅한다.

*******************************************************************************************************************/


#pragma pack(1)

typedef struct tagWILFILEHEADER
{
	CHAR	szTmp[40];
	INT		nImageCount;
}WILFILEHEADER, *LPWILFILEHEADER;

typedef struct tagWILFILEIMAGEINFO
{
	SHORT	shWidth;
	SHORT	shHeight;
	SHORT	shPX;
	SHORT	shPY;
	DWORD	dwImageLength;
}WILIMAGEINFO, *LPWILIMAGEINFO;

typedef struct tagWIXFILEIMAGEINFO
{
	CHAR	szTmp[40];
	INT		nIndexCount;
	INT*	pnPosition;
}WIXIMAGEINFO, *LPWIXIMAGEINFO;


typedef struct tagNEWWILFILEHEADER
{
	SHORT	shComp;
	CHAR	szTitle[20];
	SHORT	shVer;
	INT		nImageCount;
}NEWWILFILEHEADER, *LPNEWWILFILEHEADER;

typedef struct tagNEWWILFILEIMAGEINFO
{
	SHORT	shWidth;
	SHORT	shHeight;
	SHORT	shPX;
	SHORT	shPY;
	CHAR	bShadow;					
	SHORT	shShadowPX;
	SHORT	shShadowPY;
	DWORD	dwImageLength;
}NEWWILIMAGEINFO, *LPNEWWILIMAGEINFO;
typedef struct tagNEWWIXFILEIMAGEINFO
{
	CHAR	szTitle[20];
	INT		nIndexCount;
	INT*	pnPosition;
}NEWWIXIMAGEINFO, *LPNEWWIXIMAGEINFO;

#pragma pack(8)


class CWHWilImageData
{
private:
protected:
	BOOL				m_bIsMemMapped;				// 메모리맵사용여부.
	BOOL				m_bIsCompressed;			// 압축여부.

	LPBYTE				m_pbStartData;				// 데이타의 시작주소.(전체 데이타영역은 파일크기와 같다.)

	INT					m_nCurrImageIdx;			// 현재 가리키고 있는 인덱스.


public:
	WIXIMAGEINFO		m_stWixImgaeInfo;			// Wix 파일데이타.
	NEWWIXIMAGEINFO		m_stNewWixImgaeInfo;

	LPWILIMAGEINFO		m_lpstCurrWilImageInfo;		// 현재 윌파일의 정보를 가리키는 포인터.
	LPNEWWILIMAGEINFO	m_lpstNewCurrWilImageInfo;	

	BYTE*				m_pbCurrImage;				// 현재 윌파일의 이미지를 가리키는 포인터.
	TCHAR				m_szWilFileName[MAX_PATH];

	CWHWilImageData();
	~CWHWilImageData();

	BOOL NewLoad(CHAR* szWilFile, BOOL bIsMemMapped = TRUE, BOOL bComp = TRUE);
	BOOL NewSetIndex(DWORD dwIndex);				// 외부에서 전달받은 인덱스를 가지고 현재이미지의 정보를 가리키는 주소를 Setting한다.


	BOOL Load(CHAR* szWilFile, BOOL bIsMemMapped = TRUE, BOOL bComp = TRUE);
	VOID Init();
	VOID Destroy();
	VOID SetIndex(INT nIndex);						// 외부에서 전달받은 인덱스를 가지고 현재이미지의 정보를 가리키는 주소를 Setting한다.
};


#endif //_WILIMAGEHANDLER_
