#include "stdafx.h"

void WINAPI ProcessEvents()
{
	if (g_xEventList.GetCount())
	{
		PLISTNODE pListNode = g_xEventList.GetHead();

		while (pListNode)
		{
			CEvent* pEvent = g_xEventList.GetData(pListNode);

			if (pEvent->m_fActive && (GetTickCount() - pEvent->m_dwRunStart > pEvent->m_dwRunTick))
			{
				pEvent->m_dwRunStart = GetTickCount();
				pEvent->Run();

				if (pEvent->m_fClosed)
				{
					g_xEventCloseList.AddNewNode(pEvent);
					pListNode = g_xEventList.RemoveNode(pListNode);
					
					continue;
				}
			}

			pListNode = g_xEventList.GetNext(pListNode);
		}
	}

	if (g_xEventCloseList.GetCount())
	{
		PLISTNODE pListNode = g_xEventCloseList.GetHead();

		while (pListNode)
		{
			CEvent* pEvent = g_xEventCloseList.GetData(pListNode);
	
			if (GetTickCount() - pEvent->m_dwCloseTime > 5 * 60 * 1000)
			{
				delete pEvent;
				pEvent = NULL;

				pListNode = g_xEventCloseList.RemoveNode(pListNode);
				continue;	// 이전 버젼은 한번에 한개씩만 지웠음.
			}

			pListNode = g_xEventCloseList.GetNext(pListNode);
		}
	}

	// 결계
	if (g_xHolySeizeList.GetCount())
	{
		PLISTNODE pListNode = g_xHolySeizeList.GetHead();

		while (pListNode)
		{
			CHolySeizeInfo* pHolySeizeInfo = g_xHolySeizeList.GetData(pListNode);

			if (pHolySeizeInfo->SeizeList.GetCount())
			{
				PLISTNODE pNode = pHolySeizeInfo->SeizeList.GetHead();
				
				while (pNode)
				{
					CMonsterObject* pMonsterObject = (CMonsterObject*)pHolySeizeInfo->SeizeList.GetData(pNode);

					if (pMonsterObject->m_fIsDead || pMonsterObject->m_fIsGhost || !pMonsterObject->m_fBoHolySeize)
						pNode = pHolySeizeInfo->SeizeList.RemoveNode(pNode);
					else
						pNode = pHolySeizeInfo->SeizeList.GetNext(pNode);
				}
			}

			if ((pHolySeizeInfo->SeizeList.GetCount() <= 0) ||
				(GetTickCount() - pHolySeizeInfo->dwOpenTime > pHolySeizeInfo->dwSeizeTime) ||
				(GetTickCount() - pHolySeizeInfo->dwOpenTime > 3 * 60 * 1000))
			{
				pHolySeizeInfo->SeizeList.Clear();

				for (int i = 0; i < 8; i++)
				{
					if (pHolySeizeInfo->Event[i]) 
						pHolySeizeInfo->Event[i]->Close();
				}

				delete pHolySeizeInfo;
				pHolySeizeInfo = NULL;

				pListNode = g_xHolySeizeList.RemoveNode(pListNode);

				continue;
			}

			pListNode = g_xHolySeizeList.GetNext(pListNode);
		}
	}
}
