#define _MAX_USER_ARRAY 5000
/*
typedef struct tag_TDYNAMICARRAY
{
	CSessionInfo*	pInfo;
} _TDYNAMICARRAY, *_LPTDYNAMICARRAY;
*/

template <class T> class CWHDynamicArray
{
protected:
	T*					m_Elements[_MAX_USER_ARRAY];
	BOOL				m_IsAvElements[_MAX_USER_ARRAY];

	int					m_nLastRemoveIndex;
	int					m_nLastInsertIndex;

public:
	CWHDynamicArray();
	~CWHDynamicArray();

	int		AttachData(T* pElement);
	void	DettachData(int nIndex);
	int		GetAvailablePosition();
	T*		GetEmptyElement(int nIndex);
	BOOL	SetEmptyElement(int nIndex, T* pElement);
	
	__inline T* GetData(int nIndex) { if (nIndex >= 0 && nIndex <= _MAX_USER_ARRAY) return m_Elements[nIndex]; return NULL; }
};

template <class T> CWHDynamicArray<T>::CWHDynamicArray()
{
	m_nLastRemoveIndex = 0;
	m_nLastInsertIndex = 0;

	for (int i = 0; i < _MAX_USER_ARRAY; i++)
	{
		m_Elements[i]		= NULL;
		m_IsAvElements[i]	= FALSE;
	}
}

template <class T> CWHDynamicArray<T>::~CWHDynamicArray()
{
	for (int i = 0; i < _MAX_USER_ARRAY; i++)
	{
		if (m_Elements[i])
		{
			delete m_Elements[i];
			m_Elements[i]	= NULL;
		}
		
		m_IsAvElements[i]	= FALSE;
	}
}

template <class T> int CWHDynamicArray<T>::GetAvailablePosition()
{
	int nIndex = -1;

	nIndex = m_nLastRemoveIndex;

	if (m_IsAvElements[nIndex] == FALSE)
		return nIndex;

	for (int i = m_nLastInsertIndex; i < _MAX_USER_ARRAY; i++)
	{
		if (m_IsAvElements[i] == FALSE)
			return i;
	}

	for (i = 0; i < m_nLastInsertIndex; i++)
	{
		if (m_IsAvElements[i] == FALSE)
			return i;
	}

	return -1;
}

template <class T> T* CWHDynamicArray<T>::GetEmptyElement(int nIndex)
{
	if (m_IsAvElements[nIndex] == FALSE)
	{
		if (!m_Elements[nIndex])
			m_Elements[nIndex] = new T;

		m_IsAvElements[nIndex] = TRUE;

		return m_Elements[nIndex];
	}

	return NULL;
}

template <class T> BOOL CWHDynamicArray<T>::SetEmptyElement(int nIndex, T* pElement)
{
	if (m_IsAvElements[nIndex] == TRUE && m_Elements[nIndex] == pElement)
	{
		m_IsAvElements[nIndex] = FALSE;

		return TRUE;
	}

	return FALSE;
}

template <class T> int CWHDynamicArray<T>::AttachData(T* pElement)
{
	int nIndex = -1;

	nIndex = m_nLastRemoveIndex;

	if (m_Elements[nIndex] == NULL && m_IsAvElements[nIndex] == FALSE)
	{
		m_Elements[nIndex] = pElement;
		m_IsAvElements[nIndex] = TRUE;

		return nIndex;
	}

	for (int i = m_nLastInsertIndex; i < _MAX_USER_ARRAY; i++)
	{
		if (m_Elements[i] == NULL && m_IsAvElements[i] == FALSE)
		{
			m_Elements[i] = pElement;
			m_IsAvElements[i] = TRUE;
			m_nLastInsertIndex = i;
			nIndex = i;

			return nIndex;
		}
	}

	for (i = 0; i < m_nLastInsertIndex; i++)
	{
		if (m_Elements[i] == NULL && m_IsAvElements[i] == FALSE)
		{
			m_Elements[i] = pElement;
			m_IsAvElements[i] = TRUE;
			m_nLastInsertIndex = i;
			nIndex = i;

			return nIndex;
		}
	}
}

template <class T> void CWHDynamicArray<T>::DettachData(int nIndex)
{
	if (nIndex >= 0 && nIndex <= _MAX_USER_ARRAY)
	{
		m_Elements[nIndex]	= NULL;
		m_IsAvElements[nIndex]	= FALSE;

		m_nLastRemoveIndex = nIndex;
	}
}
