/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/

#include "StdAfx.h"

/////////////////////////////////////////////////////
#define MAX_KINDS_LINE			11
#define MAX_ITEMLIST_LINE		5
#define LINE_GAP				5
/////////////////////////////////////////////////////
#define POS_KINDS_X				44
#define POS_KINDS_Y				43
#define POS_KINDSTITLE_X		POS_KINDS_X+12
#define POS_KINDSTITLE_Y		28
#define POS_ITEMSTITLE_X		POS_ITEMLIST_LINE_X+15
#define POS_ITEMSTITLE_Y		POS_ITEMLIST_LINE_Y

#define POS_ITEMLIST_LINE_X		159
#define POS_ITEMLIST_LINE_Y		90

#define POS_ITEMNAME_X			190
#define POS_ITEMNAME_Y			28
#define POS_ITEMIMAGE_X			188
#define	POS_ITEMIMAGE_Y			56

#define LEFT_KIND_RECT			42
#define RIGHT_KIND_RECT			130
#define TOP_KIND_RECT			25
#define BOTTOM_KIND_RECT		228

#define LEFT_ITEM_RECT			157
#define RIGHT_ITEM_RECT			309
#define TOP_ITEM_RECT			103
#define BOTTOM_ITEM_RECT		199

#define POS_X				84			// ex (전:POS_X,POS_Y)(GOLD:POS__X,POS_Y)
#define POS_Y				246			// ex (무게:POS_X,POS__Y)(가방무게:POS__X,POS__Y)
#define POS__Y				270
#define POS__X				246

#define MAX_MSG_LENGTH			20480			// (20Kb)


CStoreWnd::CStoreWnd()
{
	POINT	Temp;

	m_nIsReadyToDisplay = 0;
	m_nCurrentItemTop = 0;
	m_nCurrentKindTop = 0;
	m_nMaxItemLine = 0;
	m_nMaxKindLine = 0;

	Temp.x = POS_KINDS_X;
	Temp.y = POS_KINDS_Y;


	m_xKindBtn.InitTxtBtn(MAX_KINDS_LINE,Temp,LINE_GAP);

	Temp.x = POS_ITEMLIST_LINE_X;
	Temp.y = POS_ITEMLIST_LINE_Y + 17;

//	tRect.top+POS_ITEMLIST_LINE_Y + 17 + (xtBtn->nLinePos-m_nCurrentItemTop)*(tSize.cy+LINE_GAP);

	m_xItemLstBtn.InitTxtBtn(MAX_ITEMLIST_LINE,Temp,LINE_GAP);
	m_bSelectedKind=FALSE;
	m_bSelectedItem=FALSE;

	m_xKindRect.left = LEFT_KIND_RECT;
	m_xKindRect.top	= TOP_KIND_RECT;
	m_xKindRect.right = RIGHT_KIND_RECT;
	m_xKindRect.bottom = BOTTOM_KIND_RECT;

	m_xItemRect.left = LEFT_ITEM_RECT;
	m_xItemRect.top	= TOP_ITEM_RECT;
	m_xItemRect.right = RIGHT_ITEM_RECT;
	m_xItemRect.bottom = BOTTOM_ITEM_RECT;
}

CStoreWnd::~CStoreWnd()
{
}

VOID CStoreWnd::CreateStoreWnd(INT nID, CWHWilImageData* pxWndImage, INT nFrameImgIdx, INT nStartX, INT nStartY, INT nWidth, INT nHeight, BOOL bCanMove)
{
	CreateGameWnd(nID, pxWndImage, nFrameImgIdx, bCanMove, nStartX, nStartY, nWidth, nHeight);
}


