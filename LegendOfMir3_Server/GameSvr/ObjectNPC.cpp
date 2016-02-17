#include "stdafx.h"

CNPCObject::CNPCObject() : CCharObject(NULL)
{
	m_dwRunNextTick	= 250;
	m_dwSearchTick	= 3000;

	m_btLight		= 6;
}
