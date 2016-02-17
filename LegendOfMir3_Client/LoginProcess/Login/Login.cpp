#include "stdafx.h"

#define LOGIN_BUTTON_COUNT			4
#define LOGIN_INPUT_COUNT			2
#define LOGIN_IMAGE_COUNT			4

#define POS_ID_INS_X				193
#define POS_ID_INS_Y				435
#define POS_PASS_INS_X				390
#define POS_PASS_INS_Y				435

#define _LEFT_LOGIN					0
#define _TOP_LOGIN					372
#define _RIGHT_LOGIN				640
#define _BOTTOM_LOGIN				480

#define _LEFT_ID_PASS				170
#define _TOP_ID_PASS				65

#define _IMG_IDX_LOGIN_BORDER		1
#define _IMG_IDX_LOGIN_IDPASS		2
#define _IMG_IDX_BTN_LOGIN			10
#define _IMG_IDX_BTN_NEW			12
#define _IMG_IDX_BTN_CHANGE			14
#define _IMG_IDX_BTN_EXIT			16


#define _LEFT_LOGIN_BTN				140
#define _LEFT_NEW_BTN				205
#define _LEFT_CHANGE_BTN			325
#define _LEFT_EXIT_BTN				475
#define _TOP_LOGIN_BTNS				8


CLogin::CLogin():CBMWnd()
{

}

CLogin::~CLogin()
{

}

VOID CLogin::Create(CWHWilImageData* pxImage)
{
	INT	nLoop;
	// Buttons
	BUTTONINFO LoginProcBtnInfo[] = {
									{ _IMG_IDX_BTN_LOGIN,		_LEFT_LOGIN_BTN,	_TOP_LOGIN_BTNS,			0,		0}, // Login Button
									{ _IMG_IDX_BTN_NEW,			_LEFT_NEW_BTN,		_TOP_LOGIN_BTNS,			0,		0},	// New Account
									{ _IMG_IDX_BTN_CHANGE,		_LEFT_CHANGE_BTN,	_TOP_LOGIN_BTNS,			0,		0},	// Change Password
									{ _IMG_IDX_BTN_EXIT,		_LEFT_EXIT_BTN,		_TOP_LOGIN_BTNS,			0,		0}	// Exit								
									};
	// EditBox State
	INPUTSTATE LoginInputState[] =  {
									{ LGM_INPUT_ID,				POS_ID_INS_X,		POS_ID_INS_Y,			100,				15,			0,				10,				"\0"},			// Input ID in Login
									{ LGM_INPUT_PASSWORD,		POS_PASS_INS_X,		POS_PASS_INS_Y,			100,				15,			0,				10,				"\0"}			// Input Password In Login
									};

	m_pxImage		= pxImage;

	
	for(nLoop = 0 ; nLoop < LOGIN_INPUT_COUNT ; nLoop ++)
		memcpy(&m_xInputState[nLoop],&LoginInputState[nLoop],sizeof(INPUTSTATE));

	for(nLoop = 0 ; nLoop < LOGIN_BUTTON_COUNT ; nLoop ++)
		m_xButtons[nLoop].SetBtn(&LoginProcBtnInfo[nLoop]);
	
	m_fShowIDPASS = FALSE;	

	ShowWindow(g_xChatEditBox.GetSafehWnd(), SW_HIDE);
	SetFocus(g_xMainWnd.GetSafehWnd());

	SetRect(&m_rcWnd,_LEFT_LOGIN,_TOP_LOGIN,_RIGHT_LOGIN,_BOTTOM_LOGIN);
}

HRESULT	CLogin::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	if(m_fShowIDPASS)
	{
		if (wParam == VK_RETURN || wParam == VK_TAB)
		{	SetFocusBefore();
			switch(m_nUserState)
			{	case  LGM_INPUT_ID:
				{	m_nUserState = LGM_INPUT_PASSWORD;
					break;
				}
				case LGM_INPUT_PASSWORD:
				{	if( ( lstrlen( m_xInputState[LGM_INPUT_ID-1].szData) >= LIMIT_USERID ) 
									&& lstrlen( m_xInputState[LGM_INPUT_PASSWORD-1].szData ) )
					{	g_xClientSocket.OnLogin(m_xInputState[LGM_INPUT_ID-1].szData,m_xInputState[LGM_INPUT_PASSWORD-1].szData);
						strcpy(g_szUserID,m_xInputState[LGM_INPUT_ID-1].szData);
						return 0L;
					}						
					m_nUserState = LGM_INPUT_ID;							
					break;
				}
			}
			SetFocusAfter();
		}
	}
	return 0;
}

