/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/
#include "StdAfx.h"



/******************************************************************************************************************

	함수명 : CMapHandler::CMapHandler()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
CMapHandler::CMapHandler()
{
	InitMapHandler();
}



/******************************************************************************************************************

	함수명 : CMapHandler::~CMapHandler()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
CMapHandler::~CMapHandler()
{
	DestroyMapHandler();
}



/******************************************************************************************************************

	함수명 : CMapHandler::InitMapHandler()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CMapHandler::InitMapHandler()
{
	m_pstTileInfo		= NULL;
	m_pstCellInfo		= NULL;
//	m_pstDoorInfo		= NULL;
//	m_pbCellIDoorIdx	= NULL;

	m_shStartViewTileX	= 0;	
	m_shStartViewTileY	= 0;
	m_shViewOffsetX		= 0;
	m_shViewOffsetY		= 0;
//	m_bDoorCount		= 0;

	m_ptMovePixelArray[0].x =  8; m_ptMovePixelArray[0].y =  5;
	m_ptMovePixelArray[1].x = 16; m_ptMovePixelArray[1].y = 11;
	m_ptMovePixelArray[2].x = 24; m_ptMovePixelArray[2].y = 16;
	m_ptMovePixelArray[3].x = 32; m_ptMovePixelArray[3].y = 21;
	m_ptMovePixelArray[4].x = 40; m_ptMovePixelArray[4].y = 27;
	m_ptMovePixelArray[5].x = 48; m_ptMovePixelArray[5].y = 32;

	ZeroMemory(m_wSavedMapBuffer, sizeof(WORD)*_VIEW_CELL_X_COUNT*_CELL_WIDTH*_VIEW_CELL_Y_COUNT*_CELL_HEIGHT);
	ZeroMemory(m_bAniTileFrame, sizeof(BYTE)*8*16);
	ZeroMemory(m_dwAniSaveTime, sizeof(DWORD)*8);
	ZeroMemory(&m_stMapFileHeader, sizeof(MAPFILEHEADER));

	SetRect(&m_rcView, 0, 0, _VIEW_CELL_X_COUNT*_CELL_WIDTH, _VIEW_CELL_Y_COUNT*_CELL_HEIGHT);

	ZeroMemory(m_pxTileImg, sizeof(CWHWilImageData*)*_MAX_TILE_FILE);
}



VOID CMapHandler::LoadMapHandler(CHAR* szMapFile, CImageHandler* pxImgHandler, INT nX, INT nY)
{
	InitMapHandler();
	LoadMapData(szMapFile);
	LoadMapImage(pxImgHandler);
	LoadNewMapBuffer();
	SetStartViewTile(nX, nY);
}


/******************************************************************************************************************

	함수명 : CMapHandler::DestroyMapHandler()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CMapHandler::DestroyMapHandler()
{
//	INT nCnt;

	SAFE_DELETE(m_pstTileInfo);
	SAFE_DELETE(m_pstCellInfo);
/*	for ( nCnt = 0; nCnt < m_bDoorCount; nCnt++ )
	{
		SAFE_DELETE(m_pstDoorInfo[nCnt].pstDoorImgInfo);
	}
	SAFE_DELETE(m_pstDoorInfo);
	SAFE_DELETE(m_pbCellIDoorIdx);*/

	InitMapHandler();
}



/******************************************************************************************************************

	함수명 : CMapHandler::LoadNewMapBuffer()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CMapHandler::LoadNewMapBuffer()
{
	INT nFileIdx, nImgIdx;
	INT nXCnt, nYCnt;

	ZeroMemory(m_wSavedMapBuffer, sizeof(WORD)*_VIEW_CELL_X_COUNT*_CELL_WIDTH*_VIEW_CELL_Y_COUNT*_CELL_HEIGHT);

	for ( nXCnt = m_shStartViewTileX; nXCnt < m_shStartViewTileX+_VIEW_CELL_X_COUNT; nXCnt++ )
		for ( nYCnt = m_shStartViewTileY; nYCnt < m_shStartViewTileY+_VIEW_CELL_Y_COUNT; nYCnt++ )
	{
		if ( nXCnt >= m_stMapFileHeader.shWidth || nYCnt >= m_stMapFileHeader.shHeight || nXCnt < 0 || nYCnt < 0 )
			continue; 

		if( !( nXCnt % 2 ) && !( nYCnt % 2 ) && nYCnt < m_shStartViewTileY+_VIEW_CELL_Y_COUNT )
 		{
 			nFileIdx= m_pstTileInfo[(nYCnt/2) + (nXCnt/2)*m_stMapFileHeader.shHeight/2].bFileIdx;
			nImgIdx = m_pstTileInfo[(nYCnt/2) + (nXCnt/2)*m_stMapFileHeader.shHeight/2].wTileIdx;

 			if( nFileIdx != 255 && nImgIdx != 65535 )
			{
				m_pxTileImg[nFileIdx]->NewSetIndex(nImgIdx);
				g_xMainWnd.DrawWithImageForCompMemToMem(
								(nXCnt-m_shStartViewTileX)*_CELL_WIDTH ,
								(nYCnt-m_shStartViewTileY)*_CELL_HEIGHT,
								m_pxTileImg[nFileIdx]->m_lpstNewCurrWilImageInfo->shWidth,
								m_pxTileImg[nFileIdx]->m_lpstNewCurrWilImageInfo->shHeight,
								(WORD*)m_pxTileImg[nFileIdx]->m_pbCurrImage,
								_VIEW_CELL_X_COUNT*_CELL_WIDTH, _VIEW_CELL_Y_COUNT*_CELL_HEIGHT, m_wSavedMapBuffer);
				
			}
		}
	}
}



/******************************************************************************************************************

	함수명 : CMapHandler::LoadMapData()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : CHAR* szMapFile
	출력   : BOOL 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
BOOL CMapHandler::LoadMapData(CHAR* szMapFile)
{
	HANDLE	hFile;
	char	szFullPath[128];

	sprintf(szFullPath, ".\\Map\\%s.map", szMapFile);

	hFile = CreateFile(szFullPath, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

	if ( hFile != INVALID_HANDLE_VALUE )
	{
		DWORD		dwReadLen;

		FreeMapData();

		ReadFile(hFile, &m_stMapFileHeader, sizeof(MAPFILEHEADER), &dwReadLen, NULL);

		m_pstTileInfo = new TILEINFO[(m_stMapFileHeader.shWidth*m_stMapFileHeader.shHeight)/4];
		ReadFile(hFile, m_pstTileInfo, sizeof(TILEINFO)*(m_stMapFileHeader.shWidth*m_stMapFileHeader.shHeight)/4, &dwReadLen, NULL);
		m_pstCellInfo = new CELLINFO[m_stMapFileHeader.shWidth*m_stMapFileHeader.shHeight];
		ReadFile(hFile, m_pstCellInfo, sizeof(CELLINFO)*(m_stMapFileHeader.shWidth*m_stMapFileHeader.shHeight), &dwReadLen, NULL);

		LoadNewMapBuffer();

//		ReadFile(hFile, &m_bDoorCount, sizeof(BYTE), &dwReadLen, NULL);

/*		m_pstDoorInfo = new DOORINFO[m_bDoorCount];

		for ( INT nCnt = 0; nCnt < m_bDoorCount; nCnt++ )
		{
			ReadFile(hFile, &m_pstDoorInfo[nCnt], sizeof(DOORINFO), &dwReadLen, NULL);
			m_pstDoorInfo[nCnt].pstDoorImgInfo = new DOORIMAGEINFO[m_pstDoorInfo[nCnt].bDoorImgCnt];
			
			for ( INT nLoop = 0; nLoop < m_pstDoorInfo[nCnt].bDoorImgCnt; nLoop++ )
			{
				ReadFile(hFile, &m_pstDoorInfo[nCnt].pstDoorImgInfo[nLoop], sizeof(DOORIMAGEINFO), &dwReadLen, NULL);
			}
		}

		m_pbCellIDoorIdx = new BYTE[m_stMapFileHeader.shWidth*m_stMapFileHeader.shHeight];
		ZeroMemory(m_pbCellIDoorIdx, sizeof(BYTE)*m_stMapFileHeader.shWidth*m_stMapFileHeader.shHeight);
		SetDoorIndex();
*/
		CloseHandle(hFile);
		return TRUE;
	}

	return FALSE;
}



