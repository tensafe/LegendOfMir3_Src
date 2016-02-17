/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/



#include "StdAfx.h"



/******************************************************************************************************************

	CActor Class

*******************************************************************************************************************/
/******************************************************************************************************************

	함수명 : CActor::CActor()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
CActor::CActor()
{
	InitActor();
}



/******************************************************************************************************************

	함수명 : CActor::~CActor()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
CActor::~CActor()
{
	DestroyActor();
}

VOID CActor::InitActor()
{
	m_wOldPosX			= 0;
	m_wOldPosY			= 0;
	m_bOldDir			= 0;

	m_bOpenHealth		= FALSE;
	m_bWarMode			= FALSE;
	m_bCurrMtn			= _MT_MON_STAND;
	m_bCurrDir			= _DIRECTION_LIST_1;
	m_bMoveDir			= _DIRECTION_LIST_1;
	m_bEffectFrameCnt	= _DEFAULT_SPELLFRAME;
	m_bHPPercent		= 100;

	m_bMoveNextFrmCnt	= 5;
	m_bIsMoved			= FALSE;

	m_dwWarModeTime		= 0;
	m_wMAXHP			= 0;
	m_wHP				= 0;
	m_wMP				= 0;
	m_wPosX				= 0;
	m_wPosY				= 0;
	m_bLightSize		= 2;
	m_shShiftPixelX		= 0;
	m_shShiftPixelY		= 0;
	m_shShiftTileX		= 0;
	m_shShiftTileY		= 0;
	m_bMoveSpeed		= 0;
	m_shScrnPosX		= 0;
	m_shScrnPosY		= 0;
	m_wCurrDelay		= 0;
	m_dwFstFrame		= 0;
	m_dwEndFrame		= 1;
	m_dwCurrFrame		= 0;	
	m_wDelay			= 0;		
	m_pxActorImage		= NULL;
	m_bMsgHurryCheck	= FALSE;
	m_bUseEffect		= FALSE;
	m_bUseSwordEffect	= FALSE;
	m_bReverse			= FALSE;
	m_bIsDead			= FALSE;
	m_bABlendRev		= FALSE;
	m_dwCurrEffectFrame = 0;
	m_dwFstEffectFrame	= 0;
	m_dwEndEffectFrame	= 0;
	m_bEffectFrame		= 0;
	m_bBackStepFrame	= 0;
	m_bBackStepFrameCnt = 0;
	m_nState			= 0;
	m_nDividedChatLine	= 0;
	m_dwIdentity		= 0;
	m_wCurrChatDelay	= 0;
	m_wABlendCurrDelay	= 0;
	m_wABlendCurrDelay	= 0;

	m_bFstSoundPlayed   = FALSE;

	m_bAppearState		= _DIG_NORMAL;

	m_wStateClr			= _STATE_NOTUSED;
	m_dwNameClr			= RGB(255, 255, 255);

	m_bActorImgIdx		= _IMAGE_M_HUMAN;
	m_bEffectImgIdx		= _IMAGE_MAGIC;

	ZeroMemory(m_szName, 64);
	ZeroMemory(&m_stFeature,   sizeof(FEATURE));
	ZeroMemory(&m_stHitter,    sizeof(FEATURE));
	ZeroMemory(m_bLightRadius, sizeof(BYTE)*2);
	ZeroMemory(m_bLightColor , sizeof(BYTE)*2*3);
	ZeroMemory(m_szChatMsg, MAX_PATH);
	ZeroMemory(m_szChatMsgArg, MAX_PATH*5);
	ZeroMemory(&m_rcActor, sizeof(RECT));
	ZeroMemory(&m_rcTargetRgn, sizeof(RECT));

	D3DVECTOR		vNorm(0, 0, -1);
	m_avBoard[0] = D3DVERTEX(D3DVECTOR(-0.5f, 0.5f, 0), vNorm, 0, 0);
	m_avBoard[1] = D3DVERTEX(D3DVECTOR(-0.5f,-0.5f, 0), vNorm, 0, 1);
	m_avBoard[2] = D3DVERTEX(D3DVECTOR( 0.5f, 0.5f, 0), vNorm, 1, 0);
	m_avBoard[3] = D3DVERTEX(D3DVECTOR( 0.5f,-0.5f, 0), vNorm, 1, 1);
}


VOID CActor::DestroyActor()
{
	INT			nCnt;
	SHORT		shLeftMsgCnt;
	LPPACKETMSG	lpPacketMsg;

	lpPacketMsg  = NULL;
	shLeftMsgCnt = m_xPacketQueue.GetCount();

	// 쌓여있는 패킷을 지운다.
	if ( shLeftMsgCnt > 0 )
	{
		for ( nCnt = 0; nCnt < shLeftMsgCnt; nCnt++ )
		{
			lpPacketMsg = (LPPACKETMSG)m_xPacketQueue.PopQ();
			if ( lpPacketMsg )
			{
				SAFE_DELETE(lpPacketMsg);
			}
		}
	}

	// 모든변수를 초기화 시켜둔다.
	InitActor();
}


WORD CActor::GetCharState()
{		
	WORD wCharState = 0XFFFF;

//	if( m_nState &   0X100000 )			wCharState = _STATE_SHIELDUSE;
	if( m_nState &   0X800000 )			wCharState = _STATE_ABLEND;
	if( m_nState &  0X4000000 )			wCharState = _STATE_GRAY;
	if( m_nState &  0X8000000 )			wCharState = _STATE_FUCHSIA;
	if( m_nState & 0X10000000 )			wCharState = _STATE_YELLOW;
	if( m_nState & 0X20000000 )			wCharState = _STATE_BLUE;
	if( m_nState & 0X40000000 )			wCharState = _STATE_RED;
	if( m_nState & 0X80000000 )			wCharState = _STATE_GREEN;

	m_wStateClr = _STATE_NOTUSED;

 	if ( m_wStateClr == _STATE_ABLEND || m_wStateClr == _STATE_SHIELDUSE )
		m_wStateClr = _STATE_NOTUSED;

	return wCharState;
}


VOID CActor::DrawWithEffected(INT nX, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, WORD wChooseColor1, WORD wChooseColor2, BOOL bFocused, BYTE bOpa, WORD wState)
{
	switch ( wState )
	{
	case _STATE_RED:
	case _STATE_GREEN:
	case _STATE_BLUE:		
	case _STATE_YELLOW:
	case _STATE_FUCHSIA:	
	case _STATE_GRAY:
		if ( m_wABlendDelay )
			g_xMainWnd.DrawWithImageForCompClipRgnColor(nX, nY, nXSize, nYSize, pwSrc, _CLIP_WIDTH, _CLIP_HEIGHT, wState, bFocused, TRUE);
		else
			g_xMainWnd.DrawWithImageForCompClipRgnColor(nX, nY, nXSize, nYSize, pwSrc, _CLIP_WIDTH, _CLIP_HEIGHT, wState, bFocused, FALSE);
		break;
	case _STATE_ABLEND:
		{
			if ( bFocused )		bOpa -= 20;

			if ( bOpa < 0 && bOpa > 100 )		bOpa = 0;
			g_xMainWnd.DrawWithABlendCompDataWithBackBuffer(nX, nY, nXSize, nYSize, pwSrc, _CLIP_WIDTH, _CLIP_HEIGHT, wChooseColor1, wChooseColor2, bOpa);
		}
		break;
	case _STATE_SHIELDUSE:
	case _STATE_NOTUSED:
	default:
		{
		if ( !m_wABlendDelay )
			g_xMainWnd.DrawWithImageForCompClipRgn(nX, nY, nXSize, nYSize, pwSrc, _CLIP_WIDTH, _CLIP_HEIGHT, wChooseColor1, wChooseColor2, bFocused);
		else
			g_xMainWnd.DrawWithABlendCompDataWithBackBuffer(nX, nY, nXSize, nYSize, pwSrc, _CLIP_WIDTH, _CLIP_HEIGHT, wChooseColor1, wChooseColor2, bOpa);
		}
		break;
	}
}


/******************************************************************************************************************

	함수명 : CActor::Create()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : CImageHandler* pxImgHandler
	         FEATURE stFeature
	         BYTE bMtn
	         WORD bDir
	         WORD wPosX
	         WORD wPosY
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CActor::Create(CImageHandler* pxImgHandler, FEATURE* pstFeature, BYTE bMtn, WORD bDir, WORD wPosX, WORD wPosY)
{
	// 전달인자 적용 및 확인./////////////////////////////////////////////////////////////////////////////////
	if ( /*(pstFeature->bGender >= 0 && pstFeature->bGender < _MAX_GENDER) && */(bDir >= 0 && bDir < _MAX_DIRECTION) )
	{
		switch ( pstFeature->bGender )
		{
		case _GENDER_MAN:
			{
				if ( (pstFeature->bDress < 0 && pstFeature->bDress >= _MAX_HERO_KIND) ||
					 (bMtn < 0 && bMtn >= _MAX_HERO_MTN) )	
					return FALSE;
				else	
				{
					m_bActorImgIdx = _IMAGE_M_HUMAN;
					m_dwFstFrame = g_xSpriteInfo.m_stHeroSpr[bMtn].wFstFrm + pstFeature->bDress*_MAX_HERO_FRAME + bDir*10;
					m_dwEndFrame = m_dwFstFrame + g_xSpriteInfo.m_stHeroSpr[bMtn].wFrmCnt;
					m_wDelay = g_xSpriteInfo.m_stHeroSpr[bMtn].wDelay;


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					pstFeature->bHair = 2;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


				}
			}
			break;
		case _GENDER_WOMAN:
			{
				if ( (pstFeature->bDress < 0 && pstFeature->bDress >= _MAX_HERO_KIND) ||
					 (bMtn < 0 && bMtn >= _MAX_HERO_MTN) )	
					return FALSE;
				else
				{
					m_bActorImgIdx = _IMAGE_WM_HUMAN;
					m_dwFstFrame = g_xSpriteInfo.m_stHeroSpr[bMtn].wFstFrm + pstFeature->bDress*_MAX_HERO_FRAME + bDir*10;
					m_dwEndFrame = m_dwFstFrame + g_xSpriteInfo.m_stHeroSpr[bMtn].wFrmCnt;
					m_wDelay = g_xSpriteInfo.m_stHeroSpr[bMtn].wDelay;


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					pstFeature->bHair = 2;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


				}
			}
			break;
		case _GENDER_MON:
			{	
				if ( (pstFeature->bDress < 0 && pstFeature->bDress >= _MAX_MON_KIND) ||
					 (bMtn < 0 && bMtn >= _MAX_MON_MTN) )	
					return FALSE;
				else
				{
					switch ( m_stFeature.bDress )
					{
					case 31:	// 식인초.
					case 67:	// 촉룡신.
					case 73:	// 비막원충.
					case 104:	// 적월마.
						bDir = 0;
					case 106:	// 폭안거미.
						bDir = 1;
						break;
					default:
						break;
					}

					g_xSpriteInfo.SetMonFrameInfo(pstFeature->bDress);
					m_bActorImgIdx = _IMAGE_MONSTER1 + (pstFeature->bDress / 10);
					m_dwFstFrame = g_xSpriteInfo.m_stMonSpr[bMtn].wFstFrm + (pstFeature->bDress%10)*_MAX_MON_FRAME + bDir*10;
					m_dwEndFrame = m_dwFstFrame + g_xSpriteInfo.m_stMonSpr[bMtn].wFrmCnt;
					m_wDelay = g_xSpriteInfo.m_stMonSpr[bMtn].wDelay;
				}
			}
			break;
		case _GENDER_NPC:
			{
				if ( (pstFeature->bDress < 0 && pstFeature->bDress >= _MAX_NPC_KIND) ||
					 (bMtn < 0 && bMtn >= _MAX_NPC_MTN) )	
					return FALSE;
				else
				{
					m_bActorImgIdx = _IMAGE_NPC;
					bDir++;		// bDir은 0이 될수도 있기때문이다.
					bDir = bDir/3;
					m_dwFstFrame = g_xSpriteInfo.m_stNPCSpr[bMtn].wFstFrm + pstFeature->bDress*_MAX_NPC_FRAME + bDir*10;
					m_dwEndFrame = m_dwFstFrame + g_xSpriteInfo.m_stNPCSpr[bMtn].wFrmCnt;
					m_wDelay = g_xSpriteInfo.m_stNPCSpr[bMtn].wDelay;
				}
			}
			break;
		default:
			return FALSE;
		}
		
		m_bCurrMtn		= bMtn;
		m_bCurrDir		= bDir;
		m_bMoveDir		= bDir;
		m_wPosX			= wPosX;
		m_wPosY			= wPosY;
		
		memcpy(&m_stFeature, pstFeature, sizeof(FEATURE));

		m_pxActorImage	= &(pxImgHandler->m_xImageList[m_bActorImgIdx]);
		m_dwCurrFrame	= m_dwFstFrame;
		m_bMoveDir		= m_bCurrDir;

		D3DVECTOR		vNorm(0, 0, -1);
		m_avBoard[0] = D3DVERTEX(D3DVECTOR(-0.5f, 0.5f, 0), vNorm, 0, 0);
		m_avBoard[1] = D3DVERTEX(D3DVECTOR(-0.5f,-0.5f, 0), vNorm, 0, 1);
		m_avBoard[2] = D3DVERTEX(D3DVECTOR( 0.5f, 0.5f, 0), vNorm, 1, 0);
		m_avBoard[3] = D3DVERTEX(D3DVECTOR( 0.5f,-0.5f, 0), vNorm, 1, 1);

		return TRUE;
	}
	
	return FALSE;
}



/******************************************************************************************************************

	함수명 : CActor::CheckFeatureValidate()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : FEATURE stFeature
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CActor::CheckFeatureValidate(FEATURE stFeature)
{
//	if ( (stFeature.bGender >= 0 && stFeature.bGender < _MAX_GENDER) )
	{
		switch ( stFeature.bGender )
		{
		case _GENDER_MAN:
		case _GENDER_WOMAN:
			if ( stFeature.bDress < 0 && stFeature.bDress >= _MAX_HERO_KIND )		return FALSE;
			break;
		case _GENDER_MON:
			{
				if ( stFeature.bDress < 0 && stFeature.bDress >= _MAX_MON_KIND )		return FALSE;
			}
			break;
		case _GENDER_NPC:
		default:
			return FALSE;
		}
	}
	return TRUE;
}



/******************************************************************************************************************

	함수명 : CActor::ChangeFeature()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : CImageHandler* pxImgHandler
	         FEATURE stFeature
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CActor::ChangeFeature(FEATURE stFeature)
{
	CImageHandler* pxImgHandler = &g_xGameProc.m_xImage;

	if ( /*stFeature.bGender >= 0 && stFeature.bGender < _MAX_GENDER && */CheckFeatureValidate(stFeature) )
	{
		switch ( stFeature.bGender )
		{
		case _GENDER_MAN:
			{
				if ( stFeature.bDress < 0 && stFeature.bDress >= _MAX_HERO_KIND )
					return FALSE;
				else	
				{
					m_bActorImgIdx = _IMAGE_M_HUMAN;
					m_dwFstFrame = g_xSpriteInfo.m_stHeroSpr[m_bCurrMtn].wFstFrm + stFeature.bDress*_MAX_HERO_FRAME + m_bCurrDir*10;
					m_dwEndFrame = m_dwFstFrame + g_xSpriteInfo.m_stHeroSpr[m_bCurrMtn].wFrmCnt;
					m_wDelay = g_xSpriteInfo.m_stHeroSpr[m_bCurrMtn].wDelay;



//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					stFeature.bHair = 2;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



				}
			}
			break;
		case _GENDER_WOMAN:
			{
				if ( stFeature.bDress < 0 && stFeature.bDress >= _MAX_HERO_KIND )
					return FALSE;
				else
				{
					m_bActorImgIdx = _IMAGE_WM_HUMAN;
					m_dwFstFrame = g_xSpriteInfo.m_stHeroSpr[m_bCurrMtn].wFstFrm + stFeature.bDress*_MAX_HERO_FRAME + m_bCurrDir*10;
					m_dwEndFrame = m_dwFstFrame + g_xSpriteInfo.m_stHeroSpr[m_bCurrMtn].wFrmCnt;
					m_wDelay = g_xSpriteInfo.m_stHeroSpr[m_bCurrMtn].wDelay;


//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
					stFeature.bHair = 2;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!



				}
			}
			break;
		case _GENDER_MON:
			{
				if ( stFeature.bDress < 0 && stFeature.bDress >= _MAX_MON_KIND )
					return FALSE;
				else
				{
					g_xSpriteInfo.SetMonFrameInfo(stFeature.bDress);
					m_bActorImgIdx = _IMAGE_MONSTER1 + (stFeature.bDress/10);
					m_dwFstFrame = g_xSpriteInfo.m_stMonSpr[m_bCurrMtn].wFstFrm + (stFeature.bDress%10)*_MAX_MON_FRAME + m_bCurrDir*10;
					m_dwEndFrame = m_dwFstFrame + g_xSpriteInfo.m_stMonSpr[m_bCurrMtn].wFrmCnt;
					m_wDelay = g_xSpriteInfo.m_stMonSpr[m_bCurrMtn].wDelay;
				}
			}
			break;
		// NPC는 프레임이 일정하지 않으므로 따로 적용시킨다.
		case _GENDER_NPC:
		default:
			return FALSE;
		}
		m_stFeature		= stFeature;
		m_pxActorImage	= &(pxImgHandler->m_xImageList[m_bActorImgIdx]);
		m_dwCurrFrame	= m_dwFstFrame;

		return TRUE;
	}
	
	return FALSE;
}


/******************************************************************************************************************

	함수명 : CActor::SetMotionFrame()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : BYTE bMtn
	         BYTE bDir
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CActor::SetMotionFrame(BYTE bMtn, BYTE bDir)
{
	if ( /*(bMtn < 0 && bMtn >= _MAX_HERO_MTN) || */(bDir < 0 && bDir >= _MAX_DIRECTION) )
		return FALSE;

	switch ( m_stFeature.bGender )
	{
	case _GENDER_MAN:
		{
			m_dwFstFrame = g_xSpriteInfo.m_stHeroSpr[bMtn].wFstFrm + m_stFeature.bDress*_MAX_HERO_FRAME + bDir*10;
			m_dwEndFrame = m_dwFstFrame + g_xSpriteInfo.m_stHeroSpr[bMtn].wFrmCnt;
			m_wDelay = g_xSpriteInfo.m_stHeroSpr[bMtn].wDelay;
		}
		break;
	case _GENDER_WOMAN:
		{
			m_dwFstFrame = g_xSpriteInfo.m_stHeroSpr[bMtn].wFstFrm + m_stFeature.bDress*_MAX_HERO_FRAME + bDir*10;
			m_dwEndFrame = m_dwFstFrame + g_xSpriteInfo.m_stHeroSpr[bMtn].wFrmCnt;
			m_wDelay = g_xSpriteInfo.m_stHeroSpr[bMtn].wDelay;
		}
		break;
	case _GENDER_MON:
		{
			if ( m_stFeature.bDress == 31 && bMtn == _MT_MON_STAND )
				bDir = 0;

		switch ( m_stFeature.bDress )
		{
		case 31:	// 식인초.
		case 67:	// 촉룡신.
		case 73:	// 비막원충.
		case 104:	// 적월마.
			bDir = 0;
		case 106:	// 폭안거미.
			bDir = 1;
			break;
		default:
			break;
		}

			g_xSpriteInfo.SetMonFrameInfo(m_stFeature.bDress);
			m_dwFstFrame = g_xSpriteInfo.m_stMonSpr[bMtn].wFstFrm + (m_stFeature.bDress%10)*_MAX_MON_FRAME + bDir*10;
			m_dwEndFrame = m_dwFstFrame + g_xSpriteInfo.m_stMonSpr[bMtn].wFrmCnt;
			m_wDelay = g_xSpriteInfo.m_stMonSpr[bMtn].wDelay;
		}
		break;
	case _GENDER_NPC:
		{
			bDir++;		// bDir은 0이 될수도 있기때문이다.
			bDir = bDir/3;
			m_dwFstFrame = g_xSpriteInfo.m_stNPCSpr[bMtn].wFstFrm + m_stFeature.bDress*_MAX_NPC_FRAME + bDir*10;
			m_dwEndFrame = m_dwFstFrame + g_xSpriteInfo.m_stNPCSpr[bMtn].wFrmCnt;
			m_wDelay = g_xSpriteInfo.m_stNPCSpr[bMtn].wDelay;
		}
		break;
	default:
		return FALSE;
	}

	m_bCurrMtn		= bMtn;
	m_bCurrDir		= bDir;
	m_bMoveDir		= bDir;
	m_dwCurrFrame	= m_dwFstFrame;
	m_wCurrDelay	= 0;

	if ( m_bCurrDir == _DIRECTION_LIST_8 )
		m_bMoveNextFrmCnt = 2;
	else
		m_bMoveNextFrmCnt = 5;


	return TRUE;
}


VOID CActor::PlayActSound()
{
	INT nWaveNum = -1;

	const INT nActorSndTbl[100] = 
	{
		 -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,	//  0 -  9
		 -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,	// 10 - 19
		160, 161, 100, 101, 102, 163,   0, 162,  83,  80,	// 20 - 29
		  1,  10,  20,  21,  22,  23, 150,  24,  25,  26,	// 30 - 39
		 27,  30,  32,  31, 151,  34,  28,  18,  40,  50,	// 40 - 49
		 51,  52,  53, 152,  36,  72,  37,  38,  43,  44,	// 50 - 59
		 45,  48,  49,  90,  91,  70,  73, 140,  74, 120,	// 60 - 69
		121,  81,  82,  41,  42,  39, 110, 111, 112, 130,	// 70 - 79
		164,  46,  47,  61,  62,  63,  71,  -1,  -1, 170,	// 80 - 89
		171,  33,  -1,  -1,  -1,  -1,  -1,  -1,  -1,  -1,	// 90 - 99
	};

	if ( m_dwCurrFrame == m_dwFstFrame+1 && m_bCurrMtn == _MT_MON_APPEAR )			// 나타나기.
	{
		nWaveNum = 200 + nActorSndTbl[m_stFeature.bDress]*10;
	}
	else if ( (m_dwCurrFrame == m_dwFstFrame+1) && (m_bCurrMtn == _MT_MON_STAND || _MT_MON_WALK) )		// 멈춰있기.
	{
		INT nRand = rand();

		if ( m_bFstSoundPlayed && !m_bIsDead )
		{
			if ( !(nRand%25) )
				nWaveNum = 200 + nActorSndTbl[m_stFeature.bDress]*10 + 1;
			else
				nWaveNum = -1;
		}
		else
		{
			if ( !(nRand%3) && !m_bIsDead )
			{
				nWaveNum = 200 + nActorSndTbl[m_stFeature.bDress]*10 + 1;
				m_bFstSoundPlayed = TRUE;
			}
			else
				nWaveNum = -1;
		}
	}
	else if ( m_dwCurrFrame == m_dwFstFrame+1 && m_bCurrMtn == _MT_MON_ATTACK_A )	// 일반공격1.
	{
		nWaveNum = 200 + nActorSndTbl[m_stFeature.bDress]*10 + 2;
	}
	else if ( m_dwCurrFrame == m_dwFstFrame+2 && m_bCurrMtn == _MT_MON_ATTACK_A )	// 일반공격1.
	{
		nWaveNum = 200 + nActorSndTbl[m_stFeature.bDress]*10 + 3;
	}
	else if ( m_dwCurrFrame == m_dwFstFrame+1 && m_bCurrMtn == _MT_MON_HITTED )
	{
		if ( m_stFeature.bGender < 2 )
		{
			switch ( m_stHitter.bWeapon )
			{
				case 21: case 24: // 단검, 비단검.
				case 8: case 9: // 목검, 아리수목검.
				case 18:  case 22:  case 23:  case 26:  case 27:  case 28:  case 30:  // 사모검. 청동검. 철검. 청음검. 벽사검. 천령. 곡성검.
				case 1:   case 4:  case 11:  case 13:  case 14:  case 20:  case 25:  case 29:  case 31:   // 유월도. 묵청대도. 육합도. 군도. 도룡보도. 사각도. 세첨도. 예도. 초혼도.
					nWaveNum = 70;
					break;
				case 5:  case 10:  case 12: // 삼적대부. 청동도끼. 연자부.
				case 15:  // 파뇌진당.
					nWaveNum = 71;
					break;
				case 2:  case 3:  case 6:  case 7: case 16:  case 17: case 19:  // 삼지창. 천형목. 홍아창. 곡괭이. 청마창, 용아장. 제마봉
					nWaveNum = 72;
					break;
				default:
					nWaveNum = 73;	// 맨손.
					break;
			}
		}
	}
	else if ( m_dwCurrFrame == m_dwFstFrame+2 && m_bCurrMtn == _MT_MON_HITTED )		// 맞기.
	{
		nWaveNum = 200 + nActorSndTbl[m_stFeature.bDress]*10 + 4;
	}
	else if ( m_dwCurrFrame == m_dwFstFrame+2 && m_bCurrMtn == _MT_MON_DIE )		// 죽기1.
	{
		nWaveNum = 200 + nActorSndTbl[m_stFeature.bDress]*10 + 5;
	}
	else if ( m_dwCurrFrame == m_dwFstFrame+3 && m_bCurrMtn == _MT_MON_DIE )		// 죽기2.
	{
		nWaveNum = 200 + nActorSndTbl[m_stFeature.bDress]*10 + 6;
	}

/*	if ( nWaveNum != -1 )
		g_xSound.PlayActorWav(m_wPosX, m_wPosY, g_xGameProc.m_xMyHero.m_wPosX, g_xGameProc.m_xMyHero.m_wPosY, nWaveNum);
*/
}



