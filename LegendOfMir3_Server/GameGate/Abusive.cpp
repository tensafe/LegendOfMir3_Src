#include "stdafx.h"

CWHAbusive::CWHAbusive()
{
}

CWHAbusive::~CWHAbusive()
{
}

BOOL CWHAbusive::LoadAbusiveList()
{
	FILE	*stream = fopen("Abusive.txt", "r");

	if (stream)
	{
		while(!feof(stream))
		{
			fscanf(stream, "%s", m_szAbusiveList[m_nCount]);
			m_nCount++;
		}

		fclose(stream);

		InsertLogMsgParam(IDS_LOAD_ABUSIVE, &m_nCount, LOGPARAM_INT);

		return TRUE;
	}
	else
		InsertLogMsg(IDS_LOADFAIL_ABUSIVE);

	return FALSE;
}

void CWHAbusive::ChangeAbusiveText(char *pszText)
{
	int		nChange;
	char	*pszSrc, *pszFilter;

	for (int i = 0; i < m_nCount; i++)
	{
		pszSrc		= pszText;
		pszFilter	= &m_szAbusiveList[i][0];
		nChange		= 0;
		
		while (*pszSrc)
		{
			if (*pszSrc == *pszFilter)
			{
				nChange++;

				while (*pszFilter != 0)
				{
					if (*++pszSrc != *++pszFilter) break;

					nChange++;
				}

				if (*pszFilter == 0 && nChange >= (int)(memlen(&m_szAbusiveList[i][0]) - 1))
				{
					for (int nCnt = nChange; nCnt > 0; nCnt--)
						*(pszSrc - nCnt) = '*';
				}

				pszFilter	= &m_szAbusiveList[i][0];
				nChange		= 0;
			}

			pszSrc++;
		}
	}
}
