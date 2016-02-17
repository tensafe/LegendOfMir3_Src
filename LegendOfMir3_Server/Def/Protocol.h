
#ifndef _LEGENDOFMIR2_PROTCOL
#define _LEGENDOFMIR2_PROTCOL

#define DEFBLOCKSIZE			16
#define HUMANRCDBLOCKSIZE		256
#define ITEMRCDBLOCKSIZE		70			// _TUSERITEMRCD
#define MAGICRCDBLOCKSIZE		10
#define GENITEMRCDBLOCKSIZE		18

#define HOWMANYMAGICS			31

#define USERITEMMAX				65

// For Login Process
#define CM_PROTOCOL				2000
#define CM_IDPASSWORD			2001
#define CM_ADDNEWUSER			2002
#define CM_CHANGEPASSWORD		2003
#define CM_UPDATEUSER			2004

#define CM_SELECTSERVER			104

#define SM_CERTIFICATION_FAIL	501
#define SM_ID_NOTFOUND			502
#define SM_PASSWD_FAIL			503
#define SM_NEWID_SUCCESS        504
#define SM_NEWID_FAIL           505
#define SM_PASSOK_SELECTSERVER	529
#define SM_SELECTSERVER_OK		530

// For Select Character Process
#define CM_QUERYCHR				100
#define CM_NEWCHR				101
#define CM_DELCHR				102
#define CM_SELCHR				103

#define	SM_QUERYCHR				520
#define	SM_NEWCHR_SUCCESS		521
#define	SM_NEWCHR_FAIL			522
#define	SM_DELCHR_SUCCESS		523
#define	SM_DELCHR_FAIL			524
#define	SM_STARTPLAY			525
#define	SM_STARTFAIL			526
#define SM_QUERYCHR_FAIL		527

// For Game Gate
#define GM_OPEN					1
#define GM_CLOSE				2
#define	GM_CHECKSERVER			3			// Send check signal to Server
#define GM_CHECKCLIENT			4			// Send check signal to Client
#define GM_DATA					5
#define GM_SERVERUSERINDEX		6
#define GM_RECEIVE_OK			7
#define GM_TEST					20

// For game process
// Client To Server Commands
#define CM_QUERYUSERNAME        80
#define CM_QUERYBAGITEMS        81

#define CM_DROPITEM             1000
#define CM_PICKUP               1001
#define CM_TAKEONITEM			1003
#define CM_TAKEOFFITEM          1004
#define CM_BUTCH                1007
#define CM_MAGICKEYCHANGE		1008
#define CM_EAT                  1006
#define CM_TURN                 3010
#define CM_WALK                 3011
#define CM_SITDOWN              3012
#define CM_RUN                  3013
#define CM_HIT                  3014
#define CM_HEAVYHIT             3015
#define CM_BIGHIT               3016
#define CM_SPELL                3017
#define CM_POWERHIT             3018
#define CM_LONGHIT              3019
#define CM_WIDEHIT              3024
#define CM_FIREHIT              3025
#define CM_SAY                  3030
#define CM_RIDE					3031

// Server to Client Commands
#define SM_RUSH					6
#define SM_FIREHIT              8
#define SM_BACKSTEP				9
#define SM_TURN					10
#define SM_WALK					11
#define SM_SITDOWN              12
#define SM_RUN					13
#define SM_HIT					14
#define SM_SPELL				17
#define SM_POWERHIT             18
#define SM_LONGHIT              19
#define SM_DIGUP                20
#define SM_DIGDOWN              21
#define SM_FLYAXE               22
#define SM_LIGHTING             23
#define SM_WIDEHIT              24
#define SM_DISAPPEAR            30
#define SM_STRUCK				31
#define SM_DEATH                32
#define SM_SKELETON             33
#define	SM_NOWDEATH             34

#define SM_HEAR					40
#define SM_FEATURECHANGED       41
#define SM_USERNAME             42
#define SM_WINEXP               44
#define SM_LEVELUP              45
#define SM_DAYCHANGING          46
#define SM_LOGON				50
#define SM_NEWMAP               51
#define SM_ABILITY				52
#define SM_HEALTHSPELLCHANGED   53
#define SM_MAPDESCRIPTION       54
#define SM_SPELL2               117

