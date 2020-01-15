#include "main.h"
#include "field.h"
#include "myDirect3D.h"
#include "input.h"
#include "texture.h"
#include "debugproc.h"
#include "sprite.h"
#include "player.h"
#include "enemy.h"
#include "Effect.h"	
//*****************************************************************************
// マクロ定義
//*****************************************************************************

#define	VALUE_MOVE		(5.0f)							// 移動量
#define	VALUE_ROTATE	(D3DX_PI * 0.02f)				// 回転量

#define	HIT_EFFECT_WIDTH	(20.0f)						// エフェクトの幅(X方向)
#define	HIT_EFFECT_HEIGHT	(20.0f)						// エフェクトの幅(X方向)
														 
#define	WIND_EFFECT_WIDTH	(60.0f)						// エフェクトの幅(X方向)
#define	WIND_EFFECT_HEIGHT	(60.0f)						// エフェクトの幅(X方向)

#define	EFFECT_DEPTH	(10.0f)						// 地面の奥行(Z方向)
#define HIT_EFFECT_POSX (-10.0f)					//ヒットエフェクトのX位置
#define HIT_EFFECT_POSZ (-20.0f)					//ヒットエフェクトのZ位置

#define EFFECT_SIZEX (1.0f)
#define EFFECT_SIZEY (1.0f)

#define EFFECT_MAX			3						//エフェクトの数、ちゃんと変更する
#define EFFECT_SHRINK_SPEED 1						//エフェクトの縮まるスピード
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************

//****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffEffect[EFFECT_MAX] = {};// 頂点バッファへのポインタ

D3DXMATRIX				g_mtxWorldEffect[EFFECT_MAX];		// ワールドマトリックス
D3DXVECTOR3				g_PosEffect[EFFECT_MAX];				// 地面の位置
D3DXVECTOR3				g_RotEffect[EFFECT_MAX];				// 地面の向き(回転)
D3DXVECTOR3				g_SclEffect[EFFECT_MAX];				// 地面の大きさ(スケール)

//チャージするときの変数
float g_Charge_Animation_3d_X;		 //チャージアニメーションのU値
float g_Charge_Animation_3d_Y;		 //チャージアニメーションのV値
float g_Charge_Animation_U;			 //チャージアニメーションのUの移動する値
float g_Charge_Animation_V;			 //チャージアニメーションのVの移動する値
bool g_Charge_Animation_Flg;		 //チャージアニメーションをオンにするフラグ
int	  g_Charge_Animation_Rag_Cnt;	 //エフェクトを遅くするカウント
bool  g_AnimationShrink_Flg;		 //逆エフェクトアニメーションが終わったフラグ
bool   g_NumberResetFlg;			 //アニメーションごとに値を変えるフラグ

//パンチの空を切るエフェクト
float g_Wind_Animation_3d_X;		 //パンチが空を切るアニメーションのU値
float g_Wind_Animation_3d_Y;		 //パンチが空を切るアニメーションのV値
float g_Wind_Animation_U;			 //パンチが空を切るアニメーションのUの移動する値
float g_Wind_Animation_V;			 //パンチが空を切るアニメーションのVの移動する値
bool  g_Wind_Animation_Flg;		 //チャージアニメーションをオンにするフラグ

//パンチのヒットエフェクト
float g_Hit_Animation_3d_X;		 //パンチが空を切るアニメーションのU値
float g_Hit_Animation_3d_Y;		 //パンチが空を切るアニメーションのV値
float g_Hit_Animation_U;		 //パンチが空を切るアニメーションのUの移動する値
float g_Hit_Animation_V;		 //パンチが空を切るアニメーションのVの移動する値
bool  g_Hit_Animation_Flg;		 //チャージアニメーションをオンにするフラグ


float AnimationX = 0.0f;
float AnimationY = 0.0f;

//パンチの種類
typedef enum {
	PUNCH_HIT_EFFECT = 0,
	PUNCH_CUT_THE_SKY,

	MAX_EFFECT,
}EFFECT;

int   g_Punch_Val;					 //パンチのエフェクト種類

