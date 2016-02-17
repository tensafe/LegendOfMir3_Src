#include "stdafx.h"

void CEvent::Create(CMirMap* pMap, int nX, int nY, DWORD dwETime, int nType, BOOL fVisible)
{
	m_dwOpenStartTime	= GetTickCount();
	m_nEventType		= nType;
	m_nEventParam		= 0;
	m_dwContinueTime	= dwETime;
	m_fVisible			= fVisible;
	m_fClosed			= FALSE;
	m_pMap				= pMap;
	m_nX				= nX;
	m_nY				= nY;
	m_fActive			= TRUE;
	m_nDamage			= 0;
	m_pOwnObject		= NULL;

	m_dwRunStart		= GetTickCount();
	m_dwRunTick			= 500;

	if (m_pMap && m_fVisible)
		m_pMap->AddNewObject(m_nX, m_nY, OS_EVENTOBJECT, this);
	else
		m_fVisible = FALSE;
}

void CEvent::Close()
{
	m_dwCloseTime = GetTickCount();

	if (m_fVisible)
	{
		m_fVisible = FALSE;

		if (m_pMap)
			m_pMap->RemoveObject(m_nX, m_nY, OS_EVENTOBJECT, this);

		m_pMap = NULL;
	}
}

void CEvent::Run()
{
	if (GetTickCount() - m_dwOpenStartTime > m_dwContinueTime)
	{
		m_fClosed = TRUE;
		Close();
	}
}

// **************************************************************************************
//
//			지염술 이벤트
//
// **************************************************************************************

void CFireBurnEvent::Create(int nX, int nY, DWORD dwETime, CCharObject* pCharObject, int nDamage)
{
	CEvent::Create(pCharObject->m_pMap, nX, nY, dwETime, ET_FIRE, TRUE);

	m_pOwnObject	= pCharObject;
	m_nDamage		= nDamage;
}

void CFireBurnEvent::Run()
{
	if (GetTickCount() - m_dwTickTime > 3000)
	{
		m_dwTickTime = GetTickCount();

		CWHList<CCharObject*>	ObjectList;

		m_pMap->GetAllObject(m_nX, m_nY, &ObjectList);

		if (ObjectList.GetCount())
		{
			PLISTNODE		pListNode = ObjectList.GetHead();
			CCharObject*	pTargetObject;

			while (pListNode)
			{
				if (pTargetObject = ObjectList.GetData(pListNode))
				{
					if (m_pOwnObject->IsProperTarget(pTargetObject))
					{
						pTargetObject->AddProcess(m_pOwnObject, RM_MAGSTRUCK_MINE, 0, m_nDamage, 0, 0);

#ifdef _DEBUG
						char szMsg[64];

						sprintf(szMsg, "%s 지염술 맞음 - PW:%d HP:%d", pTargetObject->m_szName, m_nDamage, pTargetObject->m_WAbility.HP);
						m_pOwnObject->SysMsg(szMsg, 0);
#endif
					}
				}
			
				pListNode = ObjectList.RemoveNode(pListNode);
			}
		}
	}

	CEvent::Run();
}
