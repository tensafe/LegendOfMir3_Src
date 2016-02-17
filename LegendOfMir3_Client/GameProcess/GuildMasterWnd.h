#ifndef _CGUILDMASTERWND_H
#define _CGUILDMASTERWND_H


class CGuildMasterWnd : public CGameWnd
{ 
private:
protected:
	CGameBtn				m_xGuildMasterBtn;

public:
	CGuildMasterWnd();
	~CGuildMasterWnd();

	virtual VOID Init();
	virtual VOID Destroy();

	VOID	CreateGuildMasterWnd(INT nID, CWHWilImageData* pxWndImage, INT nFrameImgIdx, INT nStartX, INT nStartY, INT nWidth, INT nHeight, BOOL bCanMove);
	VOID	ShowGuildMasterWnd();

	VOID	SetStatusBtnInit();

public:
//  메시지 함수.
	BOOL	OnLButtonDown(POINT ptMouse);
	BOOL	OnLButtonUp(POINT ptMouse);
	VOID	OnMouseMove(POINT ptMouse);

	VOID	OnScrollDown();
	VOID	OnScrollUp();
};


#endif // _CGUILDMASTERWND_H