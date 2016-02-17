#ifndef _WINDHORN_DXGRAPHIC
#define	_WINDHORN_DXGRAPHIC



/******************************************************************************************************************

	CWHDXGraphicWindow Class Declaration

*******************************************************************************************************************/

typedef struct tagDXG_DISPLAYINFO 
{
	WORD			wWidth;
	WORD			wHeight;
	WORD			wBPP;
}DXG_DISPLAYINFO, *LPDXG_DISPLAYINFO;

typedef struct DXG_ENUM_DEVICEINFO
{
    // 3D 디바이스 정보.
    TCHAR			szDeviceDesc[256];
    GUID*			pDeviceGUID;
    D3DDEVICEDESC7	ddDeviceDesc;
    BOOL			f3DHardware;
    // 드로우 드라이버 정보.
	TCHAR			szDriverDesc[256];
    GUID*			pDriverGUID;
    DDCAPS			ddDriverCaps;
    DDCAPS			ddHELCaps;

    GUID            guidDevice;
    GUID            guidDriver;
    DDSURFACEDESC2* pddsdModes;
    DWORD           dwNumModes;
}DXG_ENUM_DEVICEINFO, *LPDXG_ENUM_DEVICEINFO;

typedef struct tagDXG_MASKINFO
{
	BYTE			bRCnt;
	BYTE			bGCnt;
	BYTE			bBCnt;
	BYTE			bRShift;
	BYTE			bGShift;
	BYTE			bBShift;
	DWORD			dwRMask;
	DWORD			dwGMask;
	DWORD			dwBMask;
}DXG_MASKINFO, *LPDXG_MASKINFO;


class CWHDXGraphicWindow : public CWHWindow
{
//1: Constuctor/Destructor
public:
	CWHDXGraphicWindow(WORD wWidth = _SCREEN_WIDTH, WORD wHeight = _SCREEN_HEIGHT, WORD wBpp = _SCREEN_BPP);
	~CWHDXGraphicWindow();

//2: Variables
protected:
	LPDIRECT3D7				m_pD3D;
    LPDIRECTDRAW7			m_pDD;
	LPDIRECT3DDEVICE7		m_pd3dDevice;
    LPDIRECTDRAWSURFACE7	m_pddsFrontBuffer;
    LPDIRECTDRAWSURFACE7	m_pddsBackBuffer;
	LPDIRECTDRAWSURFACE7	m_pddsZBuffer;
	LPDIRECTDRAWCLIPPER		m_lpcClipper;

///////<---------------------------------------------------------------------------------------------------------------------------
	HFONT					m_hDefGameFont;
///////<---------------------------------------------------------------------------------------------------------------------------

	BYTE					m_bScreenModeFlag;
    BYTE					m_bDeviceModeFlag;

public:
	DWORD					m_dwTextureTotal;	// 전체 사용할수 있는 메모리.
	DWORD					m_dwTextureFree;    // 현재 남은 메모리.
	DWORD					m_dwVideoTotal; 	// vram 전체 메모리.
	DWORD					m_dwVideoFree;      // 현재 남은 vram 메모리 .

	BOOL					m_bIsWindowActive;
	BOOL					m_bIsWindowReady;

    RECT					m_rcWindow;
	DXG_DISPLAYINFO			m_stDisplayInfo;
	DXG_MASKINFO			m_stBitsMaskInfo;

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// 프로세스를 나누기위한 변수선언.
	static CWHDXGraphicWindow*	m_pxDXGWnd;
	CWHDefProcess*				m_pxDefProcess;

	VOID					(*RenderProcess)(int);
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//3: Methods
protected:
	BYTE	FindDriverAndDevice();
	VOID	FreeDXGEnumModeResources();

	HRESULT CreateDXG();
	HRESULT	CreatePrimarySurface();
	HRESULT CreateZBuffer(GUID* pDeviceGUID);
	HRESULT Init3DDeviceObjects();
	HRESULT DestroyDXGObjects();

public:

    __inline LPDIRECTDRAW7			GetDirectDraw()     { return m_pDD;				}
    __inline LPDIRECT3D7			GetDirect3D()		{ return m_pD3D;			}
	__inline LPDIRECT3DDEVICE7		Get3DDevice()		{ return m_pd3dDevice;		}
    __inline LPDIRECTDRAWSURFACE7	GetFrontBuffer()    { return m_pddsFrontBuffer; }
    __inline LPDIRECTDRAWSURFACE7	GetBackBuffer()     { return m_pddsBackBuffer;	}
    __inline BYTE					GetDeviceMode()     { return m_bDeviceModeFlag; }
	__inline BYTE					GetScreenMode()		{ return m_bScreenModeFlag; }

    DXG_MASKINFO			GetRGBMaskInfoIN16Bits(LPDIRECTDRAWSURFACE7 pSurface);
	virtual VOID			UpdateBoundsWnd();

