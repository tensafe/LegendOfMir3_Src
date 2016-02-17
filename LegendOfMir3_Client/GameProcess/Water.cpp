#include "StdAfx.h"


CWater::CWater()
{
	Init(RGB(220, 220, 220));
}


CWater::~CWater()
{
}


VOID CWater::Init(DWORD dwLightColor)
{
	m_dwLightColor = dwLightColor;
	m_bShowFog = FALSE;

	for ( INT nCntY = 0; nCntY < (WATER_NUM_CNTY_LIGHTTILE+1); nCntY++ )
	{
		for ( INT nCntX = 0; nCntX < (WATER_NUM_CNTX_LIGHTTILE+1); nCntX++ )
		{
			FLOAT az = (FLOAT)GetRandomNum(0, 17);
			m_avLight[nCntY][nCntX] = D3DLVERTEX(D3DVECTOR((FLOAT)(WATER_TILE_START_XPOS+(nCntX*WATER_TILE_WIDTH)), (FLOAT)(-WATER_TILE_START_YPOS-(nCntY*WATER_TILE_HEIGHT)), az),
								  			     m_dwLightColor, RGB(0, 0, 0), (FLOAT)((FLOAT)nCntX/(FLOAT)WATER_NUM_CNTX_LIGHTTILE), (FLOAT)((FLOAT)nCntY/(FLOAT)WATER_NUM_CNTY_LIGHTTILE));
		}
	}

	memcpy(m_avSaveLight, m_avLight, sizeof(D3DLVERTEX)*(WATER_NUM_CNTY_LIGHTTILE+1)*(WATER_NUM_CNTX_LIGHTTILE+1));

	// 타일폴리곤의 인덱스 생성.
    WORD* pwIndex = m_pwLightIndices;
	for ( nCntY = 0; nCntY < WATER_NUM_CNTY_LIGHTTILE; nCntY++ )
	{
		for ( INT nCntX = 0; nCntX < WATER_NUM_CNTX_LIGHTTILE; nCntX++ )
		{
			*pwIndex++ = (nCntX+1)+((WATER_NUM_CNTX_LIGHTTILE+1)*nCntY);  
			*pwIndex++ = nCntX+((WATER_NUM_CNTX_LIGHTTILE+1)*(nCntY+1));  
			*pwIndex++ = (nCntX+1)+((WATER_NUM_CNTX_LIGHTTILE+1)*(nCntY+1));
			*pwIndex++ = nCntX+((WATER_NUM_CNTX_LIGHTTILE+1)*(nCntY+1));  
			*pwIndex++ = (nCntX+1)+((WATER_NUM_CNTX_LIGHTTILE+1)*nCntY);  
			*pwIndex++ = (nCntX)+((WATER_NUM_CNTX_LIGHTTILE+1)*nCntY);
		}
	}

	m_bCurrTexIdx = 0;
	CHAR* szFileList[] = 
	{
		"Water00.bmp",
		"Water01.bmp",
		"Water02.bmp",
		"Water03.bmp",
		"Water04.bmp",
		"Water05.bmp",
		"Water06.bmp",
		"Water07.bmp",
		"Water08.bmp",
		"Water09.bmp",
		"Water10.bmp",
		"Water11.bmp",
		"Water12.bmp",
		"Water13.bmp",
		"Water14.bmp",
		"Water15.bmp",
		"Water16.bmp",
		"Water17.bmp",
		"Water18.bmp",
		"Water19.bmp",
		"Water20.bmp",
		"Water21.bmp",
		"Water22.bmp",
		"Water23.bmp",
		"Water24.bmp",
		"Water25.bmp",
	};
	for ( INT nCnt = 0; nCnt < 26; nCnt++ )
	{
		strcpy(m_szWater[nCnt], szFileList[nCnt]);
		D3DTextr_CreateTextureFromFile(m_szWater[nCnt]);
		D3DTextr_Restore(m_szWater[nCnt], g_xMainWnd.Get3DDevice());
	}
}


