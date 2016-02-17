#include "StdAfx.h"


CMagic::CMagic()
{
	m_bActiveState	= _MAGIC_FINISH;
	m_bRepeat		= FALSE;
	m_bFixed		= FALSE;
	m_bShowLight	= TRUE;

	m_pxOwner		= NULL;
	m_pxTarget		= NULL;
	m_pxMap			= NULL;

	m_shScrnX		= m_shScrnY		  = 0;
	m_shTileX		= m_shTileY		  = 0;
	m_shFireScrnX	= m_shFireScrnY	  = 0;
	m_shFireTileX	= m_shFireTileY	  = 0;
	m_shTargetScrnX = m_shTargetScrnY = 0;
	m_shTargetTileX = m_shTargetTileY = 0;	

	m_bMagicNum		= 0;

	m_wCurrDelay	= 0;	
	m_wFrmDelay		= 0;
	m_wMagicLife	= 0;

	m_dwFstFrame	= 0;				
	m_dwEndFrame	= 0;
	m_dwCurrFrame	= 0;

	m_bBlendType	= 0;
	m_bOpacity		= 0;
	m_bSwingCnt		= 0;
	m_bDir16		= 0;

	m_nPrevDistanceX = 10000;
	m_nPrevDistanceY = 10000;

	ZeroMemory(m_bLightRadius, 2);
	ZeroMemory(m_bLightColor, 2*3);
	ZeroMemory(m_bMagicColor, 3);

	m_nDuplicateNum	 = MAX_DUPLE_COUNT;

	ZeroMemory(m_lpddsMagicTextr, sizeof(LPDIRECTDRAWSURFACE7)*60);
}


CMagic::~CMagic()
{
}