//スローの時の変数
int g_SlowEffectCnt;
bool g_SlowFlg;
int g_Player_y;
//=============================================================================
// 構造体宣言
//=============================================================================
typedef struct
{
	D3DXVECTOR3 Effect_pos;	// 頂点座標
	D3DXVECTOR3 Effect_nor;	// 法線ベクトル
	D3DCOLOR	Effect_col;	// 頂点カラー
	D3DXVECTOR2 Effect_tex;	// テクスチャ座標
}EFFECT_3D;

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitEffect(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();

	MakeVertexChargeEffect(pDevice);
	MakeVertexWindEffect(pDevice);
	MakeVertexHitEffect(pDevice);

	// 位置・回転・スケールの初期設定
	g_PosEffect[0] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_RotEffect[0] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_SclEffect[0] = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	//ヒットエフェクトのアニメーションの初期値
	g_Charge_Animation_U = 0.25f;
	g_Charge_Animation_V = 0.125f;
	g_Charge_Animation_3d_X = 0.75f;
	g_Charge_Animation_3d_Y = 0.875f;
	g_Charge_Animation_Rag_Cnt = 0;
	g_AnimationShrink_Flg = false;
	g_NumberResetFlg = true;
	g_Punch_Val = 0;

	//パンチが空を切るエフェクトの初期化
	g_Wind_Animation_3d_X = 0.0f;
	g_Wind_Animation_3d_Y = 0.0f;
	g_Wind_Animation_U = 0.25f;
	g_Wind_Animation_V = 0.125f;
	g_Wind_Animation_Flg = false;

	//パンチのヒットエフェクトの初期化
	g_Hit_Animation_3d_X = 0.0f;
	g_Hit_Animation_3d_Y = 0.0f;
	g_Hit_Animation_U = 0.25f;
	g_Hit_Animation_V = 0.125f;
	g_Hit_Animation_Flg = false;

	//

	//for (int i = 0; i < EFFECT_MAX; i++) {
	//	g_Charge_Animation_3d_X[i] = 0.25f *(rand() % 3);
	//	g_Charge_Animation_3d_Y[i] = 0.125f*(rand() % 7);
	//}



	g_SlowEffectCnt = 0;
	g_SlowFlg = false;
	g_Charge_Animation_Flg = false;
	return S_OK;
}

//=============================================================================
// 終了処理
//=============================================================================
void UninitEffect(void)
{
	for (int i = 0; i < EFFECT_MAX; i++) {
		if (g_pVtxBuffEffect[i] != NULL)
		{// 頂点バッファの開放
			g_pVtxBuffEffect[i]->Release();
			g_pVtxBuffEffect[i] = NULL;
		}
	}
}

//=============================================================================
// 更新処理
//=============================================================================
void UpdateEffect(void)
{
	//エフェクトを描画させる処理
	//if (Keyboard_IsTrigger(DIK_1) || g_Charge_Animation_Flg == true) {
	//	g_Charge_Animation_Flg = true;
	//	g_Punch_Val = 0;
	//}

	//エフェクトを描画させる処理
	if (Keyboard_IsTrigger(DIK_2) || g_Wind_Animation_Flg == true) {
		g_Wind_Animation_Flg = true;
		g_Punch_Val = 2;
	}

	//エフェクトを描画させる処理
	if (Keyboard_IsTrigger(DIK_4) || g_Wind_Animation_Flg == true) {
		g_Hit_Animation_Flg = true;
		g_Punch_Val = 0;
	}

	Punch_Charge_Effect_Animation();
	Punch_Wind_Effect_Animation();
	Punch_Hit_Effect_Animation();
	
	//スローのデバッグ
	if (Keyboard_IsPress(DIK_5)) {
		g_SlowFlg = true;
		g_SlowEffectCnt += 20;
		if (g_SlowEffectCnt >= 250) {
			g_SlowEffectCnt = 250;
		}
	}


	else {
		g_SlowFlg = false;
		g_SlowEffectCnt = 0;
	}
}

