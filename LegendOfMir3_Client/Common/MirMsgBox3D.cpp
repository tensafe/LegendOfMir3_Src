#include "stdafx.h"

#define WRITESTRLENTH	50

CMirMsgBox3D::CMirMsgBox3D()
{
	m_bCanMove = TRUE;
	m_bFocused = FALSE;
}

CMirMsgBox3D::~CMirMsgBox3D()
{

}

BOOL CMirMsgBox3D::Load(CWHWilImageData* pxImage)
{
	CMirMsgBox::Load(pxImage);
	m_pxWndImage = pxImage;
	D3DWILTextr_Invalidate(pxImage->m_szWilFileName,_WNDIMGINDX_MSGBOX1);
	D3DWILTextr_DestroyTexture(pxImage->m_szWilFileName,_WNDIMGINDX_MSGBOX1);

	pxImage->NewSetIndex(_WNDIMGINDX_MSGBOX1);
	D3DWILTextr_CreateEmptyTexture(pxImage->m_szWilFileName,_WNDIMGINDX_MSGBOX1,pxImage->m_lpstNewCurrWilImageInfo->shWidth,
									pxImage->m_lpstNewCurrWilImageInfo->shHeight,(WORD*)pxImage->m_pbCurrImage,D3DTEXTR_TRANSPARENTBLACK);

	D3DWILTextr_Invalidate(pxImage->m_szWilFileName,_WNDIMGINDX_MSGBOX2);
	D3DWILTextr_DestroyTexture(pxImage->m_szWilFileName,_WNDIMGINDX_MSGBOX2);
	pxImage->NewSetIndex(_WNDIMGINDX_MSGBOX2);
	D3DWILTextr_CreateEmptyTexture(pxImage->m_szWilFileName,_WNDIMGINDX_MSGBOX2,pxImage->m_lpstNewCurrWilImageInfo->shWidth,
									pxImage->m_lpstNewCurrWilImageInfo->shHeight,(WORD*)pxImage->m_pbCurrImage,D3DTEXTR_TRANSPARENTBLACK);
	return TRUE;
}

BOOL CMirMsgBox3D::CreateWnd(CWHWilImageData* pxWndImage, BOOL bCanMove, INT nStartX, INT nStartY, INT nWidth, INT nHeight)
{
	m_nID			= _WND_ID_MESSAGEBOX;
	m_pxWndImage	= pxWndImage;
	m_bCanMove		= bCanMove;
	
	Load(pxWndImage);

	m_pxWndImage->NewSetIndex(_WNDIMGINDX_MSGBOX1);
	SetRect(&m_rcImgWnd, nStartX, nStartY, 
			nStartX+m_pxWndImage->m_lpstNewCurrWilImageInfo->shWidth,
			nStartY+m_pxWndImage->m_lpstNewCurrWilImageInfo->shHeight);

	( ! nWidth )  ?	m_nWidth = m_rcImgWnd.right -m_rcImgWnd.left: m_nWidth  = nWidth;
	( ! nHeight ) ?	m_nHeight =m_rcImgWnd.bottom-m_rcImgWnd.top : m_nHeight = nHeight;

	SetRect(&m_rcWnd, nStartX, nStartY, nStartX+m_nWidth, nStartY+m_nHeight);

 	D3DVECTOR vNorm(0, 0, -1);
    m_avMagic[0]  = D3DVERTEX(D3DVECTOR(-0.5f, 0.5f, 0), vNorm, 0, 0);
    m_avMagic[1]  = D3DVERTEX(D3DVECTOR(-0.5f,-0.5f, 0), vNorm, 0, 1);
    m_avMagic[2]  = D3DVERTEX(D3DVECTOR( 0.5f, 0.5f, 0), vNorm, 1, 0);
    m_avMagic[3]  = D3DVERTEX(D3DVECTOR( 0.5f,-0.5f, 0), vNorm, 1, 1);

	return FALSE;
}

BOOL CMirMsgBox3D::ShowMessageBox(CHAR* szMgs,INT nType,INT nImgIndex)	// Show	Message box with Initialization
{
	m_nFrameImgIdx = (nType&128) ? _WNDIMGINDX_MSGBOX2:_WNDIMGINDX_MSGBOX1;

	D3DWILTextr_Invalidate(m_pxWndImage->m_szWilFileName, m_nFrameImgIdx);
	D3DWILTextr_Restore(m_pxWndImage->m_szWilFileName, m_nFrameImgIdx, g_xMainWnd.Get3DDevice());

	CMirMsgBox::ShowMessageBox(szMgs,nType,nImgIndex);
	m_bActive = TRUE;
	return TRUE;
}