/******************************************************************************************************************

	함수명 : CMapHandler::LoadMapImage(CImageHandler* pxImgHandler)

	작성자 : 
	작성일 : 

	목적   : 
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CMapHandler::LoadMapImage(CImageHandler* pxImgHandler)
{
	m_pxTileImg[0]  = &(pxImgHandler->m_xImageList[_IMAGE_TILES]);
	m_pxTileImg[1]  = &(pxImgHandler->m_xImageList[_IMAGE_TILES30]);
	m_pxTileImg[2]  = &(pxImgHandler->m_xImageList[_IMAGE_TILES5]);
	m_pxTileImg[3]  = &(pxImgHandler->m_xImageList[_IMAGE_SMTILES]);
	m_pxTileImg[4]  = &(pxImgHandler->m_xImageList[_IMAGE_HOUSES]);
	m_pxTileImg[5]  = &(pxImgHandler->m_xImageList[_IMAGE_CLIFFS]);
	m_pxTileImg[6]  = &(pxImgHandler->m_xImageList[_IMAGE_DUNGEONS]);
	m_pxTileImg[7]  = &(pxImgHandler->m_xImageList[_IMAGE_INNERS]);
	m_pxTileImg[8]  = &(pxImgHandler->m_xImageList[_IMAGE_FUNITURES]);
	m_pxTileImg[9]  = &(pxImgHandler->m_xImageList[_IMAGE_WALLS]);
	m_pxTileImg[10] = &(pxImgHandler->m_xImageList[_IMAGE_SMOBJECTS]);
	m_pxTileImg[11] = &(pxImgHandler->m_xImageList[_IMAGE_ANIMATIONS]);
	m_pxTileImg[12] = &(pxImgHandler->m_xImageList[_IMAGE_OBJECT1]);
	m_pxTileImg[13] = &(pxImgHandler->m_xImageList[_IMAGE_OBJECT2]);
}



/******************************************************************************************************************

	함수명 : CMapHandler::FreeMapData()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CMapHandler::FreeMapData()
{
	ZeroMemory(&m_stMapFileHeader, sizeof(MAPFILEHEADER));
	ZeroMemory(m_wSavedMapBuffer, sizeof(WORD)*_VIEW_CELL_X_COUNT*_CELL_WIDTH*_VIEW_CELL_Y_COUNT*_CELL_HEIGHT);
	SAFE_DELETE(m_pstTileInfo);
	SAFE_DELETE(m_pstCellInfo);
/*	for ( INT nCnt = 0; nCnt < m_bDoorCount; nCnt++ )
	{
		SAFE_DELETE(m_pstDoorInfo[nCnt].pstDoorImgInfo);
	}
	SAFE_DELETE(m_pstDoorInfo);
	SAFE_DELETE(m_pbCellIDoorIdx);
	m_bDoorCount = 0;*/
}



