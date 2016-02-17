/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/
#include "StdAfx.h"



/******************************************************************************************************************

	CParticleSystem Class	

*******************************************************************************************************************/
CParticleSystem::CParticleSystem()
{
	InitSystem();
}


CParticleSystem::~CParticleSystem()
{
	DestroySystem();
}


VOID CParticleSystem::InitSystem()
{
	m_wNum			 = 0;		
	m_pxParticle	 = NULL;
	m_fDeltaTime	 = 0.02f;

	m_fAirFiction	 = -0.05f;
	m_vecBounding[0] = D3DVECTOR(0, 0, 0);
	m_vecBounding[1] = D3DVECTOR(0, 0, 0);
	m_vecEnvironment = D3DVECTOR(0, 300, 0);

	D3DVECTOR vNorm(0, 0, -1);
	m_avShape[0] = D3DVERTEX(D3DVECTOR(-0.5f,-0.5f, 0.0f ), vNorm, 0.0f, 1.0f);
	m_avShape[1] = D3DVERTEX(D3DVECTOR( 0.5f,-0.5f, 0.0f ), vNorm, 0.0f, 0.0f);
	m_avShape[2] = D3DVERTEX(D3DVECTOR(-0.5f, 0.5f, 0.0f ), vNorm, 1.0f, 1.0f);
	m_avShape[3] = D3DVERTEX(D3DVECTOR( 0.5f, 0.5f, 0.0f ), vNorm, 1.0f, 0.0f);
}


VOID CParticleSystem::SetupSystem(WORD wCnt)
{
	InitSystem();
	m_wNum = wCnt;
    m_pxParticle = new CParticle[m_wNum];

	for ( INT nCnt = 0; nCnt < m_wNum; nCnt++ )
	{
		m_pxParticle[nCnt].Init();
	}
}


VOID CParticleSystem::DestroySystem()
{
	SAFE_DELETE(m_pxParticle);
	InitSystem();
}


VOID CParticleSystem::UpdateAirFiction(WORD wNum)
{
	if ( !m_pxParticle[wNum].m_bIsDead )
	{
		m_pxParticle[wNum].m_vecLocalForce.x = -m_pxParticle[wNum].m_vecVel.x * m_fAirFiction;
		m_pxParticle[wNum].m_vecLocalForce.y = -m_pxParticle[wNum].m_vecVel.y * m_fAirFiction;
		m_pxParticle[wNum].m_vecLocalForce.z = m_pxParticle[wNum].m_vecVel.z * m_fAirFiction;
	}
}


VOID CParticleSystem::UpdateMove(WORD wNum)
{
	if ( !m_pxParticle[wNum].m_bIsDead )
	{
		m_pxParticle[wNum].m_vecAccel.x += (m_vecEnvironment.x + m_pxParticle[wNum].m_vecLocalForce.x) / m_pxParticle[wNum].m_fMass;
		m_pxParticle[wNum].m_vecAccel.y += (m_vecEnvironment.y + m_pxParticle[wNum].m_vecLocalForce.y) / m_pxParticle[wNum].m_fMass;
		m_pxParticle[wNum].m_vecAccel.z += (m_vecEnvironment.z + m_pxParticle[wNum].m_vecLocalForce.z) / m_pxParticle[wNum].m_fMass;

		m_pxParticle[wNum].m_vecVel.x += m_pxParticle[wNum].m_vecAccel.x * m_fDeltaTime;//*0.01f;
		m_pxParticle[wNum].m_vecVel.y += m_pxParticle[wNum].m_vecAccel.y * m_fDeltaTime;//*0.01f;
		m_pxParticle[wNum].m_vecVel.z += m_pxParticle[wNum].m_vecAccel.z * m_fDeltaTime;//*0.01f;

		m_pxParticle[wNum].m_vecOldPos = m_pxParticle[wNum].m_vecPos;

		m_pxParticle[wNum].m_vecPos.x += m_pxParticle[wNum].m_vecVel.x * m_fDeltaTime;//*0.01f;
		m_pxParticle[wNum].m_vecPos.y += m_pxParticle[wNum].m_vecVel.y * m_fDeltaTime;//*0.01f;
		m_pxParticle[wNum].m_vecPos.z += m_pxParticle[wNum].m_vecVel.z * m_fDeltaTime;//*0.01f;
	}
}


VOID CParticleSystem::UpdateCrash(WORD wNum)
{
	if ( !m_pxParticle[wNum].m_bIsDead )
	{
		if ( m_pxParticle[wNum].m_vecPos.x <= m_vecBounding[0].x || m_pxParticle[wNum].m_vecPos.x >= m_vecBounding[1].x )
		{	
			m_pxParticle[wNum].m_vecVel.x = -m_pxParticle[wNum].m_vecVel.x * 0.7f;
		}

		if ( m_pxParticle[wNum].m_vecPos.y <= m_vecBounding[0].y || m_pxParticle[wNum].m_vecPos.y >= m_vecBounding[1].y )
		{
			FLOAT fOldX = m_pxParticle[wNum].m_vecPos.x - m_pxParticle[wNum].m_vecVel.x * m_pxParticle[wNum].m_wDelay;
			FLOAT fOldY = m_pxParticle[wNum].m_vecPos.y - m_pxParticle[wNum].m_vecVel.y * m_pxParticle[wNum].m_wDelay;
			
			FLOAT fBefore = m_pxParticle[wNum].m_wDelay * (m_vecBounding[1].y - fOldY)/(m_pxParticle[wNum].m_vecPos.y - fOldY);							// 충돌전.
			FLOAT fAfter  = m_pxParticle[wNum].m_wDelay * (m_pxParticle[wNum].m_vecPos.y - m_vecBounding[1].y)/(m_pxParticle[wNum].m_vecPos.y - fOldY);	// 충돌후.
			
			m_pxParticle[wNum].m_vecPos.x = fOldX + m_pxParticle[wNum].m_vecVel.x * fBefore;
			m_pxParticle[wNum].m_vecPos.y = fOldY + m_pxParticle[wNum].m_vecVel.y * fBefore;
			
			m_pxParticle[wNum].m_vecVel.y = -m_pxParticle[wNum].m_vecVel.y * 0.6f;
			
			m_pxParticle[wNum].m_vecPos.x += m_pxParticle[wNum].m_vecVel.x * fAfter;
			m_pxParticle[wNum].m_vecPos.y += m_pxParticle[wNum].m_vecVel.y * fAfter;			
		}

		if ( m_pxParticle[wNum].m_vecPos.z <= m_vecBounding[0].z || m_pxParticle[wNum].m_vecPos.z >= m_vecBounding[1].y )
		{
			m_pxParticle[wNum].m_vecVel.z = -m_pxParticle[wNum].m_vecVel.z * 0.6f;
		}
	}
}






/******************************************************************************************************************

	CSnow Class	

*******************************************************************************************************************/
CSnow::CSnow()
{
	InitSystem();
}


CSnow::~CSnow()
{
	DestroySystem();
}


VOID CSnow::LoadTextr()
{
	CHAR* szFileList[] = 
	{
		"Snow0.bmp",
/*		"0000.bmp",
		"0001.bmp",
		"0002.bmp",
		"0003.bmp",
*/
/*		"shine0.bmp",
		"shine1.bmp",
		"shine2.bmp",
		"shine3.bmp",
		"shine4.bmp",
		"shine5.bmp",
		"shine6.bmp",
		"shine7.bmp",
		"shine8.bmp",
		"shine9.bmp",
*/	};
	for ( INT nCnt = 0; nCnt < _MAX_SNOWFRAME; nCnt++ )
	{
		strcpy(m_szSnowFile[nCnt], szFileList[nCnt]);
		D3DTextr_CreateTextureFromFile(m_szSnowFile[nCnt]);
		D3DTextr_Restore(m_szSnowFile[nCnt], g_xMainWnd.Get3DDevice());
	}
}


VOID CSnow::InitSystem()
{
	CParticleSystem::InitSystem();
	m_fWidth = m_fGround = 0.0f;
	ZeroMemory(m_szSnowFile, _MAX_SNOWFRAME*MAX_PATH);
}


VOID CSnow::DestroySystem()
{
	CParticleSystem::DestroySystem();

	for ( INT nCnt = 0; nCnt < _MAX_SNOWFRAME; nCnt++ )
	{
		D3DTextr_Invalidate(m_szSnowFile[nCnt]);
		D3DTextr_DestroyTexture(m_szSnowFile[nCnt]);
	}

	InitSystem();
}


VOID CSnow::SetupSystem(WORD wCnt, FLOAT fWidth, FLOAT fGround)
{
	InitSystem();
	CParticleSystem::SetupSystem(wCnt);

	m_fWidth  = fWidth;
	m_fGround = fGround;
	LoadTextr();

	SetEnvFactor(-0.05f, D3DVECTOR(10, 100, 0));
}