VOID CWater::ChangeLightColor(DWORD dwColor)
{
	if ( m_dwLightColor != dwColor )
	{
		for ( INT nCntY = 0; nCntY < WATER_NUM_CNTY_LIGHTTILE+1; nCntY++ )
		{
			for ( INT nCntX = 0; nCntX < WATER_NUM_CNTX_LIGHTTILE+1; nCntX++ )
			{
				m_avLight[nCntY][nCntX].color = dwColor;
			}
		}
		m_dwLightColor = dwColor;
		memcpy(m_avSaveLight, m_avLight, sizeof(D3DLVERTEX)*(WATER_NUM_CNTY_LIGHTTILE+1)*(WATER_NUM_CNTX_LIGHTTILE+1));
	}
}


VOID CWater::ClearSavedLightColor()
{
	memcpy(m_avLight, m_avSaveLight, sizeof(D3DLVERTEX)*(WATER_NUM_CNTY_LIGHTTILE+1)*(WATER_NUM_CNTX_LIGHTTILE+1));
}


BOOL CWater::GetScreenPosToLightTile(INT nX, INT nY, INT* nTileX, INT* nTileY)
{
	*nTileX = (nX - WATER_TILE_START_XPOS) / WATER_TILE_WIDTH;
	*nTileY = (nY - WATER_TILE_START_YPOS) / WATER_TILE_HEIGHT;
	
	return TRUE;
}


