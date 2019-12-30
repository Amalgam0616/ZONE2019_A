#include "Roof.h"
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
HRESULT MakeVertexRoof(LPDIRECT3DDEVICE9 pDevice);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffRoof = NULL;	// 頂点バッファへのポインタ

//=========================================================================
D3DXMATRIX				g_mtxWorldRoof;		// ワールドマトリックス
D3DXVECTOR3				g_posRoof;			// 地面の位置
D3DXVECTOR3				g_rotRoof;			// 地面の向き(回転)
D3DXVECTOR3				g_sclRoof;			// 地面の大きさ(スケール)

HRESULT InitRoof(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();

	MakeVertexRoof(pDevice);

	// 位置・回転・スケールの初期設定
	g_posRoof = D3DXVECTOR3(0.0f, 50.0f, 0.0f);
	g_rotRoof = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_sclRoof = D3DXVECTOR3(8.0f, 1.0f, 20.0f);

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void UninitRoof(void)
{
	if (g_pVtxBuffRoof != NULL)
	{// 頂点バッファの開放
		g_pVtxBuffRoof->Release();
		g_pVtxBuffRoof = NULL;
	}

}
//=============================================================================
// 更新処理
//=============================================================================
void UpdateRoof(void)
{

}
//=============================================================================
// 描画処理てんじょう
//=============================================================================
void DrawRoof(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	//テクスチャを裏表描画する関数
	//pDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&g_mtxWorldRoof);

	//スケールを反映
	D3DXMatrixScaling(&mtxScl, g_sclRoof.x, g_sclRoof.y, g_sclRoof.z);
	D3DXMatrixMultiply(&g_mtxWorldRoof, &g_mtxWorldRoof, &mtxScl);

	//回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotRoof.y, g_rotRoof.z, g_rotRoof.x);
	D3DXMatrixMultiply(&g_mtxWorldRoof, &g_mtxWorldRoof, &mtxRot);

	//移動を反映
	D3DXMatrixTranslation(&mtxTranslate, g_posRoof.x, g_posRoof.y, g_posRoof.z);
	D3DXMatrixMultiply(&g_mtxWorldRoof, &g_mtxWorldRoof, &mtxTranslate);

	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldRoof);

	//
	pDevice->SetStreamSource(0, g_pVtxBuffRoof, 0, sizeof(VERTEX_3D));

	//
	pDevice->SetFVF(FVF_VERTEX_3D);

	D3DMATERIAL9 mat = {};

	mat.Diffuse.r = 1.0f;
	mat.Diffuse.g = 1.0f;
	mat.Diffuse.b = 1.0f;
	mat.Diffuse.a = 1.0f;

	mat.Ambient.r = 1.0f;
	mat.Ambient.g = 1.0f;
	mat.Ambient.b = 1.0f;
	mat.Ambient.a = 1.0f;

	//マテリアルセット
	pDevice->SetMaterial(&mat);

	//テクスチャのセット
	pDevice->SetTexture(NULL,NULL);
	//pDevice->SetTexture(0, Texture_GetTexture(TEXTURE_INDEX_ROOF_BROKE));

	//ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, 6);

}
//=============================================================================
// 頂点の作成てんじょう
//=============================================================================
HRESULT MakeVertexRoof(LPDIRECT3DDEVICE9 pDevice)
{
	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * 6,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffRoof,
		NULL)))
	{
		return E_FAIL;
	}
	{
		//頂点バッファの中身を埋める
		VERTEX_3D *pVtx;
		//LockをするとVRAMからメモリのバッファを固定(リアルタイムで処理するのは控えめにした方がよい）
		//頂点データの範囲をロックし、頂点バッファのポインタをとる
		g_pVtxBuffRoof->Lock(0, 0, (void**)&pVtx, 0);

		//ポジション
		pVtx[0].pos = D3DXVECTOR3(-FIELD_WIDTH, FIELD_HEIGHT, FIELD_DEPTH);
		pVtx[1].pos = D3DXVECTOR3(FIELD_WIDTH, FIELD_HEIGHT, FIELD_DEPTH);
		pVtx[2].pos = D3DXVECTOR3(-FIELD_WIDTH, FIELD_HEIGHT, -FIELD_DEPTH);

		pVtx[3].pos = D3DXVECTOR3(-FIELD_WIDTH, FIELD_HEIGHT, -FIELD_DEPTH);
		pVtx[4].pos = D3DXVECTOR3(FIELD_WIDTH, FIELD_HEIGHT, FIELD_DEPTH);
		pVtx[5].pos = D3DXVECTOR3(FIELD_WIDTH, FIELD_HEIGHT, -FIELD_DEPTH);

		//法線ベクトル
		pVtx[0].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		pVtx[3].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[4].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
		pVtx[5].nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

		//カラー
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[4].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[5].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		//テクスチャ
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);

		pVtx[3].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[4].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[5].tex = D3DXVECTOR2(1.0f, 1.0f);


		//ここでVRAMへ帰す
		g_pVtxBuffRoof->Unlock();
	}

	return S_OK;

}