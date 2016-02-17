// CharacterProcess.cpp: implementation of the CCharacterProcess class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"

////////////////////////////////////////////////
////////////////////////////////////////////////


// Positio of Edit box
#define POS_NAME_X						300
#define POS_NAME_Y						460

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCharacterProcess::CCharacterProcess()
{
	Init();
}

CCharacterProcess::~CCharacterProcess()
{
//	if(m_pBtn)
//	{
//		delete[] m_pBtn;
//		m_pBtn = NULL;
//	}
}

VOID CCharacterProcess::Init()
{
	m_Progress=PRG_CHAR_SELE;
	m_ConnectionState=CONNECT_FAIL;
	ZeroMemory(m_szErrorMsg,512);
//	m_pBtn = new CMirButton[sizeof(g_CharProcBtnInfo)/sizeof(BUTTONINFO)];
}

VOID CCharacterProcess::Load()
{
	WSAStartup(MAKEWORD(2, 2), &m_wsd);
	m_Image.NewLoad(".\\Data\\interface1c.wil", TRUE, TRUE);
	m_Image2.NewLoad(".\\Data\\interfacec2.wil", TRUE, TRUE);
	// 임시
	//
	m_ConnectionState = CONNECT_FAIL;

	g_xClientSocket.ConnectToServer(g_xMainWnd.GetSafehWnd(), g_szServerIP, g_nServerPort);

	g_xChatEditBox.SetLimitText(20);
	SendMessage(g_xChatEditBox.GetSafehWnd(),EM_SETPASSWORDCHAR,NULL,0);

	m_ChrCount =0;
	m_nMessage = 0;
//	m_xMsgBox.Load(&m_Image2);
	m_xMsgBox.CreateWnd(&m_Image2,TRUE,219,110,362,180);	// 용기
	m_Progress	= PRG_CHAR_SELE;							// Character Selection
	m_xBGround.Create(&m_Image);
	m_xBGround.OpenWnd();
}	

VOID CCharacterProcess::SetNextProc()
{
	D3DMATRIX	matProj;
	DeleteProc();
 	g_xClientSocket.m_pxDefProc = g_xMainWnd.m_pxDefProcess = &g_xGameProc;
	g_xGameProc.Load();
	g_xChatEditBox.SetLimitText(90);
	ShowWindow(g_xChatEditBox.GetSafehWnd(), SW_HIDE);
	D3DUtil_SetProjectionMatrix(matProj, g_PI/2, float(600.0f/800.0f), -1.0f, 1.0f);
    g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_PROJECTION, &matProj);
	g_bProcState = _GAME_PROC; 
}

VOID CCharacterProcess::DeleteProc()
{
	
	WSACleanup();
	m_Image.Destroy();
	m_Image2.Destroy();
	// 임시
	m_CRectList.ClearAllNodes();
	m_CList.ClearAllNodes();
}


LRESULT CCharacterProcess::DefMainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch ( uMsg )
	{
		case WM_LBUTTONDOWN:
			OnLButtonDown(wParam, lParam);
			break;
		case WM_LBUTTONUP:
			OnLButtonUp(wParam, lParam);
			break;
		case ID_SOCKCLIENT_EVENT_MSG:
			g_xClientSocket.OnSocketMessage(wParam, lParam);
			break;
		case WM_KEYDOWN:
			OnKeyDown(wParam, lParam);
			break;
		case WM_MOUSEON:
			OnMouseOn(wParam,lParam);
			break;
		case WM_MOVE:
			OnWindowMove(wParam,lParam);
			break;
		case WM_MOUSEMOVE:
			OnMouseMove(wParam,lParam);
			break;
		default:
			break;
	}
	return CWHDefProcess::DefMainWndProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CCharacterProcess::OnKeyDown(WPARAM wParam, LPARAM lParam)
{
	switch ( wParam )
	{
	case VK_F1:
		m_Progress=PRG_CHAR_SELE;
		break;
	case VK_F2:
		m_Progress=PRG_CREATE_CHAR;
		break;
	}	
	return 0L;
}