/******************************************************************************************************************

	함수명 : CActor::SetMoving()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CActor::SetMoving()
{	
	WORD wFrmCnt  = m_dwEndFrame  - m_dwFstFrame;
	WORD wCurrFrm = m_dwCurrFrame - m_dwFstFrame;

	switch ( m_bMoveDir )
	{
	case _DIRECTION_LIST_1:
		m_shShiftPixelX =  0;

		if ( wCurrFrm < (6-m_bMoveNextFrmCnt) )
			m_shShiftPixelY = -(_CELL_HEIGHT/wFrmCnt * (wCurrFrm+1))*m_bMoveSpeed;
		else
			m_shShiftPixelY =  (_CELL_HEIGHT/wFrmCnt * (wFrmCnt - wCurrFrm -1))*m_bMoveSpeed;
		break;
	case _DIRECTION_LIST_2:
		if ( wCurrFrm < (6-m_bMoveNextFrmCnt) )
			m_shShiftPixelX =  (_CELL_WIDTH /wFrmCnt * (wCurrFrm+1))*m_bMoveSpeed;
		else
			m_shShiftPixelX = -(_CELL_WIDTH /wFrmCnt * (wFrmCnt - wCurrFrm -1))*m_bMoveSpeed;

		if ( wCurrFrm < (6-m_bMoveNextFrmCnt) )
			m_shShiftPixelY = -(_CELL_HEIGHT/wFrmCnt * (wCurrFrm+1))*m_bMoveSpeed;
		else
			m_shShiftPixelY =  (_CELL_HEIGHT/wFrmCnt * (wFrmCnt - wCurrFrm -1))*m_bMoveSpeed;
		break;
	case _DIRECTION_LIST_3:
		if ( wCurrFrm < (6-m_bMoveNextFrmCnt) )
			m_shShiftPixelX =  (_CELL_WIDTH /wFrmCnt * (wCurrFrm+1))*m_bMoveSpeed;
		else
			m_shShiftPixelX = -(_CELL_WIDTH /wFrmCnt * (wFrmCnt - wCurrFrm -1))*m_bMoveSpeed;

		m_shShiftPixelY = 0;
		break;
	case _DIRECTION_LIST_4:
		if ( wCurrFrm < (6-m_bMoveNextFrmCnt) )
			m_shShiftPixelX =  (_CELL_WIDTH /wFrmCnt * (wCurrFrm+1))*m_bMoveSpeed;
		else
			m_shShiftPixelX = -(_CELL_WIDTH /wFrmCnt * (wFrmCnt - wCurrFrm -1))*m_bMoveSpeed;

		if ( wCurrFrm < (6-m_bMoveNextFrmCnt) )
			m_shShiftPixelY =  (_CELL_HEIGHT/wFrmCnt * (wCurrFrm+1))*m_bMoveSpeed;
		else
			m_shShiftPixelY = -(_CELL_HEIGHT/wFrmCnt * (wFrmCnt - wCurrFrm -1))*m_bMoveSpeed;
		break;
	case _DIRECTION_LIST_5:
		m_shShiftPixelX =  0;

		if ( wCurrFrm < (6-m_bMoveNextFrmCnt) )
			m_shShiftPixelY =  (_CELL_HEIGHT/wFrmCnt * (wCurrFrm+1))*m_bMoveSpeed;
		else
			m_shShiftPixelY = -(_CELL_HEIGHT/wFrmCnt * (wFrmCnt - wCurrFrm -1))*m_bMoveSpeed;
		break;
	case _DIRECTION_LIST_6:
		if ( wCurrFrm < (6-m_bMoveNextFrmCnt) )
			m_shShiftPixelX = -(_CELL_WIDTH /wFrmCnt * (wCurrFrm+1))*m_bMoveSpeed;
		else
			m_shShiftPixelX =  (_CELL_WIDTH /wFrmCnt * (wFrmCnt - wCurrFrm -1))*m_bMoveSpeed;

		if ( wCurrFrm < (6-m_bMoveNextFrmCnt) )
			m_shShiftPixelY =  (_CELL_HEIGHT/wFrmCnt * (wCurrFrm+1))*m_bMoveSpeed;
		else
			m_shShiftPixelY = -(_CELL_HEIGHT/wFrmCnt * (wFrmCnt - wCurrFrm -1))*m_bMoveSpeed;
		break;
	case _DIRECTION_LIST_7:
		if ( wCurrFrm < (6-m_bMoveNextFrmCnt) )
			m_shShiftPixelX = -(_CELL_WIDTH /wFrmCnt * (wCurrFrm+1))*m_bMoveSpeed;
		else
			m_shShiftPixelX =  (_CELL_WIDTH /wFrmCnt * (wFrmCnt - wCurrFrm -1))*m_bMoveSpeed;

		m_shShiftPixelY =  0;
		break;
	case _DIRECTION_LIST_8:
		if ( wCurrFrm < (6-m_bMoveNextFrmCnt) )
			m_shShiftPixelX = -(_CELL_WIDTH /wFrmCnt * (wCurrFrm+1))*m_bMoveSpeed;
		else
			m_shShiftPixelX =  (_CELL_WIDTH /wFrmCnt * (wFrmCnt - wCurrFrm -1))*m_bMoveSpeed;

		if ( wCurrFrm < (6-m_bMoveNextFrmCnt) )
			m_shShiftPixelY = -(_CELL_HEIGHT/wFrmCnt * (wCurrFrm+1))*m_bMoveSpeed;
		else
			m_shShiftPixelY =  (_CELL_HEIGHT/wFrmCnt * (wFrmCnt - wCurrFrm -1))*m_bMoveSpeed;
		break;	
	}
}



/******************************************************************************************************************

	함수명 : CActor::SetBackStepMoving()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CActor::SetBackStepMoving()
{	
	switch ( m_bMoveDir )
	{
	case _DIRECTION_LIST_1:
		m_shShiftPixelX =  0;

		if ( m_bBackStepFrame < (6-m_bMoveNextFrmCnt) )
			m_shShiftPixelY = -(_CELL_HEIGHT/m_bBackStepFrameCnt * (m_bBackStepFrame+1))*m_bMoveSpeed;
		else
			m_shShiftPixelY =  (_CELL_HEIGHT/m_bBackStepFrameCnt * (m_bBackStepFrameCnt - m_bBackStepFrame -1))*m_bMoveSpeed;
		break;
	case _DIRECTION_LIST_2:
		if ( m_bBackStepFrame < (6-m_bMoveNextFrmCnt) )
			m_shShiftPixelX =  (_CELL_WIDTH /m_bBackStepFrameCnt * (m_bBackStepFrame+1))*m_bMoveSpeed;
		else
			m_shShiftPixelX = -(_CELL_WIDTH /m_bBackStepFrameCnt * (m_bBackStepFrameCnt - m_bBackStepFrame -1))*m_bMoveSpeed;

		if ( m_bBackStepFrame < (6-m_bMoveNextFrmCnt) )
			m_shShiftPixelY = -(_CELL_HEIGHT/m_bBackStepFrameCnt * (m_bBackStepFrame+1))*m_bMoveSpeed;
		else
			m_shShiftPixelY =  (_CELL_HEIGHT/m_bBackStepFrameCnt * (m_bBackStepFrameCnt - m_bBackStepFrame -1))*m_bMoveSpeed;
		break;
	case _DIRECTION_LIST_3:
		if ( m_bBackStepFrame < (6-m_bMoveNextFrmCnt) )
			m_shShiftPixelX =  (_CELL_WIDTH /m_bBackStepFrameCnt * (m_bBackStepFrame+1))*m_bMoveSpeed;
		else
			m_shShiftPixelX = -(_CELL_WIDTH /m_bBackStepFrameCnt * (m_bBackStepFrameCnt - m_bBackStepFrame -1))*m_bMoveSpeed;

		m_shShiftPixelY = 0;
		break;
	case _DIRECTION_LIST_4:
		if ( m_bBackStepFrame < (6-m_bMoveNextFrmCnt) )
			m_shShiftPixelX =  (_CELL_WIDTH /m_bBackStepFrameCnt * (m_bBackStepFrame+1))*m_bMoveSpeed;
		else
			m_shShiftPixelX = -(_CELL_WIDTH /m_bBackStepFrameCnt * (m_bBackStepFrameCnt - m_bBackStepFrame -1))*m_bMoveSpeed;

		if ( m_bBackStepFrame < (6-m_bMoveNextFrmCnt) )
			m_shShiftPixelY =  (_CELL_HEIGHT/m_bBackStepFrameCnt * (m_bBackStepFrame+1))*m_bMoveSpeed;
		else
			m_shShiftPixelY = -(_CELL_HEIGHT/m_bBackStepFrameCnt * (m_bBackStepFrameCnt - m_bBackStepFrame -1))*m_bMoveSpeed;
		break;
	case _DIRECTION_LIST_5:
		m_shShiftPixelX =  0;

		if ( m_bBackStepFrame < (6-m_bMoveNextFrmCnt) )
			m_shShiftPixelY =  (_CELL_HEIGHT/m_bBackStepFrameCnt * (m_bBackStepFrame+1))*m_bMoveSpeed;
		else
			m_shShiftPixelY = -(_CELL_HEIGHT/m_bBackStepFrameCnt * (m_bBackStepFrameCnt - m_bBackStepFrame -1))*m_bMoveSpeed;
		break;
	case _DIRECTION_LIST_6:
		if ( m_bBackStepFrame < (6-m_bMoveNextFrmCnt) )
			m_shShiftPixelX = -(_CELL_WIDTH /m_bBackStepFrameCnt * (m_bBackStepFrame+1))*m_bMoveSpeed;
		else
			m_shShiftPixelX =  (_CELL_WIDTH /m_bBackStepFrameCnt * (m_bBackStepFrameCnt - m_bBackStepFrame -1))*m_bMoveSpeed;

		if ( m_bBackStepFrame < (6-m_bMoveNextFrmCnt) )
			m_shShiftPixelY =  (_CELL_HEIGHT/m_bBackStepFrameCnt * (m_bBackStepFrame+1))*m_bMoveSpeed;
		else
			m_shShiftPixelY = -(_CELL_HEIGHT/m_bBackStepFrameCnt * (m_bBackStepFrameCnt - m_bBackStepFrame -1))*m_bMoveSpeed;
		break;
	case _DIRECTION_LIST_7:
		if ( m_bBackStepFrame < (6-m_bMoveNextFrmCnt) )
			m_shShiftPixelX = -(_CELL_WIDTH /m_bBackStepFrameCnt * (m_bBackStepFrame+1))*m_bMoveSpeed;
		else
			m_shShiftPixelX =  (_CELL_WIDTH /m_bBackStepFrameCnt * (m_bBackStepFrameCnt - m_bBackStepFrame -1))*m_bMoveSpeed;

		m_shShiftPixelY =  0;
		break;
	case _DIRECTION_LIST_8:
		if ( m_bBackStepFrame < (6-m_bMoveNextFrmCnt) )
			m_shShiftPixelX = -(_CELL_WIDTH /m_bBackStepFrameCnt * (m_bBackStepFrame+1))*m_bMoveSpeed;
		else
			m_shShiftPixelX =  (_CELL_WIDTH /m_bBackStepFrameCnt * (m_bBackStepFrameCnt - m_bBackStepFrame -1))*m_bMoveSpeed;

		if ( m_bBackStepFrame < (6-m_bMoveNextFrmCnt) )
			m_shShiftPixelY = -(_CELL_HEIGHT/m_bBackStepFrameCnt * (m_bBackStepFrame+1))*m_bMoveSpeed;
		else
			m_shShiftPixelY =  (_CELL_HEIGHT/m_bBackStepFrameCnt * (m_bBackStepFrameCnt - m_bBackStepFrame -1))*m_bMoveSpeed;
		break;	
	}
}


/******************************************************************************************************************

	함수명 : CActor::SetMoved()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CActor::SetMoved()
{
	switch ( m_bMoveDir )
	{
	case _DIRECTION_LIST_1:
		m_shShiftTileX =  0;
		m_shShiftTileY = -m_bMoveSpeed;
		break;
	case _DIRECTION_LIST_2:
		m_shShiftTileX =  m_bMoveSpeed;
		m_shShiftTileY = -m_bMoveSpeed;
		break;
	case _DIRECTION_LIST_3:
		m_shShiftTileX =  m_bMoveSpeed;
		m_shShiftTileY =  0;
		break;
	case _DIRECTION_LIST_4:
		m_shShiftTileX =  m_bMoveSpeed;
		m_shShiftTileY =  m_bMoveSpeed;
		break;
	case _DIRECTION_LIST_5:
		m_shShiftTileX =  0;
		m_shShiftTileY =  m_bMoveSpeed;
		break;
	case _DIRECTION_LIST_6:
		m_shShiftTileX = -m_bMoveSpeed;
		m_shShiftTileY =  m_bMoveSpeed;
		break;
	case _DIRECTION_LIST_7:
		m_shShiftTileX = -m_bMoveSpeed;
		m_shShiftTileY =  0;
		break;
	case _DIRECTION_LIST_8:
		m_shShiftTileX = -m_bMoveSpeed;
		m_shShiftTileY = -m_bMoveSpeed;
		break;
	}
	m_wPosX += m_shShiftTileX;
	m_wPosY += m_shShiftTileY;
	m_shShiftPixelX = 0;
	m_shShiftPixelY = 0;
}


/******************************************************************************************************************

	함수명 : CActor::OnCharDescPacket()

	작성자 : 
	작성일 : 

	목적   : SM_WALK, SM_DEATH, SM_TURN, SM_RUN, SM_DIGUP 메시지에 사용한다.
	입력   : LPPACKETMSG lpPacketMsg
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CActor::OnCharDescPacket(LPPACKETMSG lpPacketMsg)
{
	CHARDESC	stCharDesc;
	FEATURE		stFeature;
	fnDecode6BitBuf(lpPacketMsg->szEncodeData, (char*)&stCharDesc, sizeof(CHARDESC));

	memcpy(&stFeature, &stCharDesc.nFeature, sizeof(FEATURE));
	m_nState = stCharDesc.nStatus;

	if( m_nState & 0X2 )	m_bOpenHealth = TRUE;
	else					m_bOpenHealth = FALSE;

	ChangeFeature(stFeature);
}

VOID CActor::OnUserName(LPPACKETMSG lpPacketMsg)
{
	INT nPos = fnDecode6BitBuf(lpPacketMsg->szEncodeData, m_szName, sizeof(m_szName));
	m_szName[nPos] = '\0';

	m_dwNameClr = GetUserNameColor(lpPacketMsg->stDefMsg.wParam);
}

VOID CActor::OnChangeNameClr(LPPACKETMSG lpPacketMsg)
{
	m_dwNameClr = GetUserNameColor(lpPacketMsg->stDefMsg.wParam);
}

VOID CActor::OnChangeLight(LPPACKETMSG lpPacketMsg)
{
	m_bLightSize = lpPacketMsg->stDefMsg.wParam;
}

VOID CActor::OnOpenHealth(LPPACKETMSG lpPacketMsg)
{
	m_bOpenHealth = TRUE;
}

VOID CActor::OnCloseHealth(LPPACKETMSG lpPacketMsg)
{
	m_bOpenHealth = FALSE;
}

VOID CActor::OnFeatureChanged(LPPACKETMSG lpPacketMsg)
{
	FEATURE stFeature;
	LONG nFeature	  = MAKELONG(lpPacketMsg->stDefMsg.wParam, lpPacketMsg->stDefMsg.wTag);

	memcpy(&stFeature, &nFeature, sizeof(LONG));

	ChangeFeature(stFeature);
}

VOID CActor::OnHealthSpellChanged(LPPACKETMSG lpPacketMsg)
{
	m_wHP	 = lpPacketMsg->stDefMsg.wParam;
	m_wMP	 = lpPacketMsg->stDefMsg.wTag;
	m_wMAXHP = lpPacketMsg->stDefMsg.wSeries;
	FLOAT wHPRate = (FLOAT)((FLOAT)m_wHP/(FLOAT)m_wMAXHP);
	m_bHPPercent  = wHPRate*100;
}

VOID CActor::OnWalk(LPPACKETMSG lpPacketMsg)
{
	BYTE bDir;
//	m_wPosX = lpPacketMsg->stDefMsg.wParam;
//	m_wPosY = lpPacketMsg->stDefMsg.wTag;
	bDir		 = LOBYTE(lpPacketMsg->stDefMsg.wSeries);
	m_bLightSize = HIBYTE(lpPacketMsg->stDefMsg.wSeries);
	OnCharDescPacket(lpPacketMsg);
	SetMotionFrame(_MT_MON_WALK, bDir);
	m_bMoveSpeed = _SPEED_WALK;
	SetMoving();
}

VOID CActor::OnTurn(LPPACKETMSG lpPacketMsg)
{
	BYTE bDir;
	m_wPosX = lpPacketMsg->stDefMsg.wParam;
	m_wPosY = lpPacketMsg->stDefMsg.wTag;
	bDir		 = LOBYTE(lpPacketMsg->stDefMsg.wSeries);
	m_bLightSize = HIBYTE(lpPacketMsg->stDefMsg.wSeries);

	// 식인초.
	if ( m_stFeature.bDress == 31 )
		bDir = 0;

	OnCharDescPacket(lpPacketMsg);

	SetMotionFrame(_MT_MON_STAND, bDir);

	if( m_nState & 0X1 )		// 석화상태.
	{
		switch ( m_stFeature.bDress )
		{
		case 83: //주마신장.
		case 84: //주마호법.
		case 85: //주마왕.
			SetMotionFrame(_MT_MON_APPEAR, 0);
			m_dwEndFrame = m_dwFstFrame + 1;
			m_bCurrMtn = _MT_MON_STAND;
		}
	}
}

VOID CActor::OnDigup(LPPACKETMSG lpPacketMsg)
{
	//MESSAGEBODYWL ????????
	BYTE bDir;
	m_wPosX      = lpPacketMsg->stDefMsg.wParam;
	m_wPosY		 = lpPacketMsg->stDefMsg.wTag;
	bDir		 = LOBYTE(lpPacketMsg->stDefMsg.wSeries);
	m_bLightSize = HIBYTE(lpPacketMsg->stDefMsg.wSeries);
	OnCharDescPacket(lpPacketMsg);
	m_bReverse = FALSE;

	switch ( m_stFeature.bDress )
	{
	case 3: 
		m_bReverse = TRUE;
		break;
	case 85: //주마왕.
		bDir = 0;
		break;
	case 31: //식인초.		
		bDir = 0;
		m_bReverse = TRUE;
		break;
	case 56:
		{
			CMagic* pxMagic;
			pxMagic = new CMagic;
			pxMagic->CreateMagic(_SKILL_SKELLETON, m_wPosX, m_wPosY, m_wPosX, m_wPosY, NULL, 0);
			g_xGameProc.m_xMagicList.AddNode(pxMagic);	
			break;
		}
/*	case 89:
		{
			CMagic* pxMagic;
			pxMagic = new CMagic;
			pxMagic->CreateMagic(_SKILL_SINSU, m_wPosX, m_wPosY, m_wPosX, m_wPosY, NULL, 0);
			g_xGameProc.m_xMagicList.AddNode(pxMagic);	
			break;
		}
*/	}

	m_bAppearState = _DIG_UP;
	SetMotionFrame(_MT_MON_APPEAR, bDir);
}

VOID CActor::OnDigDown(LPPACKETMSG lpPacketMsg)
{
	BYTE bDir;
	m_wPosX		 = lpPacketMsg->stDefMsg.wParam;
	m_wPosY		 = lpPacketMsg->stDefMsg.wTag;
	bDir		 = m_bCurrDir;
	m_bReverse	 = TRUE;

	switch ( m_stFeature.bDress )
	{
	case 3: 
		m_bReverse = FALSE;
		break;
	case 31: //식인초.		
		bDir = 0;
		m_bReverse = FALSE;
		break;
	}

	m_bAppearState = _DIG_DOWN;

	SetMotionFrame(_MT_MON_APPEAR, bDir);
}

VOID CActor::OnDeath(LPPACKETMSG lpPacketMsg)
{
	BYTE bDir;
	m_wPosX = lpPacketMsg->stDefMsg.wParam;
	m_wPosY = lpPacketMsg->stDefMsg.wTag;
	bDir	= lpPacketMsg->stDefMsg.wSeries;
	OnCharDescPacket(lpPacketMsg);
	SetMotionFrame(_MT_MON_DIE, bDir);
	if ( lpPacketMsg->stDefMsg.wIdent == SM_DEATH )			
	{
		m_dwCurrFrame = m_dwEndFrame - 1;
		m_bIsDead = TRUE;
	}
	else 
	{
		// 몬스터일때.
		if ( m_stFeature.bGender == _GENDER_MON )
		{
			CMagic* pxMagic;
			switch ( m_stFeature.bDress )
			{
			case 4:		// 석장인.
				pxMagic = new CMagic;
				pxMagic->CreateMagic(_MONMAGIC_EXPLODE1, m_wPosX, m_wPosY, m_wPosX, m_wPosY, this);
				g_xGameProc.m_xMagicList.AddNode(pxMagic);
				break;
			case  5:		// 바쿠가르나.
			case  6:		// 바자울.
			case 11:		// 모디젼.
			case 19:		// 레디가르나.
				pxMagic = new CMagic;
				pxMagic->CreateMagic(_MONMAGIC_SINGI_DIE, m_wPosX, m_wPosY, m_wPosX, m_wPosY, this);
				g_xGameProc.m_xMagicList.AddNode(pxMagic);
/*				LoadEffect(&g_xGameProc.m_xImage, _MONMAGIC_DIE);
				m_bUseSwordEffect = TRUE;
*/				break;
			case 40:		// 허수아비.
				pxMagic = new CMagic;
				pxMagic->CreateMagic(_MONMAGIC_HUSU_DIE, m_wPosX, m_wPosY, m_wPosX, m_wPosY, this);
				g_xGameProc.m_xMagicList.AddNode(pxMagic);
/*				LoadEffect(&g_xGameProc.m_xImage, _MONMAGIC_HUSU_DIE);
				m_bUseSwordEffect = TRUE;
*/				break;
			case 48:		// 좀비.
				pxMagic = new CMagic;
				pxMagic->CreateMagic(_MONMAGIC_ZOMBIE_DIE, m_wPosX, m_wPosY, m_wPosX, m_wPosY, this);
				g_xGameProc.m_xMagicList.AddNode(pxMagic);
/*				LoadEffect(&g_xGameProc.m_xImage, _MONMAGIC_ZOMBIE_DIE);
				m_bUseSwordEffect = TRUE;
*/				break;
			case 99:		// 바오달드.
				pxMagic = new CMagic;
				pxMagic->CreateMagic(_MONMAGIC_BAODIE, m_wPosX, m_wPosY, m_wPosX, m_wPosY, this);
				g_xGameProc.m_xMagicList.AddNode(pxMagic);
				break;
			default :
				break;
			}
		}
	}	
}

VOID CActor::OnBackStep(LPPACKETMSG lpPacketMsg)
{
	BYTE bDir;
	bDir	= lpPacketMsg->stDefMsg.wSeries;
	m_wPosX = lpPacketMsg->stDefMsg.wParam;
	m_wPosY = lpPacketMsg->stDefMsg.wTag;
	SetMotionFrame(_MT_MON_WALK, bDir);
	if ( m_bCurrDir < 4 )			m_bMoveDir = m_bCurrDir + 4;
	else							m_bMoveDir = m_bCurrDir - 4;
	m_bMoveSpeed = _SPEED_WALK;
	m_bBackStepFrame	= 0;
	m_bBackStepFrameCnt = 6;
	SetBackStepMoving();
}

VOID CActor::OnStruck(LPPACKETMSG lpPacketMsg)
{
	MESSAGEBODYWL stMsgBodyWl;
	FEATURE		  stFeature;

	FLOAT wHPRate = (FLOAT)((FLOAT)lpPacketMsg->stDefMsg.wParam/(FLOAT)lpPacketMsg->stDefMsg.wTag);
	WORD  wDamage = lpPacketMsg->stDefMsg.wSeries;
	m_wHP		  = lpPacketMsg->stDefMsg.wParam;
	m_wMAXHP	  = lpPacketMsg->stDefMsg.wTag;
	m_bHPPercent  = wHPRate*100;

	fnDecode6BitBuf(lpPacketMsg->szEncodeData, (char*)&stMsgBodyWl, sizeof(MESSAGEBODYWL));

	memcpy(&stFeature, &stMsgBodyWl.lParam1, sizeof(LONG));

	ChangeFeature(stFeature);
	SetMotionFrame(_MT_MON_HITTED, m_bCurrDir);

	// 몬스터일때.
	if ( m_stHitter.bGender == _GENDER_MON )
	{
		CMagic* pxMagic;
		switch ( m_stHitter.bDress )
		{
		case 2:		// 케팔로프.
			pxMagic = new CMagic;
			pxMagic->CreateMagic(_MONMAGIC_KEPAL, m_wPosX, m_wPosY, m_wPosX, m_wPosY, this);
			g_xGameProc.m_xMagicList.AddNode(pxMagic);
			break;
		case 8:		// 갑주개미.
		case 14:	// 병용개미.
		case 16:	// 드난개미.
			pxMagic = new CMagic;
			pxMagic->CreateMagic(_MONMAGIC_GREATANT, m_wPosX, m_wPosY, m_wPosX, m_wPosY, this);
			g_xGameProc.m_xMagicList.AddNode(pxMagic);
			break;
		case 67:	// 촉룡신.
			pxMagic = new CMagic;
			pxMagic->CreateMagic(_MONMAGIC_BIGGINE_CHAR, m_wPosX, m_wPosY, m_wPosX, m_wPosY, this);
			g_xGameProc.m_xMagicList.AddNode(pxMagic);
			break;
		default :
			break;
		}
	}
	ZeroMemory(&m_stHitter, sizeof(FEATURE));
}


VOID CActor::OnLighting(LPPACKETMSG lpPacketMsg)
{
	WORD	wTargetX, wTargetY;
	INT		nTargetID;
	CMagic* pxMagic;

	m_wPosX      = lpPacketMsg->stDefMsg.wParam;
	m_wPosY		 = lpPacketMsg->stDefMsg.wTag;
	m_bCurrDir	 = lpPacketMsg->stDefMsg.wSeries;

	MESSAGEBODYWL stMsgBodyWl;
	fnDecode6BitBuf(lpPacketMsg->szEncodeData, (char*)&stMsgBodyWl, sizeof(MESSAGEBODYWL));
	wTargetX = stMsgBodyWl.lParam1;
	wTargetY = stMsgBodyWl.lParam2;
	nTargetID = MAKELONG(stMsgBodyWl.nTag1, stMsgBodyWl.nTag2);

	switch ( m_stFeature.bDress )
	{
	case 48:		// 좀비. 8방향.
		pxMagic = new CMagic;
		pxMagic->CreateMagic(_MONMAGIC_ZOMBIE, m_wPosX, m_wPosY, m_wPosX, m_wPosY, this, nTargetID);
		g_xGameProc.m_xMagicList.AddNode(pxMagic);
		SetMotionFrame(_MT_MON_ATTACK_A, m_bCurrDir);
		break;
	}
}


VOID CActor::OnFlyAxe(LPPACKETMSG lpPacketMsg)
{
	WORD	wTargetX, wTargetY;
	INT		nTargetID;
	CMagic* pxMagic;

	m_wPosX      = lpPacketMsg->stDefMsg.wParam;
	m_wPosY		 = lpPacketMsg->stDefMsg.wTag;
	m_bCurrDir	 = lpPacketMsg->stDefMsg.wSeries;

	MESSAGEBODYW stMsgBodyW;
	fnDecode6BitBuf(lpPacketMsg->szEncodeData, (char*)&stMsgBodyW, sizeof(MESSAGEBODYW));
	wTargetX = stMsgBodyW.wParam1;
	wTargetY = stMsgBodyW.wParam2;
	nTargetID = MAKELONG(stMsgBodyW.wTag1, stMsgBodyW.wTag2);

	switch ( m_stFeature.bDress )
	{
	case 33:				// 쌍도끼해골.
		pxMagic = new CMagic;
		pxMagic->CreateMagic(_MONMAGIC_DUALAXE, m_wPosX, m_wPosY, wTargetX, wTargetY, this, nTargetID);
		g_xGameProc.m_xMagicList.AddNode(pxMagic);
		SetMotionFrame(_MT_MON_ATTACK_A, m_bCurrDir);
		break;
	case 46:				// 다크.
		pxMagic = new CMagic;
		pxMagic->CreateMagic(_MONMAGIC_CHIM, m_wPosX, m_wPosY, wTargetX, wTargetY, this, nTargetID);
		g_xGameProc.m_xMagicList.AddNode(pxMagic);
		SetMotionFrame(_MT_MON_ATTACK_A, m_bCurrDir);
		break;
	case 82:				// 마궁사.
		pxMagic = new CMagic;
		pxMagic->CreateMagic(_MONMAGIC_MAARROW, m_wPosX, m_wPosY, wTargetX, wTargetY, this, nTargetID);
		g_xGameProc.m_xMagicList.AddNode(pxMagic);
		SetMotionFrame(_MT_MON_ATTACK_A, m_bCurrDir);
		break;
	}
}


VOID CActor::OnHit(LPPACKETMSG lpPacketMsg)
{
	BYTE bDir;
	bDir	= lpPacketMsg->stDefMsg.wSeries;
	m_wPosX = lpPacketMsg->stDefMsg.wParam;
	m_wPosY = lpPacketMsg->stDefMsg.wTag;
	SetMotionFrame(_MT_MON_ATTACK_A, bDir);

	// 몬스터일때.
	if ( m_stFeature.bGender == _GENDER_MON )
	{
		CMagic* pxMagic;
		switch ( m_stFeature.bDress )
		{
		case 9:		// 론.
			pxMagic = new CMagic;
			pxMagic->CreateMagic(_MONMAGIC_EXPLODE, m_wPosX, m_wPosY, m_wPosX, m_wPosY, this);
			g_xGameProc.m_xMagicList.AddNode(pxMagic);
			break;
		case 5:		// 바쿠가르나.
			pxMagic = new CMagic;
			pxMagic->CreateMagic(_MONMAGIC_BLACK1, m_wPosX, m_wPosY, m_wPosX, m_wPosY, this);
			g_xGameProc.m_xMagicList.AddNode(pxMagic);
			break;
		case 19:		// 레디가르나.
			pxMagic = new CMagic;
			pxMagic->CreateMagic(_MONMAGIC_RED1, m_wPosX, m_wPosY, m_wPosX, m_wPosY, this);
			g_xGameProc.m_xMagicList.AddNode(pxMagic);
			break;
		case 45:		// 우면귀왕. 
			pxMagic = new CMagic;
			pxMagic->CreateMagic(_MONMAGIC_COWGHOST, m_wPosX, m_wPosY, m_wPosX, m_wPosY, this);
			g_xGameProc.m_xMagicList.AddNode(pxMagic);
			break;
		case 90:		// 신수.
			pxMagic = new CMagic;
			pxMagic->CreateMagic(_MONMAGIC_SINSU, m_wPosX, m_wPosY, m_wPosX, m_wPosY, this);
			g_xGameProc.m_xMagicList.AddNode(pxMagic);
			break;
		case 43:		// 화염우면귀.
			pxMagic = new CMagic;
			pxMagic->CreateMagic(_MONMAGIC_COWFLAME, m_wPosX, m_wPosY, m_wPosX, m_wPosY, this);
			g_xGameProc.m_xMagicList.AddNode(pxMagic);
			break;
		case 85:		// 주마왕.
			pxMagic = new CMagic;
			pxMagic->CreateMagic(_MONMAGIC_JUMAWANG, m_wPosX, m_wPosY, m_wPosX, m_wPosY, this);
			g_xGameProc.m_xMagicList.AddNode(pxMagic);
			break;
		case 67:		// 촉룡신.
			pxMagic = new CMagic;
			pxMagic->CreateMagic(_MONMAGIC_BIGGINE_ATT, m_wPosX, m_wPosY, m_wPosX, m_wPosY, this);
			g_xGameProc.m_xMagicList.AddNode(pxMagic);
			break;	
		case 75:		// 쐐기나방.
			pxMagic = new CMagic;
			pxMagic->CreateMagic(_MONMAGIC_SSEGI, m_wPosX, m_wPosY, m_wPosX, m_wPosY, this);
			g_xGameProc.m_xMagicList.AddNode(pxMagic);
			break;	
		case 94:		// 사어.
			pxMagic = new CMagic;
			pxMagic->CreateMagic(_MONMAGIC_SANDFISH, m_wPosX, m_wPosY, m_wPosX, m_wPosY, this);
			g_xGameProc.m_xMagicList.AddNode(pxMagic);
			break;	
		case 98:		// 상급괴물.
			m_dwCurrEffectFrame = 50;
			m_dwFstEffectFrame  = 50;
			m_dwEndEffectFrame  = 60;
			m_bEffectFrame		= 0;
			m_bEffectFrameCnt	= _DEFAULT_SPELLFRAME;
			LoadEffect(&g_xGameProc.m_xImage, _MONMAGIC_NUMAGUMGI, bDir);
			m_bUseSwordEffect = TRUE;
			break;	
		default :
			break;
		}
	}
}

VOID CActor::StruckMsgReassign()
{
	INT			nCnt;
	SHORT		shLeftMsgCnt;
	LPPACKETMSG	lpPacketMsg;

	lpPacketMsg  = NULL;
	shLeftMsgCnt = m_xPacketQueue.GetCount();

	if ( shLeftMsgCnt > 1 )
	{
		for ( nCnt = 0; nCnt < shLeftMsgCnt; nCnt++ )
		{
			lpPacketMsg = (LPPACKETMSG)m_xPacketQueue.PopQ();
			if ( lpPacketMsg )
			{
				if ( lpPacketMsg->stDefMsg.wIdent == SM_STRUCK )
				{
					SAFE_DELETE(lpPacketMsg);
				}
				else
				{
					m_xPacketQueue.PushQ((BYTE*)lpPacketMsg);
				}
			}
		}
	}
}


