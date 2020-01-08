#pragma once


/*====================================================================================
作成者：(Amalgam)室上寛幸
作成開始日時：2019/11/07/14:25

プレイヤーを扱うソースコードです。
====================================================================================*/

#include "main.h"
#include "Xfile.h"

//アニメーションのインデックス
enum PLAYER_ANIME_INDEX
{
	PLANIME_INDEX_NORMAL = 0,
	PLANIME_INDEX_DODGE_L,
	PLANIME_INDEX_DODGE_R,
	PLANIME_INDEX_DAMAGE,
	//追加部分1==========================================
	PLANIME_INDEX_FINISHPUNCH,
	//追加部分1==========================================
	ANIME_INDEX_MAX,
};

//アニメーション関連構造体
typedef struct
{
	int		StrFrame;		//開始から終了まで

	int		EndFrame;		//終了から初期位置まで

	D3DXVECTOR3		Angle;	//角度
	D3DXVECTOR3		Move;	//移動距離
}PL_ANIME;

//初期化
HRESULT InitPlayer();
//終了
void UninitPlayer();
//描画
void DrawPlayer();
//更新
void UpdatePlayer();

//その他

//無理矢理アニメーションさせる関数
void Animation();

//アニメステート毎の動き(今後削除する可能性アリ)
void AnimMovingParFrame();


void PlayerPosReset();

//追加部分2==========================================
void Finish_Punch_Pos();

void Finish_Punch();
//追加部分2==========================================

//プレイヤーのGetter
XMODEL* GetPlayer();

bool GetDodgeFlg();

bool GetLrFlg();