VOID CSnow::UpdateSystem(INT nLoopTime, D3DVECTOR vecGenPos)
{
	INT nGenCnt = 0; 
	INT nSpeedRate = (nLoopTime / 17);

	if ( nSpeedRate < 1 ) 		
		nSpeedRate = 1;

	m_fDeltaTime = 0.02f*nSpeedRate;

    for ( INT nCnt = 0; nCnt < m_wNum; nCnt++ )
    {
		m_pxParticle[nCnt].m_wCurrLife += nLoopTime;

		if ( m_pxParticle[nCnt].m_wCurrLife > m_pxParticle[nCnt].m_wLife || m_pxParticle[nCnt].m_vecPos.y >= m_fGround )
		{
			if( (m_pxParticle[nCnt].m_wCurrLife-m_pxParticle[nCnt].m_wLife) > 255 )
			{
				m_pxParticle[nCnt].m_bIsDead = TRUE;
			}
			else
			{
				BYTE bRate;
				bRate = 255 - (m_pxParticle[nCnt].m_wCurrLife-m_pxParticle[nCnt].m_wLife);

				m_pxParticle[nCnt].m_bOpa = bRate;
				
				if ( bRate < m_pxParticle[nCnt].m_bRed )				m_pxParticle[nCnt].m_bRed = bRate;
				if ( bRate < m_pxParticle[nCnt].m_bGreen )				m_pxParticle[nCnt].m_bGreen = bRate;
				if ( bRate < m_pxParticle[nCnt].m_bBlue )				m_pxParticle[nCnt].m_bBlue = bRate;

				continue;
			}
		}

		if ( m_pxParticle[nCnt].m_bIsDead && nGenCnt < 20 )
		{
			SetParticleDefault(nCnt);
			nGenCnt++;
		}
		else
		{
			UpdateAirFiction(nCnt);
			UpdateMove(nCnt);

			m_pxParticle[nCnt].m_wCurrDelay += nLoopTime;
			if ( m_pxParticle[nCnt].m_wCurrDelay > m_pxParticle[nCnt].m_wDelay )
			{
				m_pxParticle[nCnt].m_wCurrDelay = 0;
				m_pxParticle[nCnt].m_wCurrFrame++;

//				FLOAT fRand = _RAND_FLOAT;
//				SetEnvFactor(fRand, D3DVECTOR(-fRand, fRand*100, 0));

				if ( m_pxParticle[nCnt].m_wCurrFrame >= _MAX_SNOWFRAME )
				{
					m_pxParticle[nCnt].m_wCurrFrame = 0;
				}
			}
		}
    }
}


HRESULT CSnow::RenderSystem(LPDIRECT3DDEVICE7 lpDevice)
{
	if ( lpDevice )
	{
		if( SUCCEEDED(lpDevice->BeginScene()) )
		{
			D3DMATRIX matWorldOriginal;
			D3DMATRIX matTrans;
			D3DMATRIX matScale;
			D3DMATRIX matWorld;
			D3DVECTOR vTrans;
			D3DMATERIAL7 mtrl;

			lpDevice->GetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

			for ( INT nCnt = 0; nCnt < m_wNum; nCnt++ )
			{
				if ( !m_pxParticle[nCnt].m_bIsDead )
				{
					D3DUtil_InitMaterial(mtrl, 
										 (FLOAT)m_pxParticle[nCnt].m_bRed/255.0f,
										 (FLOAT)m_pxParticle[nCnt].m_bGreen/255.0f, 
										 (FLOAT)m_pxParticle[nCnt].m_bBlue/255.0f);
					mtrl.diffuse.a = 0.0f;
//					mtrl.diffuse.a = m_pxParticle[nCnt].m_bOpa/255.0f;
					lpDevice->SetMaterial(&mtrl);

//					SetBlendRenderState(lpDevice, _BLEND_NORMAL, mtrl);
					lpDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
					lpDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, TRUE);
					lpDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
					lpDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
					lpDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

					lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
					lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);

  					vTrans.x = (FLOAT) m_pxParticle[nCnt].m_vecPos.x-400;
					vTrans.y = (FLOAT)-m_pxParticle[nCnt].m_vecPos.y+300;
					vTrans.z = (FLOAT) m_pxParticle[nCnt].m_vecPos.z; 

					D3DUtil_SetScaleMatrix(matScale, m_pxParticle[nCnt].m_fSize, m_pxParticle[nCnt].m_fSize, m_pxParticle[nCnt].m_fSize);
					D3DUtil_SetTranslateMatrix(matTrans, vTrans);
					D3DMath_MatrixMultiply(matWorld, matScale, matTrans);
					lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);
					lpDevice->SetTexture(0, D3DTextr_GetSurface(m_szSnowFile[m_pxParticle[nCnt].m_wCurrFrame]));
					lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX, m_avShape, 4, NULL);

					lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);
				}
			}
//			ResetBlendenderState(lpDevice);
			lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
			lpDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, FALSE);
			lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND , D3DBLEND_ONE);
			lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);
			lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);
			lpDevice->EndScene();

			return S_OK;
		}
	}
	return E_FAIL;
}


VOID CSnow::SetParticleDefault(WORD wNum, D3DVECTOR vecGenPos)
{
    m_pxParticle[wNum].m_vecPos		= D3DVECTOR((FLOAT)GetRandomNum(0, m_fWidth)/*(-400, 1200)*/, (FLOAT)GetRandomNum(-300,  0), 0.0f);
    m_pxParticle[wNum].m_vecVel		= D3DVECTOR((FLOAT)GetRandomNum(-30,  30), (FLOAT)GetRandomNum(70, 100), 0.0f);
	m_pxParticle[wNum].m_vecAccel	= D3DVECTOR(0.0f, 0.0f, 0.0f);
	m_pxParticle[wNum].m_vecOldPos	= D3DVECTOR(0.0f, 0.0f, 0.0f);
	m_pxParticle[wNum].m_vecLocalForce = D3DVECTOR(0.0f, 0.0f, 0.0f);
	m_pxParticle[wNum].m_wLife		= (FLOAT)GetRandomNum(2500,  7000);
	m_pxParticle[wNum].m_fMass		= 1000 + _RAND_FLOAT;
	m_pxParticle[wNum].m_fSize		= (FLOAT)GetRandomNum(2, 6) + _RAND_FLOAT;
//	m_pxParticle[wNum].m_wLife		= (FLOAT)GetRandomNum((BYTE)m_pxParticle[wNum].m_fSize*2000,  6000);
	m_pxParticle[wNum].m_bIsDead	= FALSE;
	m_pxParticle[wNum].m_bRed		= m_pxParticle[wNum].m_bGreen = m_pxParticle[wNum].m_bBlue = (FLOAT)GetRandomNum(/*(BYTE)(m_pxParticle[wNum].m_fSize)*30+20*/150, 200);
	m_pxParticle[wNum].m_bBlue		+= 50;
//	m_pxParticle[wNum].m_bRed		= (FLOAT)GetRandomNum(0, 255);
//	m_pxParticle[wNum].m_bGreen		= (FLOAT)GetRandomNum(0, 255);
//	m_pxParticle[wNum].m_bBlue		= (FLOAT)GetRandomNum(0, 255);
	m_pxParticle[wNum].m_wDelay		= 300;//(FLOAT)GetRandomNum(50, 150);
	m_pxParticle[wNum].m_wCurrLife	= 0;
	m_pxParticle[wNum].m_wCurrDelay = 0;
	m_pxParticle[wNum].m_wCurrFrame = 0;
	m_pxParticle[wNum].m_bOpa		= 255;
}









/******************************************************************************************************************

	CRain Class	

*******************************************************************************************************************/
CRain::CRain()
{
	InitSystem();
}


CRain::~CRain()
{
	DestroySystem();
}


VOID CRain::LoadTextr()
{
	CHAR* szFileList1[] = 
	{
//		"Rain0.bmp",
		"Snow0.bmp",
	};
	CHAR* szFileList2[] = 
	{
		"RainDrop0.bmp",
		"RainDrop1.bmp",
		"RainDrop2.bmp",
		"RainDrop3.bmp",
		"RainDrop4.bmp",
	};
	for ( INT nCnt = 0; nCnt < _MAX_RAINFRAME; nCnt++ )
	{
		strcpy(m_pszRainFile[nCnt], szFileList1[nCnt]);
		D3DTextr_CreateTextureFromFile(m_pszRainFile[nCnt]);
		D3DTextr_Restore(m_pszRainFile[nCnt], g_xMainWnd.Get3DDevice());
	}
	for ( nCnt = 0; nCnt < _MAX_RAINDROPFRAME; nCnt++ )
	{
		strcpy(m_pszRainDropFile[nCnt], szFileList2[nCnt]);
		D3DTextr_CreateTextureFromFile(m_pszRainDropFile[nCnt]);
		D3DTextr_Restore(m_pszRainDropFile[nCnt], g_xMainWnd.Get3DDevice());
	}
}


VOID CRain::InitSystem()
{
	CParticleSystem::InitSystem();
	m_fWidth = m_fGround = 0.0f;
	m_bRainIntensity = 0;
	m_bGenCnt = 10;
	ZeroMemory(m_pszRainFile, _MAX_RAINFRAME*MAX_PATH);
	ZeroMemory(m_pszRainDropFile, _MAX_RAINDROPFRAME*MAX_PATH);
}


VOID CRain::DestroySystem()
{
	CParticleSystem::DestroySystem();

	for ( INT nCnt = 0; nCnt < _MAX_RAINFRAME; nCnt++ )
	{
		D3DTextr_Invalidate(m_pszRainFile[nCnt]);
		D3DTextr_DestroyTexture(m_pszRainFile[nCnt]);
	}
	for ( nCnt = 0; nCnt < _MAX_RAINDROPFRAME; nCnt++ )
	{
		D3DTextr_Invalidate(m_pszRainDropFile[nCnt]);
		D3DTextr_DestroyTexture(m_pszRainDropFile[nCnt]);
	}
	InitSystem();
}


VOID CRain::SetupSystem(WORD wCnt, FLOAT fWidth, FLOAT fGround)
{
	InitSystem();
	CParticleSystem::SetupSystem(wCnt);

	m_fWidth  = fWidth;
	m_fGround = fGround;
	LoadTextr();

	SetEnvFactor(-0.05f, D3DVECTOR(10, 100, 0));
}