#define SM_SYSMESSAGE           100
#define SM_GROUPMESSAGE         101
#define SM_CRY                  102
#define SM_WHISPER              103
#define SM_GUILDMESSAGE         104

#define SM_ADDITEM				200
#define SM_BAGITEMS				201
#define SM_ADDMAGIC             210
#define SM_SENDMYMAGIC			211

#define SM_DROPITEM_SUCCESS     600
#define SM_DROPITEM_FAIL        601

#define SM_ITEMSHOW             610
#define SM_ITEMHIDE             611
#define SM_DOOROPEN				612
#define SM_TAKEON_OK            615
#define SM_TAKEON_FAIL          616
#define SM_TAKEOFF_OK           619
#define SM_TAKEOFF_FAIL         620
#define SM_SENDUSEITEMS         621
#define SM_WEIGHTCHANGED        622
#define SM_CLEAROBJECTS         633
#define SM_CHANGEMAP            634
#define SM_EAT_OK               635
#define SM_EAT_FAIL             636
#define SM_BUTCH                637
#define SM_MAGICFIRE			638
#define SM_MAGIC_LVEXP          640
#define SM_DURACHANGE           642
#define SM_MERCHANTSAY          643
#define SM_GOLDCHANGED          653
#define SM_CHANGELIGHT          654
#define SM_CHANGENAMECOLOR      656
#define SM_CHARSTATUSCHANGED	657

#define SM_SUBABILITY           752

#define SM_SHOWEVENT            804
#define SM_HIDEEVENT            805

#define SM_OPENHEALTH           1100
#define SM_CLOSEHEALTH          1101
#define SM_CHANGEFACE           1104

#define SM_ITEMUPDATE			1500
#define SM_MONSTERSAY			1501

// Server to Server Commands
#define RM_TURN					10001
#define RM_WALK					10002
#define RM_RUN					10003
#define RM_HIT					10004
#define RM_SPELL				10007
#define RM_SPELL2				10008
#define RM_POWERHIT             10009
#define RM_LONGHIT              10011
#define RM_WIDEHIT              10012
#define RM_PUSH					10013
#define RM_FIREHIT              10014
#define RM_RUSH					10015
#define RM_STRUCK				10020
#define RM_DEATH                10021
#define RM_DISAPPEAR			10022
#define RM_MAGSTRUCK            10025
#define RM_STRUCK_MAG           10027
#define RM_MAGSTRUCK_MINE       10028
#define RM_MAGHEALING           10026
#define RM_HEAR					10030
#define RM_WHISPER              10031
#define RM_CRY                  10032
#define RM_RIDE					10033
#define RM_WINEXP               10044
#define RM_USERNAME				10043
#define RM_LEVELUP				10045
#define RM_CHANGENAMECOLOR      10046

#define RM_LOGON				10050
#define RM_ABILITY				10051
#define RM_HEALTHSPELLCHANGED   10052
#define RM_DAYCHANGING          10053

#define RM_SYSMESSAGE			10100
#define RM_GROUPMESSAGE         10102
#define RM_SYSMESSAGE2			10103
#define RM_GUILDMESSAGE         10104
#define RM_ITEMSHOW             10110
#define RM_ITEMHIDE             10111
#define RM_DOOROPEN				10112
#define RM_SENDUSEITEMS         10114
#define RM_WEIGHTCHANGED        10115
#define RM_FEATURECHANGED       10116
#define RM_CLEAROBJECTS         10117
#define RM_CHANGEMAP            10118
#define RM_BUTCH                10119
#define RM_MAGICFIRE            10120
#define RM_SENDMYMAGIC			10122
#define RM_MAGIC_LVEXP          10123
#define RM_SKELETON             10024
#define RM_DURACHANGE           10125
#define RM_MERCHANTSAY          10126
#define RM_GOLDCHANGED          10136
#define RM_CHANGELIGHT          10137
#define RM_CHARSTATUSCHANGED	10139
#define RM_DELAYMAGIC			10154

#define RM_DIGUP                10200
#define RM_DIGDOWN              10201
#define RM_FLYAXE               10202
#define RM_LIGHTING             10204

