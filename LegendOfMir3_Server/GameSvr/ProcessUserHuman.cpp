#include "stdafx.h"

CPlayerObject* GetUserHuman(char *pszCharName)
{
	if (g_xUserInfoList.GetCount())
	{
		PLISTNODE pListNode = g_xUserInfoList.GetHead();

		while (pListNode)
		{
			CUserInfo *pUserInfo = g_xUserInfoList.GetData(pListNode);

			if (pUserInfo)
			{
				if (pUserInfo->m_pxPlayerObject)
				{
					if (memcmp(pUserInfo->m_szCharName, pszCharName, memlen(pUserInfo->m_szCharName) - 1) == 0)
					{
						if (!pUserInfo->m_pxPlayerObject->m_fIsDead)
							return (CPlayerObject*)pUserInfo->m_pxPlayerObject;
					}
				}
			}

			pListNode = g_xUserInfoList.GetNext(pListNode);
		}
	}

	return NULL;
}

void CryCry(UINT wMsgType, CMirMap* pMap, int nX, int nY, int nWide, char *pszSaying)
{
	if (g_xUserInfoList.GetCount())
	{
		PLISTNODE pListNode = g_xUserInfoList.GetHead();

		while (pListNode)
		{
			CUserInfo *pUserInfo = g_xUserInfoList.GetData(pListNode);

			if (pUserInfo)
			{
				if (pUserInfo->m_pxPlayerObject)
				{
					if ((pUserInfo->m_pxPlayerObject->m_pMap == pMap) && !pUserInfo->m_pxPlayerObject->m_fIsDead)
					{
						if ((abs(pUserInfo->m_pxPlayerObject->m_nCurrX - nX) < nWide) && (abs(pUserInfo->m_pxPlayerObject->m_nCurrY - nY) < nWide))
							pUserInfo->m_pxPlayerObject->AddProcess(NULL, wMsgType, 0, 0, 0, 0, pszSaying);
					}
				}
			}

			pListNode = g_xUserInfoList.GetNext(pListNode);
		}
	}
}

UINT WINAPI ProcessUserHuman(LPVOID lpParameter)
{
	PLISTNODE pListNode = NULL;

	while (TRUE)
	{
		if (g_fTerminated) 
		{
			if (g_xUserInfoList.GetCount())
			{
				pListNode = g_xUserInfoList.GetHead();

				while (pListNode)
				{
					CUserInfo *pUserInfo = g_xUserInfoList.GetData(pListNode);

					if (pUserInfo)
						pUserInfo->CloseUserHuman();

					pListNode = g_xUserInfoList.RemoveNode(pListNode);
				}
			}

			return 0L;
		}

		if (g_xUserInfoList.GetCount())
		{
			pListNode = g_xUserInfoList.GetHead();

			while (pListNode)
			{
				CUserInfo *pUserInfo = g_xUserInfoList.GetData(pListNode);

				if (pUserInfo)
				{
					if (pUserInfo->m_btCurrentMode == USERMODE_PLAYGAME)
						pUserInfo->Operate();
				}

				pListNode = g_xUserInfoList.GetNext(pListNode);
			} // while
		} // if g_xReadyUserInfoList.GetCount()

		if (g_xGateList.GetCount())
		{
			pListNode = g_xGateList.GetHead();

			while (pListNode)
			{
				CGateInfo *pGateInfo = g_xGateList.GetData(pListNode);

				if (pGateInfo)
					pGateInfo->xSend();

				pListNode = g_xUserInfoList.GetNext(pListNode);
			} // while
		}

		SleepEx(1, TRUE);
	}

//	return 0L;
}
