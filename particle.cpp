//=============================================================================
//
// パーティクル処理 [particle.cpp]
// Author : 
//
//=============================================================================
#include "main.h"
#include "debugproc.h"
#include "input.h"
#include "camera.h"
#include "particle.h"
#include "myDirect3D.h"
#include "texture.h"
#include "enemy.h"
#include <time.h>
#include "sky.h"

//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define	PARTICLE_SIZE_X		(0.5f)							// ビルボードの幅
#define	PARTICLE_SIZE_Y		(0.5f)							// ビルボードの高さ
#define	VALUE_MOVE_PARTICLE	(5.0f)							// 移動速度

#define	MAX_CHARGEPARTICLE		(50)						//チャージビルボード最大数
#define	MAX_SMOKEPARTICLE		(50)						//砂煙ビルボード最大数
#define	MAX_PARTICLE			(50)						//ビルボード最大数


#define CHARGE_IN_SPEED		37								//チャージインするときの速度
#define CHARGE_OUT_SPEED	20								//チャージアウトするときの速度

#define SMOKE_SPEED		30								//砂煙の速度

//*****************************************************************************
// 構造体定義
//*****************************************************************************
typedef struct
{
	D3DXVECTOR3 pos;		// 位置
	D3DXVECTOR3 rot;		// 回転
	D3DXVECTOR3 scale;		// スケール
	D3DXVECTOR3 move;		// 移動量
	D3DXCOLOR col;			// 色
	float fSizeX;			// 幅
	float fSizeY;			// 高さ
	int nIdxShadow;			// 影ID
	int nLife;				// 寿命
	bool bUse;				// 使用しているかどうか
} PARTICLE;

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT MakeVertexParticle(LPDIRECT3DDEVICE9 pDevice);
void SetVertexParticle(int nIdxParticle, float fSizeX, float fSizeY);
void SetColorParticle(int nIdxParticle, D3DXCOLOR col);

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DTEXTURE9		g_pD3DTextureParticle = NULL;	// テクスチャへのポインタ
LPDIRECT3DVERTEXBUFFER9 g_pD3DVtxBuffParticle = NULL;	// 頂点バッファインターフェースへのポインタ

D3DXMATRIX				g_mtxWorldParticle;				// ワールドマトリックス
D3DXVECTOR3				g_PosParticle[MAX_PARTICLE];				// 地面の位置
D3DXVECTOR3				g_RotParticle[MAX_PARTICLE];				// 地面の向き(回転)
D3DXVECTOR3				g_SclParticle[MAX_PARTICLE];				// 地面の大きさ(スケール)

PARTICLE				g_aParticle[MAX_PARTICLE];		// パーティクルワーク
D3DXVECTOR3				g_posBase;						// ビルボード発生位置
D3DXVECTOR3				g_posBase2;						// ビルボード2発生位置

bool					g_bPause = false;				// ポーズON/OFF
float					g_ParticlAlpha;					//パーティクルのアルファ値

//チャージの関数
bool					g_ChargeParticle;					//チャージしているかどうか
int						g_ChargeCnt;					//チャージ時間
int						g_ChargePattern;		        //パーティクルの動きのパターン:0 なにもなし:1 パンチチャージイン:2 パンチチャージアウト