VOID CRain::UpdateSystem(INT nLoopTime, D3DVECTOR vecGenPos)
{
	INT nGenCnt = 0; 
	INT nSpeedRate = (nLoopTime / 17);
	if ( nSpeedRate < 1 ) 		
		nSpeedRate = 1;

	m_fDeltaTime = 0.02f*nSpeedRate;

    for ( INT nCnt = 0; nCnt < m_wNum; nCnt++ )
    {
		m_pxParticle[nCnt].m_wCurrLife += nLoopTime;

		if ( m_pxParticle[nCnt].m_wCurrLife > m_pxParticle[nCnt].m_wLife || m_pxParticle[nCnt].m_vecPos.y >= m_fGround )
		{
			if( (m_pxParticle[nCnt].m_wCurrLife-m_pxParticle[nCnt].m_wLife) > /*_MAX_RAINDROPFRAME*50*/250 )
			{
				m_pxParticle[nCnt].m_bIsDead = TRUE;
			}
			else
			{
				BYTE bRate;
				bRate = /*50*_MAX_RAINDROPFRAME*/250 - (m_pxParticle[nCnt].m_wCurrLife-m_pxParticle[nCnt].m_wLife) / 4;

				m_pxParticle[nCnt].m_bOpa = bRate;
				m_pxParticle[nCnt].m_wDelay = 50;
				
				if ( bRate < m_pxParticle[nCnt].m_bRed )				m_pxParticle[nCnt].m_bRed = bRate;
				if ( bRate < m_pxParticle[nCnt].m_bGreen )				m_pxParticle[nCnt].m_bGreen = bRate;
				if ( bRate < m_pxParticle[nCnt].m_bBlue )				m_pxParticle[nCnt].m_bBlue = bRate;

				m_pxParticle[nCnt].m_wCurrDelay += nLoopTime;
				if ( m_pxParticle[nCnt].m_wCurrDelay > m_pxParticle[nCnt].m_wDelay )
				{
					m_pxParticle[nCnt].m_wCurrDelay = 0;
					m_pxParticle[nCnt].m_wCurrFrame++;

					if ( m_pxParticle[nCnt].m_wCurrFrame >= _MAX_RAINDROPFRAME )
					{
						m_pxParticle[nCnt].m_wCurrFrame = 0;
					}
				}

				continue;
			}
		}

		if ( m_pxParticle[nCnt].m_bIsDead && nGenCnt < m_bGenCnt )
		{
			SetParticleDefault(nCnt);
			nGenCnt++;
		}
		else
		{
			UpdateAirFiction(nCnt);
			UpdateMove(nCnt);

			m_pxParticle[nCnt].m_wCurrDelay += nLoopTime;
			if ( m_pxParticle[nCnt].m_wCurrDelay > m_pxParticle[nCnt].m_wDelay )
			{
				m_pxParticle[nCnt].m_wCurrDelay = 0;
				m_pxParticle[nCnt].m_wCurrFrame++;

				if ( m_pxParticle[nCnt].m_wCurrFrame >= _MAX_RAINFRAME )
				{
					m_pxParticle[nCnt].m_wCurrFrame = 0;
				}
			}
		}
    }
}


HRESULT CRain::RenderSystem(LPDIRECT3DDEVICE7 lpDevice)
{
	if ( lpDevice )
	{
		if( SUCCEEDED(lpDevice->BeginScene()) )
		{
			D3DMATRIX matWorldOriginal;
			D3DMATRIX matTrans;
			D3DMATRIX matScale;
			D3DMATRIX matRot;
			D3DMATRIX matWorld;
			D3DMATRIX matTmpWorld;
			D3DVECTOR vTrans;
			D3DMATERIAL7 mtrl;

			lpDevice->GetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

			for ( INT nCnt = 0; nCnt < m_wNum; nCnt++ )
			{
				if ( !m_pxParticle[nCnt].m_bIsDead )
				{
					D3DUtil_InitMaterial(mtrl, 
										 (FLOAT)m_pxParticle[nCnt].m_bRed/255.0f,
										 (FLOAT)m_pxParticle[nCnt].m_bGreen/255.0f, 
										 (FLOAT)m_pxParticle[nCnt].m_bBlue/255.0f);
					mtrl.diffuse.a = 0.0f;
//					mtrl.diffuse.a = m_pxParticle[nCnt].m_bOpa/255.0f;
					lpDevice->SetMaterial(&mtrl);

					SetBlendRenderState(lpDevice, _BLEND_LIGHTINV, mtrl);
/*					lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
					lpDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, TRUE);
					lpDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
					lpDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
					lpDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

					lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
					lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
*/
  					vTrans.x = (FLOAT) m_pxParticle[nCnt].m_vecPos.x-400;
					vTrans.y = (FLOAT)-m_pxParticle[nCnt].m_vecPos.y+300;
					vTrans.z = (FLOAT) m_pxParticle[nCnt].m_vecPos.z; 

					D3DUtil_SetScaleMatrix(matScale, 1.5f, m_pxParticle[nCnt].m_fSize, 1.0f);
					D3DUtil_SetRotateZMatrix(matRot, g_PI/2 - atan((m_pxParticle[nCnt].m_vecPos.y - m_pxParticle[nCnt].m_vecOldPos.y)/(m_pxParticle[nCnt].m_vecPos.x - m_pxParticle[nCnt].m_vecOldPos.x)));
					D3DUtil_SetTranslateMatrix(matTrans, vTrans);
					D3DMath_MatrixMultiply(matTmpWorld, matRot, matTrans);
					D3DMath_MatrixMultiply(matWorld, matScale, matTmpWorld);
					lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);

					if( (m_pxParticle[nCnt].m_wCurrLife-m_pxParticle[nCnt].m_wLife) > 0 && (m_pxParticle[nCnt].m_wCurrLife-m_pxParticle[nCnt].m_wLife) < 510 )
					{
						D3DUtil_SetScaleMatrix(matScale, 64.0f, 64.0f, 1.0f);
//						D3DUtil_SetRotateZMatrix(matRot, (FLOAT)GetRandomNum(0, 2) + _RAND_FLOAT);
						D3DUtil_SetTranslateMatrix(matTrans, vTrans);
						D3DMath_MatrixMultiply(matTmpWorld, matRot, matTrans);
						D3DMath_MatrixMultiply(matWorld, matScale, matTmpWorld);
						lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);
						lpDevice->SetTexture(0, D3DTextr_GetSurface(m_pszRainDropFile[m_pxParticle[nCnt].m_wCurrFrame]));
					}
					else
					{
						lpDevice->SetTexture(0, D3DTextr_GetSurface(m_pszRainFile[m_pxParticle[nCnt].m_wCurrFrame]));
					}

					lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX, m_avShape, 4, NULL);

					lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);
				}
			}
//			ResetBlendenderState(lpDevice);
			lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
			lpDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, FALSE);
			lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND , D3DBLEND_ONE);
			lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);
			lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);
			lpDevice->EndScene();

			return S_OK;
		}
	}
	return E_FAIL;
}


VOID CRain::SetParticleDefault(WORD wNum, D3DVECTOR vecGenPos)
{
	// m_bRainIntensity ---> 비의 강도가 심해질수록 gencnt, mass, size, delay, velocity등에 영향을 미친다.


	ZeroMemory(&m_pxParticle[wNum], sizeof(CParticle));
    m_pxParticle[wNum].m_vecPos		= D3DVECTOR((FLOAT)GetRandomNum(-100, m_fWidth+100)/*(-400, 1200)*/, (FLOAT)GetRandomNum(-500, 0), 0.0f);
    m_pxParticle[wNum].m_vecVel		= D3DVECTOR((FLOAT)/*GetRandomNum(0,  10)*/0, 500/*(FLOAT)GetRandomNum(10, 30)*/, 0.0f);
//	m_pxParticle[wNum].m_vecAccel	= D3DVECTOR(0.0f, 0.0f, 0.0f);
//	m_pxParticle[wNum].m_vecOldPos	= D3DVECTOR(0.0f, 0.0f, 0.0f);
//	m_pxParticle[wNum].m_vecLocalForce = D3DVECTOR(0.0f, 0.0f, 0.0f);
	m_pxParticle[wNum].m_wLife		= (FLOAT)GetRandomNum(800, 1400);
	m_pxParticle[wNum].m_fMass		= 100.0f;
	m_pxParticle[wNum].m_fSize		= (FLOAT)GetRandomNum(3, 30) + _RAND_FLOAT;
//	m_pxParticle[wNum].m_wLife		= (FLOAT)GetRandomNum((BYTE)m_pxParticle[wNum].m_fSize*2000,  6000);
//	m_pxParticle[wNum].m_bIsDead	= FALSE;
	m_pxParticle[wNum].m_bRed		= m_pxParticle[wNum].m_bGreen = m_pxParticle[wNum].m_bBlue = (FLOAT)GetRandomNum(/*(BYTE)(m_pxParticle[wNum].m_fSize)*30+20*/120, 180);
	m_pxParticle[wNum].m_bBlue		= 125;
//	m_pxParticle[wNum].m_bRed		= (FLOAT)GetRandomNum(0, 255);
//	m_pxParticle[wNum].m_bGreen		= (FLOAT)GetRandomNum(0, 255);
//	m_pxParticle[wNum].m_bBlue		= (FLOAT)GetRandomNum(0, 255);
	m_pxParticle[wNum].m_wDelay		= (FLOAT)GetRandomNum(50, 150);
//	m_pxParticle[wNum].m_wCurrLife	= 0;
//	m_pxParticle[wNum].m_wCurrDelay = 0;
//	m_pxParticle[wNum].m_wCurrFrame = 0;
//	m_pxParticle[wNum].m_bOpa		= 255;
}






/******************************************************************************************************************

	CFly Class	

*******************************************************************************************************************/
CFly::CFly()
{
	InitSystem();
}


CFly::~CFly()
{
	DestroySystem();
}


