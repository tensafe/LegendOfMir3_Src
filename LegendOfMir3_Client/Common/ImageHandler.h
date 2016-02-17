/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/



#ifndef _CIMAGEHANDLER_H
#define _CIMAGEHANDLER_H



class CImageHandler
{
public:
	CImageHandler();
	~CImageHandler();

private:
protected:
public:
	CWHWilImageData			m_xImageList[_MAX_IMAGE];

	CPDLList<TEXTUREFILE>	m_xTextrFileList[_MAX_TEXTR_FILE];

	INT						m_nLoadedEffect[_MAX_EFFECT];
	INT						m_nLoadedMagic[_MAX_MAGIC];
	INT						m_nLoadedExplode[_MAX_EXPLOSION];

	D3DVERTEX				m_avBillBoard[4];	

public:
	VOID InitAllImage();
	VOID LoadAllImage(BOOL bIsMemMapped = TRUE, BOOL bComp = TRUE);
	VOID DeleteAllImage();

	__inline VOID FreeLoadedMagic(BYTE bSkillNum)
	{
		for ( INT nCnt = 0; nCnt < _MAX_EFFECT; nCnt++ )
		{
			if ( m_nLoadedEffect[nCnt] == bSkillNum )
			{
				m_nLoadedEffect[nCnt] = 0;
			}
		}
	}

	VOID AddTextr(WORD wFileType, WORD wFileIdx, WORD wImgIdx);
	LPDIRECTDRAWSURFACE7 GetTextrImg(WORD wFileType, WORD wFileIdx, WORD wImgIdx);
	VOID DelTextr(WORD wFileType, WORD wFileIdx, WORD wImgIdx);
	VOID DelAllTextrFileList();
	
	HRESULT DrawBillBoard(LPDIRECT3DDEVICE7 lpDevice, D3DVECTOR vTrans, D3DVECTOR vScale, /*D3DVECTOR vRot, */D3DMATERIAL7 mtrl, /*FLOAT fRotRad, */LPDIRECTDRAWSURFACE7 lpddsTextr);
	
	INT CheckList(INT* pnList, INT nNumOfList, BYTE bCheck)
	{
		for ( INT nCnt = 0; nCnt < nNumOfList; nCnt++ )
		{
			if ( pnList[nCnt] == bCheck )
				return nCnt;
		}

		return -1;
	}

	VOID SetLoad(INT* pnList, INT nNumOfList, BYTE bSkillNum, BOOL bFlag)  
	{ 
		INT nIndex;
		
		if ( bFlag )
			nIndex = CheckList(pnList, nNumOfList, 0);
		else
			nIndex = CheckList(pnList, nNumOfList, bSkillNum);

		if ( nIndex >= 0 && nIndex < nNumOfList )
		{
			if ( bFlag )
				m_nLoadedEffect[nIndex] = bSkillNum;
			else
				m_nLoadedEffect[nIndex] = 0;
		}
	}

	__inline void SetEffectLoaded(BYTE bSkillNum)
	{	SetLoad(m_nLoadedEffect, _MAX_EFFECT, bSkillNum, TRUE);		}
	__inline FreeEffectLoaded(BYTE bSkillNum)
	{	SetLoad(m_nLoadedEffect, _MAX_EFFECT, bSkillNum, FALSE);	}
	__inline int EffectLoaded(BYTE bSkillNum)
	{	return CheckList(m_nLoadedEffect, _MAX_EFFECT, bSkillNum);	}

	__inline void SetMagicLoaded(BYTE bSkillNum)
	{	SetLoad(m_nLoadedMagic, _MAX_MAGIC, bSkillNum, TRUE);		}
	__inline FreeMagicLoaded(BYTE bSkillNum)
	{	SetLoad(m_nLoadedMagic, _MAX_MAGIC, bSkillNum, FALSE);		}
	__inline int MagicLoaded(BYTE bSkillNum)
	{	return CheckList(m_nLoadedMagic, _MAX_MAGIC, bSkillNum);	}

	__inline void SetExplodeLoaded(BYTE bSkillNum)
	{	SetLoad(m_nLoadedExplode, _MAX_EXPLOSION, bSkillNum, TRUE); }
	__inline FreeExplodeLoaded(BYTE bSkillNum)
	{	SetLoad(m_nLoadedExplode, _MAX_EXPLOSION, bSkillNum, FALSE);}
	__inline int ExplodeLoaded(BYTE bSkillNum)
	{	return CheckList(m_nLoadedExplode,_MAX_EXPLOSION,bSkillNum);}
};



#endif // _CIMAGEHANDLER_H