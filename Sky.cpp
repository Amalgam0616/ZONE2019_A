#include "sky.h"
#include "myDirect3D.h"
#include "input.h"
#include "texture.h"
#include "debugproc.h"
#include "enemy.h"
#include "texture.h"
#include "sprite.h"
#include "time.h"
#include "score.h"
//*****************************************************************************
// マクロ定義
//*****************************************************************************
#define ENEMY_HEAD		(1)					// エネミーの頭を指定
#define MODEL_NUM		(3)					//モデルの数
#define	MODEL_MOVE		(10.0f)				// 移動量

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void SetModel(int ModelNum, D3DXVECTOR3 Pos, D3DXVECTOR3 Rot, D3DXVECTOR3 Scale);
void DrawModel();
void PlusModelPos(int ModelNum, float x, float y, float z);
void PlusModelRot(int ModelNum, float x, float y, float z);
void InitUFO(LPDIRECT3DDEVICE9 pDevice);
void InitBGSky(LPDIRECT3DDEVICE9 pDevice);
void DrawUFO();

//*****************************************************************************
// グローバル変数
//*****************************************************************************
LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffSky = NULL;	//頂点バッファへのポインタ
D3DXMATRIXA16			g_mtxWorldModel;

D3DXMATRIX				g_mtxWorldSky;			// ワールドマトリックス
D3DXVECTOR3				g_posSky;				// 空の位置
D3DXVECTOR3				g_rotSky;				// 空の向き(回転)
D3DXVECTOR3				g_sclSky;				// 空の大きさ(スケール)
float					ty;						// テクスチャの現在指定値(縦）
float					PlusY;					// 空をスクロールさせるのに使用,この値分スクロールが進んでいく
float					PlusX;					// 雲を斜めに動かすのに使用 この値分ななめ移動が進んでいく
bool					CloudFlag;				//雲スプライトを描画するかしないかのフラグ
bool					AirplaneFlag;			//3Dオブジェクトを描画するかしないかのフラグ
bool					SatelliteFlag;			//3Dオブジェクトを描画するかしないかのフラグ
bool					MoonFlag;				//3Dオブジェクトを描画するかしないかのフラグ
int						CntFrame;				//フレームが一定値を超えるごとに足すカウンタ

LPDIRECT3DVERTEXBUFFER9 g_pVtxBuffUFO = NULL;	// 頂点バッファへのポインタ
D3DXMATRIX				g_mtxWorldUFO;			// ワールドマトリックス
D3DXVECTOR3				g_posUFO;				// UFOの位置
D3DXVECTOR3				g_rotUFO;				// UFOの向き(回転)
D3DXVECTOR3				g_sclUFO;				// UFOの大きさ(スケール)

float					AddUFO_Y;				// UFOのY座標にこれを足す。
float					AddUFO_Z;				// UFOのZ座標にこれを足す。

bool					UFO_AwayFlag;			// UFOがひゅぃって移動するかしないかのフラグ、やっつけ仕事

int						CopyScore;				// スコアの値を保持し、どこで止まるかを判断するのに使用
bool					DrawStopFlag;			// 止めないと座標足されたりしちゃって進んじゃうからそれを止めるために作られたフラグ、無理やり
//=========================================================================
//モデルの数分用意
XMODEL g_SkyModel[MODEL_NUM] = {};

