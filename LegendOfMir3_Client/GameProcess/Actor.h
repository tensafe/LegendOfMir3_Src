#ifndef _CACTOR_H
#define _CACTOR_H



/******************************************************************************************************************

	CActor Class

*******************************************************************************************************************/
#pragma pack(1)
typedef struct tagMESSAGEBODYWL
{
	LONG	lParam1;
	LONG	lParam2;
	LONG	nTag1;			// 때린놈 ID.(SM_STRUCK일때).
	LONG	nTag2;
}MESSAGEBODYWL, *LPMESSAGEBODYWL;
typedef struct tagMESSAGEBODYW
{
	WORD	wParam1;
	WORD	wParam2;
	WORD	wTag1;			// 때린놈 ID.(SM_STRUCK일때).
	WORD	wTag2;
}MESSAGEBODYW, *LPMESSAGEBODYW;

typedef struct tagCHARDESC
{
	LONG	nFeature;
	LONG	nStatus;
}CHARDESC, *LPCHARDESC;

typedef struct tagFEATURE
{
	BYTE	bGender; 
	BYTE	bWeapon;
	BYTE	bDress; 
	BYTE	bHair;
}FEATURE, *LPFEATURE;
#pragma pack(8)



class CActor
{
private:
public:
	DWORD				m_dwIdentity;					// ID Index From Server.
	CHAR				m_szName[64];					// 캐릭터나 몬스터의 이름.
	CHAR				m_szGuildName[64];

	FEATURE				m_stFeature;					// 캐릭터의 외형을 결정짓는다.
	BYTE				m_bActorImgIdx;					// 현재 Actor가 가질 이미지구조체의 번호.
	BYTE				m_bEffectImgIdx;				// 현재 Actor가 가질 이미지구조체의 번호.
	CWHWilImageData*	m_pxActorImage;					// 이미지를 얻어올 포인터들.
	RECT				m_rcActor;						// 현재 스크린상의Actor의 사각 영역.
	RECT				m_rcTargetRgn;					// 현재 Actor의 타겟영역.

	DWORD				m_dwFstFrame;					// 현재 동작의 시작 프레임.
	DWORD				m_dwEndFrame;					// 현재 동작의 마지막프레임.
	WORD				m_wDelay;						// 현재 동작에 대한 지연시간.

	BYTE				m_bCurrMtn;						// 현재 동작.
	BYTE				m_bCurrDir;						// 현재 방향.
	BYTE				m_bMoveDir;						// 현재 방향.
	DWORD				m_dwCurrFrame;					// 현재 프레임.
	WORD				m_wCurrDelay;					// 현재 지연시간.

	WORD				m_wOldPosX;						// 현재 행동이 있기전의 타일X좌표.
	WORD				m_wOldPosY;
	BYTE				m_bOldDir;						// 현재 행동이 있기전의 방향.
	WORD				m_wPosX;						// 타일 X좌표.
	WORD				m_wPosY;						// 타일 Y좌표.
	SHORT				m_shShiftPixelX;				// 현재 자신의 타일에서 X축만큼 이동한 픽셀의 거리.
	SHORT				m_shShiftPixelY;				// 현재 자신의 타일에서 Y축만큼 이동한 픽셀의 거리.
	SHORT				m_shShiftTileX;					// 현재 자신의 타일에서 X축만큼 이동한 타일의 거리.
	SHORT				m_shShiftTileY;					// 현재 자신의 타일에서 Y축만큼 이동한 타일의 거리.
	SHORT				m_shScrnPosX;					// 중심점을 고려하지 않은 화면상의 X좌표.
	SHORT				m_shScrnPosY;					// 중심점을 고려하지 않은 화면상의 Y좌표.
	BYTE				m_bMoveSpeed;					// 이동속도.

	CWHQueue			m_xPacketQueue;					// Actor가 처리해야될 메시지를 저장하고 있는 큐.
	BOOL				m_bMsgHurryCheck;				// 메시지가 2개이상 쌓여있을때는 프레임을 1프레임씩을 건너뛴다.

	BYTE				m_bBackStepFrame;
	BYTE				m_bBackStepFrameCnt;

