/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/



#include "StdAfx.h"



CChatPopWnd::CChatPopWnd()
{
	Init();
}

CChatPopWnd::~CChatPopWnd()
{
	Destroy();
}


VOID CChatPopWnd::Init()
{
	CGameWnd::Init();

	m_nCanScrlCnt		 = 0;
	m_nCurrStartChatLine = 0;
	SetRect(&m_rcChatPopFrame, 0, 0, 0, 0);	
	SetRect(&m_rcEditBoxFrame, 0, 0, 0, 0);

	m_xChatPopBtn.Init();
}

VOID CChatPopWnd::Destroy()
{
	m_xstrDividedChat.ClearAllNodes();
	Init();
}


VOID CChatPopWnd::CreateChatPopWnd(INT nID, CWHWilImageData* pxWndImage, INT nFrameImgIdx, INT nStartX, INT nStartY, INT nWidth, INT nHeight, BOOL bCanMove)
{
	CreateGameWnd(nID, pxWndImage, nFrameImgIdx, bCanMove, nStartX, nStartY, nWidth, nHeight);
	SetRect(&m_rcChatPopFrame, 40, 29, 531, 308);
	SetRect(&m_rcEditBoxFrame, 36, 312, 535, 328);

	m_xChatPopBtn.CreateGameBtn(pxWndImage, 372, 373, nStartX+542, nStartY+353);
}

BOOL CChatPopWnd::MsgAdd(DWORD dwFontColor, DWORD dwFontBackColor, CHAR* szMsg)
{
	CHAR szChatMsg[MAX_PATH];

	if ( szMsg != NULL )
	{
		strcpy(szChatMsg, szMsg);
		
		if ( szChatMsg[0] != NULL )
		{
			INT nLineCnt;
			CHAR szDivied[MAX_PATH*2];
			CHAR szArg[5][MAX_PATH];

			ZeroMemory(szDivied, MAX_PATH*2);
			ZeroMemory(&szArg[0], MAX_PATH*5);

			g_xMainWnd.StringDivide(m_rcChatPopFrame.right-m_rcChatPopFrame.left, nLineCnt, szChatMsg, szDivied);

			sscanf(szDivied, "%[^`]%*c %[^`]%*c %[^`]%*c %[^`]%*c %[^`]%*c", szArg[0], szArg[1], szArg[2], szArg[3], szArg[4]);
			if ( nLineCnt > 5 )		nLineCnt = 5;

			for ( INT nCnt = 0; nCnt < nLineCnt; nCnt++ )
			{
				if ( m_nCanScrlCnt )
					m_nCurrStartChatLine = m_nCanScrlCnt;
					
				if ( m_xstrDividedChat.GetCounter() >= _MAX_CHATLINE_POPUP )
				{
					m_nCurrStartChatLine++;
					m_nCanScrlCnt++;
				}
				CHATSTRING stChatStr;
				stChatStr.dwFontColor = dwFontColor;
				stChatStr.dwFontBackColor = dwFontBackColor;
				stChatStr.strChat = szArg[nCnt];
				m_xstrDividedChat.AddNode(stChatStr);
			}
		}
		return TRUE;
	}
	return FALSE;
}

VOID CChatPopWnd::ShowChatPopWnd()
{
	ShowGameWnd();

	// 채팅 리스트를 보여준다.
	if ( !m_xstrDividedChat.CheckEmpty() )
	{
		m_xstrDividedChat.MoveCurrentToTop();
		m_xstrDividedChat.MoveNode(m_nCurrStartChatLine);

		INT nLine = 0;
		INT nMaxLine;
		if ( !m_nCurrStartChatLine )
		{
			if ( !m_nCanScrlCnt )
				nMaxLine = m_xstrDividedChat.GetCounter();
			else
				nMaxLine = _MAX_CHATLINE_POPUP;
		}
		else
			nMaxLine = (m_nCurrStartChatLine+_MAX_CHATLINE_POPUP) > m_xstrDividedChat.GetCounter() ?
					   m_xstrDividedChat.GetCounter() : m_nCurrStartChatLine+_MAX_CHATLINE_POPUP;

		for ( INT nCnt = m_nCurrStartChatLine; nCnt < nMaxLine; nCnt++ )
		{
			LPCHATSTRING pstChatString;
			pstChatString = m_xstrDividedChat.GetCurrentData();
			//g_xMainWnd.PutsHan(g_xMainWnd.GetBackBuffer(),
			//				   m_rcWnd.left+m_rcChatPopFrame.left, m_rcWnd.top+m_rcChatPopFrame.top+nLine*14,
			//				   pstChatString->dwFontBackColor, pstChatString->dwFontColor, pstChatString->strChat.begin());

			m_xstrDividedChat.MoveNextNode();
			nLine++;
		}
	}

	m_xChatPopBtn.ShowGameBtn();
}

BOOL CChatPopWnd::OnLButtonUp(POINT ptMouse)
{
	MoveWindow(g_xChatEditBox.GetSafehWnd(), g_xMainWnd.m_rcWindow.left + m_rcWnd.left + m_rcEditBoxFrame.left, 
	 		   g_xMainWnd.m_rcWindow.top + m_rcWnd.top + m_rcEditBoxFrame.top, 
			   m_rcEditBoxFrame.right - m_rcEditBoxFrame.left, 
			   m_rcEditBoxFrame.bottom - m_rcEditBoxFrame.top, TRUE);

	if ( m_xChatPopBtn.OnLButtonUp(ptMouse) )		return TRUE;

	return FALSE;
}

BOOL CChatPopWnd::OnLButtonDown(POINT ptMouse)
{
	if ( m_xChatPopBtn.OnLButtonDown(ptMouse) )		return TRUE;

	return FALSE;
}

VOID CChatPopWnd::OnMouseMove(POINT ptMouse)
{
	m_xChatPopBtn.ChangeRect(m_rcWnd.left+542, m_rcWnd.top+353);
	m_xChatPopBtn.OnMouseMove(ptMouse);
}

VOID CChatPopWnd::OnScrollDown()
{
	if ( m_nCurrStartChatLine > 0 )															m_nCurrStartChatLine--;
}

VOID CChatPopWnd::OnScrollUp()
{
	if ( m_nCurrStartChatLine < m_xstrDividedChat.GetCounter()-_MAX_CHATLINE_POPUP )		m_nCurrStartChatLine++;
}

VOID CChatPopWnd::SetStatusBtnInit()
{
	m_xChatPopBtn.SetBtnState(_BTN_STATE_NORMAL);
}