//=============================================================================
// 初期化処理
//=============================================================================
HRESULT InitSky(void)
{
	srand((unsigned int)time(NULL));

	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();

	//UFOオブジェクトの頂点作成と初期化
	InitUFO(pDevice);
	//背景の空オブジェクトの頂点作成と初期化
	InitBGSky(pDevice);


	//モデルを読み込み
	if (FAILED(LoadXFile(&g_SkyModel[0], XFILE_INDEX_AIRPLANE)) ||
		FAILED(LoadXFile(&g_SkyModel[1], XFILE_INDEX_SATELLITE)) ||
		FAILED(LoadXFile(&g_SkyModel[2], XFILE_INDEX_MOON)))
	{
		return E_FAIL;
	}

	//================================== 各モデルの初期化
	//飛行機
	g_SkyModel[0].Pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_SkyModel[0].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
	g_SkyModel[0].Scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	//人工衛星
	g_SkyModel[1].Pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_SkyModel[1].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(45.0f), D3DXToRadian(45.0f));
	g_SkyModel[1].Scale = D3DXVECTOR3(0.3f, 0.3f, 0.3f);

	//月
	g_SkyModel[2].Pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_SkyModel[2].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
	g_SkyModel[2].Scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);

	//===================================

	ty = 0.005f;
	PlusY = 0.0f;
	PlusX = 0.0f;

	CloudFlag = false;
	AirplaneFlag = false;
	SatelliteFlag = false;
	MoonFlag = false;
	CntFrame = 0;
	AddUFO_Y = 0.0f;
	AddUFO_Z = 0.0f;
	UFO_AwayFlag = false;

	CopyScore = 0;
	DrawStopFlag = false;

	return S_OK;
}
//=============================================================================
// 終了処理
//=============================================================================
void UninitSky(void)
{
	if (g_pVtxBuffSky != NULL)
	{// 頂点バッファの開放
		g_pVtxBuffSky->Release();
		g_pVtxBuffSky = NULL;
	}
	//メッシュ情報を解放
	for (int i = 0; i < MODEL_NUM; i++)
	{
		SAFE_RELEASE(g_SkyModel[i].pMesh);
	}
}
//=============================================================================
// 更新処理
//=============================================================================
void UpdateSky(void)
{
	if (!DrawStopFlag)
	{
		//背景の空の位置設定
		g_posSky.x = -15.0f;
		g_posSky.y = (GetEnemy() + ENEMY_HEAD)->Pos.y;
		g_posSky.z = (GetEnemy() + ENEMY_HEAD)->Pos.z;

		g_posUFO = D3DXVECTOR3(10.0f, (GetEnemy() + ENEMY_HEAD)->Pos.y + AddUFO_Y, (GetEnemy() + ENEMY_HEAD)->Pos.z + 200.0f + AddUFO_Z);

		//空のテクスチャを↑に進めていく
		ty += 0.0015f;

		if (ty < 1.0f)
		{
			{	//頂点バッファの中身を埋める
				VERTEX_3D *pVtx;

				// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
				g_pVtxBuffSky->Lock(0, 0, (void**)&pVtx, 0);

				// テクスチャ座標の設定
				pVtx[0].tex = D3DXVECTOR2(0.0f, 1.0f - ty);
				pVtx[1].tex = D3DXVECTOR2(1.0f, 1.0f - ty);
				pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f - ty + 0.005f);
				pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f - ty + 0.005f);

				// 頂点データをアンロックする
				g_pVtxBuffSky->Unlock();
			}
		}
	}
}
//=============================================================================
// 空描画処理
//=============================================================================
void DrawSky(void)
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();
	D3DXMATRIX mtxView, mtxScale, mtxTranslate, mtxRot;
	//アルファテストを有効に
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	//ライティングを無効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&g_mtxWorldSky);

	//ビューマトリックスを取得
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	//POLYGONを正面に向ける
#if 1
	//逆行列を求める
	D3DXMatrixInverse(&g_mtxWorldSky, NULL, &mtxView);
	g_mtxWorldSky._41 = 0.0f;
	g_mtxWorldSky._42 = 0.0f;
	g_mtxWorldSky._43 = 0.0f;
#else
	g_mtxWorldSky._11 = mtxViex._11;
	g_mtxWorldSky._12 = mtxViex._21;
	g_mtxWorldSky._13 = mtxViex._31;
	g_mtxWorldSky._21 = mtxViex._12;
	g_mtxWorldSky._22 = mtxViex._22;
	g_mtxWorldSky._23 = mtxViex._32;
	g_mtxWorldSky._31 = mtxViex._13;
	g_mtxWorldSky._32 = mtxViex._23;
	g_mtxWorldSky._33 = mtxViex._33;
