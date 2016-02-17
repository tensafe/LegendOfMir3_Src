/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/


#ifndef _CHATEDITBOX_H
#define _CHATEDITBOX_H


class CChatEdit
{
//1: Constructor/Destructor
public:
	CChatEdit();
	~CChatEdit();
protected:
	HWND	m_hChatEdit;
	HWND	m_hMainWindow;

	HFONT	m_hFontChatEdit;
	DWORD	m_dwFontColor;

	WNDPROC	m_WndProcChatEdit;

public:
	CHAR	m_szInputMsg[MAX_PATH];


	__inline	HWND GetSafehWnd()	{ return m_hChatEdit; }
	BOOL		Create(HINSTANCE hInstance, HWND hWndParent, INT nX, INT nY, INT nXsize, INT nYSize);
	BOOL		DestroyDialog();
	BOOL		SetLimitText(INT nLimitText);
	__inline	VOID SetSelectAll() { SendMessage(m_hChatEdit, EM_SETSEL, 0, (LPARAM)-1); }
public:
//4: Message Map
	virtual	LRESULT CALLBACK ChatEditProc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
public:
};


#endif // _CHATEDITBOX