//砂煙の関数
bool					g_SmokeParticle;					//チャージしているかどうか
int						g_SmokeCnt;					//チャージ時間
int						g_SmokePattern;		        //パーティクルの動きのパターン:0 なにもなし:1 パンチチャージイン:2 パンチチャージアウト

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitParticle(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();

	// 頂点情報の作成
	MakeVertexParticle(pDevice);

	// テクスチャの読み込み
	D3DXCreateTextureFromFile(pDevice,						// デバイスへのポインタ
		"asset/texture/effect000.jpg",			// ファイルの名前
		&g_pD3DTextureParticle);	// 読み込むメモリー

	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		g_aParticle[nCntParticle].pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aParticle[nCntParticle].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_aParticle[nCntParticle].scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		g_aParticle[nCntParticle].move = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		g_aParticle[nCntParticle].col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
		g_aParticle[nCntParticle].fSizeX = PARTICLE_SIZE_X;
		g_aParticle[nCntParticle].fSizeY = PARTICLE_SIZE_Y;
		g_aParticle[nCntParticle].nIdxShadow = -1;
		g_aParticle[nCntParticle].nLife = 0;
		g_aParticle[nCntParticle].bUse = false;
	}

	g_posBase = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_posBase2 = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_ChargeParticle = false;
	g_ChargeCnt = 0;
	g_ChargePattern = 0;
	g_ParticlAlpha = 0.0f;
	
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitParticle(void)
{
	if (g_pD3DTextureParticle != NULL)
	{// テクスチャの開放
		g_pD3DTextureParticle->Release();
		g_pD3DTextureParticle = NULL;
	}

	if (g_pD3DVtxBuffParticle != NULL)
	{// 頂点バッファの開放
		g_pD3DVtxBuffParticle->Release();
		g_pD3DVtxBuffParticle = NULL;
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateParticle(void)
{
	//乱数の初期化
	srand((unsigned)time(NULL));

	if (Keyboard_IsTrigger(DIK_6)) {
		SmokeParticle_Start();
	}

	// カメラの回転を取得
	CAMERA *cam = GetCamera();

	// 更新処理
	//=============================================================================
	//チャージパーティクル
	//=============================================================================
	ChargeParticle();

	//=============================================================================
	//砂煙パーティクル
	//=============================================================================
	//SandSmokeParticle();
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawParticle(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();
	D3DXMATRIX mtxView, mtxScale, mtxTranslate;
	CAMERA *cam = GetCamera();

	// ライティングを無効に
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	// 加算合成に設定
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);			// 結果 = 転送元(SRC) + 転送先(DEST)
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// αソースカラーの指定
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_ONE);			// αデスティネーションカラーの指定
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);				// Zバッファーの書き込みをしない
//	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);			// Z比較なし

	//pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);	// アルファブレンディング処理
	//pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);	// 最初のアルファ引数
	//pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);	// ２番目のアルファ引数

	for (int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if (g_aParticle[nCntParticle].bUse)
		{
			// ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_mtxWorldParticle);

			// ビューマトリックスを取得
			mtxView = cam->mtxView;

			g_mtxWorldParticle._11 = mtxView._11;
			g_mtxWorldParticle._12 = mtxView._21;
			g_mtxWorldParticle._13 = mtxView._31;
			g_mtxWorldParticle._21 = mtxView._12;
			g_mtxWorldParticle._22 = mtxView._22;
			g_mtxWorldParticle._23 = mtxView._32;
			g_mtxWorldParticle._31 = mtxView._13;
			g_mtxWorldParticle._32 = mtxView._23;
			g_mtxWorldParticle._33 = mtxView._33;

			// スケールを反映
			D3DXMatrixScaling(&mtxScale, g_aParticle[nCntParticle].scale.x, g_aParticle[nCntParticle].scale.y, g_aParticle[nCntParticle].scale.z);
			D3DXMatrixMultiply(&g_mtxWorldParticle, &g_mtxWorldParticle, &mtxScale);

			// 移動を反映
			D3DXMatrixTranslation(&mtxTranslate, g_aParticle[nCntParticle].pos.x, g_aParticle[nCntParticle].pos.y, g_aParticle[nCntParticle].pos.z);
			D3DXMatrixMultiply(&g_mtxWorldParticle, &g_mtxWorldParticle, &mtxTranslate);

			// ワールドマトリックスの設定
			pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldParticle);

			// 頂点バッファをデバイスのデータストリームにバインド
			pDevice->SetStreamSource(0, g_pD3DVtxBuffParticle, 0, sizeof(VERTEX_3D));

			// 頂点フォーマットの設定
			pDevice->SetFVF(FVF_VERTEX_3D);

			// テクスチャの設定
			pDevice->SetTexture(0, g_pD3DTextureParticle);

			// ポリゴンの描画
			pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, (nCntParticle * 4), NUM_POLYGON);
		}
	}

	// ライティングを有効に
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// 通常ブレンドに戻す
	pDevice->SetRenderState(D3DRS_BLENDOP, D3DBLENDOP_ADD);			// 結果 = 転送元(SRC) + 転送先(DEST)
	pDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);		// αソースカラーの指定
	pDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);	// αデスティネーションカラーの指定
	pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);				// Zバッファーの書き込みをする