#endif

	//スケールを反映
	D3DXMatrixScaling(&mtxScale, g_sclSky.x, g_sclSky.y, g_sclSky.z);
	D3DXMatrixMultiply(&g_mtxWorldSky, &g_mtxWorldSky, &mtxScale);

	//移動を反映
	D3DXMatrixTranslation(&mtxTranslate, g_posSky.x, g_posSky.y, g_posSky.z);
	D3DXMatrixMultiply(&g_mtxWorldSky, &g_mtxWorldSky, &mtxTranslate);

	//回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotSky.y, g_rotSky.z, g_rotSky.x);
	D3DXMatrixMultiply(&g_mtxWorldSky, &g_mtxWorldSky, &mtxRot);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldSky);

	//頂点バッファをデバイスのデータストリームにバインド
	pDevice->SetStreamSource(0, g_pVtxBuffSky, 0, sizeof(VERTEX_3D));

	//頂点フォーマットの設定
	pDevice->SetFVF(FVF_VERTEX_3D);

	D3DMATERIAL9 mat = {};

	mat.Diffuse.r = 1.0f;
	mat.Diffuse.g = 1.0f;
	mat.Diffuse.b = 1.0f;
	mat.Diffuse.a = 1.0f;

	mat.Ambient.r = 1.0f;
	mat.Ambient.g = 0.0f;
	mat.Ambient.b = 1.0f;
	mat.Ambient.a = 1.0f;

	//マテリアルセット
	pDevice->SetMaterial(&mat);

	//テクスチャの設定
	pDevice->SetTexture(0, Texture_GetTexture(TEXTURE_INDEX_SKY));

	//POLYGONの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 12);

	//ライティングを有効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	//アルファテストを無効に
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);


	//==============================雲なんかいい感じに誰かやってくれまじで

	//会場突き破って90フレームで雲描画
	if (!DrawStopFlag && Getg_PunchFrameCnt() >= 90)
	{
		//雲のじゅうたん
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, -4100.0f + PlusX, -3900.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 4300.0f + PlusX, -3900.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 4500.0f + PlusX, -3900.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 4000.0f + PlusX, -3900.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, -4100.0f + PlusX, -3800.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 4300.0f + PlusX, -3800.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 4500.0f + PlusX, -3800.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 4700.0f + PlusX, -3800.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, -4300.0f + PlusX, -3900.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 4500.0f + PlusX, -3900.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 4700.0f + PlusX, -3900.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 4900.0f + PlusX, -3900.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, -4300.0f + PlusX, -3800.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 4500.0f + PlusX, -3800.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 4700.0f + PlusX, -3800.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 4900.0f + PlusX, -3800.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, -4500.0f + PlusX, -3900.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 4700.0f + PlusX, -3900.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 4900.0f + PlusX, -3900.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 5100.0f + PlusX, -3900.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, -4500.0f + PlusX, -3800.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 4700.0f + PlusX, -3800.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 4900.0f + PlusX, -3800.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 5100.0f + PlusX, -3800.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, -4700.0f + PlusX, -3900.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 4900.0f + PlusX, -3900.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 5100.0f + PlusX, -3900.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 5300.0f + PlusX, -3900.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, -4700.0f + PlusX, -3800.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 4900.0f + PlusX, -3800.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 5100.0f + PlusX, -3800.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 5300.0f + PlusX, -3800.0f + PlusY);

		//雲ばらばら
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 3500.0f + PlusX, -2400.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 3000.0f + PlusX, -1900.0f + PlusY);
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 1300.0f + PlusX, -1000.0f + PlusY);		//ひだりはじ
		Sprite_Draw(TEXTURE_INDEX_CLOUD2, 900.0f + PlusX, -300.0f + PlusY);	//中央

		PlusY += 20.0f;
		PlusX -= 20.0f;
	}


	//230フレームで飛行機描画
	if (Getg_PunchFrameCnt() >= 230)
	{
		if (!AirplaneFlag)
		{
			AirplaneFlag = true;
			SetModel
			(
				0,
				D3DXVECTOR3((GetEnemy() + ENEMY_HEAD)->Pos.x + 2.0f, (GetEnemy() + ENEMY_HEAD)->Pos.y + 400.0f, (GetEnemy() + ENEMY_HEAD)->Pos.z + 200.0f),
				D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(20.0f), D3DXToRadian(20.0f)),
				D3DXVECTOR3(0.3f, 0.3f, 0.3f)
			);
		}
		else
		{
			if (!DrawStopFlag)
			{
				//飛行機の座標を動かす
				PlusModelPos(0, 0.0f, -5.0f, -2.5f);
			}
		}
	}

	//330(329) + 140フレームで人工衛星描画
	if (AirplaneFlag == true && Getg_PunchFrameCnt() >= 80 && CntFrame == 1)
	{
		if (!SatelliteFlag)
		{
			SatelliteFlag = true;
			SetModel
			(
				1,
				D3DXVECTOR3((GetEnemy() + ENEMY_HEAD)->Pos.x + 2.0f, (GetEnemy() + ENEMY_HEAD)->Pos.y + 400.0f, (GetEnemy() + ENEMY_HEAD)->Pos.z + 200.0f),
				D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(60.0f), D3DXToRadian(45.0f)),
				D3DXVECTOR3(3.0f, 3.0f, 3.0f)
			);
		}
		else
		{
			if (!DrawStopFlag)
			{
				//人工衛星の座標を動かす
				PlusModelPos(1, 0.0f, -4.5f, -2.0f);
				PlusModelRot(1, D3DXToRadian(0.0f), D3DXToRadian(0.5f), D3DXToRadian(0.0f));
			}
		}
	}

	if (!DrawStopFlag)
	{
		//330(329) + 250フレームで月描画
		if (SatelliteFlag == true && Getg_PunchFrameCnt() >= 250 && CntFrame == 1)
		{
			if (!MoonFlag)
			{
				MoonFlag = true;
				SetModel
				(
					2,
					D3DXVECTOR3((GetEnemy() + ENEMY_HEAD)->Pos.x, (GetEnemy() + ENEMY_HEAD)->Pos.y + 68.0f, (GetEnemy() + ENEMY_HEAD)->Pos.z + 32.0f),
					D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(45.0f)),
					D3DXVECTOR3(3.0f, 3.0f, 3.0f)
				);
			}
			else
			{
				if (!DrawStopFlag)
				{
					//月の角度をちょい動かす
					PlusModelRot(2, D3DXToRadian(0.0f), D3DXToRadian(0.001f), D3DXToRadian(0.0f));
				}
			}
		}
	}

	//モデルの描画
	DrawModel();


	if (!DrawStopFlag)
	{
		//==============================UFOの描画
		//ランダムに飛行機よりフレーム進んでから表示する
		if (AirplaneFlag == true && Getg_PunchFrameCnt() >= rand() % 150 + 60 && CntFrame == 1)
		{
			//描画
			DrawUFO();

			//UFOをカクカク移動させる処理。無理やり感満載
			if (!UFO_AwayFlag)
			{
				if (g_posUFO.z >= 220.0f)
				{
					AddUFO_Z -= 10.0f;
				}
				else if (g_posUFO.z < 220.0f)
				{
					AddUFO_Y -= 6.0f;
					AddUFO_Z -= 5.5f;

					if (g_posUFO.y <= 740.0f && g_posUFO.z <= 162.0f)
					{
						UFO_AwayFlag = true;
					}
				}
			}
			else
			{
				AddUFO_Y += 8.0f;
				AddUFO_Z -= 0.5f;
			}
		}

		if (Getg_PunchFrameCnt() == 329)
		{
			CntFrame++;
		}
	}
}