BOOL CMagic::CreateMagic(BYTE bMagicNum, SHORT shFireTileX, SHORT shFireTileY, 
		                 SHORT shTargetTileX, SHORT shTargetTileY, CActor* pxOwner, INT nTarget)
{
	LPEFFECTSPRINFO pstMagicSpr = g_xSpriteInfo.GetMagicInfo(bMagicNum);

	if ( pstMagicSpr )
	{
		//------------------------------------------------------------------------------------------------------------
		// 좌표세팅.
		//------------------------------------------------------------------------------------------------------------
		m_pxMap		= &g_xGameProc.m_xMap;

		m_pxTarget	= NULL;
		if ( nTarget )
		{
			if ( nTarget == g_xGameProc.m_xMyHero.m_dwIdentity )
			{
				m_pxTarget = &g_xGameProc.m_xMyHero;
			}
			else
			{
				g_xGameProc.m_xActorList.MoveCurrentToTop();
				CActor* pxActor;
				for ( INT nCnt = 0; nCnt < g_xGameProc.m_xActorList.GetCounter(); nCnt++ )
				{
					pxActor = g_xGameProc.m_xActorList.GetCurrentData();

					if ( pxActor->m_dwIdentity == nTarget )
					{
						m_pxTarget = pxActor;
					}
					g_xGameProc.m_xActorList.MoveNextNode();
				}
			}
		}

		// 발사지점.
		m_shFireTileX = shFireTileX;
		m_shFireTileY = shFireTileY;
		m_pxMap->GetScrnPosFromTilePos(m_shFireTileX, m_shFireTileY, m_shFireScrnX, m_shFireScrnY);

		// 목적지점.
		m_shTargetTileX = shTargetTileX;
		m_shTargetTileY = shTargetTileY;
		m_pxMap->GetScrnPosFromTilePos(m_shTargetTileX, m_shTargetTileY, m_shTargetScrnX, m_shTargetScrnY);

		// 현재위치.
		m_shTileX = m_shFireTileX;
		m_shTileY = m_shFireTileY;
		m_shScrnX = m_fFloatScrnX = m_shFireScrnX;
		m_shScrnY = m_fFloatScrnY = m_shFireScrnY;

		m_bMagicColor[0] = 255;
		m_bMagicColor[1] = 255;
		m_bMagicColor[2] = 255;

	    SHORT shScrnGapX, shScrnGapY;

	    if ( m_shFireScrnX != m_shTargetScrnX )		shScrnGapX = abs(m_shTargetScrnX-m_shFireScrnX);
	    else										shScrnGapX = 1;

	    if ( m_shFireScrnY != m_shTargetScrnY )		shScrnGapY = abs(m_shTargetScrnY-m_shFireScrnY);
	    else										shScrnGapY = 1;

	    FLOAT fGapRateX = (FLOAT)(500/(FLOAT)shScrnGapX);
	    FLOAT fGapRateY = (FLOAT)(500/(FLOAT)shScrnGapY);

	    if ( shScrnGapX > shScrnGapY )
		{
			m_nDistanceX = (m_shTargetScrnX-m_shFireScrnX) * (fGapRateX);
			m_nDistanceY = (m_shTargetScrnY-m_shFireScrnY) * (fGapRateX);
		}
	    else
		{
			m_nDistanceX = (m_shTargetScrnX-m_shFireScrnX) * (fGapRateY);
			m_nDistanceY = (m_shTargetScrnY-m_shFireScrnY) * (fGapRateY);
		}

		m_bDir16 = (BYTE)m_pxMap->CalcDirection16(m_shFireTileX, m_shFireTileY, shTargetTileX, shTargetTileY);
		//------------------------------------------------------------------------------------------------------------

		//------------------------------------------------------------------------------------------------------------
		// 기본정보입력.
		//------------------------------------------------------------------------------------------------------------
		m_pxOwner	= pxOwner;
		m_bActiveState= _MAGIC_ACTIVE;
		m_bFixed	= TRUE;
		m_bRepeat	= FALSE;
		m_wMagicLife= 0;
		m_bMagicNum	= bMagicNum;
		m_bCurrSwing = 0;
		m_wCurrLightDelay = 0;
		//------------------------------------------------------------------------------------------------------------

		//------------------------------------------------------------------------------------------------------------
		// 마법에 따른 정보세팅.
		//------------------------------------------------------------------------------------------------------------
		switch ( bMagicNum )
		{
		case _SKILL_FIRECHARM:
		case _SKILL_FIREBALL2:
		case _SKILL_FIREBALL:
		case _SKILL_SHOOTLIGHTEN:
				m_bRepeat = TRUE;
				m_bFixed = FALSE;
				break;
		case _SKILL_SINSU:
				m_bRepeat = TRUE;
				m_bFixed = FALSE;
				break;
		case _SKILL_BIGCLOAK:
		case _SKILL_HANGMAJINBUB:
		case _SKILL_DEJIWONHO:	
				m_pxTarget	= NULL;
				m_bRepeat = TRUE;
				m_bFixed = FALSE;
				break;
		// 몬스터.
		case _MONMAGIC_MAARROW:
		case _MONMAGIC_CHIM:
				m_bRepeat = TRUE;
				m_bFixed = FALSE;
				m_bShowLight = FALSE;
				break;
		// 몬스터.
		case _MONMAGIC_ANTHEAL:
		case _MONMAGIC_DUALAXE:
				m_bRepeat = TRUE;
				m_bFixed = FALSE;
				m_bDir16 = (BYTE)m_pxMap->CalcDirection8(m_shFireTileX, m_shFireTileY, shTargetTileX, shTargetTileY);
				m_bShowLight = FALSE;
				break;
		case _SKILL_FIRE:	
				m_bDir16 = 0;                                                    
				m_bRepeat = FALSE;
				m_bFixed = TRUE;
				m_pxTarget	= NULL;
				break;
		// 몬스터.
		case _MONMAGIC_COWGHOST:
		case _MONMAGIC_SINSU:
		case _MONMAGIC_ZOMBIE:
		case _MONMAGIC_JUMAWANG:
		case _MONMAGIC_COWFLAME:
		case _MONMAGIC_SSEGI:
				if ( m_pxOwner )
					m_bDir16 = m_pxOwner->m_bCurrDir;  
				m_bRepeat = FALSE;
				m_bFixed = TRUE;
				m_pxTarget	= NULL;
				m_bShowLight = FALSE;
				break;

		case _SKILL_LIGHTENING:
		case _SKILL_FIREBOOM:	
		case _SKILL_EARTHFIRE:
		case _SKILL_SNOWWIND:	
		case _SKILL_HOLYSHIELD:
		case _SKILL_BIGHEALLING:
				m_bRepeat = FALSE;
				m_bFixed = TRUE;
				m_bDir16 = 0;
				m_pxTarget = NULL;
				break;
		case _SKILL_AMYOUNSUL:
		case _SKILL_MOOTEBO:	
		case _SKILL_TAMMING:	
		case _SKILL_KILLUNDEAD:
		case _SKILL_HEALLING:	
		case _SKILL_SKELLETON:	
		case _SKILL_SHIELD:		
		case _SKILL_SHOWHP:
		case _SKILL_SPACEMOVE2:
		case _SKILL_SPACEMOVE3:
		// 몬스터.
		case _MONMAGIC_KEPAL:
		case _MONMAGIC_EXPLODE:
		case _MONMAGIC_EXPLODE1:
		case _MONMAGIC_BLACK1:
		case _MONMAGIC_BLACK2:
		case _MONMAGIC_GREATANT:
		case _MONMAGIC_MAGENUMA:
		case _MONMAGIC_ANT:
		case _MONMAGIC_WORKANT:
		case _MONMAGIC_BIGGINE_CHAR:
		case _MONMAGIC_RED1:
		case _MONMAGIC_RED2:
		case _MONMAGIC_BAODIE:
		case _MONMAGIC_ZOMBIE_DIE:
		case _MONMAGIC_HUSU_DIE:
		case _MONMAGIC_SINGI_DIE:
		case _MONMAGIC_BIGGINE_ATT:
		case _MONMAGIC_SANDFISH:
				m_bRepeat = FALSE;
				m_bFixed = TRUE;
				m_bDir16 = 0;
				m_bShowLight = FALSE;
				break;
		}
		//------------------------------------------------------------------------------------------------------------

		//------------------------------------------------------------------------------------------------------------
		// 이미지로딩.
		//------------------------------------------------------------------------------------------------------------
		//------------------------------------------------------------------------------------------------------------
		// 프레임정보얻기.
		//------------------------------------------------------------------------------------------------------------
		m_dwFstFrame		= pstMagicSpr->dwFstFrm+(10)*m_bDir16;
		m_dwEndFrame		= pstMagicSpr->dwEndFrm+(10)*m_bDir16;
		m_dwCurrFrame		= m_dwFstFrame;
		m_wFrmDelay			= pstMagicSpr->wDelay;
		m_bLightRadius[0]	= pstMagicSpr->bLightRadius[0];
		m_bLightRadius[1]	= pstMagicSpr->bLightRadius[1];
		m_bLightColor[0][0]	= pstMagicSpr->bLightColor[0][0];
		m_bLightColor[0][1]	= pstMagicSpr->bLightColor[0][1];
		m_bLightColor[0][2]	= pstMagicSpr->bLightColor[0][2];
		m_bLightColor[1][0]	= pstMagicSpr->bLightColor[1][0];
		m_bLightColor[1][1]	= pstMagicSpr->bLightColor[1][1];
		m_bLightColor[1][2]	= pstMagicSpr->bLightColor[1][2];
		m_bBlendType		= pstMagicSpr->bBlendType;
		m_bOpacity			= pstMagicSpr->bOpa;
		m_bSwingCnt			= pstMagicSpr->bSwingCnt;
		//------------------------------------------------------------------------------------------------------------

		WORD wImgIdx = pstMagicSpr->wImgIdx;
		WORD wFileType;
		m_pxMagicImg = &g_xGameProc.m_xImage.m_xImageList[wImgIdx];
		 
		for ( INT nCnt = m_dwFstFrame; nCnt < m_dwEndFrame; nCnt++ )
		{
			if ( wImgIdx == _IMAGE_MAGIC )
			{
				wFileType = _TEXTR_FILE_MAGIC;
			}
			else if ( wImgIdx == _IMAGE_MONMAGIC )
			{
				wFileType = _TEXTR_FILE_MONMAGIC;
			}
			g_xGameProc.m_xImage.AddTextr(wFileType, wImgIdx, nCnt);
			m_lpddsMagicTextr[nCnt-m_dwFstFrame] = g_xGameProc.m_xImage.GetTextrImg(wFileType, wImgIdx, nCnt);
		}

 		D3DVECTOR vNorm(0, 0, -1);
		m_avMagic[0]  = D3DVERTEX(D3DVECTOR(-0.5f, 0.5f, 0), vNorm, 0, 0);
		m_avMagic[1]  = D3DVERTEX(D3DVECTOR(-0.5f,-0.5f, 0), vNorm, 0, 1);
		m_avMagic[2]  = D3DVERTEX(D3DVECTOR( 0.5f, 0.5f, 0), vNorm, 1, 0);
		m_avMagic[3]  = D3DVERTEX(D3DVECTOR( 0.5f,-0.5f, 0), vNorm, 1, 1);
		//------------------------------------------------------------------------------------------------------------

		const INT nMagicSndTbl[31] = 
		{
			 0,  6,  8, 14, 15,  9,  0, 20, 21, 32,
			 1,  2, 16, 29, 24, 10, 17, 33, 19, 31,
			28, 22, 13, 21,  5,  0, 12, 11,  0, 18, 
			30,
		};
		INT nWaveNum = -1;
		BOOL bLoop = FALSE;
		if ( m_bMagicNum == _SKILL_EARTHFIRE )
			bLoop = TRUE;
		static DWORD dwLastTick = 0;
		if ( timeGetTime() - dwLastTick > 100 )
		{
			if ( m_bMagicNum < 31 )
			{
				if ( m_bFixed )
					nWaveNum = nMagicSndTbl[m_bMagicNum]*10 + 10000 + 2;
				else
					nWaveNum = nMagicSndTbl[m_bMagicNum]*10 + 10000 + 1;
			}
/*			if ( nWaveNum > 10010 )
				m_nDuplicateNum = g_xSound.PlayActorWav(m_shTileX, m_shTileY, g_xGameProc.m_xMyHero.m_wPosX, g_xGameProc.m_xMyHero.m_wPosY, nWaveNum, 100, bLoop);
*/		}
		dwLastTick = timeGetTime();

		return TRUE;
	}
	return FALSE;
}


