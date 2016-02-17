#ifndef _WINDHORN_SURFACE
#define	_WINDHORN_SURFACE


class CWHSurface
{
protected:
	LPDIRECTDRAWSURFACE7	m_pDDS;

public:
	__inline CWHSurface()	{ m_pDDS = NULL; }
	__inline CWHSurface(LPDIRECTDRAW7 pDD, DWORD dwXSize, DWORD dwYSize) { CreateOffScreenSurface(pDD, dwXSize, dwYSize); }
	__inline ~CWHSurface()	{ ReleaseSurface(); }

public:
	BOOL		CreateOffScreenSurface(LPDIRECTDRAW7 pDD, DWORD dwXSize, DWORD dwYSize);
	BOOL		CopyImageToSurface(DWORD dwSrcWidth, DWORD dwSrcHeight, WORD* pwSrc);
	BOOL		ReleaseSurface();

	__inline LPDIRECTDRAWSURFACE7 GetSurface()	{ return m_pDDS; }
	__inline BOOL CopyImageToSurface(CWHImage* xImage) { return CopyImageToSurface(xImage->m_wWidth, xImage->m_wHeight, xImage->m_pwImage); }
};



#endif //_WINDHORN_SURFACE
