/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/



#include "StdAfx.h"


CGuildWnd::CGuildWnd()
{
	Init();
}

CGuildWnd::~CGuildWnd()
{
	Destroy();
}

VOID CGuildWnd::Init()
{
	CGameWnd::Init();
}

VOID CGuildWnd::Destroy()
{
	CGameWnd::Destroy();
}

VOID CGuildWnd::CreateGuildWnd(INT nID, CWHWilImageData* pxWndImage, INT nFrameImgIdx, INT nStartX, INT nStartY, INT nWidth, INT nHeight, BOOL bCanMove)
{
	CreateGameWnd(nID, pxWndImage, nFrameImgIdx, bCanMove, nStartX, nStartY, nWidth, nHeight);
}

VOID CGuildWnd::ShowGuildWnd()
{
	ShowGameWnd();
}

BOOL CGuildWnd::OnLButtonUp(POINT ptMouse)
{
	return FALSE;
}

BOOL CGuildWnd::OnLButtonDown(POINT ptMouse)
{
	return FALSE;
}

VOID CGuildWnd::OnMouseMove(POINT ptMouse)
{
}

