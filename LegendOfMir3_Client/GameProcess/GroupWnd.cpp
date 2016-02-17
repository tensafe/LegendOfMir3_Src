/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/

#include "StdAfx.h"

typedef struct tagINTERFACE_BUTTON
{
public:
	int		m_nLeft;
	int		m_nTop;
	int		m_nWidth;
	int		m_nHeight;
	BYTE	m_btCommand;
} INTERFACE_BUTTON;

#define GROUPBTN_ALLOWGROUP		1
#define GROUPBTN_CREATEGROUP	2
#define GROUPBTN_ADDMEMBER		3
#define GROUPBTN_DELMEMBER		4

INTERFACE_BUTTON GroupWndButton[] = {	{ 21, 57, 27, 28, GROUPBTN_ALLOWGROUP },
										{ 27, 200, 71, 13, GROUPBTN_CREATEGROUP }, 
										{ 91, 200, 77, 16, GROUPBTN_ADDMEMBER }, 
										{ 170, 200, 73, 16, GROUPBTN_DELMEMBER } 
									};

BOOL g_fAllowGroup = TRUE;

CGroupWnd::CGroupWnd()
{
	Init();
}

CGroupWnd::~CGroupWnd()
{
	Destroy();
}

VOID CGroupWnd::CreateGroupWnd(INT nID, CWHWilImageData* pxWndImage, INT nFrameImgIdx, INT nStartX, INT nStartY, INT nWidth, INT nHeight, BOOL bCanMove)
{
	CreateGameWnd(nID, pxWndImage, nFrameImgIdx, bCanMove, nStartX, nStartY, nWidth, nHeight);
}

VOID CGroupWnd::ShowGroupWnd()
{
	CGroupMember*	pGroupMember;

	RECT tRect = GetGameWndRect();

	int				nX = tRect.left + 45, nY = tRect.top + 100;
	
	ShowGameWnd();

	g_xMainWnd.PutsHan(NULL, tRect.left + 45, tRect.top + 25, RGB(255, 255, 255), RGB(0, 0, 0), g_szUserID, NULL);

	if (m_xGroupList.GetCounter())
	{
		m_xGroupList.MoveCurrentToTop();

		for (int i = 1; i <= m_xGroupList.GetCounter(); i++)
		{
			pGroupMember = m_xGroupList.GetCurrentData();

			g_xMainWnd.PutsHan(NULL, nX, nY, RGB(255, 255, 255), RGB(0, 0, 0), pGroupMember->szMemberName);

			if (i % 2) 
				nX += 100;
			else
			{
				nX -= 100;
				nY += 50;
			}

			m_xGroupList.MoveNextNode();
		}
	}
}

VOID CGroupWnd::Init()
{
	CGameWnd::Init();
}

VOID CGroupWnd::Destroy()
{
	CGameWnd::Destroy();
}

BOOL CGroupWnd::OnLButtonUp(POINT ptMouse)
{
	RECT tRect = GetGameWndRect();

	for (int i = 0; i < sizeof(GroupWndButton)/sizeof(INTERFACE_BUTTON); i++)
	{
		if (tRect.left + GroupWndButton[i].m_nLeft < ptMouse.x && tRect.left + GroupWndButton[i].m_nLeft + GroupWndButton[i].m_nWidth > ptMouse.x &&
			tRect.top + GroupWndButton[i].m_nTop < ptMouse.y && tRect.top + GroupWndButton[i].m_nTop + GroupWndButton[i].m_nHeight > ptMouse.y)
		{
			switch (GroupWndButton[i].m_btCommand)
			{
				case GROUPBTN_ALLOWGROUP:
					(g_fAllowGroup ? g_fAllowGroup = FALSE : g_fAllowGroup = TRUE);
					g_xClientSocket.SendGroupMode(g_fAllowGroup);
					break;
				case GROUPBTN_CREATEGROUP:
					g_xClientSocket.SendCreateGroup("mandul");
					break;
				case GROUPBTN_ADDMEMBER:
					g_xClientSocket.SendAddGroupMember("mandul");
					break;
				case GROUPBTN_DELMEMBER:
					g_xClientSocket.SendDelGroupMember("mandul");
					break;
			}
		}
	}

	return FALSE;
}

BOOL CGroupWnd::OnLButtonDown(POINT ptMouse)
{
	return FALSE;
}

VOID CGroupWnd::OnMouseMove(POINT ptMouse)
{
}
