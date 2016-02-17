#ifndef _CHORSEWND_H
#define _CHORSEWND_H


class CHorseWnd : public CGameWnd
{ 
private:
protected:
	CGameBtn				m_xHorseBtn;

public:
	CHorseWnd();
	~CHorseWnd();

	virtual VOID Init();
	virtual VOID Destroy();

	VOID	CreateHorseWnd(INT nID, CWHWilImageData* pxWndImage, INT nFrameImgIdx, INT nStartX, INT nStartY, INT nWidth, INT nHeight, BOOL bCanMove);
	VOID	ShowHorseWnd();

	VOID	SetStatusBtnInit();

public:
//  메시지 함수.
	BOOL	OnLButtonDown(POINT ptMouse);
	BOOL	OnLButtonUp(POINT ptMouse);
	VOID	OnMouseMove(POINT ptMouse);

	VOID	OnScrollDown();
	VOID	OnScrollUp();
};


#endif // _CHORSEWND_H