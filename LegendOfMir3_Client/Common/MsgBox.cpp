//#include "MsgBox.h"
#include "stdafx.h"


CMsgBox::CMsgBox()	// Constructor
{
	m_nType = 0;
	m_bActive = FALSE;
	m_bInited = FALSE;
	m_bSet = FALSE;
}

CMsgBox::~CMsgBox()	// Destructor
{

}
	// Function

/*
#define MSG_BTN_OK				1
#define MSG_BTN_YESNO			2
#define MSG_BTN_YESNOCANCLE		4
#define MSG_EDITEXIST			128
*/
BOOL CMsgBox::SetMsgBox(CHAR* szMsg,INT nType)				// Init Messages
{
	INT nLength;
	nLength = strlen(szMsg);
	if(nLength!=0)
	{
		ZeroMemory(m_szMsg,MAX_PATH);
		strcpy(m_szMsg,szMsg);
		m_nType = nType;
		m_bInited = TRUE;
	}
	else
		m_bInited = FALSE;
	return m_bInited;
}

BOOL CMsgBox::ShowMessageBox(VOID)							// Show Message box with Initialized
{
	m_bActive = TRUE;
	return m_bActive;
}

BOOL CMsgBox::ShowMessageBox(CHAR* szMsg,INT nType)		// Show	Message box with Initialization
{
	if(SetMsgBox(szMsg,nType))
		return ShowMessageBox();
	else
		m_bActive = FALSE;
	return m_bActive;
}

BOOL CMsgBox::HideMessageBox()							// Hide Message box
{
	m_bActive = FALSE;
	return TRUE;
}

BOOL CMsgBox::DestoryMessageBox(VOID)						// DestoryMessage box
{
	m_nType = 0;
	m_bActive = FALSE;
	m_bInited = FALSE;
	return TRUE;
}

HRESULT CMsgBox::OnButtonDown(LPARAM lParam, WPARAM wParam)		// Button Down
{
	HRESULT	hResult = 0;

	return hResult;
}

HRESULT	CMsgBox::OnButtonUp(LPARAM lParam,WPARAM wParam)		// Button Up
{
	HRESULT	hResult = 0;

	return hResult;	
/*
#define CLICK_NONE			0
#define CLICK_OK			1
#define CLICK_YES			2
#define CLICK_NO			4
#define CLICK_CANCLE		8
#define HAVE_EDITOBOX		128
#define HAVE_NO_EDITBOX		256
*/
}

BOOL CMsgBox::IsActive(VOID)
{
	return m_bActive;
}
BOOL CMsgBox::IsInited(VOID)
{
	return m_bInited;
}

	// Render Process
BOOL CMsgBox::RenderMessageBox(INT nLoopTIme)							// Render Function
{
	if(m_bInited&&m_bActive)
	{
		// Draw Text With Dlg Image
		return TRUE;
	}
	return FALSE;
}
