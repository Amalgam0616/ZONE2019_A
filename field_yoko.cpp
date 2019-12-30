#include "field_yoko.h"
#include "myDirect3D.h"
#include "input.h"
#include "texture.h"
#include "debugproc.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************

#define	VALUE_MOVE		(5.0f)							// 移動量
#define	VALUE_ROTATE	(D3DX_PI * 0.02f)				// 回転量

#define	FIELD_WIDTH		(800.0f)						// 地面の幅(X方向)
#define	FIELD_HEIGHT	(300.0f)						// 地面の高さ(Y方向)
#define	FIELD_DEPTH		(300.0f)					// 地面の奥行(Z方向)

#define VERTEX 6 * 2 * 5	//板ポリ１個書くのにLISTだと頂点６個が必要	* 左右（２）＊　Z軸＋方向に何枚出すか
#define POLYGON_KAZU VERTEX / 3
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexField_Yoko(LPDIRECT3DDEVICE9 pDevice);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffField_yoko = NULL;	// 頂点バッファへのポインタ

D3DXMATRIX				g_mtxWorldField_yoko;			// ワールドマトリックス
D3DXVECTOR3				g_posField_yoko;				// 地面の位置
D3DXVECTOR3				g_rotField_yoko;				// 地面の向き(回転)
D3DXVECTOR3				g_sclField_yoko;				// 地面の大きさ(スケール)
//=========================================================================

