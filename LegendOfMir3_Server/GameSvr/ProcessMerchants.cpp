#include "stdafx.h"

CMirMap* GetMap(char *pszMapName);

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
void WINAPI ProcessMerchants()
{
	DWORD	dwCurrentTick;

	dwCurrentTick = GetTickCount();

	if (!g_fInitMerchant)
	{
		for (int i = 0; i < g_nNumOfMurchantInfo; i++)
		{
			CMirMap* pMap = GetMap(g_pMerchantInfo[i].szMapName);

			if (pMap)
			{
				CMerchantObject* pMerchantObject		= new CMerchantObject;

				pMerchantObject->m_wObjectType			= _OBJECT_NPC;
				pMerchantObject->m_nIndex				= i;

				pMerchantObject->m_tFeature.btGender	= 2;
				pMerchantObject->m_tFeature.btWear		= (BYTE)g_pMerchantInfo[i].sBody;

				pMerchantObject->m_nCurrX				= g_pMerchantInfo[i].nPosX;
				pMerchantObject->m_nCurrY				= g_pMerchantInfo[i].nPosY;
				pMerchantObject->m_nDirection			= g_pMerchantInfo[i].sFace;
				pMerchantObject->m_pMap					= pMap;
				
				memcpy(pMerchantObject->m_szName, g_pMerchantInfo[i].szNPCName, memlen(g_pMerchantInfo[i].szNPCName));

				pMerchantObject->m_pMap->AddNewObject(pMerchantObject->m_nCurrX, pMerchantObject->m_nCurrY, OS_MOVINGOBJECT, pMerchantObject);

//				pMerchantObject->AddRefMsg(RM_TURN, pMerchantObject->m_nDirection, pMerchantObject->m_nCurrX, pMerchantObject->m_nCurrY, 0, pMerchantObject->m_szName);

				g_xMerchantObjList.AddNewNode(pMerchantObject);
			}
		}

		g_fInitMerchant = TRUE;
	}

	if (g_xMerchantObjList.GetCount())
	{
		PLISTNODE pListNode = g_xMerchantObjList.GetHead();

		while (pListNode)
		{
			CMerchantObject* pMerchantObject = g_xMerchantObjList.GetData(pListNode);

			if (dwCurrentTick - pMerchantObject->m_dwRunTime >= pMerchantObject->m_dwRunNextTick)
			{
				pMerchantObject->m_dwRunTime = dwCurrentTick;

				if (dwCurrentTick - pMerchantObject->m_dwSearchTime >= pMerchantObject->m_dwSearchTick)
				{
					pMerchantObject->m_dwSearchTime = dwCurrentTick;
					pMerchantObject->SearchViewRange();
				}

//					DoRunRace(pNPCObject);
				pMerchantObject->RunRace();
			}

			pListNode = g_xMerchantObjList.GetNext(pListNode);
		}
	}
}
