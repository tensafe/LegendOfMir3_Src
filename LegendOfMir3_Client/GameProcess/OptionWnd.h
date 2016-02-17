#ifndef _COPTIONWND_H
#define _COPTIONWND_H


class COptionWnd : public CGameWnd
{ 
private:
protected:
	CGameBtn				m_xOptionBtn;

public:
	COptionWnd();
	~COptionWnd();

	virtual VOID Init();
	virtual VOID Destroy();

	VOID	CreateOptionWnd(INT nID, CWHWilImageData* pxWndImage, INT nFrameImgIdx, INT nStartX, INT nStartY, INT nWidth, INT nHeight, BOOL bCanMove);
	VOID	ShowOptionWnd();

	VOID	SetStatusBtnInit();

public:
//  메시지 함수.
	BOOL	OnLButtonDown(POINT ptMouse);
	BOOL	OnLButtonUp(POINT ptMouse);
	VOID	OnMouseMove(POINT ptMouse);

	VOID	OnScrollDown();
	VOID	OnScrollUp();
};


#endif // _COPTIONWND_H