	DWORD				m_dwCurrEffectFrame;			// 캐릭터 Effect현재 프레임.
	DWORD				m_dwFstEffectFrame;				// 캐릭터 Effect처음 프레임.
	DWORD				m_dwEndEffectFrame;				// 캐릭터 Effect마지막 프레임.
	BYTE				m_bEffectFrame;
	BYTE				m_bEffectFrameCnt;
	BYTE				m_bLightRadius[2];				// 광원 너비.
	BYTE				m_bLightColor [2][3];			// 광원 색.

	BOOL				m_bUseEffect;					// 이펙트프레임을 사용.
	BOOL				m_bUseSwordEffect;				// 검법효과 사용.

	BOOL				m_bWarMode;
	DWORD				m_dwWarModeTime;

	BYTE				m_bHPPercent;
	WORD				m_wMAXHP;
	WORD				m_wHP;
	WORD				m_wMP;
	BOOL				m_bOpenHealth;

	BYTE				m_bLightSize;					// Actor주변의 광원크기.

	D3DVERTEX			m_avBoard[4];

	WORD				m_wABlendDelay;
	WORD				m_wABlendCurrDelay;
	BOOL				m_bABlendRev;

	BOOL				m_bReverse;

	INT					m_nState;
	WORD				m_wStateClr;

	DWORD				m_dwNameClr;

	BOOL				m_bIsDead;

	FEATURE				m_stHitter;

	BYTE				m_bAppearState;

	BYTE				m_bMoveNextFrmCnt;
	BOOL				m_bIsMoved;

	BOOL				m_bFstSoundPlayed;

	INT					m_nDividedChatLine;
	DWORD				m_wCurrChatDelay;
	CHAR				m_szChatMsg[MAX_PATH];			// 채팅메시지.
	CHAR				m_szChatMsgArg[5][MAX_PATH];	// 채팅메시지를 5개의 스트링으로 구분한것.

public:
//---------------------------------------------------------------------------------------------------------------//
// 생성자 및 소멸자.
	CActor();
	~CActor();

	virtual VOID InitActor();
	virtual VOID DestroyActor();
//---------------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------------//
// 생성.
	virtual BOOL Create(CImageHandler* pxImgHandler, FEATURE* pstFeature, BYTE bMtn, WORD bDir, WORD wPosX, WORD wPosY);
//---------------------------------------------------------------------------------------------------------------//

	BOOL LoadEffect(CImageHandler* pxImgHandler, WORD wEffectNum, BYTE bDir = 0);
	VOID DrawWithEffected(INT nx, INT nY, INT nXSize, INT nYSize, WORD* pwSrc, WORD wChooseColor1 = 0XFFFF, WORD wChooseColor2 = 0XFFFF, BOOL bFocused = FALSE, BYTE bOpa = 50, WORD wState = _STATE_NOTUSED);
//---------------------------------------------------------------------------------------------------------------//
// 프레임 설정관련.
	BOOL	CheckFeatureValidate(FEATURE stFeature);
	BOOL	ChangeFeature(FEATURE stFeature);
	virtual BOOL SetMotionFrame(BYTE bMtn, BYTE bDir);
//---------------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------------//
// 캐릭터 이동.
	VOID SetMoving();
	VOID SetBackStepMoving();
	VOID SetMoved();
	WORD GetCharState();
//---------------------------------------------------------------------------------------------------------------//

	VOID ChatMsgAdd();
	VOID ShowMessage(INT nLoopTime);
	VOID StruckMsgReassign();
	
