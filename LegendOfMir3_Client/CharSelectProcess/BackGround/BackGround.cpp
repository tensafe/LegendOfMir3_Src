#include "stdafx.h"

#define AVI_CREATECHAR						".\\CreateChr.avi"
#define AVI_STARTGAME						".\\StartGame.avi"

#define IMG_IDX_SELECT_CHR_BACK				50

#define IMG_IDX_CREATE_CHR_BACK				90

#define BACK_IMG_POS_X						0
#define BACK_IMG_POS_Y						0

CBackGround::CBackGround()
{
	m_nRenderState = 0;
	m_pxImage = NULL;
}

CBackGround::~CBackGround()
{
	m_xCreateChrAvi.ReleaseAvi();
	m_xStartGameAvi.ReleaseAvi();
}

VOID CBackGround::Create(CWHWilImageData* pxImage)
{
	m_pxImage = pxImage;
	m_xCreateChrAvi.InitAvi(g_xSound.GetSoundObject()->GetDS());
	m_xStartGameAvi.InitAvi(g_xSound.GetSoundObject()->GetDS());
}

HRESULT	CBackGround::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

HRESULT CBackGround::OnButtonDown(WPARAM wParam, LPARAM lParam)
{

	return 0;
}

HRESULT CBackGround::OnButtonDown(POINT ptMouse)
{
	return 0;
}

HRESULT	CBackGround::OnButtonUp(WPARAM wParam, LPARAM lParam)
{

	return 0;
}

HRESULT CBackGround::OnButtonUp(POINT ptMouse)
{

	return 0;
}

LRESULT	CBackGround::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

VOID CBackGround::Render(INT	nLoopTime)
{
	if(m_fIsActive)
	{
		switch(m_nRenderState)
		{
			case _RENDER_STILL_1:
			{
				m_pxImage->NewSetIndex(IMG_IDX_SELECT_CHR_BACK);
				g_xMainWnd.DrawWithImageForComp(BACK_IMG_POS_X, BACK_IMG_POS_Y,
												m_pxImage->m_lpstNewCurrWilImageInfo->shWidth,m_pxImage->m_lpstNewCurrWilImageInfo->shHeight,
												(WORD*)(m_pxImage->m_pbCurrImage));
				break;
			}
			case _RENDER_AVI_IN:
			{
				if(!m_xCreateChrAvi.Draw(nLoopTime))
				{
					m_nRenderState = _RENDER_STILL_2;
				}
				break;
			}
			case _RENDER_STILL_2:
			{
				m_pxImage->NewSetIndex(IMG_IDX_CREATE_CHR_BACK);
				g_xMainWnd.DrawWithImageForComp(BACK_IMG_POS_X, BACK_IMG_POS_Y,
												m_pxImage->m_lpstNewCurrWilImageInfo->shWidth,m_pxImage->m_lpstNewCurrWilImageInfo->shHeight,
												(WORD*)(m_pxImage->m_pbCurrImage));
				break;
			}
			case _RENDER_AVI_OUT:
			{
				if(!m_xCreateChrAvi.Draw(nLoopTime))
					m_nRenderState = _RENDER_STILL_1;
				break;
			}
			case _RENDER_AVI_START:
			{
				if(!m_xStartGameAvi.Draw(nLoopTime))
					m_nRenderState = _RENDER_STARTGAME;
				break;
			}
		}
	}
}


VOID CBackGround::SetRenderState(INT nState)
{
	RECT tRect;
	SetRect(&tRect,0,0,640,480);

	AllStopAvi();
	switch(m_nRenderState)
	{
		case _RENDER_AVI_IN:
		case _RENDER_AVI_OUT:
			m_xCreateChrAvi.Stop();
			break;
		case _RENDER_AVI_START:
			m_xStartGameAvi.Stop();
			break;
	}

	m_nRenderState = nState;

	switch(m_nRenderState)
	{
		case _RENDER_AVI_IN:
		{
			m_xCreateChrAvi.Create(AVI_CREATECHAR, FALSE);
			m_xCreateChrAvi.SetDestRect(tRect);
			m_xCreateChrAvi.Start();
			break;
		}
		case _RENDER_AVI_OUT:
		{
			m_xCreateChrAvi.Create(AVI_CREATECHAR, FALSE,FALSE);
			m_xCreateChrAvi.SetDestRect(tRect);
			m_xCreateChrAvi.Start();
			break;
		}
		case _RENDER_AVI_START:
		{
			m_xStartGameAvi.Create(AVI_STARTGAME,FALSE);
			m_xStartGameAvi.SetDestRect(tRect);
			m_xStartGameAvi.Start();
			break;
		}
	}
}

INT CBackGround::GetRenderState(VOID)
{
	return m_nRenderState;
}
VOID CBackGround::AllStopAvi(VOID)
{
	m_xCreateChrAvi.Stop();
	m_xStartGameAvi.Stop();
}