BOOL CMagic::DrawLight(CLightFog* xLightFog, INT nLoopTime)
{
	BYTE bSwingCnt		= 0;				// 프레임이 변화됐을때만.
	m_wCurrLightDelay  += nLoopTime;

	if ( m_bMagicNum == _SKILL_EARTHFIRE || m_bMagicNum == _SKILL_HOLYSHIELD || m_bMagicNum == _SKILL_FIRE || m_bMagicNum == _SKILL_SPACEMOVE2 || m_bMagicNum == _SKILL_SINSU || m_bMagicNum == _SKILL_LIGHTENING )
	{
		if ( m_bActiveState == _MAGIC_ACTIVE )
		{
			if ( m_wCurrLightDelay > 200 )
			{
				m_wCurrLightDelay = 0;
				m_bCurrSwing++;

				if ( m_bCurrSwing > m_bSwingCnt )
				{
					m_bCurrSwing = 0;
				}
			}
		}
		else if( m_bActiveState == _LIGHT_ACTIVE ) 
		{
			m_bCurrSwing = 0;

			m_bLightColor[0][0] -= 2;
			if ( m_bLightColor[0][0] <= 2 )		m_bLightColor[0][0] = 2;

			m_bLightColor[0][1] -= 2;
			if ( m_bLightColor[0][1] <= 2 )		m_bLightColor[0][1] = 2;

			m_bLightColor[0][2] -= 2;
			if ( m_bLightColor[0][2] <= 2 )		m_bLightColor[0][2] = 2;

			m_bLightColor[1][0] -= 2;
			if ( m_bLightColor[1][0] <= 2 )		m_bLightColor[1][0] = 2;

			m_bLightColor[1][1] -= 2;
			if ( m_bLightColor[1][1] <= 2 )		m_bLightColor[1][1] = 2;

			m_bLightColor[1][2] -= 2;
			if ( m_bLightColor[1][2] <= 2 )		m_bLightColor[1][2] = 2;


			if ( m_bLightColor[0][0] == 2 && 	 
				 m_bLightColor[0][1] == 2 &&
				 m_bLightColor[0][2] == 2 &&
				 m_bLightColor[1][0] == 2 && 
				 m_bLightColor[1][1] == 2 &&
				 m_bLightColor[1][2] == 2)
			{
				 m_bActiveState = _MAGIC_FINISH;
				 return FALSE;
			}
		}
	}
	else
	{
		if ( m_wCurrLightDelay > 200 )
		{
			if ( m_bActiveState == _MAGIC_ACTIVE )
			{
				m_wCurrLightDelay = 0;
				m_bCurrSwing++;

				if ( m_bCurrSwing > m_bSwingCnt )
				{
					m_bCurrSwing = 0;
				}
			}
			else if( m_bActiveState == _LIGHT_ACTIVE )
			{
				m_bCurrSwing = 0;
				m_wCurrLightDelay = 150;
				if ( m_bLightRadius[0] > 1 )
				{
					m_bLightRadius[0]--;
					m_bLightRadius[1]--;
				}
				else if ( m_bLightRadius[0] == 1 )
				{
					m_bActiveState = _MAGIC_FINISH;
					return FALSE;
				}
			}
		}
	}


	if ( !m_bShowLight )	return TRUE;	

	if ( m_bActiveState == _MAGIC_ACTIVE || m_bActiveState == _LIGHT_ACTIVE )
	{
		if ( !m_bFixed )
			xLightFog->SetLightRadiusWithRing(m_shScrnX+24,
							  				  m_shScrnY+16, 
											  m_bLightRadius[0] + m_bCurrSwing, m_bLightColor[0][0], m_bLightColor[0][1], m_bLightColor[0][2],
											  m_bLightRadius[1] + m_bCurrSwing, m_bLightColor[1][0], m_bLightColor[1][1], m_bLightColor[1][2]);
		else
		{
			SHORT shPosX, shPosY;
			m_pxMap->GetScrnPosFromTilePos(m_shTileX, m_shTileY, shPosX, shPosY);

			xLightFog->SetLightRadiusWithRing(shPosX+24,
							  				  shPosY+16, 
											  m_bLightRadius[0] + m_bCurrSwing, m_bLightColor[0][0], m_bLightColor[0][1], m_bLightColor[0][2],
											  m_bLightRadius[1] + m_bCurrSwing, m_bLightColor[1][0], m_bLightColor[1][1], m_bLightColor[1][2]);
		}
	}

	return TRUE;
}


