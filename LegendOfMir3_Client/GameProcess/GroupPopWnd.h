#ifndef _CGROUPPOPWND_H
#define _CGROUPPOPWND_H


class CGroupPopWnd : public CGameWnd
{ 
private:
protected:
	CGameBtn				m_xGroupPopBtn[5];

public:
	CGroupPopWnd();
	~CGroupPopWnd();
	VOID	CreateGroupPopWnd(INT nID, CWHWilImageData* pxWndImage, INT nFrameImgIdx, INT nStartX, INT nStartY, INT nWidth, INT nHeight, BOOL bCanMove);
	VOID	ShowGroupPopWnd();

	virtual VOID Init();
	virtual VOID Destroy();

public:
//  메시지 함수.
	BOOL	OnLButtonDown(POINT ptMouse);
	BOOL	OnLButtonUp(POINT ptMouse);
	VOID	OnMouseMove(POINT ptMouse);

	VOID	OnScrollDown();
	VOID	OnScrollUp();
};


#endif // _CGROUPPOPWND_H
