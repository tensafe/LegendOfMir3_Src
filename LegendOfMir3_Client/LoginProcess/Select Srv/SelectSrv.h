// LoginProcess.h: interface for the CLoginProcess class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SELECT_SERVER_H
#define _SELECT_SERVER_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSelectSrv : public CBMWnd 
{
	/////////////////////////////////////////////////////////////////////////////////////////
	// Functions
public:
	CSelectSrv();
	~CSelectSrv();

	virtual VOID		Create(CWHWilImageData* pxImage,CPDLList<ServerList>* pxSList);

	virtual HRESULT		OnButtonDown(WPARAM wParam, LPARAM lParam);
	virtual HRESULT		OnButtonDown(POINT ptMouse);
	virtual HRESULT		OnButtonUp(WPARAM wParam, LPARAM lParam);
	virtual HRESULT		OnButtonUp(POINT ptMouse);
	virtual HRESULT		OnKeyDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT		OnMouseMove(WPARAM wParam, LPARAM lParam);

	virtual	VOID		Render(INT	nLoopTime);
	VOID				Draw3DBorder(RECT rcWnd, COLORREF dwColor, INT nAlpha);
private:
protected:
	/////////////////////////////////////////////////////////////////////////////////////////
	// Variable
public:
private:
	BOOL					m_fIsButtonDown;
protected:
	CWHWilImageData			*m_pxImage;
	_PPSERVERBUTTON			m_ppxSrvBtn;
	INT						m_nSrvCount;
	D3DVERTEX				m_avBillBoard[4];
};
#endif //_SELECT_SERVER_H
