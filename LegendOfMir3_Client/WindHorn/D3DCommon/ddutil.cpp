//-----------------------------------------------------------------------------
// File: ddutil.cpp
//
// Desc: Routines for loading bitmap and palettes from resources
//
//
// Copyright (c) 1995-1999 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

//-----------------------------------------------------------------------------
// Include files
//-----------------------------------------------------------------------------
#include "stdafx.h"

#include <windows.h>
#include <windowsx.h>
#include <ddraw.h>
#include <io.h>
#include "ddutil.h"




//-----------------------------------------------------------------------------
// Name: DDLoadBitmap()
// Desc: Create a DirectDrawSurface from a bitmap resource.
//-----------------------------------------------------------------------------
extern "C" IDirectDrawSurface7 *
DDLoadBitmap(IDirectDraw7 * pdd, LPCSTR szBitmap, int dx, int dy)
{
    HBITMAP                 hbm;
    BITMAP                  bm;
    DDSURFACEDESC2          ddsd;
    IDirectDrawSurface7    *pdds;

    //
    //  Try to load the bitmap as a resource, if that fails, try it as a file
    //
    hbm = (HBITMAP) LoadImage(GetModuleHandle(NULL), szBitmap, IMAGE_BITMAP, dx,
                              dy, LR_CREATEDIBSECTION);
    if (hbm == NULL)
        hbm = (HBITMAP) LoadImage(NULL, szBitmap, IMAGE_BITMAP, dx, dy,
                                  LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (hbm == NULL)
        return NULL;
    //
    // Get size of the bitmap
    //
    GetObject(hbm, sizeof(bm), &bm);
    //
    // Create a DirectDrawSurface for this bitmap
    //
    ZeroMemory(&ddsd, sizeof(ddsd));
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
    ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;
    ddsd.dwWidth = bm.bmWidth;
    ddsd.dwHeight = bm.bmHeight;
    if (pdd->CreateSurface(&ddsd, &pdds, NULL) != DD_OK)
        return NULL;
    DDCopyBitmap(pdds, hbm, 0, 0, 0, 0);
    DeleteObject(hbm);
    return pdds;
}




//-----------------------------------------------------------------------------
// Name: DDReLoadBitmap()
// Desc: Load a bitmap from a file or resource into a directdraw surface.
//       normaly used to re-load a surface after a restore.
//-----------------------------------------------------------------------------
HRESULT
DDReLoadBitmap(IDirectDrawSurface7 * pdds, LPCSTR szBitmap)
{
    HBITMAP                 hbm;
    HRESULT                 hr;

    //
    //  Try to load the bitmap as a resource, if that fails, try it as a file
    //
    hbm = (HBITMAP) LoadImage(GetModuleHandle(NULL), szBitmap, IMAGE_BITMAP, 0,
                              0, LR_CREATEDIBSECTION);
    if (hbm == NULL)
        hbm = (HBITMAP) LoadImage(NULL, szBitmap, IMAGE_BITMAP, 0, 0,
                                  LR_LOADFROMFILE | LR_CREATEDIBSECTION);
    if (hbm == NULL)
    {
        OutputDebugString("handle is null\n");
        return E_FAIL;
    }
    hr = DDCopyBitmap(pdds, hbm, 0, 0, 0, 0);
    if (hr != DD_OK)
    {
        OutputDebugString("ddcopybitmap failed\n");
    }
    DeleteObject(hbm);
    return hr;
}




//-----------------------------------------------------------------------------
// Name: DDCopyBitmap()
// Desc: Draw a bitmap into a DirectDrawSurface
//-----------------------------------------------------------------------------
extern "C" HRESULT
DDCopyBitmap(IDirectDrawSurface7 * pdds, HBITMAP hbm, int x, int y,
             int dx, int dy)
{
    HDC                     hdcImage;
    HDC                     hdc;
    BITMAP                  bm;
    DDSURFACEDESC2          ddsd;
    HRESULT                 hr;

    if (hbm == NULL || pdds == NULL)
        return E_FAIL;
    //
    // Make sure this surface is restored.
    //
    pdds->Restore();
    //
    // Select bitmap into a memoryDC so we can use it.
    //
    hdcImage = CreateCompatibleDC(NULL);
    if (!hdcImage)
        OutputDebugString("createcompatible dc failed\n");
    SelectObject(hdcImage, hbm);
    //
    // Get size of the bitmap
    //
    GetObject(hbm, sizeof(bm), &bm);
    dx = dx == 0 ? bm.bmWidth : dx;     // Use the passed size, unless zero
    dy = dy == 0 ? bm.bmHeight : dy;
    //
    // Get size of surface.
    //
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;
    pdds->GetSurfaceDesc(&ddsd);
    
    if ((hr = pdds->GetDC(&hdc)) == DD_OK)
    {
        StretchBlt(hdc, 0, 0, ddsd.dwWidth, ddsd.dwHeight, hdcImage, x, y,
                   dx, dy, SRCCOPY);
        pdds->ReleaseDC(hdc);
    }

    DeleteDC(hdcImage);
    return hr;
}




//-----------------------------------------------------------------------------
// Name: DDLoadPalette()
// Desc: Create a DirectDraw palette object from a bitmap resource
//       if the resource does not exist or NULL is passed create a
//       default 332 palette.
//-----------------------------------------------------------------------------
extern "C" IDirectDrawPalette *
DDLoadPalette(IDirectDraw7 * pdd, LPCSTR szBitmap)
{
    IDirectDrawPalette     *ddpal;
    int                     i;
    int                     n;
    int                     fh;
    HRSRC                   h;
    LPBITMAPINFOHEADER      lpbi;
    PALETTEENTRY            ape[256];
    RGBQUAD                *prgb;

    //
    // Build a 3:3:2 palette as the default.
    //
    for (i = 0; i < 256; i++)
    {
        ape[i].peRed = (BYTE) (((i >> 5) & 0x07) * 255 / 7);
        ape[i].peGreen = (BYTE) (((i >> 2) & 0x07) * 255 / 7);
        ape[i].peBlue = (BYTE) (((i >> 0) & 0x03) * 255 / 3);
        ape[i].peFlags = (BYTE) 0;
    }
    //
    // Get a pointer to the bitmap resource.
    //
    if (szBitmap && (h = FindResource(NULL, szBitmap, RT_BITMAP)))
    {
        lpbi = (LPBITMAPINFOHEADER) LockResource(LoadResource(NULL, h));
        if (!lpbi)
            OutputDebugString("lock resource failed\n");
        prgb = (RGBQUAD *) ((BYTE *) lpbi + lpbi->biSize);
        if (lpbi == NULL || lpbi->biSize < sizeof(BITMAPINFOHEADER))
            n = 0;
        else if (lpbi->biBitCount > 8)
            n = 0;
        else if (lpbi->biClrUsed == 0)
            n = 1 << lpbi->biBitCount;
        else
            n = lpbi->biClrUsed;
        //
        //  A DIB color table has its colors stored BGR not RGB
        //  so flip them around.
        //
        for (i = 0; i < n; i++)
        {
            ape[i].peRed = prgb[i].rgbRed;
            ape[i].peGreen = prgb[i].rgbGreen;
            ape[i].peBlue = prgb[i].rgbBlue;
            ape[i].peFlags = 0;
        }
    }
    else if (szBitmap && (fh = _lopen(szBitmap, OF_READ)) != -1)
    {
        BITMAPFILEHEADER        bf;
        BITMAPINFOHEADER        bi;

        _lread(fh, &bf, sizeof(bf));
        _lread(fh, &bi, sizeof(bi));
        _lread(fh, ape, sizeof(ape));
        _lclose(fh);
        if (bi.biSize != sizeof(BITMAPINFOHEADER))
            n = 0;
        else if (bi.biBitCount > 8)
            n = 0;
        else if (bi.biClrUsed == 0)
            n = 1 << bi.biBitCount;
        else
            n = bi.biClrUsed;
        //
        //  A DIB color table has its colors stored BGR not RGB
        //  so flip them around.
        //
        for (i = 0; i < n; i++)
        {
            BYTE        r = ape[i].peRed;

            ape[i].peRed = ape[i].peBlue;
            ape[i].peBlue = r;
        }
    }
    pdd->CreatePalette(DDPCAPS_8BIT, ape, &ddpal, NULL);
    return ddpal;
}




//-----------------------------------------------------------------------------
// Name: DDColorMatch()
// Desc: Convert a RGB color to a pysical color.
//       We do this by leting GDI SetPixel() do the color matching
//       then we lock the memory and see what it got mapped to.
//-----------------------------------------------------------------------------
extern "C" DWORD
DDColorMatch(IDirectDrawSurface7 * pdds, COLORREF rgb)
{
    COLORREF                rgbT;
    HDC                     hdc;
    DWORD                   dw = CLR_INVALID;
    DDSURFACEDESC2          ddsd;
    HRESULT                 hres;

    //
    //  Use GDI SetPixel to color match for us
    //
    if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK)
    {
        rgbT = GetPixel(hdc, 0, 0);     // Save current pixel value
        SetPixel(hdc, 0, 0, rgb);       // Set our value
        pdds->ReleaseDC(hdc);
    }
    //
    // Now lock the surface so we can read back the converted color
    //
    ddsd.dwSize = sizeof(ddsd);
    while ((hres = pdds->Lock(NULL, &ddsd, 0, NULL)) == DDERR_WASSTILLDRAWING);
    if (hres == DD_OK)
    {
        dw = *(DWORD *) ddsd.lpSurface;                 // Get DWORD
        if (ddsd.ddpfPixelFormat.dwRGBBitCount < 32)
            dw &= (1 << ddsd.ddpfPixelFormat.dwRGBBitCount) - 1;  // Mask it to bpp
        pdds->Unlock(NULL);
    }
    //
    //  Now put the color that was there back.
    //
    if (rgb != CLR_INVALID && pdds->GetDC(&hdc) == DD_OK)
    {
        SetPixel(hdc, 0, 0, rgbT);
        pdds->ReleaseDC(hdc);
    }
    return dw;
}