//=============================
// 3Dモデルの描画
//=============================
void DrawModel()
{
	//Direct3DDeviceの取得
	LPDIRECT3DDEVICE9 p_D3DDevice = GetD3DDevice();

	//ワールドトランスフォーム(絶対座標変換)
	D3DXMatrixIdentity(&g_mtxWorldSky);

	if (AirplaneFlag)
	{
		//ワールドマトリクス作って適用
		p_D3DDevice->SetTransform(D3DTS_WORLD, MakeWorldMatrix(&g_SkyModel[0], &g_mtxWorldModel));

		//マテリアルの数だけ繰り返し
		for (DWORD j = 0; j < g_SkyModel[0].dwNumMaterials; j++)
		{
			//マテリアルセット
			p_D3DDevice->SetMaterial(&g_SkyModel[0].pMaterials[j]);
			//テクスチャセット
			p_D3DDevice->SetTexture(0, g_SkyModel[0].pTextures[j]);
			//透過度設定
			g_SkyModel[0].pMaterials[j].Diffuse.a = 1.0f;
			//いわゆる描画
			g_SkyModel[0].pMesh->DrawSubset(j);
		}
	}
	if (SatelliteFlag)
	{
		//ワールドマトリクス作って適用
		p_D3DDevice->SetTransform(D3DTS_WORLD, MakeWorldMatrix(&g_SkyModel[1], &g_mtxWorldModel));

		//マテリアルの数だけ繰り返し
		for (DWORD j = 0; j < g_SkyModel[1].dwNumMaterials; j++)
		{
			//マテリアルセット
			p_D3DDevice->SetMaterial(&g_SkyModel[1].pMaterials[j]);
			//テクスチャセット
			p_D3DDevice->SetTexture(0, g_SkyModel[1].pTextures[j]);
			//透過度設定
			g_SkyModel[1].pMaterials[j].Diffuse.a = 1.0f;
			//いわゆる描画
			g_SkyModel[1].pMesh->DrawSubset(j);
		}
	}
	if (MoonFlag)
	{
		//ワールドマトリクス作って適用
		p_D3DDevice->SetTransform(D3DTS_WORLD, MakeWorldMatrix(&g_SkyModel[2], &g_mtxWorldModel));

		//マテリアルの数だけ繰り返し
		for (DWORD j = 0; j < g_SkyModel[2].dwNumMaterials; j++)
		{
			//マテリアルセット
			p_D3DDevice->SetMaterial(&g_SkyModel[2].pMaterials[j]);
			//テクスチャセット
			p_D3DDevice->SetTexture(0, g_SkyModel[2].pTextures[j]);
			//透過度設定
			g_SkyModel[2].pMaterials[j].Diffuse.a = 1.0f;
			//いわゆる描画
			g_SkyModel[2].pMesh->DrawSubset(j);
		}
	}
}

