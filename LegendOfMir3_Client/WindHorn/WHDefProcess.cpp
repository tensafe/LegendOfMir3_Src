#include "StdAfx.h"


CWHDefProcess::CWHDefProcess()
{
	m_pxDXGWnd = (CWHDXGraphicWindow*)GetDXGWindowPtr();
	assert(m_pxDXGWnd);
}


CWHDefProcess::~CWHDefProcess()
{
}


HRESULT CWHDefProcess::Clear(DWORD dwColor)
{
    DDBLTFX ddbltfx;

    if (NULL == ((CWHDXGraphicWindow*)m_pxDXGWnd)->GetBackBuffer())
	{
        return E_POINTER;
	}

    ZeroMemory(&ddbltfx, sizeof(ddbltfx));
    ddbltfx.dwSize      = sizeof(ddbltfx);
    ddbltfx.dwFillColor = dwColor;

    return (((CWHDXGraphicWindow*)m_pxDXGWnd)->GetBackBuffer())->Blt(NULL, NULL, NULL, DDBLT_COLORFILL, &ddbltfx);
}


VOID CWHDefProcess::ShowStatus(INT nStartX, INT nStartY)
{
    TCHAR			szBuff[MAX_PATH];
    static DWORD	dwFPS 		= 0;
    static DWORD	dwLastTime	= 0;
    static DWORD	dwFrames	= 0;

    DWORD dwTime = timeGetTime();
    ++dwFrames;

    if ( dwTime - dwLastTime > 1000 )
    {
        dwFPS      = dwFrames / ( (dwTime - dwLastTime) / 1000 );
        dwLastTime = dwTime;
        dwFrames   = 0;

    }

    sprintf(szBuff, "FPS : %d", dwFPS);
	((CWHDXGraphicWindow*)m_pxDXGWnd)->PutsHan(((CWHDXGraphicWindow*)m_pxDXGWnd)->GetBackBuffer(), nStartX, nStartY, RGB(250, 250, 250), RGB(0, 0, 0), szBuff);
}


LRESULT CWHDefProcess::DefMainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch ( uMsg )
	{
	case WM_KEYDOWN:
		switch ( wParam )
		{
		case VK_ESCAPE:
//			SendMessage(((CWHDXGraphicWindow*)m_pxDXGWnd)->GetSafehWnd(), WM_DESTROY, wParam, lParam);
			break;
		}
		break;
	default:
		break;
	}

	return ((CWHDXGraphicWindow*)m_pxDXGWnd)->MainWndProcDXG(hWnd, uMsg, wParam, lParam);
}