	virtual VOID PlayActSound();

//---------------------------------------------------------------------------------------------------------------//
// 동작 진행.
	virtual BOOL UpdatePacketState();
	virtual VOID UpdateMotionState(INT nLoopTime, BOOL bIsMoveTime);
	virtual BOOL UpdateMove(BOOL bIsMoveTime);

// 유형별 패킷상태적용.
	virtual VOID OnCharDescPacket(LPPACKETMSG lpPacketMsg);
	virtual VOID OnUserName(LPPACKETMSG lpPacketMsg);
	virtual VOID OnChangeNameClr(LPPACKETMSG lpPacketMsg);
	virtual VOID OnChangeLight(LPPACKETMSG lpPacketMsg);
	virtual VOID OnOpenHealth(LPPACKETMSG lpPacketMsg);
	virtual VOID OnCloseHealth(LPPACKETMSG lpPacketMsg);
	virtual VOID OnFeatureChanged(LPPACKETMSG lpPacketMsg);
	virtual VOID OnHealthSpellChanged(LPPACKETMSG lpPacketMsg);
	virtual VOID OnWalk(LPPACKETMSG lpPacketMsg);
	virtual VOID OnTurn(LPPACKETMSG lpPacketMsg);
	virtual VOID OnDigup(LPPACKETMSG lpPacketMsg);
	virtual VOID OnDigDown(LPPACKETMSG lpPacketMsg);
	virtual VOID OnDeath(LPPACKETMSG lpPacketMsg);
	virtual VOID OnBackStep(LPPACKETMSG lpPacketMsg);
	virtual VOID OnStruck(LPPACKETMSG lpPacketMsg);
	virtual VOID OnHit(LPPACKETMSG lpPacketMsg);
	virtual VOID OnFlyAxe(LPPACKETMSG lpPacketMsg);
	virtual VOID OnLighting(LPPACKETMSG lpPacketMsg);
//---------------------------------------------------------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------------------//
// Rendering.
	virtual BOOL DrawActor(CMapHandler* pxMap, BOOL bFocused = FALSE, BOOL bShadowAblended = FALSE, 
		                   BOOL bUseScrnPos = TRUE, BOOL bDrawShadow = TRUE);
	virtual VOID DrawHPBar();
	virtual VOID DrawName();

	BOOL DrawEffect();
//---------------------------------------------------------------------------------------------------------------//
protected:
public:
};








/******************************************************************************************************************

	CNPC Class

*******************************************************************************************************************/
class CNPC : public CActor
{
private:
private:
public:
	virtual VOID OnHit(LPPACKETMSG lpPacketMsg);
	virtual VOID OnTurn(LPPACKETMSG lpPacketMsg);
	virtual BOOL UpdatePacketState();
	virtual VOID UpdateMotionState(INT nLoopTime, BOOL bIsMoveTime);
	virtual BOOL DrawActor(CMapHandler* pxMap, BOOL bFocused = FALSE, BOOL bShadowAblended = FALSE, 
		                   BOOL bUseScrnPos = TRUE, BOOL bDrawShadow = TRUE);
};








/******************************************************************************************************************

	CHero Class

*******************************************************************************************************************/
#pragma pack(1)
typedef struct tagFEATUREEX
{
	BYTE		bHorse;
	BYTE		wHairColor;
	BYTE		wDressColor;
}FEATUREEX, *LPFEATUREEX;
#pragma pack(8)

class CHero : public CActor
{
private:
public:
	FEATUREEX			m_stFeatureEx;

	BYTE				m_bWeaponImgIdx;				// 현재 Actor 무기가 가질 이미지구조체의 번호.
	BYTE				m_bHairImgIdx;					// 현재 Actor 머리가 가질 이미지구조체의 번호.
	BYTE				m_bHorseImgIdx;					// 현재 Actor 말이   가질 이미지구조체의 번호.

	BYTE				m_bYedoCnt;
	BYTE				m_bFireHitCnt;
	BOOL				m_bUseBanwol;
	BOOL				m_bUseErgum;

	RECT				m_rcHair;
	RECT				m_rcWeapon;
	RECT				m_rcHorse;

	CWHWilImageData*	m_pxHairImage;		
	CWHWilImageData*	m_pxWeaponImage;
	CWHWilImageData*	m_pxHorseImage;

	DWORD				m_dwCurrHairFrame;
	DWORD				m_dwCurrWeaponFrame;
	DWORD				m_dwCurrHorseFrame;

	BYTE				m_bShieldCurrFrm;
	WORD				m_wShieldCurrDelay;

	SHORT				m_shHitSpeed;

	BOOL				m_bIsMon;

	SHORT				m_shCurrMagicID;

protected:
public:
	CHero();
	~CHero();

	virtual VOID InitActor();
	virtual VOID DestroyActor();

	VOID ShowShield();
	VOID PlayMoveSound();

