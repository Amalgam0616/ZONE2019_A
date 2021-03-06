
#include "myDirect3D.h"
#include "input.h"
#include "Xfile.h"
#include "enemy.h"
#include "debugproc.h"
#include "Effect.h"
#include <time.h>	//乱数シード作るようなので、消すならこれも
#include "texture.h"
#include "player.h"

//定数定義 ==========================================================================

//プレイヤーに使うモデル数
#define		ENEMY_MODEL_NUM	(7)

//敵の初期座標Z
#define ENEMY_POS_Z (26.0f)

//アッパーで吹っ飛ぶスピード
#define UPPERED_SPEED (10.0f)

//スローエリア座標
#define		SLOW_IN		(23.0f)
#define		SLOW_OUT	(15.0f)

//1ゲーム当たりのパンチ数
#define		NUM_PUNCH		(10)

//パンチの種類ごとにSTARTから準備完了までのフレーム数を設定
#define		PPI_R_P_START_FRAME		(74)	//右通常パンチ
#define		PPI_L_P_START_FRAME		(74)	//左通常パンチ
#define		FLASH_P_START_FRAME		(74)	//瞬間移動パンチ
#define		PPI_D_P_START_FRAME		(74)	//左通常パンチ

//瞬間移動パンチのスローまでのフレーム
#define		FLASH_P_SLOW_FRAME		(74)

//グローバル変数定義 =================================================================

bool g_Huttobi;

//プレイヤーのXMODEL宣言初期化
//0:胴　1:頭　2:左手　3:右手　4:左足　5:右足
XMODEL g_Enemy[ENEMY_MODEL_NUM] = {};

//ワールド座標
static D3DXMATRIXA16 g_mtxWorld;

//待機モーション取らせるのでそれ用のヤツ(ほぼプレイヤーと同じの)
//アニメーションのリスト
static PL_ANIME g_EnmAnime =
{
	//ANIME_INDEX_MORMAL　待機モーション
	15, 15, D3DXVECTOR3(0.0f, D3DXToRadian(0.0f), 0.0f),D3DXVECTOR3(0.0f, 1.5f, 0.0f)
};

//アニメーション関連
//フレームカウンタ
int g_EnmAnimFrameCnt;
//待機状態か否か
bool isTaiki;
//1フレーム当たりの角度変化量
D3DXVECTOR3 g_EnmAnglePerFrame;
//1フレーム当たりの座標変化量
D3DXVECTOR3 g_EnmMovePerFrame;
//足を動かす系のヤツ(だと思う)
bool EnmLeg_Flg;


//パンチパターン
//０＝右ストレート　１＝左パンチ　２＝瞬間移動パンチ
int g_PunchPattern[NUM_PUNCH];

//パンチ中か否か
bool Punch_Flg;
//パンチのフェーズ管理
PUNCH_PHASE g_PunchPhase;
//撃っているパンチの種類の管理
PUNCH_PATTERN_INDEX g_Punch_Pattern_Index;
//パンチのフレームカウンタ
int g_PunchFrameCnt;
//パンチのスピード
float Punch_Speed;
//パンチ終了Z座標
float g_PunchEndLine;

//瞬間移動字の速度倍率
static float g_Slow_Multi;
//パンチ開始Z座標
//0= 右　1= 左
float g_PunchStartLine[2];
//今動いている手がどっちなのか
//L=true R=false
bool PunchLR;
//今のパンチの回数
static int g_PunchCnt;
//待機時間
int g_WaitTime;
//モデルの初期位置
static D3DXVECTOR3 g_ModelStaPos[ENEMY_MODEL_NUM - 1] = {};
static D3DXVECTOR3 g_ModelStaRot[ENEMY_MODEL_NUM - 1] = {};
//パンチ帰宅ベクトル
static D3DXVECTOR3 g_PunchEndVec[ENEMY_MODEL_NUM - 1] = {};
//パンチ帰宅角度
static D3DXVECTOR3 g_PunchEndRot[ENEMY_MODEL_NUM - 1] = {};

//パンチ終了から元の位置に戻るまでのフレーム数のリスト
static const int g_EndFrameList[PPI_MAX] =
{
	15,	//右通常パンチ
	15,	//左通常パンチ
	0,	//右瞬間移動パンチ
	0,	//左瞬間移動パンチ
	10,	//両手バーンてするパンチ
};



//スローフラグ(いる)
bool SlowFlg;

//基本関数定義 ======================================================================

//初期化
HRESULT InitEnemy()
{
	//乱数シード生成(他のとこでやるならここのは消してください)
	srand((unsigned int)time(NULL));

	//読み込み・初期値設定
	{
		//XFileから3Dモデルをロード
		if (FAILED(LoadXFile(&g_Enemy[0], XFILE_INDEX_E_BODY_001)) ||		//胴体
			FAILED(LoadXFile(&g_Enemy[1], XFILE_INDEX_E_HEAD_001)) ||		//頭
			FAILED(LoadXFile(&g_Enemy[2], XFILE_INDEX_E_GROBE_L_001)) ||	//左手(前からみて右)
			FAILED(LoadXFile(&g_Enemy[3], XFILE_INDEX_E_GROBE_R_001)) ||	//右手(前からみて左)
			FAILED(LoadXFile(&g_Enemy[4], XFILE_INDEX_E_LEG_L_001)) ||		//左足(前からみて右)
			FAILED(LoadXFile(&g_Enemy[5], XFILE_INDEX_E_LEG_R_001)) ||		//右足(前からみて左)
			FAILED(LoadXFile(&g_Enemy[6], XFILE_INDEX_E_DMGBODY_001)))		//くの字の胴体
		{
			//失敗したらここ
			return E_FAIL;
		}

		//座標・回転・拡縮　初期値設定
		EnemyPosReset();

		for (int i = 0; i < ENEMY_MODEL_NUM; i++)
		{
			g_Enemy[i].Scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
		}
	}

	//スローフラグ
	SlowFlg = false;

	//パンチパターン作成
	for (int i = 0; i < NUM_PUNCH; i++)
	{
		g_PunchPattern[i] = rand() % (PPI_MAX - 1);
	}

	//パンチ中か否か
	Punch_Flg = false;
	//パンチスピード
	Punch_Speed = 3.0f;

	//瞬間移動の速度倍率
	g_Slow_Multi = 1.0f;

	//パンチのフェーズ管理
	g_PunchPhase = PUNCH_PHASE_CHARGE;
	//撃っているパンチの種類の管理
	g_Punch_Pattern_Index = PUNCH_NULL;
	//パンチ終了Z座標(これがあるので、敵のInitはプレイヤーより後に行ってください)
	g_PunchEndLine = (GetPlayer() + 1)->Pos.z;
	//パンチスタート位置記録
	g_PunchStartLine[0] = g_Enemy[3].Pos.z;
	g_PunchStartLine[1] = g_Enemy[2].Pos.z;

	//パンチ回数初期化
	g_PunchCnt = 0;

	//モデル初期位置保存
	for (int i = 0; i < ENEMY_MODEL_NUM - 1; i++)
	{
		g_ModelStaPos[i] = g_Enemy[i].Pos;
		g_ModelStaRot[i] = g_Enemy[i].Rot;
	}

	//待機時間初期化
	g_WaitTime = rand() % 60 + 120;

	//アッパー待機状態か否か
	g_Huttobi = false;

	//アニメーション関連
	g_EnmAnimFrameCnt = 0;
	isTaiki = true;
	g_EnmAnglePerFrame = {};
	g_EnmMovePerFrame = {};
	EnmLeg_Flg = false;

	return S_OK;
}

