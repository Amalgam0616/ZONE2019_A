
#include "myDirect3D.h"
#include "input.h"
#include "Xfile.h"
#include "player.h"
#include "texture.h"
#include "sprite.h"
#include "enemy.h"
#include "Effect.h"
#include "score.h"
#include "sound.h"
#include "gauge.h"
//定数定義 ==========================================================================

//プレイヤーに使うモデル数
#define		PLAYER_MODEL_NUM	(6)

//回避関連
//回避動き時間
#define		MOVE_FRAME_MAX		(5)
//回避終了
#define		MOVE_FRAME_END		(60)
//左右わかりやすくするやつ
#define		LEFT				(true)
#define		RIGHT				(false)

//グローバル変数定義 =================================================================

//プレイヤーのXMODEL宣言初期化
//0:胴　1:頭　2:左手　3:右手　4:左足　5:右足
XMODEL g_Player[PLAYER_MODEL_NUM] = {};

//足の角度(凸作)
float g_Leg_Rotate;
static bool Leg_Flg;
static bool Step_Stop;
static bool g_Once_AddScore_Flg;

//アッパー用宣言
static int Upper_Frame;
static int Upper_Phase;

//ダメージフラグ
bool damage_flag;

//ワールド座標
static D3DXMATRIXA16 g_mtxWorld;

//回避関連
//回避フラグ
static bool g_DodgeFlg;
//左右フラグ (true:L false:R)
static bool g_LrFlg;

//スコアを追加するフラグ
static bool g_AddScoreFlg;

//スコアを減らすフラグ
static bool g_Reduce_ScoreFlg;

//アッパー関連
static float Body_Move1;
static float Body_Move2;
static float Head_Move1;
static float Grobe_Grobe_L;
static float Grobe_Grobe_R;
static float Rotate_Z_R2;
static float Rotate_Z_R3;
static float Leg_L_Move;
static float Leg_R_Move;


//アニメーションのリスト
static PL_ANIME g_AnimeList[ANIME_INDEX_MAX] =
{
	//ANIME_INDEX_MORMAL　待機モーション
	{ 10, 10, D3DXVECTOR3(0.0f, D3DXToRadian(0.0f), 0.0f),D3DXVECTOR3(0.0f, 1.0f, 0.0f)},
	//ANIME_INDEX_DODGE_L　左回避モーション　勝手に戻るモードの時は2個目の要素を25くらいにして
	{ 6, 10, D3DXVECTOR3(0.0f, 0.0f, D3DXToRadian(10.0f)), D3DXVECTOR3(7.0f, -1.8f, 0.0f)},
	//ANIME_INDEX_DODGE_R　右回避モーション　勝手に戻るモードの時は2個目の要素を25くらいにして
	{ 6, 10, D3DXVECTOR3(0.0f, 0.0f, D3DXToRadian(-10.0f)), D3DXVECTOR3(-7.0f, -1.8f, 0.0f)},
	//ANIME_INDEX_DAMAGE　ダメージモーション
	{ 3, 25, D3DXVECTOR3(D3DXToRadian(30.0f), 0.0f, 0.0f), D3DXVECTOR3(0.0f, -1.0f, -2.5f)},
};

//アニメーションのフレーム数カウント
static int g_AnimFrameCnt;
//一瞬前のアニメーションカウンタ
static int OldFrame;
//アニメーション止めるフラグ
static bool AnimStopFlg;

//待機状態か否か 待機：false それ以外：true
static bool isAction;

//プレイヤーが今行っているアニメステート
static PLAYER_ANIME_INDEX g_PlAnimState;

//1フレーム当たりの角度変化量
static D3DXVECTOR3 g_AnglePerFrame;

//1フレーム当たりの座標変化量
static D3DXVECTOR3 g_MovePerFrame;

void DamageEffect();
//基本関数定義 ======================================================================