VOID CStoreWnd::ShowStoreWnd()
{
	RECT tRect;
	CMTxtBtn* xtBtn;
	ShowGameWnd();
	OnUpdateQueue();
	if(m_nIsReadyToDisplay>0)
	{
		tRect = GetGameWndRect();
		g_xMainWnd.PutsHan(NULL,tRect.left + POS_KINDSTITLE_X,tRect.top + POS_KINDSTITLE_Y,RGB(255,255,255),RGB(0,0,0)," 상  품  명");
//		if(stdMod=1) 
			g_xMainWnd.PutsHan(NULL,tRect.left + POS_ITEMSTITLE_X,tRect.top + POS_ITEMSTITLE_Y,RGB(255,255,255),RGB(0,0,0)," 구   분          가   격");
		if(m_bSelectedKind)  
		{// Kinds가 선택 되었을때
			g_xMainWnd.PutsHan(NULL,tRect.left + POS_ITEMNAME_X+20,tRect.top + POS_ITEMNAME_Y + 10 ,RGB(255,255,255),RGB(0,0,0),"유월도");

			g_xGameProc.m_xImage.m_xImageList[_IMAGE_ITEM_INVENTORY].NewSetIndex(1); // 해당 아이템 Looks를 넣을것
			g_xMainWnd.DrawWithImageForCompClipRgn(
				tRect.left + POS_ITEMIMAGE_X - (g_xGameProc.m_xImage.m_xImageList[_IMAGE_ITEM_INVENTORY].m_lpstNewCurrWilImageInfo->shWidth / 2),tRect.top + POS_ITEMIMAGE_Y - (g_xGameProc.m_xImage.m_xImageList[_IMAGE_ITEM_INVENTORY].m_lpstNewCurrWilImageInfo->shHeight / 2)
				,g_xGameProc.m_xImage.m_xImageList[_IMAGE_ITEM_INVENTORY].m_lpstNewCurrWilImageInfo->shWidth,g_xGameProc.m_xImage.m_xImageList[_IMAGE_ITEM_INVENTORY].m_lpstNewCurrWilImageInfo->shHeight,
				(WORD*)g_xGameProc.m_xImage.m_xImageList[_IMAGE_ITEM_INVENTORY].m_pbCurrImage,800,600);
		}

		if(m_nIsReadyToDisplay&&1)
		{	// Kinds List 출력
			m_xKindBtn.m_pTxtBtn.MoveCurrentToTop();
			for(int i = 0 ; i <m_xKindBtn.GetButtonCount(); i++)
			{
				xtBtn = m_xKindBtn.m_pTxtBtn.GetCurrentData();
				if((xtBtn->nLinePos>=m_nCurrentKindTop)&&(xtBtn->nLinePos<(m_nCurrentKindTop+MAX_KINDS_LINE)))
				{
					SIZE tSize;
					tSize = g_xMainWnd.GetStrLength(NULL,NULL,xtBtn->Txt);
					g_xMainWnd.PutsHan(NULL,tRect.left + xtBtn->xRect.left +POS_KINDS_X,tRect.top + (xtBtn->nLinePos-m_nCurrentKindTop)*(tSize.cy+LINE_GAP) +POS_KINDS_Y,(!xtBtn->bIsClicked?xtBtn->nColor:xtBtn->cColor),RGB(0,0,0),xtBtn->Txt);
				}
				m_xKindBtn.m_pTxtBtn.MoveNextNode();
			}
		}
		if((m_nIsReadyToDisplay>>1)&&1)
		{	// Item List 출력
//			g_xMainWnd.PutsHan(NULL,tRect.left+POS_ITEMLIST_LINE_X,tRect.top+POS_ITEMLIST_LINE_Y+17,RGB(255,255,0),RGB(0,0,0),"123456789012345678901234567890");
			m_xItemLstBtn.m_pTxtBtn.MoveCurrentToTop();
			for(int i = 0 ; i <m_xItemLstBtn.GetButtonCount(); i++)
			{
				xtBtn = m_xItemLstBtn.m_pTxtBtn.GetCurrentData();
				if((xtBtn->nLinePos>=m_nCurrentItemTop)&&(xtBtn->nLinePos<(m_nCurrentItemTop+MAX_KINDS_LINE)))
				{
					SIZE tSize;
					tSize = g_xMainWnd.GetStrLength(NULL,NULL,xtBtn->Txt);
					g_xMainWnd.PutsHan(NULL,tRect.left+POS_ITEMLIST_LINE_X + xtBtn->xRect.left +80,tRect.top+POS_ITEMLIST_LINE_Y + 17 + (xtBtn->nLinePos-m_nCurrentItemTop)*(tSize.cy+LINE_GAP) ,(!xtBtn->bIsClicked?xtBtn->nColor:xtBtn->cColor),RGB(0,0,0),xtBtn->Txt);
					g_xMainWnd.PutsHan(NULL,tRect.left+POS_ITEMLIST_LINE_X + xtBtn->xRect.left ,tRect.top+POS_ITEMLIST_LINE_Y + 17 + (xtBtn->nLinePos-m_nCurrentItemTop)*(tSize.cy+LINE_GAP) ,(!xtBtn->bIsClicked?xtBtn->nColor:xtBtn->cColor),RGB(0,0,0),xtBtn->sTxt);
				}
				m_xItemLstBtn.m_pTxtBtn.MoveNextNode();
			}
		}
	}
}


