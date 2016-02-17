#include "StdAfx.h"


extern CWHQueue	PacketQ;

char	szBuff[8192];
int		nBuff = 0;

void DevideScript(char *pszScript);

/******************************************************************************************************************

	게임 플레이 관련 함수들.

*******************************************************************************************************************/
/******************************************************************************************************************

	함수명 : CGameProcess::CGameProcess()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
CGameProcess::CGameProcess()
{
	Init();
}



/******************************************************************************************************************

	함수명 : CGameProcess::~CGameProcess()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
CGameProcess::~CGameProcess()
{
	DeleteProc();
}


VOID CGameProcess::Init()
{
	m_bTileView				= FALSE;
	m_bObj1View				= FALSE;
	m_bObj2View				= FALSE;
	m_bCellView				= FALSE;
	m_bStatusView			= FALSE;
	m_bStopPtView			= FALSE;
	m_bRender				= FALSE;
	m_bShowSnow				= FALSE;
	m_bShowMist				= FALSE;
	m_bShowRain				= FALSE;
	m_bShadowABlend			= FALSE;

	m_bShowFPS				= FALSE;

	ZeroMemory(&m_ptMousePos, sizeof(POINT));
	ZeroMemory(&m_ptCurrTile, sizeof(POINT));
	ZeroMemory(m_stMemMappedID, sizeof(MEMMAPPEDID)*_VIEW_CELL_X_COUNT* _VIEW_CELL_Y_COUNT);

	m_bDayState				= _DAYSTATE_DAY;
	m_bFogState				= _FOGSTATE_DAY;

	m_pxMouseTargetActor	= NULL;		// 현재 마우스가 가리키는 곳의 타겟액터.
	m_pxMouseOldTargetActor	= NULL;		// 저장되어있는 타겟액터.
	m_pxSavedTargetActor	= NULL;

	m_fTileLightRed			= 255;
	m_fTileLightGreen		= 230;
	m_fTileLightBlue		= 180;

	m_fDarkRate				= 255.0f;
	m_wInputDelayTime		= 0;
	m_wCurrInputDelayTime	= 0;

	m_wNPCWndPosX			= 0;
	m_wNPCWndPosY			= 0;

	ZeroMemory(m_wShadowClr, 800);

	m_bShowShine			= FALSE;	
	m_dwLastShineTick		= timeGetTime();
	m_wShineCurrDelay		= 0;
	m_bCurrShineFrame		= 0;
	ZeroMemory(m_lpddsShineTextr, sizeof(LPDIRECTDRAWSURFACE7)*10);
 	D3DVECTOR vNorm(0, 0, -1);
	m_avBillboard[0]  = D3DVERTEX(D3DVECTOR(-0.5f, 0.5f, 0), vNorm, 0, 0);
	m_avBillboard[1]  = D3DVERTEX(D3DVECTOR(-0.5f,-0.5f, 0), vNorm, 0, 1);
	m_avBillboard[2]  = D3DVERTEX(D3DVECTOR( 0.5f, 0.5f, 0), vNorm, 1, 0);
	m_avBillboard[3]  = D3DVERTEX(D3DVECTOR( 0.5f,-0.5f, 0), vNorm, 1, 1);

	ZeroMemory(m_szMapName, MAX_PATH);

	m_xImage.InitAllImage();
	m_xInterface.Init();
	m_xMyHero.InitActor();
	m_xMap.InitMapHandler();
	m_xLightFog.Init(RGB(255, 255, 255));
	m_xMist.Init();
	m_xFlyingTail.InitSystem();
	m_xSmoke.InitSystem();

	m_xSnow.InitSystem();
	m_xRain.InitSystem();

//	m_xElec.InitElec();

	m_hBrush = CreateSolidBrush(RGB(200, 150, 100));
}


VOID CGameProcess::DeleteProc()
{
	INT			nCnt;
	SHORT		shLeftMsgCnt;
	LPPACKETMSG	lpPacketMsg;

	lpPacketMsg  = NULL;
	shLeftMsgCnt = m_xWaitPacketQueue.GetCount();

	// 쌓여있는 패킷을 지운다.
	if ( shLeftMsgCnt > 0 )
	{
		for ( nCnt = 0; nCnt < shLeftMsgCnt; nCnt++ )
		{
			lpPacketMsg = (LPPACKETMSG)m_xWaitPacketQueue.PopQ();
			if ( lpPacketMsg )
			{
				SAFE_DELETE(lpPacketMsg);
			}
		}
	}

	lpPacketMsg  = NULL;
	shLeftMsgCnt = m_xWaitDefaultPacketQueue.GetCount();

	// 쌓여있는 패킷을 지운다.
	if ( shLeftMsgCnt > 0 )
	{
		for ( nCnt = 0; nCnt < shLeftMsgCnt; nCnt++ )
		{
			lpPacketMsg = (LPPACKETMSG)m_xWaitDefaultPacketQueue.PopQ();
			if ( lpPacketMsg )
			{
				SAFE_DELETE(lpPacketMsg);
			}
		}
	}

	D3DWILTextr_InvalidateAllTextures();
	D3DWILTextr_DestroyAllTextures();

	DeleteObject(m_hBrush);
	m_xImage.DeleteAllImage();
	m_xMap.DestroyMapHandler();
	m_xInterface.Destroy();
	m_xMyHero.DestroyActor();
	m_xMist.Destory();

	m_xSnow.DestroySystem();
	m_xRain.DestroySystem();

	m_xActorList.ClearAllNodes();
	m_xMagicList.ClearAllNodes();
	m_xGroundMagicList.ClearAllNodes();
	m_stMapItemList.ClearAllNodes();

	g_xClientSocket.DisconnectToServer();
	Init();
}

/******************************************************************************************************************

	함수명 : CGameProcess::Load()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CGameProcess::Load()
{
	INT nCnt;
	g_xMainWnd.ResetDXG(800, 600, _SCREEN_BPP, _DXG_SCREENMODE_WINDOW, _DXG_DEVICEMODE_PRIMARY|_DXG_DEVICEMODE_D3D);
//	g_xMainWnd.ResetDXG(800, 600, _SCREEN_BPP, _DXG_SCREENMODE_FULLSCREEN, _DXG_DEVICEMODE_PRIMARY|_DXG_DEVICEMODE_D3D);

/*	CHAR szSetup[MAX_PATH];
	ZeroMemory(szSetup, MAX_PATH);
	jRegGetKey(_T("Software\\WEMADE Entertainment\\미르의전설2"), _T("setup path"), (LPBYTE)szSetup);
	for ( nCnt = strlen(szSetup); nCnt >= 0; nCnt--)
	{
		if ( szSetup[nCnt] == '\\' )
		{
			szSetup[nCnt+1] = NULL;
			break; 
		}
	}
*/
	Init();
	m_xLightFog.Init(RGB(255, 255, 255));
	m_xImage.LoadAllImage();
	m_xInterface.CreateInterface(&m_xImage);
	m_xNotice.Load(&m_xImage.m_xImageList[_IMAGE_INTER]);

	g_xClientSocket.ConnectToServer(g_xMainWnd.GetSafehWnd(), g_szServerIP, g_nServerPort);

/*	for ( nCnt = 0; nCnt < m_xImage.m_xImageList[_IMAGE_MAGIC].m_stNewWixImgaeInfo.nIndexCount; nCnt++)
	{
		if ( m_xImage.m_xImageList[_IMAGE_MAGIC].NewSetIndex(nCnt) )
		{
			D3DWILTextr_CreateEmptyTexture(m_xImage.m_xImageList[_IMAGE_MAGIC].m_szWilFileName, nCnt, 
										   m_xImage.m_xImageList[_IMAGE_MAGIC].m_lpstNewCurrWilImageInfo->shWidth,
										   m_xImage.m_xImageList[_IMAGE_MAGIC].m_lpstNewCurrWilImageInfo->shHeight,
										   (WORD*)m_xImage.m_xImageList[_IMAGE_MAGIC].m_pbCurrImage, D3DTEXTR_TRANSPARENTBLACK);
		}
	}
	for ( nCnt = 0; nCnt < m_xImage.m_xImageList[_IMAGE_MONMAGIC].m_stNewWixImgaeInfo.nIndexCount; nCnt++)
	{
		if ( m_xImage.m_xImageList[_IMAGE_MONMAGIC].NewSetIndex(nCnt) )
		{
			D3DWILTextr_CreateEmptyTexture(m_xImage.m_xImageList[_IMAGE_MONMAGIC].m_szWilFileName, nCnt, 
										   m_xImage.m_xImageList[_IMAGE_MONMAGIC].m_lpstNewCurrWilImageInfo->shWidth,
										   m_xImage.m_xImageList[_IMAGE_MONMAGIC].m_lpstNewCurrWilImageInfo->shHeight,
										   (WORD*)m_xImage.m_xImageList[_IMAGE_MONMAGIC].m_pbCurrImage, D3DTEXTR_TRANSPARENTBLACK);
		}
	}
*/
/*	// 아이템 반짝거림 텍스춰.
	for ( nCnt = 0; nCnt < 10; nCnt++)
	{
		if ( m_xImage.m_xImageList[_IMAGE_PROGUSE].NewSetIndex(nCnt+40) )
		{
			D3DWILTextr_CreateEmptyTexture(m_xImage.m_xImageList[_IMAGE_PROGUSE].m_szWilFileName, nCnt+40, 
										   m_xImage.m_xImageList[_IMAGE_PROGUSE].m_lpstNewCurrWilImageInfo->shWidth,
										   m_xImage.m_xImageList[_IMAGE_PROGUSE].m_lpstNewCurrWilImageInfo->shHeight,
										   (WORD*)m_xImage.m_xImageList[_IMAGE_PROGUSE].m_pbCurrImage, D3DTEXTR_TRANSPARENTBLACK);
			m_lpddsShineTextr[nCnt] = D3DWILTextr_RestoreEx(m_xImage.m_xImageList[_IMAGE_PROGUSE].m_szWilFileName, nCnt+40, g_xMainWnd.Get3DDevice());
		}
	}
*/
	m_xMist.Create();

	m_xSnow.InitSystem();
	m_xSnow.SetupSystem();

	m_xRain.InitSystem();
	m_xRain.SetupSystem();

	m_xFlyingTail.InitSystem();
	m_xFlyingTail.SetupSystem();

	m_xSmoke.InitSystem();
	m_xSmoke.SetupSystem();	

//	m_xElec.InitElec();
//	m_xElec.SetupElec();	

	ZeroMemory(m_stMemMappedID, sizeof(MEMMAPPEDID)*_VIEW_CELL_X_COUNT* _VIEW_CELL_Y_COUNT);

	SendMessage(g_xChatEditBox.GetSafehWnd(),EM_SETPASSWORDCHAR, NULL, NULL);
	g_xChatEditBox.SetLimitText(90);

	WORD wShadowClr = g_xMainWnd.ConvertColor24To16(RGB(20, 20, 20));
	for ( nCnt = 0; nCnt < 800; nCnt++ )
	{
		m_wShadowClr[nCnt] = wShadowClr;
	}

	m_xMap.LoadMapImage(&m_xImage);

	SetTimer(g_xMainWnd.GetSafehWnd(), WM_USER + 5000, 10, NULL); // Temp
}	


/******************************************************************************************************************

	함수명 : CGameProcess::LoadMapChanged()

	작성자 : 
	작성일 :  

	목적   : 맵이 바뀌었을때 메모리 해제와 생성, 및 데이타 로드.
	입력   : CHAR* szMap
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CGameProcess::LoadMapChanged(CHAR* szMap)
{
	m_xMap.LoadMapData(szMap);

	ZeroMemory(m_stMemMappedID, sizeof(MEMMAPPEDID)*_VIEW_CELL_X_COUNT* _VIEW_CELL_Y_COUNT);
}


VOID CGameProcess::SetNextProc()
{
	DeleteProc();

	g_xClientSocket.m_pxDefProc = g_xMainWnd.m_pxDefProcess = &g_xLoginProc;
	g_xLoginProc.Load();
	g_bProcState = _LOGIN_PROC;

/* 	g_xMainWnd.m_pxDefProcess = &g_xChrSelProc;
	g_xChrSelProc.Load();
	g_bProcState = _CHAR_SEL_PROC; 
	g_xChrSelProc.OnBeforeSelectChar();
*/
}

 
BOOL CGameProcess::TargetInLongAttack(BYTE bDir)
{
	INT		nCnt;
	POINT	ptLongTarget;
	if ( !m_xMap.GetNextTileCanMove(m_xMyHero.m_wPosX, m_xMyHero.m_wPosY, bDir, 2, &ptLongTarget) )
	{
		CActor*	pxActor;
		m_xActorList.MoveCurrentToTop();
		for ( nCnt = 0; nCnt < m_xActorList.GetCounter(); nCnt++)
		{
			pxActor = m_xActorList.GetCurrentData();

			if ( pxActor->m_wPosX == ptLongTarget.x && pxActor->m_wPosY == ptLongTarget.y )
			{
				return TRUE;
			}
			
			m_xActorList.MoveNextNode();
		}
	}

	return FALSE;
}


VOID CGameProcess::CheckMappedData(INT nLoopTime, BOOL bIsMoveTime)
{
	INT		nCnt, nObjCnt;
	BOOL	bIsSaveTargetExisted = FALSE;
	BOOL	bFocused;

	ZeroMemory(m_stMemMappedID, sizeof(MEMMAPPEDID)*_VIEW_CELL_X_COUNT* _VIEW_CELL_Y_COUNT);

	CActor* pxActor = NULL;
	CActor* pxDeadTargetActor = NULL;

	m_xActorList.MoveCurrentToTop();
	for ( nCnt = 0; nCnt < m_xActorList.GetCounter(); )
	{
		pxActor = m_xActorList.GetCurrentData();

		if ( !pxActor )
			continue;

		switch ( pxActor->m_stFeature.bGender )
		{
		case _GENDER_MAN:
		case _GENDER_WOMAN:
			{
				CHero* pxHero;
				pxHero = (CHero*)pxActor;
				pxHero->UpdateMotionState(nLoopTime, bIsMoveTime);
				m_xActorList.MoveNextNode();
				nCnt++;
			}
			break;
		case _GENDER_NPC:
			{
				CNPC* pxNPC;
				pxNPC = (CNPC*)pxActor;
				pxNPC->UpdateMotionState(nLoopTime, bIsMoveTime);
				m_xActorList.MoveNextNode();
				nCnt++;
			}
			break;
		case _GENDER_MON:
			{
				pxActor->UpdateMotionState(nLoopTime, bIsMoveTime);

				if ( pxActor->m_bAppearState == _DIG_DOWNDEL )
				{
					m_xActorList.DeleteCurrentNodeEx();
					delete((CActor*)pxActor);
					pxActor = NULL;
				}
				else
				{
					m_xActorList.MoveNextNode();
					nCnt++;
				}
			}
			break;
		}

		if ( !pxActor )
			continue;

		if ( pxActor->m_wPosX >= m_xMap.m_shStartViewTileX && pxActor->m_wPosX < m_xMap.m_shStartViewTileX + _VIEW_CELL_X_COUNT && 
			 pxActor->m_wPosY >= m_xMap.m_shStartViewTileY && pxActor->m_wPosY < m_xMap.m_shStartViewTileY + _VIEW_CELL_Y_COUNT )				 
		{
			for ( nObjCnt = 0; nObjCnt < _MAX_CELL_OBJ; nObjCnt++ )
			{
				if ( m_stMemMappedID[pxActor->m_wPosY - m_xMap.m_shStartViewTileY][pxActor->m_wPosX - m_xMap.m_shStartViewTileX].pxActor[nObjCnt] == NULL )
				{
					bFocused = FALSE;
					if ( pxActor == m_pxSavedTargetActor )
					{
						bIsSaveTargetExisted = TRUE;
					}

					// 그림의 시작좌표, 포커스상태, 그림자 블렌딩 상태를 넘겨준다.
					if ( !m_pxMouseTargetActor /*&& !pxActor->m_bIsDead*/ )
					{
						if ( PtInRect(&pxActor->m_rcActor, m_ptMousePos) )
						{
							m_pxMouseTargetActor = pxActor;
							bFocused = TRUE;
						}
					}
					else
					{
						if ( PtInRect(&pxActor->m_rcTargetRgn, m_ptMousePos) )
						{
							if ( !pxActor->m_bIsDead )
							{
								m_pxMouseTargetActor = pxActor;
								bFocused = TRUE;
							}
							else
							{
								pxDeadTargetActor = pxActor;
								bFocused = TRUE;
							}
						}
					}	

					// 죽은 Actor는 앞으로 땡긴다.
					if ( pxActor->m_bIsDead )
					{
						CActor* pxTempActor[_MAX_CELL_OBJ];
						ZeroMemory(pxTempActor, sizeof(CActor*)*_MAX_CELL_OBJ);

						pxTempActor[0] = pxActor;
						memcpy(&pxTempActor[1], m_stMemMappedID[pxActor->m_wPosY - m_xMap.m_shStartViewTileY][pxActor->m_wPosX - m_xMap.m_shStartViewTileX].pxActor, sizeof(CActor*)*(_MAX_CELL_OBJ-1));
						memcpy(m_stMemMappedID[pxActor->m_wPosY - m_xMap.m_shStartViewTileY][pxActor->m_wPosX - m_xMap.m_shStartViewTileX].pxActor, pxTempActor, sizeof(CActor*)*(_MAX_CELL_OBJ));

/*						switch ( pxActor->m_stFeature.bGender )
						{
						case _GENDER_MAN:
						case _GENDER_WOMAN:
							{
								CHero* pxHero;
								pxHero = (CHero*)pxActor;
								pxHero->DrawActor(&m_xMap, bFocused);
							}
							break;
						case _GENDER_NPC:
							{
								CNPC* pxNPC;
								pxNPC = (CNPC*)pxActor;
								pxNPC->DrawActor(&m_xMap, bFocused);
							}
							break;
						case _GENDER_MON:
							{
								pxActor->DrawActor(&m_xMap, bFocused);
							}
							break;
						}
*/
					}
					else
						m_stMemMappedID[pxActor->m_wPosY - m_xMap.m_shStartViewTileY][pxActor->m_wPosX - m_xMap.m_shStartViewTileX].pxActor[nObjCnt] = pxActor;

					break;
				}
			}
		}
	} 

	if ( !bIsSaveTargetExisted )		m_pxSavedTargetActor = NULL;

	if ( !m_pxMouseTargetActor && pxDeadTargetActor )
	{
		m_pxMouseTargetActor = pxDeadTargetActor;
	}

	CMagic* pxMagic;
	m_xGroundMagicList.MoveCurrentToTop();
	for ( nCnt = 0; nCnt < m_xGroundMagicList.GetCounter(); )
	{
		pxMagic = m_xGroundMagicList.GetCurrentData();

		if ( pxMagic->m_bMagicNum == _SKILL_HOLYSHIELD || pxMagic->m_bMagicNum == _SKILL_EARTHFIRE )
		{
			CRepeatMagic* pxRepeatMagic = (CRepeatMagic*)pxMagic;

			pxRepeatMagic->UpdateMagic(nLoopTime);

			if ( pxRepeatMagic->m_bActiveState == _MAGIC_FINISH )
			{
				m_xGroundMagicList.DeleteCurrentNodeEx();
				delete((CRepeatMagic*)pxRepeatMagic);
				pxRepeatMagic = NULL;
			}
			else
			{
				if ( pxMagic->m_shTargetTileX >= m_xMap.m_shStartViewTileX && pxMagic->m_shTargetTileX < m_xMap.m_shStartViewTileX  + _VIEW_CELL_X_COUNT && 
					 pxMagic->m_shTargetTileY >= m_xMap.m_shStartViewTileY && pxMagic->m_shTargetTileY < m_xMap.m_shStartViewTileY  + _VIEW_CELL_Y_COUNT )
				{
					for ( nObjCnt = 0; nObjCnt < _MAX_CELL_OBJ; nObjCnt++ )
					{
						if ( m_stMemMappedID[pxMagic->m_shTargetTileY - m_xMap.m_shStartViewTileY][pxMagic->m_shTargetTileX - m_xMap.m_shStartViewTileX].pxMagic[nObjCnt] == NULL )
						{
 							m_stMemMappedID[pxMagic->m_shTargetTileY - m_xMap.m_shStartViewTileY][pxMagic->m_shTargetTileX - m_xMap.m_shStartViewTileX].pxMagic[nObjCnt] = pxMagic;
							break;
						}
					}
				}

				m_xGroundMagicList.MoveNextNode();
				nCnt++;
			}
		}
		else if ( pxMagic->m_bMagicNum == _SKILL_FIRE )
		{
			CMagicStream* pxMagicStream = (CMagicStream*)pxMagic;

			pxMagicStream->UpdateMagic(nLoopTime);

			if ( pxMagicStream->m_bActiveState == _MAGIC_FINISH )
			{
				m_xGroundMagicList.DeleteCurrentNodeEx();
				delete((CMagicStream*)pxMagicStream);
				pxMagicStream = NULL;
			}
			else
			{
				if ( pxMagic->m_shTargetTileX >= m_xMap.m_shStartViewTileX && pxMagic->m_shTargetTileX < m_xMap.m_shStartViewTileX  + _VIEW_CELL_X_COUNT && 
					 pxMagic->m_shTargetTileY >= m_xMap.m_shStartViewTileY && pxMagic->m_shTargetTileY < m_xMap.m_shStartViewTileY  + _VIEW_CELL_Y_COUNT )
				{
					for ( nObjCnt = 0; nObjCnt < _MAX_CELL_OBJ; nObjCnt++ )
					{
						if ( m_stMemMappedID[pxMagic->m_shTargetTileY - m_xMap.m_shStartViewTileY][pxMagic->m_shTargetTileX - m_xMap.m_shStartViewTileX].pxMagic[nObjCnt] == NULL )
						{
							m_stMemMappedID[pxMagic->m_shTargetTileY - m_xMap.m_shStartViewTileY][pxMagic->m_shTargetTileX - m_xMap.m_shStartViewTileX].pxMagic[nObjCnt] = pxMagic;
							break;
						}
					}
				}

				m_xGroundMagicList.MoveNextNode();
				nCnt++;
			}
		}
	}		
	
	LPGROUNDITEM pstItem;
	m_stMapItemList.MoveCurrentToTop();
	for ( nCnt = 0; nCnt < m_stMapItemList.GetCounter(); nCnt++ )
	{
		pstItem = m_stMapItemList.GetCurrentData();

		if ( pstItem->shTileX >= m_xMap.m_shStartViewTileX && pstItem->shTileX < m_xMap.m_shStartViewTileX  + _VIEW_CELL_X_COUNT && 
			 pstItem->shTileY >= m_xMap.m_shStartViewTileY && pstItem->shTileY < m_xMap.m_shStartViewTileY  + _VIEW_CELL_Y_COUNT )
		{
			for ( nObjCnt = 0; nObjCnt < _MAX_CELL_OBJ; nObjCnt++ )
			{
				if ( m_stMemMappedID[pstItem->shTileY - m_xMap.m_shStartViewTileY][pstItem->shTileX - m_xMap.m_shStartViewTileX].pstItem[nObjCnt] == NULL )
				{
					m_stMemMappedID[pstItem->shTileY - m_xMap.m_shStartViewTileY][pstItem->shTileX - m_xMap.m_shStartViewTileX].pstItem[nObjCnt] = pstItem;
					break;
				}
			}
		}

		m_stMapItemList.MoveNextNode();
	}
}


