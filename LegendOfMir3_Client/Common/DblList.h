/******************************************************************************************************************

	DblList.h: interface for the CPDLList class.

*******************************************************************************************************************/

#ifndef _DBLLIST_H
#define _DBLLIST_H

using namespace std;

template<class T>

class CPDLList  
{
private:

	typedef struct	tagNODE
	{
		T*			ptypeData;
		tagNODE*	pNextNode;
		tagNODE*	pPrevNode;
		tagNODE(T* pData, tagNODE* pNext=NULL, tagNODE* pPrev=NULL)
		{
			ptypeData	= pData;
			pNextNode	= pNext;
			pPrevNode	= pPrev;
		}
	}NODE, *LPNODE;

	LPNODE	m_stHead;
	LPNODE	m_pstCurrNode;
	LPNODE	m_stTail;
	INT		m_nCurrentPos;
	INT		m_nCounter;

public:

	CPDLList(CPDLList<T>& PDLList);

	// Constructor
	CPDLList()
	{
		m_stHead		= NULL;
		m_stTail		= NULL;
		m_pstCurrNode	= NULL;
		m_nCounter		= 0;
		m_nCurrentPos	= 0;
	};

	// Destructor
	virtual ~CPDLList()
	{
		ClearAllNodes();
	};

	VOID AddNode(T* pData)
	{
		LPNODE pstTempNode = new NODE(pData);
		if ( m_stTail != NULL )
		{
			m_stTail->pNextNode	   = pstTempNode;
			pstTempNode->pNextNode = pstTempNode;
			pstTempNode->pPrevNode = m_stTail;
			m_stTail = m_stTail->pNextNode;
		}
		else
		{
			m_stHead				= pstTempNode;
			m_stHead->pPrevNode		= m_stHead;
			m_stHead->pNextNode		= m_stHead;
			m_stTail				= m_stHead;
			m_pstCurrNode			= m_stHead;
		}
		m_nCounter++;
	};