BOOL CStoreWnd::OnLButtonUp(POINT ptMouse)
{
//	BOOL	bResult;
	RECT tRect;
	POINT tPos;
//	CHAR*	szTest;
	CMTxtBtn* tBtn;
	tRect = GetGameWndRect();			// Get Window Rect
	tPos.x = tRect.left;
	tPos.y = tRect.top;
	//bResult = ClickCommandProcess(
	if(IsInRect(ptMouse,m_xKindRect))
	{	// 목록 List
		tBtn = m_xKindBtn.ClickCheck(tPos,ptMouse,m_nCurrentKindTop);
		if(tBtn!=NULL)
		{
			// 눌려진 버튼이 메뉴가 있느지를 확인 하고.
			// 메뉴가 없으면 그냥 선텍에서 멈추고 메뉴가 있을경우
			// 하위 메뉴를 요구한다.
			tBtn = m_xKindBtn.ClickCheck(tPos,ptMouse,m_nCurrentKindTop);
			if(tBtn->nColor==RGB(255,255,0))
			{
				// 메뉴가 있냐?
				// 사기 불가능
				SendDeTailGoodsList(0,tBtn->Param);
			}
			else
			{
				// 메규가 없냐?
				// 그럼 그냥 선텍. 사기 가능.
			}
		}
		else
		{
			m_nIsReadyToDisplay = 1;
			m_bSelectedKind= FALSE;
		}
	}
	if(IsInRect(ptMouse,m_xItemRect))
	{	// 항목  List
		tBtn = NULL;
		tBtn = m_xItemLstBtn.ClickCheck(tPos,ptMouse,m_nCurrentItemTop);
		if(tBtn!=NULL)
		{
			// 사기 가능
			m_bSelectedItem = TRUE;
			//임시
			SendButThisITme(tBtn);
		}
		else
		{
			m_bSelectedItem = FALSE;
		}
	}
	return FALSE;
}

VOID CStoreWnd::OnMouseMove(POINT ptMouse)
{

}

BOOL CStoreWnd::OnLButtonDown(POINT ptMouse)
{
	RECT tRect;
	POINT tPos;
	tRect = GetGameWndRect();			// Get Window Rect
	tPos.x = tRect.left;
	tPos.y = tRect.top;

	if(IsInRect(ptMouse,m_xKindRect))
	{
		m_xKindBtn.ClickFreeAllBtn();
		m_xKindBtn.ClickCheck(tPos,ptMouse,m_nCurrentKindTop);
	}
	if(IsInRect(ptMouse,m_xItemRect))
	{
		m_xItemLstBtn.ClickFreeAllBtn();
	}
	return FALSE;
}

