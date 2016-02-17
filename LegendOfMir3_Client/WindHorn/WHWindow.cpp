/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/



#include "stdafx.h"
#include "WHEngine.h"

static CWHApp* g_pWHApp = NULL;

/******************************************************************************************************************

	CWHApp Functions.

*******************************************************************************************************************/

/******************************************************************************************************************

	함수명 : CWHApp::CWHApp()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
CWHApp::CWHApp()
{
	g_pWHApp	= this;
	m_hInstance = NULL;
}

/******************************************************************************************************************

	함수명 : CWHApp::~CWHApp()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
CWHApp::~CWHApp()
{
}

/******************************************************************************************************************

	함수명 : CWHApp::MainWndProc()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : HWND hWnd
	         UINT uMsg
	         WPARAM wParam
	         LPARAM lParam
	출력   : LRESULT 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
LRESULT CWHApp::MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
    return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

/******************************************************************************************************************

	함수명 : WndProc()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : HWND hWnd
	         UINT uMsg
	         WPARAM wParam
	         LPARAM lParam
	출력   : LRESULT CALLBACK 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    return g_pWHApp->MainWndProc(hWnd, uMsg, wParam, lParam);
}










/******************************************************************************************************************

	CWHWindow Functions.

*******************************************************************************************************************/

/******************************************************************************************************************

	함수명 : CWHWindow::CWHWindow()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
CWHWindow::CWHWindow()
{
	m_hWnd = NULL;
}

/******************************************************************************************************************

	함수명 : CWHWindow::~CWHWindow()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
CWHWindow::~CWHWindow()
{
}

/******************************************************************************************************************

	함수명 : CWHWindow::RegisterClass()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : CHAR *pszMenuName
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CWHWindow::RegisterClass(CHAR* pszMenuName, CHAR* pszIconName)
{
    WNDCLASSEX wc;

    wc.cbSize			= sizeof(wc);
    wc.lpszClassName	= _WINDHORN_CLASS;
    wc.lpfnWndProc		= WndProc;
    wc.style			= CS_VREDRAW | CS_HREDRAW | CS_DBLCLKS;
    wc.hInstance		= m_hInstance;
    wc.hIcon			= LoadIcon(m_hInstance, pszIconName);
    wc.hIconSm			= LoadIcon(m_hInstance, pszIconName);
    wc.hCursor			= LoadCursor(m_hInstance, IDC_ARROW);
    wc.hbrBackground	= (HBRUSH)(GRAY_BRUSH);//(COLOR_WINDOW + 1);
    wc.lpszMenuName		= pszMenuName;
    wc.cbClsExtra		= 0;
    wc.cbWndExtra		= 0;

    if ( RegisterClassEx(&wc) == 0 )
	{
        return FALSE;
	}

	return TRUE;
}

/******************************************************************************************************************

	함수명 : CWHWindow::Create()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : LPTSTR lpCaption
	         CHAR *pszMenuName
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CWHWindow::Create(HINSTANCE hInst, LPTSTR lpCaption, CHAR *pszMenuName, CHAR* pszIconName)
{
	SethInstance(hInst);
	if ( !RegisterClass(pszMenuName, pszIconName) ) 
	{
		MessageBox(NULL, TEXT("[CWHWindow::Create]") TEXT("Register class failed."), "MirDXG", MB_ICONERROR | MB_OK);
		return FALSE;
	}

    DWORD	dwFrameWidth    = GetSystemMetrics(SM_CXSIZEFRAME);
    DWORD	dwFrameHeight   = GetSystemMetrics(SM_CYSIZEFRAME);
	DWORD	dwMenuHeight;
	
	if ( pszMenuName != NULL )		
		dwMenuHeight = GetSystemMetrics(SM_CYMENU);
	else
		dwMenuHeight = 0;

    DWORD dwCaptionHeight = GetSystemMetrics(SM_CYCAPTION);
    DWORD dwWindowWidth   = _SCREEN_WIDTH  + dwFrameWidth * 2;
    DWORD dwWindowHeight  = _SCREEN_HEIGHT + dwFrameHeight * 2 + dwMenuHeight + dwCaptionHeight;

    DWORD dwStyle = WS_POPUPWINDOW/*WS_OVERLAPPEDWINDOW*/ & ~WS_MAXIMIZEBOX;

    m_hWnd = CreateWindowEx(NULL, _WINDHORN_CLASS, lpCaption,
						    dwStyle, CW_USEDEFAULT, CW_USEDEFAULT,
  						    dwWindowWidth, dwWindowHeight, NULL, NULL, m_hInstance, NULL);

	if ( m_hWnd == NULL )
    	return FALSE;

//  ShowWindow(m_hWnd, SW_SHOW);
//  UpdateWindow(m_hWnd);

	return TRUE;
}

/******************************************************************************************************************

	함수명 : CWHWindow::MainWndProc()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : HWND hWnd
	         UINT uMsg
	         WPARAM wParam
	         LPARAM lParam
	출력   : LRESULT 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
LRESULT CWHWindow::MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	switch (uMsg)
	{
		case WM_DESTROY:
			return OnDestroy();
	}

    return CWHApp::MainWndProc(hWnd, uMsg, wParam, lParam);
}

/******************************************************************************************************************

	함수명 : CWHWindow::OnDestroy()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
LRESULT CWHWindow::OnDestroy()
{
	PostQuitMessage(NULL);
	return 0L;
}