LRESULT CCharacterProcess::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	int		i;
	BOOL	fOnButton = FALSE;

	if(m_xMsgBox.IsActive())
	{
		POINT  tPos;
		tPos.x = LOWORD(lParam);
		tPos.y = HIWORD(lParam);
		m_xMsgBox.MoveWnd(tPos);	// Moon
		return 0l;
	}


	if(!m_fIsLButtonClicked)//m_fIsConnected && !m_fIsLButtonClicked)
	{
		switch(m_Progress)
		{
			case PRG_CHAR_SELE:
				m_xSelectChr.OnMouseMove(wParam, lParam);
				break;
			case PRG_CREATE_CHAR:
			{
				for(i = 5; i < 9; i++)
				{
/*					if (m_pBtn[i].CheckMouseOn(LOWORD(lParam), HIWORD(lParam)))
					{
						m_pBtn[i].m_nState = BUTTON_STATE_ON;
						fOnButton = TRUE;
					}
					else
						m_pBtn[i].m_nState = BUTTON_STATE_UP;*/
				}

				break;
			}
		}
		if(fOnButton)
		{
			HCURSOR	hCursor;
			hCursor = LoadCursor(g_xMainWnd.GetSafehInstance(),MAKEINTRESOURCE(IDC_ARROW));
			SetCursor(hCursor);
			DestroyCursor(hCursor); 
		}
//		else
//			g_xMainWnd.OnSetCursor(IDC_ARROW);
	}
	return 0L;
}

LRESULT CCharacterProcess::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{	
	m_fIsLButtonClicked = FALSE;
	if(m_xMsgBox.IsActive())
	{
		POINT ptMouse;
		ptMouse.x = LOWORD(lParam);
		ptMouse.y = HIWORD(lParam);

		m_xMsgBox.OnButtonDown(ptMouse);
	}
	else
	{
		if(m_Progress==PRG_CHAR_SELE)
			m_xSelectChr.OnButtonDown(wParam, lParam);
		if (m_Progress == PRG_CREATE_CHAR)
		{
			for (int i = ID_CREATE_CHR_OK_BTN; i <= ID_CREATE_CHR_TAO_BTN; i++)
			{
/*				if (m_pBtn[i].CheckMouseOn(LOWORD(lParam), HIWORD(lParam)))
				{
					m_pBtn[i].m_nState = BUTTON_STATE_DOWN;
					m_fIsLButtonClicked = TRUE;
				}
				else
					m_pBtn[i].m_nState = BUTTON_STATE_UP;
*/			}
		}
	}
	return 0L;
}

