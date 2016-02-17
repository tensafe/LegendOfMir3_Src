/******************************************************************************************************************
                                                                                                                   
	모듈명:																											
																													
	작성자:																											
	작성일:																											
																													
	[일자][수정자] : 수정 내용																						
                                                                                                                   
*******************************************************************************************************************/



#include "StdAfx.h"



/******************************************************************************************************************

	CImageHandler Class

*******************************************************************************************************************/
/******************************************************************************************************************

	함수명 : CImageHandler::CImageHandler()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
CImageHandler::CImageHandler()
{
	INT		nCnt;

	ZeroMemory(m_xImageList, sizeof(CWHWilImageData)*_MAX_IMAGE);

	ZeroMemory(m_nLoadedMagic, sizeof(INT)*_MAX_MAGIC);

	for ( nCnt = 0; nCnt < _MAX_TEXTR_FILE; nCnt++ )
	{
		m_xTextrFileList[nCnt].ClearAllNodes();
	}
}



/******************************************************************************************************************

	함수명 : CImageHandler::~CImageHandler()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
CImageHandler::~CImageHandler()
{
	DeleteAllImage();
}



/******************************************************************************************************************

	함수명 : CImageHandler::LoadAllImage()

	작성자 : 
	작성일 : 

	목적   : 
	입력   : BOOL bIsMemMapped
	         BOOL bComp
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CImageHandler::LoadAllImage(BOOL bIsMemMapped, BOOL bComp)
{
	/* IMAGE DEFINE문과 일치해야한다.--------------------------------------------------------------*/
	CHAR* szFileList[] = 
	{
		//~~~ 배경이미지. ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		".\\Data\\tilesc.wil",								//0
		".\\Data\\tiles30c.wil",							//1
		".\\Data\\Tiles5c.wil",								//2
		".\\Data\\smtilesc.wil",							//3
		".\\Data\\housesc.wil",								//4
		".\\Data\\cliffsc.wil",								//5
		".\\Data\\dungeonsc.wil",							//6
		".\\Data\\innersc.wil",								//7
		".\\Data\\furnituresc.wil",							//8
		".\\Data\\wallsc.wil",								//9
		".\\Data\\smobjectsc.wil",							//10
		".\\Data\\animationsc.wil",							//11
		".\\Data\\object1c.wil",							//12
		".\\Data\\object2c.wil",							//13

		//~~~ 인터페이스이미지. ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		".\\Data\\GameInter.wil",							//14 

		//~~~ 남자이미지. ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		".\\Data\\M-Hum.wil",								//15 	
		".\\Data\\M-Hair.wil",								//16 
		".\\Data\\M-Weapon1.wil",							//17
		".\\Data\\M-Weapon2.wil",							//18
		".\\Data\\M-Weapon3.wil",							//19
		".\\Data\\M-Weapon4.wil",							//20

		//~~~ 여자이미지. ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		".\\Data\\WM-Hum.wil",								//21
		".\\Data\\WM-Hair.wil",								//22
		".\\Data\\WM-Weapon1.wil",							//23
		".\\Data\\WM-Weapon2.wil",							//24
		".\\Data\\WM-Weapon3.wil",							//25
		".\\Data\\WM-Weapon4.wil",							//26

		//~~~ 아이템 및 인터페이스, 마법, 기타이미지. ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		".\\Data\\Magic.wil",								//27
		".\\Data\\Inventory.wil",							//29
		".\\Data\\Equip.wil",								//29
		".\\Data\\Ground.wil",								//30
		".\\Data\\MIcon.wil",								//31
		".\\Data\\ProgUse.wil",								//32
		".\\Data\\Horse.wil",								//33

		//~~~ 몬스터이미지. ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		".\\Data\\Mon-1.wil",								//34
		".\\Data\\Mon-2.wil",								//35
		".\\Data\\Mon-3.wil",								//36
		".\\Data\\Mon-4.wil",								//37
		".\\Data\\Mon-5.wil",								//38
		".\\Data\\Mon-6.wil",								//39
		".\\Data\\Mon-7.wil",								//40
		".\\Data\\Mon-8.wil",								//41
		".\\Data\\Mon-9.wil",								//42
		".\\Data\\Mon-10.wil",								//43
		".\\Data\\Mon-11.wil",								//44
		".\\Data\\Mon-12.wil",								//45

		//~~~ 몬스터그림자이미지. ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		".\\Data\\MonS-1.wil",								//46
		".\\Data\\MonS-2.wil",								//47
		".\\Data\\MonS-3.wil",								//48
		".\\Data\\MonS-4.wil",								//49
		".\\Data\\MonS-5.wil",								//40
		".\\Data\\MonS-6.wil",								//51
		".\\Data\\MonS-7.wil",								//52
		".\\Data\\MonS-8.wil",								//53
		".\\Data\\MonS-9.wil",								//54
		".\\Data\\MonS-10.wil",								//55
		".\\Data\\MonS-11.wil",								//56
		".\\Data\\MonS-12.wil",								//57

		//~~~ NPC 이미지. ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		".\\Data\\NPC.wil",									//58

		//~~~ 몬스터마법 이미지. ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		".\\Data\\MonMagic.wil",							//59

		//~~~ 인터페이스EX 이미지. ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
		".\\Data\\interfacec2.wil",							//60
	};

	InitAllImage();

	for ( INT nCnt = 0; nCnt < _MAX_IMAGE; nCnt++ )
		m_xImageList[nCnt].NewLoad(szFileList[nCnt], bIsMemMapped, bComp);
}