BOOL CStoreWnd::SetBuyKinds(CHAR* Msg,INT Count)   // Eatch Button Divider '\n
{
	CNPCTxtAnalysis	tTxtAnalysis;				// 단지 GetValidStr함수를 사용하기 위해서..
	CHAR*	szParam;
	CHAR*	pszStr = new CHAR [MAX_MSG_LENGTH];


	if(m_nIsReadyToDisplay&&1)
	{
		m_xKindBtn.ReleaseButtons();
		m_bSelectedKind = FALSE;
		// 저장된 키 날리기
		m_nIsReadyToDisplay= 0;	// Kinds List 설정이 풀렸음을 알림
	}

	ZeroMemory(pszStr,MAX_MSG_LENGTH);
	strcpy(pszStr,Msg);

	m_nMaxKindLine = Count;

	for(int i = 0 ; i < Count ; i ++)
	{
		CHAR* szTemp;
		CHAR* szGName = new CHAR[MAX_PATH];
		CHAR* szGSubMenu = new CHAR[MAX_PATH];
		CHAR* szGPrice = new CHAR[MAX_PATH];
		CHAR* szGStock = new CHAR[MAX_PATH];
		INT   nSubMenuCount;

		szTemp = tTxtAnalysis.GetValidStr(pszStr,szGName,'/');
		strcpy(pszStr,szTemp);
		szTemp = tTxtAnalysis.GetValidStr(pszStr,szGSubMenu,'/');
		strcpy(pszStr,szTemp);
		nSubMenuCount = atoi(szGSubMenu);
		szTemp = tTxtAnalysis.GetValidStr(pszStr,szGPrice,'/');
		strcpy(pszStr,szTemp);
		szTemp = tTxtAnalysis.GetValidStr(pszStr,szGStock,'/');
		strcpy(pszStr,szTemp);

		szTemp = new CHAR[15];
		szTemp[14]=NULL;
		RECT	tRect;
		SIZE	tSize;
		ZeroMemory(&tRect,sizeof(RECT));	//
		tSize = g_xMainWnd.GetStrLength(NULL,NULL,szGName);
		tRect.top = 0;
		tRect.left = 0;				// List Button 의 경우, Left  = 0;
		tRect.right = tSize.cx;
		tRect.bottom = tSize.cy;
		szParam = new CHAR[35];
		ZeroMemory(szParam,35);
		strcpy(szParam,szGName);	// 임시 Parameter를 아이템이름과 같이 처리함.
		// Parameter...
		if(nSubMenuCount>0)
		{	// Sub Menu 가 존재 하는 경우
			// 메뉴가 존재하면 밝은 노랑색
			sprintf(szTemp,"%-14s",szGName);
			m_xKindBtn.AddButton(szTemp,tRect,i,szParam,RGB(255,255,0),RGB(255,0,0));	// Add Button to class
		}
		else
		{	// Sub Menu 가 존재 하지 않는 경우
			// 메뉴가 존재하지 않으면 어두운 노랑색
			sprintf(szTemp,"%-14s",szGName);
			m_xKindBtn.AddButton(szTemp,tRect,i,szParam,RGB(180,180,0),RGB(255,0,0));	// Add Button to class
		}
//		m_xKindBtn
		delete[] szGName;
		delete[] szGSubMenu;
		delete[] szGPrice;
		delete[] szGStock;
		delete[] szParam;
	}

	m_nIsReadyToDisplay = 1;		// Kinds List가 설정되었음을 알림
	//
	delete[] pszStr;
	return	TRUE;
}

BOOL CStoreWnd::SetBuyItemLst(CHAR* Msg,INT Count)
{
	CNPCTxtAnalysis	tTxtAnalysis;				// 단지 GetValidStr함수를 사용하기 위해서..
	CHAR*	szLastMsg = NULL;
	CHAR	szDetailitem[MAX_PATH];

	m_xItemLstBtn.ReleaseButtons();
	m_xItems.ClearAllNodes();
	// 저장된 키&데이터 날리기
	m_nCurrentItemTop = 0;

	ZeroMemory(szDetailitem,MAX_PATH);

	m_nMaxItemLine = Count;

	INT nCurrLen =0;

	for ( INT nLoop = 0; nLoop < Count; nLoop++ )
	{
		szLastMsg = strchr(Msg+nCurrLen, '/');

		if ( szLastMsg )
		{
			CHAR*	szParam;
			CHAR	bCurrentData[MAX_PATH];
			
			
			CLIENTITEMRCD* pxClItem;
			pxClItem =	new	CLIENTITEMRCD;
			
			memcpy(szDetailitem, Msg + nCurrLen, strlen(Msg)-strlen(szLastMsg));
			
			nCurrLen = nCurrLen + (int)(szLastMsg - Msg)+1;
			
			fnDecode6BitBuf(szDetailitem, (char*)pxClItem, sizeof(CLIENTITEMRCD));
			
			ZeroMemory(bCurrentData,MAX_PATH);
			
			//		szTemp	 =	new CHAR[50];
			CHAR szTemp1[50];	// 가격
			CHAR szTemp2[50];	// 기타
			
			ZeroMemory(szTemp1,50);
			ZeroMemory(szTemp2,50);
			
			m_xItems.AddNode(pxClItem);

//			szMakeIndex[12]; // A:무기 B:옷 C:반지,팔찌,목걸이 G:기타
			sprintf(szTemp1,"%d전",pxClItem->stStdItem.nPrice);
/*			switch(pxClItem->szMakeIndex[0])
			{
			case 'G':	// 기타
			case 'g':
				{
					switch(pxClItem->stStdItem.bStdMode)
					{
					case 0:	// 물약류
					case 1:	// 말린고기
					case 2:	// 주먹밥
					case 3:	// 특수한물약 or 각종전서들
					case 4:	// 각종 책들
					case 7:	// 묶음씨리즈
					case 9:	// 약제들
						break;
					case 5:	// 독가루
						sprintf(szTemp2,"%d",pxClItem->nDura);
						break;
					case 8:	// 먹지 못하는 고기
						sprintf(szTemp2,"품질 %d",pxClItem->nDura);
						break;
					case 10:	// 광석
						sprintf(szTemp2,"순도 %d",pxClItem->nDura);
						break;
					case 6:	// 횃불,초
					default:	// 기타
						sprintf(szTemp2,"내구 %d",pxClItem->nDura);
						break;
					}
					break;
				}
			default:
				sprintf(szTemp2,"내구 %d",pxClItem->nDura);
				break;
			}
*/
			RECT	tRect;
			SIZE	tSize;
			ZeroMemory(&tRect,sizeof(RECT));
			tSize = g_xMainWnd.GetStrLength(NULL,NULL,szTemp1);
			tRect.top = 0;
			tRect.left = 0;											// List Button 의 경우, Left  = 0;
			tRect.right = m_xItemRect.right - m_xItemRect.left;		//
			tRect.bottom = tSize.cy;

			// Parameter ...
			szParam = new CHAR[35];
			ZeroMemory(szParam,35);
//			strcpy(szParam,pxClItem->szMakeIndex);	// 임시 Parameter를 아이템이름과 같이 처리함.

			// Dark Yellow
			m_xItemLstBtn.AddButton(szTemp1,szTemp2,tRect,nLoop,szParam,RGB(180,180,0),RGB(255,0,0));	// Add Button to class

			delete[] szParam;
		}
	}
	m_nIsReadyToDisplay=m_nIsReadyToDisplay|2;		// Item List 설정이 됬음을 알림
		//
	return TRUE;
}