/******************************************************************************************************************

	함수명 : CActor::UpdatePacketState()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CActor::UpdatePacketState()
{
	LPPACKETMSG	lpPacketMsg = NULL;

	if ( m_bCurrMtn == _MT_MON_STAND )
	{
		SHORT shLeftMsgCnt = m_xPacketQueue.GetCount();

		if ( shLeftMsgCnt > 0 )
		{
			lpPacketMsg = (LPPACKETMSG)m_xPacketQueue.PopQ();

			if ( shLeftMsgCnt >= 3 )		m_bMsgHurryCheck = TRUE;
			else							m_bMsgHurryCheck = FALSE;

			if ( lpPacketMsg )
			{
				switch ( lpPacketMsg->stDefMsg.wIdent )
				{
					case SM_NOWDEATH:
					case SM_DEATH:				{	OnDeath(lpPacketMsg);				break;	}
					case SM_WALK:				{	OnWalk(lpPacketMsg);				break;	}
					case SM_TURN:				{	OnTurn(lpPacketMsg);				break;	}
					case SM_DIGUP:				{	OnDigup(lpPacketMsg);				break;	}
					case SM_DIGDOWN:			{	OnDigDown(lpPacketMsg);				break;	}
					case SM_FEATURECHANGED:		{	OnFeatureChanged(lpPacketMsg);		break;	}
					case SM_OPENHEALTH:			{	OnOpenHealth(lpPacketMsg);			break;	}
					case SM_CLOSEHEALTH:		{	OnCloseHealth(lpPacketMsg);			break;	}
					case SM_CHANGELIGHT:		{	OnChangeLight(lpPacketMsg);			break;	}
					case SM_CHANGENAMECOLOR:	{	OnChangeNameClr(lpPacketMsg);		break;	}
					case SM_USERNAME:			{	OnUserName(lpPacketMsg);			break;	}
					case SM_HEALTHSPELLCHANGED:	{	OnHealthSpellChanged(lpPacketMsg);	break;	}
					case SM_BACKSTEP:			{	OnBackStep(lpPacketMsg);			break;	}
					case SM_STRUCK:				{	OnStruck(lpPacketMsg);				break;	}
					case SM_HIT:				{	OnHit(lpPacketMsg);					break;	}
					case SM_FLYAXE:				{	OnFlyAxe(lpPacketMsg);				break;	}
					case SM_LIGHTING:			{	OnLighting(lpPacketMsg);			break;	}
					case SM_SKELETON:
						OnWalk(lpPacketMsg);
						break;
					default:
					{										
						break;	
					}
				}
			}
			SAFE_DELETE(lpPacketMsg);
			return TRUE;
		}
	}
	return FALSE;
}



/******************************************************************************************************************

	함수명 : CActor::LoadEffect()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : CImageHandler* pxImgHandler
	         WORD wEffectNum
	         BYTE bDir
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CActor::LoadEffect(CImageHandler* pxImgHandler, WORD wEffectNum, BYTE bDir)
{
	WORD wFileType;
	LPEFFECTSPRINFO pstEffect = g_xSpriteInfo.GetEffectInfo(wEffectNum);

	if ( pstEffect )
	{
		m_bEffectImgIdx = pstEffect->wImgIdx;		

		m_dwFstEffectFrame = pstEffect->dwFstFrm;
		m_dwEndEffectFrame = pstEffect->dwEndFrm;

		if ( bDir )
		{
			m_dwFstEffectFrame = m_dwFstEffectFrame + bDir*10;
			m_dwEndEffectFrame = m_dwEndEffectFrame + bDir*10;
		}
		m_dwCurrEffectFrame = m_dwFstEffectFrame;
		m_bEffectFrame = 0;

		m_bLightRadius[0] = pstEffect->bLightRadius[0];
		m_bLightRadius[1] = pstEffect->bLightRadius[1];

		m_bLightColor[0][0]	= pstEffect->bLightColor[0][0];
		m_bLightColor[0][1]	= pstEffect->bLightColor[0][1];
		m_bLightColor[0][2]	= pstEffect->bLightColor[0][2];
		
		m_bLightColor[1][0]	= pstEffect->bLightColor[1][0];
		m_bLightColor[1][1]	= pstEffect->bLightColor[1][1];
		m_bLightColor[1][2]	= pstEffect->bLightColor[1][2];

		for ( INT nCnt = m_dwFstEffectFrame; nCnt < m_dwEndEffectFrame; nCnt++ )
		{
			if ( m_bEffectImgIdx == _IMAGE_MAGIC )
			{
				wFileType = _TEXTR_FILE_MAGIC;
			}
			else if ( m_bEffectImgIdx == _IMAGE_MONMAGIC )
			{
				wFileType = _TEXTR_FILE_MONMAGIC;
			}

			pxImgHandler->AddTextr(wFileType, m_bEffectImgIdx, nCnt);
		}
		return TRUE;

	}
	return FALSE;
}



/******************************************************************************************************************

	함수명 : CActor::DrawEffect()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CActor::DrawEffect()
{
	if ( g_xMainWnd.Get3DDevice() )
	{
		if( SUCCEEDED(g_xMainWnd.Get3DDevice()->BeginScene()) )
		{
			WORD	  wFileType;
			D3DVECTOR vTrans;
			D3DMATRIX matTrans;
			D3DMATRIX matScale;
			D3DMATRIX matRot;
			D3DMATRIX matWorld;
			D3DMATRIX matWorldOriginal;

			g_xMainWnd.Get3DDevice()->GetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

			D3DMATERIAL7 mtrl;
			CWHWilImageData* pxWilImg;

			if ( m_bUseEffect || m_bUseSwordEffect )
			{
				BOOL bIndexSetted = FALSE;
				if ( m_bUseEffect )
				{
					pxWilImg = &g_xGameProc.m_xImage.m_xImageList[m_bEffectImgIdx];
					bIndexSetted = pxWilImg->NewSetIndex(m_dwCurrEffectFrame);
				}
				else
				{
					pxWilImg = &g_xGameProc.m_xImage.m_xImageList[m_bEffectImgIdx];
					m_dwCurrEffectFrame = m_dwFstEffectFrame + m_dwCurrFrame - m_dwFstFrame;
					bIndexSetted = pxWilImg->NewSetIndex(m_dwCurrEffectFrame);
				}

				if ( bIndexSetted )
				{
 					vTrans.x = (FLOAT)m_shScrnPosX+(FLOAT)pxWilImg->m_lpstNewCurrWilImageInfo->shWidth/2+pxWilImg->m_lpstNewCurrWilImageInfo->shPX-400; 
					vTrans.y = (FLOAT)-m_shScrnPosY-(FLOAT)pxWilImg->m_lpstNewCurrWilImageInfo->shHeight/2-pxWilImg->m_lpstNewCurrWilImageInfo->shPY+300; 
					vTrans.z = 0;
					D3DUtil_SetTranslateMatrix(matTrans, vTrans);
					D3DUtil_SetScaleMatrix(matScale, (FLOAT)pxWilImg->m_lpstNewCurrWilImageInfo->shWidth, (FLOAT)pxWilImg->m_lpstNewCurrWilImageInfo->shHeight, 0.0f);
					D3DMath_MatrixMultiply(matWorld, matScale, matTrans);
					g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);

					if ( m_bEffectImgIdx == _IMAGE_MAGIC )
					{
						wFileType = _TEXTR_FILE_MAGIC;
					}
					else if ( m_bEffectImgIdx == _IMAGE_MONMAGIC )
					{
						wFileType = _TEXTR_FILE_MONMAGIC;
					}
					LPDIRECTDRAWSURFACE7 lpddsTextr = g_xGameProc.m_xImage.GetTextrImg(wFileType, m_bEffectImgIdx, m_dwCurrEffectFrame);
					g_xMainWnd.Get3DDevice()->SetTexture(0, lpddsTextr);
//					g_xMainWnd.Get3DDevice()->SetTexture(0, D3DWILTextr_GetSurface(pxWilImg->m_szWilFileName, m_dwCurrEffectFrame));

					D3DUtil_InitMaterial(mtrl, 
										 (FLOAT)255/255.0f,
										 (FLOAT)255/255.0f, 
										 (FLOAT)255/255.0f);
					mtrl.diffuse.a = 0/255.0f;
					g_xMainWnd.Get3DDevice()->SetMaterial(&mtrl);

					SetBlendRenderState(g_xMainWnd.Get3DDevice(), _BLEND_LIGHTINV, mtrl);
					g_xMainWnd.Get3DDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX, m_avBoard, 4, NULL);

					// 원상복귀.
					ZeroMemory(&mtrl, sizeof(mtrl));
					mtrl.diffuse.r = mtrl.diffuse.g = mtrl.diffuse.b = 0.1f;
					mtrl.ambient.r = mtrl.ambient.g = mtrl.ambient.b = 1.0f;
					g_xMainWnd.Get3DDevice()->SetMaterial(&mtrl);

					ResetBlendenderState(g_xMainWnd.Get3DDevice());
					g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);
				}


				if ( m_bUseEffect )
				{
					if ( m_bEffectFrame < m_bEffectFrameCnt / 2 )
					{
						m_bLightRadius[0] = m_bEffectFrame;
						m_bLightRadius[1] = m_bEffectFrame+1;
					}
					else
					{
						m_bLightRadius[0] = m_bEffectFrameCnt - m_bEffectFrame -1;
						m_bLightRadius[1] = m_bEffectFrameCnt - m_bEffectFrame;
					}
					
					g_xGameProc.m_xLightFog.SetLightRadiusWithRing(
													  m_shScrnPosX + _CELL_WIDTH/2,
							  						  m_shScrnPosY + _CELL_HEIGHT/2, 
													  m_bLightRadius[0], m_bLightColor[0][0], m_bLightColor[0][1], m_bLightColor[0][2], 
													  m_bLightRadius[1], m_bLightColor[1][0], m_bLightColor[1][1], m_bLightColor[1][2]);
				}
				else if ( m_bUseSwordEffect )
				{
					if ( m_dwEndEffectFrame - m_dwFstEffectFrame > 8 )
					{
						m_bLightRadius[0] = (m_dwCurrEffectFrame - m_dwFstEffectFrame)/2 + 2;
						m_bLightRadius[1] = (m_dwCurrEffectFrame - m_dwFstEffectFrame)/2 + 3;
					}
					else
					{
						m_bLightRadius[0] = m_dwCurrEffectFrame - m_dwFstEffectFrame + 2;
						m_bLightRadius[1] = m_dwCurrEffectFrame - m_dwFstEffectFrame + 3;
					}
					g_xGameProc.m_xLightFog.SetLightRadiusWithRing(
													  m_shScrnPosX + _CELL_WIDTH/2,
							  						  m_shScrnPosY + _CELL_HEIGHT/2, 
													  m_bLightRadius[0], m_bLightColor[0][0], m_bLightColor[0][1], m_bLightColor[0][2], 
													  m_bLightRadius[1], m_bLightColor[1][0], m_bLightColor[1][1], m_bLightColor[1][2]);
				}

			}

			g_xMainWnd.Get3DDevice()->EndScene();

			return S_OK;
		}
	}
	return E_FAIL;
}



/******************************************************************************************************************

	함수명 : CActor::UpdateMotionState()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : INT nLoopTime
	         SHORT shStartViewTileX
	         SHORT shStartViewTileY
	         SHORT shViewOffsetX
	         SHORT shViewOffsetY
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CActor::UpdateMotionState(INT nLoopTime, BOOL bIsMoveTime)
{
	m_wABlendCurrDelay += nLoopTime;

	if ( m_wABlendCurrDelay >= m_wABlendDelay )
	{
		m_wABlendCurrDelay = 0;
		m_wABlendDelay	   = 0;
		m_bABlendRev	   = FALSE;
	}

	if ( m_bCurrMtn == _MT_MON_DIE && m_dwCurrFrame >= m_dwEndFrame-1 )
	{
		m_bIsDead = TRUE;
	}

	if ( m_bIsDead )
	{
		SetMotionFrame(_MT_MON_DIE, m_bCurrDir);
		m_dwCurrFrame = m_dwEndFrame - 1;
		return;
	}

	if ( UpdateMove(bIsMoveTime) )
	{
		UpdatePacketState();
		return;
	}
	else
	{
		m_wCurrDelay	   += nLoopTime;

		if ( m_wCurrDelay > m_wDelay )
		{
			m_wCurrDelay = 0;

			if ( m_dwCurrFrame < m_dwEndFrame )
			{
				m_dwCurrFrame++;

				PlayActSound();

				if ( m_bMsgHurryCheck )			
				{
					m_wDelay = m_wDelay/2;
					m_bMsgHurryCheck = FALSE;
				}
			}
		}

		UpdatePacketState();

		if ( m_dwCurrFrame >= m_dwEndFrame )
		{
			m_shShiftTileX = 0;
			m_shShiftTileY = 0;
			m_shShiftPixelX = 0;
			m_shShiftPixelY = 0;

			m_bReverse		= FALSE;
			m_dwCurrEffectFrame = 0;
			m_dwFstEffectFrame = 0;
			m_dwEndEffectFrame = 0;
			m_bEffectFrame = 0;
			m_bEffectFrameCnt = _DEFAULT_SPELLFRAME;
			m_bUseEffect = FALSE;
			m_bUseSwordEffect = FALSE;
			m_dwCurrFrame = m_dwFstFrame;

			SetMotionFrame(_MT_MON_STAND, m_bCurrDir);

			if( m_nState & 0X1 )		// 석화상태.
			{
				switch ( m_stFeature.bDress )
				{
				case 83: //주마신장.
				case 84: //주마호법.
				case 85: //주마왕.
					SetMotionFrame(_MT_MON_APPEAR, 0);
					m_dwEndFrame = m_dwFstFrame + 1;
					m_bCurrMtn = _MT_MON_STAND;
				}
			}

			if ( m_bAppearState == _DIG_DOWN )
				m_bAppearState = _DIG_DOWNDEL;
		}
	}
}



/******************************************************************************************************************

	함수명 : CActor::UpdateMove(BOOL bIsMoveTime)

	작성자 : 
	작성일 : 

	목적   : 
	출력   : BOOL

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CActor::UpdateMove(BOOL bIsMoveTime)
{
	if ( m_bCurrMtn == _MT_MON_WALK || m_bCurrMtn == _MT_PUSHBACK )
	{
		m_wCurrDelay = 0;

		if ( bIsMoveTime )
		{
			if ( m_bBackStepFrameCnt )
			{
				m_dwCurrFrame += 2;
			}
			else
			{
				m_dwCurrFrame++;

				if ( m_bMsgHurryCheck )			
				{
					m_dwCurrFrame++;
				}
			}

			if ( m_dwCurrFrame >= m_dwEndFrame-m_bMoveNextFrmCnt && !m_bIsMoved )
			{
				SetMoved();
				m_bIsMoved = TRUE;
			}
		}

		// 연속적인 프레임 중에서 해야할일.
		if ( m_dwCurrFrame >= m_dwEndFrame )
		{
			m_dwCurrFrame = m_dwEndFrame - 1;

			m_shShiftTileX = 0;
			m_shShiftTileY = 0;
			m_shShiftPixelX = 0;
			m_shShiftPixelY = 0;

			m_dwCurrEffectFrame = 0;
			m_dwFstEffectFrame = 0;
			m_dwEndEffectFrame = 0;
			m_bEffectFrame = 0;
			m_bUseEffect = FALSE;
			m_bUseSwordEffect = FALSE;
			m_dwCurrFrame = m_dwFstFrame;

			m_bBackStepFrame = 0;
			m_bBackStepFrameCnt = 0;

			m_bIsMoved = FALSE;

			SetMotionFrame(_MT_MON_STAND, m_bCurrDir);
		}
		else if ( m_dwCurrFrame < m_dwEndFrame )
		{
			if ( m_bCurrMtn != _MT_PUSHBACK )
				SetMoving();
			else
				SetBackStepMoving();
		}
		return TRUE;
	}
	return FALSE;
}



/******************************************************************************************************************

	함수명 : CActor::DrawActor()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : BOOL bFocused
	         BOOL bShadowAblended
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CActor::DrawActor(CMapHandler* pxMap, BOOL bFocused, BOOL bShadowAblended, BOOL bUseScrnPos, BOOL bDrawShadow)
{
	// 좌표처리.
	if ( bUseScrnPos )
	{
		m_shScrnPosX = (m_wPosX - pxMap->m_shStartViewTileX) * _CELL_WIDTH + _VIEW_CELL_X_START - pxMap->m_shViewOffsetX + m_shShiftPixelX;
		m_shScrnPosY = (m_wPosY - pxMap->m_shStartViewTileY) * _CELL_HEIGHT+ _VIEW_CELL_Y_START - pxMap->m_shViewOffsetY + m_shShiftPixelY;
	}

	BYTE	bShadowType;
	SHORT	shShadowPX;
	SHORT	shShadowPY;

	if ( m_pxActorImage )
	{
		SHORT shPX, shPY;
		DWORD dwCurrFrm;

		if ( !m_bReverse )
		{
			if ( m_bBackStepFrameCnt )		dwCurrFrm = m_dwEndFrame - (m_dwCurrFrame - m_dwFstFrame) -1;
			else							dwCurrFrm = m_dwCurrFrame;
		}
		else
		{
			if ( m_bBackStepFrameCnt )		dwCurrFrm = m_dwCurrFrame;
			else							dwCurrFrm = m_dwEndFrame - (m_dwCurrFrame - m_dwFstFrame) -1;
		}

		if ( m_pxActorImage->NewSetIndex(dwCurrFrm) )
		{
			shPX = m_pxActorImage->m_lpstNewCurrWilImageInfo->shPX;
			shPY = m_pxActorImage->m_lpstNewCurrWilImageInfo->shPY;

			bShadowType = m_pxActorImage->m_lpstNewCurrWilImageInfo->bShadow;
			shShadowPX  = m_pxActorImage->m_lpstNewCurrWilImageInfo->shShadowPX;
			shShadowPY  = m_pxActorImage->m_lpstNewCurrWilImageInfo->shShadowPY;

			SetRect(&m_rcActor, m_shScrnPosX + shPX, m_shScrnPosY + shPY, 
					m_shScrnPosX + shPX + m_pxActorImage->m_lpstNewCurrWilImageInfo->shWidth, 
					m_shScrnPosY + shPY + m_pxActorImage->m_lpstNewCurrWilImageInfo->shHeight);

			if ( ( m_rcActor.right - m_rcActor.left ) > _CELL_WIDTH + _TARGETRGN_GAPX )
			{
				m_rcTargetRgn.left  = m_rcActor.left + ( (m_rcActor.right - m_rcActor.left) - (_CELL_WIDTH + _TARGETRGN_GAPX) )/2;
				m_rcTargetRgn.right = m_rcActor.left + ( (m_rcActor.right - m_rcActor.left) + (_CELL_WIDTH + _TARGETRGN_GAPX) )/2;
			}
			else
			{
				m_rcTargetRgn.left  = m_rcActor.left;
				m_rcTargetRgn.right = m_rcActor.right;
			}

			if ( ( m_rcActor.bottom - m_rcActor.top ) > _CELL_HEIGHT + _TARGETRGN_GAPY )
			{
				m_rcTargetRgn.top    = m_rcActor.top + ( (m_rcActor.bottom - m_rcActor.top) - (_CELL_HEIGHT + _TARGETRGN_GAPY) )/2;
				m_rcTargetRgn.bottom = m_rcActor.top + ( (m_rcActor.bottom - m_rcActor.top) + (_CELL_HEIGHT + _TARGETRGN_GAPY) )/2;
			}
			else
			{
				m_rcTargetRgn.top    = m_rcActor.top;
				m_rcTargetRgn.bottom = m_rcActor.bottom;
			}


			INT nStartX1 = m_shScrnPosX + shShadowPX;
			INT nStartY1 = m_shScrnPosY + shShadowPY;

			BYTE bOpaRate = 70;
			WORD wState   = GetCharState();

			// 몬스터 그림자 파일 적용.
			if ( bDrawShadow )
			{
				CWHWilImageData* pxSahdowImage;
				pxSahdowImage = &g_xGameProc.m_xImage.m_xImageList[m_bActorImgIdx+_MAX_MONSTER_IMAGE];
				if ( pxSahdowImage->NewSetIndex(dwCurrFrm) )
				{
					SHORT shShadowPX, shShadowPY;
					if ( m_wABlendDelay || wState==_STATE_ABLEND || bShadowAblended )
					{
						shShadowPX = pxSahdowImage->m_lpstNewCurrWilImageInfo->shPX;
						shShadowPY = pxSahdowImage->m_lpstNewCurrWilImageInfo->shPY;

						g_xMainWnd.DrawWithABlendCompDataWithBackBuffer(m_shScrnPosX+shShadowPX, m_shScrnPosY+shShadowPY, pxSahdowImage->m_lpstNewCurrWilImageInfo->shWidth,	pxSahdowImage->m_lpstNewCurrWilImageInfo->shHeight, (WORD*)pxSahdowImage->m_pbCurrImage,
																		_CLIP_WIDTH, _CLIP_HEIGHT, 0XFFFF, 0XFFFF, bOpaRate);
					}
					else
					{
						shShadowPX = pxSahdowImage->m_lpstNewCurrWilImageInfo->shPX;
						shShadowPY = pxSahdowImage->m_lpstNewCurrWilImageInfo->shPY;
						
						if ( !m_bIsDead )
							g_xMainWnd.DrawWithImageForCompClipRgn(m_shScrnPosX+shShadowPX, m_shScrnPosY+shShadowPY, pxSahdowImage->m_lpstNewCurrWilImageInfo->shWidth,	pxSahdowImage->m_lpstNewCurrWilImageInfo->shHeight, (WORD*)pxSahdowImage->m_pbCurrImage,
																   _CLIP_WIDTH, _CLIP_HEIGHT, 0XFFFF, 0XFFFF, bFocused);
					}
				}
				// 계산해서 그리기.
				else
				{
					// 그림자를 그린다.
					if ( m_wABlendDelay || wState==_STATE_ABLEND || bShadowAblended )
					{
						if ( bOpaRate < 0 && bOpaRate > 100 )		bOpaRate  = 0;
						bShadowAblended = TRUE;
					}
					if ( !m_bIsDead )
					{
						g_xMainWnd.DrawWithShadowABlend(
												nStartX1, nStartY1, 
												m_pxActorImage->m_lpstNewCurrWilImageInfo->shWidth,							
												m_pxActorImage->m_lpstNewCurrWilImageInfo->shHeight,
												(WORD*)m_pxActorImage->m_pbCurrImage,
												_CLIP_WIDTH, _CLIP_HEIGHT, g_xGameProc.m_wShadowClr, bShadowAblended, bShadowType, bOpaRate);
					}
					else
					{
						g_xMainWnd.DrawWithShadowABlend(
												m_rcActor.left+3, m_rcActor.top+2, 
												m_pxActorImage->m_lpstNewCurrWilImageInfo->shWidth,							
												m_pxActorImage->m_lpstNewCurrWilImageInfo->shHeight,
												(WORD*)m_pxActorImage->m_pbCurrImage,
												_CLIP_WIDTH, _CLIP_HEIGHT, g_xGameProc.m_wShadowClr, bShadowAblended, 50, bOpaRate);
					}
				}
			}

			if ( m_wABlendDelay )
			{
				if ( !m_bABlendRev )
					bOpaRate = 100 - ( (m_wABlendCurrDelay * 100) / m_wABlendDelay );
				else
					bOpaRate = ( (m_wABlendCurrDelay * 100) / m_wABlendDelay );
			}

			if ( bOpaRate < 0 && bOpaRate > 100 )		bOpaRate  = 0;

			if ( bFocused )								
			{
				if ( !m_bABlendRev )
					bOpaRate -= 20;
				else
					bOpaRate += 20;
			}

			DrawWithEffected(m_rcActor.left, m_rcActor.top, m_pxActorImage->m_lpstNewCurrWilImageInfo->shWidth, m_pxActorImage->m_lpstNewCurrWilImageInfo->shHeight, (WORD*)m_pxActorImage->m_pbCurrImage, 
							 0XFFFF, 0XFFFF, bFocused, bOpaRate, wState);

			DrawEffect();

			return TRUE;
		}
	}
	return FALSE;
}



/******************************************************************************************************************

	함수명 : CActor::DrawHPBar()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CActor::DrawHPBar()
{
	if ( m_bOpenHealth )
	{
		FLOAT fHPPercent = (FLOAT)((FLOAT)m_bHPPercent/100);
		RECT rcHP = {0, 0, (36*fHPPercent), 4};
 		g_xGameProc.m_xImage.m_xImageList[_IMAGE_PROGUSE].NewSetIndex(2);
 		g_xMainWnd.DrawWithImageForComp(
						m_shScrnPosX+7,
						m_shScrnPosY-53,
						g_xGameProc.m_xImage.m_xImageList[_IMAGE_PROGUSE].m_lpstNewCurrWilImageInfo->shWidth,					
						g_xGameProc.m_xImage.m_xImageList[_IMAGE_PROGUSE].m_lpstNewCurrWilImageInfo->shHeight,
						(WORD*)g_xGameProc.m_xImage.m_xImageList[_IMAGE_PROGUSE].m_pbCurrImage);
		g_xGameProc.m_xImage.m_xImageList[_IMAGE_PROGUSE].NewSetIndex(3);
 		g_xMainWnd.DrawWithImageForComp(
						m_shScrnPosX+7,
						m_shScrnPosY-53,
						rcHP,
						(WORD*)g_xGameProc.m_xImage.m_xImageList[_IMAGE_PROGUSE].m_pbCurrImage);
	}
}



/******************************************************************************************************************

	함수명 : CActor::DrawName()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CActor::DrawName()
{
	SIZE sizeLen;
	sizeLen = g_xMainWnd.GetStrLength(NULL, NULL, m_szName);

//	RECT rc = {m_shScrnPosX+(_CELL_WIDTH-sizeLen.cx)/2, m_shScrnPosY-25, m_shScrnPosX+(_CELL_WIDTH+sizeLen.cx)/2, m_shScrnPosY-25+20};
	RECT rc = {m_rcActor.left + (m_rcActor.right-m_rcActor.left-sizeLen.cx)/2, m_rcActor.top + (m_rcActor.bottom-m_rcActor.top-sizeLen.cy)/2, 
		       m_rcActor.left + (m_rcActor.right-m_rcActor.left+sizeLen.cx)/2, m_rcActor.top + (m_rcActor.bottom-m_rcActor.top+sizeLen.cy)/2};	
	RECT rcLeft = {rc.left+1, rc.top-1, rc.right+1, rc.bottom-1};
	g_xMainWnd.PutsHan(NULL, rc, RGB(10, 10, 10), RGB(0, 0, 0), m_szName);
	g_xMainWnd.PutsHan(NULL, rcLeft, m_dwNameClr, RGB(0, 0, 0), m_szName);
}



VOID CActor::ChatMsgAdd()
{
	CHAR szDivied[MAX_PATH*2];
	ZeroMemory(szDivied, MAX_PATH*2);

	g_xMainWnd.StringDivide(_CHAT_WIDTH, m_nDividedChatLine, m_szChatMsg, szDivied);
	sscanf(szDivied, "%[^`]%*c %[^`]%*c %[^`]%*c %[^`]%*c %[^`]%*c", 
		   m_szChatMsgArg[0], m_szChatMsgArg[1], m_szChatMsgArg[2], m_szChatMsgArg[3], m_szChatMsgArg[4]);

	m_wCurrChatDelay = 0;
}


VOID CActor::ShowMessage(INT nLoopTime)
{
	if ( m_szChatMsg[0] != NULL )
	{
		m_wCurrChatDelay += nLoopTime;
	}

	if ( m_wCurrChatDelay > 3000 )
	{
		m_wCurrChatDelay = 0;
		ZeroMemory(m_szChatMsg, MAX_PATH);
		ZeroMemory(m_szChatMsgArg, MAX_PATH*5);
	}

	if ( m_szChatMsg[0] != NULL )
	{
		INT nStartX = m_shScrnPosX + 28 - _CHAT_WIDTH/2;
		INT nStartY;

/*		if ( m_stFeatureEx.bHorse == _HORSE_NONE )
			nStartY = m_shScrnPosY - 55;
		else
*/			nStartY = m_shScrnPosY - 55;


		RECT rcBack;
		if ( m_nDividedChatLine == 1 )
		{
			SIZE sizeLen;
			sizeLen = g_xMainWnd.GetStrLength(NULL, NULL, m_szChatMsgArg[0]);
			nStartX = m_shScrnPosX + 28 - sizeLen.cx/2;
			SetRect(&rcBack, nStartX, nStartY-14-4, nStartX+sizeLen.cx, nStartY);
		}
		else
			SetRect(&rcBack, nStartX, nStartY-m_nDividedChatLine*14-4, nStartX+_CHAT_WIDTH, nStartY);

		if ( g_xMainWnd.Get3DDevice() )
		{
			if( SUCCEEDED(g_xMainWnd.Get3DDevice()->BeginScene()) )
			{
				D3DVECTOR vTrans;
				D3DMATRIX matTrans;
				D3DMATRIX matScale;
				D3DMATRIX matWorld;
				D3DMATRIX matWorldOriginal;

				g_xMainWnd.Get3DDevice()->GetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

				D3DMATERIAL7 mtrl;

				vTrans.x = (FLOAT)nStartX-400+(rcBack.right-rcBack.left)/2;  
				vTrans.y = (FLOAT)-nStartY+300+(rcBack.bottom-rcBack.top)/2; 
				vTrans.z = 0;
				D3DUtil_SetTranslateMatrix(matTrans, vTrans);
				D3DUtil_SetScaleMatrix(matScale, (FLOAT)rcBack.right-rcBack.left, (FLOAT)rcBack.bottom-rcBack.top, 0.0f);
				D3DMath_MatrixMultiply(matWorld, matScale, matTrans);
				g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);				

				D3DUtil_InitMaterial(mtrl, 
									 (FLOAT)80/255.0f,
									 (FLOAT)60/255.0f, 
									 (FLOAT)40/255.0f);
				mtrl.diffuse.a = 120/255.0f;
				g_xMainWnd.Get3DDevice()->SetMaterial(&mtrl);

				g_xMainWnd.Get3DDevice()->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
				g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, TRUE);
				g_xMainWnd.Get3DDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				g_xMainWnd.Get3DDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE);
				g_xMainWnd.Get3DDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

				g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
				g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);

				g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_SRCBLEND, D3DBLEND_ONE);
				g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA);

				g_xMainWnd.Get3DDevice()->SetTexture(0, NULL);
				g_xMainWnd.Get3DDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX, m_avBoard, 4, NULL);

				// 원상복귀.
				ZeroMemory(&mtrl, sizeof(mtrl));
				mtrl.diffuse.r = mtrl.diffuse.g = mtrl.diffuse.b = 0.1f;
				mtrl.ambient.r = mtrl.ambient.g = mtrl.ambient.b = 1.0f;
				g_xMainWnd.Get3DDevice()->SetMaterial(&mtrl);

				g_xMainWnd.Get3DDevice()->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				g_xMainWnd.Get3DDevice()->SetTextureStageState(0, D3DTSS_COLOROP,   D3DTOP_MODULATE);
				g_xMainWnd.Get3DDevice()->SetTextureStageState(0, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
				g_xMainWnd.Get3DDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				g_xMainWnd.Get3DDevice()->SetTextureStageState(0, D3DTSS_ALPHAOP,   D3DTOP_SELECTARG1);
				g_xMainWnd.Get3DDevice()->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);

				g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);
				g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_COLORKEYENABLE, FALSE);
				g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_SRCBLEND , D3DBLEND_ONE);
				g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);
				g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

				g_xMainWnd.Get3DDevice()->EndScene();

			}
		}

		for ( INT nCnt = 0; nCnt < m_nDividedChatLine; nCnt++ )
		{
			SIZE sizeLen;
			sizeLen = g_xMainWnd.GetStrLength(NULL, NULL, m_szChatMsgArg[nCnt]);

			if ( m_nDividedChatLine != 1 )
			{
				if ( nCnt == m_nDividedChatLine-1 )
				{
					nStartX += (_CHAT_WIDTH-sizeLen.cx)/2;
				}
			}

			g_xMainWnd.PutsHan(g_xMainWnd.GetBackBuffer(),
							   nStartX, nStartY-(m_nDividedChatLine-nCnt)*14,
							   RGB(255,255,255), RGB(0,0,0), m_szChatMsgArg[nCnt]);
		}
	}
}












