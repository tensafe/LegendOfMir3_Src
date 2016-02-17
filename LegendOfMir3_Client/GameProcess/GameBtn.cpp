/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/



#include "StdAfx.h"


VOID CGameBtn::CreateGameBtn(CWHWilImageData* pxBtnImage, INT nFocusImgIdx, INT nClickImgIdx, INT nStartX, INT nStartY, BYTE bBtnType)
{
	m_bBtnType		= bBtnType;
	m_bBtnState		= _BTN_STATE_NORMAL;
	m_pxBtnImage	= pxBtnImage;
	m_nFocusImgIdx	= nFocusImgIdx;
	m_nClickImgIdx	= nClickImgIdx;

	m_pxBtnImage->NewSetIndex(nFocusImgIdx);
	SetRect(&m_rcBtn, nStartX, nStartY, 
			nStartX+m_pxBtnImage->m_lpstNewCurrWilImageInfo->shWidth,
			nStartY+m_pxBtnImage->m_lpstNewCurrWilImageInfo->shHeight);
}


VOID CGameBtn::Init()
{
	m_bBtnType		= _BTN_TYPE_FOCUS;
	m_bBtnState		= _BTN_STATE_FOCUS;
	m_pxBtnImage	= NULL;
	m_nFocusImgIdx	= 0;
	m_nClickImgIdx	= 0;
	SetRect(&m_rcBtn, 0, 0, 0, 0);
}


VOID CGameBtn::Destroy()
{
	Init();
}


VOID CGameBtn::ShowGameBtn()
{
	if ( m_bBtnState == _BTN_STATE_FOCUS && m_bBtnType == _BTN_TYPE_FOCUS )
	{
		m_pxBtnImage->NewSetIndex(m_nFocusImgIdx);
 		g_xMainWnd.DrawWithImageForCompClipRgn(m_rcBtn.left, m_rcBtn.top, m_rcBtn.right-m_rcBtn.left, m_rcBtn.bottom-m_rcBtn.top,
											   (WORD*)m_pxBtnImage->m_pbCurrImage, _CLIP_WIDTH, 600);
	}
	else if ( m_bBtnState == _BTN_STATE_CLICK )
	{
		m_pxBtnImage->NewSetIndex(m_nClickImgIdx);
 		g_xMainWnd.DrawWithImageForCompClipRgn(m_rcBtn.left, m_rcBtn.top, m_rcBtn.right-m_rcBtn.left, m_rcBtn.bottom-m_rcBtn.top,
											   (WORD*)m_pxBtnImage->m_pbCurrImage, _CLIP_WIDTH, 600);
	}
}


VOID CGameBtn::OnMouseMove(POINT ptMouse)
{
	if ( PtInRect(&m_rcBtn, ptMouse) && (m_bBtnState != _BTN_STATE_CLICK) )
	{
		m_bBtnState = _BTN_STATE_FOCUS;
		return;
	}
	else if ( m_bBtnState == _BTN_STATE_CLICK )
	{
		return;
	}
	m_bBtnState = _BTN_STATE_NORMAL;
}


BOOL CGameBtn::OnLButtonDown(POINT ptMouse)
{
	if ( PtInRect(&m_rcBtn, ptMouse) )
	{
		m_bBtnState = _BTN_STATE_CLICK;
		return TRUE;
	}
	return FALSE;
}


BOOL CGameBtn::OnLButtonUp(POINT ptMouse)
{
	m_bBtnState = _BTN_STATE_NORMAL;
	if ( PtInRect(&m_rcBtn, ptMouse) )
	{
		return TRUE;
	}
	return FALSE;
}


VOID CGameBtn::ChangeRect(INT nStartX, INT nStartY)
{
	m_pxBtnImage->NewSetIndex(m_nFocusImgIdx);
	SetRect(&m_rcBtn, nStartX, nStartY, 
			nStartX+m_pxBtnImage->m_lpstNewCurrWilImageInfo->shWidth,
			nStartY+m_pxBtnImage->m_lpstNewCurrWilImageInfo->shHeight);
}

VOID CGameBtn::ResetGameBtn(INT nFocusImgIdx, INT nClickImgIdx)
{
	m_bBtnState		= _BTN_STATE_NORMAL;
	m_nFocusImgIdx	= nFocusImgIdx;
	m_nClickImgIdx	= nClickImgIdx;
	if ( m_pxBtnImage->NewSetIndex(nFocusImgIdx) )
	{
		INT nStartX = m_rcBtn.left;
		INT nStartY = m_rcBtn.top;
		SetRect(&m_rcBtn, nStartX, nStartY, 
				nStartX+m_pxBtnImage->m_lpstNewCurrWilImageInfo->shWidth,
				nStartY+m_pxBtnImage->m_lpstNewCurrWilImageInfo->shHeight);
	}
}




CMsgBoxBtn::CMsgBoxBtn():CGameBtn()
{
}

CMsgBoxBtn::~CMsgBoxBtn()
{
}