//初期化
HRESULT InitPlayer()
{
	//読み込み・初期値設定
	{
		//XFileから3Dモデルをロード
		if (FAILED(LoadXFile(&g_Player[0], XFILE_INDEX_P_BODY_001)) ||		//胴体
			FAILED(LoadXFile(&g_Player[1], XFILE_INDEX_P_HEAD_001)) ||		//頭
			FAILED(LoadXFile(&g_Player[2], XFILE_INDEX_P_GROBE_L_001)) ||	//左手(前からみて右)
			FAILED(LoadXFile(&g_Player[3], XFILE_INDEX_P_GROBE_R_001)) ||	//右手(前からみて左)
			FAILED(LoadXFile(&g_Player[4], XFILE_INDEX_P_LEG_L_001)) ||		//左足(前からみて右)
			FAILED(LoadXFile(&g_Player[5], XFILE_INDEX_P_LEG_R_001)))		//右足(前からみて左)
		{
			//失敗したらここ
			return E_FAIL;
		}

		PlayerPosReset();
	}

	//各種変数初期化
	g_Leg_Rotate = 0.0f;
	Leg_Flg = false;
	g_Once_AddScore_Flg = false;

	g_DodgeFlg = false;
	g_LrFlg = false;
	g_AddScoreFlg = false;
	g_Reduce_ScoreFlg = false;

	Step_Stop = false;

	//アニメーション関連
	g_AnimFrameCnt = 0;
	g_PlAnimState = PLANIME_INDEX_NORMAL;
	isAction = false;
	g_AnglePerFrame = {};
	g_MovePerFrame = {};
	OldFrame = g_AnimFrameCnt;
	AnimStopFlg = false;

	//アッパー関連
	Upper_Frame = 0;
	Upper_Phase = -1;
	Body_Move1 = 0.0f;
	Body_Move2 = 0.0f;
	Head_Move1 = 0.0f;
	Grobe_Grobe_L = 0.0f;
	Grobe_Grobe_R = 0.0f;
	Rotate_Z_R2 = 1.0f;
	Rotate_Z_R3 = 1.0f;
	Leg_L_Move = 0.0f;
	Leg_R_Move = 0.0f;


	damage_flag = false;
	return S_OK;
}
//終了
void UninitPlayer()
{
	//メッシュの解放
	for (int i = 0; i < PLAYER_MODEL_NUM; i++)
	{
		SAFE_RELEASE(g_Player[i].pMesh);
	}
}