VOID CGameProcess::AutoTargeting()
{
	static BYTE bLastDir = _DIRECTION_LIST_1;

	if ( !m_xMyHero.m_bMotionLock && !m_xMyHero.m_bInputLock && m_pxSavedTargetActor )
	{
		SHORT shLeftMsgCnt = m_xMyHero.m_xPacketQueue.GetCount();
		if ( shLeftMsgCnt > 0 )
		{
			if ( m_xMyHero.m_stFeatureEx.bHorse == _HORSE_NONE )	m_xMyHero.SetMotionState(_MT_STAND,      m_xMyHero.m_bCurrDir);
			else													m_xMyHero.SetMotionState(_MT_HORSESTAND, m_xMyHero.m_bCurrDir);
			m_xMyHero.UpdatePacketState();
			return;
		}

		POINT ptTargetTile = {0, 0};
		BYTE bDir = m_xMyHero.CalcDirection(m_pxSavedTargetActor->m_wPosX, m_pxSavedTargetActor->m_wPosY);

		if ( m_pxSavedTargetActor->m_bIsDead )
			return;

		if ( (m_pxSavedTargetActor->m_stFeature.bGender == _GENDER_MON && m_pxSavedTargetActor->m_bCurrMtn == _MT_MON_DIE) ||
			 (m_pxSavedTargetActor->m_stFeature.bGender <= _GENDER_WOMAN && m_pxSavedTargetActor->m_bCurrMtn == _MT_DIE) )
			 return;


		if ( (INT)(sqrt((double)(m_xMyHero.m_wPosX-m_pxSavedTargetActor->m_wPosX)*(m_xMyHero.m_wPosX-m_pxSavedTargetActor->m_wPosX) + 	(m_xMyHero.m_wPosY-m_pxSavedTargetActor->m_wPosY)*(m_xMyHero.m_wPosY-m_pxSavedTargetActor->m_wPosY)))
			      < 2 )
		{
			if ( m_xMyHero.m_stFeatureEx.bHorse == _HORSE_NONE && !m_pxSavedTargetActor->m_bIsDead )
			{
				// 공격한다.
				ptTargetTile.x = m_xMyHero.m_wPosX;
				ptTargetTile.y = m_xMyHero.m_wPosY;
				m_xMyHero.SetMotionState(_MT_ONEHSWING, bDir, m_pxSavedTargetActor->m_dwIdentity, m_pxSavedTargetActor->m_bIsDead, &ptTargetTile);
			}
		}
		// Actor가 있는 방향으로 이동 가능한가?(맵속성체크) 
		else if ( m_xMap.GetNextTileCanMove(m_xMyHero.m_wPosX, m_xMyHero.m_wPosY, bDir, 1, &ptTargetTile) )
		{
			// 1타일 이동한다.
			if ( m_xMyHero.m_stFeatureEx.bHorse == _HORSE_NONE )
			{
				m_xMyHero.SetMotionState(_MT_WALK, bDir);
			}
			else
			{
				m_xMyHero.SetMotionState(_MT_HORSEWALK, bDir);
			}
			bLastDir = bDir;
		}
		else if ( !m_xMap.GetNextTileCanMove(m_xMyHero.m_wPosX, m_xMyHero.m_wPosY, bDir, 1, &ptTargetTile) )
		{
			BYTE	bSelectedDirList[8] = {0, 0, 0, 0, 0, 0, 0, 0};
			INT		nSeclectedDirCnt = -1;
			INT		nLastDistance = 100;
			INT		nCurrDistance;
			BYTE	bInvDir;
			BYTE	bSelectedDir = 255;

			if ( bLastDir < 4 )		bInvDir = bLastDir + 4;
			else					bInvDir = bLastDir - 4;

			for ( INT nCnt = 0; nCnt < _MAX_DIRECTION; nCnt++ )
			{
				if ( m_xMap.GetNextTileCanMove(m_xMyHero.m_wPosX, m_xMyHero.m_wPosY, nCnt, 1, &ptTargetTile) )
				{
					nCurrDistance = (INT)(sqrt((double)(ptTargetTile.x-m_pxSavedTargetActor->m_wPosX)*(ptTargetTile.x-m_pxSavedTargetActor->m_wPosX) + (ptTargetTile.y-m_pxSavedTargetActor->m_wPosY)*(ptTargetTile.y-m_pxSavedTargetActor->m_wPosY)));
					if ( nCurrDistance <= nLastDistance && bInvDir != nCnt )
					{
						nLastDistance = nCurrDistance;
						nSeclectedDirCnt++;
						bSelectedDirList[nSeclectedDirCnt] = nCnt;
					}
				}
			}

			BYTE bDirOrderList[8][8] = 
			{
				{0, 7, 1, 6, 2, 5, 3, 4},
				{1, 0, 2, 7, 3, 6, 4, 5},
				{2, 1, 3, 0, 4, 7, 5, 6},
				{3, 2, 4, 1, 5, 0, 6, 7},
				{4, 5, 3, 6, 2, 7, 1, 0},
				{5, 6, 4, 7, 3, 0, 2, 1},
				{6, 7, 5, 0, 4, 1, 3, 2},
				{7, 0, 6, 1, 5, 2, 4, 3},
			};

			BYTE bOrderCnt = 0;

			for ( nCnt = 0; nCnt < nSeclectedDirCnt; nCnt++ )
			{
				while ( bSelectedDirList[nCnt] != bDirOrderList[bDir][bOrderCnt] || bOrderCnt < 8 )	bOrderCnt++;
				if ( bOrderCnt >= 8 )			bOrderCnt = 0;
				else					
				{	bSelectedDir = bLastDir = bSelectedDirList[nCnt];	break;	}
			}

			if ( bSelectedDir == 255 )		bSelectedDir = bLastDir = bSelectedDirList[GetRandomNum(0, nSeclectedDirCnt+1)];
	

			if ( nSeclectedDirCnt != -1 )
			{
				// 1타일 이동한다.
				m_xMap.GetNextTileCanMove(m_xMyHero.m_wPosX, m_xMyHero.m_wPosY, bSelectedDir, 1, &ptTargetTile);
				if ( m_xMyHero.m_stFeatureEx.bHorse == _HORSE_NONE )
				{
					m_xMyHero.SetMotionState(_MT_WALK, bSelectedDir);
				}
				else
				{
					m_xMyHero.SetMotionState(_MT_HORSEWALK, bSelectedDir);
				}
			}
			else
			{
				if ( m_xMyHero.m_stFeatureEx.bHorse == _HORSE_NONE )
				{
					m_xMyHero.SetMotionState(_MT_STAND, bDir);
				}
				else
				{
					m_xMyHero.SetMotionState(_MT_HORSESTAND, bDir);
				}
			}
		}
		else
		{
			if ( m_xMyHero.m_stFeatureEx.bHorse == _HORSE_NONE )
			{
				m_xMyHero.SetMotionState(_MT_STAND, bDir);
			}
			else
			{
				m_xMyHero.SetMotionState(_MT_HORSESTAND, bDir);
			}
		}
	}
}


VOID CGameProcess::DropItemShow()
{
	if ( m_stMapItemList.GetCounter() != 0 )
	{
		m_stMapItemList.MoveCurrentToTop();
		LPGROUNDITEM pstItem;

		for ( INT nCnt = 0; nCnt < m_stMapItemList.GetCounter(); nCnt++ )
		{
			pstItem = m_stMapItemList.GetCurrentData();

			if ( pstItem->shTileX == m_ptCurrTile.x && pstItem->shTileY == m_ptCurrTile.y )
			{
				SIZE sizeLen;

				sizeLen		 = g_xMainWnd.GetStrLength(NULL, NULL, pstItem->szItemName);
				INT nCenterX = (pstItem->shTileX-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START-m_xMap.m_shViewOffsetX+_CELL_WIDTH /2;
				INT nCenterY = (pstItem->shTileY-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START-m_xMap.m_shViewOffsetY+_CELL_HEIGHT/2-12;

				RECT rcBack;
				SetRect(&rcBack, nCenterX-sizeLen.cx/2, nCenterY-7, nCenterX+sizeLen.cx/2, nCenterY+8);

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

						vTrans.x = (FLOAT)rcBack.left-400+(rcBack.right-rcBack.left)/2-1;  
						vTrans.y = (FLOAT)-rcBack.top+300-4; 
						vTrans.z = 0;
						D3DUtil_SetTranslateMatrix(matTrans, vTrans);
						D3DUtil_SetScaleMatrix(matScale, (FLOAT)rcBack.right-rcBack.left, (FLOAT)rcBack.bottom-rcBack.top, 0.0f);
						D3DMath_MatrixMultiply(matWorld, matScale, matTrans);
						g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);				

						D3DUtil_InitMaterial(mtrl, 
											 (FLOAT)100/255.0f,
											 (FLOAT)100/255.0f, 
											 (FLOAT)100/255.0f);
						mtrl.diffuse.a = 125/255.0f;
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
						g_xMainWnd.Get3DDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX, m_avBillboard, 4, NULL);

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

				g_xMainWnd.PutsHan(g_xMainWnd.GetBackBuffer(), nCenterX-sizeLen.cx/2-1, nCenterY-7, RGB(  0,   0,   0), RGB(0, 0, 0), pstItem->szItemName);
				g_xMainWnd.PutsHan(g_xMainWnd.GetBackBuffer(), nCenterX-sizeLen.cx/2  , nCenterY-8, RGB(255, 255, 125), RGB(0, 0, 0), pstItem->szItemName);

				break;
			}

			m_stMapItemList.MoveNextNode();
		}
	}	
}


VOID CGameProcess::RenderScene(INT nLoopTime)
{
	BOOL		 bIsMoveTime	= FALSE;
	static WORD	 wMoveTime		= 0;
	static DWORD dwReceiveTime	= 0;

	wMoveTime			  += nLoopTime;
	m_wCurrInputDelayTime += nLoopTime;

	if ( nLoopTime < 10000 )		dwReceiveTime += nLoopTime;
	if ( m_wCurrInputDelayTime >= m_wInputDelayTime )
	{
		m_wCurrInputDelayTime	= 0;
		m_wInputDelayTime		= 0;
	}
	if ( wMoveTime > 100 )
	{
		bIsMoveTime = TRUE;
		wMoveTime	= 0;
	}
	if ( !m_bRender && dwReceiveTime > 100000 )
	{
		MessageBox(g_xMainWnd.GetSafehWnd(), "로그온 정보를 받는데 실패했습니다. 프로그램을 종료합니다.", "", MB_OK);
		PostQuitMessage(NULL);
		dwReceiveTime = 0;
	}

	if ( m_bRender )
	{
		m_xLightFog.ClearSavedLightColor();

		m_xMyHero.UpdateMotionState(nLoopTime, bIsMoveTime);
		// 맵을 그린다.
//		Clear(0);
		g_xMainWnd.DrawWithImagePerLineClipRgn(
			m_xMap.m_rcView.left+_VIEW_CELL_X_START-m_xMap.m_shViewOffsetX,
			m_xMap.m_rcView.top+_VIEW_CELL_Y_START-m_xMap.m_shViewOffsetY,
			m_xMap.m_rcView.right-m_xMap.m_rcView.left,
			m_xMap.m_rcView.bottom-m_xMap.m_rcView.top,
			m_xMap.m_wSavedMapBuffer, _CLIP_WIDTH, _CLIP_HEIGHT);

		m_pxMouseTargetActor = NULL;
		CheckMappedData(nLoopTime, bIsMoveTime);
		AutoTargeting();

		RenderObject(nLoopTime);
		RenderMapTileGrid();		//For Debug	

		CMagic* pxMagic;
		m_xMagicList.MoveCurrentToTop();
		for ( INT nCnt = 0; nCnt < m_xMagicList.GetCounter(); )
		{
			pxMagic = m_xMagicList.GetCurrentData();

			if ( pxMagic->m_bMagicNum == _SKILL_SHOOTLIGHTEN )
			{
				CElecMagic* pxElecMagic = (CElecMagic*)pxMagic;;

				pxElecMagic->UpdateMagic(nLoopTime);

				if ( pxElecMagic->m_bActiveState == _MAGIC_FINISH )
				{
					m_xMagicList.DeleteCurrentNodeEx();
					delete((CElecMagic*)pxElecMagic);
					pxElecMagic = NULL;
				}
				else
				{
					m_xMagicList.MoveNextNode();
					nCnt++;
				}
			}
			else
			{
				pxMagic->UpdateMagic(nLoopTime);
				if ( pxMagic->m_bActiveState == _MAGIC_FINISH )
				{
					m_xMagicList.DeleteCurrentNodeEx();
					delete((CMagic*)pxMagic);
					pxMagic = NULL;
				}
				else
				{
					m_xMagicList.MoveNextNode();
					nCnt++;
				}
			}
		}


		if ( m_bShowMist )			m_xMist.ProgressMist();
		if ( m_bShowSnow )			
		{
			m_xSnow.UpdateSystem(nLoopTime);
			m_xSnow.RenderSystem(g_xMainWnd.Get3DDevice());
		}

		if ( m_bShowRain )			
		{
			m_xRain.UpdateSystem(nLoopTime);
			m_xRain.RenderSystem(g_xMainWnd.Get3DDevice());
		}

		m_xFlyingTail.UpdateSystem(nLoopTime);
		m_xFlyingTail.RenderSystem(g_xMainWnd.Get3DDevice());

		m_xSmoke.UpdateSystem(nLoopTime);
		m_xSmoke.RenderSystem(g_xMainWnd.Get3DDevice());		

//		m_xElec.UpdateElec(400, 300, 0, 0);
//		m_xElec.RenderElec(g_xMainWnd.Get3DDevice());


		m_xLightFog.ShowLightFog();

		m_xMyHero.ShowMessage(nLoopTime);
		m_xMyHero.DrawHPBar();

		if ( m_xActorList.GetCounter() != 0 )
		{
			m_xActorList.MoveCurrentToTop();
			CActor* pxActor;
			for ( INT nCnt = 0; nCnt < m_xActorList.GetCounter(); nCnt++ )
			{
				pxActor = m_xActorList.GetCurrentData();

				switch ( pxActor->m_stFeature.bGender )
				{
				case _GENDER_MAN:
				case _GENDER_WOMAN:
					{
						CHero*	pxHero;
						pxHero = (CHero*)pxActor;
						pxHero->ShowMessage(nLoopTime);
						pxHero->DrawHPBar();
					}
					break;
				case _GENDER_NPC:
				case _GENDER_MON:
					{
//						pxActor->ShowMessage(nLoopTime);
						pxActor->DrawHPBar();
					}
					break;
				}

				m_xActorList.MoveNextNode();
			}
		}

		DropItemShow();
//		RenderShine(nLoopTime);

		if ( m_xMagicList.GetCounter() != 0 )
		{
			m_xMagicList.MoveCurrentToTop();
			CMagic* pxMagic;

			for ( INT nCnt = 0; nCnt < m_xMagicList.GetCounter(); nCnt++ )
			{
				pxMagic = m_xMagicList.GetCurrentData();

				if ( pxMagic->m_bMagicNum == _SKILL_SHOOTLIGHTEN )
				{
					CElecMagic* pxElecMagic = (CElecMagic*)pxMagic;
					pxElecMagic->DrawMagic();
				}
				else			
					pxMagic->DrawMagic();
				m_xMagicList.MoveNextNode();
			}
		}	

		if ( m_pxMouseTargetActor )
		{
			if ( m_pxMouseOldTargetActor != m_pxMouseTargetActor )
			{
				if ( !m_pxMouseTargetActor->m_szName[0] )
					g_xClientSocket.SendQueryName(m_pxMouseTargetActor->m_dwIdentity, m_pxMouseTargetActor->m_wPosX, m_pxMouseTargetActor->m_wPosY);
			}

			m_pxMouseOldTargetActor = m_pxMouseTargetActor;
			m_pxMouseTargetActor->DrawName();
//m_pxMouseTargetActor->DrawHPBar();
		}

		if ( m_wNPCWndPosX && m_wNPCWndPosY && ( abs(m_wNPCWndPosX-m_xMyHero.m_wPosX) > 5 || abs(m_wNPCWndPosY-m_xMyHero.m_wPosY) > 5) )
		{
			CloseNPCWindow();
		}

		m_xInterface.RenderInterface(nLoopTime, m_ptMousePos);

		ShowStatus(0, 0);
	}
	else
		Clear(0);

//g_xMainWnd.DrawWithGrayBackBuffer();
	m_xNotice.RenderMessageBox(nLoopTime);
}


VOID CGameProcess::CloseNPCWindow()
{
	if ( m_xInterface.m_xNPCWnd.GetGameWndActive() )
	{
		m_xInterface.DeleteWindowToList(_WND_ID_NPCCHAT);
		m_xInterface.m_xNPCWnd.SetGameWndActive(FALSE);

		m_wNPCWndPosX = m_wNPCWndPosY = 0;
	}
}