LRESULT CCharacterProcess::OnLButtonUp(WPARAM wParam, LPARAM lParam)
{	
	INT	nRecieve;
	m_ptMousePos.x = LOWORD(lParam);
	m_ptMousePos.y = HIWORD(lParam);

	m_fIsLButtonClicked = FALSE;
	if(m_xMsgBox.IsActive())	// Message Box Process
	{
		MessageBoxProcess(lParam);
	}
	else
	{
		switch(m_Progress)
		{
		case PRG_CHAR_SELE:
			{
				nRecieve = m_xSelectChr.OnButtonUp(wParam, lParam);
				switch(nRecieve)
				{
					case RETURN_SELECT_NEW_BTN:
					{
						break;
					}
					case RETURN_SELECT_NEW_ERR:
					{
						m_nMessage = 3;
						SetErrorMessage(m_nMessage,m_szErrorMsg);
						m_xMsgBox.ShowMessageBox(m_szErrorMsg,MSG_BTN_OK,_WNDIMGINDX_MSGBOX1);
						break;
					}
					case RETURN_SELECT_DEL_BTN:
					{
						m_nMessage = 5;
						SetErrorMessage(m_nMessage,m_szErrorMsg);
						m_xMsgBox.ShowMessageBox(m_szErrorMsg,MSG_BTN_YESNO,_WNDIMGINDX_MSGBOX1);
						break;
					}
					case RETURN_SELECT_DEL_ERR:
					{
						m_nMessage = 8;
						SetErrorMessage(m_nMessage,m_szErrorMsg);
						m_xMsgBox.ShowMessageBox(m_szErrorMsg,MSG_BTN_OK,_WNDIMGINDX_MSGBOX1);
						break;
					}
					case RETURN_SELECT_START_BTN:
					{
						break;
					}
					case RETURN_SELECT_START_ERR:
					{
						m_nMessage = 7;
						SetErrorMessage(m_nMessage,m_szErrorMsg);
						m_xMsgBox.ShowMessageBox(m_szErrorMsg,MSG_BTN_OK,_WNDIMGINDX_MSGBOX1);
						break;
					}
				}
				break;
			}

		case PRG_CREATE_CHAR:
			{
			/*
				for(i = ID_CREATE_CHR_OK_BTN; i <= ID_CREATE_CHR_TAO_BTN; i++)
				{
					m_pBtn[i].m_nState = BUTTON_STATE_UP;
					if (m_pBtn[i].CheckMouseOn(LOWORD(lParam), HIWORD(lParam)))
					{
						switch(i)
						{
							case ID_CREATE_CHR_WAR_BTN:
								m_SelectJob = _JOB_JUNSA;
								break;
							case ID_CREATE_CHR_SOR_BTN:
								m_SelectJob = _JOB_SULSA;
								break;
							case ID_CREATE_CHR_TAO_BTN:
								m_SelectJob = _JOB_DOSA;
								break;
							case ID_CREATE_CHR_OK_BTN:
							{
								int Lens;
								GetWindowText(g_xChatEditBox.GetSafehWnd(),m_szChrName,sizeof(m_szChrName));
								Lens = strlen(m_szChrName);
								if(Lens!=0)
								{
									CHRINFO	ChrInfo;
									strcpy(ChrInfo.ChrName,m_szChrName);
									ChrInfo.Class=m_SelectJob;
									ChrInfo.Gender=m_SelectSex;
									ChrInfo.Level = 1;
									m_ChrCount = 0;
									m_xBGround.SetRenderState(_RENDER_AVI_OUT);
									g_xSound.PlayMp3(MP3_TOSELECTCHAR,FALSE); 
									m_Progress = PRG_CREA_TO_SEL;					// 임시
									g_xClientSocket.OnQueryChar();					// 임시
								}
								else
								{
									m_nMessage = 2;
									SetErrorMessage(m_nMessage,m_szErrorMsg);
									m_xMsgBox.ShowMessageBox(m_szErrorMsg,1,_WNDIMGINDX_MSGBOX1);
								}
							}
						}
					}
				}*/
				m_Progress = PRG_CREA_TO_SEL;
				m_xBGround.SetRenderState(_RENDER_AVI_OUT);
				break;
			}
		}
	}
	return 0L;
}

LRESULT CCharacterProcess::OnMouseOn(WPARAM wParam, LPARAM lParam)
{	
/*	UINT	t_ID;
	t_ID=(UINT)wParam;
	m_ButtonID = t_ID;		// Upon Mouse*/
	return 0L;
}

LRESULT CCharacterProcess::OnWindowMove(WPARAM wParam,LPARAM lParam)
{

	ShowWindow(g_xChatEditBox.GetSafehWnd(), SW_HIDE);
	g_xMainWnd.OnMove(wParam, lParam);
	return 0L;
}

// **************************************************************************************
//
//		Socket
//
// **************************************************************************************

VOID CCharacterProcess::OnConnectToServer()
{
	m_ConnectionState = CONNECT_SUCCESS;// Connection Success
	
	g_xClientSocket.OnQueryChar();
}

char* CCharacterProcess::OnMessageReceive(CHAR* pszMessage)
{
	char	*pszFirst = pszMessage;
	char	*pszEnd;
	
	while (pszEnd = strchr(pszFirst, '!'))
	{
		*pszEnd = '\0';

		OnSocketMessageRecieve(pszFirst + 1);

		if (*(pszEnd + 1) == '#')
			pszFirst = pszEnd + 1;
		else
			return NULL;
	}

	return pszFirst;
}

