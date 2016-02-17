#ifndef	_CHRSEL_DEFINE_H
#define _CHRSEL_DEFINE_H

// Selection Define
#define SELECT_NONE							-1
#define SELECT_FIRST						0
#define SELECT_SECOND						1
#define SELECT_THIRD						2

#define _RENDER_STILL_1						0
#define _RENDER_AVI_IN						1
#define	_RENDER_STILL_2						2
#define _RENDER_AVI_OUT						3
#define _RENDER_AVI_START					4
#define _RENDER_STARTGAME					5

#define ID_SEL_CHR_NEW_BTN					0
#define ID_SEL_CHR_DEL_BTN					1
#define ID_SEL_CHR_EXIT_BTN					2
#define ID_SEL_CHR_START_BTN				3

// Charater Select Image Index
#define IMG_IDX_SELECT_CHR_BACK				50
#define IMG_IDX_SELECT_CREATE				51
#define IMG_IDX_SELECT_DELETE				52
#define IMG_IDX_SELECT_START				53
#define IMG_IDX_SELECT_EXIT					54

// Empty Button Image Index
#define IMG_IDX_EMPTY_BTN_UP				23
#define IMG_IDX_EMPTY_BTN_ON				24
#define IMG_IDX_EMPTY_BTN_DOWN				25

// Charater Create Image Index
#define IMG_IDX_CREATE_CHR_BACK				90
#define IMG_IDX_WAR_BTN_UP					91
#define IMG_IDX_SOR_BTN_UP					94
#define IMG_IDX_TAO_BTN_UP					97
#define IMG_IDX_WAR_MALE_CREATE				100
#define IMG_IDX_WAR_FEMALE_CREATE			101
#define IMG_IDX_SOR_MALE_CREATE				102
#define IMG_IDX_SOR_FEMALE_CREATE			103
#define IMG_IDX_TAO_MALE_CREATE				104
#define IMG_IDX_TAO_FEMALE_CREATE			105

#define IDX_IMG_MALE_WAR					150
#define IDX_IMG_MALE_WAR_FIRE				180
#define IDX_IMG_FEMALE_WAR					210
#define IDX_IMG_FEMALE_WAR_FIRE				240
#define IDX_IMG_MALE_TAO					270
#define IDX_IMG_MALE_TAO_FIRE				300
#define IDX_IMG_FEMALE_TAO					330
#define IDX_IMG_FEMALE_TAO_FIRE				360
#define IDX_IMG_MALE_SOR					390
#define IDX_IMG_MALE_SOR_FIRE				420
#define IDX_IMG_FEMALE_SOR					450
#define IDX_IMG_FEMALE_SOR_FIRE				480

// Button Positions
#define CREATE_CHAR_BTN_POS_Y				420
#define CREATE_CHAR_WAR_BTN_POS_X			280
#define CREATE_CHAR_SOR_BTN_POS_X			320
#define CREATE_CHAR_TAO_BTN_POS_X			360
#define CREATE_CHAR_OK_BTN_POS_X			500
#define CREATE_CHAR_OK_BTN_POS_Y			460
#define CREATE_CHAR_OK_BTN_WIDTH			 72
#define CREATE_CHAR_OK_BTN_HEIGHT			 24

// Position of Select buttons
#define SELECT_CHAR_NEW_BTN_POS_X			442
#define SELECT_CHAR_NEW_BTN_POS_Y			98
#define SELECT_CHAR_ERASE_BTN_POS_X			81
#define SELECT_CHAR_ERASE_BTN_POS_Y			247
#define SELECT_CHAR_START_BTN_POS_X			263
#define SELECT_CHAR_START_BTN_POS_Y			53
#define SELECT_CHAR_EXIT_BTN_POS_X			29
#define SELECT_CHAR_EXIT_BTN_POS_Y			441

#define RETURN_SELECT_NONE					0
#define RETURN_SELECT_NEW_BTN				1
#define RETURN_SELECT_NEW_ERR				2
#define RETURN_SELECT_DEL_BTN				3
#define RETURN_SELECT_DEL_ERR				4
#define RETURN_SELECT_START_BTN				5
#define RETURN_SELECT_START_ERR				6


// Gender
#define SEX_MAN								0
#define SEX_WOMAN							1

// Job counts
#define JOB_KIND							3		// 직업 종류


// Button IDs
#define ID_CREATE_CHR_OK_BTN				0
#define ID_CREATE_CHR_WAR_BTN				1
#define ID_CREATE_CHR_SOR_BTN				2
#define ID_CREATE_CHR_TAO_BTN				3
	
// Button Width & Height
#define WIDTH_EMPTY_BTN						100
#define HEIGHT_EMPTY_BTN					26
#define WIDTH_JOB_BTN						40
#define HEIGHT_JOB_BTN						40

#pragma pack(1)
typedef struct tagChrInfo
{
	BYTE Class;
	BYTE Gender;
	BYTE Level;
	char ChrName[20];
} CHRINFO,* pCHRINFO;

typedef struct tag_TCREATECHR : public tagChrInfo
{
	char		szID[20];
} _TCREATECHR, *_LPTQUERYCHR;

#pragma pack(8)

typedef struct tagChrRect
{
	int nLeft;
	int nTop;
	int nHeight;
	int nWidth;
} CHRRECT,*pCHRRECT;

// Mp3 File names
#define MP3_TOCREATECHAR					".\\Sound\\ToCreateChr.mp3"
#define MP3_TOSELECTCHAR					".\\Sound\\ToSelectChr.mp3"
#define MP3_STARTGAME						".\\Sound\\StartGame.mp3"

#endif // _CHRSEL_DEFINE_H