//更新
void UpdatePlayer()
{
	if (Keyboard_IsTrigger(DIK_NUMPADENTER))
	{
		Step_Stop = true;
	}
	else if (Keyboard_IsTrigger(DIK_RETURN))
	{
		Step_Stop = true;
	}

	if (!Step_Stop)
	{
		//アニメーション進行
		Animation();
	}
	else
	{
		if (Upper_Phase == -1)
		{
			//Upper_Phaseを0にする
			Upper_Phase = 0;
		}
		Upper();
	}

	//待機モーション以外のアニメーションを行っている且つ、敵がパンチ中でない場合はキー入力を行えない
	if (!isAction)
	{
		if (GetPunch_Flg())
		{
			//キー入力に応じてステート変化
			if (!isAction && Keyboard_IsTrigger(DIK_D) && !g_DodgeFlg)
			{//右回避
				g_PlAnimState = PLANIME_INDEX_DODGE_L;
				//アニメーションフレーム初期化
				g_AnimFrameCnt = 0;
				//アクション中にする
				isAction = true;

				//回避方向にフラグを立てる
				g_LrFlg = true;
				//スコアを追加するフラグを立てる
				g_AddScoreFlg = true;

				//スコアを増やす
				if (g_AddScoreFlg == true && g_Once_AddScore_Flg == false && GetPunchIndex() == PPI_RIGHT_PUNCH)
				{
					AddScore(1000);
					AddGauge(10);
					//一度のみスコアを加えるフラグを正にする
					g_Once_AddScore_Flg = true;
					//回避フラグを正にする
					g_DodgeFlg = true;
				}

				if (g_Once_AddScore_Flg == false && GetPunchIndex() == R_FLASH_PUNCH)
				{
					AddScore(1000);
					AddGauge(10);
					//一度のみスコアを加えるフラグを正にする
					g_Once_AddScore_Flg = true;
					//回避フラグを正にする
					g_DodgeFlg = true;
				}

				if (g_Once_AddScore_Flg == false && GetPunchIndex() == PPI_DUNK_PUNCH)
				{
					//回避フラグを正にする
					g_DodgeFlg = true;
				}
			}
			else if (!isAction && Keyboard_IsTrigger(DIK_A) && !g_DodgeFlg)
			{//左回避
				//ステート変化
				g_PlAnimState = PLANIME_INDEX_DODGE_R;
				//アニメーションフレーム初期化
				g_AnimFrameCnt = 0;
				//アクション中にする
				isAction = true;

				//回避方向にフラグを立てる
				g_LrFlg = false;
				//スコアを追加するフラグを立てる
				g_AddScoreFlg = true;

				//スコアを増やす
				if (g_AddScoreFlg == true && g_Once_AddScore_Flg == false && (GetPunchIndex() == PPI_LEFT_PUNCH))
				{
					AddScore(1000);
					//一度のみスコアを加えるフラグを正にする
					g_Once_AddScore_Flg = true;
					//回避フラグを正にする
					g_DodgeFlg = true;
				}

				if (g_Once_AddScore_Flg == false && GetPunchIndex() == L_FLASH_PUNCH)
				{
					AddGauge(10);
					AddScore(1000);
					//一度のみスコアを加えるフラグを正にする
					g_Once_AddScore_Flg = true;
					//回避フラグを正にする
					g_DodgeFlg = true;
				}

				if (g_Once_AddScore_Flg == false && GetPunchIndex() == PPI_DUNK_PUNCH)
				{
					//回避フラグを正にする
					g_DodgeFlg = true;
				}
			}
		}
	}

	//ダメージモーション発生
	//両手のパンチじゃない場合
	if (GetPunchIndex() != PPI_DUNK_PUNCH)
	{
		if (GetPunchPhase() == PUNCH_PHASE_RETURN && !g_DodgeFlg && g_PlAnimState != PLANIME_INDEX_DAMAGE)
		{
			g_PlAnimState = PLANIME_INDEX_DAMAGE;
			//アニメーションフレーム初期化
			g_AnimFrameCnt = 0;
			//アクション中にする
			isAction = true;
			//アニメーションストップを解除する
			AnimStopFlg = false;
		}
	}
	else	//両手のパンチの場合
	{
		if (GetPunchPhase() == PUNCH_PHASE_RETURN && g_DodgeFlg && g_PlAnimState != PLANIME_INDEX_DAMAGE)
		{
			g_PlAnimState = PLANIME_INDEX_DAMAGE;
			//アニメーションフレーム初期化
			g_AnimFrameCnt = 0;
			//アクション中にする
			isAction = true;
			//アニメーションストップを解除する
			AnimStopFlg = false;
		}
		else if (GetPunchPhase() == PUNCH_PHASE_RETURN && g_Once_AddScore_Flg == false && !g_DodgeFlg)
		{
			AddGauge(10);
			AddScore(1000);
			//一度のみスコアを加えるフラグを正にする
			g_Once_AddScore_Flg = true;
		}
	}

	//回避ボタンがどっちも押されていたらアニメーションストップを解除する
	//デバッグの時は「押されていないとき」にしといて
	if ((AnimStopFlg && !Keyboard_IsPress(DIK_A) && !Keyboard_IsPress(DIK_D)))
	{
		AnimStopFlg = false;
	}

	//アニメがストップされていなかったらフレーム進行
	if (!AnimStopFlg)
	{
		g_AnimFrameCnt++;	//ここで　1　以上になる
	}

	//デバッグ(対称実験用)
	//AnimStopFlg = false;

	//スコアの再取得を可能にする
	if (g_Once_AddScore_Flg && !GetPunch_Flg())
	{
		g_Once_AddScore_Flg = false;
	}
}
//描画
void DrawPlayer()
{
	//Direct3DDeviceの取得
	LPDIRECT3DDEVICE9 p_D3DDevice = GetD3DDevice();

	//ワールドトランスフォーム(絶対座標変換)
	D3DXMatrixIdentity(&g_mtxWorld);

	//モデル分繰り返す
	for (int j = 5; j >= 0; j--)
	{
		//ワールドマトリクス作って適用
		p_D3DDevice->SetTransform(D3DTS_WORLD, MakeWorldMatrix(&g_Player[j], &g_mtxWorld));

		//マテリアルの数だけ繰り返し
		for (DWORD i = 0; i < g_Player[j].dwNumMaterials; i++)
		{
			//マテリアルセット
			p_D3DDevice->SetMaterial(&g_Player[j].pMaterials[i]);
			//テクスチャセット
			p_D3DDevice->SetTexture(0, g_Player[j].pTextures[i]);
			//透過度設定
			//避けるフェーズ中グローブだけ透明にしない
			if (Upper_Phase == -1) {
				if (j == 2 || j == 3) {
					g_Player[j].pMaterials[i].Diffuse.a = 1.0f;
				}
				else {
					g_Player[j].pMaterials[i].Diffuse.a = 0.3f;
				}
			}
			//避けるフェーズ中グローブだけ透明にしない
			if (Upper_Phase >= 1) {
				g_Player[j].pMaterials[i].Diffuse.a = 1.0f;
			}
			//いわゆる描画
			g_Player[j].pMesh->DrawSubset(i);
		}
	}
	//ダメージを受けたときのやつ
	if (damage_flag)
	{
		DamageEffect();
	}
	//決めろ！のテクスチャ
	if (Upper_Phase == 1) {
		Sprite_Draw
		(
			TEXTURE_INDEX_PUSHUPPER,
			SCREEN_HEIGHT / 2 + 350,
			SCREEN_WIDTH / 2 - 50,
			0,
			0,
			800,
			800,
			800 / 2,
			800 / 2,
			0.5f,
			0.5f,
			0.0f
		);
	}
}

