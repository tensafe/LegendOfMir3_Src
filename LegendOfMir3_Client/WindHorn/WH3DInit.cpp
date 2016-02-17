#include "StdAfx.h"


C3DInit::C3DInit()
{
	CWHDXGraphicWindow* pxDXGWnd = GetDXGWindowPtr();
	assert(pxDXGWnd);
	m_pd3dDevice = pxDXGWnd->Get3DDevice();

	ZeroMemory(&m_matWorld, sizeof(D3DMATRIX));
	ZeroMemory(&m_matView, sizeof(D3DMATRIX));
	ZeroMemory(&m_matProj, sizeof(D3DMATRIX));
	ZeroMemory(&m_mtrl, sizeof(D3DMATERIAL7));
}


C3DInit::~C3DInit()
{
}


HRESULT C3DInit::InitDeviceObjects()
{
	// 현재 800*600화면에 맞게 3D 좌표계를 설정한다.
 	D3DVECTOR vEyePt    = D3DVECTOR(0, 0, -300.0f);
    D3DVECTOR vLookatPt = D3DVECTOR(0, 0, 0);
    D3DVECTOR vUpVec    = D3DVECTOR(0, 1, 0);
    D3DUtil_SetIdentityMatrix(m_matWorld);
    D3DUtil_SetViewMatrix(m_matView, vEyePt, vLookatPt, vUpVec);
    D3DUtil_SetProjectionMatrix(m_matProj, g_PI/2, float(600.0f/800.0f), -1.0f, 1.0f);

    m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,      &m_matWorld);
    m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_VIEW,       &m_matView);
    m_pd3dDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &m_matProj);

	D3DUtil_InitMaterial(m_mtrl);
    m_pd3dDevice->SetMaterial(&m_mtrl);

	// RenderState의 초기값 설정.
	m_pd3dDevice->SetRenderState( D3DRENDERSTATE_AMBIENT, 0xFFFFFFFF);
    m_pd3dDevice->SetRenderState(D3DRENDERSTATE_DITHERENABLE, TRUE); 
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_CULLMODE, D3DCULL_NONE); 	
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, D3DZB_FALSE);
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID);
//	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME);
	m_pd3dDevice->SetRenderState(D3DRENDERSTATE_SHADEMODE, D3DSHADE_GOURAUD);
	
    // TextureStageState의 초기값 설정.
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
    m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE); 
    m_pd3dDevice->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
    m_pd3dDevice->SetTextureStageState(0, D3DTSS_MINFILTER, D3DTFN_POINT); 
    m_pd3dDevice->SetTextureStageState(0, D3DTSS_MAGFILTER, D3DTFG_POINT);
	m_pd3dDevice->SetTextureStageState(0, D3DTSS_MIPFILTER, D3DTFP_NONE);

    return S_OK;
}
