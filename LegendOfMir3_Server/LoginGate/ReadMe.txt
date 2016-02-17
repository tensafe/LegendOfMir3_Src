===============================================================================
       
       LoginGate
       
       All written by Euy-heon, Jeong.
       Copyright(C) 2001 Wemade Entertainment Co., LTD. All rights Reserved.
       
===============================================================================

Last Update			: 2001/10/12
Last Update Author	: Euy-heon, Jeong.

1. Using common module file name.

	- RegistryHandler.cpp
	- ServerSockHandler.cpp

2. Global variables.

	- deque<string> 	g_ReceiveDeq;		<-- ClientSockMsg.cpp
	- HANDLE			g_hThreadForComm;	<-- ClientSockMsg.cpp
	- HINSTANCE			g_hInst;			<-- LoginGate.cpp
	- HWND				g_hMainWnd;			<-- LoginGate.cpp
	- HWND				g_hLogMsgWnd;		<-- LoginGate.cpp
	- HWND				g_hToolBar;			<-- LoginGate.cpp
	- static WSADATA 	g_wsd;				<-- LoginGate.cpp
	- TBBUTTON 			tbButtons[];		<-- LoginGate.cpp
	- SOCKET			g_ssock;			<-- MainWndProc.cpp
	- SOCKADDR_IN		g_saddr;			<-- MainWndProc.cpp
	- SOCKET			g_csock;			<-- MainWndProc.cpp
	- SOCKADDR_IN		g_caddr;			<-- MainWndProc.cpp
	- map<SOCKET, CUserInfo, less<SOCKET> >	g_TUserInfoMap;	<-- ServerSockMsg.cpp
	- CRITICAL_SECTION	g_cs;				<-- ThreadFuncForMsg.cpp

3. File and function description.

	* StdAfx.cpp
	* ClientSockMsg.cpp
	- BOOL InitServerThreadForComm()
	- BOOL InitServerThreadForMsg()
	- LPARAM OnClientSockMsg(WPARAM wParam, LPARAM lParam)

	* ConfigDlgFunc.cpp
	- BOOL CALLBACK ConfigDlgFunc(HWND hWndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)

	* LoginGate.cpp
	- int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
	- BOOL InitApplication(HANDLE hInstance)
	- BOOL InitInstance(HANDLE hInstance, int nCmdShow)
	- int AddNewLogMsg()
	- void InsertLogMsg(UINT nID)
	- void InsertLogMsg(LPTSTR lpszMsg)

	* LoginGate.rc
	* MainWndProc.cpp
	- void OnCommand(WPARAM wParam, LPARAM lParam)
	- LPARAM APIENTRY MainWndProc(HWND hWnd, UINT nMsg, WPARAM wParam, LPARAM lParam)

	* ServerSockMsg.cpp
	- LPARAM OnServerSockMsg(WPARAM wParam, LPARAM lParam)

	* ThreadFuncForComm.cpp
	- DWORD WINAPI ThreadFuncForComm(LPVOID lpParameter)

	* ThreadFuncForMsg.cpp
	- DWORD WINAPI ThreadFuncForMsg(LPVOID lpParameter)

4. History

	- 2001/8/31
		- Completed Alpha Version (V0.9) and written this document.
	- 2001/09/03
		- Modified 'CUserInfo' structure : array -> map collection (Key = Socket, Value = CUserInfo)
	- 2001/09/25
		- Changed packet on between gate server to login server. (string -> TGateToSvrHeader)