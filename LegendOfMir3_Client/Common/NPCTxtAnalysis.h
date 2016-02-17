// NPCTxtAnalysis.h: interface for the CNPCTxtAnalysis class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NPCTXTANALYSIS_H__F849FE95_8E23_4362_A140_29B8011E23D1__INCLUDED_)
#define AFX_NPCTXTANALYSIS_H__F849FE95_8E23_4362_A140_29B8011E23D1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define MAX_BUFFER_SIZE				7168			// Buffer size 7Kb

class CNPCTxtAnalysis  
{
public:
	BOOL IsInitialized(VOID);
	BOOL IsReadyToDisplay(VOID);
	CNPCTxtAnalysis();
	virtual ~CNPCTxtAnalysis();

	BOOL	FreeAllMsg(VOID);
	BOOL	InitAnalysis(CTextButton* xpBtn,CPDLList<CMText>* pxpTxt,CWHDXGraphicWindow*	pxGWND);
	void	AnalySisText(CDLList<CHAR*>* pxpTxt,INT nLineCount);
	void	AnalySisText(CHAR* Msg);
	BOOL	StringDivide(INT nDivideWidth, INT& nDividedLine, CHAR* szSrc,CDLList<CHAR*>* m_pxpStr);

	CHAR*	UpperCase(CHAR* Str);
	CHAR	UpperCase(CHAR chr);

	CHAR*	GetValidStr(CHAR* szSource,CHAR* szDest,CHAR* Divider);
	CHAR*	GetValidStr(CHAR* szSource,CHAR* szDest,CHAR Divider);
	CHAR*	GetValidStr(CHAR* szSource,INT nSrcLen,CHAR* szDest,INT nDestLen,CHAR Divider);

	CHAR		m_InnerResult[MAX_BUFFER_SIZE];

private:
	//
	CTextButton* m_pxBtn;
	CPDLList<CMText>*	m_pxpTxt;
	CHAR*		 m_pSourceText;
	BOOL		m_bReady;
	BOOL		m_bInited;
	CWHDXGraphicWindow*	m_pxGWND;		// Text 의 Width와 Height를 얻기 위해 사용


	// Functions

	CHAR*	ArrestString(CHAR* szSource,CHAR SearchAfter,CHAR AssertBefore,CHAR* AssertStr);

	INT		FindChr(CHAR* str,CHAR ch);
	INT		FindChr(CHAR* str,CHAR* ch);

	INT		StrLength(CHAR* str);


};

#endif // !defined(AFX_NPCTXTANALYSIS_H__F849FE95_8E23_4362_A140_29B8011E23D1__INCLUDED_)