	HRESULT					ResetDXG(WORD wWidth, WORD wHeight, WORD wBPP, BYTE bScreenModeFlag, BYTE bDeviceModeFlag);
	BOOL					Create(HINSTANCE hInst, LPTSTR lpCaption = NULL, CHAR *pszMenuName = NULL, CHAR* pszIconName = NULL, BYTE bScreenModeFlag = _DXG_SCREENMODE_WINDOW, BYTE bDeviceModeFlag = _DXG_DEVICEMODE_PRIMARY);

///////<---------------------------------------------------------------------------------------------------------------------------
	HFONT					CreateGameFont(LPCSTR szFontName, INT nHeight, INT nWidth = 0, INT nWeight = FW_NORMAL, BOOL bItalic = FALSE, BOOL bULine = FALSE, BOOL bStrikeOut = FALSE, DWORD dwCharSet = HANGEUL_CHARSET);
	VOID					CreateDefFont();

	VOID					PutsHan(LPDIRECTDRAWSURFACE7 pSurface, INT nX, INT nY, COLORREF foreColor, COLORREF backColor, CHAR* szText, HFONT hFont = NULL);
	VOID					PutsHan(LPDIRECTDRAWSURFACE7 pSurface, RECT rc, COLORREF foreColor, COLORREF backColor, CHAR* szText, HFONT hFont = NULL);

	SIZE					GetStrLength(LPDIRECTDRAWSURFACE7 pSurface, HFONT hFont, CHAR* szFormat, ...);
///////<---------------------------------------------------------------------------------------------------------------------------

	BOOL 					StringDivide(INT nDivideWidth, INT& nDividedLine, CHAR* szSrc, CHAR* szResult);
//	BOOL					StringDivideLen(INT nDivideLen, INT& nDividedLine, CHAR* szSrc, CDLList<CHAR*>* m_pxpStr);

	VOID 					StringPlus(CHAR* szResult, CHAR* szSrc, ...);
	CHAR*					IntToStr(INT nNum);

	VOID					UsedAndFreeMemoryCheck();


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	WORD					ConvertColor24To16(COLORREF dwColor);
	// 렌더링 함수모음.
	// 1. GDI용 그리기 함수.(선, 사각형등을 백서피스에 그린다.)
	HRESULT					DrawWithGDI(RECT rc, LPDIRECTDRAWSURFACE7 pSurface, DWORD dwColor, BYTE bKind);
	// 2. WORD형 데이타 그리기 함수.(픽셀단위로 컬러키를 검사하면서 백서피스에 그린다.)
	BOOL					DrawWithImage(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc);
	// 3. WORD형 데이타 그리기 함수.(라인단위로 컬러키를 고려하지 않고 백서피스의 클리핑 내부영역에만 그린다.)
	BOOL					DrawWithImagePerLineClipRgn(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, WORD wClipWidth, WORD wClipHeight);
	// 4. Surface형 데이타 그리기 함수.(소스의 컬러키를 고려하면서 백서피스에 그린다.)
	BOOL					DrawWithSurface(INT nX, INT nY, INT nXSize, INT nYSize, LPDIRECTDRAWSURFACE7 pddsSrc);

	// wChooseColor1은 염색옷(0xC2)에 대해서, wChooseColor2(0xC3)는 머리등(칼, 기타 변하는색)의 염색으로 쓴다.
	// 5. 압축(0번)및 염색된 WORD형 데이타 그리기 함수.(백서피스에 그린다.)
	BOOL					DrawWithImageForComp(INT nX, INT nY, RECT rcSrc, WORD* pwSrc, WORD wChooseColor1 = 0XFFFF, WORD wChooseColor2 = 0XFFFF);
	BOOL					DrawWithImageForComp(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, WORD wChooseColor1 = 0XFFFF, WORD wChooseColor2 = 0XFFFF);
	// 6. 압축(0번)및 염색된 WORD형 데이타 그리기 함수.(백서피스의 클리핑 내부영역에만 그린다.)
	BOOL					DrawWithImageForCompClipRgnBase(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, WORD wClipWidth, WORD wClipHeight, WORD wChooseColor1 = 0XFFFF, WORD wChooseColor2 = 0XFFFF);
	__inline VOID			DrawWithImageForCompClipRgn(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, WORD wClipWidth, WORD wClipHeight, WORD wChooseColor1 = 0XFFFF, WORD wChooseColor2 = 0XFFFF, BOOL bLighted = FALSE)
	{
		if ( bLighted )		DrawWithABlendCompDataWithLightedColor(nX, nY, nXSize, nYSize, pwSrc, wClipWidth, wClipHeight, wChooseColor1, wChooseColor2);
		else				DrawWithImageForCompClipRgnBase(nX, nY, nXSize, nYSize, pwSrc, wClipWidth, wClipHeight, wChooseColor1, wChooseColor2);
	}