//終了
void UninitEnemy()
{
	//メッシュの解放
	for (int i = 0; i < ENEMY_MODEL_NUM; i++)
	{
		SAFE_RELEASE(g_Enemy[i].pMesh);
	}
}

//更新
void UpdateEnemy()
{
	static int i = 0;

	//回避：スロー関連
	{
		//回避が入力されていない且つ、パンチ中だったらパンチパターンごとにZ座標を見て、スローエリア内だったらスロー
		if (!GetDodgeFlg())
		{
			//パンチ本体中のみ
			if (g_PunchPhase == PUNCH_PHASE_SWING)
			{
				//パンチ回数目のパンチパターンを見て、if文分岐
				if (g_PunchPattern[g_PunchCnt] == PPI_RIGHT_PUNCH)
				{//右通常パンチ
					if (g_Enemy[3].Pos.z < SLOW_IN && g_Enemy[3].Pos.z > SLOW_OUT && !SlowFlg)
					{
						SlowFlg = true;
					}
					else if (g_Enemy[3].Pos.z < SLOW_OUT && SlowFlg)
					{
						SlowFlg = false;
					}
				}
				else if (g_PunchPattern[g_PunchCnt] == PPI_LEFT_PUNCH)
				{//左通常パンチ
					if (g_Enemy[2].Pos.z < SLOW_IN && g_Enemy[2].Pos.z > SLOW_OUT && !SlowFlg)
					{
						SlowFlg = true;
					}
					else if (g_Enemy[2].Pos.z < SLOW_OUT&& SlowFlg)
					{
						SlowFlg = false;
					}
				}
				else if (g_PunchPattern[g_PunchCnt] == R_FLASH_PUNCH)
				{//瞬間移動パンチ
				}
				else if (g_PunchPattern[g_PunchCnt] == PPI_DUNK_PUNCH)
				{//両手バーンてするパンチ
					if (g_Enemy[2].Pos.z < (SLOW_IN - 3.0f) && g_Enemy[2].Pos.z >(SLOW_OUT + 2.0f) && !SlowFlg)
					{
						SlowFlg = true;
					}
					else if (g_Enemy[2].Pos.z < (SLOW_OUT + 2.0f) && SlowFlg)
					{
						SlowFlg = false;
					}
				}
			}
		}

		//正しい方向の回避が入力されたらスロー解除
		if (GetDodgeFlg())
		{
			SlowFlg = false;
		}
	}

	//待機中だったら待機モーションをやる
	if (isTaiki)
	{
		//アニメ―ション進行
		EnmAnimation();
		//フレーム進行
		g_EnmAnimFrameCnt++;	//ここで　1　以上になる
	}

	//パンチ関連
	{
		//アッパー待機中は動かない
		if (!g_Huttobi)
		{
			//条件が来たらアニメーションを終了してPosReset、からのパンチ
			if (!Punch_Flg && g_WaitTime <= 0)
			{
				//アニメ終了：初期値にセット
				isTaiki = false;
				EnemyPosReset();
				//パンチ中にする
				Punch_Flg = true;
			}
			else
			{//待機時間進行
				g_WaitTime--;
			}

			if (Punch_Flg)
			{
				//パンチ回数目のパンチパターンを見て、if分分岐
				if (g_PunchPattern[g_PunchCnt] == PPI_RIGHT_PUNCH)
				{//右通常パンチ
					g_Punch_Pattern_Index = PPI_RIGHT_PUNCH;
					RightPunch();
				}
				else if (g_PunchPattern[g_PunchCnt] == PPI_LEFT_PUNCH)
				{//左通常パンチ
					g_Punch_Pattern_Index = PPI_LEFT_PUNCH;
					LeftPunch();
				}
				else if (g_PunchPattern[g_PunchCnt] == R_FLASH_PUNCH)
				{//瞬間移動パンチ

				//避けたら全体的に加速
					if (GetDodgeFlg())
					{
						g_Slow_Multi = 3.0f;
						g_PunchFrameCnt++;
					}

					g_Punch_Pattern_Index = R_FLASH_PUNCH;
					Right_R_FLASH_PUNCH();
				}
				else if (g_PunchPattern[g_PunchCnt] == L_FLASH_PUNCH)
				{//瞬間移動パンチ

				//避けたら全体的に加速
					if (GetDodgeFlg())
					{
						g_Slow_Multi = 3.0f;
						g_PunchFrameCnt++;
					}

					g_Punch_Pattern_Index = L_FLASH_PUNCH;
					Left_R_FLASH_PUNCH();
				}
				else if (g_PunchPattern[g_PunchCnt] == PPI_DUNK_PUNCH)
				{//両手バーンてするパンチ
					g_Punch_Pattern_Index = PPI_DUNK_PUNCH;
					DunkPunch();
				}
			}
		}
	}

	//プレイヤーのアッパーフェーズに合わせて吹っ飛ばす
	if (GetUpper_Phase() == 1)
	{
		if (!g_Huttobi)
		{
			//吹っ飛び待ち
			EnemyUpperPos();

			g_Huttobi = true;

			//アニメーションをやめる
			isTaiki = false;
		}
	}
	else if (GetUpper_Phase() == 3)
	{
		//吹っ飛ばす
		for (int i = 1; i < 7; i++)
		{
			g_Enemy[i].Pos += D3DXVECTOR3(0.0f, UPPERED_SPEED, UPPERED_SPEED * 0.7);
		}
	}


	//デバッグ用
	DebugProc_Print((char *)"パンチ終了ラインZ座標：[%f]\n", g_PunchEndLine);
	DebugProc_Print((char *)"g_Punch_Pattern_Index ：[%d]\n", g_Punch_Pattern_Index);
	DebugProc_Print((char *)"座標：[X:%f Y:%f Z:%f]\n", g_Enemy[i].Pos.x, g_Enemy[i].Pos.y, g_Enemy[i].Pos.z);
	DebugProc_Print((char *)"角度：[X:%f Y:%f Z:%f]\n", D3DXToDegree(g_Enemy[i].Rot.x), D3DXToDegree(g_Enemy[i].Rot.y), D3DXToDegree(g_Enemy[i].Rot.z));
	DebugProc_Print((char *)"SlowFlg：[%d]\n", SlowFlg);
	DebugProc_Print((char *)"パンチ種類：[%d]\n", g_PunchPattern[g_PunchCnt]);
	DebugProc_Print((char *)"パンチ回数(デバッグ時はループ)：[%d]\n", g_PunchCnt);
	DebugProc_Print((char *)"今回のパンチ種類一覧：[%d,%d,%d,%d,%d,%d,%d,%d,%d,%d]\n", g_PunchPattern[0], g_PunchPattern[1], g_PunchPattern[2], g_PunchPattern[3], g_PunchPattern[4], g_PunchPattern[5], g_PunchPattern[6], g_PunchPattern[7], g_PunchPattern[8], g_PunchPattern[9]);

	//以下デバック用
	static int a = 0;
	if (Keyboard_IsTrigger(DIK_O))
	{
		R_FLASH_PUNCH_R();
	}

	if (Keyboard_IsPress(DIK_I))
	{
		EnemyPosReset();
	}

	//デバック用（回転/シフト押しながらだと移動）
	if (Keyboard_IsPress(DIK_LEFT))
	{
		if (Keyboard_IsPress(DIK_LSHIFT))
		{
			g_Enemy[i].Pos.x -= 0.3f;
		}
		else
		{
			g_Enemy[i].Rot.y += D3DXToRadian(0.3f);
		}
	}

	if (Keyboard_IsPress(DIK_RIGHT))
	{
		if (Keyboard_IsPress(DIK_LSHIFT))
		{
			g_Enemy[i].Pos.x += 0.3f;
		}
		else
		{
			g_Enemy[i].Rot.y -= D3DXToRadian(0.3f);
		}
	}

	if (Keyboard_IsPress(DIK_UP))
	{
		if (Keyboard_IsPress(DIK_LSHIFT))
		{
			g_Enemy[i].Pos.y += 0.3f;
		}
		else if (Keyboard_IsPress(DIK_Z))
		{
			g_Enemy[i].Pos.z += 0.3f;
		}
		else
		{
			g_Enemy[i].Rot.x += D3DXToRadian(0.3f);
		}
	}

	if (Keyboard_IsPress(DIK_DOWN))
	{
		if (Keyboard_IsPress(DIK_LSHIFT))
		{
			g_Enemy[i].Pos.y -= 0.3f;
		}
		else if (Keyboard_IsPress(DIK_Z))
		{
			g_Enemy[i].Pos.z -= 0.3f;
		}
		else
		{
			g_Enemy[i].Rot.x -= D3DXToRadian(0.3f);
		}
	}

	if (Keyboard_IsTrigger(DIK_N))
	{
		i++;
	}

	if (i > 5)
	{
		i = 0;
	}
}

