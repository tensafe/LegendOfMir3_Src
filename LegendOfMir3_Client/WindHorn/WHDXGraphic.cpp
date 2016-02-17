/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/


#include "stdafx.h"
#include "WHEngine.h"


/******************************************************************************************************************

	CWHDXGraphicWindow Functions

*******************************************************************************************************************/


static BYTE						g_bNumDevices = 0;
static DXG_ENUM_DEVICEINFO		g_stDXGEnumDeviceInfo[_MAX_DEVICES];

static INT						DXGModesSortCallback(const VOID* arg1, const VOID* arg2);
static BOOL		WINAPI			DXGDriverEnumCallbackEx(GUID* pGUID, LPSTR szDesc, LPSTR szName, LPVOID pContext, HMONITOR hm);
static HRESULT	WINAPI			DXGEnumDisplayModeCallback(DDSURFACEDESC2* pddsd, VOID* pParentInfo);
static HRESULT	WINAPI			DXG3DDeviceEnumCallback(TCHAR* szDesc, TCHAR* szName, D3DDEVICEDESC7* pDesc, VOID* pParentInfo);
static HRESULT	WINAPI			DXGEnumZBufferFormatsCallback(DDPIXELFORMAT* pddpf, VOID* pContext);



CWHDXGraphicWindow* CWHDXGraphicWindow::m_pxDXGWnd = NULL;

CWHDXGraphicWindow* GetDXGWindowPtr()
{
	return CWHDXGraphicWindow::m_pxDXGWnd;
}


/******************************************************************************************************************

	함수명 : CWHDXGraphicWindow::CWHDXGraphicWindow()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : WORD wWidth
	         WORD wHeight
	         WORD wBpp
	출력   : 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
CWHDXGraphicWindow::CWHDXGraphicWindow(WORD wWidth, WORD wHeight, WORD wBpp)
{
	m_pDD				= NULL;
	m_pD3D				= NULL;
	m_hDefGameFont		= NULL;
	m_pd3dDevice		= NULL;
	m_pddsZBuffer		= NULL;
    m_pddsFrontBuffer	= NULL;
    m_pddsBackBuffer	= NULL;
//	에디트윈도우 출력을 위해서 클리퍼를 세팅한다.
	m_lpcClipper		= NULL;

	m_stDisplayInfo.wWidth	= wWidth;
	m_stDisplayInfo.wHeight	= wHeight;
	m_stDisplayInfo.wBPP	= wBpp;
	m_bScreenModeFlag		= _DXG_SCREENMODE_WINDOW;
	m_bDeviceModeFlag		= _DXG_DEVICEMODE_PRIMARY;

	SetRect(&m_rcWindow, 0, 0, 0, 0);
	ZeroMemory(&m_stBitsMaskInfo, sizeof(DXG_MASKINFO));

	m_pxDXGWnd = this;

	m_bIsWindowActive = FALSE;
	m_bIsWindowReady = FALSE;

	m_dwTextureTotal	= 0;
	m_dwTextureFree		= 0;
	m_dwVideoTotal		= 0;
	m_dwVideoFree		= 0;

	m_pxDefProcess = NULL;
}


VOID CWHDXGraphicWindow::UsedAndFreeMemoryCheck()
{
	DDSCAPS2     ddsCaps2; 

	ZeroMemory(&ddsCaps2, sizeof(ddsCaps2));

	ddsCaps2.dwCaps = DDSCAPS_TEXTURE;
	m_pDD->GetAvailableVidMem(&ddsCaps2, &m_dwTextureTotal, &m_dwTextureFree); 

	ZeroMemory(&ddsCaps2, sizeof(ddsCaps2));

	ddsCaps2.dwCaps = DDSCAPS_VIDEOMEMORY;
	m_pDD->GetAvailableVidMem(&ddsCaps2, &m_dwVideoTotal, &m_dwVideoFree); 
} 


/******************************************************************************************************************

	함수명 : CWHDXGraphicWindow::~CWHDXGraphicWindow()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
CWHDXGraphicWindow::~CWHDXGraphicWindow()
{
	FreeDXGEnumModeResources();
    DestroyDXGObjects();

	m_pxDefProcess = NULL;
	m_pxDXGWnd = NULL;
}


/******************************************************************************************************************

	함수명 : CWHDXGraphicWindow::Create()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : LPTSTR lpCaption
	         CHAR *pszMenuName
	         BYTE bScreenModeFlag
	         BYTE bDeviceModeFlag
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CWHDXGraphicWindow::Create(HINSTANCE hInst, LPTSTR lpCaption, CHAR *pszMenuName, CHAR* pszIconName, BYTE bScreenModeFlag, BYTE bDeviceModeFlag)
{
	DWORD	dwStyle;

	if ( !CWHWindow::Create(hInst, lpCaption, pszMenuName, pszIconName) )
	{
		MessageBox(m_hWnd, TEXT("[CWHDXGraphicWindow::Create]") TEXT("Window create failed."), "MirDXG", MB_ICONERROR | MB_OK);
		return FALSE;
	}

	m_bScreenModeFlag		= bScreenModeFlag;
	m_bDeviceModeFlag		= bDeviceModeFlag;

	if ( m_bScreenModeFlag & _DXG_SCREENMODE_WINDOW )
	{
		dwStyle  = GetWindowStyle(m_hWnd);
		dwStyle &= ~WS_POPUP;
		dwStyle |= WS_POPUP;//WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_SYSMENU;
		SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
	}
	else
	{
		dwStyle  = GetWindowStyle(m_hWnd);
		dwStyle &= ~dwStyle;
		dwStyle |= WS_POPUP;
		SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
	}

	HRESULT	hr;
	if ( hr = SUCCEEDED(DirectDrawEnumerateEx(DXGDriverEnumCallbackEx, NULL,
											  DDENUM_ATTACHEDSECONDARYDEVICES | DDENUM_DETACHEDSECONDARYDEVICES | DDENUM_NONDISPLAYDEVICES)) )
	{
		if ( hr = SUCCEEDED(CreateDXG()) )
		{
			m_bIsWindowReady = TRUE;
			CreateDefFont();
			return TRUE;
		}
		else
		{
			MessageBox(m_hWnd, TEXT("[CWHDXGraphicWindow::Create]") TEXT("DirectGraphic create failed."), "MirDXG", MB_ICONERROR | MB_OK);
			return FALSE;
		}
	}

	return FALSE;
}


/******************************************************************************************************************

	함수명 : CWHDXGraphicWindow::MainWndProcDXG()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : HWND hWnd
	         UINT uMsg
	         WPARAM wParam
	         LPARAM lParam
	출력   : LRESULT 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
LRESULT CWHDXGraphicWindow::MainWndProcDXG(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch (uMsg)
	{
		case WM_SYSKEYDOWN:
			return OnSysKeyDown(wParam, lParam);

		case WM_MOVE:
			return OnMove(wParam, lParam);

		case WM_SIZE:
			return OnSize(wParam, lParam);

		case WM_DESTROY:
			return OnDestroy();

        case WM_SETCURSOR:
			return OnSetCursor();   

 //      case WM_GETMINMAXINFO:			
//			return OnGetMinMaxInfo(wParam, lParam);

		default:
			break;
	}

    return CWHWindow::MainWndProc(hWnd, uMsg, wParam, lParam);
}


LRESULT CWHDXGraphicWindow::OnGetMinMaxInfo(WPARAM wParam, LPARAM lParam)
{
	if ( m_hWnd )
	{
		MINMAXINFO* pMinMax = (MINMAXINFO*) lParam;

		DWORD dwFrameWidth    = GetSystemMetrics( SM_CXSIZEFRAME );
		DWORD dwFrameHeight   = GetSystemMetrics( SM_CYSIZEFRAME );
		DWORD dwMenuHeight;
		if( GetMenu( m_hWnd ) )
			dwMenuHeight    = GetSystemMetrics( SM_CYMENU );
		else
			dwMenuHeight    = 0;					

		DWORD dwCaptionHeight = GetSystemMetrics( SM_CYCAPTION );

		pMinMax->ptMinTrackSize.x = m_stDisplayInfo.wWidth  + dwFrameWidth * 2;
		pMinMax->ptMinTrackSize.y = m_stDisplayInfo.wHeight + dwFrameHeight * 2 + 
									dwMenuHeight + dwCaptionHeight;

		pMinMax->ptMaxTrackSize.x = pMinMax->ptMinTrackSize.x;
		pMinMax->ptMaxTrackSize.y = pMinMax->ptMinTrackSize.y;
	}

    return 0L;
}


LRESULT CWHDXGraphicWindow::OnSetCursor()
{
    SetCursor(LoadCursor(NULL, IDC_ARROW));
    return 0L;
}



/******************************************************************************************************************

	함수명 : CWHDXGraphicWindow::OnSize()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : WPARAM wParam
	         LPARAM lParam
	출력   : LRESULT 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
LRESULT CWHDXGraphicWindow::OnSize(WPARAM wParam, LPARAM lParam)
{
    if ( SIZE_MAXHIDE == wParam || SIZE_MINIMIZED == wParam )
        m_bIsWindowActive = FALSE;
    else
        m_bIsWindowActive = TRUE;

	UpdateBoundsWnd();
 	return 0L;
}



/******************************************************************************************************************

	함수명 : CWHDXGraphicWindow::OnMove()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : WPARAM wParam
	         LPARAM lParam
	출력   : LRESULT 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
LRESULT CWHDXGraphicWindow::OnMove(WPARAM wParam, LPARAM lParam)
{
	UpdateBoundsWnd();
	return 0L;
}


/******************************************************************************************************************

	함수명 : CWHDXGraphicWindow::OnDestroy()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
LRESULT CWHDXGraphicWindow::OnDestroy()
{
	return CWHWindow::OnDestroy();
}


/******************************************************************************************************************

	함수명 : CWHDXGraphicWindow::OnSysKeyDown()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : WPARAM wParam
	         LPARAM lParam
	출력   : LRESULT 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
LRESULT CWHDXGraphicWindow::OnSysKeyDown(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == VK_RETURN )
	{
		m_bIsWindowReady = FALSE;

 		D3DTextr_InvalidateAllTextures();

		D3DWILTextr_InvalidateAllTextures();
		D3DWILTextr_DestroyAllTextures();

		if ( m_bScreenModeFlag & _DXG_SCREENMODE_WINDOW )
		{
			if ( FAILED(ResetDXG(_SCREEN_WIDTH, _SCREEN_HEIGHT, _SCREEN_BPP, _DXG_SCREENMODE_FULLSCREEN, _DXG_DEVICEMODE_PRIMARY|_DXG_DEVICEMODE_D3D)) )
			{
				MessageBox(m_hWnd, TEXT("DirectDraw RESETUP failed!!!") TEXT("The Program will now exit."), TEXT("MirDXG"), MB_ICONERROR | MB_OK);
			}
		}
		else
		{
			if ( FAILED(ResetDXG(_SCREEN_WIDTH, _SCREEN_HEIGHT, _SCREEN_BPP, _DXG_SCREENMODE_WINDOW, _DXG_DEVICEMODE_PRIMARY|_DXG_DEVICEMODE_D3D)) )
			{
				MessageBox(m_hWnd, TEXT("DirectDraw RESETUP failed!!!") TEXT("The Program will now exit."), TEXT("MirDXG"), MB_ICONERROR | MB_OK);
			}
		}

		D3DTextr_RestoreAllTextures(m_pd3dDevice);

		m_bIsWindowReady = TRUE;
	}

	return 0L;
}


/******************************************************************************************************************

	함수명 : CWHDXGraphicWindow::FindDriverAndDevice()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : DWORD 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BYTE CWHDXGraphicWindow::FindDriverAndDevice()
{
	BYTE bNum = 0;
	CHAR bCnt;
	 
	if ( m_bDeviceModeFlag & _DXG_DEVICEMODE_PRIMARY )
	{
		for ( bCnt = 0; bCnt < g_bNumDevices; bCnt++ )
		{
			if ( !g_stDXGEnumDeviceInfo[bCnt].pDriverGUID )
			{
				if ( g_stDXGEnumDeviceInfo[bCnt].f3DHardware )			return bCnt;
				else													bNum = bCnt;
			}
		}
	}
	else
	{
		for ( bCnt = g_bNumDevices-1; bCnt >= 0; bCnt-- )
		{
			// 비디오 카드가 2개이상
			if( g_bNumDevices > 2 )
			{
				if ( g_stDXGEnumDeviceInfo[0].dwNumModes != g_stDXGEnumDeviceInfo[bCnt].dwNumModes )
				{
					if ( g_stDXGEnumDeviceInfo[bCnt].f3DHardware )		return bCnt;
					else											
					{
						bNum = bCnt;
						break;
					}
				}
			}
			else
			{
				if ( g_stDXGEnumDeviceInfo[bCnt].f3DHardware )			return bCnt;
				else													bNum = bCnt;
			}
		}
	}

	return bNum;
}


/******************************************************************************************************************

	함수명 : CWHDXGraphicWindow::DestroyDXGObjects()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : HRESULT 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
HRESULT CWHDXGraphicWindow::DestroyDXGObjects()
{
	HRESULT	hr;
	LONG	lDDRefCnt  = 0L;
    LONG	lD3DRefCnt = 0L;

	if ( !(m_bScreenModeFlag & _DXG_SCREENMODE_WINDOW) )
	{
		if ( FAILED(hr = m_pDD->RestoreDisplayMode()) )
			return E_FAIL;
	}
    if ( m_pDD )
	{
		if ( FAILED(hr = m_pDD->SetCooperativeLevel(m_hWnd, DDSCL_NORMAL)) )
			return E_FAIL;
	}

    if ( m_pd3dDevice )
	{
        if ( 0 < ( lD3DRefCnt = m_pd3dDevice->Release() ) )
			return E_FAIL;
	}
    m_pd3dDevice = NULL;

    SAFE_RELEASE(m_pddsBackBuffer);
    SAFE_RELEASE(m_pddsFrontBuffer);
    SAFE_RELEASE(m_pddsZBuffer);
//	에디트윈도우 출력을 위해서 클리퍼를 세팅한다.
	SAFE_RELEASE(m_lpcClipper);
    SAFE_RELEASE(m_pD3D);

    if ( m_pDD )
    {
        if ( 0 < ( lDDRefCnt = m_pDD->Release() ) )
			INT nAAAAAA = 0;
//			return E_FAIL;
    }
    m_pDD = NULL;

	if ( m_hDefGameFont != NULL )
	{
		DeleteObject(m_hDefGameFont);
		m_hDefGameFont = NULL; 
	}

	return S_OK;
//    return ( lDDRefCnt==0 && lD3DRefCnt==0 ) ? S_OK : E_FAIL;
}


/******************************************************************************************************************

	함수명 : CWHDXGraphicWindow::FreeDXGEnumModeResources()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CWHDXGraphicWindow::FreeDXGEnumModeResources()
{	
    for ( BYTE bCnt = 0; bCnt < g_bNumDevices; bCnt++ )
    {
        SAFE_DELETE( g_stDXGEnumDeviceInfo[bCnt].pddsdModes );
    }
}


/******************************************************************************************************************

	함수명 : CWHDXGraphicWindow::CreateDXG()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : HRESULT 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
HRESULT CWHDXGraphicWindow::CreateDXG() 
{
    HRESULT hr;

	BYTE bSelecedDevice = FindDriverAndDevice();

	if ( SUCCEEDED(hr = DirectDrawCreateEx(g_stDXGEnumDeviceInfo[bSelecedDevice].pDriverGUID, (VOID**)&m_pDD, IID_IDirectDraw7, NULL)) ) 
	{
		if ( m_bScreenModeFlag & _DXG_SCREENMODE_WINDOW )
		{
			if ( FAILED(hr = m_pDD->SetCooperativeLevel(m_hWnd, DDSCL_NORMAL)) )
				return E_FAIL;
		}
		else
		{
			if ( FAILED(hr = m_pDD->SetCooperativeLevel(m_hWnd, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN | DDSCL_ALLOWREBOOT)) )
				return E_FAIL;

			if ( FAILED(hr = m_pDD->SetDisplayMode(m_stDisplayInfo.wWidth, m_stDisplayInfo.wHeight, m_stDisplayInfo.wBPP, 0, 0)) )
				return E_FAIL;		
		}

		if ( FAILED(hr == CreatePrimarySurface()) )
		{
			MessageBox(m_hWnd, TEXT("[CWHDXGraphicWindow::CreateDXG]") TEXT("Primary surface create failed."), TEXT("MirDXG"), MB_ICONERROR | MB_OK);
			return E_FAIL;
		}

		UpdateBoundsWnd();

		if ( m_bDeviceModeFlag & _DXG_DEVICEMODE_D3D )
		{
			if ( FAILED( hr = m_pDD->QueryInterface(IID_IDirect3D7, (VOID**)&m_pD3D)) )		
				return E_FAIL;

			if ( FAILED(hr = m_pD3D->CreateDevice(*g_stDXGEnumDeviceInfo[bSelecedDevice].pDeviceGUID, m_pddsBackBuffer, &m_pd3dDevice)) )
				return E_FAIL;

			D3DVIEWPORT7 d3dVP = { 0, 0, m_stDisplayInfo.wWidth, m_stDisplayInfo.wHeight, 0.0f, 1.0f };

			if ( FAILED( hr = m_pd3dDevice->SetViewport(&d3dVP)) )							return hr;

			if ( m_bDeviceModeFlag & _DXG_DEVICEMODE_ZBUFFER )
			{
				if ( FAILED(hr = CreateZBuffer(g_stDXGEnumDeviceInfo[bSelecedDevice].pDeviceGUID)) )
				{
					MessageBox(m_hWnd, TEXT("[CWHDXGraphicWindow::CreateDXG]") TEXT("ZBuffer create failed."), TEXT("MirDXG"), MB_ICONERROR | MB_OK);
					return E_FAIL;
				}
			}
		}

		m_stBitsMaskInfo = GetRGBMaskInfoIN16Bits(m_pddsBackBuffer);
		Init3DDeviceObjects();

		return S_OK;
	}

	return hr;
}


/******************************************************************************************************************

	함수명 : CDXGraphic::ResetDXG()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : WORD wWidth
	         WORD wHeight
	         WORD wBPP
	         BYTE bScreenModeFlag
	         BYTE bDeviceModeFlag
	출력   : HRESULT 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
HRESULT CWHDXGraphicWindow::ResetDXG(WORD wWidth, WORD wHeight, WORD wBPP, BYTE bScreenModeFlag, BYTE bDeviceModeFlag)
{
    HRESULT hr;
    DWORD	dwStyle;

	if ( m_bDeviceModeFlag == bDeviceModeFlag && m_bScreenModeFlag == bScreenModeFlag && 
		 m_stDisplayInfo.wWidth == wWidth && m_stDisplayInfo.wHeight == wHeight && m_stDisplayInfo.wBPP == wBPP )
		return E_FAIL;													    

    if ( FAILED(DestroyDXGObjects()) )
	{
		MessageBox(m_hWnd, TEXT("[CWHDXGraphicWindow::ResetDXG]") TEXT("DirectGraphic reset failed."), "MirDXG", MB_ICONERROR | MB_OK);
		return E_FAIL;
	}

	m_stDisplayInfo.wWidth	= wWidth;
	m_stDisplayInfo.wHeight	= wHeight;
	m_stDisplayInfo.wBPP	= wBPP;
	m_bScreenModeFlag		= bScreenModeFlag;
	m_bDeviceModeFlag		= bDeviceModeFlag;

	if ( m_bScreenModeFlag & _DXG_SCREENMODE_WINDOW )
	{
		dwStyle  = GetWindowStyle(m_hWnd);
		dwStyle &= ~WS_POPUP;
		dwStyle |= WS_OVERLAPPED | WS_CAPTION | WS_THICKFRAME | WS_MINIMIZEBOX | WS_SYSMENU;
		SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
	}
	else
	{
		dwStyle  = GetWindowStyle(m_hWnd);
		dwStyle &= ~dwStyle;
		dwStyle |= WS_POPUP;
		SetWindowLong(m_hWnd, GWL_STYLE, dwStyle);
	}

	if ( hr = FAILED(CreateDXG()) )
	{
		MessageBox(m_hWnd, TEXT("[CWHDXGraphicWindow::ResetDXG]") TEXT("DirectGraphic create failed."), "MirDXG", MB_ICONERROR | MB_OK);
		return E_FAIL;
	}

	CreateDefFont();

	return S_OK;
}


/******************************************************************************************************************

	함수명 : CWHDXGraphicWindow::CreatePrimarySurface()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : HRESULT 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
HRESULT CWHDXGraphicWindow::CreatePrimarySurface()
{		
    DDSURFACEDESC2	ddsd;
    HRESULT			hr;

	ZeroMemory(&ddsd, sizeof(ddsd));
	ddsd.dwSize = sizeof(ddsd);
	
	if ( m_bScreenModeFlag & _DXG_SCREENMODE_WINDOW )
	{
		RECT				rcWork;
		RECT				rcTemp;

	    GetWindowRect(m_hWnd, &rcTemp);
		
		// 클라이언트영역의 크기를 잡는부분은 윈도우의 생성과 관련이 있다.
		// 화면해상도 즉, 클라이언트 사각형 영역의 크기를 얻어온다.
		SetRect(&rcTemp, 0, 0, m_stDisplayInfo.wWidth, m_stDisplayInfo.wHeight);

		// 클라이언트 사각형 영역크기에서 윈도우 전체영역의 크기로 변환한다.
		AdjustWindowRectEx(&rcTemp, GetWindowStyle(m_hWnd), GetMenu(m_hWnd) != NULL, GetWindowExStyle(m_hWnd));

		// 윈도우 전체영역크기로 윈도우를 세팅한다. 단 시작좌표는 무시된다.
		SetWindowPos(m_hWnd, NULL, 0, 0, rcTemp.right-rcTemp.left, rcTemp.bottom-rcTemp.top,
					 SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);

		// HWND_NOTOPMOST를 세팅하기 위해서이다.
		SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE | SWP_NOACTIVATE);

		// 바탕화면영역의 크기를 얻어온다..
		SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWork, 0);
		GetWindowRect(m_hWnd, &rcTemp);
		if ( rcTemp.left < rcWork.left )		rcTemp.left = rcWork.left;
		if ( rcTemp.top  < rcWork.top )			rcTemp.top  = rcWork.top;

		// 윈도우의 시작좌표를 세팅한다.
		SetWindowPos(m_hWnd, NULL, rcTemp.left, rcTemp.top, 0, 0, SWP_NOSIZE | SWP_NOZORDER | SWP_NOACTIVATE);
    
		ddsd.dwFlags        = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

		if ( FAILED(hr = m_pDD->CreateSurface(&ddsd, &m_pddsFrontBuffer, NULL)) )			return E_FAIL;

		ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;

		if ( m_bDeviceModeFlag & _DXG_DEVICEMODE_D3D )
			ddsd.ddsCaps.dwCaps |= DDSCAPS_3DDEVICE;

		ddsd.dwWidth    = m_stDisplayInfo.wWidth;
		ddsd.dwHeight   = m_stDisplayInfo.wHeight;

		if ( FAILED(hr = m_pDD->CreateSurface(&ddsd, &m_pddsBackBuffer, NULL)) )			return E_FAIL;
	}
	else
	{
		ddsd.dwFlags           = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
		ddsd.ddsCaps.dwCaps    = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
		ddsd.dwBackBufferCount = 1;
		if ( m_bDeviceModeFlag & _DXG_DEVICEMODE_D3D )
			ddsd.ddsCaps.dwCaps |= DDSCAPS_3DDEVICE;

		if ( FAILED(hr = m_pDD->CreateSurface(&ddsd, &m_pddsFrontBuffer, NULL)) )			return E_FAIL;

		DDSCAPS2 ddscaps;
		ZeroMemory(&ddscaps, sizeof(ddscaps));
		ddscaps.dwCaps = DDSCAPS_BACKBUFFER;

		if ( FAILED(hr = m_pddsFrontBuffer->GetAttachedSurface(&ddscaps, &m_pddsBackBuffer)) )
			return E_FAIL;
	}

//	에디트윈도우 출력을 위해서 클리퍼를 세팅한다.////////////////////////////////////////////////////////////////
	if ( FAILED(hr = m_pDD->CreateClipper(0, &m_lpcClipper, NULL)) )						return E_FAIL;
	if ( FAILED(hr = m_lpcClipper->SetHWnd(0, m_hWnd)) )
	{
		m_lpcClipper->Release();
		return E_FAIL;
	}
	if ( FAILED(hr = m_pddsFrontBuffer->SetClipper(m_lpcClipper)) )
	{
		m_lpcClipper->Release();
		return E_FAIL;
	}
    m_pDD->FlipToGDISurface();
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////

	return S_OK;
}


/******************************************************************************************************************

	함수명 : CWHDXGraphicWindow::UpdateBoundsWnd()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CWHDXGraphicWindow::UpdateBoundsWnd()
{
	if ( m_bScreenModeFlag & _DXG_SCREENMODE_WINDOW )
	{
        GetClientRect(m_hWnd, &m_rcWindow);
        ClientToScreen(m_hWnd, (POINT*)&m_rcWindow);
        ClientToScreen(m_hWnd, (POINT*)&m_rcWindow+1);
    }
    else
    {
        SetRect(&m_rcWindow, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN));
    }
}


/******************************************************************************************************************

	함수명 : CWHDXGraphicWindow::CreateZBuffer()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : GUID* pDeviceGUID
	출력   : HRESULT 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
HRESULT CWHDXGraphicWindow::CreateZBuffer(GUID* pDeviceGUID)
{
    HRESULT hr;

    D3DDEVICEDESC7 ddDesc;
    m_pd3dDevice->GetCaps(&ddDesc);
    if (ddDesc.dpcTriCaps.dwRasterCaps & D3DPRASTERCAPS_ZBUFFERLESSHSR)
        return S_OK;

    DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
    ddsd.dwSize = sizeof(ddsd);
    m_pddsBackBuffer->GetSurfaceDesc(&ddsd);

    ddsd.dwFlags				= DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS | DDSD_PIXELFORMAT;
    ddsd.ddsCaps.dwCaps			= DDSCAPS_ZBUFFER;
    ddsd.ddpfPixelFormat.dwSize = 0;

    if ( FAILED(hr = m_pD3D->EnumZBufferFormats(*pDeviceGUID, DXGEnumZBufferFormatsCallback, (VOID*)&ddsd.ddpfPixelFormat)) )
	        return E_FAIL;

    if ( 0 == ddsd.ddpfPixelFormat.dwSize )
    {
        ddsd.ddpfPixelFormat.dwRGBBitCount = 16;
        if ( FAILED(hr = m_pD3D->EnumZBufferFormats(*pDeviceGUID, DXGEnumZBufferFormatsCallback,  (VOID*)&ddsd.ddpfPixelFormat)) )
	        return E_FAIL;
        if ( 0 == ddsd.ddpfPixelFormat.dwSize )
	        return E_FAIL;
    }

    if ( FAILED(hr = m_pDD->CreateSurface(&ddsd, &m_pddsZBuffer, NULL)) )		return E_FAIL;
    if ( FAILED(hr = m_pddsBackBuffer->AddAttachedSurface(m_pddsZBuffer)) )     return E_FAIL;

    if ( FAILED(hr = m_pd3dDevice->SetRenderTarget(m_pddsBackBuffer, 0L)) )
    {
        return E_FAIL;
    }

    return S_OK;
}


HRESULT CWHDXGraphicWindow::Init3DDeviceObjects()
{
    // Set the transform matrices
    D3DMATRIX matWorld, matView, matProj;

	// 현재 800*600화면에 맞게 3D 좌표계를 설정한다.
 	D3DVECTOR vEyePt    = D3DVECTOR(0, 0, -300.0f);
    D3DVECTOR vLookatPt = D3DVECTOR(0, 0, 0);
    D3DVECTOR vUpVec    = D3DVECTOR(0, 1, 0);
    D3DUtil_SetIdentityMatrix(matWorld);
    D3DUtil_SetViewMatrix(matView, vEyePt, vLookatPt, vUpVec);
    D3DUtil_SetProjectionMatrix(matProj, g_PI/2, float(600.0f/800.0f), -1.0f, 1.0f);

	// MATRIAL의 초기화를 설정한다.
    D3DMATERIAL7 mtrl;
	ZeroMemory(&mtrl, sizeof(mtrl));
	mtrl.diffuse.r = mtrl.diffuse.g = mtrl.diffuse.b = 0.1f;
	mtrl.ambient.r = mtrl.ambient.g = mtrl.ambient.b = 1.0f;
    m_pd3dDevice->SetMaterial(&mtrl);

	if ( !m_pd3dDevice )
		return E_FAIL;

    m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,      &matWorld);
    m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_VIEW,       &matView);
    m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &matProj);

	// RenderState의 초기값 설정.
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_AMBIENT, 0xFFFFFFFF);
    m_pd3dDevice->SetRenderState(D3DRENDERSTATE_DITHERENABLE, FALSE); 
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE); 	
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_FALSE);
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID);
//	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME);
//	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
	
    // TextureStageState의 초기값 설정.
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE); 
    m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
    m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_POINT); 
    m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_POINT);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTFP_NONE);

    return S_OK;
}


/******************************************************************************************************************

	함수명 : CWHDXGraphicWindow::CreateGameFont()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : LPCSTR szFontName
	         INT nXsize
	         INT nYsize
	         BYTE bFontType
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
HFONT CWHDXGraphicWindow::CreateGameFont(LPCSTR szFontName, INT nHeight, INT nWidth, INT nWeight, BOOL bItalic, BOOL bULine, BOOL bStrikeOut, DWORD dwCharSet)
{
	INT nYsize;

	// 폰트크기 맞추기.
	if ( m_pddsBackBuffer )
	{
		HDC hDC;
		m_pddsBackBuffer->GetDC(&hDC);
		nYsize = -MulDiv(nHeight, GetDeviceCaps(hDC, LOGPIXELSY), 72); 
		m_pddsBackBuffer->ReleaseDC(hDC);
	}

	return CreateFont(nYsize, nWidth, 0, 0, nWeight, bItalic, bULine, bStrikeOut, dwCharSet, 
					  OUT_DEFAULT_PRECIS,	CLIP_DEFAULT_PRECIS, 
					  DEFAULT_QUALITY, DEFAULT_PITCH | FF_ROMAN, szFontName);

}
VOID CWHDXGraphicWindow::CreateDefFont()
{
	// 이미 기본폰트가 생성되어 있으면 새로 만들수 있도록 Clear.
	if ( m_hDefGameFont != NULL )
	{
		DeleteObject(m_hDefGameFont); 
		m_hDefGameFont = NULL;
	}
//is font name
	m_hDefGameFont	= CreateGameFont("芥竟", 9);
}



VOID CWHDXGraphicWindow::StringPlus(CHAR* szResult, CHAR* szSrc, ...)
{
	INT nCnt = 0;

	va_list vaMarker;
	CHAR*	pszString;


//	ZeroMemory(szResult, strlen(szResult));
	va_start(vaMarker, szSrc);
	pszString = szSrc;
	
	while ( TRUE )
	{		
		strcpy(&szResult[strlen(szResult)], pszString);
		if ( !strcmp(pszString,  "") )		break;
		pszString = (CHAR*)va_arg(vaMarker, CHAR*);
	}

	va_end(vaMarker);
}


CHAR* CWHDXGraphicWindow::IntToStr(INT nNum)
{
	static CHAR szResult[MAX_PATH];
	ZeroMemory(szResult, MAX_PATH);
	_itoa(nNum, szResult, 10);
	return szResult;
}




/******************************************************************************************************************

	함수명 : CWHDXGraphicWindow::PutsHan()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : LPDIRECTDRAWSURFACE7 pSurface
	         INT nX
	         INT nY
	         COLORREF foreColor
	         COLORREF backColor
	         CHAR* szFormat
	         ...
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CWHDXGraphicWindow::PutsHan(LPDIRECTDRAWSURFACE7 pSurface, INT nX, INT nY, COLORREF foreColor, COLORREF backColor, CHAR* szText, HFONT hFont)
{
	HDC		hDC;
	HFONT	hOldFont;

    if (NULL == pSurface)
		pSurface = m_pddsBackBuffer;
			
    if ( NULL == pSurface )
		return;

	if ( NULL == hFont )
		hFont = m_hDefGameFont;

	HRESULT hr = pSurface->GetDC(&hDC);

	if (backColor == RGB( 0, 0, 0 ))		SetBkMode(hDC, TRANSPARENT);	
	else									SetBkColor(hDC, backColor);

	hOldFont = (HFONT)SelectObject(hDC, hFont);

	SetTextColor(hDC, foreColor);

	TextOut(hDC, nX, nY, szText, strlen(szText));

	SelectObject(hDC, hOldFont);

	if ( hFont != m_hDefGameFont )
		DeleteObject(hFont);
	
	pSurface->ReleaseDC(hDC);
}


/******************************************************************************************************************

	함수명 : CWHDXGraphicWindow::PutsHan()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : LPDIRECTDRAWSURFACE7 pSurface
	         INT nX
	         INT nY
	         COLORREF foreColor
	         COLORREF backColor
	         CHAR* szFormat
	         ...
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CWHDXGraphicWindow::PutsHan(LPDIRECTDRAWSURFACE7 pSurface, RECT rc, COLORREF foreColor, COLORREF backColor, CHAR* szText, HFONT hFont)
{
	HDC		hDC;
	HFONT	hOldFont;

    if (NULL == pSurface)
		pSurface = m_pddsBackBuffer;
			
    if ( NULL == pSurface )
		return;

	if ( NULL == hFont )
		hFont = m_hDefGameFont;

	HRESULT hr = pSurface->GetDC(&hDC);

	if (backColor == RGB( 0, 0, 0 ))		SetBkMode(hDC, TRANSPARENT);	
	else									SetBkColor(hDC, backColor);

	hOldFont = (HFONT)SelectObject(hDC, hFont);

	SetTextColor(hDC, foreColor);

	DrawText(hDC, szText, -1, &rc, DT_CENTER|DT_SINGLELINE);

	SelectObject(hDC, hOldFont);

	if ( hFont != m_hDefGameFont )
		DeleteObject(hFont);
	
	pSurface->ReleaseDC(hDC);
}


SIZE CWHDXGraphicWindow::GetStrLength(LPDIRECTDRAWSURFACE7 pSurface, HFONT hFont, CHAR* szFormat, ...)
{
	HDC		hDC;
	va_list arg;
    CHAR	szBuf[1024];
	SIZE	sSize;
	
	sSize.cx = 0;
	sSize.cy = 0;

	HFONT	hOldFont;

	if ( NULL == pSurface )
		pSurface = m_pddsBackBuffer;
			
    if ( NULL == pSurface )					return sSize;

	if ( NULL == hFont )
		hFont = m_hDefGameFont;

	HRESULT hr = pSurface->GetDC(&hDC);

	va_start(arg, szFormat);
    vsprintf(szBuf, szFormat, arg);
    va_end(arg);

	hOldFont = (HFONT)SelectObject(hDC, hFont);

	GetTextExtentPoint32(hDC,szBuf,strlen(szBuf),&sSize);

	SelectObject(hDC, hOldFont);
	
	pSurface->ReleaseDC(hDC);

	return sSize;
}


/*BOOL CWHDXGraphicWindow::StringDivideLen(INT nDivideLen, INT& nDividedLine, CHAR* szSrc,CDLList<CHAR*>* m_pxpStr)
{
	CHAR szTmpCheck[MAX_PATH];
	CHAR szResult[MAX_PATH];
	
	nDividedLine = 1;

	INT nStartLen	= 0;
	INT nEndLen		= 0;
	INT nResultLen	= 0;
	INT nWordCheck	= 0;

	if ( szSrc[0] != NULL )
	{
		ZeroMemory(szResult,MAX_PATH);

		for ( INT nCnt = 0; nCnt < (INT)strlen(szSrc); nCnt++)
		{
			ZeroMemory(szTmpCheck, MAX_PATH);
			nEndLen = nCnt+1;

			// 현재부분까지의 문자열을 읽는다.
			memcpy(&szTmpCheck, &szSrc[nStartLen], nEndLen-nStartLen);
			INT nsLen;
			nsLen = strlen(szTmpCheck);

			// 현재까지 읽어온 부분이 허용넓이를 넘어섰을때.
			if (nsLen > nDivideLen )
			{
				// 현재 2바이트 문자라면. 
				if ( szSrc[nEndLen-1] < 0 )
				{
					// 현재 앞에문자가 2바이트 문자가 아니라면
					if ( !(nWordCheck%2) )
					{
						nStartLen += strlen(szTmpCheck)-1;
						nCnt--;
						CHAR* pszNewLine;
						pszNewLine = new CHAR[nResultLen+1];
						memcpy(pszNewLine,szResult,nResultLen);
						pszNewLine[nResultLen]=NULL;
						m_pxpStr->AddNode(pszNewLine);
						nResultLen = 0;
						nDividedLine++;
					}
					else
					{
						nStartLen += strlen(szTmpCheck)-2;
						nCnt -= 2;
						CHAR* pszNewLine;
						pszNewLine = new CHAR[nResultLen];
						memcpy(pszNewLine,szResult,nResultLen-1);
						pszNewLine[nResultLen-1]=NULL;
						m_pxpStr->AddNode(pszNewLine);
						nResultLen = 0;
						nDividedLine++;
						nWordCheck--;
					}
				}
				// 1바이트 문자. 
				// 현재보다 한바이트앞부분까지만 고려해주면 된다.
				else
				{
					nStartLen += strlen(szTmpCheck)-1;
					nCnt--;
					CHAR* pszNewLine;
					pszNewLine = new CHAR[nResultLen+1];
					memcpy(pszNewLine,szResult,nResultLen);
					pszNewLine[nResultLen]=NULL;
					m_pxpStr->AddNode(pszNewLine);
					nResultLen=0;
					nDividedLine++;
				}
			}
			else if(szSrc[nEndLen-1]=='\n')	// 강제개행     \n으로하면  Error ????
			{
				nStartLen += strlen(szTmpCheck)-1;
				CHAR* pszNewLine;
				pszNewLine = new CHAR[nResultLen+1];
				memcpy(pszNewLine,szResult,nResultLen);
				pszNewLine[nResultLen]=NULL;
				m_pxpStr->AddNode(pszNewLine);
				nResultLen=0;
				nDividedLine++;
			}
			else
			{
				if ( szSrc[nEndLen-1] < 0 )
					nWordCheck++;

				szResult[nResultLen] = szSrc[nEndLen-1];
				nResultLen++;
			}
		}
		if(nResultLen!=0)
		{
			CHAR* pszNewLine;
			pszNewLine = new CHAR[nResultLen+1];
			memcpy(pszNewLine,szResult,nResultLen);
			pszNewLine[nResultLen]=NULL;
			m_pxpStr->AddNode(pszNewLine);
			nDividedLine++;
			nResultLen=0;
		}
		return TRUE;
	}
	return FALSE;
}
*/