/******************************************************************************************************************

	함수명 : CMapHandler::ScrollMap()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : INT nCharDir
	         INT nCharFrame
	         INT nSpeed
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CMapHandler::ScrollMap(INT nCharDir, INT nCharFrame, INT nSpeed)
{
	nCharFrame %= 10;

	switch ( nCharDir )
	{
	// 직선 위로 이동.
	case _DIRECTION_LIST_1:	
		m_shViewOffsetY = -(m_ptMovePixelArray[nCharFrame].y * nSpeed);		
		break;
	// 오른쪽위로 이동.
	case _DIRECTION_LIST_2:
		m_shViewOffsetX =  (m_ptMovePixelArray[nCharFrame].x * nSpeed);
		m_shViewOffsetY = -(m_ptMovePixelArray[nCharFrame].y * nSpeed);
		break;
	// 오른쪽으로 이동.
	case _DIRECTION_LIST_3:
		m_shViewOffsetX =  (m_ptMovePixelArray[nCharFrame].x * nSpeed);
		break;
	// 오른쪽아래로 이동.
	case _DIRECTION_LIST_4:
		m_shViewOffsetX =  (m_ptMovePixelArray[nCharFrame].x * nSpeed);
		m_shViewOffsetY =  (m_ptMovePixelArray[nCharFrame].y * nSpeed);
		break;
	// 직선 아래로 이동.
	case _DIRECTION_LIST_5:
		m_shViewOffsetY =  (m_ptMovePixelArray[nCharFrame].y * nSpeed);
		break;
	// 왼쪽아래로 이동.
	case _DIRECTION_LIST_6:
		m_shViewOffsetX = -(m_ptMovePixelArray[nCharFrame].x * nSpeed);
		m_shViewOffsetY =  (m_ptMovePixelArray[nCharFrame].y * nSpeed);
		break;
	// 왼쪽으로 이동.
	case _DIRECTION_LIST_7:
		m_shViewOffsetX = -(m_ptMovePixelArray[nCharFrame].x * nSpeed);
		break;
	// 왼쪽위로 이동.
	case _DIRECTION_LIST_8:
		m_shViewOffsetX = -(m_ptMovePixelArray[nCharFrame].x * nSpeed);
		m_shViewOffsetY = -(m_ptMovePixelArray[nCharFrame].y * nSpeed);
		break;
	}
}



VOID CMapHandler::ScrollMap(INT nCharDir, WORD wCurrDelay, WORD wMoveDelay, INT nSpeed)
{
	FLOAT fViewOffSetX = (FLOAT)((FLOAT)((wCurrDelay+1)/wMoveDelay) * _CELL_WIDTH ) * nSpeed;
	FLOAT fViewOffSetY = (FLOAT)((FLOAT)((wCurrDelay+1)/wMoveDelay) * _CELL_HEIGHT) * nSpeed;

	switch ( nCharDir )
	{
	// 직선 위로 이동.
	case _DIRECTION_LIST_1:	
		m_shViewOffsetY -= (SHORT)-fViewOffSetY;
		break;
	// 오른쪽위로 이동.
	case _DIRECTION_LIST_2:
		m_shViewOffsetX = (SHORT)fViewOffSetX;
		m_shViewOffsetY = (SHORT)-fViewOffSetY;
		break;
	// 오른쪽으로 이동.
	case _DIRECTION_LIST_3:
		m_shViewOffsetX = (SHORT)fViewOffSetX;
		break;
	// 오른쪽아래로 이동.
	case _DIRECTION_LIST_4:
		m_shViewOffsetX = (SHORT)fViewOffSetX;
		m_shViewOffsetY = (SHORT)fViewOffSetY;
		break;
	// 직선 아래로 이동.
	case _DIRECTION_LIST_5:
		m_shViewOffsetY = (SHORT)fViewOffSetY;
		break;
	// 왼쪽아래로 이동.
	case _DIRECTION_LIST_6:
		m_shViewOffsetX = (SHORT)-fViewOffSetX;
		m_shViewOffsetY = (SHORT)fViewOffSetY;
		break;
	// 왼쪽으로 이동.
	case _DIRECTION_LIST_7:
		m_shViewOffsetX = (SHORT)-fViewOffSetX;
		break;
	// 왼쪽위로 이동.
	case _DIRECTION_LIST_8:
		m_shViewOffsetX = (SHORT)fViewOffSetX;
		m_shViewOffsetY = (SHORT)-fViewOffSetY;
		break;
	}
}



/******************************************************************************************************************

	함수명 : CMapHandler::DrawBaseTile()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : INT nX
	         INT nY
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CMapHandler::DrawBaseTile(INT nX, INT nY)
{
	INT nFileIdx;
	INT nImgIdx;
	if ( nX >= m_stMapFileHeader.shWidth || nY >= m_stMapFileHeader.shHeight || nX < 0 || nY < 0 )
		return; 
 	if( !( nX % 2 ) && !( nY % 2 ) && nY < m_shStartViewTileY+_VIEW_CELL_Y_COUNT )
 	{
		nFileIdx= m_pstTileInfo[(nY/2) + (nX/2)*m_stMapFileHeader.shHeight/2].bFileIdx;
		nImgIdx = m_pstTileInfo[(nY/2) + (nX/2)*m_stMapFileHeader.shHeight/2].wTileIdx;

		if( nFileIdx != 255 && nImgIdx != 65535 )
		{
			m_pxTileImg[nFileIdx]->NewSetIndex(nImgIdx);
			g_xMainWnd.DrawWithImageForCompMemToMem(
							(nX-m_shStartViewTileX)*_CELL_WIDTH ,
							(nY-m_shStartViewTileY)*_CELL_HEIGHT,
							m_pxTileImg[nFileIdx]->m_lpstNewCurrWilImageInfo->shWidth,
							m_pxTileImg[nFileIdx]->m_lpstNewCurrWilImageInfo->shHeight,
							(WORD*)m_pxTileImg[nFileIdx]->m_pbCurrImage,
							_VIEW_CELL_X_COUNT*_CELL_WIDTH, _VIEW_CELL_Y_COUNT*_CELL_HEIGHT, m_wSavedMapBuffer);
			
		}
	}
}



/******************************************************************************************************************

	함수명 : CMapHandler::DrawOpenDoor()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : INT nX
	         INT nY
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
/*VOID CMapHandler::DrawOpenDoor(INT nX, INT nY)
{
	if ( GetDoorState(nX, nY) )
	{
		BYTE bDoorIndex = m_pbCellIDoorIdx[nX + nY*m_stMapFileHeader.shWidth];
		for ( INT nCnt = 0; nCnt < m_bDoorCount; nCnt++ )
		{
			if ( m_pstDoorInfo[nCnt].bDoorIdx == bDoorIndex )
			{
				for ( INT nLoop = 0; nLoop < m_pstDoorInfo[nCnt].bDoorImgCnt; nLoop++ )
				{
					WORD wPosX = m_pstDoorInfo[nCnt].pstDoorImgInfo[nLoop].wPosX;
					WORD wPosY = m_pstDoorInfo[nCnt].pstDoorImgInfo[nLoop].wPosY;
					if ( nX == wPosX && nY == wPosY )
					{
						WORD wImgIndex = m_pstDoorInfo[nCnt].pstDoorImgInfo[nLoop].wImageNum;						
						// 그림을 그린다.

						// ".\\Data\\housesc.wil"
						m_pxTileImg[4].NewSetIndex(wImgIndex);

						g_xMainWnd.DrawWithImageForCompClipRgn(
						(nX-m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START-m_shViewOffsetX,
						(nY-m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START-m_pxTileImg[4].m_lpstNewCurrWilImageInfo->shHeight+_CELL_HEIGHT-m_shViewOffsetY,
						m_pxTileImg[4].m_lpstNewCurrWilImageInfo->shWidth,
						m_pxTileImg[4].m_lpstNewCurrWilImageInfo->shHeight,
						(WORD*)m_pxTileImg[4].m_pbCurrImage,
						_CLIP_WIDTH, _CLIP_HEIGHT);
						break;
					}
				}
			}
		}
	}
}


VOID CMapHandler::SetDoorIndex()
{
	for ( INT nCnt = 0; nCnt < m_bDoorCount; nCnt++ )
	{
		for ( INT nLoop = 0; nLoop < m_pstDoorInfo[nCnt].bDoorImgCnt; nLoop++ )
		{
			WORD wPosX = m_pstDoorInfo[nCnt].pstDoorImgInfo[nLoop].wPosX;
			WORD wPosY = m_pstDoorInfo[nCnt].pstDoorImgInfo[nLoop].wPosY;
			m_pbCellIDoorIdx[wPosX + wPosY*m_stMapFileHeader.shWidth] = m_pstDoorInfo[nCnt].bDoorIdx;
		}
	}
}


BOOL CMapHandler::GetDoorState(INT nX, INT nY)
{
	if ( (nX == 432 || nX == 433) && (nY == 70 || nY == 71 ||nY == 72 ) )
		int a = 0;
	if ( (m_pstCellInfo[nX + nY*m_stMapFileHeader.shWidth].wLigntNEvent == 65535) ||
		 !(m_pstCellInfo[nX + nY*m_stMapFileHeader.shWidth].wLigntNEvent & 0X08) )
		return FALSE;

	// 문이열려있다.
	return TRUE;
}


VOID CMapHandler::SetDoorState(BYTE bIndex, BOOL bDoorOpen)
{
	for ( INT nCnt = 0; nCnt < m_bDoorCount; nCnt++ )
	{
		if ( m_pstDoorInfo[nCnt].bDoorIdx == bIndex )
		{
			for ( INT nLoop = 0; nLoop < m_pstDoorInfo[nCnt].bDoorImgCnt; nLoop++ )
			{
				WORD wPosX = m_pstDoorInfo[nCnt].pstDoorImgInfo[nLoop].wPosX;
				WORD wPosY = m_pstDoorInfo[nCnt].pstDoorImgInfo[nLoop].wPosY;
				
				if ( bDoorOpen )
					m_pstCellInfo[wPosX + wPosY*m_stMapFileHeader.shWidth].wLigntNEvent = m_pstCellInfo[wPosX + wPosY*m_stMapFileHeader.shWidth].wLigntNEvent | 0X08;
				else
					m_pstCellInfo[wPosX + wPosY*m_stMapFileHeader.shWidth].wLigntNEvent = m_pstCellInfo[wPosX + wPosY*m_stMapFileHeader.shWidth].wLigntNEvent & 0XFFF7;
			}
		}
	}
}
*/



