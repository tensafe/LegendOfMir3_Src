#include "stdafx.h"

#define WND_WIDTH		640
#define WND_HEIGHT		480

CBMWnd::CBMWnd()
{
	m_fIsActive = FALSE;
	m_fIsFocused = FALSE;
	SetRect(&m_rcWnd,0,0,0,0);
}

CBMWnd::~CBMWnd()
{

}

VOID CBMWnd::OpenWnd(VOID)
{
	m_fIsActive = TRUE;
}

VOID CBMWnd::CloseWnd(VOID)
{
	m_fIsActive = FALSE;
}

HRESULT CBMWnd::OnButtonDown(WPARAM wParam, LPARAM lParam)
{	
	POINT	ptMouse;
	ptMouse.x = LOWORD(lParam);
	ptMouse.y = HIWORD(lParam);
	
	return OnButtonDown(ptMouse);
}

HRESULT CBMWnd::OnButtonDown(POINT ptMouse)
{
	
	return 0;
}

HRESULT	CBMWnd::OnButtonUp(WPARAM wParam, LPARAM lParam)
{
	POINT	ptMouse;
	ptMouse.x = LOWORD(lParam);
	ptMouse.y = HIWORD(lParam);
	
	return OnButtonUp(ptMouse);
}

HRESULT CBMWnd::OnButtonUp(POINT ptMouse)
{
	return 0;
}

HRESULT CBMWnd::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	return 0;
}


LRESULT CBMWnd::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	return 0;
}


VOID CBMWnd::Render(INT nLoopTime)
{
}