//=============================================================================
// 描画処理
//=============================================================================
void DrawEffect(void)
{
	if (g_Charge_Animation_Flg == true || g_AnimationShrink_Flg == true || g_Wind_Animation_Flg == true || g_Hit_Animation_Flg == true) {
		LPD3DXBUFFER pD3DXMtrlBuffer = NULL;

		LPDIRECT3DDEVICE9 pEffectDevice = GetD3DDevice();
		D3DXMATRIX EffectMtxScl[EFFECT_MAX], EffectMtxRot[EFFECT_MAX], EffectMtxTranslate[EFFECT_MAX];

		//テクスチャを裏表描画する関数
		//これをやるといろいろおかしくなる
		//pEffectDevice->SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);

		//αテストを有効に
		pEffectDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		pEffectDevice->SetRenderState(D3DRS_ALPHAREF, 0);
		pEffectDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

		for (int j = 0; j < EFFECT_MAX; j++) {
			//ワールドマトリックスの初期化
			D3DXMatrixIdentity(&g_mtxWorldEffect[j]);

			//スケールを反映
			D3DXMatrixScaling(&EffectMtxScl[j], g_SclEffect[j].x, g_SclEffect[j].y, g_SclEffect[j].z);
			D3DXMatrixMultiply(&g_mtxWorldEffect[j], &g_mtxWorldEffect[j], &EffectMtxScl[j]);

			//回転を反映
			D3DXMatrixRotationYawPitchRoll(&EffectMtxRot[j], g_RotEffect[j].x, g_RotEffect[j].y, g_RotEffect[j].z);
			D3DXMatrixMultiply(&g_mtxWorldEffect[j], &g_mtxWorldEffect[j], &EffectMtxRot[j]);

			//移動を反映
			D3DXMatrixTranslation(&EffectMtxTranslate[j], g_PosEffect[j].x, g_PosEffect[j].y, g_PosEffect[j].z);
			D3DXMatrixMultiply(&g_mtxWorldEffect[j], &g_mtxWorldEffect[j], &EffectMtxTranslate[j]);

			//ワールドマトリックスの設定
			pEffectDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldEffect[j]);

			//
			pEffectDevice->SetStreamSource(0, g_pVtxBuffEffect[j], 0, sizeof(VERTEX_3D));

			//
			pEffectDevice->SetFVF(FVF_VERTEX_3D);
			//マテリアルの設定
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
			pEffectDevice->SetMaterial(&mat);

			if (g_Punch_Val == 0) {
				//テクスチャの設定
				pEffectDevice->SetTexture(0, Texture_GetTexture(TEXTURE_INDEX_HIT_EFFECT));
			}
			else if (g_Punch_Val == 1) {
				pEffectDevice->SetTexture(0, Texture_GetTexture(TEXTURE_INDEX_CUT_THE_SKY_SIDE));
			}
			else if (g_Punch_Val == 2) {
				pEffectDevice->SetTexture(0, Texture_GetTexture(TEXTURE_INDEX_CUT_THE_SKY_FRONT));
			}
			//ポリゴンの描画
			pEffectDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, NUM_POLYGON);
		}
		//αテストを有効に
		pEffectDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	}
}



void SlowEffect() {
	if (g_SlowFlg == true) {
		Sprite_Draw(TEXTURE_INDEX_SLOW_EFFECT,
			SCREEN_WIDTH / 2,
			SCREEN_HEIGHT / 2,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			SCREEN_WIDTH,
			SCREEN_HEIGHT,
			SCREEN_WIDTH / 2,
			SCREEN_HEIGHT / 2,
			1.0f,
			1.0f,
			0.0f,
			g_SlowEffectCnt);
	}
}

void DamageEffect() {
	Sprite_Draw(TEXTURE_INDEX_DAMAGE_EFFECT,
		SCREEN_WIDTH / 2,
		SCREEN_HEIGHT / 2,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		SCREEN_WIDTH,
		SCREEN_HEIGHT,
		SCREEN_WIDTH / 2,
		SCREEN_HEIGHT / 2,
		1.0f,
		1.0f,
		0.0f,
		255);
}