VOID CFly::LoadTextr()
{
	CHAR* szFileList[] = 
	{
		"shine0.bmp",
		"shine1.bmp",
		"shine2.bmp",
		"shine3.bmp",
		"shine4.bmp",
		"shine5.bmp",
		"shine6.bmp",
		"shine7.bmp",
		"shine8.bmp",
		"shine9.bmp",
	};
	for ( INT nCnt = 0; nCnt < _MAX_FLYFRAME; nCnt++ )
	{
		strcpy(m_pszFlyFile[nCnt], szFileList[nCnt]);
		D3DTextr_CreateTextureFromFile(m_pszFlyFile[nCnt]);
		D3DTextr_Restore(m_pszFlyFile[nCnt], g_xMainWnd.Get3DDevice());
	}
}


VOID CFly::InitSystem()
{
	CParticleSystem::InitSystem();
	m_bGenCnt = 3;
	ZeroMemory(m_pszFlyFile, _MAX_FLYFRAME*MAX_PATH);
}


VOID CFly::DestroySystem()
{
	CParticleSystem::DestroySystem();

	for ( INT nCnt = 0; nCnt < _MAX_FLYFRAME; nCnt++ )
	{
		D3DTextr_Invalidate(m_pszFlyFile[nCnt]);
		D3DTextr_DestroyTexture(m_pszFlyFile[nCnt]);
	}
	InitSystem();
}


VOID CFly::SetupSystem(WORD wCnt)
{
	InitSystem();
	CParticleSystem::SetupSystem(wCnt);

	LoadTextr();

	SetEnvFactor(-0.05f, D3DVECTOR(100, 1000, 0));
}


VOID CFly::UpdateSystem(INT nLoopTime, D3DVECTOR vecGenPos)
{
	BYTE bRate;
	INT nGenCnt = 0; 

	INT nSpeedRate = (nLoopTime / 17);
	if ( nSpeedRate < 1 ) 		
		nSpeedRate = 1;

	m_fDeltaTime = 0.02f*nSpeedRate;

    for ( INT nCnt = 0; nCnt < m_wNum; nCnt++ )
    {
		m_pxParticle[nCnt].m_wCurrLife += nLoopTime;

		if ( m_pxParticle[nCnt].m_wCurrLife > m_pxParticle[nCnt].m_wLife )
		{
			m_pxParticle[nCnt].m_bIsDead = TRUE;
		}
		else
		{
			m_pxParticle[nCnt].m_wCurrDelay += nLoopTime;
			if ( m_pxParticle[nCnt].m_wCurrDelay > m_pxParticle[nCnt].m_wDelay )
			{
				m_pxParticle[nCnt].m_wCurrDelay = 0;
				m_pxParticle[nCnt].m_wCurrFrame++;

				m_pxParticle[nCnt].m_fSize += 0.5f;


				if ( m_pxParticle[nCnt].m_wCurrFrame >= _MAX_FLYFRAME )
				{
					m_pxParticle[nCnt].m_wCurrFrame = 0;
				}
			}

/*			bRate = 255 - 255*(FLOAT)((FLOAT)m_pxParticle[nCnt].m_wCurrLife/(FLOAT)m_pxParticle[nCnt].m_wLife);
			m_pxParticle[nCnt].m_bRed = m_pxParticle[nCnt].m_bGreen = m_pxParticle[nCnt].m_bBlue = bRate;
*/			bRate = m_pxParticle[nCnt].m_bFstRed - m_pxParticle[nCnt].m_bFstRed*(FLOAT)((FLOAT)m_pxParticle[nCnt].m_wCurrLife/(FLOAT)m_pxParticle[nCnt].m_wLife);
			m_pxParticle[nCnt].m_bRed	= bRate;
			bRate = m_pxParticle[nCnt].m_bFstGreen - m_pxParticle[nCnt].m_bFstGreen*(FLOAT)((FLOAT)m_pxParticle[nCnt].m_wCurrLife/(FLOAT)m_pxParticle[nCnt].m_wLife);
			m_pxParticle[nCnt].m_bGreen = bRate;
			bRate = m_pxParticle[nCnt].m_bFstBlue - m_pxParticle[nCnt].m_bFstBlue*(FLOAT)((FLOAT)m_pxParticle[nCnt].m_wCurrLife/(FLOAT)m_pxParticle[nCnt].m_wLife);
			m_pxParticle[nCnt].m_bBlue	= bRate;

			UpdateAirFiction(nCnt);
			UpdateMove(nCnt);
		}

		if ( m_pxParticle[nCnt].m_bIsDead )
		{
			if ( nGenCnt < m_bGenCnt )
			{
				SetParticleDefault(nCnt, vecGenPos);
				nGenCnt++;
			}
		}
    }
}


HRESULT CFly::RenderSystem(LPDIRECT3DDEVICE7 lpDevice)
{
	if ( lpDevice )
	{
		if( SUCCEEDED(lpDevice->BeginScene()) )
		{
			D3DMATRIX matWorldOriginal;
			D3DMATRIX matTrans;
			D3DMATRIX matScale;
			D3DMATRIX matWorld;
			D3DVECTOR vTrans;
			D3DMATERIAL7 mtrl;

			lpDevice->GetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

			for ( INT nCnt = 0; nCnt < m_wNum; nCnt++ )
			{
				if ( !m_pxParticle[nCnt].m_bIsDead )
				{
					D3DUtil_InitMaterial(mtrl, 
										 (FLOAT)m_pxParticle[nCnt].m_bRed/255.0f,
										 (FLOAT)m_pxParticle[nCnt].m_bGreen/255.0f, 
										 (FLOAT)m_pxParticle[nCnt].m_bBlue/255.0f);
					mtrl.diffuse.a = 0.0f;
//					mtrl.diffuse.a = m_pxParticle[nCnt].m_bOpa/255.0f;
					lpDevice->SetMaterial(&mtrl);
				
//					SetBlendRenderState(lpDevice, _BLEND_NORMAL, mtrl);
					lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
					lpDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, TRUE);
					lpDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
					lpDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
					lpDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

					lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
					lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);

  					vTrans.x = (FLOAT) m_pxParticle[nCnt].m_vecPos.x-400;
					vTrans.y = (FLOAT)-m_pxParticle[nCnt].m_vecPos.y+300;
					vTrans.z = (FLOAT) m_pxParticle[nCnt].m_vecPos.z; 

					D3DUtil_SetScaleMatrix(matScale, m_pxParticle[nCnt].m_fSize, m_pxParticle[nCnt].m_fSize, 1);
					D3DUtil_SetTranslateMatrix(matTrans, vTrans);
					D3DMath_MatrixMultiply(matWorld, matScale, matTrans);
					lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);

					lpDevice->SetTexture(0, D3DTextr_GetSurface(m_pszFlyFile[m_pxParticle[nCnt].m_wCurrFrame]));
					lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX, m_avShape, 4, NULL);

					lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);
				}
			}
//			ResetBlendenderState(lpDevice);
			lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
			lpDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, FALSE);
			lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND , D3DBLEND_ONE);
			lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);
			lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);
			lpDevice->EndScene();

			return S_OK;
		}
	}
	return E_FAIL;
}


VOID CFly::SetParticleDefault(WORD wNum, D3DVECTOR vecGenPos)
{
	// m_bRainIntensity ---> 비의 강도가 심해질수록 gencnt, mass, size, delay, velocity등에 영향을 미친다.

	ZeroMemory(&m_pxParticle[wNum], sizeof(CParticle));
    m_pxParticle[wNum].m_vecPos		= vecGenPos;
    m_pxParticle[wNum].m_vecVel		= D3DVECTOR((FLOAT)GetRandomNum(-30, 30), (FLOAT)GetRandomNum(-20, 40), 0.0f);
	m_pxParticle[wNum].m_wLife		= (FLOAT)GetRandomNum(500, 1500);
	m_pxParticle[wNum].m_fMass		= 1000.0f;
	m_pxParticle[wNum].m_fSize		= (FLOAT)GetRandomNum(5, 40) + _RAND_FLOAT;
//	m_pxParticle[wNum].m_wLife		= (FLOAT)GetRandomNum((BYTE)m_pxParticle[wNum].m_fSize*2000,  6000);
//	m_pxParticle[wNum].m_bIsDead	= FALSE;
	m_pxParticle[wNum].m_bRed		= m_pxParticle[wNum].m_bFstRed   = (FLOAT)GetRandomNum(200, 255);
	m_pxParticle[wNum].m_bGreen		= m_pxParticle[wNum].m_bFstGreen = m_pxParticle[wNum].m_bFstRed/2;//(FLOAT)GetRandomNum(0, 125);
	m_pxParticle[wNum].m_bBlue		= m_pxParticle[wNum].m_bFstBlue  = (FLOAT)GetRandomNum(0, 30);
	m_pxParticle[wNum].m_wDelay		= (FLOAT)GetRandomNum(200, 300);

//	m_pxParticle[wNum].m_wCurrLife	= 0;
//	m_pxParticle[wNum].m_wCurrDelay = 0;
//	m_pxParticle[wNum].m_wCurrFrame = 0;
//	m_pxParticle[wNum].m_bOpa		= 255;
}








/******************************************************************************************************************

	CBoom Class	

*******************************************************************************************************************/
CBoom::CBoom()
{
	InitSystem();
}


CBoom::~CBoom()
{
	DestroySystem();
}


VOID CBoom::LoadTextr()
{
	CHAR* szFileList[] = 
	{
		"shine0.bmp",
		"shine1.bmp",
		"shine2.bmp",
		"shine3.bmp",
		"shine4.bmp",
		"shine5.bmp",
		"shine6.bmp",
		"shine7.bmp",
		"shine8.bmp",
		"shine9.bmp",
	};
	for ( INT nCnt = 0; nCnt < _MAX_BOOMFRAME; nCnt++ )
	{
		strcpy(m_pszBoomFile[nCnt], szFileList[nCnt]);
		D3DTextr_CreateTextureFromFile(m_pszBoomFile[nCnt]);
		D3DTextr_Restore(m_pszBoomFile[nCnt], g_xMainWnd.Get3DDevice());
	}
}


