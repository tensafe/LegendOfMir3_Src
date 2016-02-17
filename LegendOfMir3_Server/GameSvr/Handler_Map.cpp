#include "stdafx.h"

BOOL jRegGetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, LPBYTE pValue);

/* **************************************************************************************

		CMapCellInfo Class Members

   **************************************************************************************/

CMapCellInfo::CMapCellInfo()
{
//	InitializeCriticalSection(&m_cs);
}

CMapCellInfo::~CMapCellInfo()
{
//	DeleteCriticalSection(&m_cs);
}

/* **************************************************************************************

		CMirMap	Global Function

   **************************************************************************************/

void LoadMap(CMapInfo* pMapInfo)
{
	int			nLen = memlen(pMapInfo->szMapFileName) - 1;
	int			nLen2 = memlen(pMapInfo->szMapName) - 1;
	char		szFullName[15];

	CMirMap*	pMirMap = new CMirMap;

	if (pMirMap)
	{
		pMirMap->m_btSeries		= pMapInfo->btMapSeries;
		pMirMap->m_btSeriesVal	= pMapInfo->btMapSeriesValue;

		memcpy(pMirMap->m_szMapName, pMapInfo->szMapFileName, nLen);
		pMirMap->m_szMapName[nLen] = '\0';
		memcpy(pMirMap->m_szMapTextName, pMapInfo->szMapName, nLen2);
		pMirMap->m_szMapTextName[nLen2] = '\0';

		memcpy(szFullName, pMapInfo->szMapFileName, nLen);
		memcpy(&szFullName[nLen], ".map", 4);
		szFullName[nLen + 4] = '\0';

		pMirMap->LoadMapData(szFullName);
	}
}

CMirMap* GetMap(char *pszMapName)
{
	PLISTNODE		pListNode;
	CMirMap*		pMirMap = NULL;

	if (g_xMirMapList.GetCount())
	{
		pListNode = g_xMirMapList.GetHead();

		while (pListNode)
		{
			pMirMap = g_xMirMapList.GetData(pListNode);

			if (memcmp(pMirMap->m_szMapName, pszMapName, memlen(pszMapName) -1) == 0)
				return pMirMap;
			
			pListNode = g_xMirMapList.GetNext(pListNode);
		} // while (pListNode)
	} // if (pMapCellInfo->m_pObjectList.GetCount())
	
	return NULL;
}

/* **************************************************************************************

		CMirMap	Class Members

   **************************************************************************************/

CMirMap::CMirMap()
{
	m_pMapCellInfo = NULL;
}

CMirMap::~CMirMap()
{
	FreeMapData();
}

BOOL CMirMap::IsValidObject(int nX, int nY, int nCheckRange, CCharObject* pCharObject)
{
	CMapCellInfo*	pMapCellInfo = NULL;
	_LPTOSOBJECT	pOSObject = NULL;

	for (int x = nX - nCheckRange; x <= nX + nCheckRange; x++)
	{
		for (int y = nY - nCheckRange; y <= nY + nCheckRange; y++)
		{
			pMapCellInfo = GetMapCellInfo(x, y);

			if (pMapCellInfo)
			{
				if (pMapCellInfo->m_xpObjectList)
				{	
					if (pMapCellInfo->m_xpObjectList->GetCount())
					{
						PLISTNODE pListNode = pMapCellInfo->m_xpObjectList->GetHead();

						while (pListNode)
						{
							pOSObject = (_LPTOSOBJECT)pMapCellInfo->m_xpObjectList->GetData(pListNode);

							if (pOSObject->btType == OS_MOVINGOBJECT)
							{
								if (pCharObject = (CCharObject*)pOSObject->pObject)
									return TRUE;
							}
						}

						pListNode = pMapCellInfo->m_xpObjectList->GetNext(pListNode);
					} // while (pListNode)
				} // if (pMapCellInfo->m_pObjectList.GetCount())
			}
		}
	}

	return FALSE;
}

