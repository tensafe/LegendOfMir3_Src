// ManConsoleDlg.h : header file
//

#if !defined(AFX_MANCONSOLEDLG_H__5DD61023_2762_40C6_BDB1_0A4B6F393ECE__INCLUDED_)
#define AFX_MANCONSOLEDLG_H__5DD61023_2762_40C6_BDB1_0A4B6F393ECE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CManConsoleDlg dialog

class CManConsoleDlg : public CDialog
{
// Construction
public:
	CManConsoleDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CManConsoleDlg)
	enum { IDD = IDD_MANCONSOLE_DIALOG };
		// NOTE: the ClassWizard will add data members here
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CManConsoleDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CManConsoleDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MANCONSOLEDLG_H__5DD61023_2762_40C6_BDB1_0A4B6F393ECE__INCLUDED_)
