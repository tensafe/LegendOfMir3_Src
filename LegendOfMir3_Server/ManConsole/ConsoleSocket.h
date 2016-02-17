#if !defined(AFX_CONSOLESOCKET_H__C2E973DB_90BA_4535_9633_38278EAB8EDB__INCLUDED_)
#define AFX_CONSOLESOCKET_H__C2E973DB_90BA_4535_9633_38278EAB8EDB__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConsoleSocket.h : header file
//



/////////////////////////////////////////////////////////////////////////////
// CConsoleSocket command target

class CConsoleSocket : public CAsyncSocket
{
// Attributes
public:

// Operations
public:
	CConsoleSocket();
	virtual ~CConsoleSocket();

// Overrides
public:
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CConsoleSocket)
	//}}AFX_VIRTUAL

	// Generated message map functions
	//{{AFX_MSG(CConsoleSocket)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

// Implementation
protected:
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CONSOLESOCKET_H__C2E973DB_90BA_4535_9633_38278EAB8EDB__INCLUDED_)
