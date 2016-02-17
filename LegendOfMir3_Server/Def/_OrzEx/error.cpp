

#include "stdafx.h"
#include "error.h"
#include <windows.h>
#include <stdio.h>


CError::CError( char *pMsg )
{
	strcpy( m_szMsg, pMsg );
}


CError::~CError()
{
}


const char * CError::GetMsg()
{
	return m_szMsg;
}
