#include "floor.h"
#include "myDirect3D.h"
#include "input.h"
#include "texture.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

#define	VALUE_MOVE		(5.0f)							// 移動量
#define	VALUE_ROTATE	(D3DX_PI * 0.02f)				// 回転量

#define	FIELD_WIDTH		(50.0f)						// 地面の幅(X方向)
#define	FIELD_HEIGHT	(20.0f)						// 地面の幅(X方向)
#define	FIELD_DEPTH		(30.0f)					// 地面の奥行(Z方向)

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexFloor(LPDIRECT3DDEVICE9 pDevice);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffFloor = NULL;	// 頂点バッファへのポインタ

//=========================================================================

D3DXMATRIX				g_mtxWorldFloor;		// ワールドマトリックス
D3DXVECTOR3				g_posFloor;			// 地面の位置
D3DXVECTOR3				g_rotFloor;			// 地面の向き(回転)
D3DXVECTOR3				g_sclFloor;			// 地面の大きさ(スケール)
//=============================================================================
HRESULT InitFloor(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();
	MakeVertexFloor(pDevice);

	g_posFloor = D3DXVECTOR3(0.0f, 0.0f, 200.0f);
	g_rotFloor = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_sclFloor = D3DXVECTOR3(8.0f, 5.0f, 20.0f);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitFloor(void)
{
	if (g_pVtxBuffFloor != NULL)
	{// 頂点バッファの開放
		g_pVtxBuffFloor->Release();
		g_pVtxBuffFloor = NULL;
	}
}
//=============================================================================
// 更新処理
//=============================================================================
void UpdateFloor(void)
{

}
//=============================================================================
// 描画処理ゆか
//=============================================================================
void DrawFloor(void)
{
	LPD3DXBUFFER pD3DXMtrlBuffer = NULL;

	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&g_mtxWorldFloor);

	//スケールを反映
	D3DXMatrixScaling(&mtxScl, g_sclFloor.x, g_sclFloor.y, g_sclFloor.z);
	D3DXMatrixMultiply(&g_mtxWorldFloor, &g_mtxWorldFloor, &mtxScl);

	//回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotFloor.y, g_rotFloor.z, g_rotFloor.x);
	D3DXMatrixMultiply(&g_mtxWorldFloor, &g_mtxWorldFloor, &mtxRot);

	//移動を反映
	D3DXMatrixTranslation(&mtxTranslate, g_posFloor.x, g_posFloor.y, g_posFloor.z);
	D3DXMatrixMultiply(&g_mtxWorldFloor, &g_mtxWorldFloor, &mtxTranslate);

	//	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);


	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldFloor);

	//
	pDevice->SetStreamSource(0, g_pVtxBuffFloor, 0, sizeof(VERTEX_3D));

	pDevice->SetFVF(FVF_VERTEX_3D);

	D3DMATERIAL9 mat = {};

	mat.Diffuse.r = 1.0f;
	mat.Diffuse.g = 1.0f;
	mat.Diffuse.b = 1.0f;
	mat.Diffuse.a = 1.0f;

	mat.Ambient.r = 0.0f;
	mat.Ambient.g = 0.f;
	mat.Ambient.b = 0.1f;
	mat.Ambient.a = 0.0f;

	//マテリアルセット
	pDevice->SetMaterial(&mat);

	//テクスチャのセット
	pDevice->SetTexture(0,NULL);

	//ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 6);

	////pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

}
//=============================================================================
// 頂点の作成ゆか
//=============================================================================
HRESULT MakeVertexFloor(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 6,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffFloor,
		NULL)))
	{
		return E_FAIL;
	}
	{
		//頂点バッファの中身を埋める
		VERTEX_3D *pVtx;
		//LockをするとVRAMからメモリのバッファを固定(リアルタイムで処理するのは控えめにした方がよい）
		//頂点データの範囲をロックし、頂点バッファのポインタをとる
		g_pVtxBuffFloor->Lock(0, 0, (void**)&pVtx, 0);

		//床
		pVtx[0].pos = D3DXVECTOR3(-FIELD_WIDTH, -FIELD_HEIGHT, FIELD_DEPTH);
		pVtx[1].pos = D3DXVECTOR3(FIELD_WIDTH, -FIELD_HEIGHT, FIELD_DEPTH);
		pVtx[2].pos = D3DXVECTOR3(-FIELD_WIDTH, -FIELD_HEIGHT, -FIELD_DEPTH);

		pVtx[3].pos = D3DXVECTOR3(-FIELD_WIDTH, -FIELD_HEIGHT, -FIELD_DEPTH);
		pVtx[4].pos = D3DXVECTOR3(FIELD_WIDTH, -FIELD_HEIGHT, FIELD_DEPTH);
		pVtx[5].pos = D3DXVECTOR3(FIELD_WIDTH, -FIELD_HEIGHT, -FIELD_DEPTH);

		//法線ベクトル
		pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[4].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[5].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		//カラー
		pVtx[0].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		pVtx[1].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		pVtx[2].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);

		pVtx[3].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		pVtx[4].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);
		pVtx[5].col = D3DXCOLOR(1.0f, 0.0f, 0.0f, 1.0f);


		//テクスチャ
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);

		pVtx[3].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[4].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[5].tex = D3DXVECTOR2(1.0f, 1.0f);

		//ここでVRAMへ帰す
		g_pVtxBuffFloor->Unlock();
	}

	return S_OK;

}