BOOL CMagic::DrawMagic()
{
// 임시.
POINT ptArr[16] = 
{
	{40, 34}, {42, 35}, {49, 33}, {59, 31}, {65, 31}, {62, 33}, {54, 36}, {41, 44}, {47, 48}, {47, 45}, 
	{43, 37}, {41, 38}, {40, 36}, {42, 37}, {43, 35}, {46, 34}
};
POINT ptArr2[16] = 
{
	{34, 37}, {47, 38}, {62, 48}, {73, 41}, {76, 34}, {72, 43}, {58, 53}, {50, 48}, {34, 49}, {45, 49}, 
	{55, 49}, {56, 43}, {48, 35}, {46, 40}, {46, 47}, {47, 38}, 
};

	if ( g_xMainWnd.Get3DDevice() )
	{
		if( SUCCEEDED(g_xMainWnd.Get3DDevice()->BeginScene()) )
		{
			D3DVECTOR vTrans;
			D3DMATRIX matTrans;
			D3DMATRIX matScale;
			D3DMATRIX matRot;
			D3DMATRIX matWorld;
			D3DMATRIX matTempWorld;
			D3DMATRIX matWorldOriginal;

			g_xMainWnd.Get3DDevice()->GetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

			D3DMATERIAL7 mtrl;

			if ( m_bActiveState == _MAGIC_ACTIVE )
			{
				if ( m_pxMagicImg->NewSetIndex(m_dwCurrFrame) )
				{
					vTrans.x = (FLOAT) m_shScrnX+(FLOAT)m_pxMagicImg->m_lpstNewCurrWilImageInfo->shWidth /2+m_pxMagicImg->m_lpstNewCurrWilImageInfo->shPX-400; 
					vTrans.y = (FLOAT)-m_shScrnY-(FLOAT)m_pxMagicImg->m_lpstNewCurrWilImageInfo->shHeight/2-m_pxMagicImg->m_lpstNewCurrWilImageInfo->shPY+300; 
					vTrans.z = 0;

// 임시.
	if ( !m_bFixed )
	{
					if ( m_bMagicNum == _SKILL_FIREBALL2 )
					{
		g_xGameProc.m_xFlyingTail.SetFlyTailParticle(D3DVECTOR(m_shScrnX+m_pxMagicImg->m_lpstNewCurrWilImageInfo->shPX+ptArr[m_bDir16].x, m_shScrnY+m_pxMagicImg->m_lpstNewCurrWilImageInfo->shPY+ptArr[m_bDir16].y, 0));
		g_xGameProc.m_xSmoke.SetSmokeParticle(D3DVECTOR(m_shScrnX+m_pxMagicImg->m_lpstNewCurrWilImageInfo->shPX+ptArr[m_bDir16].x, m_shScrnY+m_pxMagicImg->m_lpstNewCurrWilImageInfo->shPY+ptArr[m_bDir16].y, 0));
					}
					else if ( m_bMagicNum == _SKILL_FIREBALL )
					{
		g_xGameProc.m_xFlyingTail.SetFlyTailParticle(D3DVECTOR(m_shScrnX+m_pxMagicImg->m_lpstNewCurrWilImageInfo->shPX+ptArr2[m_bDir16].x, m_shScrnY+m_pxMagicImg->m_lpstNewCurrWilImageInfo->shPY+ptArr2[m_bDir16].y, 0));
		g_xGameProc.m_xSmoke.SetSmokeParticle(D3DVECTOR(m_shScrnX+m_pxMagicImg->m_lpstNewCurrWilImageInfo->shPX+ptArr2[m_bDir16].x, m_shScrnY+m_pxMagicImg->m_lpstNewCurrWilImageInfo->shPY+ptArr2[m_bDir16].y, 0));
					}
	}

					D3DUtil_SetTranslateMatrix(matTrans, vTrans);
					D3DUtil_SetScaleMatrix(matScale, (FLOAT)m_pxMagicImg->m_lpstNewCurrWilImageInfo->shWidth, (FLOAT)m_pxMagicImg->m_lpstNewCurrWilImageInfo->shHeight, 0.0f);
					D3DMath_MatrixMultiply(matWorld, matScale, matTrans);
	//				D3DMath_MatrixMultiply(matTempWorld, matScale, matTrans);

	//				D3DUtil_SetRotationMatrix(matRot, D3DVECTOR(1.0f,1.0f,0.0f), g_PI/m_dwCurrFrame);
	//				D3DMath_MatrixMultiply(matWorld, matRot, matTempWorld);
					g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);

					if ( m_lpddsMagicTextr[m_dwCurrFrame-m_dwFstFrame] )
						g_xMainWnd.Get3DDevice()->SetTexture(0, m_lpddsMagicTextr[m_dwCurrFrame-m_dwFstFrame]);
					D3DUtil_InitMaterial(mtrl, 
										 (FLOAT)m_bMagicColor[0]/255.0f,
										 (FLOAT)m_bMagicColor[1]/255.0f,
										 (FLOAT)m_bMagicColor[2]/255.0f);
	/*									 (FLOAT)255/255.0f,
										 (FLOAT)255/255.0f,
										 (FLOAT)255/255.0f);
	*/				mtrl.diffuse.a = (m_bOpacity+1)/255.0f;
					g_xMainWnd.Get3DDevice()->SetMaterial(&mtrl);

					SetBlendRenderState(g_xMainWnd.Get3DDevice(), m_bBlendType, mtrl);

					g_xMainWnd.Get3DDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX, m_avMagic, 4, NULL);

					// 원상복귀.
					ZeroMemory(&mtrl, sizeof(mtrl));
					mtrl.diffuse.r = mtrl.diffuse.g = mtrl.diffuse.b = 0.1f;
					mtrl.ambient.r = mtrl.ambient.g = mtrl.ambient.b = 1.0f;
					g_xMainWnd.Get3DDevice()->SetMaterial(&mtrl);

					ResetBlendenderState(g_xMainWnd.Get3DDevice());
					g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);
				}
			}
			g_xMainWnd.Get3DDevice()->EndScene();
			return S_OK;
		}
	}
	return E_FAIL;
}
	


VOID CMagic::DestoryMagic()
{
}