//その他いろんな関数定義 =============================================================

//無理矢理アニメーションさせる関数
void Animation()
{
	//g_AnimFrameCnt が OldFrame と同じ(加算されていない)場合はアニメーションしない
	if (g_AnimFrameCnt != OldFrame)
	{
		//g_AnimFrame　が　0　以上且つ　StrFrame　以下だった場合は、アニメステート毎に動かす部位を決めて１フレーム分動かす
		if ((g_AnimFrameCnt >= 0 + 1) && (g_AnimFrameCnt <= g_AnimeList[g_PlAnimState].StrFrame + 1))
		{
			//g_AnimFrameCnt　が　0　だった場合は、モデルの座標・回転を初期位置に戻して、アニメステート毎に１フレーム当たりの動きを計算
			if (g_AnimFrameCnt == 0 + 1)
			{
				PlayerPosReset();
				Leg_Flg = true;

				//1フレーム当たりの角度
				g_AnglePerFrame = D3DXVECTOR3(g_AnimeList[g_PlAnimState].Angle.x / g_AnimeList[g_PlAnimState].StrFrame,
					g_AnimeList[g_PlAnimState].Angle.y / g_AnimeList[g_PlAnimState].StrFrame,
					g_AnimeList[g_PlAnimState].Angle.z / g_AnimeList[g_PlAnimState].StrFrame);

				//1フレーム当たりの移動
				g_MovePerFrame = D3DXVECTOR3(g_AnimeList[g_PlAnimState].Move.x / g_AnimeList[g_PlAnimState].StrFrame,
					g_AnimeList[g_PlAnimState].Move.y / g_AnimeList[g_PlAnimState].StrFrame,
					g_AnimeList[g_PlAnimState].Move.z / g_AnimeList[g_PlAnimState].StrFrame);
			}

			//アニメステート毎の動き
			AnimMovingParFrame();
		}
		//g_AnimFrameCnt　が　StrFrame + 1　以上且つ　StrFrame + EndFrame　以下だった場合は、アニメステート毎に動かす部位を決めて１フレーム分動かす
		else if ((g_AnimFrameCnt >= g_AnimeList[g_PlAnimState].StrFrame + 2) && (g_AnimFrameCnt <= (g_AnimeList[g_PlAnimState].StrFrame + g_AnimeList[g_PlAnimState].EndFrame + 1)))
		{
			//g_AnimFrameCnt　が　StrFrame　より　1　大きい場合は、1フレーム当たりの移動距離を戻り用に再計算する
			if (g_AnimFrameCnt == (g_AnimeList[g_PlAnimState].StrFrame + 2))
			{
				Leg_Flg = false;
				//角度　全体の角度に戻して、帰りのフレーム数で割って、反転
				g_AnglePerFrame = D3DXVECTOR3((g_AnimeList[g_PlAnimState].Angle.x / g_AnimeList[g_PlAnimState].EndFrame) * -1.0f,
					(g_AnimeList[g_PlAnimState].Angle.y / g_AnimeList[g_PlAnimState].EndFrame) * -1.0f,
					(g_AnimeList[g_PlAnimState].Angle.z / g_AnimeList[g_PlAnimState].EndFrame) * -1.0f);

				//移動量　全体の移動量に戻して、帰りのフレーム数で割って、反転
				g_MovePerFrame = D3DXVECTOR3((g_AnimeList[g_PlAnimState].Move.x / g_AnimeList[g_PlAnimState].EndFrame) * -1.0f,
					(g_AnimeList[g_PlAnimState].Move.y / g_AnimeList[g_PlAnimState].EndFrame) * -1.0f,
					(g_AnimeList[g_PlAnimState].Move.z / g_AnimeList[g_PlAnimState].EndFrame) * -1.0f);

				//回避中且つどっちかが押されていない状態だったらアニメーションをストップする
				if ((g_PlAnimState == PLANIME_INDEX_DODGE_L) || (g_PlAnimState == PLANIME_INDEX_DODGE_R))
				{
					//デバッグの時は！つけないで、フットペダルつけるときは！つけて。
					if ((Keyboard_IsPress(DIK_D)) || (Keyboard_IsPress(DIK_A)))
					{
						AnimStopFlg = true;
					}
				}
			}

			//アニメステート毎の動き
			AnimMovingParFrame();
		}
		//g_AnimFrameCnt　が　StrFrame + EndFrame + 1　以上だった場合は、アニメーションを終了して待機モーションに戻す
		else if (g_AnimFrameCnt >= (g_AnimeList[g_PlAnimState].StrFrame + g_AnimeList[g_PlAnimState].EndFrame + 2))
		{

			//g_PlAnimStateを待機モーションに変える
			g_PlAnimState = PLANIME_INDEX_NORMAL;
			//g_AnimFrameCntを0に戻す
			g_AnimFrameCnt = 0;
			//フレーム当たりの移動回転を0に戻す
			g_AnglePerFrame = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
			g_MovePerFrame = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

			//isActionがtrueだったらfalseに戻す
			if (isAction)
			{
				isAction = false;
			}

			//回避とかも戻す
			g_DodgeFlg = false;
			g_LrFlg = false;
		}

		//OldFrameを同期する
		OldFrame = g_AnimFrameCnt;
	}
}