//============================================================================
//パンチのチャージするときのエフェクト
//============================================================================
//=============================================================================
// 頂点の作成
//=============================================================================
HRESULT MakeVertexChargeEffect(LPDIRECT3DDEVICE9 pDevice)
{
	for (int i = 0; i < EFFECT_MAX; i++) {
		if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D)*NUM_VERTEX,
			D3DUSAGE_WRITEONLY,
			FVF_VERTEX_3D,
			D3DPOOL_MANAGED,
			&g_pVtxBuffEffect[i],
			NULL)))
		{
			return E_FAIL;
		}
		{
			//頂点バッファの髪を埋める
			//VERTEX_3D *pVtx;
			EFFECT_3D *pVtx;
			//VRAM上の情報を持ってくるから、頂点をいじれるようになる
			g_pVtxBuffEffect[i]->Lock(0, 0, (void**)&pVtx, 0);

			//位置
			pVtx[0].Effect_pos = D3DXVECTOR3(0, HIT_EFFECT_HEIGHT, 0);
			pVtx[1].Effect_pos = D3DXVECTOR3(HIT_EFFECT_WIDTH, HIT_EFFECT_HEIGHT, 0);
			pVtx[2].Effect_pos = D3DXVECTOR3(0, 0, 0);
			pVtx[3].Effect_pos = D3DXVECTOR3(HIT_EFFECT_WIDTH, 0, 0);

			//法線、色
			for (int i = 0; i < 4; i++) {
				pVtx[i].Effect_nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
				pVtx[i].Effect_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			}

			//テクスチャ
			pVtx[0].Effect_tex = D3DXVECTOR2(g_Charge_Animation_3d_X, g_Charge_Animation_3d_Y);
			pVtx[1].Effect_tex = D3DXVECTOR2(g_Charge_Animation_3d_X + g_Charge_Animation_U, g_Charge_Animation_3d_Y);
			pVtx[2].Effect_tex = D3DXVECTOR2(g_Charge_Animation_3d_X, g_Charge_Animation_3d_Y + g_Charge_Animation_V);
			pVtx[3].Effect_tex = D3DXVECTOR2(g_Charge_Animation_3d_X + g_Charge_Animation_U, g_Charge_Animation_3d_Y + g_Charge_Animation_V);

			//回転
			g_RotEffect[0] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

			//ここでVRAMへ帰す
			g_pVtxBuffEffect[i]->Unlock();
		}
	}
	return S_OK;
}

