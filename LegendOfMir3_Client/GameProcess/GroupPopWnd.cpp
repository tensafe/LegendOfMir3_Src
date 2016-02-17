/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/



#include "StdAfx.h"



CGroupPopWnd::CGroupPopWnd()
{
	Init();
}

CGroupPopWnd::~CGroupPopWnd()
{
	Destroy();
}

VOID CGroupPopWnd::CreateGroupPopWnd(INT nID, CWHWilImageData* pxWndImage, INT nFrameImgIdx, INT nStartX, INT nStartY, INT nWidth, INT nHeight, BOOL bCanMove)
{
	CreateGameWnd(nID, pxWndImage, nFrameImgIdx, bCanMove, nStartX, nStartY, nWidth, nHeight);
}

VOID CGroupPopWnd::Init()
{
	CGameWnd::Init();
}

VOID CGroupPopWnd::Destroy()
{
	CGameWnd::Destroy();
}


VOID CGroupPopWnd::ShowGroupPopWnd()
{
	ShowGameWnd();
}

BOOL CGroupPopWnd::OnLButtonUp(POINT ptMouse)
{
	return FALSE;
}

BOOL CGroupPopWnd::OnLButtonDown(POINT ptMouse)
{
	return FALSE;
}

VOID CGroupPopWnd::OnMouseMove(POINT ptMouse)
{
}