VOID CStoreWnd::OnScrollDown()
{
	POINT tPos;

	tPos = g_xGameProc.m_ptMousePos;
	if(IsInRect(tPos,m_xKindRect))
	{
		m_nCurrentKindTop=m_nCurrentKindTop > 0 ? m_nCurrentKindTop - 1: 0;
	}

	if(IsInRect(tPos,m_xItemRect))
	{
		m_nCurrentItemTop=m_nCurrentItemTop>0 ? m_nCurrentItemTop - 1 : 0;
	}
}

VOID CStoreWnd::OnScrollUp()
{
	POINT tPos;

	tPos = g_xGameProc.m_ptMousePos;
	if(IsInRect(tPos,m_xKindRect))
	{
		m_nCurrentKindTop=m_nCurrentKindTop < m_nMaxKindLine-1 ? m_nCurrentKindTop + 1: m_nMaxKindLine-1;
	}

	if(IsInRect(tPos,m_xItemRect))
	{
		m_nCurrentItemTop=m_nCurrentItemTop < m_nMaxItemLine-1 ? m_nCurrentItemTop + 1 : m_nMaxItemLine-1;
	}
}

BOOL CStoreWnd::SetSellKinds(CInventoryWnd* pxInvenWnd,CHAR Kind,INT nStdMode)
{
	if(pxInvenWnd!=NULL)
	{
	}
	return FALSE;
}

