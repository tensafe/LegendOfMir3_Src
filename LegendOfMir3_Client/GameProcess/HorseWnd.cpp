/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/



#include "StdAfx.h"


CHorseWnd::CHorseWnd()
{
	Init();
}

CHorseWnd::~CHorseWnd()
{
	Destroy();
}

VOID CHorseWnd::Init()
{
	CGameWnd::Init();

	m_xHorseBtn.Init();
}

VOID CHorseWnd::Destroy()
{
	CGameWnd::Destroy();
}

VOID CHorseWnd::CreateHorseWnd(INT nID, CWHWilImageData* pxWndImage, INT nFrameImgIdx, INT nStartX, INT nStartY, INT nWidth, INT nHeight, BOOL bCanMove)
{
	CreateGameWnd(nID, pxWndImage, nFrameImgIdx, bCanMove, nStartX, nStartY, nWidth, nHeight);
	m_xHorseBtn.CreateGameBtn(pxWndImage, 630, 631, nStartX+567, nStartY+413);
}

VOID CHorseWnd::ShowHorseWnd()
{
	ShowGameWnd();
	m_xHorseBtn.ShowGameBtn();
}

BOOL CHorseWnd::OnLButtonUp(POINT ptMouse)
{
	if ( m_xHorseBtn.OnLButtonUp(ptMouse) )		return TRUE;

	return FALSE;
}

BOOL CHorseWnd::OnLButtonDown(POINT ptMouse)
{
	if ( m_xHorseBtn.OnLButtonDown(ptMouse) )		return TRUE;

	return FALSE;
}

VOID CHorseWnd::OnMouseMove(POINT ptMouse)
{
	m_xHorseBtn.ChangeRect(m_rcWnd.left+567, m_rcWnd.top+413);
	m_xHorseBtn.OnMouseMove(ptMouse);
}

VOID CHorseWnd::SetStatusBtnInit()
{
	m_xHorseBtn.SetBtnState(_BTN_STATE_NORMAL);
}

