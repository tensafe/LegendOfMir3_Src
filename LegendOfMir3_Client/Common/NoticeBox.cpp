#include "stdafx.h"

#define WRITESTRLENTH 50

#define BTN_POS_X	165
#define BTN_POS_Y	483

#define _IDX_UP_BTN_IMG			1
#define _IDX_ON_BTN_IMG			2
#define _IDX_DOWN_BTN_IMG		3


CNoticeBox::CNoticeBox()
{
	m_bActive = FALSE;
	m_bInited = FALSE;
	m_bIsSelected = FALSE;
	m_bSet = FALSE;
	ZeroMemory(&m_rcWnd,sizeof(RECT));
}

BOOL CNoticeBox::Load(CWHWilImageData* pxBtnImage=NULL)
{
	if(pxBtnImage!=NULL)
	{
		// Init Button;
		m_bSet			=	TRUE;
		m_pxBtnImage	=	pxBtnImage;
		m_xMsgBtn.CreateGameBtn(pxBtnImage, _IDX_ON_BTN_IMG, _IDX_DOWN_BTN_IMG, BTN_POS_X,BTN_POS_Y);
	}
	else
	{
		m_bSet = FALSE;
	}
	return m_bSet;
}

BOOL CNoticeBox::DestoryMessageBox(VOID)						// DestoryMessage box
{
	m_bActive = FALSE;
	m_bInited = FALSE;
	m_bSet = FALSE;
	m_xMsg.ClearAllNodes();
	return FALSE;
}

BOOL CNoticeBox::HideMessageBox(VOID)							// Hide Message box
{
	m_bActive = FALSE;
	return FALSE;
}

CNoticeBox::~CNoticeBox()										// Destructor
{
}

BOOL CNoticeBox::SetBoxImage(INT nImageIndex)
{
	POINT xSize,xPos;
	if(m_bSet)
	{
		m_pxBtnImage->NewSetIndex(nImageIndex);
		m_wpData = (WORD*)m_pxBtnImage->m_pbCurrImage;
		m_shWidth = m_pxBtnImage->m_lpstNewCurrWilImageInfo->shWidth;
		m_shHeight = m_pxBtnImage->m_lpstNewCurrWilImageInfo->shHeight;
		xSize.x = g_xMainWnd.m_rcWindow.right - g_xMainWnd.m_rcWindow.left;
		xSize.y = g_xMainWnd.m_rcWindow.bottom - g_xMainWnd.m_rcWindow.top;
		xPos.x = (xSize.x - m_shWidth)/2;
		xPos.y = (xSize.y - m_shHeight)/2;

		SetRect(&m_rcWnd, xPos.x, xPos.y, xPos.x + m_shWidth, xPos.y + m_shHeight);
	}
	return m_bSet;
}

BOOL CNoticeBox::ShowMessageBox(CHAR* szMsg,INT nImgIndex) // Show	Message box with Initialization
{
	INT Lenth;
	if(!m_bActive)
	{
		if(SetBoxImage(nImgIndex))
		{
			if(ShowMessage(szMsg))
			{
				m_xMsg.ClearAllNodes();
				StringDivideLen(WRITESTRLENTH,Lenth, m_szMsg,&m_xMsg);

				m_xMsgBtn.ChangeRect(m_rcWnd.left + BTN_POS_X, m_rcWnd.top + BTN_POS_Y);
			}
			else
			{
				m_bActive = FALSE;
			}
		}
		else
		{
			m_bActive = FALSE;
			m_bInited = FALSE;
		}
	}
	return m_bActive;
}

// Message Process
HRESULT CNoticeBox::OnButtonDown(POINT tpMouse)		// Button Down
{
	HRESULT hResult=0;

	if(m_xMsgBtn.OnLButtonDown(tpMouse))
	{
		g_xClientSocket.SendNoticeOK();
		hResult = 1;
	}

	if(!hResult)
	{
		if(IsInRect(tpMouse.x,tpMouse.y,m_rcWnd))
		{
			m_bGrabbedMousePos.x  = tpMouse.x - m_rcWnd.left;
			m_bGrabbedMousePos.y  = tpMouse.y - m_rcWnd.top;
			m_bIsSelected = TRUE;
			hResult = 0;
		}
	}

	return hResult;
}

HRESULT	CNoticeBox::OnButtonUp(POINT tpMouse)			// Button Up
{
	m_bIsSelected = FALSE; 
	HRESULT hResult=0;
	if(m_xMsgBtn.OnLButtonUp(tpMouse))
		hResult = 1; // Here
	return hResult;
}

// Render Process
BOOL CNoticeBox::RenderMessageBox(INT nLoopTIme)							// Render Function
{
	INT	nCount;

	if(m_bActive&&m_bInited&&m_bSet)
	{	
		g_xMainWnd.DrawWithImageForCompClipRgn( m_rcWnd.left, m_rcWnd.top, m_shWidth, m_shHeight, m_wpData, 800, 600 );

		POINT	xTxtPos;
		nCount = m_xMsg.GetCounter();
		m_xMsg.MoveCurrentToTop();

		xTxtPos.x = m_rcWnd.left + 40;
		xTxtPos.y = m_rcWnd.top	 + 20;

		for(int i = 0 ; i < nCount ; i++)
		{
			g_xMainWnd.PutsHan(NULL,xTxtPos.x,xTxtPos.y,RGB(255,255,255),RGB(0,0,0),*m_xMsg.GetCurrentData());
			xTxtPos.y = xTxtPos.y + 16;	// Line Skip

			m_xMsg.MoveNextNode();
		}
		// Draw Button
		m_xMsgBtn.ShowGameBtn();

		return FALSE;
	}
	return TRUE;
}

