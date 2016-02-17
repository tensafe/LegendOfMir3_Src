/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/


#ifndef _GAMEPROC_H
#define	_GAMEPROC_H



typedef struct tagMEMMAPPEDID
{
	CActor*			pxActor[_MAX_CELL_OBJ];
	CMagic*			pxMagic[_MAX_CELL_OBJ];
	LPGROUNDITEM	pstItem[_MAX_CELL_OBJ];
}MEMMAPPEDID, *LPMEMMAPPEDID;



class CGameProcess : public CWHDefProcess
{
protected:
	VOID	OnSvrMsgNewMap(_TDEFAULTMESSAGE	*ptdm, char *pszMapName);
	VOID	OnSvrMsgLogon(_TDEFAULTMESSAGE	*ptdm, char *pszMsg);
	VOID	OnSvrMsgHear(_TDEFAULTMESSAGE *ptdm, char *pszMsg);

public:
	 CGameProcess();
	~CGameProcess();
public:
	BOOL					m_bStatusView;
	BOOL					m_bTileView;
	BOOL					m_bCellView;
	BOOL					m_bObj1View;
	BOOL					m_bObj2View;
	BOOL					m_bStopPtView;
	BOOL					m_bShowSnow;
	BOOL					m_bShowMist;
	BOOL					m_bShowRain;
	BOOL					m_bShadowABlend;
	BOOL					m_bShowFPS;

	CHAR					m_szMapName[MAX_PATH];
	MEMMAPPEDID				m_stMemMappedID[_VIEW_CELL_Y_COUNT][_VIEW_CELL_X_COUNT];

	CPDLList<CActor>		m_xActorList;
	CPDLList<CMagic>		m_xMagicList;
	CPDLList<CMagic>		m_xGroundMagicList;
	CPDLList<GROUNDITEM>	m_stMapItemList;

	CBMMp3					m_xBGM;
	CParticle				m_xParticles;
	CImageHandler			m_xImage;
	CMapHandler				m_xMap;
	CInterface				m_xInterface;
	CMyHero					m_xMyHero;
	HBRUSH					m_hBrush;
	POINT					m_ptMousePos;
	POINT					m_ptCurrTile;
	BOOL					m_bRender;
	BYTE					m_bDayState;
	BYTE					m_bFogState;
	CLightFog				m_xLightFog;
	CMist					m_xMist;
	CSnow					m_xSnow;
	CRain					m_xRain;
	CFlyingTail				m_xFlyingTail;
	CSmoke					m_xSmoke;

	CElec					m_xElec;

	CNoticeBox				m_xNotice;

	CActor*					m_pxMouseTargetActor;
	CActor*					m_pxMouseOldTargetActor;
	CActor*					m_pxSavedTargetActor;

	CWHQueue				m_xWaitPacketQueue;	
	CWHQueue				m_xWaitDefaultPacketQueue;	

	WORD					m_wInputDelayTime;
	WORD					m_wCurrInputDelayTime;

	FLOAT					m_fDarkRate;

	FLOAT					m_fTileLightRed;
	FLOAT					m_fTileLightGreen;
	FLOAT					m_fTileLightBlue;

	WORD					m_wShadowClr[800];

	D3DMATRIX				m_matWorld;

	BOOL					m_bShowShine;	
	DWORD					m_dwLastShineTick;
	WORD					m_wShineCurrDelay;
	BYTE					m_bCurrShineFrame;
	LPDIRECTDRAWSURFACE7	m_lpddsShineTextr[10];		// 아이템 반짝거림.
	D3DVERTEX				m_avBillboard[4];
	BOOL					RenderShine(INT nLoopTime);

	WORD					m_wNPCWndPosX, m_wNPCWndPosY;

	virtual LRESULT DefMainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void				OnProcPacketNotEncode(char *pszMsg);

	void				OnSocketMessageRecieve(char *pszMsg);

	virtual VOID		OnConnectToServer();
	virtual char*		OnMessageReceive(CHAR* pszMessage);

	LRESULT OnKeyDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnKeyUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnMouseMove(WPARAM wParam, LPARAM lParam);
	LRESULT OnRButtonDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnRButtonUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnMove(WPARAM wParam, LPARAM lParam);
	LRESULT OnSysKeyDown(WPARAM wParam, LPARAM lParam);
	LRESULT OnSysKeyUp(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgInputted(WPARAM wParam, LPARAM lParam);
	LRESULT OnWheel(WPARAM wParam, LPARAM lParam);
	LRESULT OnLButtonDoubleClick(WPARAM wParam, LPARAM lParam);
	LRESULT OnTimer(WPARAM wParam, LPARAM lParam);
	LRESULT OnDestroy(WPARAM wParam, LPARAM lParam);

public:
	VOID	Init();
	VOID	Load();
	VOID	DeleteProc();
	VOID	SetNextProc();

	VOID	DropItemShow();
	VOID	LoadMapChanged(CHAR* szMap);

	VOID	AutoTargeting();

	BOOL	ReceivePacket(CHAR* szMsg, INT nPacketLen);
	VOID	RenderObject(INT nLoopTime);
	VOID	RenderScene(INT nLoopTime);

	BOOL	CheckMove(INT nX, INT nY, INT nSpeed, POINT* ptDestTile = NULL, INT* nDestDir = NULL);
	VOID	CheckMappedData(INT nLoopTime, BOOL bIsMoveTime);

	VOID	RenderMapTileGrid();
	VOID	CommandAnalyze();

	BOOL	TargetInLongAttack(BYTE bDir);

	VOID	ProcessPacket();
	VOID	ProcessDefaultPacket();

	VOID	CloseNPCWindow();

	virtual VOID ShowStatus(INT nStartX, INT nStartY);

	CActor*	FindActor(int nID); // gadget

};



#endif //_GAMEPROC_H