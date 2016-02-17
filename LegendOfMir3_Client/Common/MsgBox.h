#ifndef _MSGBOX_H_
#define _MSGBOX_H_
//////////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////
// CMsgBox.h
//////////////////////////////////////////////////////////////////////////////////////////
//
//
//
//////////////////////////////////////////////////////////////////////////////////////////

#define CLICK_NONE			0
#define CLICK_OK			1
#define CLICK_YES			2
#define CLICK_NO			4
#define CLICK_CANCLE		8
#define HAVE_EDITOBOX		128

#define MSG_BTN_OK				1
#define MSG_BTN_YESNO			2
#define MSG_BTN_YESNOCANCLE		4
#define MSG_EDITEXIST			128

#define ASK_MAKE_BIG_GUILD			1		// 설립할 문파의 이름을 적으십시오.
#define ASK_MAKE_SMALL_GUILD		2		// 설립할 방파의 이름을 적으십시오.
#define ASK_OTHERSNAME				3		// 상대 문파명을 입려가세요.
#define ASK_DELMEMBER_NAME			4		// 문원에서 제외할 사람의 이름을 입력하십시오.
#define ASK_ADDMEMBER_NAME			5		// 문원으로 추가할 사람의 이름을 입력하십시오.
#define ASK_DROP_GOLD				6		// 내려 놓을 금액을 입력하세요.
#define ASK_DEAL_GOLD				7		// 거래할 금액을 입력하세요.
#define ASK_GROUP_ADD_MEMBER_NAME	8		// 그룹에 참여할 사람의 이름을 적으십시오.
#define ASK_GROUP_DEL_MEMBER_NAME	9		// 그룹에서 빠질 사람의 이름을 적으십시오.
#define	ASK_GUILD_BREAK_ALLY		10		// 동맹을 파기 할 문파의 이름을 입력하십시오.
#define	ASK_YN_LOGOUT_GAME			11		// 로그아웃 하시겠습니까?
#define ASK_YN_QUIT_GAME			12		// 미르의전설2를 종료하시겠습니까?
#define	ASK_YN_GUILD_ALLY			13		// 동맹을 하기 위해서는 상대방 문파가 [동맹가능] 상태 이어야 하며 상대 문주와 마주고보 있어야 합니다.
#define	ASK_YN_GAIN_SKILL			14		// 을(를) 익히시겠습니까?
#define	ASK_YN_USE_ITEM				15		// 을(를) 사용하시겠습니까?
#define ASK_WITHDRAWAL				16		// 찾고자 하는 금액을 입려하세요.
#define ASK_RECEIPT					17		// 맡기고자하는 금액을 입력하세요.


class	CMsgBox
{
	//////////////////////////////
	// Function & Procedure
	//////////////////////////////
public:
	CMsgBox();			// Constructor
	~CMsgBox();			// Destructor
	// Function
	virtual BOOL	SetMsgBox(CHAR* szMsg,INT nType);				// Init Messages
	virtual BOOL	ShowMessageBox(VOID);							// Show Message box with Initialized
	virtual BOOL	ShowMessageBox(CHAR* szMsg,INT nType=0);		// Show	Message box with Initialization
	virtual BOOL	HideMessageBox(VOID);							// Hide Message box
	virtual BOOL	DestoryMessageBox(VOID);						// DestoryMessage box

	// Information
	BOOL	IsActive(VOID);
	BOOL	IsInited(VOID);

	// Message Process
	virtual HRESULT	OnButtonDown(LPARAM lParam, WPARAM wParam);		// Button Down
	virtual HRESULT	OnButtonUp(LPARAM lParam,WPARAM wParam);		// Button Up

	// Render Process
	virtual BOOL	RenderMessageBox(INT nLoopTIme);				// Render Function
private:
protected:
	__inline BOOL	IsInRect(int nPosX, int nPosY, RECT rc)	// 정확하게 Rect 안해 있는가를 체크
	{	if (nPosX >= rc.left && nPosX <= rc.right && nPosY >= rc.top && nPosY <= rc.bottom)
			return TRUE;
		else 
			return FALSE;
	}
public:
	CHAR	m_szMsg[MAX_PATH];			// 출력할 문자
private:
protected:
	INT		m_nType;			// 0: OK(확인) ,1 : YES/NO ,2 : YES/NO/Cancle ,3 : EditBox & OK(확인) ...
	BOOL	m_bActive;			// Show or Hide ??
	BOOL	m_bInited;			// Initialized ?
	BOOL	m_bSet;				// Set ?
};

#endif  _MSGBOX_H_