VOID CBoom::InitSystem()
{
	CParticleSystem::InitSystem();
	m_shPartNum = 0;
	ZeroMemory(m_pszBoomFile, _MAX_BOOMFRAME*MAX_PATH);
}


VOID CBoom::DestroySystem()
{
	CParticleSystem::DestroySystem();

	for ( INT nCnt = 0; nCnt < _MAX_BOOMFRAME; nCnt++ )
	{
		D3DTextr_Invalidate(m_pszBoomFile[nCnt]);
		D3DTextr_DestroyTexture(m_pszBoomFile[nCnt]);
	}

	InitSystem();
}


VOID CBoom::SetupSystem(WORD wCnt)
{
	InitSystem();

	CParticleSystem::SetupSystem(wCnt);

	LoadTextr();

	SetEnvFactor(-0.05f, D3DVECTOR(0, 200, 0));
}


VOID CBoom::UpdateSystem(INT nLoopTime, D3DVECTOR vecGenPos)
{
	INT nSpeedRate = (nLoopTime / 17);
	INT nPartCnt = 0;

	if ( nSpeedRate < 1 ) 		
		nSpeedRate = 1;

	m_fDeltaTime = 0.02f*nSpeedRate;

    for ( INT nCnt = 0; nCnt < m_wNum; nCnt++ )
    {
		if ( !m_pxParticle[nCnt].m_bIsDead )
		{
			m_pxParticle[nCnt].m_wCurrLife += nLoopTime;

			if ( m_pxParticle[nCnt].m_wCurrLife > m_pxParticle[nCnt].m_wLife )
			{
				m_pxParticle[nCnt].Init();
				m_shPartNum--;
				nPartCnt--;
			}
			else
			{
				m_pxParticle[nCnt].m_wCurrDelay += nLoopTime;

				m_pxParticle[nCnt].m_fMass		+= 3.0f;

				if ( m_pxParticle[nCnt].m_fSize < 0 )
					m_pxParticle[nCnt].m_fSize = 0;

				if ( m_pxParticle[nCnt].m_wCurrDelay > m_pxParticle[nCnt].m_wDelay )
				{
					m_pxParticle[nCnt].m_wCurrDelay = 0;
					m_pxParticle[nCnt].m_wCurrFrame++;

					if ( m_pxParticle[nCnt].m_wCurrFrame >= _MAX_BOOMFRAME )
					{
						m_pxParticle[nCnt].m_wCurrFrame = 0;
					}
				}
				m_pxParticle[nCnt].m_fSize = m_pxParticle[nCnt].m_fOriSize - m_pxParticle[nCnt].m_fOriSize*(FLOAT)((FLOAT)m_pxParticle[nCnt].m_wCurrLife/(FLOAT)m_pxParticle[nCnt].m_wLife);

				UpdateAirFiction(nCnt);
				UpdateMove(nCnt);
			}

			nPartCnt++;
			if ( nPartCnt >= m_shPartNum )
				return;
		}
    }
}


HRESULT CBoom::RenderSystem(LPDIRECT3DDEVICE7 lpDevice)
{
	INT nPartCnt = 0;
	if ( lpDevice )
	{
		if( SUCCEEDED(lpDevice->BeginScene()) )
		{
			D3DMATRIX matWorldOriginal;
			D3DMATRIX matTrans;
			D3DMATRIX matScale;
			D3DMATRIX matWorld;
			D3DVECTOR vTrans;
			D3DMATERIAL7 mtrl;

			lpDevice->GetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

			for ( INT nCnt = 0; nCnt < m_wNum; nCnt++ )
			{
				if ( !m_pxParticle[nCnt].m_bIsDead )
				{
					D3DUtil_InitMaterial(mtrl, 
										 (FLOAT)m_pxParticle[nCnt].m_bRed/255.0f,
										 (FLOAT)m_pxParticle[nCnt].m_bGreen/255.0f, 
										 (FLOAT)m_pxParticle[nCnt].m_bBlue/255.0f);
					mtrl.diffuse.a = 0.0f;
					lpDevice->SetMaterial(&mtrl);
				
					lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
					lpDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, TRUE);
					lpDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
					lpDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
					lpDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

					lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
					lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);

  					vTrans.x = (FLOAT) m_pxParticle[nCnt].m_vecPos.x-400;
					vTrans.y = (FLOAT)-m_pxParticle[nCnt].m_vecPos.y+300;
					vTrans.z = (FLOAT) m_pxParticle[nCnt].m_vecPos.z; 

					D3DUtil_SetScaleMatrix(matScale, m_pxParticle[nCnt].m_fSize, m_pxParticle[nCnt].m_fSize, 1);
					D3DUtil_SetTranslateMatrix(matTrans, vTrans);
					D3DMath_MatrixMultiply(matWorld, matScale, matTrans);
					lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);

					lpDevice->SetTexture(0, D3DTextr_GetSurface(m_pszBoomFile[m_pxParticle[nCnt].m_wCurrFrame]));

					lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX, m_avShape, 4, NULL);

					lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

					nPartCnt++;
					if ( nPartCnt >= m_shPartNum )
						break;
				}
			}
			lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
			lpDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, FALSE);
			lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND , D3DBLEND_ONE);
			lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);
			lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);
			lpDevice->EndScene();

			return S_OK;
		}
	}
	return E_FAIL;
}


VOID CBoom::SetBoomParticle(D3DVECTOR vecGenPos)
{
	INT nGenCnt = 0;
	for ( INT nCnt = 0; nCnt < m_wNum; nCnt++ )
	{
		if ( m_pxParticle[nCnt].m_bIsDead )
		{
			SetParticleDefault(nCnt, vecGenPos);
			m_shPartNum++;

			nGenCnt++;

			if ( nGenCnt > 10 )
				return;
		}
	}

/*	for ( INT nCnt = 0; nCnt < m_wNum; nCnt++ )
	{
		if ( m_pxParticle[nCnt].m_bIsDead )
		{
//			vecGenPos.x += GetRandomNum(-2, 2);
//			vecGenPos.y += GetRandomNum(-2, 2);
//			vecGenPos.z += GetRandomNum(-10, 10);
			SetParticleDefault(nCnt, vecGenPos);
		}
	}
*/
}



VOID CBoom::SetParticleDefault(WORD wNum, D3DVECTOR vecGenPos)
{
	ZeroMemory(&m_pxParticle[wNum], sizeof(CParticle));
    m_pxParticle[wNum].m_vecPos		= vecGenPos;
//    m_pxParticle[wNum].m_vecVel		= D3DVECTOR((FLOAT)GetRandomNum(-40, 40), (FLOAT)GetRandomNum(-100, -20), 0.0f);
//    m_pxParticle[wNum].m_vecVel		= D3DVECTOR((FLOAT)GetRandomNum(-60, 60), (FLOAT)GetRandomNum(-80, 0), 0.0f);
//    m_pxParticle[wNum].m_vecVel		= D3DVECTOR((FLOAT)GetRandomNum(-50, 50), (FLOAT)GetRandomNum(-100, -20), 0.0f);
//    m_pxParticle[wNum].m_vecVel		= D3DVECTOR((FLOAT)GetRandomNum(-60, 60), (FLOAT)GetRandomNum(-150, -50), 0.0f);
    m_pxParticle[wNum].m_vecVel		= D3DVECTOR((FLOAT)GetRandomNum(-75, 75), (FLOAT)GetRandomNum(-180, -50), 0.0f);
	m_pxParticle[wNum].m_wLife		= (FLOAT)GetRandomNum(800, 1200);
	m_pxParticle[wNum].m_fMass		= 1.0f;
	m_pxParticle[wNum].m_fSize		= m_pxParticle[wNum].m_fOriSize = (FLOAT)GetRandomNum(5, 40) + _RAND_FLOAT;
//	m_pxParticle[wNum].m_wLife		= (FLOAT)GetRandomNum((BYTE)m_pxParticle[wNum].m_fSize*2000,  6000);
//	m_pxParticle[wNum].m_bIsDead	= FALSE;
/*	m_pxParticle[wNum].m_bRed		= m_pxParticle[wNum].m_bFstRed   = (FLOAT)GetRandomNum(200, 255);
	m_pxParticle[wNum].m_bGreen		= m_pxParticle[wNum].m_bFstGreen = (FLOAT)GetRandomNum(0, 150);
	m_pxParticle[wNum].m_bBlue		= m_pxParticle[wNum].m_bFstBlue  = (FLOAT)GetRandomNum(0, 150);
*/
/*	m_pxParticle[wNum].m_bRed		= m_pxParticle[wNum].m_bFstRed   = (FLOAT)GetRandomNum(150, 200);
	m_pxParticle[wNum].m_bGreen		= m_pxParticle[wNum].m_bFstGreen = (FLOAT)GetRandomNum(150, 200);
	m_pxParticle[wNum].m_bBlue		= m_pxParticle[wNum].m_bFstBlue  = (FLOAT)GetRandomNum(200, 255);
*/
	m_pxParticle[wNum].m_bRed		= m_pxParticle[wNum].m_bFstRed   = (FLOAT)GetRandomNum(0, 255);
	m_pxParticle[wNum].m_bGreen		= m_pxParticle[wNum].m_bFstGreen = m_pxParticle[wNum].m_bFstRed/2;
	m_pxParticle[wNum].m_bBlue		= m_pxParticle[wNum].m_bFstBlue  = 0;
	m_pxParticle[wNum].m_wDelay		= (FLOAT)GetRandomNum(200, 300);

//	m_pxParticle[wNum].m_wCurrLife	= 0;
//	m_pxParticle[wNum].m_wCurrDelay = 0;
//	m_pxParticle[wNum].m_wCurrFrame = 0;
//	m_pxParticle[wNum].m_bOpa		= 255;
}