BOOL CWater::SetLightRadiusWithRing(INT XLightPos, INT YLightPos, 
									   INT nSmallRadius, 
									   INT nSmallRed, INT nSmallGreen, INT nSmallBlue, 
									   INT nLargeRadius,
									   INT nLargeRed, INT nLargeGreen, INT nLargeBlue)
{
	INT nX, nY;
	GetScreenPosToLightTile(XLightPos, YLightPos, &nX, &nY);

	INT nStartTileX			= nX - nLargeRadius;
	INT nStartTileY			= nY - nLargeRadius;
	INT nEndTileX			= nX + nLargeRadius + 1;
	INT nEndTileY			= nY + nLargeRadius + 1;

	// y를 축으로 해서 젤 어두운 부분을 구한다.(x보다 y가 더 적으므로)
	nLargeRadius = (nLargeRadius*WATER_TILE_HEIGHT);
	nSmallRadius = (nSmallRadius*WATER_TILE_HEIGHT);

	DOUBLE rLengthCnt		= 0;
	INT nSelectedRedValue	= 0; 
	INT nSelectedGreenValue	= 0;
	INT nSelectedBlueValue	= 0;

	if ( nStartTileX < 0 )					nStartTileX = 0;
	if ( nStartTileY < 0 )					nStartTileY = 0;
	if ( nEndTileX > WATER_NUM_CNTX_LIGHTTILE+1 )	nEndTileX = WATER_NUM_CNTX_LIGHTTILE+1;
	if ( nEndTileY > WATER_NUM_CNTY_LIGHTTILE+1 )	nEndTileY = WATER_NUM_CNTY_LIGHTTILE+1;	

	for ( INT nCntY = nStartTileY; nCntY < nEndTileY; nCntY++ )
	{
		for ( INT nCntX = nStartTileX; nCntX < nEndTileX; nCntX++ )
		{			
			rLengthCnt	= sqrt((XLightPos-(nCntX*WATER_TILE_WIDTH+WATER_TILE_START_XPOS))*(XLightPos-(nCntX*WATER_TILE_WIDTH+WATER_TILE_START_XPOS)) + 
							   (YLightPos-(nCntY*WATER_TILE_HEIGHT+WATER_TILE_START_YPOS))*(YLightPos-(nCntY*WATER_TILE_HEIGHT+WATER_TILE_START_YPOS)));
			if ( nCntX < WATER_NUM_CNTX_LIGHTTILE && nCntY < WATER_NUM_CNTY_LIGHTTILE )
			{
				// 포인트점을 포함하지 않는 좌하. 포인트점을 포함하지 않는 우상.
				if ( (nCntX > nX && nCntY < nY) || (nCntX < nX && nCntY > nY) )
				{
					WORD* pwIndex = ((WORD*)m_pwLightIndices+(nCntX+nCntY*(WATER_NUM_CNTX_LIGHTTILE))*6);
					*pwIndex++ = (nCntX)+((WATER_NUM_CNTX_LIGHTTILE+1)*nCntY);
					*pwIndex++ = (nCntX+1)+((WATER_NUM_CNTX_LIGHTTILE+1)*nCntY);  
					*pwIndex++ = (nCntX+1)+((WATER_NUM_CNTX_LIGHTTILE+1)*(nCntY+1));
					*pwIndex++ = (nCntX)+((WATER_NUM_CNTX_LIGHTTILE+1)*nCntY);
					*pwIndex++ = nCntX+((WATER_NUM_CNTX_LIGHTTILE+1)*(nCntY+1));  
					*pwIndex++ = (nCntX+1)+((WATER_NUM_CNTX_LIGHTTILE+1)*(nCntY+1));
				}
				else
				{
					WORD* pwIndex = ((WORD*)m_pwLightIndices+(nCntX+nCntY*(WATER_NUM_CNTX_LIGHTTILE))*6);
					*pwIndex++ = (nCntX+1)+((WATER_NUM_CNTX_LIGHTTILE+1)*nCntY);  
					*pwIndex++ = nCntX+((WATER_NUM_CNTX_LIGHTTILE+1)*(nCntY+1));  
					*pwIndex++ = (nCntX+1)+((WATER_NUM_CNTX_LIGHTTILE+1)*(nCntY+1));
					*pwIndex++ = nCntX+((WATER_NUM_CNTX_LIGHTTILE+1)*(nCntY+1));  
					*pwIndex++ = (nCntX+1)+((WATER_NUM_CNTX_LIGHTTILE+1)*nCntY);  
					*pwIndex++ = (nCntX)+((WATER_NUM_CNTX_LIGHTTILE+1)*nCntY);
				}
			}

			if ( rLengthCnt	< nSmallRadius )
			{
				nSelectedRedValue	= nSmallRed - INT((nSmallRed*rLengthCnt)/(nLargeRadius));
				nSelectedGreenValue	= nSmallGreen - INT((nSmallGreen*rLengthCnt)/(nLargeRadius));
				nSelectedBlueValue	= nSmallBlue - INT((nSmallBlue*rLengthCnt)/(nLargeRadius));
			}
			else if ( rLengthCnt >= nSmallRadius && rLengthCnt <= nLargeRadius )
			{
				nSelectedRedValue	= nLargeRed - INT((nLargeRed*rLengthCnt)/(nLargeRadius));
				nSelectedGreenValue	= nLargeGreen - INT((nLargeGreen*rLengthCnt)/(nLargeRadius));
				nSelectedBlueValue	= nLargeBlue - INT((nLargeBlue*rLengthCnt)/(nLargeRadius));
			}
			else
			{
				nSelectedRedValue = nSelectedGreenValue = nSelectedBlueValue = 0;
			}

			if ( nSelectedRedValue	 < 0 )		nSelectedRedValue	= 0;
			if ( nSelectedGreenValue < 0 )		nSelectedGreenValue = 0;
			if ( nSelectedBlueValue	 < 0 )		nSelectedBlueValue	= 0;

			if ( m_avLight[nCntY][nCntX].color == 0 )
				m_avLight[nCntY][nCntX].color = RGB(nSelectedRedValue, nSelectedGreenValue, nSelectedBlueValue); 
			else
			{
				nSelectedRedValue += (INT)RGB_GETBLUE(m_avLight[nCntY][nCntX].color);
				nSelectedGreenValue += (INT)RGB_GETGREEN(m_avLight[nCntY][nCntX].color);
				nSelectedBlueValue += (INT)RGB_GETRED(m_avLight[nCntY][nCntX].color);

				if ( nSelectedRedValue	 > 255 )		nSelectedRedValue	= 255;
				if ( nSelectedGreenValue > 255 )		nSelectedGreenValue = 255;
				if ( nSelectedBlueValue	 > 255 )		nSelectedBlueValue	= 255;

				m_avLight[nCntY][nCntX].color =  RGB(nSelectedRedValue, nSelectedGreenValue, nSelectedBlueValue);
			}
		}
	}

	return TRUE;
}