//====================================
// モデルのPos,Rot,Scaleをセットする
//====================================
void SetModel(int ModelNum, D3DXVECTOR3 Pos, D3DXVECTOR3 Rot, D3DXVECTOR3 Scale)
{
	g_SkyModel[ModelNum].Pos = Pos;
	g_SkyModel[ModelNum].Rot = Rot;
	g_SkyModel[ModelNum].Scale = Scale;
}
//====================================
// モデルのPosを足す
//====================================
void PlusModelPos(int ModelNum, float x, float y, float z)
{
	g_SkyModel[ModelNum].Pos.x += x;
	g_SkyModel[ModelNum].Pos.y += y;
	g_SkyModel[ModelNum].Pos.z += z;
}
//====================================
// モデルのRotを足す
//====================================
void PlusModelRot(int ModelNum, float x, float y, float z)
{
	g_SkyModel[ModelNum].Rot.x += x;
	g_SkyModel[ModelNum].Rot.y += y;
	g_SkyModel[ModelNum].Rot.z += z;
}

//===========================================
//  月モデルの使用フラグ取得関数
//===========================================
bool GetMoonFlag()
{
	return MoonFlag;
}
//===========================================
//  衛星モデルの使用フラグ取得関数
//===========================================
bool GetSatelliteFlag()
{
	return SatelliteFlag;
}
//===========================================
//  飛行機モデルの使用フラグ取得関数
//===========================================
bool GetAirplaneFlag()
{
	return AirplaneFlag;
}

