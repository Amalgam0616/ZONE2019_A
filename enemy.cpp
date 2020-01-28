#include "myDirect3D.h"
#include "input.h"
#include "Xfile.h"
#include "enemy.h"
#include "debugproc.h"
#include "Effect.h"
#include <time.h>	//乱数シード作るようなので、消すならこれも
#include "texture.h"
#include "player.h"

#include "sky.h"
#include "game.h"
#include "Roof.h"
#include "result.h"
#include "particle.h"
#include "score.h"
#include "sprite.h"
#include "Title.h"

#include <math.h>

//定数定義 ==========================================================================

//プレイヤーに使うモデル数
#define		ENEMY_MODEL_NUM	(7)

//敵の初期座標Z
#define ENEMY_POS_Z (26.0f)

//アッパーで吹っ飛ぶスピード
#define UPPERED_SPEED (15.0f)

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
#define		PPI_JP_START_FRAME		(120)	//ジャンプパンチ(左右一貫)


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

//チュートリアル用の変数たちを紹介するぜ！
static bool g_Tutorial_Flg;				//チュートリアルを通るためのフラグ
static bool g_EnemyMove_Flg;			//チュートリアル中に敵が動くことを許可するフラグ
static int g_Description_cnt;			//説明ボードの番号
TUTORIAL_PHASE g_TutorialPhase;			//チュートリアルのフェーズ
static int Tutorial_cnt;

//パンチパターン
//０＝右ストレート　１＝左パンチ　２＝瞬間移動パンチ
int g_PunchPattern[NUM_PUNCH];

//パンチ中か否か
bool Punch_Flg;
//パンチのフェーズ管理
PUNCH_PHASE g_PunchPhase;

LAST_PUNCH_PHASE g_Last_Punch_Phase;

//撃っているパンチの種類の管理
PUNCH_PATTERN_INDEX g_Punch_Pattern_Index;
//パンチのフレームカウンタ
int g_PunchFrameCnt;
//パンチのスピード
float Punch_Speed;
//パンチ終了Z座標
float g_PunchEndLine;

//吹っ飛ばされるときに使うカウント
int g_FLYAWAY_Cnt;

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
	15,	//ジャンプパンチ左
	15,	//ジャンプパンチ右
};

//スコア係数用にZ距離保存
static float g_ZDist;

//スローフラグ(いる)
bool SlowFlg;

//基本関数定義 ======================================================================
void ENEMY_OUTFLYAWAY();

