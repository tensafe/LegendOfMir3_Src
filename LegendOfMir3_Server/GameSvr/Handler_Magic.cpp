#include "stdafx.h"

int CMagicInfo::GetPower13(int nPwr, int nLevel)
{
	float p1, p2;

	p1 = (float)nPwr / 3;
	p2 = (float)nPwr - p1;

	if ((sDefMaxPower - sDefPower))
		return ROUND(p1 + p2 / (3/*MaxTrainLevel*/ + 1) * (nLevel + 1) + (sDefPower + rand() % (sDefMaxPower - sDefPower))); 
	else
		return ROUND(p1 + p2 / (3/*MaxTrainLevel*/ + 1) * (nLevel + 1) + sDefPower);
}

BOOL CMagicInfo::CheckMagicLevelup(CCharObject *pCharObject, _LPTHUMANMAGICRCD lptMagicRcd)
{
	int nLevel;

	if (lptMagicRcd->btLevel >= 0 && lptMagicRcd->btLevel <= 3/*MaxTrainLevel*/)
		nLevel = (int)lptMagicRcd->btLevel;
	else
		nLevel = 0;

	if (lptMagicRcd->btLevel < 3/*MaxTrainLevel*/)
	{
		if (lptMagicRcd->nCurrTrain >= nTrain[nLevel])
		{
			if (lptMagicRcd->btLevel < 3/*MaxTrainLevel*/)
			{
				lptMagicRcd->nCurrTrain -= nTrain[nLevel];
				lptMagicRcd->btLevel	+= 1;
				
				pCharObject->UpdateDelayProcessCheckParam1(pCharObject, RM_MAGIC_LVEXP, 0, lptMagicRcd->nCurrTrain, lptMagicRcd->btLevel, lptMagicRcd->btMagicID, NULL, 800);
//				CheckMagicSpecialAbility (pum);
			}
			else
				lptMagicRcd->nCurrTrain = nTrain[nLevel];

			return TRUE;
		}
	}

	return FALSE;
}