BOOL CWHDXGraphicWindow::StringDivide(INT nDivideWidth, INT& nDividedLine, CHAR* szSrc, CHAR* szResult)
{
	CHAR szTmpCheck[MAX_PATH];
	
	nDividedLine = 1;

	INT nStartLen	= 0;
	INT nEndLen		= 0;
	INT nResultLen	= 0;

	INT nWordCheck	= 0;

	if ( szSrc[0] != NULL )
	{
		ZeroMemory(szResult, MAX_PATH);

		for ( INT nCnt = 0; nCnt < (INT)strlen(szSrc); nCnt++)
		{
			ZeroMemory(szTmpCheck, MAX_PATH);

			nEndLen = nCnt+1;

			// 만약에 글자크기가 최대 글자크기를 넘어가면 최대치로 맞춰준다.
			if ( nEndLen >= MAX_PATH )
			{
				szResult[MAX_PATH-1] = NULL;
				break;
			}

			// 현재부분까지의 문자열을 읽는다.
			memcpy(&szTmpCheck, &szSrc[nStartLen], nEndLen-nStartLen);

			SIZE sizeLen;
			sizeLen = GetStrLength(NULL, NULL, szTmpCheck);

			// 현재까지 읽어온 부분이 허용넓이를 넘어섰을때.
			if ( sizeLen.cx > nDivideWidth )
			{
				// 현재 2바이트 문자라면. 
				if ( szSrc[nEndLen-1] < 0 )
				{
					// 현재 앞에문자가 2바이트 문자가 아니라면
					if ( !(nWordCheck%2) )
					{
						nStartLen += strlen(szTmpCheck)-1;
						nCnt--;
						szResult[nResultLen] = '`';
						nResultLen++;
						nDividedLine++;
					}
					else
					{
						nStartLen += strlen(szTmpCheck)-2;
						nCnt -= 2;
						szResult[nResultLen-1] = '`';
						nDividedLine++;
						nWordCheck--;
					}
				}
				// 1바이트 문자. 
				// 현재보다 한바이트앞부분까지만 고려해주면 된다.
				else
				{
					nStartLen += strlen(szTmpCheck)-1;
					nCnt--;
					szResult[nResultLen] = '`';
					nResultLen++;
					nDividedLine++;
				}
			}
			else
			{
				if ( szSrc[nEndLen-1] < 0 )
					nWordCheck++;

				szResult[nResultLen] = szSrc[nEndLen-1];
				nResultLen++;
			}
		}
		return TRUE;
	}
	return FALSE;

}


