#include "stdafx.h"

#define POS_SERVERLIST_BORDER_X		50
#define POS_SERVERLIST_BORDER_Y		0
#define SERVERLIST_BORDER_WIDTH		151

#define POS_TOP_SERVER_BTN_X		0
#define POS_TOP_SERVER_BTN_Y		10

#define SERVER_BTN_WIDTH			100
#define SERVER_BTN_HEIGHT			20
#define	SERVER_BTN_GAP				10
#define COUNT_BUTTON_PER_COLUME		8

#define SERVER_BTN_TEXTCOLOR_NORMAL	RGB(10, 10, 10)			// 어두운 색
#define SERVER_BTN_TEXTCOLOR_FOCUS	RGB(0, 255, 255)		// YELLOW
#define SERVER_BTN_TEXTCOLOR_CLICK	RGB(0,0,0)				// COLORKEY

#define SERVER_BTN_BKCOLOR_NORMAL	RGB(125,125,125)				// 
#define SERVER_BTN_BKCOLOR_FOCUS	RGB(95,95,95)				//
#define SERVER_BTN_BKCOLOR_CLICK	RGB(125,125,0)			//  YELLOW

#define _LEFT_BORDER				20
#define _TOP_BORDER					60
#define _RIGHT_BORDER				120
#define _BOTTOM_BORDER				310

CSelectSrv::CSelectSrv():CBMWnd()
{
	m_ppxSrvBtn = NULL;
	D3DVECTOR vNorm(0, 0, -1);
    m_avBillBoard[0]  = D3DVERTEX(D3DVECTOR(-0.5f, 0.5f, 0), vNorm, 0, 0);
    m_avBillBoard[1]  = D3DVERTEX(D3DVECTOR(-0.5f,-0.5f, 0), vNorm, 0, 1);
    m_avBillBoard[2]  = D3DVERTEX(D3DVECTOR( 0.5f, 0.5f, 0), vNorm, 1, 0);
    m_avBillBoard[3]  = D3DVERTEX(D3DVECTOR( 0.5f,-0.5f, 0), vNorm, 1, 1);
}

CSelectSrv::~CSelectSrv()
{
	INT	i;
	for(i = 0 ; i <m_nSrvCount; i++)
		delete m_ppxSrvBtn[i];
	delete[] m_ppxSrvBtn;
}

VOID CSelectSrv::Create(CWHWilImageData* pxImage,CPDLList<ServerList>* pxSList)
{
	INT		i,j;
	INT		nTop,nLeft;
	RECT	rcTemp;
	// Buttons
	m_nSrvCount = pxSList->GetCounter() + 1;
	m_ppxSrvBtn	= new _PSERVERBUTTON[m_nSrvCount];
	m_pxImage		= pxImage;

	pxSList->MoveCurrentToTop();
	for(i = 0 ; i <= ((m_nSrvCount - 1) / COUNT_BUTTON_PER_COLUME) ; i++)
	{	for(j = i * COUNT_BUTTON_PER_COLUME; (j < m_nSrvCount && j < (COUNT_BUTTON_PER_COLUME * (i + 1))) ; j ++)
		{	m_ppxSrvBtn[j] = new _SERVERBUTTON;

			strcpy(m_ppxSrvBtn[j]->szName, pxSList->GetCurrentData()->Name);
			strcpy(m_ppxSrvBtn[j]->szCaption, pxSList->GetCurrentData()->Caption);
			nLeft = POS_TOP_SERVER_BTN_X + i * 100;
			nTop  = POS_TOP_SERVER_BTN_Y + (j - i * COUNT_BUTTON_PER_COLUME) * (SERVER_BTN_HEIGHT+SERVER_BTN_GAP);
			SetRect(&rcTemp, nLeft, nTop, nLeft + SERVER_BTN_WIDTH, nTop + SERVER_BTN_HEIGHT);
			m_ppxSrvBtn[j]->rcBtn = rcTemp;
			m_ppxSrvBtn[j]->dwTxtColor = SERVER_BTN_TEXTCOLOR_NORMAL;
			m_ppxSrvBtn[j]->dwBkColor = SERVER_BTN_BKCOLOR_NORMAL;
			m_ppxSrvBtn[j]->nAlpha = 125;
			pxSList->MoveNextNode();

		}
	}
	SetRect(&m_rcWnd,_LEFT_BORDER,_TOP_BORDER,_LEFT_BORDER+SERVER_BTN_WIDTH*(m_nSrvCount/COUNT_BUTTON_PER_COLUME+1),(m_nSrvCount/COUNT_BUTTON_PER_COLUME) > 0 ? _BOTTOM_BORDER : POS_TOP_SERVER_BTN_Y + (m_nSrvCount * COUNT_BUTTON_PER_COLUME) * (SERVER_BTN_HEIGHT+SERVER_BTN_GAP));
}