VOID CImageHandler::InitAllImage()
{
	for ( INT nCnt = 0; nCnt < _MAX_IMAGE; nCnt++ )
		m_xImageList[nCnt].Init();

	ZeroMemory(m_nLoadedMagic, sizeof(INT)*_MAX_MAGIC);

 	D3DVECTOR vNorm(0, 0, -1);
	m_avBillBoard[0]  = D3DVERTEX(D3DVECTOR(-0.5f, 0.5f, 0), vNorm, 0, 0);
	m_avBillBoard[1]  = D3DVERTEX(D3DVECTOR(-0.5f,-0.5f, 0), vNorm, 0, 1);
	m_avBillBoard[2]  = D3DVERTEX(D3DVECTOR( 0.5f, 0.5f, 0), vNorm, 1, 0);
	m_avBillBoard[3]  = D3DVERTEX(D3DVECTOR( 0.5f,-0.5f, 0), vNorm, 1, 1);

	for ( nCnt = 0; nCnt < _MAX_TEXTR_FILE; nCnt++ )
	{
		m_xTextrFileList[nCnt].ClearAllNodes();
	}
}


/******************************************************************************************************************

	함수명 : CImageHandler::DeleteAllImage()

	작성자 : 
	작성일 : 

	목적   : 
	출력   : VOID 

	[일자][수정자] : 수정내용

*******************************************************************************************************************/
VOID CImageHandler::DeleteAllImage()
{
	INT nCnt;

	for ( nCnt = 0; nCnt < _MAX_IMAGE; nCnt++ )
		m_xImageList[nCnt].Destroy();

	ZeroMemory(m_nLoadedMagic, sizeof(INT)*_MAX_MAGIC);

	for ( nCnt = 0; nCnt < _MAX_TEXTR_FILE; nCnt++ )
	{
		m_xTextrFileList[nCnt].ClearAllNodes();
	}
}





VOID CImageHandler::AddTextr(WORD wFileType, WORD wFileIdx, WORD wImgIdx)
{
	if ( wFileType < _MAX_TEXTR_FILE )
	{
		if ( m_xImageList[wFileIdx].NewSetIndex(wImgIdx) )
		{
			if ( D3DWILTextr_CreateEmptyTexture(m_xImageList[wFileIdx].m_szWilFileName, wImgIdx, 
											    m_xImageList[wFileIdx].m_lpstNewCurrWilImageInfo->shWidth,
												m_xImageList[wFileIdx].m_lpstNewCurrWilImageInfo->shHeight,
												(WORD*)m_xImageList[wFileIdx].m_pbCurrImage, D3DTEXTR_TRANSPARENTBLACK) )
			{
				LPTEXTUREFILE pstTexFile = new TEXTUREFILE;
				pstTexFile->wFileIdx	 = wFileIdx;
				pstTexFile->wImgIdx		 = wImgIdx;
				pstTexFile->lpddsTextr	 = D3DWILTextr_RestoreEx(m_xImageList[wFileIdx].m_szWilFileName, wImgIdx, g_xMainWnd.Get3DDevice());

				m_xTextrFileList[wFileType].AddNode(pstTexFile);

				return;
			}
		}		
	}
}