/******************************************************************************************************************

	CFlyingTail Class	

*******************************************************************************************************************/
CFlyingTail::CFlyingTail()
{
	InitSystem();
}


CFlyingTail::~CFlyingTail()
{
	DestroySystem();
}


VOID CFlyingTail::LoadTextr()
{
	CHAR* szFileList[] = 
	{
		"shine0.bmp",
		"shine1.bmp",
		"shine2.bmp",
		"shine3.bmp",
		"shine4.bmp",
		"shine5.bmp",
		"shine6.bmp",
		"shine7.bmp",
		"shine8.bmp",
		"shine9.bmp",
	};
	for ( INT nCnt = 0; nCnt < _MAX_FLYTAILFRAME; nCnt++ )
	{
		strcpy(m_pszFlyingTailFile[nCnt], szFileList[nCnt]);
		D3DTextr_CreateTextureFromFile(m_pszFlyingTailFile[nCnt]);
		D3DTextr_Restore(m_pszFlyingTailFile[nCnt], g_xMainWnd.Get3DDevice());
	}
}


VOID CFlyingTail::InitSystem()
{
	CParticleSystem::InitSystem();
	m_shPartNum = 0;
	ZeroMemory(m_pszFlyingTailFile, _MAX_FLYTAILFRAME*MAX_PATH);
}


VOID CFlyingTail::DestroySystem()
{
	CParticleSystem::DestroySystem();

	for ( INT nCnt = 0; nCnt < _MAX_FLYTAILFRAME; nCnt++ )
	{
		D3DTextr_Invalidate(m_pszFlyingTailFile[nCnt]);
		D3DTextr_DestroyTexture(m_pszFlyingTailFile[nCnt]);
	}

	InitSystem();
}


VOID CFlyingTail::SetupSystem(WORD wCnt)
{
	InitSystem();
	CParticleSystem::SetupSystem(wCnt);

	LoadTextr();

	SetEnvFactor(-0.05f, D3DVECTOR(100, 1000, 0));
}


VOID CFlyingTail::SetFlyTailParticle(D3DVECTOR vecGenPos)
{
	INT nGenCnt = 0;
	for ( INT nCnt = 0; nCnt < m_wNum; nCnt++ )
	{
		if ( m_pxParticle[nCnt].m_bIsDead )
		{
			SetParticleDefault(nCnt, vecGenPos);
			m_shPartNum++;

			nGenCnt++;

			if ( nGenCnt > 2 )
				return;
		}
	}
}


VOID CFlyingTail::UpdateSystem(INT nLoopTime, D3DVECTOR vecGenPos)
{
	BYTE bRate;
	INT  nSpeedRate, nPartCnt;
	
	nPartCnt = 0;

	nSpeedRate = (nLoopTime / 17);

	if ( nSpeedRate < 1 ) 	nSpeedRate = 1;

	int check = 0;

	m_fDeltaTime = 0.02f*nSpeedRate;

    for ( INT nCnt = 0; nCnt < m_wNum; nCnt++ )
    {
		if ( !m_pxParticle[nCnt].m_bIsDead )
		{
			m_pxParticle[nCnt].m_wCurrLife += nLoopTime;

			if ( m_pxParticle[nCnt].m_wCurrLife > m_pxParticle[nCnt].m_wLife )
			{				
				m_pxParticle[nCnt].Init();
				m_shPartNum--;
				nPartCnt--;
			}
			else
			{
				m_pxParticle[nCnt].m_wCurrDelay += nLoopTime;
				if ( m_pxParticle[nCnt].m_wCurrDelay > m_pxParticle[nCnt].m_wDelay )
				{
					m_pxParticle[nCnt].m_wCurrDelay = 0;
					m_pxParticle[nCnt].m_wCurrFrame++;

					if ( m_pxParticle[nCnt].m_wCurrFrame >= _MAX_FLYTAILFRAME )
					{
						m_pxParticle[nCnt].m_wCurrFrame = 0;
					}
				}

				bRate = m_pxParticle[nCnt].m_bFstRed - m_pxParticle[nCnt].m_bFstRed*(FLOAT)((FLOAT)m_pxParticle[nCnt].m_wCurrLife/(FLOAT)m_pxParticle[nCnt].m_wLife);
				m_pxParticle[nCnt].m_bRed	= bRate;
				bRate = m_pxParticle[nCnt].m_bFstGreen - m_pxParticle[nCnt].m_bFstGreen*(FLOAT)((FLOAT)m_pxParticle[nCnt].m_wCurrLife/(FLOAT)m_pxParticle[nCnt].m_wLife);
				m_pxParticle[nCnt].m_bGreen = bRate;
				bRate = m_pxParticle[nCnt].m_bFstBlue - m_pxParticle[nCnt].m_bFstBlue*(FLOAT)((FLOAT)m_pxParticle[nCnt].m_wCurrLife/(FLOAT)m_pxParticle[nCnt].m_wLife);
				m_pxParticle[nCnt].m_bBlue	= bRate;

				UpdateAirFiction(nCnt);
				UpdateMove(nCnt);
			}

			nPartCnt++;
			if ( nPartCnt >= m_shPartNum )
			{
				return;
			}
		}
    }
}


HRESULT CFlyingTail::RenderSystem(LPDIRECT3DDEVICE7 lpDevice)
{
	INT  nPartCnt;
	
	nPartCnt = 0;

	if ( lpDevice )
	{
		if( SUCCEEDED(lpDevice->BeginScene()) )
		{
			D3DMATRIX matWorldOriginal;
			D3DMATRIX matTrans;
			D3DMATRIX matScale;
			D3DMATRIX matWorld;
			D3DVECTOR vTrans;
			D3DMATERIAL7 mtrl;

			lpDevice->GetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

			for ( INT nCnt = 0; nCnt < m_wNum; nCnt++ )
			{
				if ( !m_pxParticle[nCnt].m_bIsDead )
				{
					D3DUtil_InitMaterial(mtrl, 
										 (FLOAT)m_pxParticle[nCnt].m_bRed/255.0f,
										 (FLOAT)m_pxParticle[nCnt].m_bGreen/255.0f, 
										 (FLOAT)m_pxParticle[nCnt].m_bBlue/255.0f);
					mtrl.diffuse.a = 0.0f;
					lpDevice->SetMaterial(&mtrl);
				
					lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
					lpDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, TRUE);
					lpDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
					lpDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
					lpDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

					lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
					lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);

  					vTrans.x = (FLOAT) m_pxParticle[nCnt].m_vecPos.x-400;
					vTrans.y = (FLOAT)-m_pxParticle[nCnt].m_vecPos.y+300;
					vTrans.z = (FLOAT) m_pxParticle[nCnt].m_vecPos.z; 

					D3DUtil_SetScaleMatrix(matScale, m_pxParticle[nCnt].m_fSize, m_pxParticle[nCnt].m_fSize, 1);
					D3DUtil_SetTranslateMatrix(matTrans, vTrans);
					D3DMath_MatrixMultiply(matWorld, matScale, matTrans);
					lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);

					lpDevice->SetTexture(0, D3DTextr_GetSurface(m_pszFlyingTailFile[m_pxParticle[nCnt].m_wCurrFrame]));
					lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX, m_avShape, 4, NULL);

					lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

					nPartCnt++;

					if ( nPartCnt >= m_shPartNum )
						break;
				}
			}

			lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
			lpDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, FALSE);
			lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND , D3DBLEND_ONE);
			lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);
			lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);
			lpDevice->EndScene();

			return S_OK;
		}
	}
	return E_FAIL;
}


VOID CFlyingTail::SetParticleDefault(WORD wNum, D3DVECTOR vecGenPos)
{
	ZeroMemory(&m_pxParticle[wNum], sizeof(CParticle));
    m_pxParticle[wNum].m_vecPos		= vecGenPos;
    m_pxParticle[wNum].m_vecVel		= D3DVECTOR((FLOAT)GetRandomNum(-50, 50), (FLOAT)GetRandomNum(-30, 60), 0.0f);

//	m_pxParticle[wNum].m_wLife		= (FLOAT)GetRandomNum(500, 1000);
	m_pxParticle[wNum].m_wLife		= (FLOAT)GetRandomNum(150, 400);

	m_pxParticle[wNum].m_fMass		= 1000.0f;
	m_pxParticle[wNum].m_fSize		= (FLOAT)GetRandomNum(5, 40) + _RAND_FLOAT;
	m_pxParticle[wNum].m_bRed		= m_pxParticle[wNum].m_bFstRed   = (FLOAT)GetRandomNum(200, 255);
	m_pxParticle[wNum].m_bGreen		= m_pxParticle[wNum].m_bFstGreen = m_pxParticle[wNum].m_bFstRed/2;//(FLOAT)GetRandomNum(0, 125);
	m_pxParticle[wNum].m_bBlue		= m_pxParticle[wNum].m_bFstBlue  = (FLOAT)GetRandomNum(0, 30);
/*
	m_pxParticle[wNum].m_bRed		= m_pxParticle[wNum].m_bFstRed   = (FLOAT)GetRandomNum(200, 255);
	m_pxParticle[wNum].m_bGreen		= m_pxParticle[wNum].m_bFstGreen = (FLOAT)GetRandomNum(0, 150);
	m_pxParticle[wNum].m_bBlue		= m_pxParticle[wNum].m_bFstBlue  = (FLOAT)GetRandomNum(0, 150);
*/	m_pxParticle[wNum].m_wDelay		= (FLOAT)GetRandomNum(200, 300);
}











