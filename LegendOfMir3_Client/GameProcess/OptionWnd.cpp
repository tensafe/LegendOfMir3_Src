/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/



#include "StdAfx.h"


COptionWnd::COptionWnd()
{
	Init();
}

COptionWnd::~COptionWnd()
{
	Destroy();
}

VOID COptionWnd::Init()
{
	CGameWnd::Init();

	m_xOptionBtn.Init();
}

VOID COptionWnd::Destroy()
{
	CGameWnd::Destroy();
}

VOID COptionWnd::CreateOptionWnd(INT nID, CWHWilImageData* pxWndImage, INT nFrameImgIdx, INT nStartX, INT nStartY, INT nWidth, INT nHeight, BOOL bCanMove)
{
	CreateGameWnd(nID, pxWndImage, nFrameImgIdx, bCanMove, nStartX, nStartY, nWidth, nHeight);
	m_xOptionBtn.CreateGameBtn(pxWndImage, 764, 765, nStartX+220, nStartY+11);
}

VOID COptionWnd::ShowOptionWnd()
{
	ShowGameWnd();
	m_xOptionBtn.ShowGameBtn();
}

BOOL COptionWnd::OnLButtonUp(POINT ptMouse)
{
	if ( m_xOptionBtn.OnLButtonUp(ptMouse) )		return TRUE;

	return FALSE;
}

BOOL COptionWnd::OnLButtonDown(POINT ptMouse)
{
	if ( m_xOptionBtn.OnLButtonDown(ptMouse) )		return TRUE;

	return FALSE;
}

VOID COptionWnd::OnMouseMove(POINT ptMouse)
{
	m_xOptionBtn.ChangeRect(m_rcWnd.left+220, m_rcWnd.top+11);
	m_xOptionBtn.OnMouseMove(ptMouse);
}

VOID COptionWnd::SetStatusBtnInit()
{
	m_xOptionBtn.SetBtnState(_BTN_STATE_NORMAL);
}