//	pDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESS);				// Z比較あり

	//pDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);	// アルファブレンディング処理
	//pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);		// 最初のアルファ引数
	//pDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2, D3DTA_CURRENT);		// ２番目のアルファ引数

}

//=============================================================================
// 頂点情報の作成
//=============================================================================
HRESULT MakeVertexParticle(LPDIRECT3DDEVICE9 pDevice)
{
	// オブジェクトの頂点バッファを生成
    if( FAILED( pDevice->CreateVertexBuffer(sizeof(VERTEX_3D) * NUM_VERTEX * MAX_PARTICLE,	// 頂点データ用に確保するバッファサイズ(バイト単位)
												D3DUSAGE_WRITEONLY,							// 頂点バッファの使用法　
												FVF_VERTEX_3D,								// 使用する頂点フォーマット
												D3DPOOL_MANAGED,							// リソースのバッファを保持するメモリクラスを指定
												&g_pD3DVtxBuffParticle,					// 頂点バッファインターフェースへのポインタ
												NULL)))										// NULLに設定
	{
        return E_FAIL;
	}

	{//頂点バッファの中身を埋める
		VERTEX_3D *pVtx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		g_pD3DVtxBuffParticle->Lock(0, 0, (void**)&pVtx, 0);

		for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++, pVtx += 4)
		{
			// 頂点座標の設定
			pVtx[0].pos = D3DXVECTOR3(-PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);
			pVtx[1].pos = D3DXVECTOR3(PARTICLE_SIZE_X / 2, -PARTICLE_SIZE_Y / 2, 0.0f);
			pVtx[2].pos = D3DXVECTOR3(-PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);
			pVtx[3].pos = D3DXVECTOR3(PARTICLE_SIZE_X / 2, PARTICLE_SIZE_Y / 2, 0.0f);

			// 法線の設定
			pVtx[0].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx[1].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx[2].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);
			pVtx[3].nor = D3DXVECTOR3(0.0f, 0.0f, -1.0f);

			// 反射光の設定
			pVtx[0].col = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f);
			pVtx[1].col = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f);
			pVtx[2].col = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f);
			pVtx[3].col = D3DXCOLOR(1.0f, 1.0f, 0.0f, 0.0f);

			// テクスチャ座標の設定
			pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
			pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
			pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
			pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);
		}

		// 頂点データをアンロックする
		g_pD3DVtxBuffParticle->Unlock();
	}

	return S_OK;
}