//-----------------------------------------------------------------------------
// Name: DDSetColorKey()
// Desc: Set a color key for a surface, given a RGB.
//       If you pass CLR_INVALID as the color key, the pixel
//       in the upper-left corner will be used.
//-----------------------------------------------------------------------------
extern "C" HRESULT
DDSetColorKey(IDirectDrawSurface7 * pdds, COLORREF rgb)
{
    DDCOLORKEY              ddck;

    ddck.dwColorSpaceLowValue = RGB(0, 0, 0);//DDColorMatch(pdds, rgb);
    ddck.dwColorSpaceHighValue = RGB(1, 1, 1);//ddck.dwColorSpaceLowValue;
    return pdds->SetColorKey(DDCKEY_SRCBLT, &ddck);
}




VOID SurfaceDataToFile(CHAR* szSaveFileName, LPDIRECTDRAWSURFACE7 lpdds)
{
    DDSURFACEDESC2 ddsd;
    ddsd.dwSize = sizeof(ddsd);
    ddsd.dwFlags = DDSD_HEIGHT | DDSD_WIDTH;
    lpdds->Lock(NULL, &ddsd, 0, NULL);
	BYTE* pb;
	pb = new BYTE[ddsd.dwWidth*ddsd.dwHeight];
	ZeroMemory(pb, ddsd.dwWidth*ddsd.dwHeight);
	memcpy(pb, ddsd.lpSurface, ddsd.dwWidth*ddsd.dwHeight);

	WORD wWidth = (WORD)ddsd.dwWidth;
	WORD wHeight = (WORD)ddsd.dwHeight;

    lpdds->Unlock(NULL);

	HANDLE hFile;
	hFile = CreateFile( szSaveFileName, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
						FILE_ATTRIBUTE_NORMAL, NULL );

	if ( hFile != INVALID_HANDLE_VALUE )
	{
		DWORD dwReadLen;
		SetFilePointer(hFile, 0, 0, FILE_BEGIN);
		WriteFile(hFile, &wWidth, sizeof(WORD), &dwReadLen, NULL);
		WriteFile(hFile, &wHeight, sizeof(WORD), &dwReadLen, NULL);
		WriteFile(hFile, pb, ddsd.dwWidth*ddsd.dwHeight, &dwReadLen, NULL);
		CloseHandle(hFile);
	}
	free(pb);
}


