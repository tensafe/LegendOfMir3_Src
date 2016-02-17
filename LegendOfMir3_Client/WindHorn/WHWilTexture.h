/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/

#ifndef D3DWILTEXTR_H
#define D3DWILTEXTR_H
#include <ddraw.h>
#include <d3d.h>

#define D3DTEXTR_TRANSPARENTWHITE 0x00000001
#define D3DTEXTR_TRANSPARENTBLACK 0x00000002

LPDIRECTDRAWSURFACE7 D3DWILTextr_GetSurface(TCHAR* strWilFileName, INT nIndex);
HRESULT D3DWILTextr_Invalidate(TCHAR* strWilFileName, INT nIndex);
HRESULT D3DWILTextr_Restore(TCHAR* strWilFileName, INT nIndex, LPDIRECT3DDEVICE7 pd3dDevice);
LPDIRECTDRAWSURFACE7 D3DWILTextr_RestoreEx(TCHAR* strWilFileName, INT nIndex, LPDIRECT3DDEVICE7 pd3dDevice);
HRESULT D3DWILTextr_InvalidateAllTextures();
HRESULT D3DWILTextr_DestroyAllTextures();
HRESULT D3DWILTextr_RestoreAllTextures(LPDIRECT3DDEVICE7 pd3dDevice);
//HRESULT D3DWILTextr_CreateTextureFromFile(TCHAR* strWilFileName, INT nIndex, DWORD dwFlags=0L);
BOOL	D3DWILTextr_CreateEmptyTexture(TCHAR* strWilFileName, INT nIndex, 
									   DWORD dwWidth, DWORD dwHeight, WORD* pwData, DWORD dwFlags);
HRESULT D3DWILTextr_DestroyTexture(TCHAR* strWilFileName, INT nIndex);
VOID    D3DWILTextr_SetTexturePath(TCHAR* strTexturePath);
BOOL    D3DWILTextr_IsTextrExisted(TCHAR* strWilFileName, INT nIdx);
INT		GetRandomNum(INT nFrom, INT nTo);

//POINT	D3DWILTextr_GetImageSize(TCHAR* strWilFileName, INT nIndex);


// 텍스춰로 쓰는 모든 윌파일을 관리한다.
/*
    WILTextureContainer* m_pNext;

    TCHAR   m_strWilFileName[80];
	INT		m_nIdx;
    DWORD   m_dwFlags;
    DWORD   m_dwWidth;
    DWORD   m_dwHeight;
    LPDIRECTDRAWSURFACE7 m_pddsSurface;
*/
// [개요]  파일 맵핑을 이용하여 데이타에 접근한다.(윌파일과 윅스 파일이용)
//    ---> CMappedWilFile클래스에서 m_strWilFileName, m_nIdx, m_dwWidth, 
//		   m_dwHeight, 이미지데이타주소 를 넘겨 받아서 텍스춰용 서피스를 만든다.


#endif // D3DWILTEXTR_H