BOOL CMirMsgBox3D::HideMessageBox(VOID)									// Hide Message box
{
	m_bActive = FALSE;
	return FALSE;
}

BOOL CMirMsgBox3D::DestoryMessageBox(VOID)								// DestoryMessage box
{
	m_bActive = FALSE;
	m_bInited = FALSE;
	m_bSet = FALSE;
	m_xMsg.ClearAllNodes();
	return TRUE;
}

BOOL CMirMsgBox3D::OnKeyDown(WPARAM wParam,LPARAM lParam)
{
	return TRUE;
}

BOOL CMirMsgBox3D::RenderMessageBox(INT nLoopTIme)
{
	INT	nCount;

	RenderBox();

	if(m_bActive&&m_bInited&&m_bSet)
	{
		// 진짜 찍어야 할때만 찍을것
		// 찍을 이미지 영역 확인
		POINT	xTxtPos;

		m_xBoxPos.x = m_rcWnd.left;
		m_xBoxPos.y = m_rcWnd.top;

		nCount = m_xMsg.GetCounter();
		m_xMsg.MoveCurrentToTop();

		xTxtPos.x = m_xBoxPos.x +24;
		switch(nCount)
		{
		case 1:	// 찍을 줄이 1개 뿐일때
			{
				xTxtPos.y = 68;
				INT n = strlen(m_szMsg);
				if(n<WRITESTRLENTH)
				{
					xTxtPos.x= xTxtPos.x + (WRITESTRLENTH-n)*2;
				}
				break;
			}
		case 2:	// 찍을 줄이 2개 일때
			{
				xTxtPos.y = 61;
				break;
			}
		case 3:	// 찍을 줄이 3개 일때
			{
				xTxtPos.y = 54;
				break;
			}
		case 4:	// 찍을 줄이 4개 일때
			{
				xTxtPos.y = 47;
				break;
			}
		case 5:	// 찍을 줄이 5개 일때
			{
				xTxtPos.y = 40;
				break;
			}
		case 6:	// 찍을 줄이 6개 일때
			{
				xTxtPos.y = 33;
				break;
			}
		default: // 찍을 줄이 7개 이상일 경우
			{
				xTxtPos.y = 26;
				break;
			}
		}

		for(int i = 0 ; i < nCount ; i++)
		{
			g_xMainWnd.PutsHan(NULL,xTxtPos.x,m_xBoxPos.y+xTxtPos.y,RGB(255,255,255),RGB(0,0,0),*m_xMsg.GetCurrentData());
			xTxtPos.y = xTxtPos.y + 16;
			m_xMsg.MoveNextNode();
		}

		// Draw Button
		switch(m_nType)
		{
		case MSG_BTN_YESNO:
		case MSG_BTN_YESNO|MSG_EDITEXIST:
			{
				m_nStart=1;
				m_nEnd = 3;
				
				m_xMsgBtn[1].ChangeRect(m_xBoxPos.x+78,m_xBoxPos.y+134);
				m_xMsgBtn[2].ChangeRect(m_xBoxPos.x+210,m_xBoxPos.y+134);
				
				break;
			}
		case MSG_BTN_YESNOCANCLE:
		case MSG_BTN_YESNOCANCLE|MSG_EDITEXIST:
			{
				m_nStart=1;
				m_nEnd = 4;
				
				m_xMsgBtn[1].ChangeRect(m_xBoxPos.x+54,m_xBoxPos.y+134);
				m_xMsgBtn[2].ChangeRect(m_xBoxPos.x+144,m_xBoxPos.y+134);
				m_xMsgBtn[3].ChangeRect(m_xBoxPos.x+234,m_xBoxPos.y+134);
				break;
			}
		case MSG_BTN_OK:
			{
				m_nStart=0;
				m_nEnd = 1;
				
				m_xMsgBtn[0].ChangeRect(m_xBoxPos.x+144,m_xBoxPos.y+134);
				break;
			}
		case MSG_BTN_OK|MSG_EDITEXIST:
		default:
			{
				m_nStart=0;
				m_nEnd = 1;
				
				m_xMsgBtn[0].ChangeRect(m_xBoxPos.x+260,m_xBoxPos.y+134);
				break;
			}
		}
		for(i = m_nStart; i< m_nEnd; i++)
		{
			m_xMsgBtn[i].ShowGameBtn();
		}
		return FALSE;
	}
	return TRUE;
}

