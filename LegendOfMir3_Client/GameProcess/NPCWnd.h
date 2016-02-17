#ifndef _CNPCWnd_H
#define _CNPCWnd_H

/*using namespace std;

*/
class CNPCWnd : public CGameWnd
{ 
public:
	INT						m_nNpcId;

protected:
	CWHWilImageData			m_xNPCImage;
	int						m_nStartLine;
	int						m_nMaxLine;

public:
	CNPCWnd();
	~CNPCWnd();
	VOID	CreateNPCChatWnd(INT nID, CWHWilImageData* pxWndImage, INT nFrameImgIdx, INT nStartX, INT nStartY, INT nWidth, INT nHeight, BOOL bCanMove);
	VOID	ShowNPCChatWnd();

	__inline void	SetNPC(int nNPCID) { m_nNpcId = nNPCID; }
	void	ResetDialog();

public:
	VOID SetNpcEditBoxPos(VOID);
	VOID SetWndRectReduce(VOID);
	VOID SetWndRectExtend(VOID);
//  메시지 함수.
	BOOL	OnLButtonDown(POINT ptMouse);
	BOOL	OnLButtonUp(POINT ptMouse);
	VOID	OnMouseMove(POINT ptMouse);
	BOOL	OnMsgInputted(VOID);

	VOID	OnScrollDown();
	VOID	OnScrollUp();
	VOID	OnEnterKeyDown(VOID);

private:
	// Command Click 처리
	BOOL	ClickCommandProcess(CMTxtBtn* ClickedBtn);		// 아래 에 있는 것들 종합
	BOOL	SendCommandToSrv(CHAR* szMsg);
	INT		m_nNpcMsgState;
	CMTxtBtn* m_pClickedTxtBtn;
	CMirMsgBox	m_xMsgBox;
/*	ClickPepairCommand(VOID);		// 고치다(@REPAIR)
	ClickSellCommand(VOID);			// 팔다(@SELL)
	ClickBuyCommand(VOID);			// 사다(@BUY)
	ClickBuildGuildCommand(VOID);	// 문파설립(@@BUILDGUILDNOW)
	ClickSpecialRepairCommand(VOID);// 특수수리(@SPECIALREPAIR)
	ClickMakeCommand(VOID);			// 만들다(@MAKE)
*/
};

#endif // _CNPCWnd_H