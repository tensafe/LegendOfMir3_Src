// LoginProcess.h: interface for the CSelectChrProcess class.
//
//////////////////////////////////////////////////////////////////////

#ifndef _SELECT_CHR_H
#define _SELECT_CHR_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CSelectChr : public CBMWnd 
{
	/////////////////////////////////////////////////////////////////////////////////////////
	// Functions
public:
	CSelectChr();
	~CSelectChr();

	virtual VOID		Create(CWHWilImageData* pxImage,CPDLList<CHRINFO>* pxList, CPDLList<CHRRECT>* pxrcList,INT nChrCount);

	virtual HRESULT		OnButtonDown(WPARAM wParam, LPARAM lParam);
	virtual HRESULT		OnButtonDown(POINT ptMouse);
	virtual HRESULT		OnButtonUp(WPARAM wParam, LPARAM lParam);
	virtual HRESULT		OnButtonUp(POINT ptMouse);
	virtual HRESULT		OnKeyDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT		OnMouseMove(WPARAM wParam, LPARAM lParam);
	virtual VOID		OpenWnd(VOID);
	virtual VOID		CloseWnd(VOID);

	VOID				ShowChrInfo(VOID);
	virtual	VOID		Render(INT	nLoopTime);
private:
protected:
	/////////////////////////////////////////////////////////////////////////////////////////
	// Variable
public:
	INT					m_nRenderState;
	INT					m_nSelectChr;
private:
protected:
	CLightFog			m_xLightFog;
	CPDLList<CHRINFO>*	m_pxSList;
	CPDLList<CHRRECT>*	m_pxrcList;
	CWHWilImageData		*m_pxImage;
	CMirButton			**m_pBtn;
	POINT				m_PosName;
	BOOL				m_fIsButtonDown;
	INT					m_nChrCount;
	INT					m_nAnimationCount;
	INT					m_nBlend;
	INT					m_nRoll;
};
#endif //_SELECT_CHR_H
