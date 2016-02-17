#ifndef _CGUILDWND_H
#define _CGUILDWND_H


class CGuildWnd : public CGameWnd
{ 
private:
protected:
	CGameBtn				m_xGuildBtn[5];

public:
	CGuildWnd();
	~CGuildWnd();

	virtual VOID Init();
	virtual VOID Destroy();

	VOID	CreateGuildWnd(INT nID, CWHWilImageData* pxWndImage, INT nFrameImgIdx, INT nStartX, INT nStartY, INT nWidth, INT nHeight, BOOL bCanMove);
	VOID	ShowGuildWnd();
public:
//  메시지 함수.
	BOOL	OnLButtonDown(POINT ptMouse);
	BOOL	OnLButtonUp(POINT ptMouse);
	VOID	OnMouseMove(POINT ptMouse);

	VOID	OnScrollDown();
	VOID	OnScrollUp();
};


#endif // _CGUILDWND_H