BOOL CMagic::UpdateMagic(INT nLoopTime)
{
	m_wCurrDelay += nLoopTime;
	m_wMagicLife += nLoopTime;

	if ( !DrawLight(&g_xGameProc.m_xLightFog, nLoopTime) )
		return FALSE;

	if ( m_wCurrDelay > m_wFrmDelay )
	{
		m_dwCurrFrame++;
		m_wCurrDelay = 0;


		if ( m_bMagicNum == _SKILL_SINSU && m_dwCurrFrame >= 2362 && m_dwCurrFrame <=2373 )
		{
			m_wFrmDelay = 30;
		}
		else if ( m_bMagicNum == _SKILL_SINSU )
		{
			m_wFrmDelay = 110;
		}

		if ( m_nDuplicateNum < MAX_DUPLE_COUNT )
		{
			INT nWaveNum = -1;
			const INT nMagicSndTbl[31] = 
			{
				 0,  6,  8, 14, 15,  9,  0, 20, 21, 32,
				 1,  2, 16, 29, 24, 10, 17, 33, 19, 31,
				28, 22, 13, 21,  5,  0, 12, 11,  0, 18, 
				30,
			};
			if ( m_bMagicNum < 31 )
			{
				if ( m_bFixed )
					nWaveNum = nMagicSndTbl[m_bMagicNum]*10 + 10000 + 2;
				else
					nWaveNum = nMagicSndTbl[m_bMagicNum]*10 + 10000 + 1; 
			}
/*			if ( nWaveNum > 10010 )
				g_xSound.ChgPlayingSet(nWaveNum, m_nDuplicateNum, m_shTileX, m_shTileY, g_xGameProc.m_xMyHero.m_wPosX, g_xGameProc.m_xMyHero.m_wPosY);
*/		}

		if ( m_dwCurrFrame >= m_dwEndFrame )
		{
			if ( m_bRepeat )
			{
				m_dwCurrFrame = m_dwFstFrame;
			}
			else
			{
				m_dwCurrFrame  = m_dwEndFrame-1;
				m_bActiveState = _LIGHT_ACTIVE;
				return FALSE;
			}
		}
	}

	BOOL	bCrash = FALSE;
	SHORT	shScrnGapX, shScrnGapY;

	if ( !m_bFixed )
	{
		if ( m_pxTarget )
		{
			// 타겟의 좌표를 재설정한다.
			m_shTargetTileX = m_pxTarget->m_wPosX;
			m_shTargetTileY = m_pxTarget->m_wPosY;
			m_shTargetScrnX = m_pxTarget->m_shScrnPosX;
			m_shTargetScrnY = m_pxTarget->m_shScrnPosY;		
			
			m_shTileX = m_shFireTileX;
			m_shTileY = m_shFireTileY;
//			m_shScrnX = m_fFloatScrnX = m_shFireScrnX;
//			m_shScrnY = m_fFloatScrnY = m_shFireScrnY;

			// 거리재설정.
		   if ( m_shScrnX != m_shTargetScrnX )		shScrnGapX = abs(m_shTargetScrnX-m_shScrnX);
		   else										shScrnGapX = 1;

		   if ( m_shScrnY != m_shTargetScrnY )		shScrnGapY = abs(m_shTargetScrnY-m_shScrnY);
		   else										shScrnGapY = 1;

		   if ( shScrnGapX > shScrnGapY )
		   {
			   m_nDistanceX = (m_shTargetScrnX-m_shScrnX) * (500/shScrnGapX);
			   m_nDistanceY = (m_shTargetScrnY-m_shScrnY) * (500/shScrnGapX);
		   }
		   else
		   {
			   m_nDistanceX = (m_shTargetScrnX-m_shScrnX) * (500/shScrnGapY);
			   m_nDistanceY = (m_shTargetScrnY-m_shScrnY) * (500/shScrnGapY);
		   }

		   m_fFloatScrnX += (FLOAT)(((FLOAT)m_nDistanceX/700) * nLoopTime);
		   m_fFloatScrnY += (FLOAT)(((FLOAT)m_nDistanceY/700) * nLoopTime);

		   m_shScrnX = m_fFloatScrnX;
		   m_shScrnY = m_fFloatScrnY;


		   // 현재 스크린좌표를 타일좌표로 변환하여 마법통과여부를 결정한다.
//		   if ( 마법이 통과할수 없으면 )
		   INT nabsX, nabsY;
		   nabsX = abs(m_shTargetScrnX-m_shScrnX);
		   nabsY = abs(m_shTargetScrnY-m_shScrnY);
 		   if ( (nabsX <= 48 && nabsY <= 32) || (nabsX >= m_nPrevDistanceX && nabsY >= m_nPrevDistanceY) )
		   {
			   bCrash = TRUE;
		   }
		   else
		   {
				m_nPrevDistanceX = nabsX;
				m_nPrevDistanceY = nabsY;
		   }
		}
		// Target Actor가 없다면.
		else
		{
			FLOAT fDisX = (FLOAT)((FLOAT)m_nDistanceX/900);
			FLOAT fDisY = (FLOAT)((FLOAT)m_nDistanceY/900);
			
			m_fFloatScrnX += ((fDisX) * (FLOAT)nLoopTime);
			m_fFloatScrnY += ((fDisY) * (FLOAT)nLoopTime);

			m_shScrnX = (SHORT)m_fFloatScrnX;
			m_shScrnY = (SHORT)m_fFloatScrnY;

			m_pxMap->GetTilePosFromScrnPos(m_shScrnX, m_shScrnY, m_shTileX, m_shTileY);


		   if ( m_bMagicNum	== _SKILL_FIRECHARM || m_bMagicNum == _SKILL_FIREBALL2 || m_bMagicNum == _SKILL_FIREBALL || m_bMagicNum == _SKILL_SHOOTLIGHTEN )
		   {
			   if ( m_wMagicLife >= 3000 )
			   {
				   bCrash = TRUE;
				   m_bActiveState = _MAGIC_FINISH;
			   }
		   }
		   else
		   {
			   if ( (abs(m_shTargetScrnX-m_shScrnX) <= 32 && abs(m_shTargetScrnY-m_shScrnY) <= 20) )
			   {
				   bCrash = TRUE;
			   }
		   }
		}
	}
	// 움직이지 않는 효과라면.
	else
	{
		if ( m_pxTarget )
		{
			m_shTileX = m_shTargetTileX = m_pxTarget->m_wPosX;
			m_shTileY = m_shTargetTileY = m_pxTarget->m_wPosY;
			m_shScrnX = m_shTargetScrnX = m_pxTarget->m_shScrnPosX;
			m_shScrnY = m_shTargetScrnY = m_pxTarget->m_shScrnPosY;		
		}
		else
		{
			m_shTileX = m_shTargetTileX;
			m_shTileY = m_shTargetTileY;
			m_pxMap->GetScrnPosFromTilePos(m_shTargetTileX, m_shTargetTileY, m_shTargetScrnX, m_shTargetScrnY);
			m_shScrnX = m_shTargetScrnX;
			m_shScrnY = m_shTargetScrnY;
		}
	}

	if ( bCrash )
	{
		m_bFixed = TRUE;
		m_bRepeat= FALSE;
		bCrash = TRUE;
		// 폭발프레임으로전환한다.

		LPEFFECTSPRINFO pstMagicSpr = g_xSpriteInfo.GetExplosionInfo(m_bMagicNum);

		if ( !pstMagicSpr )
		{
			// 폭발프레임이 없다면.
			m_bActiveState	= _MAGIC_FINISH;
			return FALSE;
		}

		//------------------------------------------------------------------------------------------------------------
		// 프레임정보얻기.
		//------------------------------------------------------------------------------------------------------------
		m_dwFstFrame		= pstMagicSpr->dwFstFrm;
		m_dwEndFrame		= pstMagicSpr->dwEndFrm;
		m_dwCurrFrame		= m_dwFstFrame;
		m_wFrmDelay			= pstMagicSpr->wDelay;
		m_bLightRadius[0]	= pstMagicSpr->bLightRadius[0];
		m_bLightRadius[1]	= pstMagicSpr->bLightRadius[1];
		m_bLightColor[0][0]	= pstMagicSpr->bLightColor[0][0];
		m_bLightColor[0][1]	= pstMagicSpr->bLightColor[0][1];
		m_bLightColor[0][2]	= pstMagicSpr->bLightColor[0][2];
		m_bLightColor[1][0]	= pstMagicSpr->bLightColor[1][0];
		m_bLightColor[1][1]	= pstMagicSpr->bLightColor[1][1];
		m_bLightColor[1][2]	= pstMagicSpr->bLightColor[1][2];
		m_bBlendType		= pstMagicSpr->bBlendType;
		m_bOpacity			= pstMagicSpr->bOpa;
		m_bSwingCnt			= pstMagicSpr->bSwingCnt;
		//------------------------------------------------------------------------------------------------------------

		//------------------------------------------------------------------------------------------------------------
		// 이미지로딩.
		//------------------------------------------------------------------------------------------------------------
		WORD wFileType;
		WORD wImgIdx = pstMagicSpr->wImgIdx;
		m_pxMagicImg = &g_xGameProc.m_xImage.m_xImageList[wImgIdx];
		 
		for ( INT nCnt = pstMagicSpr->dwFstFrm; nCnt < pstMagicSpr->dwEndFrm; nCnt++ )
		{
			if ( wImgIdx == _IMAGE_MAGIC )
			{
				wFileType = _TEXTR_FILE_MAGIC;
			}
			else if ( wImgIdx == _IMAGE_MONMAGIC )
			{
				wFileType = _TEXTR_FILE_MONMAGIC;
			}
			g_xGameProc.m_xImage.AddTextr(wFileType, wImgIdx, nCnt);
			m_lpddsMagicTextr[nCnt-m_dwFstFrame] = g_xGameProc.m_xImage.GetTextrImg(wFileType, wImgIdx, nCnt);
		}

		D3DVECTOR vNorm(0, 0, -1);
		m_avMagic[0]  = D3DVERTEX(D3DVECTOR(-0.5f, 0.5f, 0), vNorm, 0, 0);
		m_avMagic[1]  = D3DVERTEX(D3DVECTOR(-0.5f,-0.5f, 0), vNorm, 0, 1);
		m_avMagic[2]  = D3DVERTEX(D3DVECTOR( 0.5f, 0.5f, 0), vNorm, 1, 0);
		m_avMagic[3]  = D3DVERTEX(D3DVECTOR( 0.5f,-0.5f, 0), vNorm, 1, 1);
		//------------------------------------------------------------------------------------------------------------

		const INT nMagicSndTbl[31] = 
		{
			 0,  6,  8, 14, 15,  9,  0, 20, 21, 32,
			 1,  2, 16, 29, 24, 10, 17, 33, 19, 31,
			28, 22, 13, 21,  5,  0, 12, 11,  0, 18, 
			30,
		};
		INT nWaveNum = -1;
		static DWORD dwLastTick = 0;
		if ( timeGetTime() - dwLastTick > 100 )
		{
			if ( m_bMagicNum < 31 )
			{
				if ( m_bFixed )
					nWaveNum = nMagicSndTbl[m_bMagicNum]*10 + 10000 + 2;
				else
					nWaveNum = nMagicSndTbl[m_bMagicNum]*10 + 10000 + 1;
			}
/*			if ( nWaveNum > 10000 )
				g_xSound.PlayActorWav(m_shTileX, m_shTileY, g_xGameProc.m_xMyHero.m_wPosX, g_xGameProc.m_xMyHero.m_wPosY, nWaveNum);
*/		}
		dwLastTick = timeGetTime();
	}

	return TRUE;
}






