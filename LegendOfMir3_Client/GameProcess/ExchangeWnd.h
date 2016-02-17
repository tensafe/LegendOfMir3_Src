#ifndef _CEXCHANGEWND_H
#define _CEXCHANGEWND_H


class CExchangeWnd : public CGameWnd
{ 
private:
protected:
public:
	virtual VOID Init();
	virtual VOID Destroy();

	CGameBtn		m_xExchangeBtn;


	CExchangeWnd();
	~CExchangeWnd();
	VOID	CreateExchangeWnd(INT nID, CWHWilImageData* pxWndImage, INT nFrameImgIdx, INT nStartX, INT nStartY, INT nWidth, INT nHeight, BOOL bCanMove);
	VOID	ShowExchangeWnd();
	VOID	SetStatusBtnInit();
public:
//  메시지 함수.
	BOOL	OnLButtonDown(POINT ptMouse);
	BOOL	OnLButtonUp(POINT ptMouse);
	VOID	OnMouseMove(POINT ptMouse);
};


#endif // _CEXCHANGEWND_H
