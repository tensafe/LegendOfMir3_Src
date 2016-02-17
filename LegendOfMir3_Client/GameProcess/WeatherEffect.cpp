#include "StdAfx.h"


CMist::CMist()
{
 	D3DVECTOR vNorm(0, 0, -1);
    m_avBillboard[0] = D3DVERTEX(D3DVECTOR(-400.0f, 300.0f, 0), vNorm, 0, 0);
    m_avBillboard[1] = D3DVERTEX(D3DVECTOR(-400.0f,-300.0f, 0), vNorm, 0, 1);
    m_avBillboard[2] = D3DVERTEX(D3DVECTOR( 400.0f, 300.0f, 0), vNorm, 1, 0);
    m_avBillboard[3] = D3DVERTEX(D3DVECTOR( 400.0f,-300.0f, 0), vNorm, 1, 1);

	m_vTrans.x = 0;
	m_vTrans.y = 0;
	m_vTrans.z = 0;
}

CMist::~CMist()
{
}

BOOL CMist::Create()
{
	D3DTextr_CreateTextureFromFile("mist.bmp");
	D3DTextr_Restore("mist.bmp", g_xMainWnd.Get3DDevice());
	return FALSE;
}

VOID CMist::Init()
{
}

VOID CMist::Destory()
{
}

HRESULT CMist::DrawMist()
{
	if ( g_xMainWnd.Get3DDevice() )
	{
		D3DMATRIX matWorldOriginal;
		D3DMATRIX matTrans;
		D3DMATRIX matScale;
		D3DMATRIX matWorld;

		if( SUCCEEDED(g_xMainWnd.Get3DDevice()->BeginScene()) )
		{
			g_xMainWnd.Get3DDevice()->GetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

			static int nCnt = 0;

			nCnt++;

			if ( nCnt < 800 )
			{
 				m_vTrans.x -= 1.9f; 
				m_vTrans.y -= 0.2f; 
				m_vTrans.z = (FLOAT)0;
			}
			else if ( nCnt >= 800 &&  nCnt < 1600 )
			{
 				m_vTrans.x += 1.9f; 
				m_vTrans.y += 0.2f; 
				m_vTrans.z = (FLOAT)0;
			}
			else
			{
				nCnt = 0;
			}

			D3DUtil_SetTranslateMatrix(matTrans, m_vTrans);
			D3DUtil_SetScaleMatrix(matScale, 5.0f, 2.0f, 0.0f);
			D3DMath_MatrixMultiply(matWorld, matScale, matTrans);
			g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);

			D3DMATERIAL7 mtrl;
			D3DUtil_InitMaterial(mtrl, 
								 (FLOAT)100/255.0f,
								 (FLOAT)200/255.0f, 
								 (FLOAT)200/255.0f);
			mtrl.diffuse.a = 1/255.0f;
			g_xMainWnd.Get3DDevice()->SetMaterial(&mtrl);

			SetBlendRenderState(g_xMainWnd.Get3DDevice(), 2, mtrl);

			g_xMainWnd.Get3DDevice()->SetTexture(0, D3DTextr_GetSurface("mist.bmp"));
			g_xMainWnd.Get3DDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX, m_avBillboard, 4, NULL);

			g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

			ResetBlendenderState(g_xMainWnd.Get3DDevice());
			g_xMainWnd.Get3DDevice()->EndScene();
			return S_OK;
		}
	}
	return E_FAIL;
}

VOID CMist::ProgressMist()
{
	DrawMist();
}