VOID CImageHandler::DelTextr(WORD wFileType, WORD wFileIdx, WORD wImgIdx)
{
	if ( wFileType < _MAX_TEXTR_FILE )
	{
		D3DWILTextr_Invalidate(m_xImageList[wFileIdx].m_szWilFileName, wImgIdx);
		D3DWILTextr_DestroyTexture(m_xImageList[wFileIdx].m_szWilFileName, wImgIdx);

		m_xTextrFileList[wFileType].MoveCurrentToTop();
		for ( INT nCnt = 0; nCnt < m_xTextrFileList[wFileType].GetCounter(); nCnt++ )
		{
			LPTEXTUREFILE pstTexFile = m_xTextrFileList[wFileType].GetCurrentData();

			if ( pstTexFile->wFileIdx == wFileIdx && pstTexFile->wImgIdx == wImgIdx )
			{
				m_xTextrFileList[wFileType].DeleteCurrentNode();
				return;
			}

			m_xTextrFileList[wFileType].MoveNextNode();
		}
	}
}

LPDIRECTDRAWSURFACE7 CImageHandler::GetTextrImg(WORD wFileType, WORD wFileIdx, WORD wImgIdx)
{
	if ( wFileType < _MAX_TEXTR_FILE )
	{
		m_xTextrFileList[wFileType].MoveCurrentToTop();
		for ( INT nCnt = 0; nCnt < m_xTextrFileList[wFileType].GetCounter(); nCnt++ )
		{
			LPTEXTUREFILE pstTexFile = m_xTextrFileList[wFileType].GetCurrentData();

			if ( pstTexFile->wFileIdx == wFileIdx && pstTexFile->wImgIdx == wImgIdx )
			{
				return pstTexFile->lpddsTextr;				
			}

			m_xTextrFileList[wFileType].MoveNextNode();
		}
	}

	return NULL;
}

VOID CImageHandler::DelAllTextrFileList()
{
	INT		nCnt;
	for ( nCnt = 0; nCnt < _MAX_TEXTR_FILE; nCnt++ )
	{
		m_xTextrFileList[nCnt].ClearAllNodes();
	}
}



HRESULT CImageHandler::DrawBillBoard(LPDIRECT3DDEVICE7 lpDevice, D3DVECTOR vTrans, D3DVECTOR vScale, /*D3DVECTOR vRot, */D3DMATERIAL7 mtrl, /*FLOAT fRotRad, */LPDIRECTDRAWSURFACE7 lpddsTextr)
{
	if ( lpDevice )
	{
		if( SUCCEEDED(lpDevice->BeginScene()) )
		{
			D3DMATRIX matTrans;
			D3DMATRIX matScale;
			D3DMATRIX matRot;

			D3DMATRIX matWorld;
			D3DMATRIX matTempWorld;

			D3DMATRIX matWorldOriginal;

			lpDevice->GetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

			vTrans.x = vTrans.x+vScale.x/2-400;
			vTrans.y = -vTrans.y-vScale.y/2+300;

			D3DUtil_SetTranslateMatrix(matTrans, vTrans);
			D3DUtil_SetScaleMatrix(matScale, vScale.x, vScale.y, vScale.z);
//			D3DUtil_SetRotationMatrix(matRot, vRot, fRotRad);
			D3DMath_MatrixMultiply(/*matTempWorld*/matWorld, matScale, matTrans);
//			D3DMath_MatrixMultiply(matWorld, matRot, matTempWorld);
			lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);

			SetBlendRenderState(lpDevice, _BLEND_NORMAL, mtrl);
			lpDevice->SetMaterial(&mtrl);

			lpDevice->SetTexture(0, lpddsTextr);

			lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX, m_avBillBoard, 4, NULL);

			// 원상복귀.
			ZeroMemory(&mtrl, sizeof(mtrl));
			mtrl.diffuse.r = mtrl.diffuse.g = mtrl.diffuse.b = 0.1f;
			mtrl.ambient.r = mtrl.ambient.g = mtrl.ambient.b = 1.0f;
			lpDevice->SetMaterial(&mtrl);

			ResetBlendenderState(lpDevice);
			lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);
		}
		lpDevice->EndScene();
		return S_OK;
	}
	return E_FAIL;
}