BOOL CWater::SetLightRadiusWithCircle(INT XLightPos, INT YLightPos, INT nRadius, 
										 INT nRedValue, INT nGreenValue, INT nBlueValue, 
										 FLOAT rDrakRate)
{
	INT nX, nY;
	GetScreenPosToLightTile(XLightPos, YLightPos, &nX, &nY);

	INT nStartTileX			= nX - nRadius;
	INT nStartTileY			= nY - nRadius;
	INT nEndTileX			= nX + nRadius + 1;
	INT nEndTileY			= nY + nRadius + 1;

	// y를 축으로 해서 젤 어두운 부분을 구한다.(x보다 y가 더 적으므로)
	nRadius = (nRadius*WATER_TILE_HEIGHT);

	DOUBLE rLengthCnt		= 0;
	INT nSelectedRedValue	= 0; 
	INT nSelectedGreenValue	= 0;
	INT nSelectedBlueValue	= 0;

	if ( nStartTileX < 0 )					nStartTileX = 0;
	if ( nStartTileY < 0 )					nStartTileY = 0;
	if ( nEndTileX > WATER_NUM_CNTX_LIGHTTILE+1 )	nEndTileX = WATER_NUM_CNTX_LIGHTTILE+1;
	if ( nEndTileY > WATER_NUM_CNTY_LIGHTTILE+1 )	nEndTileY = WATER_NUM_CNTY_LIGHTTILE+1;	

	for ( INT nCntY = nStartTileY; nCntY < nEndTileY; nCntY++ )
	{
		for ( INT nCntX = nStartTileX; nCntX < nEndTileX; nCntX++ )
		{			
			rLengthCnt	= sqrt((XLightPos-(nCntX*WATER_TILE_WIDTH+WATER_TILE_START_XPOS))*(XLightPos-(nCntX*WATER_TILE_WIDTH+WATER_TILE_START_XPOS)) + 
							   (YLightPos-(nCntY*WATER_TILE_HEIGHT+WATER_TILE_START_YPOS))*(YLightPos-(nCntY*WATER_TILE_HEIGHT+WATER_TILE_START_YPOS)));
			if ( nCntX < WATER_NUM_CNTX_LIGHTTILE && nCntY < WATER_NUM_CNTY_LIGHTTILE )
			{
				// 포인트점을 포함하지 않는 좌하. 포인트점을 포함하지 않는 우상.
				if ( (nCntX > nX && nCntY < nY) || (nCntX < nX && nCntY > nY) )
				{
					WORD* pwIndex = ((WORD*)m_pwLightIndices+(nCntX+nCntY*(WATER_NUM_CNTX_LIGHTTILE))*6);
					*pwIndex++ = (nCntX)+((WATER_NUM_CNTX_LIGHTTILE+1)*nCntY);
					*pwIndex++ = (nCntX+1)+((WATER_NUM_CNTX_LIGHTTILE+1)*nCntY);  
					*pwIndex++ = (nCntX+1)+((WATER_NUM_CNTX_LIGHTTILE+1)*(nCntY+1));
					*pwIndex++ = (nCntX)+((WATER_NUM_CNTX_LIGHTTILE+1)*nCntY);
					*pwIndex++ = nCntX+((WATER_NUM_CNTX_LIGHTTILE+1)*(nCntY+1));  
					*pwIndex++ = (nCntX+1)+((WATER_NUM_CNTX_LIGHTTILE+1)*(nCntY+1));
				}
				else
				{
					WORD* pwIndex = ((WORD*)m_pwLightIndices+(nCntX+nCntY*(WATER_NUM_CNTX_LIGHTTILE))*6);
					*pwIndex++ = (nCntX+1)+((WATER_NUM_CNTX_LIGHTTILE+1)*nCntY);  
					*pwIndex++ = nCntX+((WATER_NUM_CNTX_LIGHTTILE+1)*(nCntY+1));  
					*pwIndex++ = (nCntX+1)+((WATER_NUM_CNTX_LIGHTTILE+1)*(nCntY+1));
					*pwIndex++ = nCntX+((WATER_NUM_CNTX_LIGHTTILE+1)*(nCntY+1));  
					*pwIndex++ = (nCntX+1)+((WATER_NUM_CNTX_LIGHTTILE+1)*nCntY);  
					*pwIndex++ = (nCntX)+((WATER_NUM_CNTX_LIGHTTILE+1)*nCntY);
				}
			}

/*			RECT rc;
			SetRect(&rc,
				   (nCntX*WATER_TILE_WIDTH+WATER_TILE_START_XPOS),
				   (nCntY*WATER_TILE_HEIGHT+WATER_TILE_START_YPOS),
				   (nCntX*WATER_TILE_WIDTH+WATER_TILE_START_XPOS+WATER_TILE_WIDTH),
				   (nCntY*WATER_TILE_HEIGHT+WATER_TILE_START_YPOS+WATER_TILE_HEIGHT));
			g_xMainWnd.DrawWithGDI(rc, NULL, RGB(0, 255, 255), 1);
*/
			if ( rLengthCnt	<= nRadius )
			{
				nSelectedRedValue	= nRedValue - INT((nRedValue*rLengthCnt*rDrakRate)/(nRadius));
				nSelectedGreenValue	= nGreenValue - INT((nGreenValue*rLengthCnt*rDrakRate)/(nRadius));
				nSelectedBlueValue	= nBlueValue - INT((nBlueValue*rLengthCnt*rDrakRate)/(nRadius));
/*				SetRect(&rc,
					   (nCntX*WATER_TILE_WIDTH+WATER_TILE_START_XPOS),
					   (nCntY*WATER_TILE_HEIGHT+WATER_TILE_START_YPOS),
					   (nCntX*WATER_TILE_WIDTH+WATER_TILE_START_XPOS+2),
					   (nCntY*WATER_TILE_HEIGHT+WATER_TILE_START_YPOS+2));
				g_xMainWnd.DrawWithGDI(rc, NULL, RGB(255, 0, 0), 1);
*/			}
			else
			{
				nSelectedRedValue = nSelectedGreenValue = nSelectedBlueValue = 0;
			}

			if ( nSelectedRedValue	 < 0 )		nSelectedRedValue	= 0;
			if ( nSelectedGreenValue < 0 )		nSelectedGreenValue = 0;
			if ( nSelectedBlueValue	 < 0 )		nSelectedBlueValue	= 0;

			if ( m_avLight[nCntY][nCntX].color == 0 )
				m_avLight[nCntY][nCntX].color = RGB(nSelectedRedValue, nSelectedGreenValue, nSelectedBlueValue); 
			else
			{
				nSelectedRedValue += (INT)RGB_GETBLUE(m_avLight[nCntY][nCntX].color);
				nSelectedGreenValue += (INT)RGB_GETGREEN(m_avLight[nCntY][nCntX].color);
				nSelectedBlueValue += (INT)RGB_GETRED(m_avLight[nCntY][nCntX].color);

				if ( nSelectedRedValue	 > 255 )		nSelectedRedValue	= 255;
				if ( nSelectedGreenValue > 255 )		nSelectedGreenValue = 255;
				if ( nSelectedBlueValue	 > 255 )		nSelectedBlueValue	= 255;

				m_avLight[nCntY][nCntX].color =  RGB(nSelectedRedValue, nSelectedGreenValue, nSelectedBlueValue);
			}
		}
	}

	return TRUE;
}


