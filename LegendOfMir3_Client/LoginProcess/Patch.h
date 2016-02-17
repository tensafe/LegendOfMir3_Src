// Patch.h: interface for the CPatch class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PATCH_H__F89946B9_87EB_4621_933C_41AEFF63C4A4__INCLUDED_)
#define AFX_PATCH_H__F89946B9_87EB_4621_933C_41AEFF63C4A4__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include<Wininet.h>

#define		NO_ERROR_PATCH			  0
#define		ERROR_TO_PATCH			999

class CPatch  
{
public:
	UnCompressedFileNode*	GetFileNode(int Pos);
	VOID InitPatch(VOID);
// Public Function
								CPatch();
			HRESULT				GetFtpFiles(VOID);
			VOID				RenderSence(INT nLoopTime);
	virtual						~CPatch();
			BOOL				m_bEndPatch;
			BOOL				m_bPatched;
			VOID				DrawProgressImage(VOID);
private:

// Private Variable
			HINSTANCE			m_hLib;					// For Dll
			HINTERNET			m_HInet;
			HINTERNET			m_HFtp;
			HINTERNET			m_HFileFtp;
			HBITMAP				m_xBmp[2];				// For Image
			long				m_AnAmount;				// Current	Amount
			long				m_TheAmount;			// Total	Amount
			long				m_FACount;				// Total	File     Count
			HANDLE				m_hFile;
			char				m_UpdatePath[1024];
			UINT				m_PatchState;
			char				m_szPatchServerIP[16];
			int					m_Server_Port;
			char				m_UID[11];
			char				m_PASS[11];
			char				m_szPatchDate[9];

			BOOL				WillBePatch(VOID);
			HRESULT				GetFile(char* name);
			HINTERNET			OpenInet(char* name);
			HINTERNET			ConnectInet(HINTERNET hInet, char *sIP, WORD lPort, char *pID, char *pPass);
			short				Progress(long Total, long Current);
			BOOL				CheckPatchDate(VOID);
			BOOL				m_bWillbePatch1;
			BOOL				m_bWillbePatch2;
			UnCompressedFileNode**	 m_ppFilesData;

};

#endif // !defined(AFX_PATCH_H__F89946B9_87EB_4621_933C_41AEFF63C4A4__INCLUDED_)


// Patch States
// 0 Start
// 1 DownLoading File
// 2 Next File Load Need
// 3 No more File exist