#ifndef _CQUESTWND_H
#define _CQUESTWND_H


class CQuestWnd : public CGameWnd
{ 
private:
protected:
	CGameBtn				m_xQuestBtn;

public:
	CQuestWnd();
	~CQuestWnd();

	virtual VOID Init();
	virtual VOID Destroy();

	VOID	CreateQuestWnd(INT nID, CWHWilImageData* pxWndImage, INT nFrameImgIdx, INT nStartX, INT nStartY, INT nWidth, INT nHeight, BOOL bCanMove);
	VOID	ShowQuestWnd();

	VOID	SetStatusBtnInit();

public:
//  메시지 함수.
	BOOL	OnLButtonDown(POINT ptMouse);
	BOOL	OnLButtonUp(POINT ptMouse);
	VOID	OnMouseMove(POINT ptMouse);

	VOID	OnScrollDown();
	VOID	OnScrollUp();
};


#endif // _CQUESTWND_H