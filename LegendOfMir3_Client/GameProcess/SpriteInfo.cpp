/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/



#include "StdAfx.h"


CSpriteInfo::CSpriteInfo()
{
	Init();
}

CSpriteInfo::~CSpriteInfo()
{
}

CSpriteInfo::Init()
{
	ZeroMemory(m_stHuman, sizeof(HUMANRACEINFO)*_MAX_HUMAN_RACE);
	ZeroMemory(m_stMon, sizeof(MONRACEINFO)*_MAX_HUMAN_RACE);
}

VOID CSpriteInfo::SetSpriteInfo()
{
	SetHumanSpriteInfo();
	SetMonSpriteInfo();
}

VOID CSpriteInfo::SetHumanSpriteInfo()
{
	INT nOffset = 0;
	for ( INT nCnt = 0; nCnt < _MAX_HUMAN_RACE; nCnt++ )
	{
		nOffset = _MAX_HUMAN_RACE_FRAME*nCnt;
		SetHumanRaceInfo(&m_stHuman[nCnt], _IMAGE_CHARACTER, nOffset, nOffset+_MAX_HUMAN_RACE_FRAME);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[0] , nOffset     ,  4, 6, 200, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[1] , nOffset+  80,  6, 4, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[2] , nOffset+ 160,  5, 5, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[3] , nOffset+ 240,  5, 5, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[4] , nOffset+ 320,  1, 9, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[5] , nOffset+ 400,  1, 9, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[6] , nOffset+ 480,  1, 9, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[7] , nOffset+ 560,  3, 7, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[8] , nOffset+ 640,  2, 8, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[9] , nOffset+ 720,  6, 4, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[10], nOffset+ 800,  6, 4, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[11], nOffset+ 880,  6, 4, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[12], nOffset+ 960,  6, 4, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[13], nOffset+1040,  6, 4, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[14], nOffset+1120,  6, 4, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[15], nOffset+1200,  3, 7, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[16], nOffset+1280, 10, 0, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[17], nOffset+1360, 10, 0, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[18], nOffset+1440, 10, 0, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[19], nOffset+1520, 10, 0, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[20], nOffset+1600, 10, 0, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[21], nOffset+1680,  6, 4,  90, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[22], nOffset+1760,  6, 4, 120, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[23], nOffset+1840,  6, 4, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[24], nOffset+1920, 10, 0, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[25], nOffset+2000,  4, 6, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[26], nOffset+2080,  3, 7, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[27], nOffset+2160,  8, 2, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[28], nOffset+2240,  8, 2, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[29], nOffset+2320,  4, 6, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[30], nOffset+2400,  6, 4, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[31], nOffset+2480,  6, 4, 130, FALSE);
		SetMotionInfo(&m_stHuman[nCnt].stMtn[32], nOffset+2560,  3, 7, 130, FALSE);
	}
}

      
VOID CSpriteInfo::SetMonSpriteInfo()
{
	// 사막 도마뱀 프레임정의.
	SetMonRaceInfo(&m_stMon[_RACE_LIZARD], _IMAGE_MONSTER, 0, 400);
	SetMotionInfo(&m_stMon[_RACE_LIZARD].stMtn[_MT_MON_STAND],    0,  4, 6, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_LIZARD].stMtn[_MT_MON_WALK],    80,  6, 4, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_LIZARD].stMtn[_MT_MON_ATTACK], 160,  6, 4, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_LIZARD].stMtn[_MT_MON_STRUCK], 240,  2, 8, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_LIZARD].stMtn[_MT_MON_DIE],    320, 10, 0, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_LIZARD].stMtn[_MT_MON_DEATH],  319,  1, 9, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_LIZARD].stMtn[_MT_MON_APPEAR],   0,  0, 0, 0, FALSE);
	SetMotionInfo(&m_stMon[_RACE_LIZARD].stMtn[_MT_MON_DISAPPEAR],0,  0, 0, 0, FALSE);
	SetMotionInfo(&m_stMon[_RACE_LIZARD].stMtn[_MT_MON_CRITICAL], 0,  0, 0, 0, FALSE);

	// 야행귀 프레임정의.
	SetMonRaceInfo(&m_stMon[_RACE_NIGHTGHOST], _IMAGE_MONSTER, 400, 800);
	SetMotionInfo(&m_stMon[_RACE_NIGHTGHOST].stMtn[_MT_MON_STAND],	 400+  0,  4, 6, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_NIGHTGHOST].stMtn[_MT_MON_WALK],	 400+ 80,  6, 4, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_NIGHTGHOST].stMtn[_MT_MON_ATTACK],  400+160,  6, 4, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_NIGHTGHOST].stMtn[_MT_MON_STRUCK],  400+240,  2, 8, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_NIGHTGHOST].stMtn[_MT_MON_DIE],	 400+320, 10, 0, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_NIGHTGHOST].stMtn[_MT_MON_DEATH],	 400+319,  1, 9, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_NIGHTGHOST].stMtn[_MT_MON_APPEAR],	  	   0,  0, 0, 0, FALSE);
	SetMotionInfo(&m_stMon[_RACE_NIGHTGHOST].stMtn[_MT_MON_DISAPPEAR],	   0,  0, 0, 0, FALSE);
	SetMotionInfo(&m_stMon[_RACE_NIGHTGHOST].stMtn[_MT_MON_CRITICAL],	   0,  0, 0, 0, FALSE);

	// 촉수괴물 프레임정의.
	SetMonRaceInfo(&m_stMon[_RACE_THORN], _IMAGE_MONSTER, 800, 1200);
	SetMotionInfo(&m_stMon[_RACE_THORN].stMtn[_MT_MON_STAND],	 800+  0,  4, 6, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_THORN].stMtn[_MT_MON_WALK],	 800+ 80,  6, 4, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_THORN].stMtn[_MT_MON_ATTACK],   800+160,  6, 4, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_THORN].stMtn[_MT_MON_STRUCK],   800+240,  2, 8, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_THORN].stMtn[_MT_MON_DIE],		 800+320, 10, 0, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_THORN].stMtn[_MT_MON_DEATH],	 800+319,  1, 9, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_THORN].stMtn[_MT_MON_APPEAR],	  	   0,  0, 0, 0, FALSE);
	SetMotionInfo(&m_stMon[_RACE_THORN].stMtn[_MT_MON_DISAPPEAR],	   0,  0, 0, 0, FALSE);
	SetMotionInfo(&m_stMon[_RACE_THORN].stMtn[_MT_MON_CRITICAL],	   0,  0, 0, 0, FALSE);

	// 모래괴물 프레임정의.
	SetMonRaceInfo(&m_stMon[_RACE_SANDMON], _IMAGE_MONSTER, 1200, 1680);
	SetMotionInfo(&m_stMon[_RACE_SANDMON].stMtn[_MT_MON_STAND],	   1200+  0,  4, 6, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_SANDMON].stMtn[_MT_MON_WALK],	   1200+ 80,  6, 4, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_SANDMON].stMtn[_MT_MON_ATTACK],   1200+160,  6, 4, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_SANDMON].stMtn[_MT_MON_STRUCK],   1200+240,  2, 8, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_SANDMON].stMtn[_MT_MON_DIE],	   1200+320, 10, 0, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_SANDMON].stMtn[_MT_MON_DEATH],    1200+319,  1, 9, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_SANDMON].stMtn[_MT_MON_APPEAR],   1200+400, 10, 0, 150, TRUE);
	SetMotionInfo(&m_stMon[_RACE_SANDMON].stMtn[_MT_MON_DISAPPEAR],1200+400, 10, 0, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_SANDMON].stMtn[_MT_MON_CRITICAL],       0,  0, 0, 0, FALSE);

	// 골렘 프레임정의.
	SetMonRaceInfo(&m_stMon[_RACE_GOLEM], _IMAGE_MONSTER, 1680, 2160);
	SetMotionInfo(&m_stMon[_RACE_GOLEM].stMtn[_MT_MON_STAND],	 1680+  0,  4, 6, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_GOLEM].stMtn[_MT_MON_WALK],	 1680+ 80,  6, 4, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_GOLEM].stMtn[_MT_MON_ATTACK],   1680+160,  6, 4, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_GOLEM].stMtn[_MT_MON_STRUCK],   1680+240,  2, 8, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_GOLEM].stMtn[_MT_MON_DIE],	     1680+320, 10, 0, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_GOLEM].stMtn[_MT_MON_DEATH],    1680+319,  1, 9, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_GOLEM].stMtn[_MT_MON_APPEAR],   1680+400, 10, 0, 150, TRUE);
	SetMotionInfo(&m_stMon[_RACE_GOLEM].stMtn[_MT_MON_DISAPPEAR],1680+400, 10, 0, 150, FALSE);
	SetMotionInfo(&m_stMon[_RACE_GOLEM].stMtn[_MT_MON_CRITICAL],        0,  0, 0, 0, FALSE);
}

