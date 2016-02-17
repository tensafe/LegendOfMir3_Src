#include "stdafx.h"

#define CHR_ANI_COUNT			21
#define ANIMATION_TICK			200
#define BLEND_TICK				50


CSelectChr::CSelectChr()
{
	m_nRenderState = 0;
	m_nAnimationCount = 0;
	m_nRoll = 0;
	m_nBlend = -999;
	m_fIsButtonDown	= FALSE;
}

CSelectChr::~CSelectChr()
{
	if(m_pBtn!=NULL)
	{
		delete[] m_pBtn;
	}
}

VOID CSelectChr::Create(CWHWilImageData* pxImage,CPDLList<CHRINFO>* pxList, CPDLList<CHRRECT>* pxrcList,INT nChrCount)
{
	INT	nLoop;
	// Buttons
	BUTTONINFO CharProcBtnInfo[] ={
									{ IMG_IDX_SELECT_CREATE,	SELECT_CHAR_NEW_BTN_POS_X,		SELECT_CHAR_NEW_BTN_POS_Y,		0,					0},	// Create New Charector Button
									{ IMG_IDX_SELECT_DELETE,	SELECT_CHAR_ERASE_BTN_POS_X,	SELECT_CHAR_ERASE_BTN_POS_Y,	0,					0},	// Erase Charector Button
									{ IMG_IDX_SELECT_EXIT,		SELECT_CHAR_EXIT_BTN_POS_X,		SELECT_CHAR_EXIT_BTN_POS_Y,		0,					0},	// Exit Button
									{ IMG_IDX_SELECT_START,		SELECT_CHAR_START_BTN_POS_X,	SELECT_CHAR_START_BTN_POS_Y,	0,					0},	// Start Button									
									};

	m_pBtn = new CMirButton*[sizeof(CharProcBtnInfo)/sizeof(BUTTONINFO)];

	m_pxImage			= pxImage;
	m_pxSList			= pxList;
	m_pxrcList			= pxrcList;
	m_nChrCount			= nChrCount;
	
	m_xLightFog.Init(RGB(125,125,125));

	for( nLoop = 0 ; nLoop < (sizeof(CharProcBtnInfo)/sizeof(BUTTONINFO)) ; nLoop++)
	{
		m_pBtn[nLoop] = new CMirButton;
		m_pxImage->NewSetIndex(CharProcBtnInfo[nLoop].nID);
		CharProcBtnInfo[nLoop].right = m_pxImage->m_lpstNewCurrWilImageInfo->shWidth;
		CharProcBtnInfo[nLoop].bottom = m_pxImage->m_lpstNewCurrWilImageInfo->shHeight;
		m_pBtn[nLoop]->SetBtn(&CharProcBtnInfo[nLoop]);
	}
}

VOID CSelectChr::OpenWnd(VOID)
{
	m_fIsActive = TRUE;
	m_nRenderState = 9;
	m_nBlend = 100;
}
HRESULT	CSelectChr::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

HRESULT CSelectChr::OnButtonDown(WPARAM wParam, LPARAM lParam)
{
	if(m_fIsActive)
		for (int i = ID_SEL_CHR_NEW_BTN; i <= ID_SEL_CHR_START_BTN; i++)
		{
			if (IsInRect(m_pBtn[i]->m_Rect,LOWORD(lParam), HIWORD(lParam)))
			{
				m_pBtn[i]->m_nState = BUTTON_STATE_DOWN;
				m_fIsButtonDown = TRUE;
			}
			else
				m_pBtn[i]->m_nState = BUTTON_STATE_UP;
		}

	return 0;
}

HRESULT CSelectChr::OnButtonDown(POINT ptMouse)
{
	m_fIsButtonDown = TRUE;
	return 0;
}