BOOL CWater::SetLightRadiusWithElipse(INT XLightPos, INT YLightPos, INT nXRadius, INT nYRadius, 
										 INT nRedValue, INT nGreenValue, INT nBlueValue, 
										 FLOAT rDrakRate)
{
	INT nX, nY;
	GetScreenPosToLightTile(XLightPos, YLightPos, &nX, &nY);

	INT nStartTileX			= nX - nXRadius;
	INT nStartTileY			= nY - nYRadius;
	INT nEndTileX			= nX + nXRadius + 1;
	INT nEndTileY			= nY + nYRadius + 1;
	
	FLOAT rXRadius = (FLOAT)(nXRadius*WATER_TILE_WIDTH);
	FLOAT rYRadius = (FLOAT)(nYRadius*WATER_TILE_HEIGHT);
	INT	  nRadius  = 0;

	if ( rXRadius > rYRadius )
	{
		nRadius = (INT)rXRadius;
	}
	else
	{
		nRadius = (INT)rYRadius;
	}

	DOUBLE rLengthCnt		= 0;
	INT nSelectedRedValue	= 0; 
	INT nSelectedGreenValue	= 0; 
	INT nSelectedBlueValue	= 0; 

	if ( nStartTileX < 0 )					nStartTileX = 0;
	if ( nStartTileY < 0 )					nStartTileY = 0;
	if ( nEndTileX > WATER_NUM_CNTX_LIGHTTILE+1 )	nEndTileX = WATER_NUM_CNTX_LIGHTTILE+1;
	if ( nEndTileY > WATER_NUM_CNTY_LIGHTTILE+1 )	nEndTileY = WATER_NUM_CNTY_LIGHTTILE+1;


	for ( INT nCntY = nStartTileY; nCntY < nEndTileY; nCntY++ )
	{
		for ( INT nCntX = nStartTileX; nCntX < nEndTileX; nCntX++ )
		{
			if ( nCntX < WATER_NUM_CNTX_LIGHTTILE && nCntY < WATER_NUM_CNTY_LIGHTTILE )
			{
				// 포인트점을 포함하지 않는 좌하. 포인트점을 포함하지 않는 우상.
				if ( (nCntX > nX && nCntY < nY) || (nCntX < nX && nCntY > nY) )
				{
					WORD* pwIndex = ((WORD*)m_pwLightIndices+(nCntX+nCntY*(WATER_NUM_CNTX_LIGHTTILE))*6);
					*pwIndex++ = (nCntX)+((WATER_NUM_CNTX_LIGHTTILE+1)*nCntY);
					*pwIndex++ = (nCntX+1)+((WATER_NUM_CNTX_LIGHTTILE+1)*nCntY);  
					*pwIndex++ = (nCntX+1)+((WATER_NUM_CNTX_LIGHTTILE+1)*(nCntY+1));
					*pwIndex++ = (nCntX)+((WATER_NUM_CNTX_LIGHTTILE+1)*nCntY);
					*pwIndex++ = nCntX+((WATER_NUM_CNTX_LIGHTTILE+1)*(nCntY+1));  
					*pwIndex++ = (nCntX+1)+((WATER_NUM_CNTX_LIGHTTILE+1)*(nCntY+1));
				}
				else
				{
					WORD* pwIndex = ((WORD*)m_pwLightIndices+(nCntX+nCntY*(WATER_NUM_CNTX_LIGHTTILE))*6);
					*pwIndex++ = (nCntX+1)+((WATER_NUM_CNTX_LIGHTTILE+1)*nCntY);  
					*pwIndex++ = nCntX+((WATER_NUM_CNTX_LIGHTTILE+1)*(nCntY+1));  
					*pwIndex++ = (nCntX+1)+((WATER_NUM_CNTX_LIGHTTILE+1)*(nCntY+1));
					*pwIndex++ = nCntX+((WATER_NUM_CNTX_LIGHTTILE+1)*(nCntY+1));  
					*pwIndex++ = (nCntX+1)+((WATER_NUM_CNTX_LIGHTTILE+1)*nCntY);  
					*pwIndex++ = (nCntX)+((WATER_NUM_CNTX_LIGHTTILE+1)*nCntY);
				}
			}

/*			RECT rc;
			SetRect(&rc,
				   (nCntX*WATER_TILE_WIDTH+WATER_TILE_START_XPOS),
				   (nCntY*WATER_TILE_HEIGHT+WATER_TILE_START_YPOS),
				   (nCntX*WATER_TILE_WIDTH+WATER_TILE_START_XPOS+WATER_TILE_WIDTH),
				   (nCntY*WATER_TILE_HEIGHT+WATER_TILE_START_YPOS+WATER_TILE_HEIGHT));
			g_xMainWnd.DrawWithGDI(rc, NULL, RGB(0, 255, 255), 1);
*/
	
			INT nCurrCntXRate, nCurrCntYRate;
			if ( rXRadius > rYRadius )		
			{
				nCurrCntXRate = nCntX;
				nCurrCntYRate = nCntY + (INT)((nCntY-nY)*(FLOAT)(rXRadius/rYRadius-1.0f));
			}
			else if ( rXRadius < rYRadius )										
			{
				nCurrCntXRate = nCntX + (INT)((nCntX-nX)*(FLOAT)(rYRadius/rXRadius-1.0f));
				nCurrCntYRate = nCntY;
			}
			else
			{
				nCurrCntXRate = nCntX;
				nCurrCntYRate = nCntY;
			}

			rLengthCnt	= sqrt((XLightPos-(nCurrCntXRate*WATER_TILE_WIDTH+WATER_TILE_START_XPOS))*(XLightPos-(nCurrCntXRate*WATER_TILE_WIDTH+WATER_TILE_START_XPOS)) + 
							   (YLightPos-(nCurrCntYRate*WATER_TILE_HEIGHT+WATER_TILE_START_YPOS))*(YLightPos-(nCurrCntYRate*WATER_TILE_HEIGHT+WATER_TILE_START_YPOS)));

/*			SetRect(&rc,
				   (XLightPos),
				   (YLightPos),
				   (XLightPos+5),
				   (YLightPos+5));
			g_xMainWnd.DrawWithGDI(rc, NULL, RGB(255, 0, 0), 1);
*/
			if ( rLengthCnt	<= nRadius )
			{
				nSelectedRedValue	= nRedValue - INT((nRedValue*rLengthCnt*rDrakRate)/(nRadius));
				nSelectedGreenValue	= nGreenValue - INT((nGreenValue*rLengthCnt*rDrakRate)/(nRadius));
				nSelectedBlueValue	= nBlueValue - INT((nBlueValue*rLengthCnt*rDrakRate)/(nRadius));
	/*			SetRect(&rc,
					   (nCntX*WATER_TILE_WIDTH+WATER_TILE_START_XPOS),
					   (nCntY*WATER_TILE_HEIGHT+WATER_TILE_START_YPOS),
					   (nCntX*WATER_TILE_WIDTH+WATER_TILE_START_XPOS+2),
					   (nCntY*WATER_TILE_HEIGHT+WATER_TILE_START_YPOS+2));
				g_xMainWnd.DrawWithGDI(rc, NULL, RGB(255, 0, 0), 1);
	*/			}
			else
			{
				nSelectedRedValue = nSelectedGreenValue = nSelectedBlueValue = 0;
			}

			if ( nSelectedRedValue	 < 0 )		nSelectedRedValue	= 0;
			if ( nSelectedGreenValue < 0 )		nSelectedGreenValue = 0;
			if ( nSelectedBlueValue	 < 0 )		nSelectedBlueValue	= 0;

			if ( m_avLight[nCntY][nCntX].color == 0 )
				m_avLight[nCntY][nCntX].color = RGB(nSelectedRedValue, nSelectedGreenValue, nSelectedBlueValue); 
			else
			{
				if ( nSelectedRedValue < (INT)RGB_GETBLUE(m_avLight[nCntY][nCntX].color) )
					nSelectedRedValue = (INT)RGB_GETBLUE(m_avLight[nCntY][nCntX].color);
				if ( nSelectedGreenValue < (INT)RGB_GETGREEN(m_avLight[nCntY][nCntX].color) )
					nSelectedGreenValue = (INT)RGB_GETGREEN(m_avLight[nCntY][nCntX].color);
				if ( nSelectedBlueValue < (INT)RGB_GETRED(m_avLight[nCntY][nCntX].color) )
					nSelectedBlueValue = (INT)RGB_GETRED(m_avLight[nCntY][nCntX].color);

				m_avLight[nCntY][nCntX].color =  RGB(nSelectedRedValue, nSelectedGreenValue, nSelectedBlueValue);
			}
		}
	}

	return TRUE;
}




