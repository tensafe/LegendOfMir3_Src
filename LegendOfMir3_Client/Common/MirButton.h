// MirButton.h: interface for the CMirButton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MIRBUTTON_H__45F351B2_5B11_4EB4_B067_1EE418C2530A__INCLUDED_)
#define AFX_MIRBUTTON_H__45F351B2_5B11_4EB4_B067_1EE418C2530A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct tagBUTTONINFO
{
	UINT	nID;
	LONG	left; 
	LONG	top; 
	LONG	right; 
	LONG	bottom;
	UINT	nState;
} BUTTONINFO;

#define BUTTON_STATE_UP			1// 0
#define BUTTON_STATE_ON			2// 1
#define BUTTON_STATE_DOWN		3// 2

class CMirButton  
{
public:
	CMirButton();
	CMirButton(HWND hWnd, BUTTONINFO* lpButtonInfo);
	~CMirButton();

	BOOL	CheckMouseOn(int nPosX, int nPosY);
	BOOL	CheckClick(POINT Pos);
	VOID	SetBtn(HWND hWnd, BUTTONINFO* lpButtonInfo);
	VOID	SetBtn(BUTTONINFO* lpButtonInfo);
	VOID	ChangeBtnRect(RECT rc);

	VOID	OnLButtonDown(POINT	xPoint);
	VOID	OnMouseMove(POINT xPoint);
	VOID	OnLButtonUp(POINT xPoint);
public:
	__inline BOOL	IsInRect(int nPosX, int nPosY)
	{	if (nPosX >= m_Rect.left && nPosX <= m_Rect.right && 
			nPosY >= m_Rect.top && nPosY <= m_Rect.bottom) return TRUE;
		else return FALSE;	}

	INT		m_nButtonID;
	HWND	m_hParentWnd;
	RECT	m_Rect;
	UINT	m_nState;
};

#endif // !defined(AFX_MIRBUTTON_H__45F351B2_5B11_4EB4_B067_1EE418C2530A__INCLUDED_)
