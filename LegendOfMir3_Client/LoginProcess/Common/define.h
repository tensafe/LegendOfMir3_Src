#ifndef		_LOGIN_DEFINE_H
#define		_LOGIN_DEFINE_H


#define BTN_LOGIN_ID				0
#define BTN_NEW_ID					1
#define BTN_CHANGE_ID				2
#define BTN_EXIT_ID					3


#define IMG_IDX_LOGIN_BACK			0
#define IMG_IDX_BTN_NEW				5
#define IMG_IDX_BTN_CHGPASS			8
#define IMG_IDX_BTN_OK				11
#define IMG_IDX_SRVBORDER_LEFT		20
#define IMG_IDX_SRVBORDER_MIDDLE	21
#define IMG_IDX_SRVBORDER_RIGHT		22
#define IMG_IDX_BTN_UP				23
#define IMG_IDX_BTN_ON				24
#define IMG_IDX_BTN_DOWN			25


#define LOGBTN_HEIGHT		26
#define LOGBTN_WIDTH		176

// AVI Screen Rect
#define SCR_LEFT			0
#define SCR_TOP				50 
#define SCR_RIGHT			_LOGIN_SCREEN_WIDTH
#define SCR_BOTTOM			410

#define SCR_TOP2			30
#define SCR_BOTTOM2			390

#define PRG_PATCH_FILES				77
#define PRG_NEED_ACCOUNT_CHANGE		88
#define PRG_QUIT					99

#define CONNECT_FAIL				0
#define CONNECT_SUCCESS				1
#define CONNECT_CLOSE				2

#define IMAGE_INDEX_NEWACCOUNT		208
#define IMAGE_INDEX_DOOR_0			35
#define IMAGE_INDEX_SELECT_CHR		18
#define IMAGE_INDEX_MESSAGEBOX_1	206
#define IMAGE_INDEX_MESSAGEBOX_2	213
#define IMAGE_INDEX_LOGINBASE		29
#define IMAGE_INDEX_LOGINBOX		2

#define PUNISHMENT_ID_USING				1		//	"현재 본 ID는 사용중입니다."
#define PUNISHMENT_ID_THEFT				2		//	"아이디 도용과 관련하여 계정이 블럭된 상태입니다."
#define PUNISHMENT_DISTURB_GAME			3		//	"게임의 진행을 방해하여 계정이 블럭된 상태입니다."
#define PUNISHMENT_BBS_CURSES			4		//	"게시판등에 욕설과 관련하여 계정이 블럭된 상태입니다."
#define PUNISHMENT_CONFIRMED_CURSES		5		//	"상습적인 욕설로 인하여 계정이 블럭된 상태입니다."
#define PUNISHMENT_USAGE_BUG			6		//	"버그를 이용한 게임으로 인하여 계정이 블럭된 상태 입니다."
#define PUNISHMENT_ASSUMED_GAMEMASTER	7		//	"운영자 사칭등에 관련하여 계정이 블럭된 상태입니다."
#define PUNISHMENT_SPREAD_FALLACY		8		//	"허위사실 유포등에 의하여 계정이 블럭된 상태입니다."

//  New
#define POS_ID_TITLE_X				111
#define POS_ID_TITLE_Y				410
#define POS_PASS_TITLE_X			296
#define POS_PASS_TITLE_Y			410

/*#define	POS_LOGEXIT_X				POS_LOGINBOX_X + 176
#define	POS_LOGEXIT_Y				POS_LOGINBOX_Y + 227*/

// New Account Dialog Pos
#define ACCOUNT_WIN_POS_X			 80
#define ACCOUNT_WIN_POS_Y			 61

// New Account Edit Box Position
#define NEW_WIDTH1					114
#define NEW_WIDTH2					47
#define NEW_WIDTH3					160

#define POS_NEW_X1					ACCOUNT_WIN_POS_X + 143
#define POS_NEW_X2					ACCOUNT_WIN_POS_X + 208

#define POS_NEWID_Y					ACCOUNT_WIN_POS_Y +	 52
#define POS_NEWPASS_Y				ACCOUNT_WIN_POS_Y +  72
#define POS_NEWREPASS_Y				ACCOUNT_WIN_POS_Y +	 92

