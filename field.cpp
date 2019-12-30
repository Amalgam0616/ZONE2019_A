#include "field.h"
#include "myDirect3D.h"
#include "input.h"
#include "texture.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

#define	VALUE_MOVE		(5.0f)							// 移動量
#define	VALUE_ROTATE	(D3DX_PI * 0.02f)				// 回転量

#define	FIELD_WIDTH		(300.0f)			// 地面の幅(X方向)
#define	FIELD_HEIGHT	(300.0f)			// 地面の高さ(Y方向)
#define	FIELD_DEPTH		(0.0f)			// 地面の奥行(Z方向)

#define VERTEX 6 * 1 * 9	//板ポリ１個書くのにLISTだと頂点６個が必要	* 奥のみ（１）＊　X軸＋方向に何枚出すか
#define POLYGON_KAZU VERTEX / 3
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexField(LPDIRECT3DDEVICE9 pDevice);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffField = NULL;	// 頂点バッファへのポインタ

D3DXMATRIX				g_mtxWorldField;		// ワールドマトリックス
D3DXVECTOR3				g_posField;				// 地面の位置
D3DXVECTOR3				g_rotField;				// 地面の向き(回転)
D3DXVECTOR3				g_sclField;				// 地面の大きさ(スケール)
//=========================================================================

HRESULT InitField(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();

	// 頂点情報の作成
	MakeVertexField(pDevice);

	//位置・回転・スケールの初期設定
	g_posField = D3DXVECTOR3(-1200.0f, 0.0f, 1500.0f);
	g_rotField = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_sclField = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitField(void)
{
	if (g_pVtxBuffField != NULL)
	{// 頂点バッファの開放
		g_pVtxBuffField->Release();
		g_pVtxBuffField = NULL;
	}
}
//=============================================================================
// 更新処理
//=============================================================================
void UpdateField(void)
{
}
//=============================================================================
// 描画処理
//=============================================================================
void DrawField(void)
{
	LPD3DXBUFFER pD3DXMtrlBuffer = NULL;

	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&g_mtxWorldField);

	//スケールを反映
	D3DXMatrixScaling(&mtxScl, g_sclField.x, g_sclField.y, g_sclField.z);
	D3DXMatrixMultiply(&g_mtxWorldField, &g_mtxWorldField, &mtxScl);

	//回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotField.y, g_rotField.z, g_rotField.x);
	D3DXMatrixMultiply(&g_mtxWorldField, &g_mtxWorldField, &mtxRot);

	//移動を反映
	D3DXMatrixTranslation(&mtxTranslate, g_posField.x, g_posField.y, g_posField.z);
	D3DXMatrixMultiply(&g_mtxWorldField, &g_mtxWorldField, &mtxTranslate);

//	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);


	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldField);

	//
	pDevice->SetStreamSource(0, g_pVtxBuffField, 0, sizeof(VERTEX_3D));

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
	pDevice->SetTexture(0, Texture_GetTexture(TEXTURE_INDEX_AUDIENCE));

	//ポリゴンの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLELIST, 0, POLYGON_KAZU);
}
//=============================================================================
// 頂点の作成
//=============================================================================
HRESULT MakeVertexField(LPDIRECT3DDEVICE9 pDevice)
{
	//for文を回すときに座標を変えるのに必要
	int cnt = 0;

	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D)* VERTEX,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffField,
		NULL)))
	{
		return E_FAIL;
	}
	{
		//頂点バッファの中身を埋める
		VERTEX_3D *pVtx;
		//LockをするとVRAMからメモリのバッファを固定(リアルタイムで処理するのは控えめにした方がよい）
		//頂点データの範囲をロックし、頂点バッファのポインタをとる
		g_pVtxBuffField->Lock(0, 0, (void**)&pVtx, 0);

		for (int i = 0; i < VERTEX; i = i + 6)
		{
			//==============
			//  背面壁
			//==============
			//ポジション
			pVtx[i].pos = D3DXVECTOR3(-FIELD_WIDTH + FIELD_WIDTH * cnt, FIELD_HEIGHT, FIELD_DEPTH + 200.0f);
			pVtx[i + 1].pos = D3DXVECTOR3(FIELD_WIDTH + FIELD_WIDTH * cnt, FIELD_HEIGHT, FIELD_DEPTH + 200.0f);
			pVtx[i + 2].pos = D3DXVECTOR3(-FIELD_WIDTH + FIELD_WIDTH * cnt, -FIELD_HEIGHT, FIELD_DEPTH);

			pVtx[i + 3].pos = D3DXVECTOR3(-FIELD_WIDTH + FIELD_WIDTH * cnt, -FIELD_HEIGHT, FIELD_DEPTH);
			pVtx[i + 4].pos = D3DXVECTOR3(FIELD_WIDTH + FIELD_WIDTH * cnt, FIELD_HEIGHT, FIELD_DEPTH + 200.0f);
			pVtx[i + 5].pos = D3DXVECTOR3(FIELD_WIDTH + FIELD_WIDTH * cnt, -FIELD_HEIGHT, FIELD_DEPTH);

			//法線ベクトル
			pVtx[i].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx[i + 1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx[i + 2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

			pVtx[i + 3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx[i + 4].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx[i + 5].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

			//頂点カラー
			pVtx[i].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[i + 1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[i + 2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

			pVtx[i + 3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[i + 4].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[i + 5].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

			//テクスチャ
			pVtx[i].tex = D3DXVECTOR2(0.0f, 0.0f);
			pVtx[i + 1].tex = D3DXVECTOR2(1.0f, 0.0f);
			pVtx[i + 2].tex = D3DXVECTOR2(0.0f, 0.5f);

			pVtx[i + 3].tex = D3DXVECTOR2(0.0f, 0.5f);
			pVtx[i + 4].tex = D3DXVECTOR2(1.0f, 0.0f);
			pVtx[i + 5].tex = D3DXVECTOR2(1.0f, 0.5f);

			cnt++;
		}
	}
		
	//ここでVRAMへ帰す
	g_pVtxBuffField->Unlock();

	return S_OK;
}