HRESULT CLogin::OnButtonDown(WPARAM wParam, LPARAM lParam)
{
	INT i;
	RECT tRect;

	m_fIsButtonDown = TRUE;

	if(m_fShowIDPASS)
		for( i = LGM_INPUT_ID-1 ; i < LGM_INPUT_PASSWORD; i ++)
		{	SetRect(&tRect
					,m_xInputState[i].Left,m_xInputState[i].Top
					,m_xInputState[i].Left+m_xInputState[i].Width,m_xInputState[i].Top+m_xInputState[i].Height);
			if( IsInRect( tRect, LOWORD( lParam ), HIWORD( lParam ) ) )
			{	SetFocusBefore();
				m_nUserState = i+1;
				SetFocusAfter();
			}
		}
	return 0;
}

HRESULT CLogin::OnButtonDown(POINT ptMouse)
{
	m_fIsButtonDown = TRUE;
	return 0;
}

HRESULT	CLogin::OnButtonUp(WPARAM wParam, LPARAM lParam)
{
	INT		i;
	RECT	tRect;

	m_fIsButtonDown = FALSE;

	for(i = BTN_LOGIN_ID; i <= BTN_EXIT_ID; i++)
	{//	m_xButtons[i].m_nState = BUTTON_STATE_UP;		
		m_pxImage->NewSetIndex(m_xButtons[i].m_nButtonID + m_xButtons[i].m_nState - 1);
		SetRect(&tRect
				,m_xButtons[i].m_Rect.left + m_rcWnd.left ,m_xButtons[i].m_Rect.top + m_rcWnd.top 
				,m_xButtons[i].m_Rect.left + m_rcWnd.left + m_pxImage->m_lpstNewCurrWilImageInfo->shWidth
				,m_xButtons[i].m_Rect.top  + m_rcWnd.top + m_pxImage->m_lpstNewCurrWilImageInfo->shHeight);
		if (IsInRect(tRect,LOWORD( lParam ), HIWORD( lParam ) ) )
		{
			switch(m_xButtons[i].m_nButtonID)
			{	case _IMG_IDX_BTN_LOGIN:
				{
					m_fShowIDPASS = !m_fShowIDPASS;
					break;
				}
			case _IMG_IDX_BTN_NEW:
				{
					m_fShowIDPASS = FALSE;

					break; 
				}
			case _IMG_IDX_BTN_CHANGE:
				{
					m_fShowIDPASS = FALSE;

					break;
				}
			case _IMG_IDX_BTN_EXIT:
				{
					m_fShowIDPASS = FALSE;
					SendMessage(g_xMainWnd.GetSafehWnd(), WM_DESTROY, NULL, NULL);
					return 0L;break;
				}
			}// switch
		}// if
	}// for
	if(m_fShowIDPASS)
	{	ShowWindow(g_xChatEditBox.GetSafehWnd(), SW_SHOW);
		SetFocus(g_xChatEditBox.GetSafehWnd());
	}
		else
	{
		ShowWindow(g_xChatEditBox.GetSafehWnd(), SW_HIDE);
		SetFocus(g_xMainWnd.GetSafehWnd());
	}
	return 0;
}

HRESULT CLogin::OnButtonUp(POINT ptMouse)
{

	return 0;
}

LRESULT	CLogin::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	INT	i;
	RECT tRect;

	if(!m_fIsButtonDown)
	{
		for( i = BTN_LOGIN_ID ; i <= BTN_EXIT_ID ; i ++)
		{	m_pxImage->NewSetIndex(m_xButtons[i].m_nButtonID + m_xButtons[i].m_nState - 1);
			SetRect(&tRect
					,m_xButtons[i].m_Rect.left + m_rcWnd.left ,m_xButtons[i].m_Rect.top + m_rcWnd.top 
					,m_xButtons[i].m_Rect.left + m_rcWnd.left + m_pxImage->m_lpstNewCurrWilImageInfo->shWidth
					,m_xButtons[i].m_Rect.top  + m_rcWnd.top + m_pxImage->m_lpstNewCurrWilImageInfo->shHeight);
			if (IsInRect(tRect,LOWORD( lParam ), HIWORD( lParam ) ) )
				m_xButtons[i].m_nState = BUTTON_STATE_ON;
			else
				m_xButtons[i].m_nState = BUTTON_STATE_UP;
		}
	}

	return 0;
}