//アニメステート毎の動き(今後削除するかもしれない)
void AnimMovingParFrame()
{
	switch (g_PlAnimState)
	{
	case PLANIME_INDEX_NORMAL:
		//待機モーション　頭、胴、両腕　を動かす
		for (int i = 0; i < PLAYER_MODEL_NUM - 2; i++)
		{
			damage_flag = false;
			//座標移動
			g_Player[i].Pos += g_MovePerFrame;
			//角度変化
			g_Player[i].Rot += g_AnglePerFrame;
		}

		break;
	case PLANIME_INDEX_DODGE_R:

		//左回避モーション　頭、胴、両腕　を動かす
		//座標移動
		g_Player[0].Pos += g_MovePerFrame;
		g_Player[1].Pos += g_MovePerFrame * 1.25f;
		g_Player[2].Pos += g_MovePerFrame * 1.2f;
		g_Player[3].Pos += g_MovePerFrame * 1.0f;
		g_Player[4].Pos.x += g_MovePerFrame.x / 2.0f;
		g_Player[5].Pos.x += g_MovePerFrame.x / 2.0f;
		//角度変化
		g_Player[0].Rot += g_AnglePerFrame;
		g_Player[1].Rot += g_AnglePerFrame * 2.0f;
		g_Player[2].Rot -= g_AnglePerFrame * 1.2f;
		g_Player[3].Rot += g_AnglePerFrame;
		break;
	case PLANIME_INDEX_DODGE_L:

		//右回避モーション　頭、胴、両腕　を動かす
		//座標移動
		g_Player[0].Pos += g_MovePerFrame;
		g_Player[1].Pos += g_MovePerFrame * 1.3f;
		g_Player[2].Pos += g_MovePerFrame * 1.05f;
		g_Player[3].Pos += g_MovePerFrame * 1.25f;
		g_Player[4].Pos.x += g_MovePerFrame.x / 2.05f;
		g_Player[5].Pos.x += g_MovePerFrame.x / 2.05f;
		//角度変化
		g_Player[0].Rot += g_AnglePerFrame;
		g_Player[1].Rot += g_AnglePerFrame * 2.0f;
		g_Player[2].Rot += g_AnglePerFrame;
		g_Player[3].Rot -= g_AnglePerFrame * 1.2f;
		break;
	case PLANIME_INDEX_DAMAGE:
		damage_flag = true;
		//ダメージモーション　頭、胴、両腕　を動かす
		//座標移動
		g_Player[0].Pos += g_MovePerFrame;
		g_Player[1].Pos += g_MovePerFrame * 2.0f;
		g_Player[2].Pos += g_MovePerFrame;
		g_Player[3].Pos += g_MovePerFrame;
		//角度変化
		g_Player[0].Rot += g_AnglePerFrame;
		g_Player[1].Rot += g_AnglePerFrame;
		g_Player[2].Rot += g_AnglePerFrame;
		g_Player[3].Rot += g_AnglePerFrame;
		break;
	default:
		break;
	}
}