HRESULT InitField_Yoko(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();

	// 頂点情報の作成
	MakeVertexField_Yoko(pDevice);

	// 位置・回転・スケールの初期設定
	g_posField_yoko = D3DXVECTOR3(0.0f, 0.0f, 200.0f);
	g_rotField_yoko = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_sclField_yoko = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitField_Yoko(void)
{
	if (g_pVtxBuffField_yoko != NULL)
	{// 頂点バッファの開放
		g_pVtxBuffField_yoko->Release();
		g_pVtxBuffField_yoko = NULL;
	}
}
//=============================================================================
// 更新処理
//=============================================================================
void UpdateField_Yoko(void)
{

}
//=============================================================================
// 描画処理
//=============================================================================
void DrawField_Yoko(void)
{
	LPD3DXBUFFER pD3DXMtrlBuffer = NULL;

	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();
	D3DXMATRIX mtxScl, mtxRot, mtxTranslate;

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&g_mtxWorldField_yoko);

	//スケールを反映
	D3DXMatrixScaling(&mtxScl, g_sclField_yoko.x, g_sclField_yoko.y, g_sclField_yoko.z);
	D3DXMatrixMultiply(&g_mtxWorldField_yoko, &g_mtxWorldField_yoko, &mtxScl);

	//回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotField_yoko.y, g_rotField_yoko.z, g_rotField_yoko.x);
	D3DXMatrixMultiply(&g_mtxWorldField_yoko, &g_mtxWorldField_yoko, &mtxRot);

	//移動を反映
	D3DXMatrixTranslation(&mtxTranslate, g_posField_yoko.x, g_posField_yoko.y, g_posField_yoko.z);
	D3DXMatrixMultiply(&g_mtxWorldField_yoko, &g_mtxWorldField_yoko, &mtxTranslate);

//	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);


	//ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldField_yoko);

	//
	pDevice->SetStreamSource(0, g_pVtxBuffField_yoko, 0, sizeof(VERTEX_3D));

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

	////pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
}
//=============================================================================
// 頂点の作成
//=============================================================================
HRESULT MakeVertexField_Yoko(LPDIRECT3DDEVICE9 pDevice)
{
	//for文を回すのに利用,posの代入時
	int cnt = 0;

	if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D)* VERTEX,
		D3DUSAGE_WRITEONLY,
		FVF_VERTEX_3D,
		D3DPOOL_MANAGED,
		&g_pVtxBuffField_yoko,
		NULL)))
	{
		return E_FAIL;
	}
	{
		//頂点バッファの中身を埋める
		VERTEX_3D *pVtx;
		//LockをするとVRAMからメモリのバッファを固定(リアルタイムで処理するのは控えめにした方がよい）
		//頂点データの範囲をロックし、頂点バッファのポインタをとる
		g_pVtxBuffField_yoko->Lock(0, 0, (void**)&pVtx, 0);

		/*
			ほんとはこんな書き方したくないけど仕方ないね
		*/
		//ポジション
		for (int i = 0; i < VERTEX; i = i + 12)
		{
			//========
			//  右壁
			//========
			//ポジション
			pVtx[i].pos = D3DXVECTOR3(FIELD_WIDTH + 200.0f, FIELD_HEIGHT, FIELD_DEPTH + FIELD_DEPTH * cnt);
			pVtx[i + 1].pos = D3DXVECTOR3(FIELD_WIDTH + 200.0f, FIELD_HEIGHT, -FIELD_DEPTH + FIELD_DEPTH * cnt);
			pVtx[i + 2].pos = D3DXVECTOR3(FIELD_WIDTH, -FIELD_HEIGHT, FIELD_DEPTH + FIELD_DEPTH * cnt);

			pVtx[i + 3].pos = D3DXVECTOR3(FIELD_WIDTH, -FIELD_HEIGHT, FIELD_DEPTH + FIELD_DEPTH * cnt);
			pVtx[i + 4].pos = D3DXVECTOR3(FIELD_WIDTH + 200.0f, FIELD_HEIGHT, -FIELD_DEPTH + FIELD_DEPTH * cnt);
			pVtx[i + 5].pos = D3DXVECTOR3(FIELD_WIDTH, -FIELD_HEIGHT, -FIELD_DEPTH + FIELD_DEPTH * cnt);

			//法線ベクトル
			pVtx[i].nor = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
			pVtx[i + 1].nor = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
			pVtx[i + 2].nor = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

			pVtx[i + 3].nor = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
			pVtx[i + 4].nor = D3DXVECTOR3(1.0f, 0.0f, 0.0f);
			pVtx[i + 5].nor = D3DXVECTOR3(1.0f, 0.0f, 0.0f);

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

			//========
			//  左壁
			//========
			//ポジション
			pVtx[i + 6].pos = D3DXVECTOR3(-FIELD_WIDTH - 200.0f, FIELD_HEIGHT, -FIELD_DEPTH + FIELD_DEPTH * cnt);
			pVtx[i + 7].pos = D3DXVECTOR3(-FIELD_WIDTH - 200.0f, FIELD_HEIGHT, FIELD_DEPTH + FIELD_DEPTH * cnt);
			pVtx[i + 8].pos = D3DXVECTOR3(-FIELD_WIDTH, -FIELD_HEIGHT, -FIELD_DEPTH + FIELD_DEPTH * cnt);

			pVtx[i + 9].pos = D3DXVECTOR3(-FIELD_WIDTH, -FIELD_HEIGHT, -FIELD_DEPTH + FIELD_DEPTH * cnt);
			pVtx[i + 10].pos = D3DXVECTOR3(-FIELD_WIDTH - 200.0f, FIELD_HEIGHT, FIELD_DEPTH + FIELD_DEPTH * cnt);
			pVtx[i + 11].pos = D3DXVECTOR3(-FIELD_WIDTH, -FIELD_HEIGHT, FIELD_DEPTH + FIELD_DEPTH * cnt);

			//法線ベクトル
			pVtx[i + 6].nor = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
			pVtx[i + 7].nor = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
			pVtx[i + 8].nor = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);

			pVtx[i + 9].nor = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
			pVtx[i + 10].nor = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);
			pVtx[i + 11].nor = D3DXVECTOR3(-1.0f, 0.0f, 0.0f);

			//頂点カラー
			pVtx[i + 6].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[i + 7].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[i + 8].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

			pVtx[i + 9].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[i + 10].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			pVtx[i + 11].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

			//テクスチャ
			pVtx[i + 6].tex = D3DXVECTOR2(0.0f, 0.0f);
			pVtx[i + 7].tex = D3DXVECTOR2(1.0f, 0.0f);
			pVtx[i + 8].tex = D3DXVECTOR2(0.0f, 0.5f);

			pVtx[i + 9].tex = D3DXVECTOR2(0.0f, 0.5f);
			pVtx[i + 10].tex = D3DXVECTOR2(1.0f, 0.0f);
			pVtx[i + 11].tex = D3DXVECTOR2(1.0f, 0.5f);

			cnt++;
		}

		//ここでVRAMへ帰す
		g_pVtxBuffField_yoko->Unlock();
	}
	
	return S_OK;
}