/******************************************************************************************************************

	CSmoke Class	

*******************************************************************************************************************/
CSmoke::CSmoke()
{
	InitSystem();
}


CSmoke::~CSmoke()
{
	DestroySystem();
}


VOID CSmoke::LoadTextr()
{
	CHAR* szFileList[] = 
	{
		"Smoke.bmp",
	};
	for ( INT nCnt = 0; nCnt < _MAX_SMOKEFRAME; nCnt++ )
	{
		strcpy(m_pszSmokeFile[nCnt], szFileList[nCnt]);
		D3DTextr_CreateTextureFromFile(m_pszSmokeFile[nCnt], 0L, D3DTEXTR_TRANSPARENTBLACK);
		D3DTextr_Restore(m_pszSmokeFile[nCnt], g_xMainWnd.Get3DDevice());
	}
}


VOID CSmoke::InitSystem()
{
	CParticleSystem::InitSystem();
	m_shPartNum = 0;
	ZeroMemory(m_pszSmokeFile, _MAX_SMOKEFRAME*MAX_PATH);
}


VOID CSmoke::DestroySystem()
{
	CParticleSystem::DestroySystem();

	for ( INT nCnt = 0; nCnt < _MAX_SMOKEFRAME; nCnt++ )
	{
		D3DTextr_Invalidate(m_pszSmokeFile[nCnt]);
		D3DTextr_DestroyTexture(m_pszSmokeFile[nCnt]);
	}

	InitSystem();
}


VOID CSmoke::SetupSystem(WORD wCnt)
{
	InitSystem();
	CParticleSystem::SetupSystem(wCnt);

	LoadTextr();

	SetEnvFactor(-0.05f, D3DVECTOR(0, 1000, 0));
}


VOID CSmoke::UpdateSystem(INT nLoopTime, D3DVECTOR vecGenPos)
{
	BYTE bRate;
	INT  nSpeedRate, nPartCnt;
	D3DVECTOR vecAddVel;
	
	nPartCnt = 0;

	nSpeedRate = (nLoopTime / 17);

	if ( nSpeedRate < 1 ) 	nSpeedRate = 1;

	m_fDeltaTime = 0.02f*nSpeedRate;

//	SetSmokeParticle(vecGenPos);

    for ( INT nCnt = 0; nCnt < m_wNum; nCnt++ )
    {
		if ( !m_pxParticle[nCnt].m_bIsDead )
		{
			m_pxParticle[nCnt].m_wCurrLife += nLoopTime;

			if ( m_pxParticle[nCnt].m_wCurrLife > m_pxParticle[nCnt].m_wLife )
			{				
				m_pxParticle[nCnt].Init();
				m_shPartNum--;
				nPartCnt--;
			}
			else
			{
				m_pxParticle[nCnt].m_wCurrDelay += nLoopTime;
				if ( m_pxParticle[nCnt].m_wCurrDelay > m_pxParticle[nCnt].m_wDelay )
				{
					m_pxParticle[nCnt].m_wCurrDelay = 0;
					m_pxParticle[nCnt].m_wCurrFrame++;

					if ( m_pxParticle[nCnt].m_wCurrFrame >= _MAX_SMOKEFRAME )
					{
						m_pxParticle[nCnt].m_wCurrFrame = 0;
					}
				}

//				m_pxParticle[nCnt].m_fSize = m_pxParticle[nCnt].m_fOriSize + m_pxParticle[nCnt].m_fOriSize*15.0f*(FLOAT)((FLOAT)m_pxParticle[nCnt].m_wCurrLife/(FLOAT)m_pxParticle[nCnt].m_wLife);
//				vecAddVel = D3DVECTOR((FLOAT)GetRandomNum(-1, 1), GetRandomNum(-2, 0), 0.0f);
				m_pxParticle[nCnt].m_fSize = m_pxParticle[nCnt].m_fOriSize + m_pxParticle[nCnt].m_fOriSize*7.0f*(FLOAT)((FLOAT)m_pxParticle[nCnt].m_wCurrLife/(FLOAT)m_pxParticle[nCnt].m_wLife);
				vecAddVel = D3DVECTOR((FLOAT)GetRandomNum(-1, 1), 0.0f, 0.0f);
				m_pxParticle[nCnt].m_vecVel += vecAddVel;

				bRate = m_pxParticle[nCnt].m_bFstRed - m_pxParticle[nCnt].m_bFstRed*(FLOAT)((FLOAT)m_pxParticle[nCnt].m_wCurrLife/(FLOAT)m_pxParticle[nCnt].m_wLife);
				m_pxParticle[nCnt].m_bRed	= bRate;
				bRate = m_pxParticle[nCnt].m_bFstGreen - m_pxParticle[nCnt].m_bFstGreen*(FLOAT)((FLOAT)m_pxParticle[nCnt].m_wCurrLife/(FLOAT)m_pxParticle[nCnt].m_wLife);
				m_pxParticle[nCnt].m_bGreen = bRate;
				bRate = m_pxParticle[nCnt].m_bFstBlue - m_pxParticle[nCnt].m_bFstBlue*(FLOAT)((FLOAT)m_pxParticle[nCnt].m_wCurrLife/(FLOAT)m_pxParticle[nCnt].m_wLife);
				m_pxParticle[nCnt].m_bBlue	= bRate;

				UpdateAirFiction(nCnt);
				UpdateMove(nCnt);
			}

			nPartCnt++;
			if ( nPartCnt >= m_shPartNum )
				return;
		}
    }
}


VOID CSmoke::SetSmokeParticle(D3DVECTOR vecGenPos)
{
	INT nGenCnt = 0;
	for ( INT nCnt = 0; nCnt < m_wNum; nCnt++ )
	{
		if ( m_pxParticle[nCnt].m_bIsDead )
		{
			SetParticleDefault(nCnt, vecGenPos);
			m_shPartNum++;

			nGenCnt++;

			if ( nGenCnt > 0 )
				return;
		}
	}
}


HRESULT CSmoke::RenderSystem(LPDIRECT3DDEVICE7 lpDevice)
{
	INT  nPartCnt;
	
	nPartCnt = 0;

	if ( lpDevice )
	{
		if( SUCCEEDED(lpDevice->BeginScene()) )
		{
			D3DMATRIX matWorldOriginal;
			D3DMATRIX matTrans;
			D3DMATRIX matScale;
			D3DMATRIX matWorld;
			D3DVECTOR vTrans;
			D3DMATERIAL7 mtrl;

			lpDevice->GetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

			for ( INT nCnt = 0; nCnt < m_wNum; nCnt++ )
			{
				if ( !m_pxParticle[nCnt].m_bIsDead )
				{
					D3DUtil_InitMaterial(mtrl, 
										 (FLOAT)m_pxParticle[nCnt].m_bRed/255.0f,
										 (FLOAT)m_pxParticle[nCnt].m_bGreen/255.0f, 
										 (FLOAT)m_pxParticle[nCnt].m_bBlue/255.0f);
					mtrl.diffuse.a = 0.0f;
					lpDevice->SetMaterial(&mtrl);
				
					/*
					lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
					lpDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, TRUE);
					lpDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
					lpDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
					lpDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

					lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCCOLOR);
					lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE);
					*/
					SetBlendRenderState(lpDevice, _BLEND_LIGHTINV, mtrl);

  					vTrans.x = (FLOAT) m_pxParticle[nCnt].m_vecPos.x-400;
					vTrans.y = (FLOAT)-m_pxParticle[nCnt].m_vecPos.y+300;
					vTrans.z = (FLOAT) m_pxParticle[nCnt].m_vecPos.z; 

					D3DUtil_SetScaleMatrix(matScale, m_pxParticle[nCnt].m_fSize, m_pxParticle[nCnt].m_fSize, 1);
					D3DUtil_SetTranslateMatrix(matTrans, vTrans);
					D3DMath_MatrixMultiply(matWorld, matScale, matTrans);
					lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);

					lpDevice->SetTexture(0, D3DTextr_GetSurface(m_pszSmokeFile[m_pxParticle[nCnt].m_wCurrFrame]));
					lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX, m_avShape, 4, NULL);

					lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

					nPartCnt++;

					if ( nPartCnt >= m_shPartNum )
						break;
				}
			}

			ResetBlendenderState(lpDevice);

/*
			lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
			lpDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, FALSE);
			lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND , D3DBLEND_ONE);
			lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);
			lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);*/
			lpDevice->EndScene();

			return S_OK;
		}
	}
	return E_FAIL;
}


VOID CSmoke::SetParticleDefault(WORD wNum, D3DVECTOR vecGenPos)
{
	ZeroMemory(&m_pxParticle[wNum], sizeof(CParticle));
//	vecGenPos.x += (FLOAT)GetRandomNum(-5, 5);
//	vecGenPos.y += (FLOAT)GetRandomNum(-8, 2);
    m_pxParticle[wNum].m_vecPos		= vecGenPos;
    m_pxParticle[wNum].m_vecVel		= D3DVECTOR((FLOAT)GetRandomNum(-8, 8), /*(FLOAT)GetRandomNum(-15, -10)*/0.0f, 0.0f);
//	m_pxParticle[wNum].m_wLife		= (FLOAT)GetRandomNum(500, 1000);
	m_pxParticle[wNum].m_wLife		= (FLOAT)GetRandomNum(150, 400);
	m_pxParticle[wNum].m_fMass		= 1000.0f;
//	m_pxParticle[wNum].m_fSize		= m_pxParticle[wNum].m_fOriSize = (FLOAT)GetRandomNum(5, 10) + _RAND_FLOAT;
	m_pxParticle[wNum].m_fSize		= m_pxParticle[wNum].m_fOriSize = (FLOAT)GetRandomNum(5, 10) + _RAND_FLOAT;
	m_pxParticle[wNum].m_bRed		= m_pxParticle[wNum].m_bFstRed = m_pxParticle[wNum].m_bGreen = m_pxParticle[wNum].m_bFstGreen = m_pxParticle[wNum].m_bBlue = m_pxParticle[wNum].m_bFstBlue = (FLOAT)GetRandomNum(100, 150);
	m_pxParticle[wNum].m_wDelay		= (FLOAT)GetRandomNum(200, 300);
}