/******************************************************************************************************************

	함수명 : CWHDXGraphicWindow::GetRGBMaskInfoIN16Bits()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : LPDIRECTDRAWSURFACE7 pSurface
	출력   : DXG_MASKINFO 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
DXG_MASKINFO CWHDXGraphicWindow::GetRGBMaskInfoIN16Bits(LPDIRECTDRAWSURFACE7 pSurface)
{	
	DDPIXELFORMAT ddpf;
	DXG_MASKINFO stRGBMaskInfo;

	ZeroMemory(&stRGBMaskInfo, sizeof(DXG_MASKINFO));

	if ( pSurface == NULL )		pSurface = m_pddsBackBuffer;

    if ( pSurface )
	{
		memset(&ddpf, 0, sizeof(ddpf));
		ddpf.dwSize = sizeof(ddpf);
		ddpf.dwFlags = DDPF_RGB;
		pSurface->GetPixelFormat(&ddpf);

		stRGBMaskInfo.dwRMask = ddpf.dwRBitMask;
		stRGBMaskInfo.dwGMask = ddpf.dwGBitMask;
		stRGBMaskInfo.dwBMask = ddpf.dwBBitMask;

		DWORD dwCnt;
		DWORD dwMask;
		for ( dwCnt = 0, dwMask = ddpf.dwRBitMask; !( dwMask & 1 ); dwCnt++, dwMask >>= 1 );
		stRGBMaskInfo.bRShift	= (BYTE) dwCnt;
		for ( dwCnt = 0; ( ( dwMask>>dwCnt ) & 1 ); dwCnt++ );
		stRGBMaskInfo.bRCnt		= (BYTE) dwCnt;

		for ( dwCnt = 0, dwMask = ddpf.dwGBitMask; !( dwMask & 1 ); dwCnt++, dwMask >>= 1 );
		stRGBMaskInfo.bGShift	= (BYTE) dwCnt;
		for ( dwCnt = 0; ( ( dwMask>>dwCnt ) & 1 ); dwCnt++ );
		stRGBMaskInfo.bGCnt		= (BYTE) dwCnt;

		for ( dwCnt = 0, dwMask = ddpf.dwBBitMask; !( dwMask & 1 ); dwCnt++, dwMask >>= 1 );
		stRGBMaskInfo.bBShift	= (BYTE) dwCnt;
		for ( dwCnt = 0; ( ( dwMask>>dwCnt ) & 1 ); dwCnt++ );
		stRGBMaskInfo.bBCnt		= (BYTE) dwCnt;
	}

	return stRGBMaskInfo;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 그리는 함수들 모음.

WORD CWHDXGraphicWindow::ConvertColor24To16(COLORREF dwColor)
{
	WORD wRet;

	INT nRed  = RGB_GETRED(dwColor);
	INT nGreen = RGB_GETGREEN(dwColor);
	INT nBlue   = RGB_GETBLUE(dwColor);
	
	nRed = (nRed*(1<<m_stBitsMaskInfo.bRCnt))/(1<<8);
	nGreen = (nGreen*(1<<m_stBitsMaskInfo.bGCnt))/(1<<8);
	nBlue = (nBlue*(1<<m_stBitsMaskInfo.bBCnt))/(1<<8);

	wRet = (nRed << m_stBitsMaskInfo.bRShift) | (nGreen << m_stBitsMaskInfo.bGShift) | (nBlue << m_stBitsMaskInfo.bBShift); 
	return wRet;
}


/******************************************************************************************************************

	함수명 : CWHDXGraphicWindow::DrawWithGDI()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : RECT rc
	         LPDIRECTDRAWSURFACE7 pSurface
	         DWORD dwColor
	         DWORD dwPenStyleFlg
	         BYTE bKind
	출력   : HRESULT 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
HRESULT CWHDXGraphicWindow::DrawWithGDI(RECT rc, LPDIRECTDRAWSURFACE7 pSurface, DWORD dwColor, BYTE bKind)
{
	POINT	pt;
	HDC		hDC;
	HPEN	hPen, hOldPen;
	HBRUSH	hBrush, hOldBrush;
	HRESULT hr;

	if ( pSurface == NULL )
	{
		pSurface = m_pddsBackBuffer;
	}

	if ( pSurface == NULL )
		return E_FAIL;

	if ( FAILED(hr = pSurface->GetDC(&hDC)) )
	{
		return E_FAIL;
	}

	switch ( bKind )
	{
	case 0:
		{
			hPen = CreatePen(PS_SOLID, NULL, dwColor);
		
			hOldPen = (HPEN)SelectObject(hDC, hPen);

			MoveToEx(hDC, rc.left, rc.top, &pt);
			LineTo(hDC, rc.right, rc.bottom);

			SelectObject(hDC, hOldPen);
			DeleteObject(hPen);
		}
		break;
	case 1:
		{
			hBrush = CreateSolidBrush(dwColor);

			hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

			FrameRect(hDC, &rc, hBrush);

			SelectObject(hDC, hOldBrush);
			DeleteObject(hBrush);
		}
		break;
	case 2:
		{
			hBrush = CreateSolidBrush(dwColor);

			hOldBrush = (HBRUSH)SelectObject(hDC, hBrush);

			FillRect(hDC, &rc, hBrush);

			SelectObject(hDC, hOldBrush);
			DeleteObject(hBrush);
		}
		break;
	default:
		break;
	}

	pSurface->ReleaseDC(hDC);

	return S_OK;
}


/******************************************************************************************************************

	함수명 : CWHDXGraphicWindow::DrawSprite()

	작성자 : 
	작성일 : 

	목적   : 일반적인 그리기. 한픽셀씩 그린다. 스크린영역 클리핑, 소스영역 클리핑
	입력   : INT nX
	         INT nY
	         INT nXSize
	         INT nYSize
	         WORD* pwSrc
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CWHDXGraphicWindow::DrawWithImage(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc)
{
	RECT			rc;
	DDSURFACEDESC2	ddsd;

	INT	nWidth		= nXSize;
	INT	nHeight		= nYSize;
	INT	nXOffset	= 0;
	INT	nYOffset	= 0;
	INT	nStartX		= 0;
	INT	nStartY		= 0;
	INT	nEndX		= m_stDisplayInfo.wWidth -1;
	INT	nEndY		= m_stDisplayInfo.wHeight-1;

	if ( m_pddsBackBuffer != NULL )
	{
		if (nX < nStartX )
		{ 
			nXOffset = nStartX - nX;
			nWidth	 = nXSize - nXOffset;
		}
		if ( (nX+nXSize-1) > nEndX )
			nWidth	 = nEndX - nX - nXOffset + 1;
		
		if ( nY < nStartY )
		{ 
			nYOffset	= nStartY - nY;
			nHeight		= nYSize - nYOffset;
		}
		if ( (nY+nYSize-1) > nEndY )
			nHeight	= nEndY - nY - nYOffset + 1;

		if ( (nWidth > 0) && (nHeight > 0) )
		{
			rc.left		= nXOffset;
			rc.right	= nXOffset + nWidth;
			rc.top		= nYOffset;
			rc.bottom	= nYOffset + nHeight;

			ddsd.dwSize	= sizeof(DDSURFACEDESC2);
			ddsd.lpSurface = NULL;
			
			m_pddsBackBuffer->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
			if ( !ddsd.lpSurface )		return FALSE;
			WORD* pwdDst;
			pwdDst = (WORD*)ddsd.lpSurface;

			for ( INT y=rc.top ; y < rc.bottom ; y++ )
			{
				for ( INT x=rc.left ; x < rc.right ; x++ )
				{
					if ( pwSrc[y * nXSize + x] != 0 )
					{
						pwdDst[((y+nY) * (ddsd.lPitch >> 1)) + (nX+x)] = pwSrc[y * nXSize + x];
					}
				}
			}

			m_pddsBackBuffer->Unlock(NULL);
		}
		return TRUE;
	}
	return FALSE;
}


BOOL CWHDXGraphicWindow::DrawWithImagePerLineClipRgn(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, WORD wClipWidth, WORD wClipHeight)
{
	RECT			rc;
	DDSURFACEDESC2	ddsd;

	INT	nWidth		= nXSize;
	INT	nHeight		= nYSize;
	INT	nXOffset	= 0;
	INT	nYOffset	= 0;
	INT	nStartX		= 0;
	INT	nStartY		= 0;
	INT	nEndX		= wClipWidth  - 1;
	INT	nEndY		= wClipHeight - 1;

	if ( m_pddsBackBuffer != NULL )
	{
		if (nX < nStartX )
		{ 
			nXOffset = nStartX - nX;
			nWidth	 = nXSize - nXOffset;
		}
		if ( (nX+nXSize-1) > nEndX )
			nWidth	 = nEndX - nX - nXOffset + 1;		
		if ( nY < nStartY )
		{ 
			nYOffset	= nStartY - nY;
			nHeight		= nYSize - nYOffset;
		}
		if ( (nY+nYSize-1) > nEndY )
			nHeight	= nEndY - nY - nYOffset + 1;

		if ( (nWidth > 0) && (nHeight > 0) )
		{
			rc.left		= nXOffset;
			rc.right	= nXOffset + nWidth;
			rc.top		= nYOffset;
			rc.bottom	= nYOffset + nHeight;

			ddsd.dwSize	= sizeof(DDSURFACEDESC2);
			ddsd.lpSurface = NULL;
			
			m_pddsBackBuffer->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
			if ( !ddsd.lpSurface )			return FALSE;
			WORD* pwdDst;
			pwdDst = (WORD*)ddsd.lpSurface;

			for ( INT y=rc.top ; y < rc.bottom ; y++ )
			{
				memcpy(&pwdDst[((y+nY) * (ddsd.lPitch >> 1)) + (nX + rc.left)], &pwSrc[y * nXSize + rc.left], (rc.right-rc.left)*2);
			}

			m_pddsBackBuffer->Unlock(NULL);
		}
		return TRUE;
	}
	return FALSE;
}



/******************************************************************************************************************

	함수명 : CWHDXGraphicWindow::DrawWithSurface()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : LPDIRECTDRAWSURFACE7 pddsSrc
	         RECT* prcSrc
	         RECT* prcDst
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CWHDXGraphicWindow::DrawWithSurface(INT nX, INT nY, INT nXSize, INT nYSize, LPDIRECTDRAWSURFACE7 pddsSrc)
{
	RECT	rc;
	RECT	rcDst;
	INT		nXOffset	= 0;
	INT		nYOffset	= 0;
	INT		nWidth		= nXSize;
	INT		nHeight		= nYSize;
	INT		nStartX		= 0;
	INT		nStartY		= 0;

	INT		nEndX		= m_stDisplayInfo.wWidth -1;
	INT		nEndY		= m_stDisplayInfo.wHeight-1;;


	if ( m_pddsBackBuffer != NULL )
	{
		if (nX < nStartX )
		{ 
			nXOffset	= nStartX - nX;
			nWidth		= nXSize - nXOffset;
		}
		if ( (nX+nXSize-1) > nEndX )
			nWidth		= nEndX - nX - nXOffset + 1;		
		if ( nY < nStartY )
		{ 
			nYOffset	= nStartY - nY;
			nHeight		= nYSize - nYOffset;
		}		
		if ( (nY+nYSize-1) > nEndY )
			nHeight	= nEndY - nY - nYOffset + 1;

		if ( (nWidth > 0) && (nHeight > 0) )
		{
			rc.left		 = nXOffset;
			rc.right	 = nXOffset+nWidth;
			rc.top		 = nYOffset;
			rc.bottom	 = nYOffset+nHeight;

			rcDst.left	 = rc.left+nX;
			rcDst.top	 = rc.top+nY;
			rcDst.right  = rc.right+nX;
			rcDst.bottom = rc.bottom+nY;

			m_pddsBackBuffer->Blt(&rcDst, pddsSrc, &rc, DDBLT_WAIT|DDBLT_KEYSRC, NULL);
		}
		return TRUE;
	}
	return FALSE;
}


BOOL CWHDXGraphicWindow::DrawWithImageForCompMemToMem(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, INT nDstXSize, INT nDstYSize, WORD* pwDst, WORD wChooseColor1, WORD wChooseColor2)
{
	RECT			rc;

	INT	nWidth		= nXSize;
	INT	nHeight		= nYSize;
	INT	nXOffset	= 0;
	INT	nYOffset	= 0;
	INT	nStartX		= 0;
	INT	nStartY		= 0;
	INT	nEndX		= nDstXSize - 1;
	INT	nEndY		= nDstYSize - 1;

	if ( pwDst != NULL )
	{
		if (nX < nStartX )
		{ 
			nXOffset	= nStartX - nX;
			nWidth		= nXSize - nXOffset;
		}
		if ( (nX+nXSize-1) > nEndX )
			nWidth		= nEndX - nX - nXOffset + 1;		
		if ( nY < nStartY )		{ 
			nYOffset	= nStartY - nY;
			nHeight		= nYSize - nYOffset;
		}		
		if ( (nY+nYSize-1) > nEndY )
			nHeight	= nEndY - nY - nYOffset + 1;

		if ( (nWidth > 0) && (nHeight > 0) )
		{
			rc.left		= nXOffset;
			rc.right	= nXOffset+nWidth;
			rc.top		= nYOffset;
			rc.bottom	= nYOffset+nHeight;

			INT nWidthStart	= 0;
			INT nWidthEnd	= 0;
			INT nCurrWidth  = 0;
			INT nCntCopyWord = 0;
			INT nYCnt =0;
			INT nLastWidth = 0;

			FLOAT rBlueRate, rGreenRate, bRedRate;
			BYTE  bRedSrc, bGreenSrc, bBlueSrc;
			BYTE  bRedWantedColor, bGreenWantedColor, bBlueWantedColor;
			WORD  wPixel;

			// y축 클리핑.
			for ( nYCnt=0 ; nYCnt < rc.top ; nYCnt++ )
			{
				nWidthEnd += pwSrc[nWidthStart];
				nWidthStart++;
				nWidthEnd++;
				nWidthStart	= nWidthEnd;
			}

			// y축에 대해서 실제로 루핑시킬 Count만을 정한다.
			for ( nYCnt=rc.top ; nYCnt < rc.bottom ; nYCnt++ )
			{
				// 한라인의 길이를 얻어온다.(단위는 워드)
				nWidthEnd += pwSrc[nWidthStart];
				nWidthStart++;

				// 라인의 길이만큼 화면에 뿌려준다.
				for ( INT x = nWidthStart; x < nWidthEnd ; )
				{
					if ( pwSrc[x] == 0xC0 )
					{
						x++;
						nCntCopyWord = pwSrc[x];
						x++;
						nCurrWidth += nCntCopyWord;
					}
					else if ( pwSrc[x] == 0xC1 )
					{
						x++;
						nCntCopyWord = pwSrc[x];
						x++;

						nLastWidth = nCurrWidth;
						nCurrWidth += nCntCopyWord;

						if ( rc.left > nCurrWidth || rc.right < nLastWidth )
						{
							x += nCntCopyWord;
						}
						else
						{
							// rc.left점을 기준으로 카피할 영역이 걸린 경우.
							if ( nLastWidth < rc.left && rc.left <= nCurrWidth )
							{
								x += (rc.left-nLastWidth);
								memcpy(&pwDst[((nYCnt+nY) * nDstXSize) + (rc.left+nX)], &pwSrc[x], sizeof(WORD)*(nCurrWidth-rc.left));
								x += (nCurrWidth-rc.left);
							}
							// rc.right점을 기준으로 카피할 영역이 걸린 경우.
							else if ( nLastWidth <= rc.right && rc.right < nCurrWidth )
							{
								memcpy(&pwDst[((nYCnt+nY) * nDstXSize) + (nLastWidth+nX)], &pwSrc[x], sizeof(WORD)*(rc.right-nLastWidth));
								x += nCntCopyWord;
							}
							else
							{
								memcpy(&pwDst[((nYCnt+nY) * nDstXSize) + (nLastWidth+nX)], &pwSrc[x], sizeof(WORD)*nCntCopyWord);
								x += nCntCopyWord;
							}
						}
					}
					else if ( pwSrc[x] == 0xC2 || pwSrc[x] == 0xC3 )
					{
						WORD wDyingKind, wChooseColor;
						wDyingKind = pwSrc[x];
						switch ( wDyingKind )
						{
						case 0xC2: 
							wChooseColor = wChooseColor1;
							break;
						case 0xC3: 
							wChooseColor = wChooseColor2;
							break;
						}
						x++;
						nCntCopyWord = pwSrc[x];
						x++;

						nLastWidth = nCurrWidth;
						nCurrWidth += nCntCopyWord;

						if ( rc.left > nCurrWidth || rc.right < nLastWidth )
						{
							x += nCntCopyWord;
						}
						else
						{
							// rc.left점을 기준으로 카피할 영역이 걸린 경우.
							if ( nLastWidth < rc.left && rc.left <= nCurrWidth )
							{
								x += (rc.left-nLastWidth);
								for ( INT nCheck = 0; nCheck < nCurrWidth-rc.left; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									pwDst[((nYCnt+nY) * nDstXSize) + (nLastWidth+nX+nCheck)] = ((bRedWantedColor  <<m_stBitsMaskInfo.bRShift) |
																							    (bGreenWantedColor<<m_stBitsMaskInfo.bGShift) |
																								(bBlueWantedColor <<m_stBitsMaskInfo.bBShift));			
								}
								x += (nCurrWidth-rc.left);
							}
							// rc.right점을 기준으로 카피할 영역이 걸린 경우.
							else if ( nLastWidth <= rc.right && rc.right < nCurrWidth )
							{
								for ( INT nCheck = 0; nCheck < rc.right-nLastWidth; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									pwDst[((nYCnt+nY) * nDstXSize) + (nLastWidth+nX+nCheck)] = ((bRedWantedColor  <<m_stBitsMaskInfo.bRShift) |
																								(bGreenWantedColor<<m_stBitsMaskInfo.bGShift) |
																								(bBlueWantedColor <<m_stBitsMaskInfo.bBShift));			
								}
								x += nCntCopyWord;
							}
							else
							{
								for ( INT nCheck = 0; nCheck < nCntCopyWord; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									pwDst[((nYCnt+nY) * nDstXSize) + (nLastWidth+nX+nCheck)] = ((bRedWantedColor  <<m_stBitsMaskInfo.bRShift) |
																								(bGreenWantedColor<<m_stBitsMaskInfo.bGShift) |
																								(bBlueWantedColor <<m_stBitsMaskInfo.bBShift));			
								}
								x += nCntCopyWord;
							}
						}
					}	
				}
				// 라인의 끝을 다음 라인의 시작으로 옮겨준다.
				nWidthEnd++;

				nWidthStart	= nWidthEnd;
				nCurrWidth = 0;
			}
		}

		m_pddsBackBuffer->Unlock(NULL);
		return TRUE;
	}
	return FALSE;
}


BOOL CWHDXGraphicWindow::DrawWithImageForComp(INT nX, INT nY, RECT rcSrc, WORD* pwSrc, WORD wChooseColor1, WORD wChooseColor2)
{
	RECT			rc;
	DDSURFACEDESC2	ddsd;

	INT	nWidth		= rcSrc.right-rcSrc.left;
	INT	nHeight		= rcSrc.bottom-rcSrc.top;
	INT	nXOffset	= rcSrc.left;
	INT	nYOffset	= rcSrc.top;
	INT	nStartX		= 0;
	INT	nStartY		= 0;
	INT	nEndX		= m_stDisplayInfo.wWidth -1;
	INT	nEndY		= m_stDisplayInfo.wHeight-1;

	if ( m_pddsBackBuffer != NULL )
	{
		if (nX < nStartX )
		{ 
			nXOffset += nStartX - nX;
			nWidth	 = rcSrc.right - nXOffset;
		}
		if ( (nX+rcSrc.right-rcSrc.left-1) > nEndX )
			nWidth		= nEndX - nX - nXOffset + 1;		
		if ( nY < nStartY )	
		{ 
			nYOffset += nStartY - nY;
			nHeight	 = rcSrc.bottom - nYOffset;
		}		
		if ( (nY+rcSrc.bottom-rcSrc.top-1) > nEndY )
			nHeight	= nEndY - nY - nYOffset + 1;


		if ( (nWidth > 0) && (nHeight > 0) )
		{
			rc.left		= nXOffset;
			rc.right	= nXOffset+nWidth;
			rc.top		= nYOffset;
			rc.bottom	= nYOffset+nHeight;

			ddsd.dwSize	= sizeof(DDSURFACEDESC2);
			ddsd.lpSurface = NULL;
			
			m_pddsBackBuffer->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
			if ( !ddsd.lpSurface )			return FALSE;
			WORD* pwdDst;
			pwdDst = (WORD*)ddsd.lpSurface;

			INT nWidthStart	= 0;
			INT nWidthEnd	= 0;
			INT nCurrWidth  = 0;
			INT nCntCopyWord = 0;
			INT nYCnt =0;
			INT nLastWidth = 0;

			FLOAT rBlueRate, rGreenRate, bRedRate;
			BYTE  bRedSrc, bGreenSrc, bBlueSrc;
			BYTE  bRedWantedColor, bGreenWantedColor, bBlueWantedColor;
			WORD  wPixel;

			// y축 클리핑.
			for ( nYCnt=0 ; nYCnt < rc.top ; nYCnt++ )
			{
				nWidthEnd += pwSrc[nWidthStart];
				nWidthStart++;
				nWidthEnd++;
				nWidthStart	= nWidthEnd;
			}

			// y축에 대해서 실제로 루핑시킬 Count만을 정한다.
			for ( nYCnt=rc.top ; nYCnt < rc.bottom ; nYCnt++ )
			{
				// 한라인의 길이를 얻어온다.(단위는 워드)
				nWidthEnd += pwSrc[nWidthStart];
				nWidthStart++;

				// 라인의 길이만큼 화면에 뿌려준다.
				for ( INT x = nWidthStart; x < nWidthEnd ; )
				{
					if ( pwSrc[x] == 0xC0 )
					{
						x++;
						nCntCopyWord = pwSrc[x];
						x++;
						nCurrWidth += nCntCopyWord;
					}
					else if ( pwSrc[x] == 0xC1 )
					{
						x++;
						nCntCopyWord = pwSrc[x];
						x++;

						nLastWidth = nCurrWidth;
						nCurrWidth += nCntCopyWord;

						if ( rc.left > nCurrWidth || rc.right < nLastWidth )
						{
							x += nCntCopyWord;
						}
						else
						{
							// rc.left점을 기준으로 카피할 영역이 걸린 경우.
							if ( nLastWidth < rc.left && rc.left <= nCurrWidth )
							{
								x += (rc.left-nLastWidth);
								memcpy(&pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (rc.left+nX)], &pwSrc[x], sizeof(WORD)*(nCurrWidth-rc.left));
								x += (nCurrWidth-rc.left);
							}
							// rc.right점을 기준으로 카피할 영역이 걸린 경우.
							else if ( nLastWidth <= rc.right && rc.right < nCurrWidth )
							{
								memcpy(&pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX)], &pwSrc[x], sizeof(WORD)*(rc.right-nLastWidth));
								x += nCntCopyWord;
							}
							else
							{
								memcpy(&pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX)], &pwSrc[x], sizeof(WORD)*nCntCopyWord);
								x += nCntCopyWord;
							}
						}
					}
					else if ( pwSrc[x] == 0xC2 || pwSrc[x] == 0xC3 )
					{
						WORD wDyingKind, wChooseColor;
						wDyingKind = pwSrc[x];
						switch ( wDyingKind )
						{
						case 0xC2: 
							wChooseColor = wChooseColor1;
							break;
						case 0xC3: 
							wChooseColor = wChooseColor2;
							break;
						}
						x++;
						nCntCopyWord = pwSrc[x];
						x++;

						nLastWidth = nCurrWidth;
						nCurrWidth += nCntCopyWord;

						if ( rc.left > nCurrWidth || rc.right < nLastWidth )
						{
							x += nCntCopyWord;
						}
						else
						{
							// rc.left점을 기준으로 카피할 영역이 걸린 경우.
							if ( nLastWidth < rc.left && rc.left <= nCurrWidth )
							{
								x += (rc.left-nLastWidth);
								for ( INT nCheck = 0; nCheck < nCurrWidth-rc.left; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)] = ((bRedWantedColor  <<m_stBitsMaskInfo.bRShift) |
																										  (bGreenWantedColor<<m_stBitsMaskInfo.bGShift) |
																										  (bBlueWantedColor <<m_stBitsMaskInfo.bBShift));			
								}
								x += (nCurrWidth-rc.left);
							}
							// rc.right점을 기준으로 카피할 영역이 걸린 경우.
							else if ( nLastWidth <= rc.right && rc.right < nCurrWidth )
							{
								for ( INT nCheck = 0; nCheck < rc.right-nLastWidth; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)] = ((bRedWantedColor  <<m_stBitsMaskInfo.bRShift) |
																										  (bGreenWantedColor<<m_stBitsMaskInfo.bGShift) |
																										  (bBlueWantedColor <<m_stBitsMaskInfo.bBShift));			
								}
								x += nCntCopyWord;
							}
							else
							{
								for ( INT nCheck = 0; nCheck < nCntCopyWord; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)] = ((bRedWantedColor  <<m_stBitsMaskInfo.bRShift) |
																										  (bGreenWantedColor<<m_stBitsMaskInfo.bGShift) |
																									 	  (bBlueWantedColor <<m_stBitsMaskInfo.bBShift));			
								}
								x += nCntCopyWord;
							}
						}
					}	
				}
				// 라인의 끝을 다음 라인의 시작으로 옮겨준다.
				nWidthEnd++;

				nWidthStart	= nWidthEnd;
				nCurrWidth = 0;
			}
		}

		m_pddsBackBuffer->Unlock(NULL);
		return TRUE;
	}
	return FALSE;
}


BOOL CWHDXGraphicWindow::DrawWithImageForComp(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, WORD wChooseColor1, WORD wChooseColor2)
{
	RECT			rc;
	DDSURFACEDESC2	ddsd;

	INT	nWidth		= nXSize;
	INT	nHeight		= nYSize;
	INT	nXOffset	= 0;
	INT	nYOffset	= 0;
	INT	nStartX		= 0;
	INT	nStartY		= 0;
	INT	nEndX		= m_stDisplayInfo.wWidth -1;
	INT	nEndY		= m_stDisplayInfo.wHeight-1;

	if ( m_pddsBackBuffer != NULL )
	{
		if (nX < nStartX )
		{ 
			nXOffset	= nStartX - nX;
			nWidth		= nXSize - nXOffset;
		}
		if ( (nX+nXSize-1) > nEndX )
			nWidth		= nEndX - nX - nXOffset + 1;		
		if ( nY < nStartY )	
		{ 
			nYOffset	= nStartY - nY;
			nHeight		= nYSize - nYOffset;
		}		
		if ( (nY+nYSize-1) > nEndY )
			nHeight	= nEndY - nY - nYOffset + 1;

		if ( (nWidth > 0) && (nHeight > 0) )
		{
			rc.left		= nXOffset;
			rc.right	= nXOffset+nWidth;
			rc.top		= nYOffset;
			rc.bottom	= nYOffset+nHeight;

			ddsd.dwSize	= sizeof(DDSURFACEDESC2);
			ddsd.lpSurface = NULL;
			
			m_pddsBackBuffer->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
			if ( !ddsd.lpSurface )			return FALSE;
			WORD* pwdDst;
			pwdDst = (WORD*)ddsd.lpSurface;

			INT nWidthStart	= 0;
			INT nWidthEnd	= 0;
			INT nCurrWidth  = 0;
			INT nCntCopyWord = 0;
			INT nYCnt =0;
			INT nLastWidth = 0;

			FLOAT rBlueRate, rGreenRate, bRedRate;
			BYTE  bRedSrc, bGreenSrc, bBlueSrc;
			BYTE  bRedWantedColor, bGreenWantedColor, bBlueWantedColor;
			WORD  wPixel;

			// y축 클리핑.
			for ( nYCnt=0 ; nYCnt < rc.top ; nYCnt++ )
			{
				nWidthEnd += pwSrc[nWidthStart];
				nWidthStart++;
				nWidthEnd++;
				nWidthStart	= nWidthEnd;
			}

			// y축에 대해서 실제로 루핑시킬 Count만을 정한다.
			for ( nYCnt=rc.top ; nYCnt < rc.bottom ; nYCnt++ )
			{
				// 한라인의 길이를 얻어온다.(단위는 워드)
				nWidthEnd += pwSrc[nWidthStart];
				nWidthStart++;

				// 라인의 길이만큼 화면에 뿌려준다.
				for ( INT x = nWidthStart; x < nWidthEnd ; )
				{
					if ( pwSrc[x] == 0xC0 )
					{
						x++;
						nCntCopyWord = pwSrc[x];
						x++;
						nCurrWidth += nCntCopyWord;
					}
					else if ( pwSrc[x] == 0xC1 )
					{
						x++;
						nCntCopyWord = pwSrc[x];
						x++;

						nLastWidth = nCurrWidth;
						nCurrWidth += nCntCopyWord;

						if ( rc.left > nCurrWidth || rc.right < nLastWidth )
						{
							x += nCntCopyWord;
						}
						else
						{
							// rc.left점을 기준으로 카피할 영역이 걸린 경우.
							if ( nLastWidth < rc.left && rc.left <= nCurrWidth )
							{
								x += (rc.left-nLastWidth);
								memcpy(&pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (rc.left+nX)], &pwSrc[x], sizeof(WORD)*(nCurrWidth-rc.left));
								x += (nCurrWidth-rc.left);
							}
							// rc.right점을 기준으로 카피할 영역이 걸린 경우.
							else if ( nLastWidth <= rc.right && rc.right < nCurrWidth )
							{
								memcpy(&pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX)], &pwSrc[x], sizeof(WORD)*(rc.right-nLastWidth));
								x += nCntCopyWord;
							}
							else
							{
								memcpy(&pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX)], &pwSrc[x], sizeof(WORD)*nCntCopyWord);
								x += nCntCopyWord;
							}
						}
					}
					else if ( pwSrc[x] == 0xC2 || pwSrc[x] == 0xC3 )
					{
						WORD wDyingKind, wChooseColor;
						wDyingKind = pwSrc[x];
						switch ( wDyingKind )
						{
						case 0xC2: 
							wChooseColor = wChooseColor1;
							break;
						case 0xC3: 
							wChooseColor = wChooseColor2;
							break;
						}
						x++;
						nCntCopyWord = pwSrc[x];
						x++;

						nLastWidth = nCurrWidth;
						nCurrWidth += nCntCopyWord;

						if ( rc.left > nCurrWidth || rc.right < nLastWidth )
						{
							x += nCntCopyWord;
						}
						else
						{
							// rc.left점을 기준으로 카피할 영역이 걸린 경우.
							if ( nLastWidth < rc.left && rc.left <= nCurrWidth )
							{
								x += (rc.left-nLastWidth);
								for ( INT nCheck = 0; nCheck < nCurrWidth-rc.left; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)] = ((bRedWantedColor  <<m_stBitsMaskInfo.bRShift) |
																										  (bGreenWantedColor<<m_stBitsMaskInfo.bGShift) |
																										  (bBlueWantedColor <<m_stBitsMaskInfo.bBShift));			
								}
								x += (nCurrWidth-rc.left);
							}
							// rc.right점을 기준으로 카피할 영역이 걸린 경우.
							else if ( nLastWidth <= rc.right && rc.right < nCurrWidth )
							{
								for ( INT nCheck = 0; nCheck < rc.right-nLastWidth; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)] = ((bRedWantedColor  <<m_stBitsMaskInfo.bRShift) |
																										  (bGreenWantedColor<<m_stBitsMaskInfo.bGShift) |
																										  (bBlueWantedColor <<m_stBitsMaskInfo.bBShift));			
								}
								x += nCntCopyWord;
							}
							else
							{
								for ( INT nCheck = 0; nCheck < nCntCopyWord; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)] = ((bRedWantedColor  <<m_stBitsMaskInfo.bRShift) |
																										  (bGreenWantedColor<<m_stBitsMaskInfo.bGShift) |
																									 	  (bBlueWantedColor <<m_stBitsMaskInfo.bBShift));			
								}
								x += nCntCopyWord;
							}
						}
					}	
				}
				// 라인의 끝을 다음 라인의 시작으로 옮겨준다.
				nWidthEnd++;

				nWidthStart	= nWidthEnd;
				nCurrWidth = 0;
			}
		}

		m_pddsBackBuffer->Unlock(NULL);
		return TRUE;
	}
	return FALSE;
}


BOOL CWHDXGraphicWindow::DrawWithImageForCompClipRgnBase(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, WORD wClipWidth, WORD wClipHeight, WORD wChooseColor1, WORD wChooseColor2)
{
	RECT			rc;
	DDSURFACEDESC2	ddsd;

	INT	nWidth		= nXSize;
	INT	nHeight		= nYSize;
	INT	nXOffset	= 0;
	INT	nYOffset	= 0;
	INT	nStartX		= 0;
	INT	nStartY		= 0;
	INT	nEndX		= wClipWidth -1;
	INT	nEndY		= wClipHeight-1;

	if ( m_pddsBackBuffer != NULL )
	{
		if (nX < nStartX )
		{ 
			nXOffset	= nStartX - nX;
			nWidth		= nXSize - nXOffset;
		}
		if ( (nX+nXSize-1) > nEndX )
			nWidth		= nEndX - nX - nXOffset + 1;		
		if ( nY < nStartY )	
		{ 
			nYOffset	= nStartY - nY;
			nHeight		= nYSize - nYOffset;
		}		
		if ( (nY+nYSize-1) > nEndY )
			nHeight	= nEndY - nY - nYOffset + 1;

		if ( (nWidth > 0) && (nHeight > 0) )
		{
			rc.left		= nXOffset;
			rc.right	= nXOffset+nWidth;
			rc.top		= nYOffset;
			rc.bottom	= nYOffset+nHeight;

			ddsd.dwSize	= sizeof(DDSURFACEDESC2);
			ddsd.lpSurface = NULL;
			
			m_pddsBackBuffer->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
			if ( !ddsd.lpSurface )			return FALSE;
			WORD* pwdDst;
			pwdDst = (WORD*)ddsd.lpSurface;

			INT nWidthStart	= 0;
			INT nWidthEnd	= 0;
			INT nCurrWidth  = 0;
			INT nCntCopyWord = 0;
			INT nYCnt =0;
			INT nLastWidth = 0;

			FLOAT rBlueRate, rGreenRate, bRedRate;
			BYTE  bRedSrc, bGreenSrc, bBlueSrc;
			BYTE  bRedWantedColor, bGreenWantedColor, bBlueWantedColor;
			WORD  wPixel;

			// y축 클리핑.
  			for ( nYCnt=0 ; nYCnt < rc.top ; nYCnt++ )
			{
				nWidthEnd += pwSrc[nWidthStart];
				nWidthStart++;
				nWidthEnd++;
				nWidthStart	= nWidthEnd;
			}

			// y축에 대해서 실제로 루핑시킬 Count만을 정한다.
			for ( nYCnt=rc.top ; nYCnt < rc.bottom ; nYCnt++ )
			{
				// 한라인의 길이를 얻어온다.(단위는 워드)
				nWidthEnd += pwSrc[nWidthStart];
				nWidthStart++;

				// 라인의 길이만큼 화면에 뿌려준다.
				for ( INT x = nWidthStart; x < nWidthEnd ; )
				{
					if ( pwSrc[x] == 0xC0 )
					{
						x++;
						nCntCopyWord = pwSrc[x];
						x++;
						nCurrWidth += nCntCopyWord;
					}
					else if ( pwSrc[x] == 0xC1 )
					{
						x++;
						nCntCopyWord = pwSrc[x];
						x++;

						nLastWidth = nCurrWidth;
						nCurrWidth += nCntCopyWord;

						if ( rc.left > nCurrWidth || rc.right < nLastWidth )
						{
							x += nCntCopyWord;
						}
						else
						{
							// rc.left점을 기준으로 카피할 영역이 걸린 경우.
							if ( nLastWidth < rc.left && rc.left <= nCurrWidth )
							{
								x += (rc.left-nLastWidth);
								memcpy(&pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (rc.left+nX)], &pwSrc[x], sizeof(WORD)*(nCurrWidth-rc.left));
								x += (nCurrWidth-rc.left);
							}
							// rc.right점을 기준으로 카피할 영역이 걸린 경우.
							else if ( nLastWidth <= rc.right && rc.right < nCurrWidth )
							{
								memcpy(&pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX)], &pwSrc[x], sizeof(WORD)*(rc.right-nLastWidth));
								x += nCntCopyWord;
							}
							else
							{
								memcpy(&pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX)], &pwSrc[x], sizeof(WORD)*nCntCopyWord);
								x += nCntCopyWord;
							}
						}
					}
					else if ( pwSrc[x] == 0xC2 || pwSrc[x] == 0xC3 )
					{
						WORD wDyingKind, wChooseColor;
						wDyingKind = pwSrc[x];
						switch ( wDyingKind )
						{
						case 0xC2: 
							wChooseColor = wChooseColor1;
							break;
						case 0xC3: 
							wChooseColor = wChooseColor2;
							break;
						}
						x++;
						nCntCopyWord = pwSrc[x];
						x++;

						nLastWidth = nCurrWidth;
						nCurrWidth += nCntCopyWord;

						if ( rc.left > nCurrWidth || rc.right < nLastWidth )
						{
							x += nCntCopyWord;
						}
						else
						{
							// rc.left점을 기준으로 카피할 영역이 걸린 경우.
							if ( nLastWidth < rc.left && rc.left <= nCurrWidth )
							{
								x += (rc.left-nLastWidth);
								for ( INT nCheck = 0; nCheck < nCurrWidth-rc.left; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)] = ((bRedWantedColor  <<m_stBitsMaskInfo.bRShift) |
																										  (bGreenWantedColor<<m_stBitsMaskInfo.bGShift) |
																									 	  (bBlueWantedColor <<m_stBitsMaskInfo.bBShift));			
								}
								x += (nCurrWidth-rc.left);
							}
							// rc.right점을 기준으로 카피할 영역이 걸린 경우.
							else if ( nLastWidth <= rc.right && rc.right < nCurrWidth )
							{
								for ( INT nCheck = 0; nCheck < rc.right-nLastWidth; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)] = ((bRedWantedColor  <<m_stBitsMaskInfo.bRShift) |
																										  (bGreenWantedColor<<m_stBitsMaskInfo.bGShift) |
																									 	  (bBlueWantedColor <<m_stBitsMaskInfo.bBShift));			
								}
								x += nCntCopyWord;
							}
							else
							{
								for ( INT nCheck = 0; nCheck < nCntCopyWord; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)] = ((bRedWantedColor  <<m_stBitsMaskInfo.bRShift) |
																										  (bGreenWantedColor<<m_stBitsMaskInfo.bGShift) |
																									 	  (bBlueWantedColor <<m_stBitsMaskInfo.bBShift));			
								}
								x += nCntCopyWord;
							}
						}
					}	
				}
				// 라인의 끝을 다음 라인의 시작으로 옮겨준다.
				nWidthEnd++;

				nWidthStart	= nWidthEnd;
				nCurrWidth = 0;
			}
		}

		m_pddsBackBuffer->Unlock(NULL);
		return TRUE;
	}
	return FALSE;
}


BOOL CWHDXGraphicWindow::DrawWithABlendCompDataWithBackBuffer(INT nX, INT nY, 
															  INT nXSize, INT nYSize, WORD* pwSrc,
															  WORD wClipWidth, WORD wClipHeight,
															  WORD wChooseColor1, WORD wChooseColor2, BYTE bOpa)
{
	RECT			rc;
	DDSURFACEDESC2	ddsd;

	INT	nWidth		= nXSize;
	INT	nHeight		= nYSize;
	INT	nXOffset	= 0;
	INT	nYOffset	= 0;
	INT	nStartX		= 0;
	INT	nStartY		= 0;
	INT	nEndX		= wClipWidth -1;
	INT	nEndY		= wClipHeight-1;

	WORD wDyingKind, wChooseColor;

	if ( m_pddsBackBuffer != NULL )
	{
		if (nX < nStartX )
		{ 
			nXOffset	= nStartX - nX;
			nWidth		= nXSize - nXOffset;
		}
		if ( (nX+nXSize-1) > nEndX )
			nWidth		= nEndX - nX - nXOffset + 1;		
		if ( nY < nStartY )	
		{ 
			nYOffset	= nStartY - nY;
			nHeight		= nYSize - nYOffset;
		}		
		if ( (nY+nYSize-1) > nEndY )
			nHeight	= nEndY - nY - nYOffset + 1;

		if ( (nWidth > 0) && (nHeight > 0) )
		{
			rc.left		= nXOffset;
			rc.right	= nXOffset+nWidth;
			rc.top		= nYOffset;
			rc.bottom	= nYOffset+nHeight;

			ddsd.dwSize	= sizeof(DDSURFACEDESC2);
			ddsd.lpSurface = NULL;
			
			m_pddsBackBuffer->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
			if ( !ddsd.lpSurface )			return FALSE;
			WORD* pwdDst;
			pwdDst = (WORD*)ddsd.lpSurface;

			INT nWidthStart	= 0;
			INT nWidthEnd	= 0;
			INT nCurrWidth  = 0;
			INT nCntCopyWord = 0;
			INT nYCnt =0;
			INT nLastWidth = 0;

			BYTE bRedDst, bGreenDst, bBlueDst;
			BYTE bRedSrc, bGreenSrc, bBlueSrc;
			BYTE bRedWantedColor, bGreenWantedColor, bBlueWantedColor;
			WORD wPixel;
			FLOAT rBlueRate, rGreenRate, bRedRate;

			// y축 클리핑.
  			for ( nYCnt=0 ; nYCnt < rc.top ; nYCnt++ )
			{
				nWidthEnd += pwSrc[nWidthStart];
				nWidthStart++;
				nWidthEnd++;
				nWidthStart	= nWidthEnd;
			}

			// y축에 대해서 실제로 루핑시킬 Count만을 정한다.
			for ( nYCnt=rc.top ; nYCnt < rc.bottom ; nYCnt++ )
			{
				// 한라인의 길이를 얻어온다.(단위는 워드)
				nWidthEnd += pwSrc[nWidthStart];
				nWidthStart++;

				// 라인의 길이만큼 화면에 뿌려준다.
				for ( INT x = nWidthStart; x < nWidthEnd ; )
				{
					if ( pwSrc[x] == 0xC0 )
					{
						x++;
						nCntCopyWord = pwSrc[x];
						x++;
						nCurrWidth += nCntCopyWord;
					}
					else if ( pwSrc[x] == 0xC1 )
					{
						x++;
						nCntCopyWord = pwSrc[x];
						x++;

						nLastWidth = nCurrWidth;
						nCurrWidth += nCntCopyWord;

						if ( rc.left > nCurrWidth || rc.right < nLastWidth )
						{
							x += nCntCopyWord;
						}
						else
						{
							// rc.left점을 기준으로 카피할 영역이 걸린 경우.
							if ( nLastWidth < rc.left && rc.left <= nCurrWidth )
							{
								x += (rc.left-nLastWidth);
								for ( INT nCheck = 0; nCheck < nCurrWidth-rc.left; nCheck++ )
								{
									wPixel	   = pwSrc[x+nCheck];
									bBlueSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc    = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	   = pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (rc.left+nX+nCheck)];
									bBlueDst   = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenDst  = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedDst    = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									bBlueDst   = (BYTE)((bOpa*(bBlueDst -bBlueSrc )+100*bBlueSrc  ) / 100);
									bGreenDst  = (BYTE)((bOpa*(bGreenDst-bGreenSrc)+100*bGreenSrc ) / 100);
									bRedDst    = (BYTE)((bOpa*(bRedDst  -bRedSrc  )+100*bRedSrc   ) / 100);

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (rc.left+nX+nCheck)] = ((bRedDst  <<m_stBitsMaskInfo.bRShift) |
																									   (bGreenDst<<m_stBitsMaskInfo.bGShift) |
																									   (bBlueDst <<m_stBitsMaskInfo.bBShift));										
								}
								x += (nCurrWidth-rc.left);
							}
							// rc.right점을 기준으로 카피할 영역이 걸린 경우.
							else if ( nLastWidth <= rc.right && rc.right < nCurrWidth )
							{
								for ( INT nCheck = 0; nCheck < rc.right-nLastWidth; nCheck++ )
								{
									wPixel	   = pwSrc[x+nCheck];
									bBlueSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc    = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	   = pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)];
									bBlueDst   = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenDst  = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedDst    = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									bBlueDst   = (BYTE)((bOpa*(bBlueDst -bBlueSrc )+100*bBlueSrc  ) / 100);
									bGreenDst  = (BYTE)((bOpa*(bGreenDst-bGreenSrc)+100*bGreenSrc ) / 100);
									bRedDst    = (BYTE)((bOpa*(bRedDst  -bRedSrc  )+100*bRedSrc   ) / 100);

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)] = ((bRedDst  <<m_stBitsMaskInfo.bRShift) |
																									      (bGreenDst<<m_stBitsMaskInfo.bGShift) |
																										  (bBlueDst <<m_stBitsMaskInfo.bBShift));			
								}
								x += nCntCopyWord;
							}
							else
							{
								for ( INT nCheck = 0; nCheck < nCntCopyWord; nCheck++ )
								{
									wPixel	   = pwSrc[x+nCheck];
									bBlueSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc    = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	   = pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)];
									bBlueDst   = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenDst  = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedDst    = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									bBlueDst   = (BYTE)((bOpa*(bBlueDst -bBlueSrc )+100*bBlueSrc  ) / 100);
									bGreenDst  = (BYTE)((bOpa*(bGreenDst-bGreenSrc)+100*bGreenSrc ) / 100);
									bRedDst    = (BYTE)((bOpa*(bRedDst  -bRedSrc  )+100*bRedSrc   ) / 100);

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)] = ((bRedDst  <<m_stBitsMaskInfo.bRShift) |
																									      (bGreenDst<<m_stBitsMaskInfo.bGShift) |
																										  (bBlueDst <<m_stBitsMaskInfo.bBShift));			
								}
								x += nCntCopyWord;
							}
						}
					}
					else if ( pwSrc[x] == 0xC2 || pwSrc[x] == 0xC3 )
					{
						wDyingKind = pwSrc[x];
						switch ( wDyingKind )
						{
						case 0xC2: 
							wChooseColor = wChooseColor1;
							break;
						case 0xC3: 
							wChooseColor = wChooseColor2;
							break;
						}
						x++;
						nCntCopyWord = pwSrc[x];
						x++;

						nLastWidth = nCurrWidth;
						nCurrWidth += nCntCopyWord;

						if ( rc.left > nCurrWidth || rc.right < nLastWidth )
						{
							x += nCntCopyWord;
						}
						else
						{
							// rc.left점을 기준으로 카피할 영역이 걸린 경우.
							if ( nLastWidth < rc.left && rc.left <= nCurrWidth )
							{
								x += (rc.left-nLastWidth);
								for ( INT nCheck = 0; nCheck < nCurrWidth-rc.left; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);


									wPixel  = pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (rc.left+nX+nCheck)];
									bBlueDst   = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenDst  = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedDst    = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									bBlueDst   = (BYTE)((bOpa*(bBlueDst -bBlueWantedColor )+100*bBlueWantedColor ) / 100);
									bGreenDst  = (BYTE)((bOpa*(bGreenDst-bGreenWantedColor)+100*bGreenWantedColor) / 100);
									bRedDst    = (BYTE)((bOpa*(bRedDst  -bRedWantedColor  )+100*bRedWantedColor  ) / 100);

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (rc.left+nX+nCheck)] = ((bRedDst  <<m_stBitsMaskInfo.bRShift) |
																									   (bGreenDst<<m_stBitsMaskInfo.bGShift) |
																									   (bBlueDst <<m_stBitsMaskInfo.bBShift));
								}
								x += (nCurrWidth-rc.left);
							}
							// rc.right점을 기준으로 카피할 영역이 걸린 경우.
							else if ( nLastWidth <= rc.right && rc.right < nCurrWidth )
							{
								for ( INT nCheck = 0; nCheck < rc.right-nLastWidth; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);


									wPixel  = pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)];
									bBlueDst   = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenDst  = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedDst    = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									bBlueDst   = (BYTE)((bOpa*(bBlueDst -bBlueWantedColor )+100*bBlueWantedColor ) / 100);
									bGreenDst  = (BYTE)((bOpa*(bGreenDst-bGreenWantedColor)+100*bGreenWantedColor) / 100);
									bRedDst    = (BYTE)((bOpa*(bRedDst  -bRedWantedColor  )+100*bRedWantedColor  ) / 100);

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)] = ((bRedDst  <<m_stBitsMaskInfo.bRShift) |
																									      (bGreenDst<<m_stBitsMaskInfo.bGShift) |
																										  (bBlueDst <<m_stBitsMaskInfo.bBShift));			
								}
								x += nCntCopyWord;
							}
							else
							{
								for ( INT nCheck = 0; nCheck < nCntCopyWord; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);


									wPixel  = pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)];
									bBlueDst   = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenDst  = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedDst    = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									bBlueDst   = (BYTE)((bOpa*(bBlueDst -bBlueWantedColor )+100*bBlueWantedColor ) / 100);
									bGreenDst  = (BYTE)((bOpa*(bGreenDst-bGreenWantedColor)+100*bGreenWantedColor) / 100);
									bRedDst    = (BYTE)((bOpa*(bRedDst  -bRedWantedColor  )+100*bRedWantedColor  ) / 100);

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)] = ((bRedDst  <<m_stBitsMaskInfo.bRShift) |
																									      (bGreenDst<<m_stBitsMaskInfo.bGShift) |
																										  (bBlueDst <<m_stBitsMaskInfo.bBShift));			
								}
								x += nCntCopyWord;
							}
						}
					}	
				}
				// 라인의 끝을 다음 라인의 시작으로 옮겨준다.
				nWidthEnd++;

				nWidthStart	= nWidthEnd;
				nCurrWidth = 0;
			}
		}

		m_pddsBackBuffer->Unlock(NULL);
		return TRUE;
	}
	return FALSE;
}


// nX, nY는 중심점의 좌표이다.
BOOL CWHDXGraphicWindow::DrawWithShadowABlend(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, WORD wClipWidth, WORD wClipHeight,
											  WORD* pwShadowClrSrc, BOOL bBlend, BYTE bShadowType, BYTE bOpa)

{
	// 찍지 않는 그림자.
	if ( bShadowType == 48 )
		return FALSE;

	RECT			rc;
	DDSURFACEDESC2	ddsd;

	INT	nWidth		= nXSize;
	INT	nHeight		= nYSize;
	INT	nXOffset	= 0;
	INT	nYOffset	= 0;
	INT	nStartX		= 0;
	INT	nStartY		= 0;
	INT	nEndX		= wClipWidth -1;
	INT	nEndY		= wClipHeight-1;


	if ( m_pddsBackBuffer != NULL )
	{
		rc.left		= 0;
		rc.right	= nWidth;
		rc.top		= 0;
		rc.bottom	= nHeight;

		ddsd.dwSize	= sizeof(DDSURFACEDESC2);
		ddsd.lpSurface = NULL;
			
		m_pddsBackBuffer->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
		if ( !ddsd.lpSurface )			return FALSE;
		WORD* pwdDst;
		pwdDst = (WORD*)ddsd.lpSurface;

		INT nWidthStart	= 0;
		INT nWidthEnd	= 0;
		INT nCurrWidth  = 0;
		INT nCntCopyWord = 0;
		INT nYCnt =0;
		INT nLastWidth = 0;
		INT nDstXPos = 0;
		INT nDstYPos = 0;

		WORD wPixel;
		BYTE bBlueWantedColor, bGreenWantedColor, bRedWantedColor, bBlueDst, bGreenDst, bRedDst;
		RECT rcScrnShadow;
		WORD wSahdowColor = pwShadowClrSrc[0];

		if ( bShadowType == 49 )
		{
			SetRect(&rcScrnShadow, nX, nY, nX+nHeight/2+nWidth, nY+nHeight/2);
		}
		else if ( bShadowType == 50 )
		{
			SetRect(&rcScrnShadow, nX, nY, nX+nWidth, nY+nHeight);
		}

		// y축에 대해서 실제로 루핑시킬 Count만을 정한다.
		for ( nYCnt=rc.top ; nYCnt < rc.bottom ; nYCnt++ )
		{
			// 한라인의 길이를 얻어온다.(단위는 워드)
			nWidthEnd += pwSrc[nWidthStart];
			nWidthStart++;

			if ( bShadowType == 49 )
			{
				nDstYPos = nYCnt-nYCnt/2+nY;
			}
			else if ( bShadowType == 50 )
			{
				nDstYPos = nYCnt+nY;
			}

			if ( nDstYPos < 0 || nDstYPos >= wClipHeight || (bShadowType==49 && nYCnt%2) )
			{
				// 라인의 끝을 다음 라인의 시작으로 옮겨준다.
				nWidthEnd++;
				nWidthStart	= nWidthEnd;
				nCurrWidth = 0;
				continue;
			}

			// 라인의 길이만큼 화면에 뿌려준다.
			for ( INT x = nWidthStart; x < nWidthEnd ; )
			{
				if ( pwSrc[x] == 0xC0 )
				{
					x++;
					nCntCopyWord = pwSrc[x];
					x++;
					nCurrWidth += nCntCopyWord;
				}
				else if ( pwSrc[x] == 0xC1 || pwSrc[x] == 0xC2 || pwSrc[x] == 0xC3 )
				{
					x++;
					nCntCopyWord = pwSrc[x];
					x++;

					nLastWidth = nCurrWidth;
					nCurrWidth += nCntCopyWord;

					if ( bShadowType == 49 )
					{
						nDstXPos = (nLastWidth+nX+(rc.bottom-nYCnt)/2);
					}
					else if ( bShadowType == 50 )
					{
						nDstXPos = (nLastWidth+nX);
					}

					if ( !bBlend ) 
					{
						// 왼쪽 걸릴때.
						if ( nDstXPos < 0 && nDstXPos+nCntCopyWord >= 0 )
						{
							for ( INT nCheck = 0; nCheck < nCntCopyWord; nCheck++ )
							{
								if ( nDstXPos+nCheck >= 0 && nDstXPos+nCheck < wClipWidth )
								{
									pwdDst[(nDstYPos * (ddsd.lPitch >> 1)) + nDstXPos+nCheck] = wSahdowColor;

								}
							}
							x += nCntCopyWord;
						}
						// 오른쪽 걸릴때.
						else if ( nDstXPos < wClipWidth && nDstXPos+nCntCopyWord >= wClipWidth )
						{
							for ( INT nCheck = 0; nCheck < nCntCopyWord; nCheck++ )
							{
								if ( nDstXPos+nCheck >= 0 && nDstXPos+nCheck < wClipWidth )
								{
									pwdDst[(nDstYPos * (ddsd.lPitch >> 1)) + nDstXPos+nCheck] = wSahdowColor;

								}
							}
							x += nCntCopyWord;
						}
						// 안걸릴때.
						else if ( nDstXPos >= 0 && nDstXPos+nCntCopyWord < wClipWidth )
						{
							memcpy(&pwdDst[(nDstYPos * (ddsd.lPitch >> 1)) + nDstXPos], pwShadowClrSrc, sizeof(WORD)*nCntCopyWord);
//							memset(&pwdDst[(nDstYPos * (ddsd.lPitch >> 1)) + nDstXPos], wSahdowColor, sizeof(WORD)*nCntCopyWord);
							x += nCntCopyWord;
						}
						else
						{
							x += nCntCopyWord;
						}
					}
					else
					{
						// 왼쪽 걸릴때.
						if ( nDstXPos < 0 && nDstXPos+nCntCopyWord >= 0 )
						{
							for ( INT nCheck = 0; nCheck < nCntCopyWord; nCheck++ )
							{
								if ( nDstXPos+nCheck >= 0 && nDstXPos+nCheck < wClipWidth )
								{
//									pwdDst[(nDstYPos * (ddsd.lPitch >> 1)) + nDstXPos+nCheck] = wSahdowColor;
									wPixel = wSahdowColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel  = pwdDst[(nDstYPos * (ddsd.lPitch >> 1)) + nDstXPos+nCheck];
									bBlueDst   = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenDst  = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedDst    = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									bBlueDst   = (BYTE)((bOpa*(bBlueDst -bBlueWantedColor)+100*bBlueWantedColor  ) / 100);
									bGreenDst  = (BYTE)((bOpa*(bGreenDst-bGreenWantedColor)+100*bGreenWantedColor) / 100);
									bRedDst    = (BYTE)((bOpa*(bRedDst  -bRedWantedColor)+100*bRedWantedColor    ) / 100);

									pwdDst[(nDstYPos * (ddsd.lPitch >> 1)) + nDstXPos+nCheck] = ((bRedDst  <<m_stBitsMaskInfo.bRShift) |
																						  		 (bGreenDst<<m_stBitsMaskInfo.bGShift) |
																								 (bBlueDst <<m_stBitsMaskInfo.bBShift));							
								}
							}
							x += nCntCopyWord;
						}
						// 오른쪽 걸릴때.
						else if ( nDstXPos < wClipWidth && nDstXPos+nCntCopyWord >= wClipWidth )
						{
							for ( INT nCheck = 0; nCheck < nCntCopyWord; nCheck++ )
							{
								if ( nDstXPos+nCheck >= 0 && nDstXPos+nCheck < wClipWidth )
								{
//									pwdDst[(nDstYPos * (ddsd.lPitch >> 1)) + nDstXPos+nCheck] = wSahdowColor;

									WORD wPixel = wSahdowColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel  = pwdDst[(nDstYPos * (ddsd.lPitch >> 1)) + nDstXPos+nCheck];
									bBlueDst   = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenDst  = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedDst    = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									bBlueDst   = (BYTE)((bOpa*(bBlueDst -bBlueWantedColor)+100*bBlueWantedColor  ) / 100);
									bGreenDst  = (BYTE)((bOpa*(bGreenDst-bGreenWantedColor)+100*bGreenWantedColor) / 100);
									bRedDst    = (BYTE)((bOpa*(bRedDst  -bRedWantedColor)+100*bRedWantedColor    ) / 100);

									pwdDst[(nDstYPos * (ddsd.lPitch >> 1)) + nDstXPos+nCheck] = ((bRedDst  <<m_stBitsMaskInfo.bRShift) |
																						  		 (bGreenDst<<m_stBitsMaskInfo.bGShift) |
																								 (bBlueDst <<m_stBitsMaskInfo.bBShift));							

								}
							}
							x += nCntCopyWord;
						}
						// 안걸릴때.
						else if ( nDstXPos >= 0 && nDstXPos+nCntCopyWord < wClipWidth )
						{
	//						memset(&pwdDst[(nDstYPos * (ddsd.lPitch >> 1)) + nDstXPos], wSahdowColor, sizeof(WORD)*nCntCopyWord);
	//						x += nCntCopyWord;

							for ( INT nCheck = 0; nCheck < nCntCopyWord; nCheck++ )
							{
	//							pwdDst[(nDstYPos * (ddsd.lPitch >> 1)) + nDstXPos+nCheck] = wSahdowColor;

								wPixel = wSahdowColor;
								bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
								bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
								bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

								wPixel  = pwdDst[(nDstYPos * (ddsd.lPitch >> 1)) + nDstXPos+nCheck];
								bBlueDst   = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
								bGreenDst  = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
								bRedDst    = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

								bBlueDst   = (BYTE)((bOpa*(bBlueDst -bBlueWantedColor)+100*bBlueWantedColor  ) / 100);
								bGreenDst  = (BYTE)((bOpa*(bGreenDst-bGreenWantedColor)+100*bGreenWantedColor) / 100);
								bRedDst    = (BYTE)((bOpa*(bRedDst  -bRedWantedColor)+100*bRedWantedColor    ) / 100);

								pwdDst[(nDstYPos * (ddsd.lPitch >> 1)) + nDstXPos+nCheck] = ((bRedDst  <<m_stBitsMaskInfo.bRShift) |
																							 (bGreenDst<<m_stBitsMaskInfo.bGShift) |
																							 (bBlueDst <<m_stBitsMaskInfo.bBShift));							
							}
							x += nCntCopyWord;
						}
						else
						{
							x += nCntCopyWord;
						}
					}
				}
			}
			// 라인의 끝을 다음 라인의 시작으로 옮겨준다.
			nWidthEnd++;

			nWidthStart	= nWidthEnd;
			nCurrWidth = 0;
		}
			
		m_pddsBackBuffer->Unlock(NULL);
		return TRUE;
	}
	return FALSE;
}



VOID CWHDXGraphicWindow::DrawWithShadowABlend(INT nX, INT nY, INT nXSize, INT nYSize, INT nPX, INT nPY, WORD* pwSrc, WORD wClipWidth, WORD wClipHeight,
											  WORD* pwShadowClrSrc, BOOL bBlend, BYTE bOpa)
{
	DDSURFACEDESC2	ddsd;

#define _HALF_CELL_WIDTH	24
#define _HALF_CELL_HEIGHT	16

//  화면(800*600)에서 그림자가 찍힐 사이즈를 구한다.
	
	// 화면(800*600)에서의 셀시작좌표가 아닌 실제 중심점 좌표(셀위 정중앙점이다.)
	INT		nScrnCenterX = nX - nPX + 24;//_HALF_CELL_WIDTH;
	INT		nScrnCenterY = nY - nPY + 16;//_HALF_CELL_HEIGHT;
	INT		nRealCenterX = nX - nScrnCenterX;
	INT		nRealCenterY = nY - nScrnCenterY;

	INT		nShadowCenterX = nX;
	INT		nShadowCenterY = nY-nRealCenterY/2;

	RECT	rcSrc		 = {0, 0, nXSize, nYSize};

	BYTE bRedDst, bGreenDst, bBlueDst;
	BYTE bRedWantedColor, bGreenWantedColor, bBlueWantedColor;
	WORD wPixel;
	WORD wSahdowColor = pwShadowClrSrc[0];

	if ( m_pddsBackBuffer != NULL )
	{
		ddsd.dwSize	= sizeof(DDSURFACEDESC2);
		ddsd.lpSurface = NULL;

		m_pddsBackBuffer->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
		if ( !ddsd.lpSurface )			return;
		WORD* pwdDst = NULL;
		pwdDst = (WORD*)ddsd.lpSurface;

		if ( pwdDst )
		{
			INT nWidthStart	 = 0;
			INT nWidthEnd	 = 0;
			INT nCurrWidth   = 0;
			INT nLastWidth	 = 0;
			INT nCntCopyWord = 0;
			INT nYCnt		 = 0;

			INT nDstYPos = nShadowCenterY;

			for ( nYCnt=rcSrc.top ; nYCnt < rcSrc.bottom ; nYCnt++ )
			{
				if ( nDstYPos >= wClipHeight )
				{
					m_pddsBackBuffer->Unlock(NULL);
					return;
				}
				// y축 클리핑및 한줄씩 건너뛰기.
				if ( nYCnt%2 == 0 ||  nDstYPos < 0 )
				{
					nWidthEnd += pwSrc[nWidthStart];
					nWidthStart++;
					nWidthEnd++;
					nWidthStart	= nWidthEnd;

					nDstYPos++;
				}
				else
				{
					// 한라인의 길이를 얻어온다.(단위는 워드)
					nWidthEnd += pwSrc[nWidthStart];
					nWidthStart++;

					// 라인의 길이만큼 화면에 뿌려준다.
					for ( INT x = nWidthStart; x < nWidthEnd ; )
					{
						if ( pwSrc[x] == 0xC0 )
						{		
							x++;
							nCntCopyWord = pwSrc[x];
							x++;
							nCurrWidth += nCntCopyWord;
						}
						else
						{
							x++;
							nCntCopyWord = pwSrc[x];
							x++;
							nLastWidth = nCurrWidth;
							nCurrWidth += nCntCopyWord;

							if ( bBlend )
							{
								if ((nShadowCenterX+nLastWidth+((-nRealCenterY/2)-nYCnt/2)) < 0 && (nShadowCenterX+nCurrWidth+((-nRealCenterY/2)-nYCnt/2)) >= 0 )
								{
								}
								else if ((nShadowCenterX+nLastWidth+((-nRealCenterY/2)-nYCnt/2)) < wClipWidth && (nShadowCenterX+nCurrWidth+((-nRealCenterY/2)-nYCnt/2)) >= wClipWidth )
								{
								}
								else if ( (nShadowCenterX+nCurrWidth+((-nRealCenterY/2)-nYCnt/2)) < 0 )
								{
								}
								else if ( (nShadowCenterX+nCurrWidth+((-nRealCenterY/2)-nYCnt/2)) >= wClipWidth )
								{
								}
								else
								{
									for ( INT nCheck = 0; nCheck < nCntCopyWord; nCheck++ )
									{
										wPixel = wSahdowColor;
										bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
										bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
										bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

										wPixel  = pwdDst[nDstYPos*(ddsd.lPitch >> 1) + (nShadowCenterX+nLastWidth+((-nRealCenterY/2)-nYCnt/2)) + nCheck];
										bBlueDst   = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
										bGreenDst  = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
										bRedDst    = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

										bBlueDst   = (BYTE)((bOpa*(bBlueDst -bBlueWantedColor)+100*bBlueWantedColor  ) / 100);
										bGreenDst  = (BYTE)((bOpa*(bGreenDst-bGreenWantedColor)+100*bGreenWantedColor) / 100);
										bRedDst    = (BYTE)((bOpa*(bRedDst  -bRedWantedColor)+100*bRedWantedColor    ) / 100);

										pwdDst[nDstYPos*(ddsd.lPitch >> 1) + (nShadowCenterX+nLastWidth+((-nRealCenterY/2)-nYCnt/2)+nCheck)] = ((bRedDst  <<m_stBitsMaskInfo.bRShift) |
																																				(bGreenDst<<m_stBitsMaskInfo.bGShift) |
																																				(bBlueDst <<m_stBitsMaskInfo.bBShift));							}
								}
							}
							else
							{
								if ((nShadowCenterX+nLastWidth+((-nRealCenterY/2)-nYCnt/2)) < 0 && (nShadowCenterX+nCurrWidth+((-nRealCenterY/2)-nYCnt/2)) >= 0 )
								{
								}
								else if ((nShadowCenterX+nLastWidth+((-nRealCenterY/2)-nYCnt/2)) < wClipWidth && (nShadowCenterX+nCurrWidth+((-nRealCenterY/2)-nYCnt/2)) >= wClipWidth )
								{
								}
								else if ( (nShadowCenterX+nCurrWidth+((-nRealCenterY/2)-nYCnt/2)) < 0 )
								{
								}
								else if ( (nShadowCenterX+nCurrWidth+((-nRealCenterY/2)-nYCnt/2)) >= wClipWidth )
								{
								}
								else
								{
//									memset(&pwdDst[nDstYPos*(ddsd.lPitch >> 1) + (nShadowCenterX+nLastWidth+((-nRealCenterY/2)-nYCnt/2))], wSahdowColor, sizeof(WORD)*nCntCopyWord);
									memcpy(&pwdDst[nDstYPos*(ddsd.lPitch >> 1) + (nShadowCenterX+nLastWidth+((-nRealCenterY/2)-nYCnt/2))], pwShadowClrSrc, sizeof(WORD)*nCntCopyWord);
								}
							}
							x += nCntCopyWord;
						}
					}
					// 라인의 끝을 다음 라인의 시작으로 옮겨준다.
					nWidthEnd++;

					nWidthStart	= nWidthEnd;
					nCurrWidth = 0;
				}
			}
			m_pddsBackBuffer->Unlock(NULL);
		}
	}
}


BOOL CWHDXGraphicWindow::DrawWithABlendCompDataWithLightedColor(INT nX, INT nY, 
															    INT nXSize, INT nYSize, WORD* pwSrc,
															    WORD wClipWidth, WORD wClipHeight,
															    WORD wChooseColor1, WORD wChooseColor2)
{
	RECT			rc;
	DDSURFACEDESC2	ddsd;

	INT	nWidth		= nXSize;
	INT	nHeight		= nYSize;
	INT	nXOffset	= 0;
	INT	nYOffset	= 0;
	INT	nStartX		= 0;
	INT	nStartY		= 0;
	INT	nEndX		= wClipWidth -1;
	INT	nEndY		= wClipHeight-1;

	FLOAT fBright = 0.3f;

	if ( m_pddsBackBuffer != NULL )
	{
		if (nX < nStartX )
		{ 
			nXOffset	= nStartX - nX;
			nWidth		= nXSize - nXOffset;
		}
		if ( (nX+nXSize-1) > nEndX )
			nWidth		= nEndX - nX - nXOffset + 1;		
		if ( nY < nStartY )	
		{ 
			nYOffset	= nStartY - nY;
			nHeight		= nYSize - nYOffset;
		}		
		if ( (nY+nYSize-1) > nEndY )
			nHeight	= nEndY - nY - nYOffset + 1;

		if ( (nWidth > 0) && (nHeight > 0) )
		{
			rc.left		= nXOffset;
			rc.right	= nXOffset+nWidth;
			rc.top		= nYOffset;
			rc.bottom	= nYOffset+nHeight;

			ddsd.dwSize	= sizeof(DDSURFACEDESC2);
			ddsd.lpSurface = NULL;
			
			m_pddsBackBuffer->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
			if ( !ddsd.lpSurface )			return FALSE;
			WORD* pwdDst;
			pwdDst = (WORD*)ddsd.lpSurface;

			INT nWidthStart	= 0;
			INT nWidthEnd	= 0;
			INT nCurrWidth  = 0;
			INT nCntCopyWord = 0;
			INT nYCnt =0;
			INT nLastWidth = 0;

			BYTE  bRedWantedColor, bGreenWantedColor, bBlueWantedColor;
			BYTE bRedSrc, bGreenSrc, bBlueSrc;
			WORD wPixel;
			WORD wDyingKind, wChooseColor;
			FLOAT rBlueRate, rGreenRate, bRedRate;

			// y축 클리핑.
  			for ( nYCnt=0 ; nYCnt < rc.top ; nYCnt++ )
			{
				nWidthEnd += pwSrc[nWidthStart];
				nWidthStart++;
				nWidthEnd++;
				nWidthStart	= nWidthEnd;
			}

			// y축에 대해서 실제로 루핑시킬 Count만을 정한다.
			for ( nYCnt=rc.top ; nYCnt < rc.bottom ; nYCnt++ )
			{
				// 한라인의 길이를 얻어온다.(단위는 워드)
				nWidthEnd += pwSrc[nWidthStart];
				nWidthStart++;

				// 라인의 길이만큼 화면에 뿌려준다.
				for ( INT x = nWidthStart; x < nWidthEnd ; )
				{
					if ( pwSrc[x] == 0xC0 )
					{
						x++;
						nCntCopyWord = pwSrc[x];
						x++;
						nCurrWidth += nCntCopyWord;
					}
					else if ( pwSrc[x] == 0xC1 )
					{
						x++;
						nCntCopyWord = pwSrc[x];
						x++;

						nLastWidth = nCurrWidth;
						nCurrWidth += nCntCopyWord;

						if ( rc.left > nCurrWidth || rc.right < nLastWidth )
						{
							x += nCntCopyWord;
						}
						else
						{
							// rc.left점을 기준으로 카피할 영역이 걸린 경우.
							if ( nLastWidth < rc.left && rc.left <= nCurrWidth )
							{
								x += (rc.left-nLastWidth);
								for ( INT nCheck = 0; nCheck < nCurrWidth-rc.left; nCheck++ )
								{
									wPixel = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueSrc = (BYTE)((FLOAT)bBlueSrc + ((FLOAT)bBlueSrc*rBlueRate*fBright));
									bGreenSrc = (BYTE)((FLOAT)bGreenSrc + ((FLOAT)bGreenSrc*rGreenRate*fBright));
									bRedSrc = (BYTE)((FLOAT)bRedSrc + ((FLOAT)bRedSrc*bRedRate*fBright));

									if ( bBlueSrc > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueSrc = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenSrc > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenSrc = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedSrc > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedSrc = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (rc.left+nX+nCheck)] = ((bBlueSrc <<m_stBitsMaskInfo.bBShift) |
																									   (bGreenSrc<<m_stBitsMaskInfo.bGShift) |
																									   (bRedSrc  <<m_stBitsMaskInfo.bRShift));			
								}
								x += (nCurrWidth-rc.left);
							}
							// rc.right점을 기준으로 카피할 영역이 걸린 경우.
							else if ( nLastWidth <= rc.right && rc.right < nCurrWidth )
							{
								for ( INT nCheck = 0; nCheck < rc.right-nLastWidth; nCheck++ )
								{
									wPixel = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueSrc = (BYTE)((FLOAT)bBlueSrc + ((FLOAT)bBlueSrc*rBlueRate*fBright));
									bGreenSrc = (BYTE)((FLOAT)bGreenSrc + ((FLOAT)bGreenSrc*rGreenRate*fBright));
									bRedSrc = (BYTE)((FLOAT)bRedSrc + ((FLOAT)bRedSrc*bRedRate*fBright));

									if ( bBlueSrc > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueSrc = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenSrc > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenSrc = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedSrc > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedSrc = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)] = ((bBlueSrc <<m_stBitsMaskInfo.bRShift) |
																									      (bGreenSrc<<m_stBitsMaskInfo.bGShift) |
																										  (bRedSrc  <<m_stBitsMaskInfo.bBShift));			
								}
								x += nCntCopyWord;
							}
							else
							{
								for ( INT nCheck = 0; nCheck < nCntCopyWord; nCheck++ )
								{
									wPixel = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueSrc = (BYTE)((FLOAT)bBlueSrc + ((FLOAT)bBlueSrc*rBlueRate*fBright));
									bGreenSrc = (BYTE)((FLOAT)bGreenSrc + ((FLOAT)bGreenSrc*rGreenRate*fBright));
									bRedSrc = (BYTE)((FLOAT)bRedSrc + ((FLOAT)bRedSrc*bRedRate*fBright));

									if ( bBlueSrc > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueSrc = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenSrc > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenSrc = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedSrc > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedSrc = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)] = ((bBlueSrc <<m_stBitsMaskInfo.bBShift) |
																									      (bGreenSrc<<m_stBitsMaskInfo.bGShift) |
																										  (bRedSrc  <<m_stBitsMaskInfo.bRShift));			
								}
								x += nCntCopyWord;
							}
						}
					}
					else if ( pwSrc[x] == 0xC2 || pwSrc[x] == 0xC3 )
					{
						wDyingKind = pwSrc[x];
						switch ( wDyingKind )
						{
						case 0xC2: 
							wChooseColor = wChooseColor1;
							break;
						case 0xC3: 
							wChooseColor = wChooseColor2;
							break;
						}
						x++;
						nCntCopyWord = pwSrc[x];
						x++;

						nLastWidth = nCurrWidth;
						nCurrWidth += nCntCopyWord;

						if ( rc.left > nCurrWidth || rc.right < nLastWidth )
						{
							x += nCntCopyWord;
						}
						else
						{
							// rc.left점을 기준으로 카피할 영역이 걸린 경우.
							if ( nLastWidth < rc.left && rc.left <= nCurrWidth )
							{
								x += (rc.left-nLastWidth);
								for ( INT nCheck = 0; nCheck < nCurrWidth-rc.left; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									bBlueSrc = bBlueWantedColor;
									bGreenSrc = bGreenWantedColor;
									bRedSrc = bRedWantedColor;

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueSrc = (BYTE)((FLOAT)bBlueSrc + ((FLOAT)bBlueSrc*rBlueRate*fBright));
									bGreenSrc = (BYTE)((FLOAT)bGreenSrc + ((FLOAT)bGreenSrc*rGreenRate*fBright));
									bRedSrc = (BYTE)((FLOAT)bRedSrc + ((FLOAT)bRedSrc*bRedRate*fBright));

									if ( bBlueSrc > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueSrc = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenSrc > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenSrc = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedSrc > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedSrc = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (rc.left+nX+nCheck)] = ((bBlueSrc <<m_stBitsMaskInfo.bBShift) |
																									   (bGreenSrc<<m_stBitsMaskInfo.bGShift) |
																									   (bRedSrc  <<m_stBitsMaskInfo.bRShift));			
								}
								x += (nCurrWidth-rc.left);
							}
							// rc.right점을 기준으로 카피할 영역이 걸린 경우.
							else if ( nLastWidth <= rc.right && rc.right < nCurrWidth )
							{
								for ( INT nCheck = 0; nCheck < rc.right-nLastWidth; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									bBlueSrc = bBlueWantedColor;
									bGreenSrc = bGreenWantedColor;
									bRedSrc = bRedWantedColor;

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueSrc = (BYTE)((FLOAT)bBlueSrc + ((FLOAT)bBlueSrc*rBlueRate*fBright));
									bGreenSrc = (BYTE)((FLOAT)bGreenSrc + ((FLOAT)bGreenSrc*rGreenRate*fBright ));
									bRedSrc = (BYTE)((FLOAT)bRedSrc + ((FLOAT)bRedSrc*bRedRate*fBright));

									if ( bBlueSrc > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueSrc = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenSrc > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenSrc = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedSrc > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedSrc = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)] = ((bBlueSrc <<m_stBitsMaskInfo.bRShift) |
																									      (bGreenSrc<<m_stBitsMaskInfo.bGShift) |
																										  (bRedSrc  <<m_stBitsMaskInfo.bBShift));			
								}
								x += nCntCopyWord;
							}
							else
							{
								for ( INT nCheck = 0; nCheck < nCntCopyWord; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									bBlueSrc = bBlueWantedColor;
									bGreenSrc = bGreenWantedColor;
									bRedSrc = bRedWantedColor;

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueSrc = (BYTE)((FLOAT)bBlueSrc + ((FLOAT)bBlueSrc*rBlueRate*fBright));
									bGreenSrc = (BYTE)((FLOAT)bGreenSrc + ((FLOAT)bGreenSrc*rGreenRate*fBright));
									bRedSrc = (BYTE)((FLOAT)bRedSrc + ((FLOAT)bRedSrc*bRedRate*fBright));

									if ( bBlueSrc > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueSrc = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenSrc > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenSrc = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedSrc > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedSrc = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)] = ((bBlueSrc <<m_stBitsMaskInfo.bBShift) |
																									      (bGreenSrc<<m_stBitsMaskInfo.bGShift) |
																										  (bRedSrc  <<m_stBitsMaskInfo.bRShift));			
								}
							x += nCntCopyWord;
							}
						}
					}	
				}
				// 라인의 끝을 다음 라인의 시작으로 옮겨준다.
				nWidthEnd++;

				nWidthStart	= nWidthEnd;
				nCurrWidth = 0;
			}
		}

		m_pddsBackBuffer->Unlock(NULL);
		return TRUE;
	}
	return FALSE;
}



BOOL CWHDXGraphicWindow::DrawWithImageForCompClipRgnColor(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, WORD wClipWidth, WORD wClipHeight, WORD wColor, BOOL bFocused, BOOL bBlend)
{
	RECT			rc;
	DDSURFACEDESC2	ddsd;

	INT	nWidth		= nXSize;
	INT	nHeight		= nYSize;
	INT	nXOffset	= 0;
	INT	nYOffset	= 0;
	INT	nStartX		= 0;
	INT	nStartY		= 0;
	INT	nEndX		= wClipWidth -1;
	INT	nEndY		= wClipHeight-1;

	if ( m_pddsBackBuffer != NULL )
	{
		if (nX < nStartX )
		{ 
			nXOffset	= nStartX - nX;
			nWidth		= nXSize - nXOffset;
		}
		if ( (nX+nXSize-1) > nEndX )
			nWidth		= nEndX - nX - nXOffset + 1;		
		if ( nY < nStartY )	
		{ 
			nYOffset	= nStartY - nY;
			nHeight		= nYSize - nYOffset;
		}		
		if ( (nY+nYSize-1) > nEndY )
			nHeight	= nEndY - nY - nYOffset + 1;

		if ( (nWidth > 0) && (nHeight > 0) )
		{
			rc.left		= nXOffset;
			rc.right	= nXOffset+nWidth;
			rc.top		= nYOffset;
			rc.bottom	= nYOffset+nHeight;

			ddsd.dwSize	= sizeof(DDSURFACEDESC2);
			ddsd.lpSurface = NULL;
			
			m_pddsBackBuffer->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
			if ( !ddsd.lpSurface )			return FALSE;
			WORD* pwdDst;
			pwdDst = (WORD*)ddsd.lpSurface;

			INT nWidthStart	= 0;
			INT nWidthEnd	= 0;
			INT nCurrWidth  = 0;
			INT nCntCopyWord = 0;
			INT nYCnt =0;
			INT nLastWidth = 0;

			BYTE  bRedStateColor,  bGreenStateColor,  bBlueStateColor;
			BYTE  bRedSrc, bGreenSrc, bBlueSrc;
			BYTE  bBlueDst, bGreenDst, bRedDst;
			WORD  wPixel;
			FLOAT rBlueRate, rGreenRate, rRedRate;
			FLOAT rbLightRate = 0.0f;

			// y축 클리핑.
  			for ( nYCnt=0 ; nYCnt < rc.top ; nYCnt++ )
			{
				nWidthEnd += pwSrc[nWidthStart];
				nWidthStart++;
				nWidthEnd++;
				nWidthStart	= nWidthEnd;
			}

			// y축에 대해서 실제로 루핑시킬 Count만을 정한다.
			for ( nYCnt=rc.top ; nYCnt < rc.bottom ; nYCnt++ )
			{
				// 한라인의 길이를 얻어온다.(단위는 워드)
				nWidthEnd += pwSrc[nWidthStart];
				nWidthStart++;

				// 라인의 길이만큼 화면에 뿌려준다.
				for ( INT x = nWidthStart; x < nWidthEnd ; )
				{
					if ( pwSrc[x] == 0xC0 )
					{
						x++;
						nCntCopyWord = pwSrc[x];
						x++;
						nCurrWidth += nCntCopyWord;
					}
					else if ( pwSrc[x] == 0xC1 || pwSrc[x] == 0xC2 || pwSrc[x] == 0xC3 )
					{
						x++;
						nCntCopyWord = pwSrc[x];
						x++;

						nLastWidth = nCurrWidth;
						nCurrWidth += nCntCopyWord;

						if ( rc.left > nCurrWidth || rc.right < nLastWidth )
						{
							x += nCntCopyWord;
						}
						else
						{
							// rc.left점을 기준으로 카피할 영역이 걸린 경우.
							if ( nLastWidth < rc.left && rc.left <= nCurrWidth )
							{
								x += (rc.left-nLastWidth);
								for ( INT nCheck = 0; nCheck < nCurrWidth-rc.left; nCheck++ )
								{
									wPixel = wColor;
									bBlueStateColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenStateColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedStateColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									// Focus여부.
									if ( bFocused )			rbLightRate = 0.6f;
									else					rbLightRate = 0.0f;

									if ( !wColor )	
									{
										BYTE bTemp = (BYTE)(bBlueSrc+(bGreenSrc>>1)+bRedSrc)/3;
										bTemp = bTemp + (BYTE)(bTemp*rbLightRate);
										if ( bTemp > 31 )	bTemp = 31;
										bBlueStateColor = bRedStateColor = bTemp;
										bGreenStateColor = bTemp<<1;
									}
									else
									{
										rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
										rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
										rRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

										bBlueStateColor  = ( (BYTE)((FLOAT)bBlueStateColor *rBlueRate ) + (BYTE)((FLOAT)bBlueStateColor *rBlueRate *rbLightRate) );
										bGreenStateColor = ( (BYTE)((FLOAT)bGreenStateColor*rGreenRate) + (BYTE)((FLOAT)bGreenStateColor*rGreenRate*rbLightRate) );
										bRedStateColor   = ( (BYTE)((FLOAT)bRedStateColor  *rRedRate  ) + (BYTE)((FLOAT)bRedStateColor  *rRedRate  *rbLightRate) );					

										if ( bBlueStateColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
											bBlueStateColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
										if ( bGreenStateColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
											bGreenStateColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
										if ( bRedStateColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
											bRedStateColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);
									}

									if ( bBlend )
									{
										wPixel	   = pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)];
										bBlueDst   = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
										bGreenDst  = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
										bRedDst    = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

										bBlueDst   = (BYTE)((50*(bBlueDst -bBlueStateColor )+100*bBlueStateColor  ) / 100);
										bGreenDst  = (BYTE)((50*(bGreenDst-bGreenStateColor)+100*bGreenStateColor ) / 100);
										bRedDst    = (BYTE)((50*(bRedDst  -bRedStateColor  )+100*bRedStateColor   ) / 100);
									}
									else
									{
										bBlueDst   = bBlueStateColor;
										bGreenDst  = bGreenStateColor;
										bRedDst    = bRedStateColor;
									}

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (rc.left+nX+nCheck)] = ((bBlueDst <<m_stBitsMaskInfo.bBShift) |
																									   (bGreenDst<<m_stBitsMaskInfo.bGShift) |
																									   (bRedDst  <<m_stBitsMaskInfo.bRShift));			
								}
								x += (nCurrWidth-rc.left);
							}
							// rc.right점을 기준으로 카피할 영역이 걸린 경우.
							else if ( nLastWidth <= rc.right && rc.right < nCurrWidth )
							{
								for ( INT nCheck = 0; nCheck < rc.right-nLastWidth; nCheck++ )
								{
									wPixel = wColor;
									bBlueStateColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenStateColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedStateColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									// Focus여부.
									if ( bFocused )			rbLightRate = 0.6f;
									else					rbLightRate = 0.0f;

									if ( !wColor )	
									{
										BYTE bTemp = (BYTE)(bBlueSrc+(bGreenSrc>>1)+bRedSrc)/3;
										bTemp = bTemp + (BYTE)(bTemp*rbLightRate);
										if ( bTemp > 31 )	bTemp = 31;
										bBlueStateColor = bRedStateColor = bTemp;
										bGreenStateColor = bTemp<<1;
									}
									else
									{
										rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
										rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
										rRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

										bBlueStateColor  = ( (BYTE)((FLOAT)bBlueStateColor *rBlueRate ) + (BYTE)((FLOAT)bBlueStateColor *rBlueRate *rbLightRate) );
										bGreenStateColor = ( (BYTE)((FLOAT)bGreenStateColor*rGreenRate) + (BYTE)((FLOAT)bGreenStateColor*rGreenRate*rbLightRate) );
										bRedStateColor   = ( (BYTE)((FLOAT)bRedStateColor  *rRedRate  ) + (BYTE)((FLOAT)bRedStateColor  *rRedRate  *rbLightRate) );					

										if ( bBlueStateColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
											bBlueStateColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
										if ( bGreenStateColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
											bGreenStateColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
										if ( bRedStateColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
											bRedStateColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);
									}

									if ( bBlend )
									{
										wPixel	   = pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)];
										bBlueDst   = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
										bGreenDst  = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
										bRedDst    = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

										bBlueDst   = (BYTE)((50*(bBlueDst -bBlueStateColor )+100*bBlueStateColor  ) / 100);
										bGreenDst  = (BYTE)((50*(bGreenDst-bGreenStateColor)+100*bGreenStateColor ) / 100);
										bRedDst    = (BYTE)((50*(bRedDst  -bRedStateColor  )+100*bRedStateColor   ) / 100);
									}
									else
									{
										bBlueDst   = bBlueStateColor;
										bGreenDst  = bGreenStateColor;
										bRedDst    = bRedStateColor;
									}

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)] = ((bBlueDst <<m_stBitsMaskInfo.bRShift) |
																									      (bGreenDst<<m_stBitsMaskInfo.bGShift) |
																										  (bRedDst  <<m_stBitsMaskInfo.bBShift));			
								}
								x += nCntCopyWord;
							}
							else
							{
								for ( INT nCheck = 0; nCheck < nCntCopyWord; nCheck++ )
								{
									wPixel = wColor;
									bBlueStateColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenStateColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedStateColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									// Focus여부.
									if ( bFocused )			rbLightRate = 0.6f;
									else					rbLightRate = 0.0f;

									if ( !wColor )	
									{
										BYTE bTemp = (BYTE)(bBlueSrc+(bGreenSrc>>1)+bRedSrc)/3;
										bTemp = bTemp + (BYTE)(bTemp*rbLightRate);
										if ( bTemp > 31 )	bTemp = 31;
										bBlueStateColor = bRedStateColor = bTemp;
										bGreenStateColor = bTemp<<1;
									}
									else
									{
										rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
										rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
										rRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

										bBlueStateColor  = ( (BYTE)((FLOAT)bBlueStateColor *rBlueRate ) + (BYTE)((FLOAT)bBlueStateColor *rBlueRate *rbLightRate) );
										bGreenStateColor = ( (BYTE)((FLOAT)bGreenStateColor*rGreenRate) + (BYTE)((FLOAT)bGreenStateColor*rGreenRate*rbLightRate) );
										bRedStateColor   = ( (BYTE)((FLOAT)bRedStateColor  *rRedRate  ) + (BYTE)((FLOAT)bRedStateColor  *rRedRate  *rbLightRate) );					

										if ( bBlueStateColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
											bBlueStateColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
										if ( bGreenStateColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
											bGreenStateColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
										if ( bRedStateColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
											bRedStateColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);
									}

									if ( bBlend )
									{
										wPixel	   = pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)];
										bBlueDst   = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
										bGreenDst  = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
										bRedDst    = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

										bBlueDst   = (BYTE)((50*(bBlueDst -bBlueStateColor )+100*bBlueStateColor  ) / 100);
										bGreenDst  = (BYTE)((50*(bGreenDst-bGreenStateColor)+100*bGreenStateColor ) / 100);
										bRedDst    = (BYTE)((50*(bRedDst  -bRedStateColor  )+100*bRedStateColor   ) / 100);
									}
									else
									{
										bBlueDst   = bBlueStateColor;
										bGreenDst  = bGreenStateColor;
										bRedDst    = bRedStateColor;
									}

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)] = ((bBlueDst <<m_stBitsMaskInfo.bBShift) |
																									      (bGreenDst<<m_stBitsMaskInfo.bGShift) |
																										  (bRedDst  <<m_stBitsMaskInfo.bRShift));			
								}
								x += nCntCopyWord;
							}
						}
					}
				}
				// 라인의 끝을 다음 라인의 시작으로 옮겨준다.
				nWidthEnd++;

				nWidthStart	= nWidthEnd;
				nCurrWidth = 0;
			}
		}

		m_pddsBackBuffer->Unlock(NULL);
		return TRUE;
	}
	return FALSE;
}



BOOL CWHDXGraphicWindow::DrawWithImageForCompClipRgnColor(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, WORD wClipWidth, WORD wClipHeight,WORD wColor , WORD wChooseColor1, WORD wChooseColor2)
{
	RECT			rc;
	DDSURFACEDESC2	ddsd;

	INT	nWidth		= nXSize;
	INT	nHeight		= nYSize;
	INT	nXOffset	= 0;
	INT	nYOffset	= 0;
	INT	nStartX		= 0;
	INT	nStartY		= 0;
	INT	nEndX		= wClipWidth -1;
	INT	nEndY		= wClipHeight-1;

	INT FiveMiddle	= 0xf;
	INT SixMiddle	= 0x1f;

	BYTE bsBlueColor,bsGreenColor,bsRedColor;
	BYTE bdBlueColor,bdGreenColor,bdRedColor;

	bdBlueColor  = (BYTE)((wColor & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
	bdGreenColor = (BYTE)((wColor & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
	bdRedColor   = (BYTE)((wColor & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

	if ( m_pddsBackBuffer != NULL )
	{
		if (nX < nStartX )
		{ 
			nXOffset	= nStartX - nX;
			nWidth		= nXSize - nXOffset;
		}
		if ( (nX+nXSize-1) > nEndX )
			nWidth		= nEndX - nX - nXOffset + 1;		
		if ( nY < nStartY )	
		{ 
			nYOffset	= nStartY - nY;
			nHeight		= nYSize - nYOffset;
		}		
		if ( (nY+nYSize-1) > nEndY )
			nHeight	= nEndY - nY - nYOffset + 1;

		if ( (nWidth > 0) && (nHeight > 0) )
		{
			rc.left		= nXOffset;
			rc.right	= nXOffset+nWidth;
			rc.top		= nYOffset;
			rc.bottom	= nYOffset+nHeight;

			ddsd.dwSize	= sizeof(DDSURFACEDESC2);
			ddsd.lpSurface = NULL;
			
			m_pddsBackBuffer->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
			if ( !ddsd.lpSurface )			return FALSE;
			WORD* pwdDst;
			pwdDst = (WORD*)ddsd.lpSurface;

			INT nWidthStart	= 0;
			INT nWidthEnd	= 0;
			INT nCurrWidth  = 0;
			INT nCntCopyWord = 0;
			INT nYCnt =0;
			INT nLastWidth = 0;

			FLOAT rBlueRate, rGreenRate, bRedRate;
			BYTE  bRedSrc, bGreenSrc, bBlueSrc;
			BYTE  bRedWantedColor, bGreenWantedColor, bBlueWantedColor;
			WORD  wPixel;

			// y축 클리핑.
			for ( nYCnt=0 ; nYCnt < rc.top ; nYCnt++ )
			{
				nWidthEnd += pwSrc[nWidthStart];
				nWidthStart++;
				nWidthEnd++;
				nWidthStart	= nWidthEnd;
			}

			// y축에 대해서 실제로 루핑시킬 Count만을 정한다.
			for ( nYCnt=rc.top ; nYCnt < rc.bottom ; nYCnt++ )
			{
				// 한라인의 길이를 얻어온다.(단위는 워드)
				nWidthEnd += pwSrc[nWidthStart];
				nWidthStart++;

				// 라인의 길이만큼 화면에 뿌려준다.
				for ( INT x = nWidthStart; x < nWidthEnd ; )
				{
					if ( pwSrc[x] == 0xC0 )
					{
						x++;
						nCntCopyWord = pwSrc[x];
						x++;
						nCurrWidth += nCntCopyWord;
					}
					else if ( pwSrc[x] == 0xC1 )
					{
						x++;
						nCntCopyWord = pwSrc[x];	// 갯수
						x++;

						nLastWidth = nCurrWidth;
						nCurrWidth += nCntCopyWord;

						if ( rc.left > nCurrWidth || rc.right < nLastWidth )
						{
							x += nCntCopyWord;		// 얼토 당토 않은 경우
						}
						else
						{
							// rc.left점을 기준으로 카피할 영역이 걸린 경우.
							if ( nLastWidth < rc.left && rc.left <= nCurrWidth )
							{
								x += (rc.left-nLastWidth);

								for(WORD i = 0 ; i < (sizeof(WORD)*(nCurrWidth-rc.left)/2);i++)
								{
									bsBlueColor  = (BYTE)((pwSrc[x+i] & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bsGreenColor = (BYTE)((pwSrc[x+i] & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bsRedColor   = (BYTE)((pwSrc[x+i] & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									BYTE	temp;

									temp = (bsRedColor+(bsGreenColor>>1)+bsBlueColor)/3;

									bsBlueColor = temp>0 ? (temp&bdBlueColor) : 0;
									bsGreenColor = temp>0 ? (temp&(bdGreenColor<<1)) : 0;
									bsRedColor = temp>0 ? (temp&bdRedColor) :  0;

									if ( bsBlueColor >= 32 )  bsBlueColor = 31;
									if ( bsGreenColor >= 64 )  bsGreenColor = 63;
									if ( bsRedColor >= 32 )  bsRedColor = 31;

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (rc.left+nX) + i] = (bsBlueColor<<m_stBitsMaskInfo.bBShift) |  (bsGreenColor<<m_stBitsMaskInfo.bGShift) | (bsRedColor<<m_stBitsMaskInfo.bRShift);
								}

								x += (nCurrWidth-rc.left);
							}
							// rc.right점을 기준으로 카피할 영역이 걸린 경우.
							else if ( nLastWidth <= rc.right && rc.right < nCurrWidth )
							{
								for(WORD i = 0 ; i < (sizeof(WORD)*(rc.right-nLastWidth));i++)
								{
									bsBlueColor  = (BYTE)((pwSrc[x+i] & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bsGreenColor = (BYTE)((pwSrc[x+i] & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bsRedColor   = (BYTE)((pwSrc[x+i] & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);
									
									BYTE	temp;

									temp = (bsRedColor+(bsGreenColor>>1)+bsBlueColor)/3;

									bsBlueColor = temp>0 ? (temp&bdBlueColor) : 0;
									bsGreenColor = temp>0 ? (temp&(bdGreenColor<<1)) : 0;
									bsRedColor = temp>0 ? (temp&bdRedColor) :  0;

									if ( bsBlueColor >= 32 )  bsBlueColor = 31;
									if ( bsGreenColor >= 64 )  bsGreenColor = 63;
									if ( bsRedColor >= 32 )  bsRedColor = 31;
									
									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX) + i] = (bsBlueColor<<m_stBitsMaskInfo.bBShift) |  (bsGreenColor<<m_stBitsMaskInfo.bGShift) | (bsRedColor<<m_stBitsMaskInfo.bRShift);
								}
//								memcpy(&pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX)], &pwSrc[x], sizeof(WORD)*(rc.right-nLastWidth));
								x += nCntCopyWord;
							}
							else
							{
								for(WORD i = 0 ; i < (sizeof(WORD)*nCntCopyWord/2);i++)
								{
									bsBlueColor  = (BYTE)((pwSrc[x+i] & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bsGreenColor = (BYTE)((pwSrc[x+i] & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bsRedColor   = (BYTE)((pwSrc[x+i] & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									BYTE	temp;

									temp = (bsRedColor+(bsGreenColor>>1)+bsBlueColor)/3;

									bsBlueColor = temp>0 ? (temp&bdBlueColor) : 0;
									bsGreenColor = temp>0 ? (temp&(bdGreenColor<<1)) : 0;
									bsRedColor = temp>0 ? (temp&bdRedColor) :  0;

									if ( bsBlueColor >= 32 )  bsBlueColor = 31;
									if ( bsGreenColor >= 64 )  bsGreenColor = 63;
									if ( bsRedColor >= 32 )  bsRedColor = 31;

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX) + i] = (bsBlueColor<<m_stBitsMaskInfo.bBShift) |  (bsGreenColor<<m_stBitsMaskInfo.bGShift) | (bsRedColor<<m_stBitsMaskInfo.bRShift);
								}
								x += nCntCopyWord;
							}
						}
					}
					else if ( pwSrc[x] == 0xC2 || pwSrc[x] == 0xC3 )
					{
						WORD wDyingKind, wChooseColor;
						wDyingKind = pwSrc[x];
						switch ( wDyingKind )
						{
						case 0xC2: 
							wChooseColor = wChooseColor1;
							break;
						case 0xC3: 
							wChooseColor = wChooseColor2;
							break;
						}
						x++;
						nCntCopyWord = pwSrc[x];
						x++;

						nLastWidth = nCurrWidth;
						nCurrWidth += nCntCopyWord;

						if ( rc.left > nCurrWidth || rc.right < nLastWidth )
						{
							x += nCntCopyWord;
						}
						else
						{
							// rc.left점을 기준으로 카피할 영역이 걸린 경우.
							if ( nLastWidth < rc.left && rc.left <= nCurrWidth )
							{
								x += (rc.left-nLastWidth);
								for ( INT nCheck = 0; nCheck < nCurrWidth-rc.left; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									BYTE	temp;

									temp = (bsRedColor+(bGreenWantedColor>>1)+bBlueWantedColor)/3;

									bBlueWantedColor = temp>0 ? (temp&bdBlueColor) : 0;
									bGreenWantedColor = temp>0 ? (temp&(bdGreenColor<<1)) : 0;
									bRedWantedColor = temp>0 ? (temp&bdRedColor) :  0;

									if ( bBlueWantedColor >= 32 )  bBlueWantedColor = 31;
									if ( bGreenWantedColor >= 64 )  bGreenWantedColor = 63;
									if ( bRedWantedColor >= 32 )  bRedWantedColor = 31;

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)] = ((bRedWantedColor  <<m_stBitsMaskInfo.bRShift) |
																										  (bGreenWantedColor<<m_stBitsMaskInfo.bGShift) |
																										  (bBlueWantedColor <<m_stBitsMaskInfo.bBShift));			
								}
								x += (nCurrWidth-rc.left);
							}
							// rc.right점을 기준으로 카피할 영역이 걸린 경우.
							else if ( nLastWidth <= rc.right && rc.right < nCurrWidth )
							{
								for ( INT nCheck = 0; nCheck < rc.right-nLastWidth; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									BYTE	temp;

									temp = (bsRedColor+(bGreenWantedColor>>1)+bBlueWantedColor)/3;

									bBlueWantedColor = temp>0 ? (temp&bdBlueColor) : 0;
									bGreenWantedColor = temp>0 ? (temp&(bdGreenColor<<1)) : 0;
									bRedWantedColor = temp>0 ? (temp&bdRedColor) :  0;

									if ( bBlueWantedColor >= 32 )  bBlueWantedColor = 31;
									if ( bGreenWantedColor >= 64 )  bGreenWantedColor = 63;
									if ( bRedWantedColor >= 32 )  bRedWantedColor = 31;

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)] = ((bRedWantedColor  <<m_stBitsMaskInfo.bRShift) |
																										  (bGreenWantedColor<<m_stBitsMaskInfo.bGShift) |
																										  (bBlueWantedColor <<m_stBitsMaskInfo.bBShift));			
								}
								x += nCntCopyWord;
							}
							else
							{
								for ( INT nCheck = 0; nCheck < nCntCopyWord; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									BYTE	temp;

									temp = (bsRedColor+(bGreenWantedColor>>1)+bBlueWantedColor)/3;

									bBlueWantedColor = temp>0 ? (temp&bdBlueColor) : 0;
									bGreenWantedColor = temp>0 ? (temp&(bdGreenColor<<1)) : 0;
									bRedWantedColor = temp>0 ? (temp&bdRedColor) :  0;

									if ( bBlueWantedColor >= 32 )  bBlueWantedColor = 31;
									if ( bGreenWantedColor >= 64 )  bGreenWantedColor = 63;
									if ( bRedWantedColor >= 32 )  bRedWantedColor = 31;

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)] = ((bRedWantedColor  <<m_stBitsMaskInfo.bRShift) |
																										  (bGreenWantedColor<<m_stBitsMaskInfo.bGShift) |
																									 	  (bBlueWantedColor <<m_stBitsMaskInfo.bBShift));			
								}
								x += nCntCopyWord;
							}
						}
					}	
				}
				// 라인의 끝을 다음 라인의 시작으로 옮겨준다.
				nWidthEnd++;

				nWidthStart	= nWidthEnd;
				nCurrWidth = 0;
			}
		}

		m_pddsBackBuffer->Unlock(NULL);
		return TRUE;
	}
	return FALSE;
}



BOOL CWHDXGraphicWindow::DrawWithImageForCompClipRgnGray(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc , WORD wClipWidth, WORD wClipHeight, WORD wChooseColor1, WORD wChooseColor2)
{
	RECT			rc;
	DDSURFACEDESC2	ddsd;

	INT	nWidth		= nXSize;
	INT	nHeight		= nYSize;
	INT	nXOffset	= 0;
	INT	nYOffset	= 0;
	INT	nStartX		= 0;
	INT	nStartY		= 0;
	INT	nEndX		= wClipWidth -1;
	INT	nEndY		= wClipHeight-1;

	INT FiveMiddle	= 0xf;
	INT SixMiddle	= 0x1f;

	BYTE bsBlueColor,bsGreenColor,bsRedColor;
	BYTE bdBlueColor,bdGreenColor,bdRedColor;

	bdBlueColor  = 0x0f;
	bdGreenColor = 0x1f;
	bdRedColor   = 0x0f;

	if ( m_pddsBackBuffer != NULL )
	{
		if (nX < nStartX )
		{ 
			nXOffset	= nStartX - nX;
			nWidth		= nXSize - nXOffset;
		}
		if ( (nX+nXSize-1) > nEndX )
			nWidth		= nEndX - nX - nXOffset + 1;		
		if ( nY < nStartY )	
		{ 
			nYOffset	= nStartY - nY;
			nHeight		= nYSize - nYOffset;
		}		
		if ( (nY+nYSize-1) > nEndY )
			nHeight	= nEndY - nY - nYOffset + 1;

		if ( (nWidth > 0) && (nHeight > 0) )
		{
			rc.left		= nXOffset;
			rc.right	= nXOffset+nWidth;
			rc.top		= nYOffset;
			rc.bottom	= nYOffset+nHeight;

			ddsd.dwSize	= sizeof(DDSURFACEDESC2);
			ddsd.lpSurface = NULL;
			
			m_pddsBackBuffer->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
			if ( !ddsd.lpSurface )			return FALSE;
			WORD* pwdDst;
			pwdDst = (WORD*)ddsd.lpSurface;

			INT nWidthStart	= 0;
			INT nWidthEnd	= 0;
			INT nCurrWidth  = 0;
			INT nCntCopyWord = 0;
			INT nYCnt =0;
			INT nLastWidth = 0;

			FLOAT rBlueRate, rGreenRate, bRedRate;
			BYTE  bRedSrc, bGreenSrc, bBlueSrc;
			BYTE  bRedWantedColor, bGreenWantedColor, bBlueWantedColor;
			WORD  wPixel;

			// y축 클리핑.
			for ( nYCnt=0 ; nYCnt < rc.top ; nYCnt++ )
			{
				nWidthEnd += pwSrc[nWidthStart];
				nWidthStart++;
				nWidthEnd++;
				nWidthStart	= nWidthEnd;
			}

			// y축에 대해서 실제로 루핑시킬 Count만을 정한다.
			for ( nYCnt=rc.top ; nYCnt < rc.bottom ; nYCnt++ )
			{
				// 한라인의 길이를 얻어온다.(단위는 워드)
				nWidthEnd += pwSrc[nWidthStart];
				nWidthStart++;

				// 라인의 길이만큼 화면에 뿌려준다.
				for ( INT x = nWidthStart; x < nWidthEnd ; )
				{
					if ( pwSrc[x] == 0xC0 )
					{
						x++;
						nCntCopyWord = pwSrc[x];
						x++;
						nCurrWidth += nCntCopyWord;
					}
					else if ( pwSrc[x] == 0xC1 )
					{
						x++;
						nCntCopyWord = pwSrc[x];	// 갯수
						x++;

						nLastWidth = nCurrWidth;
						nCurrWidth += nCntCopyWord;

						if ( rc.left > nCurrWidth || rc.right < nLastWidth )
						{
							x += nCntCopyWord;		// 얼토 당토 않은 경우
						}
						else
						{
							// rc.left점을 기준으로 카피할 영역이 걸린 경우.
							if ( nLastWidth < rc.left && rc.left <= nCurrWidth )
							{
								x += (rc.left-nLastWidth);

								for(WORD i = 0 ; i < (sizeof(WORD)*(nCurrWidth-rc.left)/2);i++)
								{
									bsBlueColor  = (BYTE)((pwSrc[x+i] & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bsGreenColor = (BYTE)((pwSrc[x+i] & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bsRedColor   = (BYTE)((pwSrc[x+i] & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									// Draw Gray 									
									BYTE	temp;

									temp = (bsRedColor+bsGreenColor/2+bsBlueColor)/3;

									bsBlueColor = temp>0 ? (temp) : 0;
									bsGreenColor = temp>0 ? (temp<<1) : 0;
									bsRedColor = temp>0 ? (temp) :  0;									

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (rc.left+nX) + i] = (bsBlueColor<<m_stBitsMaskInfo.bBShift) |  (bsGreenColor<<m_stBitsMaskInfo.bGShift) | (bsRedColor<<m_stBitsMaskInfo.bRShift);
								}

								x += (nCurrWidth-rc.left);
							}
							// rc.right점을 기준으로 카피할 영역이 걸린 경우.
							else if ( nLastWidth <= rc.right && rc.right < nCurrWidth )
							{
								for(WORD i = 0 ; i < (sizeof(WORD)*(rc.right-nLastWidth));i++)
								{
									bsBlueColor  = (BYTE)((pwSrc[x+i] & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bsGreenColor = (BYTE)((pwSrc[x+i] & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bsRedColor   = (BYTE)((pwSrc[x+i] & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									// Draw Gray 									

									BYTE	temp;

									temp = (bsRedColor+bsGreenColor/2+bsBlueColor)/3;

									bsBlueColor = temp>0 ? (temp) : 0;
									bsGreenColor = temp>0 ? (temp<<1) : 0;
									bsRedColor = temp>0 ? (temp) :  0;

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX) + i] = (bsBlueColor<<m_stBitsMaskInfo.bBShift) |  (bsGreenColor<<m_stBitsMaskInfo.bGShift) | (bsRedColor<<m_stBitsMaskInfo.bRShift);
								}
//								memcpy(&pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX)], &pwSrc[x], sizeof(WORD)*(rc.right-nLastWidth));
								x += nCntCopyWord;
							}
							else
							{
								for(WORD i = 0 ; i < (sizeof(WORD)*nCntCopyWord/2);i++)
								{
									bsBlueColor  = (BYTE)((pwSrc[x+i] & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bsGreenColor = (BYTE)((pwSrc[x+i] & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bsRedColor   = (BYTE)((pwSrc[x+i] & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);
									// Draw Gray 
									BYTE	temp;

									temp = (bsRedColor+bsGreenColor/2+bsBlueColor)/3;

									bsBlueColor = temp>0 ? (temp) : 0;
									bsGreenColor = temp>0 ? (temp<<1) : 0;
									bsRedColor = temp>0 ? (temp) :  0;

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX) + i] = (bsBlueColor<<m_stBitsMaskInfo.bBShift) |  (bsGreenColor<<m_stBitsMaskInfo.bGShift) | (bsRedColor<<m_stBitsMaskInfo.bRShift);
								}
								x += nCntCopyWord;
							}
						}
					}
					else if ( pwSrc[x] == 0xC2 || pwSrc[x] == 0xC3 )
					{
						WORD wDyingKind, wChooseColor;
						wDyingKind = pwSrc[x];
						switch ( wDyingKind )
						{
						case 0xC2: 
							wChooseColor = wChooseColor1;
							break;
						case 0xC3: 
							wChooseColor = wChooseColor2;
							break;
						}
						x++;
						nCntCopyWord = pwSrc[x];
						x++;

						nLastWidth = nCurrWidth;
						nCurrWidth += nCntCopyWord;

						if ( rc.left > nCurrWidth || rc.right < nLastWidth )
						{
							x += nCntCopyWord;
						}
						else
						{
							// rc.left점을 기준으로 카피할 영역이 걸린 경우.
							if ( nLastWidth < rc.left && rc.left <= nCurrWidth )
							{
								x += (rc.left-nLastWidth);
								for ( INT nCheck = 0; nCheck < nCurrWidth-rc.left; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									// Draw Gray 
									BYTE	temp;
									temp = (bRedWantedColor+bGreenWantedColor/2+bBlueWantedColor)/3;

									bBlueWantedColor = temp>0 ? (temp) : 0;
									bGreenWantedColor = temp>0 ? (temp<<1) : 0;
									bRedWantedColor = temp>0 ? (temp) :  0;

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)] = ((bRedWantedColor  <<m_stBitsMaskInfo.bRShift) |
																										  (bGreenWantedColor<<m_stBitsMaskInfo.bGShift) |
																										  (bBlueWantedColor <<m_stBitsMaskInfo.bBShift));			
								}
								x += (nCurrWidth-rc.left);
							}
							// rc.right점을 기준으로 카피할 영역이 걸린 경우.
							else if ( nLastWidth <= rc.right && rc.right < nCurrWidth )
							{
								for ( INT nCheck = 0; nCheck < rc.right-nLastWidth; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

									// Draw Gray 
									BYTE	temp;
									temp = (bRedWantedColor+bGreenWantedColor/2+bBlueWantedColor)/3;

									bBlueWantedColor = temp>0 ? (temp) : 0;
									bGreenWantedColor = temp>0 ? (temp<<1) : 0;
									bRedWantedColor = temp>0 ? (temp) :  0;

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)] = ((bRedWantedColor  <<m_stBitsMaskInfo.bRShift) |
																										  (bGreenWantedColor<<m_stBitsMaskInfo.bGShift) |
																										  (bBlueWantedColor <<m_stBitsMaskInfo.bBShift));			
								}
								x += nCntCopyWord;
							}
							else
							{
								for ( INT nCheck = 0; nCheck < nCntCopyWord; nCheck++ )
								{
									wPixel	  = wChooseColor;
									bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									wPixel	  = pwSrc[x+nCheck];
									bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
									bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
									bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

									rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
									rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
									bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

									bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
									bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
									bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

									if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
										bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
									if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
										bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
									if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
										bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);
									
									// Draw Gray 
									BYTE	temp;
									temp = (bRedWantedColor+bGreenWantedColor/2+bBlueWantedColor)/3;

									bBlueWantedColor = temp>0 ? (temp) : 0;
									bGreenWantedColor = temp>0 ? (temp<<1) : 0;
									bRedWantedColor = temp>0 ? (temp) :  0;

									pwdDst[((nYCnt+nY) * (ddsd.lPitch >> 1)) + (nLastWidth+nX+nCheck)] = ((bRedWantedColor  <<m_stBitsMaskInfo.bRShift) |
																										  (bGreenWantedColor<<m_stBitsMaskInfo.bGShift) |
																									 	  (bBlueWantedColor <<m_stBitsMaskInfo.bBShift));			
								}
								x += nCntCopyWord;
							}
						}
					}	
				}
				// 라인의 끝을 다음 라인의 시작으로 옮겨준다.
				nWidthEnd++;

				nWidthStart	= nWidthEnd;
				nCurrWidth = 0;
			}
		}

		m_pddsBackBuffer->Unlock(NULL);
		return TRUE;
	}
	return FALSE;
}


VOID CWHDXGraphicWindow::DrawWithGrayBackBuffer()
{
	DDSURFACEDESC2	ddsd;
	RECT			rc = {0, 0, 800, 500};

	if ( m_pddsBackBuffer == NULL )			return;

	ddsd.dwSize	= sizeof(DDSURFACEDESC2);
	ddsd.lpSurface = NULL;
		
	m_pddsBackBuffer->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
	if ( !ddsd.lpSurface )		return;
	WORD *pwdSrc, *pwdDst;
	BYTE bsBlueColor, bsGreenColor, bsRedColor;
	pwdDst = (WORD*)ddsd.lpSurface;
	pwdSrc = (WORD*)ddsd.lpSurface;

	for ( INT y=rc.top ; y < rc.bottom ; y++ )
	{
		for ( INT x=rc.left ; x < rc.right ; x++ )
		{
			bsBlueColor  = (BYTE)((pwdSrc[y * (ddsd.lPitch >> 1) + x] & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
			bsGreenColor = (BYTE)((pwdSrc[y * (ddsd.lPitch >> 1) + x] & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
			bsRedColor   = (BYTE)((pwdSrc[y * (ddsd.lPitch >> 1) + x] & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

			// Draw Gray 									
			BYTE	temp;

			temp = (bsRedColor+bsGreenColor/2+bsBlueColor)/3;

			bsBlueColor  = temp>0 ? (temp)    : 0;
			bsGreenColor = temp>0 ? (temp<<1) : 0;
			bsRedColor   = temp>0 ? (temp)    : 0;									

			pwdDst[y * (ddsd.lPitch >> 1) + x] = (bsBlueColor<<m_stBitsMaskInfo.bBShift) |  (bsGreenColor<<m_stBitsMaskInfo.bGShift) | (bsRedColor<<m_stBitsMaskInfo.bRShift);
		}
	}

	m_pddsBackBuffer->Unlock(NULL);
}



/******************************************************************************************************************

	함수명 : CWHDXGraphicWindow::DrawWithImageForCompClipRgnToMem()

	작성자 : 
	작성일 : 

	목적   : 압축된 소스 이미지의 원하는 사이즈 압축을 풀은 상태로 전달받은 메모리(pwDst)에 카피한다.

	입력   : INT nStartX
	         INT nStartY
	         INT nWantedXSize
	         INT nWantedYSize
	         WORD* pwSrc
	         WORD wClipWidth
	         WORD wClipHeight
	         WORD wChooseColor1
	         WORD wChooseColor2
	출력   : BOOL

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CWHDXGraphicWindow::DrawWithImageForCompColorToMem(RECT rcWanted, WORD* pwSrc, WORD wColor, WORD* pwDst)
{
	if ( rcWanted.left < 0 || rcWanted.right < 0 || rcWanted.top < 0 || rcWanted.bottom < 0 ||
		 rcWanted.right - rcWanted.left < 0 || rcWanted.bottom - rcWanted.top < 0 || pwDst == NULL )
		 return FALSE;

	INT nWidthStart		= 0;
	INT nWidthEnd		= 0;
	INT nCurrWidth		= 0;
	INT nCntCopyWord	= 0;
	INT nYCnt			= 0;
	INT nLastWidth		= 0;

	// y축 클리핑.
  	for ( nYCnt=0 ; nYCnt < rcWanted.top ; nYCnt++ )
	{
 		nWidthEnd += pwSrc[nWidthStart];
		nWidthStart++;
		nWidthEnd++;
		nWidthStart	= nWidthEnd;
	}

	INT nNewCurrWidth = 0;

	FLOAT rBlueRate, rGreenRate, rRedRate;
	BYTE  bRedSrc, bGreenSrc, bBlueSrc;
	BYTE  bRedStateColor, bGreenStateColor, bBlueStateColor;
	WORD  wPixel;

	// y축에 대해서 실제로 루핑시킬 Count만을 정한다.
	for ( nYCnt=rcWanted.top ; nYCnt < rcWanted.bottom ; nYCnt++ )
	{
		// 한라인의 길이를 얻어온다.(단위는 워드)
		nWidthEnd += pwSrc[nWidthStart];
		nWidthStart++;

		// 라인의 길이만큼 화면에 뿌려준다.
		for ( INT x = nWidthStart; x < nWidthEnd ; )
		{
			if ( pwSrc[x] == 0xC0 )
			{
				x++;
				nCntCopyWord = pwSrc[x];
				x++;
				nLastWidth = nCurrWidth;
				nCurrWidth += nCntCopyWord;

				// rcWanted.left점을 기준으로 카피할 영역이 걸린 경우.
				if ( nLastWidth <= rcWanted.left && nCurrWidth >= rcWanted.right )
				{
					memset(&pwDst[((nYCnt-rcWanted.top) * (rcWanted.right-rcWanted.left))  + nNewCurrWidth], 0, sizeof(WORD)*(rcWanted.right-rcWanted.left));
					nNewCurrWidth += rcWanted.right-rcWanted.left;
				}
				else if ( nLastWidth < rcWanted.left && nCurrWidth > rcWanted.left )
				{
					memset(&pwDst[((nYCnt-rcWanted.top) * (rcWanted.right-rcWanted.left)) + nNewCurrWidth], 0, sizeof(WORD)*(nCurrWidth-rcWanted.left));
					nNewCurrWidth += nCurrWidth-rcWanted.left;
				}
				else if ( nLastWidth >= rcWanted.left && nCurrWidth <= rcWanted.right )
				{
					memset(&pwDst[((nYCnt-rcWanted.top) * (rcWanted.right-rcWanted.left))  + nNewCurrWidth], 0, sizeof(WORD)*(nCntCopyWord));
					nNewCurrWidth += nCntCopyWord;
				}
				// rcWanted.right점을 기준으로 카피할 영역이 걸린 경우.
				else if ( nLastWidth < rcWanted.right && nCurrWidth > rcWanted.right )
				{
					memset(&pwDst[((nYCnt-rcWanted.top) * (rcWanted.right-rcWanted.left)) + nNewCurrWidth], 0, sizeof(WORD)*(rcWanted.right-nLastWidth));
					nNewCurrWidth += rcWanted.right-nLastWidth;
				}
			}
			else if ( pwSrc[x] == 0xC1 || pwSrc[x] == 0xC2 || pwSrc[x] == 0xC3 )
			{
				x++;
				nCntCopyWord = pwSrc[x];
				x++;

				nLastWidth = nCurrWidth;
				nCurrWidth += nCntCopyWord;

				if ( rcWanted.left > nCurrWidth || rcWanted.right < nLastWidth )
				{
					x += nCntCopyWord;
				}
				else
				{
					// rcWanted.left점을 기준으로 카피할 영역이 걸린 경우.
					if ( nLastWidth <= rcWanted.left && nCurrWidth >= rcWanted.right )
					{
						x += (rcWanted.left-nLastWidth);
						for ( INT nCheck = 0; nCheck < rcWanted.right-rcWanted.left; nCheck++ )
						{
							wPixel = wColor;
							bBlueStateColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
							bGreenStateColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
							bRedStateColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

							wPixel = pwSrc[x+nCheck];
							bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
							bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
							bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

							if ( !wColor )	
							{
								BYTE bTemp = (BYTE)(bBlueSrc+(bGreenSrc>>1)+bRedSrc)/3;
								if ( bTemp > 31 )	bTemp = 31;
								bBlueStateColor = bRedStateColor = bTemp;
								bGreenStateColor = bTemp<<1;
							}
							else
							{
								rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
								rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
								rRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

								bBlueStateColor  = (BYTE)((FLOAT)bBlueStateColor *rBlueRate );
								bGreenStateColor = (BYTE)((FLOAT)bGreenStateColor*rGreenRate);
								bRedStateColor   = (BYTE)((FLOAT)bRedStateColor  *rRedRate  );

								if ( bBlueStateColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
									bBlueStateColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
								if ( bGreenStateColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
									bGreenStateColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
								if ( bRedStateColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
									bRedStateColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);
							}

							if ( !bRedStateColor && !bGreenStateColor && !bBlueStateColor )		bRedStateColor = bGreenStateColor = bBlueStateColor = 1;

							pwDst[((nYCnt-rcWanted.top) * (rcWanted.right-rcWanted.left)) + (nNewCurrWidth+nCheck)] = ((bRedStateColor  <<m_stBitsMaskInfo.bRShift) |
																													   (bGreenStateColor<<m_stBitsMaskInfo.bGShift) |
																													   (bBlueStateColor <<m_stBitsMaskInfo.bBShift));			
						}
						nNewCurrWidth += rcWanted.right-rcWanted.left;
						x += (nCurrWidth-rcWanted.left);
					}
					else if ( nLastWidth < rcWanted.left && nCurrWidth > rcWanted.left )
					{
						x += (rcWanted.left-nLastWidth);
						for ( INT nCheck = 0; nCheck < nCurrWidth-rcWanted.left; nCheck++ )
						{
							wPixel = wColor;
							bBlueStateColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
							bGreenStateColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
							bRedStateColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

							wPixel = pwSrc[x+nCheck];
							bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
							bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
							bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

							if ( !wColor )	
							{
								BYTE bTemp = (BYTE)(bBlueSrc+(bGreenSrc>>1)+bRedSrc)/3;
								if ( bTemp > 31 )	bTemp = 31;
								bBlueStateColor = bRedStateColor = bTemp;
								bGreenStateColor = bTemp<<1;
							}
							else
							{
								rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
								rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
								rRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

								bBlueStateColor  = (BYTE)((FLOAT)bBlueStateColor *rBlueRate );
								bGreenStateColor = (BYTE)((FLOAT)bGreenStateColor*rGreenRate);
								bRedStateColor   = (BYTE)((FLOAT)bRedStateColor  *rRedRate  );

								if ( bBlueStateColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
									bBlueStateColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
								if ( bGreenStateColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
									bGreenStateColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
								if ( bRedStateColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
									bRedStateColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);
							}

							if ( !bRedStateColor && !bGreenStateColor && !bBlueStateColor )		bRedStateColor = bGreenStateColor = bBlueStateColor = 1;

							pwDst[((nYCnt-rcWanted.top) * (rcWanted.right-rcWanted.left)) + (nNewCurrWidth+nCheck)] = ((bRedStateColor  <<m_stBitsMaskInfo.bRShift) |
																													   (bGreenStateColor<<m_stBitsMaskInfo.bGShift) |
																													   (bBlueStateColor <<m_stBitsMaskInfo.bBShift));										
						}
						nNewCurrWidth += nCurrWidth-rcWanted.left;
						x += (nCurrWidth-rcWanted.left);
					}
					else if ( nLastWidth >= rcWanted.left && nCurrWidth <= rcWanted.right )
					{
						for ( INT nCheck = 0; nCheck < nCntCopyWord; nCheck++ )
						{
							wPixel = wColor;
							bBlueStateColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
							bGreenStateColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
							bRedStateColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

							wPixel = pwSrc[x+nCheck];
							bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
							bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
							bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

							if ( !wColor )	
							{
								BYTE bTemp = (BYTE)(bBlueSrc+(bGreenSrc>>1)+bRedSrc)/3;
								if ( bTemp > 31 )	bTemp = 31;
								bBlueStateColor = bRedStateColor = bTemp;
								bGreenStateColor = bTemp<<1;
							}
							else
							{
								if ( nCheck == 17 )
									int b = 0;

								rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
								rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
								rRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

								bBlueStateColor  = (BYTE)((FLOAT)bBlueStateColor *rBlueRate );
								bGreenStateColor = (BYTE)((FLOAT)bGreenStateColor*rGreenRate);
								bRedStateColor   = (BYTE)((FLOAT)bRedStateColor  *rRedRate  );

								if ( bBlueStateColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
									bBlueStateColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
								if ( bGreenStateColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
									bGreenStateColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
								if ( bRedStateColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
									bRedStateColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);
							}

							if ( !bRedStateColor && !bGreenStateColor && !bBlueStateColor )		bRedStateColor = bGreenStateColor = bBlueStateColor = 1;

							pwDst[((nYCnt-rcWanted.top) * (rcWanted.right-rcWanted.left)) + (nNewCurrWidth+nCheck)] = ((bRedStateColor  <<m_stBitsMaskInfo.bRShift) |
																													   (bGreenStateColor<<m_stBitsMaskInfo.bGShift) |
																													   (bBlueStateColor <<m_stBitsMaskInfo.bBShift));			
						}
						nNewCurrWidth += nCntCopyWord;
						x += nCntCopyWord;
					}
					// rcWanted.right점을 기준으로 카피할 영역이 걸린 경우.
					else if ( nLastWidth < rcWanted.right && nCurrWidth > rcWanted.right )
					{
						for ( INT nCheck = 0; nCheck < rcWanted.right-nLastWidth; nCheck++ )
						{
							wPixel = wColor;
							bBlueStateColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
							bGreenStateColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
							bRedStateColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

							wPixel = pwSrc[x+nCheck];
							bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
							bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
							bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

							if ( !wColor )	
							{
								BYTE bTemp = (BYTE)(bBlueSrc+(bGreenSrc>>1)+bRedSrc)/3;
								if ( bTemp > 31 )	bTemp = 31;
								bBlueStateColor = bRedStateColor = bTemp;
								bGreenStateColor = bTemp<<1;
							}
							else
							{
								rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
								rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
								rRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

								bBlueStateColor  = (BYTE)((FLOAT)bBlueStateColor *rBlueRate );
								bGreenStateColor = (BYTE)((FLOAT)bGreenStateColor*rGreenRate);
								bRedStateColor   = (BYTE)((FLOAT)bRedStateColor  *rRedRate  );

								if ( bBlueStateColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
									bBlueStateColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
								if ( bGreenStateColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
									bGreenStateColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
								if ( bRedStateColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
									bRedStateColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);
							}

							if ( !bRedStateColor && !bGreenStateColor && !bBlueStateColor )		bRedStateColor = bGreenStateColor = bBlueStateColor = 1;

							pwDst[((nYCnt-rcWanted.top) * (rcWanted.right-rcWanted.left)) + (nNewCurrWidth+nCheck)] = ((bRedStateColor  <<m_stBitsMaskInfo.bRShift) |
																													   (bGreenStateColor<<m_stBitsMaskInfo.bGShift) |
																													   (bBlueStateColor <<m_stBitsMaskInfo.bBShift));			
						}
						nNewCurrWidth += rcWanted.right-nLastWidth;
						x += nCntCopyWord;
					}
					else
					{
						x += nCntCopyWord;
					}
				}
			}	
		}
		// 라인의 끝을 다음 라인의 시작으로 옮겨준다.
		nWidthEnd++;

		nWidthStart	= nWidthEnd;
		nCurrWidth = 0;
		nNewCurrWidth = 0;
	}
	return TRUE;
}




BOOL CWHDXGraphicWindow::DrawWithImageForCompToMem(RECT rcWanted, WORD* pwSrc, WORD wChooseColor1, WORD wChooseColor2, WORD* pwDst)
{
	if ( rcWanted.left < 0 || rcWanted.right < 0 || rcWanted.top < 0 || rcWanted.bottom < 0 ||
		 rcWanted.right - rcWanted.left < 0 || rcWanted.bottom - rcWanted.top < 0 || pwDst == NULL )
		 return FALSE;

	INT nWidthStart		= 0;
	INT nWidthEnd		= 0;
	INT nCurrWidth		= 0;
	INT nCntCopyWord	= 0;
	INT nYCnt			= 0;
	INT nLastWidth		= 0;

	// y축 클리핑.
  	for ( nYCnt=0 ; nYCnt < rcWanted.top ; nYCnt++ )
	{
 		nWidthEnd += pwSrc[nWidthStart];
		nWidthStart++;
		nWidthEnd++;
		nWidthStart	= nWidthEnd;
	}

	INT nNewCurrWidth = 0;

	FLOAT rBlueRate, rGreenRate, bRedRate;
	BYTE  bRedSrc, bGreenSrc, bBlueSrc;
	BYTE  bRedWantedColor, bGreenWantedColor, bBlueWantedColor;
	WORD  wPixel;

	// y축에 대해서 실제로 루핑시킬 Count만을 정한다.
	for ( nYCnt=rcWanted.top ; nYCnt < rcWanted.bottom ; nYCnt++ )
	{
		// 한라인의 길이를 얻어온다.(단위는 워드)
		nWidthEnd += pwSrc[nWidthStart];
		nWidthStart++;

		// 라인의 길이만큼 화면에 뿌려준다.
		for ( INT x = nWidthStart; x < nWidthEnd ; )
		{
			if ( pwSrc[x] == 0xC0 )
			{
				x++;
				nCntCopyWord = pwSrc[x];
				x++;
				nLastWidth = nCurrWidth;
				nCurrWidth += nCntCopyWord;

				// rcWanted.left점을 기준으로 카피할 영역이 걸린 경우.
				if ( nLastWidth <= rcWanted.left && nCurrWidth >= rcWanted.right )
				{
					memset(&pwDst[((nYCnt-rcWanted.top) * (rcWanted.right-rcWanted.left))  + nNewCurrWidth], 0, sizeof(WORD)*(rcWanted.right-rcWanted.left));
					nNewCurrWidth += rcWanted.right-rcWanted.left;
				}
				else if ( nLastWidth < rcWanted.left && nCurrWidth > rcWanted.left )
				{
					memset(&pwDst[((nYCnt-rcWanted.top) * (rcWanted.right-rcWanted.left)) + nNewCurrWidth], 0, sizeof(WORD)*(nCurrWidth-rcWanted.left));
					nNewCurrWidth += nCurrWidth-rcWanted.left;
				}
				else if ( nLastWidth >= rcWanted.left && nCurrWidth <= rcWanted.right )
				{
					memset(&pwDst[((nYCnt-rcWanted.top) * (rcWanted.right-rcWanted.left))  + nNewCurrWidth], 0, sizeof(WORD)*(nCntCopyWord));
					nNewCurrWidth += nCntCopyWord;
				}
				// rcWanted.right점을 기준으로 카피할 영역이 걸린 경우.
				else if ( nLastWidth < rcWanted.right && nCurrWidth > rcWanted.right )
				{
					memset(&pwDst[((nYCnt-rcWanted.top) * (rcWanted.right-rcWanted.left)) + nNewCurrWidth], 0, sizeof(WORD)*(rcWanted.right-nLastWidth));
					nNewCurrWidth += rcWanted.right-nLastWidth;
				}
			}
			else if ( pwSrc[x] == 0xC1 )
			{
				x++;
				nCntCopyWord = pwSrc[x];
				x++;

				nLastWidth = nCurrWidth;
				nCurrWidth += nCntCopyWord;

				if ( rcWanted.left > nCurrWidth || rcWanted.right < nLastWidth )
				{
					x += nCntCopyWord;
				}
				else
				{
					// rcWanted.left점을 기준으로 카피할 영역이 걸린 경우.
					if ( nLastWidth <= rcWanted.left && nCurrWidth >= rcWanted.right )
					{
						x += (rcWanted.left-nLastWidth);
						memcpy(&pwDst[((nYCnt-rcWanted.top) * (rcWanted.right-rcWanted.left)) + nNewCurrWidth], &pwSrc[x], sizeof(WORD)*(rcWanted.right-rcWanted.left));
						nNewCurrWidth += rcWanted.right-rcWanted.left;
						x += (nCurrWidth-rcWanted.left);
					}
					else if ( nLastWidth < rcWanted.left && nCurrWidth > rcWanted.left )
					{
						x += (rcWanted.left-nLastWidth);
						memcpy(&pwDst[((nYCnt-rcWanted.top) * (rcWanted.right-rcWanted.left)) + nNewCurrWidth], &pwSrc[x], sizeof(WORD)*(nCurrWidth-rcWanted.left));
						nNewCurrWidth += nCurrWidth-rcWanted.left;
						x += (nCurrWidth-rcWanted.left);
					}
					else if ( nLastWidth >= rcWanted.left && nCurrWidth <= rcWanted.right )
					{
						memcpy(&pwDst[((nYCnt-rcWanted.top) * (rcWanted.right-rcWanted.left)) + nNewCurrWidth], &pwSrc[x], sizeof(WORD)*nCntCopyWord);
						nNewCurrWidth += nCntCopyWord;
						x += nCntCopyWord;
					}
					// rcWanted.right점을 기준으로 카피할 영역이 걸린 경우.
					else if ( nLastWidth < rcWanted.right && nCurrWidth > rcWanted.right )
					{
						memcpy(&pwDst[((nYCnt-rcWanted.top) * (rcWanted.right-rcWanted.left)) + nNewCurrWidth], &pwSrc[x], sizeof(WORD)*(rcWanted.right-nLastWidth));
						nNewCurrWidth += rcWanted.right-nLastWidth;
						x += nCntCopyWord;
					}
					else
					{
						x += nCntCopyWord;
					}
				}
			}
			else if ( pwSrc[x] == 0xC2 || pwSrc[x] == 0xC3 )
			{
				WORD wDyingKind, wChooseColor;
				wDyingKind = pwSrc[x];
				switch ( wDyingKind )
				{
				case 0xC2: 
					wChooseColor = wChooseColor1;
					break;
				case 0xC3: 
					wChooseColor = wChooseColor2;
					break;
				}
				x++;
				nCntCopyWord = pwSrc[x];
				x++;

				nLastWidth = nCurrWidth;
				nCurrWidth += nCntCopyWord;

				if ( rcWanted.left > nCurrWidth || rcWanted.right < nLastWidth )
				{
					x += nCntCopyWord;
				}
				else
				{
					// rcWanted.left점을 기준으로 카피할 영역이 걸린 경우.
					if ( nLastWidth <= rcWanted.left && nCurrWidth >= rcWanted.right )
					{
						x += (rcWanted.left-nLastWidth);
						for ( INT nCheck = 0; nCheck < rcWanted.right-rcWanted.left; nCheck++ )
						{
							wPixel	  = wChooseColor;
							bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
							bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
							bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

							wPixel	  = pwSrc[x+nCheck];
							bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
							bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
							bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

							rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
							rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
							bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

							bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
							bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
							bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

							if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
								bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
							if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
								bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
							if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
								bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

							pwDst[((nYCnt-rcWanted.top) * (rcWanted.right-rcWanted.left)) + (nNewCurrWidth+nCheck)] = ((bRedWantedColor  <<m_stBitsMaskInfo.bRShift) |
																													   (bGreenWantedColor<<m_stBitsMaskInfo.bGShift) |
																													   (bBlueWantedColor <<m_stBitsMaskInfo.bBShift));			
						}
						nNewCurrWidth += rcWanted.right-rcWanted.left;
						x += (nCurrWidth-rcWanted.left);
					}
					else if ( nLastWidth < rcWanted.left && nCurrWidth > rcWanted.left )
					{
						x += (rcWanted.left-nLastWidth);
						for ( INT nCheck = 0; nCheck < nCurrWidth-rcWanted.left; nCheck++ )
						{
							wPixel	  = wChooseColor;
							bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
							bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
							bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

							wPixel	  = pwSrc[x+nCheck];
							bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
							bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
							bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

							rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
							rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
							bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

							bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
							bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
							bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

							if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
								bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
							if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
								bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
							if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
								bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

							pwDst[((nYCnt-rcWanted.top) * (rcWanted.right-rcWanted.left)) + (nNewCurrWidth+nCheck)] = ((bRedWantedColor  <<m_stBitsMaskInfo.bRShift) |
																													   (bGreenWantedColor<<m_stBitsMaskInfo.bGShift) |
																													   (bBlueWantedColor <<m_stBitsMaskInfo.bBShift));										
						}
						nNewCurrWidth += nCurrWidth-rcWanted.left;
						x += (nCurrWidth-rcWanted.left);
					}
					else if ( nLastWidth >= rcWanted.left && nCurrWidth <= rcWanted.right )
					{
						for ( INT nCheck = 0; nCheck < nCntCopyWord; nCheck++ )
						{
							wPixel	  = wChooseColor;
							bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
							bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
							bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

							wPixel	  = pwSrc[x+nCheck];
							bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
							bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
							bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

							rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
							rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
							bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

							bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
							bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
							bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

							if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
								bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
							if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
								bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
							if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
								bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

							pwDst[((nYCnt-rcWanted.top) * (rcWanted.right-rcWanted.left)) + (nNewCurrWidth+nCheck)] = ((bRedWantedColor  <<m_stBitsMaskInfo.bRShift) |
																													   (bGreenWantedColor<<m_stBitsMaskInfo.bGShift) |
																													   (bBlueWantedColor <<m_stBitsMaskInfo.bBShift));			
						}
						nNewCurrWidth += nCntCopyWord;
						x += nCntCopyWord;
					}
					// rcWanted.right점을 기준으로 카피할 영역이 걸린 경우.
					else if ( nLastWidth < rcWanted.right && nCurrWidth > rcWanted.right )
					{
						for ( INT nCheck = 0; nCheck < rcWanted.right-nLastWidth; nCheck++ )
						{
							wPixel	  = wChooseColor;
							bBlueWantedColor  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
							bGreenWantedColor = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
							bRedWantedColor   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

							wPixel	  = pwSrc[x+nCheck];
							bBlueSrc  = (BYTE)((wPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
							bGreenSrc = (BYTE)((wPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
							bRedSrc   = (BYTE)((wPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

							rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
							rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
							bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

							bBlueWantedColor = (BYTE)(((FLOAT)bBlueWantedColor*rBlueRate));
							bGreenWantedColor = (BYTE)(((FLOAT)bGreenWantedColor*rGreenRate));
							bRedWantedColor = (BYTE)(((FLOAT)bRedWantedColor*bRedRate));

							if ( bBlueWantedColor > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
								bBlueWantedColor = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
							if ( bGreenWantedColor > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
								bGreenWantedColor = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
							if ( bRedWantedColor > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
								bRedWantedColor = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);


							pwDst[((nYCnt-rcWanted.top) * (rcWanted.right-rcWanted.left)) + (nNewCurrWidth+nCheck)] = ((bRedWantedColor  <<m_stBitsMaskInfo.bRShift) |
																													   (bGreenWantedColor<<m_stBitsMaskInfo.bGShift) |
																													   (bBlueWantedColor <<m_stBitsMaskInfo.bBShift));			
						}
						nNewCurrWidth += rcWanted.right-nLastWidth;
						x += nCntCopyWord;
					}
					else
					{
						x += nCntCopyWord;
					}
				}
			}	
		}
		// 라인의 끝을 다음 라인의 시작으로 옮겨준다.
		nWidthEnd++;

		nWidthStart	= nWidthEnd;
		nCurrWidth = 0;
		nNewCurrWidth = 0;
	}
	return TRUE;
}




/******************************************************************************************************************

	함수명 : CWHDXGraphicWindow::DrawWithABlendForIntersectCompData()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : INT nX					: 백버퍼상의 그림시작위치.
	         INT nY					
	         INT nSrcXSize			: 원본그림의 사이즈.
	         INT nSrcYSize
	         WORD* pwSrc			: 원본그림의 데이타.
	         INT nDstXSize			: 타겟그림의 사이즈.
	         INT nDstYSize
	         WORD* pwDst			: 타겟그림의 데이타.
	         WORD wSrcChooseColor1	: 원본그림의 염색 이미지1 컬러.
	         WORD wSrcChooseColor2
	         WORD wDstChooseColor1	: 타겟그림의 염색 이미지1 컬러.
	         WORD wDstChooseColor2
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CWHDXGraphicWindow::DrawWithABlendForIntersectCompData(INT nSrcX, INT nSrcY, 
															INT nSrcXSize, INT nSrcYSize, WORD* pwSrc,
															INT nDstX, INT nDstY,
															INT nDstXSize, INT nDstYSize, WORD* pwDst,
															WORD wClipWidth, WORD wClipHeight,
															BYTE bOpa, BOOL bFocused,
															WORD wSrcChooseColor1, WORD wSrcChooseColor2, 
															WORD wDstChooseColor1, WORD wDstChooseColor2,
															WORD wSrcColor, WORD wDstColor)
{
	DDSURFACEDESC2	ddsd;
	RECT rcSrc, rcDst;
	RECT rcSrcIntersect = { 0, 0, 0, 0 };
	RECT rcDstIntersect = { 0, 0, 0, 0 };
	WORD* pwdDst = NULL;

	INT	nSrcWidth	= nSrcXSize;
	INT	nSrcHeight	= nSrcYSize;
	INT	nXSrcOffset	= 0;
	INT	nYSrcOffset	= 0;

	INT	nDstWidth	= nDstXSize;
	INT	nDstHeight	= nDstYSize;
	INT	nXDstOffset	= 0;
	INT	nYDstOffset	= 0;

	INT	nEndX		= wClipWidth -1;
	INT	nEndY		= wClipHeight-1;
	INT	nStartX		= 0;
	INT	nStartY		= 0;

	WORD* pwDstSave = NULL;
	WORD* pwSrcSave = NULL;

	if ( m_pddsBackBuffer != NULL )
	{
		// Source의 화면클리핑한 영역.
		if ( nSrcX < nStartX )
		{ 
			nXSrcOffset	= nStartX - nSrcX;
			nSrcWidth	= nSrcXSize - nXSrcOffset;
		}
		if ( (nSrcX+nSrcXSize-1) > nEndX )
			nSrcWidth	= nEndX - nSrcX - nXSrcOffset + 1;		
		if ( nSrcY < nStartY )	
		{ 
			nYSrcOffset	= nStartY - nSrcY;
			nSrcHeight	= nSrcYSize - nYSrcOffset;
		}		
		if ( (nSrcY+nSrcYSize-1) > nEndY )
			nSrcHeight	= nEndY - nSrcY - nYSrcOffset + 1;

		if ( (nSrcWidth > 0) && (nSrcHeight > 0) )
		{
			// Source영역의 설정.
			rcSrc.left		= nXSrcOffset;
			rcSrc.right		= nXSrcOffset+nSrcWidth;
			rcSrc.top		= nYSrcOffset;
			rcSrc.bottom	= nYSrcOffset+nSrcHeight;

			// Destination의 화면클리핑한 영역.
			if ( nDstX < nStartX )
			{ 
				nXDstOffset	= nStartX - nDstX;
				nDstWidth	= nDstXSize - nXDstOffset;
			}
			if ( (nDstX+nDstXSize-1) > nEndX )
				nDstWidth	= nEndX - nDstX - nXDstOffset + 1;		
			if ( nDstY < nStartY )	
			{ 
				nYDstOffset	= nStartY - nDstY;
				nDstHeight	= nDstYSize - nYDstOffset;
			}		
			if ( (nDstY+nDstYSize-1) > nEndY )
				nDstHeight	= nEndY - nDstY - nYDstOffset + 1;

			if ( (nDstWidth > 0) && (nDstHeight > 0) )
			{
				// Destination영역의 설정.
				rcDst.left		= nXDstOffset;
				rcDst.right		= nXDstOffset+nDstWidth;
				rcDst.top		= nYDstOffset;
				rcDst.bottom	= nYDstOffset+nDstHeight;

				// 두이미지의 겹치는 영역을 구한다.
				RECT rcTemp;

				// 소스영역에 상대적인 데스트영역을 구한다.
				rcTemp.left = rcDst.left + (nDstX-nSrcX);
				rcTemp.top = rcDst.top + (nDstY-nSrcY);
				rcTemp.right = rcDst.right + (nDstX-nSrcX);
				rcTemp.bottom = rcDst.bottom + (nDstY-nSrcY);

				// 소스 렉트영역을 기준으로 한 두 사각형의 교차사각형을 구한다.
				IntersectRect(&rcSrcIntersect, &rcSrc, &rcTemp);

				// 데스트영역에 상대적인 소스영역을 구한다.
				rcTemp.left = rcSrc.left + (nSrcX-nDstX);
				rcTemp.top = rcSrc.top + (nSrcY-nDstY);
				rcTemp.right = rcSrc.right + (nSrcX-nDstX);
				rcTemp.bottom = rcSrc.bottom + (nSrcY-nDstY);

				// 데스트 렉트영역을 기준으로 한 두 사각형의 교차사각형을 구한다.
				IntersectRect(&rcDstIntersect, &rcTemp, &rcDst);

				ddsd.dwSize	= sizeof(DDSURFACEDESC2);
				ddsd.lpSurface = NULL;
				m_pddsBackBuffer->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
				if ( !ddsd.lpSurface )			return FALSE;
				pwdDst = (WORD*)ddsd.lpSurface;

				pwSrcSave = new WORD[(rcSrcIntersect.right-rcSrcIntersect.left)*(rcSrcIntersect.bottom-rcSrcIntersect.top)];
				pwDstSave = new WORD[(rcDstIntersect.right-rcDstIntersect.left)*(rcDstIntersect.bottom-rcDstIntersect.top)];

				if ( wSrcColor == 0XFFFF && wDstColor == 0XFFFF )
				{
					DrawWithImageForCompToMem(rcSrcIntersect, (WORD*)pwSrc, wSrcChooseColor1, wSrcChooseColor2, pwSrcSave);
					DrawWithImageForCompToMem(rcDstIntersect, (WORD*)pwDst, wDstChooseColor1, wDstChooseColor2, pwDstSave);
				}
				else if ( wSrcColor != 0XFFFF && wDstColor == 0XFFFF )
				{
					DrawWithImageForCompColorToMem(rcSrcIntersect, (WORD*)pwSrc, wSrcColor, pwSrcSave);
					DrawWithImageForCompToMem(rcDstIntersect, (WORD*)pwDst, wDstChooseColor1, wDstChooseColor2, pwDstSave);
				}
				else if ( wSrcColor == 0XFFFF && wDstColor != 0XFFFF )
				{
					DrawWithImageForCompToMem(rcSrcIntersect, (WORD*)pwSrc, wSrcChooseColor1, wSrcChooseColor2, pwSrcSave);
					DrawWithImageForCompColorToMem(rcDstIntersect, (WORD*)pwDst, wDstColor, pwDstSave);
				}
				else
				{
					DrawWithImageForCompColorToMem(rcSrcIntersect, (WORD*)pwSrc, wSrcColor, pwSrcSave);
					DrawWithImageForCompColorToMem(rcDstIntersect, (WORD*)pwDst, wDstColor, pwDstSave);
				}

				BYTE bRedDst, bGreenDst, bBlueDst;
				BYTE bRedSrc, bGreenSrc, bBlueSrc;
				WORD wSrcPixel, wDstPixel;
				FLOAT rBlueRate, rGreenRate, bRedRate;

				for ( INT nYCnt = 0; nYCnt < rcSrcIntersect.bottom - rcSrcIntersect.top; nYCnt++ )
					for ( INT nXCnt = 0; nXCnt < rcSrcIntersect.right - rcSrcIntersect.left; nXCnt++ )
				{
					wSrcPixel = pwSrcSave[nYCnt*(rcSrcIntersect.right - rcSrcIntersect.left) + nXCnt];
					wDstPixel = pwDstSave[nYCnt*(rcDstIntersect.right - rcDstIntersect.left) + nXCnt];
					if ( wSrcPixel != 0 && wDstPixel != 0 )
					{
						bBlueSrc  = (BYTE)((wSrcPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
						bGreenSrc = (BYTE)((wSrcPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
						bRedSrc   = (BYTE)((wSrcPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

						if ( bFocused )
						{
							rBlueRate = (FLOAT)((FLOAT)bBlueSrc / (FLOAT)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift));
							rGreenRate = (FLOAT)((FLOAT)bGreenSrc / (FLOAT)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift));
							bRedRate = (FLOAT)((FLOAT)bRedSrc / (FLOAT)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift));

							bBlueSrc = (BYTE)((FLOAT)bBlueSrc + ((FLOAT)bBlueSrc*rBlueRate));
							bGreenSrc = (BYTE)((FLOAT)bGreenSrc + ((FLOAT)bGreenSrc*rGreenRate));
							bRedSrc = (BYTE)((FLOAT)bRedSrc + ((FLOAT)bRedSrc*bRedRate));

							if ( bBlueSrc > (m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift) )
								bBlueSrc = (BYTE)(m_stBitsMaskInfo.dwBMask>>m_stBitsMaskInfo.bBShift);
							if ( bGreenSrc > (m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift) )
								bGreenSrc = (BYTE)(m_stBitsMaskInfo.dwGMask>>m_stBitsMaskInfo.bGShift);
							if ( bRedSrc > (m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift) )
								bRedSrc = (BYTE)(m_stBitsMaskInfo.dwRMask>>m_stBitsMaskInfo.bRShift);

							// 포커스가 됐을때 Opacity값을 높여준다.
							bOpa = 40;
						}

						bBlueDst  = (BYTE)((wDstPixel & m_stBitsMaskInfo.dwBMask) >> m_stBitsMaskInfo.bBShift);
						bGreenDst = (BYTE)((wDstPixel & m_stBitsMaskInfo.dwGMask) >> m_stBitsMaskInfo.bGShift);
						bRedDst   = (BYTE)((wDstPixel & m_stBitsMaskInfo.dwRMask) >> m_stBitsMaskInfo.bRShift);

						bBlueDst  = (BYTE)((bOpa*(bBlueDst -bBlueSrc )+100*bBlueSrc ) / 100);
						bGreenDst = (BYTE)((bOpa*(bGreenDst-bGreenSrc)+100*bGreenSrc) / 100);
						bRedDst   = (BYTE)((bOpa*(bRedDst  -bRedSrc  )+100*bRedSrc  ) / 100);

						pwdDst[((nYCnt+nSrcY+rcSrcIntersect.top) * (ddsd.lPitch >> 1)) + (rcSrcIntersect.left+nSrcX+nXCnt)] = 
												((bRedDst  <<m_stBitsMaskInfo.bRShift) |
												 (bGreenDst<<m_stBitsMaskInfo.bGShift) |
												 (bBlueDst <<m_stBitsMaskInfo.bBShift));
					}
				}

				SAFE_DELETE(pwSrcSave);
				SAFE_DELETE(pwDstSave);
				m_pddsBackBuffer->Unlock(NULL);
				return TRUE;
			}
		}
	}
	return FALSE;
}





/******************************************************************************************************************

	함수명 : CWHDXGraphicWindow::Present()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : LPRECT lprcDest
	         LPRECT lprcSrc
	         BYTE bCntRgn
	출력   : HRESULT 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
HRESULT CWHDXGraphicWindow::Present()
{
	HRESULT hr;

	if ( NULL == m_pddsFrontBuffer || NULL == m_pddsBackBuffer )
	{
		return E_POINTER;
	}

	while ( TRUE )
	{
//		if ( m_bScreenModeFlag & _DXG_SCREENMODE_WINDOW )
//		{
//		 에디트윈도우 출력을 위해서 클리퍼를 세팅한다.
			m_pddsFrontBuffer->SetClipper(m_lpcClipper);
			hr = m_pddsFrontBuffer->Blt(&m_rcWindow, m_pddsBackBuffer, NULL, DDBLT_WAIT, NULL);
/*		}
		else
		{
			hr = m_pddsFrontBuffer->Flip(NULL, DDFLIP_WAIT );
		}
*/
		if(hr == DDERR_SURFACELOST)
		{
			m_pddsFrontBuffer->Restore();
			m_pddsBackBuffer->Restore();
		}

		if(hr != DDERR_WASSTILLDRAWING)
			return hr;
	} 
}



