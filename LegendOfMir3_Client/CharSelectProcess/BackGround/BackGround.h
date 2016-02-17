// LoginProcess.h: interface for the CBackGroundProcess class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _BACK_GROUND_H
#define _BACK_GROUND_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBackGround : public CBMWnd 
{
	/////////////////////////////////////////////////////////////////////////////////////////
	// Functions
public:
	CBackGround();
	~CBackGround();

	virtual VOID		Create(CWHWilImageData* pxImage);

	VOID				SetRenderState(INT nState);
	INT					GetRenderState(VOID);
	VOID				AllStopAvi(VOID);
	virtual HRESULT		OnButtonDown(WPARAM wParam, LPARAM lParam);
	virtual HRESULT		OnButtonDown(POINT ptMouse);
	virtual HRESULT		OnButtonUp(WPARAM wParam, LPARAM lParam);
	virtual HRESULT		OnButtonUp(POINT ptMouse);
	virtual HRESULT		OnKeyDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT		OnMouseMove(WPARAM wParam, LPARAM lParam);

	virtual	VOID			Render(INT	nLoopTime);
private:
protected:
	/////////////////////////////////////////////////////////////////////////////////////////
	// Variable
public:
	CAvi				m_xCreateChrAvi;					// For Create Charector
	CAvi				m_xStartGameAvi;					// For Create Start Game
	INT					m_nRenderState;
private:
protected:
	CWHWilImageData		*m_pxImage;
};
#endif //_BACK_GROUND_H
