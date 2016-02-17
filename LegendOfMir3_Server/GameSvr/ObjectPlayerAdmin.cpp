#include "stdafx.h"

CMirMap*		GetMap(char *pszMapName);
CMonRaceInfo*	GetMonRaceInfo(char *pszMonName);

void			GetFrontPosition(CCharObject *pObject, int &nX, int &nY);

BOOL			UpdateItemToDB(_LPTUSERITEMRCD lpMakeItemRcd, char *pszUserID, char *pszCharName, int nAction);

void CPlayerObject::CmdChangeItemPrefix(char *pszParam1, char *pszParam2)
{
	_LPTUSERITEMRCD lpUserItemRcd = NULL;

	for (int i = U_DRESS; i <= U_RINGR; i++)
	{
		if (lpUserItemRcd = m_pUserInfo->GetAccessory(i))
		{
			if (strcmp(pszParam1, g_pStdItemSpecial[lpUserItemRcd->nStdIndex].szName) == 0)
			{
				strcpy(lpUserItemRcd->szPrefixName, pszParam2);
				
				if (UpdateItemToDB(lpUserItemRcd, _ITEM_ACTION_UPDATE))
					AddProcess(this, RM_ITEMUPDATE, i, 0, 0, 0, lpUserItemRcd->szPrefixName);

				return;
			}
		}
	}
}

void CPlayerObject::CmdRandomSpaceMove(char *pszParam1)
{
	int nX;
	int nY;
	int nEgdey;

	CMirMap* pMirMap = GetMap(pszParam1);

	if (pMirMap)
	{
		if (pMirMap->m_stMapFH.shHeight < 150)
		{
			if (pMirMap->m_stMapFH.shHeight < 30)
				nEgdey = 2;
			else
				nEgdey = 20;
		}
		else
			nEgdey = 50;

		nX = nEgdey + (rand() % (pMirMap->m_stMapFH.shWidth - nEgdey - 1));
		nY = nEgdey + (rand() % (pMirMap->m_stMapFH.shHeight - nEgdey - 1));

		if (GetAvailablePosition(pMirMap, nX, nY, 20))
			SpaceMove(nX, nY, pMirMap);
		else 
			SysMsg("Command Failed", 0);
	}
	else
		SysMsg("Command Failed : Invalid map file name.", 0);
}

void CPlayerObject::CmdFreeSpaceMove(char *pszParam1, char *pszParam2, char *pszParam3)
{
	int nX		= AnsiStrToVal(pszParam2);
	int nY		= AnsiStrToVal(pszParam3);

	CMirMap* pMirMap = GetMap(pszParam1);

	if (pMirMap)
	{
		if (pMirMap->CanMove(nX, nY))
			SpaceMove(nX, nY, pMirMap);
		else
			SysMsg("Command Failed", 0);
	}
	else
		SysMsg("Command Failed : Invalid map file name.", 0);
}

void CPlayerObject::CmdCallMakeMonster(char *pszParam1, char *pszParam2)
{
	int nMax = AnsiStrToVal(pszParam2);
	int nX, nY;

	CMonRaceInfo* pMonRaceInfo = GetMonRaceInfo(pszParam1);

	if (pMonRaceInfo)
	{
		GetFrontPosition(nX, nY);

		for (int i = 0; i < nMax; i++)
			AddCreatureSysop(nX, nY, pMonRaceInfo, TRUE);
	}
}

void CPlayerObject::CmdCallMakeMonsterXY(char *pszParam1, char *pszParam2, char *pszParam3, char *pszParam4)
{
	int nMax	= _MIN(500, AnsiStrToVal(pszParam4));
	int nX		= AnsiStrToVal(pszParam1);
	int nY		= AnsiStrToVal(pszParam2);

	CMonRaceInfo* pMonRaceInfo = GetMonRaceInfo(pszParam3);

	if (pMonRaceInfo)
	{
		for (int i = 0; i < nMax; i++)
			AddCreatureSysop(nX, nY, pMonRaceInfo, FALSE);
	}
}

void CPlayerObject::CmdMakeItem(char *pszParam1, char *pszParam2)
{
	int nGain = AnsiStrToVal(pszParam2);

	for (int i = 0; i < nGain; i++)
		m_pUserInfo->AddNewItem(pszParam1);
}