//初期化
HRESULT InitEnemy()
{
	//チュートリアル関係の初期化
	g_Tutorial_Flg = false;
	g_EnemyMove_Flg = true;
	g_Description_cnt = 0;
	g_TutorialPhase = TUTORIAL_1;
	Tutorial_cnt = 0;


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
		g_PunchPattern[i] = rand() % (PPI_MAX - 2);
	}

	//for (int i = 0; i < LAST_PUNCH; i++)
	//{
	//	g_PunchPattern[i] = i;
	//}

	//最初のパンチは必ず左、右
	g_PunchPattern[0] = PPI_LEFT_PUNCH;
	g_PunchPattern[1] = PPI_LEFT_PUNCH;

	//最後のパンチは必ず
	g_PunchPattern[NUM_PUNCH - 1] = LAST_PUNCH;
	//g_PunchPattern[3] = LAST_PUNCH;

	//パンチフレームカウントの初期化
	g_PunchFrameCnt = 0;

	//パンチ中か否か
	Punch_Flg = false;
	//パンチスピード
	Punch_Speed = 3.0f;

	//吹っ飛ばされるときに使うカウント
	g_FLYAWAY_Cnt = 0;

	//瞬間移動の速度倍率
	g_Slow_Multi = 1.0f;

	//パンチのフェーズ管理
	g_PunchPhase = PUNCH_PHASE_CHARGE;
	//撃っているパンチの種類の管理
	g_Punch_Pattern_Index = PUNCH_NULL;

	//最後のパンチのフェーズの初期化
	g_Last_Punch_Phase = PUNCH_PHASE_LCHARGE;

	//パンチ終了Z座標(これがあるので、敵のInitはプレイヤーより後に行ってください)
	g_PunchEndLine = (GetPlayer() + 1)->Pos.z;

	//パンチスタート位置記録
	g_PunchStartLine[0] = g_Enemy[3].Pos.z;
	g_PunchStartLine[1] = g_Enemy[2].Pos.z;

	//パンチ回数初期化
	g_PunchCnt = 0;

	//Z距離保存初期化
	g_ZDist = 0;

	//モデル初期位置保存
	for (int i = 0; i < ENEMY_MODEL_NUM - 1; i++)
	{
		g_ModelStaPos[i] = g_Enemy[i].Pos;
		g_ModelStaRot[i] = g_Enemy[i].Rot;
	}

	//待機時間初期化
	g_WaitTime = 180;

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
	if (Keyboard_IsTrigger(DIK_6)) {
		Tutorial_cnt = 0;
		g_Tutorial_Flg = false;
		g_EnemyMove_Flg = true;
	}

	if (g_Tutorial_Flg)
	{
		DebugProc_Print((char *)"\nチュートリアルフェーズ:[%d],説明ボードのカウント:[%d],敵の移動許可:[%d]\n", g_TutorialPhase, g_Description_cnt, g_EnemyMove_Flg);
		PlayTutorial();
	}

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
				else if (g_PunchPattern[g_PunchCnt] == PPI_L_JUMP_PUNCH)
				{//ジャンプパンチ左
					if (g_Enemy[2].Pos.z < SLOW_IN && g_Enemy[2].Pos.z >(SLOW_OUT - 2.0f) && !SlowFlg)
					{
						SlowFlg = true;
					}
					else if (g_Enemy[2].Pos.z < (SLOW_OUT - 2.0f) && SlowFlg)
					{
						SlowFlg = false;
					}
				}
				else if (g_PunchPattern[g_PunchCnt] == PPI_R_JUMP_PUNCH)
				{//ジャンプパンチ右
					if (g_Enemy[3].Pos.z < SLOW_IN && g_Enemy[3].Pos.z >(SLOW_OUT - 2.0f) && !SlowFlg)
					{
						SlowFlg = true;
					}
					else if (g_Enemy[3].Pos.z < (SLOW_OUT - 2.0f) && SlowFlg)
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
			//行動許可があるときしか動けない
			if (g_EnemyMove_Flg)
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
					//ラストのパンチを一回目で見たい場合は下のコメントを解除してください
					//g_PunchPattern[g_PunchCnt] = LAST_PUNCH;

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
					else if (g_PunchPattern[g_PunchCnt] == PPI_L_JUMP_PUNCH)
					{//ジャンプパンチ左
						g_Punch_Pattern_Index = PPI_L_JUMP_PUNCH;
						JumpPunch_L();
					}
					else if (g_PunchPattern[g_PunchCnt] == PPI_R_JUMP_PUNCH)
					{//ジャンプパンチ右
						g_Punch_Pattern_Index = PPI_R_JUMP_PUNCH;
						JumpPunch_R();
					}
					else if (g_PunchPattern[g_PunchCnt] == LAST_PUNCH)
					{
						g_Punch_Pattern_Index = LAST_PUNCH;
						Last_Punch();
					}
				}
			}
		}
	}

	//デバッグ用
	DebugProc_Print((char *)"最後のパンチのフェーズ管理：[%d]\n", g_Last_Punch_Phase);
	DebugProc_Print((char *)"i：[%d]\n", i);
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

	}

	if (Keyboard_IsPress(DIK_I))
	{

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

	if (i > 6)
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

	if (g_Last_Punch_Phase < PUNCH_PHASE_FLYAWAY)
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

//瞬間移動パンチ(右)のポジション
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

//瞬間移動パンチ(左)のポジション
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
				//Punch_Charge_Effect();
				Charge_Start();
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

			//Z距離保存
			g_ZDist = fabsf(g_PunchEndLine - g_Enemy[3].Pos.z);

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
				g_WaitTime = 180;

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
				//Punch_Charge_Effect();
				Charge_Start();
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

			//Z距離保存
			g_ZDist = fabsf(g_PunchEndLine - g_Enemy[2].Pos.z);

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
				g_WaitTime = 180;

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
			Charge_Start();
			//Punch_Charge_Effect();

			//Z距離保存
			g_ZDist = fabsf(g_PunchEndLine - g_Enemy[3].Pos.z);
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

				g_WaitTime = 180;

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
			Charge_Start();
			//Punch_Charge_Effect();

			//Z距離保存
			g_ZDist = fabsf(g_PunchEndLine - g_Enemy[2].Pos.z);
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

				g_WaitTime = 180;

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
				Charge_Start();
				//Punch_Charge_Effect();
				PunchLR = true;
			}
		}
		else
		{
			//プレイヤーの足元へのベクトルを出す
			dir.y = 0.0f - g_Enemy[3].Pos.y;
			dir.z = g_PunchEndLine - g_Enemy[3].Pos.z;
			D3DXVec3Normalize(&dir, &dir);

			//Z距離保存
			g_ZDist = fabsf(g_PunchEndLine - g_Enemy[3].Pos.z);

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
				g_WaitTime = 180;

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

//ジャンピングパンチ左(PPI_L_JUMP_PUNCH)(作成者：Amalgam、No.04)
void JumpPunch_L()
{
	//目標地点へのベクトル
	static D3DXVECTOR3 dir;

	//フレーム進行
	g_PunchFrameCnt++;

	if (g_PunchPhase == PUNCH_PHASE_CHARGE)
	{//予備動作
		if (g_PunchFrameCnt < PPI_JP_START_FRAME)
		{
			if (g_PunchFrameCnt == 1)
			{
				Charge_Start();
				//Punch_Charge_Effect();
				PunchLR = true;
			}
			//グッ
			if (g_PunchFrameCnt < 10)
			{
				//胴体
				g_Enemy[0].Pos.y -= 0.2f;

				//頭
				g_Enemy[1].Pos.y -= 0.3f;
				g_Enemy[1].Rot.x -= D3DXToRadian(1.5f);

				//左腕
				g_Enemy[2].Pos.x -= 0.2f;
				g_Enemy[2].Pos.y -= 0.3f;

				//右腕
				g_Enemy[3].Pos.x += 0.2f;
				g_Enemy[3].Pos.y -= 0.3f;
			}
			//グ2
			else if (g_PunchFrameCnt > 10 && g_PunchFrameCnt < 15)
			{
				//胴体
				g_Enemy[0].Pos.y -= 0.4f;

				//頭
				g_Enemy[1].Pos.y -= 0.6f;
				g_Enemy[1].Rot.x -= D3DXToRadian(3.0f);

				//左腕
				g_Enemy[2].Pos.x -= 0.4f;
				g_Enemy[2].Pos.y -= 0.5f;

				//右腕
				g_Enemy[3].Pos.x += 0.4f;
				g_Enemy[3].Pos.y -= 0.6f;
			}
			//ピョン1
			else if (g_PunchFrameCnt > 45 && g_PunchFrameCnt < 50)
			{
				//胴
				g_Enemy[0].Pos.y += 1.0f;
				g_Enemy[0].Rot.x += D3DXToRadian(1.0f);

				//頭
				g_Enemy[1].Pos.y += 1.0f;
				g_Enemy[1].Rot.x += D3DXToRadian(10.0f);

				//左腕
				g_Enemy[2].Pos.x += 1.0f;
				g_Enemy[2].Pos.y += 4.0f;

				//右腕
				g_Enemy[3].Pos.x -= 0.6f;
				g_Enemy[3].Pos.y += 0.1f;

				//左足
				g_Enemy[4].Pos.y += 0.5f;
				g_Enemy[4].Rot.x -= D3DXToRadian(20.0f);

				//右足
				g_Enemy[5].Pos.y += 0.5f;
				g_Enemy[5].Rot.x -= D3DXToRadian(20.0f);
			}
			//ピョン2
			else if (g_PunchFrameCnt > 50 && g_PunchFrameCnt < 65)
			{
				for (int i = 0; i < ENEMY_MODEL_NUM - 1; i++)
				{
					g_Enemy[i].Pos.y += 1.0f;
					g_Enemy[i].Pos.z += 0.5f;
				}
			}
			//グーッ1
			else if (g_PunchFrameCnt == 66)
			{
				//胴
				g_Enemy[0].Rot.x = D3DXToRadian(-20.0f);

				//頭
				g_Enemy[1].Pos.y -= 2.0f;
				g_Enemy[1].Pos.z -= 5.0f;
				g_Enemy[1].Rot.x = D3DXToRadian(-20.0f);

				//左腕
				g_Enemy[2].Pos.y -= 8.0f;
				g_Enemy[2].Rot = D3DXVECTOR3(D3DXToRadian(10.0f), D3DXToRadian(120.0f), D3DXToRadian(0.0f));
			}
			//グーッ2(エフェクト入れるならここ)
			else if (g_PunchFrameCnt > 70 && g_PunchFrameCnt < 85)
			{
				//左腕
				g_Enemy[2].Pos.x += 0.09f;
				g_Enemy[2].Pos.y += 0.09f;
				g_Enemy[2].Pos.z += 0.25f;

				PunchLR = false;
			}

		}
		else
		{
			//プレイヤーの頭へのベクトルを出す
			dir.x = 0.0f - g_Enemy[2].Pos.x;
			dir.y = 8.0f - g_Enemy[2].Pos.y;
			dir.z = g_PunchEndLine - g_Enemy[2].Pos.z;
			D3DXVec3Normalize(&dir, &dir);

			//Z距離保存
			g_ZDist = fabsf(g_PunchEndLine - g_Enemy[2].Pos.z);

			//フェーズ進行
			g_PunchPhase = PUNCH_PHASE_SWING;
			g_PunchFrameCnt = 0;
		}
	}
	else if (g_PunchPhase == PUNCH_PHASE_SWING)
	{//パンチ本体
		if (g_Enemy[2].Pos.z >= g_PunchEndLine)
		{//エンドラインに到達してなければ動かす(プレイヤーは敵からみてZマイナス方向にいる)
			if (SlowFlg)
			{
				//胴体
				g_Enemy[0].Pos.y += dir.y * 0.25f;
				g_Enemy[0].Pos.z += dir.z * 0.25f;
				g_Enemy[0].Rot.x -= D3DXToRadian(0.5f);
				g_Enemy[0].Rot.y += D3DXToRadian(0.5f);

				//頭
				g_Enemy[1].Pos.x -= 0.05f;
				g_Enemy[1].Pos.y += dir.y * 0.25f;
				g_Enemy[1].Pos.z += dir.z * 0.25f - 0.05f;
				g_Enemy[1].Rot.x -= D3DXToRadian(0.5f);
				g_Enemy[1].Rot.y += D3DXToRadian(0.5f);

				//右腕
				g_Enemy[2].Pos.x += dir.x * 0.5f;
				g_Enemy[2].Pos.y += dir.y * 0.5f;
				g_Enemy[2].Pos.z += dir.z * 0.5f;
			}
			else
			{
				//胴体
				g_Enemy[0].Pos.y += dir.y * 0.5f;
				g_Enemy[0].Pos.z += dir.z * 0.5f;
				g_Enemy[0].Rot.x += D3DXToRadian(1.0f);
				g_Enemy[0].Rot.y += D3DXToRadian(1.0f);

				//頭
				g_Enemy[1].Pos.x -= 0.1f;
				g_Enemy[1].Pos.y += dir.y * 0.5f;
				g_Enemy[1].Pos.z += dir.z * 0.5f - 0.1f;
				g_Enemy[1].Rot.x += D3DXToRadian(1.0f);
				g_Enemy[1].Rot.y += D3DXToRadian(1.0f);

				//右腕
				g_Enemy[2].Pos.x += dir.x * 3.0f;
				g_Enemy[2].Pos.y += dir.y * 3.0f;
				g_Enemy[2].Pos.z += dir.z * 3.0f;
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
				g_WaitTime = 180;

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
//ジャンピングパンチ右(PPI_R_JUMP_PUNCH)(作成者：Amalgam、No.05)
void JumpPunch_R()
{
	//目標地点へのベクトル
	static D3DXVECTOR3 dir;

	//フレーム進行
	g_PunchFrameCnt++;

	if (g_PunchPhase == PUNCH_PHASE_CHARGE)
	{//予備動作
		if (g_PunchFrameCnt < PPI_JP_START_FRAME)
		{
			if (g_PunchFrameCnt == 1)
			{
				Charge_Start();
				//Punch_Charge_Effect();
				PunchLR = false;
			}

			//グッ
			if (g_PunchFrameCnt < 10)
			{
				//胴体
				g_Enemy[0].Pos.y -= 0.2f;

				//頭
				g_Enemy[1].Pos.y -= 0.3f;
				g_Enemy[1].Rot.x -= D3DXToRadian(1.5f);

				//左腕
				g_Enemy[2].Pos.x -= 0.2f;
				g_Enemy[2].Pos.y -= 0.3f;

				//右腕
				g_Enemy[3].Pos.x += 0.2f;
				g_Enemy[3].Pos.y -= 0.3f;
			}
			//グ2
			else if (g_PunchFrameCnt > 10 && g_PunchFrameCnt < 15)
			{
				//胴体
				g_Enemy[0].Pos.y -= 0.4f;

				//頭
				g_Enemy[1].Pos.y -= 0.6f;
				g_Enemy[1].Rot.x -= D3DXToRadian(3.0f);

				//左腕
				g_Enemy[2].Pos.x -= 0.4f;
				g_Enemy[2].Pos.y -= 0.5f;

				//右腕
				g_Enemy[3].Pos.x += 0.4f;
				g_Enemy[3].Pos.y -= 0.6f;
			}
			//ピョン1
			else if (g_PunchFrameCnt > 45 && g_PunchFrameCnt < 50)
			{
				//胴
				g_Enemy[0].Pos.y += 1.0f;
				g_Enemy[0].Rot.x += D3DXToRadian(1.0f);

				//頭
				g_Enemy[1].Pos.y += 1.0f;
				g_Enemy[1].Rot.x += D3DXToRadian(10.0f);

				//左腕
				g_Enemy[2].Pos.x += 0.6f;
				g_Enemy[2].Pos.y += 0.1f;

				//右腕
				g_Enemy[3].Pos.x -= 1.0f;
				g_Enemy[3].Pos.y += 4.0f;

				//左足
				g_Enemy[4].Pos.y += 0.5f;
				g_Enemy[4].Rot.x -= D3DXToRadian(20.0f);

				//右足
				g_Enemy[5].Pos.y += 0.5f;
				g_Enemy[5].Rot.x -= D3DXToRadian(20.0f);
			}
			//ピョン2
			else if (g_PunchFrameCnt > 50 && g_PunchFrameCnt < 65)
			{
				for (int i = 0; i < ENEMY_MODEL_NUM - 1; i++)
				{
					g_Enemy[i].Pos.y += 1.0f;
					g_Enemy[i].Pos.z += 0.5f;
				}
			}
			//グーッ1
			else if (g_PunchFrameCnt == 66)
			{
				//胴
				g_Enemy[0].Rot.x = D3DXToRadian(-20.0f);

				//頭
				g_Enemy[1].Pos.y -= 2.0f;
				g_Enemy[1].Pos.z -= 5.0f;
				g_Enemy[1].Rot.x = D3DXToRadian(-20.0f);

				//右腕
				g_Enemy[3].Pos.y -= 8.0f;
				g_Enemy[3].Rot = D3DXVECTOR3(D3DXToRadian(10.0f), D3DXToRadian(-90.0f), D3DXToRadian(0.0f));
			}
			//グーッ2(エフェクト入れるならここ)
			else if (g_PunchFrameCnt > 70 && g_PunchFrameCnt < 85)
			{
				//左腕
				g_Enemy[3].Pos.x -= 0.09f;
				g_Enemy[3].Pos.y += 0.09f;
				g_Enemy[3].Pos.z += 0.25f;

				PunchLR = true;
			}
		}
		else
		{
			//プレイヤーの頭へのベクトルを出す
			dir.x = 0.0f - g_Enemy[3].Pos.x;
			dir.y = 8.0f - g_Enemy[3].Pos.y;
			dir.z = g_PunchEndLine - g_Enemy[3].Pos.z;
			D3DXVec3Normalize(&dir, &dir);

			//Z距離保存
			g_ZDist = fabsf(g_PunchEndLine - g_Enemy[3].Pos.z);

			//フェーズ進行
			g_PunchPhase = PUNCH_PHASE_SWING;
			g_PunchFrameCnt = 0;
		}
	}
	else if (g_PunchPhase == PUNCH_PHASE_SWING)
	{//パンチ本体
		if (g_Enemy[3].Pos.z >= g_PunchEndLine)
		{//エンドラインに到達してなければ動かす(プレイヤーは敵からみてZマイナス方向にいる)
			if (SlowFlg)
			{
				//胴体
				g_Enemy[0].Pos.y += dir.y * 0.25f;
				g_Enemy[0].Pos.z += dir.z * 0.25f;
				g_Enemy[0].Rot.x -= D3DXToRadian(0.5f);
				g_Enemy[0].Rot.y -= D3DXToRadian(0.5f);

				//頭
				g_Enemy[1].Pos.x += 0.05f;
				g_Enemy[1].Pos.y += dir.y * 0.25f;
				g_Enemy[1].Pos.z += dir.z * 0.25f - 0.05f;
				g_Enemy[1].Rot.x -= D3DXToRadian(0.5f);
				g_Enemy[1].Rot.y -= D3DXToRadian(0.5f);

				//右腕
				g_Enemy[3].Pos.x += dir.x * 0.5f;
				g_Enemy[3].Pos.y += dir.y * 0.5f;
				g_Enemy[3].Pos.z += dir.z * 0.5f;
			}
			else
			{
				//胴体
				g_Enemy[0].Pos.y += dir.y * 0.5f;
				g_Enemy[0].Pos.z += dir.z * 0.5f;
				g_Enemy[0].Rot.x -= D3DXToRadian(1.0f);
				g_Enemy[0].Rot.y -= D3DXToRadian(1.0f);

				//頭
				g_Enemy[1].Pos.x += 0.1f;
				g_Enemy[1].Pos.y += dir.y * 0.5f;
				g_Enemy[1].Pos.z += dir.z * 0.5f - 0.1f;
				g_Enemy[1].Rot.x -= D3DXToRadian(1.0f);
				g_Enemy[1].Rot.y -= D3DXToRadian(1.0f);

				//右腕
				g_Enemy[3].Pos.x += dir.x * 3.0f;
				g_Enemy[3].Pos.y += dir.y * 3.0f;
				g_Enemy[3].Pos.z += dir.z * 3.0f;
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
				g_WaitTime = 180;

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


//追加部分8===============================
//最後のパンチ
void Last_Punch()
{
	//フレーム進行
	g_PunchFrameCnt++;

	if (g_Last_Punch_Phase == PUNCH_PHASE_LCHARGE)
	{//予備動作
		//溜めポーズ
		if (g_PunchFrameCnt < LAST_PUNCH_CHAGE_FRAME + 60)
		{
			//溜めをつくるために先に動きが止まる
			if (g_PunchFrameCnt < LAST_PUNCH_CHAGE_FRAME)
			{
				g_Enemy[1].Rot.x += D3DXToRadian(20.0f) / LAST_PUNCH_CHAGE_FRAME;
				g_Enemy[2].Pos += D3DXVECTOR3(-7.0f / LAST_PUNCH_CHAGE_FRAME, 15.0f / LAST_PUNCH_CHAGE_FRAME, -5.0f / LAST_PUNCH_CHAGE_FRAME);
				g_Enemy[3].Pos += D3DXVECTOR3(7.0f / LAST_PUNCH_CHAGE_FRAME, 15.0f / LAST_PUNCH_CHAGE_FRAME, -5.0f / LAST_PUNCH_CHAGE_FRAME);
			}
		}
		else
		{
			g_PunchFrameCnt = 0;
			//フェーズ進行
			g_Last_Punch_Phase = PUNCH_PHASE_WAVE;
		}
	}
	else if (g_Last_Punch_Phase == PUNCH_PHASE_WAVE)
	{//プレイヤーが吹っ飛ばされる
		if (g_PunchFrameCnt < LAST_PUNCH_WAVE_FRAME)
		{
			//波動モーション
			g_Enemy[1].Rot = D3DXVECTOR3(D3DXToRadian(22.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
			g_Enemy[2].Pos = D3DXVECTOR3(10.0f, 14.0f, 22.6f);
			g_Enemy[2].Rot = D3DXVECTOR3(D3DXToRadian(10.0f), D3DXToRadian(192.0f), D3DXToRadian(90.0f));
			g_Enemy[3].Pos = D3DXVECTOR3(-11.0f, 15.6f, 26.0f);
			g_Enemy[3].Rot = D3DXVECTOR3(D3DXToRadian(15.0f), D3DXToRadian(180.0f), D3DXToRadian(270.0f));

			//プレイヤーが吹き飛ばされる
			for (int i = 0; i < 6; i++)
			{
				(GetPlayer() + i)->Pos.z -= 20.0f / g_PunchFrameCnt;
			}
		}
		else
		{
			g_PunchFrameCnt = 0;
			//フェーズ進行
			g_Last_Punch_Phase = PUNCH_PHASE_SET;
		}
	}
	else if (g_Last_Punch_Phase == PUNCH_PHASE_SET)
	{
		if (g_PunchFrameCnt < LAST_PUNCH_SET_FRAME)
		{
			//構えをとる
			g_Enemy[0].Rot += D3DXVECTOR3(0, D3DXToRadian(60.0f) / LAST_PUNCH_SET_FRAME, 0);
			g_Enemy[1].Pos += D3DXVECTOR3(0, -1.0f / LAST_PUNCH_SET_FRAME, 0);
			g_Enemy[1].Rot += D3DXVECTOR3(D3DXToRadian(-20.0f) / LAST_PUNCH_SET_FRAME, 0, 0);
			g_Enemy[2].Pos += D3DXVECTOR3(-10.0f / LAST_PUNCH_SET_FRAME, 3.4f / LAST_PUNCH_SET_FRAME, 0);
			g_Enemy[2].Rot += D3DXVECTOR3(D3DXToRadian(5.0f) / LAST_PUNCH_SET_FRAME, D3DXToRadian(50.0f) / LAST_PUNCH_SET_FRAME, 0);
			g_Enemy[3].Pos += D3DXVECTOR3(4.0f / LAST_PUNCH_SET_FRAME, 2.4f / LAST_PUNCH_SET_FRAME, 6.0f / LAST_PUNCH_SET_FRAME);
			g_Enemy[3].Rot += D3DXVECTOR3(D3DXToRadian(-5.0f) / LAST_PUNCH_SET_FRAME, D3DXToRadian(100.0f) / LAST_PUNCH_SET_FRAME, D3DXToRadian(100.0f) / LAST_PUNCH_SET_FRAME);
			g_Enemy[4].Pos += D3DXVECTOR3(-7.0f / LAST_PUNCH_SET_FRAME, 0.0f / LAST_PUNCH_SET_FRAME, -10.0f / LAST_PUNCH_SET_FRAME);
			g_Enemy[4].Rot += D3DXVECTOR3(0, D3DXToRadian(12.0f) / LAST_PUNCH_SET_FRAME, 0);
			g_Enemy[5].Pos += D3DXVECTOR3(0, 0, 10.0f / LAST_PUNCH_SET_FRAME);
			g_Enemy[5].Rot += D3DXVECTOR3(D3DXToRadian(-20.0f) / LAST_PUNCH_SET_FRAME, 0, D3DXToRadian(12.0f) / LAST_PUNCH_SET_FRAME);

		}
		if(g_PunchFrameCnt> LAST_PUNCH_SET_FRAME + 90)
		{
			g_Enemy[2].Pos += D3DXVECTOR3(10.0f, 0, 10.0f);
			g_Enemy[3].Pos += D3DXVECTOR3(0, 0, -10.0f);

			g_PunchFrameCnt = 0;
			//フェーズ進行
			g_Last_Punch_Phase = PUNCH_PHASE_PUNCH;
		}
	}
	else if (g_Last_Punch_Phase == PUNCH_PHASE_PUNCH)
	{
		if (g_PunchFrameCnt < LAST_PUNCH_SLOW_FRAME)
		{
			for (int i = 0; i < ENEMY_MODEL_NUM; i++)
			{
				g_Enemy[i].Pos += D3DXVECTOR3(0, 0.5f / LAST_PUNCH_PUNCH_FRAME, -60.0f / LAST_PUNCH_PUNCH_FRAME);
			}
			g_Enemy[3].Pos += D3DXVECTOR3(5.0f / LAST_PUNCH_PUNCH_FRAME, -8.0f / LAST_PUNCH_PUNCH_FRAME, -10.0f / LAST_PUNCH_PUNCH_FRAME);

		}
		else
		{
			g_PunchFrameCnt = 0;
			//フェーズ進行
			g_Last_Punch_Phase = PUNCH_PHASE_SLOW;
		}
	}
	else if (g_Last_Punch_Phase == PUNCH_PHASE_SLOW)
	{
		if (Keyboard_IsTrigger(DIK_RETURN))//後で変えて
		{

			g_PunchFrameCnt = 0;
			//フェーズ進行
			g_Last_Punch_Phase = PUNCH_PHASE_STOP;
		}
		else if (Keyboard_IsTrigger(DIK_NUMPADENTER))//後で変えて
		{

			g_PunchFrameCnt = 0;
			//フェーズ進行
			g_Last_Punch_Phase = PUNCH_PHASE_STOP;
		}
	}
	else if (g_Last_Punch_Phase == PUNCH_PHASE_STOP)
	{
		if (g_PunchFrameCnt < LAST_PUNCH_STOP_FRAME)
		{
			/*for (int i = 0; i < ENEMY_MODEL_NUM; i++)
			{
				g_Enemy[i].Pos += D3DXVECTOR3(0, 0.5f / LAST_PUNCH_STOP_FRAME, -60.0f / LAST_PUNCH_STOP_FRAME);
			}*/

			//デバック用
			if (g_PunchFrameCnt < LAST_PUNCH_PUNCH_FRAME)
			{
				for (int i = 0; i < ENEMY_MODEL_NUM; i++)
				{
					g_Enemy[i].Pos += D3DXVECTOR3(0, 0.5f / LAST_PUNCH_PUNCH_FRAME, -30.0f / LAST_PUNCH_PUNCH_FRAME);
				}
				g_Enemy[3].Rot.y += 15.0f / LAST_PUNCH_PUNCH_FRAME;
			}
		}
		else
		{


			g_PunchFrameCnt = 0;
			//フェーズ進行
			g_Last_Punch_Phase = PUNCH_PHASE_YOIN;
		}
	}
	else if (g_Last_Punch_Phase == PUNCH_PHASE_YOIN)
	{
		if (g_PunchFrameCnt < LAST_PUNCH_YOIN_FRAME)
		{


		}
		else
		{
			g_Enemy[1].Pos = D3DXVECTOR3(5.0f, 22.0f, -56.5f);
			g_Enemy[1].Rot = D3DXVECTOR3(D3DXToRadian(-31.2f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
			g_Enemy[2].Pos = D3DXVECTOR3(15.0f, 16.7f, -56.2f);
			g_Enemy[2].Rot = D3DXVECTOR3(D3DXToRadian(83.0f), D3DXToRadian(145.0f), D3DXToRadian(90.0f));
			g_Enemy[3].Pos = D3DXVECTOR3(-6.6f, 12.2f, -65.0f);
			g_Enemy[3].Rot = D3DXVECTOR3(D3DXToRadian(70.7f), D3DXToRadian(272.2f), D3DXToRadian(8.3f));
			g_Enemy[4].Pos = D3DXVECTOR3(3.1f, -5.0f, -65.0f);
			g_Enemy[4].Rot = D3DXVECTOR3(D3DXToRadian(56.4f), D3DXToRadian(16.3f), D3DXToRadian(0.0f));
			g_Enemy[5].Pos = D3DXVECTOR3(-6.0f, -8.0f, -60.0f);
			g_Enemy[5].Rot = D3DXVECTOR3(D3DXToRadian(-23.0f), D3DXToRadian(-3.0f), D3DXToRadian(12.0f));
			g_Enemy[6].Pos = D3DXVECTOR3(6.0f, 12.3f, -54.0f);
			g_Enemy[6].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(5.0f), D3DXToRadian(0.0f));

			g_PunchFrameCnt = 0;
			//フェーズ進行
			g_Last_Punch_Phase = PUNCH_PHASE_FLYAWAY;
		}
	}
	else if (g_Last_Punch_Phase == PUNCH_PHASE_FLYAWAY)
	{
		if (g_PunchFrameCnt < LAST_PUNCH_FLYAWAY_FRAME)
		{
			ENEMY_FLYAWAY();
		}
		else
		{
			g_PunchFrameCnt = 0;
			g_Last_Punch_Phase = PUNCH_PHASE_INFLYING;
		}
	}
	else if (g_Last_Punch_Phase == PUNCH_PHASE_INFLYING)
	{
		//会場内を飛行中
		if (g_PunchFrameCnt < LAST_PUNCH_FLYING_FRAME)
		{
			ENEMY_FLYAWAY();
		}
		else
		{
			if (!GetSkyFlag())
			{
				//これはデバッグ用
				AddScore(100000);

				//飛行が始まるとスコアが変動するため、現在時点でのスコアを保持して退避させる（ほんとはこんな微妙なとこに置きたくない）
				SetCopy_Score();
				//天井割れたら空の描画にうつるためフラグをtrueに変更（１回のみ）
				SetSkyFlag_ture();
			}
			g_PunchFrameCnt = 0;
			g_Last_Punch_Phase = PUNCH_PHASE_OUTFLYING;
		}
	}
	else if (g_Last_Punch_Phase == PUNCH_PHASE_OUTFLYING)
	{
		//会場の外の処理
		if (g_PunchFrameCnt < LAST_PUNCH_OUTFLYING_FRAME)
		{
			ENEMY_OUTFLYAWAY();

			//2020/1/16時点の設定：ゲージ％×1000
			//AddScore(1000);
		}
	}
}
void ENEMY_FLYAWAY()
{
	g_FLYAWAY_Cnt++;

	if (g_FLYAWAY_Cnt < FLYAWAY_MOVE1_FRAME)
	{
		for (int i = 1; i < 7; i++)
		{
			g_Enemy[i].Pos += D3DXVECTOR3(0.0f, 2.0f / g_FLYAWAY_Cnt, 3.0f / g_FLYAWAY_Cnt);
		}
	}
	else
	{
		//天井割れるまで飛ぶ
		if (g_Enemy[1].Pos.y < ROOF_Y + 100.0f)
		{
			for (int i = 1; i < 7; i++)
			{
				g_Enemy[i].Pos += D3DXVECTOR3(0.0f, 30.0f, 10.0f);
			}
		}
	}
}
//追加部分8==========================================

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

//スコア・ゲージの係数出す関数
float CoefCal(bool s_or_g)
{
	//スコアの方
	if (s_or_g)
	{
		//準備中　１
		if (g_PunchPhase == PUNCH_PHASE_CHARGE)
		{
			return 0.1f;
		}
		else if (g_PunchPhase == PUNCH_PHASE_SWING)
		{
			//一時メモリ
			float tmp_ZDist;

			//左手パンチ系
			if ((g_PunchPattern[g_PunchCnt] == PPI_LEFT_PUNCH) ||
				(g_PunchPattern[g_PunchCnt] == PPI_L_JUMP_PUNCH) ||
				(g_PunchPattern[g_PunchCnt] == L_FLASH_PUNCH))
			{
				tmp_ZDist = fabsf(g_PunchEndLine - g_Enemy[2].Pos.z);

				tmp_ZDist = tmp_ZDist / g_ZDist;

				//最低点　２
				if (tmp_ZDist > 0.9f)
				{
					return 0.2f;
				}
				//３
				else if ((tmp_ZDist <= 0.9f) && (tmp_ZDist > 0.8f))
				{
					return 0.3f;
				}
				//４
				else if ((tmp_ZDist <= 0.8f) && (tmp_ZDist > 0.7f))
				{
					return 0.4f;
				}
				//５
				else if ((tmp_ZDist <= 0.7f) && (tmp_ZDist > 0.6f))
				{
					return 0.5f;
				}
				//普通くらい　６
				else if ((tmp_ZDist <= 0.6f) && (tmp_ZDist > 0.5f))
				{
					return 0.6f;
				}
				//７
				else if ((tmp_ZDist <= 0.5f) && (tmp_ZDist > 0.4f))
				{
					return 0.7f;
				}
				//８
				else if ((tmp_ZDist <= 0.4f) && (tmp_ZDist > 0.3f))
				{
					return 0.8f;
				}
				//９
				else if ((tmp_ZDist <= 0.4f) && (tmp_ZDist > 0.3f))
				{
					return 0.9f;
				}
				//すごい　１０
				else if (tmp_ZDist <= 0.3f)
				{
					return 1.0f;
				}
			}
			//右手パンチ系
			else if ((g_PunchPattern[g_PunchCnt] == PPI_RIGHT_PUNCH) ||
				(g_PunchPattern[g_PunchCnt] == PPI_R_JUMP_PUNCH) ||
				(g_PunchPattern[g_PunchCnt] == R_FLASH_PUNCH) ||
				(g_PunchPattern[g_PunchCnt] == PPI_DUNK_PUNCH))
			{
				tmp_ZDist = fabsf(g_PunchEndLine - g_Enemy[3].Pos.z);

				tmp_ZDist = tmp_ZDist / g_ZDist;

				//最低点　２
				if (tmp_ZDist > 0.9f)
				{
					return 0.2f;
				}
				//３
				else if ((tmp_ZDist <= 0.9f) && (tmp_ZDist > 0.8f))
				{
					return 0.3f;
				}
				//４
				else if ((tmp_ZDist <= 0.8f) && (tmp_ZDist > 0.7f))
				{
					return 0.4f;
				}
				//５
				else if ((tmp_ZDist <= 0.7f) && (tmp_ZDist > 0.6f))
				{
					return 0.5f;
				}
				//普通くらい　６
				else if ((tmp_ZDist <= 0.6f) && (tmp_ZDist > 0.5f))
				{
					return 0.6f;
				}
				//７
				else if ((tmp_ZDist <= 0.5f) && (tmp_ZDist > 0.4f))
				{
					return 0.7f;
				}
				//８
				else if ((tmp_ZDist <= 0.4f) && (tmp_ZDist > 0.3f))
				{
					return 0.8f;
				}
				//９
				else if ((tmp_ZDist <= 0.4f) && (tmp_ZDist > 0.3f))
				{
					return 0.9f;
				}
				//すごい　１０
				else if (tmp_ZDist <= 0.3f)
				{
					return 1.0f;
				}
			}
		}
	}
	//ゲージの方
	else if (!s_or_g)
	{
		//準備中　１
		if (g_PunchPhase == PUNCH_PHASE_CHARGE)
		{
			return 10.0f;
		}
		else if (g_PunchPhase == PUNCH_PHASE_SWING)
		{
			//一時メモリ
			float tmp_ZDist;

			//左手パンチ系
			if ((g_PunchPattern[g_PunchCnt] == PPI_LEFT_PUNCH) ||
				(g_PunchPattern[g_PunchCnt] == PPI_L_JUMP_PUNCH) ||
				(g_PunchPattern[g_PunchCnt] == L_FLASH_PUNCH))
			{
				tmp_ZDist = fabsf(g_PunchEndLine - g_Enemy[2].Pos.z);

				tmp_ZDist = tmp_ZDist / g_ZDist;

				//最低点　２
				if (tmp_ZDist > 0.9f)
				{
					return 8.0f;
				}
				//３
				else if ((tmp_ZDist <= 0.9f) && (tmp_ZDist > 0.8f))
				{
					return 7.0f;
				}
				//４
				else if ((tmp_ZDist <= 0.8f) && (tmp_ZDist > 0.7f))
				{
					return 6.0f;
				}
				//５
				else if ((tmp_ZDist <= 0.7f) && (tmp_ZDist > 0.6f))
				{
					return 5.0f;
				}
				//普通くらい　６
				else if ((tmp_ZDist <= 0.6f) && (tmp_ZDist > 0.5f))
				{
					return 4.0f;
				}
				//７
				else if ((tmp_ZDist <= 0.5f) && (tmp_ZDist > 0.4f))
				{
					return 3.0f;
				}
				//８
				else if ((tmp_ZDist <= 0.4f) && (tmp_ZDist > 0.3f))
				{
					return 2.0f;
				}
				//９
				else if ((tmp_ZDist <= 0.4f) && (tmp_ZDist > 0.3f))
				{
					return 1.0f;
				}
				//すごい　１０
				else if (tmp_ZDist <= 0.3f)
				{
					return 0.0f;
				}
			}
			//右手パンチ系
			else if ((g_PunchPattern[g_PunchCnt] == PPI_RIGHT_PUNCH) ||
				(g_PunchPattern[g_PunchCnt] == PPI_R_JUMP_PUNCH) ||
				(g_PunchPattern[g_PunchCnt] == R_FLASH_PUNCH) ||
				(g_PunchPattern[g_PunchCnt] == PPI_DUNK_PUNCH))
			{
				tmp_ZDist = fabsf(g_PunchEndLine - g_Enemy[3].Pos.z);

				tmp_ZDist = tmp_ZDist / g_ZDist;

				//最低点　２
				if (tmp_ZDist > 0.9f)
				{
					return 8.0f;
				}
				//３
				else if ((tmp_ZDist <= 0.9f) && (tmp_ZDist > 0.8f))
				{
					return 7.0f;
				}
				//４
				else if ((tmp_ZDist <= 0.8f) && (tmp_ZDist > 0.7f))
				{
					return 6.0f;
				}
				//５
				else if ((tmp_ZDist <= 0.7f) && (tmp_ZDist > 0.6f))
				{
					return 5.0f;
				}
				//普通くらい　６
				else if ((tmp_ZDist <= 0.6f) && (tmp_ZDist > 0.5f))
				{
					return 4.0f;
				}
				//７
				else if ((tmp_ZDist <= 0.5f) && (tmp_ZDist > 0.4f))
				{
					return 3.0f;
				}
				//８
				else if ((tmp_ZDist <= 0.4f) && (tmp_ZDist > 0.3f))
				{
					return 2.0f;
				}
				//９
				else if ((tmp_ZDist <= 0.4f) && (tmp_ZDist > 0.3f))
				{
					return 1.0f;
				}
				//すごい
				else if (tmp_ZDist <= 0.3f)
				{
					return 0.0f;
				}
			}
		}
	}

	return 0.0f;
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

//追加部分9===============================
//最後のパンチのGetter
LAST_PUNCH_PHASE GetLastPunchPhase()
{
	return g_Last_Punch_Phase;
}

//追加ｂｙ佐々木
void ENEMY_OUTFLYAWAY()
{
	for (int i = 1; i < 7; i++)
	{
		g_Enemy[i].Rot += D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(0.0f), D3DXToRadian(0.0f));
	}
	//飛行機で止まる場合
	if (GetCopy_Score() <= 300000)
	{
		//飛行機を描画していて尚且つエネミーと飛行機が接触したとき（310フレーム）
		if (GetAirplaneFlag() && g_PunchFrameCnt >= 310)
		{
			g_PunchFrameCnt = 0;
			g_Last_Punch_Phase = PUNCH_PHASE_CLASH;

			//UpdateをUpdateさせない、そう止めるんだよぶつかった瞬間に
			SetDrawStop();

			//リザルトシーンに遷移
			if (!GetResultStart())
			{
				SetResultStart_true();
			}
		}
	}
	//衛星で止まる場合
	else if (GetCopy_Score() > 300000 && GetCopy_Score() <= 360000)
	{
		if (GetSatelliteFlag() && g_PunchFrameCnt >= 420)
		{
			g_PunchFrameCnt = 0;
			g_Last_Punch_Phase = PUNCH_PHASE_CLASH;

			SetDrawStop();

			//リザルトシーンに遷移
			if (!GetResultStart())
			{
				SetResultStart_true();
			}
		}
	}
	//月で止まる場合
	else if (GetCopy_Score() >= 360000)
	{
		//目標物にぶつかったときの処理はこの中に
		if (g_PunchFrameCnt < LAST_PUNCH_HIT_FRAME)
		{
			//月が描画された状態で
			if (GetMoonFlag())
			{
				//月よりも↓にいたら敵の座標を月に近づくよう動かす（10.0fは頭の分ちょっととった）
				if (g_Enemy[1].Pos.y + 6.0f < ((GetSkyModel() + 2)->Pos.y))
				{
					for (int i = 1; i < 7; i++)
					{
						g_Enemy[i].Pos += D3DXVECTOR3(0.0f, 1.2f, 0.6f);
					}
				}
				//月に衝突したら
				else
				{
					g_PunchFrameCnt = 0;
					g_Last_Punch_Phase = PUNCH_PHASE_CLASH;
					//リザルトシーンに遷移
					if (!GetResultStart())
					{
						SetResultStart_true();
					}
				}
			}
		}
		else
		{
			g_PunchFrameCnt = 0;
		}
	}

}
int Getg_PunchFrameCnt()
{
	return g_PunchFrameCnt;
}

void PlayTutorial()
{

	Tutorial_cnt++;
	if (g_TutorialPhase == TUTORIAL_1)
	{
		//ゲーム開始～避け方の説明終了まで
		if (Tutorial_cnt < TUTORIAL_1_FRAME)
		{
		}
		else
		{
			//一回目殴って次の説明、もう一回殴って次のフェーズへ
			if (Keyboard_IsTrigger(DIK_NUMPADENTER) || Keyboard_IsTrigger(DIK_RETURN))
			{
				g_Description_cnt++;

				if (g_Description_cnt == 2)
				{
					g_TutorialPhase = TUTORIAL_2;
					//カウント初期化
					Tutorial_cnt = 0;
					//敵の行動を許可する
					g_EnemyMove_Flg = true;
				}
			}
		}
	}
	if (g_TutorialPhase == TUTORIAL_2)
	{
		//左パンチ開始～回避待ちまで
		if (Tutorial_cnt < TUTORIAL_2_FRAME)
		{
		}
		else
		{
			//敵の行動を制限する
			g_EnemyMove_Flg = false;
			if (GetLrFlg())
			{
				g_TutorialPhase = TUTORIAL_3;
				Tutorial_cnt = 0;

				//敵の行動を許可する
				g_EnemyMove_Flg = true;
			}
		}
	}
	if (g_TutorialPhase == TUTORIAL_3)
	{
		//回避成功～予兆説明
		if (Tutorial_cnt < TUTORIAL_3_FRAME)
		{

		}
		else
		{
			//敵の行動を許可する
			g_EnemyMove_Flg = false;

			//一回殴ってギリギリ避けの説明、もう一回殴って次のフェーズへ
			if (Keyboard_IsTrigger(DIK_NUMPADENTER) || Keyboard_IsTrigger(DIK_RETURN))
			{
				g_Description_cnt++;
				if (g_Description_cnt == 5)
				{
					Tutorial_cnt = 0;
					g_TutorialPhase = TUTORIAL_4;
				}
			}
		}
	}
	if (g_TutorialPhase == TUTORIAL_4)
	{
		//予兆説明終了～右パンチ開始まで
		if (Tutorial_cnt < TUTORIAL_4_FRAME)
		{
		}
		else
		{
			Tutorial_cnt = 0;
			g_TutorialPhase = TUTORIAL_5;
			//敵の行動を許可する
			g_EnemyMove_Flg = true;
		}
	}
	if (g_TutorialPhase == TUTORIAL_5)
	{
		//～右パンチ終了
		if (Tutorial_cnt < TUTORIAL_5_FRAME)
		{

		}
		else
		{
			//敵の行動を許可する
			g_EnemyMove_Flg = false;

			if (GetBigEnter() == true) {
				//右回避で次のフェーズへ
				if (Keyboard_IsRelease(DIK_D))
				{
					Tutorial_cnt = 0;
					g_TutorialPhase = TUTORIAL_6;

					//敵の行動を許可する
					g_EnemyMove_Flg = true;
				}
			}
			if (GetBigEnter() == false) {
				//右回避で次のフェーズへ
				if (Keyboard_IsPress(DIK_D))
				{
					Tutorial_cnt = 0;
					g_TutorialPhase = TUTORIAL_6;

					//敵の行動を許可する
					g_EnemyMove_Flg = true;
				}
			}

		}

	}
	if (g_TutorialPhase == TUTORIAL_6)
	{
		//チュートリアル終了の説明～ゲーム本編へ移行
		if (Tutorial_cnt < TUTORIAL_6_FRAME)
		{

		}
		else
		{
			g_EnemyMove_Flg = false;
			Tutorial_cnt = 0;
			g_TutorialPhase = TUTORIAL_7;
		}
	}
	if (g_TutorialPhase == TUTORIAL_7)
	{
		//チュートリアル終了の説明～ゲーム本編へ移行
		if (Tutorial_cnt < TUTORIAL_7_FRAME)
		{

		}
		else
		{
			g_EnemyMove_Flg = false;
			//殴って次のフェーズへ
			if (Keyboard_IsTrigger(DIK_NUMPADENTER) || Keyboard_IsTrigger(DIK_RETURN))
			{
				Tutorial_cnt = 0;
				g_EnemyMove_Flg = true;
				g_Tutorial_Flg = false;
			}
		}
	}
}

void DrawTutorial()
{
	if (g_Description_cnt == 0)
	{
		Sprite_Draw(TEXTURE_INDEX_TUTORIAL1,
			SCREEN_WIDTH / 2,
			SCREEN_HEIGHT / 2,
			0,
			0,
			512,
			288,
			512 / 2,
			288 / 2,
			1.0f,
			1.0f,
			0.0f,
			255);
	}
	else if (g_Description_cnt == 1)
	{
		Sprite_Draw(TEXTURE_INDEX_TUTORIAL2,
			SCREEN_WIDTH / 2,
			SCREEN_HEIGHT / 2,
			0,
			0,
			512,
			288,
			512 / 2,
			288 / 2,
			1.0f,
			1.0f,
			0.0f,
			255);
	}
	else if (g_Description_cnt == 2)
	{
		Sprite_Draw(TEXTURE_INDEX_TUTORIAL3,
			SCREEN_WIDTH / 2,
			SCREEN_HEIGHT / 2,
			0,
			0,
			512,
			288,
			512 / 2,
			288 / 2,
			1.0f,
			1.0f,
			0.0f,
			255);
	}
	else if (g_Description_cnt == 3)
	{
		Sprite_Draw(TEXTURE_INDEX_TUTORIAL4,
			SCREEN_WIDTH / 2,
			SCREEN_HEIGHT / 2,
			0,
			0,
			512,
			288,
			512 / 2,
			288 / 2,
			1.0f,
			1.0f,
			0.0f,
			255);
	}
	else if (g_Description_cnt == 4)
	{
		Sprite_Draw(TEXTURE_INDEX_TUTORIAL5,
			SCREEN_WIDTH / 2,
			SCREEN_HEIGHT / 2,
			0,
			0,
			512,
			288,
			512 / 2,
			288 / 2,
			1.0f,
			1.0f,
			0.0f,
			255);
	}
	else if (g_Description_cnt == 5)
	{
		Sprite_Draw(TEXTURE_INDEX_TUTORIAL6,
			SCREEN_WIDTH / 2,
			SCREEN_HEIGHT / 2,
			0,
			0,
			512,
			288,
			512 / 2,
			288 / 2,
			1.0f,
			1.0f,
			0.0f,
			255);
	}
	else if (g_Description_cnt == 6)
	{

	}
	else if (g_Description_cnt == 7)
	{

	}

}

TUTORIAL_PHASE GetTutorialPhase()
{
	return g_TutorialPhase;
}

bool GetEnemyMove_Flg()
{
	return g_EnemyMove_Flg;
}

bool GetTutorialFlg()
{
	return g_Tutorial_Flg;
}