VOID CNoticeBox::MoveWnd(POINT ptMouse)					// Move Window
{
	if ( m_bActive && m_bIsSelected)
	{
		SetRect(&m_rcWnd, ptMouse.x-m_bGrabbedMousePos.x, ptMouse.y-m_bGrabbedMousePos.y,
				ptMouse.x-m_bGrabbedMousePos.x+m_shWidth, ptMouse.y-m_bGrabbedMousePos.y+m_shHeight);
		m_xMsgBtn.ChangeRect(m_rcWnd.left + BTN_POS_X, m_rcWnd.top + BTN_POS_Y);
	}
	else
	{
		m_xMsgBtn.OnMouseMove(ptMouse);
	}
}


BOOL CNoticeBox::ShowMessage(CHAR* szMsg)		// Show	Message box with Initialization
{
	if(SetMsg(szMsg))
		m_bActive = TRUE;
	else
		m_bActive = FALSE;

	return m_bActive;
}

BOOL CNoticeBox::SetMsg(CHAR* szMsg)				// Init Messages
{
	INT nLength;
	nLength = strlen(szMsg);
	if(nLength!=0)
	{
		ZeroMemory(m_szMsg,MAX_PATH);
		strcpy(m_szMsg,szMsg);
		m_bInited = TRUE;
	}
	else
		m_bInited = FALSE;

	return m_bInited;
}

BOOL CNoticeBox::StringDivideLen(INT nDivideLen, INT& nDividedLine, CHAR* szSrc,CDLList<CHAR*>* m_pxpStr)
{
	CHAR* szTmpCheck;
	CHAR szResult[MAX_PATH];

	INT nStartLen	= 0;
	INT nEndLen		= 0;
	INT nResultLen	= 0;
	INT nWordCheck	= 0;

	nDividedLine = 1;

	if ( szSrc[0] != NULL )
	{
		ZeroMemory(szResult,MAX_PATH);

		for ( INT nCnt = 0; nCnt < (INT)strlen(szSrc); nCnt++)
		{
			nEndLen = nCnt+1;

			// 현재부분까지의 문자열을 읽는다.
			szTmpCheck = szSrc+nStartLen;
			INT nsLen;
			nsLen = strlen(szTmpCheck);

			// 현재까지 읽어온 부분이 허용넓이를 넘어섰을때.
			if (nsLen > nDivideLen )
			{
				// 현재 2바이트 문자라면. 
				if ( szSrc[nEndLen-1] < 0 )
				{
					// 현재 앞에문자가 2바이트 문자가 아니라면
					if ( !(nWordCheck%2) )
					{
						nStartLen += strlen(szTmpCheck)-1;
						nCnt--;
						CHAR* pszNewLine;
						pszNewLine = new CHAR[nResultLen+1];
						memcpy(pszNewLine,szResult,nResultLen);
						pszNewLine[nResultLen]=NULL;
						m_pxpStr->AddNode(pszNewLine);
						nResultLen = 0;
						nDividedLine++;
					}
					else
					{
						nStartLen += strlen(szTmpCheck)-2;
						nCnt -= 2;
						CHAR* pszNewLine;
						pszNewLine = new CHAR[nResultLen];
						memcpy(pszNewLine,szResult,nResultLen-1);
						pszNewLine[nResultLen-1]=NULL;
						m_pxpStr->AddNode(pszNewLine);
						nResultLen = 0;
						nDividedLine++;
						nWordCheck--;
					}
				}
				// 1바이트 문자. 
				// 현재보다 한바이트앞부분까지만 고려해주면 된다.
				else
				{
					nStartLen += strlen(szTmpCheck)-1;
					nCnt--;
					CHAR* pszNewLine;
					pszNewLine = new CHAR[nResultLen+1];
					memcpy(pszNewLine,szResult,nResultLen);
					pszNewLine[nResultLen]=NULL;
					m_pxpStr->AddNode(pszNewLine);
					nResultLen=0;
					nDividedLine++;
				}
			}
			else if(szSrc[nEndLen-1] == '\n' || szSrc[nEndLen-1] == 0x1B)	// 강제개행     \n으로하면  Error ????

			{
				nStartLen += strlen(szTmpCheck)-1;
				CHAR* pszNewLine;
				pszNewLine = new CHAR[nResultLen+1];
				memcpy(pszNewLine,szResult,nResultLen);
				pszNewLine[nResultLen]=NULL;
				m_pxpStr->AddNode(pszNewLine);
				nResultLen=0;
				nDividedLine++;
			}
			else
			{
				if ( szSrc[nEndLen-1] < 0 )
					nWordCheck++;

				szResult[nResultLen] = szSrc[nEndLen-1];
				nResultLen++;
			}
		}
		if(nResultLen!=0)
		{
			CHAR* pszNewLine;
			pszNewLine = new CHAR[nResultLen+1];
			memcpy(pszNewLine,szResult,nResultLen);
			pszNewLine[nResultLen]=NULL;
			m_pxpStr->AddNode(pszNewLine);
			nDividedLine++;
			nResultLen=0;
		}
		return TRUE;
	}
	return FALSE;
}