/*
CRain::CRain()
{
 	D3DVECTOR vNorm(0, 0, -1);
	m_avBillboard[0] = D3DVERTEX( D3DVECTOR(-0.5f,-5.0f, 0.0f ), vNorm, 0.0f, 1.0f );
    m_avBillboard[1] = D3DVERTEX( D3DVECTOR( 0.5f,-5.0f, 0.0f ), vNorm, 0.0f, 0.0f );
    m_avBillboard[2] = D3DVERTEX( D3DVECTOR(-0.5f, 5.0f, 0.0f ), vNorm, 1.0f, 1.0f );
    m_avBillboard[3] = D3DVERTEX( D3DVECTOR( 0.5f, 5.0f, 0.0f ), vNorm, 1.0f, 0.0f );
}

CRain::~CRain()
{
}

BOOL CRain::Create()
{
//	D3DTextr_CreateTextureFromFile("snow.bmp");
//	D3DTextr_Restore("snow.bmp", g_xMainWnd.Get3DDevice());
	m_xParticles.m_rParticleWind = 0.03f;
	m_xParticles.m_rParticleGravity = 0.25f;
	return FALSE;
}

VOID CRain::Init()
{
}

VOID CRain::Destory()
{
}

HRESULT CRain::DrawRain()
{
	if ( g_xMainWnd.Get3DDevice() )
	{
		D3DMATRIX matWorldOriginal;
		D3DMATRIX matTrans;
		D3DMATRIX matScale;
		D3DMATRIX matWorld;

		D3DVECTOR vTrans;

		m_xParticles.SetRain();

		if( SUCCEEDED(g_xMainWnd.Get3DDevice()->BeginScene()) )
		{
			g_xMainWnd.Get3DDevice()->GetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

			m_xParticles.ProcessParticles();

			D3DMATERIAL7 mtrl;
			D3DUtil_InitMaterial(mtrl, 
								 (FLOAT)150/255.0f,
								 (FLOAT)150/255.0f, 
								 (FLOAT)150/255.0f);
			mtrl.diffuse.a = 10/255.0f;
			g_xMainWnd.Get3DDevice()->SetMaterial(&mtrl);

			SetBlendRenderState(g_xMainWnd.Get3DDevice(), 0, mtrl);

			for ( INT nCnt = 0; nCnt < MAX_PARTICLES; nCnt++ )
			{
				if ( m_xParticles.m_stParticles[nCnt].nState == PARTICLE_STATE_ALIVE )
				{
  					vTrans.x = (FLOAT)m_xParticles.m_stParticles[nCnt].rX-400; 
					vTrans.y = (FLOAT)-m_xParticles.m_stParticles[nCnt].rY+300; 
					vTrans.z = 0;
					FLOAT rSize = (FLOAT)(10+(nCnt%20));
					D3DUtil_SetScaleMatrix(matScale, 1, 1, 0.0f);
					D3DUtil_SetTranslateMatrix(matTrans, vTrans);
					D3DMath_MatrixMultiply(matWorld, matScale, matTrans);
					g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);
//					g_xMainWnd.Get3DDevice()->SetTexture(0, D3DTextr_GetSurface("snow.bmp"));
					g_xMainWnd.Get3DDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX, m_avBillboard, 4, NULL);
					g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);
				}
			}
			ResetBlendenderState(g_xMainWnd.Get3DDevice());
			g_xMainWnd.Get3DDevice()->EndScene();
			return S_OK;
		}
	}
	return E_FAIL;
}


VOID CRain::ProgressRain()
{
	DrawRain();
}












CSnow::CSnow()
{
 	D3DVECTOR vNorm(0, 0, -1);
	m_avBillboard[0] = D3DVERTEX( D3DVECTOR(-2.0f,-2.0f, 0.0f ), vNorm, 0.0f, 1.0f );
    m_avBillboard[1] = D3DVERTEX( D3DVECTOR(-2.0f, 2.0f, 0.0f ), vNorm, 0.0f, 0.0f );
    m_avBillboard[2] = D3DVERTEX( D3DVECTOR( 2.0f,-2.0f, 0.0f ), vNorm, 1.0f, 1.0f );
    m_avBillboard[3] = D3DVERTEX( D3DVECTOR( 2.0f, 2.0f, 0.0f ), vNorm, 1.0f, 0.0f );
}

CSnow::~CSnow()
{
}

BOOL CSnow::Create()
{
	D3DTextr_CreateTextureFromFile("snow.bmp");
	D3DTextr_Restore("snow.bmp", g_xMainWnd.Get3DDevice());
	return FALSE;
}

VOID CSnow::Init()
{
}

VOID CSnow::Destory()
{
}

HRESULT CSnow::DrawSnow()
{
	if ( g_xMainWnd.Get3DDevice() )
	{
		D3DMATRIX matWorldOriginal;
		D3DMATRIX matTrans;
		D3DMATRIX matScale;
		D3DMATRIX matWorld;

		D3DVECTOR vTrans;

		m_xParticles.SetSnow();

		if( SUCCEEDED(g_xMainWnd.Get3DDevice()->BeginScene()) )
		{
			g_xMainWnd.Get3DDevice()->GetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);

			m_xParticles.ProcessParticles();

			D3DMATERIAL7 mtrl;
			D3DUtil_InitMaterial(mtrl, 
								 (FLOAT)255/255.0f,
								 (FLOAT)255/255.0f, 
								 (FLOAT)255/255.0f);
			mtrl.diffuse.a = 1/255.0f;
			g_xMainWnd.Get3DDevice()->SetMaterial(&mtrl);
			SetBlendRenderState(g_xMainWnd.Get3DDevice(), _BLEND_NORMAL, mtrl);

			for ( INT nCnt = 0; nCnt < MAX_PARTICLES; nCnt++ )
			{
				if ( m_xParticles.m_stParticles[nCnt].nState == PARTICLE_STATE_ALIVE )
				{

  					vTrans.x = (FLOAT)m_xParticles.m_stParticles[nCnt].rX-400; 
					vTrans.y = (FLOAT)-m_xParticles.m_stParticles[nCnt].rY+300; 
					vTrans.z = 0;
					FLOAT rSize = (FLOAT)(10+(nCnt%20));
					D3DUtil_SetScaleMatrix(matScale, rSize/22.0f, rSize/22.0f, 0.0f);
					D3DUtil_SetTranslateMatrix(matTrans, vTrans);
					D3DMath_MatrixMultiply(matWorld, matScale, matTrans);
					g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);
					g_xMainWnd.Get3DDevice()->SetTexture(0, D3DTextr_GetSurface("snow.bmp"));
					g_xMainWnd.Get3DDevice()->DrawPrimitive(D3DPT_TRIANGLESTRIP, D3DFVF_VERTEX, m_avBillboard, 4, NULL);
					g_xMainWnd.Get3DDevice()->SetTransform(D3DTRANSFORMSTATE_WORLD, &matWorldOriginal);
				}
			}
			ResetBlendenderState(g_xMainWnd.Get3DDevice());
			g_xMainWnd.Get3DDevice()->EndScene();
			return S_OK;
		}
	}
	return E_FAIL;
}


VOID CSnow::ProgressSnow()
{
	DrawSnow();
}
*/