void CCharacterProcess::OnSocketMessageRecieve(char* pszMsg)
{
	_TDEFAULTMESSAGE	tdm;
	INT	nChrPos[] = { 350, 280, 350, 250, 320, 390 };

	fnDecodeMessage(&tdm, pszMsg);

	switch(tdm.wIdent)
	{
		case SM_QUERYCHR:
		{
			m_SelectChr = SELECT_NONE;
			if (tdm.nRecog)
			{
				char	szDecodeMsg[512];
				char	*pszID = NULL, *pszJob = NULL, *pszFace = NULL, *pszLevel = NULL, *pszSex = NULL, *pszNext = NULL;
				int		nPos = 0;

				m_SelectChr = SELECT_FIRST;
				m_CRectList.ClearAllNodes();

				ZeroMemory(szDecodeMsg, sizeof(szDecodeMsg));

				fnDecode6BitBuf((pszMsg + DEFBLOCKSIZE), szDecodeMsg, sizeof(szDecodeMsg));

				m_ChrCount = tdm.nRecog;

				for (int i = 0; i < tdm.nRecog; i++)
				{
					CHRINFO* pxChInfo = new CHRINFO;

					if (szDecodeMsg[nPos] == '*')
					{
						pszID		= &szDecodeMsg[nPos + 1];
						m_SelectChr = i;
					}
					else
						pszID = &szDecodeMsg[nPos];

					if (pszJob = strchr(pszID, '/'))
					{
						*pszJob = '\0';
						pszJob++;

						strcpy(pxChInfo->ChrName, pszID);
						
						pxChInfo->Class = atoi(pszJob);

						if (pszFace = strchr(pszJob, '/'))
						{
							*pszFace = '\0';
							pszFace++;

							if (pszLevel = strchr(pszFace, '/'))
							{
								*pszLevel = '\0';
								pszLevel++;

								pxChInfo->Level = atoi(pszLevel);

								if (pszSex = strchr(pszLevel, '/'))
								{
									*pszSex = '\0';
									pszSex++;

									pxChInfo->Gender = atoi(pszSex);
									
									m_CList.AddNode(pxChInfo);
 
									m_Image.NewSetIndex(IDX_IMG_MALE_WAR + m_CList.GetCurrentData()->Gender + (m_CList.GetCurrentData()->Class * 2)) ;

									CHRRECT* pxChRect = new CHRRECT;
									pxChRect->nLeft		= nChrPos[i + tdm.nRecog - 1];
									pxChRect->nTop		= 280;
									pxChRect->nWidth	= m_Image.m_lpstNewCurrWilImageInfo->shWidth;
									pxChRect->nHeight	= m_Image.m_lpstNewCurrWilImageInfo->shHeight;

									m_CRectList.AddNode(pxChRect);

									if (pszNext = strchr(pszSex, '/'))
									{
										pszNext++;

										if (*pszNext != '\0')
											nPos = pszNext - &szDecodeMsg[0];
									}
								}
							}
						}
					} // For
					m_xSelectChr.Create(&m_Image,&m_CList,&m_CRectList,tdm.nRecog);
					m_xSelectChr.OpenWnd ();
				}
			}

			break;
		}
		case	SM_NEWCHR_SUCCESS:		// New character Create Success
		{
			m_Progress = PRG_CREA_TO_SEL;
			g_xClientSocket.OnQueryChar();
			break;
		}
		case	SM_STARTPLAY:			// Start Game
		{
			char	szDecodeMsg[512];
			char	*pszIP = &szDecodeMsg[0], *pszPort;

			ZeroMemory(szDecodeMsg, sizeof(szDecodeMsg));

			fnDecode6BitBuf((pszMsg + DEFBLOCKSIZE), szDecodeMsg, sizeof(szDecodeMsg));

			if (pszPort = strchr(pszIP, '/'))
			{
				*pszPort = '\0';
				pszPort++;
				strcpy(g_szServerIP, pszIP);
				g_nServerPort = atoi(pszPort);
				g_xClientSocket.DisconnectToServer();
				m_xSelectChr.m_nRenderState = 2;			// Set Select Renderstaet to Ready startgame
			}
			break;
		}
	}
/*	_TDEFAULTMESSAGE	tdm;
	char				szDecodeMsg[256];
	char				szTmpDecodeMsg[512];

	ZeroMemory(szTmpDecodeMsg, 512);
	strcpy(szTmpDecodeMsg, pszMsg);

	char *pszData = &szTmpDecodeMsg[1];

	fnDecodeMessage(&tdm, pszData);

	if(lstrlen(szTmpDecodeMsg) >= DEFBLOCKSIZE + 1)
		fnDecode6BitBuf((szTmpDecodeMsg + DEFBLOCKSIZE + 1), szDecodeMsg, sizeof(szDecodeMsg));

	switch(tdm.wIdent)
	{
	case	SM_QUERYCHR:			// Getting character List
		{
			pCHRINFO ChrInfo;
			pCHRRECT tChRect;
			m_CRectList.ClearAllNodes();
			m_CList.ClearAllNodes();
			if(tdm.wParam!=0)	// 케릭터가 존재 할경우
			{
				ChrInfo = new CHRINFO[tdm.wParam]; // 받아온 겟수만큼 케릭터 정보 를 만든다.
				m_SelectChr = SELECT_FIRST;
				m_ChrCount = tdm.wParam;
				fnDecode6BitBuf((szTmpDecodeMsg + DEFBLOCKSIZE + 1), (char*)ChrInfo, sizeof(CHRINFO)*(tdm.wParam));
				pCHRINFO  tChInfo;
				for(int i = 0 ;i<tdm.wParam;i++)
				{
					// 케릭터가 존재 하지 않을경우?
					// 케릭터 Info
					tChInfo = new CHRINFO;
					strcpy(tChInfo->ChrName,ChrInfo[i].ChrName);
					tChInfo->Class=ChrInfo[i].Class;
					tChInfo->Gender=ChrInfo[i].Gender;
					tChInfo->Level=ChrInfo[i].Level;
					m_CList.AddNode(tChInfo);

					// 케릭터 Rect 수정 할것  // 임시
					tChRect = new CHRRECT;

					m_tImage1.NewSetIndex((2-(INT)(tChInfo->Class)));

					switch(tdm.wParam)
					{
					case 1:
						tChRect->nLeft = 350;
						break;
					case 2:
						{
							switch(i)
							{
							case 0:
								tChRect->nLeft = 280;
								break;
							case 1:
								tChRect->nLeft = 420;
								break;
							}
							break;
						}
					case 3:
						{
							switch(i)
							{
							case 0:
								tChRect->nLeft = 180;
								break;
							case 1:
								tChRect->nLeft = 350;
								break;
							case 2:
								tChRect->nLeft  = 480;
								break;
							}
							break;
						}
					}
					
					tChRect->nTop  = 250+((2-tChInfo->Class)/2)*30;
					tChRect->nWidth = m_tImage1.m_lpstNewCurrWilImageInfo->shWidth;
					tChRect->nHeight = m_tImage1.m_lpstNewCurrWilImageInfo->shHeight;
					m_CRectList.AddNode(tChRect);
				}
				delete ChrInfo;
			}
			else
			{	// 케릭터가 존재 하지 않을경우는?
				m_SelectChr = SELECT_NONE;
			}
			break;
		}
	case	SM_NEWCHR_SUCCESS:		// New character Create Success
		{
		m_Progress = PRG_CREA_TO_SEL;
		g_xClientSocket.OnQueryChar();
		break;
		}
	case	SM_NEWCHR_FAIL:			// New character Create Fail
		{
			m_nMessage = tdm.wParam;		// Message Parameter 전달
			SetErrorMessage(m_nMessage,m_szErrorMsg);
			m_xMsgBox.ShowMessageBox(m_szErrorMsg,1,_WNDIMGINDX_MSGBOX1);
			m_Progress	= PRG_CREATE_CHAR;	// Create New Character
		}
		break; 
	case	SM_DELCHR_SUCCESS:		// Delete character Success

		g_xClientSocket.OnQueryChar();
		m_Progress	= PRG_CHAR_SELE;	// Character Selection

		break;
	case	SM_DELCHR_FAIL:			// Delete character Fail
		{
			m_nMessage = tdm.wParam;
			SetErrorMessage(m_nMessage,m_szErrorMsg);
			m_xMsgBox.ShowMessageBox(m_szErrorMsg,1,_WNDIMGINDX_MSGBOX1);
			// 1. 존재하지 않는 케릭터를 지울경우
			// 2. 알수 없는 Error
			// 3. 
			m_Progress	= PRG_CHAR_SELE;	// Character Selection
		}
		break;
	case	SM_STARTPLAY:			// Start Game
		{
			int Count;
			Count = strlen(szDecodeMsg);
			for(int i = 0 ; i< Count ; i++)
			{
				if(szDecodeMsg[i]==':')
				{
					strncpy(g_szServerIP,szDecodeMsg,i);
					g_nServerPort = atoi(szDecodeMsg+i+1);
				}
			}
			g_xClientSocket.DisconnectToServer();		// Char sever와의 연결 종료
			m_Progress=PRG_SEL_TO_GAME;
			break;
		}
	case	SM_STARTFAIL:
		{	// Fail to start game
			m_Progress	= PRG_QUIT;	// Quit for Some Reason
			SendMessage(g_xMainWnd.GetSafehWnd(), WM_DESTROY, NULL, NULL);
		}
		break;
	case	SM_QUERYCHR_FAIL:
		{	// Getting character List Fail
			m_Progress	= PRG_QUIT;	// Quit for Some Reason
			SendMessage(g_xMainWnd.GetSafehWnd(), WM_DESTROY, NULL, NULL);
		}
		break;
	case	SM_OUTOFCONNECTION:
		{	// Connection Lost(close Connection)
			m_Progress	= PRG_QUIT;	// Quit for Some Reason
			SendMessage(g_xMainWnd.GetSafehWnd(), WM_DESTROY, NULL, NULL);
		}
		break;
	}
	return 0L; */
}
		/**/