//=============================================================================
// 頂点座標の設定
//=============================================================================
void SetVertexParticle(int nIdxParticle, float fSizeX, float fSizeY)
{
	{//頂点バッファの中身を埋める
		VERTEX_3D *pVtx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		g_pD3DVtxBuffParticle->Lock(0, 0, (void**)&pVtx, 0);

		pVtx += (nIdxParticle * 4);

		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(-fSizeX / 2, -fSizeY / 2, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(-fSizeX / 2, fSizeY / 2, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(fSizeX / 2, -fSizeY / 2, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(fSizeX / 2, fSizeY / 2, 0.0f);

		// 頂点データをアンロックする
		g_pD3DVtxBuffParticle->Unlock();
	}
}

//=============================================================================
// 頂点カラーの設定
//=============================================================================
void SetColorParticle(int nIdxParticle, D3DXCOLOR col)
{
	{//頂点バッファの中身を埋める
		VERTEX_3D *pVtx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		g_pD3DVtxBuffParticle->Lock(0, 0, (void**)&pVtx, 0);

		pVtx += (nIdxParticle * 4);

		// 頂点座標の設定
		pVtx[0].col =
		pVtx[1].col =
		pVtx[2].col =
		pVtx[3].col = col;

		// 頂点データをアンロックする
		g_pD3DVtxBuffParticle->Unlock();
	}
}

//=============================================================================
// 頂点情報の作成
//=============================================================================
int SetParticle(D3DXVECTOR3 pos, D3DXVECTOR3 move, D3DXCOLOR col, float fSizeX, float fSizeY, int nLife)
{
	int nIdxParticle = -1;

	for(int nCntParticle = 0; nCntParticle < MAX_PARTICLE; nCntParticle++)
	{
		if(!g_aParticle[nCntParticle].bUse)
		{
			g_aParticle[nCntParticle].pos = pos;
			g_aParticle[nCntParticle].rot = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			g_aParticle[nCntParticle].scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
			g_aParticle[nCntParticle].move = move;
			g_aParticle[nCntParticle].col = col;
			g_aParticle[nCntParticle].fSizeX = fSizeX;
			g_aParticle[nCntParticle].fSizeY = fSizeY;
			g_aParticle[nCntParticle].nLife = nLife;
			g_aParticle[nCntParticle].bUse = true;

			// 頂点座標の設定
			SetVertexParticle(nCntParticle, fSizeX, fSizeY);

			nIdxParticle = nCntParticle;


			break;
		}
	}

	return nIdxParticle;
}

//チャージパーティクル
void ChargeParticle() {
	//ベースの位置を取得
	if (GetPunchIndex() == 0 || GetPunchIndex() == 2 || GetPunchIndex() == 6) {
		g_posBase = (GetEnemy() + 3)->Pos;
	}
	else if (GetPunchIndex() == 1 || GetPunchIndex() == 3 || GetPunchIndex() == 5) {
		g_posBase = (GetEnemy() + 2)->Pos;
	}
	else if (GetPunchIndex() == 4) {
		//g_posBase = (GetEnemy() + 2)->Pos;
		//g_posBase2 = (GetEnemy() + 3)->Pos;
	}
	//チャージされたら
	if (g_ChargeParticle == true) {
		if (g_bPause == false)
		{
			for (int nCntParticle = 0; nCntParticle < MAX_CHARGEPARTICLE; nCntParticle++)
			{
				if (g_aParticle[nCntParticle].bUse)
				{// 使用中
					g_aParticle[nCntParticle].pos.x += g_aParticle[nCntParticle].move.x;
					g_aParticle[nCntParticle].pos.y += g_aParticle[nCntParticle].move.y;
					g_aParticle[nCntParticle].pos.z += g_aParticle[nCntParticle].move.z;


					if (GetPunchIndex() == 4) {
						g_aParticle[nCntParticle].pos.y += (GetEnemy() + 3)->Pos.y - g_posBase2.y;
					}
					g_aParticle[nCntParticle].nLife--;

					SetColorParticle(nCntParticle, g_aParticle[nCntParticle].col);
					if (g_ChargePattern == 1) {
						SetColorParticle(nCntParticle, D3DXCOLOR(0.1f, 0.1f, 0.0f, g_ParticlAlpha));
						g_ParticlAlpha += 1.0f / (CHARGE_IN_SPEED* MAX_CHARGEPARTICLE);
					}
					if (g_ChargePattern == 2) {
						SetColorParticle(nCntParticle, D3DXCOLOR(0.1f, 0.1f, 0.0f, g_ParticlAlpha));
						g_ParticlAlpha -= 1.0f / (CHARGE_OUT_SPEED * MAX_CHARGEPARTICLE);
					}
				}
				//ライフが0になった時
				if (g_aParticle[nCntParticle].nLife <= 0)
				{
					g_aParticle[nCntParticle].bUse = false;
					g_aParticle[nCntParticle].nIdxShadow = -1;
				}

			}
			//両手の時
			if (GetPunchIndex() == 4) {
				g_posBase = (GetEnemy() + 2)->Pos;
				g_posBase2 = (GetEnemy() + 3)->Pos;
			}
			////ジャンピングパンチ右の時
			//if (GetPunchIndex() == 5) {
			//	g_posBase2.y = (GetEnemy() + 2)->Pos.y;
			//}
			////ジャンピングパンチ左の時
			//if (GetPunchIndex() == 6) {
			//	g_posBase2.y = (GetEnemy() + 3)->Pos.y;
			//}

			if (g_ChargePattern == 1) {
				for (int nCntParticle = 0; nCntParticle < MAX_CHARGEPARTICLE; nCntParticle++)
				{

					// パーティクル発生
					D3DXVECTOR3 pos, pos2;
					D3DXVECTOR3 move, move2;
					int nLife;
					float fSize;

					//位置設定
					//左手
					if (GetPunchIndex() == 0 || GetPunchIndex() == 2 || GetPunchIndex() == 6) {
						pos = (GetEnemy() + 3)->Pos;
					}
					//右手
					else if (GetPunchIndex() == 1 || GetPunchIndex() == 3 || GetPunchIndex() == 5) {
						pos = (GetEnemy() + 2)->Pos;
					}
					//両手パンチ
					else if (GetPunchIndex() == 4) {
						pos = (GetEnemy() + 2)->Pos;
						pos2 = (GetEnemy() + 3)->Pos;

						pos2.x += (rand() % MAX_CHARGEPARTICLE - 25);
						pos2.y += (rand() % MAX_CHARGEPARTICLE - 25);


						move2.x = (g_posBase2.x - pos2.x) / CHARGE_IN_SPEED;
						move2.y = (g_posBase2.y - pos2.y) / CHARGE_IN_SPEED;
						move2.z = 0;
					}

					pos.x += (rand() % MAX_CHARGEPARTICLE - 25);
					pos.y += (rand() % MAX_CHARGEPARTICLE - 25);
					pos.z += (rand() % MAX_CHARGEPARTICLE - 25);


					move.x = (g_posBase.x - pos.x) / CHARGE_IN_SPEED;
					move.y = (g_posBase.y - pos.y) / CHARGE_IN_SPEED;
					move.z = (g_posBase.z - pos.z) / CHARGE_IN_SPEED;

					nLife = CHARGE_IN_SPEED;

					fSize = (float)(rand() % 1 + 2);
					//fSize = fSize / 2;
					// ビルボードの設定
					SetParticle(pos, move, D3DXCOLOR(0.8f, 0.8f, 0.0f, 1.0f), fSize, fSize, nLife);
					if (GetPunchIndex() == 4) {
						SetParticle(pos2, move2, D3DXCOLOR(0.8f, 0.8f, 0.0f, 1.0f), fSize, fSize, nLife);
					}
				}
			}

			if (g_ChargePattern == 2) {
				for (int nCntParticle = 0; nCntParticle < MAX_CHARGEPARTICLE; nCntParticle++)
				{

					// パーティクル発生
					D3DXVECTOR3 pos, pos2;
					D3DXVECTOR3 move, move2;
					int nLife;
					float fSize;

					//位置設定
					if (GetPunchIndex() == 0 || GetPunchIndex() == 2 || GetPunchIndex() == 6) {
						pos = (GetEnemy() + 3)->Pos;
					}
					else if (GetPunchIndex() == 1 || GetPunchIndex() == 3 || GetPunchIndex() == 5) {
						pos = (GetEnemy() + 2)->Pos;
					}
					else if (GetPunchIndex() == 4) {
						pos = (GetEnemy() + 2)->Pos;
						pos2 = (GetEnemy() + 3)->Pos;

						pos2.x += (rand() % MAX_CHARGEPARTICLE - 25);
						pos2.y += (rand() % MAX_CHARGEPARTICLE - 25);

						move2.x = (rand() % MAX_CHARGEPARTICLE - 25);
						move2.y = (rand() % MAX_CHARGEPARTICLE - 25);
						move2.z = (rand() % MAX_CHARGEPARTICLE - 25);

						move2.x = move2.x / CHARGE_OUT_SPEED;
						move2.y = move2.y / CHARGE_OUT_SPEED;
						move2.z = 0;
					}

					move.x = (rand() % MAX_CHARGEPARTICLE - 25);
					move.y = (rand() % MAX_CHARGEPARTICLE - 25);
					move.z = (rand() % MAX_CHARGEPARTICLE - 25);

					move.x = move.x / CHARGE_OUT_SPEED;
					move.y = move.y / CHARGE_OUT_SPEED;
					move.z = move.z / CHARGE_OUT_SPEED;

					nLife = CHARGE_OUT_SPEED * 2;

					fSize = (float)(rand() % 1 + 2);
					//fSize = fSize / 2;
					// ビルボードの設定
					SetParticle(pos, move, D3DXCOLOR(0.8f, 0.8f, 0.0f, 1.0f), fSize, fSize, nLife);
					if (GetPunchIndex() == 4) {
						SetParticle(pos2, move2, D3DXCOLOR(0.8f, 0.8f, 0.0f, 1.0f), fSize, fSize, nLife);
					}
				}
			}

			g_ChargeCnt++;
			if (g_ChargeCnt >= CHARGE_IN_SPEED) {
				g_ChargePattern = 2;
			}
			if (g_ChargeCnt >= CHARGE_IN_SPEED + CHARGE_OUT_SPEED * 2) {
				for (int nCntParticle = 0; nCntParticle < MAX_CHARGEPARTICLE; nCntParticle++)
				{
					g_aParticle[nCntParticle].bUse = false;
					g_aParticle[nCntParticle].nIdxShadow = -1;
				}
				g_ChargeParticle = false;
			}
		}
	}
	else {
		g_ParticlAlpha = 0.0f;
		g_ChargeCnt = 0;
	}
}
//チャージをtrueにする関数
void Charge_Start(){
	g_ChargeParticle = true;
	g_ChargePattern = 1;
}
//=============================================================================
//砂煙パーティクル
//=============================================================================
void SandSmokeParticle() {
	if (g_SmokeParticle == true) {
		g_posBase = (GetEnemy() + 2)->Pos;

		for (int nCntParticle = 0; nCntParticle < MAX_SMOKEPARTICLE; nCntParticle++)
		{
			if (g_aParticle[nCntParticle].bUse)
			{// 使用中
				g_aParticle[nCntParticle].pos.x += g_aParticle[nCntParticle].move.x;
				g_aParticle[nCntParticle].pos.y += g_aParticle[nCntParticle].move.y;
				g_aParticle[nCntParticle].pos.z += g_aParticle[nCntParticle].move.z;

				g_aParticle[nCntParticle].nLife--;

				SetColorParticle(nCntParticle, g_aParticle[nCntParticle].col);
				SetColorParticle(nCntParticle, D3DXCOLOR(0.1f, 0.1f, 0.0f, g_ParticlAlpha));
				g_ParticlAlpha += 1.0f / (SMOKE_SPEED* MAX_SMOKEPARTICLE);

			}
		}
		for (int nCntParticle = 0; nCntParticle < MAX_SMOKEPARTICLE; nCntParticle++)
		{

			// パーティクル発生
			D3DXVECTOR3 pos;
			D3DXVECTOR3 move;
			int nLife;
			float fSize;

			//位置設定
			pos = (GetEnemy() + 2)->Pos;
	
			move.x = (rand() % 50 - 25);
			move.y = (rand() % 25 - 25);
			move.z = 0;

			move.x = move.x / SMOKE_SPEED;
			move.y = move.y / SMOKE_SPEED;
			move.z = move.z / SMOKE_SPEED;

			nLife = SMOKE_SPEED * 2;

			fSize = (float)(rand() % 1 + 2) * 0.1f;
			//fSize = fSize / 2;
			// ビルボードの設定
			SetParticle(pos, move, D3DXCOLOR(0.8f, 0.8f, 0.0f, 1.0f), fSize, fSize, nLife);

		}
		g_ChargeCnt++;

		//ライフが0になった時
		if (g_SmokeCnt >= SMOKE_SPEED) {
			for (int nCntParticle = 0; nCntParticle < MAX_SMOKEPARTICLE; nCntParticle++)
			{
				g_aParticle[nCntParticle].bUse = false;
				g_aParticle[nCntParticle].nIdxShadow = -1;
			}
			g_SmokeParticle = false;
		}
	}
	else {
		g_ParticlAlpha = 0.0f;
		g_SmokeCnt = 0;
	}
}

//砂煙パーティクルをtrueにする関数
void SmokeParticle_Start() {
	g_SmokeParticle = true;
	g_SmokePattern = 1;
}