INT	CMapHandler::GetDoor(INT nX, INT nY)
{
	INT nResult = 0;

 	if ( (m_pstCellInfo[nY + nX*m_stMapFileHeader.shWidth].bDoorIdx & 0X80) > 0 )
	{
		nResult = m_pstCellInfo[nY + nX*m_stMapFileHeader.shWidth].bDoorIdx & 0X7F;
	}

	return nResult;
}


BOOL CMapHandler::IsDoorOpen(INT nX, INT nY)
{
	if ( (m_pstCellInfo[nY + nX*m_stMapFileHeader.shWidth].bDoorIdx & 0X80) > 0 )
	{
		if ( (m_pstCellInfo[nY + nX*m_stMapFileHeader.shWidth].bDoorOffset & 0X80) == 0 )
		{
			return FALSE;
		}
	}
	return TRUE;
}


VOID CMapHandler::OpenDoor(INT nX, INT nY, INT nIdx)
{
	for ( INT nCntY = nY-8; nCntY < nY+10; nCntY++ )
	{
		for ( INT nCntX = nX-8; nCntX < nX+10; nCntX++ )
		{
			if ( nCntX >= 0 && nCntY >= 0 && nCntX < m_stMapFileHeader.shWidth && nCntY < m_stMapFileHeader.shHeight )
			{
				if ( (m_pstCellInfo[nCntY + nCntX*m_stMapFileHeader.shWidth].bDoorIdx & 0X7F) == nIdx )
				{
					m_pstCellInfo[nCntY + nCntX*m_stMapFileHeader.shWidth].bDoorOffset |= 0X80;
				}
			}
		}
	}
}


VOID CMapHandler::CloseDoor(INT nX, INT nY, INT nIdx)
{
	for ( INT nCntY = nY-8; nCntY < nY+10; nCntY++ )
	{
		for ( INT nCntX = nX-8; nCntX < nX+10; nCntX++ )
		{
			if ( nCntX >= 0 && nCntY >= 0 && nCntX < m_stMapFileHeader.shWidth && nCntY < m_stMapFileHeader.shHeight )
			{
				if ( (m_pstCellInfo[nCntY + nCntX*m_stMapFileHeader.shWidth].bDoorIdx & 0X7F) == nIdx )
				{
					m_pstCellInfo[nCntY + nCntX*m_stMapFileHeader.shWidth].bDoorOffset &= 0X7F;
				}
			}
		}
	}
}


INT CMapHandler::GetDoorImgIdx(INT nX, INT nY)
{
	INT nDoorIdx = 0;
 	if ( (m_pstCellInfo[nY + nX*m_stMapFileHeader.shWidth].bDoorOffset & 0X80) > 0 )
	{
	 	if ( (m_pstCellInfo[nY + nX*m_stMapFileHeader.shWidth].bDoorIdx & 0X7F) > 0 )
			nDoorIdx += m_pstCellInfo[nY + nX*m_stMapFileHeader.shWidth].bDoorOffset & 0X7F;
	}

	return nDoorIdx;
}