SHORT	m_shWaveMap[2][_MAX_X*_MAX_Y];

CAirWave::CAirWave()
{
}

CAirWave::~CAirWave()
{
}

VOID CAirWave::InitAirWave()
{
	SHORT shTmp;
	for ( SHORT shCnt = -256; shCnt <= 255; shCnt++ )
	{	
		shTmp = shCnt / 4;
		m_bDispLut[shCnt+256] = (BYTE)(tan(asin((sin(atan((double)shCnt))/4.0f)))*shTmp);
	}
	ZeroMemory(m_shWaveMap, sizeof(WORD)*_MAX_X*_MAX_Y*2);

	m_bCurr = _CURR;
	m_bNew  = _NEW;

}

VOID CAirWave::UpdateAirWave()
{
	SHORT shWaveHeight;
	for ( INT y = 2; y <= _MAX_Y-2; y++ )
		for ( INT x = 2; x <= _MAX_X-2; x++ )
		{
			shWaveHeight = (
			m_shWaveMap[m_bCurr][x-1 + _MAX_X *  y] +
			m_shWaveMap[m_bCurr][x-2 + _MAX_X *  y] +
			m_shWaveMap[m_bCurr][x+1 + _MAX_X *  y] +
			m_shWaveMap[m_bCurr][x+2 + _MAX_X *  y] +
			m_shWaveMap[m_bCurr][x   + _MAX_X * (y-1)] +
			m_shWaveMap[m_bCurr][x   + _MAX_X * (y-2)] +
			m_shWaveMap[m_bCurr][x   + _MAX_X * (y+1)] +
			m_shWaveMap[m_bCurr][x   + _MAX_X * (y+2)] +
			m_shWaveMap[m_bCurr][x-1 + _MAX_X * (y-1)] +
			m_shWaveMap[m_bCurr][x+1 + _MAX_X * (y-1)] +
			m_shWaveMap[m_bCurr][x-1 + _MAX_X * (y+1)] +
			m_shWaveMap[m_bCurr][x+1 + _MAX_X * (y+1)]
			) / 6 - m_shWaveMap[m_bNew][x + _MAX_X * y];

//			shWaveHeight = shWaveHeight - (shWaveHeight/_DAMP);
			
			_asm
			{
				push bx
				mov bx, shWaveHeight
				sar bx, _DAMP
				sub shWaveHeight, bx
				pop bx
			}

			m_shWaveMap[m_bNew][x + _MAX_X * y] = shWaveHeight;
		}
}