VOID CWater::ShowLightFog(BOOL bShowFog)
{
	if ( g_xMainWnd.Get3DDevice() )
	{
		if( SUCCEEDED(g_xMainWnd.Get3DDevice()->BeginScene()) )
		{
			D3DVECTOR vTrans;
			D3DMATRIX matTrans;
			D3DMATRIX matWorldOriginal;

			g_xMainWnd.Get3DDevice()->GetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

//			if ( bShowFog )
//				SetLightRadiusWithElipse(400, 242, 6, 6, 230, 230, 230);
//				SetLightRadiusWithElipse(400, 242, 1, 1, 230, 230, 230);
//				SetLightRadiusWithCircle(400, 242, 8, 230, 230, 230);
/*				SetLightRadiusWithCircle(586, 248, 3, 230, 230, 230);
				SetLightRadiusWithCircle(604, 248, 3, 230, 230, 230);
*///				SetLightRadiusWithRing(400, 242, 7, 230, 230, 230, 8, 230, 230, 230);
			g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, TRUE);
			g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_SRCBLEND,  D3DBLEND_ZERO);
			g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_SRCCOLOR);

/*			BYTE bOpa = 255;
			D3DMATERIAL7 mtrl;
			D3DUtil_InitMaterial(mtrl, 
								 (FLOAT)255/255.0f,
								 (FLOAT)255/255.0f,
								 (FLOAT)255/255.0f);
			mtrl.diffuse.a = (bOpa+1)/255.0f;
			g_xMainWnd.Get3DDevice()->SetMaterial(&mtrl);
			SetBlendRenderState(g_xMainWnd.Get3DDevice(), _BLEND_NORMAL, mtrl);
*/
			vTrans.x = -400; vTrans.y = 300; vTrans.z = 0;
			D3DUtil_SetTranslateMatrix(matTrans, vTrans);
			g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &matTrans);