BOOL CMirMap::LoadMapData(char *pszName)
{
	HANDLE			hFile;
	LPCELLINFO		pstCellInfo;
	TCHAR			szMapName[15];
	TCHAR			szMapFileName[256];
	TCHAR			szPath[128];

	jRegGetKey(_GAME_SERVER_REGISTRY, _TEXT("MapFileLoc"), (LPBYTE)szPath);

	MultiByteToWideChar(CP_ACP, 0, pszName, -1, szMapName, sizeof(szMapName)/sizeof(TCHAR));

	lstrcpy(szMapFileName, szPath);
	lstrcat(szMapFileName, _TEXT("\\"));
	lstrcat(szMapFileName, szMapName);

	hFile = CreateFile(szMapFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if ( hFile != INVALID_HANDLE_VALUE )
	{
		DWORD		dwReadLen;

		FreeMapData();

		ReadFile(hFile, &m_stMapFH, sizeof(MAPFILEHEADER), &dwReadLen, NULL);

		int nMapSize = m_stMapFH.shWidth * m_stMapFH.shHeight;

		SetFilePointer(hFile, sizeof(TILEINFO) * (nMapSize) / 4, 0, FILE_CURRENT);

		pstCellInfo = new CELLINFO[nMapSize];

		if (pstCellInfo)
		{
			ReadFile(hFile, pstCellInfo, sizeof(CELLINFO) * (nMapSize), &dwReadLen, NULL);
			
			CloseHandle(hFile);

			m_pMapCellInfo = new CMapCellInfo[nMapSize];

			if (m_pMapCellInfo)
			{
				for (int i = 0; i < nMapSize; i++)
				{
					m_pMapCellInfo[i].m_chFlag			= pstCellInfo[i].cFlag;
					m_pMapCellInfo[i].m_sLightNEvent	= pstCellInfo[i].shLigntNEvent;

//					if (m_pMapCellInfo[i].m_chFlag & 0x01)
//						m_pMapCellInfo[i].m_xpObjectList = new CWHList<_LPTOSOBJECT>;
//					else
						m_pMapCellInfo[i].m_xpObjectList = NULL;
				}
			}

			int nSize = sizeof(m_pMapCellInfo);

			delete [] pstCellInfo;
			pstCellInfo = NULL;

			InsertLogMsgParam(IDS_LOADMAPFILE_GOOD, szMapFileName, LOGPARAM_STR);

			g_xMirMapList.AddNewNode(this);

			return TRUE;
		}
	}

	InsertLogMsgParam(IDS_LOADMAPFILE_FAIL, szMapFileName, LOGPARAM_STR);

	return FALSE;
}

VOID CMirMap::FreeMapData()
{
	ZeroMemory(&m_stMapFH, sizeof(MAPFILEHEADER));

	if (m_pMapCellInfo)
	{
		int nMapSize = m_stMapFH.shWidth * m_stMapFH.shHeight;

		for (int i = 0; i < nMapSize; i++)
		{
			if (m_pMapCellInfo[i].m_xpObjectList)
			{
				delete m_pMapCellInfo[i].m_xpObjectList;
				m_pMapCellInfo[i].m_xpObjectList = NULL;
			}
		}

		delete [] m_pMapCellInfo;
		m_pMapCellInfo = NULL;
	}
}

BOOL CMirMap::CanMove(int nX, int nY, BOOL fFlag)
{
	BOOL			fRet = FALSE;
	CMapCellInfo*	pMapCellInfo = GetMapCellInfo(nX, nY);
	CCharObject*	pCharObject;
	_LPTOSOBJECT	pOSObject;
	
	if (pMapCellInfo)
	{
		if (pMapCellInfo->m_chFlag & 0x01)
		{
			fRet = TRUE;

//			EnterCriticalSection(&pMapCellInfo->m_cs);

			if (pMapCellInfo->m_xpObjectList)
			{	
				if (pMapCellInfo->m_xpObjectList->GetCount())
				{
					PLISTNODE pListNode = pMapCellInfo->m_xpObjectList->GetHead();

					while (pListNode)
					{
						pOSObject = (_LPTOSOBJECT)pMapCellInfo->m_xpObjectList->GetData(pListNode);

						if (pOSObject->btType == OS_MOVINGOBJECT)
						{
							if (pCharObject = (CCharObject*)pOSObject->pObject)
							{
								if (!pCharObject->m_fIsDead && !pCharObject->m_fInspector && !pCharObject->m_fHideMode)
								{
									if (!fFlag) 
									{
										fRet = FALSE;
										break;
									}
								}
							}
						}

						pListNode = pMapCellInfo->m_xpObjectList->GetNext(pListNode);
					} // while (pListNode)
				} // if (pMapCellInfo->m_pObjectList.GetCount())
			}

//			LeaveCriticalSection(&pMapCellInfo->m_cs);
		}
	}

	return fRet;
}

int CMirMap::CheckDoorEvent(int nX, int nY, int &nEvent)
{
	CMapCellInfo*	pMapCellInfo = GetMapCellInfo(nX, nY);
	
	if (pMapCellInfo)
	{
		if (pMapCellInfo->m_sLightNEvent & 0x02)		// Door Event
		{
			nEvent = ((pMapCellInfo->m_sLightNEvent & 0x3FFF) >> 4);

			if (pMapCellInfo->m_sLightNEvent & 0x08)	// Event
			{
				if (pMapCellInfo->m_sLightNEvent & 0xC000)
					return _DOOR_MAPMOVE_BACK;
				else
					return _DOOR_MAPMOVE_FRONT;
			}

			return _DOOR_OPEN;
		}
	}

	return _DOOR_NOT;
}

int CMirMap::CheckEvent(int nX, int nY)
{
	CMapCellInfo*	pMapCellInfo = GetMapCellInfo(nX, nY);
	
	if (pMapCellInfo)
	{
		if (pMapCellInfo->m_sLightNEvent & 0x04)	// Event
			return ((pMapCellInfo->m_sLightNEvent & 0xC000) >> 4);
	}

	return 0;
}

BOOL CMirMap::MoveToMovingObject(int nX, int nY, int nTargetX, int nTargetY, CCharObject* pObject)
{
	BOOL			fRet = FALSE;

//	CMapCellInfo*	pMapTargetCellInfo	= GetMapCellInfo(nTargetX, nTargetY);
//	CMapCellInfo*	pMapCurrCellInfo	= GetMapCellInfo(nX, nY);

//	if (pMapCurrCellInfo && pMapTargetCellInfo)
//	{
//		if (pMapTargetCellInfo->m_chFlag & 0x01)
//		{
//			if (pMapTargetCellInfo->m_xpObjectList->GetCount())
//				fRet = FALSE;
//			else
//			{
		if (RemoveObject(nX, nY, OS_MOVINGOBJECT, pObject))
		{
			if (AddNewObject(nTargetX, nTargetY, OS_MOVINGOBJECT, pObject))
				fRet = TRUE;
			else
				fRet = FALSE;
		}
		else
		{
#ifdef _DEBUG
			_RPT4(_CRT_WARN, "Remove Failed : %d, %d, %d, %d", nX, nY, nTargetX, nTargetY);
#endif
			fRet = FALSE;
		}
//			}
//		} // if ((pMapCellInfo->m_chFlag & 0x01)
//	} // if (pMapCurrCellInfo && pMapTargetCellInfo)

	return fRet;
}

BOOL CMirMap::RemoveObject(int nX, int nY, BYTE btType, VOID* pRemoveObject)
{
	CMapCellInfo*	pMapCellInfo = GetMapCellInfo(nX, nY);
	PLISTNODE		pListNode;
	_LPTOSOBJECT	pOSObject;

	if (pMapCellInfo)
	{
//		EnterCriticalSection(&pMapCellInfo->m_cs);

		if (!pMapCellInfo->m_xpObjectList) 
			return FALSE;

		if (pMapCellInfo->m_xpObjectList->GetCount())
		{
			pListNode = pMapCellInfo->m_xpObjectList->GetHead();

			while (pListNode)
			{
				pOSObject = (_LPTOSOBJECT)pMapCellInfo->m_xpObjectList->GetData(pListNode);

				if (pOSObject)
				{
					if ((pOSObject->pObject == pRemoveObject) && (pOSObject->btType == btType))
					{
						pListNode = pMapCellInfo->m_xpObjectList->RemoveNode(pListNode);
//						LeaveCriticalSection(&pMapCellInfo->m_cs);
						return TRUE;
					}
				}
				
				pListNode = pMapCellInfo->m_xpObjectList->GetNext(pListNode);
			} // while (pListNode)
		} // if (pMapCellInfo->m_pObjectList.GetCount())

//		LeaveCriticalSection(&pMapCellInfo->m_cs);
	}

	return FALSE;
}

BOOL CMirMap::AddNewObject(int nX, int nY, BYTE btType, VOID* pAddObject)
{
	CMapCellInfo*	pMapCellInfo = GetMapCellInfo(nX, nY);

	if (pMapCellInfo)
	{
		if (pMapCellInfo->m_chFlag & 0x01)
		{
			if (!pMapCellInfo->m_xpObjectList)
				pMapCellInfo->m_xpObjectList = new CWHList<_LPTOSOBJECT>;

			_LPTOSOBJECT	pOSObject = new _TOSOBJECT;
	
			if (pOSObject)
			{
//				EnterCriticalSection(&pMapCellInfo->m_cs);

				pOSObject->btType		= btType;
				pOSObject->pObject		= pAddObject;
				pOSObject->dwAddTime	= GetTickCount();

				pMapCellInfo->m_xpObjectList->AddNewNode(pOSObject);

//				LeaveCriticalSection(&pMapCellInfo->m_cs);

				return TRUE;
			}
		}
	}

	return FALSE;
}

CCharObject* CMirMap::GetObject(int nX, int nY)
{
	CMapCellInfo*	pMapCellInfo = GetMapCellInfo(nX, nY);
	_LPTOSOBJECT	pOSObject;
	PLISTNODE		pListNode;

	if (pMapCellInfo)
	{
		if (!(pMapCellInfo->m_chFlag & 0x01))
			return NULL;

//		EnterCriticalSection(&pMapCellInfo->m_cs);

		if (pMapCellInfo->m_xpObjectList)
		{
			if (pMapCellInfo->m_xpObjectList->GetCount())
			{
				pListNode = pMapCellInfo->m_xpObjectList->GetHead();

				while (pListNode)
				{
					pOSObject = pMapCellInfo->m_xpObjectList->GetData(pListNode);

					if (pOSObject)
					{
						if (pOSObject->btType == OS_MOVINGOBJECT)
						{
							CCharObject* pCharObject = (CCharObject*)pOSObject->pObject;

							if (!pCharObject->m_fIsDead && !pCharObject->m_fIsGhost)
							{
//								LeaveCriticalSection(&pMapCellInfo->m_cs);
							
								return pCharObject;
							}
						}
					}

					pListNode = pMapCellInfo->m_xpObjectList->GetNext(pListNode);
				} // while (pListNode)
			} // if (pMapCellInfo->m_pObjectList.GetCount())
		}

//		LeaveCriticalSection(&pMapCellInfo->m_cs);
	}

	return NULL;
}

void CMirMap::GetMapObject(int nX, int nY, int nArea, CWHList<CCharObject*>* pList)
{
	int nStartX = nX - nArea;
	int nEndX	= nX + nArea;
	int nStartY = nY - nArea;
	int nEndY	= nY + nArea;

	_LPTOSOBJECT	pOSObject;

	__try
	{
		for (int x = nStartX; x <= nEndX; x++)
		{
			for (int y = nStartY; y <= nEndY; y++)
			{
				if (CMapCellInfo* pMapCellInfo = GetMapCellInfo(x, y))
				{
//					EnterCriticalSection(&pMapCellInfo->m_cs);

					if (pMapCellInfo->m_xpObjectList)
					{
						if (pMapCellInfo->m_xpObjectList->GetCount())
						{
							PLISTNODE pListNode = pMapCellInfo->m_xpObjectList->GetHead();

							while (pListNode)
							{
								pOSObject = pMapCellInfo->m_xpObjectList->GetData(pListNode);

								if (pOSObject->btType == OS_MOVINGOBJECT)
								{
									CCharObject* pCharObject = (CCharObject*)pOSObject->pObject;

									if (!pCharObject->m_fIsGhost && !pCharObject->m_fIsDead )
										pList->AddNewNode(pCharObject);
								}
								
								pListNode = pMapCellInfo->m_xpObjectList->GetNext(pListNode);
							} // while (pListNode)
						} // if (pMapCellInfo->m_pObjectList.GetCount())
					}

//					LeaveCriticalSection(&pMapCellInfo->m_cs);
				} // if (pMapCellInfo)
			}// for (y)
		} // for (x)
	}
	__finally
	{
	}
}

void CMirMap::GetAllObject(int nX, int nY, CWHList<CCharObject*>* pList)
{
	_LPTOSOBJECT	pOSObject;

	if (CMapCellInfo* pMapCellInfo = GetMapCellInfo(nX, nY))
	{
//					EnterCriticalSection(&pMapCellInfo->m_cs);

		if (pMapCellInfo->m_xpObjectList)
		{
			if (pMapCellInfo->m_xpObjectList->GetCount())
			{
				PLISTNODE pListNode = pMapCellInfo->m_xpObjectList->GetHead();

				while (pListNode)
				{
					pOSObject = pMapCellInfo->m_xpObjectList->GetData(pListNode);

					if (pOSObject->btType == OS_MOVINGOBJECT)
					{
						CCharObject* pCharObject = (CCharObject*)pOSObject->pObject;

						if (!pCharObject->m_fIsGhost && !pCharObject->m_fIsDead )
							pList->AddNewNode(pCharObject);
					}
					
					pListNode = pMapCellInfo->m_xpObjectList->GetNext(pListNode);
				} // while (pListNode)
			} // if (pMapCellInfo->m_pObjectList.GetCount())
		}

//					LeaveCriticalSection(&pMapCellInfo->m_cs);
	} // if (pMapCellInfo)
}

int CMirMap::GetDupCount(int nX, int nY)
{
	CMapCellInfo*	pMapCellInfo = GetMapCellInfo(nX, nY);
	_LPTOSOBJECT	pOSObject;
	PLISTNODE		pListNode;
	int				nCount = 0;

	if (pMapCellInfo)
	{
//		EnterCriticalSection(&pMapCellInfo->m_cs);

		if (pMapCellInfo->m_xpObjectList)
		{
			if (pMapCellInfo->m_xpObjectList->GetCount())
			{
				pListNode = pMapCellInfo->m_xpObjectList->GetHead();

				while (pListNode)
				{
					pOSObject = pMapCellInfo->m_xpObjectList->GetData(pListNode);

					if (pOSObject)
					{
						if (pOSObject->btType == OS_MOVINGOBJECT)
						{
							CCharObject* pCharObject = (CCharObject*)pOSObject->pObject;

							if (!pCharObject->m_fIsDead && !pCharObject->m_fIsGhost && !pCharObject->m_fHideMode)
								nCount++;
						}
					}

					pListNode = pMapCellInfo->m_xpObjectList->GetNext(pListNode);
				} // while (pListNode)
			} // if (pMapCellInfo->m_pObjectList.GetCount())
		}

//		LeaveCriticalSection(&pMapCellInfo->m_cs);
	}

	return nCount;
}

CMapItem* CMirMap::GetItem(int nX, int nY)
{
	CMapCellInfo*	pMapCellInfo = GetMapCellInfo(nX, nY);
	_LPTOSOBJECT	pOSObject;
	PLISTNODE		pListNode;

	if (pMapCellInfo)
	{
//		EnterCriticalSection(&pMapCellInfo->m_cs);

		if (!(pMapCellInfo->m_chFlag & 0x01))
			return NULL;

		if (pMapCellInfo->m_xpObjectList)
		{
			if (pMapCellInfo->m_xpObjectList->GetCount())
			{
				pListNode = pMapCellInfo->m_xpObjectList->GetHead();

				while (pListNode)
				{
					pOSObject = pMapCellInfo->m_xpObjectList->GetData(pListNode);

					if (pOSObject)
					{
						if (pOSObject->btType == OS_ITEMOBJECT)
						{
//							LeaveCriticalSection(&pMapCellInfo->m_cs);
							return (CMapItem*)pOSObject->pObject;
						}
					}

					pListNode = pMapCellInfo->m_xpObjectList->GetNext(pListNode);
				} // while (pListNode)
			} // if (pMapCellInfo->m_pObjectList.GetCount())
		}

//		LeaveCriticalSection(&pMapCellInfo->m_cs);
	}

	return NULL;
}

BOOL CMirMap::GetDropPosition(int nOrgX, int nOrgY, int nRange, int &nX, int &nY)
{
	int nLoonCnt	= (4 * nRange) * (nRange + 1);

	for (int i = 0; i < nLoonCnt; i++)
	{
		nX = nOrgX + g_SearchTable[i].x;
		nY = nOrgY + g_SearchTable[i].y;

		if (GetItem(nX, nY) == NULL)
			return TRUE;
	}
	
	nX = nOrgX;
	nY = nOrgY;

	return FALSE;
}

CEvent* CMirMap::GetEvent(int nX, int nY)
{
	CMapCellInfo*	pMapCellInfo = GetMapCellInfo(nX, nY);
	_LPTOSOBJECT	pOSObject;
	PLISTNODE		pListNode;

	if (pMapCellInfo)
	{
//		EnterCriticalSection(&pMapCellInfo->m_cs);

		if (!(pMapCellInfo->m_chFlag & 0x01))
			return NULL;

		if (pMapCellInfo->m_xpObjectList)
		{
			if (pMapCellInfo->m_xpObjectList->GetCount())
			{
				pListNode = pMapCellInfo->m_xpObjectList->GetHead();

				while (pListNode)
				{
					pOSObject = pMapCellInfo->m_xpObjectList->GetData(pListNode);

					if (pOSObject)
					{
						if (pOSObject->btType == OS_EVENTOBJECT)
						{
//							LeaveCriticalSection(&pMapCellInfo->m_cs);
							return (CEvent*)pOSObject->pObject;
						}
					}

					pListNode = pMapCellInfo->m_xpObjectList->GetNext(pListNode);
				} // while (pListNode)
			} // if (pMapCellInfo->m_pObjectList.GetCount())
		}

//		LeaveCriticalSection(&pMapCellInfo->m_cs);
	}

	return NULL;
}

BOOL CMirMap::CanSafeWalk(int nX, int nY)
{
	CMapCellInfo*	pMapCellInfo = GetMapCellInfo(nX, nY);
	_LPTOSOBJECT	pOSObject;
	PLISTNODE		pListNode;

	if (pMapCellInfo)
	{
//		EnterCriticalSection(&pMapCellInfo->m_cs);

		if (!(pMapCellInfo->m_chFlag & 0x01))
			return NULL;

		if (pMapCellInfo->m_xpObjectList)
		{
			if (pMapCellInfo->m_xpObjectList->GetCount())
			{
				pListNode = pMapCellInfo->m_xpObjectList->GetHead();

				while (pListNode)
				{
					pOSObject = pMapCellInfo->m_xpObjectList->GetData(pListNode);

					if (pOSObject)
					{
						if (pOSObject->btType == OS_EVENTOBJECT)
						{
//							LeaveCriticalSection(&pMapCellInfo->m_cs);
							if (((CEvent*)pOSObject->pObject)->m_nDamage > 0)
								return FALSE;
						}
					}

					pListNode = pMapCellInfo->m_xpObjectList->GetNext(pListNode);
				} // while (pListNode)
			} // if (pMapCellInfo->m_pObjectList.GetCount())
		}

//		LeaveCriticalSection(&pMapCellInfo->m_cs);
	}

	return TRUE;
}