BOOL CStoreWnd::SetSellItemLst(CInventoryWnd* pxInvenWnd,CHAR Kind,BYTE nStdMode,CHAR bKind)
{
	CLIENTITEMRCD*	pxItem;
	RECT	tRect;
	SIZE	tSize;
	CHAR szTemp1[50];
	CHAR szTemp2[50];
	CHAR szParam[50];
	INT nCount=0;
//	if(여관이 아니냐?)
//	{	// 여관이 아니면 걸러서 넣어야 한다.
		if(pxInvenWnd!=NULL)
		{
			for(int i=0 ; i<60; i++)
			{
				pxItem = &pxInvenWnd->m_stInventoryItem[i].xItem.m_stItemInfo;
//				if(bKind==pxItem->szMakeIndex[0])
				{
					if(bKind!='a' && bKind!='A' && bKind!='b' && bKind!='B')
					{
						if(nStdMode==pxItem->stStdItem.bStdMode)
						{	// 기타는 다시 걸러 주어야 한다.
							m_xItems.AddNode(pxItem);
							strcpy(szTemp1,pxItem->stStdItem.szName);
							itoa(pxItem->nDura,szTemp2,10);
							strcpy(szParam,pxItem->stStdItem.szName);

							tSize = g_xMainWnd.GetStrLength(NULL,NULL,szTemp1);
							tRect.top = 0;
							tRect.left = 0;											// List Button 의 경우, Left  = 0;
							tRect.right = m_xItemRect.right - m_xItemRect.left;		//
							tRect.bottom = tSize.cy;

							m_xItemLstBtn.AddButton(szTemp1,szTemp2,tRect,nCount,szParam,RGB(180,180,0),RGB(255,0,0));
							nCount++;
						}
					}
					else
					{
						// 무기나 갑옷은 그냥 넣어도 무관..
						m_xItems.AddNode(pxItem);
						strcpy(szTemp1,pxItem->stStdItem.szName);
						itoa(pxItem->nDura,szTemp2,10);
						strcpy(szParam,pxItem->stStdItem.szName);

						tSize = g_xMainWnd.GetStrLength(NULL,NULL,szTemp1);
						tRect.top = 0;
						tRect.left = 0;											// List Button 의 경우, Left  = 0;
						tRect.right = m_xItemRect.right - m_xItemRect.left;		//
						tRect.bottom = tSize.cy;

						m_xItemLstBtn.AddButton(szTemp1,szTemp2,tRect,nCount,szParam,RGB(180,180,0),RGB(255,0,0));
						nCount++;
					}
				}
			}
		}
//	}
//	else
//	{	// 여관 이면 마구 넣어도 된다.
//		for(int i=0 ; i<60; i++)
//		{
//			pxItem = &pxInvenWnd->m_stInventoryItem[i].xItem.m_stItemInfo;
			// 무기나 갑옷은 그냥 넣어도 무관..
//			m_xItems.AddNode(pxItem);
//			strcpy(szTemp1,pxItem->stStdItem.szName);
//			itoa(pxItem->nDura,szTemp2,10);
//			strcpy(szParam,pxItem->stStdItem.szName);

//			tSize = g_xMainWnd.GetStrLength(NULL,NULL,szTemp1);
//			tRect.top = 0;
//			tRect.left = 0;											// List Button 의 경우, Left  = 0;
//			tRect.right = m_xItemRect.right - m_xItemRect.left;		//
//			tRect.bottom = tSize.cy;
//
//			m_xItemLstBtn.AddButton(szTemp1,szTemp2,tRect,nCount,szParam,RGB(180,180,0),RGB(255,0,0));
//			nCount++;
//		}
//	}
	return FALSE;
}


BOOL CStoreWnd::OnUpdateQueue(VOID)
{
	if(m_xPacketQueue.GetCount()>0)
	{
		LPPACKETMSG	lpPacketMsg;
		lpPacketMsg =(LPPACKETMSG) m_xPacketQueue.PopQ();
		if(lpPacketMsg->stDefMsg.nRecog == m_nNpcID)
		{
			switch(lpPacketMsg->stDefMsg.wIdent)
			{
			case SM_SENDGOODSLIST:
				{
					INT nPos;
					CHAR  szDecodeMsg[MAX_PATH*4];
					nPos = fnDecode6BitBuf(lpPacketMsg->szEncodeData, szDecodeMsg, sizeof(szDecodeMsg));
					SetBuyKinds(szDecodeMsg,lpPacketMsg->stDefMsg.wIdent);
					break;
				}
			case SM_SENDDETAILGOODSLIST:
				{
					SetBuyItemLst(lpPacketMsg->szEncodeData,(lpPacketMsg->stDefMsg).wSeries);	
					break;
				}
			case SM_BUYITEM_FAIL:
				{
					break;
				}
				/*			default :
				{
				m_xPacketQueue.PushQ((BYTE*)lpPacketMsg);
				return FALSE
			}*/
			}
		}
		else
		{
			// 해당 NPC가 아님
			// 해야할것
			// NPC창을 닫을것
			// NPC를 갱신 할것
			m_xPacketQueue.PushQ((BYTE*)lpPacketMsg);
			return FALSE;
		}
		delete lpPacketMsg;
		return TRUE;
	}
	return FALSE;
}

VOID CStoreWnd::SendDeTailGoodsList(INT nIndex,CHAR* szItemName)
{
	g_xClientSocket.SendGetDetailItem(m_nNpcID, nIndex, szItemName);
}

VOID CStoreWnd::SendButThisITme(CMTxtBtn* tBtn)
{
	CHAR	szItemName[MAX_PATH];

	ZeroMemory(szItemName,MAX_PATH);

	m_xItems.MoveCurrentToTop();
	for(INT i = 0 ; i<m_xItems.GetCounter();i++)
	{
//		if(strncmp(m_xItems.GetCurrentData()->szMakeIndex,tBtn->Param,12)==0)
		{
//			g_xClientSocket.SendBuyItem(m_nNpcID,m_xItems.GetCurrentData()->stStdItem.szName,tBtn->Param);					// MOON
			return;
		}
		m_xItems.MoveNextNode();
	}
}