//回避フラグのGetter
bool GetDodgeFlg()
{
	return g_DodgeFlg;
}

//左右フラグのGetter
bool GetLrFlg()
{
	return g_LrFlg;
}

void PlayerPosReset()
{

	//座標・回転・拡縮　初期値設定
	//胴体
	g_Player[0].Pos = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
	g_Player[0].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(180.0f), D3DXToRadian(0.0f));
	g_Player[0].Scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	//頭
	g_Player[1].Pos = D3DXVECTOR3(0.0f, 8.0f, 0.0f);
	g_Player[1].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(180.0f), D3DXToRadian(0.0f));
	g_Player[1].Scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	//左腕
	g_Player[2].Pos = D3DXVECTOR3(-3.5f, 4.0f, 3.0f);
	g_Player[2].Rot = D3DXVECTOR3(D3DXToRadian(165.0f), D3DXToRadian(180.0f), D3DXToRadian(255.0f));
	g_Player[2].Scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	//右腕
	g_Player[3].Pos = D3DXVECTOR3(3.5f, 2.5f, 3.0f);
	g_Player[3].Rot = D3DXVECTOR3(D3DXToRadian(165.0f), D3DXToRadian(180.0f), D3DXToRadian(105.0f));
	g_Player[3].Scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	//左足
	g_Player[4].Pos = D3DXVECTOR3(-2.5f, -7.0f, 0.0f);
	g_Player[4].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(180.0f), D3DXToRadian(0.0f));
	g_Player[4].Scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
	//右足
	g_Player[5].Pos = D3DXVECTOR3(2.5f, -7.0f, 0.0f);
	g_Player[5].Rot = D3DXVECTOR3(D3DXToRadian(0.0f), D3DXToRadian(180.0f), D3DXToRadian(0.0f));
	g_Player[5].Scale = D3DXVECTOR3(1.0f, 1.0f, 1.0f);
}

