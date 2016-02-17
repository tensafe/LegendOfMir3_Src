#include "stdafx.h"

CWHDynamicArray::CWHDynamicArray()
{
	InitializeCriticalSection(&m_cs);

	m_nLastRemoveIndex = 0;
	m_nLastInsertIndex = 0;

	for (int i = 0; i < _MAX_USER_ARRAY; i++)
		m_Elements[i] = NULL;
}

CWHDynamicArray::~CWHDynamicArray()
{
	DeleteCriticalSection(&m_cs);
}

int CWHDynamicArray::AttachData(VOID *pElement)
{
	int nIndex = -1;

	EnterCriticalSection(&m_cs);

	__try
	{
		nIndex = m_nLastRemoveIndex;

		if (m_Elements[nIndex] == NULL)
		{
			m_Elements[nIndex] = pElement;
			__leave;
		}

		for (int i = m_nLastInsertIndex; i < _MAX_USER_ARRAY; i++)
		{
			if (m_Elements[nIndex] == NULL)
			{
				m_Elements[nIndex] = pElement;
				m_nLastInsertIndex = nIndex;
				__leave;
			}
		}

		for (i = 0; i < m_nLastInsertIndex; i++)
		{
			if (m_Elements[nIndex] == NULL)
			{
				m_Elements[nIndex] = pElement;
				m_nLastInsertIndex = nIndex;
				break;
			}
		}
	}
	__finally
	{
		LeaveCriticalSection(&m_cs);
	}

	return nIndex;
}

void CWHDynamicArray::DettachData(int nIndex)
{
	EnterCriticalSection(&m_cs);

	__try
	{
		m_Elements[nIndex] = NULL;
		m_nLastRemoveIndex = nIndex;
	}
	__finally
	{
		LeaveCriticalSection(&m_cs);
	}
}