BOOL CElecMagic::CreateMagic(BYTE bMagicNum, SHORT shFireTileX, SHORT shFireTileY, SHORT shTargetTileX, SHORT shTargetTileY, CActor* pxOwner, INT nTarget)
{
	if ( CMagic::CreateMagic(bMagicNum, shFireTileX, shFireTileY, shTargetTileX, shTargetTileY, pxOwner, nTarget) )
	{
		m_xElec.InitElec();
		m_xElec.SetupElec();

		m_bArrived = FALSE;

		m_shEPosX = m_shFireScrnX;
		m_shEPosY = m_shFireScrnY;

		SHORT	shScrnGapX, shScrnGapY;
		INT		nDisX, nDisY;

	    if ( m_shFireScrnX != m_shTargetScrnX )		shScrnGapX = abs(m_shTargetScrnX-m_shFireScrnX);
	    else										shScrnGapX = 1;

	    if ( m_shFireScrnY != m_shTargetScrnY )		shScrnGapY = abs(m_shTargetScrnY-m_shFireScrnY);
	    else										shScrnGapY = 1;

	    FLOAT fGapRateX = (FLOAT)(25/(FLOAT)shScrnGapX);
	    FLOAT fGapRateY = (FLOAT)(25/(FLOAT)shScrnGapY);

	    if ( shScrnGapX > shScrnGapY )
		{
			nDisX = (m_shTargetScrnX-m_shFireScrnX) * (fGapRateX);
			nDisY = (m_shTargetScrnY-m_shFireScrnY) * (fGapRateX);
		}
	    else
		{
			nDisX = (m_shTargetScrnX-m_shFireScrnX) * (fGapRateY);
			nDisY = (m_shTargetScrnY-m_shFireScrnY) * (fGapRateY);
		}

		m_shTargetScrnX += nDisX;
		m_shTargetScrnY += nDisY;

		return TRUE;
	}

	return FALSE;	
}

BOOL CElecMagic::UpdateMagic(INT nLoopTime)
{
	m_wMagicLife += nLoopTime;

	if ( !DrawLight(&g_xGameProc.m_xLightFog, nLoopTime) )
		return FALSE;

	FLOAT fDisX = (FLOAT)((FLOAT)m_nDistanceX/300);
	FLOAT fDisY = (FLOAT)((FLOAT)m_nDistanceY/300);
	
	m_fFloatScrnX += ((fDisX) * (FLOAT)nLoopTime);
	m_fFloatScrnY += ((fDisY) * (FLOAT)nLoopTime);

	if ( !m_bArrived )
	{
		m_shScrnX = (SHORT)m_fFloatScrnX;
		m_shScrnY = (SHORT)m_fFloatScrnY;

		m_pxMap->GetTilePosFromScrnPos(m_shScrnX, m_shScrnY, m_shTileX, m_shTileY);

		if ( (abs(m_shTargetScrnX-m_shScrnX) <= 32 && abs(m_shTargetScrnY-m_shScrnY) <= 20) )
		{
			m_fFloatScrnX = m_shFireScrnX;
			m_fFloatScrnY = m_shFireScrnY;
			m_bArrived = TRUE;
		}
	}
	else 
	{
		m_shEPosX = (SHORT)m_fFloatScrnX;
		m_shEPosY = (SHORT)m_fFloatScrnY;

		m_pxMap->GetTilePosFromScrnPos(m_shScrnX, m_shScrnY, m_shTileX, m_shTileY);

		if ( (abs(m_shTargetScrnX-m_shEPosX) <= 32 && abs(m_shTargetScrnY-m_shEPosY) <= 20) )
		{
			m_bActiveState = _MAGIC_FINISH;
		}
	}

	if ( m_wMagicLife > 2000 )
		m_bActiveState = _MAGIC_FINISH;

	return TRUE;
}