//			g_xMainWnd.Get3DDevice()->SetTexture(0, NULL);


			static int a = 0;
			static int b = 0;
			static float scrlcnt = 0.0f;
			if ( scrlcnt >= WATER_NUM_CNTX_LIGHTTILE )		scrlcnt = 0;
			a++;
			if ( a >= 1 )
			{
				for ( INT nCntY = 0; nCntY < (WATER_NUM_CNTY_LIGHTTILE+1); nCntY++ )
				{
					FLOAT av = (FLOAT)((FLOAT)nCntY/(FLOAT)WATER_NUM_CNTY_LIGHTTILE);

					for ( INT nCntX = 0; nCntX < (WATER_NUM_CNTX_LIGHTTILE+1); nCntX++ )
					{
						FLOAT au = (FLOAT)((FLOAT)nCntX/(FLOAT)WATER_NUM_CNTX_LIGHTTILE) + scrlcnt;
						if ( au > 1 )							au = 1-au;
						FLOAT az = m_avLight[nCntY][nCntX].z;
						if ( b > 10 )
						{
//							az = (FLOAT)GetRandomNum(0, 50);
						}
						FLOAT ax = /*(FLOAT)GetRandomNum(0, 10) + */(FLOAT)(WATER_TILE_START_XPOS+(nCntX*WATER_TILE_WIDTH));
						FLOAT ay = /*(FLOAT)GetRandomNum(0, 1) + */(FLOAT)(-WATER_TILE_START_YPOS-(nCntY*WATER_TILE_HEIGHT));

//						if ( av > 1 )							av = 1-av;
						
						m_avLight[nCntY][nCntX] = D3DLVERTEX(D3DVECTOR(ax, ay, az),
								  							 m_dwLightColor, RGB(0, 0, 0), au, av);
					}
				}
				memcpy(m_avSaveLight, m_avLight, sizeof(D3DLVERTEX)*(WATER_NUM_CNTY_LIGHTTILE+1)*(WATER_NUM_CNTX_LIGHTTILE+1));
				a = 0;
			}
			scrlcnt += 0.0006f;
			if ( b > 10 )
			{
				b = 0;
			}
			b++;

			g_xMainWnd.Get3DDevice()->SetTexture(0, D3DTextr_GetSurface(m_szWater[m_bCurrTexIdx]));

			g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_LIGHTING, FALSE);
			g_xMainWnd.Get3DDevice()->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,
														  D3DFVF_LVERTEX,
														  m_avLight, WATER_NUM_LIGHTTILE_VERTICES, 
														  m_pwLightIndices, WATER_NUM_LIGHTTILE_INDICES, NULL );
			g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_LIGHTING, TRUE);

//			ResetBlendenderState(g_xMainWnd.Get3DDevice());
			g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_SRCBLEND,  D3DBLEND_ONE);
			g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_DESTBLEND, D3DBLEND_ZERO);
			g_xMainWnd.Get3DDevice()->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE, FALSE);

			g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

			g_xMainWnd.Get3DDevice()->EndScene();
		}
	}
}
