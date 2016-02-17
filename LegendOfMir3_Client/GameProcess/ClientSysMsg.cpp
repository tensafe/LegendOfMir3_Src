/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/



#include "StdAfx.h"



/******************************************************************************************************************

	ClientSysMsg Class

*******************************************************************************************************************/

ClientSysMsg::ClientSysMsg()
{
	Init();
}

ClientSysMsg::~ClientSysMsg()
{
	Destroy();
}

VOID ClientSysMsg::Init()
{
	m_wCurrDelay	= 0;
}

VOID ClientSysMsg::Destroy()
{
	m_stSysMsg.ClearAllNodes();
	Init();
}

VOID ClientSysMsg::AddSysMsg(CHAR* szStr)
{
	LPCONSTSTRING pstMsg = NULL;
	while ( m_stSysMsg.GetCounter() >= _MAX_SYSMSG )
	{
		m_stSysMsg.MoveCurrentToTop();
		pstMsg = m_stSysMsg.GetCurrentData();

		if ( pstMsg ) 
		{
			m_stSysMsg.DeleteCurrentNodeEx();
			SAFE_DELETE((LPCONSTSTRING)pstMsg);
		}
	}

	m_stSysMsg.MoveCurrentToTail();
	pstMsg = new CONSTSTRING;
	strcpy(pstMsg->szMsg, szStr);
	m_stSysMsg.AddNode(pstMsg);
	m_wCurrDelay = 0;
}

VOID ClientSysMsg::ShowSysMsg(INT nLoopTime, INT nX, INT nY)
{
	DWORD dwDelay = 3000;
	LPCONSTSTRING pstMsg = NULL;
	m_wCurrDelay += nLoopTime;

	m_stSysMsg.MoveCurrentToTop();
	for ( INT nCnt = 0; nCnt < m_stSysMsg.GetCounter(); nCnt++ )
	{
		LPCONSTSTRING pstMsg = m_stSysMsg.GetCurrentData();

		if ( pstMsg )
		{
			g_xMainWnd.PutsHan(NULL, nX+1, nY+nCnt*15+1, RGB(10, 10, 10), RGB(0, 0, 0), pstMsg->szMsg);
			g_xMainWnd.PutsHan(NULL, nX, nY+nCnt*15, RGB(100, 200, 100), RGB(0, 0, 0), pstMsg->szMsg);
		}
		m_stSysMsg.MoveNextNode();
	}

	if ( m_stSysMsg.GetCounter() > 5 )		dwDelay = 1000;

	if ( m_wCurrDelay > dwDelay )
	{
		m_stSysMsg.MoveCurrentToTop();
		if ( m_stSysMsg.GetCounter() )
		{
			pstMsg = m_stSysMsg.GetCurrentData();

			if ( pstMsg )
			{
				m_stSysMsg.DeleteCurrentNodeEx();
				SAFE_DELETE((LPCONSTSTRING)pstMsg);
			}
		}
		m_wCurrDelay = 0;
	}
}
