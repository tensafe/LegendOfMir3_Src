// CharacterProcess.h: interface for the CCharacterProcess class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CHARACTERPROCESS_H__70098477_5700_4CC3_88C4_5E7460A1F63A__INCLUDED_)
#define AFX_CHARACTERPROCESS_H__70098477_5700_4CC3_88C4_5E7460A1F63A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000



//#define ANITICK				200


class CCharacterProcess: public CWHDefProcess  
{
public:
						CCharacterProcess();
	virtual				~CCharacterProcess();
	VOID				Init();
	VOID				Load();

// Message Function
	LRESULT				OnKeyDown(WPARAM wParam, LPARAM lParam);
	LRESULT				OnLButtonDown(WPARAM wParam, LPARAM lParam);
	LRESULT				OnLButtonUp(WPARAM wParam, LPARAM lParam);
	LRESULT				OnSocketMessage(WPARAM wParam,LPARAM lParam);
	LRESULT				OnMouseOn(WPARAM wParam,LPARAM lParam);
	LRESULT				OnWindowMove(WPARAM wParam,LPARAM lParam);
	LRESULT				OnMouseMove(WPARAM wParam,LPARAM lParam);

	void				OnSocketMessageRecieve(char* pszMsg);

	virtual VOID		OnConnectToServer();
	virtual char*		OnMessageReceive(CHAR* pszMessage);

	virtual LRESULT DefMainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

// Socket
	BOOL				ConnectServer(SOCKET &s, SOCKADDR_IN* addr, UINT nMsgID, LPCTSTR lpServerIP, DWORD dwIP, int nPort, long lEvent);

// Process control
	VOID				DeleteProc();
	VOID				SetNextProc();

// Scene Renders
	VOID				RenderScene(INT nLoopTime);
	VOID				RenderCharCreate(INT nLoopTime);
	VOID				SetErrorMessage(INT nMsg,CHAR* szMgs);
	virtual VOID		ShowStatus(INT nStartX, INT nStartY);
	BOOL				MessageBoxProcess(LPARAM lParam);
	
private:
	CBackGround			m_xBGround;
	CSelectChr			m_xSelectChr;
	char				m_ServerIP[15];
	WSADATA				m_wsd;
	CWHWilImageData		m_Image;
	CWHWilImageData		m_Image2;
	CWHWilImageData		m_tImage1;	// 임시
	int					m_Progress;
	SOCKET 				m_LoginSocket;
	SOCKADDR_IN			m_LoginAddress;
	UINT				m_ButtonID;
	POINT				m_ptMousePos;
	int					m_ConnectionState;		// 0: not login.
												// 1: login success.
												// 2: login fail etc.
	BOOL				m_fIsLButtonClicked;

	CPDLList<CHRINFO>	m_CList;				// 케릭터 Info 저장 List
	CPDLList<CHRRECT>	m_CRectList;			// 케릭터 Rect 저장 List
	// Buttons
	CMirButton			*m_pBtn;
	INT					m_nMessage;				// Error Message Box Index
	BYTE				m_SelectJob;			// 생성창에서 선택된 직업
	BYTE				m_SelectSex;			// 생성창에서 선택된 성별
	INT					m_SelectChr;			// 선책창에서 선택된 케릭터 번호
	INT					m_ChrCount;				// 쿼리로 받어온 케릭터 숫자
	char				m_szChrName[16];
	CHAR				m_szErrorMsg[512];
	int					m_Distance;
	INT					m_Tick;
	POINT				m_PosName;
	CMirMsgBox3D		m_xMsgBox;
/*	CAvi				m_xCreateChrAvi;					// For Create Charector
	CAvi				m_xStartGameAvi;					// For Create Start Game*/
}; 

#endif // !defined(AFX_CHARACTERPROCESS_H__70098477_5700_4CC3_88C4_5E7460A1F63A__INCLUDED_)