VOID CCharacterProcess::RenderScene(INT nLoopTime)
{
	m_xBGround.Render(nLoopTime);
	switch(m_Progress)
	{
		case PRG_CHAR_SELE:
		{
			ShowWindow(g_xChatEditBox.GetSafehWnd(), SW_HIDE);
			switch(m_xSelectChr.m_nRenderState)
			{
				case 11:	// Zoom in
				{
					m_xBGround.SetRenderState(_RENDER_AVI_IN);
					m_Progress = PRG_SEL_TO_CREA;
					m_xSelectChr.CloseWnd();
					break;
				}
				case 12:	// Zoom Out
				{
					m_xBGround.SetRenderState(_RENDER_AVI_START);	
					m_Progress = PRG_SEL_TO_GAME;
					m_xSelectChr.CloseWnd();
					break;
				}
				case 19:	// Select;
				{
					m_xSelectChr.m_nRenderState = 0;
				}
			}
			m_xSelectChr.Render(nLoopTime);
			break;
		}
		case PRG_SEL_TO_CREA:
			ShowWindow(g_xChatEditBox.GetSafehWnd(), SW_HIDE);
			if(m_xBGround.GetRenderState()==_RENDER_STILL_2)
				m_Progress = PRG_CREATE_CHAR;
			break;
		case PRG_CREATE_CHAR:
			ShowWindow(g_xChatEditBox.GetSafehWnd(), SW_SHOW);
			RenderCharCreate(nLoopTime);
			break;
		case PRG_CREA_TO_SEL:
			ShowWindow(g_xChatEditBox.GetSafehWnd(), SW_HIDE);
			if(m_xBGround.GetRenderState() == _RENDER_STILL_1)
			{	m_Progress = PRG_CHAR_SELE;
				m_xSelectChr.OpenWnd();
			}
			break;
		case PRG_SEL_TO_GAME:
			ShowWindow(g_xChatEditBox.GetSafehWnd(), SW_HIDE);
			if(m_xBGround.GetRenderState()==_RENDER_STARTGAME)
			{	m_Progress	= PRG_PLAY_GAME;				// Play Game
				SetNextProc();
			}
			break;
	}
	m_xMsgBox.RenderMessageBox(0);
}

