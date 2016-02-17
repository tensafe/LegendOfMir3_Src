/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/



#ifndef _CWATER_H
#define _CWATER_H

class CWater
{
public:
	CWater();
	~CWater();
protected:
	D3DLVERTEX			m_avLight[(WATER_NUM_CNTY_LIGHTTILE+1)][(WATER_NUM_CNTX_LIGHTTILE+1)];
	D3DLVERTEX			m_avSaveLight[(WATER_NUM_CNTY_LIGHTTILE+1)][(WATER_NUM_CNTX_LIGHTTILE+1)];
	WORD				m_pwLightIndices[WATER_NUM_LIGHTTILE_INDICES];
	DWORD				m_dwLightColor;	

public:
	BOOL				m_bShowFog;
	
	CHAR				m_szWater[26][MAX_PATH];	
	BYTE				m_bCurrTexIdx;

public:
	VOID	Init(DWORD dwLightColor);
	VOID	ShowLightFog(BOOL bShowFog = TRUE);
	// 새로 바뀐 색을 m_avSaveLight에 저장시킨다.
	VOID	ChangeLightColor(DWORD dwColor);
	VOID	ClearSavedLightColor();
	BOOL	GetScreenPosToLightTile(INT nX, INT nY, INT* nTileX, INT* nTileY);
	BOOL	SetLightRadiusWithRing(INT XLightPos, INT YLightPos, INT nSmallRadius, 
								   INT nSmallRed, INT nSmallGreen, INT nSmallBlue, 
								   INT nLargeRadius,
								   INT nLargeRed, INT nLargeGreen, INT nLargeBlue);
	BOOL	SetLightRadiusWithCircle(INT XLightPos, INT YLightPos, INT nRadius,
									 INT nRedValue, INT nGreenValue, INT nBlueValue, 
									 FLOAT rDrakRate = 1.0f);
	BOOL	SetLightRadiusWithElipse(INT XLightPos, INT YLightPos, INT nXRadius, INT nYRadius, 
		                             INT nRedValue, INT nGreenValue, INT nBlueValue, 
									 FLOAT rDrakRate = 1.0f);
};


#endif // _CWATER_H