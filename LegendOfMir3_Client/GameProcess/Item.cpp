/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/


#include "StdAfx.h"


CItem::CItem()
{
	m_shCellNum	= 0;
	m_bAttr		= 0;
	ZeroMemory(&m_stItemInfo, sizeof(CLIENTITEMRCD));
}


CItem::~CItem()
{
}


BOOL CItem::SetItem(CLIENTITEMRCD stItemInfo, SHORT shInventoryNum)
{
	m_stItemInfo = stItemInfo;
	m_shCellNum = shInventoryNum;
/*
a:무기
b:옷
c:반지 팔찌 목걸이
g:기타
*/
/*
#define _U_DRESS					0
#define _U_WEAPON					1
#define _U_RIGHTHAND				2
#define _U_NECKLACE					3
#define _U_HELMET					4
#define _U_ARMRINGL					5
#define _U_ARMRINGR					6            
#define _U_RINGL					7 
#define _U_RINGR					8 
#define _U_CHARM					9
#define _U_UNEQUIP					100
*/

	switch ( m_stItemInfo.stStdItem.bStdMode )
	{
	// 무기.
	case 5:		
	case 6:		
		m_bAttr = _U_WEAPON;
		break;
	// 옷.
	case 10:
	case 11:
		m_bAttr = _U_DRESS;
		break;
	case 15:
		m_bAttr = _U_HELMET;
		break;
	case 19:
	case 20:
	case 21:
		m_bAttr = _U_NECKLACE;
		break;
	case 22:
	case 23:
		m_bAttr = _U_RINGL;
		break;
	case 24:
	case 26:
		m_bAttr = _U_ARMRINGL;
		break;
	// 초, 횃불.
	case 30:
		m_bAttr = _U_RIGHTHAND;
		break;
	// 독가루, 부적.
	case 25:
		m_bAttr = _U_ARMRINGL;
		break;
	}

	return TRUE;
}


BOOL CItem::DrawItem(INT nX, INT nY, BYTE bItemType)
{
	CWHWilImageData*	pxItemImg;	
	BOOL bDrawItem = FALSE;
	switch ( bItemType )
	{
	case _ITEM_TYPE_INVENTORY:
		pxItemImg = &g_xGameProc.m_xImage.m_xImageList[_IMAGE_ITEM_INVENTORY];
		bDrawItem = pxItemImg->NewSetIndex(m_stItemInfo.stStdItem.wLooks);
		if ( bDrawItem )
		{
			nX = nX+_INVENTORY_CELL_WIDTH/2-pxItemImg->m_lpstNewCurrWilImageInfo->shWidth/2;
			nY = nY+_INVENTORY_CELL_HEIGHT/2-pxItemImg->m_lpstNewCurrWilImageInfo->shHeight/2;
		}
		break;
	case _ITEM_TYPE_EQUIP:
		pxItemImg = &g_xGameProc.m_xImage.m_xImageList[_IMAGE_ITEM_EQUIP];
		bDrawItem = pxItemImg->NewSetIndex(m_stItemInfo.stStdItem.wLooks);
		if ( bDrawItem )
		{
			nX += g_xGameProc.m_xImage.m_xImageList[_IMAGE_ITEM_EQUIP].m_lpstNewCurrWilImageInfo->shPX;
			nY += g_xGameProc.m_xImage.m_xImageList[_IMAGE_ITEM_EQUIP].m_lpstNewCurrWilImageInfo->shPY;
		}
		break;
	}
		
	if ( bDrawItem )
	{
 		g_xMainWnd.DrawWithImageForCompClipRgn(
						nX,
						nY,
						pxItemImg->m_lpstNewCurrWilImageInfo->shWidth,
						pxItemImg->m_lpstNewCurrWilImageInfo->shHeight,
						(WORD*)pxItemImg->m_pbCurrImage, _CLIP_WIDTH, 600);
	}
	return TRUE;
}


