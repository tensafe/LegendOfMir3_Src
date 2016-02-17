// LoginProcess.h: interface for the CCreateChrProcess class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _CREATE_CHR_H
#define _CREATE_CHR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CCreateChr : public CBMWnd 
{
	/////////////////////////////////////////////////////////////////////////////////////////
	// Functions
public:
	CCreateChr();
	~CCreateChr();

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
	/////////////////////////////////////////////////////////////////////////////////////////
	// Variable
public:
private:
	CMirButton			m_xButtons[4];
	BOOL				m_fIsButtonDown;
protected:
	CWHWilImageData		*m_pxImage;
	INT					m_nSelectGender;
	INT					m_nSelectJob;
};
#endif //_CREATE_CHR_H
