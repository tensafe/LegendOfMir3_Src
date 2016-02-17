#include "stdafx.h"

#define _RENDER_AVI_NONE		0
#define _RENDER_AVI_INTRO		1
#define _RENDER_AVI_LOGO		2
#define _RENDER_AVI_STILL		9

#define _LEFT_INTRO				0
#define _TOP_INTRO				60
#define _RIGHT_INTRO			640
#define _BOTTOM_INTRO			420

#define _INTRO_FILE_NAME		".\\ei_login.avi"
#define _LOGO_FILE_NAME			".\\ei_logo.avi"

CLoginAvi::CLoginAvi():CBMWnd()
{
	m_pxImage			= NULL;

	m_nRenderAviState	= _RENDER_AVI_NONE;

	SetRect(&m_rcWnd,0,0,640,480);
	SetRect(&m_rcSrc,_LEFT_INTRO,_TOP_INTRO,_RIGHT_INTRO,_BOTTOM_INTRO);
}

CLoginAvi::~CLoginAvi()
{
	StopAllAvis();
	m_xAvi.ReleaseAvi();
	m_xLogoAvi.ReleaseAvi();
}

VOID CLoginAvi::Create(CWHWilImageData* pxImage)
{
	m_pxImage = pxImage;
	m_xAvi.InitAvi(g_xSound.GetSoundObject()->GetDS());
	m_xLogoAvi.InitAvi(g_xSound.GetSoundObject()->GetDS());
	m_xAvi.Create(_INTRO_FILE_NAME,FALSE);
	m_xLogoAvi.Create(_LOGO_FILE_NAME,FALSE);
	m_xAvi.SetDestRect(m_rcSrc);
	m_xLogoAvi.SetDestRect(m_rcSrc);

}

VOID CLoginAvi::SetRenderAviState(INT nState)
{
	switch(m_nRenderAviState)
	{
		case _RENDER_AVI_INTRO:
		{
			m_xAvi.Stop();			
			break;
		}
		case _RENDER_AVI_LOGO:
		{
			m_xLogoAvi.Stop();
			break;
		}
	}
	m_nRenderAviState = nState;
	switch(m_nRenderAviState)
	{
		case _RENDER_AVI_INTRO:
		{
			m_xAvi.Start();			
			break;
		}
		case _RENDER_AVI_LOGO:
		{
			m_xLogoAvi.Start();
			break;
		}
	}
}

HRESULT CLoginAvi::OnButtonDown(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

HRESULT CLoginAvi::OnButtonDown(POINT ptMouse)
{
	return 0;
}

HRESULT CLoginAvi::OnButtonUp(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

HRESULT CLoginAvi::OnButtonUp(POINT ptMouse)
{
	return 0;
}

LRESULT CLoginAvi::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

VOID CLoginAvi::Render(INT	nLoopTime)
{
	switch(m_nRenderAviState)
	{
		case _RENDER_AVI_INTRO:
		{	if(!m_xAvi.Draw(nLoopTime))
				m_nRenderAviState = _RENDER_AVI_STILL;
			break;
		}
		case _RENDER_AVI_LOGO:
		{
//			if(!m_xLogoAvi.DrawBlend(nLoopTime))
				m_nRenderAviState = _RENDER_AVI_STILL;
			break;
		}
		case _RENDER_AVI_STILL:
		{	m_pxImage->NewSetIndex(IMG_IDX_LOGIN_BACK);
			g_xMainWnd.DrawWithImageForComp(_LEFT_INTRO, _TOP_INTRO,
											m_pxImage->m_lpstNewCurrWilImageInfo->shWidth,
											m_pxImage->m_lpstNewCurrWilImageInfo->shHeight,
											(WORD*)(m_pxImage->m_pbCurrImage));
			break;
		}
	}
}


VOID CLoginAvi::StopAllAvis(VOID)
{
	m_xAvi.Stop();
	m_xLogoAvi.Stop();
}