BOOL CGameProcess::RenderShine(INT nLoopTime)
{
	DWORD dwCurrTick = timeGetTime();

	if ( dwCurrTick - m_dwLastShineTick > 5000 )
	{
		m_bShowShine = TRUE;
		m_dwLastShineTick = dwCurrTick;
		m_wShineCurrDelay = 0;
		m_bCurrShineFrame = 0;
	}

	m_wShineCurrDelay += nLoopTime;
	if ( m_wShineCurrDelay > 120 )
	{
		m_bCurrShineFrame++;
		m_wShineCurrDelay = 0;
		if ( m_bCurrShineFrame > 9 )
		{
			m_bCurrShineFrame = 0;
			m_bShowShine = FALSE;
		}
	}

	if ( g_xMainWnd.Get3DDevice() && m_bShowShine )
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

			CWHWilImageData* pxShineImg = &m_xImage.m_xImageList[_IMAGE_PROGUSE];
			if ( pxShineImg->NewSetIndex(m_bCurrShineFrame+40) )
			{
				if ( m_stMapItemList.GetCounter() != 0 )
				{
					m_stMapItemList.MoveCurrentToTop();
					LPGROUNDITEM pstItem;

					for ( INT nCnt = 0; nCnt < m_stMapItemList.GetCounter(); nCnt++ )
					{
						pstItem = m_stMapItemList.GetCurrentData();

/*						INT nCenterX = (pstItem->shTileX-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START-m_xMap.m_shViewOffsetX;
						INT nCenterY = (pstItem->shTileY-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START-m_xMap.m_shViewOffsetY;
*/						INT nCenterX = (pstItem->shTileX-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START-m_xMap.m_shViewOffsetX+_CELL_WIDTH /2;
						INT nCenterY = (pstItem->shTileY-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START-m_xMap.m_shViewOffsetY+_CELL_HEIGHT/2;

						g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);
						vTrans.x = (FLOAT) nCenterX-400; 
						vTrans.y = (FLOAT)-nCenterY+300; 
						vTrans.z = 0;

						D3DUtil_SetTranslateMatrix(matTrans, vTrans);
						D3DUtil_SetScaleMatrix(matScale, (FLOAT)pxShineImg->m_lpstNewCurrWilImageInfo->shWidth, (FLOAT)pxShineImg->m_lpstNewCurrWilImageInfo->shHeight, 0.0f);
						D3DMath_MatrixMultiply(matWorld, matScale, matTrans);
						g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);

						if ( m_lpddsShineTextr[m_bCurrShineFrame] )
							g_xMainWnd.Get3DDevice()->SetTexture(0, m_lpddsShineTextr[m_bCurrShineFrame]);
						D3DUtil_InitMaterial(mtrl, (FLOAT)255/255.0f, (FLOAT)255/255.0f, (FLOAT)255/255.0f);
						mtrl.diffuse.a = 1/255.0f;
						g_xMainWnd.Get3DDevice()->SetMaterial(&mtrl);
						SetBlendRenderState(g_xMainWnd.Get3DDevice(), _BLEND_LIGHTINV, mtrl);
						g_xMainWnd.Get3DDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX, m_avBillboard, 4, NULL);

						// 원상복귀.
						ZeroMemory(&mtrl, sizeof(mtrl));
						mtrl.diffuse.r = mtrl.diffuse.g = mtrl.diffuse.b = 0.1f;
						mtrl.ambient.r = mtrl.ambient.g = mtrl.ambient.b = 1.0f;
						g_xMainWnd.Get3DDevice()->SetMaterial(&mtrl);

						ResetBlendenderState(g_xMainWnd.Get3DDevice());
						g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

						m_stMapItemList.MoveNextNode();
					}
				}				
			}
			g_xMainWnd.Get3DDevice()->EndScene();
			return S_OK;
		}
	}
	return E_FAIL;


}