/******************************************************************************************************************

	CNPC Class

*******************************************************************************************************************/
/******************************************************************************************************************

	함수명 : CNPC::DrawActor()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : CMapHandler* pxMap
	         BOOL bFocused
	         BOOL bShadowAblended
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CNPC::DrawActor(CMapHandler* pxMap, BOOL bFocused, BOOL bShadowAblended, BOOL bUseScrnPos, BOOL bDrawShadow)
{
	// 좌표처리.
	m_shScrnPosX = (m_wPosX - pxMap->m_shStartViewTileX) * _CELL_WIDTH + _VIEW_CELL_X_START - pxMap->m_shViewOffsetX + m_shShiftPixelX;
	m_shScrnPosY = (m_wPosY - pxMap->m_shStartViewTileY) * _CELL_HEIGHT+ _VIEW_CELL_Y_START - pxMap->m_shViewOffsetY + m_shShiftPixelY;

	BYTE	bShadowType;
	SHORT	shShadowPX;
	SHORT	shShadowPY;

	if ( m_pxActorImage )
	{
		SHORT shPX, shPY;
		m_pxActorImage->NewSetIndex(m_dwCurrFrame);
		shPX = m_pxActorImage->m_lpstNewCurrWilImageInfo->shPX;
		shPY = m_pxActorImage->m_lpstNewCurrWilImageInfo->shPY;

		bShadowType = m_pxActorImage->m_lpstNewCurrWilImageInfo->bShadow;
		shShadowPX  = m_pxActorImage->m_lpstNewCurrWilImageInfo->shShadowPX;
		shShadowPY  = m_pxActorImage->m_lpstNewCurrWilImageInfo->shShadowPY;

		SetRect(&m_rcActor, m_shScrnPosX + shPX, m_shScrnPosY + shPY, 
				m_shScrnPosX + shPX + m_pxActorImage->m_lpstNewCurrWilImageInfo->shWidth, 
				m_shScrnPosY + shPY + m_pxActorImage->m_lpstNewCurrWilImageInfo->shHeight);

		if ( ( m_rcActor.right - m_rcActor.left ) > _CELL_WIDTH + _TARGETRGN_GAPX )
		{
			m_rcTargetRgn.left  = m_rcActor.left + ( (m_rcActor.right - m_rcActor.left) - (_CELL_WIDTH + _TARGETRGN_GAPX) )/2;
			m_rcTargetRgn.right = m_rcActor.left + ( (m_rcActor.right - m_rcActor.left) + (_CELL_WIDTH + _TARGETRGN_GAPX) )/2;
		}
		else
		{
			m_rcTargetRgn.left  = m_rcActor.left;
			m_rcTargetRgn.right = m_rcActor.right;
		}

		if ( ( m_rcActor.bottom - m_rcActor.top ) > _CELL_HEIGHT + _TARGETRGN_GAPY )
		{
			m_rcTargetRgn.top    = m_rcActor.top + ( (m_rcActor.bottom - m_rcActor.top) - (_CELL_HEIGHT + _TARGETRGN_GAPY) )/2;
			m_rcTargetRgn.bottom = m_rcActor.top + ( (m_rcActor.bottom - m_rcActor.top) + (_CELL_HEIGHT + _TARGETRGN_GAPY) )/2;
		}
		else
		{
			m_rcTargetRgn.top    = m_rcActor.top;
			m_rcTargetRgn.bottom = m_rcActor.bottom;
		}

		// 캐릭터 그림자.
		INT nStartX1 = m_shScrnPosX + shShadowPX;
		INT nStartY1 = m_shScrnPosY + shShadowPY;

		BYTE bOpaRate = 70;
		WORD wState   = GetCharState();

		// 그림자를 그린다.
		if ( m_wABlendDelay || wState==_STATE_ABLEND )
		{
			bShadowAblended = TRUE;
		}

		if ( bDrawShadow )
			g_xMainWnd.DrawWithShadowABlend(
									nStartX1, nStartY1,
									m_pxActorImage->m_lpstNewCurrWilImageInfo->shWidth,							
									m_pxActorImage->m_lpstNewCurrWilImageInfo->shHeight,
									(WORD*)m_pxActorImage->m_pbCurrImage,
									_CLIP_WIDTH, _CLIP_HEIGHT, g_xGameProc.m_wShadowClr, bShadowAblended, bShadowType, bOpaRate);

		if ( m_wABlendDelay )
		{
			if ( !m_bABlendRev )
				bOpaRate = 100 - ( (m_wABlendCurrDelay * 100) / m_wABlendDelay );
			else
				bOpaRate = ( (m_wABlendCurrDelay * 100) / m_wABlendDelay );
		}

		if ( bOpaRate < 0 && bOpaRate > 100 )		bOpaRate  = 0;

		if ( bFocused )								
		{
			if ( !m_bABlendRev )
				bOpaRate -= 20;
			else
				bOpaRate += 20;
		}

		DrawWithEffected(m_rcActor.left, m_rcActor.top, m_pxActorImage->m_lpstNewCurrWilImageInfo->shWidth, m_pxActorImage->m_lpstNewCurrWilImageInfo->shHeight, (WORD*)m_pxActorImage->m_pbCurrImage, 
			             0XFFFF, 0XFFFF, bFocused, bOpaRate, wState);

		return TRUE;
	}
	return FALSE;
}


VOID CNPC::OnTurn(LPPACKETMSG lpPacketMsg)
{
	BYTE bDir;
	m_wPosX = lpPacketMsg->stDefMsg.wParam;
	m_wPosY = lpPacketMsg->stDefMsg.wTag;
	bDir		 = LOBYTE(lpPacketMsg->stDefMsg.wSeries);
	m_bLightSize = HIBYTE(lpPacketMsg->stDefMsg.wSeries);
	OnCharDescPacket(lpPacketMsg);
	SetMotionFrame(_MT_NPC_STAND, bDir);
}


VOID CNPC::OnHit(LPPACKETMSG lpPacketMsg)
{
	BYTE bDir;
	bDir	= lpPacketMsg->stDefMsg.wSeries;
	m_wPosX = lpPacketMsg->stDefMsg.wParam;
	m_wPosY = lpPacketMsg->stDefMsg.wTag;
	SetMotionFrame(_MT_NPC_ACT01, bDir);
}


/******************************************************************************************************************

	함수명 : CNPC::UpdatePacketState()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CNPC::UpdatePacketState()
{
//	if ( m_bCurrMtn == _MT_NPC_STAND )
	{
		LPPACKETMSG	lpPacketMsg;

		SHORT shLeftMsgCnt = m_xPacketQueue.GetCount();

		if ( shLeftMsgCnt > 0 )
		{
			lpPacketMsg = (LPPACKETMSG)m_xPacketQueue.PopQ();

			if ( shLeftMsgCnt >= 3 )			m_bMsgHurryCheck = TRUE;
			else								m_bMsgHurryCheck = FALSE;

			if ( lpPacketMsg )
			{
				switch ( lpPacketMsg->stDefMsg.wIdent )
				{
					case SM_OPENHEALTH:			{	OnOpenHealth(lpPacketMsg);			break;	}
					case SM_CLOSEHEALTH:		{	OnCloseHealth(lpPacketMsg);			break;	}
					case SM_CHANGELIGHT:		{	OnChangeLight(lpPacketMsg);			break;	}
					case SM_USERNAME:			{	OnUserName(lpPacketMsg);			break;	}
					case SM_CHANGENAMECOLOR:	{	OnChangeNameClr(lpPacketMsg);		break;	}
					case SM_HEALTHSPELLCHANGED:	{	OnHealthSpellChanged(lpPacketMsg);	break;	}
					case SM_TURN:				{	OnTurn(lpPacketMsg);				break;	}
					case SM_HIT:				{	OnHit(lpPacketMsg);					break;	}
					default:
					{										
						break;	
					}
				}
			}
			SAFE_DELETE(lpPacketMsg);
			return TRUE;
		}
	}
	return FALSE;
}



/******************************************************************************************************************

	함수명 : CNPC::UpdateMotionState()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : INT nLoopTime
	         BOOL bIsMoveTime
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CNPC::UpdateMotionState(INT nLoopTime, BOOL bIsMoveTime)
{
	m_wCurrDelay += nLoopTime;

	if ( m_wCurrDelay > m_wDelay )
	{
		m_wCurrDelay = 0;

		if ( m_dwCurrFrame < m_dwEndFrame )			m_dwCurrFrame++;
	}

	UpdatePacketState();

	if ( m_dwCurrFrame >= m_dwEndFrame )			m_dwCurrFrame = m_dwFstFrame;
}









/******************************************************************************************************************

	CHero Class

*******************************************************************************************************************/
/******************************************************************************************************************

	함수명 : CHero::CHero()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
CHero::CHero()
{
	InitActor();
}



/******************************************************************************************************************

	함수명 : CHero::~CHero()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
CHero::~CHero()
{
	DestroyActor();
}


VOID CHero::InitActor()
{
	CActor::InitActor();

	ZeroMemory(&m_stFeatureEx, sizeof(FEATUREEX));

	m_bWeaponImgIdx = 0;
	m_bHairImgIdx	= 0;	
	m_bHorseImgIdx	= 0;	
	m_bYedoCnt		= 0;
	m_bFireHitCnt	= 0;
	m_shHitSpeed	= 0;

	m_pxHairImage	= NULL;
	m_pxWeaponImage	= NULL;
	m_pxHorseImage	= NULL;

	m_bUseBanwol	= FALSE;
	m_bUseErgum		= FALSE;
	m_bIsMon		= FALSE;

	m_bShieldCurrFrm	= 0;
	m_wShieldCurrDelay	= 0;
	m_dwCurrHairFrame	= 0;
	m_dwCurrWeaponFrame	= 0;
	m_dwCurrHorseFrame	= 0;
	m_shCurrMagicID		= 0;

	ZeroMemory(&m_rcHair, sizeof(RECT));
	ZeroMemory(&m_rcWeapon, sizeof(RECT));
	ZeroMemory(&m_rcHorse, sizeof(RECT));
}


VOID CHero::DestroyActor()
{
	CActor::DestroyActor();
	InitActor();
}




/******************************************************************************************************************

	함수명 : CHero::Create()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : CImageHandler* pxImgHandler
	         WORD wActor
	         BYTE bMtn
	         WORD bDir
	         WORD wPosX
	         WORD wPosY
	         FEATURE stFeature
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CHero::Create(CImageHandler* pxImgHandler, BYTE bMtn, BYTE bDir, WORD wPosX, WORD wPosY, 
				   FEATURE* pstFeature, FEATUREEX* pstFeatureEx)
{
	if ( CActor::Create(pxImgHandler, pstFeature, bMtn, bDir, wPosX, wPosY) )
	{
		m_bIsMon		= FALSE;
		
//		memcpy(&m_stFeatureEx, pstFeatureEx, sizeof(FEATUREEX));

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
		m_stFeatureEx.bHorse = 0;
		m_stFeatureEx.wDressColor = 0XFF;
		m_stFeatureEx.wHairColor = 0XFF;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!


		if ( m_stFeature.bHair	  >= _MAX_HAIR )	m_stFeature.bHair	= _HAIR_NONE;
		if ( m_stFeature.bWeapon  >= _MAX_WEAPON )	m_stFeature.bWeapon	= _WEAPON_NONE;
		if ( m_stFeatureEx.bHorse >= _MAX_HORSE )	m_stFeatureEx.bHorse= _HORSE_NONE;

		m_bHorseImgIdx	= _IMAGE_HORSE;
		if ( m_stFeature.bGender == _GENDER_MAN )
		{
			m_bHairImgIdx	= _IMAGE_M_HAIR;

			if ( m_stFeature.bWeapon != _WEAPON_NONE )			m_bWeaponImgIdx	= _IMAGE_M_WEAPON1+(m_stFeature.bWeapon-1)/10;
		}
		else if ( m_stFeature.bGender == _GENDER_WOMAN )
		{
			m_bHairImgIdx	= _IMAGE_WM_HAIR;
			if ( m_stFeature.bWeapon != _WEAPON_NONE )			m_bWeaponImgIdx	= _IMAGE_WM_WEAPON1+(m_stFeature.bWeapon-1)/10;
		}
		else			return FALSE;

		if ( m_stFeature.bHair != _HAIR_NONE )		m_pxHairImage	= &pxImgHandler->m_xImageList[m_bHairImgIdx];
		else										m_pxHairImage	= NULL;

		if ( m_stFeature.bWeapon != _WEAPON_NONE )	m_pxWeaponImage	= &pxImgHandler->m_xImageList[m_bWeaponImgIdx];
		else										m_pxWeaponImage	= NULL;

		if ( m_stFeatureEx.bHorse != _HORSE_NONE )	m_pxHorseImage	= &pxImgHandler->m_xImageList[m_bHorseImgIdx];
		else										m_pxHorseImage	= NULL;
  
		return TRUE;
	}
	return FALSE;
}



VOID CHero::PlayActSound()
{
	INT nWaveNum = -1;

	if ( m_dwCurrFrame == m_dwFstFrame+1 && m_bCurrMtn == _MT_HITTED )
	{
		switch ( m_stHitter.bWeapon )
		{
			case 21: case 24: // 단검, 비단검.
			case 8: case 9: // 목검, 아리수목검.
			case 18:  case 22:  case 23:  case 26:  case 27:  case 28:  case 30:  // 사모검. 청동검. 철검. 청음검. 벽사검. 천령. 곡성검.
			case 1:   case 4:  case 11:  case 13:  case 14:  case 20:  case 25:  case 29:  case 31:   // 유월도. 묵청대도. 육합도. 군도. 도룡보도. 사각도. 세첨도. 예도. 초혼도.
				nWaveNum = 70;
				break;
			case 5:  case 10:  case 12: // 삼적대부. 청동도끼. 연자부.
			case 15:  // 파뇌진당.
				nWaveNum = 71;
				break;
			case 2:  case 3:  case 6:  case 7: case 16:  case 17: case 19:  // 삼지창. 천형목. 홍아창. 곡괭이. 청마창, 용아장. 제마봉
				nWaveNum = 72;
				break;
			default:
				nWaveNum = 73;	// 맨손.
				break;
		}
		if ( m_stFeature.bDress == 3 )		// 미갑주.
			nWaveNum += 10;
	}
	else if ( m_dwCurrFrame == m_dwFstFrame+2 && m_bCurrMtn == _MT_HITTED )
	{
		if ( m_stFeature.bGender == _GENDER_MAN )	nWaveNum = 138;
		else										nWaveNum = 139;
	}
	else if ( m_dwCurrFrame == m_dwFstFrame+1 && m_bCurrMtn == _MT_DIE )
	{
		if ( m_stFeature.bGender == _GENDER_MAN )	nWaveNum = 144;
		else										nWaveNum = 145;

	}
	else if ( m_dwCurrFrame == m_dwFstFrame+1 && m_bCurrMtn >= _MT_ONEVSWING && m_bCurrMtn <= _MT_SPEARHSWING )
	{
		if ( m_bFireHitCnt == 2 )
		{
			nWaveNum = 137;
		}
		else if ( m_bYedoCnt == 2 )
		{
/*			if ( m_stFeature.bGender == _GENDER_MAN )
			{
				g_xSound.PlayActorWav(m_wPosX, m_wPosY, g_xGameProc.m_xMyHero.m_wPosX, g_xGameProc.m_xMyHero.m_wPosY, 130);
			}
			else
			{
				g_xSound.PlayActorWav(m_wPosX, m_wPosY, g_xGameProc.m_xMyHero.m_wPosX, g_xGameProc.m_xMyHero.m_wPosY, 131);
			}
*/			m_bYedoCnt = 0;
		}
		else if ( m_bUseBanwol )
		{
			nWaveNum = 133;
		}
		else if ( m_bUseErgum )
		{
			nWaveNum = 132;
		}
	}
	else if ( m_dwCurrFrame == m_dwFstFrame+2 && m_bCurrMtn >= _MT_ONEVSWING && m_bCurrMtn <= _MT_SPEARHSWING )
	{
		switch ( m_stFeature.bWeapon )
		{
		case 21: case 24: // 단검, 비단검.
			nWaveNum = 50;
			break;
		case 8: case 9: // 목검, 아리수목검.
			nWaveNum = 51;
			break;
		case 18:  case 22:  case 23:  case 26:  case 27:  case 28:  case 30:  // 사모검. 청동검. 철검. 청음검. 벽사검. 천령. 곡성검.
			nWaveNum = 52;
			break;
		case 1:   case 4:  case 11:  case 13:  case 14:  case 20:  case 25:  case 29:  case 31:   // 유월도. 묵청대도. 육합도. 군도. 도룡보도. 사각도. 세첨도. 예도. 초혼도.
			nWaveNum = 53;
			break;
		case 5:  case 10:  case 12: // 삼적대부. 청동도끼. 연자부.
			nWaveNum = 54;
			break;
		case 15:  // 파뇌진당.
			nWaveNum = 55;
			break;
		case 2:  case 3:  case 6:  case 7: case 16:  case 17: case 19:  // 삼지창. 천형목. 홍아창. 곡괭이. 청마창, 용아장. 제마봉
			nWaveNum = 56;
			break;
		default:
			nWaveNum = 57;	// 맨손.
			break;
		}
	}
	else if ( m_dwCurrFrame == m_dwFstFrame+1 && (m_bCurrMtn == _MT_SPELL1 || m_bCurrMtn == _MT_SPELL2) )
	{
		const INT nMagicSndTbl[31] = 
		{
			 0,  6,  8, 14, 15,  9,  0, 20, 21, 32,
			 1,  2, 16, 29, 24, 10, 17, 33, 19, 31,
			28, 22, 13, 21,  5,  0, 12, 11,  0, 18, 
			30,
		};
		if ( m_shCurrMagicID < 31 )
		{
			nWaveNum = nMagicSndTbl[m_shCurrMagicID]*10 + 10000;
		}
		if ( nWaveNum < 10010 )
			nWaveNum = -1;
	}

/*	if ( nWaveNum != -1 )
		g_xSound.PlayActorWav(m_wPosX, m_wPosY, g_xGameProc.m_xMyHero.m_wPosX, g_xGameProc.m_xMyHero.m_wPosY, nWaveNum);
*/}


VOID CHero::PlayMoveSound()
{
	if ( m_bCurrMtn == _MT_WALK || m_bCurrMtn == _MT_RUN || m_bCurrMtn == _MT_HORSEWALK || m_bCurrMtn == _MT_HORSERUN || m_bCurrMtn == _MT_MOODEPO || m_bCurrMtn == _MT_PUSHBACK )
	{
/*		if ( m_dwCurrFrame == m_dwEndFrame-5 || m_bBackStepFrame == m_dwEndFrame-5 )
			g_xSound.PlayActorWav(m_wPosX, m_wPosY, g_xGameProc.m_xMyHero.m_wPosX, g_xGameProc.m_xMyHero.m_wPosY, 1,100);
		if ( m_dwCurrFrame == m_dwEndFrame-2 || m_bBackStepFrame == m_bBackStepFrameCnt-2 )
			g_xSound.PlayActorWav(m_wPosX, m_wPosY, g_xGameProc.m_xMyHero.m_wPosX, g_xGameProc.m_xMyHero.m_wPosY, 1,100);
*/	}
}


VOID CHero::ShowShield()
{
	if ( m_nState & 0X100000 )
	{
		WORD wShieldFrm;
		if ( m_bCurrMtn == _MT_HITTED )			wShieldFrm = m_bShieldCurrFrm + 853;
		else									wShieldFrm = m_bShieldCurrFrm + 850;

		if ( g_xMainWnd.Get3DDevice() )
		{
			if( SUCCEEDED(g_xMainWnd.Get3DDevice()->BeginScene()) )
			{
				D3DVECTOR vTrans;
				D3DMATRIX matTrans;
				D3DMATRIX matScale;
				D3DMATRIX matRot;
				D3DMATRIX matWorld;
				D3DMATRIX matWorldOriginal;

				g_xMainWnd.Get3DDevice()->GetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

				D3DMATERIAL7 mtrl;
				CWHWilImageData* pxWilImg;

				pxWilImg = &g_xGameProc.m_xImage.m_xImageList[_IMAGE_MAGIC];

				if ( pxWilImg->NewSetIndex(wShieldFrm) )
				{
//					if ( !D3DWILTextr_GetSurface(pxWilImg->m_szWilFileName, wShieldFrm) )
//						D3DWILTextr_Restore(pxWilImg->m_szWilFileName, wShieldFrm, g_xMainWnd.Get3DDevice());

 					vTrans.x = (FLOAT)m_shScrnPosX+(FLOAT)pxWilImg->m_lpstNewCurrWilImageInfo->shWidth/2+pxWilImg->m_lpstNewCurrWilImageInfo->shPX-400; 
					vTrans.y = (FLOAT)-m_shScrnPosY-(FLOAT)pxWilImg->m_lpstNewCurrWilImageInfo->shHeight/2-pxWilImg->m_lpstNewCurrWilImageInfo->shPY+300; 
					vTrans.z = 0;
					D3DUtil_SetTranslateMatrix(matTrans, vTrans);
					D3DUtil_SetScaleMatrix(matScale, (FLOAT)pxWilImg->m_lpstNewCurrWilImageInfo->shWidth, (FLOAT)pxWilImg->m_lpstNewCurrWilImageInfo->shHeight, 0.0f);
					D3DMath_MatrixMultiply(matWorld, matScale, matTrans);
					g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);

//					g_xMainWnd.Get3DDevice()->SetTexture(0, D3DWILTextr_GetSurface(pxWilImg->m_szWilFileName, wShieldFrm));
					g_xGameProc.m_xImage.AddTextr(_TEXTR_FILE_MAGIC,_IMAGE_MAGIC, wShieldFrm);
					LPDIRECTDRAWSURFACE7 lpddsTextr = g_xGameProc.m_xImage.GetTextrImg(_TEXTR_FILE_MAGIC, _IMAGE_MAGIC, wShieldFrm);
					g_xMainWnd.Get3DDevice()->SetTexture(0, lpddsTextr);
					D3DUtil_InitMaterial(mtrl, 
										 (FLOAT)255/255.0f,
										 (FLOAT)255/255.0f, 
										 (FLOAT)255/255.0f);
					mtrl.diffuse.a = 0/255.0f;
					g_xMainWnd.Get3DDevice()->SetMaterial(&mtrl);

					SetBlendRenderState(g_xMainWnd.Get3DDevice(), _BLEND_LIGHTINV, mtrl);

					g_xMainWnd.Get3DDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX, m_avBoard, 4, NULL);

					// 원상복귀.
					ZeroMemory(&mtrl, sizeof(mtrl));
					mtrl.diffuse.r = mtrl.diffuse.g = mtrl.diffuse.b = 0.1f;
					mtrl.ambient.r = mtrl.ambient.g = mtrl.ambient.b = 1.0f;
					g_xMainWnd.Get3DDevice()->SetMaterial(&mtrl);

					ResetBlendenderState(g_xMainWnd.Get3DDevice());
					g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);
				}
				
				g_xMainWnd.Get3DDevice()->EndScene();
			}
		}
	}
}