#define POS_NEWNAME_Y				ACCOUNT_WIN_POS_Y + 125
#define POS_NEWSSNO_Y				ACCOUNT_WIN_POS_Y +	145
#define POS_NEWBIRTH_Y				ACCOUNT_WIN_POS_Y +	165

#define POS_NEWZIPCODE_Y			ACCOUNT_WIN_POS_Y + 198
#define POS_NEWADDRESS1_Y			ACCOUNT_WIN_POS_Y + 218
#define	POS_NEWADDRESS2_Y			ACCOUNT_WIN_POS_Y + 238

#define POS_NEWTELNUM_Y				ACCOUNT_WIN_POS_Y + 269
#define POS_NEWMOBILE_Y				ACCOUNT_WIN_POS_Y + 289
#define POS_NEWEMAIL_Y				ACCOUNT_WIN_POS_Y + 309

#define POS_NEWQUIZ1_Y				ACCOUNT_WIN_POS_Y + 340
#define POS_NEWANSWER1_Y			ACCOUNT_WIN_POS_Y + 361
#define POS_NEWQUIZ2_Y				ACCOUNT_WIN_POS_Y + 382
#define POS_NEWANSWER2_Y			ACCOUNT_WIN_POS_Y + 403


// New Account Button
#define POS_NEW_OK_POS_X			ACCOUNT_WIN_POS_X + 399
#define POS_NEW_OK_POS_Y			ACCOUNT_WIN_POS_Y + 422
#define POS_NEW_CANCLE_POS_X		ACCOUNT_WIN_POS_X + 506
#define POS_NEW_CANCLE_POS_Y		ACCOUNT_WIN_POS_Y + 422
#define POS_NEW_QUIT_POS_X			ACCOUNT_WIN_POS_X + 616
#define POS_NEW_QUIT_POS_Y			ACCOUNT_WIN_POS_Y + 453

// New Account ex
#define POS_NEWEXP_X				ACCOUNT_WIN_POS_X +	378
#define POS_NEWEXP_Y				ACCOUNT_WIN_POS_Y +	112

#define NEW_EXP_WIDTH				214
#define NEW_EXP_HEIGHT				457

 
#define POS_MSG_POS_X				100
#define	POS_MSG_POS_Y				100

#define LIMIT_USERID				1

#define BTN_EXIT_ID					3
#define BTN_PRE_SERVER_ID			4
#define BTN_NEXT_SERVER_ID			5
#define BTN_SERVER_SELECT_ID		6
#define BTN_NEW_OK_ID				7
#define BTN_NEW_CANCLE_ID			8
#define BTN_NEW_QUIT_ID				9

// NEW

// NEW
#define POS_BTN_NEW_X				50
#define POS_BTN_NEW_Y				440
#define POS_BTN_CHG_X				250
#define POS_BTN_CHG_Y				440
#define POS_BTN_OK_X				450
#define POS_BTN_OK_Y				440

// NEW


// NEW
#define	COUNT_BUTTON_PER_COLUME		8

#define MIR2EI_INI_FILE_NAME		"Mir3.ini"
#define MIR2EI_INI_SECTION			"Initial"
#define MIR2EI_SRV_SECTION			"Server"
#define LOGIN_SETVER_NAME			"ServerAddr"
#define LOGIN_SERVER_PORT			"Param1"
#define SERVER_COUNT				"ServerCount"
#define LOGIN_GATE_SERVER_IP		"192.168.0.200"
#define	LOGIN_SERVER_PORT_DATA		7000

// File Names
#define IMAGE_INTERFACE_1			".\\Data\\Interface1c.wil"
#define IMAGE_INTERFACE_2			".\\Data\\Inventory.wil"//".\\Data\\Interfacec2.wil"
#define AVI_INTRO_MAIN				".\\ei_login.avi"
#define AVI_EI_LOGO					".\\ei_logo.avi"
#define MP3_INTRO_MAIN				".\\Sound\\StartGame.mp3" //EI_Login.mp3"

#define BTN_NEW_OK			209
#define BTN_NEW_CANCLE		211
#define BTN_NEW_QUIT		50


#endif	//	_LOGIN_DEFINE_H