	// 7. 압축(0번)및 염색된 WORD형 데이타 그리기 함수.(소스메모리의 영역에서 데스트영역메모리로 그린다.)
	BOOL					DrawWithImageForCompMemToMem(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, INT nDstXSize, INT nDstYSize, WORD* pwDst, WORD wChooseColor1 = 0XFFFF, WORD wChooseColor2 = 0XFFFF);
	// 8. 압축(0번)및 염색된 WORD형 데이타 그리기 함수.(압축된 데이타의 소스메모리의 영역에서 일부렉트영역만을 데스트영역메모리로 그린다.)
	BOOL					DrawWithImageForCompToMem(RECT rcWanted, WORD* pwSrc, WORD wChooseColor1, WORD wChooseColor2, WORD* pwDst);
	BOOL					DrawWithImageForCompColorToMem(RECT rcWanted, WORD* pwSrc, WORD wColor, WORD* pwDst);
	// 9. 압축(0번)및 염색된 WORD형 데이타 그리기 함수.(압축된 데이타의 소스영역과 데스트영역의 교차렉트를 알파블렌딩해서 백서피스에그린다.)
	BOOL					DrawWithABlendForIntersectCompData(INT nSrcX, INT nSrcY, 
															   INT nSrcXSize, INT nSrcYSize, WORD* pwSrc,
															   INT nDstX, INT nDstY,
															   INT nDstXSize, INT nDstYSize, WORD* pwDst, 
															   WORD wClipWidth, WORD wClipHeight,
															   BYTE bOpa = 50, BOOL bFocused = FALSE,
															   WORD wSrcChooseColor1 = 0XFFFF, WORD wSrcChooseColor2 = 0XFFFF,
															   WORD wDstChooseColor1 = 0XFFFF, WORD wDstChooseColor2 = 0XFFFF,
															   WORD wSrcColor = 0XFFFF, WORD wDstColor = 0XFFFF);
	// 10. 압축(0번)및 염색된 WORD형 데이타 그리기 함수.(압축된 데이타의 소스영역을 백서피스와 알파블렌딩한다.)
	BOOL					DrawWithABlendCompDataWithBackBuffer(INT nX, INT nY, 
																 INT nXSize, INT nYSize, WORD* pwSrc,
																 WORD wClipWidth, WORD wClipHeight,
																 WORD wChooseColor1 = 0XFFFF, WORD wChooseColor2 = 0XFFFF, BYTE bOpa = 50);
	// 11. 압축(0번)및 염색된 WORD형 데이타 그리기 함수.(압축된 데이타의 소스영역을 원하는 컬러로 라이팅 상태로(밝게) 백서피스에 그린다.)
	BOOL					DrawWithABlendCompDataWithLightedColor(INT nX, INT nY, 
																   INT nXSize, INT nYSize, WORD* pwSrc,
																   WORD wClipWidth, WORD wClipHeight,
																   WORD wChooseColor1 = 0XFFFF, WORD wChooseColor2 = 0XFFFF);
	// 그림자 그리기 함수.
	// 12. 압축(0번)및 염색된 WORD형 데이타 그리기 함수.(압축된 데이타의 소스영역의 그림자를 백서피스에 알파블렌딩한다. 그림자는 높이가 절반으로 줄어들고 넓이는 YSize만큼 늘어나서 기울여 백서피스에 찍힌다.)
	BOOL					DrawWithShadowABlend(INT nX, INT nY, 
												 INT nXSize, INT nYSize, WORD* pwSrc,
												 WORD wClipWidth, WORD wClipHeight, WORD* pwShadowClrSrc,
												 BOOL bBlend = FALSE, BYTE bShadowType = 48, BYTE bOpa = 50);
	VOID					DrawWithShadowABlend(INT nX, INT nY, INT nXSize, INT nYSize, 
		                                         INT nPX, INT nPY, WORD* pwSrc, WORD wClipWidth, WORD wClipHeight, WORD* pwShadowClrSrc,
												 BOOL bBlend = FALSE, BYTE bOpa = 50);
	// 컬러처리.
	// 13.
	BOOL					DrawWithImageForCompClipRgnColor(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, WORD wClipWidth, WORD wClipHeight, WORD wColor, BOOL bFocused = FALSE, BOOL bBlend = FALSE);
	BOOL					DrawWithImageForCompClipRgnColor(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, WORD wClipWidth, WORD wClipHeight, WORD wColor, WORD wChooseColor1 = 0XFFFF, WORD wChooseColor2 = 0XFFFF);
	// 14.
	BOOL					DrawWithImageForCompClipRgnGray(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc , WORD wClipWidth, WORD wClipHeight, WORD wChooseColor1 = 0XFFFF, WORD wChooseColor2 = 0XFFFF);
	// 흑백처리.
	// 15.
	VOID					DrawWithGrayBackBuffer();

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


	HRESULT					Present();
	HRESULT					RestoreSurfaces();

//4: Message Map
public:
	virtual LRESULT MainWndProcDXG(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSize(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnMove(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnSysKeyDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT OnGetMinMaxInfo(WPARAM wParam, LPARAM lParam);


protected:

	__inline virtual LRESULT MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{ 
		if ( m_pxDefProcess )	return m_pxDefProcess->DefMainWndProc(hWnd, uMsg, wParam, lParam);
		else					return MainWndProcDXG(hWnd, uMsg, wParam, lParam); 
	}

	virtual LRESULT	OnDestroy();
	virtual LRESULT	OnSetCursor();
};

CWHDXGraphicWindow*		GetDXGWindowPtr();

#endif //_WINDHORN_DXGRAPHIC