HRESULT	CSelectChr::OnButtonUp(WPARAM wParam, LPARAM lParam)
{
	INT		i;
	int		Px,Py;
	if(m_fIsActive)
	{
		for(i = ID_SEL_CHR_NEW_BTN; i <= ID_SEL_CHR_START_BTN; i++)
		{	m_pBtn[i]->m_nState = BUTTON_STATE_UP;
			if (IsInRect(m_pBtn[i]->m_Rect,LOWORD(lParam), HIWORD(lParam)))
			{	switch(i)
				{	
					case ID_SEL_CHR_NEW_BTN:
					{
						if(m_nChrCount<3)
						{	m_nBlend = 0;
							m_nRenderState = 1;
							return RETURN_SELECT_NEW_BTN;
						}
						else
							return RETURN_SELECT_NEW_ERR;
						break;
					}
					case ID_SEL_CHR_DEL_BTN:
					{
						if(m_nChrCount!=0)
						{
							return RETURN_SELECT_DEL_BTN;
						}
						else
							return RETURN_SELECT_DEL_ERR;
						break;
					}
					case ID_SEL_CHR_EXIT_BTN:
					{
						SendMessage(g_xMainWnd.GetSafehWnd(), WM_DESTROY, NULL, NULL);
						return 0L;
					}
					case ID_SEL_CHR_START_BTN:
					{
						if(m_nSelectChr != SELECT_NONE)
						{	m_nBlend = 0;
							m_pxSList->MoveCurrentToTop();
							m_pxSList->MoveNode(m_nSelectChr);
							g_xClientSocket.OnSelChar(m_pxSList->GetCurrentData()->ChrName);
							return RETURN_SELECT_START_BTN;
						}
						else
						{
							return RETURN_SELECT_START_ERR;
						}
						break;
					}
				}
			}
		}

		m_pxrcList->MoveCurrentToTop();
		m_pxSList->MoveCurrentToTop();
		for(i=0; i < m_pxrcList->ListLength(); i++)
		{
			Px = m_pxrcList->GetCurrentData()->nLeft - LOWORD(lParam);
			Py = m_pxrcList->GetCurrentData()->nTop - HIWORD(lParam);
			if(Px < 0 && ((Px +m_pxrcList->GetCurrentData()->nWidth)>0))
			{
				if(Py<0 && ((Py+m_pxrcList->GetCurrentData()->nHeight)>0))
				{
					m_nSelectChr = i;

				}
			}
			m_pxrcList->MoveNextNode();
		}
	}

	m_fIsButtonDown = FALSE;

	return 0;
}

HRESULT CSelectChr::OnButtonUp(POINT ptMouse)
{

	return 0;
}

LRESULT	CSelectChr::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	INT	i;
	BOOL	fOnButton = FALSE;
	if(m_fIsActive&&(!m_fIsButtonDown))
	{
		for (i = ID_SEL_CHR_NEW_BTN; i <= ID_SEL_CHR_START_BTN; i++)
		{
			if (IsInRect(m_pBtn[i]->m_Rect,LOWORD(lParam), HIWORD(lParam)))
			{
				m_pBtn[i]->m_nState = BUTTON_STATE_DOWN;
				fOnButton = TRUE;
			}
			else
				m_pBtn[i]->m_nState = BUTTON_STATE_UP;
		}

/*		if(fOnButton)
		{
			HCURSOR	hCursor;
			hCursor = LoadCursor(g_xMainWnd.GetSafehInstance(),MAKEINTRESOURCE(IDC_HAND1));
			SetCursor(hCursor);
			DestroyCursor(hCursor); 
		}
		else
			g_xMainWnd.OnSetCursor(IDC_ARROW);
*/
	}
	return 0;
}

