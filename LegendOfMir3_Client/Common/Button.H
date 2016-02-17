#ifndef _CTEXTBUTTON_H
#define _CTEXTBUTTON_H

class	CTextButton
{
public:
	VOID ClickFreeAllBtn(VOID);
	INT GetButtonCount(VOID);
	CMTxtBtn* GetButton(INT nIndex);
	CTextButton();												// Constructor
	virtual	~CTextButton();										// Destructor
	BOOL	AddButton(CHAR *pszTxt,RECT xRect,INT nLinePos,CHAR *pszParam,COLORREF nColor,COLORREF cColor);	// Add Button to class
	BOOL	AddButton(CHAR *pszTxt,CHAR *pszTxt2,RECT xRect,INT nLinePos,CHAR *pszParam,COLORREF nColor,COLORREF cColor);	// Add Button to class

	CMTxtBtn*	ClickCheck(POINT xWndPos,POINT	xPos,INT CurrentTopLine);	// xWndPos: 실제 Text 가 찍히는 가장 끝점
	CMTxtBtn*	ButtonDownCheck(POINT xWndPos,POINT xPos,INT CurrentTopLine);
	CMTxtBtn*	ButtonUpCheck(POINT xWndPos,POINT xPos,INT CurrentTopLine);

	BOOL		IsCurrsorOnThisButton(CMTxtBtn* pBtn,POINT xWndPos,POINT xPos,INT CurrentTopLine);

	BOOL InitTxtBtn(INT nMaxLine,POINT nStartPos,INT LineGap);
//	BOOL InitTxtBtn(INT nMaxLine);	
	CPDLList<CMTxtBtn> m_pTxtBtn; 
	BOOL		ReleaseButtons(VOID);	
private:
	BOOL		m_bIsReady;
	RECT		m_rtWndRect;			// 윈도우의 Rect 단, 글자가 표시되는 부분
	INT			m_nTxtHeight;			// Text별 Height
	INT			m_nMaxLine;				// 윈도우에 표시되는 최대 Line 수
	INT			m_nLineGap;
	POINT		m_xStartPos;
	BOOL		m_IsUseSubTxt;
};

#endif _CTEXTBUTTON_H