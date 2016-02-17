// ManConsole.h : main header file for the MANCONSOLE application
//

#if !defined(AFX_MANCONSOLE_H__F8877CC8_9C56_4B4D_970F_49791465A3D9__INCLUDED_)
#define AFX_MANCONSOLE_H__F8877CC8_9C56_4B4D_970F_49791465A3D9__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// CManConsoleApp:
// See ManConsole.cpp for the implementation of this class
//

class CManConsoleApp : public CWinApp
{
public:
	CManConsoleApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CManConsoleApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(CManConsoleApp)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MANCONSOLE_H__F8877CC8_9C56_4B4D_970F_49791465A3D9__INCLUDED_)