VOID CCharacterProcess::ShowStatus(INT nStartX, INT nStartY)
{
/*	TCHAR			szBuff[MAX_PATH];
	sprintf(szBuff, "Character Procedure");*/

	CWHDefProcess::ShowStatus(nStartX, nStartY+20);
}


VOID	CCharacterProcess::RenderCharCreate(INT nLoopTime)
{
	POINT nPos[] = { {120, 88}, {120, 85}, {150, 108}};
//	int i ;

	MoveWindow(g_xChatEditBox.GetSafehWnd(), 
					   g_xMainWnd.m_rcWindow.left+POS_NAME_X,
					   g_xMainWnd.m_rcWindow.top+POS_NAME_Y, 75, 15, TRUE);

/*	for (i = ID_CREATE_CHR_OK_BTN ; i <= ID_CREATE_CHR_TAO_BTN; i++)
	{
		m_Image.NewSetIndex(m_pBtn[i].m_nButtonID + m_pBtn[i].m_nState -1);
		g_xMainWnd.DrawWithImageForComp(m_pBtn[i].m_Rect.left, m_pBtn[i].m_Rect.top,
									m_Image.m_lpstNewCurrWilImageInfo->shWidth,
									m_Image.m_lpstNewCurrWilImageInfo->shHeight					
									, (WORD*)(m_Image.m_pbCurrImage));
	}
*/
	// 임시~!
	m_Image.NewSetIndex(IMG_IDX_WAR_MALE_CREATE + m_SelectSex + m_SelectJob * 2);

	g_xMainWnd.DrawWithImageForComp(nPos[m_SelectJob].x,nPos[m_SelectJob].y
			,m_Image.m_lpstNewCurrWilImageInfo->shWidth,m_Image.m_lpstNewCurrWilImageInfo->shHeight
			,(WORD*)(m_Image.m_pbCurrImage));
}