VOID CGameProcess::RenderObject(INT nLoopTime)
{
	INT	 nStartX, nStartY, nObjFileIdx, nImgIdx, nArrayNum, nObjCnt;
	BOOL bFocused = FALSE;
	BOOL bLongObj = FALSE;
	BOOL bBlend	  = FALSE;
	BOOL bDrawShadow = TRUE;

	m_xMap.SetAniTileFrame(nLoopTime);

// 48*32 타일형 오브젝트 그리기.
	for ( INT nYCnt = m_xMap.m_shStartViewTileY-4; nYCnt < m_xMap.m_shStartViewTileY+_VIEW_CELL_Y_COUNT+6; nYCnt++ )
	{
		for ( INT nXCnt = m_xMap.m_shStartViewTileX-4; nXCnt < m_xMap.m_shStartViewTileX+_VIEW_CELL_X_COUNT+6; nXCnt++ )
		{
			if ( nYCnt >= m_xMap.m_stMapFileHeader.shHeight )
				break;

			if ( nXCnt >= m_xMap.m_stMapFileHeader.shWidth || nXCnt < 0 || nYCnt < 0 )
				continue; 

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Test : 광원을 위해서 -4부터 +5까지의 영역을 더 잡는다.
			LIGHTINFO stLight = m_xMap.GetTileLightInfo(nXCnt, nYCnt);
			if ( stLight.bIsLight )
			{
				m_xLightFog.SetLightRadiusWithCircle((nXCnt-m_xMap.m_shStartViewTileX)*_CELL_WIDTH-m_xMap.m_shViewOffsetX+_VIEW_CELL_X_START,
													 (nYCnt-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT-m_xMap.m_shViewOffsetY+_VIEW_CELL_Y_START, 
													 12, m_fTileLightBlue, m_fTileLightGreen, m_fTileLightRed);
			}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

			nArrayNum = nYCnt + nXCnt*m_xMap.m_stMapFileHeader.shHeight;

			// 오브젝트가 있을때.
			if ( m_xMap.m_pstCellInfo[nArrayNum].bFileIdx != 255 )
			{
				// 오브젝트1이 있을때.
				if ( m_xMap.m_pstCellInfo[nArrayNum].wObj1 != 65535 )
				{
					nObjFileIdx = (m_xMap.m_pstCellInfo[nArrayNum].bFileIdx & 0XF0) >> 4;
					nImgIdx	= m_xMap.m_pstCellInfo[nArrayNum].wObj1;

					if ( nObjFileIdx > 2 && nObjFileIdx < 14 )
					{
						if ( m_xMap.m_pxTileImg[nObjFileIdx]->NewSetIndex(nImgIdx) )
						{
							if ( m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shWidth == 48 &&
  								 m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight == 32 )
							{
								bBlend = FALSE;
								
								nImgIdx += m_xMap.GetDoorImgIdx(nXCnt, nYCnt);

								// 애니메이션 오브젝트이면.
								if ( m_xMap.m_pstCellInfo[nArrayNum].bObj1Ani != 255 )
								{
									BYTE  bTickType;
									SHORT shAniCnt;
									
									bTickType = (m_xMap.m_pstCellInfo[nArrayNum].bObj1Ani & 0X70) >> 4;
									shAniCnt = m_xMap.m_pstCellInfo[nArrayNum].bObj1Ani & 0X0F;
									
									if ( (m_xMap.m_pstCellInfo[nArrayNum].bObj1Ani & 0X80) >> 7 )
										bBlend = TRUE;

									nImgIdx += m_xMap.m_bAniTileFrame[bTickType][shAniCnt];
									m_xMap.m_pxTileImg[nObjFileIdx]->NewSetIndex(nImgIdx);
								}
						
								if ( !bBlend )
									g_xMainWnd.DrawWithImageForCompClipRgn(
									(nXCnt-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START-m_xMap.m_shViewOffsetX,
									(nYCnt-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START-m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight+_CELL_HEIGHT-m_xMap.m_shViewOffsetY,
									m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shWidth,
									m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight,
									(WORD*)m_xMap.m_pxTileImg[nObjFileIdx]->m_pbCurrImage,
									_CLIP_WIDTH, _CLIP_HEIGHT);
								else
								{
									m_xImage.AddTextr(_TEXTR_FILE_MAP, nObjFileIdx, nImgIdx);
									LPDIRECTDRAWSURFACE7 lpddsTextr = m_xImage.GetTextrImg(_TEXTR_FILE_MAP, nObjFileIdx, nImgIdx);

									nStartX = (nXCnt-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START-m_xMap.m_shViewOffsetX;
									nStartY = -(nYCnt-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START-m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight+_CELL_HEIGHT-m_xMap.m_shViewOffsetY;

									D3DMATERIAL7 mtrl;
									D3DVECTOR	vecTrans(nStartX, nStartY, 0);
									D3DVECTOR	vecScale(m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shWidth, m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight, 1);

									D3DUtil_InitMaterial(mtrl, (FLOAT)255/255.0f, (FLOAT)255/255.0f, (FLOAT)255/255.0f);
									mtrl.diffuse.a = 1.0f/255.0f;
									g_xMainWnd.Get3DDevice()->SetMaterial(&mtrl);
									m_xImage.DrawBillBoard(g_xMainWnd.Get3DDevice(), vecTrans, vecScale, mtrl, lpddsTextr);
								}
							}
						}
					}
				}
				// 오브젝트2가 있을때.
				if ( m_xMap.m_pstCellInfo[nArrayNum].wObj2 != 65535 )
				{
					nObjFileIdx = (m_xMap.m_pstCellInfo[nArrayNum].bFileIdx & 0X0F);
					nImgIdx		= m_xMap.m_pstCellInfo[nArrayNum].wObj2;

					if ( nObjFileIdx > 2 && nObjFileIdx < 14 )
					{
						if ( m_xMap.m_pxTileImg[nObjFileIdx]->NewSetIndex(nImgIdx) )
						{
							if ( m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shWidth == 48 &&
								 m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight == 32 )
							{
								bBlend = FALSE;
								
								nImgIdx += m_xMap.GetDoorImgIdx(nXCnt, nYCnt);

								// 애니메이션 오브젝트이면.
								if ( m_xMap.m_pstCellInfo[nArrayNum].bObj2Ani != 255 )
								{
									BYTE  bTickType;
									SHORT shAniCnt;
									
									bTickType = (m_xMap.m_pstCellInfo[nArrayNum].bObj2Ani & 0X70) >> 4;
									shAniCnt = m_xMap.m_pstCellInfo[nArrayNum].bObj2Ani & 0X0F;
									
									if ( (m_xMap.m_pstCellInfo[nArrayNum].bObj2Ani & 0X80) >> 7 )
										bBlend = TRUE;

									nImgIdx += m_xMap.m_bAniTileFrame[bTickType][shAniCnt];
									m_xMap.m_pxTileImg[nObjFileIdx]->NewSetIndex(nImgIdx);
								}
								
								if ( !bBlend )
									g_xMainWnd.DrawWithImageForCompClipRgn(
									(nXCnt-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START-m_xMap.m_shViewOffsetX,
									(nYCnt-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START-m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight+_CELL_HEIGHT-m_xMap.m_shViewOffsetY,
									m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shWidth,
									m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight,
									(WORD*)m_xMap.m_pxTileImg[nObjFileIdx]->m_pbCurrImage,
									_CLIP_WIDTH, _CLIP_HEIGHT);
								else
								{
									m_xImage.AddTextr(_TEXTR_FILE_MAP, nObjFileIdx, nImgIdx);
									LPDIRECTDRAWSURFACE7 lpddsTextr = m_xImage.GetTextrImg(_TEXTR_FILE_MAP, nObjFileIdx, nImgIdx);

									nStartX = (nXCnt-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START-m_xMap.m_shViewOffsetX;
									nStartY = -(nYCnt-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START-m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight+_CELL_HEIGHT-m_xMap.m_shViewOffsetY;

									D3DMATERIAL7 mtrl;
									D3DVECTOR	vecTrans(nStartX, nStartY, 0);
									D3DVECTOR	vecScale(m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shWidth, m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight, 1);

									D3DUtil_InitMaterial(mtrl, (FLOAT)255/255.0f, (FLOAT)255/255.0f, (FLOAT)255/255.0f);
									mtrl.diffuse.a = 1.0f/255.0f;
									g_xMainWnd.Get3DDevice()->SetMaterial(&mtrl);
									m_xImage.DrawBillBoard(g_xMainWnd.Get3DDevice(), vecTrans, vecScale, mtrl, lpddsTextr);
								}
							}
						}
					}
				}
			}
		}
	}	

	// 48*32가 아닌 길이가 있는 타일형 오브젝트 그리기.
	for ( nYCnt = m_xMap.m_shStartViewTileY; nYCnt < m_xMap.m_shStartViewTileY+_VIEW_CELL_Y_COUNT_FOR_OBJ; nYCnt++ )
	{
		for ( INT nXCnt = m_xMap.m_shStartViewTileX; nXCnt < m_xMap.m_shStartViewTileX+_VIEW_CELL_X_COUNT; nXCnt++ )
		{
			if ( nYCnt >= m_xMap.m_stMapFileHeader.shHeight )
				break;
			if ( nXCnt >= m_xMap.m_stMapFileHeader.shWidth || nXCnt < 0 || nYCnt < 0 )
				continue; 

			bLongObj = FALSE;

			nArrayNum = nYCnt + nXCnt*m_xMap.m_stMapFileHeader.shHeight;

			// 오브젝트가 있을때.
			if ( m_xMap.m_pstCellInfo[nArrayNum].bFileIdx != 255 )
			{
				// 오브젝트1이 있을때.
				if ( m_xMap.m_pstCellInfo[nArrayNum].wObj1 != 65535 )
				{
					nObjFileIdx = (m_xMap.m_pstCellInfo[nArrayNum].bFileIdx & 0XF0) >> 4;
					nImgIdx	= m_xMap.m_pstCellInfo[nArrayNum].wObj1;	
	
					if ( nObjFileIdx > 2 && nObjFileIdx < 14 )
					{
						if ( m_xMap.m_pxTileImg[nObjFileIdx]->NewSetIndex(nImgIdx) )
						{
							if ( !(m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shWidth == 48 &&
								   m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight == 32) )
							{
								bBlend = FALSE;
								
								nImgIdx += m_xMap.GetDoorImgIdx(nXCnt, nYCnt);

								// 애니메이션 오브젝트이면.
								if ( m_xMap.m_pstCellInfo[nArrayNum].bObj1Ani != 255 )
								{
									BYTE  bTickType;
									SHORT shAniCnt;
									
									bTickType = (m_xMap.m_pstCellInfo[nArrayNum].bObj1Ani & 0X70) >> 4;
									shAniCnt = m_xMap.m_pstCellInfo[nArrayNum].bObj1Ani & 0X0F;
									
									if ( (m_xMap.m_pstCellInfo[nArrayNum].bObj1Ani & 0X80) >> 7 )
										bBlend = TRUE;

									nImgIdx += m_xMap.m_bAniTileFrame[bTickType][shAniCnt];
									m_xMap.m_pxTileImg[nObjFileIdx]->NewSetIndex(nImgIdx);
								}
								
								if ( !bBlend )
									g_xMainWnd.DrawWithImageForCompClipRgn(
									(nXCnt-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START-m_xMap.m_shViewOffsetX,
									(nYCnt-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START-m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight+_CELL_HEIGHT-m_xMap.m_shViewOffsetY,
									m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shWidth,
									m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight,
									(WORD*)m_xMap.m_pxTileImg[nObjFileIdx]->m_pbCurrImage,
									_CLIP_WIDTH, _CLIP_HEIGHT);
								else
								{
									m_xImage.AddTextr(_TEXTR_FILE_MAP, nObjFileIdx, nImgIdx);
									LPDIRECTDRAWSURFACE7 lpddsTextr = m_xImage.GetTextrImg(_TEXTR_FILE_MAP, nObjFileIdx, nImgIdx);

									nStartX = (nXCnt-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START-m_xMap.m_shViewOffsetX+m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shPX;
									nStartY = (nYCnt-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START-m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight+_CELL_HEIGHT-m_xMap.m_shViewOffsetY+m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shPY;

									D3DMATERIAL7 mtrl;
									D3DVECTOR	vecTrans(nStartX, nStartY, 0);
									D3DVECTOR	vecScale(m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shWidth, m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight, 1);

									D3DUtil_InitMaterial(mtrl, (FLOAT)255/255.0f, (FLOAT)255/255.0f, (FLOAT)255/255.0f);
									mtrl.diffuse.a = 0.0f/255.0f;
									g_xMainWnd.Get3DDevice()->SetMaterial(&mtrl);
									m_xImage.DrawBillBoard(g_xMainWnd.Get3DDevice(), vecTrans, vecScale, mtrl, lpddsTextr);
								}
	
								bLongObj = TRUE;
							}
						}
					}
				}
				// 오브젝트2가 있을때.
				if ( m_xMap.m_pstCellInfo[nArrayNum].wObj2 != 65535 )
				{
					nObjFileIdx = (m_xMap.m_pstCellInfo[nArrayNum].bFileIdx & 0X0F);
					nImgIdx		= m_xMap.m_pstCellInfo[nArrayNum].wObj2;

					if ( nObjFileIdx > 2 && nObjFileIdx < 14 )
					{
						if ( m_xMap.m_pxTileImg[nObjFileIdx]->NewSetIndex(nImgIdx) )
						{
							if ( !(m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shWidth == 48 &&
								 m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight == 32) )
							{
								bBlend = FALSE;
								
								nImgIdx += m_xMap.GetDoorImgIdx(nXCnt, nYCnt);

								// 애니메이션 오브젝트이면.
								if ( m_xMap.m_pstCellInfo[nArrayNum].bObj2Ani != 255 )
								{
									BYTE  bTickType;
									SHORT shAniCnt;
									
									bTickType = (m_xMap.m_pstCellInfo[nArrayNum].bObj2Ani & 0X70) >> 4;
									shAniCnt = m_xMap.m_pstCellInfo[nArrayNum].bObj2Ani & 0X0F;
									
//									if ( (m_xMap.m_pstCellInfo[nArrayNum].bObj2Ani & 0X80) >> 7 )
//										bBlend = TRUE;

									nImgIdx += m_xMap.m_bAniTileFrame[bTickType][shAniCnt];
									m_xMap.m_pxTileImg[nObjFileIdx]->NewSetIndex(nImgIdx);
								}

								if ( !bBlend )
									g_xMainWnd.DrawWithImageForCompClipRgn(
												(nXCnt-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START-m_xMap.m_shViewOffsetX,
												(nYCnt-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START-m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight+_CELL_HEIGHT-m_xMap.m_shViewOffsetY,
												m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shWidth,
												m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight,
												(WORD*)m_xMap.m_pxTileImg[nObjFileIdx]->m_pbCurrImage,
												_CLIP_WIDTH, _CLIP_HEIGHT);
								else
								{
									m_xImage.AddTextr(_TEXTR_FILE_MAP, nObjFileIdx, nImgIdx);
									LPDIRECTDRAWSURFACE7 lpddsTextr = m_xImage.GetTextrImg(_TEXTR_FILE_MAP, nObjFileIdx, nImgIdx);

									nStartX = (nXCnt-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START-m_xMap.m_shViewOffsetX;
									nStartY = -(nYCnt-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START-m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight+_CELL_HEIGHT-m_xMap.m_shViewOffsetY;

									D3DMATERIAL7 mtrl;
									D3DVECTOR	vecTrans(nStartX, nStartY, 0);
									D3DVECTOR	vecScale(m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shWidth, m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight, 1);

									D3DUtil_InitMaterial(mtrl, (FLOAT)255/255.0f, (FLOAT)255/255.0f, (FLOAT)255/255.0f);
									mtrl.diffuse.a = 1.0f/255.0f;
									g_xMainWnd.Get3DDevice()->SetMaterial(&mtrl);
									m_xImage.DrawBillBoard(g_xMainWnd.Get3DDevice(), vecTrans, vecScale, mtrl, lpddsTextr);
								}
								
   								bLongObj = TRUE;
							}
						}
					}
				}
			}

//			m_xMap.DrawOpenDoor(nXCnt, nYCnt);

			INT	 nMapViewX		= 0;
			INT	 nMapViewY		= 0;
			BOOL bIsMapViewTime = FALSE;
			if ( nXCnt >= m_xMap.m_shStartViewTileX && nXCnt < m_xMap.m_shStartViewTileX + _VIEW_CELL_X_COUNT && 
				 nYCnt >= m_xMap.m_shStartViewTileY && nYCnt < m_xMap.m_shStartViewTileY + _VIEW_CELL_Y_COUNT )
			{
				bIsMapViewTime = TRUE;
				nMapViewX = nXCnt - m_xMap.m_shStartViewTileX;
				nMapViewY = nYCnt - m_xMap.m_shStartViewTileY;
			}
	
			// 바닥아이템 그리기.
			if ( bIsMapViewTime )
			{
				for ( nObjCnt = 0; nObjCnt < _MAX_CELL_OBJ; nObjCnt++ )
				{
					LPGROUNDITEM pstItem = m_stMemMappedID[nMapViewY][nMapViewX].pstItem[nObjCnt];
					if ( pstItem )
					{
						CWHWilImageData* pxItemImg = &g_xGameProc.m_xImage.m_xImageList[_IMAGE_ITEM_GROUND];
						if ( pxItemImg->NewSetIndex(pstItem->wLooks) )
						{
							nStartX = (pstItem->shTileX-m_xMap.m_shStartViewTileX)*_CELL_WIDTH+_VIEW_CELL_X_START-m_xMap.m_shViewOffsetX;
							nStartY = (pstItem->shTileY-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START-m_xMap.m_shViewOffsetY;
							INT nX  = nStartX+_CELL_WIDTH/2-pxItemImg->m_lpstNewCurrWilImageInfo->shWidth/2;
							INT nY  = nStartY+_CELL_HEIGHT/2-pxItemImg->m_lpstNewCurrWilImageInfo->shHeight/2;

							g_xMainWnd.DrawWithShadowABlend(
													nX+1, nY+1, 
													pxItemImg->m_lpstNewCurrWilImageInfo->shWidth,
													pxItemImg->m_lpstNewCurrWilImageInfo->shHeight,
													(WORD*)pxItemImg->m_pbCurrImage, 
													_CLIP_WIDTH, _CLIP_HEIGHT, m_wShadowClr, FALSE, 50);

							if ( nXCnt == m_ptCurrTile.x && nYCnt == m_ptCurrTile.y )
								g_xMainWnd.DrawWithImageForCompClipRgn(nX, nY, pxItemImg->m_lpstNewCurrWilImageInfo->shWidth,pxItemImg->m_lpstNewCurrWilImageInfo->shHeight,
																	   (WORD*)pxItemImg->m_pbCurrImage, _CLIP_WIDTH, _CLIP_HEIGHT, 0XFFFF, 0XFFFF, TRUE);
							else
								g_xMainWnd.DrawWithImageForCompClipRgn(nX, nY, pxItemImg->m_lpstNewCurrWilImageInfo->shWidth,pxItemImg->m_lpstNewCurrWilImageInfo->shHeight,
																	   (WORD*)pxItemImg->m_pbCurrImage, _CLIP_WIDTH, _CLIP_HEIGHT);

						}
					}
					else
						break;
				}
			}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 내캐릭터 그리기.
			if ( nXCnt == m_xMyHero.m_wPosX && nYCnt == m_xMyHero.m_wPosY )
			{
				// 그림의 시작좌표, 포커스상태, 그림자 블렌딩 상태를 넘겨준다.
				if ( PtInRect(&m_xMyHero.m_rcActor, m_ptMousePos) && !m_pxMouseTargetActor )
				{
					bFocused			 = TRUE;
					m_pxMouseTargetActor = &m_xMyHero;
				}
				else
					bFocused			 = FALSE;

				bDrawShadow = TRUE;
				if ( m_bShadowABlend )
					bDrawShadow = FALSE;

				m_xMyHero.DrawActor(bFocused, m_bShadowABlend, FALSE, bDrawShadow);
			} 

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 다른 캐릭터 그리기.
			if ( bIsMapViewTime )
			{
				for ( nObjCnt = 0; nObjCnt < _MAX_CELL_OBJ; nObjCnt++ )
				{
					CActor* pxActor = m_stMemMappedID[nMapViewY][nMapViewX].pxActor[nObjCnt];

					if ( pxActor )
					{
						BYTE bOpa = 50;

						if ( pxActor->m_bIsDead )
							bOpa = 30;

						if ( pxActor == m_pxMouseTargetActor )
							bFocused = TRUE;
						else
							bFocused = FALSE;

						bDrawShadow = TRUE;
						if ( pxActor == m_pxMouseTargetActor && m_bShadowABlend )
							bDrawShadow = FALSE;

						switch ( pxActor->m_stFeature.bGender )
						{
						case _GENDER_MAN:
						case _GENDER_WOMAN:
							{
								CHero* pxHero;
								pxHero = (CHero*)pxActor;

								pxHero->DrawActor(&m_xMap, bFocused, m_bShadowABlend, TRUE, bDrawShadow);
							}
							break;
						case _GENDER_NPC:
							{
								CNPC* pxNPC;
								pxNPC = (CNPC*)pxActor;

								pxNPC->DrawActor(&m_xMap, bFocused, m_bShadowABlend, TRUE, bDrawShadow);
							}
							break;
						case _GENDER_MON:
							{
								pxActor->DrawActor(&m_xMap, bFocused, m_bShadowABlend, TRUE, bDrawShadow);

/*								// 말일때.
								RECT rc, rcIntersect;
//								if ( bLongObj && pxActor->m_wPosY <= nYCnt && pxActor->m_stFeature.bDress > 114 && pxActor->m_stFeature.bDress < 118 )
								{
									SetRect(&rc, 
									(nXCnt-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START-m_xMap.m_shViewOffsetX, 
									(nYCnt-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START-m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight+_CELL_HEIGHT-m_xMap.m_shViewOffsetY, 
									m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shWidth  + (nXCnt-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START-m_xMap.m_shViewOffsetX, 
									m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight + (nYCnt-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START-m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight+_CELL_HEIGHT-m_xMap.m_shViewOffsetY);

									// 말과배경.
									if ( IntersectRect(&rcIntersect, &rc, &pxActor->m_rcActor) )
									{
										m_xImage.m_xImageList[pxActor->m_bActorImgIdx].NewSetIndex(pxActor->m_dwCurrFrame);
 										g_xMainWnd.DrawWithABlendForIntersectCompData(
														pxActor->m_rcActor.left, 
														pxActor->m_rcActor.top,
														m_xImage.m_xImageList[pxActor->m_bActorImgIdx].m_lpstNewCurrWilImageInfo->shWidth,
														m_xImage.m_xImageList[pxActor->m_bActorImgIdx].m_lpstNewCurrWilImageInfo->shHeight,
														(WORD*)m_xImage.m_xImageList[pxActor->m_bActorImgIdx].m_pbCurrImage,
														rc.left,
														rc.top,
														m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shWidth,
														m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight,
														(WORD*)m_xMap.m_pxTileImg[nObjFileIdx]->m_pbCurrImage,
														_CLIP_WIDTH, _CLIP_HEIGHT, 50, bFocused, 0XFFFF, 0XFFFF, 0XFFFF, 0XFFFF, m_xMyHero.m_wStateClr);
									}
								}
*/							}
							break;
						}
						
						bFocused = FALSE;
					}
					else
						break;
				}
			}
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			if ( bIsMapViewTime )
			{
				for ( nObjCnt = 0; nObjCnt < _MAX_CELL_OBJ; nObjCnt++ )
				{
					CMagic* pxMagic = m_stMemMappedID[nMapViewY][nMapViewX].pxMagic[nObjCnt];
					if ( pxMagic )
					{
						if ( pxMagic->m_bMagicNum == _SKILL_HOLYSHIELD || pxMagic->m_bMagicNum == _SKILL_EARTHFIRE )
						{
							CRepeatMagic* pxRepeatMagic = (CRepeatMagic*)pxMagic;

							pxRepeatMagic->DrawMagic();
						}
						else if ( pxMagic->m_bMagicNum == _SKILL_FIRE )
						{
							CMagicStream* pxMagicStream = (CMagicStream*)pxMagic;

							pxMagicStream->DrawMagic();
						}
					}
					else
						break;
				}
			}	
		}
	}		


	bDrawShadow = FALSE;
	if ( m_bShadowABlend )
		bDrawShadow = TRUE;

	m_xMyHero.m_wABlendDelay	 = 10;
	m_xMyHero.m_wABlendCurrDelay = 5;

	m_xMyHero.DrawActor(FALSE, m_bShadowABlend, FALSE, bDrawShadow);
	
	m_xMyHero.m_wABlendDelay	 = 0;
	m_xMyHero.m_wABlendCurrDelay = 0;

	if ( m_pxMouseTargetActor && m_pxMouseTargetActor != &m_xMyHero /*&& !m_pxMouseTargetActor->m_bIsDead*/ )
	{
		m_pxMouseTargetActor->m_wABlendDelay	 = 10;
		m_pxMouseTargetActor->m_wABlendCurrDelay = 4;

		m_pxMouseTargetActor->DrawActor(&m_xMap, FALSE, m_bShadowABlend, FALSE, bDrawShadow);
		
		m_pxMouseTargetActor->m_wABlendDelay	 = 0;
		m_pxMouseTargetActor->m_wABlendCurrDelay = 0;
	}
}









/******************************************************************************************************************

	메시지 관련 함수.

*******************************************************************************************************************/
LRESULT CGameProcess::OnDestroy(WPARAM wParam, LPARAM lParam)
{
	m_xInterface.m_xInventoryWnd.SaveItemPos(m_xMyHero.m_szName);
	return 0L;
}

LRESULT CGameProcess::OnTimer(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == _TIMER_ID )
	{
		DWORD	dwFogColor;
		FLOAT	fDstDarkRate;
		switch ( m_bFogState )
		{
		case _FOGSTATE_DAWN:
			{
				fDstDarkRate = 50;
			}
			break;
		case _FOGSTATE_DAY:
			{
				fDstDarkRate = 255;
			}
			break;
		case _FOGSTATE_NIGHT:
			{
				fDstDarkRate = 10;
			}
			break;
		}

		if		( m_fDarkRate > fDstDarkRate )	m_fDarkRate -= 0.5f;
		else if ( m_fDarkRate < fDstDarkRate )	m_fDarkRate += 0.5f;
		else	
		{
			KillTimer(g_xMainWnd.GetSafehWnd(), _TIMER_ID);
		}

		dwFogColor = RGB(m_fDarkRate, m_fDarkRate, m_fDarkRate); 
		m_xLightFog.ChangeLightColor(dwFogColor);
	}
	else	// Temp
	{
		if (PacketQ.GetCount())
		{
			char *pszPacket = (char*)PacketQ.PopQ();

			if (pszPacket)
			{
				strcpy(&szBuff[nBuff], pszPacket);

				char *pszRemain = OnMessageReceive(szBuff);

				if (pszRemain)
				{
					nBuff = strlen(pszRemain);
					memcpy(szBuff, pszRemain, nBuff);
					szBuff[nBuff] = '\0';
				}
				else
				{
					nBuff = 0;
					ZeroMemory(szBuff, sizeof(szBuff));
				}

				delete [] pszPacket;
			}
		}

		ProcessPacket();
		ProcessDefaultPacket();
	}

	return 0L;
}


LRESULT CGameProcess::OnLButtonDoubleClick(WPARAM wParam, LPARAM lParam)
{
	m_ptMousePos.x = LOWORD(lParam);
	m_ptMousePos.y = HIWORD(lParam);

	m_xInterface.OnLButtonDoubleClick(m_ptMousePos);

	return 0L;
}


LRESULT CGameProcess::OnWheel(WPARAM wParam, LPARAM lParam)
{
	INT nDelta;
	nDelta = ( (INT)wParam < 0 ) ? -1 : 1;
	if ( nDelta > 0 )
		m_xInterface.OnScrollDown();
	else
		m_xInterface.OnScrollUp();

	return 0L;
}


LRESULT CGameProcess::OnSysKeyDown(WPARAM wParam, LPARAM lParam)
{
	if ( wParam == VK_RETURN )
	{
		m_bRender = FALSE;
		g_xMainWnd.OnSysKeyDown(wParam, lParam);
		SetFocus(g_xChatEditBox.GetSafehWnd());
		SetFocus(g_xMainWnd.GetSafehWnd());

		for ( INT nCnt = 0; nCnt < 10; nCnt++)
		{
			if ( m_xImage.m_xImageList[_IMAGE_PROGUSE].NewSetIndex(nCnt+40) )
			{
				D3DWILTextr_CreateEmptyTexture(m_xImage.m_xImageList[_IMAGE_PROGUSE].m_szWilFileName, nCnt+40, 
											   m_xImage.m_xImageList[_IMAGE_PROGUSE].m_lpstNewCurrWilImageInfo->shWidth,
											   m_xImage.m_xImageList[_IMAGE_PROGUSE].m_lpstNewCurrWilImageInfo->shHeight,
											   (WORD*)m_xImage.m_xImageList[_IMAGE_PROGUSE].m_pbCurrImage, D3DTEXTR_TRANSPARENTBLACK);
				m_lpddsShineTextr[nCnt] = D3DWILTextr_RestoreEx(m_xImage.m_xImageList[_IMAGE_PROGUSE].m_szWilFileName, nCnt+40, g_xMainWnd.Get3DDevice());
			}
		}
		m_xImage.DelAllTextrFileList();

		m_bRender = TRUE;
	}

	if ( wParam == VK_F10 )
	{
		m_xInterface.OnSysKeyDown();
	}
	return 0L;
}


LRESULT CGameProcess::OnSysKeyUp(WPARAM wParam, LPARAM lParam)
{
	return 0L;
}


LRESULT CGameProcess::OnMove(WPARAM wParam, LPARAM lParam)
{
	g_xMainWnd.OnMove(wParam, lParam);
	MoveWindow(g_xChatEditBox.GetSafehWnd(), 
			   g_xMainWnd.m_rcWindow.left+_INPUT_EDITWND_XPOS, 
			   g_xMainWnd.m_rcWindow.top+_INPUT_EDITWND_YPOS, _INPUT_EDITWND_WIDTH, _INPUT_EDITWND_HEIGHT, TRUE);

	return 0L;
}


LRESULT CGameProcess::OnMouseMove(WPARAM wParam, LPARAM lParam)
{	
	m_ptMousePos.x = LOWORD(lParam);
	m_ptMousePos.y = HIWORD(lParam);
	m_ptCurrTile.x = m_xMap.m_shStartViewTileX + (m_ptMousePos.x - _VIEW_CELL_X_START) / TILE_WIDTH;
	m_ptCurrTile.y = m_xMap.m_shStartViewTileY + (m_ptMousePos.y - _VIEW_CELL_Y_START) / TILE_HEIGHT;

	if(m_xNotice.IsActive())
	{
		m_xNotice.MoveWnd(m_ptMousePos);
	}
	else
	{
		m_xInterface.OnMouseMove(m_ptMousePos);
	}
	return 0L;
}


LRESULT CGameProcess::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{	
	POINT ptTaretTilePos;

	// 게임데이타가 로드되었는지...
	if(m_xNotice.IsActive())
	{
		m_xNotice.OnButtonDown(m_ptMousePos);
	}

	if ( !m_bRender )			return 0L;

	m_pxSavedTargetActor = NULL;

	m_ptMousePos.x = LOWORD(lParam);
	m_ptMousePos.y = HIWORD(lParam);
	m_ptCurrTile.x = m_xMap.m_shStartViewTileX + (m_ptMousePos.x - _VIEW_CELL_X_START) / TILE_WIDTH;
	m_ptCurrTile.y = m_xMap.m_shStartViewTileY + (m_ptMousePos.y - _VIEW_CELL_Y_START) / TILE_HEIGHT;

	if ( !m_xInterface.OnLButtonDown(m_ptMousePos) )
	{
		// 인터페이스에서 하는일이 없을때.

		// 1. 아이템을 떨어뜨리기.
		if ( m_xInterface.m_stCommonItem.bSetted && !m_xInterface.m_stCommonItem.bWaitResponse && !m_xInterface.m_stCommonItem.bIsEquipItem )
		{
			g_xClientSocket.SendItemIndex(CM_DROPITEM, m_xInterface.m_stCommonItem.xItem.m_stItemInfo.nMakeIndex, m_xInterface.m_stCommonItem.xItem.m_stItemInfo.stStdItem.szName);
			m_xInterface.m_stCommonItem.bWaitResponse = TRUE;
			return 0L;
		}

		// 2. 맵아이템 줍기.
		if ( m_stMapItemList.GetCounter() != 0 )
		{
			m_stMapItemList.MoveCurrentToTop();
			LPGROUNDITEM pstItem;

			for ( INT nCnt = 0; nCnt < m_stMapItemList.GetCounter(); nCnt++ )
			{
				pstItem = m_stMapItemList.GetCurrentData();

				if ( pstItem->shTileX == m_ptCurrTile.x && pstItem->shTileY == m_ptCurrTile.y && pstItem->shTileX == m_xMyHero.m_wPosX && pstItem->shTileY == m_xMyHero.m_wPosY )
				{
					g_xClientSocket.SendPickUp(pstItem->shTileX, pstItem->shTileY);
					return 0L;
				}
				m_stMapItemList.MoveNextNode();
			}
		}

		if ( m_pxMouseTargetActor )
		{
			if ( !m_pxMouseTargetActor->m_bIsDead )
			{
				// 공격대상 Actor정하기.
				m_pxSavedTargetActor = m_pxMouseTargetActor;
			}

			ptTaretTilePos.x = m_pxMouseTargetActor->m_wPosX;
			ptTaretTilePos.y = m_pxMouseTargetActor->m_wPosY;
			if ( m_pxMouseTargetActor->m_stFeature.bGender == _GENDER_NPC )
			{
				g_xClientSocket.SendNPCClick(m_pxMouseTargetActor->m_dwIdentity);
				m_pxSavedTargetActor = NULL;
			}
			else
			{
				m_xMyHero.OnLButtonDown(m_ptMousePos, m_pxMouseTargetActor->m_dwIdentity, m_pxMouseTargetActor->m_bIsDead, &ptTaretTilePos);
			}
		}
		else
		{
			m_xMyHero.OnLButtonDown(m_ptMousePos);
		}
	}

	return 0L;
}


LRESULT CGameProcess::OnRButtonDown(WPARAM wParam, LPARAM lParam)
{	
	m_ptMousePos.x = LOWORD(lParam);
	m_ptMousePos.y = HIWORD(lParam);
	m_ptCurrTile.x = m_xMap.m_shStartViewTileX + (m_ptMousePos.x - _VIEW_CELL_X_START) / TILE_WIDTH;
	m_ptCurrTile.y = m_xMap.m_shStartViewTileY + (m_ptMousePos.y - _VIEW_CELL_Y_START) / TILE_HEIGHT;

	m_pxSavedTargetActor = NULL;

	m_xMyHero.OnRButtonDown(m_ptMousePos);

	return 0L;
}


LRESULT CGameProcess::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{
	m_ptMousePos.x = LOWORD(lParam);
	m_ptMousePos.y = HIWORD(lParam);
	m_ptCurrTile.x = m_xMap.m_shStartViewTileX + (m_ptMousePos.x - _VIEW_CELL_X_START) / TILE_WIDTH;
	m_ptCurrTile.y = m_xMap.m_shStartViewTileY + (m_ptMousePos.y - _VIEW_CELL_Y_START) / TILE_HEIGHT;
	if(m_xNotice.IsActive())
	{
		if(m_xNotice.OnButtonUp(m_ptMousePos))
		{
			// OK Button Click
			m_xNotice.HideMessageBox();
			Clear(RGB(0,0,0));
			// Start Game From Here
		}
	}
	else
		m_xInterface.OnLButtonUp(m_ptMousePos);

	return 0L;
}


LRESULT CGameProcess::OnRButtonUp(WPARAM wParam, LPARAM lParam)
{
	m_ptMousePos.x = LOWORD(lParam);
	m_ptMousePos.y = HIWORD(lParam);
	m_ptCurrTile.x = m_xMap.m_shStartViewTileX + (m_ptMousePos.x - _VIEW_CELL_X_START) / TILE_WIDTH;
	m_ptCurrTile.y = m_xMap.m_shStartViewTileY + (m_ptMousePos.y - _VIEW_CELL_Y_START) / TILE_HEIGHT;

	return 0L;
}


LRESULT CGameProcess::OnKeyUp(WPARAM wParam, LPARAM lParam)
{
	m_xInterface.OnKeyUp(wParam, lParam);
	return 0L;
}


LRESULT CGameProcess::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	INT		nTargetID;
	POINT	ptTaretTilePos;
	FEATURE stFeature;

	D3DMATRIX matTrans;
	D3DMATRIX matScale;
	D3DMATRIX matRot;
	D3DMATRIX matView;
	D3DMATRIX matTemp;
	FLOAT	  fRot = 0.0f;

	D3DVECTOR vTrans  = D3DVECTOR(0.0f, 0.0f, 0.0f);
	D3DVECTOR vRotate = D3DVECTOR(0.0f, 0.0f, 0.0f);
	D3DVECTOR vScale  = D3DVECTOR(1.0f, 1.0f, 1.0f);

	g_xMainWnd.Get3DDevice()->GetTransform(D3DTRANSFORMSTATE_VIEW, &matView);

	if ( m_pxMouseTargetActor )	
	{
		nTargetID			= m_pxMouseTargetActor->m_dwIdentity;
		ptTaretTilePos.x	= m_pxMouseTargetActor->m_wPosX;
		ptTaretTilePos.y	= m_pxMouseTargetActor->m_wPosY;
		stFeature			= m_pxMouseTargetActor->m_stFeature;
	}
	else
	{
		nTargetID			= 0;
		ptTaretTilePos.x	= 0;
		ptTaretTilePos.y	= 0;
		ZeroMemory(&stFeature, sizeof(FEATURE));
	}

	if ( !m_xInterface.OnKeyDown(wParam, lParam) )
		m_xMyHero.OnKeyDown(wParam, lParam, m_ptMousePos, ptTaretTilePos, nTargetID, stFeature);

	switch ( wParam )
	{
	case 'L':
		g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME);
		break;
	case 'M':
		g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_SOLID);
		break;
	case 'N':
		{
			SetNextProc();
			break;
		}
	case '6':
		{
/*			g_xSound.PlayActorWav(m_ptCurrTile.x, m_ptCurrTile.y, m_xMyHero.m_wPosX, m_xMyHero.m_wPosY, 1);
*/			break;
		}
	case '7':
		{
/*			g_xSound.StopSound(1);
*/			break;
		}
	case VK_RETURN:
		{
			SetFocus(g_xChatEditBox.GetSafehWnd());
			ShowWindow(g_xChatEditBox.GetSafehWnd(), SW_SHOW);
			OnMsgInputted(0, 0);
		}
		break;
	case 'U':
		{
			DWORD dwFogColor;

			m_bFogState++;

			if ( m_bFogState > _FOGSTATE_DAWN )			m_bFogState = _FOGSTATE_DAY;

			switch ( m_bFogState )
			{
			case _FOGSTATE_DAY:
				{
					m_fDarkRate = 255.0f;
					dwFogColor = RGB(m_fDarkRate, m_fDarkRate, m_fDarkRate);
					break;
				}
			case _FOGSTATE_NIGHT:
				{
					m_fDarkRate = 10.0f;
					dwFogColor = RGB(m_fDarkRate, m_fDarkRate, m_fDarkRate);
					break;
				}
			case _FOGSTATE_DAWN:
				{
					m_fDarkRate = 50.0f;
					dwFogColor = RGB(m_fDarkRate, m_fDarkRate, m_fDarkRate);
					break;
				}
			}

			m_xLightFog.ChangeLightColor(dwFogColor);
		}
		break;
	case VK_LEFT:
		{
			vTrans.x = -2.0f;
			D3DUtil_SetTranslateMatrix(matTrans, vTrans);
			D3DMath_MatrixMultiply(matTemp, matView, matTrans);
			g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_VIEW, &matTemp);
		}
		break;
	case VK_RIGHT:
		{
			vTrans.x = 2.0f;
			D3DUtil_SetTranslateMatrix(matTrans, vTrans);
			D3DMath_MatrixMultiply(matTemp, matView, matTrans);
			g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_VIEW, &matTemp);
		}
		break;
	case VK_UP:
		{
			vTrans.y = -2.0f;
			D3DUtil_SetTranslateMatrix(matTrans, vTrans);
			D3DMath_MatrixMultiply(matTemp, matView, matTrans);
			g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_VIEW, &matTemp);
		}
		break;
	case VK_DOWN:
		{
			vTrans.y = 2.0f;
			D3DUtil_SetTranslateMatrix(matTrans, vTrans);
			D3DMath_MatrixMultiply(matTemp, matView, matTrans);
			g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_VIEW, &matTemp);
		}
		break;
	case VK_INSERT:
		vScale.x = 1.01f;
		vScale.y = 1.01f;
		vScale.z = 1.00f;
		D3DUtil_SetScaleMatrix(matScale, vScale.x, vScale.y, vScale.z);
		D3DMath_MatrixMultiply(matTemp, matView, matScale);
		g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_VIEW, &matTemp);
		break;
	case VK_DELETE:
		vScale.x = 0.99f;
		vScale.y = 0.99f;
		vScale.z = 1.00f;
		D3DUtil_SetScaleMatrix(matScale, vScale.x, vScale.y, vScale.z);
		D3DMath_MatrixMultiply(matTemp, matView, matScale);
		g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_VIEW, &matTemp);
		break;
	case VK_NUMPAD1:
		fRot = 0.01f;
		D3DUtil_SetRotateYMatrix(matRot, fRot);
		D3DMath_MatrixMultiply(matTemp, matView, matRot);
		g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_VIEW, &matTemp);
		break;
	case VK_NUMPAD7:
		fRot = -0.01f;
		D3DUtil_SetRotateYMatrix(matRot, fRot);
		D3DMath_MatrixMultiply(matTemp, matView, matRot);
		g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_VIEW, &matTemp);
		break;
	case VK_NUMPAD4:
		fRot = 0.01f;
		D3DUtil_SetRotateZMatrix(matRot, fRot);
		D3DMath_MatrixMultiply(matTemp, matView, matRot);
		g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_VIEW, &matTemp);
		break;
	case VK_NUMPAD6:
		fRot = -0.01f;
		D3DUtil_SetRotateZMatrix(matRot, fRot);
		D3DMath_MatrixMultiply(matTemp, matView, matRot);
		g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_VIEW, &matTemp);
		break;
	case VK_NUMPAD2:
		fRot = 0.01f;
		D3DUtil_SetRotateXMatrix(matRot, fRot);
		D3DMath_MatrixMultiply(matTemp, matView, matRot);
		g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_VIEW, &matTemp);
		break;
	case VK_NUMPAD8:
		fRot = -0.01f;
		D3DUtil_SetRotateXMatrix(matRot, fRot);
		D3DMath_MatrixMultiply(matTemp, matView, matRot);
		g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_VIEW, &matTemp);
		break;
	}			

	return 0L;
}




/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 유저 디파인 메시지관련 함수.
LRESULT CGameProcess::OnMsgInputted(WPARAM wParam, LPARAM lParam)
{
	if ( m_xInterface.OnMsgInputted(wParam, lParam) )
	{
		return 0L;
	}

	if ( g_xChatEditBox.m_szInputMsg[0] == NULL )
		return 0L;

	// 커맨드 메시지.
	if ( wParam )
	{
		CommandAnalyze();
	}

	CHAR	szCommand[MAX_PATH];
	CHAR	szArg1[MAX_PATH];
	CHAR	szArg2[MAX_PATH];
	CHAR	szArg3[MAX_PATH];
	CHAR	szArg4[MAX_PATH];
	ZeroMemory(szCommand, MAX_PATH);
	ZeroMemory(szArg1, MAX_PATH);
	ZeroMemory(szArg2, MAX_PATH);
	ZeroMemory(szArg3, MAX_PATH);
	ZeroMemory(szArg4, MAX_PATH);
	sscanf(g_xChatEditBox.m_szInputMsg, "%[^ ]%*c %[^ ]%*c %[^ ]%*c %[^ ]%*c %[^ ]%*c", szCommand, szArg1, szArg2, szArg3, szArg4);
	INT nArg1;	INT nArg2;	INT nArg3;	INT nArg4;
	nArg1 = nArg2 = nArg3 = nArg4 = 0;
	if ( !strcmp(szCommand, "@옷염색") )
	{
		nArg1 = atoi(szArg1);
		nArg2 = atoi(szArg2);
		nArg3 = atoi(szArg3);
		
		WORD wColor = g_xMainWnd.ConvertColor24To16(RGB(nArg3, nArg2, nArg1));

		sprintf(g_xChatEditBox.m_szInputMsg, "@옷염색 %d", wColor );
		m_xMyHero.m_stFeatureEx.wDressColor = wColor;
	}	
	else if ( !strcmp(szCommand, "@speed") )
	{
		nArg1 = atoi(szArg1);
		m_xMyHero.m_shHitSpeed = nArg1;
	}	
	else if ( !strcmp(szCommand, "@npc") )
	{
		static int nNPC = 0;
		CActor* pxActor = NULL;

		nArg1 = atoi(szArg1);
		pxActor = new CNPC;
		CNPC* pxNPC = (CNPC*)pxActor;

		pxNPC->m_dwIdentity = nNPC;
		FEATURE stFeature;
		stFeature.bGender = _GENDER_NPC;
		stFeature.bDress = nArg1;
		stFeature.bHair = 0;
		stFeature.bWeapon = 0;

		if ( pxNPC->Create(&m_xImage, &stFeature, _MT_STAND, GetRandomNum(0, 7), m_xMyHero.m_wPosX, m_xMyHero.m_wPosY-1) )
			m_xActorList.AddNode(pxActor);

		pxNPC->m_wABlendCurrDelay = 0;
		pxNPC->m_wABlendDelay	  = 0;
		nNPC++;
	}	
	else if ( !strcmp(szCommand, "@빛") )
	{
		m_fTileLightRed = atoi(szArg1);
		m_fTileLightGreen = atoi(szArg2);
		m_fTileLightBlue = atoi(szArg3);
	}	
	else if ( !strcmp(szCommand, "@시간변화") )
	{
		m_bDayState = atoi(szArg1);
		m_bFogState = atoi(szArg2);
		SetTimer(g_xMainWnd.GetSafehWnd(), _TIMER_ID, 50, NULL);
	}	
/*	else if ( !strcmp(szCommand, "@젠곳") )
	{
		CHAR szTmp[MAX_PATH];
		nArg2 = atoi(szArg2);
		sprintf(szTmp, "%s %d %d %s %d", szCommand, m_ptCurrTile.x, m_ptCurrTile.y, szArg1, nArg2);
		strcpy(g_xChatEditBox.m_szInputMsg, szTmp);
	}*/	
	else if ( !strcmp(szCommand, "@그림자") )
	{
		m_bShadowABlend = !m_bShadowABlend;
		WORD wShadowClr;

		if ( m_bShadowABlend )
		{
			wShadowClr = g_xMainWnd.ConvertColor24To16(RGB(0, 0, 0));
		}
		else
		{
			wShadowClr = g_xMainWnd.ConvertColor24To16(RGB(20, 20, 20));
		}
			
		for ( INT nCnt = 0; nCnt < 800; nCnt++ )
		{
			m_wShadowClr[nCnt] = wShadowClr;
		}	

	}	
	else if ( !strcmp(szCommand, "@변신") )
	{		
		static FEATURE stLastCahrFeature;
		FEATURE stFeature;
		ZeroMemory(&stFeature, sizeof(FEATURE));

		if ( !m_xMyHero.m_bIsMon )	stLastCahrFeature = m_xMyHero.m_stFeature;

		if ( !strcmp(szArg1, "안하기") )
		{
			m_xMyHero.m_bIsMon = FALSE;
			stFeature = stLastCahrFeature;
			m_xMyHero.ChangeFeature(stFeature, m_xMyHero.m_stFeatureEx);
		}
		else
		{
			m_xMyHero.m_bIsMon = TRUE;
			stFeature.bGender = _GENDER_MON;
			stFeature.bDress = atoi(szArg1);
		}
		
		if ( !m_xMyHero.ChangeFeature(stFeature, m_xMyHero.m_stFeatureEx) )
		{
			strcpy(g_xChatEditBox.m_szInputMsg, "변신할수 없는 몬스터입니다.");
		}
		else
		{
			m_xMyHero.m_stFeature = stFeature;
		}
	}	
	else if ( !strcmp(szCommand, "@머리염색") )
	{
		nArg1 = atoi(szArg1);
		nArg2 = atoi(szArg2);
		nArg3 = atoi(szArg3);
		
		WORD wColor = g_xMainWnd.ConvertColor24To16(RGB(nArg3, nArg2, nArg1));

		sprintf(g_xChatEditBox.m_szInputMsg, "@머리염색 %d", wColor );
		m_xMyHero.m_stFeatureEx.wHairColor = wColor;
	}	
	else if ( !strcmp(szCommand, "@머리모양") )
	{
		nArg1 = atoi(szArg1);

		FEATURE stFeature = m_xMyHero.m_stFeature;
		stFeature.bHair = nArg1;
		if ( stFeature.bHair > 12 )
			stFeature.bHair = _HAIR_NONE;

		m_xMyHero.ChangeFeature(stFeature, m_xMyHero.m_stFeatureEx);
	}	

// 일반메시지.
	g_xClientSocket.SendSay(g_xChatEditBox.m_szInputMsg);

	if ( g_xChatEditBox.m_szInputMsg[0] == '/' )
	{
		DWORD dwBack = GetChatColor(_CHAT_COLOR6);
		DWORD dwFont = GetChatColor(_CHAT_COLOR3);
		m_xInterface.MsgAdd(dwFont, dwBack, g_xChatEditBox.m_szInputMsg);
	}

	ZeroMemory(g_xChatEditBox.m_szInputMsg, sizeof(g_xChatEditBox.m_szInputMsg));

	return 0L;
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////









/******************************************************************************************************************

	기타 함수및 디버그용 함수.

*******************************************************************************************************************/
VOID CGameProcess::ShowStatus(INT nStartX, INT nStartY)
{
	if ( m_bStatusView )
	{
		TCHAR			szBuff[MAX_PATH];

		sprintf(szBuff, "현재타일좌표 x:%d, y:%d", m_ptCurrTile.x, m_ptCurrTile.y);
		g_xMainWnd.PutsHan(g_xMainWnd.GetBackBuffer(), nStartX, nStartY+10, RGB(250, 250, 250), RGB(0, 0, 0), szBuff);

		sprintf(szBuff, "접속서버번호 : %s", g_szServerIP);
		g_xMainWnd.PutsHan(g_xMainWnd.GetBackBuffer(), nStartX, nStartY+25, RGB(250, 250, 250), RGB(0, 0, 0), szBuff);

		sprintf(szBuff, "마우스좌표 x:%d, y:%d", m_ptMousePos.x, m_ptMousePos.y);
		g_xMainWnd.PutsHan(g_xMainWnd.GetBackBuffer(), nStartX, nStartY+40, RGB(250, 250, 250), RGB(0, 0, 0), szBuff);

		sprintf(szBuff, "MYIDNUM:%x, %d", m_xMyHero.m_dwIdentity, m_xWaitPacketQueue.GetCount());
		g_xMainWnd.PutsHan(g_xMainWnd.GetBackBuffer(), nStartX, nStartY+55, RGB(250, 250, 250), RGB(0, 0, 0), szBuff);

		m_xActorList.MoveCurrentToTop();
		CActor* pxActor;
		for ( INT nCnt = 0; nCnt < m_xActorList.GetCounter(); nCnt++ )
		{
			pxActor = m_xActorList.GetCurrentData();

			sprintf(szBuff, "IDNUM:%d, X:%d, Y:%d, MSGCNT:%d", pxActor->m_dwIdentity, pxActor->m_wPosX, pxActor->m_wPosY, pxActor->m_xPacketQueue.GetCount());
			g_xMainWnd.PutsHan(g_xMainWnd.GetBackBuffer(), pxActor->m_shScrnPosX, pxActor->m_shScrnPosY, RGB(250, 250, 250), RGB(0, 0, 0), szBuff);
			m_xActorList.MoveNextNode();

		}

		g_xMainWnd.UsedAndFreeMemoryCheck();
		sprintf(szBuff, "텍스춰전체:%d, 텍스춰여유:%d, 사용한텍스춰:%d", g_xMainWnd.m_dwTextureTotal, g_xMainWnd.m_dwTextureFree, g_xMainWnd.m_dwTextureTotal-g_xMainWnd.m_dwTextureFree);
		g_xMainWnd.PutsHan(g_xMainWnd.GetBackBuffer(), nStartX, nStartY+70, RGB(250, 250, 250), RGB(0, 0, 0), szBuff);

		sprintf(szBuff, "비디오램전체:%d, 비디오램여유:%d, 사용한비디오램:%d", g_xMainWnd.m_dwVideoTotal, g_xMainWnd.m_dwVideoFree, g_xMainWnd.m_dwVideoTotal-g_xMainWnd.m_dwVideoFree);
		g_xMainWnd.PutsHan(g_xMainWnd.GetBackBuffer(), nStartX, nStartY+85, RGB(250, 250, 250), RGB(0, 0, 0), szBuff);
	}

	if ( m_bShowFPS )
		CWHDefProcess::ShowStatus(750, 0);
}



VOID CGameProcess::CommandAnalyze()
{
	CHAR	szCommand[MAX_PATH];
	CHAR	szArg1[MAX_PATH];
	CHAR	szArg2[MAX_PATH];
	CHAR	szArg3[MAX_PATH];
	CHAR	szArg4[MAX_PATH];

	ZeroMemory(szCommand, MAX_PATH);
	ZeroMemory(szArg1, MAX_PATH);
	ZeroMemory(szArg2, MAX_PATH);
	ZeroMemory(szArg3, MAX_PATH);
	ZeroMemory(szArg4, MAX_PATH);

	sscanf(g_xChatEditBox.m_szInputMsg, "%[^ ]%*c %[^ ]%*c %[^ ]%*c %[^ ]%*c %[^ ]%*c", szCommand, szArg1, szArg2, szArg3, szArg4);

	INT nArg1;	INT nArg2;	INT nArg3;	INT nArg4;
	nArg1 = nArg2 = nArg3 = nArg4 = 0;

	if ( !strcmp(szCommand, "@grid") )
	{
		nArg1 = atoi(szArg1);
		switch ( nArg1 )
		{
		case 0:
			m_bCellView= !m_bCellView;
			break;
		case 1:
			m_bTileView = !m_bTileView;
			break;
		case 2:
			m_bObj1View = !m_bObj1View;
			break;
		case 3:
			m_bObj2View = !m_bObj2View;
			break;
		case 4:
			m_bStopPtView = !m_bStopPtView;
			break;
		}
	}
	else if ( !strcmp(szCommand, "@fps") )
	{
		m_bShowFPS = !m_bShowFPS;
	}
	else if ( !strcmp(szCommand, "@show") )
	{
		if ( !strcmp(szArg1, "snow") )
		{
			m_bShowSnow = !m_bShowSnow;
		}
		else if ( !strcmp(szArg1, "rain") )
		{
			m_bShowRain = !m_bShowRain;
		}
		else if ( !strcmp(szArg1, "fog2") )
		{
			m_bShowMist = !m_bShowMist;
		}
		else if ( !strcmp(szArg1, "state") )
		{
			m_bStatusView = !m_bStatusView;
		}
	}
/*	else if ( !strcmp(szCommand, "@이동") )
	{
		nArg1 = atoi(szArg1);
		nArg2 = atoi(szArg2);

		m_xMyHero.m_wPosX = nArg1+11;
		m_xMyHero.m_wPosY = nArg2+11;
		m_xMap.SetStartViewTile(nArg1, nArg2);
		m_xMap.LoadNewMapBuffer();
	}
	else if ( !strcmp(szCommand, "@자유이동") )
	{
		m_bRender = FALSE;
		nArg2 = atoi(szArg2);
		nArg3 = atoi(szArg3);

		m_xMyHero.m_wPosX = nArg2+11;
		m_xMyHero.m_wPosY = nArg3+11;
		m_xMap.SetStartViewTile(nArg2, nArg3);
		LoadMapChanged(szArg1);
		Sleep(100);
		m_bRender = TRUE;
	}*/	
	else if ( !strcmp(szCommand, "@fix") )
	{
		if ( !strcmp(szArg1, "delay") )
		{
			nArg2 = atoi(szArg2);
			m_xMyHero.m_wDelay	= nArg2;
		}
	}	
}


VOID CGameProcess::RenderMapTileGrid()
{
	WORD	bCanMove = FALSE;
	INT		nObjCnt1 = 0;
	INT		nObjCnt2 = 0;

	INT		nObjFileIdx;
	INT		nFileIdx;
	INT		nImgIdx;

	RECT	rc;

	if ( m_bCellView || m_bTileView || m_bObj1View || m_bObj2View || m_bStopPtView )
	{
		for ( INT nXCnt = m_xMap.m_shStartViewTileX; nXCnt < m_xMap.m_shStartViewTileX+_VIEW_CELL_X_COUNT; nXCnt++ )
			for ( INT nYCnt = m_xMap.m_shStartViewTileY; nYCnt < m_xMap.m_shStartViewTileY+_VIEW_CELL_Y_COUNT_FOR_OBJ; nYCnt++ )
		{
			if ( nXCnt >= m_xMap.m_stMapFileHeader.shWidth || nYCnt >= m_xMap.m_stMapFileHeader.shHeight || nXCnt < 0 || nYCnt < 0 )
				continue; 

			INT nArrayNum = nYCnt + nXCnt*m_xMap.m_stMapFileHeader.shHeight;

			// Cell그리드 보여주기.
			if ( m_bCellView )
			{
				SetRect(&rc,
					   (nXCnt-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START-m_xMap.m_shViewOffsetX,
					   (nYCnt-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START-m_xMap.m_shViewOffsetY,
					   (nXCnt-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START+_CELL_WIDTH -m_xMap.m_shViewOffsetX,
					   (nYCnt-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START+_CELL_HEIGHT-m_xMap.m_shViewOffsetY);
				g_xMainWnd.DrawWithGDI(rc, NULL, RGB(0, 255, 255), 1);
			}

			if ( m_bTileView )
			{
				// 타일그리드보여주기.
 				if( !( nXCnt % 2 ) && !( nYCnt % 2 ) && nYCnt < m_xMap.m_shStartViewTileY+_VIEW_CELL_Y_COUNT )
 				{
					nFileIdx= m_xMap.m_pstTileInfo[(nYCnt/2) + (nXCnt/2)*m_xMap.m_stMapFileHeader.shHeight/2].bFileIdx;
					nImgIdx = m_xMap.m_pstTileInfo[(nYCnt/2) + (nXCnt/2)*m_xMap.m_stMapFileHeader.shHeight/2].wTileIdx;

					if( nFileIdx != 255 && nImgIdx != 65535 )
					{
						m_xMap.m_pxTileImg[nFileIdx]->NewSetIndex(nImgIdx);

						SetRect(&rc,
							   (nXCnt-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START-m_xMap.m_shViewOffsetX,
							   (nYCnt-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START-m_xMap.m_shViewOffsetY,
							   (nXCnt-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START+m_xMap.m_pxTileImg[nFileIdx]->m_lpstNewCurrWilImageInfo->shWidth -m_xMap.m_shViewOffsetX,
							   (nYCnt-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START+m_xMap.m_pxTileImg[nFileIdx]->m_lpstNewCurrWilImageInfo->shHeight-m_xMap.m_shViewOffsetY);
						g_xMainWnd.DrawWithGDI(rc, NULL, RGB(255, 0, 0), 1);
					}
				}
			}

			if ( m_bObj1View )
			{
				// Obj1타일 그리드 보여주기.
				if ( m_xMap.m_pstCellInfo[nArrayNum].bFileIdx != 255 )
				{
					if ( m_xMap.m_pstCellInfo[nArrayNum].wObj1 != 255 )
					{
						nObjFileIdx = (m_xMap.m_pstCellInfo[nArrayNum].bFileIdx & 0XF0) >> 4;
						nImgIdx		= m_xMap.m_pstCellInfo[nArrayNum].wObj1;

						if ( nObjFileIdx > 2 && nObjFileIdx < 14 )
						{
							m_xMap.m_pxTileImg[nObjFileIdx]->NewSetIndex(nImgIdx);
							SetRect(&rc,
								   (nXCnt-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START-m_xMap.m_shViewOffsetX,
								   (nYCnt-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START-m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight+_CELL_HEIGHT-m_xMap.m_shViewOffsetY,
								   (nXCnt-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START+m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shWidth -m_xMap.m_shViewOffsetX,
								   (nYCnt-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START+m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight-m_xMap.m_shViewOffsetY);

//							if ( PtInRect(&rc, m_ptMousePos) )
							{
								nObjCnt1++;
								g_xMainWnd.DrawWithGDI(rc, NULL, RGB(255, 0, 255), 1);

								TCHAR szBuff[MAX_PATH];
								sprintf(szBuff, "[%d]%d%d", nObjCnt1, nXCnt, nYCnt);
								g_xMainWnd.PutsHan(g_xMainWnd.GetBackBuffer(), 
												   (nXCnt-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START-m_xMap.m_shViewOffsetX,
												   (nYCnt-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START-m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight+_CELL_HEIGHT-m_xMap.m_shViewOffsetY,
												   RGB(255, 0, 255), RGB(0, 0, 0), szBuff);
							}
						}
					}
				}
			}
			
			if ( m_bObj2View )
			{
				// Obj2타일 그리드 보여주기.
				if ( m_xMap.m_pstCellInfo[nArrayNum].bFileIdx != 255 )
				{
					if ( m_xMap.m_pstCellInfo[nArrayNum].wObj2 != 65535 /*&& m_shObjTileViewState == 2*/ )
					{
						nObjFileIdx = (m_xMap.m_pstCellInfo[nArrayNum].bFileIdx & 0X0F);
						nImgIdx		= m_xMap.m_pstCellInfo[nArrayNum].wObj2;

						if ( nObjFileIdx > 2 && nObjFileIdx < 14 )
						{
							m_xMap.m_pxTileImg[nObjFileIdx]->NewSetIndex(nImgIdx);
							SetRect(&rc,
								   (nXCnt-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START-m_xMap.m_shViewOffsetX,
								   (nYCnt-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START-m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight+_CELL_HEIGHT-m_xMap.m_shViewOffsetY,
								   (nXCnt-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START+m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shWidth -m_xMap.m_shViewOffsetX,
								   (nYCnt-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START+_CELL_HEIGHT-m_xMap.m_shViewOffsetY);

//							if ( PtInRect(&rc, m_ptMousePos) )
							{
								nObjCnt2++;
								g_xMainWnd.DrawWithGDI(rc, NULL, RGB(0, 255, 255), 1);

								TCHAR szBuff[MAX_PATH];
								sprintf(szBuff, "[%d]%d%d", nObjCnt2, nXCnt, nYCnt);
								g_xMainWnd.PutsHan(g_xMainWnd.GetBackBuffer(), 
												   (nXCnt-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START-m_xMap.m_shViewOffsetX,
												   (nYCnt-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START-m_xMap.m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight+_CELL_HEIGHT-m_xMap.m_shViewOffsetY,
												   RGB(0, 255, 255), RGB(0, 0, 0), szBuff);
							}
						}
					}
				}
			}

			if ( m_bStopPtView )
			{
				bCanMove  = m_xMap.GetTileAttribute(nXCnt, nYCnt);
				if ( bCanMove != _CAN_WALK && m_bStopPtView )
				{
					SetRect(&rc,
						   (nXCnt-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START-m_xMap.m_shViewOffsetX,
						   (nYCnt-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START-m_xMap.m_shViewOffsetY,
						   (nXCnt-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START+_CELL_WIDTH -m_xMap.m_shViewOffsetX,
						   (nYCnt-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START+_CELL_HEIGHT-m_xMap.m_shViewOffsetY);
					g_xMainWnd.DrawWithGDI(rc, NULL, RGB(255, 255, 0), 1);
				}
			}

/*			if ( ( m_xMap.m_pstCellInfo[nArrayNum].wLigntNEvent & 0X07 ) == 2 )
			{
				SetRect(&rc,
					   (nXCnt-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START-m_xMap.m_shViewOffsetX,
					   (nYCnt-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START-m_xMap.m_shViewOffsetY,
					   (nXCnt-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START+_CELL_WIDTH -m_xMap.m_shViewOffsetX,
					   (nYCnt-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START+_CELL_HEIGHT-m_xMap.m_shViewOffsetY);
				g_xMainWnd.DrawWithGDI(rc, NULL, RGB(255, 255, 255), 2);
			}
*/
		}
	}
	for ( INT nXCnt = m_xMap.m_shStartViewTileX; nXCnt < m_xMap.m_shStartViewTileX+_VIEW_CELL_X_COUNT; nXCnt++ )
		for ( INT nYCnt = m_xMap.m_shStartViewTileY; nYCnt < m_xMap.m_shStartViewTileY+_VIEW_CELL_Y_COUNT_FOR_OBJ; nYCnt++ )
	{
		if ( nXCnt >= m_xMap.m_stMapFileHeader.shWidth || nYCnt >= m_xMap.m_stMapFileHeader.shHeight || nXCnt < 0 || nYCnt < 0 )
			continue; 

//		if ( m_xMap.GetDoor(nXCnt, nYCnt) )
		if ( !m_xMap.IsDoorOpen(nXCnt, nYCnt) )
		{
			SetRect(&rc,
				   (nXCnt-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START-m_xMap.m_shViewOffsetX,
				   (nYCnt-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START-m_xMap.m_shViewOffsetY,
				   (nXCnt-m_xMap.m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START+_CELL_WIDTH -m_xMap.m_shViewOffsetX,
				   (nYCnt-m_xMap.m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START+_CELL_HEIGHT-m_xMap.m_shViewOffsetY);
			g_xMainWnd.DrawWithGDI(rc, NULL, RGB(255, 255, 255), 1);
		}
	}
}


LRESULT CGameProcess::DefMainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch ( uMsg )
	{
		case WM_CTLCOLOREDIT:
		{
			if ((HWND)lParam == g_xChatEditBox.GetSafehWnd())
			{
				SetBkColor((HDC)wParam, RGB(0, 125, 125));
				SetTextColor((HDC)wParam, RGB(255, 255, 255));

				UnrealizeObject(m_hBrush);
				
				POINT pt;

				SetBrushOrgEx((HDC)wParam, 0, 0, &pt);

				return (LRESULT)m_hBrush;
			}

			break;
		}
		case WM_DESTROY:
			OnDestroy(wParam, lParam);
			break;
		case WM_TIMER:
			OnTimer(wParam, lParam);
			break;
		case WM_MOUSEMOVE:
			OnMouseMove(wParam, lParam);
			break;
		case WM_LBUTTONDBLCLK:
			OnLButtonDoubleClick(wParam, lParam);
			break;

		case WM_LBUTTONDOWN:
			OnLButtonDown(wParam, lParam);
			break;
		case WM_RBUTTONDOWN:
			OnRButtonDown(wParam, lParam);
			break;
		case WM_KEYDOWN:
			OnKeyDown(wParam, lParam);
			break;
		case WM_KEYUP:
			OnKeyUp(wParam, lParam);
			break;
		case WM_LBUTTONUP:
			OnLButtonUp(wParam, lParam);
			break;
		case WM_RBUTTONUP:
			OnRButtonUp(wParam, lParam);
			break;
		case WM_MOVE:
			OnMove(wParam, lParam);
			break;
		case WM_SYSKEYDOWN:
			return OnSysKeyDown(wParam, lParam);
		case WM_SYSKEYUP:
			return OnSysKeyUp(wParam, lParam);
		case WM_MOUSEWHEEL:
			return OnWheel(wParam,lParam);
		case _WM_USER_MSG_INPUTTED:
			return OnMsgInputted(wParam, lParam);
		case ID_SOCKCLIENT_EVENT_MSG:
			g_xClientSocket.OnSocketMessage(wParam, lParam);
			break;
		default:
			break;
	}

	return CWHDefProcess::DefMainWndProc(hWnd, uMsg, wParam, lParam);
}

VOID CGameProcess::OnConnectToServer()
{
	g_xClientSocket.SendRunLogin(g_szUserID, g_szCharName, g_nCertifyCode, g_nVersion);
}

char* CGameProcess::OnMessageReceive(CHAR* pszMessage)
{
	char	*pszFirst;
	char	*pszEnd;
	int		nCount = 0;

	if (*pszMessage == '*')
	{
		pszFirst = pszMessage + 1;
		send(g_xClientSocket.m_sockClient, "*", 1, 0);
	}
	else
		pszFirst = pszMessage;

	while (pszEnd = strchr(pszFirst, '!'))
	{
		*pszEnd++ = '\0';

		OnSocketMessageRecieve(pszFirst + 1);

		if (*pszEnd == '*')
		{
			send(g_xClientSocket.m_sockClient, "*", 1, 0);
			pszEnd++;
		}

		if (*pszEnd == '#')
			pszFirst = pszEnd;
		else
			return NULL;
	}

	return pszFirst;
}

void CGameProcess::OnProcPacketNotEncode(char *pszMsg)
{
	static char pszGood[]	= "GOOD";
	static char pszFail[]	= "FAIL";
	static char pszLng[]	= "LNG";
	static char pszUlng[]	= "ULNG";
	static char pszWid[]	= "WID";
	static char pszUWid[]	= "UWID";
	static char pszFir[]	= "FIR";
	static char pszUFir[]	= "UFIR";
	static char pszUPwr[]	= "PWR";

	if ( memcmp(pszMsg, pszGood, strlen(pszGood)) == 0 )
		m_xMyHero.m_bMotionLock = FALSE;
 	else if( memcmp(pszMsg, pszFail, strlen(pszFail)) == 0 )
		m_xMyHero.SetOldPosition();
 	else if( memcmp(pszMsg, pszLng, strlen(pszLng)) == 0 )
		m_xMyHero.m_bUseErgum  = TRUE;
 	else if( memcmp(pszMsg, pszUlng, strlen(pszUlng)) == 0 )
		m_xMyHero.m_bUseErgum  = FALSE;
 	else if( memcmp(pszMsg, pszWid, strlen(pszWid)) == 0 )
		m_xMyHero.m_bUseBanwol = TRUE;
 	else if( memcmp(pszMsg, pszUWid, strlen(pszUWid)) == 0 )
		m_xMyHero.m_bUseBanwol = FALSE;
 	else if( memcmp(pszMsg, pszFir, strlen(pszFir)) == 0 )
		m_xMyHero.m_bFireHitCnt  = 1;
 	else if( memcmp(pszMsg, pszUFir, strlen(pszUFir)) == 0 )
		m_xMyHero.m_bFireHitCnt  = 0;
 	else if( memcmp(pszMsg, pszUPwr, strlen(pszUPwr)) == 0 )
		m_xMyHero.m_bYedoCnt  = 1;
}

CActor*	CGameProcess::FindActor(int nID) // gadget
{
	CActor* pxActor = NULL;

	m_xActorList.MoveCurrentToTop();
	
	for ( INT nCnt = 0; nCnt < m_xActorList.GetCounter(); nCnt++ )
	{
		pxActor = m_xActorList.GetCurrentData();

		if ( pxActor->m_dwIdentity == nID )
			return pxActor;
	
		m_xActorList.MoveNextNode();
	}

	return NULL;
}

void CGameProcess::OnSocketMessageRecieve(char *pszMsg)
{
	if (*pszMsg == '+')
		OnProcPacketNotEncode(pszMsg + 1);
	else
	{
		int					nPos = 0;
		_TDEFAULTMESSAGE	tdm;

		fnDecodeMessage(&tdm, pszMsg);

		switch (tdm.wIdent)
		{
			case SM_SENDNOTICE:
			{
				char szDecodeMsg[8192];

				nPos = fnDecode6BitBuf((pszMsg + DEFBLOCKSIZE), szDecodeMsg, sizeof(szDecodeMsg));
				szDecodeMsg[nPos] = '\0';
				m_xNotice.ShowMessageBox(szDecodeMsg, _IDX_IMG_NOTICEBOX);

				break;
			}
			case SM_NEWMAP:
				OnSvrMsgNewMap(&tdm, (pszMsg + DEFBLOCKSIZE));
				break;
			case SM_LOGON:
				OnSvrMsgLogon(&tdm, (pszMsg + DEFBLOCKSIZE));
				break;
			case SM_MAPDESCRIPTION:
				nPos = fnDecode6BitBuf((pszMsg + DEFBLOCKSIZE), (char*)m_szMapName, sizeof(m_szMapName));
				m_szMapName[nPos] = '\0';
				break;
			case SM_ABILITY:
			{
				m_xMyHero.m_nGlod	= tdm.nRecog;
				m_xMyHero.m_bJob	= tdm.wParam;

				fnDecode6BitBuf((pszMsg + DEFBLOCKSIZE), (char*)&m_xMyHero.m_stAbility, sizeof(ACTORABILITY));

				break;
			}
			case SM_WINEXP:
			{
				CHAR szExp[MAX_PATH];											
				
				m_xMyHero.m_stAbility.dwExp = tdm.nRecog;
				
				sprintf(szExp, "경험치를 %d 얻었습니다.", tdm.wParam);
				
				DWORD dwFont = GetChatColor(_CHAT_COLOR4);
				DWORD dwBack = GetChatColor(_CHAT_COLOR3);
				m_xInterface.MsgAdd(dwFont, dwBack, szExp);

				break;
			}
			case SM_SUBABILITY:
			{
				m_xMyHero.m_stSubAbility.wAntiMagic		= LOWORD(LOBYTE(tdm.nRecog));	
				m_xMyHero.m_stSubAbility.bHitPoint		= LOBYTE(tdm.wParam);	
				m_xMyHero.m_stSubAbility.bSpeedPoint	= HIBYTE(tdm.wParam);	
				m_xMyHero.m_stSubAbility.bAntiPoison	= LOBYTE(tdm.wTag);	
				m_xMyHero.m_stSubAbility.bPoisonRecover = HIBYTE(tdm.wTag);				
				m_xMyHero.m_stSubAbility.bHealthRecover = LOBYTE(tdm.wSeries);
				m_xMyHero.m_stSubAbility.bSpellRecover	= HIBYTE(tdm.wSeries);

				break;
			}
			case SM_SENDMYMAGIC:
			{
				SAFE_DELETE(m_xInterface.m_xStatusWnd.m_pstMyMagic);
				
				m_xInterface.m_xStatusWnd.m_bMyMagicCnt = tdm.wSeries;
				m_xInterface.m_xStatusWnd.m_pstMyMagic	= new CLIENTMAGICRCD[m_xInterface.m_xStatusWnd.m_bMyMagicCnt];
				
				ZeroMemory(m_xInterface.m_xStatusWnd.m_pstMyMagic, sizeof(CLIENTMAGICRCD) * m_xInterface.m_xStatusWnd.m_bMyMagicCnt);

				char *pszMagic = (pszMsg + DEFBLOCKSIZE);
				char *pszNextMagic;

				for ( INT nLoop = 0; nLoop < m_xInterface.m_xStatusWnd.m_bMyMagicCnt; nLoop++ )
				{
					if (pszNextMagic = strchr(pszMagic, '/'))
					{
						*pszNextMagic = '\0';

						fnDecode6BitBuf(pszMagic, (char*)&m_xInterface.m_xStatusWnd.m_pstMyMagic[nLoop], sizeof(CLIENTMAGICRCD));

						pszMagic = pszNextMagic + 1;
					}
				}

				break;
			} 
			case SM_MAGIC_LVEXP:
			{
				for ( INT nLoop = 0; nLoop < m_xInterface.m_xStatusWnd.m_bMyMagicCnt; nLoop++ )
				{
					if ( m_xInterface.m_xStatusWnd.m_pstMyMagic[nLoop].stStdMagic.wMagicID == tdm.nRecog )
					{
						m_xInterface.m_xStatusWnd.m_pstMyMagic[nLoop].nCurrTrain	= MAKELONG(tdm.wTag, tdm.wSeries);
						m_xInterface.m_xStatusWnd.m_pstMyMagic[nLoop].bLevel		= tdm.wParam;

						break;
					}
				}
				break;
			}
			case SM_BAGITEMS:
			{
				char *pszItem  = (pszMsg + DEFBLOCKSIZE);
				char *pszNextItem;
				int  nCount = 0;

				CLIENTITEMRCD	stReceivedItem;
				CItem			xItem;

				m_xInterface.m_xInventoryWnd.DeleteAllInvenItem();

				do
				{
					if (pszNextItem = strchr(pszItem, '/'))
					{
						*pszNextItem = '\0';

						fnDecode6BitBuf(pszItem, (char*)&stReceivedItem, sizeof(CLIENTITEMRCD));

						xItem.SetItem(stReceivedItem, 0);

						m_xInterface.m_xInventoryWnd.AddInvenItem(xItem, 0, FALSE);

						pszItem = pszNextItem + 1;

						nCount++;
					}
				} while (nCount < tdm.wSeries && pszNextItem);
				break;
			}
			case SM_SENDUSEITEMS:
			{
				CLIENTITEMRCD cItemRcd[9];

				char *pszUseIndex = (pszMsg + DEFBLOCKSIZE);
				char *pszItem;
				char *pszNextItem;

				ZeroMemory(cItemRcd, sizeof(CLIENTITEMRCD));

				for (int i = 0; i < 9; i++)
				{
					if (pszItem = strchr(pszUseIndex, '/'))
					{
						*pszItem = '\0';
						pszItem++;
						
						if (pszNextItem = strchr(pszItem, '/'))
						{
							*pszNextItem = '\0';

							fnDecode6BitBuf(pszItem, (char*)&cItemRcd[i], sizeof(CLIENTITEMRCD));	
							
							CItem xItem;
							xItem.SetItem(cItemRcd[i], 0);

							m_xInterface.m_xStatusWnd.m_stEquipItem[atoi(pszUseIndex)].xItem = xItem;
							m_xInterface.m_xStatusWnd.m_stEquipItem[atoi(pszUseIndex)].bSetted = TRUE;

							pszUseIndex = pszNextItem + 1;
						}
						else
							break;
					}
					else
						break;
				}

				break;
			}
			case SM_ADDITEM:
			{											
				CLIENTITEMRCD	stReceivedItem;

				char *pszItem = (pszMsg + DEFBLOCKSIZE);

				fnDecode6BitBuf(pszItem, (char*)&stReceivedItem, sizeof(CLIENTITEMRCD));
				CItem xItem;
				xItem.SetItem(stReceivedItem, 0);
				m_xInterface.m_xInventoryWnd.AddInvenItem(xItem, 0, FALSE);

				CHAR szSysMsg[MAX_PATH];
				sprintf(szSysMsg, "%s을 얻었습니다.", xItem.m_stItemInfo.stStdItem.szName);
				m_xInterface.m_xClientSysMsg.AddSysMsg(szSysMsg);
				break;
			}
			case SM_DELITEM:
			{											
				CLIENTITEMRCD	stReceivedItem;

				char *pszItem = (pszMsg + DEFBLOCKSIZE);

				fnDecode6BitBuf(pszItem, (char*)&stReceivedItem, sizeof(CLIENTITEMRCD));

				m_xInterface.m_xInventoryWnd.DeleteInvenItem(stReceivedItem.nMakeIndex, stReceivedItem.stStdItem.szName);
				m_xInterface.m_xStatusWnd.DeleteEquipItem(stReceivedItem.nMakeIndex, stReceivedItem.stStdItem.szName);

				CHAR szSysMsg[MAX_PATH];
				sprintf(szSysMsg, "%s아이템이 삭제되었습니다.", stReceivedItem.stStdItem.szName);
				m_xInterface.m_xClientSysMsg.AddSysMsg(szSysMsg);
				break;
			}
			case SM_DELITEMS:
			{		
				CHAR szSysMsg[MAX_PATH];

				char szItemName[64];
				char szItemIndex[32];
				char nItemIndex;

				char *pszItem  = (pszMsg + DEFBLOCKSIZE);
				char *pszItemIndex;
				int  nCount = 0;

				do
				{
					if (pszItemIndex = strchr(pszItem, '/'))
					{
						*pszItemIndex++ = '\0';

						nPos = fnDecode6BitBuf(pszItem, (char*)&szItemName, sizeof(szItemName));
						szItemName[nPos] = '\0';

						if (pszItem = strchr(pszItemIndex, '/'))
						{
							*pszItem++ = '\0';

							fnDecode6BitBuf(pszItem, (char*)&szItemIndex, sizeof(szItemIndex));
							szItemIndex[nPos] = '\0';

							nItemIndex = atoi(szItemIndex);

							m_xInterface.m_xInventoryWnd.DeleteInvenItem(nItemIndex, szItemName);
							m_xInterface.m_xStatusWnd.DeleteEquipItem(nItemIndex, szItemName);

							sprintf(szSysMsg, "%s아이템이 삭제되었습니다.", szItemName);
							m_xInterface.m_xClientSysMsg.AddSysMsg(szSysMsg);

							nCount++;
						}
					}
				} while (nCount < tdm.wSeries && pszItem);

				break;
			}
			case SM_EAT_OK:
			{
				m_xInterface.m_xInventoryWnd.AddNewBeltItem(&m_xInterface.m_stCommonItem);
				ZeroMemory(&m_xInterface.m_stCommonItem, sizeof(COMMONITEMSET));
				break;
			}
			case SM_EAT_FAIL:
			{
				if ( m_xInterface.m_stCommonItem.bSetted )
				{
					if ( !m_xInterface.m_stCommonItem.bIsBeltItem )
						m_xInterface.m_xInventoryWnd.AddInvenItem(m_xInterface.m_stCommonItem.xItem, m_xInterface.m_stCommonItem.xItem.m_shCellNum, TRUE);
					else
						m_xInterface.m_xInventoryWnd.AddBeltItem(m_xInterface.m_stCommonItem.xItem, m_xInterface.m_stCommonItem.xItem.m_shCellNum, TRUE);
				}
				ZeroMemory(&m_xInterface.m_stCommonItem, sizeof(COMMONITEMSET));
				break;
			}
			case SM_ADDMAGIC:
			{
				m_xInterface.m_xStatusWnd.m_bMyMagicCnt++;
				CLIENTMAGICRCD* pstMagicRCD;
				pstMagicRCD = new CLIENTMAGICRCD[m_xInterface.m_xStatusWnd.m_bMyMagicCnt];

				memcpy(pstMagicRCD, m_xInterface.m_xStatusWnd.m_pstMyMagic, sizeof(CLIENTMAGICRCD)*(m_xInterface.m_xStatusWnd.m_bMyMagicCnt-1));

				fnDecode6BitBuf(&pszMsg[_DEFBLOCKSIZE], (char*)&pstMagicRCD[m_xInterface.m_xStatusWnd.m_bMyMagicCnt-1], sizeof(CLIENTMAGICRCD));

				SAFE_DELETE(m_xInterface.m_xStatusWnd.m_pstMyMagic);

				m_xInterface.m_xStatusWnd.m_pstMyMagic = pstMagicRCD;
				break;
			}
			case SM_TAKEON_OK:
			{
				FEATURE stFeature;
				memcpy(&stFeature, &tdm.nRecog, sizeof(LONG));
				g_xGameProc.m_xMyHero.ChangeFeature(stFeature, g_xGameProc.m_xMyHero.m_stFeatureEx);

				// 장착창의 착용아이템(m_stTakeOnItemSet)을 장착창의 맞는셀에 적용시키고, 착용아이템(m_stTakeOnItemSet)을 지운다.
				m_xInterface.m_xStatusWnd.SetEquipFromTakeOn();
				// 만약 공통(마우스)아이템이 있다면 인벤토리윈도우의 빈셀에 넣고, 공통(마우스)아이템을 지운다.
				if ( m_xInterface.m_stCommonItem.bSetted )
				{
//					m_xInterface.m_xInventoryWnd.AddInvenItem(m_xInterface.m_stCommonItem.xItem, 0, FALSE);
					ZeroMemory(&m_xInterface.m_stCommonItem, sizeof(ITEMSET));
				}
				break;
			}
			case SM_TAKEON_FAIL:
			{
				// 장착창의 착용아이템(m_stTakeOnItemSet)을 다시 인벤토리에 넣는다.
				// 만약에 공통(마우스)아이템이 있다면 그걸 착용셀에 넣는다.
				if ( m_xInterface.m_xStatusWnd.m_stTakeOnItemSet.bSetted )
				{
					m_xInterface.m_xInventoryWnd.AddInvenItem(m_xInterface.m_xStatusWnd.m_stTakeOnItemSet.xItem, 0, FALSE);
					ZeroMemory(&m_xInterface.m_xStatusWnd.m_stTakeOnItemSet, sizeof(ITEMSET));
				}
				if ( m_xInterface.m_stCommonItem.bSetted )
				{
					m_xInterface.m_xStatusWnd.SetEquipFromCommon(m_xInterface.m_stCommonItem.xItem.m_shCellNum, &m_xInterface.m_stCommonItem);
	//												ZeroMemory(&m_xInterface.m_stCommonItem, sizeof(ITEMSET));
				}
				break;
			}
			case SM_TAKEOFF_OK:
			{
				FEATURE stFeature;
				memcpy(&stFeature, &tdm.nRecog, sizeof(LONG));
				g_xGameProc.m_xMyHero.ChangeFeature(stFeature, g_xGameProc.m_xMyHero.m_stFeatureEx);

				// 벗을려고 한아이템을 인벤토리에 넣는다.
				if ( m_xInterface.m_stCommonItem.bSetted )
				{
//					m_xInterface.m_xInventoryWnd.AddInvenItem(m_xInterface.m_stCommonItem.xItem, 0, FALSE);
					ZeroMemory(&m_xInterface.m_stCommonItem, sizeof(ITEMSET));
				}
				break;
			}
			case SM_TAKEOFF_FAIL:
			{
				// 공통(마우스)아이템을 다시 착용셀에 넣는다.
				if ( m_xInterface.m_stCommonItem.bSetted )
				{
					m_xInterface.m_xStatusWnd.SetEquipFromCommon(m_xInterface.m_stCommonItem.xItem.m_shCellNum, &m_xInterface.m_stCommonItem);
	//												ZeroMemory(&m_xInterface.m_stCommonItem, sizeof(ITEMSET));
				}
				break;
			}
			case SM_ITEMSHOW:
			{
				BOOL bExisted = FALSE;

				if ( m_stMapItemList.GetCounter() != 0 )
				{
					m_stMapItemList.MoveCurrentToTop();
					LPGROUNDITEM pstItem;

					for ( INT nCnt = 0; nCnt < m_stMapItemList.GetCounter(); nCnt++ )
					{
						pstItem = m_stMapItemList.GetCurrentData();

						if ( pstItem->nRecog == tdm.nRecog )
							bExisted = TRUE;

						m_stMapItemList.MoveNextNode();
					}
				}	

				if ( !bExisted )
				{
					LPGROUNDITEM pstItem = new GROUNDITEM;

					pstItem->nRecog  = tdm.nRecog;
					pstItem->shTileX = tdm.wParam;
					pstItem->shTileY = tdm.wTag;
					pstItem->wLooks  = tdm.wSeries;
					nPos = fnDecode6BitBuf((pszMsg + DEFBLOCKSIZE), (char*)pstItem->szItemName, sizeof(pstItem->szItemName));
					pstItem->szItemName[nPos] = '\0';

					m_stMapItemList.AddNode(pstItem);
				}											

				break;
			}
			case SM_ITEMHIDE:
			{
				LPGROUNDITEM pstItem;

				m_stMapItemList.MoveCurrentToTop();
				
				for ( INT nCnt = 0; nCnt < m_stMapItemList.GetCounter(); nCnt++ )
				{
					pstItem = m_stMapItemList.GetCurrentData();												
					
					if ( pstItem->nRecog == tdm.nRecog )
					{
						m_stMapItemList.DeleteCurrentNodeEx();
						SAFE_DELETE(pstItem);
						break;
					}
				
					m_stMapItemList.MoveNextNode();
				}	

				break;
			}
			case SM_SHOWEVENT:
			{
				BOOL bExisted = FALSE;

				_TSHORTMSSEAGE stShortMsg;
				
				fnDecode6BitBuf((pszMsg + DEFBLOCKSIZE), (char*)&stShortMsg, sizeof(_TSHORTMSSEAGE));

				if ( m_xGroundMagicList.GetCounter() != 0 )
				{
					m_xGroundMagicList.MoveCurrentToTop();
					CMagic* pxMagic;

					for ( INT nCnt = 0; nCnt < m_xGroundMagicList.GetCounter(); nCnt++ )
					{
						pxMagic = m_xGroundMagicList.GetCurrentData();

						switch ( pxMagic->m_bMagicNum )
						{
							case _SKILL_EARTHFIRE: 
							case _SKILL_HOLYSHIELD:
							{
								CRepeatMagic* pxRepeatMagic = (CRepeatMagic*)pxMagic;
								if ( pxRepeatMagic->m_nEventID == tdm.nRecog )
									bExisted = TRUE;

								break;
							}
						}

						m_xGroundMagicList.MoveNextNode();
					}
				}	

				// 생성한다.
				if ( !bExisted )
				{
					BYTE bEffectNum = 0;

					if ( tdm.wParam == _ET_FIRE )
					{
						bEffectNum = _SKILL_EARTHFIRE;
						CRepeatMagic* pxRepeatMagic;
						pxRepeatMagic = new CRepeatMagic;
						pxRepeatMagic->CreateMagic(tdm.nRecog, bEffectNum, 0, 0, tdm.wTag, tdm.wSeries, 0XFFFFFFFF, NULL, NULL);

						pxRepeatMagic->m_dwCurrFrame += GetRandomNum(1, 8);

						g_xGameProc.m_xGroundMagicList.AddNode(pxRepeatMagic);

					}
					else if ( tdm.wParam == _ET_HOLYCURTAIN )
					{
						bEffectNum = _SKILL_HOLYSHIELD;
						CRepeatMagic* pxRepeatMagic;
						pxRepeatMagic = new CRepeatMagic;
						pxRepeatMagic->CreateMagic(tdm.nRecog, bEffectNum, 0, 0, tdm.wTag, tdm.wSeries, 0XFFFFFFFF, NULL, NULL);

						pxRepeatMagic->m_dwCurrFrame += GetRandomNum(1, 8);

						g_xGameProc.m_xGroundMagicList.AddNode(pxRepeatMagic);
					}
				}							
				break;
			}
			case SM_HIDEEVENT:
			{
				if ( m_xGroundMagicList.GetCounter() != 0 )
				{
					m_xGroundMagicList.MoveCurrentToTop();
					CMagic* pxMagic;

					for ( INT nCnt = 0; nCnt < m_xGroundMagicList.GetCounter(); nCnt++ )
					{
						pxMagic = m_xGroundMagicList.GetCurrentData();
						CRepeatMagic* pxRepeatMagic = NULL;

						switch ( pxMagic->m_bMagicNum )
						{
							case _SKILL_EARTHFIRE: 
							case _SKILL_HOLYSHIELD:
							{
								pxRepeatMagic = (CRepeatMagic*)pxMagic;
								break;
							}
						}
						if ( pxRepeatMagic && pxRepeatMagic->m_nEventID == tdm.nRecog )
						{
							pxRepeatMagic->m_dwMagiLifeTotal = 0;
							break;
						}

						m_xGroundMagicList.MoveNextNode();
					}
				}	
				break;
			}
			case SM_LEVELUP:
			{
				m_xMyHero.m_stAbility.dwExp = tdm.nRecog;
				m_xMyHero.m_stAbility.bLevel = tdm.wParam;

				m_xInterface.m_xClientSysMsg.AddSysMsg("레벨이 올랐습니다");
				break;
			}
			case SM_DAYCHANGING:
			{
				m_bDayState = tdm.wParam;
				m_bFogState = tdm.wTag;

				SetTimer(g_xMainWnd.GetSafehWnd(), _TIMER_ID, 50, NULL);
				break;
			}
			case SM_AREASTATE:
			{
				break;
			}
			case SM_HEAR:
			case SM_SYSMESSAGE:
			case SM_GUILDMESSAGE:
			case SM_CRY:
			case SM_WHISPER:
			case SM_GROUPMESSAGE:
//			case SM_MONSTERSAY:
				OnSvrMsgHear(&tdm, (pszMsg + DEFBLOCKSIZE));
				break;
			case SM_DISAPPEAR:
			case SM_CLEAROBJECT:
			{
				_TDEFAULTMESSAGE*	lpstDefMsg;
				lpstDefMsg	= new _TDEFAULTMESSAGE;

				memcpy(lpstDefMsg, &tdm, sizeof(_TDEFAULTMESSAGE));
				
				m_xWaitDefaultPacketQueue.PushQ((BYTE*)lpstDefMsg);
				break;
			}
			case SM_MERCHANTSAY:		// gadget
			{	
				int		nPos;
				char	szDecodeMsg[8192];
				char	*pszSay;

				m_xInterface.m_xNPCWnd.ResetDialog();

				m_wNPCWndPosX = m_xMyHero.m_wPosX;
				m_wNPCWndPosY = m_xMyHero.m_wPosY;

//				if (tdm.nRecog != m_xInterface.m_xNPCWnd.m_nNpcId)
//				{
//				}
				
				nPos = fnDecode6BitBuf((pszMsg + DEFBLOCKSIZE), szDecodeMsg, sizeof(szDecodeMsg));
				szDecodeMsg[nPos] = '\0';

				if (pszSay = strchr(szDecodeMsg, '/'))
				{
					*pszSay++ = '\0';

					if (pszSay)
						DevideScript(pszSay);
				}

				if(!m_xInterface.m_xNPCWnd.GetGameWndActive())
				{
					m_xInterface.WindowActivate(_WND_ID_NPCCHAT);
					m_xInterface.m_xNPCWnd.SetNPC(tdm.nRecog);
				}
				
				break;
			}
			case SM_MERCHANTDLGCLOSE:
			{
				CloseNPCWindow();
				break;
			}
			case SM_SENDGOODSLIST:
			case SM_SENDDETAILGOODSLIST:
			case SM_BUYITEM_FAIL:
			
			{	// 상점창 처리 루틴
				LPPACKETMSG	lpPacketMsg	= new PACKETMSG;
				lpPacketMsg->stDefMsg	= tdm;
				lstrcpy(lpPacketMsg->szEncodeData, (pszMsg + DEFBLOCKSIZE));
				m_xInterface.m_xStoreWnd.m_xPacketQueue.PushQ((BYTE*)lpPacketMsg);	// Packet을 Queue에 넣는다.
				if(!m_xInterface.m_xStoreWnd.GetGameWndActive())
				{
					m_xInterface.WindowActivate(_WND_ID_STORE);
					m_xInterface.m_xStoreWnd.SetNPC(tdm.nRecog);
				}
				break;
			}
			case SM_DROPITEM_SCCESS: 
			{
				CHAR	pszItemName[15];
				if (m_xInterface.m_stCommonItem.bWaitResponse )
				{
					if ( m_xInterface.m_stCommonItem.bSetted && m_xInterface.m_stCommonItem.xItem.m_stItemInfo.nMakeIndex == tdm.nRecog )
					{
						nPos = fnDecode6BitBuf((pszMsg + DEFBLOCKSIZE), (char*)pszItemName, sizeof(pszItemName));
						pszItemName[nPos] = '\0';

						if ( !strcmp(pszItemName, m_xInterface.m_stCommonItem.xItem.m_stItemInfo.stStdItem.szName) )
						{
							ZeroMemory(&m_xInterface.m_stCommonItem, sizeof(COMMONITEMSET));
						}
					}
					m_xInterface.m_stCommonItem.bWaitResponse = FALSE;
				}
				break;
			}
			case SM_DROPITEM_FAIL:
			{
				// 가방에 다시 넣어버린다.
				CHAR	pszItemName[15];
				if (m_xInterface.m_stCommonItem.bWaitResponse )
				{
					if ( m_xInterface.m_stCommonItem.bSetted && m_xInterface.m_stCommonItem.xItem.m_stItemInfo.nMakeIndex == tdm.nRecog )
					{
						nPos = fnDecode6BitBuf((pszMsg + DEFBLOCKSIZE), (char*)pszItemName, sizeof(pszItemName));
						pszItemName[nPos] = '\0';

						if ( !strcmp(pszItemName, m_xInterface.m_stCommonItem.xItem.m_stItemInfo.stStdItem.szName) )
						{
							if ( !m_xInterface.m_stCommonItem.bIsBeltItem )
								m_xInterface.m_xInventoryWnd.AddInvenItem(m_xInterface.m_stCommonItem.xItem, m_xInterface.m_stCommonItem.xItem.m_shCellNum, TRUE);
							else
								m_xInterface.m_xInventoryWnd.AddBeltItem(m_xInterface.m_stCommonItem.xItem, m_xInterface.m_stCommonItem.xItem.m_shCellNum, TRUE);
							ZeroMemory(&m_xInterface.m_stCommonItem, sizeof(COMMONITEMSET));
						}
					}
					m_xInterface.m_stCommonItem.bWaitResponse = FALSE;
				}
			}
			case SM_DURACHANGE:
			{
				if ( m_xInterface.m_xStatusWnd.m_stEquipItem[tdm.wParam].bSetted )
				{
					m_xInterface.m_xStatusWnd.m_stEquipItem[tdm.wParam].xItem.m_stItemInfo.nDura = tdm.nRecog;
					m_xInterface.m_xStatusWnd.m_stEquipItem[tdm.wParam].xItem.m_stItemInfo.nDuraMax = MAKELONG(tdm.wTag, tdm.wSeries);
				}
				break;
			}
			case SM_GOLDCHANGE:
			{
				m_xMyHero.m_nGlod = tdm.nRecog;
				break;
			}
			case SM_WEIGHTCHANGED:
			{
				m_xMyHero.m_stAbility.wWeight	  = tdm.nRecog;
				m_xMyHero.m_stAbility.bWearWeight = tdm.wParam;
				m_xMyHero.m_stAbility.bHandWeight = tdm.wTag;											
				break;
			}
			case SM_RIDEHORSE:
			{
				m_xMyHero.m_stFeatureEx.bHorse = tdm.nRecog;
				m_xMyHero.ChangeFeature(m_xMyHero.m_stFeature, m_xMyHero.m_stFeatureEx);

				if ( m_xMyHero.m_stFeatureEx.bHorse != _HORSE_NONE )
					m_xMyHero.SetMotionFrame(_MT_HORSESTAND, m_xMyHero.m_bCurrDir);
				else
					m_xMyHero.SetMotionFrame(_MT_STAND, m_xMyHero.m_bCurrDir);

				break;
			}
			case SM_OPENDOOR_OK:
			{
				INT nIdx = m_xMap.GetDoor(tdm.wParam, tdm.wTag);
				if ( nIdx )
				{
					m_xMap.OpenDoor(tdm.wParam, tdm.wTag, nIdx);
				}
				break;
			}
			case SM_OPENDOOR_LOCK:
			{
				m_xInterface.m_xClientSysMsg.AddSysMsg("문이 잠겨져 있습니다.");
				break;
			}
			case SM_CLOSEDOOR:
			{
				INT nIdx = m_xMap.GetDoor(tdm.wParam, tdm.wTag);
				if ( nIdx )
				{
					m_xMap.CloseDoor(tdm.wParam, tdm.wTag, nIdx);
				}
				break;
			}
			case SM_HEALTHSPELLCHANGED: // gadget
			{
				if ( tdm.nRecog == m_xMyHero.m_dwIdentity )
				{
					m_xMyHero.m_wHP			= tdm.wParam;
					m_xMyHero.m_wMP			= tdm.wTag;
					m_xMyHero.m_wMAXHP		= tdm.wSeries;
					FLOAT wHPRate			= (FLOAT)((FLOAT)m_xMyHero.m_wHP/(FLOAT)m_xMyHero.m_wMAXHP);
					m_xMyHero.m_bHPPercent  = wHPRate * 100;
#ifdef _DEBUG
					_RPT2(_CRT_WARN, "HP:%d, MAXHP:%d\n", tdm.wParam, tdm.wSeries);
#endif
				}
				else
				{
					CActor* pxActor = FindActor(tdm.nRecog);

					if (pxActor)
					{
						pxActor->m_wHP			= tdm.wParam;
						pxActor->m_wMP			= tdm.wTag;
						pxActor->m_wMAXHP		= tdm.wSeries;
						FLOAT wHPRate			= (FLOAT)((FLOAT)pxActor->m_wHP/(FLOAT)pxActor->m_wMAXHP);
						pxActor->m_bHPPercent	= wHPRate * 100;
					}
				}

				break;
			}
			case SM_CREATEGROUP_OK:
			{
//            changegroupmodetime := GetTickCount;
//            AllowGroup := TRUE;
				break;
			}
			case SM_CREATEGROUP_FAIL:
			{
//            changegroupmodetime := GetTickCount;
				switch (tdm.nRecog)
				{
					case -1: 
						//FrmDlg.DMessageDlg ('이미 그룹에 가입되어 있습니다.', [mbOk]);
						break;
					case -2: 
						//FrmDlg.DMessageDlg ('그룹에 참여할 이름이 바르지 않습니다.', [mbOk]);
						break;
					case -3: 
						//FrmDlg.DMessageDlg ('함께 참여하려는 사람이 다른 그룹에 참여중입니다.', [mbOk]);
						break;
					case -4: 
						//FrmDlg.DMessageDlg ('상대방이 그룹거부중입니다.', [mbOk]);
						break;
				}
			}
			case SM_GROUPMEMBERS:
			{
				char	szDecodeMsg[8192];
				char	*pszMembers, *pszNext;

				m_xInterface.m_xGroupWnd.m_xGroupList.ClearAllNodes();

				nPos = fnDecode6BitBuf((pszMsg + DEFBLOCKSIZE), szDecodeMsg, sizeof(szDecodeMsg));
				szDecodeMsg[nPos] = '\0';

				pszMembers = szDecodeMsg;

				while (pszNext = strchr(pszMembers, '/'))
				{
					*pszNext++ = '\0';

					CGroupMember* pGroupMember = new CGroupMember;

					if (pGroupMember)
					{
						strcpy(pGroupMember->szMemberName, pszMembers);
						m_xInterface.m_xGroupWnd.m_xGroupList.AddNode(pGroupMember);
					}

					pszMembers = pszNext;
				}

				break;
			}
			case SM_GROUPCANCEL:
			{
				m_xInterface.m_xGroupWnd.m_xGroupList.ClearAllNodes();

				break;
			}
			case SM_BUILDGUILD_OK:
			{
//         begin
//            FrmDlg.LastestClickTime := GetTickCount;
//          FrmDlg.DMessageDlg ('문파가 만들어 졌습니다.', [mbOk]);
//         end;
				break;
			}
			case SM_BUILDGUILD_FAIL:
			{
//         begin
//            FrmDlg.LastestClickTime := GetTickCount;
//            case msg.Recog of
//               -1: FrmDlg.DMessageDlg ('이미 문파에 가입해 있습니다.', [mbOk]);
//               -2: FrmDlg.DMessageDlg ('등록비용이 부족합니다.', [mbOk]);
//               -3: FrmDlg.DMessageDlg ('필요아이템을 모두 가지고 있지 않습니다.', [mbOk]);
//            end;
//         end;
				break;
			}
			case SM_OPENGUILDDLG:
			{
				char szDecodeMsg[8192];

				INT nPos = fnDecode6BitBuf((pszMsg + DEFBLOCKSIZE), szDecodeMsg, sizeof(szDecodeMsg));
				szDecodeMsg[nPos] = '\0';

				m_xInterface.WindowActivate(_WND_ID_GUILDMASTER);	

				break;
			}
			/*case SM_DOOROPEN:
			{
				BOOL bIsOpen = stDefMsg.nRecog;
				BYTE bDoorIdx = stDefMsg.wParam;

				m_xMap.SetDoorState(bDoorIdx, bIsOpen);
				break;
			}*/
			case SM_USERNAME:
			{
				char szDecodeMsg[1024];
				char *pszGuildName;

				INT nPos = fnDecode6BitBuf((pszMsg + DEFBLOCKSIZE), szDecodeMsg, sizeof(szDecodeMsg));
				szDecodeMsg[nPos] = '\0';

				if (pszGuildName = strchr(szDecodeMsg, '\\'))
					*pszGuildName++ = '\0';

				if ( tdm.nRecog == m_xMyHero.m_dwIdentity )
				{				
					m_xMyHero.m_dwNameClr = GetUserNameColor(tdm.wParam);

					strcpy(m_xMyHero.m_szName, szDecodeMsg);

					if (pszGuildName)
						strcpy(m_xMyHero.m_szGuildName, pszGuildName);
				}
				else
				{
					CActor* pxActor = FindActor(tdm.nRecog);

					if (pxActor)
					{
						pxActor->m_dwNameClr = GetUserNameColor(tdm.wParam);

						strcpy(pxActor->m_szName, szDecodeMsg);

						if (pszGuildName)
							strcpy(pxActor->m_szGuildName, pszGuildName);
					}
				}

				break;
			}
			case SM_CHANGEGUILDNAME:
			{
				char szDecodeMsg[1024];
				char *pszGuildPos;

				INT nPos = fnDecode6BitBuf((pszMsg + DEFBLOCKSIZE), szDecodeMsg, sizeof(szDecodeMsg));
				szDecodeMsg[nPos] = '\0';

				if (pszGuildPos = strchr(szDecodeMsg, '/'))
				{
					*pszGuildPos = ' ';

					strcpy(m_xMyHero.m_szGuildName, szDecodeMsg);
				}

				break;
			}
			default:
			{
				LPPACKETMSG	lpPacketMsg	= new PACKETMSG;

				lpPacketMsg->stDefMsg			= tdm;
				lpPacketMsg->szEncodeData[0]	= NULL;

				if ( tdm.wIdent != SM_HIT && tdm.wIdent != SM_BACKSTEP && tdm.wIdent != SM_RUSH )
 					lstrcpy(lpPacketMsg->szEncodeData, (pszMsg + DEFBLOCKSIZE));

 				m_xWaitPacketQueue.PushQ((BYTE*)lpPacketMsg);

				break;
			}
		}
	}
}

VOID CGameProcess::ProcessDefaultPacket()
{
	_TDEFAULTMESSAGE*	lpstDefMsg;

	for (int i = 0; i < m_xWaitDefaultPacketQueue.GetCount(); i++)
	{
		lpstDefMsg = (_TDEFAULTMESSAGE*)m_xWaitDefaultPacketQueue.PopQ();

		if ( lpstDefMsg )
		{
			if ( lpstDefMsg->wIdent == SM_DISAPPEAR )
			{
				CActor* pxActor = NULL;

				m_xActorList.MoveCurrentToTop();
				for ( INT nCnt = 0; nCnt < m_xActorList.GetCounter(); nCnt++ )
				{
					pxActor = m_xActorList.GetCurrentData();
					
					if ( pxActor->m_dwIdentity == lpstDefMsg->nRecog )
					{
						INT nPacketCnt = pxActor->m_xPacketQueue.GetCount();
						for ( INT nLoopCnt = 0; nLoopCnt < nPacketCnt; nLoopCnt++ )
						{
							LPPACKETMSG	lpObjPacketMsg = (LPPACKETMSG)pxActor->m_xPacketQueue.PopQ();
							SAFE_DELETE(lpObjPacketMsg);
						}

						switch ( pxActor->m_stFeature.bGender )
						{
						case _GENDER_MAN:
						case _GENDER_WOMAN:
							{
								m_xActorList.DeleteCurrentNodeEx();
								CHero* pxHero = (CHero*)pxActor;
								delete((CHero*)pxHero);
								pxHero = NULL;
							}
							break;
						case _GENDER_NPC:
							{
								m_xActorList.DeleteCurrentNodeEx();
								CNPC* pxNPC = (CNPC*)pxActor;
								delete((CNPC*)pxNPC);
								pxNPC = NULL;
							}
							break;
						case _GENDER_MON:
							{
								m_xActorList.DeleteCurrentNodeEx();
								delete(pxActor);
								pxActor = NULL;
							}
							break;
						}						
						break;		//for문을 빠져나간다.
					}					
					m_xActorList.MoveNextNode();

				}
			}
			else if ( lpstDefMsg->wIdent == SM_CLEAROBJECT )
			{
				CActor* pxActor = NULL;

				m_xActorList.MoveCurrentToTop();
				for ( INT nCnt = 0; nCnt < m_xActorList.GetCounter(); )
				{
					pxActor = m_xActorList.GetCurrentData();
					
					INT nPacketCnt = pxActor->m_xPacketQueue.GetCount();
					for ( INT nLoopCnt = 0; nLoopCnt < nPacketCnt; nLoopCnt++ )
					{
						LPPACKETMSG	lpObjPacketMsg = (LPPACKETMSG)pxActor->m_xPacketQueue.PopQ();
						SAFE_DELETE(lpObjPacketMsg);
					}

					switch ( pxActor->m_stFeature.bGender )
					{
					case _GENDER_MAN:
					case _GENDER_WOMAN:
						{
							m_xActorList.DeleteCurrentNodeEx();
							CHero* pxHero = (CHero*)pxActor;
							delete((CHero*)pxHero);
							pxHero = NULL;
						}
						break;
					case _GENDER_NPC:
						{
							m_xActorList.DeleteCurrentNodeEx();
							CNPC* pxNPC = (CNPC*)pxActor;
							delete((CNPC*)pxNPC);
							pxNPC = NULL;
						}
						break;
					case _GENDER_MON:
						{
							m_xActorList.DeleteCurrentNodeEx();
							delete(pxActor);
							pxActor = NULL;
						}
						break;
					}
//					m_xActorList.MoveNextNode();
				}					
			}
		}
		SAFE_DELETE(lpstDefMsg);
	}						
}


VOID CGameProcess::ProcessPacket()
{
	LPPACKETMSG	lpPacketMsg;

	for (int i = 0; i < m_xWaitPacketQueue.GetCount(); i++)
	{
		BOOL bExist = FALSE;
		CActor* pxActor = NULL;

		lpPacketMsg = (LPPACKETMSG)m_xWaitPacketQueue.PopQ();

		if ( lpPacketMsg )
		{
			if ( lpPacketMsg->stDefMsg.nRecog == m_xMyHero.m_dwIdentity )
			{
				if ( lpPacketMsg->stDefMsg.wIdent == SM_NOWDEATH || lpPacketMsg->stDefMsg.wIdent == SM_DEATH )
				{
					m_xMyHero.m_xPriorPacketQueue.PushQ((BYTE*)lpPacketMsg);
				}
				else if ( lpPacketMsg->stDefMsg.wIdent == SM_CHANGEMAP )
				{
					DWORD dwFogColor;

					m_xMyHero.SetMotionFrame(_MT_STAND, m_xMyHero.m_bCurrDir);
					m_xMyHero.m_wPosX = lpPacketMsg->stDefMsg.wParam;	// + GAPX_TILE_CHAR_MAP;
					m_xMyHero.m_wPosY = lpPacketMsg->stDefMsg.wTag;		// + GAPY_TILE_CHAR_MAP;
					m_xMap.SetStartViewTile(m_xMyHero.m_wPosX - _GAPX_TILE_CHAR_MAP, m_xMyHero.m_wPosY - _GAPY_TILE_CHAR_MAP);

					char szDecodeMsg[MAX_PATH];
					int nPos = fnDecode6BitBuf(lpPacketMsg->szEncodeData, szDecodeMsg, sizeof(szDecodeMsg));
					szDecodeMsg[nPos] = '\0';

					LoadMapChanged(szDecodeMsg);

					m_bFogState = LOBYTE(lpPacketMsg->stDefMsg.wSeries);

					switch ( m_bFogState )
					{
					case _FOGSTATE_DAY:
						{
							dwFogColor = RGB(240, 240, 240);
							break;
						}
					case _FOGSTATE_NIGHT:
						{
							dwFogColor = RGB(10, 10, 10);
							break;
						}
					case _FOGSTATE_DAWN:
						{
							dwFogColor = RGB(50, 50, 50);
							break;
						}
					}

					m_xLightFog.ChangeLightColor(dwFogColor);

					// gadget			
					m_wInputDelayTime = 2300;
					m_wCurrInputDelayTime = 0;
					m_xMyHero.m_wABlendCurrDelay = 0;
					m_xMyHero.m_wABlendDelay = 2300;

					CloseNPCWindow();
				}
				else
				{
					if ( lpPacketMsg->stDefMsg.wIdent == SM_STRUCK )
					{
//						m_xMyHero.StruckMsgReassign();

						if ( timeGetTime() - m_xMyHero.m_dwLastStruckTime < 500 )
						{
							SAFE_DELETE(lpPacketMsg);
							continue;
						}

						MESSAGEBODYWL stMsgBodyWl;
						fnDecode6BitBuf(lpPacketMsg->szEncodeData, (char*)&stMsgBodyWl, sizeof(MESSAGEBODYWL));

						for ( INT nCnt = 0; nCnt < m_xActorList.GetCounter(); nCnt++ )
						{
							CActor* pxGaheja = m_xActorList.GetCurrentData();

							if ( pxGaheja->m_dwIdentity == stMsgBodyWl.nTag1 )
							{
								m_xMyHero.m_stHitter = pxGaheja->m_stFeature;
								break;
							}

							m_xActorList.MoveNextNode();
						}						
					}					
					
					m_xMyHero.StruckMsgReassign();
					m_xMyHero.m_xPacketQueue.PushQ((BYTE*)lpPacketMsg);
				}
			}
			else
			{
				FEATURE		stFeature;
//				FEATUREEX	stFeatureEx;

 				fnDecode6BitBuf(lpPacketMsg->szEncodeData, (char*)&stFeature, sizeof(FEATURE));

				m_xActorList.MoveCurrentToTop();
				
				for ( INT nCnt = 0; nCnt < m_xActorList.GetCounter(); nCnt++ )
				{
					pxActor = m_xActorList.GetCurrentData();
			
					if ( pxActor->m_dwIdentity == lpPacketMsg->stDefMsg.nRecog )
					{
						bExist = TRUE;
						break;
					}
				
					m_xActorList.MoveNextNode();
				}
				if ( !bExist )
				{
					if ( stFeature.bGender == _GENDER_MAN || stFeature.bGender == _GENDER_WOMAN ) 
					{
						pxActor = new CHero;
						CHero* pxHero = (CHero*)pxActor;

						pxHero->m_dwIdentity = lpPacketMsg->stDefMsg.nRecog;

						if ( pxHero->Create(&m_xImage, _MT_STAND, _DIRECTION_LIST_1, lpPacketMsg->stDefMsg.wParam, lpPacketMsg->stDefMsg.wTag, &stFeature, NULL) )
						{
							m_xActorList.AddNode(pxActor);
							pxHero->m_wABlendCurrDelay = 0;
							pxHero->m_wABlendDelay  = 1500;
						}
					}
					else if ( stFeature.bGender == _GENDER_NPC )
					{
						pxActor = new CNPC;
						CNPC* pxNPC = (CNPC*)pxActor;

						pxNPC->m_dwIdentity = lpPacketMsg->stDefMsg.nRecog;

						if ( pxNPC->Create(&m_xImage, &stFeature, _MT_STAND, _DIRECTION_LIST_1, lpPacketMsg->stDefMsg.wParam, lpPacketMsg->stDefMsg.wTag) )
							m_xActorList.AddNode(pxActor);

						pxNPC->m_wABlendCurrDelay = 0;
						pxNPC->m_wABlendDelay	  = 0;
					}
					else
//					else if ( stFeature.bGender == _GENDER_MON )
					{
						stFeature.bGender	= _GENDER_MON;

						// 적용시켜야함.
//						if ( stFeature.bWeapon == 0 ) // 기본.
//						if ( stFeature.bWeapon == 1 ) // SKELLEON.

						pxActor = new CActor;
						pxActor->m_dwIdentity = lpPacketMsg->stDefMsg.nRecog;

						if ( pxActor->Create(&m_xImage, &stFeature, _MT_STAND, _DIRECTION_LIST_1, lpPacketMsg->stDefMsg.wParam, lpPacketMsg->stDefMsg.wTag) )
						{
							m_xActorList.AddNode(pxActor);
							pxActor->m_wABlendCurrDelay = 0;
							pxActor->m_wABlendDelay		= 150;
						}
					}
				}

				if ( pxActor )				
				{
					if ( lpPacketMsg->stDefMsg.wIdent == SM_STRUCK )
					{
//						pxActor->StruckMsgReassign();

						MESSAGEBODYWL stMsgBodyWl;
						fnDecode6BitBuf(lpPacketMsg->szEncodeData, (char*)&stMsgBodyWl, sizeof(MESSAGEBODYWL));

						for ( INT nCnt = 0; nCnt < m_xActorList.GetCounter(); nCnt++ )
						{
							CActor* pxGaheja = m_xActorList.GetCurrentData();

							if ( pxGaheja->m_dwIdentity == stMsgBodyWl.nTag1 )
							{
								pxActor->m_stHitter = pxGaheja->m_stFeature;
								break;
							}

							m_xActorList.MoveNextNode();
						}
						
					}

					pxActor->StruckMsgReassign();
					pxActor->m_xPacketQueue.PushQ((BYTE*)lpPacketMsg);
				}
				else							
					SAFE_DELETE(lpPacketMsg);
			}
		}
	}
}

// **************************************************************************************
//
//		 Packet Handling
//
// **************************************************************************************

VOID CGameProcess::OnSvrMsgNewMap(_TDEFAULTMESSAGE	*ptdm, char *pszMapName)
{
	DWORD dwFogColor;
	char szMapName[32];

	m_xMyHero.m_wPosX = ptdm->wParam;
	m_xMyHero.m_wPosY = ptdm->wTag;

	m_bDayState = LOBYTE(ptdm->wSeries);

	int nPos = fnDecode6BitBuf(pszMapName, (char*)szMapName, sizeof(szMapName));
	szMapName[nPos] = '\0';

	m_xMap.SetStartViewTile(m_xMyHero.m_wPosX - _GAPY_TILE_CHAR_MAP, m_xMyHero.m_wPosY - _GAPY_TILE_CHAR_MAP);
	m_xMap.LoadMapData(szMapName);

	switch ( m_bFogState )
	{
	case _FOGSTATE_DAY:
		{
			m_fDarkRate = 255.0f;
			dwFogColor = RGB(m_fDarkRate, m_fDarkRate, m_fDarkRate);
			break;
		}
	case _FOGSTATE_NIGHT:
		{
			m_fDarkRate = 10.0f;
			dwFogColor = RGB(m_fDarkRate, m_fDarkRate, m_fDarkRate);
			break;
		}
	case _FOGSTATE_DAWN:
		{
			m_fDarkRate = 50.0f;
			dwFogColor = RGB(m_fDarkRate, m_fDarkRate, m_fDarkRate);
			break;
		}
	}

	m_xLightFog.ChangeLightColor(dwFogColor);

/*	g_xSound.PlayBkMusicMp3(TRUE, 0);
*/
	m_bRender = TRUE;
}

VOID CGameProcess::OnSvrMsgLogon(_TDEFAULTMESSAGE	*ptdm, char *pszMsg)
{
	MESSAGEBODYWL wl;

	m_xMyHero.m_dwIdentity	= ptdm->nRecog;
	m_xMyHero.m_bLightSize	= HIBYTE(ptdm->wSeries);

	BYTE bDir = LOBYTE(ptdm->wSeries);

	if ( bDir > 8 )
		bDir = 0;

	// 추가코드
	fnDecode6BitBuf(pszMsg, (char*)&wl, sizeof(wl));

//	if ( stFeatureEx.bHorse == _HORSE_NONE )
		m_xMyHero.Create(&m_xImage, _MT_STAND, bDir, ptdm->wParam/*x*/, ptdm->wTag/*y*/, (FEATURE *)&wl.lParam1, NULL);
//	else
//		m_xMyHero.Create(&m_xImage, _MT_HORSESTAND, bDir, m_xMyHero.m_wPosX, m_xMyHero.m_wPosY, stFeature, stFeatureEx);

	strcpy(g_xGameProc.m_xMyHero.m_szName, g_szCharName);
	
	m_xMyHero.SetMapHandler(&m_xMap);

	CMagic* pxMagic;
	pxMagic = new CMagic;
	pxMagic->CreateMagic(_SKILL_SPACEMOVE2, m_xMyHero.m_wPosX, m_xMyHero.m_wPosY, m_xMyHero.m_wPosX, m_xMyHero.m_wPosY, NULL, m_xMyHero.m_dwIdentity);
	m_xMagicList.AddNode(pxMagic);	
	
	m_wInputDelayTime				= 2300;
	m_wCurrInputDelayTime			= 0;
	m_xMyHero.m_wABlendCurrDelay	= 0;
	m_xMyHero.m_wABlendDelay		= 2300;

	g_xClientSocket.SendQueryMsg(CM_QUERYBAGITEMS);
}

VOID CGameProcess::OnSvrMsgHear(_TDEFAULTMESSAGE *ptdm, char *pszMsg)
{
//	DWORD dwFontBackClr = m_xInterface.m_dwFontClrDfnList[ptdm->wParam];
//	DWORD dwFontClr		= m_xInterface.m_dwFontClrDfnList[ptdm->wTag];

	char szDecodeMsg[MAX_PATH];

	int nPos = fnDecode6BitBuf(pszMsg, szDecodeMsg, sizeof(szDecodeMsg));
	szDecodeMsg[nPos] = '\0';

	DWORD dwBack = GetChatColor(ptdm->wParam);
	DWORD dwFont = GetChatColor(ptdm->wTag);
	m_xInterface.MsgAdd(dwFont, dwBack, szDecodeMsg);

	if ( ptdm->wIdent == SM_HEAR || ptdm->wIdent == SM_MONSTERSAY )
	{
		if ( ptdm->nRecog == m_xMyHero.m_dwIdentity )
		{
			strcpy(m_xMyHero.m_szChatMsg, szDecodeMsg);
			
			m_xMyHero.ChatMsgAdd();
			m_xMyHero.m_wCurrChatDelay = 0;
		}
		else
		{
			m_xActorList.MoveCurrentToTop();
			
			CActor* pxActor;
			
			if ( m_xActorList.GetCounter() != 0 )
			{
				for ( INT nCnt = 0; nCnt <= m_xActorList.GetCounter(); nCnt++ )
				{ 
					pxActor = m_xActorList.GetCurrentData();

					if ( pxActor->m_dwIdentity == ptdm->nRecog )
					{
						strcpy(pxActor->m_szChatMsg, szDecodeMsg);

						pxActor->ChatMsgAdd();
						pxActor->m_wCurrChatDelay = 0;
					}

					m_xActorList.MoveNextNode();							
				}
			}
		}
	}
}
