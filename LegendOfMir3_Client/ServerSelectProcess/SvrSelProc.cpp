#include "StdAfx.h"


/******************************************************************************************************************

	게임 플레이 관련 함수들.

*******************************************************************************************************************/


CSvrSelProcess::CSvrSelProcess()
{
}

CSvrSelProcess::~CSvrSelProcess()
{
}

VOID CSvrSelProcess::Load()
{
}	

VOID CSvrSelProcess::DeleteProc()
{
}

VOID CSvrSelProcess::RenderScene(INT nLoopTime)
{
	Clear(RGB(0, 0, 0));
//	ShowStatus(0, 0);
}

VOID CSvrSelProcess::SetNextProc()
{
	DeleteProc();
	g_xMainWnd.m_pxDefProcess = &g_xChrSelProc;
	g_xChrSelProc.Load();
	g_bProcState = _CHAR_SEL_PROC;
}

/******************************************************************************************************************

	메시지 관련 함수.

*******************************************************************************************************************/
LRESULT CSvrSelProcess::DefMainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch ( uMsg )
	{
	case WM_KEYDOWN:
		OnKeyDown(wParam, lParam);
		break;
	default:
		break;
	}

	return CWHDefProcess::DefMainWndProc(hWnd, uMsg, wParam, lParam);
}


LRESULT CSvrSelProcess::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	switch ( wParam )
	{
	case VK_RETURN:
		{
			SetNextProc();
		}
		break;
	}	
	return 0L;
}


/******************************************************************************************************************

	기타 함수및 디버그용 함수.

*******************************************************************************************************************/

VOID CSvrSelProcess::ShowStatus(INT nStartX, INT nStartY)
{
	TCHAR			szBuff[MAX_PATH];
	sprintf(szBuff, "Server Select Procedure");
	g_xMainWnd.PutsHan(g_xMainWnd.GetBackBuffer(), nStartX, nStartY, RGB(250, 250, 250), RGB(0, 0, 0), szBuff);

	CWHDefProcess::ShowStatus(nStartX, nStartY+20);
}