BOOL CPlayerObject::CmdChangeJob(char *pszParam1)
{
	TCHAR	wszJobName[32];
	char	szJobName[32];

	for (int i = 0; i < 3; i++)
	{
		LoadString(g_hInst, IDS_JOBNAME_W + i, wszJobName, sizeof(wszJobName)/sizeof(TCHAR));
		WideCharToMultiByte(CP_ACP, 0, wszJobName, -1, szJobName, sizeof(szJobName), NULL, NULL);

		if (memcmp(szJobName, pszParam1, memlen(szJobName) - 1) == 0)
		{
			m_pUserInfo->SetJob(i);
			SysMsg(szJobName, 1);
			
			return TRUE;
		}
	}

	return FALSE;
}

void CPlayerObject::CmdMakeFullSkill(char *pszParam1, char *pszParam2)
{
	for (int i = 0; i < g_nNumOfMagicInfo; i++)
	{
		if (memcmp(g_pMagicInfo[i].szName, pszParam1, memlen(g_pMagicInfo[i].szName) - 1) == 0)
		{
			int nLevel = _MIN(3, AnsiStrToVal(pszParam2));

			PLISTNODE pListNode = m_pUserInfo->m_lpTMagicRcd.GetHead();

			while (pListNode)
			{
				_LPTHUMANMAGICRCD	lptUserMagicRcd = m_pUserInfo->m_lpTMagicRcd.GetData(pListNode);

				if (lptUserMagicRcd)
				{
					if (lptUserMagicRcd->btMagicID == i)
					{
						lptUserMagicRcd->btLevel	= (BYTE)nLevel;

						AddProcess(this, RM_MAGIC_LVEXP, 0, lptUserMagicRcd->btMagicID, lptUserMagicRcd->btLevel, 
										lptUserMagicRcd->nCurrTrain);

						TCHAR	wszMsg[64], wszMagicName[32], wszCompMsg[128];
						char	szMsg[128];

						MultiByteToWideChar(CP_ACP, 0, pszParam1, -1, wszMagicName, sizeof(wszMagicName)/sizeof(TCHAR));
						LoadString(g_hInst, IDS_SKIL_LEVELUP, wszMsg, sizeof(wszMsg)/sizeof(TCHAR));
						wsprintf(wszCompMsg, wszMsg, wszMagicName, nLevel);
						WideCharToMultiByte(CP_ACP, 0, wszCompMsg, -1, szMsg, sizeof(szMsg), NULL, NULL);
						
						SysMsg(szMsg, 1);

						switch (lptUserMagicRcd->btMagicID) 
						{
							case _SKILL_YEDO:
							case _SKILL_ILKWANG:
							case _SKILL_ONESWORD:
								RecalcAbilitys();
								AddProcess(this, RM_SUBABILITY, 0, 0, 0, 0, NULL);
								break;
						}

						return;
					}
				}

				pListNode = m_pUserInfo->m_lpTMagicRcd.GetNext(pListNode);
			}
		}
	}
}

