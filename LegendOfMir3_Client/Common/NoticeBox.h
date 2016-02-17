#ifndef _NOTICEBOX_H_
#define _NOTICEBOX_H_
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
// CMirMsgBox.h
//////////////////////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////////////////////
class	CNoticeBox
{
public:
	CNoticeBox();	// Constructor
	~CNoticeBox();	// Destructor

	// Function
	BOOL	IsActive(VOID){return m_bActive;}
	BOOL	IsInited(VOID){return m_bSet;}

	BOOL	StringDivideLen(INT nDivideLen, INT& nDividedLine, CHAR* szSrc,CDLList<CHAR*>* m_pxpStr);

	virtual BOOL	Load(CWHWilImageData* pxBtnImage);
	virtual BOOL	SetBoxImage(INT nImageIndex);
	virtual BOOL	SetMsg(CHAR* szMsg);
	virtual BOOL	ShowMessage(CHAR* szMsg);
	virtual BOOL	ShowMessageBox(CHAR* szMsg,INT nImgIndex);		// Show	Message box with Initialization	
	virtual BOOL	HideMessageBox(VOID);							// Hide Message box
	virtual BOOL	DestoryMessageBox(VOID);						// DestoryMessage box
	
	// Message Process
	virtual HRESULT	OnButtonDown(POINT ptMouse);					// Button Down
	virtual HRESULT	OnButtonUp(POINT ptMouse);						// Button Up
			VOID	MoveWnd(POINT ptMouse);							// Move Window
		
	// Render Process
	virtual BOOL	RenderMessageBox(INT nLoopTIme);				// Render Function

private:
	RECT	m_rcWnd;
	POINT	m_bGrabbedMousePos;
	BOOL	m_bActive;			// Show or Hide ??
	BOOL	m_bInited;			// Initialized ?
	BOOL	m_bSet;				// Set ?
	CHAR	m_szMsg[8196];
	WORD*	m_wpData;
	SHORT	m_shWidth;
	SHORT	m_shHeight;
	BOOL	m_bIsSelected;

	CDLList<CHAR*>		m_xMsg;		// 출력될 문자
	CGameBtn			m_xMsgBtn;
	CWHWilImageData*	m_pxBtnImage;
protected:
	__inline BOOL	IsInRect(int nPosX, int nPosY, RECT rc)	// 정확하게 Rect 안해 있는가를 체크
	{	if (nPosX >= rc.left && nPosX <= rc.right && nPosY >= rc.top && nPosY <= rc.bottom)
			return TRUE;
		else 
			return FALSE;
	}
};

#endif  _NOTICEBOX_H_