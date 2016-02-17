/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/



#include "stdafx.h"
#include "WHEngine.h"



/******************************************************************************************************************

	함수명 : CWHImage::CWHImage()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
CWHImage::CWHImage()
{
	m_wWidth	= 0;
	m_wHeight	= 0;
	m_shCenterX	= 0;;
	m_shCenterY	= 0;;
	m_bBitCount	= 0;;
	m_pwImage	= NULL;
	m_dwImageLength = 0;
}



/******************************************************************************************************************

	함수명 : CWHImage::~CWHImage()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
CWHImage::~CWHImage()
{
	DeleteImage();
}



/******************************************************************************************************************

	함수명 : CWHImage::Convert24To16()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : BYTE *pOrgImage
	         BYTE bOrgImgBitCount
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CWHImage::Convert24To16(BYTE *pOrgImage, BYTE bOrgImgBitCount)
{
	BYTE byRBit, byGBit, byBBit;
	WORD wPixel;

	int nByteAlignCX = (((m_wWidth * bOrgImgBitCount) + 31) & ~31) >> 3;

	for (int nYIdx = 0; nYIdx < m_wHeight; nYIdx++)
	{
		for (int nXIdx = 0; nXIdx < m_wWidth; nXIdx++)
		{			
			byRBit = pOrgImage[(nXIdx * 3 + 2) + (nYIdx * nByteAlignCX)];
			byGBit = pOrgImage[(nXIdx * 3 + 1) + (nYIdx * nByteAlignCX)];
			byBBit = pOrgImage[(nXIdx * 3 + 0) + (nYIdx * nByteAlignCX)];
			
			wPixel = ((byRBit >> 3) << 11) | ((byGBit >> 2) << 5) | (byBBit >> 3);

			if (((byRBit != 0) || (byGBit != 0) || (byBBit != 0)) && (wPixel == 0))
				wPixel = 1;

			// 비트맵을 거꾸로 돌림
			m_pwImage[nXIdx + ((m_wHeight - 1) - nYIdx) * m_wWidth] = wPixel;
		}
	}

	return TRUE;
}



/******************************************************************************************************************

	함수명 : CWHImage::LoadBitmap()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : LPCTSTR pszFileName
	         BYTE bBitCount
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CWHImage::LoadBitmap(LPCTSTR pszFileName, BOOL fCut, BOOL fCompress, BYTE bBitCount)
{
	BITMAPFILEHEADER	stFileHeader;
	BITMAPINFOHEADER	stInfoHeader;
	DWORD				dwReadSize = 0;

	HANDLE hFile = ::CreateFile(pszFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING,
								FILE_ATTRIBUTE_NORMAL, NULL);

	if ( hFile )
	{
		ZeroMemory(&stFileHeader, sizeof(stFileHeader));

		ReadFile(hFile, &stFileHeader, sizeof(stFileHeader), &dwReadSize, NULL);

		if ( HIBYTE(stFileHeader.bfType) == 'M' && LOBYTE(stFileHeader.bfType) == 'B' )
		{
			ReadFile(hFile, &stInfoHeader, sizeof(stInfoHeader), &dwReadSize, NULL);

			m_wWidth  = (WORD)stInfoHeader.biWidth;
			m_wHeight = (WORD)stInfoHeader.biHeight;

			m_dwImageLength = m_wWidth*m_wHeight*sizeof(WORD);

			if ( stInfoHeader.biBitCount == 24 )
			{
				m_pwImage = new WORD[sizeof(WORD) * m_wWidth * m_wHeight];

				UINT nSize = stFileHeader.bfSize -  sizeof(stFileHeader) -  sizeof(stInfoHeader);

				if ( bBitCount != 16 )
				{
					ReadFile(hFile, m_pwImage, nSize, &dwReadSize, NULL);

					CloseHandle(hFile);

					if ( fCut )						Optimize();
					if ( fCompress )				XOptimize();
					

					return TRUE;
				}
				else
				{	
					BYTE *pOrgImage = new BYTE[stInfoHeader.biSizeImage];

					ReadFile(hFile, pOrgImage, nSize, &dwReadSize, NULL);

					Convert24To16(pOrgImage, (BYTE)stInfoHeader.biBitCount);
		
					CloseHandle(hFile);

					delete [] pOrgImage;

					if ( fCut )						Optimize();
					if ( fCompress )				XOptimize();

					return TRUE;
				}
			}
		}

		CloseHandle(hFile);
	}

	return FALSE;
}



BOOL CWHImage::Optimize()
{
	INT nStartX, nStartY, nEndX, nEndY;	
	INT nX, nY;
	INT nCntX = 0;
	INT nCntY = 0;
		
	for ( nY=0; nY<m_wHeight; nY++ )
	{
		for ( nX=0; nX<m_wWidth; nX++ )
		{
			if ( *(m_pwImage + nX + (nY * m_wWidth)) != 0 )
			{
				nStartY = nY;
				nX = m_wWidth;
				nY = m_wHeight;
			}
		}
	}

	for ( nX=0; nX<m_wWidth; nX++ )
	{
		for ( nY=0; nY<m_wHeight; nY++ )
		{
			if ( *(m_pwImage + nX + (nY * m_wWidth)) != 0 )
			{
				nStartX = nX;
				nX = m_wWidth;
				nY = m_wHeight;
			}
		}
	}

	for ( nY=m_wHeight-1; nY>=0; nY-- )
	{
		for(nX=m_wWidth-1; nX>=0; nX-- )
		{
			if ( *(m_pwImage + nX + (nY * m_wWidth)) != 0 )
			{
				nEndY = nY;
				nX = -1;
				nY = -1;
			}
		}	
	}

	for ( nX=m_wWidth-1; nX>=0; nX-- )
	{
		for ( nY=m_wHeight-1; nY>=0; nY-- )
		{
			if ( *(m_pwImage + nX + (nY * m_wWidth)) != 0 )
			{
				nEndX = nX;
				nX = -1;
				nY = -1;
			}
		}	
	}

	m_shCenterX = m_wWidth /2 - nStartX;
	m_shCenterY = m_wHeight /2 - nStartY;


	WORD*	pwTmpImage	= NULL;
	WORD	wTmpWidth	= 0;
	WORD	wTmpHeight	= 0;

	wTmpWidth	= (nEndX + 1)- nStartX;

	while( wTmpWidth % 4 != 0 )
	{
		nEndX++;
		wTmpWidth	= (nEndX + 1) - nStartX;
	}
	wTmpHeight	= (nEndY + 1) - nStartY;

	pwTmpImage  = new WORD[wTmpWidth * wTmpHeight];

	for ( nY=nStartY; nY<(nEndY+1); nY++ )
	{
		for ( nX=nStartX; nX<(nEndX+1); nX++ )
		{
			*(pwTmpImage+nCntY+(nCntX*wTmpWidth)) = *(m_pwImage+nX+(nY*m_wWidth));
			nCntY++;
		}
		nCntY = 0;
		nCntX++;
	}

	DeleteImage();

	if ( !m_pwImage )
		m_pwImage  = new WORD[wTmpWidth * wTmpHeight];
	
	CopyMemory(m_pwImage, pwTmpImage, sizeof(WORD) * wTmpWidth * wTmpHeight);

	m_wWidth	= wTmpWidth;
	m_wHeight	= wTmpHeight;

	m_dwImageLength = m_wWidth*m_wHeight*sizeof(WORD);

	SAFE_DELETE(pwTmpImage);

	return TRUE;
}


BOOL CWHImage::XOptimize()
{
	INT nTmpMaxLen = m_wWidth * 2;
	WORD* pwTmpMaxLineImage = new WORD[nTmpMaxLen];

	DWORD dwWordCntPerLine = 0;
	WORD  wContinuousCnt = 0;
	DWORD dwWordCntPerFile = 0;
	DWORD dwWrittenLen	= 0;

	HANDLE hFile;
	hFile = CreateFile("CompTest.tmp", GENERIC_WRITE|GENERIC_READ, FILE_SHARE_WRITE|FILE_SHARE_READ, NULL, 
					   CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);


	INT nAddCnt = 0;

	if ( hFile != INVALID_HANDLE_VALUE )
	{
		for ( INT nY = 0; nY < m_wHeight; nY++ )
		{
			for ( INT nX = 0; nX < m_wWidth;  )
			{
				// 라인단위로 압축을 한다.

				if ( *(m_pwImage+nX+(nY*m_wWidth)) == 0 )
				{
					while ( (*(m_pwImage+(nX+nAddCnt)+(nY*m_wWidth)) == 0) && (nX+nAddCnt < m_wWidth) )
					{
						nAddCnt++;
					}
					pwTmpMaxLineImage[dwWordCntPerLine] = 0XC0;
					dwWordCntPerLine++;
					pwTmpMaxLineImage[dwWordCntPerLine] = nAddCnt;
					dwWordCntPerLine++;

					nX += nAddCnt;
					nAddCnt = 0;
				}
				else if ( *(m_pwImage+nX+(nY*m_wWidth)) == 0XFFFF )
				{
					while ( (*(m_pwImage+(nX+nAddCnt)+(nY*m_wWidth)) == 0XFFFF) && (nX+nAddCnt < m_wWidth) )
					{
						nAddCnt++;
					}
					pwTmpMaxLineImage[dwWordCntPerLine] = 0XC1;
					dwWordCntPerLine++;
					pwTmpMaxLineImage[dwWordCntPerLine] = nAddCnt;
					dwWordCntPerLine++;

					nX += nAddCnt;
					nAddCnt = 0;
				}
				else
				{
					while ( (*(m_pwImage+(nX+nAddCnt)+(nY*m_wWidth)) != 0) && (*(m_pwImage+(nX+nAddCnt)+(nY*m_wWidth)) != 0XFFFF) && (nX+nAddCnt < m_wWidth) )
					{
						nAddCnt++;
					}
					pwTmpMaxLineImage[dwWordCntPerLine] = 0XC2;
					dwWordCntPerLine++;
					pwTmpMaxLineImage[dwWordCntPerLine] = nAddCnt;
					dwWordCntPerLine++;

					while ( nAddCnt > 0 )
					{
						pwTmpMaxLineImage[dwWordCntPerLine] = *(m_pwImage+nX+(nY*m_wWidth));
						dwWordCntPerLine++;
						nAddCnt--;
						nX++;
					}
				}
			}
			nAddCnt = 0;
			WriteFile(hFile, &dwWordCntPerLine, sizeof(WORD), &dwWrittenLen, NULL);
			WriteFile(hFile, pwTmpMaxLineImage, sizeof(WORD)*dwWordCntPerLine, &dwWrittenLen, NULL);
			dwWordCntPerFile += (dwWordCntPerLine+1);
			dwWordCntPerLine = 0;
		}

		SAFE_DELETE(pwTmpMaxLineImage);

		SAFE_DELETE(m_pwImage);
		m_pwImage  = new WORD[dwWordCntPerFile];
		ZeroMemory(m_pwImage, sizeof(WORD)*dwWordCntPerFile);
		m_dwImageLength = dwWordCntPerFile;

		SetFilePointer(hFile, 0, NULL, FILE_BEGIN);
		ReadFile(hFile, m_pwImage, sizeof(WORD)*dwWordCntPerFile, &dwWrittenLen, NULL);

		CloseHandle(hFile);

		return TRUE;
	}
	return FALSE;
}


BOOL CWHImage::LoadBitmapFromRes(LPCTSTR lpResName, LPCTSTR lpTypeName, BOOL fCut, BOOL fCompress, BYTE bBitCount)
{
	BITMAPFILEHEADER	stFileHeader;
	BITMAPINFOHEADER	stInfoHeader;
	DWORD				dwReadSize = 0;
	DWORD				dwCurrMemPos = 0;
	HRSRC hRes = FindResource(GetModuleHandle(NULL), lpResName, lpTypeName);

	if ( hRes )
	{
		DWORD dwSize = SizeofResource(GetModuleHandle(NULL), hRes);
		HGLOBAL hMem = LoadResource(GetModuleHandle(NULL), hRes);
		BYTE* pbData = (BYTE*)LockResource(hMem);

		memcpy(&stFileHeader, pbData+dwCurrMemPos, sizeof(stFileHeader));
		dwCurrMemPos += sizeof(stFileHeader);

		if ( HIBYTE(stFileHeader.bfType) == 'M' && LOBYTE(stFileHeader.bfType) == 'B' )
		{
			memcpy(&stInfoHeader, pbData+dwCurrMemPos, sizeof(stInfoHeader));
			dwCurrMemPos += sizeof(stInfoHeader);

			m_wWidth  = (WORD)stInfoHeader.biWidth;
			m_wHeight = (WORD)stInfoHeader.biHeight;

			m_dwImageLength = m_wWidth*m_wHeight*sizeof(WORD);

			if ( stInfoHeader.biBitCount == 24 )
			{
				m_pwImage = new WORD[sizeof(WORD) * m_wWidth * m_wHeight];

				UINT nSize = stFileHeader.bfSize -  sizeof(stFileHeader) -  sizeof(stInfoHeader);

				if ( bBitCount != 16 )
				{
					memcpy(m_pwImage, pbData+dwCurrMemPos, nSize);
					dwCurrMemPos += nSize;


					if ( fCut )						Optimize();
					if ( fCompress )				XOptimize();

					return TRUE;
				}
				else
				{	
					BYTE *pOrgImage = new BYTE[stInfoHeader.biSizeImage];

					memcpy(pOrgImage, pbData+dwCurrMemPos, nSize);
					dwCurrMemPos += nSize;

					Convert24To16(pOrgImage, (BYTE)stInfoHeader.biBitCount);
		
					delete [] pOrgImage;

					if ( fCut )						Optimize();
					if ( fCompress )				XOptimize();

					return TRUE;
				}
			}
		}
	}

	return FALSE;
}