VOID CAirWave::DropAirWaveMap(INT nX, INT nY, INT nW, INT nMulFactor)
{
	INT nSqrX, nSqrY, nSqrW;

	nSqrW = nW*nW;

	if ( (nX > nW) && (nX < _MAX_X-nW) && (nY > nW) && (nY < _MAX_Y-nW) )
	{
		for ( INT nV = nY-nW; nV <= nY+nW; nV++ )
		{
			nSqrY = (nV-nY)*(nV-nY);
			for ( INT nU = nX-nW; nU <= nX+nW; nU++ )
			{
				nSqrX = (nU-nX)*(nU-nX);

				if ( (nSqrX+nSqrY) <= nSqrW )
				{
					INT nTmpValue = nW-(INT)(sqrt((double)(nSqrX+nSqrY)));
					m_shWaveMap[m_bCurr][nU + _MAX_X * nV] = (nMulFactor*nTmpValue);
				}
			}
		}
	}
}


VOID CAirWave::RenderAirWave()
{
	DDSURFACEDESC2	ddsd;
	WORD  wColor;
	SHORT shXDiff, shYDiff, shXDisp, shYDisp;


	if ( g_xMainWnd.GetBackBuffer() != NULL )
	{
		ddsd.dwSize	= sizeof(DDSURFACEDESC2);
		ddsd.lpSurface = NULL;
			
		g_xMainWnd.GetBackBuffer()->Lock(NULL, &ddsd, DDLOCK_WAIT, NULL);
		
		if ( !ddsd.lpSurface )		return;
		
		WORD* pwdDst;
		pwdDst = (WORD*)ddsd.lpSurface;

		for ( INT y=0 ; y < _MAX_Y; y++ )
		{
			for ( INT x=0 ; x < _MAX_X; x++ )
			{
				shXDiff = m_shWaveMap[m_bNew][(x+1) + _MAX_X * y] - m_shWaveMap[m_bNew][x + _MAX_X * y];
				shYDiff = m_shWaveMap[m_bNew][x + _MAX_X * (y+1)] - m_shWaveMap[m_bNew][x + _MAX_X * y];
				shXDisp = m_bDispLut[shXDiff+256];
				shYDisp = m_bDispLut[shYDiff+256];

				if ( shXDiff < 0 )
				{
					if ( shYDiff < 0 )
					  wColor = pwdDst[((y-shYDisp) * (ddsd.lPitch >> 1)) + (x-shXDisp)];
					else
					  wColor = pwdDst[((y+shYDisp) * (ddsd.lPitch >> 1)) + (x-shXDisp)];
				}
				else
				{
					if ( shYDiff < 0 )
					  wColor = pwdDst[((y-shYDisp) * (ddsd.lPitch >> 1)) + (x+shXDisp)];
					else
					  wColor = pwdDst[((y+shYDisp) * (ddsd.lPitch >> 1)) + (x+shXDisp)];
				}
				pwdDst[(y * (ddsd.lPitch >> 1)) + x] = wColor;
			}
		}

		BYTE bTmp = m_bCurr;
		m_bCurr = m_bNew;
		m_bNew = bTmp;

		g_xMainWnd.GetBackBuffer()->Unlock(NULL);

		return;
	}
}