VOID CCharacterProcess::SetErrorMessage(INT nMsg,CHAR* szMgs)
{
	HINSTANCE hLib;
	hLib = LoadLibrary(MESSAGE_DLL_1); // Load Dll
	if(nMsg!=99 && nMsg!=0)
	{
		LoadString(hLib,(nMsg+199),szMgs,512);
	}
	else 
	{
		if(nMsg!=0)
			LoadString(hLib,9999,szMgs,512);
		else
			ZeroMemory(szMgs,512);
	}
	FreeLibrary(hLib);
}


BOOL CCharacterProcess::MessageBoxProcess(LPARAM lParam)
{
	HRESULT hResult;
	POINT	ptMouse;
	ptMouse.x = LOWORD(lParam);
	ptMouse.y = HIWORD(lParam);
	hResult  = m_xMsgBox.OnButtonUp(ptMouse);
	if((hResult&HAVE_EDITOBOX)==HAVE_EDITOBOX)
	{
		// edit box exist
		switch(hResult^HAVE_EDITOBOX)
		{
		case CLICK_OK:
			{
				m_xMsgBox.HideMessageBox();
/*					switch(m_nMessage)
				{
				}*/
				break;
			}
		case CLICK_YES:
			{
				m_xMsgBox.HideMessageBox();
/*					switch(m_nMessage)
				{
				}*/
				break;
			}
		case CLICK_NO:
			{
				m_xMsgBox.HideMessageBox();
/*					switch(m_nMessage)
				{
				}*/
				break;
			}
		case CLICK_CANCLE:
			{
				m_xMsgBox.HideMessageBox();
/*					switch(m_nMessage)
				{
				}*/
				break;
			}
		}
	}
	else
	{
		// edit box not exist
		switch(hResult)
		{
		case CLICK_OK:
			{
				m_xMsgBox.HideMessageBox();
				switch(m_nMessage)
				{
				case 103:	//
					{
						PostQuitMessage(NULL);
						m_nMessage = 0;
						break;
					}
				}
				break;
			}
		case CLICK_YES:
			{
				m_xMsgBox.HideMessageBox();
				switch(m_nMessage)
				{
					case 5:
					{
						m_CList.MoveCurrentToTop();
						m_CList.MoveNode(m_xSelectChr.m_nSelectChr);
						g_xClientSocket.OnDelChar(m_CList.GetCurrentData()->ChrName);

						g_xClientSocket.OnQueryChar();

						m_ChrCount =0 ;
						break;
					}
				}
				break;
			}
		case CLICK_NO:
			{
				m_xMsgBox.HideMessageBox();
/*					switch(m_nMessage)
				{
				}*/
				break;
			}
		case CLICK_CANCLE:
			{
				m_xMsgBox.HideMessageBox();
/*					switch(m_nMessage)
				{
				}*/
				break;
			}
		}
	}
	return TRUE;
}