#define RM_SUBABILITY           10302
#define RM_TRANSPARENT          10308

#define RM_SPACEMOVE_SHOW       10331
#define RM_HIDEEVENT            10333
#define RM_SHOWEVENT            10334
#define RM_ZEN_BEE				10337

#define RM_OPENHEALTH           10410
#define RM_CLOSEHEALTH          10411
#define RM_DOOPENHEALTH         10412
#define RM_CHANGEFACE           10415

#define RM_ITEMUPDATE			11000
#define RM_MONSTERSAY			11001
#define RM_MAKESLAVE			11002

// For DB Server
#define DB_LOADHUMANRCD			100
#define DB_SAVEHUMANRCD			101
#define DB_MAKEITEMRCD			150
#define DB_ITEMTHROW			151
#define DB_MAKEITEMRCD2			152

#define DBR_LOADHUMANRCD		1100
#define DBR_LOADHUMANRCD2		1101
#define DBR_MAKEITEMRCD			1500
#define DBR_MAKEITEMRCD2		1501

#define DBR_FAIL				2000

typedef struct tag_TMESSAGEBODYWL
{
	LONG	lParam1;
	LONG	lParam2;
	LONG	nTag1;
	LONG	nTag2;
} _TMESSAGEBODYWL, _LPTMESSAGEBODYWL;

typedef struct tag_TSHORTMSSEAGE
{
	WORD	wIdent;
	WORD	wMsg;
} _TSHORTMSSEAGE, *_LPTSHORTMSSEAGE;

typedef struct tag_TCHARDESC
{
	LONG	Feature;
	LONG	Status;
} _TCHARDESC, *_LPTCHARDESC;

typedef struct tag_TDEFUSERINFO
{
	TCHAR		szLoginID[10];
	TCHAR		szPassword[10];
	TCHAR		szUserName[20];
} _TDEFUSERINFO, *_LPTUDEFUSERINFO;

typedef struct tag_TADDUSERINFO1
{
	TCHAR		szZipCode[15];
	TCHAR		szAddress1[21];
	TCHAR		szAddress2[21];
	TCHAR		szSSNo[15];
	TCHAR		szPhone[14];
	TCHAR		szQuiz[20];
	TCHAR		szAnswer[12];
	TCHAR		szEmail[40];
	TCHAR		szQuiz2[20];
	TCHAR		szAnswer2[12];
	TCHAR		szBirthDay[11];
	TCHAR		szMobilePhone[15];
} _TADDUSERINFO1, *_LPTADDUSERINFO;

typedef struct tag_TUSERENTRYINFO
{
	_TDEFUSERINFO	tDefUserInfo;
	_TADDUSERINFO1	tAddUserInfo;
} _TUSERENTRYINFO, *_LPTUSERENTRYINFO;

typedef struct tag_AUSERENTRYINFO
{
	char		szLoginID[10];
	char		szPassword[10];
	char		szUserName[20];
	
	char		szZipCode[15];
	char		szAddress1[21];
	char		szAddress2[21];
	char		szSSNo[15];
	char		szPhone[14];
	char		szQuiz[20];
	char		szAnswer[12];
	char		szEmail[40];
	char		szQuiz2[20];
	char		szAnswer2[12];
	char		szBirthDay[11];
	char		szMobilePhone[15];
} _AUSERENTRYINFO, *_LPAUSERENTRYINFO;

typedef struct tag_TLOADMUHAN
{
	char		szUserID[16];
	char		szCharName[20];
	char		szUserAddr[15];
	int			nCertification;
} _TLOADHUMAN, *_LPTLOADHUMAN;

#define U_DRESS				0
#define U_WEAPON			1
#define U_RIGHTHAND			2
#define U_NECKLACE			3
#define U_HELMET			4
#define U_ARMRINGL			5
#define U_ARMRINGR			6                   
#define U_RINGL				7
#define U_RINGR				8

#pragma pack(1)
typedef struct tag_THUMANRCD
{
	char		szUserID[16];
	char		szCharName[20];

	BYTE		btJob;
	BYTE		btGender;
	char		szTakeItem[10][12];
	BYTE		szLevel;	
	BYTE		szHair;
	BYTE		nDirection;
	char		szMapName[15];

	BYTE		btAttackMode;
	BYTE		fIsAdmin;

	short		nCX;
	short		nCY;

	DWORD		dwGold;

	WORD		wHairColor;

	int			nExp;
} _THUMANRCD, *_LPTHUMANRCD;