BOOL CElecMagic::DrawMagic()
{
	INT nCnt = 0;
	while ( nCnt < 1 )
	{
//		m_xElec.UpdateElec(m_shEPosX + 24, m_shEPosY - 32, m_shScrnX + 24, m_shScrnY - 32);
//		m_xElec.UpdateElec(m_shFireScrnX + 24, m_shFireScrnY - 32, m_shScrnX + 24, m_shScrnY - 32);
		m_xElec.UpdateElec(m_shFireScrnX + 24, m_shFireScrnY - 32, m_shTargetScrnX + 24, m_shTargetScrnY - 32);
		m_xElec.RenderElec(g_xMainWnd.Get3DDevice());

		nCnt++;
	}
	return TRUE;
}








BOOL CMagicStream::CreateMagic(BYTE bMagicNum, SHORT shFireTileX, SHORT shFireTileY, 
		                      SHORT shTargetTileX, SHORT shTargetTileY, CActor* pxOwner, INT nTarget, WORD wStartMagicTime, POINT ptTileGap)
{
	CMagic::CreateMagic(bMagicNum, shFireTileX, shFireTileY, shTargetTileX, shTargetTileY, pxOwner, nTarget);
	m_wMagicStart = wStartMagicTime;
	m_ptTileGap	  = ptTileGap;

	m_bDir16 = (BYTE)m_pxMap->CalcDirection8(m_shFireTileX, m_shFireTileY, shTargetTileX, shTargetTileY);

	return TRUE;
}


BOOL CMagicStream::UpdateMagic(INT nLoopTime)
{
	m_wCurrDelay += nLoopTime;
	m_wMagicLife += nLoopTime;

	if ( m_wMagicLife > m_wMagicStart )
	{
		if ( m_wCurrDelay > m_wFrmDelay )
		{
			m_dwCurrFrame++;
			m_wCurrDelay = 0;

			if ( m_nDuplicateNum < MAX_DUPLE_COUNT )
			{
				INT nWaveNum = -1;
				const INT nMagicSndTbl[31] = 
				{
					 0,  6,  8, 14, 15,  9,  0, 20, 21, 32,
					 1,  2, 16, 29, 24, 10, 17, 33, 19, 31,
					28, 22, 13, 21,  5,  0, 12, 11,  0, 18, 
					30,
				};
				if ( m_bMagicNum < 31 )
				{
					if ( m_bFixed )
						nWaveNum = nMagicSndTbl[m_bMagicNum]*10 + 10000 + 2;
					else
						nWaveNum = nMagicSndTbl[m_bMagicNum]*10 + 10000 + 1;
				}
/*				if ( nWaveNum > 10010 )
					g_xSound.ChgPlayingSet(nWaveNum, m_nDuplicateNum, m_shTileX, m_shTileY, g_xGameProc.m_xMyHero.m_wPosX, g_xGameProc.m_xMyHero.m_wPosY);
*/			}

			if ( m_bMagicNum == _SKILL_FIRE )
			{
				if ( m_dwCurrFrame >= 1925 )				m_wFrmDelay = 80;
				if ( m_dwCurrFrame <  1915 )				m_dwCurrFrame++;

				if ( m_dwCurrFrame >= m_dwEndFrame-30 )
				{
					m_dwCurrFrame  = m_dwEndFrame-29;
					m_bActiveState = _LIGHT_ACTIVE;
				}
			}
			else
			{
				if ( m_dwCurrFrame >= m_dwEndFrame )
				{
					m_dwCurrFrame  = m_dwEndFrame-1;
					m_bActiveState = _LIGHT_ACTIVE;
				}
			}
		}
	}

	if ( !DrawLight(&g_xGameProc.m_xLightFog, nLoopTime) )
		return FALSE;

	return TRUE;
}


BOOL CMagicStream::DrawMagic()
{
	if ( m_wMagicLife > m_wMagicStart )
	{
		m_shTileX = m_shTargetTileX;
		m_shTileY = m_shTargetTileY;
		m_pxMap->GetScrnPosFromTilePos(m_shTargetTileX, m_shTargetTileY, m_shTargetScrnX, m_shTargetScrnY);
		m_shScrnX = m_shTargetScrnX;
		m_shScrnY = m_shTargetScrnY;

		if ( m_bMagicNum == _SKILL_FIRE )
		{
			BYTE bCanMove  = m_pxMap->GetTileAttribute(m_shTileX, m_shTileY);
			if ( bCanMove == _CAN_WALK )
			{
				CMagic::DrawMagic();

				if ( m_bDir16 != 0 && m_bDir16 != 4 )
				{
					m_dwCurrFrame += 30;
//					m_dwFstFrame += 30;
					m_shScrnX = m_shScrnX+(m_ptTileGap.x*_CELL_WIDTH/2);
					m_shScrnY = m_shScrnY+(m_ptTileGap.y*_CELL_HEIGHT/2);
					CMagic::DrawMagic();
					m_dwCurrFrame -= 30;
//					m_dwFstFrame -= 30;
					return TRUE;
				}
			}
		}
		else
		{
			CMagic::DrawMagic();
			m_shScrnX = m_shScrnX+(m_ptTileGap.x*_CELL_WIDTH/2);
			m_shScrnY = m_shScrnY+(m_ptTileGap.y*_CELL_HEIGHT/2);
			CMagic::DrawMagic();
		}
	}
	return FALSE;
}












BOOL CRepeatMagic::CreateMagic(INT nEventID, BYTE bMagicNum, SHORT shFireTileX, SHORT shFireTileY, 
		                      SHORT shTargetTileX, SHORT shTargetTileY, DWORD dwMagicLifeTotal, CActor* pxOwner, INT nTarget)
{
	CMagic::CreateMagic(bMagicNum, shFireTileX, shFireTileY, shTargetTileX, shTargetTileY, pxOwner, nTarget);

	m_nEventID = nEventID;

	m_shTileX = m_shTargetTileX;
	m_shTileY = m_shTargetTileY;
	m_pxMap->GetScrnPosFromTilePos(m_shTargetTileX, m_shTargetTileY, m_shTargetScrnX, m_shTargetScrnY);
	m_shScrnX = m_shTargetScrnX;
	m_shScrnY = m_shTargetScrnY;

	m_dwMagiLifeTotal = dwMagicLifeTotal;

	return TRUE;
}