LPDIRECTDRAWSURFACE7 FileToSurfaceData(LPDIRECTDRAW7 lpdd, CHAR* szFileName)
{
	HANDLE hFile;

	hFile = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
					   FILE_ATTRIBUTE_NORMAL, NULL );

	WORD wWidth, wHeight;
	LPDIRECTDRAWSURFACE7 lpdds;
    DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
    ddsd.dwSize = sizeof(ddsd);

	if ( hFile != INVALID_HANDLE_VALUE )
	{
		DWORD dwReadLen;
		ReadFile(hFile, &wWidth, sizeof(WORD), &dwReadLen, NULL);
		ReadFile(hFile, &wHeight, sizeof(WORD), &dwReadLen, NULL);

		ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;// | DDSCAPS_SYSTEMMEMORY;
		ddsd.dwWidth    = wWidth;
		ddsd.dwHeight   = wHeight;
		lpdd->CreateSurface(&ddsd, &lpdds, NULL);
		lpdds->Lock(NULL, &ddsd, 0, NULL);
		ReadFile(hFile, (BYTE*)ddsd.lpSurface, wWidth*wHeight, &dwReadLen, NULL);
		lpdds->Unlock(NULL);
		CloseHandle(hFile);
		return lpdds;
	}
	return NULL;
}

	
LPDIRECTDRAWSURFACE7 LoadBitmap8BitsInSurface(LPDIRECTDRAW7 lpdd, LPCTSTR pszFileName)
{
	BITMAPFILEHEADER	stFileHeader;
	LPBITMAPINFO		pstBmpInfo;
	LPBITMAPINFOHEADER	pstInfoHeader;
	DWORD				dwReadSize = 0;

	HANDLE hFile = ::CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL, NULL);
	// 비트맵 헤더정보.(순서대로 기록된다.)
	//BITMAPFILEHEADER
	//BITMAPINFO(BITMAPINFOHEADER, RGBQUAD)
	//이미지데이터


	if ( hFile )
	{
		ZeroMemory(&stFileHeader, sizeof(stFileHeader));

		// 파일헤더읽기.
		ReadFile(hFile, &stFileHeader, sizeof(stFileHeader), &dwReadSize, NULL);

		// BMP인지를 확인한다.
		if ( HIBYTE(stFileHeader.bfType) == 'M' && LOBYTE(stFileHeader.bfType) == 'B' )
		{
			pstBmpInfo = (LPBITMAPINFO) malloc(sizeof(BITMAPINFO) + sizeof(RGBQUAD)*256);

			// 인포헤더만큼만 일단 읽는다.
			ReadFile(hFile, pstBmpInfo, sizeof(BITMAPINFO)-sizeof(RGBQUAD), &dwReadSize, NULL);

			pstInfoHeader = &(pstBmpInfo->bmiHeader);

			// 8비트임을 확인한다.
			if( pstInfoHeader->biBitCount == 8 && pstInfoHeader->biCompression == BI_RGB)
			{
				DWORD dwColorTableSize = 256 * sizeof(RGBQUAD);

				ReadFile(hFile, pstBmpInfo->bmiColors, dwColorTableSize, &dwReadSize, NULL);

				BYTE* pbDataBit = NULL;
//				BYTE* pbDataBit = (BYTE*)malloc(pstBmpInfo->bmiHeader.biSizeImage);
				if ( pstBmpInfo->bmiHeader.biSizeImage )
				{
					pbDataBit = (BYTE*)malloc(pstBmpInfo->bmiHeader.biSizeImage);
					ReadFile(hFile, pbDataBit, pstBmpInfo->bmiHeader.biSizeImage, &dwReadSize, NULL);
				}
				else
				{
					pbDataBit = (BYTE*)malloc(pstBmpInfo->bmiHeader.biWidth*pstBmpInfo->bmiHeader.biHeight);
					ReadFile(hFile, pbDataBit, pstBmpInfo->bmiHeader.biWidth*pstBmpInfo->bmiHeader.biHeight, &dwReadSize, NULL);
				}

				// 실제얻은데이타를 서피스에 넣는다.
				LPDIRECTDRAWSURFACE7 lpdds;
				DDSURFACEDESC2 ddsd;
				ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
				ddsd.dwSize			= sizeof(ddsd);
				ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;/*| DDSD_PIXELFORMAT;
				ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
				ddsd.ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED8 | DDPF_RGB;
				ddsd.ddpfPixelFormat.dwRGBBitCount = 8;*/
				ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
				ddsd.dwWidth		= pstBmpInfo->bmiHeader.biWidth;
				ddsd.dwHeight		= pstBmpInfo->bmiHeader.biHeight;
				lpdd->CreateSurface(&ddsd, &lpdds, NULL);
				lpdds->Lock(NULL, &ddsd, 0, NULL);

//라인별 데이타는 같지만 라인이 위부터 뒤바뀌었기때문에 다시 돌려준다.
/*				INT nLineCnt = 0;
				for ( INT nY=(ddsd.dwHeight-1); nY>=0; nY-- )
				{
					memcpy((BYTE*)ddsd.lpSurface+(nLineCnt*ddsd.lPitch), pbDataBit+(nY*ddsd.lPitch), ddsd.lPitch);
					nLineCnt++;
				}
*/
				for ( UINT nY=0; nY<(ddsd.dwHeight-1); nY++ )
				{
					memcpy((BYTE*)ddsd.lpSurface+(nY*ddsd.lPitch), pbDataBit+((ddsd.dwHeight-nY-1)*ddsd.dwWidth), ddsd.dwWidth);
				}

//				memcpy((BYTE*)ddsd.lpSurface, pbDataBit, ddsd.dwWidth*ddsd.dwHeight);
				lpdds->Unlock(NULL);

				free(pbDataBit);

				return lpdds;
			}
		}

		CloseHandle(hFile);
	}

	return NULL;
}