VOID CSpriteInfo::SetHumanRaceInfo(LPHUMANRACEINFO pstHumanRaceInfo, SHORT shImgIdx, SHORT shRaceStartFrm, SHORT shRaceEndFrm)
{
	pstHumanRaceInfo->shImgIdx			= shImgIdx;
	pstHumanRaceInfo->shRaceStartFrm	= shRaceStartFrm;
	pstHumanRaceInfo->shRaceEndFrm		= shRaceEndFrm;
}

VOID CSpriteInfo::SetMonRaceInfo(LPMONRACEINFO pstMonRaceInfo, SHORT shImgIdx, SHORT shRaceStartFrm, SHORT shRaceEndFrm)
{
	pstMonRaceInfo->shImgIdx			= shImgIdx;
	pstMonRaceInfo->shRaceStartFrm		= shRaceStartFrm;
	pstMonRaceInfo->shRaceEndFrm		= shRaceEndFrm;
}

VOID CSpriteInfo::SetMotionInfo(LPMTNFRMINFO pstMtnFrmInfo, SHORT shStartFrm, BYTE bFrmCnt, BYTE bSkip, SHORT shDelay, BOOL bReverse)
{
	pstMtnFrmInfo->shStartFrm	= shStartFrm; 
	pstMtnFrmInfo->bFrmCnt		= bFrmCnt;
	pstMtnFrmInfo->bSkip		= bSkip;
	pstMtnFrmInfo->shDelay		= shDelay;
	pstMtnFrmInfo->bReverse		= bReverse;
}