/******************************************************************************************************************

	함수명 : RestoreSurfaces()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : HRESULT 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
HRESULT CWHDXGraphicWindow::RestoreSurfaces()
{
    HRESULT hr;

	if ( !m_pDD )
		return E_FAIL;

    if ( FAILED(hr = m_pDD->RestoreAllSurfaces()) )
        return hr;

    return S_OK;
}













/******************************************************************************************************************

  Callback Function

*******************************************************************************************************************/


/******************************************************************************************************************

	함수명 : WINAPI DXGDriverEnumCallbackEx()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : GUID *pGUID
	         LPSTR szDesc
	         LPSTR szName
	         LPVOID pContext
	         HMONITOR hm
	출력   : static BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
static BOOL WINAPI DXGDriverEnumCallbackEx(GUID *pGUID, LPSTR szDesc, LPSTR szName, LPVOID pContext, HMONITOR hm)
{
    LPDIRECTDRAW7			pDD	 = NULL;
    LPDIRECT3D7				pD3D = NULL;
	DXG_ENUM_DEVICEINFO		stDXGEnumDeviceInfo;
    HRESULT					hr;

    if ( FAILED(hr = DirectDrawCreateEx(pGUID, (VOID**) &pDD, IID_IDirectDraw7, NULL)) )
	{
		MessageBox(NULL, TEXT("[DXGDriverEnumCallbackEx]") TEXT("Can't create DDraw during enumeration!"), "MirDXG", MB_ICONERROR | MB_OK);
        return D3DENUMRET_OK;
	}

    if ( FAILED(hr = pDD->QueryInterface(IID_IDirect3D7, (VOID**) &pD3D)) )
    {
		SAFE_RELEASE(pDD);
		MessageBox(NULL, TEXT("[DXGDriverEnumCallbackEx]") TEXT("Can't query IDirect3D7 during enumeration!"), "MirDXG", MB_ICONERROR | MB_OK);
        return D3DENUMRET_OK;
    }

    ZeroMemory(&stDXGEnumDeviceInfo, sizeof(DXG_ENUM_DEVICEINFO));
    lstrcpyn(stDXGEnumDeviceInfo.szDriverDesc, szDesc, 255);

    stDXGEnumDeviceInfo.ddDriverCaps.dwSize = sizeof(DDCAPS);
    stDXGEnumDeviceInfo.ddHELCaps.dwSize    = sizeof(DDCAPS);
    pDD->GetCaps(&stDXGEnumDeviceInfo.ddDriverCaps, &stDXGEnumDeviceInfo.ddHELCaps);

    if ( pGUID )
    {
        stDXGEnumDeviceInfo.guidDriver  = (*pGUID);
        stDXGEnumDeviceInfo.pDriverGUID = &stDXGEnumDeviceInfo.guidDriver;
    }

    if ( FAILED(hr = pDD->EnumDisplayModes(0, NULL, &stDXGEnumDeviceInfo, DXGEnumDisplayModeCallback)) )
	{
		MessageBox(NULL, TEXT("[DXGDriverEnumCallbackEx]") TEXT("Can't enumerate displaymode!"), "MirDXG", MB_ICONERROR | MB_OK);
	}

    qsort(stDXGEnumDeviceInfo.pddsdModes, stDXGEnumDeviceInfo.dwNumModes, sizeof(DDSURFACEDESC2), DXGModesSortCallback);

    if ( FAILED(hr = pD3D->EnumDevices(DXG3DDeviceEnumCallback, &stDXGEnumDeviceInfo)) )
	{
		MessageBox(NULL, TEXT("[DXGDriverEnumCallbackEx]") TEXT("Can't enumerate device!"), "MirDXG", MB_ICONERROR | MB_OK);
	}

    SAFE_DELETE(stDXGEnumDeviceInfo.pddsdModes);
	SAFE_RELEASE(pD3D);
	SAFE_RELEASE(pDD);

	return DDENUMRET_OK;
}


/******************************************************************************************************************

	함수명 : WINAPI DXGEnumDisplayModeCallback()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : DDSURFACEDESC2* pddsd
	         VOID* pParentInfo
	출력   : static HRESULT 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
static HRESULT WINAPI DXGEnumDisplayModeCallback(DDSURFACEDESC2* pddsd, VOID* pParentInfo)
{
    DXG_ENUM_DEVICEINFO* pDevice = (DXG_ENUM_DEVICEINFO*) pParentInfo;

    DDSURFACEDESC2* pddsdNewModes = new DDSURFACEDESC2[pDevice->dwNumModes+1];
    memcpy(pddsdNewModes, pDevice->pddsdModes, pDevice->dwNumModes * sizeof(DDSURFACEDESC2));
    SAFE_DELETE(pDevice->pddsdModes);
    pDevice->pddsdModes = pddsdNewModes;

    pDevice->pddsdModes[pDevice->dwNumModes++] = (*pddsd);

    return DDENUMRET_OK;
}


/******************************************************************************************************************

	함수명 : DXGModesSortCallback()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : const VOID* arg1
	         const VOID* arg2
	출력   : static int 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
static int DXGModesSortCallback(const VOID* arg1, const VOID* arg2)
{
    DDSURFACEDESC2* p1 = (DDSURFACEDESC2*)arg1;
    DDSURFACEDESC2* p2 = (DDSURFACEDESC2*)arg2;

    if ( p1->dwWidth < p2->dwWidth )        return -1;
    if ( p1->dwWidth > p2->dwWidth )        return +1;

    if ( p1->dwHeight < p2->dwHeight )      return -1;
    if ( p1->dwHeight > p2->dwHeight )      return +1;

    if ( p1->ddpfPixelFormat.dwRGBBitCount < p2->ddpfPixelFormat.dwRGBBitCount )        return -1;
    if ( p1->ddpfPixelFormat.dwRGBBitCount > p2->ddpfPixelFormat.dwRGBBitCount )        return +1;

    return 0;
}


/******************************************************************************************************************

	함수명 : WINAPI DXG3DDeviceEnumCallback()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : TCHAR* szDesc
	         TCHAR* szName
	         D3DDEVICEDESC7* pDesc
	         VOID* pParentInfo
	출력   : static HRESULT 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
static HRESULT WINAPI DXG3DDeviceEnumCallback(TCHAR* szDesc, TCHAR* szName, D3DDEVICEDESC7* pDesc, VOID* pParentInfo)
{
    DXG_ENUM_DEVICEINFO* pDriverInfo = (DXG_ENUM_DEVICEINFO*)pParentInfo;
    DXG_ENUM_DEVICEINFO* pDeviceInfo = &g_stDXGEnumDeviceInfo[g_bNumDevices];
    ZeroMemory(pDeviceInfo, sizeof(DXG_ENUM_DEVICEINFO));

    pDeviceInfo->f3DHardware = pDesc->dwDevCaps & D3DDEVCAPS_HWRASTERIZATION;
    memcpy(&pDeviceInfo->ddDeviceDesc, pDesc, sizeof(D3DDEVICEDESC7));

    pDeviceInfo->ddDriverCaps       = pDriverInfo->ddDriverCaps;
    pDeviceInfo->ddHELCaps          = pDriverInfo->ddHELCaps;
    pDeviceInfo->guidDevice         = pDesc->deviceGUID;
    pDeviceInfo->pDeviceGUID        = &pDeviceInfo->guidDevice;
    pDeviceInfo->pddsdModes         = new DDSURFACEDESC2[pDriverInfo->dwNumModes];

    if ( pDriverInfo->pDriverGUID )
    {
        pDeviceInfo->guidDriver  = pDriverInfo->guidDriver;
        pDeviceInfo->pDriverGUID = &pDeviceInfo->guidDriver;
    }
    else
    {
        pDeviceInfo->pDriverGUID = NULL;
    }
    lstrcpyn(pDeviceInfo->szDriverDesc, pDriverInfo->szDriverDesc, 255);
    lstrcpyn(pDeviceInfo->szDeviceDesc, szName, 255);

    for ( DWORD dwCnt=0; dwCnt<pDriverInfo->dwNumModes; dwCnt++ )
    {
        DDSURFACEDESC2 ddsdMode = pDriverInfo->pddsdModes[dwCnt];
        DWORD dwRenderDepths    = pDeviceInfo->ddDeviceDesc.dwDeviceRenderBitDepth;
        DWORD dwDepth           = ddsdMode.ddpfPixelFormat.dwRGBBitCount;

        if ( ( ( dwDepth == 32 ) && ( dwRenderDepths & DDBD_32 ) ) ||
             ( ( dwDepth == 24 ) && ( dwRenderDepths & DDBD_24 ) ) ||
             ( ( dwDepth == 16 ) && ( dwRenderDepths & DDBD_16 ) ) )
        {
            pDeviceInfo->pddsdModes[pDeviceInfo->dwNumModes++] = ddsdMode;
        }
    }

    if ( 0 == pDeviceInfo->dwNumModes )
        return D3DENUMRET_OK;

    g_bNumDevices++;
    if ( NULL != pDeviceInfo->pDriverGUID && FALSE == pDeviceInfo->f3DHardware )
            return D3DENUMRET_OK;

    return D3DENUMRET_OK;
}


/******************************************************************************************************************

	함수명 : WINAPI DXGEnumZBufferFormatsCallback()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : DDPIXELFORMAT* pddpf
	         VOID* pContext
	출력   : static HRESULT 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
static HRESULT WINAPI DXGEnumZBufferFormatsCallback(DDPIXELFORMAT* pddpf, VOID* pContext)
{
    DDPIXELFORMAT* pddpfOut = (DDPIXELFORMAT*) pContext;

    if ( pddpfOut->dwRGBBitCount == pddpf->dwRGBBitCount )
    {
        (*pddpfOut) = (*pddpf);
        return D3DENUMRET_CANCEL;
    }

    return D3DENUMRET_OK;
}