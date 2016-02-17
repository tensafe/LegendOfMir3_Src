/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/



#include "stdafx.h"
#include "WHEngine.h"



/******************************************************************************************************************

	함수명 : CWHSurface::ReleaseSurface()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CWHSurface::ReleaseSurface()
{
	if ( m_pDDS )
	{
		m_pDDS->Release();
		m_pDDS = NULL;

		return TRUE;
	}

	return FALSE;
}



/******************************************************************************************************************

	함수명 : CWHSurface::CreateOffScreenSurface()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : LPDIRECTDRAW7 pDD
	         DWORD dwXSize
	         DWORD dwYSize
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CWHSurface::CreateOffScreenSurface(LPDIRECTDRAW7 pDD, DWORD dwXSize, DWORD dwYSize) 
{
	HRESULT					hr;
	DDSURFACEDESC2			ddsd;

	ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));

	ddsd.dwSize			= sizeof(DDSURFACEDESC2);
	ddsd.dwFlags		= DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth		= dwXSize;
	ddsd.dwHeight		= dwYSize;

	hr = pDD->CreateSurface(&ddsd, &m_pDDS, NULL);

	if ( hr != DD_OK )	return FALSE;

	return TRUE;
}



/******************************************************************************************************************

	함수명 : CWHSurface::CopyImageToSurface()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : DWORD dwSrcWidth
	         DWORD dwSrcHeight
	         WORD* pwSrc
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CWHSurface::CopyImageToSurface(DWORD dwSrcWidth, DWORD dwSrcHeight, WORD* pwSrc)
{
	DDSURFACEDESC2			ddsd;
	WORD*					pwdDst = NULL;

	ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));

	ddsd.dwSize	= sizeof(DDSURFACEDESC2);

	if (m_pDDS->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL) == DD_OK)
	{
		pwdDst = (WORD*)ddsd.lpSurface;

		CopyMemory(pwdDst, pwSrc, dwSrcWidth * dwSrcHeight * sizeof(WORD));

		m_pDDS->Unlock(NULL);

		DDCOLORKEY              ddck;
		ddck.dwColorSpaceLowValue =  RGB( 0,  0,  0);
		ddck.dwColorSpaceHighValue = RGB(10, 10, 10);
		m_pDDS->SetColorKey(DDCKEY_SRCBLT, &ddck);

		return TRUE;
	}

	return FALSE;
}