	virtual VOID PlayActSound();
	virtual BOOL ChangeFeature(FEATURE stFeature, FEATUREEX stFeatureEx);
	virtual BOOL Create(CImageHandler* pxImgHandler, BYTE bMtn, BYTE bDir, WORD wPosX, WORD wPosY, FEATURE* pstFeature, FEATUREEX* pstFeatureEx);
	virtual BOOL UpdatePacketState();
	virtual VOID UpdateMotionState(INT nLoopTime, BOOL bIsMoveTime);
	virtual BOOL UpdateMove(BOOL bIsMoveTime);
	virtual VOID OnRun(LPPACKETMSG lpPacketMsg);
	virtual VOID OnRush(LPPACKETMSG lpPacketMsg);
	virtual VOID OnMagicFire(LPPACKETMSG lpPacketMsg);
	virtual VOID OnSpell(LPPACKETMSG lpPacketMsg);
	virtual VOID OnCharDescPacket(LPPACKETMSG lpPacketMsg);
	virtual VOID OnFeatureChanged(LPPACKETMSG lpPacketMsg);
	virtual VOID OnCharStatusChanged(LPPACKETMSG lpPacketMsg);
	virtual VOID OnBackStep(LPPACKETMSG lpPacketMsg);
	virtual VOID OnDeath(LPPACKETMSG lpPacketMsg);
	virtual VOID OnWalk(LPPACKETMSG lpPacketMsg);
	virtual VOID OnTurn(LPPACKETMSG lpPacketMsg);
	virtual VOID OnStruck(LPPACKETMSG lpPacketMsg);
	virtual VOID OnHit(LPPACKETMSG lpPacketMsg);
	virtual VOID OnButch(LPPACKETMSG lpPacketMsg);
	virtual BOOL DrawActor(CMapHandler* pxMap, BOOL bFocused = FALSE, BOOL bShadowAblended = FALSE, 
		                   BOOL bUseScrnPos = TRUE, BOOL bDrawShadow = TRUE);
};










/******************************************************************************************************************

	CMyHero Class

*******************************************************************************************************************/
#pragma pack(1)
typedef struct tagACTORABILITY
{
    WORD	wAC;
    WORD	wMAC;
	WORD	wDC;
    WORD	wMC;
    WORD	wSC;
	WORD	wHP;
	WORD	wMP;
    WORD	wMaxHP;
    WORD	wMaxMP;
	WORD	wWeight;
 	BYTE	bExpCount;
	BYTE	bExpMaxCount;
	BYTE	bLevel;
	BYTE	bReserved1;
	BYTE	bWearWeight;
	BYTE	bMaxWearWeight;
    BYTE	bHandWeight;
    BYTE	bMaxHandWeight;

	DWORD	dwExp;
	DWORD	dwMaxExp;
	WORD	wMaxWeight;
}ACTORABILITY, *LPACTORABILITY;

typedef struct tagACTORSUBABILITY
{
	WORD wAntiMagic;		// 마법회피.
	BYTE bHitPoint;			// 정확.
	BYTE bSpeedPoint;		// 민첩.
	BYTE bAntiPoison;		// 중독회피.
	BYTE bPoisonRecover;	// 중독회복.
	BYTE bHealthRecover;	// 체력회복.
	BYTE bSpellRecover;		// 마력회복.
}ACTORSUBABILITY, *LPACTORSUBABILITY;
#pragma pack(8)

class CMyHero : public CHero
{
private:
public:
//---------------------------------------------------------------------------------------------------------------//
//  캐릭터 보정용 변수.
	DWORD			m_dwMotionLockTime;
	BOOL			m_bInputLock;				// 캐릭터가 동작되면 어떤입력도 받지 않는다.
	BOOL			m_bMotionLock;				// 캐릭터의 동작이 세팅되면 서버에서 동작승인이 오기전까진 어떤동작도 받지 않는다.
//---------------------------------------------------------------------------------------------------------------//

	CMapHandler*	m_pxMap;
	BOOL			m_bCanRun;
	BYTE			m_bAttackMode;
	ACTORABILITY    m_stAbility;
	ACTORSUBABILITY m_stSubAbility;
	BYTE		    m_bJob;
	UINT			m_nGlod;

