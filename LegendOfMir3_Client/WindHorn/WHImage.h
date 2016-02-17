#ifndef _WINDHORN_IMAGEHANDLER
#define	_WINDHORN_IMAGEHANDLER


class CWHImage
{
//1: Constructor/Destructor
public:
	CWHImage();
	~CWHImage();

//2: Variables
public:
	WORD	m_wWidth;
	WORD	m_wHeight;

	SHORT	m_shCenterX;
	SHORT	m_shCenterY;

	WORD*	m_pwImage;
	BYTE	m_bBitCount;

	DWORD	m_dwImageLength;

//3: Methods
	BOOL LoadBitmap(LPCTSTR pszFileName, BOOL fCut = FALSE, BOOL fCompress = FALSE, BYTE bBitCount = 16);
	BOOL LoadBitmapFromRes(LPCTSTR lpResName, LPCTSTR lpTypeName, BOOL fCut = FALSE, BOOL fCompress = FALSE, BYTE bBitCount = 16);
	BOOL Convert24To16(BYTE *pOrgImage, BYTE bOrgImgBitCount);
	BOOL Optimize();
// æ–√‡
	BOOL XOptimize();


	__inline VOID DeleteImage()	{ if ( m_pwImage ) { delete m_pwImage; m_pwImage = NULL; } }

//4: Message Map
};


#endif //_WINDHORN_IMAGEHANDLER