VOID CItem::ShowItemStatus(INT nX, INT nY)
{
	CHAR	szLineBuf[3][MAX_PATH];
	CHAR	szName[MAX_PATH];
	SIZE	sizeLen;
	BOOL	bUseAble = TRUE;
	const INT nLineGap = 14;

	ZeroMemory(szName, MAX_PATH);
	ZeroMemory(szLineBuf, 3*MAX_PATH);

	if ( m_stItemInfo.stStdItem.szName )
	{
		switch ( m_stItemInfo.stStdItem.bStdMode )
		{
		// 남자옷, 여자옷.
		case 10:
		case 11:
			{
				bUseAble = FALSE;

				// 첫째줄.
				if ( m_stItemInfo.stStdItem.szName[20] )
					g_xMainWnd.StringPlus(szName, &m_stItemInfo.stStdItem.szName[20], " ", &m_stItemInfo.stStdItem.szName[0], "");
				else
					strcpy(szName, m_stItemInfo.stStdItem.szName);
				g_xMainWnd.PutsHan(NULL, nX, nY, RGB(250, 250, 0), RGB(0, 0, 0), szName);
				sizeLen = g_xMainWnd.GetStrLength(NULL, NULL, szName);

				sprintf(szLineBuf[0], " 무게%d 내구%d/%d", (m_stItemInfo.stStdItem.bWeight), (m_stItemInfo.nDura/1000), (m_stItemInfo.nDuraMax)/1000);

				g_xMainWnd.PutsHan(NULL, nX+sizeLen.cx, nY, RGB(250, 250, 250), RGB(0, 0, 0), szLineBuf[0]);
				

				// 둘째줄.
				if ( m_stItemInfo.stStdItem.wAC > 0 )
					sprintf(&szLineBuf[1][strlen(szLineBuf[1])], "방어%d-%d ", LOBYTE(m_stItemInfo.stStdItem.wAC), HIBYTE(m_stItemInfo.stStdItem.wAC));

				if ( m_stItemInfo.stStdItem.wMAC > 0 )
					sprintf(&szLineBuf[1][strlen(szLineBuf[1])], "마항%d-%d ", LOBYTE(m_stItemInfo.stStdItem.wMAC), HIBYTE(m_stItemInfo.stStdItem.wMAC));

				if ( m_stItemInfo.stStdItem.wDC > 0 )
					sprintf(&szLineBuf[1][strlen(szLineBuf[1])], "파괴%d-%d ", LOBYTE(m_stItemInfo.stStdItem.wDC), HIBYTE(m_stItemInfo.stStdItem.wDC));

				if ( m_stItemInfo.stStdItem.wMC > 0 )
					sprintf(&szLineBuf[1][strlen(szLineBuf[1])], "마법%d-%d ", LOBYTE(m_stItemInfo.stStdItem.wMC), HIBYTE(m_stItemInfo.stStdItem.wMC));

				if ( m_stItemInfo.stStdItem.wSC > 0 )
					sprintf(&szLineBuf[1][strlen(szLineBuf[1])], "도력%d-%d ", LOBYTE(m_stItemInfo.stStdItem.wSC), HIBYTE(m_stItemInfo.stStdItem.wSC));

				g_xMainWnd.PutsHan(NULL, nX, nY+nLineGap, RGB(250, 250, 250), RGB(0, 0, 0), szLineBuf[1]);

				// 세째줄.
				switch ( m_stItemInfo.stStdItem.bNeed )
				{
				case 0:
					{
						if ( g_xGameProc.m_xMyHero.m_stAbility.bLevel >= m_stItemInfo.stStdItem.bNeedLevel )
						   bUseAble = TRUE;
						sprintf(&szLineBuf[2][strlen(szLineBuf[2])], "필요레벨%d ", m_stItemInfo.stStdItem.bNeedLevel);
					}
					break;
				case 1:
					{
						if ( HIBYTE(g_xGameProc.m_xMyHero.m_stAbility.wDC) >= m_stItemInfo.stStdItem.bNeedLevel )
						   bUseAble = TRUE;
						sprintf(&szLineBuf[2][strlen(szLineBuf[2])], "필요파괴력%d ", m_stItemInfo.stStdItem.bNeedLevel);
					}
					break;
				case 2:
					{
						if ( HIBYTE(g_xGameProc.m_xMyHero.m_stAbility.wMC) >= m_stItemInfo.stStdItem.bNeedLevel )
						   bUseAble = TRUE;
						sprintf(&szLineBuf[2][strlen(szLineBuf[2])], "필요마법력%d ", m_stItemInfo.stStdItem.bNeedLevel);
					}
					break;
				case 3:
					{
						if ( HIBYTE(g_xGameProc.m_xMyHero.m_stAbility.wSC) >= m_stItemInfo.stStdItem.bNeedLevel )
						   bUseAble = TRUE;
						sprintf(&szLineBuf[2][strlen(szLineBuf[2])], "필요도력%d ", m_stItemInfo.stStdItem.bNeedLevel);
					}
					break;
				}

				if ( bUseAble )
					g_xMainWnd.PutsHan(NULL, nX, nY+nLineGap*2, RGB(250, 250, 250), RGB(0, 0, 0), szLineBuf[2]);
				else
					g_xMainWnd.PutsHan(NULL, nX, nY+nLineGap*2, RGB(250, 0, 0), RGB(0, 0, 0), szLineBuf[2]);
			}
			break;
		// 무기.
		case 5:
		case 6:
			{
				bUseAble = FALSE;

				// 첫째줄.
				if ( (m_stItemInfo.stStdItem.bNeedIdentify & 0X01) != 0 )			
				{
					if ( m_stItemInfo.stStdItem.szName[20] )
						g_xMainWnd.StringPlus(szName, "(*)", &m_stItemInfo.stStdItem.szName[20], " ", &m_stItemInfo.stStdItem.szName[0], "");
					else
						strcpy(szName, m_stItemInfo.stStdItem.szName);
				}
				else																
				{
					if ( m_stItemInfo.stStdItem.szName[20] )
						g_xMainWnd.StringPlus(szName, &m_stItemInfo.stStdItem.szName[20], " ", &m_stItemInfo.stStdItem.szName[0], "");
					else
						strcpy(szName, m_stItemInfo.stStdItem.szName);
				}

				g_xMainWnd.PutsHan(NULL, nX, nY, RGB(250, 250, 0), RGB(0, 0, 0), szName);
				sizeLen = g_xMainWnd.GetStrLength(NULL, NULL, szName);

				sprintf(szLineBuf[0], " 무게%d 내구%d/%d", (m_stItemInfo.stStdItem.bWeight), (m_stItemInfo.nDura/1000), (m_stItemInfo.nDuraMax)/1000);
				g_xMainWnd.PutsHan(NULL, nX+sizeLen.cx, nY, RGB(250, 250, 250), RGB(0, 0, 0), szLineBuf[0]);

				// 둘째줄.
				if ( m_stItemInfo.stStdItem.wDC > 0 )
					sprintf(&szLineBuf[1][strlen(szLineBuf[1])], "파괴%d-%d ", LOBYTE(m_stItemInfo.stStdItem.wDC), HIBYTE(m_stItemInfo.stStdItem.wDC));

				if ( m_stItemInfo.stStdItem.wMC > 0 )
					sprintf(&szLineBuf[1][strlen(szLineBuf[1])], "마법%d-%d ", LOBYTE(m_stItemInfo.stStdItem.wMC), HIBYTE(m_stItemInfo.stStdItem.wMC));

				if ( m_stItemInfo.stStdItem.wSC > 0 )
					sprintf(&szLineBuf[1][strlen(szLineBuf[1])], "도력%d-%d ", LOBYTE(m_stItemInfo.stStdItem.wSC), HIBYTE(m_stItemInfo.stStdItem.wSC));

				if ( m_stItemInfo.stStdItem.sSpecialPower > 0 )
					sprintf(&szLineBuf[1][strlen(szLineBuf[1])], "강도%d ", m_stItemInfo.stStdItem.sSpecialPower);

				g_xMainWnd.PutsHan(NULL, nX, nY+nLineGap, RGB(250, 250, 250), RGB(0, 0, 0), szLineBuf[1]);

				// 세째줄.
				if ( HIBYTE(m_stItemInfo.stStdItem.wAC) > 0 )
					sprintf(&szLineBuf[2][strlen(szLineBuf[2])], "정확+%d ", HIBYTE(m_stItemInfo.stStdItem.wAC));

				if ( HIBYTE(m_stItemInfo.stStdItem.wMAC) > 0 )
				{
					if ( HIBYTE(m_stItemInfo.stStdItem.wMAC) > 10 )
						sprintf(&szLineBuf[2][strlen(szLineBuf[2])], "공격속도+%d ", HIBYTE(m_stItemInfo.stStdItem.wMAC)-10);
					else
						sprintf(&szLineBuf[2][strlen(szLineBuf[2])], "공격속도-%d ", HIBYTE(m_stItemInfo.stStdItem.wMAC));
				}

				if ( LOBYTE(m_stItemInfo.stStdItem.wAC) > 0 )
					sprintf(&szLineBuf[2][strlen(szLineBuf[2])], "행운+%d ", LOBYTE(m_stItemInfo.stStdItem.wAC));

				if ( LOBYTE(m_stItemInfo.stStdItem.wMAC) > 0 )
					sprintf(&szLineBuf[2][strlen(szLineBuf[2])], "저주+%d ", LOBYTE(m_stItemInfo.stStdItem.wMAC));

				switch ( m_stItemInfo.stStdItem.bNeed )
				{
				case 0:
					{
						if ( g_xGameProc.m_xMyHero.m_stAbility.bLevel >= m_stItemInfo.stStdItem.bNeedLevel )
						   bUseAble = TRUE;
						sprintf(&szLineBuf[2][strlen(szLineBuf[2])], "필요레벨%d ", m_stItemInfo.stStdItem.bNeedLevel);
					}
					break;
				case 1:
					{
						if ( HIBYTE(g_xGameProc.m_xMyHero.m_stAbility.wDC) >= m_stItemInfo.stStdItem.bNeedLevel )
						   bUseAble = TRUE;
						sprintf(&szLineBuf[2][strlen(szLineBuf[2])], "필요파괴력%d ", m_stItemInfo.stStdItem.bNeedLevel);
					}
					break;
				case 2:
					{
						if ( HIBYTE(g_xGameProc.m_xMyHero.m_stAbility.wMC) >= m_stItemInfo.stStdItem.bNeedLevel )
						   bUseAble = TRUE;
						sprintf(&szLineBuf[2][strlen(szLineBuf[2])], "필요마법력%d ", m_stItemInfo.stStdItem.bNeedLevel);
					}
					break;
				case 3:
					{
						if ( HIBYTE(g_xGameProc.m_xMyHero.m_stAbility.wSC) >= m_stItemInfo.stStdItem.bNeedLevel )
						   bUseAble = TRUE;
						sprintf(&szLineBuf[2][strlen(szLineBuf[2])], "필요도력%d ", m_stItemInfo.stStdItem.bNeedLevel);
					}
					break;
				}

				if ( bUseAble )
					g_xMainWnd.PutsHan(NULL, nX, nY+nLineGap*2, RGB(250, 250, 250), RGB(0, 0, 0), szLineBuf[2]);
				else
					g_xMainWnd.PutsHan(NULL, nX, nY+nLineGap*2, RGB(250, 0, 0), RGB(0, 0, 0), szLineBuf[2]);

			}
			break;			
        case 15:	//모자,투구
        case 19:	//목걸이
		case 20:
		case 21:  
        case 22:	//반지
		case 23:  
        case 24:	//팔찌
		case 26:  
			{
				bUseAble = FALSE;

				// 첫째줄.
				if ( m_stItemInfo.stStdItem.szName[20] )
					g_xMainWnd.StringPlus(szName, &m_stItemInfo.stStdItem.szName[20], " ", &m_stItemInfo.stStdItem.szName[0], "");
				else
					strcpy(szName, m_stItemInfo.stStdItem.szName);
				g_xMainWnd.PutsHan(NULL, nX, nY, RGB(250, 250, 0), RGB(0, 0, 0), szName);
				sizeLen = g_xMainWnd.GetStrLength(NULL, NULL, szName);

				sprintf(szLineBuf[0], " 무게%d 내구%d/%d", (m_stItemInfo.stStdItem.bWeight), (m_stItemInfo.nDura/1000), (m_stItemInfo.nDuraMax)/1000);

				g_xMainWnd.PutsHan(NULL, nX+sizeLen.cx, nY, RGB(250, 250, 250), RGB(0, 0, 0), szLineBuf[0]);

				// 두번째줄.
				// 목걸이.
				if ( m_stItemInfo.stStdItem.bStdMode == 19 && m_stItemInfo.stStdItem.bStdMode == 20 )
				{
					if ( m_stItemInfo.stStdItem.wAC > 0 )
						sprintf(&szLineBuf[1][strlen(szLineBuf[1])], "마법회피+%d ", HIBYTE(m_stItemInfo.stStdItem.wAC));

					if ( LOBYTE(m_stItemInfo.stStdItem.wMAC) > 0 )
						sprintf(&szLineBuf[1][strlen(szLineBuf[1])], "저주+%d ", LOBYTE(m_stItemInfo.stStdItem.wMAC));

					if ( HIBYTE(m_stItemInfo.stStdItem.wMAC) > 0 )
						sprintf(&szLineBuf[1][strlen(szLineBuf[1])], "행운+%d ", HIBYTE(m_stItemInfo.stStdItem.wMAC));		
				}				
				// 반지.
				if ( m_stItemInfo.stStdItem.bStdMode == 22 && m_stItemInfo.stStdItem.bStdMode == 23 )
				{
					if ( HIBYTE(m_stItemInfo.stStdItem.wMAC) > 0 )
						sprintf(&szLineBuf[1][strlen(szLineBuf[1])], "중독회복+%d ", HIBYTE(m_stItemInfo.stStdItem.wMAC));

					if ( LOBYTE(m_stItemInfo.stStdItem.wAC) > 0 )
						sprintf(&szLineBuf[1][strlen(szLineBuf[1])], "공격속도+%d ", LOBYTE(m_stItemInfo.stStdItem.wAC));
					
					if ( LOBYTE(m_stItemInfo.stStdItem.wMAC) > 0 )
						sprintf(&szLineBuf[1][strlen(szLineBuf[1])], "공격속도-%d ", LOBYTE(m_stItemInfo.stStdItem.wMAC));
				}
				// 팔찌.
				else if ( m_stItemInfo.stStdItem.bStdMode == 24 && m_stItemInfo.stStdItem.bStdMode == 26 )
				{
					if ( m_stItemInfo.stStdItem.wAC > 0 )
						sprintf(&szLineBuf[1][strlen(szLineBuf[1])], "정확+%d ", HIBYTE(m_stItemInfo.stStdItem.wAC));
			
					if ( m_stItemInfo.stStdItem.wMAC > 0 )
						sprintf(&szLineBuf[1][strlen(szLineBuf[1])], "민첩+%d ", HIBYTE(m_stItemInfo.stStdItem.wMAC));
				}
				else
				{

					if ( m_stItemInfo.stStdItem.wAC > 0 )
						sprintf(&szLineBuf[1][strlen(szLineBuf[1])], "방어%d-%d ", LOBYTE(m_stItemInfo.stStdItem.wAC), HIBYTE(m_stItemInfo.stStdItem.wAC));
					
					if ( m_stItemInfo.stStdItem.wMAC > 0 )
						sprintf(&szLineBuf[1][strlen(szLineBuf[1])], "마항%d-%d ", LOBYTE(m_stItemInfo.stStdItem.wMAC), HIBYTE(m_stItemInfo.stStdItem.wMAC));
				}

				if ( m_stItemInfo.stStdItem.wDC > 0 )
					sprintf(&szLineBuf[1][strlen(szLineBuf[1])], "파괴%d-%d ", LOBYTE(m_stItemInfo.stStdItem.wDC), HIBYTE(m_stItemInfo.stStdItem.wDC));

				if ( m_stItemInfo.stStdItem.wMC > 0 )
					sprintf(&szLineBuf[1][strlen(szLineBuf[1])], "마법%d-%d ", LOBYTE(m_stItemInfo.stStdItem.wMC), HIBYTE(m_stItemInfo.stStdItem.wMC));

				if ( m_stItemInfo.stStdItem.wSC > 0 )
					sprintf(&szLineBuf[1][strlen(szLineBuf[1])], "도력%d-%d ", LOBYTE(m_stItemInfo.stStdItem.wSC), HIBYTE(m_stItemInfo.stStdItem.wSC));

				g_xMainWnd.PutsHan(NULL, nX, nY+nLineGap, RGB(250, 250, 250), RGB(0, 0, 0), szLineBuf[1]);

				// 세번째줄.
				switch ( m_stItemInfo.stStdItem.bNeed )
				{
				case 0:
					{
						if ( g_xGameProc.m_xMyHero.m_stAbility.bLevel >= m_stItemInfo.stStdItem.bNeedLevel )
						   bUseAble = TRUE;
						sprintf(&szLineBuf[2][strlen(szLineBuf[2])], "필요레벨%d ", m_stItemInfo.stStdItem.bNeedLevel);
					}
					break;
				case 1:
					{
						if ( HIBYTE(g_xGameProc.m_xMyHero.m_stAbility.wDC) >= m_stItemInfo.stStdItem.bNeedLevel )
						   bUseAble = TRUE;
						sprintf(&szLineBuf[2][strlen(szLineBuf[2])], "필요파괴력%d ", m_stItemInfo.stStdItem.bNeedLevel);
					}
					break;
				case 2:
					{
						if ( HIBYTE(g_xGameProc.m_xMyHero.m_stAbility.wMC) >= m_stItemInfo.stStdItem.bNeedLevel )
						   bUseAble = TRUE;
						sprintf(&szLineBuf[2][strlen(szLineBuf[2])], "필요마법력%d ", m_stItemInfo.stStdItem.bNeedLevel);
					}
					break;
				case 3:
					{
						if ( HIBYTE(g_xGameProc.m_xMyHero.m_stAbility.wSC) >= m_stItemInfo.stStdItem.bNeedLevel )
						   bUseAble = TRUE;
						sprintf(&szLineBuf[2][strlen(szLineBuf[2])], "필요도력%d ", m_stItemInfo.stStdItem.bNeedLevel);
					}
					break;
				}

				if ( bUseAble )
					g_xMainWnd.PutsHan(NULL, nX, nY+nLineGap*2, RGB(250, 250, 250), RGB(0, 0, 0), szLineBuf[2]);
				else
					g_xMainWnd.PutsHan(NULL, nX, nY+nLineGap*2, RGB(250, 0, 0), RGB(0, 0, 0), szLineBuf[2]);
			}
			break;

/*		case 25:	//뿌리는 독가루
		case 30:	//초,횟불
		case 40:	//고기덩어리
		case 42:
		case 43:
*/		default:
			{
				bUseAble = FALSE;

				// 첫번째줄.
				g_xMainWnd.PutsHan(NULL, nX, nY, RGB(250, 250, 0), RGB(0, 0, 0), m_stItemInfo.stStdItem.szName);
				sizeLen = g_xMainWnd.GetStrLength(NULL, NULL, m_stItemInfo.stStdItem.szName);

				switch ( m_stItemInfo.stStdItem.bStdMode )
				{
				case 0:			// 물약.
					if ( m_stItemInfo.stStdItem.wAC > 0 )
						sprintf(&szLineBuf[0][strlen(szLineBuf[0])], " +%dHP ", m_stItemInfo.stStdItem.wAC);

					if ( m_stItemInfo.stStdItem.wMAC > 0 )
						sprintf(&szLineBuf[0][strlen(szLineBuf[0])], " +%dMP ", m_stItemInfo.stStdItem.wMAC);

					sprintf(&szLineBuf[0][strlen(szLineBuf[0])], " 무게%d ", m_stItemInfo.stStdItem.bWeight);

					g_xMainWnd.PutsHan(NULL, nX+sizeLen.cx, nY, RGB(250, 250, 250), RGB(0, 0, 0), szLineBuf[0]);

					break;
				case 1:			// 말린고기.
				case 2:			// 주먹밥.
					break;
				case 3:			// 특수한물약 OR 각종전서들.
					sprintf(&szLineBuf[0][strlen(szLineBuf[0])], " 무게%d ", m_stItemInfo.stStdItem.bWeight);
					g_xMainWnd.PutsHan(NULL, nX+sizeLen.cx, nY, RGB(250, 250, 250), RGB(0, 0, 0), szLineBuf[0]);
					break;
				case 4:			// 각종책들.
					sprintf(&szLineBuf[0][strlen(szLineBuf[0])], " 무게%d ", m_stItemInfo.stStdItem.bWeight);
					g_xMainWnd.PutsHan(NULL, nX+sizeLen.cx, nY, RGB(250, 250, 250), RGB(0, 0, 0), szLineBuf[0]);

					switch ( m_stItemInfo.stStdItem.bShape )
					{
					case 0:
						{
							// 둘째줄.
							sprintf(&szLineBuf[1][strlen(szLineBuf[1])], " 전사무공비급 ");
							g_xMainWnd.PutsHan(NULL, nX, nY+nLineGap, RGB(250, 250, 250), RGB(0, 0, 0), szLineBuf[1]);

							sprintf(&szLineBuf[2][strlen(szLineBuf[2])], " 필요레벨%d ", m_stItemInfo.stStdItem.wDuraMax);

							if ( (g_xGameProc.m_xMyHero.m_bJob == _JOB_JUNSA) && (g_xGameProc.m_xMyHero.m_stAbility.bLevel >= m_stItemInfo.stStdItem.wDuraMax) )
							   bUseAble = TRUE;
							if ( bUseAble )
								g_xMainWnd.PutsHan(NULL, nX, nY+nLineGap*2, RGB(250, 250, 250), RGB(0, 0, 0), szLineBuf[2]);
							else
								g_xMainWnd.PutsHan(NULL, nX, nY+nLineGap*2, RGB(250, 0, 0), RGB(0, 0, 0), szLineBuf[2]);
						}
						break;
					case 1:
						{
							// 둘째줄.
							sprintf(&szLineBuf[1][strlen(szLineBuf[1])], " 주술사마법책 ");
							g_xMainWnd.PutsHan(NULL, nX, nY+nLineGap, RGB(250, 250, 250), RGB(0, 0, 0), szLineBuf[1]);

							sprintf(&szLineBuf[2][strlen(szLineBuf[2])], " 필요레벨%d ", m_stItemInfo.stStdItem.wDuraMax);

							if ( (g_xGameProc.m_xMyHero.m_bJob == _JOB_SULSA) && (g_xGameProc.m_xMyHero.m_stAbility.bLevel >= m_stItemInfo.stStdItem.wDuraMax) )
							   bUseAble = TRUE;

							if ( bUseAble )
								g_xMainWnd.PutsHan(NULL, nX, nY+nLineGap*2, RGB(250, 250, 250), RGB(0, 0, 0), szLineBuf[2]);
							else
								g_xMainWnd.PutsHan(NULL, nX, nY+nLineGap*2, RGB(250, 0, 0), RGB(0, 0, 0), szLineBuf[2]);
						}
						break;
					case 2:
						{
							sprintf(&szLineBuf[1][strlen(szLineBuf[1])], " 도사무공비급 ");
							g_xMainWnd.PutsHan(NULL, nX, nY+nLineGap, RGB(250, 250, 250), RGB(0, 0, 0), szLineBuf[1]);

							sprintf(&szLineBuf[2][strlen(szLineBuf[2])], " 필요레벨%d ", m_stItemInfo.stStdItem.wDuraMax);

							if ( (g_xGameProc.m_xMyHero.m_bJob == _JOB_DOSA) && (g_xGameProc.m_xMyHero.m_stAbility.bLevel >= m_stItemInfo.stStdItem.wDuraMax) )
							   bUseAble = TRUE;

							if ( bUseAble )
								g_xMainWnd.PutsHan(NULL, nX, nY+nLineGap*2, RGB(250, 250, 250), RGB(0, 0, 0), szLineBuf[2]);
							else
								g_xMainWnd.PutsHan(NULL, nX, nY+nLineGap*2, RGB(250, 0, 0), RGB(0, 0, 0), szLineBuf[2]);
						}
						break;
					}
					break;						
				case 25:			// 각종 독가루.
					// 첫번째줄.
					sprintf(&szLineBuf[1][strlen(szLineBuf[1])], " 무게%d ", m_stItemInfo.stStdItem.bWeight);
					g_xMainWnd.PutsHan(NULL, nX, nY+nLineGap, RGB(250, 250, 250), RGB(0, 0, 0), szLineBuf[1]);

					// 두번째줄.
					sprintf(&szLineBuf[2][strlen(szLineBuf[2])], " 사용%d/%d ", m_stItemInfo.nDura, m_stItemInfo.nDuraMax);
					g_xMainWnd.PutsHan(NULL, nX, nY+nLineGap*2, RGB(250, 250, 250), RGB(0, 0, 0), szLineBuf[2]);
					break;
				case 30:			// 횃불과 초.
					// 두번째줄.
					sprintf(&szLineBuf[1][strlen(szLineBuf[1])], " 무게%d 내구%d/%d ", m_stItemInfo.stStdItem.bWeight, m_stItemInfo.nDura/1000, m_stItemInfo.nDuraMax/1000);
					g_xMainWnd.PutsHan(NULL, nX, nY+nLineGap, RGB(250, 250, 250), RGB(0, 0, 0), szLineBuf[1]);
					break;
				case 31:			// 각종묶음시리즈.
					// 첫번째줄.
					sprintf(&szLineBuf[1][strlen(szLineBuf[1])], " 무게%d ", m_stItemInfo.stStdItem.bWeight);
					g_xMainWnd.PutsHan(NULL, nX, nY+nLineGap, RGB(250, 250, 250), RGB(0, 0, 0), szLineBuf[1]);
					break;
				case 40:		// 각종고기들(고기덩어리)
					// 두번째줄.
					sprintf(&szLineBuf[1][strlen(szLineBuf[1])], " 무게%d 품질%d/%d ", m_stItemInfo.stStdItem.bWeight, m_stItemInfo.nDura/1000, m_stItemInfo.nDuraMax/1000);
					g_xMainWnd.PutsHan(NULL, nX, nY+nLineGap, RGB(250, 250, 250), RGB(0, 0, 0), szLineBuf[1]);
					break;
				case 42:			// 각종약재들.
					// 두번째줄.
					sprintf(&szLineBuf[1][strlen(szLineBuf[1])], " 무게%d 약재 ", m_stItemInfo.stStdItem.bWeight);
					g_xMainWnd.PutsHan(NULL, nX, nY+nLineGap, RGB(250, 250, 250), RGB(0, 0, 0), szLineBuf[1]);
					break;
				case 43:		// 각종광석들.
					// 두번째줄.
					sprintf(&szLineBuf[1][strlen(szLineBuf[1])], " 무게%d 순도%d ", m_stItemInfo.stStdItem.bWeight, m_stItemInfo.nDura/1000);
					g_xMainWnd.PutsHan(NULL, nX, nY+nLineGap, RGB(250, 250, 250), RGB(0, 0, 0), szLineBuf[1]);
					break;
				case 44:		// 상점에는 없는 특수 아이템.
					break;
				case 45:		// 땅에 떨어질때 바뀌는 기능성 아이템.
					break;
				case 46:		// 기능은 없는 단순아이템.
				default:
					// 두번째줄.
					sprintf(&szLineBuf[1][strlen(szLineBuf[1])], " 무게%d ", m_stItemInfo.stStdItem.bWeight);
					g_xMainWnd.PutsHan(NULL, nX, nY+nLineGap, RGB(250, 250, 250), RGB(0, 0, 0), szLineBuf[1]);
					break;
				}
			}
			break;
		}				
	}
}

