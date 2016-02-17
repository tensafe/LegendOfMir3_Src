/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/



#include "StdAfx.h"


VOID CGameWnd::CreateGameWnd(INT nID, CWHWilImageData* pxWndImage, INT nFrameImgIdx, BOOL bCanMove, INT nStartX, INT nStartY, INT nWidth, INT nHeight)
{
	m_nID			= nID;
	m_nFrameImgIdx	= nFrameImgIdx;
	m_pxWndImage	= pxWndImage;
	m_bCanMove		= bCanMove;

	if ( m_pxWndImage->NewSetIndex(m_nFrameImgIdx) )
	{
		SetRect(&m_rcImgWnd, nStartX, nStartY, 
				nStartX+m_pxWndImage->m_lpstNewCurrWilImageInfo->shWidth,
				nStartY+m_pxWndImage->m_lpstNewCurrWilImageInfo->shHeight);
	}

	( ! nWidth )  ?	m_nWidth = m_rcImgWnd.right -m_rcImgWnd.left: m_nWidth  = nWidth;
	( ! nHeight ) ?	m_nHeight =m_rcImgWnd.bottom-m_rcImgWnd.top : m_nHeight = nHeight;

	SetRect(&m_rcWnd, nStartX, nStartY, nStartX+m_nWidth, nStartY+m_nHeight);

	// 3D테스트
 	D3DVECTOR vNorm(0, 0, -1);
    m_avBoard[0]  = D3DVERTEX(D3DVECTOR(-0.5f, 0.5f, 0), vNorm, 0, 0);
    m_avBoard[1]  = D3DVERTEX(D3DVECTOR(-0.5f,-0.5f, 0), vNorm, 0, 1);
    m_avBoard[2]  = D3DVERTEX(D3DVECTOR( 0.5f, 0.5f, 0), vNorm, 1, 0);
    m_avBoard[3]  = D3DVERTEX(D3DVECTOR( 0.5f,-0.5f, 0), vNorm, 1, 1);
}


VOID CGameWnd::Init()
{
	m_nID			= 0; 
	m_nFrameImgIdx	= 0;
	m_nWidth		= 0;
	m_nHeight		= 0;
	m_pxWndImage	= NULL;
	m_bActive		= FALSE;
	m_bFocused		= FALSE;
	m_bMoving		= FALSE;
	m_bCanMove		= FALSE;
	SetRect(&m_rcWnd, 0, 0, 0, 0);
	SetRect(&m_rcImgWnd, 0, 0, 0, 0);
	ZeroMemory(&m_bGrabbedMousePos, sizeof(POINT));
	D3DVECTOR		vNorm(0, 0, -1);
	m_avBoard[0] = D3DVERTEX(D3DVECTOR(-0.5f, 0.5f, 0), vNorm, 0, 0);
	m_avBoard[1] = D3DVERTEX(D3DVECTOR(-0.5f,-0.5f, 0), vNorm, 0, 1);
	m_avBoard[2] = D3DVERTEX(D3DVECTOR( 0.5f, 0.5f, 0), vNorm, 1, 0);
	m_avBoard[3] = D3DVERTEX(D3DVECTOR( 0.5f,-0.5f, 0), vNorm, 1, 1);
}


VOID CGameWnd::Destroy()
{
	Init();
}


