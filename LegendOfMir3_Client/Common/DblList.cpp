/******************************************************************************************************************

	DLinkedList.h: interface for the CPDLList class.

*******************************************************************************************************************/

#include "stdafx.h"


template <class T>
CPDLList<T>::CPDLList(CPDLList<T>& PDLList)
{
	for ( INT nLoops = 0; nLoops<DLList.ListLength(); nLoops++ )
	{
		T* tempT	= new T;
		tempt->ptypeData = DDList.Current();
		AddNode(tempT);
	}
}