void Punch_Charge_Effect_Animation() {
	//逆に描画する処理
	if (g_Charge_Animation_Flg == true) {
		if (g_NumberResetFlg == true) {
			g_Charge_Animation_3d_X = 0.75f;
			g_Charge_Animation_3d_Y = 0.875f;
			g_NumberResetFlg = false;
		}
		g_Charge_Animation_Rag_Cnt++;
		if (g_Charge_Animation_Rag_Cnt >= EFFECT_SHRINK_SPEED) {
			//上端まで行ってないとき
			if (g_Charge_Animation_3d_Y >= 0.0f) {
				//U値を減らす
				g_Charge_Animation_3d_X -= g_Charge_Animation_U;

				//U値が右端へ行ったとき
				if (g_Charge_Animation_3d_X < 0.0f) {
					g_Charge_Animation_3d_Y -= g_Charge_Animation_V;	//V値を1つずらす
					g_Charge_Animation_3d_X = 0.75f;	//U値を左端へ
				}
			}
			//V値が上端へ行ったとき
			if (g_Charge_Animation_3d_Y < 0.0f) {
				g_Charge_Animation_Rag_Cnt++;
				//20Fのラグを作る
				if (g_Charge_Animation_Rag_Cnt >= 10) {
					g_Charge_Animation_3d_X = 0.0f;
					g_Charge_Animation_3d_Y = 0.0f;
					g_AnimationShrink_Flg = true;
					g_NumberResetFlg = true;
					g_Charge_Animation_Flg = false;
				}
			}
			else {
				g_Charge_Animation_Rag_Cnt = 0;
			}
			//描画
			for (int i = 0; i < EFFECT_MAX; i++) {
				//頂点バッファの髪を埋める
				//VERTEX_3D *pVtx;
				EFFECT_3D *pVtx;

				//VRAM上の情報を持ってくるから、頂点をいじれるようになる
				g_pVtxBuffEffect[i]->Lock(0, 0, (void**)&pVtx, 0);
				//位置
				pVtx[0].Effect_pos = D3DXVECTOR3(0, HIT_EFFECT_HEIGHT, 0);
				pVtx[1].Effect_pos = D3DXVECTOR3(HIT_EFFECT_WIDTH, HIT_EFFECT_HEIGHT, 0);
				pVtx[2].Effect_pos = D3DXVECTOR3(0, 0, 0);
				pVtx[3].Effect_pos = D3DXVECTOR3(HIT_EFFECT_WIDTH, 0, 0);

				//位置(左手)
				if (GetPunchIndex() == 0|| GetPunchIndex() == 2) {
					g_PosEffect[0] = D3DXVECTOR3((GetEnemy() + 3)->Pos.x - 10, (GetEnemy() + 3)->Pos.y - 9.0, (GetEnemy() + 3)->Pos.z);
					g_SclEffect[1] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				}
				//右手
				else if (GetPunchIndex() == 1|| GetPunchIndex()==3) {
					g_PosEffect[0] = D3DXVECTOR3((GetEnemy() + 2)->Pos.x - 10, (GetEnemy() + 2)->Pos.y - 9.0, (GetEnemy() + 2)->Pos.z);
					g_SclEffect[1] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				}
				//両手
				else if (GetPunchIndex() == 4) {
					g_PosEffect[0] = D3DXVECTOR3((GetEnemy() + 2)->Pos.x - 10, (GetEnemy() + 2)->Pos.y - 9.0, (GetEnemy() + 2)->Pos.z);
					g_PosEffect[1] = D3DXVECTOR3((GetEnemy() + 3)->Pos.x - 10, (GetEnemy() + 3)->Pos.y - 9.0, (GetEnemy() + 3)->Pos.z);
					g_RotEffect[1] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
					g_SclEffect[1] = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
				}
				//テクスチャ
				pVtx[0].Effect_tex = D3DXVECTOR2(g_Charge_Animation_3d_X, g_Charge_Animation_3d_Y);
				pVtx[1].Effect_tex = D3DXVECTOR2(g_Charge_Animation_3d_X + g_Charge_Animation_U, g_Charge_Animation_3d_Y);
				pVtx[2].Effect_tex = D3DXVECTOR2(g_Charge_Animation_3d_X, g_Charge_Animation_3d_Y + g_Charge_Animation_V);
				pVtx[3].Effect_tex = D3DXVECTOR2(g_Charge_Animation_3d_X + g_Charge_Animation_U, g_Charge_Animation_3d_Y + g_Charge_Animation_V);

				//回転
				g_RotEffect[0] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

				//ここでVRAMへ帰す
				g_pVtxBuffEffect[i]->Unlock();
			}

		}
	}

	if (g_AnimationShrink_Flg == true) {

		//値を変更
		if (g_NumberResetFlg == true) {
			g_Charge_Animation_3d_X = 0.0f;
			g_Charge_Animation_3d_Y = 0.0f;
			g_NumberResetFlg = false;
		}
		//U値を増やす
		g_Charge_Animation_3d_X += g_Charge_Animation_U;

		//U値が右端へ行ったとき
		if (g_Charge_Animation_3d_X >= 1.0f) {
			g_Charge_Animation_3d_Y += g_Charge_Animation_V;	//V値を1つずらす
			g_Charge_Animation_3d_X = 0.0f;	//U値を左端へ
		}
		//V値が下端へ行ったとき
		if (g_Charge_Animation_3d_Y >= 1.0f) {
			g_NumberResetFlg = true;
			g_AnimationShrink_Flg = false;
		}

		//描画
		for (int i = 0; i < EFFECT_MAX; i++) {
			//頂点バッファの髪を埋める
			//VERTEX_3D *pVtx;
			EFFECT_3D *pVtx;

			//VRAM上の情報を持ってくるから、頂点をいじれるようになる
			g_pVtxBuffEffect[i]->Lock(0, 0, (void**)&pVtx, 0);

			//位置
			pVtx[0].Effect_pos = D3DXVECTOR3(0, HIT_EFFECT_HEIGHT, 0);
			pVtx[1].Effect_pos = D3DXVECTOR3(HIT_EFFECT_WIDTH, HIT_EFFECT_HEIGHT, 0);
			pVtx[2].Effect_pos = D3DXVECTOR3(0, 0, 0);
			pVtx[3].Effect_pos = D3DXVECTOR3(HIT_EFFECT_WIDTH, 0, 0);

			//位置(左手)
			if (GetPunchIndex() == 0 || GetPunchIndex() == 2) {
				g_PosEffect[0] = D3DXVECTOR3((GetEnemy() + 3)->Pos.x - 10, (GetEnemy() + 3)->Pos.y - 9.0, (GetEnemy() + 3)->Pos.z);
				g_SclEffect[1] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			}
			//右手
			else if (GetPunchIndex() == 1 || GetPunchIndex() == 3) {
				g_PosEffect[0] = D3DXVECTOR3((GetEnemy() + 2)->Pos.x - 10, (GetEnemy() + 2)->Pos.y - 9.0, (GetEnemy() + 2)->Pos.z);
				g_SclEffect[1] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			}
			//両手
			else if (GetPunchIndex() == 4) {
				g_PosEffect[0] = D3DXVECTOR3((GetEnemy() + 2)->Pos.x - 10, (GetEnemy() + 2)->Pos.y - 9.0, (GetEnemy() + 2)->Pos.z);
				g_PosEffect[1] = D3DXVECTOR3((GetEnemy() + 3)->Pos.x - 10, (GetEnemy() + 3)->Pos.y - 9.0, (GetEnemy() + 3)->Pos.z);
				g_RotEffect[1] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				g_SclEffect[1] = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
			}

			//テクスチャ
			pVtx[0].Effect_tex = D3DXVECTOR2(g_Charge_Animation_3d_X, g_Charge_Animation_3d_Y);
			pVtx[1].Effect_tex = D3DXVECTOR2(g_Charge_Animation_3d_X + g_Charge_Animation_U, g_Charge_Animation_3d_Y);
			pVtx[2].Effect_tex = D3DXVECTOR2(g_Charge_Animation_3d_X, g_Charge_Animation_3d_Y + g_Charge_Animation_V);
			pVtx[3].Effect_tex = D3DXVECTOR2(g_Charge_Animation_3d_X + g_Charge_Animation_U, g_Charge_Animation_3d_Y + g_Charge_Animation_V);

			//回転
			g_RotEffect[0] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

			//ここでVRAMへ帰す
			g_pVtxBuffEffect[i]->Unlock();
		}
	}
}