/******************************************************************************************************************

	CElec Class	

*******************************************************************************************************************/
CElec::CElec()
{
	InitElec();
}

CElec::~CElec()
{
	DestroyElec();
}


VOID CElec::LoadTextr()
{
	CHAR* szFileList[] = 
	{
		"Elec.bmp",
	};
	for ( INT nCnt = 0; nCnt < _MAX_ELECFRAME; nCnt++ )
	{
		strcpy(m_szElecFile[nCnt], szFileList[nCnt]);
		D3DTextr_CreateTextureFromFile(m_szElecFile[nCnt]);
		D3DTextr_Restore(m_szElecFile[nCnt], g_xMainWnd.Get3DDevice());
	}
}


VOID CElec::SetupElec()
{
	D3DVECTOR vNorm(0, 0, -1);

	for ( INT nCntY = 0; nCntY < (_MAX_ELEC_NODEY+1); nCntY++ )
	{
		for ( INT nCntX = 0; nCntX < (_MAX_ELEC_NODEX+1); nCntX++ )
		{
			FLOAT fXPos = -0.5f+(FLOAT)nCntX/(FLOAT)(_MAX_ELEC_NODEX);
			FLOAT fYPos = /*-0.5f+*/(FLOAT)nCntY/(FLOAT)(_MAX_ELEC_NODEY);
			FLOAT fu = (FLOAT)nCntX/(FLOAT)(_MAX_ELEC_NODEX);
			FLOAT fv = (FLOAT)nCntY/(FLOAT)(_MAX_ELEC_NODEY);

			m_avElec[nCntY][nCntX] = D3DVERTEX(D3DVECTOR(fXPos, fYPos, 0), vNorm, fu, fv);
//			m_avElec[nCntY][nCntX] = D3DLVERTEX(D3DVECTOR(fXPos, fYPos, 0), RGB(0, 255, 0), RGB(255, 0, 0), fu, fv);
		}
	}

	memcpy(m_avSaveElec, m_avElec, (_MAX_ELEC_NODEY+1)*(_MAX_ELEC_NODEX+1)*sizeof(D3DVERTEX));

	// 타일폴리곤의 인덱스 생성.
    WORD* pwIndex = m_pwElecIndices;
	for ( nCntY = 0; nCntY < _MAX_ELEC_NODEY; nCntY++ )
	{
		for ( INT nCntX = 0; nCntX < _MAX_ELEC_NODEX; nCntX++ )
		{
			*pwIndex++ = (nCntX+1)+((_MAX_ELEC_NODEX+1)*nCntY);
			*pwIndex++ = nCntX+((_MAX_ELEC_NODEX+1)*(nCntY+1));
			*pwIndex++ = (nCntX+1)+((_MAX_ELEC_NODEX+1)*(nCntY+1));
			*pwIndex++ = nCntX+((_MAX_ELEC_NODEX+1)*(nCntY+1));
			*pwIndex++ = (nCntX+1)+((_MAX_ELEC_NODEX+1)*nCntY);
			*pwIndex++ = (nCntX)+((_MAX_ELEC_NODEX+1)*nCntY);
		}
	}

	LoadTextr();
}

VOID CElec::InitElec()
{
	ZeroMemory(m_szElecFile, _MAX_SNOWFRAME*MAX_PATH);
	ZeroMemory(m_pwElecIndices, _ELEC_INDICES*sizeof(WORD));
	ZeroMemory(m_avElec, sizeof(D3DVERTEX)*_MAX_ELEC_NODEY+1*_MAX_ELEC_NODEX+1);
	ZeroMemory(m_avSaveElec, (_MAX_ELEC_NODEY+1)*(_MAX_ELEC_NODEX+1)*sizeof(D3DVERTEX));

	m_fSize		= 0;
	m_fRadian	= 0;
	m_shPosX	= 0;
	m_shPosY	= 0;
	m_shDstPosX = 0;
	m_shDstPosY = 0;
}

VOID CElec::DestroyElec()
{
	for ( INT nCnt = 0; nCnt < _MAX_ELECFRAME; nCnt++ )
	{
		D3DTextr_Invalidate(m_szElecFile[nCnt]);
		D3DTextr_DestroyTexture(m_szElecFile[nCnt]);
	}

	InitElec();
}

VOID CElec::UpdateElec(SHORT shPosX, SHORT shPosY, SHORT shDstPosX, SHORT shDstPosY)
{
	DOUBLE	dTheta	= 0.0f;
	FLOAT	fDiv	= 0.0f;

	m_shPosX = shPosX;
	m_shPosY = shPosY;
	m_shDstPosX = shDstPosX;
	m_shDstPosY = shDstPosY;

	INT nAbsX = abs(m_shDstPosX - m_shPosX);
	INT nAbsY = abs(m_shDstPosY - m_shPosY);

	m_fSize = (FLOAT)sqrt((double)(nAbsX*nAbsX + nAbsY*nAbsY));

//	fDiv = (FLOAT)(nAbsY)/(FLOAT)(nAbsX);

	fDiv = (FLOAT)-(m_shDstPosX - m_shPosX)/m_fSize;

	m_fRadian = (FLOAT)asin(fDiv);

	if ( m_shDstPosY - m_shPosY > 0 )
		m_fRadian = g_PI/2 + g_PI/2 - m_fRadian;


//	for ( INT nCntY = 0; nCntY < (_MAX_ELEC_NODEY+1); nCntY++ )

	static int nCnt = 0;

	nCnt++;

	if ( nCnt >= 3 )
		nCnt = 0;

	if ( nCnt == 2 )
	{
	
	for ( INT nCntY = 1; nCntY < _MAX_ELEC_NODEY; nCntY++ )
	{
//		FLOAT fRandX = _RAND_FLOAT*1.3f	 - 1.0f;
		FLOAT fRandX = _RAND_FLOAT*0.8f - 0.4f;
		FLOAT fRandY = _RAND_FLOAT*0.2f;
		for ( INT nCntX = 0; nCntX < (_MAX_ELEC_NODEX+1); nCntX++ )
		{
			m_avElec[nCntY][nCntX].x = m_avSaveElec[nCntY][nCntX].x+fRandX;
//			m_avElec[nCntY][nCntX].y = m_avSaveElec[nCntY][nCntX].y+fRandY;
		}
	}

	}
	
/*
	CHAR szBuff[MAX_PATH];
	sprintf(szBuff, "->라디안:[%f] %f", m_fRadian, fDiv);
	g_xMainWnd.PutsHan(g_xMainWnd.GetBackBuffer(), 0, 100, RGB(250, 125, 0), RGB(0, 0, 0), szBuff);
*/
}

HRESULT CElec::RenderElec(LPDIRECT3DDEVICE7 lpDevice)
{
	if ( lpDevice )
	{
		if( SUCCEEDED(lpDevice->BeginScene()) )
		{
			D3DMATRIX matWorldOriginal;
			D3DMATRIX matTrans;
			D3DMATRIX matScale;
			D3DMATRIX matRot;
			D3DMATRIX matTmpWorld;
			D3DMATRIX matWorld;
			D3DVECTOR vTrans;
			D3DMATERIAL7 mtrl;

			lpDevice->GetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

			D3DUtil_InitMaterial(mtrl, 
								 (FLOAT)200/255.0f,
								 (FLOAT)200/255.0f, 
								 (FLOAT)255/255.0f);
			mtrl.diffuse.a = 0.0f;
			lpDevice->SetMaterial(&mtrl);

			SetBlendRenderState(lpDevice, _BLEND_NORMAL, mtrl);

/*			lpDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
			lpDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, TRUE);
			lpDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			lpDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
			lpDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

			lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
			lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);
*/
  			vTrans.x = (FLOAT)m_shPosX-400;
			vTrans.y = (FLOAT)-m_shPosY+300;
			vTrans.z = (FLOAT)0; 

			D3DUtil_SetRotateZMatrix(matRot, m_fRadian);
//			D3DUtil_SetRotateZMatrix(matRot, g_PI*2/2);
			D3DUtil_SetTranslateMatrix(matTrans, vTrans);
			D3DUtil_SetScaleMatrix(matScale, 128/2, 512	, 1);

			D3DMath_MatrixMultiply(matTmpWorld, matRot, matTrans);
			D3DMath_MatrixMultiply(matWorld, matScale, matTmpWorld);

			lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);

//			lpDevice->SetTexture(0, NULL);
			lpDevice->SetTexture(0, D3DTextr_GetSurface(m_szElecFile[0]));
//			lpDevice->SetRenderState(D3DRENDERSTATE_LIGHTING, FALSE);
			lpDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
										   D3DFVF_VERTEX,
										   m_avElec, _ELEC_VERTICES, 
										   m_pwElecIndices, _ELEC_INDICES, NULL);

/*			lpDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
										   D3DFVF_LVERTEX,
										   m_avElec, _ELEC_VERTICES, 
										   m_pwElecIndices, _ELEC_INDICES, NULL);
*/
			lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);
//			lpDevice->SetRenderState(D3DRENDERSTATE_LIGHTING, TRUE);

			ResetBlendenderState(lpDevice);
/*			lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
			lpDevice->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, FALSE);
			lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND , D3DBLEND_ONE);
			lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);
*/
			lpDevice->EndScene();

			return S_OK;
		}
	}
	return E_FAIL;
}