HRESULT	CSelectSrv::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	return 0;
}

HRESULT CSelectSrv::OnButtonDown(WPARAM wParam, LPARAM lParam)
{
	INT i;
	RECT	tRect;
	m_fIsButtonDown = TRUE;

	for( i = 0 ;  i < m_nSrvCount  ; i++)
	{	SetRect(&tRect,m_rcWnd.left + m_ppxSrvBtn[i]->rcBtn.left, m_rcWnd.top + m_ppxSrvBtn[i]->rcBtn.top, m_rcWnd.left +m_ppxSrvBtn[i]->rcBtn.right ,m_rcWnd.top +m_ppxSrvBtn[i]->rcBtn.bottom);
		if(IsInRect(tRect,LOWORD(lParam), HIWORD(lParam)))
		{	m_ppxSrvBtn[i]->dwTxtColor	= SERVER_BTN_TEXTCOLOR_CLICK;			// COLOR(BLACK)
			m_ppxSrvBtn[i]->dwBkColor	= SERVER_BTN_BKCOLOR_CLICK;	
			m_ppxSrvBtn[i]->nAlpha		= 125;
		}
	}

	return 0;
}

HRESULT CSelectSrv::OnButtonDown(POINT ptMouse)
{
	m_fIsButtonDown = TRUE;

	return 0;
}

HRESULT	CSelectSrv::OnButtonUp(WPARAM wParam, LPARAM lParam)
{
	INT	i;
	RECT tRect;

	m_fIsButtonDown = FALSE;

	for( i = 0 ;  i < m_nSrvCount  ; i++)
	{	SetRect(&tRect,m_rcWnd.left + m_ppxSrvBtn[i]->rcBtn.left, m_rcWnd.top + m_ppxSrvBtn[i]->rcBtn.top, m_rcWnd.left +m_ppxSrvBtn[i]->rcBtn.right ,m_rcWnd.top +m_ppxSrvBtn[i]->rcBtn.bottom);
		if(IsInRect(tRect,LOWORD(lParam), HIWORD(lParam)))
		{
			g_xClientSocket.OnSelectServer(m_ppxSrvBtn[i]->szName);
		}
		m_ppxSrvBtn[i]->dwTxtColor	= SERVER_BTN_TEXTCOLOR_NORMAL;			// COLOR(YELLOW)
		m_ppxSrvBtn[i]->dwBkColor	= SERVER_BTN_BKCOLOR_NORMAL;
		m_ppxSrvBtn[i]->nAlpha		= 125;
	}

	return 0;
}


HRESULT CSelectSrv::OnButtonUp(POINT ptMouse)
{
	return 0;
}

LRESULT	CSelectSrv::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	INT	i;
	RECT tRect;

	if(!m_fIsButtonDown)
		for( i = 0 ;  i < m_nSrvCount  ; i++)
		{	SetRect(&tRect,m_rcWnd.left + m_ppxSrvBtn[i]->rcBtn.left, m_rcWnd.top + m_ppxSrvBtn[i]->rcBtn.top, m_rcWnd.left +m_ppxSrvBtn[i]->rcBtn.right ,m_rcWnd.top +m_ppxSrvBtn[i]->rcBtn.bottom);
			if(IsInRect(tRect,LOWORD(lParam), HIWORD(lParam)))
			{	m_ppxSrvBtn[i]->dwTxtColor	= SERVER_BTN_TEXTCOLOR_FOCUS;			// COLOR(YELLOW)
				m_ppxSrvBtn[i]->dwBkColor	= SERVER_BTN_BKCOLOR_FOCUS;
				m_ppxSrvBtn[i]->nAlpha		= 125;
			}
			else
			{	m_ppxSrvBtn[i]->dwTxtColor	= SERVER_BTN_TEXTCOLOR_NORMAL;		// COLOR(WHITE)
				m_ppxSrvBtn[i]->dwBkColor	= SERVER_BTN_BKCOLOR_NORMAL;
				m_ppxSrvBtn[i]->nAlpha		= 125;
			}
		}
	return 0;
}