VOID CMsgBoxBtn::ShowGameBtn()
{
	if ( m_bBtnState != _BTN_STATE_CLICK)
	{
		m_pxBtnImage->NewSetIndex(m_nFocusImgIdx);
 		g_xMainWnd.DrawWithImageForCompClipRgn(m_rcBtn.left, m_rcBtn.top, m_rcBtn.right-m_rcBtn.left, m_rcBtn.bottom-m_rcBtn.top,
											   (WORD*)m_pxBtnImage->m_pbCurrImage, _CLIP_WIDTH, 600);
	}
	else
	{
		m_pxBtnImage->NewSetIndex(m_nClickImgIdx);
 		g_xMainWnd.DrawWithImageForCompClipRgn(m_rcBtn.left, m_rcBtn.top, m_rcBtn.right-m_rcBtn.left, m_rcBtn.bottom-m_rcBtn.top,
											   (WORD*)m_pxBtnImage->m_pbCurrImage, _CLIP_WIDTH, 600);
	}
}












CScrlBar::CScrlBar()
{
	Init();
}

CScrlBar::~CScrlBar()
{
}

VOID CScrlBar::CreateScrlBar(CWHWilImageData* pxImage, INT ImgIdx, INT nPageLine, WORD wScrlWidth, WORD wScrlHeight, WORD wScrlBtnHeight)
{
	m_pxScrlImage	= pxImage;
	m_nScrlImgIdx	= ImgIdx;
	m_nPageLine		= nPageLine;

	m_wScrlWidth	= wScrlWidth;
	m_wScrlHeight	= wScrlHeight;

	m_wScrlBtnHeight= wScrlBtnHeight;
}

VOID CScrlBar::ShowScrlBar(INT nX, INT nY, INT nFstPos, INT nCounter)
{
	RECT	rcImg;

	if ( m_pxScrlImage->NewSetIndex(m_nScrlImgIdx) )
	{
		INT nImgHeight = m_pxScrlImage->m_lpstNewCurrWilImageInfo->shHeight;
		INT nImgWidth  = m_pxScrlImage->m_lpstNewCurrWilImageInfo->shWidth;

		m_fScrlRate = (FLOAT)nFstPos/(FLOAT)(nCounter-1);
		m_fPageRate = (FLOAT)m_nPageLine/(FLOAT)(nCounter);

		SetRect(&m_rcScrlRgn, 
				nX, nY+nImgHeight-m_wScrlHeight, nX+m_wScrlWidth, nY+nImgHeight);

		SetRect(&m_rcScrlBtnRgn, 
				nX, nY+nImgHeight-m_wScrlHeight+(m_wScrlHeight*m_fScrlRate),
				nX+m_wScrlWidth, nY+nImgHeight-m_wScrlHeight+(m_wScrlHeight*m_fScrlRate)+m_wScrlBtnHeight);

		SetRect(&rcImg,
				0, nImgHeight-m_wScrlHeight-m_fScrlRate*m_wScrlHeight,
				m_wScrlWidth, nImgHeight-m_fScrlRate*m_wScrlHeight);

//		g_xMainWnd.DrawWithGDI(m_rcScrlRgn, NULL, RGB(255, 250, 180), 1);
//		g_xMainWnd.DrawWithGDI(m_rcScrlBtnRgn, NULL, RGB(255, 250, 180), 2);

		RECT rc = {nX+10, nY, nX+10+nImgWidth, nY+nImgHeight};
//		g_xMainWnd.DrawWithGDI(rc, NULL, RGB(255, 250, 180), 2);
// 		g_xMainWnd.DrawWithImageForComp(nX, nY+m_fScrlRate*m_wScrlHeight, nImgWidth, nImgHeight, (WORD*)m_pxScrlImage->m_pbCurrImage);
 		g_xMainWnd.DrawWithImageForComp(nX, nY+m_fScrlRate*m_wScrlHeight, rcImg, (WORD*)m_pxScrlImage->m_pbCurrImage);
	}
}

BOOL CScrlBar::OnMouseMove(POINT ptMouse)
{
	if ( m_bGrapsed )
	{
		if ( ptMouse.y <= m_rcScrlRgn.top )
		{
			m_fScrlRate = 0.0f;
		}
		else if ( ptMouse.y >= m_rcScrlRgn.bottom )
		{
			m_fScrlRate = 1.0f;
		}
		else
			m_fScrlRate = (FLOAT)(ptMouse.y-m_rcScrlRgn.top)/(FLOAT)(m_rcScrlRgn.bottom-m_rcScrlRgn.top);

		return TRUE;
	}

	return FALSE;
}

BOOL CScrlBar::OnLButtonDown(POINT ptMouse)
{
	if ( PtInRect(&m_rcScrlBtnRgn, ptMouse)  )
	{
		m_bGrapsed = TRUE;
	}
	else if ( PtInRect(&m_rcScrlRgn, ptMouse) )
	{
		// 위로 한페이지 스크롤.
		if ( m_rcScrlBtnRgn.top+m_wScrlBtnHeight/2 > ptMouse.y )
		{
			m_fScrlRate -= m_fPageRate;

			if ( m_fScrlRate < 0.0f )
			{
				m_fScrlRate = 0.0f;
			}
		}
		// 아래로 한페이지 스크롤.
		else if ( m_rcScrlBtnRgn.top+m_wScrlBtnHeight/2 <= ptMouse.y )
		{
			m_fScrlRate += m_fPageRate;

			if ( m_fScrlRate > 1.0f )
			{
				m_fScrlRate = 1.0f;
			}
		}

		return TRUE;
	}

	return FALSE;
}

BOOL CScrlBar::OnLButtonUp(POINT ptMouse)
{
	if ( m_bGrapsed )
	{
		m_bGrapsed = FALSE;
		return TRUE;
	}
	else
		return FALSE;
}
