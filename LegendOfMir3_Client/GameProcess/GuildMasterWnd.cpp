/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/



#include "StdAfx.h"


CGuildMasterWnd::CGuildMasterWnd()
{
	Init();
}

CGuildMasterWnd::~CGuildMasterWnd()
{
	Destroy();
}

VOID CGuildMasterWnd::Init()
{
	CGameWnd::Init();

	m_xGuildMasterBtn.Init();
}

VOID CGuildMasterWnd::Destroy()
{
	CGameWnd::Destroy();
}

VOID CGuildMasterWnd::CreateGuildMasterWnd(INT nID, CWHWilImageData* pxWndImage, INT nFrameImgIdx, INT nStartX, INT nStartY, INT nWidth, INT nHeight, BOOL bCanMove)
{
	CreateGameWnd(nID, pxWndImage, nFrameImgIdx, bCanMove, nStartX, nStartY, nWidth, nHeight);
	m_xGuildMasterBtn.CreateGameBtn(pxWndImage, 630, 631, nStartX+567, nStartY+413);
}

VOID CGuildMasterWnd::ShowGuildMasterWnd()
{
	ShowGameWnd();
	m_xGuildMasterBtn.ShowGameBtn();
}

BOOL CGuildMasterWnd::OnLButtonUp(POINT ptMouse)
{
	if ( m_xGuildMasterBtn.OnLButtonUp(ptMouse) )		return TRUE;

	return FALSE;
}

BOOL CGuildMasterWnd::OnLButtonDown(POINT ptMouse)
{
	if ( m_xGuildMasterBtn.OnLButtonDown(ptMouse) )		return TRUE;

	return FALSE;
}

VOID CGuildMasterWnd::OnMouseMove(POINT ptMouse)
{
	m_xGuildMasterBtn.ChangeRect(m_rcWnd.left+567, m_rcWnd.top+413);
	m_xGuildMasterBtn.OnMouseMove(ptMouse);
}

VOID CGuildMasterWnd::SetStatusBtnInit()
{
	m_xGuildMasterBtn.SetBtnState(_BTN_STATE_NORMAL);
}

