#ifndef _MAGIC_HANDLER
#define _MAGIC_HANDLER

#define _SKILL_BANWOL				0			// 반월.
#define _SKILL_AMYOUNSUL			1			// 암연술.
#define _SKILL_FIREWIND				2			// 화염풍.
#define _SKILL_HANGMAJINBUB			3			// 항마진법.
#define _SKILL_DEJIWONHO			4			// 대지원호.
#define _SKILL_FIRE					5			// 염사장.
#define _SKILL_MOOTEBO				6			// 무태보.

#define _SKILL_TAMMING				7			// 뢰혼격.
#define _SKILL_SPACEMOVE			8			// 아공행법.
#define _SKILL_KILLUNDEAD			9			// 사자윤회.

#define _SKILL_FIREBALL				10			// 화염장.
#define _SKILL_HEALLING				11			// 회복술.
#define _SKILL_HOLYSHIELD			12			// 결계.
#define _SKILL_BIGHEALLING			13			// 대회복.
#define _SKILL_LIGHTFLOWER			14			// 뢰설화.
#define _SKILL_SHOOTLIGHTEN			15			// 뢰인장.
#define _SKILL_SKELLETON			16			// 백골소환술.
#define _SKILL_SNOWWIND				17			// 빙설풍.
#define _SKILL_BIGCLOAK				18			// 대은신.
#define _SKILL_SHIELD				19			// 주술의막.
#define _SKILL_SHOWHP				20			// 탐기파연.
#define _SKILL_EATTHFIRE			21			// 지염술.
#define _SKILL_FIRECHARM			22			// 폭살계.
#define _SKILL_FIREBOOM				23			// 폭열파.
#define _SKILL_FIREBALL2			24			// 금강화염장.
#define _SKILL_YEDO					25			// 예도.
#define _SKILL_ERGUM				26			// 어검술.
#define _SKILL_LIGHTENING			27			// 강격.
#define _SKILL_FIRESWORD			28			// 염화결.
#define _SKILL_CLOAK				29			// 은신.
#define _SKILL_SINSU				30			// 신수소환.
#define _SKILL_ILKWANG				31			// 일광검법.
#define _SKILL_ONESWORD				32			// 외수검법.

class CMagicInfo
{
public:
	int		nIndex;
	char	szName[13];
	
	short	sEffectType;
	short	sEffect;
	short	sSpell;
	short	sPower;
	short	sMaxPower;
	short	sDefSpell;
	short	sDefPower;
	short	sDefMaxPower;
	
	short	sJob;

	short	sNeed[3];
	int		nTrain[3];
	
	short	sDelay;

public:
	int		GetPower13(int nPwr, int nLevel);
	BOOL	CheckMagicLevelup(CCharObject* pCharObject, _LPTHUMANMAGICRCD lptMagicRcd);

	__inline int MPow() 
		{ if ((sDefMaxPower - sDefPower) > 0) return sDefPower + (rand() % (sDefMaxPower - sDefPower)); 
			else return sDefPower; }
	__inline int GetSpellPoint(int nLevel)
		{ return ROUND(sSpell / (3/*MaxTrainLevel*/ + 1) * (nLevel + 1)) + sDefSpell; }
	__inline int GetPower(int nPwr, int nLevel)
		{ if ((sDefMaxPower - sDefPower) > 0) return ROUND(nPwr / (3/*MaxTrainLevel*/ + 1) * (nLevel + 1)) + (sDefPower + (rand() % (sDefMaxPower - sDefPower))); 
			else return ROUND(nPwr / (3/*MaxTrainLevel*/ + 1) * (nLevel + 1)) + sDefPower; }	
};

#endif
