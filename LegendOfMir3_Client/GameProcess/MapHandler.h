/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/


#ifndef _MAPHANDLER_
#define	_MAPHANDLER_


#pragma pack(1)
typedef struct tagLIGHTINFO
{
	BOOL	bIsLight;
	CHAR	cLightSizeType;
	CHAR	cLightColorType;
}LIGHTINFO, *LPLIGHTINFO;

// 맵파일 로딩관련 정보저장.
// 현재 있는 지역의 전체 맵에 관련된 사항을 보관한다.
typedef struct tagMAPFILEHEADER
{
	CHAR	szDesc[20];
	WORD	wAttr;
	SHORT	shWidth;
	SHORT	shHeight;
	CHAR	cEventFileIdx;
	CHAR	cFogColor;
}MAPFILEHEADER, *LPMAPFILEHEADER;

typedef struct tagTILEINFO
{
	BYTE	bFileIdx;
	WORD	wTileIdx;
}TILEINFO, *LPTILEINFO;

typedef struct tagCELLINFO
{
	BYTE	bFlag;
	BYTE	bObj1Ani;
	BYTE	bObj2Ani;
	BYTE	bFileIdx;
	WORD	wObj1;
	WORD	wObj2;
	BYTE	bDoorIdx;
	BYTE	bDoorOffset;
	WORD	wLigntNEvent;
}CELLINFO, *LPCELLINFO;
/*
typedef struct tagDOORIMAGEINFO
{
	WORD	wPosX;
	WORD	wPosY;
	WORD	wImageNum;
}DOORIMAGEINFO, *LPDOORIMAGEINFO;

typedef struct tagDOORINFO
{
	BYTE			bDoorIdx;
	BYTE			bDoorImgCnt;
	LPDOORIMAGEINFO	pstDoorImgInfo;
}DOORINFO, *LPDOORINFO;
*/
#pragma pack(8)


class CMapHandler
{
private:
protected:
public:
	LPTILEINFO		m_pstTileInfo;
	LPCELLINFO		m_pstCellInfo;

	MAPFILEHEADER	m_stMapFileHeader;				// 로딩된 맵파일의 헤더.

	// 캐릭터의 이동과 관련된다.
	SHORT			m_shStartViewTileX;				// 뷰영역의 시작 X Tile 좌표.(실제 스크린 시작 타일 좌표보다 조금 더 큰 사이즈이다.)
	SHORT			m_shStartViewTileY;				// 뷰영역은 실제 스크린 영역보다 가로, 세로 1타일씩을 여분으로 둔다.
	SHORT			m_shViewOffsetX;				// 스크롤한 X축한 길이.(캐릭터의 걷기동작의 프레임.)
	SHORT			m_shViewOffsetY;				

	RECT			m_rcView;						// 맵기본타일이 그려질 영역.(화면클리핑을 위해서)

	POINT			m_ptMovePixelArray[6];			// 이동을 위한 픽셀값의 보관데이타.

	WORD			m_wSavedMapBuffer[_VIEW_CELL_X_COUNT*_CELL_WIDTH*_VIEW_CELL_Y_COUNT*_CELL_HEIGHT];				// 기본타일과 SM타일을 기록한 버퍼. 자기 캐릭터가 이동하기전엔 갱신되지 않는다.


	DWORD			m_dwAniSaveTime[8];
	BYTE			m_bAniTileFrame[8][16];

	CWHWilImageData*	m_pxTileImg[_MAX_TILE_FILE];
/*
	BYTE			m_bDoorCount;
	LPDOORINFO		m_pstDoorInfo;
	BYTE*			m_pbCellIDoorIdx;
*/

public:
	CMapHandler();
	~CMapHandler();
	VOID			InitMapHandler();				// 클래스 정보 초기화.
	VOID			LoadMapHandler(CHAR* szMapFile, CImageHandler* pxImgHandler, INT nX, INT nY);// 이미지및 각종 맵용데이타 로드.
	VOID			DestroyMapHandler();			// 클래스 정보 초기화및 메모리 해제.

	BOOL			LoadMapData(CHAR* szMapFile);	// 적용되는 맵의 데이타를 로딩.(맵이 바뀔때마다 불려진다.)
	VOID			LoadMapImage(CImageHandler* pxImgHandler);					// 맵에 관련된 이미지 데이타 로딩(메모리맵사용).
	VOID			LoadNewMapBuffer();				// 맵이 처음 로딩되었을때 타일, SM타일 기록버퍼에 최초의 내용을 적용한다.

	VOID			FreeMapData();					// 적용되는 맵의 데이타를 해제.(맵이 바뀔때마다 불려진다.)

	VOID			DrawBaseTile(INT nX, INT nY);	// 기본타일을 그린다.
//	VOID			DrawOpenDoor(INT nX, INT nY);
//	VOID			SetDoorIndex();

	VOID			ScrollMap(INT nCharDir, INT nCharFrame, INT nSpeed);		// 캐릭터 이동시 맵의 Scroll관련 변수들 적용.
	VOID			ScrollMap(INT nCharDir, WORD wCurrDelay, WORD wMoveDelay, INT nSpeed);
	VOID			SetMovedTileBuffer(SHORT shMovedTileX, SHORT shMovedTileY);	// 캐릭터 이동시 새로 갱신되는 기본 이미지(타일, SM타일)를 적용.
	VOID			SetStartViewTile(INT nX, INT nY);							// 그리는 뷰영역의 초기치세팅.

	BOOL			GetNextTileCanMove(INT nXPos, INT nYPos, BYTE bDir, INT nGap, POINT* lpptTarget);

/////////////////////////////////////////////////////////////////////////////////////////////////////
	VOID			GetScrnPosFromTilePos(SHORT shTileX, SHORT shTileY, SHORT& shScrnX, SHORT& shScrnY);
	VOID			GetTilePosFromScrnPos(SHORT shScrnX, SHORT shScrnY, SHORT& shTileX, SHORT& shTileY);

	BYTE			CalcDirection16(WORD wFireTileX, WORD wFireTileY, WORD wDestTileX, WORD wDestTileY);
	BYTE			CalcDirection8(WORD wFireTileX, WORD wFireTileY, WORD wDestTileX, WORD wDestTileY);


//  현재 타일에 대한 맵속성 얻기 함수들.
	BYTE			GetTileAttribute(INT nX, INT nY);
	BYTE			GetEventNum(INT nX, INT nY);
//	BOOL			GetDoorState(INT nX, INT nY);

	INT				GetDoor(INT nX, INT nY);
	VOID			OpenDoor(INT nX, INT nY, INT nIdx);
	VOID			CloseDoor(INT nX, INT nY, INT nIdx);
	INT				GetDoorImgIdx(INT nX, INT nY);
	BOOL			IsDoorOpen(INT nX, INT nY);

	LIGHTINFO		GetTileLightInfo(INT nX, INT nY);
//	VOID			SetDoorState(BYTE bIndex, BOOL bDoorOpen);
	VOID			SetAniTileFrame(INT nLoopTime);
};


#endif //_MAPHANDLER_
























//	VOID			DrawOneCellObjTile(INT nX, INT nY);
//	BOOL			DrawOneCellOverObjTile(INT nX, INT nY);

