#include "stdafx.h"

void CPlayerCloneObject::Create(CPlayerObject* pPlayerObject)
{
	m_dwRunTime			= GetTickCount();
	m_dwRunNextTick		= 5000;

	strcpy(m_szName, "°¡Á¦Æ®");

	m_nCurrX			= pPlayerObject->m_nCurrX;
	m_nCurrY			= pPlayerObject->m_nCurrY;
	m_nDirection		= GetBack(pPlayerObject->m_nDirection);
	m_pMap				= pPlayerObject->m_pMap;

	m_tFeature = pPlayerObject->m_tFeature;

	m_pMap->AddNewObject(m_nCurrX, m_nCurrY, OS_MOVINGOBJECT, this);

	AddRefMsg(RM_TURN, m_nDirection, m_nCurrX, m_nCurrY, 0, m_szName);
}

void CPlayerCloneObject::Operate()
{
}