	CWHQueue		m_xPriorPacketQueue;					// Actor가 처리해야될 메시지를 저장하고 있는 큐.

//---------------------------------------------------------------------------------------------------------------//
//  캐릭터 Delay관련 변수.
	DWORD			m_dwLastHitTime;						// 칼질이 들어갔을때의 틱 보관값.
	DWORD			m_dwLastSpellTime;						// 마법이 들어갔을때의 틱 보관값.
	DWORD			m_dwLastMagicTime;						// 염화결 시전후의 틱보관값.
	DWORD			m_dwLastStruckTime;						// 맞았을때의 틱 보관값.
	DWORD			m_dwLastPKStruckTime;
	DWORD			m_dwLastRushTime;						// 무태보 시전후의 틱보관값.
	DWORD			m_dwLastFireHitTime;					// 염화결 시전후의 틱보관값.

	WORD			m_wMagicPKDelayTime;					// 마법으로 사람을 공격했을때의 패널티Delay.(걷기와 뛰기에 적용된다.)
	WORD			m_wMagicDelayTime;						// 마법을쓰고 난후 다음마법이 들어갈때까지의 Delay.
//---------------------------------------------------------------------------------------------------------------//

protected:
public:
	CMyHero();
	~CMyHero();

	virtual VOID InitActor();
	virtual VOID DestroyActor();

	VOID  SetMotionState(BYTE bMtn, BYTE bDir, INT nMouseTargetID = NULL, BOOL bIsDead = FALSE, LPPOINT lpptPos = NULL, SHORT shSkill = -1);
	VOID  SetMapHandler(CMapHandler* pxMap);
	VOID  SetMagic(LPCLIENTMAGICRCD	pstMagic, BYTE bKeyNum, BYTE bDir, INT nTargetID, FEATURE stTargetFeature, POINT ptTargetPos);
	BOOL  SetOldPosition();
	BOOL  CheckMyPostion();
	VOID  AdjustMyPostion();
	BYTE  CalcDirection(INT nTargetTileX, INT nTargetTileY);
	POINT GetPosMouseToTile(INT nXPos, INT nYPos);

	virtual BOOL Create(CImageHandler* pxImgHandler, BYTE bMtn, BYTE bDir, WORD wPosX, WORD wPosY, FEATURE* pstFeature, FEATUREEX* pstFeatureEx);
	virtual VOID OnHealthSpellChanged(LPPACKETMSG lpPacketMsg);
	virtual BOOL UpdatePacketState();
	virtual VOID UpdateMotionState(INT nLoopTime, BOOL bIsMoveTime);
	virtual BOOL UpdateMove(BOOL bIsMoveTime);
	virtual BOOL DrawActor(BOOL bFocused = FALSE, BOOL bShadowAblended = FALSE, 
		                   BOOL bUseScrnPos = FALSE, BOOL bDrawShadow = TRUE);

	__inline virtual BOOL ChangeFeature(FEATURE stFeature, FEATUREEX stFeatureEx)
	{
		if ( m_bIsMon )		
		{
			if ( CActor::ChangeFeature(stFeature) )					return TRUE;
		}
		else	
		{
			if ( CHero::ChangeFeature(stFeature, stFeatureEx) )		return TRUE;
		}

		return FALSE;
	}

//---------------------------------------------------------------------------------------------------------------//
//  캐릭터 Delay관련 함수.
	BOOL CanNextHit();									// 지금 칼질할수 있나의 여부. 레벨과 착용무게에 따라 달라진다.
	BOOL CanWalk();										// Delay와 관련지어서 캐릭터가 움직일수 있는지의 여부.
	BOOL CanRun();										// Delay와 관련지어서 캐릭터가 움직일수 있는지의 여부.
//---------------------------------------------------------------------------------------------------------------//
//---------------------------------------------------------------------------------------------------------------//
//  유저 입력함수.
	BOOL OnLButtonDown(POINT ptMouse, INT nTargetID = -1, BOOL bIsDead = FALSE, POINT* lpptTargetPos = NULL);
	BOOL OnRButtonDown(POINT ptMouse);
	BOOL OnKeyDown(WPARAM wParam, LPARAM lParam, POINT ptMouse, POINT ptTargetPos, INT nMosueTargetID, FEATURE stTargetFeature);
//---------------------------------------------------------------------------------------------------------------//
};








#endif // _CACTOR_H