BOOL CMirMsgBox3D::RenderBox(VOID)
{
	if ( m_bActive )
	{
		if ( g_xMainWnd.Get3DDevice() )
		{
			D3DVECTOR vTrans;
			D3DMATRIX matTrans;
			D3DMATRIX matScale;
			D3DMATRIX matWorld;
			D3DMATRIX matWorldOriginal;
			D3DMATERIAL7 mtrl;
			if( SUCCEEDED(g_xMainWnd.Get3DDevice()->BeginScene()))
			{
				g_xMainWnd.Get3DDevice()->GetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

				m_pxWndImage->NewSetIndex(m_nFrameImgIdx);

				vTrans.x = (FLOAT)m_rcWnd.left+(FLOAT)(m_rcImgWnd.right-m_rcImgWnd.left)/2-400;
				vTrans.y = (FLOAT)-m_rcWnd.top-(FLOAT)(m_rcImgWnd.bottom-m_rcImgWnd.top)/2+300;
				vTrans.z = 0;
				D3DUtil_SetTranslateMatrix(matTrans, vTrans);
				D3DUtil_SetScaleMatrix(matScale, (FLOAT)m_rcImgWnd.right-m_rcImgWnd.left, (FLOAT)m_rcImgWnd.bottom-m_rcImgWnd.top, 0.0f);
				D3DMath_MatrixMultiply(matWorld, matScale, matTrans);
				g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);

				if ( m_bFocused )
				{
					g_xMainWnd.Get3DDevice()->SetTexture(0, D3DWILTextr_GetSurface(m_pxWndImage->m_szWilFileName, m_nFrameImgIdx));
					D3DUtil_InitMaterial(mtrl, 
										 (FLOAT)255/255.0f,
										 (FLOAT)255/255.0f, 
										 (FLOAT)255/255.0f);
					mtrl.diffuse.a = 200/255.0f;
					g_xMainWnd.Get3DDevice()->SetMaterial(&mtrl);
				}
				else
				{
					g_xMainWnd.Get3DDevice()->SetTexture(0, D3DWILTextr_GetSurface(m_pxWndImage->m_szWilFileName, m_nFrameImgIdx));
					D3DUtil_InitMaterial(mtrl,
										 (FLOAT)255/255.0f,
										 (FLOAT)255/255.0f,
										 (FLOAT)255/255.0f);
					mtrl.diffuse.a = 170/255.0f;
					g_xMainWnd.Get3DDevice()->SetMaterial(&mtrl);
				}

				g_xMainWnd.Get3DDevice()->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
				g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, TRUE);
				g_xMainWnd.Get3DDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				g_xMainWnd.Get3DDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
				g_xMainWnd.Get3DDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

				g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
				g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);

				g_xMainWnd.Get3DDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX, m_avMagic, 4, NULL);

				g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
				g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, FALSE);
				g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_SRCBLEND , D3DBLEND_ONE);
				g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);
				g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

				g_xMainWnd.Get3DDevice()->EndScene();
			}
		}
	}
	return TRUE;
}

VOID CMirMsgBox3D::MoveWnd(POINT ptMouse)		// Move Window
{
	if ( m_bActive && m_bFocused && m_bCanMove )
	{
		SetRect(&m_rcWnd, ptMouse.x-m_bGrabbedMousePos.x, ptMouse.y-m_bGrabbedMousePos.y,
				ptMouse.x-m_bGrabbedMousePos.x+m_nWidth, ptMouse.y-m_bGrabbedMousePos.y+m_nHeight);	
	}
}

HRESULT	CMirMsgBox3D::OnButtonDown(POINT tpMouse)		// Button Down
{
	if(CMirMsgBox::OnButtonDown(tpMouse)==1) return 0;

	if(IsInRect(tpMouse.x,tpMouse.y,m_rcWnd))
	{	
		m_bFocused = TRUE;
		
		if ( m_bActive && m_bFocused && m_bCanMove )		// Get the gaps between Mouse and Window
		{
			m_bGrabbedMousePos.x = tpMouse.x - m_rcWnd.left;
			m_bGrabbedMousePos.y = tpMouse.y - m_rcWnd.top;
		}
	}
	return 0;
}

HRESULT	CMirMsgBox3D::OnButtonUp(POINT tpMouse)		// Button Up
{
	m_bFocused = FALSE;
	return CMirMsgBox::OnButtonUp(tpMouse);
}