void Punch_Charge_Effect() {
	g_Charge_Animation_Flg = true;
}


//=====================================================================================
//パンチの空を切るときのエフェクト
//=====================================================================================
HRESULT MakeVertexWindEffect(LPDIRECT3DDEVICE9 pDevice)
{
	for (int i = 0; i < EFFECT_MAX; i++) {
		if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D)*NUM_VERTEX,
			D3DUSAGE_WRITEONLY,
			FVF_VERTEX_3D,
			D3DPOOL_MANAGED,
			&g_pVtxBuffEffect[i],
			NULL)))
		{
			return E_FAIL;
		}
		{
			//頂点バッファの髪を埋める
			//VERTEX_3D *pVtx;
			EFFECT_3D *pVtx;
			//VRAM上の情報を持ってくるから、頂点をいじれるようになる
			g_pVtxBuffEffect[i]->Lock(0, 0, (void**)&pVtx, 0);

			//位置
			pVtx[0].Effect_pos = D3DXVECTOR3(0, WIND_EFFECT_HEIGHT, 0);
			pVtx[1].Effect_pos = D3DXVECTOR3(WIND_EFFECT_WIDTH, WIND_EFFECT_HEIGHT, 0);
			pVtx[2].Effect_pos = D3DXVECTOR3(0, 0, 0);
			pVtx[3].Effect_pos = D3DXVECTOR3(WIND_EFFECT_WIDTH, 0, 0);

			//法線、色
			for (int i = 0; i < 4; i++) {
				pVtx[i].Effect_nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
				pVtx[i].Effect_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			}

			//テクスチャ
			pVtx[0].Effect_tex = D3DXVECTOR2(g_Wind_Animation_3d_X, g_Wind_Animation_3d_Y);
			pVtx[1].Effect_tex = D3DXVECTOR2(g_Wind_Animation_3d_X + g_Wind_Animation_U, g_Wind_Animation_3d_Y);
			pVtx[2].Effect_tex = D3DXVECTOR2(g_Wind_Animation_3d_X, g_Wind_Animation_3d_Y + g_Wind_Animation_V);
			pVtx[3].Effect_tex = D3DXVECTOR2(g_Wind_Animation_3d_X + g_Wind_Animation_U, g_Wind_Animation_3d_Y + g_Wind_Animation_V);

			//回転
			g_RotEffect[0] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

			
			//ここでVRAMへ帰す
			g_pVtxBuffEffect[i]->Unlock();
		}
	}
	return S_OK;
}

