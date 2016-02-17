#include "stdafx.h"

#define GENDER_MALE		0
#define GENDER_FEMALE	1

#define JOB_WOR			0
#define JOB_TAO			1
#define JOB_SOR			2

CCreateChr::CCreateChr()
{
	m_pxImage = NULL;
	m_nSelectGender=GENDER_MALE;
	m_nSelectJob=JOB_WOR;
}

CCreateChr::~CCreateChr()
{

}

VOID CCreateChr::Create(CWHWilImageData* pxImage)
{
	INT	nLoop;
	// Buttons
	BUTTONINFO CharProcBtnInfo[] = {
									{ IMG_IDX_EMPTY_BTN_UP, 	CREATE_CHAR_OK_BTN_POS_X,		CREATE_CHAR_OK_BTN_POS_Y,		WIDTH_EMPTY_BTN,	HEIGHT_EMPTY_BTN},	// Create Charector Ok Button
									{ IMG_IDX_WAR_BTN_UP,	 	CREATE_CHAR_WAR_BTN_POS_X,		CREATE_CHAR_BTN_POS_Y,			WIDTH_JOB_BTN,		HEIGHT_JOB_BTN},	// 전사 Button
									{ IMG_IDX_SOR_BTN_UP,		CREATE_CHAR_SOR_BTN_POS_X,		CREATE_CHAR_BTN_POS_Y,			WIDTH_JOB_BTN,		HEIGHT_JOB_BTN},	// 술사 Button
									{ IMG_IDX_TAO_BTN_UP, 		CREATE_CHAR_TAO_BTN_POS_X,		CREATE_CHAR_BTN_POS_Y,			WIDTH_JOB_BTN,		HEIGHT_JOB_BTN}		// 도사 Button
									};

	m_pxImage		= pxImage;
	for(nLoop = 0 ; nLoop < 4; nLoop++)
	{
		m_xButtons[nLoop].SetBtn(&CharProcBtnInfo[nLoop]);
	}
}

HRESULT	CCreateChr::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	if (wParam == VK_RETURN || wParam == VK_TAB)
	{
		return 1;
	}
	return 0;
}

HRESULT CCreateChr::OnButtonDown(WPARAM wParam, LPARAM lParam)
{
//	INT i;
//	RECT tRect;

	m_fIsButtonDown = TRUE;

	return 0;
}

HRESULT CCreateChr::OnButtonDown(POINT ptMouse)
{
	m_fIsButtonDown = TRUE;
	return 0;
}

HRESULT	CCreateChr::OnButtonUp(WPARAM wParam, LPARAM lParam)
{
//	INT	i;

	m_fIsButtonDown = FALSE;

	return 0;
}

HRESULT CCreateChr::OnButtonUp(POINT ptMouse)
{

	return 0;
}

LRESULT	CCreateChr::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
/*	INT	i;

	if(!m_fIsButtonDown)
	{
		for( i = BTN_NEW_ID ; i <= BTN_OK_ID ; i ++)
		{
			if (m_xButtons[i].CheckMouseOn(LOWORD(lParam), HIWORD(lParam)))
				m_xButtons[i].m_nState = BUTTON_STATE_ON;
			else
				m_xButtons[i].m_nState = BUTTON_STATE_UP;
		}
	}
*/
	return 0;
}

VOID CCreateChr::Render(INT	nLoopTime)
{
	INT i ;
	POINT nPos[] = { {120, 88}, {120, 85}, {150, 108}};

	if(m_fIsActive)
	{
		MoveWindow(g_xChatEditBox.GetSafehWnd(), 
						   g_xMainWnd.m_rcWindow.left+280,
						   g_xMainWnd.m_rcWindow.top+400, 75, 15, TRUE);

		for (i = ID_CREATE_CHR_OK_BTN ; i <= ID_CREATE_CHR_TAO_BTN; i++)
		{
			m_pxImage->NewSetIndex(m_xButtons[i].m_nButtonID + m_xButtons[i].m_nState -1);
			g_xMainWnd.DrawWithImageForComp(m_xButtons[i].m_Rect.left, m_xButtons[i].m_Rect.top,
										m_pxImage->m_lpstNewCurrWilImageInfo->shWidth,
										m_pxImage->m_lpstNewCurrWilImageInfo->shHeight					
										, (WORD*)(m_pxImage->m_pbCurrImage));
		}

		// 임시~!
		m_pxImage->NewSetIndex(IMG_IDX_WAR_MALE_CREATE + m_nSelectGender + m_nSelectJob * 2);

		g_xMainWnd.DrawWithImageForComp(nPos[m_nSelectJob].x,nPos[m_nSelectJob].y
				,m_pxImage->m_lpstNewCurrWilImageInfo->shWidth,m_pxImage->m_lpstNewCurrWilImageInfo->shHeight
				,(WORD*)(m_pxImage->m_pbCurrImage));
	}
}

/*
#define GENDER_MALE		0
#define GENDER_FEMALE	1

#define JOB_WOR			0
#define JOB_TAO			1
#define JOB_SOR			2
*/