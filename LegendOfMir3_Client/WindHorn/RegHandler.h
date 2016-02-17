// ****************************************************************************
//
//		Registry Handler 
//
//      All written by Jeong, Euy-heon
//      Copyright(C) 1997, 1998 Euy-heon, Jeong. All right reserved.
//
// ****************************************************************************

#include "stdafx.h"



BOOL jRegSetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, DWORD dwFlags, 
				LPBYTE pValue, DWORD nValueSize);
BOOL jRegGetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, LPBYTE pValue);
LONG jRegEnumKey(LPCTSTR pSubKeyName, int nIndex, LPTSTR lpSubKey, int nSubkeyLen);

/*
#define _LOGINGATE_SERVER_REGISTRY	_T("Software\\LegendOfMir\\LoginGate")
jRegGetKey(_LOGINGATE_SERVER_REGISTRY, _T("RemoteIP"), (LPBYTE)&dwIP);
jRegSetKey(_LOGINGATE_SERVER_REGISTRY, _T("RemoteIP"), REG_DWORD, (LPBYTE)&dwIP, sizeof(DWORD));
*/