void CPlayerObject::CmdSendMonsterLevelInfos()
{
	CWHList<CCharObject*>	ObjectList;
	TCHAR					wszInfo[1024];
	char					szInfo[1024];
	TCHAR					wszTemp1[32], wszTemp2[32];

	m_pMap->GetMapObject(m_nCurrX, m_nCurrY, 2, &ObjectList);

	if (ObjectList.GetCount())
	{
		PLISTNODE		pListNode = ObjectList.GetHead();
		CCharObject*	pTargetObject;

		while (pListNode)
		{
			if (pTargetObject = ObjectList.GetData(pListNode))
			{
				if (pTargetObject->m_wObjectType & _OBJECT_ANIMAL)
				{
					MultiByteToWideChar(CP_ACP, 0, pTargetObject->m_szName, -1, wszTemp1, sizeof(wszTemp1)/sizeof(TCHAR));
					MultiByteToWideChar(CP_ACP, 0, pTargetObject->m_pMap->m_szMapName, -1, wszTemp2, sizeof(wszTemp2)/sizeof(TCHAR));

					wsprintf(wszInfo, _TEXT("%s, MAP:%s, X:%d, Y:%d, Lv:%d, Exp:%d, HP:%d/%d, MP:%d/%d, DC:%d/%d, MC:%d/%d, SC:%d/%d, AC:%d/%d, MAC:%d/%d, Hit:%d, Spd:%d, Water:%d/%d, Fire:%d/%d, Wind:%d/%d, Light:%d/%d, Earth:%d/%d"),
						wszTemp1, wszTemp2,
						pTargetObject->m_nCurrX, pTargetObject->m_nCurrY,
						pTargetObject->m_Ability.Level,
						pTargetObject->m_Ability.Exp,
						pTargetObject->m_WAbility.HP, pTargetObject->m_WAbility.MaxHP,
						pTargetObject->m_WAbility.MP, pTargetObject->m_WAbility.MaxMP,
						LOBYTE(pTargetObject->m_WAbility.DC), HIBYTE(pTargetObject->m_WAbility.DC),
						LOBYTE(pTargetObject->m_WAbility.MC), HIBYTE(pTargetObject->m_WAbility.MC),
						LOBYTE(pTargetObject->m_WAbility.SC), HIBYTE(pTargetObject->m_WAbility.SC),
						LOBYTE(pTargetObject->m_WAbility.AC), HIBYTE(pTargetObject->m_WAbility.AC),
						LOBYTE(pTargetObject->m_WAbility.MAC), HIBYTE(pTargetObject->m_WAbility.MAC),
						pTargetObject->m_btHitPoint,
						pTargetObject->m_btSpeedPoint,
						LOBYTE(pTargetObject->m_WAbility.m_wWater), HIBYTE(pTargetObject->m_WAbility.m_wWater),
						LOBYTE(pTargetObject->m_WAbility.m_wFire), HIBYTE(pTargetObject->m_WAbility.m_wFire),
						LOBYTE(pTargetObject->m_WAbility.m_wWind), HIBYTE(pTargetObject->m_WAbility.m_wWind),
						LOBYTE(pTargetObject->m_WAbility.m_wLight), HIBYTE(pTargetObject->m_WAbility.m_wLight),
						LOBYTE(pTargetObject->m_WAbility.m_wEarth), HIBYTE(pTargetObject->m_WAbility.m_wEarth));

					WideCharToMultiByte(CP_ACP, 0, wszInfo, -1, szInfo, sizeof(szInfo), NULL, NULL);
					SysMsg(szInfo, 1);
				}
			}
		
			pListNode = ObjectList.RemoveNode(pListNode);
		}
	}
}

void CPlayerObject::CmdDyeingHair(char *pszParam1)
{
	m_tFeatureEx.dwHairColor = AnsiStrToVal(pszParam1);

	AddRefMsg(RM_TURN, m_nDirection, m_nCurrX, m_nCurrY, 0, m_pUserInfo->m_szCharName);
}

void CPlayerObject::CmdDyeingWear(char *pszParam1)
{
	m_tFeatureEx.dwWearColor = AnsiStrToVal(pszParam1);

	AddRefMsg(RM_TURN, m_nDirection, m_nCurrX, m_nCurrY, 0, m_pUserInfo->m_szCharName);
}

void CPlayerObject::CmdHairStyle(char *pszParam1)
{
	m_tFeature.btHair = AnsiStrToVal(pszParam1);

	AddRefMsg(RM_TURN, m_nDirection, m_nCurrX, m_nCurrY, 0, m_pUserInfo->m_szCharName);
}

void CPlayerObject::CmdCallMakeSlaveMonster(char *pszParam1, char *pszParam2)
{
	int nMax = AnsiStrToVal(pszParam2);
	int nX, nY;

	CMonRaceInfo* pMonRaceInfo = GetMonRaceInfo(pszParam1);

	if (pMonRaceInfo)
	{
		GetFrontPosition(nX, nY);

		for (int i = 0; i < nMax; i++)
		{
			CMonsterObject* pMonsterObject = (CMonsterObject*)AddCreatureSysop(nX, nY, pMonRaceInfo, TRUE);

			if (pMonsterObject)
			{
				char	szMonName[64];

				pMonsterObject->m_pMasterObject = this;

				pMonsterObject->GetCharName(szMonName);

				AddRefMsg(RM_USERNAME, 0, 0, 0, 0, szMonName);
				
				m_xSlaveObjList.AddNewNode(pMonsterObject);
/*				cret.MasterRoyaltyTime := GetTickCount + 24 * 60 * 60 * 1000;
                  cret.SlaveMakeLevel := 3;
                  cret.SlaveExpLevel := momlv;
                  cret.UserNameChanged;
                  SlaveList.Add (cret); */
			}
		}
	}
}
