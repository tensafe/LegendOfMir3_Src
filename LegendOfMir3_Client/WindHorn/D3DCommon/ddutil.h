/*==========================================================================
 *
 *  Copyright (C) 1998-1999 Microsoft Corporation. All Rights Reserved.
 *
 *  File:       ddutil.cpp
 *  Content:    Routines for loading bitmap and palettes from resources
 *
 ***************************************************************************/

#ifdef __cplusplus
extern "C" {            /* Assume C declarations for C++ */
#endif	/* __cplusplus */

extern IDirectDrawPalette  *DDLoadPalette(IDirectDraw7 *pdd, LPCSTR szBitmap);
extern IDirectDrawSurface7 *DDLoadBitmap(IDirectDraw7 *pdd, LPCSTR szBitmap, int dx, int dy);
extern HRESULT              DDReLoadBitmap(IDirectDrawSurface7 *pdds, LPCSTR szBitmap);
extern HRESULT              DDCopyBitmap(IDirectDrawSurface7 *pdds, HBITMAP hbm, int x, int y, int dx, int dy);
extern DWORD                DDColorMatch(IDirectDrawSurface7 *pdds, COLORREF rgb);
extern HRESULT              DDSetColorKey(IDirectDrawSurface7 *pdds, COLORREF rgb);


extern LPDIRECTDRAWSURFACE7 FileToSurfaceData(LPDIRECTDRAW7 lpdd, CHAR* szFileName);
extern VOID SurfaceDataToFile(CHAR* szSaveFileName, LPDIRECTDRAWSURFACE7 lpdds);
extern LPDIRECTDRAWSURFACE7 LoadBitmap8BitsInSurface(LPDIRECTDRAW7 lpdd, LPCTSTR pszFileName);
extern LPDIRECTDRAWSURFACE7 LoadFile(LPDIRECTDRAW7 lpdd, LPCSTR filename );
extern LPDIRECTDRAWSURFACE7 LoadWilIntoSurface(LPDIRECTDRAW7 lpdd, CHAR* szWilFile);
#ifdef __cplusplus
}
#endif	/* __cplusplus */
