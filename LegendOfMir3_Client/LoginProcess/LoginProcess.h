// LoginProcess.h: interface for the CLoginProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGINPROCESS_H__4CCDFADE_D80A_4B06_8551_CCA6882693B1__INCLUDED_)
#define AFX_LOGINPROCESS_H__4CCDFADE_D80A_4B06_8551_CCA6882693B1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CLoginProcess : public CWHDefProcess  
{
public:
	CWHWilImageData		m_Image;
	CWHWilImageData		m_Image2;

///////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////
// Public Functions
	CLoginProcess();
	virtual				~CLoginProcess();

	VOID				Init(VOID);
	void				Load(void);	
	VOID				DeleteProc(VOID);
	VOID				SetNextProc(VOID);
	VOID				ReadIniFileData(VOID);
	virtual LRESULT		DefMainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT				OnWindowMove(WPARAM wParam,LPARAM lParam);
	LRESULT				OnLButtonDown(WPARAM wParam, LPARAM lParam);
	LRESULT				OnLButtonUp(WPARAM wParam, LPARAM lParam);
	LRESULT				OnMouseMove(WPARAM wParam, LPARAM lParam);
	LRESULT				OnButtonClick(WPARAM wParam,LPARAM lParam);
	LRESULT				OnMouseOn(WPARAM wParam,LPARAM lParam);
	LRESULT				OnKeyDown(WPARAM wParam,LPARAM lParam);
	LRESULT				OnEnterPressed(WPARAM wParam,LPARAM lParam);

	VOID				RenderIntro(INT nLoopTime);
	VOID				RenderScene(INT nLoopTime);
	VOID				RenderScroll(INT nLoopTime);
	VOID				RenderNewAccount(INT nLoopTime);
	VOID				RenderPatch(INT nLoopTime);
	VOID				RenderPassword(INT nLoopTime);
	VOID				RenderNewAccountHelpTxt(VOID);

	virtual VOID		OnConnectToServer();
	virtual char*		OnMessageReceive(CHAR* pszMessage);

private:
	CLoginAvi			m_xAvi;					// Avi Degree
	CLogin				m_xLogin;				// Login Degree
	CSelectSrv			m_xSelectSrv;			// Select Server Degree

	int					m_nAnimationCounter;
	int					m_nServerPort;
	int					m_UserState;			
	int					m_Progress;
	CMirButton			m_pBtn[6];									// LOGIN BUTTON

	char				m_szServerIP[15];
	char				m_szErrorMsg[512];
	BOOL				m_fIsConnected;
	BOOL				m_fIsLButtonClicked;
	HBRUSH				m_hBrush;

	POINT				m_MousePos;
	UINT				m_ButtonID;
	SOCKADDR_IN			m_LoginAddress;

	CPDLList<ServerList>  m_SList;
	CPatch				*pPatch;
	CMirMsgBox3D		m_xMsgBox;

	VOID				SetErrorMessage(INT nMsg,CHAR* szMgs);			// Error Message를 보낸다.
	VOID				SetFocusBefore(VOID);
	VOID				SetFocusAfter(VOID);

	BOOL				CheckSSNO(char* szSSNo);
	BOOL				DateCehck(char* szDate);
	BOOL				IsLeapYear(int nYear);
	BOOL				CheckEMail(char* szEMail);

	void				OnSocketMessageRecieve(char *szMsg);

	__inline BOOL		IsInRect(int nPosX, int nPosY, RECT rc)
	{	if (nPosX >= rc.left && nPosX <= rc.right && 
			nPosY >= rc.top && nPosY <= rc.bottom) return TRUE;
		else return FALSE;	}

	BOOL				StringDivideLen(INT nDivideLen, INT& nDividedLine, CHAR* szSrc,CDLList<CHAR*>* m_pxpStr);
};
#endif // !defined(AFX_LOGINPROCESS_H__4CCDFADE_D80A_4B06_8551_CCA6882693B1__INCLUDED_)

//	m_Progress  //
// 0: Login
// 1: New Account
// 2: Patch
// 3: Change Password
// 4: Server Selection
// 5: Character Selection
// 6: Create New Character
// 7: Play Game
// 88: Need Account change
// 99: Quit for Some reason


//	m_UserState	//
// 0: None.
// 1: Input ID.
// 2: Input PassWord.
// 3: Click Button.
// 4: Etc.

//	m_ShowMMsgBox	//
// 0 : No Error Messgae
// 1 : Not Exist Account
// 2 : Incorrect Password