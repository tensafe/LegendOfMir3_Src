#include "stdafx.h"

CMerchantObject::CMerchantObject() : CCharObject(NULL)
{
	m_fHideMode = FALSE;

	m_dwRunTime			= GetTickCount();
	m_dwSearchTime		= GetTickCount();

	m_dwRunNextTick		= 250;
	m_dwSearchTick		= 2000 + rand() % 2000;
}

void CMerchantObject::RunRace()
{
	if ((rand() % 40) == 0)
		TurnTo(rand() % 8);
	else if ((rand() % 50) == 0)
		AddRefMsg(RM_HIT, m_nDirection, m_nCurrX, m_nCurrY, 0, NULL);
}