/******************************************************************************************************************

	함수명 : CMapHandler::SetMovedTileBuffer()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : SHORT shMovedTileX
	         SHORT shMovedTileY
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CMapHandler::SetMovedTileBuffer(SHORT shMovedTileX, SHORT shMovedTileY)
{
	m_shStartViewTileX += shMovedTileX;
	m_shStartViewTileY += shMovedTileY;
	m_shViewOffsetX = 0;
	m_shViewOffsetY = 0;

	INT nX, nY;

	// 현재 타일버퍼에 있는 메모리를 알맞게 이동시키고 추가되는그림을 그린다.
	// 셀단위가 아닌 타일단위로 업데이트한다.

	// 현재 Y타일이거나, 아래로 갈때.
	if ( shMovedTileY >= 0 )
	{
		if ( shMovedTileX >= 0 )
		{
			// 오른쪽으로 갈때.
			for ( INT nYCnt = 0; nYCnt < _VIEW_CELL_Y_COUNT*_CELL_HEIGHT-_CELL_HEIGHT*shMovedTileY; nYCnt++ )
			{
				memmove(&m_wSavedMapBuffer[(nYCnt*_VIEW_CELL_X_COUNT*_CELL_WIDTH)], 
						&m_wSavedMapBuffer[((nYCnt+_CELL_HEIGHT*shMovedTileY)*_VIEW_CELL_X_COUNT*_CELL_WIDTH)+_CELL_WIDTH*shMovedTileX], 
						sizeof(WORD)*_CELL_WIDTH*(_VIEW_CELL_X_COUNT-shMovedTileX));
			}

			// 그림상으로 오른쪽과 아래부분의 타일을 갱신해줘야한다.
			// 기본타일은 2셀로 이뤄져있고 (오른쪽과 아래는 1셀만의 여유)를 가지기 때문에 1셀씩을 더 읽어줘서 갱신을 시킨다.
			// 아래와 같은 형태이다. '1'은 memmove로 옮겨진 데이타이고 'o'가 새로 갱신시킬 타일그림데이타이다. '0'은 더읽어줄 데이타이다.
			// 11111111111111...0o
			// 11111111111111...0o
			// 11111111111111...0o
			// .................0o
			// .................0o
			// 00000000000000...0o
			// oooooooooooooo...oo
			
			// 기본타일 갱신.
			for ( nX = m_shStartViewTileX+_VIEW_CELL_X_COUNT-shMovedTileX-1; nX < m_shStartViewTileX+_VIEW_CELL_X_COUNT; nX++ )
				for ( nY = m_shStartViewTileY; nY < m_shStartViewTileY+_VIEW_CELL_Y_COUNT; nY++ )		
				{
					DrawBaseTile(nX, nY);
				}
			for ( nX = m_shStartViewTileX; nX < m_shStartViewTileX+_VIEW_CELL_X_COUNT; nX++ )		
				for ( nY = m_shStartViewTileY+_VIEW_CELL_Y_COUNT-shMovedTileY-1; nY < m_shStartViewTileY+_VIEW_CELL_Y_COUNT; nY++ )
				{
					DrawBaseTile(nX, nY);
				}
		}
		else
		{
			// 왼쪽으로 갈때.
			// 왼쪽과 아래부분의 타일을 갱신.
			// o11111111111111...1
			// o11111111111111...1
			// o11111111111111...1
			// o1................1
			// o1................1
			// o00000000000000...0
			// ooooooooooooooo...o

			// 기본타일 갱신.
			// 좌측은 2타일의 여유를 가지므로 1셀씩만 읽고, 아래로는 한타일만의 여유를 가지므로 아래로는 1셀씩을 더 읽어준다.
			for ( INT nYCnt = 0; nYCnt < _VIEW_CELL_Y_COUNT*_CELL_HEIGHT-_CELL_HEIGHT*shMovedTileY; nYCnt++ )
			{
				memmove(&m_wSavedMapBuffer[(nYCnt*_VIEW_CELL_X_COUNT*_CELL_WIDTH)+_CELL_WIDTH*(-shMovedTileX)], 
						&m_wSavedMapBuffer[((nYCnt+_CELL_HEIGHT*shMovedTileY)*_VIEW_CELL_X_COUNT*_CELL_WIDTH)], sizeof(WORD)*_CELL_WIDTH*(_VIEW_CELL_X_COUNT-(-shMovedTileX)));
			}

			for ( nX = m_shStartViewTileX; nX < m_shStartViewTileX+(-shMovedTileX); nX++ )
				for ( nY = m_shStartViewTileY; nY < m_shStartViewTileY+_VIEW_CELL_Y_COUNT; nY++ )		
				{
					DrawBaseTile(nX, nY);
				}
			for ( nX = m_shStartViewTileX; nX < m_shStartViewTileX+_VIEW_CELL_X_COUNT; nX++ )		
				for ( nY = m_shStartViewTileY+_VIEW_CELL_Y_COUNT-shMovedTileY-1; nY < m_shStartViewTileY+_VIEW_CELL_Y_COUNT; nY++ )
				{
					DrawBaseTile(nX, nY);
				}
		}
	}
	// 위로 갈때.
	else
	{
		if ( shMovedTileX >= 0 )
		{
			// 오른쪽으로 갈때.

			// ooooooooooooo...0o
			// 1111111111111...0o
			// ................0o
			// ................0o
			// 1111111111111...0o
			// 1111111111111...0o
			// 1111111111111...0o

			for ( INT nYCnt = (_VIEW_CELL_Y_COUNT*_CELL_HEIGHT)+(_CELL_HEIGHT*shMovedTileY)-1; nYCnt >= 0 ; nYCnt-- )
			{
				memmove(&m_wSavedMapBuffer[((nYCnt+_CELL_HEIGHT*(-shMovedTileY))*_VIEW_CELL_X_COUNT*_CELL_WIDTH)], 
						&m_wSavedMapBuffer[(nYCnt*_VIEW_CELL_X_COUNT*_CELL_WIDTH)+_CELL_WIDTH*shMovedTileX], sizeof(WORD)*_CELL_WIDTH*(_VIEW_CELL_X_COUNT-shMovedTileX));
			}

			for ( nX = m_shStartViewTileX+_VIEW_CELL_X_COUNT-shMovedTileX-1; nX < m_shStartViewTileX+_VIEW_CELL_X_COUNT; nX++ )
				for ( nY = m_shStartViewTileY; nY < m_shStartViewTileY+_VIEW_CELL_Y_COUNT; nY++ )		
				{
					DrawBaseTile(nX, nY);
				}
			for ( nX = m_shStartViewTileX; nX < m_shStartViewTileX+_VIEW_CELL_X_COUNT; nX++ )		
				for ( nY = m_shStartViewTileY; nY < m_shStartViewTileY+(-shMovedTileY); nY++ )
				{
					DrawBaseTile(nX, nY);
				}
		}
		else
		{
			for ( INT nYCnt = (_VIEW_CELL_Y_COUNT*_CELL_HEIGHT)+(_CELL_HEIGHT*shMovedTileY)-1; nYCnt >= 0 ; nYCnt-- )
			{
				// 왼쪽으로 갈때.
				memmove(&m_wSavedMapBuffer[((nYCnt+_CELL_HEIGHT*(-shMovedTileY))*_VIEW_CELL_X_COUNT*_CELL_WIDTH)+_CELL_WIDTH*(-shMovedTileX)], 
 						&m_wSavedMapBuffer[(nYCnt*_VIEW_CELL_X_COUNT*_CELL_WIDTH)], sizeof(WORD)*_CELL_WIDTH*(_VIEW_CELL_X_COUNT-(-shMovedTileX)));
			}

			// oooooooooooooo...o
			// o1111111111111...1
			// o1...............1
			// o1...............1
			// o1111111111111...1
			// o1111111111111...1
			// o1111111111111...1
			for ( nX = m_shStartViewTileX; nX < m_shStartViewTileX+(-shMovedTileX); nX++ )
				for ( nY = m_shStartViewTileY; nY < m_shStartViewTileY+_VIEW_CELL_Y_COUNT; nY++ )		
				{
					DrawBaseTile(nX, nY);
				}
			for ( nX = m_shStartViewTileX; nX < m_shStartViewTileX+_VIEW_CELL_X_COUNT; nX++ )		
				for ( nY = m_shStartViewTileY; nY < m_shStartViewTileY+(-shMovedTileY); nY++ )
				{
					DrawBaseTile(nX, nY);
				}
		}
	}
}


VOID CMapHandler::SetStartViewTile(INT nX, INT nY)
{
	m_shStartViewTileX	= nX;
	m_shStartViewTileY	= nY;
	m_shViewOffsetX		= 0;	
	m_shViewOffsetY		= 0;
}



BYTE CMapHandler::GetTileAttribute(INT nX, INT nY)
{
	if ( m_pstCellInfo )
	{
		BYTE bAttr = _CAN_WALK;

		if ( m_pstCellInfo[nY + nX*m_stMapFileHeader.shHeight].bFlag & 0X01 )
			return bAttr;
	}

	return _CAN_NOTWALK;
}



LIGHTINFO CMapHandler::GetTileLightInfo(INT nX, INT nY)
{
	LIGHTINFO stLight = {FALSE, -1, -1};
	
	if ( (m_pstCellInfo[nY + nX*m_stMapFileHeader.shHeight].wLigntNEvent == 0) && 
		 ((m_pstCellInfo[nY + nX*m_stMapFileHeader.shHeight].wLigntNEvent & 0X07) != 1) )
		return stLight;

	stLight.bIsLight		= TRUE;
	stLight.cLightSizeType	= ((m_pstCellInfo[nY + nX*m_stMapFileHeader.shHeight].wLigntNEvent & 0XC000) >> 14);
	stLight.cLightColorType	= ((m_pstCellInfo[nY + nX*m_stMapFileHeader.shHeight].wLigntNEvent & 0X3FF0) >> 4);

	return stLight;	
}


BYTE CMapHandler::GetEventNum(INT nX, INT nY)
{
	BYTE bEventNum = -1;
	
	if ( (m_pstCellInfo[nY + nX*m_stMapFileHeader.shHeight].wLigntNEvent == -1) && 
		 ((m_pstCellInfo[nY + nX*m_stMapFileHeader.shHeight].wLigntNEvent & 0X07) != 3) )
		return bEventNum;

	bEventNum	= ((m_pstCellInfo[nY + nX*m_stMapFileHeader.shHeight].wLigntNEvent & 0XF0) >> 4);

	return bEventNum;	
}


BOOL CMapHandler::GetNextTileCanMove(INT nXPos, INT nYPos, BYTE bDir, INT nGap, POINT* lpptTarget)
{
	INT  nXGapPos, nYGapPos;
	BYTE bCanMove = _CAN_WALK;

	nXGapPos = nYGapPos = 0;

	switch ( bDir )
	{
	case _DIRECTION_LIST_1:
		{
			nXGapPos = nXPos;
			nYGapPos = nYPos-nGap;
			bCanMove  = GetTileAttribute(nXGapPos, nYGapPos);
		}
		break;
	case _DIRECTION_LIST_2:
		{
			nXGapPos = nXPos+nGap;
			nYGapPos = nYPos-nGap;
			bCanMove  = GetTileAttribute(nXGapPos, nYGapPos);
		}
		break;
	case _DIRECTION_LIST_3:
		{
			nXGapPos = nXPos+nGap;
			nYGapPos = nYPos;
			bCanMove  = GetTileAttribute(nXGapPos, nYGapPos);
		}
		break;
	case _DIRECTION_LIST_4:
		{
			nXGapPos = nXPos+nGap;
			nYGapPos = nYPos+nGap;
			bCanMove  = GetTileAttribute(nXGapPos, nYGapPos);
		}
		break;
	case _DIRECTION_LIST_5:
		{
			nXGapPos = nXPos;
			nYGapPos = nYPos+nGap;
			bCanMove  = GetTileAttribute(nXGapPos, nYGapPos);
		}
		break;
	case _DIRECTION_LIST_6:
		{
			nXGapPos = nXPos-nGap;
			nYGapPos = nYPos+nGap;
			bCanMove  = GetTileAttribute(nXGapPos, nYGapPos);
		}
		break;
	case _DIRECTION_LIST_7:
		{
			nXGapPos = nXPos-nGap;
			nYGapPos = nYPos;
			bCanMove  = GetTileAttribute(nXGapPos, nYGapPos);
		}
		break;
	case _DIRECTION_LIST_8:
		{
			nXGapPos = nXPos-nGap;
			nYGapPos = nYPos-nGap;
			bCanMove  = GetTileAttribute(nXGapPos, nYGapPos);
		}
		break;
	}

	lpptTarget->x = nXGapPos;
	lpptTarget->y = nYGapPos;

	if ( bCanMove != _CAN_WALK )
		return FALSE;

	if ( g_xGameProc.m_xActorList.GetCounter() != 0 )
	{
		g_xGameProc.m_xActorList.MoveCurrentToTop();

		CActor* pxActor;

		for ( INT nCnt = 0; nCnt < g_xGameProc.m_xActorList.GetCounter(); nCnt++ )
		{
			pxActor = g_xGameProc.m_xActorList.GetCurrentData();
			if ( pxActor->m_wPosX == nXGapPos &&  pxActor->m_wPosY == nYGapPos && !pxActor->m_bIsDead )	
			{
				return FALSE;
			}
			g_xGameProc.m_xActorList.MoveNextNode();
		}
	} 

	return TRUE;
}



VOID CMapHandler::SetAniTileFrame(INT nLoopTime)
{
	INT nCnt;

	for ( nCnt = 0; nCnt < 8; nCnt++ )
	{
		m_dwAniSaveTime[nCnt] += nLoopTime;
	}

	if ( m_dwAniSaveTime[0] > _TILE_ANI_DELAY_1 )
	{
		for ( nCnt = 0; nCnt < 16; nCnt++ )
		{
			m_bAniTileFrame[0][nCnt]++;

			if ( m_bAniTileFrame[0][nCnt] >= nCnt )
				m_bAniTileFrame[0][nCnt] = 0;
		}
		m_dwAniSaveTime[0] = 0;
	}

	if ( m_dwAniSaveTime[1] > _TILE_ANI_DELAY_2 )
	{
		for ( nCnt = 0; nCnt < 16; nCnt++ )
		{
			m_bAniTileFrame[1][nCnt]++;

			if ( m_bAniTileFrame[1][nCnt] >= nCnt )
				m_bAniTileFrame[1][nCnt] = 0;
		}
		m_dwAniSaveTime[1] = 0;
	}

	if ( m_dwAniSaveTime[2] > _TILE_ANI_DELAY_3 )
	{
		for ( nCnt = 0; nCnt < 16; nCnt++ )
		{
			m_bAniTileFrame[2][nCnt]++;

			if ( m_bAniTileFrame[2][nCnt] >= nCnt )
				m_bAniTileFrame[2][nCnt] = 0;
		}
		m_dwAniSaveTime[2] = 0;
	}
	
	if ( m_dwAniSaveTime[3] > _TILE_ANI_DELAY_4 )
	{
		for ( nCnt = 0; nCnt < 16; nCnt++ )
		{
			m_bAniTileFrame[3][nCnt]++;

			if ( m_bAniTileFrame[3][nCnt] >= nCnt )
				m_bAniTileFrame[3][nCnt] = 0;
		}
		m_dwAniSaveTime[3] = 0;
	}

	if ( m_dwAniSaveTime[4] > _TILE_ANI_DELAY_5 )
	{
		for ( nCnt = 0; nCnt < 16; nCnt++ )
		{
			m_bAniTileFrame[4][nCnt]++;

			if ( m_bAniTileFrame[4][nCnt] >= nCnt )
				m_bAniTileFrame[4][nCnt] = 0;
		}
		m_dwAniSaveTime[4] = 0;
	}

	if ( m_dwAniSaveTime[5] > _TILE_ANI_DELAY_6 )
	{
		for ( nCnt = 0; nCnt < 16; nCnt++ )
		{
			m_bAniTileFrame[5][nCnt]++;

			if ( m_bAniTileFrame[5][nCnt] >= nCnt )
				m_bAniTileFrame[5][nCnt] = 0;
		}
		m_dwAniSaveTime[5] = 0;
	}

	if ( m_dwAniSaveTime[6] > _TILE_ANI_DELAY_7 )
	{
		for ( nCnt = 0; nCnt < 16; nCnt++ )
		{
			m_bAniTileFrame[6][nCnt]++;

			if ( m_bAniTileFrame[6][nCnt] >= nCnt )
				m_bAniTileFrame[6][nCnt] = 0;
		}
		m_dwAniSaveTime[6] = 0;
	}

	if ( m_dwAniSaveTime[7] > _TILE_ANI_DELAY_8 )
	{
		for ( nCnt = 0; nCnt < 16; nCnt++ )
		{
			m_bAniTileFrame[7][nCnt]++;

			if ( m_bAniTileFrame[7][nCnt] >= nCnt )
				m_bAniTileFrame[7][nCnt] = 0;
		}
		m_dwAniSaveTime[7] = 0;
	}
}


VOID CMapHandler::GetScrnPosFromTilePos(SHORT shTileX, SHORT shTileY, SHORT& shScrnX, SHORT& shScrnY)
{
	shScrnX = (shTileX - m_shStartViewTileX)*_CELL_WIDTH  - m_shViewOffsetX + _VIEW_CELL_X_START;
	shScrnY = (shTileY - m_shStartViewTileY)*_CELL_HEIGHT - m_shViewOffsetY + _VIEW_CELL_Y_START;
}


VOID CMapHandler::GetTilePosFromScrnPos(SHORT shScrnX, SHORT shScrnY, SHORT& shTileX, SHORT& shTileY)
{
	shTileX = m_shStartViewTileX + (shScrnX - _VIEW_CELL_X_START) / _CELL_WIDTH;
	shTileY = m_shStartViewTileY + (shScrnY - _VIEW_CELL_Y_START) / _CELL_HEIGHT;
}


BYTE CMapHandler::CalcDirection16(WORD wFireTileX, WORD wFireTileY, WORD wDestTileX, WORD wDestTileY)
{
	INT nWidth	= wDestTileX - wFireTileX;
	INT nHeight = wDestTileY - wFireTileY;

	FLOAT	rLineLength, rBottomInTriangle;
	INT		nDimension;
	BYTE	bDir;
	rLineLength = (FLOAT)sqrt((double)(nWidth*nWidth+nHeight*nHeight));
	
	// 기본.
	// 7  0  1          
	// 6     2
	// 5  4  3
	// 일단은 4개의 분면(90도기준)으로 나누고 분면에 대한 cos값을 적용한다.
	( nWidth >= 0 ) ?
		( nHeight <  0 ? (rBottomInTriangle=(FLOAT)-nHeight, nDimension=0) : (rBottomInTriangle=(FLOAT) nWidth, nDimension=4) ):
		( nHeight >= 0 ? (rBottomInTriangle=(FLOAT) nHeight, nDimension=8) : (rBottomInTriangle=(FLOAT)-nWidth, nDimension=12) );
	// 6(cos45)  0(cos 0)  0(cos45)
	// 4(cos90)  2(cos 0)  2(cos 0)
	// 4(cos45)  2(cos90)  2(cos45)

	FLOAT rCosVal = rBottomInTriangle/rLineLength;

	CONST FLOAT rCosVal16[8] = { 1.0f, 0.980785f, 0.923880f, 0.831470f, 0.707107f, 0.555570f, 0.382683f, 0.195090f };
	
	// 각분면을 3개의 영역으로 나누어서 영역을 재조정한다.
	bDir = 0;
	for ( INT nCnt = 0; nCnt < 8; nCnt++ )
	{
		if( rCosVal <= rCosVal16[nCnt] )
			bDir = nDimension+(nCnt+1)/2;
		else break;
	}
	
	if( bDir >= 16 )					bDir = 0;
	
	return bDir;
}



BYTE CMapHandler::CalcDirection8(WORD wFireTileX, WORD wFireTileY, WORD wDestTileX, WORD wDestTileY)
{
	INT nWidth	= wDestTileX - wFireTileX;
	INT nHeight = wDestTileY - wFireTileY;

	FLOAT	rLineLength, rBottomInTriangle;
	INT		nDimension;
	BYTE	bDir;
	rLineLength = (FLOAT)sqrt((double)(nWidth*nWidth+nHeight*nHeight));
	
	// 기본.
	// 7  0  1          
	// 6     2
	// 5  4  3
	// 일단은 4개의 분면(90도기준)으로 나누고 분면에 대한 cos값을 적용한다.
	( nWidth >= 0 ) ?
		( nHeight <  0 ? (rBottomInTriangle=(FLOAT)-nHeight, nDimension=0) : (rBottomInTriangle=(FLOAT) nWidth, nDimension=2) ):
		( nHeight >= 0 ? (rBottomInTriangle=(FLOAT) nHeight, nDimension=4) : (rBottomInTriangle=(FLOAT)-nWidth, nDimension=6) );
	// 6(cos45)  0(cos 0)  0(cos45)
	// 4(cos90)  2(cos 0)  2(cos 0)
	// 4(cos45)  2(cos90)  2(cos45)

	FLOAT rCosVal = rBottomInTriangle/rLineLength;
	
//	cos(0), cos(pi/8), cos(pi/4), cos(pi/2)
	CONST FLOAT rCosVal8[4] = { 1.0f, 0.923880f, 0.707107f, 0.382683f };
	
	// 각분면을 3개의 영역으로 나누어서 영역을 재조정한다.
	bDir = 0;
	for ( INT nCnt = 0; nCnt < 4; nCnt++ )
	{
		if( rCosVal <= rCosVal8[nCnt] )
			bDir = nDimension+(nCnt+1)/2;
		else break;
	}
	
	if( bDir >= 8 )					bDir = 0;
	
	return bDir;
}








/*
VOID CMapHandler::DrawOneCellObjTile(INT nX, INT nY)
{
	if ( nY >= m_stMapFileHeader.shHeight )
		return;

	INT		nObjFileIdx, nImgIdx;

	// 오브젝트가 있을때.
	if ( m_pstCellInfo[nY + nX*m_stMapFileHeader.shHeight].cFileIdx != -1 )
	{
		// 오브젝트1이 있을때.
		if ( m_pstCellInfo[nY + nX*m_stMapFileHeader.shHeight].shObj1 != -1 )
		{
			nObjFileIdx = (m_pstCellInfo[nY + nX*m_stMapFileHeader.shHeight].cFileIdx & 0XF0) >> 4;
			nImgIdx		= m_pstCellInfo[nY + nX*m_stMapFileHeader.shHeight].shObj1;

			m_pxTileImg[nObjFileIdx]->NewSetIndex(nImgIdx);

			if ( m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shWidth == 48 &&
				 m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight == 32 )
			{
				g_xMainWnd.DrawWithImageForCompClipRgn(
							(nX-m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START-m_shViewOffsetX,
							(nY-m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START-m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight+_CELL_HEIGHT-m_shViewOffsetY,
							m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shWidth,
							m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight,
							(WORD*)m_pxTileImg[nObjFileIdx]->m_pbCurrImage,
							_CLIP_WIDTH, _CLIP_HEIGHT);
			}
		}
		// 오브젝트2가 있을때.
		if ( m_pstCellInfo[nY + nX*m_stMapFileHeader.shHeight].shObj2 != -1 )
		{
			nObjFileIdx = (m_pstCellInfo[nY + nX*m_stMapFileHeader.shHeight].cFileIdx & 0X0F);
			nImgIdx		= m_pstCellInfo[nY + nX*m_stMapFileHeader.shHeight].shObj2;

			m_pxTileImg[nObjFileIdx]->NewSetIndex(nImgIdx);

			if ( m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shWidth == 48 &&
				 m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight == 32 )
			{
				g_xMainWnd.DrawWithImageForCompClipRgn(
							(nX-m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START-m_shViewOffsetX,
							(nY-m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START-m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight+_CELL_HEIGHT-m_shViewOffsetY,
							m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shWidth,
							m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight,
							(WORD*)m_pxTileImg[nObjFileIdx]->m_pbCurrImage,
							_CLIP_WIDTH, _CLIP_HEIGHT);
			}
		}
	}
}



BOOL CMapHandler::DrawOneCellOverObjTile(INT nX, INT nY)
{
	if ( nY >= m_stMapFileHeader.shHeight )
		return FALSE;

	INT		nObjFileIdx, nImgIdx;

	// 오브젝트가 있을때.
	if ( m_pstCellInfo[nY + nX*m_stMapFileHeader.shHeight].cFileIdx != -1 )
	{
		// 오브젝트1이 있을때.
		if ( m_pstCellInfo[nY + nX*m_stMapFileHeader.shHeight].shObj1 != -1 )
		{
			nObjFileIdx = (m_pstCellInfo[nY + nX*m_stMapFileHeader.shHeight].cFileIdx & 0XF0) >> 4;
			nImgIdx		= m_pstCellInfo[nY + nX*m_stMapFileHeader.shHeight].shObj1;

			m_pxTileImg[nObjFileIdx]->NewSetIndex(nImgIdx);

			if ( m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shWidth != 48 ||
				 m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight != 32 )
			{
				g_xMainWnd.DrawWithImageForCompClipRgn(
							(nX-m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START-m_shViewOffsetX,
							(nY-m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START-m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight+_CELL_HEIGHT-m_shViewOffsetY,
							m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shWidth,
							m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight,
							(WORD*)m_pxTileImg[nObjFileIdx]->m_pbCurrImage,
							_CLIP_WIDTH, _CLIP_HEIGHT);
				return TRUE;
			}
		}
		// 오브젝트2가 있을때.
		if ( m_pstCellInfo[nY + nX*m_stMapFileHeader.shHeight].shObj2 != -1 )
		{
			nObjFileIdx = (m_pstCellInfo[nY + nX*m_stMapFileHeader.shHeight].cFileIdx & 0X0F);
			nImgIdx		= m_pstCellInfo[nY + nX*m_stMapFileHeader.shHeight].shObj2;

			m_pxTileImg[nObjFileIdx]->NewSetIndex(nImgIdx);

			if ( m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shWidth != 48 ||
				 m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight != 32 )
			{
				g_xMainWnd.DrawWithImageForCompClipRgn(
							(nX-m_shStartViewTileX)*_CELL_WIDTH +_VIEW_CELL_X_START-m_shViewOffsetX,
							(nY-m_shStartViewTileY)*_CELL_HEIGHT+_VIEW_CELL_Y_START-m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight+_CELL_HEIGHT-m_shViewOffsetY,
							m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shWidth,
							m_pxTileImg[nObjFileIdx]->m_lpstNewCurrWilImageInfo->shHeight,
							(WORD*)m_pxTileImg[nObjFileIdx]->m_pbCurrImage,
							_CLIP_WIDTH, _CLIP_HEIGHT);
				return TRUE;
			}
		}
	}
	return FALSE;
}
*/