VOID CSelectSrv::Render(INT	nLoopTime)
{
	INT		i,j;
	RECT	tRect;

	COLORREF nBkColor;
	if(m_fIsActive)
	{
		// Write Texts;
		Draw3DBorder(m_rcWnd,RGB(125,125,125),125);
		for(i = 0 ; i <= ((m_nSrvCount - 1) / COUNT_BUTTON_PER_COLUME) ; i++)
		{
			for(j = i * COUNT_BUTTON_PER_COLUME; (j < m_nSrvCount && j < (COUNT_BUTTON_PER_COLUME * (i + 1))) ; j ++)
			{	nBkColor = m_ppxSrvBtn[j]->dwBkColor;
				SetRect(&tRect,m_rcWnd.left + m_ppxSrvBtn[j]->rcBtn.left, m_rcWnd.top + m_ppxSrvBtn[j]->rcBtn.top, m_rcWnd.left +m_ppxSrvBtn[j]->rcBtn.right ,m_rcWnd.top +m_ppxSrvBtn[j]->rcBtn.bottom);
				if(nBkColor!=RGB(125,125,125))Draw3DBorder(tRect, nBkColor,m_ppxSrvBtn[j]->nAlpha);								
				g_xMainWnd.PutsHan(NULL, tRect,
										m_ppxSrvBtn[j]->dwTxtColor, RGB(0,0,0), m_ppxSrvBtn[j]->szCaption,g_xMainWnd.CreateGameFont("명조",12 ));
			}
		}
	}
}


VOID CSelectSrv::Draw3DBorder(RECT rcWnd,COLORREF dwColor,INT nAlpha)
{
	if ( g_xMainWnd.Get3DDevice() )
	{
		D3DVECTOR vTrans;
		D3DMATRIX matTrans;
		D3DMATRIX matScale;
		D3DMATRIX matWorld;
		D3DMATRIX matView;
		D3DMATRIX matProj;
		D3DMATRIX matWorldOriginal;
		D3DMATERIAL7 mtrl;
		if( SUCCEEDED(g_xMainWnd.Get3DDevice()->BeginScene()))
		{
			g_xMainWnd.Get3DDevice()->GetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

			ZeroMemory(&matView, sizeof(D3DMATRIX));
		 	D3DVECTOR vEyePt    = D3DVECTOR(0, 0, -(float)((g_xMainWnd.m_rcWindow.bottom - g_xMainWnd.m_rcWindow.top)/2));
			D3DVECTOR vLookatPt = D3DVECTOR(0, 0, 0);
			D3DVECTOR vUpVec    = D3DVECTOR(0, 1, 0);

			D3DUtil_SetViewMatrix(matView, vEyePt, vLookatPt, vUpVec);		
			D3DUtil_SetProjectionMatrix(matProj, g_PI/2, float(240.0f/320.0f), -1.0f, 1.0f);

			g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_VIEW,       &matView);
			g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &matProj);

			vTrans.x = (FLOAT)rcWnd.left - 320.0f + (rcWnd.right  - rcWnd.left)/2;
			vTrans.y = (FLOAT)-rcWnd.top + 240.0f - (rcWnd.bottom - rcWnd.top )/2;
			vTrans.z = 0;

			D3DUtil_SetTranslateMatrix(matTrans, vTrans);
			D3DUtil_SetScaleMatrix(matScale, (FLOAT)(rcWnd.right - rcWnd.left), (FLOAT)(rcWnd.bottom - rcWnd.top), 0.0f);
			D3DMath_MatrixMultiply(matWorld, matScale, matTrans);
			g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);

			// 임시
			D3DUtil_InitMaterial(mtrl, 
								 (FLOAT)GetRValue(dwColor)/255.0f,
								 (FLOAT)GetGValue(dwColor)/255.0f, 
								 (FLOAT)GetBValue(dwColor)/255.0f);
			mtrl.diffuse.a = nAlpha/255.0f;					// alpha value

			g_xMainWnd.Get3DDevice()->SetMaterial(&mtrl);

			g_xMainWnd.Get3DDevice()->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
			g_xMainWnd.Get3DDevice()->SetRenderState( D3DRENDERSTATE_COLORKEYENABLE, TRUE);
			g_xMainWnd.Get3DDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			g_xMainWnd.Get3DDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
			g_xMainWnd.Get3DDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

			g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
			g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);

			g_xMainWnd.Get3DDevice()->SetTexture(0, NULL);
			g_xMainWnd.Get3DDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX, m_avBillBoard, 4, NULL);

			g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
			g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, FALSE);
			g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_SRCBLEND , D3DBLEND_ONE);
			g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);
			g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

			g_xMainWnd.Get3DDevice()->EndScene();
		}
	}
}