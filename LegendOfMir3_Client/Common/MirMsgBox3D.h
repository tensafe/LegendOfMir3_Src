#ifndef _MIRMSGBOX3D_H_
#define _MIRMSGBOX3D_H_

class	CMirMsgBox3D : public CMirMsgBox
{
public:
	CMirMsgBox3D();
	~CMirMsgBox3D();

	virtual BOOL	Load(CWHWilImageData* pxBtnImage);
	virtual BOOL	DestoryMessageBox(VOID);								// DestoryMessage box

	BOOL			CreateWnd(CWHWilImageData* pxWndImage, BOOL bCanMove, INT nStartX, INT nStartY, INT nWidth, INT nHeight);

	virtual	BOOL	ShowMessageBox(CHAR* szMgs,INT nType,INT nImgIndex);	// Show	Message box with Initialization
	virtual BOOL	HideMessageBox(VOID);									// Hide Message box

	virtual BOOL	OnKeyDown(WPARAM wParam,LPARAM lParam);

	virtual HRESULT	OnButtonDown(POINT ptMouse);		// Button Down
	virtual HRESULT	OnButtonUp(POINT ptMouse);		// Button Up

	VOID			MoveWnd(POINT ptMouse);

	// Render
	virtual BOOL	RenderMessageBox(INT nLoopTIme);
	BOOL			RenderBox(VOID);
private:
	INT						m_nID;
	INT						m_nFrameImgIdx;
	CWHWilImageData*		m_pxWndImage;
	BOOL					m_bCanMove;
	BOOL					m_bFocused;
	POINT					m_bGrabbedMousePos;
	// 3DÅ×½ºÆ®
	D3DVERTEX		m_avMagic[4];
	RECT			m_rcWnd;
	RECT			m_rcImgWnd;
	INT				m_nCurrImageIdx;

	INT				m_nWidth;
	INT				m_nHeight;
protected:
};

#endif _MIRMSGBOX3D_H_