//=======================================================================
//空のモデルの状態取得
//=======================================================================
XMODEL* GetSkyModel()
{
	return g_SkyModel;
}
//=======================================================
// 2DのUFOを描画する頂点の作成(Init)、長くなるから分けた
//=======================================================
void InitUFO(LPDIRECT3DDEVICE9 pDevice)
{
	// オブジェクトの頂点バッファを生成
	if (FAILED(pDevice->CreateVertexBuffer
	(
		sizeof(VERTEX_3D) * NUM_VERTEX,	// 頂点データ用に確保するバッファサイズ(バイト単位)
		D3DUSAGE_WRITEONLY,			// 頂点バッファの使用法　
		FVF_VERTEX_3D,				// 使用する頂点フォーマット
		D3DPOOL_MANAGED,			// リソースのバッファを保持するメモリクラスを指定
		&g_pVtxBuffUFO,				// 頂点バッファインターフェースへのポインタ
		NULL)))						// NULLに設定
	{
	}
	{//頂点バッファの中身を埋める
		VERTEX_3D *pVtx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		g_pVtxBuffUFO->Lock(0, 0, (void**)&pVtx, 0);

		// 頂点座標の設定
		pVtx[0].pos = D3DXVECTOR3(-100.0f / 2, 50.0f / 2, 0.0f);
		pVtx[1].pos = D3DXVECTOR3(100.0f / 2, 50.0f / 2, 0.0f);
		pVtx[2].pos = D3DXVECTOR3(-100.0f / 2, -50.0f / 2, 0.0f);
		pVtx[3].pos = D3DXVECTOR3(100.0f / 2, -50.0f / 2, 0.0f);

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
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(1.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 1.0f);
		pVtx[3].tex = D3DXVECTOR2(1.0f, 1.0f);

		// 頂点データをアンロックする
		g_pVtxBuffUFO->Unlock();
	}

	//UFOの初期設定===================================================
	g_posUFO = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_sclUFO = D3DXVECTOR3(0.2f, 0.2f, 0.2f);
	g_rotUFO = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
}

//=======================================================
// 2Dの空を描画する頂点の作成(Init)、長くなるから分けた
//=======================================================
void InitBGSky(LPDIRECT3DDEVICE9 pDevice)
{
	// オブジェクトの頂点バッファを生成
	if (FAILED(pDevice->CreateVertexBuffer
	(
		sizeof(VERTEX_3D) * NUM_VERTEX,	// 頂点データ用に確保するバッファサイズ(バイト単位)
		D3DUSAGE_WRITEONLY,			// 頂点バッファの使用法　
		FVF_VERTEX_3D,				// 使用する頂点フォーマット
		D3DPOOL_MANAGED,			// リソースのバッファを保持するメモリクラスを指定
		&g_pVtxBuffSky,				// 頂点バッファインターフェースへのポインタ
		NULL)))						// NULLに設定
	{
	}

	{//頂点バッファの中身を埋める
		VERTEX_3D *pVtx;

		// 頂点データの範囲をロックし、頂点バッファへのポインタを取得
		g_pVtxBuffSky->Lock(0, 0, (void**)&pVtx, 0);

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
		pVtx[0].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[1].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[2].tex = D3DXVECTOR2(0.0f, 0.0f);
		pVtx[3].tex = D3DXVECTOR2(0.0f, 0.0f);

		// 頂点データをアンロックする
		g_pVtxBuffSky->Unlock();
	}

	//背景の空の初期設定===================================================
	g_posSky = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_sclSky = D3DXVECTOR3(0.1f, 20.0f, 20.0f);
	g_rotSky = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(6.0f));
}