//描画
void DrawEnemy()
{
	//Direct3DDeviceの取得
	LPDIRECT3DDEVICE9 p_D3DDevice = GetD3DDevice();

	//ワールドトランスフォーム(絶対座標変換)
	D3DXMatrixIdentity(&g_mtxWorld);

	if (GetUpper_Phase() != 3)
	{
		//モデル分繰り返す
		for (int j = 0; j < ENEMY_MODEL_NUM - 1; j++)
		{
			//ワールドマトリクス作って適用
			p_D3DDevice->SetTransform(D3DTS_WORLD, MakeWorldMatrix(&g_Enemy[j], &g_mtxWorld));

			//マテリアルの数だけ繰り返し
			for (DWORD i = 0; i < g_Enemy[j].dwNumMaterials; i++)
			{
				//マテリアルセット
				p_D3DDevice->SetMaterial(&g_Enemy[j].pMaterials[i]);
				//テクスチャセット
				p_D3DDevice->SetTexture(0, g_Enemy[j].pTextures[i]);
				//透過度設定
				g_Enemy[j].pMaterials[i].Diffuse.a = 1.0f;
				//いわゆる描画
				g_Enemy[j].pMesh->DrawSubset(i);
			}
		}
	}
	else
	{
		//モデル分繰り返す
		for (int j = 1; j < ENEMY_MODEL_NUM; j++)
		{
			//ワールドマトリクス作って適用
			p_D3DDevice->SetTransform(D3DTS_WORLD, MakeWorldMatrix(&g_Enemy[j], &g_mtxWorld));

			//マテリアルの数だけ繰り返し
			for (DWORD i = 0; i < g_Enemy[j].dwNumMaterials; i++)
			{
				//マテリアルセット
				p_D3DDevice->SetMaterial(&g_Enemy[j].pMaterials[i]);
				//テクスチャセット
				p_D3DDevice->SetTexture(0, g_Enemy[j].pTextures[i]);
				//透過度設定
				g_Enemy[j].pMaterials[i].Diffuse.a = 1.0f;
				//いわゆる描画
				g_Enemy[j].pMesh->DrawSubset(i);
			}
		}
	}
}

//その他いろんな関数定義 =============================================================