void Punch_Wind_Effect_Animation() {
	if (g_Wind_Animation_Flg == true) {
		//U値を増やす
		g_Wind_Animation_3d_X += g_Wind_Animation_U;

		//U値が右端へ行ったとき
		if (g_Wind_Animation_3d_X >= 1.0f) {
			g_Wind_Animation_3d_Y += g_Wind_Animation_V;	//V値を1つずらす
			g_Wind_Animation_3d_X = 0.0f;	//U値を左端へ
		}
		//V値が下端へ行ったとき
		if (g_Wind_Animation_3d_Y >= 1.0f) {
			g_Wind_Animation_3d_X = 0.0f;
			g_Wind_Animation_3d_Y = 0.0f;
			g_Wind_Animation_Flg = false;
		}

		//描画
		for (int i = 0; i < EFFECT_MAX; i++) {
			//頂点バッファの髪を埋める
			//VERTEX_3D *pVtx;
			EFFECT_3D *pVtx;

			//VRAM上の情報を持ってくるから、頂点をいじれるようになる
			g_pVtxBuffEffect[i]->Lock(0, 0, (void**)&pVtx, 0);

			//位置
			pVtx[0].Effect_pos = D3DXVECTOR3(0, WIND_EFFECT_HEIGHT, 0);
			pVtx[1].Effect_pos = D3DXVECTOR3(WIND_EFFECT_WIDTH, WIND_EFFECT_HEIGHT, 0);
			pVtx[2].Effect_pos = D3DXVECTOR3(0, 0, 0);
			pVtx[3].Effect_pos = D3DXVECTOR3(WIND_EFFECT_WIDTH, 0, 0);

			//位置(左手)
			if (GetPunchLR() == true) {
				g_PosEffect[0] = D3DXVECTOR3((GetEnemy() + 3)->Pos.x - 34,(GetEnemy() + 3)->Pos.y - 24.0, (GetEnemy() + 3)->Pos.z);
			}
			//右手
			else {
				g_PosEffect[0] = D3DXVECTOR3((GetEnemy() + 2)->Pos.x - 34, (GetEnemy() + 2)->Pos.y - 24.0, (GetEnemy() + 2)->Pos.z);
			}
			//テクスチャ
			pVtx[0].Effect_tex = D3DXVECTOR2(g_Wind_Animation_3d_X, g_Wind_Animation_3d_Y);
			pVtx[1].Effect_tex = D3DXVECTOR2(g_Wind_Animation_3d_X + g_Wind_Animation_U, g_Wind_Animation_3d_Y);
			pVtx[2].Effect_tex = D3DXVECTOR2(g_Wind_Animation_3d_X, g_Wind_Animation_3d_Y + g_Wind_Animation_V);
			pVtx[3].Effect_tex = D3DXVECTOR2(g_Wind_Animation_3d_X + g_Wind_Animation_U, g_Wind_Animation_3d_Y + g_Wind_Animation_V);

			//回転
			g_RotEffect[0] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			
			//ここでVRAMへ帰す
			g_pVtxBuffEffect[i]->Unlock();
		}
	}
}


void Punch_Wind_Effect() {
	g_Wind_Animation_Flg = true;
}


//=====================================================================================
//パンチのヒットエフェクト
//=====================================================================================
HRESULT MakeVertexHitEffect(LPDIRECT3DDEVICE9 pDevice)
{
	for (int i = 0; i < EFFECT_MAX; i++) {
		if (FAILED(pDevice->CreateVertexBuffer(sizeof(VERTEX_3D)*NUM_VERTEX,
			D3DUSAGE_WRITEONLY,
			FVF_VERTEX_3D,
			D3DPOOL_MANAGED,
			&g_pVtxBuffEffect[i],
			NULL)))
		{
			return E_FAIL;
		}
		{
			//頂点バッファの髪を埋める
			//VERTEX_3D *pVtx;
			EFFECT_3D *pVtx;
			//VRAM上の情報を持ってくるから、頂点をいじれるようになる
			g_pVtxBuffEffect[i]->Lock(0, 0, (void**)&pVtx, 0);

			//位置
			pVtx[0].Effect_pos = D3DXVECTOR3(0, WIND_EFFECT_HEIGHT, 0);
			pVtx[1].Effect_pos = D3DXVECTOR3(WIND_EFFECT_WIDTH, WIND_EFFECT_HEIGHT, 0);
			pVtx[2].Effect_pos = D3DXVECTOR3(0, 0, 0);
			pVtx[3].Effect_pos = D3DXVECTOR3(WIND_EFFECT_WIDTH, 0, 0);

			//法線、色
			for (int i = 0; i < 4; i++) {
				pVtx[i].Effect_nor = D3DXVECTOR3(0.0f, 1.0f, 0.0f);
				pVtx[i].Effect_col = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
			}

			//テクスチャ
			pVtx[0].Effect_tex = D3DXVECTOR2(g_Wind_Animation_3d_X, g_Wind_Animation_3d_Y);
			pVtx[1].Effect_tex = D3DXVECTOR2(g_Wind_Animation_3d_X + g_Wind_Animation_U, g_Wind_Animation_3d_Y);
			pVtx[2].Effect_tex = D3DXVECTOR2(g_Wind_Animation_3d_X, g_Wind_Animation_3d_Y + g_Wind_Animation_V);
			pVtx[3].Effect_tex = D3DXVECTOR2(g_Wind_Animation_3d_X + g_Wind_Animation_U, g_Wind_Animation_3d_Y + g_Wind_Animation_V);

			//回転
			g_RotEffect[0] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);


			//ここでVRAMへ帰す
			g_pVtxBuffEffect[i]->Unlock();
		}
	}
	return S_OK;
}