/*
procedure TfrmDxEffect.InitWaveMap;
var x,y: SmallInt;
begin
  for y:=0 to MAXY do begin
    for x:=0 to MAXX do begin
      WaveMap[CT,x,y] := 0;
      WaveMap[NW,x,y] := 0;
    end;
  end;
end;



procedure TfrmDxEffect.UpdateWaveMap;
var
  x,y,n: Smallint;
begin
  (* ------------------------------------------------------------------------ *)
  (* Calculate the next frame of our waterwaves at time t                     *)
  (*                                                                          *)
  (* The height of the wave at (x, y) is determined by adding the heights of  *)
  (* the waves located above, below, to the left and to the right of (x, y).  *)
  (* The sum is then divided by two, giving us a waveheight roughly twice     *)
  (* as high as the rest.                                                     *)
  (*                                                                          *)
  (* Here comes the key to wave animation: from our total, we subtract the    *)
  (* height of the wave at (x, y) at time t-1.                                *)
  (*                                                                          *)
  (* If the previous wave was higher than average, the wave will drop.        *)
  (* And if the previous wave was lower than average, the wave will rise.     *)
  (*                                                                          *)
  (* Check out these sketches:                                                *)
  (*                                                                          *)
  (*   ****                        ****                                       *)
  (*   ----*** -----------------***----------- average height;                *)
  (*          ****          ****               the wave will rise             *)
  (*              **********                                                  *)
  (*                                                                          *)
  (*                                                                          *)
  (*              **********                                                  *)
  (*   -------****----------****-------------- average height;                *)
  (*       ***                  ***            the wave will drop             *)
  (*   ****                         ****                                      *)
  (*                                                                          *)
  (* While moving up and down, the wave loses energy. This is simulated by    *)
  (* using a damping factor. This factor depends on the height of the wave,   *)
  (* so that high waves lose energy fast, and low waves lose energy slowly:   *)
  (*                                                                          *)
  (*   wave_height = wave_height - (wave_height / damp_factor)                *)
  (* ------------------------------------------------------------------------ *)
  for y:=2 to MAXY-2 do begin
    for x:=2 to MAXX-2 do begin
      n:=( WaveMap[CT,x-1,y] +
           WaveMap[CT,x-2,y] +
           WaveMap[CT,x+1,y] +
           WaveMap[CT,x+2,y] +
           WaveMap[CT,x,y-1] +
           WaveMap[CT,x,y-2] +
           WaveMap[CT,x,y+1] +
           WaveMap[CT,x,y+2] +
           WaveMap[CT,x-1,y-1] +
           WaveMap[CT,x+1,y-1] +
           WaveMap[CT,x-1,y+1] +
           WaveMap[CT,x+1,y+1]
           ) div 6 - WaveMap[NW,x,y];
      asm
        push bx
        mov bx, n
        sar bx, DAMP
        sub n, bx
        pop bx
      end;
      WaveMap[NW,x,y] := n;  // Store result
    end;
  end;
end;


procedure TfrmDxEffect.RenderWaveToDIB(DIB: TDIB);
var
  x,y,newcolor,xDiff,yDiff,xDisp,yDisp: Smallint;
begin
  for y:=0 to MAXY do begin
    for x:=0 to MAXX do begin
      xDiff := WaveMap[NW,x+1,y] - WaveMap[NW,x,y];
      yDiff := WaveMap[NW,x,y+1] - WaveMap[NW,x,y];
      xDisp := dispLut[xDiff+256];
      yDisp := dispLut[yDiff+256];

      if xDiff<0 then begin
        // Current position is higher - Clockwise rotation
        if (yDiff<0) then
          newcolor := Background.DIB.Pixels[x-xDisp,y-yDisp]
        else
          newcolor := Background.DIB.Pixels[x-xDisp,y+yDisp]
      end else begin
        if (yDiff<0) then
          newcolor := Background.DIB.Pixels[x+xDisp,y-yDisp]
        else
          newcolor := Background.DIB.Pixels[x+xDisp,y+yDisp]
      end;
      DIB.Pixels[x,y] := newcolor;
    end;
  end;
end;*/