VOID CGameWnd::ShowGameWnd()
{
	if ( m_bActive )
	{
		if ( m_pxWndImage->NewSetIndex(m_nFrameImgIdx) )
		{
 			g_xMainWnd.DrawWithImageForCompClipRgn(m_rcWnd.left, m_rcWnd.top, m_pxWndImage->m_lpstNewCurrWilImageInfo->shWidth, m_pxWndImage->m_lpstNewCurrWilImageInfo->shHeight,
												   (WORD*)m_pxWndImage->m_pbCurrImage, _CLIP_WIDTH, 600);
		}

/*		if ( g_xMainWnd.Get3DDevice() )
		{
			D3DVECTOR vTrans;
			D3DMATRIX matTrans;
			D3DMATRIX matScale;
			D3DMATRIX matWorld;
			D3DMATRIX matWorldOriginal;
			D3DMATERIAL7 mtrl;
			if( SUCCEEDED(g_xMainWnd.Get3DDevice()->BeginScene()) )
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

				LPDIRECTDRAWSURFACE7 lpddsTextr = g_xGameProc.m_xImage.GetTextrImg(_TEXTR_FILE_INTER, _IMAGE_INTEREX, m_nFrameImgIdx);
				g_xMainWnd.Get3DDevice()->SetTexture(0, lpddsTextr);

//				g_xMainWnd.Get3DDevice()->SetTexture(0, D3DWILTextr_GetSurface(m_pxWndImage->m_szWilFileName, m_nFrameImgIdx));
				D3DUtil_InitMaterial(mtrl, 
									 (FLOAT)255/255.0f,
									 (FLOAT)255/255.0f, 
									 (FLOAT)255/255.0f);
				if ( m_bFocused )
				{
					mtrl.diffuse.a = 200/255.0f;
				}
				else
				{
					mtrl.diffuse.a = 170/255.0f;
				}
				g_xMainWnd.Get3DDevice()->SetMaterial(&mtrl);


				g_xMainWnd.Get3DDevice()->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
				g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, TRUE);
				g_xMainWnd.Get3DDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				g_xMainWnd.Get3DDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
				g_xMainWnd.Get3DDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

				g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
				g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);

				g_xMainWnd.Get3DDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX, m_avBoard, 4, NULL);

				g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
				g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, FALSE);
				g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_SRCBLEND , D3DBLEND_ONE);
				g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);
				g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

				g_xMainWnd.Get3DDevice()->EndScene();
			}
		}
*/	}
}


VOID CGameWnd::ResetWndImg(INT nIdx)
{
//	D3DWILTextr_Invalidate(m_pxWndImage->m_szWilFileName, m_nFrameImgIdx);
	if ( m_pxWndImage->NewSetIndex(nIdx) )
	{
		m_nFrameImgIdx = nIdx;
		SetRect(&m_rcImgWnd, m_rcWnd.left, m_rcWnd.top, 
				m_rcWnd.left+m_pxWndImage->m_lpstNewCurrWilImageInfo->shWidth,
				m_rcWnd.top+m_pxWndImage->m_lpstNewCurrWilImageInfo->shHeight);

		m_nWidth = m_rcImgWnd.right-m_rcWnd.left;
		m_nHeight =m_rcImgWnd.bottom-m_rcWnd.top;

		m_rcWnd = m_rcImgWnd;
	}

//	g_xGameProc.m_xImage.AddTextr(_TEXTR_FILE_INTER, _IMAGE_INTEREX, m_nFrameImgIdx);
//	D3DWILTextr_Restore(m_pxWndImage->m_szWilFileName, m_nFrameImgIdx, g_xMainWnd.Get3DDevice());
}

// 윈도우를 활성화 한다.
VOID CGameWnd::SetGameWndActive(BOOL bActive)
{
	m_bActive = bActive;

	g_xGameProc.m_xImage.AddTextr(_TEXTR_FILE_INTER, _IMAGE_INTEREX, m_nFrameImgIdx);

/*	if ( m_bActive )
	{
		D3DWILTextr_Restore(m_pxWndImage->m_szWilFileName, m_nFrameImgIdx, g_xMainWnd.Get3DDevice());
	}
	else
	{
		D3DWILTextr_Invalidate(m_pxWndImage->m_szWilFileName, m_nFrameImgIdx);
	}
*/
}

// 윈도우에 포커스를 준다.
VOID CGameWnd::SetGameWndFocused(BOOL bFocused)
{
	m_bFocused = bFocused;
}

VOID CGameWnd::MoveGameWnd(POINT ptMouse)
{
	if ( m_bActive && m_bFocused && m_bCanMove )
	{
		SetRect(&m_rcWnd, ptMouse.x-m_bGrabbedMousePos.x, ptMouse.y-m_bGrabbedMousePos.y, 
				ptMouse.x-m_bGrabbedMousePos.x+m_nWidth, ptMouse.y-m_bGrabbedMousePos.y+m_nHeight); 
	}
}

// 윈도우가 움직이기 시작할때 마우스와의 Gap을 얻는다.
VOID CGameWnd::SetReadyMove(POINT ptMouse)
{
	if ( m_bActive && m_bFocused && m_bCanMove )
	{
		m_bGrabbedMousePos.x = ptMouse.x - m_rcWnd.left;
		m_bGrabbedMousePos.y = ptMouse.y - m_rcWnd.top;
	}
}