void Upper()
{
	//フレーム加算
	Upper_Frame++;

	//フェーズ毎の処理
	switch (Upper_Phase)
	{
		//ステップのフェーズ
	case 0:
		if (Upper_Frame < 10)
		{
			static float Phase1_Frame = 10.0f;
			Upper_Body1();
			Upper_Head1();
			Upper_Grobe_L1();
			Upper_Grobe_R1();
			Upper_Leg_L();
			Upper_Leg_R1();
		}
		else
		{
			//次のフェーズの準備&フレームリセット(以後else文全てに搭載)
			g_Player[3].Rot = D3DXVECTOR3(D3DXToRadian(180.0f), D3DXToRadian(90.0f), D3DXToRadian(0.0f));
			//g_Player[0].Rot.y +=D3DXToRadian(30.0f);
			Upper_Phase++;
			Upper_Frame = 0;
		}
		break;

		//キー入力
	case 1:
		if (Keyboard_IsTrigger(DIK_RETURN))
		{
			Upper_Phase++;
			Upper_Frame = 0;
		}
		else if (Keyboard_IsTrigger(DIK_NUMPADENTER))
		{
			Upper_Phase++;
			Upper_Frame = 0;
		}
		break;

		//アッパーの体に当たるまでのフェーズ
	case 2:

		//腕を体まで動かす処理
		if (Upper_Frame < 8)
		{
			static float Phase2_Frame = 10.0f;
			Upper_Grobe_R2(Phase2_Frame);
		}
		//ヒットストップ
		if (Upper_Frame >= 10) {
			Upper_Phase++;
			Upper_Frame = 0;
		}
		break;

		//アッパーの体に当たった後のフェーズ
	case 3:
		//体から腕を振り切る処理
		if (Upper_Frame < 8)
		{
			//スコアを増やす
			AddScore(111111);
			static float Phase3_Frame = 10.0f;
			Upper_Grobe_R3(Phase3_Frame);
		}
		break;

	default:
		break;
	}
}

//パーツごとのアッパー中の動き
void Upper_Body1()
{

	Body_Move1 = Body_Move1 + 0.75f;
	g_Player[0].Pos.y -= 0.05f;
	g_Player[0].Pos.z = Body_Move1;
	g_Player[0].Rot.x -= D3DXToRadian(3.0f);
}

void Upper_Body2()
{
	Body_Move2 += 3.0f;
	g_Player[0].Rot.y = D3DXToRadian(Body_Move2);
}

void Upper_Head1()
{
	Head_Move1 = Head_Move1 + 1.2f;
	g_Player[1].Pos.y -= 0.12f;
	g_Player[1].Pos.z = Head_Move1;
	g_Player[1].Rot.x -= D3DXToRadian(3.0f);
}

void Upper_Grobe_L1()
{
	Grobe_Grobe_L = Grobe_Grobe_L + 0.75f;
	g_Player[2].Pos.y -= 0.05f;
	g_Player[2].Pos.z = Grobe_Grobe_L * 1.5f;
	g_Player[2].Rot.x -= D3DXToRadian(3.0f);

}

void Upper_Grobe_R1()
{
	Grobe_Grobe_R = Grobe_Grobe_R + 0.75f;
	g_Player[3].Pos.y -= 0.05f;
	g_Player[3].Pos.z = Grobe_Grobe_R * 1.5f;
	g_Player[3].Rot.x -= D3DXToRadian(3.0f);

}

void Upper_Grobe_R2(float Time)
{
	Rotate_Z_R2 = Rotate_Z_R2 * 1.35f;
	g_Player[3].Pos.x += D3DXToRadian(90.0f / Time) * -1.8f;
	g_Player[3].Pos.y += D3DXToRadian(90.0f / Time) * 1.5f; //1.5f
	g_Player[3].Pos.z += D3DXToRadian(90.0f / Time) * 11.0f;
	g_Player[3].Rot.z += D3DXToRadian(Rotate_Z_R2);
}

void Upper_Grobe_R3(float Time)
{
	Rotate_Z_R3 = Rotate_Z_R3 * 1.35f;
	g_Player[3].Pos.y += D3DXToRadian(90.0f / Time) * 6.5f;
	g_Player[3].Pos.z += D3DXToRadian(90.0f / Time) * 7.0f;
	g_Player[3].Rot.z += D3DXToRadian(Rotate_Z_R3);
}

void Upper_Leg_L()
{
	Leg_L_Move += 1.5f;
	g_Player[4].Pos.z = Leg_L_Move;
}

void Upper_Leg_R1()
{
	Leg_R_Move += 0.05f;
	g_Player[5].Pos.y += Leg_R_Move;
	g_Player[5].Rot.x -= D3DXToRadian(8.0f);
}

//プレイヤーのGetter
XMODEL* GetPlayer()
{
	return g_Player;
}

//Upper_PhaseのGetter
int GetUpper_Phase()
{
	return Upper_Phase;
}