//=================================================
// 2DのUFOを描画する関数(Draw)、長くなるから分けた
//=================================================
void DrawUFO()
{
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();
	D3DXMATRIX mtxView, mtxScale, mtxTranslate, mtxRot;
	//アルファテストを有効に
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
	pDevice->SetRenderState(D3DRS_ALPHAREF, 0);
	pDevice->SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

	//ライティングを無効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, FALSE);

	//ワールドマトリックスの初期化
	D3DXMatrixIdentity(&g_mtxWorldUFO);

	//ビューマトリックスを取得
	pDevice->GetTransform(D3DTS_VIEW, &mtxView);

	//POLYGONを正面に向ける
#if 1
	//逆行列を求める
	D3DXMatrixInverse(&g_mtxWorldUFO, NULL, &mtxView);
	g_mtxWorldUFO._41 = 0.0f;
	g_mtxWorldUFO._42 = 0.0f;
	g_mtxWorldUFO._43 = 0.0f;
#else
	g_mtxWorldUFO._11 = mtxViex._11;
	g_mtxWorldUFO._12 = mtxViex._21;
	g_mtxWorldUFO._13 = mtxViex._31;
	g_mtxWorldUFO._21 = mtxViex._12;
	g_mtxWorldUFO._22 = mtxViex._22;
	g_mtxWorldUFO._23 = mtxViex._32;
	g_mtxWorldUFO._31 = mtxViex._13;
	g_mtxWorldUFO._32 = mtxViex._23;
	g_mtxWorldUFO._33 = mtxViex._33;
#endif

	//スケールを反映
	D3DXMatrixScaling(&mtxScale, g_sclUFO.x, g_sclUFO.y, g_sclUFO.z);
	D3DXMatrixMultiply(&g_mtxWorldUFO, &g_mtxWorldUFO, &mtxScale);

	//移動を反映
	D3DXMatrixTranslation(&mtxTranslate, g_posUFO.x, g_posUFO.y, g_posUFO.z);
	D3DXMatrixMultiply(&g_mtxWorldUFO, &g_mtxWorldUFO, &mtxTranslate);

	//回転を反映
	D3DXMatrixRotationYawPitchRoll(&mtxRot, g_rotUFO.y, g_rotUFO.z, g_rotUFO.x);
	D3DXMatrixMultiply(&g_mtxWorldUFO, &g_mtxWorldUFO, &mtxRot);

	// ワールドマトリックスの設定
	pDevice->SetTransform(D3DTS_WORLD, &g_mtxWorldUFO);

	//頂点バッファをデバイスのデータストリームにバインド
	pDevice->SetStreamSource(0, g_pVtxBuffUFO, 0, sizeof(VERTEX_3D));

	//頂点フォーマットの設定
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

	//テクスチャの設定
	pDevice->SetTexture(0, Texture_GetTexture(TEXTURE_INDEX_UFO));

	//POLYGONの描画
	pDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 12);

	//ライティングを有効にする
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	//アルファテストを無効に
	pDevice->SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);

}

//===============================================
// これを呼び出すと描画を現在の状態で止められる
//===============================================
void SetDrawStop()
{
	DrawStopFlag = true;
}
//========================================================================================
// 吹っ飛ぶ前のスコア（純得点）のゲッター、ほんとはscoreかなんかに持たせたかった、はい
//========================================================================================
int GetCopy_Score()
{
	return CopyScore;
}
//=======================================================================================================
// 吹っ飛ぶ前のスコア（純得点）をCopyScoreに保持するやつ、ほんとはscoreかなんかに持たせたかった、はい
//=======================================================================================================
void SetCopy_Score()
{
	CopyScore = GetScore();
}