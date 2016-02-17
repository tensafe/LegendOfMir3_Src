// LoginProcess.h: interface for the CNewAccountProcess class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _NEW_ACCOUNT_H
#define _NEW_ACCOUNT_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CNewAccount : public CBMWnd 
{
	/////////////////////////////////////////////////////////////////////////////////////////
	// Functions
public:
	CNewAccount();
	~CNewAccount();

	virtual VOID		Create(CWHWilImageData* pxImage);

	virtual HRESULT		OnButtonDown(WPARAM wParam, LPARAM lParam);
	virtual HRESULT		OnButtonDown(POINT ptMouse);
	virtual HRESULT		OnButtonUp(WPARAM wParam, LPARAM lParam);
	virtual HRESULT		OnButtonUp(POINT ptMouse);
	virtual HRESULT		OnKeyDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT		OnMouseMove(WPARAM wParam, LPARAM lParam);

	virtual	VOID		Render(INT	nLoopTime);
private:
protected:
	VOID				SetFocusBefore(VOID);
	VOID				SetFocusAfter(VOID);
	/////////////////////////////////////////////////////////////////////////////////////////
	// Variable
public:
private:
	INPUTSTATE			m_xInputState[2];
	_LOGINIMAGEINFO		m_xInputImg[4];			// 이미지 삭제 농후
	CMirButton			m_xButtons[3];
	INT					m_nUserState;
	BOOL				m_fIsButtonDown;
protected:
	CWHWilImageData		*m_pxImage;
	CMirButton			*m_pBtn;
};
#endif //_NEW_ACCOUNT_H