/******************************************************************************************************************

	함수명 : CHero::UpdateMove(BOOL bIsMoveTime)

	작성자 : 
	작성일 : 

	목적   : 
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CHero::UpdateMove(BOOL bIsMoveTime)
{
	if ( m_bCurrMtn == _MT_WALK || m_bCurrMtn == _MT_RUN || m_bCurrMtn == _MT_HORSEWALK || m_bCurrMtn == _MT_HORSERUN || m_bCurrMtn == _MT_MOODEPO || m_bCurrMtn == _MT_PUSHBACK )
	{
		m_wCurrDelay = 0;

		if ( bIsMoveTime )
		{
			if ( m_bCurrMtn == _MT_PUSHBACK )
			{
				m_bBackStepFrame += 2;
				if ( m_bBackStepFrame >= m_bBackStepFrameCnt )
				{
					m_dwCurrFrame++;
					m_bBackStepFrame = m_bBackStepFrameCnt;
				}
			}
			else if ( m_bCurrMtn == _MT_MOODEPO )
			{
				m_dwCurrFrame+=2;
				if ( m_dwCurrFrame >= m_dwEndFrame )		m_dwCurrFrame = m_dwEndFrame;
			}
			else
			{
				m_dwCurrFrame++;
				PlayMoveSound();
				if ( m_bMsgHurryCheck )			
				{
					m_dwCurrFrame++;
				}
			}

			if ( m_bCurrMtn == _MT_PUSHBACK )
			{
				if ( m_bBackStepFrame >= m_bBackStepFrameCnt-m_bMoveNextFrmCnt && !m_bIsMoved )
				{
					SetMoved();
					m_bIsMoved = TRUE;
				}
			}
			else
			{
				if ( m_dwCurrFrame >= m_dwEndFrame-m_bMoveNextFrmCnt && !m_bIsMoved )
				{
					SetMoved();
					m_bIsMoved = TRUE;
				}
			}
		}

		// 연속적인 프레임 중에서 해야할일.
		if ( m_dwCurrFrame >= m_dwEndFrame )
		{
			m_dwCurrFrame = m_dwEndFrame - 1;

			m_shShiftTileX = 0;
			m_shShiftTileY = 0;
			m_shShiftPixelX = 0;
			m_shShiftPixelY = 0;

			m_dwCurrEffectFrame = 0;
			m_dwFstEffectFrame = 0;
			m_dwEndEffectFrame = 0;
			m_bEffectFrame = 0;
			m_bEffectFrameCnt = _DEFAULT_SPELLFRAME;
			m_bUseEffect = FALSE;
			m_bUseSwordEffect = FALSE;
			m_dwCurrFrame = m_dwFstFrame;

			m_bBackStepFrame = 0;
			m_bBackStepFrameCnt = 0;

			m_bIsMoved = FALSE;

			if ( m_stFeatureEx.bHorse == _HORSE_NONE )
			{
				if ( m_bWarMode )
					SetMotionFrame(_MT_ATTACKMODE, m_bCurrDir);
				else
					SetMotionFrame(_MT_STAND, m_bCurrDir);
			}
			else
			{
				SetMotionFrame(_MT_HORSESTAND, m_bCurrDir);
			}
		}
		else if ( m_dwCurrFrame < m_dwEndFrame )
		{
			if ( m_bCurrMtn != _MT_PUSHBACK )
				SetMoving();
			else
				SetBackStepMoving();
		}
		return TRUE;
	}
	return FALSE;
}


VOID CHero::OnCharDescPacket(LPPACKETMSG lpPacketMsg)
{
	CHARDESC	stCharDesc;
	FEATURE		stFeature;
	fnDecode6BitBuf(lpPacketMsg->szEncodeData, (char*)&stCharDesc, sizeof(CHARDESC));

	memcpy(&stFeature, &stCharDesc.nFeature, sizeof(FEATURE));
	m_nState = stCharDesc.nStatus;

	if( m_nState & 0X2 )	m_bOpenHealth = TRUE;
	else					m_bOpenHealth = FALSE;

	ChangeFeature(stFeature, m_stFeatureEx);
}

VOID CHero::OnFeatureChanged(LPPACKETMSG lpPacketMsg)
{
	FEATURE stFeature;
	LONG nFeature	  = MAKELONG(lpPacketMsg->stDefMsg.wParam, lpPacketMsg->stDefMsg.wTag);

	memcpy(&stFeature, &nFeature, sizeof(LONG));

	ChangeFeature(stFeature, m_stFeatureEx);
}

VOID CHero::OnCharStatusChanged(LPPACKETMSG lpPacketMsg)
{
	m_shHitSpeed = lpPacketMsg->stDefMsg.wSeries;
	m_nState = MAKELONG(lpPacketMsg->stDefMsg.wParam, lpPacketMsg->stDefMsg.wTag);

	if( m_nState & 0X2 )	m_bOpenHealth = TRUE;
	else					m_bOpenHealth = FALSE;
}

VOID CHero::OnRush(LPPACKETMSG lpPacketMsg)
{
	BYTE bDir = lpPacketMsg->stDefMsg.wSeries;
	m_wPosX   = lpPacketMsg->stDefMsg.wParam;
	m_wPosY   = lpPacketMsg->stDefMsg.wTag;
	SetMotionFrame(_MT_MOODEPO, bDir);
	m_bMoveSpeed = _SPEED_WALK;
	SetMoving();
}

VOID CHero::OnBackStep(LPPACKETMSG lpPacketMsg)
{
	BYTE bDir = lpPacketMsg->stDefMsg.wSeries;
	m_wPosX   = lpPacketMsg->stDefMsg.wParam;
	m_wPosY   = lpPacketMsg->stDefMsg.wTag;
	SetMotionFrame(_MT_PUSHBACK, bDir);
	if ( m_bCurrDir < 4 )	m_bMoveDir = m_bCurrDir + 4;
	else					m_bMoveDir = m_bCurrDir - 4;
	m_bBackStepFrame	= 0;
	m_bBackStepFrameCnt = 6;
	m_bMoveSpeed		= _SPEED_WALK;
	SetBackStepMoving();
}

VOID CHero::OnDeath(LPPACKETMSG lpPacketMsg)
{
	BYTE bDir;
	m_wPosX = lpPacketMsg->stDefMsg.wParam;
	m_wPosY = lpPacketMsg->stDefMsg.wTag;
	bDir	= lpPacketMsg->stDefMsg.wSeries;
	OnCharDescPacket(lpPacketMsg);
	SetMotionFrame(_MT_DIE, bDir);
	if ( lpPacketMsg->stDefMsg.wIdent == SM_DEATH )	
	{
		m_dwCurrFrame = m_dwEndFrame - 1;
		m_bIsDead = TRUE;
	}
}

VOID CHero::OnWalk(LPPACKETMSG lpPacketMsg)
{
	BYTE		bDir;
	CHARDESC	stCharDesc;
	FEATURE		stFeature;
//	FEATUREEX	stFeatureEx;
//	m_wPosX = lpPacketMsg->stDefMsg.wParam;
//	m_wPosY = lpPacketMsg->stDefMsg.wTag;
	bDir		 = LOBYTE(lpPacketMsg->stDefMsg.wSeries);
	m_bLightSize = HIBYTE(lpPacketMsg->stDefMsg.wSeries);

	fnDecode6BitBuf(lpPacketMsg->szEncodeData, (char*)&stCharDesc, sizeof(CHARDESC));
//	fnDecode6BitBuf(&lpPacketMsg->szEncodeData[_FEATURESIZE*2], (char*)&stFeatureEx, sizeof(FEATUREEX));

	memcpy(&stFeature, &stCharDesc.nFeature, sizeof(FEATURE));
	m_nState = stCharDesc.nStatus;

	if( m_nState & 0X2 )	m_bOpenHealth = TRUE;
	else					m_bOpenHealth = FALSE;

	ChangeFeature(stFeature, m_stFeatureEx);

	if ( m_stFeatureEx.bHorse == _HORSE_NONE )		SetMotionFrame(_MT_WALK, bDir);
	else											SetMotionFrame(_MT_HORSEWALK, bDir);
						
	m_bMoveSpeed = _SPEED_WALK;
	SetMoving();
}

VOID CHero::OnRun(LPPACKETMSG lpPacketMsg)
{
	BYTE		bDir;
	CHARDESC	stCharDesc;
	FEATURE		stFeature;
//	FEATUREEX	stFeatureEx;
//	m_wPosX = lpPacketMsg->stDefMsg.wParam;
//	m_wPosY = lpPacketMsg->stDefMsg.wTag;
	bDir		 = LOBYTE(lpPacketMsg->stDefMsg.wSeries);
	m_bLightSize = HIBYTE(lpPacketMsg->stDefMsg.wSeries);

	fnDecode6BitBuf(lpPacketMsg->szEncodeData, (char*)&stCharDesc, sizeof(CHARDESC));
//	fnDecode6BitBuf(&lpPacketMsg->szEncodeData[_CHARDESCSIZE], (char*)&stFeatureEx, sizeof(FEATUREEX));

	memcpy(&stFeature, &stCharDesc.nFeature, sizeof(FEATURE));
	m_nState = stCharDesc.nStatus;

	if( m_nState & 0X2 )	m_bOpenHealth = TRUE;
	else					m_bOpenHealth = FALSE;

	ChangeFeature(stFeature, m_stFeatureEx);

	if ( m_stFeatureEx.bHorse == _HORSE_NONE )	
	{
		SetMotionFrame(_MT_RUN, bDir);
		m_bMoveSpeed = _SPEED_RUN;
	}
	else
	{
		SetMotionFrame(_MT_HORSERUN, bDir);
		m_bMoveSpeed = _SPEED_HORSERUN;
	}

	SetMoving();
}


VOID CHero::OnTurn(LPPACKETMSG lpPacketMsg)
{
	BYTE		bDir;
	CHARDESC	stCharDesc;
	FEATURE		stFeature;
	INT			nPos;
	m_wPosX = lpPacketMsg->stDefMsg.wParam;
	m_wPosY = lpPacketMsg->stDefMsg.wTag;
	bDir		 = LOBYTE(lpPacketMsg->stDefMsg.wSeries);
	m_bLightSize = HIBYTE(lpPacketMsg->stDefMsg.wSeries);

	fnDecode6BitBuf(lpPacketMsg->szEncodeData, (char*)&stCharDesc, sizeof(CHARDESC));
	fnDecode6BitBuf(&lpPacketMsg->szEncodeData[_CHARDESCSIZE], (char*)&m_stFeatureEx , sizeof(FEATUREEX));

	if (strlen(lpPacketMsg->szEncodeData) > _CHARDESCSIZE)
	{
		nPos = fnDecode6BitBuf(&lpPacketMsg->szEncodeData[_CHARDESCSIZE +_FEATURESIZEEX], m_szName, sizeof(m_szName));
		m_szName[nPos] = '\0';

		CHAR* szClr = strrchr(m_szName, '/');

		if ( szClr )
		{
			m_dwNameClr = GetUserNameColor(atoi(szClr+1));
			*szClr = '\0';
		}
	}

	memcpy(&stFeature, &stCharDesc.nFeature, sizeof(FEATURE));
	m_nState = stCharDesc.nStatus;

	if( m_nState & 0X2 )	m_bOpenHealth = TRUE;
	else					m_bOpenHealth = FALSE;

	if ( !m_bIsMon )
	{
		ChangeFeature(stFeature, m_stFeatureEx);

		if ( m_stFeatureEx.bHorse == _HORSE_NONE )		SetMotionFrame(_MT_STAND, bDir);
		else											SetMotionFrame(_MT_HORSESTAND, bDir);
	}
	else
	{
		SetMotionFrame(_MT_MON_STAND, bDir);
	}
}

VOID CHero::OnStruck(LPPACKETMSG lpPacketMsg)
{
	MESSAGEBODYWL stMsgBodyWl;
	FEATURE		  stFeature;

	FLOAT wHPRate	 = (FLOAT)((FLOAT)lpPacketMsg->stDefMsg.wParam/(FLOAT)lpPacketMsg->stDefMsg.wTag);
	WORD  wDamage	 = lpPacketMsg->stDefMsg.wSeries;
	m_wHP			 = lpPacketMsg->stDefMsg.wParam;
	m_wMAXHP		 = lpPacketMsg->stDefMsg.wTag;
	m_bHPPercent     = wHPRate*100;
	m_bShieldCurrFrm = 0;

	fnDecode6BitBuf(lpPacketMsg->szEncodeData, (char*)&stMsgBodyWl, sizeof(MESSAGEBODYWL));
	memcpy(&stFeature, &stMsgBodyWl.lParam1, sizeof(LONG));

	if ( !m_bIsMon )
	{
		ChangeFeature(stFeature, m_stFeatureEx);

		if ( m_stFeatureEx.bHorse == _HORSE_NONE )		SetMotionFrame(_MT_HITTED, m_bCurrDir);
		else											SetMotionFrame(_MT_HORSEHIT, m_bCurrDir);
	}
	else
	{
		SetMotionFrame(_MT_MON_HITTED, m_bCurrDir);
	}

	// 몬스터일때.
	if ( m_stHitter.bGender == _GENDER_MON )
	{
		CMagic* pxMagic;
		switch ( m_stHitter.bDress )
		{
		case 2:		// 케팔로프.
			pxMagic = new CMagic;
			pxMagic->CreateMagic(_MONMAGIC_KEPAL, m_wPosX, m_wPosY, m_wPosX, m_wPosY, this, 0);
			g_xGameProc.m_xMagicList.AddNode(pxMagic);
			break;
		case 8:		// 갑주개미.
			pxMagic = new CMagic;
			pxMagic->CreateMagic(_MONMAGIC_GREATANT, m_wPosX, m_wPosY, m_wPosX, m_wPosY, this);
			g_xGameProc.m_xMagicList.AddNode(pxMagic);
			break;
		case 14:	// 병용개미.
			pxMagic = new CMagic;
			pxMagic->CreateMagic(_MONMAGIC_ANT, m_wPosX, m_wPosY, m_wPosX, m_wPosY, this);
			g_xGameProc.m_xMagicList.AddNode(pxMagic);
			break;
		case 16:	// 드난개미.
			pxMagic = new CMagic;
			pxMagic->CreateMagic(_MONMAGIC_WORKANT, m_wPosX, m_wPosY, m_wPosX, m_wPosY, this);
			g_xGameProc.m_xMagicList.AddNode(pxMagic);
			break;
		case 67:	// 촉룡신.
			pxMagic = new CMagic;
			pxMagic->CreateMagic(_MONMAGIC_BIGGINE_CHAR, m_wPosX, m_wPosY, m_wPosX, m_wPosY, this);
			g_xGameProc.m_xMagicList.AddNode(pxMagic);
			break;
		default :
			break;
		}
	}
	ZeroMemory(&m_stHitter, sizeof(FEATURE));
}

VOID CHero::OnHit(LPPACKETMSG lpPacketMsg)
{
	BYTE bDir;
	bDir	= lpPacketMsg->stDefMsg.wSeries;
	m_wPosX = lpPacketMsg->stDefMsg.wParam;
	m_wPosY = lpPacketMsg->stDefMsg.wTag;

	if ( !m_bIsMon )
	{
		m_dwCurrEffectFrame = 50;
		m_dwFstEffectFrame  = 50;
		m_dwEndEffectFrame  = 60;
		m_bEffectFrame		= 0;
		m_bEffectFrameCnt	= _DEFAULT_SPELLFRAME;
		m_bWarMode			= TRUE;

		if ( lpPacketMsg->stDefMsg.wIdent == SM_WIDEHIT )
		{
			SetMotionFrame(_MT_ONEHSWING, bDir);
			LoadEffect(&g_xGameProc.m_xImage, _SKILL_BANWOL, bDir);
			m_bUseSwordEffect = TRUE;
		}
		else if ( lpPacketMsg->stDefMsg.wIdent == SM_POWERHIT )
		{
			SetMotionFrame(_MT_ONEVSWING, bDir);
			LoadEffect(&g_xGameProc.m_xImage, _SKILL_YEDO, bDir);
			m_bUseSwordEffect = TRUE;
		}
		else if ( lpPacketMsg->stDefMsg.wIdent == SM_FIREHIT )
		{
			SetMotionFrame(_MT_ONEVSWING, bDir);
			LoadEffect(&g_xGameProc.m_xImage, _SKILL_FIRESWORD, bDir);
			m_bUseSwordEffect = TRUE;
		}
		else if ( lpPacketMsg->stDefMsg.wIdent == SM_LONGHIT )
		{
			SetMotionFrame(_MT_ONEVSWING, bDir);
			LoadEffect(&g_xGameProc.m_xImage, _SKILL_ERGUM, bDir);
			m_bUseSwordEffect = TRUE;
		}
		else
		{
			bDir = LOBYTE(lpPacketMsg->stDefMsg.wSeries);
			BYTE bAttackMode = HIBYTE(lpPacketMsg->stDefMsg.wSeries);
			SetMotionFrame(_MT_ONEVSWING, bDir);
		}
	}
}

VOID CHero::OnMagicFire(LPPACKETMSG lpPacketMsg)
{
	POINT	ptPos;
	BYTE	bMagicID;
	INT		nMagicTargetID, nPos;

	ptPos.x	 = lpPacketMsg->stDefMsg.wParam;
	ptPos.y  = lpPacketMsg->stDefMsg.wTag;
	bMagicID = lpPacketMsg->stDefMsg.wSeries;

	nPos = fnDecode6BitBuf(lpPacketMsg->szEncodeData, (CHAR*)&nMagicTargetID, sizeof(INT));

	if ( bMagicID == _SKILL_SKELLETON )
	{
		return;
	}

	if ( bMagicID == _SKILL_SHOOTLIGHTEN )
	{
		CElecMagic* pxElecMagic;
		pxElecMagic = new CElecMagic;
		pxElecMagic->CreateMagic(bMagicID, m_wPosX, m_wPosY, ptPos.x, ptPos.y, NULL, nMagicTargetID);
		g_xGameProc.m_xMagicList.AddNode(pxElecMagic);
	}
//	if ( bMagicID == _SKILL_SHOOTLIGHTEN || bMagicID == _SKILL_FIRE )
	else if ( bMagicID == _SKILL_FIRE )
	{
		BYTE  bDir16	 = (BYTE)g_xGameProc.m_xMap.CalcDirection8(m_wPosX, m_wPosY, ptPos.x, ptPos.y);
		POINT ptTileGap  = {0, 0};
		POINT ptTileDest = {0, 0};
		BYTE  bLoopCnt	 = 0;
		BYTE  bDelay     = 0;

		if ( bMagicID == _SKILL_SHOOTLIGHTEN )			{	bLoopCnt = 15;	bDelay =  70; }
		else											{	bLoopCnt =  5;	bDelay = 150; }

		switch ( bDir16 )
		{
		case 0 :	ptTileGap.x =  0; ptTileGap.y = -1; break;
		case 1 :	ptTileGap.x =  1; ptTileGap.y = -1; break;
		case 2 :	ptTileGap.x =  1; ptTileGap.y =  0; break;
		case 3 :	ptTileGap.x =  1; ptTileGap.y =  1; break;
		case 4 :	ptTileGap.x =  0; ptTileGap.y =  1; break;
		case 5 :	ptTileGap.x = -1; ptTileGap.y =  1; break;
		case 6 :	ptTileGap.x = -1; ptTileGap.y =  0; break;
		case 7 :	ptTileGap.x = -1; ptTileGap.y = -1; break;
		}

		for ( INT nCnt = 0; nCnt < bLoopCnt; nCnt++ )
		{
			CMagicStream* pxMagicStream;
			pxMagicStream = new CMagicStream;
			ptTileDest.x = m_wPosX + ptTileGap.x*(nCnt+1);
			ptTileDest.y = m_wPosY + ptTileGap.y*(nCnt+1);

			pxMagicStream->CreateMagic(bMagicID, m_wPosX, m_wPosY, ptTileDest.x, ptTileDest.y, NULL, nMagicTargetID, nCnt*bDelay, ptTileGap);

			if ( bMagicID == _SKILL_SHOOTLIGHTEN )
				g_xGameProc.m_xMagicList.AddNode(pxMagicStream);
			else
				g_xGameProc.m_xGroundMagicList.AddNode(pxMagicStream);
		}
	}
	else if ( bMagicID == _SKILL_SHIELD )
	{
		CMagic* pxMagic;
		pxMagic = new CMagic;
		pxMagic->CreateMagic(bMagicID, m_wPosX, m_wPosY, m_wPosX, m_wPosY, NULL, m_dwIdentity);
		g_xGameProc.m_xMagicList.AddNode(pxMagic);
	}
	else if ( bMagicID != _SKILL_EARTHFIRE &&  bMagicID != _SKILL_HOLYSHIELD )
	{
		CMagic* pxMagic;
		pxMagic = new CMagic;
		pxMagic->CreateMagic(bMagicID, m_wPosX, m_wPosY, ptPos.x, ptPos.y, NULL, nMagicTargetID);
		g_xGameProc.m_xMagicList.AddNode(pxMagic);						
	}

	m_shCurrMagicID = bMagicID;
}

VOID CHero::OnSpell(LPPACKETMSG lpPacketMsg)
{
	BYTE  bMagicDir; 
	INT	  nMagicID;
	POINT ptTargetPos;

	ptTargetPos.x = lpPacketMsg->stDefMsg.wParam;
	ptTargetPos.y = lpPacketMsg->stDefMsg.wTag;
//	bDir		  = lpPacketMsg->stDefMsg.wSeries;
//	nPos		  = fnDecode6BitBuf(lpPacketMsg->szEncodeData, szDecodeMsg, sizeof(szDecodeMsg));
//	szDecodeMsg[nPos] = '\0';

	bMagicDir = g_xGameProc.m_xMap.CalcDirection8(m_wPosX, m_wPosY, ptTargetPos.x, ptTargetPos.y);
	nMagicID  = atoi(lpPacketMsg->szEncodeData);

	if ( nMagicID == _SKILL_FIREBALL     || nMagicID == _SKILL_FIREBALL2 || nMagicID == _SKILL_FIRE      || nMagicID == _SKILL_SHOOTLIGHTEN ||
		 nMagicID == _SKILL_HANGMAJINBUB || nMagicID== _SKILL_DEJIWONHO  || nMagicID == _SKILL_FIRECHARM || nMagicID == _SKILL_BIGCLOAK || nMagicID == _SKILL_SINSU )
		LoadEffect(&g_xGameProc.m_xImage, nMagicID, bMagicDir);
	else
		LoadEffect(&g_xGameProc.m_xImage, nMagicID);

	if	( nMagicID == _SKILL_SHOWHP )				m_bEffectFrameCnt = 20;
	else if ( nMagicID == _SKILL_LIGHTFLOWER )		m_bEffectFrameCnt = 20;
	else if ( nMagicID == _SKILL_SPACEMOVE )		m_bEffectFrameCnt = 19;
	else if ( nMagicID == _SKILL_LIGHTENING )		m_bEffectFrameCnt = 17;

	m_bWarMode = m_bUseEffect = TRUE;

/*	if ( nMagicID == _SKILL_SHIELD )
	{
		m_dwFstEffectFrame  = 50;
		m_dwEndEffectFrame  = 60;
		m_dwCurrEffectFrame = 50;
		m_bEffectFrame		= 0;
		m_bEffectFrameCnt	= _DEFAULT_SPELLFRAME;
	}
*/	m_shCurrMagicID	= nMagicID;

	switch( nMagicID )
	{
		case _SKILL_HANGMAJINBUB:
		case _SKILL_DEJIWONHO:
		case _SKILL_FIRECHARM:
		case _SKILL_FIRE:
		case _SKILL_FIREBALL2:
		case _SKILL_SINSU:
		case _SKILL_FIREBALL:
		case _SKILL_SHOOTLIGHTEN:
		case _SKILL_BIGCLOAK:
			SetMotionFrame(_MT_SPELL1, bMagicDir);
			break;
		case _SKILL_FIREWIND:	
		case _SKILL_AMYOUNSUL:
		case _SKILL_TAMMING:
		case _SKILL_KILLUNDEAD:
		case _SKILL_HEALLING:
		case _SKILL_HOLYSHIELD:
		case _SKILL_BIGHEALLING:
		case _SKILL_LIGHTFLOWER:
		case _SKILL_SKELLETON:
		case _SKILL_SNOWWIND:
		case _SKILL_SHIELD:
		case _SKILL_SHOWHP:
		case _SKILL_EARTHFIRE:
		case _SKILL_FIREBOOM:
		case _SKILL_SPACEMOVE:
		case _SKILL_CLOAK:
		case _SKILL_LIGHTENING:
		default:
			SetMotionFrame(_MT_SPELL2, bMagicDir);
			break;			
	}
}



VOID CHero::OnButch(LPPACKETMSG lpPacketMsg)
{
	BYTE bDir;
	m_wPosX = lpPacketMsg->stDefMsg.wParam;
	m_wPosY = lpPacketMsg->stDefMsg.wTag;
	bDir	= lpPacketMsg->stDefMsg.wSeries;
	SetMotionFrame(_MT_CUT, bDir);
}


