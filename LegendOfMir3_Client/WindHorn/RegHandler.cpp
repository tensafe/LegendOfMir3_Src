// ****************************************************************************
//
//		Registry Handler 
//
//      All written by Jeong, Euy-heon
//      Copyright(C) 1997, 1998 Euy-heon, Jeong. All right reserved.
//
// ****************************************************************************

#include "stdafx.h"

// ****************************************************************************
//
//      jRegGetKey/jRegSetKey
//
//      Last update : 1997.07.16 (Euy-heon, Jeong)
//      Purpose     : Get and set Windows null-terminate string regstry keys. 
//
// ****************************************************************************

BOOL jRegSetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, DWORD dwFlags, 
						LPBYTE pValue, DWORD nValueSize)
{
	HKEY	hk = NULL;
	DWORD	dwDisposition = 0;

	if (RegCreateKeyEx(HKEY_LOCAL_MACHINE, pSubKeyName, 0, NULL, 
					REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, 
					&hk, &dwDisposition) == ERROR_SUCCESS)
	{
		if (RegSetValueEx(hk, pValueName, 0, dwFlags, pValue, nValueSize) != ERROR_SUCCESS)
		{
			RegCloseKey(hk);
			return FALSE;
		}

		RegCloseKey(hk);

		return TRUE;
	}

	return FALSE;
}

BOOL jRegGetKey(LPCTSTR pSubKeyName, LPCTSTR pValueName, LPBYTE pValue)
{
	HKEY	hk = NULL;
	DWORD	dwFlags = 0, dwValueSize = 0;

    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, pSubKeyName, 0, KEY_READ, &hk) == ERROR_SUCCESS)
	{
		if (RegQueryValueEx(hk, pValueName, NULL, &dwFlags, NULL, &dwValueSize) != ERROR_SUCCESS)
		{
			RegCloseKey(hk);
			return FALSE;
		}

		RegQueryValueEx(hk, pValueName, NULL, &dwFlags, pValue, &dwValueSize);
		RegCloseKey(hk);

		return TRUE;
	}

	return FALSE;
}

LONG jRegEnumKey(LPCTSTR pSubKeyName, int nIndex, LPTSTR lpSubKey, int nSubkeyLen)
{
	HKEY		hk = NULL;
	DWORD		dwSubKeyLen = nSubkeyLen;
	FILETIME	ft;
	LONG		lRet = -1;

    if (RegOpenKeyEx(HKEY_LOCAL_MACHINE, pSubKeyName, 0, KEY_READ, &hk) == ERROR_SUCCESS)
	{
		lRet = RegEnumKeyEx(hk, nIndex, lpSubKey, &dwSubKeyLen, NULL, NULL, NULL, &ft);

		RegCloseKey(hk);
	}

	return lRet;
}