void Punch_Hit_Effect_Animation() {
	if (g_Hit_Animation_Flg == true) {
		//U値を増やす
		g_Hit_Animation_3d_X += g_Hit_Animation_U;

		//U値が右端へ行ったとき
		if (g_Hit_Animation_3d_X >= 1.0f) {
			g_Hit_Animation_3d_Y += g_Hit_Animation_V;	//V値を1つずらす
			g_Hit_Animation_3d_X = 0.0f;	//U値を左端へ
		}
		//V値が下端へ行ったとき
		if (g_Hit_Animation_3d_Y >= 1.0f) {
			g_Hit_Animation_3d_X = 0.0f;
			g_Hit_Animation_3d_Y = 0.0f;
			g_Hit_Animation_Flg = false;
		}

		//描画
		for (int i = 0; i < EFFECT_MAX; i++) {
			//頂点バッファの髪を埋める
			//VERTEX_3D *pVtx;
			EFFECT_3D *pVtx;

			//VRAM上の情報を持ってくるから、頂点をいじれるようになる
			g_pVtxBuffEffect[i]->Lock(0, 0, (void**)&pVtx, 0);

			//位置
			pVtx[0].Effect_pos = D3DXVECTOR3(0, HIT_EFFECT_HEIGHT, 0);
			pVtx[1].Effect_pos = D3DXVECTOR3(HIT_EFFECT_WIDTH, HIT_EFFECT_HEIGHT, 0);
			pVtx[2].Effect_pos = D3DXVECTOR3(0, 0, 0);
			pVtx[3].Effect_pos = D3DXVECTOR3(HIT_EFFECT_WIDTH, 0, 0);

			//位置
			g_PosEffect[0] = D3DXVECTOR3(GetEnemy()->Pos.x, GetEnemy()->Pos.y, GetEnemy()->Pos.z -4);
			g_PosEffect[1] = D3DXVECTOR3(GetEnemy()->Pos.x + 10, GetEnemy()->Pos.y + 10, GetEnemy()->Pos.z);
			g_PosEffect[2] = D3DXVECTOR3(GetEnemy()->Pos.x - 10, GetEnemy()->Pos.y - 10, GetEnemy()->Pos.z);


			//大きさ
			g_SclEffect[0] = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
			g_SclEffect[1] = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
			g_SclEffect[2] = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
			//テクスチャ
			pVtx[0].Effect_tex = D3DXVECTOR2(g_Hit_Animation_3d_X, g_Hit_Animation_3d_Y);
			pVtx[1].Effect_tex = D3DXVECTOR2(g_Hit_Animation_3d_X + g_Hit_Animation_U, g_Hit_Animation_3d_Y);
			pVtx[2].Effect_tex = D3DXVECTOR2(g_Hit_Animation_3d_X, g_Hit_Animation_3d_Y + g_Hit_Animation_V);
			pVtx[3].Effect_tex = D3DXVECTOR2(g_Hit_Animation_3d_X + g_Hit_Animation_U, g_Hit_Animation_3d_Y + g_Hit_Animation_V);

			//回転
			g_RotEffect[0] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

			//ここでVRAMへ帰す
			g_pVtxBuffEffect[i]->Unlock();
		}
	}
}


void Punch_Hit_Effect() {
	g_Hit_Animation_Flg = true;
}