/******************************************************************************************************************

	함수명 : CHero::UpdatePacketState()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CHero::UpdatePacketState()
{
	LPPACKETMSG	lpPacketMsg = NULL;


	if ( m_bCurrMtn == _MT_STAND || m_bCurrMtn == _MT_ATTACKMODE || m_bCurrMtn == _MT_HORSESTAND || (m_bCurrMtn==_MT_SPELL1 && m_dwCurrFrame == m_dwEndFrame-2) )
	{
		LPPACKETMSG	lpPacketMsg;
		SHORT shLeftMsgCnt = m_xPacketQueue.GetCount();

		if ( shLeftMsgCnt > 0 )
		{
			lpPacketMsg = (LPPACKETMSG)m_xPacketQueue.PopQ();

			if ( shLeftMsgCnt >= 3 )		m_bMsgHurryCheck = TRUE;
			else							m_bMsgHurryCheck = FALSE;

			if ( lpPacketMsg )
			{
				switch ( lpPacketMsg->stDefMsg.wIdent )
				{
					case SM_SITDOWN:
					case SM_BUTCH:				{	OnButch(lpPacketMsg);				break;	}
					case SM_FEATURECHANGED:		{	OnFeatureChanged(lpPacketMsg);		break;	}
					case SM_CHARSTATUSCHANGE:	{	OnCharStatusChanged(lpPacketMsg);	break;	}
					case SM_OPENHEALTH:			{	OnOpenHealth(lpPacketMsg);			break;	}
					case SM_CLOSEHEALTH:		{	OnCloseHealth(lpPacketMsg);			break;	}
					case SM_CHANGELIGHT:		{	OnChangeLight(lpPacketMsg);			break;	}
					case SM_USERNAME:			{	OnUserName(lpPacketMsg);			break;	}
					case SM_CHANGENAMECOLOR:	{	OnChangeNameClr(lpPacketMsg);		break;	}
					case SM_HEALTHSPELLCHANGED:	{	OnHealthSpellChanged(lpPacketMsg);	break;	}
					case SM_RUSH:				{	OnRush(lpPacketMsg);				break;	}
					case SM_BACKSTEP:			{	OnBackStep(lpPacketMsg);			break;	}
					case SM_NOWDEATH:
					case SM_DEATH:				{	OnDeath(lpPacketMsg);				break;	}
					case SM_WALK:				{	OnWalk(lpPacketMsg);				break;	}
					case SM_RUN:				{	OnRun(lpPacketMsg);					break;	}
					case SM_TURN:				{	OnTurn(lpPacketMsg);				break;	}
					case SM_STRUCK:				{	OnStruck(lpPacketMsg);				break;	}
					case SM_HIT:
					case SM_FIREHIT:
					case SM_LONGHIT:
					case SM_POWERHIT:
					case SM_WIDEHIT:			{	OnHit(lpPacketMsg);					break;	}
					case SM_MAGICFIRE:			{	OnMagicFire(lpPacketMsg);			break;	}
					case SM_SPELL:				{	OnSpell(lpPacketMsg);				break;	}
					default:
					{										
						break;	
					}
				}
			}

			SAFE_DELETE(lpPacketMsg);
			return TRUE;
		}
	}
	return FALSE;
}



/******************************************************************************************************************

	함수명 : CHero::UpdateMotionState()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : INT nLoopTime
	         BOOL bIsMoveTime
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CHero::UpdateMotionState(INT nLoopTime, BOOL bIsMoveTime)
{
	if ( m_bWarMode ) 
		m_dwWarModeTime += nLoopTime;

	m_wABlendCurrDelay += nLoopTime;

	if ( m_wABlendCurrDelay >= m_wABlendDelay )
	{
		m_wABlendCurrDelay = 0;
		m_wABlendDelay  = 0;
		m_bABlendRev	= FALSE;
	}

	if ( m_bCurrMtn == _MT_DIE && m_dwCurrFrame >= m_dwEndFrame-1 )
	{
		m_bIsDead = TRUE;
	}

	if ( m_bIsDead )
	{
		SetMotionFrame(_MT_DIE, m_bCurrDir);
		m_dwCurrFrame = m_dwEndFrame - 1;
		return;
	}

	if ( UpdateMove(bIsMoveTime) )
	{
		UpdatePacketState();
		return;
	}
	else
	{
		m_wCurrDelay += nLoopTime;

		m_wShieldCurrDelay += nLoopTime;
		if ( m_wShieldCurrDelay > 150 )  
		{
			m_bShieldCurrFrm++;
			m_wShieldCurrDelay = 0;
			if ( m_bShieldCurrFrm > 2 )
				m_bShieldCurrFrm = 0;
		}

		if ( m_wCurrDelay > m_wDelay )
		{
			m_wCurrDelay = 0;

			if ( m_dwCurrFrame < m_dwEndFrame )
			{
				m_dwCurrFrame++;

				PlayActSound();

				if ( (m_bCurrMtn == _MT_SPELL2 || m_bCurrMtn == _MT_SPELL1) && m_bUseEffect )
				{
					m_dwCurrEffectFrame++;
					m_bEffectFrame++;
				}

				if ( m_bMsgHurryCheck )			
				{
					m_wDelay = m_wDelay/2;
					m_bMsgHurryCheck = FALSE;
				}
			}
		}

		UpdatePacketState();
		
		if ( m_dwWarModeTime > _WARMODE_TIME )
		{
			m_dwWarModeTime = 0;
			m_bWarMode		= FALSE;
		}

		if ( m_dwCurrFrame >= m_dwEndFrame-1 )
		{
			if ( (m_bCurrMtn == _MT_SPELL2) && m_bUseEffect )
			{
				if ( m_dwCurrEffectFrame - m_dwFstEffectFrame < m_bEffectFrameCnt-2 )
				{
					m_dwCurrFrame = m_dwEndFrame - 2;
				}
			}
		}
		else if ( m_dwCurrFrame >= m_dwEndFrame-3 )
		{
			if ( (m_bCurrMtn == _MT_SPELL1) && m_bUseEffect )
			{
				if ( m_dwCurrEffectFrame - m_dwFstEffectFrame < m_bEffectFrameCnt-5 )
				{
					m_dwCurrFrame = m_dwEndFrame - 4;
				}
			}
		}

		if ( m_dwCurrFrame >= m_dwEndFrame )
		{
			m_dwCurrEffectFrame = 0;
			m_dwFstEffectFrame = 0;
			m_dwEndEffectFrame = 0;
			m_bEffectFrame = 0;
			m_bEffectFrameCnt = _DEFAULT_SPELLFRAME;
			m_bUseEffect = FALSE;
			m_bUseSwordEffect = FALSE;
			m_dwCurrFrame = m_dwFstFrame;

			if ( m_stFeatureEx.bHorse == _HORSE_NONE )
			{
				if ( m_bWarMode )
					SetMotionFrame(_MT_ATTACKMODE, m_bCurrDir);
				else
					SetMotionFrame(_MT_STAND, m_bCurrDir);
			}
			else
			{
				SetMotionFrame(_MT_HORSESTAND, m_bCurrDir);
			}
		}

	}
}



/******************************************************************************************************************

	함수명 : CHero::ChangeFeature()

	작성자 : 
	작성일 : 

	목적   : BYTE bGender, BYTE bWear, BYTE bHair, BYTE bWeapon;
	입력   : WORD wActor
	         BYTE bHair
	         BYTE bWeapon
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CHero::ChangeFeature(FEATURE stFeature, FEATUREEX stFeatureEx)
{
	if ( CActor::ChangeFeature(stFeature) )
	{
		CImageHandler* pxImgHandler = &g_xGameProc.m_xImage;

		m_stFeatureEx = stFeatureEx;




//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
//		m_stFeatureEx.bHorse = 0;
//		m_stFeatureEx.wDressColor = 0XFFFF;
//		m_stFeatureEx.wHairColor = 0XFFFF;
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!






		if ( m_stFeature.bHair	  >= _MAX_HAIR )	m_stFeature.bHair	= _HAIR_NONE;
		if ( m_stFeature.bWeapon  >= _MAX_WEAPON )	m_stFeature.bWeapon	= _WEAPON_NONE;
		if ( m_stFeatureEx.bHorse >= _MAX_HORSE )	m_stFeatureEx.bHorse= _HORSE_NONE;

		m_bHorseImgIdx	= _IMAGE_HORSE;
		if ( m_stFeature.bGender == _GENDER_MAN )
		{
			m_bHairImgIdx	= _IMAGE_M_HAIR;
			if ( m_stFeature.bWeapon != _WEAPON_NONE )			m_bWeaponImgIdx	= _IMAGE_M_WEAPON1+(m_stFeature.bWeapon-1)/10;
		}
		else if ( m_stFeature.bGender == _GENDER_WOMAN )
		{
			m_bHairImgIdx	= _IMAGE_WM_HAIR;
			if ( m_stFeature.bWeapon != _WEAPON_NONE )			m_bWeaponImgIdx	= _IMAGE_WM_WEAPON1+(m_stFeature.bWeapon-1)/10;
		}
		else			return FALSE;

		if ( m_stFeature.bHair != _HAIR_NONE )		m_pxHairImage	= &pxImgHandler->m_xImageList[m_bHairImgIdx];
		else										m_pxHairImage	= NULL;

		if ( m_stFeature.bWeapon != _WEAPON_NONE )	m_pxWeaponImage	= &pxImgHandler->m_xImageList[m_bWeaponImgIdx];
		else										m_pxWeaponImage	= NULL;

		if ( m_stFeatureEx.bHorse != _HORSE_NONE )	m_pxHorseImage	= &pxImgHandler->m_xImageList[m_bHorseImgIdx];
		else										m_pxHorseImage	= NULL;

		return TRUE;
	}
	return FALSE;
}



/******************************************************************************************************************

	함수명 : CHero::DrawActor()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : BOOL bFocused
	         BOOL bShadowAblended
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CHero::DrawActor(CMapHandler* pxMap, BOOL bFocused, BOOL bShadowAblended, BOOL bUseScrnPos, BOOL bDrawShadow)
{
	// 좌표처리.
	if ( bUseScrnPos )
	{
		m_shScrnPosX = (m_wPosX - pxMap->m_shStartViewTileX) * _CELL_WIDTH + _VIEW_CELL_X_START - pxMap->m_shViewOffsetX + m_shShiftPixelX;
		m_shScrnPosY = (m_wPosY - pxMap->m_shStartViewTileY) * _CELL_HEIGHT+ _VIEW_CELL_Y_START - pxMap->m_shViewOffsetY + m_shShiftPixelY;
	}

	BYTE	bShadowType;
	SHORT	shShadowPX;
	SHORT	shShadowPY;

	SHORT	shPX, shPY;

	//---------------------------------------------------------------------------------------------------------------//
	// 각 이미지 세팅및 좌표영역 체크.
	//---------------------------------------------------------------------------------------------------------------//
	// 말.
	if ( m_stFeatureEx.bHorse != _HORSE_NONE && m_pxHorseImage != NULL )
	{
		if ( m_bCurrMtn >= _START_HORSE_MTN )
		{		
			m_dwCurrHorseFrame = (m_stFeatureEx.bHorse*_MAX_HORSE_FRAME-_MAX_HORSE_FRAME)+(m_dwCurrFrame-(_MAX_HERO_FRAME*m_stFeature.bDress)-_START_HORSE_FRAME);
			if ( !m_pxHorseImage->NewSetIndex(m_dwCurrHorseFrame) )
				return FALSE;
			shPX = m_pxHorseImage->m_lpstNewCurrWilImageInfo->shPX;
			shPY = m_pxHorseImage->m_lpstNewCurrWilImageInfo->shPY;
			SetRect(&m_rcHorse, m_shScrnPosX + shPX, m_shScrnPosY + shPY,
					m_shScrnPosX + shPX + m_pxHorseImage->m_lpstNewCurrWilImageInfo->shWidth, 
					m_shScrnPosX + shPY + m_pxHorseImage->m_lpstNewCurrWilImageInfo->shHeight);
		}
	} 

	// 무기.
	if ( m_stFeature.bWeapon != _WEAPON_NONE && m_pxWeaponImage != NULL )
	{
		if ( m_bCurrMtn < _MAX_WEAPON_MTN )
		{	
			m_dwCurrWeaponFrame = (((m_stFeature.bWeapon-1)%10)*_MAX_WEAPON_FRAME)+(m_dwCurrFrame-(_MAX_HERO_FRAME*m_stFeature.bDress));
			if ( !m_pxWeaponImage->NewSetIndex(m_dwCurrWeaponFrame) )
				return FALSE;

			shPX = m_pxWeaponImage->m_lpstNewCurrWilImageInfo->shPX;
			shPY = m_pxWeaponImage->m_lpstNewCurrWilImageInfo->shPY;
			SetRect(&m_rcWeapon, m_shScrnPosX + shPX, m_shScrnPosY + shPY, 
					m_shScrnPosX + shPX + m_pxWeaponImage->m_lpstNewCurrWilImageInfo->shWidth, 
					m_shScrnPosY + shPY + m_pxWeaponImage->m_lpstNewCurrWilImageInfo->shHeight);
		}
	}

	// 캐릭터.
	if ( m_pxActorImage )
	{
		if ( !m_pxActorImage->NewSetIndex(m_dwCurrFrame) )
			return FALSE;
		shPX = m_pxActorImage->m_lpstNewCurrWilImageInfo->shPX;
		shPY = m_pxActorImage->m_lpstNewCurrWilImageInfo->shPY;
		bShadowType = m_pxActorImage->m_lpstNewCurrWilImageInfo->bShadow;
		shShadowPX  = m_pxActorImage->m_lpstNewCurrWilImageInfo->shShadowPX;
		shShadowPY  = m_pxActorImage->m_lpstNewCurrWilImageInfo->shShadowPY;

		SetRect(&m_rcActor, m_shScrnPosX + shPX, m_shScrnPosY + shPY, 
				m_shScrnPosX + shPX + m_pxActorImage->m_lpstNewCurrWilImageInfo->shWidth, 
				m_shScrnPosY + shPY + m_pxActorImage->m_lpstNewCurrWilImageInfo->shHeight);

		if ( ( m_rcActor.right - m_rcActor.left ) > _CELL_WIDTH + _TARGETRGN_GAPX )
		{
			m_rcTargetRgn.left  = m_rcActor.left + ( (m_rcActor.right - m_rcActor.left) - (_CELL_WIDTH + _TARGETRGN_GAPX) )/2;
			m_rcTargetRgn.right = m_rcActor.left + ( (m_rcActor.right - m_rcActor.left) + (_CELL_WIDTH + _TARGETRGN_GAPX) )/2;
		}
		else
		{
			m_rcTargetRgn.left  = m_rcActor.left;
			m_rcTargetRgn.right = m_rcActor.right;
		}

		if ( ( m_rcActor.bottom - m_rcActor.top ) > _CELL_HEIGHT + _TARGETRGN_GAPY )
		{
			m_rcTargetRgn.top    = m_rcActor.top + ( (m_rcActor.bottom - m_rcActor.top) - (_CELL_HEIGHT + _TARGETRGN_GAPY) )/2;
			m_rcTargetRgn.bottom = m_rcActor.top + ( (m_rcActor.bottom - m_rcActor.top) + (_CELL_HEIGHT + _TARGETRGN_GAPY) )/2;
		}
		else
		{
			m_rcTargetRgn.top    = m_rcActor.top;
			m_rcTargetRgn.bottom = m_rcActor.bottom;
		}
	}

	// 머리.
	if ( m_stFeature.bHair != _HAIR_NONE && m_pxHairImage != NULL )
	{
		m_dwCurrHairFrame = (m_stFeature.bHair*_MAX_HERO_FRAME-_MAX_HERO_FRAME)+(m_dwCurrFrame-(_MAX_HERO_FRAME*m_stFeature.bDress));
		if ( !m_pxHairImage->NewSetIndex(m_dwCurrHairFrame) )
			return FALSE;
		shPX = m_pxHairImage->m_lpstNewCurrWilImageInfo->shPX;
		shPY = m_pxHairImage->m_lpstNewCurrWilImageInfo->shPY;
		SetRect(&m_rcHair, m_shScrnPosX + shPX, m_shScrnPosY + shPY, 
					m_shScrnPosX + shPX + m_pxHairImage->m_lpstNewCurrWilImageInfo->shWidth, 
					m_shScrnPosY + shPY + m_pxHairImage->m_lpstNewCurrWilImageInfo->shHeight);
	}

	//---------------------------------------------------------------------------------------------------------------//
	// 그림자그리기.
	//---------------------------------------------------------------------------------------------------------------//
	BYTE bOpaRate = 70;
	WORD wState   = GetCharState();

	// 무기.
	if ( m_stFeature.bWeapon != _WEAPON_NONE && m_pxWeaponImage != NULL )
	{
		if ( m_bCurrMtn < _MAX_WEAPON_MTN )
		{
			if ( bDrawShadow )
			{
				if ( !m_bIsDead )
				{
					g_xMainWnd.DrawWithShadowABlend(
									m_rcWeapon.left, m_rcWeapon.top,
									m_pxWeaponImage->m_lpstNewCurrWilImageInfo->shWidth,
									m_pxWeaponImage->m_lpstNewCurrWilImageInfo->shHeight,
									m_pxWeaponImage->m_lpstNewCurrWilImageInfo->shPX,
									m_pxWeaponImage->m_lpstNewCurrWilImageInfo->shPY,
									(WORD*)m_pxWeaponImage->m_pbCurrImage, 						
									_CLIP_WIDTH, _CLIP_HEIGHT, g_xGameProc.m_wShadowClr, bShadowAblended, bOpaRate);
				}
/*				else
				{
					g_xMainWnd.DrawWithShadowABlend(
									m_rcWeapon.left+3, m_rcWeapon.top-2,
									m_pxWeaponImage->m_lpstNewCurrWilImageInfo->shWidth,
									m_pxWeaponImage->m_lpstNewCurrWilImageInfo->shHeight,
									m_pxWeaponImage->m_lpstNewCurrWilImageInfo->shPX,
									m_pxWeaponImage->m_lpstNewCurrWilImageInfo->shPY,
									(WORD*)m_pxWeaponImage->m_pbCurrImage, 						
									_CLIP_WIDTH, _CLIP_HEIGHT, g_xGameProc.m_wShadowClr, bShadowAblended, bOpaRate);
				}
*/			}
		}
	}

	// 말.
	if ( m_stFeatureEx.bHorse != _HORSE_NONE && m_pxHorseImage != NULL )
	{
		if ( m_bCurrMtn >= _START_HORSE_MTN )
		{	
			if ( bDrawShadow )
			{
				if ( !m_bIsDead )
				{
					g_xMainWnd.DrawWithShadowABlend(
									m_rcHorse.left, m_rcHorse.top,
									m_pxHorseImage->m_lpstNewCurrWilImageInfo->shWidth,
									m_pxHorseImage->m_lpstNewCurrWilImageInfo->shHeight,
									m_pxHorseImage->m_lpstNewCurrWilImageInfo->shPX,
									m_pxHorseImage->m_lpstNewCurrWilImageInfo->shPY,
									(WORD*)m_pxHorseImage->m_pbCurrImage, 						
									_CLIP_WIDTH, _CLIP_HEIGHT, g_xGameProc.m_wShadowClr, bShadowAblended, bOpaRate);
				}
				else
				{
					g_xMainWnd.DrawWithShadowABlend(
									m_rcHorse.left+3, m_rcHorse.top+2,
									m_pxHorseImage->m_lpstNewCurrWilImageInfo->shWidth,
									m_pxHorseImage->m_lpstNewCurrWilImageInfo->shHeight,
									m_pxHorseImage->m_lpstNewCurrWilImageInfo->shPX,
									m_pxHorseImage->m_lpstNewCurrWilImageInfo->shPY,
									(WORD*)m_pxHorseImage->m_pbCurrImage, 						
									_CLIP_WIDTH, _CLIP_HEIGHT, g_xGameProc.m_wShadowClr, bShadowAblended, bOpaRate);
				}
			}
		}
	}

	// 캐릭터.
	if ( m_pxActorImage )
	{
		INT nStartX1 = m_shScrnPosX + shShadowPX;
		INT nStartY1 = m_shScrnPosY + shShadowPY;

		if ( bDrawShadow )
		{
			if ( !m_bIsDead )
			{
				g_xMainWnd.DrawWithShadowABlend(
									nStartX1, nStartY1,
									m_pxActorImage->m_lpstNewCurrWilImageInfo->shWidth,
									m_pxActorImage->m_lpstNewCurrWilImageInfo->shHeight,
									(WORD*)m_pxActorImage->m_pbCurrImage,
									_CLIP_WIDTH, _CLIP_HEIGHT, g_xGameProc.m_wShadowClr, bShadowAblended, bShadowType, bOpaRate);
			}
			else
			{
				g_xMainWnd.DrawWithShadowABlend(
									m_rcActor.left+3, m_rcActor.top+2,
									m_pxActorImage->m_lpstNewCurrWilImageInfo->shWidth,
									m_pxActorImage->m_lpstNewCurrWilImageInfo->shHeight,
									(WORD*)m_pxActorImage->m_pbCurrImage,
									_CLIP_WIDTH, _CLIP_HEIGHT, g_xGameProc.m_wShadowClr, bShadowAblended, 50, bOpaRate);
			}
		}
	}

	// 머리는 그림자를 그리지 않는다.
	if ( m_wABlendDelay )		
	{
		bShadowAblended = TRUE;

		if ( m_wABlendDelay )	
		{
			if ( !m_bABlendRev )
				bOpaRate = 100 - ( (m_wABlendCurrDelay * 100) / m_wABlendDelay );
			else
				bOpaRate = ( (m_wABlendCurrDelay * 100) / m_wABlendDelay );
		}

		if ( bFocused )								
		{
			if ( !m_bABlendRev )
				bOpaRate -= 20;
			else
				bOpaRate += 20;
		}

		if ( bOpaRate < 0 && bOpaRate > 100 )		bOpaRate  = 0;
	}
	else if ( wState==_STATE_ABLEND )			bShadowAblended = TRUE;

	//---------------------------------------------------------------------------------------------------------------//
	// 이미지 그리기.
	//---------------------------------------------------------------------------------------------------------------//
	// 말.
	if ( m_stFeatureEx.bHorse != _HORSE_NONE && m_pxHorseImage != NULL )
	{
		if ( m_bCurrMtn >= _START_HORSE_MTN )
		{			
			DrawWithEffected(m_rcHorse.left, m_rcHorse.top, m_pxHorseImage->m_lpstNewCurrWilImageInfo->shWidth, m_pxHorseImage->m_lpstNewCurrWilImageInfo->shHeight, (WORD*)m_pxHorseImage->m_pbCurrImage, 
				             0XFFFF, 0XFFFF, bFocused, bOpaRate, wState);
		}
	}

	// 무기를 먼저 그려야될방향.
	if ( m_stFeature.bWeapon != _WEAPON_NONE && m_pxWeaponImage != NULL && g_xSpriteInfo.m_bWOrder[(m_dwCurrFrame-(_MAX_HERO_FRAME*m_stFeature.bDress))] )
	{
		if ( m_bCurrMtn < _MAX_WEAPON_MTN )
		{			
			DrawWithEffected(m_rcWeapon.left, m_rcWeapon.top, m_pxWeaponImage->m_lpstNewCurrWilImageInfo->shWidth, m_pxWeaponImage->m_lpstNewCurrWilImageInfo->shHeight, (WORD*)m_pxWeaponImage->m_pbCurrImage,
							 0XFFFF, 0XFFFF, bFocused, bOpaRate, wState);
/* 			g_xMainWnd.DrawWithImageForCompClipRgn(
							m_rcWeapon.left, m_rcWeapon.top,
							m_pxWeaponImage->m_lpstNewCurrWilImageInfo->shWidth,
							m_pxWeaponImage->m_lpstNewCurrWilImageInfo->shHeight,
							(WORD*)m_pxWeaponImage->m_pbCurrImage, _CLIP_WIDTH, _CLIP_HEIGHT, bFocused);
*/		}
	}

	// 캐릭터.
	if ( m_pxActorImage != NULL )
	{
		DrawWithEffected(m_rcActor.left, m_rcActor.top, m_pxActorImage->m_lpstNewCurrWilImageInfo->shWidth, m_pxActorImage->m_lpstNewCurrWilImageInfo->shHeight, (WORD*)m_pxActorImage->m_pbCurrImage,
				         m_stFeatureEx.wDressColor, m_stFeatureEx.wHairColor, bFocused, bOpaRate, wState);
	}

	// 머리.
 	if ( m_stFeature.bHair != _HAIR_NONE && m_pxHairImage != NULL )
	{
		DrawWithEffected(m_rcHair.left, m_rcHair.top, m_pxHairImage->m_lpstNewCurrWilImageInfo->shWidth, m_pxHairImage->m_lpstNewCurrWilImageInfo->shHeight, (WORD*)m_pxHairImage->m_pbCurrImage, 
				         m_stFeatureEx.wDressColor, m_stFeatureEx.wHairColor, bFocused, bOpaRate, wState);
	}			

	// 무기를 나중에 그려야될 방향.
	if ( m_stFeature.bWeapon != _WEAPON_NONE && m_pxWeaponImage != NULL && !g_xSpriteInfo.m_bWOrder[(m_dwCurrFrame-(_MAX_HERO_FRAME*m_stFeature.bDress))] ) 
	{
		if ( m_bCurrMtn < _MAX_WEAPON_MTN )
		{			
 			DrawWithEffected(m_rcWeapon.left, m_rcWeapon.top, m_pxWeaponImage->m_lpstNewCurrWilImageInfo->shWidth, m_pxWeaponImage->m_lpstNewCurrWilImageInfo->shHeight, (WORD*)m_pxWeaponImage->m_pbCurrImage,
							 0XFFFF, 0XFFFF, bFocused, /*bOpaRate*/20, wState);
/*			g_xMainWnd.DrawWithImageForCompClipRgn(
						m_rcWeapon.left, m_rcWeapon.top,
						m_pxWeaponImage->m_lpstNewCurrWilImageInfo->shWidth,
						m_pxWeaponImage->m_lpstNewCurrWilImageInfo->shHeight,
						(WORD*)m_pxWeaponImage->m_pbCurrImage, _CLIP_WIDTH, _CLIP_HEIGHT, bFocused);
*/		}
	}

	DrawEffect();
	ShowShield();

	if ( !bDrawShadow )
		return TRUE;

	if ( bUseScrnPos )
	{
		if ( m_bLightSize == 2 )
			g_xGameProc.m_xLightFog.SetLightRadiusWithCircle(m_rcActor.left+_CELL_WIDTH/2, m_rcActor.top+_CELL_HEIGHT/2, 3, 100, 100, 100);
		else
			g_xGameProc.m_xLightFog.SetLightRadiusWithCircle(m_rcActor.left+_CELL_WIDTH/2, m_rcActor.top+_CELL_HEIGHT/2, m_bLightSize, 255, 255, 255);
	}
	else
	{
		if ( m_bLightSize == 2 )
			g_xGameProc.m_xLightFog.SetLightRadiusWithCircle(400, 242, 3, 100, 100, 100);
		else
			g_xGameProc.m_xLightFog.SetLightRadiusWithCircle(400, 242, m_bLightSize, 255, 255, 255);
	}
	return TRUE;
}

























/******************************************************************************************************************

	CMyHero Class

*******************************************************************************************************************/
/******************************************************************************************************************

	함수명 : CMyHero::CMyHero()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
CMyHero::CMyHero()
{
	InitActor();
}



/******************************************************************************************************************

	함수명 : CMyHero::~CMyHero()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
CMyHero::~CMyHero()
{
	DestroyActor();
}


VOID CMyHero::InitActor()
{
	CHero::InitActor();

	m_wOldPosX			= 0;
	m_wOldPosY			= 0;
	m_bOldDir			= 0;
	m_dwWarModeTime		= 0;
	m_dwMotionLockTime	= 0;

	m_wMagicDelayTime	= 0;
	m_wMagicPKDelayTime	= 0;

	m_bJob				= 0;
	m_nGlod				= 0;

	m_dwLastHitTime		= timeGetTime();
	m_dwLastSpellTime	= timeGetTime();
	m_dwLastMagicTime	= timeGetTime();
	m_dwLastStruckTime  = timeGetTime();
	m_dwLastPKStruckTime= timeGetTime();
	m_dwLastRushTime	= timeGetTime();
	m_dwLastFireHitTime = timeGetTime();

	m_bCanRun			= FALSE;
	m_bInputLock		= FALSE;
	m_bMotionLock		= FALSE;
	m_bWarMode			= FALSE;

	m_pxMap				= NULL;

	m_bAttackMode		= _MT_ONEHSWING;

	ZeroMemory(&m_stAbility, sizeof(ACTORABILITY));
	ZeroMemory(&m_stSubAbility, sizeof(ACTORSUBABILITY));
}

VOID CMyHero::DestroyActor()
{
	INT			nCnt;
	SHORT		shLeftMsgCnt;
	LPPACKETMSG	lpPacketMsg;

	CHero::DestroyActor();

	lpPacketMsg  = NULL;
	shLeftMsgCnt = m_xPriorPacketQueue.GetCount();

	// 쌓여있는 패킷을 지운다.
	if ( shLeftMsgCnt > 0 )
	{
		for ( nCnt = 0; nCnt < shLeftMsgCnt; nCnt++ )
		{
			lpPacketMsg = (LPPACKETMSG)m_xPriorPacketQueue.PopQ();
			if ( lpPacketMsg )
			{
				SAFE_DELETE(lpPacketMsg);
			}
		}
	}

	InitActor();
}

BOOL CMyHero::Create(CImageHandler* pxImgHandler, BYTE bMtn, BYTE bDir, WORD wPosX, WORD wPosY, 
				   FEATURE* pstFeature, FEATUREEX* pstFeatureEx)
{
	ZeroMemory(&m_stAbility, sizeof(ACTORABILITY));
	ZeroMemory(&m_stSubAbility, sizeof(ACTORSUBABILITY));
	
	if ( !(CHero::Create(pxImgHandler, bMtn, bDir, wPosX, wPosY, pstFeature, pstFeatureEx)) )
		return FALSE;

	return TRUE;
}



/******************************************************************************************************************

	함수명 : CMyHero::SetMapHandler()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : CMapHandler* pxMap
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CMyHero::SetMapHandler(CMapHandler* pxMap)
{
	m_pxMap = pxMap;
}


VOID CMyHero::OnHealthSpellChanged(LPPACKETMSG lpPacketMsg)
{
	m_stAbility.wHP	   = lpPacketMsg->stDefMsg.wParam;
	m_stAbility.wMaxHP = lpPacketMsg->stDefMsg.wSeries;
	m_stAbility.wMP    = lpPacketMsg->stDefMsg.wTag;

	if ( m_stAbility.wHP <= 0 )			m_stAbility.wHP = m_bHPPercent = 0;
	else
	{
		FLOAT wHPRate= (FLOAT)((FLOAT)m_stAbility.wHP/(FLOAT)m_stAbility.wMaxHP);
		m_bHPPercent = wHPRate * 100;
	}
}


/******************************************************************************************************************

	함수명 : CMyHero::UpdatePacketState()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CMyHero::UpdatePacketState()
{	
	LPPACKETMSG	lpPacketMsg = NULL;

	SHORT shLeftMsgCnt = m_xPriorPacketQueue.GetCount();

	if ( shLeftMsgCnt > 0 )
	{
		lpPacketMsg = (LPPACKETMSG)m_xPriorPacketQueue.PopQ();
		if ( lpPacketMsg )
		{
			if ( lpPacketMsg->stDefMsg.wIdent == SM_NOWDEATH || lpPacketMsg->stDefMsg.wIdent == SM_DEATH )
			{
				OnDeath(lpPacketMsg);
				SAFE_DELETE(lpPacketMsg);
				return TRUE;
			}
		}
	}

	if ( m_bCurrMtn == _MT_STAND || m_bCurrMtn == _MT_ATTACKMODE || m_bCurrMtn == _MT_HORSESTAND || (m_bCurrMtn==_MT_SPELL1 && m_dwCurrFrame==m_dwEndFrame - 2) )
	{
		LPPACKETMSG	lpPacketMsg;

		SHORT shLeftMsgCnt = m_xPacketQueue.GetCount();

		if ( shLeftMsgCnt > 0 )
		{
			lpPacketMsg = (LPPACKETMSG)m_xPacketQueue.PopQ();

			if ( shLeftMsgCnt >= 3 )		m_bMsgHurryCheck = TRUE;
			else							m_bMsgHurryCheck = FALSE;
			
			if ( lpPacketMsg )
			{
				switch ( lpPacketMsg->stDefMsg.wIdent )
				{
					case SM_STRUCK:
					{
						m_bInputLock = TRUE;
						m_stAbility.wHP	   = lpPacketMsg->stDefMsg.wParam;
						m_stAbility.wMaxHP = lpPacketMsg->stDefMsg.wTag;
						OnStruck(lpPacketMsg);

						if ( m_dwNameClr == RGB(255, 0, 0) )
						{
							m_dwLastPKStruckTime = timeGetTime();
						}
						m_dwLastStruckTime = timeGetTime();
						break;
					}
					case SM_RUSH:
					{
						m_bInputLock = TRUE;
						m_wOldPosX	 = m_wPosX;
						m_wOldPosY	 = m_wPosY;
						m_bOldDir	 = m_bCurrDir;
						OnRush(lpPacketMsg);
						if ( !CheckMyPostion() )			AdjustMyPostion();
						m_pxMap->ScrollMap(m_bMoveDir, m_dwCurrFrame-m_dwFstFrame, m_bMoveSpeed);
						break;
					}
					case SM_BACKSTEP:
					{
						m_bInputLock = TRUE;
						m_wOldPosX = m_wPosX;
						m_wOldPosY = m_wPosY;
						m_bOldDir  = m_bCurrDir;
						OnBackStep(lpPacketMsg);

						if ( !CheckMyPostion() )			AdjustMyPostion();
						m_pxMap->ScrollMap(m_bMoveDir, m_dwCurrFrame-m_dwFstFrame, m_bMoveSpeed);			
						break;
					}
					case SM_FEATURECHANGED:		{	OnFeatureChanged(lpPacketMsg);		break;	}
					case SM_OPENHEALTH:			{	OnOpenHealth(lpPacketMsg);			break;	}
					case SM_CLOSEHEALTH:		{	OnCloseHealth(lpPacketMsg);			break;	}
					case SM_CHANGELIGHT:		{	OnChangeLight(lpPacketMsg);			break;	}
					case SM_USERNAME:			{	OnUserName(lpPacketMsg);			break;	}
					case SM_CHANGENAMECOLOR:	{	OnChangeNameClr(lpPacketMsg);		break;	}
					case SM_CHARSTATUSCHANGE:	{	OnCharStatusChanged(lpPacketMsg);	break;	}
					case SM_MAGICFIRE:			{	OnMagicFire(lpPacketMsg);			break;	}
					case SM_HEALTHSPELLCHANGED:	{	OnHealthSpellChanged(lpPacketMsg);	break;	}
					default:
					{										
						break;	
					}
				}				
			}
			SAFE_DELETE(lpPacketMsg);

			return TRUE;
		}
	}
	return FALSE;
}



/******************************************************************************************************************

	함수명 : CMyHero::SetOldPosition()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CMyHero::SetOldPosition()
{
	m_wPosX	   = m_wOldPosX;
	m_wPosY	   = m_wOldPosY;
	m_bCurrDir = m_bOldDir;

	if ( m_stFeatureEx.bHorse == _HORSE_NONE )
	{
		if ( SetMotionFrame(_MT_STAND, m_bCurrDir) )
		{
			AdjustMyPostion();
			m_bMotionLock = m_bInputLock = FALSE;
			return TRUE;
		}
	}
	else
	{
		if ( SetMotionFrame(_MT_HORSESTAND, m_bCurrDir) )
		{
			AdjustMyPostion();
			m_bMotionLock = m_bInputLock = FALSE;
			return TRUE;
		}
	}
	return FALSE;
}



/******************************************************************************************************************

	함수명 : CMyHero::CheckMyPostion()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CMyHero::CheckMyPostion()
{
	if ( m_wPosX != m_pxMap->m_shStartViewTileX + _GAPX_TILE_CHAR_MAP ||
		 m_wPosY != m_pxMap->m_shStartViewTileY + _GAPY_TILE_CHAR_MAP )
	{
		return FALSE;
	}
	return TRUE;
}



/******************************************************************************************************************

	함수명 : CMyHero::AdjustMyPostion()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CMyHero::AdjustMyPostion()
{
	m_pxMap->SetStartViewTile(m_wPosX-_GAPX_TILE_CHAR_MAP, m_wPosY-_GAPY_TILE_CHAR_MAP);
	m_pxMap->LoadNewMapBuffer();
}


/******************************************************************************************************************

	함수명 : CMyHero::SetMotionState()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : BYTE bMtn
	         BYTE bDir
	         LPPOINT lpptTarget
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CMyHero::SetMotionState(BYTE bMtn, BYTE bDir, INT nMouseTargetID, BOOL bIsDead, LPPOINT lpptPos, SHORT shSkill)
{
	if ( !m_bIsMon )
	{
		switch ( bMtn )
		{
			case _MT_WALK:
			case _MT_HORSEWALK:
			{
				if ( lpptPos )
				{
					POINT ptNext;

					for ( INT nCnt = 0; nCnt < _MAX_DIRECTION; nCnt++ )
					{
						m_pxMap->GetNextTileCanMove(lpptPos->x, lpptPos->y, nCnt, 1, &ptNext);
						if ( !m_pxMap->IsDoorOpen(ptNext.x, ptNext.y) )
						{
							g_xClientSocket.SendOpenDoor(ptNext.x, ptNext.y, m_pxMap->GetDoor(ptNext.x, ptNext.y));
							break;
						}
					}

					g_xClientSocket.SendActMsg(CM_WALK, lpptPos->x, lpptPos->y, bDir);
					m_bMotionLock = m_bInputLock = TRUE;
					m_wOldPosX = m_wPosX;
					m_wOldPosY = m_wPosY;
					m_bOldDir  = m_bCurrDir;

					SetMotionFrame(bMtn, bDir);
					m_bMoveSpeed = _SPEED_WALK;

					m_pxMap->ScrollMap(m_bMoveDir, m_dwCurrFrame-m_dwFstFrame, m_bMoveSpeed);
				}
				break;
			}
			case _MT_RUN:
			case _MT_HORSERUN:
			{
				if ( lpptPos )
				{
					POINT ptNext;
					POINT ptStart;

					if ( bMtn == _MT_RUN )
						m_bMoveSpeed = _SPEED_RUN;
					else
						m_bMoveSpeed = _SPEED_HORSERUN;
					
					m_pxMap->GetNextTileCanMove(m_wPosX, m_wPosY, bDir, 1, &ptStart);
					for ( INT nCnt = 0; nCnt < _MAX_DIRECTION; nCnt++ )
					{
						for ( INT nSpeedCnt = 0; nSpeedCnt < m_bMoveSpeed; nSpeedCnt++ )
						{
							m_pxMap->GetNextTileCanMove(ptStart.x, ptStart.y, nCnt, nSpeedCnt+1, &ptNext);
							if ( !m_pxMap->IsDoorOpen(ptNext.x, ptNext.y) )
							{
								g_xClientSocket.SendOpenDoor(ptNext.x, ptNext.y, m_pxMap->GetDoor(ptNext.x, ptNext.y));
								break;
							}
						}
					}

					g_xClientSocket.SendActMsg(CM_RUN, lpptPos->x, lpptPos->y, bDir);
					m_bMotionLock = m_bInputLock = TRUE;
					m_wOldPosX = m_wPosX;
					m_wOldPosY = m_wPosY;
					m_bOldDir  = m_bCurrDir;

					SetMotionFrame(bMtn, bDir);

					m_pxMap->ScrollMap(m_bMoveDir, m_dwCurrFrame-m_dwFstFrame, m_bMoveSpeed);			
				}
				break;
			}
			case _MT_ONEHSWING:
			{
				if ( m_bAttackMode == _MT_ONEHSWING || m_bAttackMode == _MT_ONEVSWING )
				{
					BYTE bAttackStyle = rand()%2;
					if ( bAttackStyle )			m_bAttackMode = _MT_ONEVSWING;
					else						m_bAttackMode = _MT_ONEHSWING;

					m_bAttackMode = _MT_ONEVSWING;
				}			

				if ( lpptPos && CanNextHit() )
				{
					m_bMotionLock	= m_bInputLock = TRUE;
					m_wOldPosX		= m_wPosX;
					m_wOldPosY		= m_wPosY;
					m_bOldDir		= m_bCurrDir;
					m_bWarMode		= TRUE;
					m_dwWarModeTime	= 0;

					if ( m_bUseErgum && g_xGameProc.TargetInLongAttack(bDir) )
					{
						SetMotionFrame(_MT_ONEVSWING, bDir);
						LoadEffect(&g_xGameProc.m_xImage, _SKILL_ERGUM, bDir);
						m_bUseSwordEffect = TRUE;
						g_xClientSocket.SendActMsg(CM_LONGHIT, lpptPos->x, lpptPos->y, bDir);
					}
					else if ( m_bFireHitCnt == 1 && m_stAbility.wMP > 7 )
					{
						SetMotionFrame(_MT_ONEVSWING, bDir);
						LoadEffect(&g_xGameProc.m_xImage, _SKILL_FIRESWORD, bDir);
						m_bUseSwordEffect = TRUE;
						g_xClientSocket.SendActMsg(CM_FIREHIT, lpptPos->x, lpptPos->y, bDir);
						m_bFireHitCnt = 2;
					}
					else if ( m_bYedoCnt == 1 )
					{
						SetMotionFrame(_MT_ONEVSWING, bDir);
						LoadEffect(&g_xGameProc.m_xImage, _SKILL_YEDO, bDir);
						m_bUseSwordEffect = TRUE;
						g_xClientSocket.SendActMsg(CM_POWERHIT, lpptPos->x, lpptPos->y, bDir);
						m_bYedoCnt = 2;
					}
					else if ( m_bUseBanwol && m_stAbility.wMP > 3 )
					{
						SetMotionFrame(_MT_ONEHSWING, bDir);
						LoadEffect(&g_xGameProc.m_xImage, _SKILL_BANWOL, bDir);
						m_bUseSwordEffect = TRUE;
						g_xClientSocket.SendActMsg(CM_WIDEHIT, lpptPos->x, lpptPos->y, bDir);
					}
					else
					{
						SetMotionFrame(m_bAttackMode, bDir);
						if ( m_bAttackMode == _MT_WHEELWIND )
						{
							LoadEffect(&g_xGameProc.m_xImage, _SKILL_JUMPSHOT, bDir);
							m_bUseSwordEffect = TRUE;
						}
						else if ( m_bAttackMode == _MT_RANDSWING )
						{
							LoadEffect(&g_xGameProc.m_xImage, _SKILL_RANDSWING, bDir);
							m_bUseSwordEffect = TRUE;
						}
						WORD wAttackStyle;
						if ( m_bAttackMode != _MT_ONEVSWING && m_bAttackMode != _MT_ONEHSWING )
							wAttackStyle = _MT_ONEVSWING;
						else
						{
							wAttackStyle = m_bAttackMode;
						}
						g_xClientSocket.SendHitMsg(CM_HIT, lpptPos->x, lpptPos->y, bDir, wAttackStyle);
					}
				}
				break;
			}
			case _MT_MOODEPO:
			{
				if ( lpptPos )		
				{
					g_xClientSocket.SendSpellMsg(shSkill, lpptPos->x, lpptPos->y, 0);
					// 무태보 시간값 기록.
					m_dwLastRushTime = timeGetTime();
					m_bMotionLock = m_bInputLock = TRUE;
					m_bWarMode	= TRUE;
				}

				break;
			}
			case _MT_SPELL2:
			case _MT_SPELL1:
			{
				if ( lpptPos )
				{
					g_xClientSocket.SendSpellMsg(shSkill, lpptPos->x, lpptPos->y, nMouseTargetID);

					m_wOldPosX = m_wPosX;
					m_wOldPosY = m_wPosY;
					m_bOldDir  = m_bCurrDir;

					m_bMotionLock = m_bInputLock = TRUE;
					m_bWarMode	= TRUE;

					if ( m_shCurrMagicID == _SKILL_FIREBALL || m_shCurrMagicID == _SKILL_FIREBALL2 || m_shCurrMagicID == _SKILL_FIRE || m_shCurrMagicID == _SKILL_SHOOTLIGHTEN ||
						 m_shCurrMagicID == _SKILL_HANGMAJINBUB || m_shCurrMagicID== _SKILL_DEJIWONHO || m_shCurrMagicID == _SKILL_FIRECHARM || m_shCurrMagicID == _SKILL_SINSU ||
						 m_shCurrMagicID == _SKILL_BIGCLOAK )
						LoadEffect(&g_xGameProc.m_xImage, m_shCurrMagicID, bDir);
					else
						LoadEffect(&g_xGameProc.m_xImage, m_shCurrMagicID);

					m_bUseEffect = TRUE;

/*					if ( m_shCurrMagicID == _SKILL_SHIELD )
					{
						m_dwFstEffectFrame = 50;
						m_dwEndEffectFrame = 60;
						m_dwCurrEffectFrame = 50;
						m_bEffectFrame = 0;
						m_bEffectFrameCnt = _DEFAULT_SPELLFRAME;
					}
*/
 					if ( m_shCurrMagicID == _SKILL_SHOWHP )
						m_bEffectFrameCnt = 20;
					else if ( m_shCurrMagicID == _SKILL_LIGHTFLOWER )
						m_bEffectFrameCnt = 15;
					else if ( m_shCurrMagicID == _SKILL_SPACEMOVE )
						m_bEffectFrameCnt = 19;
					else if ( m_shCurrMagicID == _SKILL_LIGHTENING )			
						m_bEffectFrameCnt = 17;


					SetMotionFrame(bMtn, bDir);
				}
				break;
			}
			case _MT_CUT:
			{
				m_bInputLock = TRUE;
				m_bMotionLock = TRUE;
				m_wOldPosX = m_wPosX;
				m_wOldPosY = m_wPosY;
				m_bOldDir  = m_bCurrDir;
				SetMotionFrame(bMtn, bDir);

				g_xClientSocket.SendActMsg(CM_SITDOWN, m_wPosX, m_wPosY, bDir);
				if ( nMouseTargetID && bIsDead )
				{
					g_xClientSocket.SendButchAnimal(m_wPosX, m_wPosY, bDir, nMouseTargetID);
				}
				break;
			}
			case _MT_HORSESTAND:
			case _MT_STAND:
			{
				if ( bDir != m_bCurrDir )	
				{
					g_xClientSocket.SendActMsg(CM_TRUN, m_wPosX, m_wPosY, bDir);
					m_bMotionLock = TRUE;
					m_bInputLock = TRUE;
					m_wOldPosX = m_wPosX;
					m_wOldPosY = m_wPosY;
					m_bOldDir  = m_bCurrDir;
					SetMotionFrame(bMtn, bDir);
				}
				break;
			}
		}
	}
	else
	{
		switch ( bMtn )
		{
			case _MT_HORSESTAND:
			case _MT_STAND:
			{
				bMtn = _MT_MON_STAND;
				if ( bDir != m_bCurrDir )			g_xClientSocket.SendActMsg(CM_TRUN, m_wPosX, m_wPosY, bDir);
	//			m_bMotionLock = TRUE;
				m_wOldPosX = m_wPosX;
				m_wOldPosY = m_wPosY;
				m_bOldDir  = m_bCurrDir;
				SetMotionFrame(bMtn, bDir);
				break;
			}
			case _MT_WALK:
			case _MT_HORSEWALK:
			{
				bMtn = _MT_MON_WALK;
				g_xClientSocket.SendActMsg(CM_WALK, m_wPosX, m_wPosY, bDir);
				m_bMotionLock = m_bInputLock = TRUE;
				m_wOldPosX = m_wPosX;
				m_wOldPosY = m_wPosY;
				m_bOldDir  = m_bCurrDir;

				SetMotionFrame(bMtn, bDir);
				m_bMoveSpeed = _SPEED_WALK;
				m_pxMap->ScrollMap(m_bMoveDir, m_dwCurrFrame-m_dwFstFrame, m_bMoveSpeed);
				break;
			}
			case _MT_ONEHSWING:
			{
				bMtn = _MT_MON_ATTACK_A;
				m_bMotionLock	= m_bInputLock = TRUE;
				m_wOldPosX		= m_wPosX;
				m_wOldPosY		= m_wPosY;
				m_bOldDir		= m_bCurrDir;
				m_bWarMode		= TRUE;
				m_dwWarModeTime	= 0;

				SetMotionFrame(_MT_MON_ATTACK_A, bDir);
				g_xClientSocket.SendHitMsg(CM_HIT, lpptPos->x, lpptPos->y, bDir, 0);
				break;
			}
		}
	}

	m_dwMotionLockTime	= 0;
}



