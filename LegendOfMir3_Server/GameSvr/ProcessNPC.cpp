#include "stdafx.h"

void WINAPI ProcessMerchants();
void WINAPI ProcessEvents();

/*
void DoRunRace(CNPCObject* pNPCObject)
{
	switch (pNPCObject->m_wObjectType)
	{
		case _OBJECT_NPC:
			((CChickenObject*)pNPCObject)->RunRace();
			break;
	}
}
*/
UINT WINAPI ProcessNPC(LPVOID lpParameter)
{
	FILETIME					ftKernelTimeStart, ftKernelTimeEnd;
	FILETIME					ftUserTimeStart, ftUserTimeEnd;
	FILETIME					ftDummy, ftTotalTimeElapsed;
	__int64						qwKernelTimeElapsed, qwUserTimeElapsed, qwTotalTimeElapsed;
	TCHAR						wszThreadTime[32];
	
	DWORD						dwCurrentTick;

	int							nHour = 0;

	while (TRUE)
	{
		GetThreadTimes(GetCurrentThread(), &ftDummy, &ftDummy, &ftKernelTimeStart, &ftUserTimeStart);

		if (g_fTerminated) 
		{
			return 0L;
		}

		if (g_xScripterList.GetCount())
		{
			PLISTNODE pListNode = g_xScripterList.GetHead();

			while (pListNode)
			{
				CScripterObject* pObject = g_xScripterList.GetData(pListNode);

				if (GetTickCount() - pObject->m_dwRunTime >= pObject->m_dwRunNextTick)
				{
					pObject->m_dwRunTime = GetTickCount();
					pObject->Operate();
				}

				pListNode = g_xScripterList.GetNext(pListNode);
			}
		}

		//
		ProcessMerchants();

		//
		ProcessEvents();

		//
		dwCurrentTick = GetTickCount();

		if (g_xNPCObjList.GetCount())
		{
			PLISTNODE pListNode = g_xNPCObjList.GetHead();

			while (pListNode)
			{
				CNPCObject* pNPCObject = g_xNPCObjList.GetData(pListNode);

				if (dwCurrentTick - pNPCObject->m_dwRunTime >= pNPCObject->m_dwRunNextTick)
				{
					pNPCObject->m_dwRunTime = dwCurrentTick;

					if (dwCurrentTick - pNPCObject->m_dwSearchTime >= pNPCObject->m_dwSearchTick)
					{
						pNPCObject->m_dwSearchTime = dwCurrentTick;
						pNPCObject->SearchViewRange();
					}

//					DoRunRace(pNPCObject);
//					pNPCObject->RunRace();
				}

				pListNode = g_xNPCObjList.GetNext(pListNode);
			}
		}

		nHour = GetTime();

		if (nHour == 23 || nHour == 11)
			g_nMirDayTime = 2; // Àú³á
		else if (nHour == 4 || nHour == 15)
			g_nMirDayTime = 0; // »õº®
		else if ((nHour >= 0 && nHour <= 3) || (nHour >= 12 && nHour <= 14))
			g_nMirDayTime = 3;
		else
			g_nMirDayTime = 1;

		GetThreadTimes(GetCurrentThread(), &ftDummy, &ftDummy, &ftKernelTimeEnd, &ftUserTimeEnd);

		qwKernelTimeElapsed = FileTimeToQuadWord(&ftKernelTimeEnd) - FileTimeToQuadWord(&ftKernelTimeStart);
		qwUserTimeElapsed	= FileTimeToQuadWord(&ftUserTimeEnd) - FileTimeToQuadWord(&ftUserTimeStart);
		
		qwTotalTimeElapsed = qwKernelTimeElapsed + qwUserTimeElapsed;

		QuadTimeToFileTime(qwTotalTimeElapsed, &ftTotalTimeElapsed);

		wsprintf(wszThreadTime, _TEXT("%lu%lu ns"), ftTotalTimeElapsed.dwHighDateTime, ftTotalTimeElapsed.dwLowDateTime);
		
		SendMessage(g_hStatusBar, SB_SETTEXT, MAKEWORD(5, 0), (LPARAM)wszThreadTime);

		SleepEx(1, TRUE);
	}

	return 0L;
}
