#include "stdafx.h"

#define LOGIN_BUTTON_COUNT			3
#define LOGIN_INPUT_COUNT			2
#define LOGIN_IMAGE_COUNT			4



CNewAccount::CNewAccount()
{

}

CNewAccount::~CNewAccount()
{

}

VOID CNewAccount::Create(CWHWilImageData* pxImage)
{
/*	INT	nLoop;
	// Buttons
	BUTTONINFO LoginProcBtnInfo[] = {
									{ IMG_IDX_BTN_NEW,		POS_BTN_NEW_X,		POS_BTN_NEW_Y,			LOGBTN_WIDTH,		LOGBTN_HEIGHT}, // OK Button
									{ IMG_IDX_BTN_CHGPASS,	POS_BTN_CHG_X,		POS_BTN_CHG_Y,			LOGBTN_WIDTH,		LOGBTN_HEIGHT},	// New Account
									{ IMG_IDX_BTN_OK,		POS_BTN_OK_X,		POS_BTN_OK_Y,			LOGBTN_WIDTH,		LOGBTN_HEIGHT}	// Change Password
									};
	// EditBox State
	INPUTSTATE LoginInputState[] =  {
									{ LGM_INPUT_ID,				POS_ID_INS_X,		POS_ID_INS_Y,			126,				15,			0,				10,				"\0"},			// Input ID in Login
									{ LGM_INPUT_PASSWORD,		POS_PASS_INS_X,		POS_PASS_INS_Y,			126,				15,			0,				10,				"\0"}			// Input Password In Login
									};

	_LOGINIMAGEINFO LoginImageInfo[]	=	{
											{IMG_IDX_ID,		POS_ID_TITLE_X,		POS_ID_TITLE_Y},
											{IMG_IDX_PASS,		POS_PASS_TITLE_X,	POS_PASS_TITLE_Y},
											{IMG_IDX_ID_INS,	POS_ID_INS_X,		POS_ID_INS_Y},
											{IMG_IDX_PASS_INS,	POS_PASS_INS_X,		POS_PASS_INS_Y}
										};

	m_pxImage		= pxImage;

	
	for(nLoop = 0 ; nLoop < LOGIN_INPUT_COUNT ; nLoop ++)
		memcpy(&m_xInputState[nLoop],&LoginInputState[nLoop],sizeof(INPUTSTATE));
	for(nLoop = 0 ; nLoop < LOGIN_BUTTON_COUNT ; nLoop ++)
		m_xButtons[nLoop].SetBtn(&LoginProcBtnInfo[nLoop]);
	for(nLoop = 0 ; nLoop < LOGIN_IMAGE_COUNT ; nLoop ++)
		m_xInputImg[nLoop] = LoginImageInfo[nLoop];*/
}

HRESULT	CNewAccount::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
/*	if (wParam == VK_RETURN || wParam == VK_TAB)
	{	SetFocusBefore();
		switch(m_nUserState)
		{	case  LGM_INPUT_ID:
			{	m_nUserState = LGM_INPUT_PASSWORD;
				break;
			}
			case LGM_INPUT_PASSWORD:
			{	if( ( lstrlen( m_xInputState[LGM_INPUT_ID-1].szData) >= LIMIT_USERID ) 
								&& lstrlen( m_xInputState[LGM_INPUT_PASSWORD-1].szData ) )
				{
					g_xClientSocket.OnLogin(m_xInputState[LGM_INPUT_ID-1].szData,m_xInputState[LGM_INPUT_PASSWORD-1].szData);
					return 0L;
				}						
				m_nUserState = LGM_INPUT_ID;							
				break;
			}
		}
		SetFocusAfter();
	}*/
	return 0;
}

HRESULT CNewAccount::OnButtonDown(WPARAM wParam, LPARAM lParam)
{
/*	INT i;
	RECT tRect;

	m_fIsButtonDown = TRUE;

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

	// Image Button Down Check
	for ( i = BTN_NEW_ID; i <= BTN_OK_ID ; i++)
	{	if (m_xButtons[i].CheckMouseOn( LOWORD( lParam ), HIWORD( lParam ) ) )
			m_xButtons[i].m_nState = BUTTON_STATE_DOWN;
		else
			m_xButtons[i].m_nState = BUTTON_STATE_UP;
	}*/
	return 0;
}

HRESULT CNewAccount::OnButtonDown(POINT ptMouse)
{
	m_fIsButtonDown = TRUE;
	return 0;
}

