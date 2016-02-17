/******************************************************************************************************************

	DLinkedList.h: interface for the CDLList class.

*******************************************************************************************************************/

#include "stdafx.h"


template <class T>
CDLList<T>::CDLList(CDLList<T>& DLList)
{
	for ( INT nLoops = 0; nLoops<DLList.ListLength(); nLoops++ )
	{
		T* tempT	= new T;
		tempt->Data = DDList.Current();
		AddNode(tempT);
	}
}