LPDIRECTDRAWSURFACE7 LoadWilIntoSurface(LPDIRECTDRAW7 lpdd, CHAR* szWilFile)
{
	HANDLE hFile;
	hFile = CreateFile(szWilFile, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_READ, NULL, OPEN_ALWAYS,
					   FILE_ATTRIBUTE_NORMAL, NULL );
	if ( hFile != INVALID_HANDLE_VALUE )
	{
		DWORD		dwReadLen;
		CHAR		szTitle[44];
		INT			nImageCount;
		INT			nColorCount;
		INT			nPaletteSize;
		SHORT		shWidth;
		SHORT		shHeight;
		SHORT		shPX;
		SHORT		shPY;
		RGBQUAD		rgbQuad[256];
		BYTE*		pbData;

		ReadFile(hFile, szTitle, 44, &dwReadLen, NULL);
		ReadFile(hFile, &nImageCount, sizeof(INT), &dwReadLen, NULL);
		ReadFile(hFile, &nColorCount, sizeof(INT), &dwReadLen, NULL);
		ReadFile(hFile, &nPaletteSize, sizeof(INT), &dwReadLen, NULL);
		ReadFile(hFile, rgbQuad, sizeof(RGBQUAD)*256, &dwReadLen, NULL);
		ReadFile(hFile, &shWidth, sizeof(SHORT), &dwReadLen, NULL);
		ReadFile(hFile, &shHeight, sizeof(SHORT), &dwReadLen, NULL);
		ReadFile(hFile, &shPX, sizeof(SHORT), &dwReadLen, NULL);
		ReadFile(hFile, &shPY, sizeof(SHORT), &dwReadLen, NULL);

		

		pbData = new BYTE[shWidth*shHeight];
		ReadFile(hFile, pbData, shWidth*shHeight, &dwReadLen, NULL);
		CloseHandle(hFile);

		// 실제얻은데이타를 서피스에 넣는다.
		LPDIRECTDRAWSURFACE7 lpdds;
		DDSURFACEDESC2 ddsd;
		ZeroMemory(&ddsd, sizeof(DDSURFACEDESC2));
		ddsd.dwSize			= sizeof(ddsd);
		ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;/*| DDSD_PIXELFORMAT;
		ddsd.ddpfPixelFormat.dwSize = sizeof(DDPIXELFORMAT);
		ddsd.ddpfPixelFormat.dwFlags = DDPF_PALETTEINDEXED8 | DDPF_RGB;
		ddsd.ddpfPixelFormat.dwRGBBitCount = 8;*/
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
		ddsd.dwWidth		= shWidth;
		ddsd.dwHeight		= shHeight;
		lpdd->CreateSurface(&ddsd, &lpdds, NULL);
		lpdds->Lock(NULL, &ddsd, 0, NULL);

//라인별 데이타는 같지만 라인이 위부터 뒤바뀌었기때문에 다시 돌려준다.
/*				INT nLineCnt = 0;
		for ( INT nY=(ddsd.dwHeight-1); nY>=0; nY-- )
		{
			memcpy((BYTE*)ddsd.lpSurface+(nLineCnt*ddsd.lPitch), pbDataBit+(nY*ddsd.lPitch), ddsd.lPitch);
			nLineCnt++;
		}
*/
		for ( UINT nY=0; nY<(ddsd.dwHeight-1); nY++ )
		{
			memcpy((BYTE*)ddsd.lpSurface+(nY*ddsd.lPitch), pbData+((ddsd.dwHeight-nY-1)*ddsd.dwWidth), ddsd.dwWidth);
		}

//				memcpy((BYTE*)ddsd.lpSurface, pbDataBit, ddsd.dwWidth*ddsd.dwHeight);
		lpdds->Unlock(NULL);

		free(pbData);		

		CloseHandle(hFile);
		return lpdds;
	}
	return NULL;
}