typedef struct tag_TMAKEITEMRCD
{
	char		szStdType;
	int			nStdIndex;
	WORD		nDura;
	WORD		nDuraMax;
	BYTE		btValue[14];
} _TMAKEITEMRCD, *_LPTMAKEITEMRCD;

typedef struct tag_GENERALITEMRCD
{
	char		szMakeIndex[12];
	int			nStdIndex;
	int			nDura;
	int			nDuraMax;
} _TGENERALITEMRCD, *_LPTGENERALITEMRCD;

typedef struct tag_TUSERITEMRCD
{
	char		szMakeIndex[12];
	WORD		nStdIndex;
	WORD		nDura;
	WORD		nDuraMax;
	BYTE		btValue[14];
	char		szPrefixName[20];
} _TUSERITEMRCD, *_LPTUSERITEMRCD;

typedef struct tag_TGENITEMRCD
{
	char		szItem[13];
} _TGENITEMRCD, *_LPTGENITEMRCD;

typedef struct tag_TSTANDARDITEM
{
	char		szName[20];			// 아이템 이름 (천하제일검)
	char		szPrefixName[20];
	BYTE		btStdMode;          //
	BYTE		btShape;            // 형태별 이름 (철검)
	BYTE		btWeight;           // 무게
	BYTE		btAniCount;         // 1보다 크면 애니메이션 되는 아이템
	BYTE		btSource;           // 재질 (0은 기본, 1보다 크면 더 단단함)
	BYTE		btNeedIdentify;     // $01 (아이댄티파이 안 된 것)
	WORD		wLooks;             // 그림 번호
	WORD		wDuraMax;
	WORD		wAC;				// 방어력
	WORD		wMAC;				// 마항력
	WORD		wDC;				// 데미지
	WORD		wMC;				// 술사의 마법 파워
	WORD		wSC;				// 도사의 정신력
	BYTE		btNeed;             // 0:Level, 1:DC, 2:MC, 3:SC
	BYTE		btNeedLevel;        // 1..60 level value...
	UINT		nPrice;
} _TSTANDARDITEM, *_LPTSTANDARDITEM;

typedef struct tag_TCLIENTITEMRCD
{
	char			szMakeIndex[12];
	int				nDura;
	int				nDuraMax;
	_TSTANDARDITEM	tStdItem;
} _TCLIENTITEMRCD, *_LPTCLIENTITEMRCD;

typedef struct tag_TQUERYCHR
{
	BYTE		btClass;
	BYTE		btGender;
	BYTE		btLevel;
	char		szName[20];
} _TQUERYCHR,*_LPTQUERYCHR;

typedef struct tag_TCREATECHR : public tag_TQUERYCHR
{
	char		szID[20];
} _TCREATECHR, *_LPTCREATECHR;

typedef struct tag_THORSERCD
{
	char		szHorseIndex[12];
	BYTE		btHorseType;
} _THORSERCD, *_LPTHORSERCD;

typedef struct tag_THUMANMAGICRCD
{
	BYTE		btMagicID;
	char		btLevel;
	char		btUseKey;
	int			nCurrTrain;
} _THUMANMAGICRCD, *_LPTHUMANMAGICRCD;

typedef struct tag_TSTANDARDMAGIC
{
	BYTE	btEffectType;
	BYTE	btEffect;
	short	sSpell;
	BYTE	btDefSpell;
	
	short	sNeed[3];
	int		nTrain[3];
	
	int		nDelayTime;
} _TSTANDARDMAGIC, *_LPTSTANDARDMAGIC;

typedef struct tag_TCLIENTMAGICRCD
{
	BYTE			btMagicID;
	char			btLevel;
	char			btUseKey;
	int				nCurrTrain;

	_TSTANDARDMAGIC	tStdMagic;
} _TCLIENTMAGICRCD, *_LPTCLIENTMAGICRCD;


#pragma pack(8)

#endif
