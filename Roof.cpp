#include "Roof.h"
#include "myDirect3D.h"
#include "input.h"
#include "texture.h"
#include "debugproc.h"
#include "enemy.h"
#include "game.h"
#include "sprite.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	VALUE_MOVE		(5.0f)					// 移動量
#define	VALUE_ROTATE	(D3DX_PI * 0.02f)		// 回転量

#define ENEMY_HEAD		1						// エネミーの頭を指定
//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffRoof = NULL;	// 頂点バッファへのポインタ
D3DXMATRIX				g_mtxWorldRoof;		// ワールドマトリックス
D3DXVECTOR3				g_posRoof;			// 天井の位置
D3DXVECTOR3				g_rotRoof;			// 天井の向き(回転)
D3DXVECTOR3				g_sclRoof;			// 天井の大きさ(スケール)
XMODEL					*g_enemy;			// エネミーの座標を入手するのに使用

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitRoof(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();
	g_enemy = GetEnemy();

	// オブジェクトの頂点バッファを生成
	if (FAILED(pDevice->CreateVertexBuffer
	(
		sizeof(VERTEX_3D) * NUM_VERTEX,	// 頂点データ用に確保するバッファサイズ(バイト単位)
		D3DUSAGE_WRITEONLY,				// 頂点バッファの使用法　
		FVF_VERTEX_3D,					// 使用する頂点フォーマット
		D3DPOOL_MANAGED,				// リソースのバッファを保持するメモリクラスを指定
		&g_pVtxBuffRoof,				// 頂点バッファインターフェースへのポインタ
		NULL)))							// NULLに設定
	{
		return E_FAIL;
	}

	{//頂点バッファの中身を埋める
		VERTEX_3D *pVtx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		g_pVtxBuffRoof->Lock(0, 0, (void**)&pVtx, 0);

		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(-50.0f, 50.0f, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(50.0f, 50.0f, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-50.0f, -50.0f, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(50.0f, -50.0f, 0.0f);

		// 法線の設定
		pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
		pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

		// 頂点カラーの設定
		pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

		// テクスチャ座標の設定
		pVtx[0].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[1].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(1.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 0.0f);

		// 頂点データをアンロックする
		g_pVtxBuffRoof->Unlock();
	}

	g_posRoof = D3DXVECTOR3(0.0f, ROOF_Y, ROOF_Z);
	g_sclRoof = D3DXVECTOR3(1.0f, 1.0f, 1.5f);

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
	g_enemy = GetEnemy();

	DebugProc_Print((char*)"enemy.x:%f,enemy.y:%f,enemy.z:%f", (g_enemy + ENEMY_HEAD)->Pos.x, (g_enemy + ENEMY_HEAD)->Pos.y, (g_enemy + ENEMY_HEAD)->Pos.z);
}
//=============================================================================
// 描画処理てんじょう
//=============================================================================
void DrawRoof(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();
	D3DXMATRIX mtxView, mtxScale, mtxTranslate;

	//アルファテストを有効に
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	//ライティングを無効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&g_mtxWorldRoof);

	//ビューマトリックスを取得
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	//POLYGONを正面に向ける
#if 1
	//逆行列を求める
	D3DXMatrixInverse(&g_mtxWorldRoof, NULL, &mtxView);
	g_mtxWorldRoof._41 = 0.0f;
	g_mtxWorldRoof._42 = 0.0f;
	g_mtxWorldRoof._43 = 0.0f;
#else
	g_mtxWorldRoof._11 = mtxViex._11;
	g_mtxWorldRoof._12 = mtxViex._21;
	g_mtxWorldRoof._13 = mtxViex._31;
	g_mtxWorldRoof._21 = mtxViex._12;
	g_mtxWorldRoof._22 = mtxViex._22;
	g_mtxWorldRoof._23 = mtxViex._32;
	g_mtxWorldRoof._31 = mtxViex._13;
	g_mtxWorldRoof._32 = mtxViex._23;
	g_mtxWorldRoof._33 = mtxViex._33;
#endif

	//スケールを反映
	D3DXMatrixScaling(&mtxScale, g_sclRoof.x, g_sclRoof.y, g_sclRoof.z);
	D3DXMatrixMultiply(&g_mtxWorldRoof, &g_mtxWorldRoof, &mtxScale);

	//移動を反映
	D3DXMatrixTranslation(&mtxTranslate, g_posRoof.x, g_posRoof.y, g_posRoof.z);
	D3DXMatrixMultiply(&g_mtxWorldRoof, &g_mtxWorldRoof, &mtxTranslate);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldRoof);

	//頂点バッファをデバイスのデータストリームにバインド
	pDevice->SetStreamSource(0, g_pVtxBuffRoof, 0, sizeof(VERTEX_3D));

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	//テクスチャの設定
	if ((g_enemy + ENEMY_HEAD)->Pos.y < ROOF_Y)
	{
		pDevice->SetTexture(0, Texture_GetTexture(TEXTURE_INDEX_ROOF_STD));
	}
	if ((g_enemy + ENEMY_HEAD)->Pos.y >= ROOF_Y)
	{
		pDevice->SetTexture(0, Texture_GetTexture(TEXTURE_INDEX_ROOF_BROKE));
	}


	//POLYGONの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 12);

	//ライティングを有効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	//アルファテストを無効に
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);


	//================================================

}