/******************************************************************************************************************

	함수명 : CMapHandler::DrawSMTile()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : INT nX
	         INT nY
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
/*
VOID CMapHandler::DrawSMTile(INT nX, INT nY)
{
	INT nObjImgIdx, nMidImgIdx;
	if ( nX >= m_stMapFileHeader.wWidth || nY >= m_stMapFileHeader.wHeight || nX < 0 || nY < 0 )
		return; 
	nObjImgIdx = (m_lpstCellInfo[nY + nX*m_stMapFileHeader.wHeight].bArea * 10000) + (m_lpstCellInfo[nY + nX*m_stMapFileHeader.wHeight].wFrImgIdx & 0x7FFF) - 1;
	if (m_lpstCellInfo[nY + nX*m_stMapFileHeader.wHeight].bArea < 8 )//<---------------??????????3번맵에 안쓰면 에러남
	{
		nMidImgIdx = m_lpstCellInfo[nY + nX*m_stMapFileHeader.wHeight].wMidImgIdx - 1;
		if ( nMidImgIdx >= 0 && nMidImgIdx < 938 && nY < m_shStartViewTileY+_VIEW_CELL_Y_COUNT )
		{
			m_xSMTileImg.SetIndex(nMidImgIdx);
			g_xMainWnd.DrawWithImageForCompMemToMem(
							(nX-m_shStartViewTileX)*_CELL_WIDTH ,
							(nY-m_shStartViewTileY)*_CELL_HEIGHT,
							m_xSMTileImg.m_lpstCurrWilImageInfo->shWidth,
							m_xSMTileImg.m_lpstCurrWilImageInfo->shHeight,
							(WORD*)m_xSMTileImg.m_pbCurrImage,
							_VIEW_CELL_X_COUNT*_CELL_WIDTH, _VIEW_CELL_Y_COUNT*_CELL_HEIGHT, m_wSavedMapBuffer);
		}
	}
}
*/