/******************************************************************************************************************

	함수명 : CMyHero::UpdateMotionState()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : INT nLoopTime
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CMyHero::UpdateMotionState(INT nLoopTime, BOOL bIsMoveTime)
{
	if ( m_bMotionLock ) 
		m_dwMotionLockTime += nLoopTime;

	if ( m_bWarMode ) 
		m_dwWarModeTime += nLoopTime;

	m_wABlendCurrDelay += nLoopTime;
	if ( m_wABlendCurrDelay >= m_wABlendDelay )
	{
		m_wABlendCurrDelay = 0;
		m_wABlendDelay	   = 0;
		m_bABlendRev	   = FALSE;
	}

	if ( m_bCurrMtn == _MT_DIE && m_dwCurrFrame >= m_dwEndFrame-1 )
	{
		m_bIsDead = TRUE;
	}

	if ( m_bIsDead )
	{
		if ( m_bIsMon )
			SetMotionFrame(_MT_MON_DIE, m_bCurrDir);
		else
			SetMotionFrame(_MT_DIE, m_bCurrDir);

		m_dwCurrFrame = m_dwEndFrame - 1;
		m_bInputLock  = TRUE;
		return;
	}


	if ( UpdateMove(bIsMoveTime) )
	{
		UpdatePacketState();
		return;
	}
	else
	{
		if ( !m_bIsMon )
		{
			m_wCurrDelay += nLoopTime;

			m_wShieldCurrDelay += nLoopTime;

			if ( m_wShieldCurrDelay > 150 )  
			{
				m_bShieldCurrFrm++;
				m_wShieldCurrDelay = 0;
				if ( m_bShieldCurrFrm > 2 )
					m_bShieldCurrFrm = 0;
			}

			if ( m_wCurrDelay > m_wDelay )
			{
				m_wCurrDelay  = 0;

				if ( m_dwCurrFrame < m_dwEndFrame )
				{
					m_dwCurrFrame++;

					PlayActSound();

					if ( (m_bCurrMtn == _MT_SPELL2 || m_bCurrMtn == _MT_SPELL1) && m_bUseEffect )
					{
						m_dwCurrEffectFrame++;
						m_bEffectFrame++;
					}
				}
			}

			UpdatePacketState();

			// 연속적인 프레임 중에서 해야할일.
			if ( m_dwMotionLockTime > _MOTION_LOCKTIME )
			{
				m_dwMotionLockTime = 0;
				m_bMotionLock	   = FALSE;
//				SetOldPosition();
			}

			if ( m_dwWarModeTime > _WARMODE_TIME )
			{
				m_dwWarModeTime = 0;
				m_bWarMode		= FALSE;
			}

			if ( m_dwCurrFrame >= m_dwEndFrame-1 )
			{
				if ( (m_bCurrMtn == _MT_SPELL2) && m_bUseEffect )
				{
					if ( m_dwCurrEffectFrame - m_dwFstEffectFrame < m_bEffectFrameCnt-2 )
					{
						m_dwCurrFrame = m_dwEndFrame - 2;
					}
				}
			}
			else if ( m_dwCurrFrame >= m_dwEndFrame-3 )
			{
				if ( (m_bCurrMtn == _MT_SPELL1) && m_bUseEffect )
				{
					if ( m_dwCurrEffectFrame - m_dwFstEffectFrame < m_bEffectFrameCnt-5 )
					{
						m_dwCurrFrame = m_dwEndFrame - 4;
					}
				}
			}

			if ( m_dwCurrFrame >= m_dwEndFrame )
			{
				switch ( m_bCurrMtn )
				{
				case _MT_CUT:
					{
						m_bInputLock  = FALSE;
						m_bMotionLock = FALSE;
						m_dwCurrFrame = m_dwFstFrame;
						if ( m_stFeatureEx.bHorse == _HORSE_NONE )		SetMotionFrame(_MT_STAND, m_bCurrDir);
						else											SetMotionFrame(_MT_HORSESTAND, m_bCurrDir);
					}
					break;
				case _MT_STAND:
				case _MT_HORSESTAND:
					{
						m_bInputLock  = FALSE;
						m_bMotionLock = FALSE;
						m_bUseEffect = FALSE;
						m_bUseSwordEffect = FALSE;
						m_dwCurrFrame = m_dwFstFrame;
						break;
					}
				case _MT_ATTACKMODE:
					{
						if ( !m_bWarMode )
							SetMotionFrame(_MT_STAND, m_bCurrDir);
						else
						{
							m_bInputLock  = FALSE;
							m_bMotionLock = FALSE;
							m_dwCurrFrame = m_dwFstFrame;
						}
						break;
					}
				default:
					{
						m_dwCurrEffectFrame = 0;
						m_dwFstEffectFrame = 0;
						m_dwEndEffectFrame = 0;
						m_bEffectFrame = 0;
						m_bEffectFrameCnt = _DEFAULT_SPELLFRAME;
						m_bUseEffect = FALSE;
						m_bUseSwordEffect = FALSE;
						if ( !m_bMotionLock )
						{				
							m_bInputLock = FALSE;

							if ( m_stFeatureEx.bHorse == _HORSE_NONE )
							{
								if ( m_bWarMode )
									SetMotionFrame(_MT_ATTACKMODE, m_bCurrDir);
								else
									SetMotionFrame(_MT_STAND, m_bCurrDir);
							}
							else
							{
								SetMotionFrame(_MT_HORSESTAND, m_bCurrDir);
							}
						}
						else
						{
							m_dwCurrFrame = m_dwEndFrame-1;
						}
					}
					break;
				}
			}

			if ( m_bCurrMtn == _MT_STAND || m_bCurrMtn == _MT_HORSESTAND )
			{
				POINT ptMouse;
				GetCursorPos(&ptMouse);
				ScreenToClient(g_xMainWnd.GetSafehWnd(), &ptMouse);
				m_bCanRun = FALSE;

				if ( HIBYTE(GetKeyState(VK_RBUTTON)) || ( HIBYTE(GetKeyState(VK_LBUTTON)) && HIBYTE(GetKeyState(VK_CONTROL))) )
				{
					OnRButtonDown(ptMouse);
				}
				else if ( HIBYTE(GetKeyState(VK_LBUTTON)) )
				{
					OnLButtonDown(ptMouse);

/*					LPARAM lParam = MAKELPARAM((WORD)ptMouse.x, (WORD)ptMouse.y);
					WPARAM wParam = 0;
					g_xGameProc.OnLButtonDown(wParam, lParam);
*/				}
			}
		}
		else
		{
			m_wCurrDelay += nLoopTime;
			m_wShieldCurrDelay += nLoopTime;
			m_wABlendCurrDelay += nLoopTime;

			if ( m_wShieldCurrDelay > 150 )  
			{
				m_bShieldCurrFrm++;
				m_wShieldCurrDelay = 0;
				if ( m_bShieldCurrFrm > 2 )
					m_bShieldCurrFrm = 0;
			}

			if ( m_wCurrDelay > m_wDelay )
			{
				m_wCurrDelay  = 0;

				if ( m_dwCurrFrame < m_dwEndFrame )
				{
					m_dwCurrFrame++;
				}
			}

			UpdatePacketState();

			// 연속적인 프레임 중에서 해야할일.
			if ( m_dwMotionLockTime > _MOTION_LOCKTIME )
			{
				m_dwMotionLockTime = 0;
				m_bMotionLock	   = FALSE;
//				SetOldPosition();
			}

			if ( m_dwWarModeTime > _WARMODE_TIME )
			{
				m_dwWarModeTime = 0;
				m_bWarMode		= FALSE;
			}


			if ( m_dwCurrFrame >= m_dwEndFrame )
			{
				switch ( m_bCurrMtn )
				{
				case _MT_MON_STAND:
					{
						m_bInputLock  = FALSE;
						m_bMotionLock = FALSE;
						m_bUseEffect = FALSE;
						m_bUseSwordEffect = FALSE;
						m_dwCurrFrame = m_dwFstFrame;
						break;
					}
				default:
					{
						m_dwCurrEffectFrame = 0;
						m_dwFstEffectFrame = 0;
						m_dwEndEffectFrame = 0;
						m_bEffectFrame = 0;
						m_bEffectFrameCnt = _DEFAULT_SPELLFRAME;
						m_bUseEffect = FALSE;
						m_bUseSwordEffect = FALSE;
						if ( !m_bMotionLock )
						{				
							m_bInputLock = FALSE;
							SetMotionFrame(_MT_MON_STAND, m_bCurrDir);
						}
						else
						{
							m_dwCurrFrame = m_dwEndFrame-1;
						}
					}
					break;
				}
			}

			if ( m_bCurrMtn == _MT_MON_STAND )
			{
				POINT ptMouse;
				GetCursorPos(&ptMouse);
				ScreenToClient(g_xMainWnd.GetSafehWnd(), &ptMouse);
				m_bCanRun = FALSE;

				if ( HIBYTE(GetKeyState(VK_RBUTTON)) || ( HIBYTE(GetKeyState(VK_LBUTTON)) && HIBYTE(GetKeyState(VK_CONTROL))) )
				{
					OnRButtonDown(ptMouse);
				}
				else if ( HIBYTE(GetKeyState(VK_LBUTTON)) )
				{
					OnLButtonDown(ptMouse);
/*					LPARAM lParam = MAKELPARAM((WORD)ptMouse.x, (WORD)ptMouse.y);
					WPARAM wParam = 0;
					g_xGameProc.OnLButtonDown(wParam, lParam);
*/				}
			}
		}
	}
}




/******************************************************************************************************************

	함수명 : CMyHero::UpdateMove(BOOL bIsMoveTime)

	작성자 : 
	작성일 : 

	목적   : 
	입력   : BOOL bIsMoveTime
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CMyHero::UpdateMove(BOOL bIsMoveTime)
{
	if ( !m_bIsMon )
	{
		if ( m_bCurrMtn == _MT_WALK || m_bCurrMtn == _MT_RUN || m_bCurrMtn == _MT_HORSEWALK || m_bCurrMtn == _MT_HORSERUN || m_bCurrMtn == _MT_MOODEPO || m_bCurrMtn == _MT_PUSHBACK)
		{
			m_wCurrDelay = 0;

			if ( bIsMoveTime )
			{
				if ( m_bCurrMtn == _MT_PUSHBACK )
				{
					m_bBackStepFrame += 2;
					if ( m_bBackStepFrame >= m_bBackStepFrameCnt )
					{
						m_dwCurrFrame++;
						m_bBackStepFrame = m_bBackStepFrameCnt;
					}
				}
				else if ( m_bCurrMtn == _MT_MOODEPO )
				{
					m_dwCurrFrame += 2;
					if ( m_dwCurrFrame >= m_dwEndFrame )
					{
						m_dwCurrFrame = m_dwEndFrame;
					}
				}
				else
				{
					m_dwCurrFrame++;

					PlayMoveSound();
/*					if ( m_bMsgHurryCheck )			
					{
						m_dwCurrFrame++;
					}
*/				}

				if ( m_bCurrMtn == _MT_PUSHBACK )
				{
					if ( m_bBackStepFrame >= m_bBackStepFrameCnt-m_bMoveNextFrmCnt && !m_bIsMoved )
					{
						SetMoved();
						m_bIsMoved = TRUE;
					}
				}
				else
				{
					if ( m_dwCurrFrame >= m_dwEndFrame-m_bMoveNextFrmCnt && !m_bIsMoved )
					{
						SetMoved();
						m_bIsMoved = TRUE;
					}
				}
			}

			// 연속적인 프레임 중에서 해야할일.
			if ( m_dwCurrFrame >= m_dwEndFrame )
			{
				m_dwCurrFrame = m_dwEndFrame-1;
				m_bCanRun = FALSE;

				switch ( m_bCurrMtn )
				{
				case _MT_MOODEPO:
					{
						if ( m_bInputLock )
						{
							m_bIsMoved = FALSE;
							m_bInputLock = FALSE;
							m_pxMap->SetMovedTileBuffer(m_shShiftTileX, m_shShiftTileY);
							m_bMoveSpeed	= 0;
							m_shShiftPixelX	= 0;
							m_shShiftPixelY	= 0;
							m_shShiftTileX	= 0;
							m_shShiftTileY	= 0;
							// 이동후 좌표확인.
							if ( CheckMyPostion() == FALSE )
								AdjustMyPostion();

							if ( m_bWarMode )
								SetMotionFrame(_MT_ATTACKMODE, m_bCurrDir);
							else
								SetMotionFrame(_MT_STAND, m_bCurrDir);
						}
						break;
					}
				case _MT_PUSHBACK:
					{
						if ( m_bInputLock )
						{
							m_bIsMoved = FALSE;
							m_bInputLock = FALSE;
							m_pxMap->SetMovedTileBuffer(m_shShiftTileX, m_shShiftTileY);
							m_bMoveSpeed	= 0;
							m_shShiftPixelX	= 0;
							m_shShiftPixelY	= 0;
							m_shShiftTileX	= 0;
							m_shShiftTileY	= 0;

							m_bBackStepFrame = 0;
							m_bBackStepFrameCnt = 0;
							// 이동후 좌표확인.
							if ( CheckMyPostion() == FALSE )
								AdjustMyPostion();

							if ( m_bWarMode )
								SetMotionFrame(_MT_ATTACKMODE, m_bCurrDir);
							else
								SetMotionFrame(_MT_STAND, m_bCurrDir);
						}
						break;
					}
				case _MT_WALK:
				case _MT_HORSEWALK:
					{
						if ( !m_bMotionLock && m_bInputLock )
						{
							m_bIsMoved = FALSE;
							m_bInputLock = FALSE;
							m_pxMap->SetMovedTileBuffer(m_shShiftTileX, m_shShiftTileY);
							m_bMoveSpeed	= 0;
							m_shShiftPixelX	= 0;
							m_shShiftPixelY	= 0;
							m_shShiftTileX	= 0;
							m_shShiftTileY	= 0;

							if ( m_bCurrMtn == _MT_WALK && m_stFeatureEx.bHorse == _HORSE_NONE )
							{
								if ( m_bWarMode )
								{
									SetMotionFrame(_MT_ATTACKMODE, m_bCurrDir);
								}
								else		SetMotionFrame(_MT_STAND, m_bCurrDir);
							}
							else	SetMotionFrame(_MT_HORSESTAND, m_bCurrDir);

							// 이동후 좌표확인.
							if ( CheckMyPostion() == FALSE )
								AdjustMyPostion();
							POINT ptMouse;
							GetCursorPos(&ptMouse);
							ScreenToClient(g_xMainWnd.GetSafehWnd(), &ptMouse);
							if ( HIBYTE(GetKeyState(VK_RBUTTON)) || ( HIBYTE(GetKeyState(VK_LBUTTON)) && HIBYTE(GetKeyState(VK_CONTROL))) )
							{
								m_bCanRun = TRUE;
								OnRButtonDown(ptMouse);
							}
							else if ( HIBYTE(GetKeyState(VK_LBUTTON)) )
							{
								OnLButtonDown(ptMouse);
							}
						}
						break;
					}
				case _MT_RUN:
				case _MT_HORSERUN:
					{
						if ( !m_bMotionLock && m_bInputLock )
						{				
							m_bIsMoved = FALSE;
							m_bInputLock = FALSE;
							m_pxMap->SetMovedTileBuffer(m_shShiftTileX, m_shShiftTileX);
							m_bMoveSpeed	= 0;
							m_shShiftPixelX	= 0;
							m_shShiftPixelY	= 0;
							m_shShiftTileX	= 0;
							m_shShiftTileY	= 0;

							if ( m_bCurrMtn == _MT_RUN && m_stFeatureEx.bHorse == _HORSE_NONE )
							{
								if ( m_bWarMode )
								{
									SetMotionFrame(_MT_ATTACKMODE, m_bCurrDir);
								}
								else	SetMotionFrame(_MT_STAND, m_bCurrDir);
							}
							else	SetMotionFrame(_MT_HORSESTAND, m_bCurrDir);

							if ( CheckMyPostion() == FALSE )
								AdjustMyPostion();
							POINT ptMouse;
							GetCursorPos(&ptMouse);
							ScreenToClient(g_xMainWnd.GetSafehWnd(), &ptMouse);

							if ( HIBYTE(GetKeyState(VK_RBUTTON)) || ( HIBYTE(GetKeyState(VK_LBUTTON)) && HIBYTE(GetKeyState(VK_CONTROL))) )
							{
								m_bCanRun = TRUE;
								OnRButtonDown(ptMouse);
							}
							else if ( HIBYTE(GetKeyState(VK_LBUTTON)) )
							{
								OnLButtonDown(ptMouse);
							}
						}
						break;
					}
				}
			}
			else
			{
				switch ( m_bCurrMtn )
				{
				case _MT_WALK:
				case _MT_RUN:
				case _MT_HORSEWALK:
				case _MT_HORSERUN:
				case _MT_MOODEPO:
					m_pxMap->ScrollMap(m_bMoveDir, m_dwCurrFrame-m_dwFstFrame, m_bMoveSpeed);			
//					m_pxMap->ScrollMap(m_bMoveDir, m_wCurrDelay, m_wDelay*(m_dwEndFrame-m_dwFstFrame), m_bMoveSpeed);
					break;
				case _MT_PUSHBACK:
					m_pxMap->ScrollMap(m_bMoveDir, m_bBackStepFrame, m_bMoveSpeed);			
					break;
				}
			}

			// 연속적인 프레임 중에서 해야할일.
			if ( m_dwMotionLockTime > _MOTION_LOCKTIME )
			{
				m_dwMotionLockTime = 0;
				m_bMotionLock	   = FALSE;
//				SetOldPosition();
			}

			if ( m_dwWarModeTime > _WARMODE_TIME )
			{
				m_dwWarModeTime = 0;
				m_bWarMode		= FALSE;
			}

			return TRUE;
		}
	}
	else
	{
		if ( m_bCurrMtn == _MT_MON_WALK )
		{
			m_wCurrDelay = 0;

			if ( bIsMoveTime )
			{
				m_dwCurrFrame++;

				if ( m_dwCurrFrame >= m_dwEndFrame-m_bMoveNextFrmCnt && !m_bIsMoved )
				{
					SetMoved();
					m_bIsMoved = TRUE;
				}
			}

			if ( m_dwCurrFrame >= m_dwEndFrame )
			{
				m_dwCurrFrame = m_dwEndFrame-1;
				m_bCanRun = FALSE;
				switch ( m_bCurrMtn )
				{
				case _MT_MON_WALK:
					{
						if ( !m_bMotionLock && m_bInputLock )
						{
							m_bIsMoved = FALSE;
							m_bInputLock = FALSE;
							m_pxMap->SetMovedTileBuffer(m_shShiftTileX, m_shShiftTileY);
							m_bMoveSpeed	= 0;
							m_shShiftPixelX	= 0;
							m_shShiftPixelY	= 0;
							m_shShiftTileX	= 0;
							m_shShiftTileY	= 0;
							
							SetMotionFrame(_MT_MON_STAND, m_bCurrDir);

							// 이동후 좌표확인.
							if ( CheckMyPostion() == FALSE )
								AdjustMyPostion();
							POINT ptMouse;
							GetCursorPos(&ptMouse);
							ScreenToClient(g_xMainWnd.GetSafehWnd(), &ptMouse);
							if ( HIBYTE(GetKeyState(VK_LBUTTON)) )
							{
								OnLButtonDown(ptMouse);
							}
						}
						break;
					}
				}
			}
			else
			{
				switch ( m_bCurrMtn )
				{
				case _MT_MON_WALK:
					m_pxMap->ScrollMap(m_bMoveDir, m_dwCurrFrame-m_dwFstFrame, m_bMoveSpeed);			
					break;
				}
			}

			// 연속적인 프레임 중에서 해야할일.
			if ( m_dwMotionLockTime > _MOTION_LOCKTIME )
			{
				m_dwMotionLockTime = 0;
				m_bMotionLock	   = FALSE;
//				SetOldPosition();
			}

			if ( m_dwWarModeTime > _WARMODE_TIME )
			{
				m_dwWarModeTime = 0;
				m_bWarMode		= FALSE;
			}

			return TRUE;
		}
	}

	return FALSE;
}





