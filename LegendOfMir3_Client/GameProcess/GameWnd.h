#ifndef _CGAMEWND_H
#define _CGAMEWND_H


class CGameWnd
{ 
public:
	INT				m_nID;					// 윈도우 관리자가 이윈도우를 구분할 식별자.
	RECT			m_rcImgWnd;				// 윈도우의 이미지의영역. 
	RECT			m_rcWnd;				// 윈도우의 화면영역. 

	INT				 m_nFrameImgIdx;		// 이미지 관련 변수들. 
	CWHWilImageData* m_pxWndImage;

	BOOL			m_bActive;				// 윈도우 활성화 여부. 
	BOOL			m_bFocused; 
	BOOL			m_bMoving;				// 윈도우 움직이는지의 여부.
	BOOL			m_bCanMove;				// 윈도우 고정여부.

	INT				m_nWidth;
	INT				m_nHeight;

	POINT			m_bGrabbedMousePos;
	// 3D테스트
	D3DVERTEX		m_avBoard[4];

public:
	__inline RECT GetGameWndRect()
	{
		return m_rcWnd;
	}
	__inline BOOL GetGameWndActive()
	{
		return m_bActive;
	}

	__inline CGameWnd()	
	{ 
		Init();
	}

	~CGameWnd()
	{
		Destroy();
	}

	virtual VOID Init();
	virtual VOID Destroy();
	virtual VOID ShowGameWnd();


	VOID ResetWndImg(INT nIdx);
	VOID CreateGameWnd(INT nID, CWHWilImageData* pxWndImage, INT nFrameImgIdx, BOOL bCanMove, INT nStartX, INT nStartY, INT nWidth = 0, INT nHeight = 0);
	VOID DestroyGameWnd();
	VOID SetGameWndActive(BOOL bActive);
	VOID SetGameWndFocused(BOOL bFocused);
	VOID SetReadyMove(POINT ptMouse);
	VOID MoveGameWnd(POINT ptMouse);
};


#endif // _CGAMEWND_H