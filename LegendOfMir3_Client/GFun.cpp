#include "StdAfx.h"


DWORD GetChatColor(BYTE bIdx)
{
	DWORD dwClr = 0;

	switch ( bIdx )
	{
	case _CHAT_COLOR1:		dwClr = RGB(  0,   0,   0);		break;
	case _CHAT_COLOR2:		dwClr = RGB( 10,  10,  10);		break;
	case _CHAT_COLOR3:		dwClr = RGB(255, 255, 255);		break;
	case _CHAT_COLOR4:		dwClr = RGB(255,   0,   0);		break;
	case _CHAT_COLOR5:		dwClr = RGB(  0, 255,   0);		break;
	case _CHAT_COLOR6:		dwClr = RGB(  0,   0, 255);		break;
	case _CHAT_COLOR7:		dwClr = RGB(255, 255,   0);		break;
	case _CHAT_COLOR8:		dwClr = RGB(255, 128,   0);		break;
	}

	return dwClr;
}


DWORD GetUserNameColor(BYTE bIdx)
{
	DWORD dwClr = 0;

	switch ( bIdx )
	{
	case _USER_NAMECLR1:		dwClr = RGB(255, 255, 255);		break;	// 255
	case _USER_NAMECLR2:		dwClr = RGB(255, 255,   0);		break;	// 251
	case _USER_NAMECLR3:		dwClr = RGB(255,   0,   0);		break;	// 249
	case _USER_NAMECLR4:		dwClr = RGB(255,   0,   0);		break;	// 125
	case _USER_NAMECLR5:		dwClr = RGB(165,  99,  57);		break;	// 47
	case _USER_NAMECLR6:		dwClr = RGB(  0,   0, 255);		break;	// 180
	case _USER_NAMECLR7:		dwClr = RGB(239, 107,   0);		break;	// 69
	case _USER_NAMECLR8:		dwClr = RGB(  0, 148,   0);		break;	// 221
	case _USER_NAMECLR9:		dwClr = RGB(  0, 255, 255);		break;	// 254
	case _USER_NAMECLR10:		dwClr = RGB(140, 214, 239);		break;	// 147
	case _USER_NAMECLR11:		dwClr = RGB( 57, 181, 239);		break;	// 154
	case _USER_NAMECLR12:		dwClr = RGB(119, 136, 255);		break;	// 229
	case _USER_NAMECLR13:		dwClr = RGB(  0, 123, 222);		break;	// 168
	case _USER_NAMECLR14:		dwClr = RGB(  0,   0, 255);		break;	// 252
	}

	return dwClr;
}