HRESULT	CNewAccount::OnButtonUp(WPARAM wParam, LPARAM lParam)
{
/*	INT	i;

	m_fIsButtonDown = FALSE;

	for(i = BTN_NEW_ID; i <= BTN_OK_ID; i++)
	{	m_xButtons[i].m_nState = BUTTON_STATE_UP;
		
		if( m_xButtons[i].CheckMouseOn( LOWORD( lParam ), HIWORD( lParam ) ) )
		{	switch(m_xButtons[i].m_nButtonID)
			{	case IMG_IDX_BTN_NEW:
				{
					//m_nProgress = PRG_NEW_ACCOUNT;	- > New Account로 넘기는 함수
					SetFocusBefore();
					SetWindowText(g_xChatEditBox.GetSafehWnd(),NULL);
					m_nUserState = LGM_INPUT_USER_ID;
					SetFocusAfter();
					break;
				}
			case IMG_IDX_BTN_CHGPASS:
				{	// 임시로 종료버튼으로 이용
					//	Avi 관련 함수 종료 하는 코드
					//	break; 
				}
//						case BTN_EXIT:
				{
					SendMessage(g_xMainWnd.GetSafehWnd(), WM_DESTROY, NULL, NULL);
					return 0L;break;
				}
			case IMG_IDX_BTN_OK:
				{
					SendMessage(g_xChatEditBox.GetSafehWnd(),WM_CHAR,VK_RETURN,0);
					g_xClientSocket.OnLogin(m_xInputState[LGM_INPUT_ID-1].szData,m_xInputState[LGM_INPUT_PASSWORD-1].szData);
					break;
				}
			}// switch
		}// if
	}// for*/
	return 0;
}

HRESULT CNewAccount::OnButtonUp(POINT ptMouse)
{

	return 0;
}

LRESULT	CNewAccount::OnMouseMove(WPARAM wParam, LPARAM lParam)
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

VOID CNewAccount::Render(INT	nLoopTime)
{
/*	if(m_fIsActive)
	{
		int i;
		char Pass[16]="";

		MoveWindow(g_xChatEditBox.GetSafehWnd(), 
					g_xMainWnd.m_rcWindow.left + m_xInputState[m_nUserState-1].Left +5,
					g_xMainWnd.m_rcWindow.top + m_xInputState[m_nUserState-1].Top + 5 ,	m_xInputState[m_nUserState-1].Width,		m_xInputState[m_nUserState-1].Height,		TRUE);


		// Draw ID & Pass Image
		for( i = IMG_IDX_ID-1 ; i < IMG_IDX_PASS_INS ; i ++)
		{
			m_pxImage->NewSetIndex(m_xInputImg[i].nImgIdx);
			g_xMainWnd.DrawWithImageForComp(m_xInputImg[i].Left , m_xInputImg[i].Top,
						m_pxImage->m_lpstNewCurrWilImageInfo->shWidth,
						m_pxImage->m_lpstNewCurrWilImageInfo->shHeight,
						(WORD*)(m_pxImage->m_pbCurrImage));
		}

		// Draw Button Image
		for ( i = BTN_NEW_ID ; i <=  BTN_OK_ID ; i++)
		{
				m_pxImage->NewSetIndex(m_xButtons[i].m_nButtonID + m_xButtons[i].m_nState - 1 );
				g_xMainWnd.DrawWithImageForComp(m_xButtons[i].m_Rect.left, m_xButtons[i].m_Rect.top, 
											m_xButtons[i].m_Rect.right - m_xButtons[i].m_Rect.left, 
											m_xButtons[i].m_Rect.bottom - m_xButtons[i].m_Rect.top, (WORD*)(m_pxImage->m_pbCurrImage));
		}

		// Write ID & Password
		memset(Pass,'*',strlen(m_xInputState[1].szData));
		g_xMainWnd.PutsHan(NULL, m_xInputState[0].Left+7, m_xInputState[0].Top+6, RGB(5,5,5), RGB(0,0,0), m_xInputState[0].szData);
		g_xMainWnd.PutsHan(NULL, m_xInputState[1].Left+7, m_xInputState[1].Top+6, RGB(5,5,5), RGB(0,0,0), Pass);
	}*/
}


VOID CNewAccount::SetFocusBefore(VOID)
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

VOID CNewAccount::SetFocusAfter(VOID)
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