//無理矢理アニメーションさせる関数
void EnmAnimation()
{
	//g_AnimFrame　が　0　以上且つ　StrFrame　以下だった場合は、アニメステート毎に動かす部位を決めて１フレーム分動かす
	if ((g_EnmAnimFrameCnt >= 0 + 1) && (g_EnmAnimFrameCnt <= g_EnmAnime.StrFrame + 1))
	{
		//g_AnimFrameCnt　が　0　だった場合は、モデルの座標・回転を初期位置に戻して、アニメステート毎に１フレーム当たりの動きを計算
		if (g_EnmAnimFrameCnt == 0 + 1)
		{
			EnemyPosReset();
			EnmLeg_Flg = true;

			//1フレーム当たりの角度
			g_EnmAnglePerFrame = D3DXVECTOR3(g_EnmAnime.Angle.x / g_EnmAnime.StrFrame,
				g_EnmAnime.Angle.y / g_EnmAnime.StrFrame,
				g_EnmAnime.Angle.z / g_EnmAnime.StrFrame);

			//1フレーム当たりの移動
			g_EnmMovePerFrame = D3DXVECTOR3(g_EnmAnime.Move.x / g_EnmAnime.StrFrame,
				g_EnmAnime.Move.y / g_EnmAnime.StrFrame,
				g_EnmAnime.Move.z / g_EnmAnime.StrFrame);
		}

		//待機モーション　頭、胴、両腕　を動かす
		for (int i = 0; i < ENEMY_MODEL_NUM - 3; i++)
		{
			//座標移動
			g_Enemy[i].Pos += g_EnmMovePerFrame;
			//角度変化
			g_Enemy[i].Rot += g_EnmAnglePerFrame;
		}
	}
	//g_AnimFrameCnt　が　StrFrame + 1　以上且つ　StrFrame + EndFrame　以下だった場合は、アニメステート毎に動かす部位を決めて１フレーム分動かす
	else if ((g_EnmAnimFrameCnt >= g_EnmAnime.StrFrame + 2) && (g_EnmAnimFrameCnt <= (g_EnmAnime.StrFrame + g_EnmAnime.EndFrame + 1)))
	{
		//g_AnimFrameCnt　が　StrFrame　より　1　大きい場合は、1フレーム当たりの移動距離を戻り用に再計算する
		if (g_EnmAnimFrameCnt == (g_EnmAnime.StrFrame + 2))
		{
			EnmLeg_Flg = false;
			//角度　全体の角度に戻して、帰りのフレーム数で割って、反転
			g_EnmAnglePerFrame = D3DXVECTOR3((g_EnmAnime.Angle.x / g_EnmAnime.EndFrame) * -1.0f,
				(g_EnmAnime.Angle.y / g_EnmAnime.EndFrame) * -1.0f,
				(g_EnmAnime.Angle.z / g_EnmAnime.EndFrame) * -1.0f);

			//移動量　全体の移動量に戻して、帰りのフレーム数で割って、反転
			g_EnmMovePerFrame = D3DXVECTOR3((g_EnmAnime.Move.x / g_EnmAnime.EndFrame) * -1.0f,
				(g_EnmAnime.Move.y / g_EnmAnime.EndFrame) * -1.0f,
				(g_EnmAnime.Move.z / g_EnmAnime.EndFrame) * -1.0f);
		}

		//待機モーション　頭、胴、両腕　を動かす
		for (int i = 0; i < ENEMY_MODEL_NUM - 3; i++)
		{
			//座標移動
			g_Enemy[i].Pos += g_EnmMovePerFrame;
			//角度変化
			g_Enemy[i].Rot += g_EnmAnglePerFrame;
		}
	}
	//g_AnimFrameCnt　が　StrFrame + EndFrame + 1　以上だった場合は、アニメーションを終了して待機モーションに戻す
	else if (g_EnmAnimFrameCnt >= (g_EnmAnime.StrFrame + g_EnmAnime.EndFrame + 2))
	{
		//g_AnimFrameCntを0に戻す
		g_EnmAnimFrameCnt = 0;
		//フレーム当たりの移動回転を0に戻す
		g_EnmAnglePerFrame = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
		g_EnmMovePerFrame = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	}
}

//エネミーの初期位置に戻す関数
void EnemyPosReset()
{
	//胴体
	g_Enemy[0].Pos = D3DXVECTOR3(0.0f, 8.0f, ENEMY_POS_Z);
	g_Enemy[0].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
	//頭
	g_Enemy[1].Pos = D3DXVECTOR3(0.0f, 23.0f, ENEMY_POS_Z);
	g_Enemy[1].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
	//左腕
	g_Enemy[2].Pos = D3DXVECTOR3(7.0f, 12.0f, ENEMY_POS_Z - 4.0f);
	g_Enemy[2].Rot = D3DXVECTOR3(D3DXToRadian(15.0f), D3DXToRadian(180.0f), D3DXToRadian(90.0f));
	//右腕
	g_Enemy[3].Pos = D3DXVECTOR3(-7.0f, 15.0f, 26.0f);
	g_Enemy[3].Rot = D3DXVECTOR3(D3DXToRadian(15.0f), D3DXToRadian(180.0f), D3DXToRadian(-90.0f));
	//左足
	g_Enemy[4].Pos = D3DXVECTOR3(8.0f, -6.0f, ENEMY_POS_Z);
	g_Enemy[4].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
	//右足
	g_Enemy[5].Pos = D3DXVECTOR3(-6.0f, -5.85f, ENEMY_POS_Z);
	g_Enemy[5].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
}