VOID CSelectChr::Render(INT	nLoopTime)
{
	static DWORD	dwTicks = 0;
	int i;
	INT	nPx,nPy;
	POINT	ptPos[]	=	{ 
						{310,310},	// One
						{300,270},	// Two-One
						{360,310},	// Two-Two
						{250,270},	// Three-One
						{320,310},	// Three-Two
						{390,340} 
						};// Three-Three	

	if(m_fIsActive)
	{
		m_xLightFog.ClearSavedLightColor();

		for (i = ID_SEL_CHR_NEW_BTN; i <= ID_SEL_CHR_START_BTN ; i++)	// 단추 임시
		{
			if(m_pBtn[i]->m_nState==BUTTON_STATE_DOWN)		// Draw Button when Button state is Downed..
			{
				m_pxImage->NewSetIndex(m_pBtn[i]->m_nButtonID);

				g_xMainWnd.DrawWithImageForComp(m_pBtn[i]->m_Rect.left, m_pBtn[i]->m_Rect.top,
											m_pxImage->m_lpstNewCurrWilImageInfo->shWidth,
											m_pxImage->m_lpstNewCurrWilImageInfo->shHeight					
											, (WORD*)(m_pxImage->m_pbCurrImage));
			}
		}

		dwTicks +=nLoopTime;

		if(dwTicks > BLEND_TICK)
		{
			m_nBlend = m_nBlend +((m_nRenderState != 0)? dwTicks / BLEND_TICK *(m_nRenderState != 9 ? 1 : -1): 0);
			m_nRoll = (m_nRoll+ 2)%100;
			if(dwTicks > ANIMATION_TICK)
			{
				m_nAnimationCount++;
				m_nAnimationCount%=CHR_ANI_COUNT;
				dwTicks = 0;
			}
		}

		m_pxSList->MoveCurrentToTop();
		for( i = 0 ; i < m_nChrCount ; i ++)
		{
				// Draw Person
				m_pxImage->NewSetIndex(IDX_IMG_MALE_WAR 
										+ (m_pxSList->GetCurrentData()->Gender) * 60
										+ (m_pxSList->GetCurrentData()->Class) * 120 
										+ ((i == m_nSelectChr) ? m_nAnimationCount : 0)) ;
				nPx = ptPos[i + m_nChrCount - 1].x + m_pxImage->m_lpstNewCurrWilImageInfo->shPX;
				nPy = ptPos[i + m_nChrCount - 1].y + m_pxImage->m_lpstNewCurrWilImageInfo->shPY;

				g_xMainWnd.DrawWithABlendCompDataWithBackBuffer(nPx,nPy
								,m_pxImage->m_lpstNewCurrWilImageInfo->shWidth,m_pxImage->m_lpstNewCurrWilImageInfo->shHeight
								,(WORD*)(m_pxImage->m_pbCurrImage),640,480,0xffff,0xffff,((m_nBlend>=0)? m_nBlend : 0));

				// Draw Fire
				if(i == m_nSelectChr)
				{
					m_pxImage->NewSetIndex(IDX_IMG_MALE_WAR_FIRE 
											+ (m_pxSList->GetCurrentData()->Gender) * 60
											+ (m_pxSList->GetCurrentData()->Class) * 120 
											+ (m_nAnimationCount)) ;

					nPx = ptPos[i + m_nChrCount - 1].x + m_pxImage->m_lpstNewCurrWilImageInfo->shPX;
					nPy = ptPos[i + m_nChrCount - 1].y + m_pxImage->m_lpstNewCurrWilImageInfo->shPY;

					g_xMainWnd.DrawWithABlendCompDataWithBackBuffer(nPx,nPy
									,m_pxImage->m_lpstNewCurrWilImageInfo->shWidth,m_pxImage->m_lpstNewCurrWilImageInfo->shHeight
									,(WORD*)(m_pxImage->m_pbCurrImage),640,480,0xffff,0x1f1f,((m_nBlend>=0)? m_nBlend : 50));

					INT nTemp = (INT)(125.0f * sin((double)(m_nRoll/10)));

					m_xLightFog.SetLightRadiusWithCircle(nPx+100,nPy+100,((m_nBlend>=0) ? 5 - ((m_nBlend)/20) : 5),125 + nTemp,255 + nTemp,255 + nTemp);
				}

			m_pxSList->MoveNextNode();
		}
		m_xLightFog.ShowLightFog();

		ShowChrInfo();
		if((m_nBlend>=100 && m_nRenderState!=9)||(m_nBlend<=0 && m_nRenderState==9))
		{
			m_nBlend = -1;
			m_nRenderState = m_nRenderState + 10;
		}

	}
}

VOID CSelectChr::ShowChrInfo(VOID)
{
	char ChrName[20];
	char ChrLev[3];
	if(m_nChrCount!=0)
	{
		m_pxSList->MoveCurrentToTop();
		m_pxSList->MoveNode(m_nSelectChr);
		strcpy(ChrName,m_pxSList->GetCurrentData()->ChrName);
		strcpy(g_szCharName,m_pxSList->GetCurrentData()->ChrName);
		itoa(m_pxSList->GetCurrentData()->Level,ChrLev,10);
		g_xMainWnd.PutsHan(NULL,m_PosName.x,m_PosName.y,RGB(255,255,255),RGB(0,0,0),ChrName);
	}
}


VOID CSelectChr::CloseWnd(VOID)
{
	m_fIsActive = FALSE;
}
