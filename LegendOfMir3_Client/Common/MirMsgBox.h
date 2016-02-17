#ifndef _MIRMSGBOX_H_
#define _MIRMSGBOX_H_
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
// CMirMsgBox.h
//////////////////////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////////////////////

class	CMirMsgBox : public CMsgBox
{
	//////////////////////////////
	// Function & Procedure
	//////////////////////////////
public:
	CMirMsgBox();	// Constructor
	~CMirMsgBox();	// Destructor
	// Function
	BOOL	ShowMessageBox(CHAR* szMgs,INT nType,INT nImgIndex);	// Show	Message box with Initialization
	BOOL	ShowMessageBox(CHAR* szMgs,INT nType);	// Show	Message box with Initialization
	BOOL	StringDivideLen(INT nDivideLen, INT& nDividedLine, CHAR* szSrc,CDLList<CHAR*>* m_pxpStr);
	BOOL	HideMessageBox(VOID);									// Hide Message box
	BOOL	DestoryMessageBox(VOID);								// DestoryMessage box
	BOOL	Load(CWHWilImageData* pxBtnImage);
	// Message Process
	virtual HRESULT	OnButtonDown(POINT ptMouse);		// Button Down
	virtual HRESULT	OnButtonUp(POINT ptMouse);		// Button Up

	// Render Process
	virtual BOOL	RenderMessageBox(INT nLoopTIme);							// Render Function
private:
protected:
	BOOL	SetBoxImage(INT nImageIndex,INT nType);
	//////////////////////////////
	// Variables
	//////////////////////////////
public:
private:
protected:
	CDLList<CHAR*>	m_xMsg;		// 출력될 문자
	INT		m_nImgIndex;
	POINT	m_xBoxPos;
	SHORT	m_shWidth;
	SHORT	m_shHeight;
	WORD*	m_wpData;
	INT		m_nStart;
	INT		m_nEnd;
	CWHWilImageData* m_pxBtnImage;
	CMsgBoxBtn	m_xMsgBtn[4];	// 0 : OK  1: YES  2:NO  3: Cancle
};

#endif  _MIRMSGBOX_H_