//エネミーをアッパー受ける位置に変える関数
void EnemyUpperPos()
{
	//胴体
	g_Enemy[0].Pos = D3DXVECTOR3(0.0f, 5.0f, ENEMY_POS_Z);
	g_Enemy[0].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
	//頭
	g_Enemy[1].Pos = D3DXVECTOR3(g_Enemy[1].Pos.x, g_Enemy[1].Pos.y - 7, g_Enemy[1].Pos.z - 5.0f);
	g_Enemy[1].Rot = D3DXVECTOR3(D3DXToRadian(340.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
	//左腕
	g_Enemy[2].Pos = D3DXVECTOR3(g_Enemy[2].Pos.x, g_Enemy[2].Pos.y, g_Enemy[2].Pos.z - 8.0f);
	g_Enemy[2].Rot = D3DXVECTOR3(D3DXToRadian(90.0f), D3DXToRadian(90.0f), D3DXToRadian(0.0f));
	//右腕
	g_Enemy[3].Pos = D3DXVECTOR3(g_Enemy[3].Pos.x, g_Enemy[3].Pos.y, g_Enemy[3].Pos.z - 8.0f);
	g_Enemy[3].Rot = D3DXVECTOR3(D3DXToRadian(90.0f), D3DXToRadian(270.0f), D3DXToRadian(0.0f));
	//左足
	g_Enemy[4].Pos = D3DXVECTOR3(g_Enemy[4].Pos.x, g_Enemy[4].Pos.y, g_Enemy[4].Pos.z - 6.0f);
	g_Enemy[4].Rot = D3DXVECTOR3(D3DXToRadian(20.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
	//右足
	g_Enemy[5].Pos = D3DXVECTOR3(g_Enemy[5].Pos.x, g_Enemy[5].Pos.y, g_Enemy[5].Pos.z - 6.0f);
	g_Enemy[5].Rot = D3DXVECTOR3(D3DXToRadian(20.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));

	//アッパーされた時の胴体
	g_Enemy[6].Pos = D3DXVECTOR3(0.0f, 5.0f, ENEMY_POS_Z);
	g_Enemy[6].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
}

void R_FLASH_PUNCH_R()
{
	//胴体
	g_Enemy[0].Pos = R_FLASH_PUNCH_R_POS0;
	g_Enemy[0].Rot = R_FLASH_PUNCH_R_ROT0;
	//頭
	g_Enemy[1].Pos = R_FLASH_PUNCH_R_POS1;
	g_Enemy[1].Rot = R_FLASH_PUNCH_R_ROT1;
	//左腕
	g_Enemy[2].Pos = R_FLASH_PUNCH_R_POS2;
	g_Enemy[2].Rot = R_FLASH_PUNCH_R_ROT2;
	//右腕
	g_Enemy[3].Pos = R_FLASH_PUNCH_R_POS3;
	g_Enemy[3].Rot = R_FLASH_PUNCH_R_ROT3;
	//左足
	g_Enemy[4].Pos = R_FLASH_PUNCH_R_POS4;
	g_Enemy[4].Rot = R_FLASH_PUNCH_R_ROT4;
	//右足
	g_Enemy[5].Pos = R_FLASH_PUNCH_R_POS5;
	g_Enemy[5].Rot = R_FLASH_PUNCH_R_ROT5;
}

void L_FLASH_PUNCH_R()
{
	//胴体
	g_Enemy[0].Pos = L_FLASH_PUNCH_POS0;
	g_Enemy[0].Rot = L_FLASH_PUNCH_ROT0;
	//頭
	g_Enemy[1].Pos = L_FLASH_PUNCH_POS1;
	g_Enemy[1].Rot = L_FLASH_PUNCH_ROT1;
	//左腕
	g_Enemy[2].Pos = L_FLASH_PUNCH_POS2;
	g_Enemy[2].Rot = L_FLASH_PUNCH_ROT2;
	//右腕
	g_Enemy[3].Pos = L_FLASH_PUNCH_POS3;
	g_Enemy[3].Rot = L_FLASH_PUNCH_ROT3;
	//左足
	g_Enemy[4].Pos = L_FLASH_PUNCH_POS4;
	g_Enemy[4].Rot = L_FLASH_PUNCH_ROT4;
	//右足
	g_Enemy[5].Pos = L_FLASH_PUNCH_POS5;
	g_Enemy[5].Rot = L_FLASH_PUNCH_ROT5;
}

//敵のパンチを設定する関数群
//No.01のが見本です。基本はこれに従えば作れます(多分)

//右通常パンチ(PPI_RIGHT_PUNCH)(作成者：Amalgam、No.01　このコメントはパンチ関数群を作る場合に踏襲してください)
void RightPunch()
{
	//目標地点へのベクトル
	static D3DXVECTOR2 dir;

	//フレーム進行
	g_PunchFrameCnt++;

	if (g_PunchPhase == PUNCH_PHASE_CHARGE)
	{//予備動作
		if (g_PunchFrameCnt < PPI_R_P_START_FRAME)
		{
			if (g_PunchFrameCnt < 30)
			{
				//右手を引く予備動作
				g_Enemy[3].Pos.z += 8.0f / PPI_R_P_START_FRAME;
			}

			if (g_PunchFrameCnt == 1)
			{
				Punch_Charge_Effect();
				g_Enemy[3].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(285.0f), D3DXToRadian(10.0f));
				PunchLR = true;
			}
		}
		else
		{
			//プレイヤーの頭へのベクトルを出す
			dir.x = 0.0f - g_Enemy[3].Pos.x;
			dir.y = 8.0f - g_Enemy[3].Pos.y;
			D3DXVec2Normalize(&dir, &dir);

			//フェーズ進行
			g_PunchPhase = PUNCH_PHASE_SWING;
		}
	}
	else if (g_PunchPhase == PUNCH_PHASE_SWING)
	{//パンチ本体
		if (g_Enemy[3].Pos.z >= g_PunchEndLine)
		{//エンドラインに到達してなければ動かす(プレイヤーは敵からみてZマイナス方向にいる)
			if (SlowFlg)
			{//スローエリア内だったら速度に下方補正
				g_Enemy[3].Pos += D3DXVECTOR3((dir.x / 2.0f) * 0.1f, (dir.y / 2.0f) * 0.1f, -(Punch_Speed / 10.0f));
			}
			else
			{
				g_Enemy[3].Pos += D3DXVECTOR3(dir.x / 2.0f, dir.y / 1.5f, -(Punch_Speed));
			}
		}
		else if (g_Enemy[3].Pos.z < g_PunchEndLine)
		{//到達したら終わり
			//後々つかうので０にする
			g_PunchFrameCnt = 0;
			//フェーズ進行
			g_PunchPhase = PUNCH_PHASE_RETURN;
		}
	}
	else if (g_PunchPhase == PUNCH_PHASE_RETURN)
	{
		if (g_PunchFrameCnt == 1)
		{//最初のフレームに戻り用のベクトルを作る
			CreatePunchEndVec();
		}
		else if (g_PunchFrameCnt >= 3)
		{//数フレームまってから元の位置に戻す
			if (g_PunchFrameCnt < g_EndFrameList[g_PunchPattern[g_PunchCnt]])
			{
				for (int i = 0; i < ENEMY_MODEL_NUM - 1; i++)
				{
					g_Enemy[i].Pos -= g_PunchEndVec[i];
					g_Enemy[i].Rot -= g_PunchEndRot[i];
				}
			}
			else
			{//スタート位置に戻ったらパンチ処理を終了する
				//フレーム：フェーズを初期化
				g_PunchFrameCnt = 0;
				g_PunchPhase = PUNCH_PHASE_CHARGE;
				//初期位置に戻す
				EnemyPosReset();

				//パンチ終了
				Punch_Flg = false;
				isTaiki = true;
				g_WaitTime = rand() % 60 + 120;

				//パンチ回数加算(デバッグ用に無限ループする)
				if (g_PunchCnt < NUM_PUNCH - 1)
				{
					g_PunchCnt++;
				}
				else
				{
					g_PunchCnt = 0;
				}
			}
		}
	}
}
//左通常パンチ(PPI_LEFT_PUNCH)(作成者：Amalgam、No.02)
void LeftPunch()
{
	//目標地点へのベクトル
	static D3DXVECTOR2 dir;

	//フレーム進行
	g_PunchFrameCnt++;

	if (g_PunchPhase == PUNCH_PHASE_CHARGE)
	{//予備動作
		if (g_PunchFrameCnt < PPI_L_P_START_FRAME)
		{
			if (g_PunchFrameCnt < 30)
			{
				//右手を引く予備動作
				g_Enemy[2].Pos.z += 8.0f / PPI_R_P_START_FRAME;
			}

			if (g_PunchFrameCnt == 1)
			{
				Punch_Charge_Effect();
				g_Enemy[2].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(75.0f), D3DXToRadian(10.0f));
				PunchLR = false;
			}
		}
		else
		{
			//プレイヤーの頭へのベクトルを出す
			dir.x = 0.0f - g_Enemy[2].Pos.x;
			dir.y = 8.0f - g_Enemy[2].Pos.y;
			D3DXVec2Normalize(&dir, &dir);

			//フェーズ進行
			g_PunchPhase = PUNCH_PHASE_SWING;
		}
	}
	else if (g_PunchPhase == PUNCH_PHASE_SWING)
	{//パンチ本体
		if (g_Enemy[2].Pos.z >= g_PunchEndLine)
		{//エンドラインに到達してなければ動かす(プレイヤーは敵からみてZマイナス方向にいる)
			if (SlowFlg)
			{//スローエリア内だったら速度に下方補正
				g_Enemy[2].Pos += D3DXVECTOR3((dir.x / 2.0f) * 0.1f, (dir.y / 2.0f) * 0.1f, -(Punch_Speed / 10.0f));
			}
			else
			{
				g_Enemy[2].Pos += D3DXVECTOR3(dir.x / 2.0f, dir.y / 1.5f, -(Punch_Speed));
			}
		}
		else if (g_Enemy[2].Pos.z < g_PunchEndLine)
		{//到達したら終わり
			//後々つかうので０にする
			g_PunchFrameCnt = 0;
			//フェーズ進行
			g_PunchPhase = PUNCH_PHASE_RETURN;
		}
	}
	else if (g_PunchPhase == PUNCH_PHASE_RETURN)
	{
		if (g_PunchFrameCnt == 1)
		{//最初のフレームに戻り用のベクトルを作る
			CreatePunchEndVec();
		}
		else if (g_PunchFrameCnt >= 3)
		{//数フレームまってから元の位置に戻す
			if (g_PunchFrameCnt < g_EndFrameList[g_PunchPattern[g_PunchCnt]])
			{
				for (int i = 0; i < ENEMY_MODEL_NUM - 1; i++)
				{
					g_Enemy[i].Pos -= g_PunchEndVec[i];
					g_Enemy[i].Rot -= g_PunchEndRot[i];
				}
			}
			else
			{//スタート位置に戻ったらパンチ処理を終了する
				//フレーム：フェーズを初期化
				g_PunchFrameCnt = 0;
				g_PunchPhase = PUNCH_PHASE_CHARGE;
				//初期位置に戻す
				EnemyPosReset();

				//パンチ終了
				Punch_Flg = false;
				isTaiki = true;
				g_WaitTime = rand() % 60 + 120;

				//パンチ回数加算(デバッグ用に無限ループする)
				if (g_PunchCnt < NUM_PUNCH - 1)
				{
					g_PunchCnt++;
				}
				else
				{
					g_PunchCnt = 0;
				}
			}
		}
	}
}
//右瞬間移動パンチ(R_FLASH_PUNCH)(作成者：凸、No.01)
void Right_R_FLASH_PUNCH()
{
	//フレーム進行
	g_PunchFrameCnt++;

	if (g_PunchPhase == PUNCH_PHASE_CHARGE)
	{//予備動作
		if (g_PunchFrameCnt < FLASH_P_START_FRAME)
		{

		}
		else
		{
			R_FLASH_PUNCH_R();
			g_PunchPhase = PUNCH_PHASE_SWING;
			g_PunchFrameCnt = 0;
			g_Slow_Multi = 1.0f;
		}
		if (g_PunchFrameCnt == 1)
		{
			PunchLR = true;
			Punch_Charge_Effect();
		}

	}
	else if (g_PunchPhase == PUNCH_PHASE_SWING)
	{//パンチ本体
		if (!(g_PunchFrameCnt < 20))
		{
			if (g_PunchFrameCnt < FLASH_P_SLOW_FRAME + 20)
			{
				//現れる
				for (int j = 0; j < ENEMY_MODEL_NUM - 1; j++)
				{
					for (DWORD i = 0; i < g_Enemy[j].dwNumMaterials; i++)
					{
						g_Enemy[j].pMaterials[i].Diffuse.a = 1.0f;
					}
				}


				//移動
				g_Enemy[0].Rot.y -= D3DXToRadian(35.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi);		//35.0fは最終的に回転する総角度
				g_Enemy[2].Pos += D3DXVECTOR3(1.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi, 2.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi, 8.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi);
				g_Enemy[3].Pos += D3DXVECTOR3(28.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi, -20.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi, -35.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi);
				g_Enemy[3].Rot += D3DXVECTOR3(D3DXToRadian(50.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi), D3DXToRadian(-75.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi), 0.0f);
			}
			else
			{//到達したら終わり
				//後々つかうので０にする
				g_PunchFrameCnt = 0;
				//フェーズ進行
				g_PunchPhase = PUNCH_PHASE_RETURN;
			}
		}
		else
		{
			//消える
			for (int j = 0; j < ENEMY_MODEL_NUM - 1; j++)
			{
				for (DWORD i = 0; i < g_Enemy[j].dwNumMaterials; i++)
				{
					g_Enemy[j].pMaterials[i].Diffuse.a = 0.0f;
				}
			}
		}
	}
	else if (g_PunchPhase == PUNCH_PHASE_RETURN)
	{
		if (g_PunchFrameCnt > 3)
		{
			{//スタート位置に戻ったらパンチ処理を終了する
				//フレーム：フェーズを初期化
				g_PunchFrameCnt = 0;
				g_PunchPhase = PUNCH_PHASE_CHARGE;
				//初期位置に戻す
				EnemyPosReset();

				//パンチ終了
				Punch_Flg = false;
				isTaiki = true;
				//パンチを撃っていない状態にもどす
				g_Punch_Pattern_Index = PUNCH_NULL;

				g_WaitTime = rand() % 60 + 120;

				//パンチ回数加算(デバッグ用に無限ループする)
				if (g_PunchCnt < NUM_PUNCH - 1)
				{
					g_PunchCnt++;
				}
				else
				{
					g_PunchCnt = 0;
				}
			}
		}
	}
}
//左瞬間移動パンチ(L_FLASH_PUNCH)(作成者:凸、No.02)
void  Left_R_FLASH_PUNCH()
{
	//フレーム進行
	g_PunchFrameCnt++;

	if (g_PunchPhase == PUNCH_PHASE_CHARGE)
	{//予備動作
		if (g_PunchFrameCnt < FLASH_P_START_FRAME)
		{

		}
		else
		{
			L_FLASH_PUNCH_R();
			g_PunchPhase = PUNCH_PHASE_SWING;
			g_PunchFrameCnt = 0;
			g_Slow_Multi = 1.0f;
		}
		if (g_PunchFrameCnt == 1)
		{
			PunchLR = false;
			Punch_Charge_Effect();
		}
	}
	else if (g_PunchPhase == PUNCH_PHASE_SWING)
	{//パンチ本体
		if (!(g_PunchFrameCnt < 20))
		{
			if (g_PunchFrameCnt < FLASH_P_SLOW_FRAME + 20)
			{
				//現れる
				for (int j = 0; j < ENEMY_MODEL_NUM - 1; j++)
				{
					for (DWORD i = 0; i < g_Enemy[j].dwNumMaterials; i++)
					{
						g_Enemy[j].pMaterials[i].Diffuse.a = 1.0f;
					}
				}

				//移動
				g_Enemy[0].Rot.y -= D3DXToRadian(-35.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi);  //35.0fは最終的に回転する総角度
				g_Enemy[3].Pos += D3DXVECTOR3(-1.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi, 2.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi, 8.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi);
				g_Enemy[2].Pos += D3DXVECTOR3(-23.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi, -20.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi, -36.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi);
				g_Enemy[2].Rot += D3DXVECTOR3(D3DXToRadian(-85.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi), D3DXToRadian(0.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi), D3DXToRadian(-28.0f / FLASH_P_SLOW_FRAME * g_Slow_Multi));

			}
			else
			{//到達したら終わり
			 //後々つかうので０にする
				g_PunchFrameCnt = 0;
				//フェーズ進行
				g_PunchPhase = PUNCH_PHASE_RETURN;
			}
		}
		else
		{
			//消える
			for (int j = 0; j < ENEMY_MODEL_NUM - 1; j++)
			{
				for (DWORD i = 0; i < g_Enemy[j].dwNumMaterials; i++)
				{
					g_Enemy[j].pMaterials[i].Diffuse.a = 0.0f;
				}
			}
		}
	}
	else if (g_PunchPhase == PUNCH_PHASE_RETURN)
	{
		if (g_PunchFrameCnt > 3)
		{
			{//スタート位置に戻ったらパンチ処理を終了する
			 //フレーム：フェーズを初期化
				g_PunchFrameCnt = 0;
				g_PunchPhase = PUNCH_PHASE_CHARGE;
				//初期位置に戻す
				EnemyPosReset();

				//パンチ終了
				Punch_Flg = false;
				isTaiki = true;
				//パンチを撃っていない状態にもどす
				g_Punch_Pattern_Index = PUNCH_NULL;

				g_WaitTime = rand() % 60 + 120;

				//パンチ回数加算(デバッグ用に無限ループする)
				if (g_PunchCnt < NUM_PUNCH - 1)
				{
					g_PunchCnt++;
				}
				else
				{
					g_PunchCnt = 0;
				}
			}
		}
	}
}
//両手バーンてするパンチ(PPI_DUNK_PUNCH)(作成者：Amalgam、No.03)
void DunkPunch()
{
	//目標地点へのベクトル
	static D3DXVECTOR3 dir;

	//フレーム進行
	g_PunchFrameCnt++;

	if (g_PunchPhase == PUNCH_PHASE_CHARGE)
	{//予備動作
		if (g_PunchFrameCnt < PPI_D_P_START_FRAME)
		{
			if (g_PunchFrameCnt < 30)
			{
				//予備動作

				//胴
				g_Enemy[0].Pos.y += 2.0f / 30;
				g_Enemy[0].Rot.x += D3DXToRadian(15.0f) / 30;
				//頭
				g_Enemy[1].Pos.y += 2.0f / 30;
				g_Enemy[1].Pos.z += 2.0f / 30;
				g_Enemy[1].Rot.x += D3DXToRadian(15.0f) / 30;

				//左腕
				g_Enemy[2].Pos += D3DXVECTOR3(3.0f, 20.0f, 4.0f) / 30;
				g_Enemy[2].Rot.x += D3DXToRadian(-20.0f) / 30;
				//右腕
				g_Enemy[3].Pos += D3DXVECTOR3(-3.0f, 20.0f, 4.0f) / 30;
				g_Enemy[3].Rot.x += D3DXToRadian(-20.0f) / 30;
			}

			if (g_PunchFrameCnt == 1)
			{
				//左腕
				g_Enemy[2].Pos = D3DXVECTOR3(7.0f, 15.0f, ENEMY_POS_Z);
				//右腕
				g_Enemy[3].Pos = D3DXVECTOR3(-7.0f, 15.0f, ENEMY_POS_Z);
				Punch_Charge_Effect();
				PunchLR = true;
			}
		}
		else
		{
			//プレイヤーの足元へのベクトルを出す
			dir.y = 0.0f - g_Enemy[3].Pos.y;
			dir.z = g_PunchEndLine - g_Enemy[3].Pos.z;
			D3DXVec3Normalize(&dir, &dir);

			//後々つかうので０にする
			g_PunchFrameCnt = 0;
			//フェーズ進行
			g_PunchPhase = PUNCH_PHASE_SWING;
		}
	}
	else if (g_PunchPhase == PUNCH_PHASE_SWING)
	{//パンチ本体
		if (g_Enemy[3].Pos.z >= g_PunchEndLine)
		{//エンドラインに到達してなければ動かす(プレイヤーは敵からみてZマイナス方向にいる)
			if (SlowFlg)
			{//スローエリア内だったら速度に下方補正
				//頭、胴の移動・回転
				g_Enemy[0].Pos.y -= 0.05f * 0.2f;
				g_Enemy[0].Rot.x -= D3DXToRadian(0.5f) * 0.2f;

				g_Enemy[1].Pos.y -= 0.08f * 0.2f;
				g_Enemy[1].Pos.z -= 0.1f * 0.2f;
				g_Enemy[1].Rot.x -= D3DXToRadian(0.5f) * 0.2f;

				//左腕
				g_Enemy[2].Pos.x += 0.0f;
				g_Enemy[2].Pos.y += dir.y * 0.2f;
				g_Enemy[2].Pos.z += dir.z * 0.2f;
				//右腕
				g_Enemy[3].Pos.x += 0.0f;
				g_Enemy[3].Pos.y += dir.y * 0.2f;
				g_Enemy[3].Pos.z += dir.z * 0.2f;
			}
			else
			{
				//頭、胴の移動・回転
				g_Enemy[0].Pos.y -= 0.05f * 1.5f;
				g_Enemy[0].Rot.x -= D3DXToRadian(0.5f) * 1.5f;

				g_Enemy[1].Pos.y -= 0.08f * 1.5f;
				g_Enemy[1].Pos.z -= 0.1f * 1.5f;
				g_Enemy[1].Rot.x -= D3DXToRadian(0.5f) * 1.5f;

				//左腕
				g_Enemy[2].Pos.x += 0.0f;
				g_Enemy[2].Pos.y += dir.y * 3.0f;
				g_Enemy[2].Pos.z += dir.z * 3.0f;
				//右腕
				g_Enemy[3].Pos.x += 0.0f;
				g_Enemy[3].Pos.y += dir.y * 3.0f;
				g_Enemy[3].Pos.z += dir.z * 3.0f;
			}

			//腕の回転
			if (g_PunchFrameCnt < 10)
			{
				g_Enemy[3].Rot.x += D3DXToRadian(90.0f) / 10;
				g_Enemy[2].Rot.x += D3DXToRadian(90.0f) / 10;
			}
		}
		else
		{//到達したら終わり
			//後々つかうので０にする
			g_PunchFrameCnt = 0;
			//フェーズ進行
			g_PunchPhase = PUNCH_PHASE_RETURN;
		}
	}
	else if (g_PunchPhase == PUNCH_PHASE_RETURN)
	{
		if (g_PunchFrameCnt == 1)
		{//最初のフレームに戻り用のベクトルを作る
			CreatePunchEndVec();
		}
		else if (g_PunchFrameCnt >= 3)
		{//数フレームまってから元の位置に戻す
			if (g_PunchFrameCnt < g_EndFrameList[g_PunchPattern[g_PunchCnt]])
			{
				for (int i = 0; i < ENEMY_MODEL_NUM - 1; i++)
				{
					g_Enemy[i].Pos -= g_PunchEndVec[i];
					g_Enemy[i].Rot -= g_PunchEndRot[i];
				}
			}
			else
			{//スタート位置に戻ったらパンチ処理を終了する
				//フレーム：フェーズを初期化
				g_PunchFrameCnt = 0;
				g_PunchPhase = PUNCH_PHASE_CHARGE;
				//初期位置に戻す
				EnemyPosReset();

				//パンチ終了
				Punch_Flg = false;
				isTaiki = true;
				g_WaitTime = rand() % 60 + 120;

				//パンチ回数加算(デバッグ用に無限ループする)
				if (g_PunchCnt < NUM_PUNCH - 1)
				{
					g_PunchCnt++;
				}
				else
				{
					g_PunchCnt = 0;
				}
			}
		}
	}
}

//パンチ後に元の位置に戻すやつ作るやつ
void CreatePunchEndVec()
{
	//各成分ごとに距離出してフレーム数で割る
	for (int i = 0; i < ENEMY_MODEL_NUM - 1; i++)
	{
		//距離
		g_PunchEndVec[i].x = (g_Enemy[i].Pos.x - g_ModelStaPos[i].x) / g_EndFrameList[g_PunchPattern[g_PunchCnt]];
		g_PunchEndVec[i].y = (g_Enemy[i].Pos.y - g_ModelStaPos[i].y) / g_EndFrameList[g_PunchPattern[g_PunchCnt]];
		g_PunchEndVec[i].z = (g_Enemy[i].Pos.z - g_ModelStaPos[i].z) / g_EndFrameList[g_PunchPattern[g_PunchCnt]];

		//角度
		g_PunchEndRot[i].x = (g_Enemy[i].Rot.x - g_ModelStaRot[i].x) / g_EndFrameList[g_PunchPattern[g_PunchCnt]];
		g_PunchEndRot[i].y = (g_Enemy[i].Rot.y - g_ModelStaRot[i].y) / g_EndFrameList[g_PunchPattern[g_PunchCnt]];
		g_PunchEndRot[i].z = (g_Enemy[i].Rot.z - g_ModelStaRot[i].z) / g_EndFrameList[g_PunchPattern[g_PunchCnt]];
	}
}

//敵のGetter
XMODEL* GetEnemy()
{
	return g_Enemy;
}

//パンチ中か否かのGetter
bool GetPunch_Flg()
{
	return Punch_Flg;
}

//パンチフェーズのGetter
PUNCH_PHASE GetPunchPhase()
{
	return g_PunchPhase;
}

//パンチの左右Getter
bool GetPunchLR()
{
	return PunchLR;
}

//撃っているパンチの種類のGetter
PUNCH_PATTERN_INDEX GetPunchIndex()
{
	return g_Punch_Pattern_Index;
}