	BOOL DeleteCurrentNode(VOID)
	{
		LPNODE pstTempNode;
		if ( m_pstCurrNode != NULL )
		{
			m_nCounter--;
			m_pstCurrNode->pPrevNode->pNextNode = m_pstCurrNode->pNextNode;
			m_pstCurrNode->pNextNode->pPrevNode = m_pstCurrNode->pPrevNode;

			pstTempNode		= m_pstCurrNode;
			m_pstCurrNode	= m_pstCurrNode->pNextNode;
			if ( pstTempNode == m_pstCurrNode )				// Current = Tail ?
			{
				m_stTail = m_stTail->pPrevNode;
				m_pstCurrNode = m_pstCurrNode->pPrevNode;
				m_pstCurrNode->pNextNode = m_pstCurrNode;
				m_nCurrentPos--;
				if ( m_pstCurrNode == pstTempNode )			// Current = Head = Tail ?
				{
					m_stHead = NULL;
					m_stTail = NULL;
					m_pstCurrNode = NULL;
					m_nCurrentPos = 0;
				}
			}
			else if(pstTempNode == m_stHead)				// Current  = Head ?
			{
				m_stHead = pstTempNode->pNextNode;
				m_pstCurrNode = pstTempNode->pNextNode;
				m_pstCurrNode->pPrevNode = m_pstCurrNode;
				if ( pstTempNode == m_stHead )			// Current = Head = Tail ?
				{
					m_stHead = NULL;
					m_stTail = NULL;
					m_pstCurrNode = NULL;
					m_nCurrentPos = 0;
				}
			}
			SAFE_DELETE(pstTempNode->ptypeData);
			SAFE_DELETE(pstTempNode);
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	};

	// 외부에서 typeData를 지우는 형태이다.
	BOOL DeleteCurrentNodeEx(VOID)
	{
		LPNODE pstTempNode;
		if ( m_pstCurrNode != NULL )
		{
			m_nCounter--;
			m_pstCurrNode->pPrevNode->pNextNode = m_pstCurrNode->pNextNode;
			m_pstCurrNode->pNextNode->pPrevNode = m_pstCurrNode->pPrevNode;

			pstTempNode		= m_pstCurrNode;
			m_pstCurrNode	= m_pstCurrNode->pNextNode;
			if ( pstTempNode == m_pstCurrNode )				// Current = Tail ?
			{
				m_stTail = m_stTail->pPrevNode;
				m_pstCurrNode = m_pstCurrNode->pPrevNode;
				m_pstCurrNode->pNextNode = m_pstCurrNode;
				m_nCurrentPos--;
				if ( m_pstCurrNode == pstTempNode )			// Current = Head = Tail ?
				{
					m_stHead = NULL;
					m_stTail = NULL;
					m_pstCurrNode = NULL;
					m_nCurrentPos = 0;
				}
			}
			else if(pstTempNode == m_stHead)				// Current  = Head ?
			{
				m_stHead = pstTempNode->pNextNode;
				m_pstCurrNode = pstTempNode->pNextNode;
				m_pstCurrNode->pPrevNode = m_pstCurrNode;
				if ( pstTempNode == m_stHead )			// Current = Head = Tail ?
				{
					m_stHead = NULL;
					m_stTail = NULL;
					m_pstCurrNode = NULL;
					m_nCurrentPos = 0;
				}
			}
			SAFE_DELETE(pstTempNode);
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	};

	INT	ListLength(VOID)
	{
		return m_nCounter;
	};
	
	INT	MoveNextNode(VOID)
	{
		LPNODE pstTempNode;

		if ( m_pstCurrNode && m_pstCurrNode->pNextNode )
		{
			pstTempNode		= m_pstCurrNode;
			m_pstCurrNode	= pstTempNode->pNextNode;
			m_nCurrentPos	= (m_nCurrentPos<m_nCounter) ? m_nCurrentPos+1: m_nCounter;
		}

		return m_nCurrentPos;
	};

	INT	MovePreviousNode(VOID)
	{
		LPNODE pstTempNode;
		pstTempNode		= m_pstCurrNode;
		m_pstCurrNode	= pstTempNode->pPrevNode;
		m_nCurrentPos	= (m_nCurrentPos>1) ? m_nCurrentPos-1: 1;

		return m_nCurrentPos;
	};

	INT	MoveNode(INT nMovement)
	{
		if ( nMovement > 0 )
		{
			for ( INT nLoops = 0; nLoops < nMovement; nLoops++ )
				MoveNextNode();
		}
		else
		{
			nMovement = (-nMovement);
			for ( INT nLoops = 0; nLoops < nMovement; nLoops++)
				MovePreviousNode();
		}
		return m_nCurrentPos;
	};

	VOID MoveCurrentToTop(VOID)
	{
		m_pstCurrNode = m_stHead;	
		m_nCurrentPos = 0;
	};

	VOID MoveCurrentToTail(VOID)
	{
		m_pstCurrNode = m_stTail;	
		m_nCurrentPos = m_nCounter-1;
	};

	VOID ClearAllNodes(VOID)
	{
		LPNODE pstTempNode;
		if ( m_nCounter > 0 )
		{
			for ( INT nLoops = 0; nLoops < m_nCounter; nLoops++ )
			{
				pstTempNode = m_stHead;
				m_stHead	= m_stHead->pNextNode;
				SAFE_DELETE(pstTempNode->ptypeData);
				SAFE_DELETE(pstTempNode);
			}
		}
		m_nCounter		= 0;
		m_stHead		= NULL;
		m_stTail		= NULL;
		m_pstCurrNode	= NULL;
		m_nCurrentPos	=0;
	};

	T* GetCurrentData(VOID)
	{
		return m_pstCurrNode->ptypeData;
	};

	INT GetCounter(VOID)
	{
		return m_nCounter;
	};

	INT GetCurrPosition(VOID)
	{
		return m_nCurrentPos;
	};

	BOOL IsCurrentTail(VOID)
	{
		if ( m_pstCurrNode == m_stTail )
			return TRUE;
		else
			return FALSE;
	};

	BOOL IsCurrentHead(VOID)
	{
		if (m_pstCurrNode == m_stHead )
			return TRUE;
		else
			return FALSE;
	}

	BOOL CheckEmpty()
	{
		if ( m_stHead == NULL )
			return TRUE;
		else
			return FALSE;
	}
};


#endif  //_DBLLIST_H
