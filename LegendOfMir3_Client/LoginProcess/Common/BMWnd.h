#ifndef	_BMWND_H
#define _BMWND_H


class CBMWnd
{
	/////////////////////////////////////////////////////////////////////////////////////////
	// Functions
public:
	CBMWnd();
	~CBMWnd();
	
	VOID	SetFocused(VOID);
	
	virtual VOID	OpenWnd(VOID);
	virtual VOID	CloseWnd(VOID);

	VOID	SetWndRect(RECT rc)		{m_rcWnd = rc;}
	BOOL	IsWndActive(VOID)		{return m_fIsActive;}
	BOOL	IsWndFocused(VOID)		{return m_fIsFocused;}
	
	virtual HRESULT	OnButtonDown(WPARAM wParam, LPARAM lParam);
	virtual HRESULT OnButtonDown(POINT ptMouse);
	virtual HRESULT	OnButtonUp(WPARAM wParam, LPARAM lParam);
	virtual HRESULT OnButtonUp(POINT ptMouse);
	virtual HRESULT OnKeyDown(WPARAM wParam, LPARAM lParam);
	virtual LRESULT	OnMouseMove(WPARAM wParam, LPARAM lParam);
	
	virtual	VOID	Render(INT	nLoopsTime);
private:
protected:
	__inline	BOOL	IsInRect(RECT rc,INT nX,INT nY)
	{
		if((rc.left > nX) || (rc.right < nX) || (rc.top > nY) || (rc.bottom < nY) ) return FALSE;
		return TRUE;
	}
	__inline	BOOL	IsInRect(RECT rc,POINT pt) 
	{
		return IsInRect(rc, pt.x, pt.y);
	}
	
	/////////////////////////////////////////////////////////////////////////////////////////
	// Variable
public:
private:
protected:
	BOOL	m_fIsActive;
	BOOL	m_fIsFocused;
	RECT	m_rcWnd;
};

#endif //_BMWND_H