BOOL CRepeatMagic::UpdateMagic(INT nLoopTime)
{
	m_wCurrDelay += nLoopTime;
	m_wMagicLife += nLoopTime;

	if ( m_wCurrDelay > m_wFrmDelay )
	{
		m_wCurrDelay = 0;
		m_dwCurrFrame++;

		if ( m_nDuplicateNum < MAX_DUPLE_COUNT )
		{
			INT nWaveNum = -1;
			const INT nMagicSndTbl[31] = 
			{
				 0,  6,  8, 14, 15,  9,  0, 20, 21, 32,
				 1,  2, 16, 29, 24, 10, 17, 33, 19, 31,
 				28, 22, 13, 21,  5,  0, 12, 11,  0, 18, 
				30,
			};
			if ( m_bMagicNum < 31 )
			{
				if ( m_bFixed )
					nWaveNum = nMagicSndTbl[m_bMagicNum]*10 + 10000 + 2;
				else
					nWaveNum = nMagicSndTbl[m_bMagicNum]*10 + 10000 + 1;
			}
/*			if ( nWaveNum > 10010 )
				g_xSound.ChgPlayingSet(nWaveNum, m_nDuplicateNum, m_shTileX, m_shTileY, g_xGameProc.m_xMyHero.m_wPosX, g_xGameProc.m_xMyHero.m_wPosY);
*/		}

		if ( m_dwCurrFrame >= m_dwEndFrame )		m_dwCurrFrame = m_dwFstFrame;

		if ( m_wMagicLife > m_dwMagiLifeTotal )
		{
			m_bActiveState = _LIGHT_ACTIVE;
		}
	}

	if ( !DrawLight(&g_xGameProc.m_xLightFog, nLoopTime) )
	{
		INT nWaveNum = -1;
		const INT nMagicSndTbl[31] = 
		{
			 0,  6,  8, 14, 15,  9,  0, 20, 21, 32,
			 1,  2, 16, 29, 24, 10, 17, 33, 19, 31,
			28, 22, 13, 21,  5,  0, 12, 11,  0, 18, 
			30,
		};
		nWaveNum = nMagicSndTbl[m_bMagicNum]*10 + 10000 + 2;
/*		if ( m_nDuplicateNum < MAX_DUPLE_COUNT )
			g_xSound.StopSound(nWaveNum, m_nDuplicateNum);
		else
			g_xSound.StopSound(nWaveNum);
*/
		return FALSE;
	}

	return TRUE;
}



BOOL CRepeatMagic::DrawMagic()
{
	m_shTileX = m_shTargetTileX;
	m_shTileY = m_shTargetTileY;
	m_pxMap->GetScrnPosFromTilePos(m_shTargetTileX, m_shTargetTileY, m_shTargetScrnX, m_shTargetScrnY);
	m_shScrnX = m_shTargetScrnX;
	m_shScrnY = m_shTargetScrnY;

/*	if ( CMagic::DrawMagic() )
		return TRUE;
*/
	if ( g_xMainWnd.Get3DDevice() )
	{
		if( SUCCEEDED(g_xMainWnd.Get3DDevice()->BeginScene()) )
		{
			D3DVECTOR vTrans;
			D3DMATRIX matTrans;
			D3DMATRIX matScale;
			D3DMATRIX matRot;
			D3DMATRIX matWorld;
			D3DMATRIX matTempWorld;
			D3DMATRIX matWorldOriginal;

			g_xMainWnd.Get3DDevice()->GetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

			D3DMATERIAL7 mtrl;

			if ( m_bActiveState == _MAGIC_ACTIVE || m_bActiveState == _LIGHT_ACTIVE )
			{
				if ( m_pxMagicImg->NewSetIndex(m_dwCurrFrame) )
				{
					g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);
					vTrans.x = (FLOAT) m_shScrnX+(FLOAT)m_pxMagicImg->m_lpstNewCurrWilImageInfo->shWidth /2+m_pxMagicImg->m_lpstNewCurrWilImageInfo->shPX-400; 
					vTrans.y = (FLOAT)-m_shScrnY-(FLOAT)m_pxMagicImg->m_lpstNewCurrWilImageInfo->shHeight/2-m_pxMagicImg->m_lpstNewCurrWilImageInfo->shPY+300; 
					vTrans.z = 0;
					D3DUtil_SetTranslateMatrix(matTrans, vTrans);
					D3DUtil_SetScaleMatrix(matScale, (FLOAT)m_pxMagicImg->m_lpstNewCurrWilImageInfo->shWidth, (FLOAT)m_pxMagicImg->m_lpstNewCurrWilImageInfo->shHeight, 0.0f);
					D3DMath_MatrixMultiply(matWorld, matScale, matTrans);
	//				D3DMath_MatrixMultiply(matTempWorld, matScale, matTrans);

	//				D3DUtil_SetRotationMatrix(matRot, D3DVECTOR(1.0f,1.0f,0.0f), g_PI/m_dwCurrFrame);
	//				D3DMath_MatrixMultiply(matWorld, matRot, matTempWorld);
					g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);

					if ( m_bActiveState == _LIGHT_ACTIVE )
					{
						m_bMagicColor[0] -= 5;
						m_bMagicColor[1] -= 5;
						m_bMagicColor[2] -= 5;

						if ( m_bMagicColor[0] < 10 )		m_bMagicColor[0] = 5;
						if ( m_bMagicColor[1] < 10 )		m_bMagicColor[1] = 5;
						if ( m_bMagicColor[2] < 10 )		m_bMagicColor[2] = 5;
					}

					if ( m_lpddsMagicTextr[m_dwCurrFrame-m_dwFstFrame] )
						g_xMainWnd.Get3DDevice()->SetTexture(0, m_lpddsMagicTextr[m_dwCurrFrame-m_dwFstFrame]);
					D3DUtil_InitMaterial(mtrl, 
										 (FLOAT)m_bMagicColor[0]/255.0f,
										 (FLOAT)m_bMagicColor[1]/255.0f,
										 (FLOAT)m_bMagicColor[2]/255.0f);
	/*									 (FLOAT)255/255.0f,
										 (FLOAT)255/255.0f,
										 (FLOAT)255/255.0f);
	*/				mtrl.diffuse.a = (m_bOpacity+1)/255.0f;
					g_xMainWnd.Get3DDevice()->SetMaterial(&mtrl);

					SetBlendRenderState(g_xMainWnd.Get3DDevice(), m_bBlendType, mtrl);

					g_xMainWnd.Get3DDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX, m_avMagic, 4, NULL);

					// 원상복귀.
					ZeroMemory(&mtrl, sizeof(mtrl));
					mtrl.diffuse.r = mtrl.diffuse.g = mtrl.diffuse.b = 0.1f;
					mtrl.ambient.r = mtrl.ambient.g = mtrl.ambient.b = 1.0f;
					g_xMainWnd.Get3DDevice()->SetMaterial(&mtrl);

					ResetBlendenderState(g_xMainWnd.Get3DDevice());
					g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);
				}
			}
			g_xMainWnd.Get3DDevice()->EndScene();
			return TRUE;
		}
	}

	return FALSE;
}