/******************************************************************************************************************

	함수명 : CMyHero:DrawActor()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : BOOL bFocused
	         BOOL bShadowAblended
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CMyHero::DrawActor(BOOL bFocused, BOOL bShadowAblended, BOOL bUseScrnPos, BOOL bDrawShadow)
{
	// 캐릭터 좌표는 고정.
	m_shScrnPosX = _CHAR_CENTER_XPOS - 24; 
	m_shScrnPosY = _CHAR_CENTER_YPOS - 16;

	if ( m_bIsMon )
	{
		if ( CActor::DrawActor(m_pxMap, bFocused, bShadowAblended, bUseScrnPos, bDrawShadow) )
			return TRUE;
	}
	else
	{
		if ( CHero::DrawActor(m_pxMap, bFocused, bShadowAblended, bUseScrnPos, bDrawShadow) )
			return TRUE;
	}

	return FALSE;
}



/******************************************************************************************************************

	함수명 : CMyHero::CalcDirection()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : INT nTargetTileX
	         INT nTargetTileY
	출력   : 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BYTE CMyHero::CalcDirection(INT nTargetTileX, INT nTargetTileY)
{
	INT nWidth	= nTargetTileX - m_wPosX;
	INT nHeight = nTargetTileY - m_wPosY;

	FLOAT	rLineLength, rBottomInTriangle;
	INT		nDimension;
	BYTE	bDir;
	rLineLength = (FLOAT)sqrt((double)(nWidth*nWidth+nHeight*nHeight));
	
	// 기본.
	// 7  0  1          
	// 6     2
	// 5  4  3
	// 일단은 4개의 분면(90도기준)으로 나누고 분면에 대한 cos값을 적용한다.
	( nWidth >= 0 ) ?
		( nHeight <  0 ? (rBottomInTriangle=(FLOAT)-nHeight, nDimension=0) : (rBottomInTriangle=(FLOAT) nWidth, nDimension=2) ):
		( nHeight >= 0 ? (rBottomInTriangle=(FLOAT) nHeight, nDimension=4) : (rBottomInTriangle=(FLOAT)-nWidth, nDimension=6) );
	// 6(cos45)  0(cos 0)  0(cos45)
	// 4(cos90)  2(cos 0)  2(cos 0)
	// 4(cos45)  2(cos90)  2(cos45)

	FLOAT rCosVal = rBottomInTriangle/rLineLength;
	
//	cos(0), cos(pi/8), cos(pi/4), cos(pi/2)
	CONST FLOAT rCosVal8[4] = { 1.0f, 0.923880f, 0.707107f, 0.382683f };
	
	// 각분면을 3개의 영역으로 나누어서 영역을 재조정한다.
	bDir = 0;
	for ( INT nCnt = 0; nCnt < 4; nCnt++ )
	{
		if( rCosVal <= rCosVal8[nCnt] )
			bDir = nDimension+(nCnt+1)/2;
		else break;
	}
	
	if( bDir >= 8 )					bDir = 0;
	
	return bDir;

}



/******************************************************************************************************************

	함수명 : CMyHero::GetPosMouseToTile()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : INT nXPos
	         INT nYPos
	출력   : POINT 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
POINT CMyHero::GetPosMouseToTile(INT nXPos, INT nYPos)
{
	POINT ptTilePos;

	if ( m_pxMap )
	{
		ptTilePos.x = m_pxMap->m_shStartViewTileX + (nXPos - _VIEW_CELL_X_START) / _CELL_WIDTH;
		ptTilePos.y = m_pxMap->m_shStartViewTileY + (nYPos - _VIEW_CELL_Y_START) / _CELL_HEIGHT;
	}

	return ptTilePos;
}


BOOL CMyHero::CanNextHit()
{
	DWORD	dwCurrTick;
	INT		nNextHit, nLevelFast;
	BOOL	bAttackSlow = FALSE;

	nLevelFast = min(370, m_stAbility.bLevel*14);
	nLevelFast = min(800, nLevelFast + m_shHitSpeed * 60);

	if ( m_stAbility.bHandWeight > m_stAbility.bMaxHandWeight )		
		bAttackSlow = TRUE;

	if ( bAttackSlow )		nNextHit = 1400 - nLevelFast + 1500;
	else					nNextHit = 1400 - nLevelFast;

	dwCurrTick = timeGetTime();

	if ( dwCurrTick - m_dwLastHitTime > nNextHit )
	{
		m_dwLastHitTime = dwCurrTick;
		return TRUE;
	}

	return FALSE;
}


BOOL CMyHero::CanWalk()
{
   if ( timeGetTime() - m_dwLastSpellTime < m_wMagicPKDelayTime )
      return TRUE;

   return FALSE;
}


BOOL CMyHero::CanRun()
{
   if ( m_stAbility.wHP < _RUN_MINHEALTH )
      return FALSE;

   if ( (timeGetTime() - m_dwLastPKStruckTime < 3000) || (timeGetTime() - m_dwLastSpellTime < m_wMagicPKDelayTime ) )
      return FALSE;

   return TRUE;
}





//---------------------------------------------------------------------------------------------------------------//
//  유저 입력함수.
//---------------------------------------------------------------------------------------------------------------//
/******************************************************************************************************************

	함수명 : CMyHero::OnLButtonDown()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : POINT ptMouse
	         INT nTargetID
	         POINT* lpptTargetPos
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CMyHero::OnLButtonDown(POINT ptMouse, INT nTargetID, BOOL bIsDead, POINT* lpptTargetPos)
{
	POINT ptTargetTile;
	POINT ptMouseTilePos;
	BYTE  bDir;

	if ( g_xGameProc.m_xInterface.OnLButtonDown(ptMouse) )
		return FALSE;

	if ( m_wABlendDelay )
		return FALSE;

	if ( !m_bMotionLock && !m_bInputLock )
	{
		SHORT shLeftMsgCnt = m_xPacketQueue.GetCount();
		if ( shLeftMsgCnt > 0 )
		{
			UpdatePacketState();
			return FALSE;
		}
		
		ptMouseTilePos = GetPosMouseToTile(ptMouse.x, ptMouse.y);
		bDir		   = CalcDirection(ptMouseTilePos.x, ptMouseTilePos.y);

		if ( m_wPosX == ptMouseTilePos.x && m_wPosY == ptMouseTilePos.y )
			bDir = m_bCurrDir;

		INT nLength;
		if ( lpptTargetPos ) 
			nLength = (INT)sqrt((double)(m_wPosX-lpptTargetPos->x)*(m_wPosX-lpptTargetPos->x) + (m_wPosY-lpptTargetPos->y)*(m_wPosY-lpptTargetPos->y));
		else
			nLength = (INT)sqrt((double)(m_wPosX-ptMouseTilePos.x)*(m_wPosX-ptMouseTilePos.x) + (m_wPosY-ptMouseTilePos.y)*(m_wPosY-ptMouseTilePos.y));


		// 1. SHIFT.(강제 공격)
		if ( HIBYTE(GetKeyState(VK_SHIFT)) )
		{
			if ( m_stFeatureEx.bHorse == _HORSE_NONE )
			{
				if ( m_bUseErgum && g_xGameProc.TargetInLongAttack(bDir) )
				{
					if ( m_stFeatureEx.bHorse == _HORSE_NONE && !bIsDead )
					{
						// 공격한다.
						ptTargetTile.x = m_wPosX;
						ptTargetTile.y = m_wPosY;
						SetMotionState(_MT_ONEHSWING, bDir, nTargetID, bIsDead, &ptTargetTile);
					}
				}
				else
				{
					ptTargetTile.x = m_wPosX;
					ptTargetTile.y = m_wPosY;
					SetMotionState(_MT_ONEHSWING, bDir, nTargetID, bIsDead, &ptTargetTile);				
				}
			}
		}
		// 2. ALT.(썰기)
		else if ( HIBYTE(GetKeyState(VK_MENU)) )
		{
			SetMotionState(_MT_CUT, bDir, nTargetID, bIsDead, &ptTargetTile);
		}
		/*
		// 3. 클릭한타일이 내 캐릭터가 위치한 곳인가?
		else if ()
		{
			// 아이템이 있는가?
			if ()
			{
				// 아이템을 줍는다.
			}
		}*/
		// 4. 클릭한 좌표가 Actor 영역 안에 있는가?
		else if ( nTargetID != -1 && lpptTargetPos )
		{
			//1. 타겟 타일의 위치가 1타일 이내인가?
			if ( nLength < 2 && nLength > 0 ) 
			{
				if ( m_stFeatureEx.bHorse == _HORSE_NONE && !bIsDead )
				{
					// 공격한다.
					ptTargetTile.x = m_wPosX;
					ptTargetTile.y = m_wPosY;
					bDir = CalcDirection(lpptTargetPos->x, lpptTargetPos->y);
					SetMotionState(_MT_ONEHSWING, bDir, nTargetID, bIsDead, &ptTargetTile);

				}
			}
			// Actor가 있는 방향으로 이동 가능한가?(맵속성체크) 
			else if ( m_pxMap->GetNextTileCanMove(m_wPosX, m_wPosY, bDir, 1, &ptTargetTile) && nLength > 0)
			{
				// 1타일 이동한다.
				if ( m_stFeatureEx.bHorse == _HORSE_NONE )		SetMotionState(_MT_WALK, bDir, nTargetID, bIsDead, &ptTargetTile);
				else											SetMotionState(_MT_HORSEWALK, bDir, nTargetID, bIsDead, &ptTargetTile);
			}
			else
			{
				if ( m_stFeatureEx.bHorse == _HORSE_NONE )		SetMotionState(_MT_STAND, bDir);
				else											SetMotionState(_MT_HORSESTAND, bDir);

			}
			/*// Actor가 NPC인가?
			if ()
			{
				// 대화창이나 상점창을 연다.
			}
			// Actor가 공격 가능한가?
			else if()
			{
				// Actor의 위치가 1타일 내인가?
				if ()
				{
					// 공격한다.
				}
				// Actor가 있는 방향으로 이동 가능한가?(맵속성체크) 
				else if ()
				{
					// 1타일 이동한다.
				}
			}*/
		}
		// 6. 현재 마우스타일방향으로 이동 가능한가?(맵속성체크)
		else if ( m_pxMap->GetNextTileCanMove(m_wPosX, m_wPosY, bDir, 1, &ptTargetTile) && nLength > 0 )
		{
			// 1타일 이동한다.
			if ( m_stFeatureEx.bHorse == _HORSE_NONE )			SetMotionState(_MT_WALK, bDir, nTargetID, bIsDead, &ptTargetTile);
			else												SetMotionState(_MT_HORSEWALK, bDir, nTargetID, bIsDead, &ptTargetTile);

		}
		// 7. 이동 할수 없다면 주변에 이동할수 있는 타일을 찾아내어 목적 타일과의 가장 가까운 타일을 찾아낸다.
		else if ( !m_pxMap->GetNextTileCanMove(m_wPosX, m_wPosY, bDir, 1, &ptTargetTile) )
		{
			INT nSelectedDir = -1;
			INT nDistance;
			INT nSelectedDistance = 100;
			for ( INT nCnt = 0; nCnt < _MAX_DIRECTION; nCnt++ )
			{
				if ( m_pxMap->GetNextTileCanMove(m_wPosX, m_wPosY, nCnt, 1, &ptTargetTile) )
				{
					nDistance = (INT)(sqrt((double)(ptTargetTile.x-ptMouseTilePos.x)*(ptTargetTile.x-ptMouseTilePos.x) + 
									  (ptTargetTile.y-ptMouseTilePos.y)*(ptTargetTile.y-ptMouseTilePos.y)));
					if ( nDistance <= nSelectedDistance )
					{
						nSelectedDistance = nDistance;
						nSelectedDir = nCnt;
					}
				}
			}

			if ( nSelectedDir != -1 && nDistance > 0 )
			{
				// 1타일 이동한다.
				m_pxMap->GetNextTileCanMove(m_wPosX, m_wPosY, nSelectedDir, 1, &ptTargetTile);
				if ( m_stFeatureEx.bHorse == _HORSE_NONE )		SetMotionState(_MT_WALK, nSelectedDir, nTargetID, bIsDead, &ptTargetTile);
				else											SetMotionState(_MT_HORSEWALK, nSelectedDir, nTargetID, bIsDead, &ptTargetTile);
			}
			else
			{
				if ( m_stFeatureEx.bHorse == _HORSE_NONE )		SetMotionState(_MT_STAND, bDir);
				else											SetMotionState(_MT_HORSESTAND, bDir);
			}
		}
		else
		{
			if ( m_stFeatureEx.bHorse == _HORSE_NONE )		SetMotionState(_MT_STAND, bDir);
			else											SetMotionState(_MT_HORSESTAND, bDir);
		}
	}

	return FALSE;
}



/******************************************************************************************************************

	함수명 : CMyHero::OnRButtonDown()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : POINT ptMouse
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CMyHero::OnRButtonDown(POINT ptMouse)
{
	POINT ptTargetTile;
	POINT ptMouseTilePos;
	BYTE  bDir;

	if ( m_wABlendDelay )
		return FALSE;


	if ( !m_bMotionLock && !m_bInputLock )
	{
		SHORT shLeftMsgCnt = m_xPacketQueue.GetCount();
		if ( shLeftMsgCnt > 0 )
		{
			UpdatePacketState();
			return FALSE;
		}
		
		ptMouseTilePos = GetPosMouseToTile(ptMouse.x, ptMouse.y);
		bDir		   = CalcDirection(ptMouseTilePos.x, ptMouseTilePos.y);			

		//1. 타겟 타일의 위치가 1타일 이내인가?
		if ( (INT)(sqrt((double)(m_wPosX-ptMouseTilePos.x)*(m_wPosX-ptMouseTilePos.x) + (m_wPosY-ptMouseTilePos.y)*(m_wPosY-ptMouseTilePos.y)))
			 < 2 ) 
		{
			// 방향을 바꾼다.
			if ( m_stFeatureEx.bHorse == _HORSE_NONE )		SetMotionState(_MT_STAND, bDir);
			else											SetMotionState(_MT_HORSESTAND, bDir);
		}
		//2. 움직인다.
		else
		{
			// 달릴수 있을때.
			if ( m_bCanRun )
			{
				m_bCanRun = FALSE;

				if ( m_stFeatureEx.bHorse != _HORSE_NONE )
				{
					// 목적지타일과의 방향으로 1, 2, 3타일째의 위치에 방해물이 없을때.
					if ( m_pxMap->GetNextTileCanMove(m_wPosX, m_wPosY, bDir, 1, &ptTargetTile) && 
						 m_pxMap->GetNextTileCanMove(m_wPosX, m_wPosY, bDir, 2, &ptTargetTile) &&
						 m_pxMap->GetNextTileCanMove(m_wPosX, m_wPosY, bDir, 3, &ptTargetTile) )
					{
						// 3타일 이동한다.
						m_pxMap->GetNextTileCanMove(m_wPosX, m_wPosY, bDir, 3, &ptTargetTile);
						SetMotionState(_MT_HORSERUN, bDir, 0, FALSE, &ptTargetTile);
					}
					// 목적지타일과의 방향으로 1타일째의 위치에 방해물이 없을때.
					else if ( m_pxMap->GetNextTileCanMove(m_wPosX, m_wPosY, bDir, 1, &ptTargetTile) )
					{
						SetMotionState(_MT_HORSEWALK, bDir, 0, FALSE, &ptTargetTile);
					}
					// 목적지타일과의 방향으로 1타일째의 위치에 방해물이 있을때.
					else
					{
						INT nSelectedDir = -1;
						INT nDistance;
						INT nSelectedDistance = 100;
						for ( INT nCnt = 0; nCnt < _MAX_DIRECTION; nCnt++ )
						{
							if ( m_pxMap->GetNextTileCanMove(m_wPosX, m_wPosY, nCnt, 1, &ptTargetTile) )
							{
								nDistance = (INT)(sqrt((double)(ptTargetTile.x-ptMouseTilePos.x)*(ptTargetTile.x-ptMouseTilePos.x) + 
												  (ptTargetTile.y-ptMouseTilePos.y)*(ptTargetTile.y-ptMouseTilePos.y)));
								if ( nDistance <= nSelectedDistance )
								{
									nSelectedDistance = nDistance;
									nSelectedDir = nCnt;
								}
							}
						}
						if ( nSelectedDir != -1 )
						{
							// 1타일 이동한다.
							m_pxMap->GetNextTileCanMove(m_wPosX, m_wPosY, nSelectedDir, 1, &ptTargetTile);
							SetMotionState(_MT_HORSEWALK, nSelectedDir, 0, FALSE, &ptTargetTile);
						}
						else
						{
							SetMotionState(_MT_HORSESTAND, nSelectedDir);
						}
					}
				}
				// 말을 안타고 있을때.
				else
				{
					// 목적지타일과의 방향으로 1, 2타일째의 위치에 방해물이 없을때.
					if ( m_pxMap->GetNextTileCanMove(m_wPosX, m_wPosY, bDir, 1, &ptTargetTile) && 
						 m_pxMap->GetNextTileCanMove(m_wPosX, m_wPosY, bDir, 2, &ptTargetTile) )
					{
						// 2타일 이동한다.
						m_pxMap->GetNextTileCanMove(m_wPosX, m_wPosY, bDir, 2, &ptTargetTile);
						SetMotionState(_MT_RUN, bDir, 0, FALSE, &ptTargetTile);
					}
					// 목적지타일과의 방향으로 1타일째의 위치에 방해물이 없을때.
					else if ( m_pxMap->GetNextTileCanMove(m_wPosX, m_wPosY, bDir, 1, &ptTargetTile) )
					{
						SetMotionState(_MT_WALK, bDir, 0, FALSE, &ptTargetTile);
					}
					// 목적지타일과의 방향으로 1타일째의 위치에 방해물이 있을때.
					else
					{
						INT nSelectedDir = -1;
						INT nDistance;
						INT nSelectedDistance = 100;
						for ( INT nCnt = 0; nCnt < _MAX_DIRECTION; nCnt++ )
						{
							if ( m_pxMap->GetNextTileCanMove(m_wPosX, m_wPosY, nCnt, 1, &ptTargetTile) )
							{
								nDistance = (INT)(sqrt((double)(ptTargetTile.x-ptMouseTilePos.x)*(ptTargetTile.x-ptMouseTilePos.x) + 
												  (ptTargetTile.y-ptMouseTilePos.y)*(ptTargetTile.y-ptMouseTilePos.y)));
								if ( nDistance <= nSelectedDistance )
								{
									nSelectedDistance = nDistance;
									nSelectedDir = nCnt;
								}
							}
						}
						if ( nSelectedDir != -1 )
						{
							// 1타일 이동한다.
							m_pxMap->GetNextTileCanMove(m_wPosX, m_wPosY, nSelectedDir, 1, &ptTargetTile);
							SetMotionState(_MT_WALK, nSelectedDir, 0, FALSE, &ptTargetTile);
						}
						else
						{
							SetMotionState(_MT_STAND, nSelectedDir);
						}
					}
				}
			}
			else
			{
				// 목적지타일과의 방향으로 1타일째의 위치에 방해물이 없을때.
				if ( m_pxMap->GetNextTileCanMove(m_wPosX, m_wPosY, bDir, 1, &ptTargetTile) )
				{
					if ( m_stFeatureEx.bHorse == _HORSE_NONE )		SetMotionState(_MT_WALK, bDir, 0, FALSE, &ptTargetTile);
					else											SetMotionState(_MT_HORSEWALK, bDir, 0, FALSE, &ptTargetTile);

				}
				// 목적지타일과의 방향으로 1타일째의 위치에 방해물이 있을때.
				else
				{
					INT nSelectedDir = -1;
					INT nDistance;
					INT nSelectedDistance = 100;
					for ( INT nCnt = 0; nCnt < _MAX_DIRECTION; nCnt++ )
					{
						if ( m_pxMap->GetNextTileCanMove(m_wPosX, m_wPosY, nCnt, 1, &ptTargetTile) )
						{
							nDistance = (INT)(sqrt((double)(ptTargetTile.x-ptMouseTilePos.x)*(ptTargetTile.x-ptMouseTilePos.x) + 
											  (ptTargetTile.y-ptMouseTilePos.y)*(ptTargetTile.y-ptMouseTilePos.y)));
							if ( nDistance <= nSelectedDistance )
							{
								nSelectedDistance = nDistance;
								nSelectedDir = nCnt;
							}
						}
					}
					if ( nSelectedDir != -1 )
					{
						// 1타일 이동한다.
						m_pxMap->GetNextTileCanMove(m_wPosX, m_wPosY, nSelectedDir, 1, &ptTargetTile);
						if ( m_stFeatureEx.bHorse == _HORSE_NONE )	SetMotionState(_MT_WALK, nSelectedDir, 0, FALSE, &ptTargetTile);
						else										SetMotionState(_MT_HORSEWALK, nSelectedDir, 0, FALSE, &ptTargetTile);
					}
					else
					{
						if ( m_stFeatureEx.bHorse == _HORSE_NONE )	SetMotionState(_MT_STAND, bDir);
						else										SetMotionState(_MT_HORSESTAND, bDir);
					}
				}
			}
		}
	}

	return FALSE;
}


VOID CMyHero::SetMagic(LPCLIENTMAGICRCD	pstMagic, BYTE bKeyNum, BYTE bDir, INT nTargetID, FEATURE stTargetFeature, POINT ptTargetPos)
{
	DWORD dwCurrTick = timeGetTime();
	if ( !m_bMotionLock && !m_bInputLock )
	{
		switch ( m_shCurrMagicID )
		{
		case _SKILL_MOOTEBO:
            if ( dwCurrTick - m_dwLastRushTime > 3000 )
			{
				SetMotionState(_MT_MOODEPO, bDir, 0, FALSE, &ptTargetPos, m_shCurrMagicID);
//				SetMotionState(_MT_MOODEPO, bDir, &ptTargetPos, bKeyNum, 0);
			}
			break;
		case _SKILL_HANGMAJINBUB:
		case _SKILL_DEJIWONHO:
		case _SKILL_FIRECHARM:
		case _SKILL_FIRE:
		case _SKILL_FIREBALL2:
		case _SKILL_SINSU:
		case _SKILL_FIREBALL:
		case _SKILL_SHOOTLIGHTEN:
		case _SKILL_BIGCLOAK:
			m_dwLastSpellTime = dwCurrTick;
            m_wMagicDelayTime = 200 + pstMagic->stStdMagic.nDelayTime;
			SetMotionState(_MT_SPELL1, bDir, nTargetID, TRUE, &ptTargetPos, m_shCurrMagicID);
			break;
		case _SKILL_FIREWIND:	
		case _SKILL_AMYOUNSUL:
		case _SKILL_TAMMING:
		case _SKILL_KILLUNDEAD:
		case _SKILL_HEALLING:
		case _SKILL_HOLYSHIELD:
		case _SKILL_BIGHEALLING:
		case _SKILL_LIGHTFLOWER:
		case _SKILL_SKELLETON:
		case _SKILL_SNOWWIND:
		case _SKILL_SHIELD:
		case _SKILL_SHOWHP:
		case _SKILL_EARTHFIRE:
		case _SKILL_FIREBOOM:
		case _SKILL_SPACEMOVE:
		case _SKILL_CLOAK:
		case _SKILL_LIGHTENING:
			m_dwLastSpellTime = dwCurrTick;
            m_wMagicDelayTime = 200 + pstMagic->stStdMagic.nDelayTime;
			SetMotionState(_MT_SPELL2, bDir, nTargetID, TRUE, &ptTargetPos, m_shCurrMagicID);
			break;
		}
	}

	switch ( m_shCurrMagicID )
	{
	// 염화결.
	case _SKILL_FIRESWORD:
        if ( dwCurrTick - m_dwLastFireHitTime > 10000 )
			g_xClientSocket.SendSpellMsg(m_shCurrMagicID, 0, 0, nTargetID);
		break;
	// 염화결이외의 검법.
	case _SKILL_BANWOL:
	case _SKILL_ERGUM:
        if ( dwCurrTick - m_dwLastSpellTime > 200 )
		{
			m_dwLastSpellTime = dwCurrTick;
			m_wMagicDelayTime = 0;
			g_xClientSocket.SendSpellMsg(m_shCurrMagicID, 0, 0, nTargetID);
		}
		break;
	}

	// 공격마법일때 접속종료 못하게하는 Delay.
	switch ( m_shCurrMagicID )
	{
	case _SKILL_MOOTEBO:	case _SKILL_FIRESWORD:		case _SKILL_BANWOL:		case _SKILL_ERGUM:			case _SKILL_FIRECHARM:
	case _SKILL_FIRE:		case _SKILL_FIREBALL2:		case _SKILL_FIREBALL:	case _SKILL_SHOOTLIGHTEN:	case _SKILL_AMYOUNSUL:
	case _SKILL_KILLUNDEAD: case _SKILL_LIGHTFLOWER:	case _SKILL_SNOWWIND:	case _SKILL_EARTHFIRE:
		m_dwLastMagicTime = dwCurrTick;
		break;
	}

	// 사람을 공격했을때 움직임 패널티 Delay.
	m_wMagicPKDelayTime = 0;
	if ( nTargetID )
	{
		if ( stTargetFeature.bGender == 0 || stTargetFeature.bGender == 1 )
			m_wMagicPKDelayTime = 300 + GetRandomNum(1, 1100);
	}
}


/******************************************************************************************************************

	함수명 : CMyHero::OnKeyDown()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : WPARAM wParam
	         LPARAM lParam
	         POINT ptMouse
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CMyHero::OnKeyDown(WPARAM wParam, LPARAM lParam, POINT ptMouse, POINT ptTargetPos, INT nMosueTargetID, FEATURE stTargetFeature)
{
	BYTE				bDir, bMagicKey;
	POINT				ptTargetTile, ptMouseTilePos;
	LPCLIENTMAGICRCD	pstMagic;
	DWORD				dwCurrTick;

	bMagicKey = 0;
	ZeroMemory(&ptTargetTile, sizeof(POINT));

	switch ( wParam )
	{
	case 'A':
		{
		}
	case VK_F1:	{	bMagicKey = '1';	break;	}
	case VK_F2:	{	bMagicKey = '2';	break;	}
	case VK_F3:	{	bMagicKey = '3';	break;	}
	case VK_F4:	{	bMagicKey = '4';	break;	}
	case VK_F5:	{	bMagicKey = '5';	break;	}
	case VK_F6:	{	bMagicKey = '6';	break;	}
	case VK_F7:	{	bMagicKey = '7';	break;	}
	case VK_F8:	{	bMagicKey = '8';	break;	}
	case VK_SPACE:
		{
			static INT nMode = 0;
			nMode++;
			switch ( nMode )
			{
			case 0:		m_bAttackMode = _MT_ONEHSWING;		break;
			case 1:		m_bAttackMode = _MT_ONEVSWING;		break;
			case 2:		m_bAttackMode = _MT_TWOVSWING;		break;
			case 3:		m_bAttackMode = _MT_TWOHSWING;		break;
			case 4:		m_bAttackMode = _MT_SPEARVSWING;	break;
			case 5:		m_bAttackMode = _MT_SPEARHSWING;	break;
			case 6:		m_bAttackMode = _MT_WHEELWIND;		break;
			case 7:		m_bAttackMode = _MT_RANDSWING;		break;
			case 8:		m_bAttackMode = _MT_BACKDROPKICK;	break;
			case 9:		m_bAttackMode = _MT_ONEHSWING;		break;
			}
			if ( nMode > 8 )		nMode = 0;			
			break;
		}
	}

	dwCurrTick = timeGetTime();
	// 마법이용가능 Delay Time Check.
	if ( dwCurrTick - m_dwLastSpellTime > 500 + m_wMagicDelayTime )
	{
		// 마법이 제대로 세팅되었는지 Check.
		if ( bMagicKey )
		{
			if ( nMosueTargetID == NULL )	ptMouseTilePos = GetPosMouseToTile(ptMouse.x, ptMouse.y);
			else							ptMouseTilePos = ptTargetPos;

			bDir = CalcDirection(ptMouseTilePos.x, ptMouseTilePos.y);
			pstMagic = g_xGameProc.m_xInterface.m_xStatusWnd.GetMagicByKey(bMagicKey);

			if ( pstMagic )
			{
				if ( (10 + pstMagic->stStdMagic.bDefSpell) <= m_stAbility.wMP )
				{
					m_shCurrMagicID = pstMagic->stStdMagic.wMagicID;
					SetMagic(pstMagic, bMagicKey, bDir, nMosueTargetID, stTargetFeature, ptMouseTilePos);
				}
				else
					g_xGameProc.m_xInterface.m_xClientSysMsg.AddSysMsg("마력이 모자랍니다.");
			}
		}
	}

	return TRUE;
}