VOID CLogin::Render(INT	nLoopTime)
{
	int i;
	char Pass[16]="";

	if(m_fIsActive)
	{

		MoveWindow(g_xChatEditBox.GetSafehWnd(), 
					g_xMainWnd.m_rcWindow.left + m_xInputState[m_nUserState-1].Left +5,
					g_xMainWnd.m_rcWindow.top + m_xInputState[m_nUserState-1].Top + 5 ,	m_xInputState[m_nUserState-1].Width,		m_xInputState[m_nUserState-1].Height,		TRUE);
	
		ShowWindow(g_xChatEditBox.GetSafehWnd(), (m_fShowIDPASS ? SW_SHOW:SW_HIDE));
		// Draw Login Border
		if(!m_fShowIDPASS)
		{	m_pxImage->NewSetIndex(_IMG_IDX_LOGIN_BORDER);
			g_xMainWnd.DrawWithABlendCompDataWithBackBuffer(m_rcWnd.left ,m_rcWnd.top ,
															m_pxImage->m_lpstNewCurrWilImageInfo->shWidth ,m_pxImage->m_lpstNewCurrWilImageInfo->shHeight ,(WORD*)m_pxImage->m_pbCurrImage,640,480);
			// Draw Button Image
			for ( i = BTN_LOGIN_ID ; i <=  BTN_EXIT_ID ; i++)
			{
					m_pxImage->NewSetIndex(m_xButtons[i].m_nButtonID + m_xButtons[i].m_nState - 1);
					g_xMainWnd.DrawWithImageForComp(m_rcWnd.left + m_xButtons[i].m_Rect.left, m_rcWnd.top + m_xButtons[i].m_Rect.top, 
												m_pxImage->m_lpstNewCurrWilImageInfo->shWidth, 
												m_pxImage->m_lpstNewCurrWilImageInfo->shHeight, (WORD*)(m_pxImage->m_pbCurrImage));
			}
		}
		// Draw ID & Pass Image
		if(m_fShowIDPASS)
		{	m_pxImage->NewSetIndex(_IMG_IDX_LOGIN_IDPASS);
			g_xMainWnd.DrawWithImageForComp(_LEFT_ID_PASS,m_rcWnd.top + _TOP_ID_PASS,
												m_pxImage->m_lpstNewCurrWilImageInfo->shWidth ,m_pxImage->m_lpstNewCurrWilImageInfo->shHeight,(WORD*)m_pxImage->m_pbCurrImage);
			memset(Pass,'*',strlen(m_xInputState[1].szData));
			g_xMainWnd.PutsHan(NULL, m_xInputState[0].Left+7, m_xInputState[0].Top+6, RGB(255,255,255), RGB(0,0,0), m_xInputState[0].szData);
			g_xMainWnd.PutsHan(NULL, m_xInputState[1].Left+7, m_xInputState[1].Top+6, RGB(255,255,255), RGB(0,0,0), Pass);
		}		
	}
}


VOID CLogin::SetFocusBefore(VOID)
{
	INT nTemp;

	nTemp =  m_nUserState - 1;

	if(g_xChatEditBox.m_szInputMsg[0]!=NULL)
			lstrcpy(m_xInputState[nTemp].szData , g_xChatEditBox.m_szInputMsg);
		else
			GetWindowText(g_xChatEditBox.GetSafehWnd(),m_xInputState[nTemp].szData ,m_xInputState[nTemp].nSize);
		
	ZeroMemory(g_xChatEditBox.m_szInputMsg,sizeof(g_xChatEditBox.m_szInputMsg));
	ShowWindow(g_xChatEditBox.GetSafehWnd(), SW_HIDE);
	g_xChatEditBox.SetSelectAll();
}

VOID CLogin::SetFocusAfter(VOID)
{
	CHAR	cChr;
	INT		nTemp;

	ShowWindow(g_xChatEditBox.GetSafehWnd(), SW_SHOW);

	nTemp = m_nUserState - 1;

	g_xChatEditBox.SetLimitText(m_xInputState[nTemp].nSize);

	if( m_nUserState != LGM_INPUT_PASSWORD )
		cChr = NULL;
	else
		cChr = '*';

	SendMessage(g_xChatEditBox.GetSafehWnd(),EM_SETPASSWORDCHAR,(WPARAM)cChr,0);
	SetWindowText(g_xChatEditBox.GetSafehWnd(), m_xInputState[nTemp].szData);

	SetFocus(g_xChatEditBox.GetSafehWnd());
	g_xChatEditBox.SetSelectAll();
}
