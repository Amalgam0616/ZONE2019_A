#pragma once

/*====================================================================================
作成者：(Amalgam)室上寛幸
作成開始日時：2019/11/07/14:25

敵を扱うソースコードです。
====================================================================================*/

#include "main.h"
#include "Xfile.h"

//瞬間移動パンチ（右）の初期位置、回転。数字はパーツの配列と同じ
#define R_FLASH_PUNCH_R_POS0 D3DXVECTOR3(-10.0f,15.0f,26.0f) 
#define R_FLASH_PUNCH_R_POS1 D3DXVECTOR3(-10.0f,30.0f,26.0f) 
#define R_FLASH_PUNCH_R_POS2 D3DXVECTOR3(-5.6f,21.0f,20.0f) 
#define R_FLASH_PUNCH_R_POS3 D3DXVECTOR3(-30.0f,28.0f,39.0f) 
#define R_FLASH_PUNCH_R_POS4 D3DXVECTOR3(-18.0f,1.0f,26.0f) 
#define R_FLASH_PUNCH_R_POS5 D3DXVECTOR3(-16.0f,2.85f,26.0f) 

#define R_FLASH_PUNCH_R_ROT0 D3DXVECTOR3(D3DXToRadian(0.0f),D3DXToRadian(20.0f),D3DXToRadian(0.0f)) 
#define R_FLASH_PUNCH_R_ROT1 D3DXVECTOR3(D3DXToRadian(-22.0f),D3DXToRadian(-26.0f),D3DXToRadian(0.0f)) 
#define R_FLASH_PUNCH_R_ROT2 D3DXVECTOR3(D3DXToRadian(32.0f),D3DXToRadian(162.0f),D3DXToRadian(90.0f)) 
#define R_FLASH_PUNCH_R_ROT3 D3DXVECTOR3(D3DXToRadian(90.0f),D3DXToRadian(140.0f),D3DXToRadian(180.0f)) 
#define R_FLASH_PUNCH_R_ROT4 D3DXVECTOR3(D3DXToRadian(0.0f),D3DXToRadian(0.0f),D3DXToRadian(0.0f)) 
#define R_FLASH_PUNCH_R_ROT5 D3DXVECTOR3(D3DXToRadian(0.0f),D3DXToRadian(0.0f),D3DXToRadian(0.0f)) 

//瞬間移動パンチ（左）の初期位置、回転。数字はパーツの配列と同じ
#define L_FLASH_PUNCH_POS0 D3DXVECTOR3(10.0f,15.0f,26.0f) 
#define L_FLASH_PUNCH_POS1 D3DXVECTOR3(10.0f,30.0f,26.0f) 
#define L_FLASH_PUNCH_POS2 D3DXVECTOR3(25.4f,27.2f,35.9f) 
#define L_FLASH_PUNCH_POS3 D3DXVECTOR3(2.5f,21.0f,23.4f) 
#define L_FLASH_PUNCH_POS4 D3DXVECTOR3(24.0f,1.0f,26.0f) 
#define L_FLASH_PUNCH_POS5 D3DXVECTOR3(6.0f,2.85f,26.0f) 

#define L_FLASH_PUNCH_ROT0 D3DXVECTOR3(D3DXToRadian(-2.4f),D3DXToRadian(-8.0f),D3DXToRadian(0.0f)) 
#define L_FLASH_PUNCH_ROT1 D3DXVECTOR3(D3DXToRadian(-26.5f),D3DXToRadian(21.4f),D3DXToRadian(0.0f)) 
#define L_FLASH_PUNCH_ROT2 D3DXVECTOR3(D3DXToRadian(90.0f),D3DXToRadian(121.0f),D3DXToRadian(55.6f)) 
#define L_FLASH_PUNCH_ROT3 D3DXVECTOR3(D3DXToRadian(20.3f),D3DXToRadian(155.7f),D3DXToRadian(231.5f)) 
#define L_FLASH_PUNCH_ROT4 D3DXVECTOR3(D3DXToRadian(0.0f),D3DXToRadian(0.0f),D3DXToRadian(0.0f)) 
#define L_FLASH_PUNCH_ROT5 D3DXVECTOR3(D3DXToRadian(0.0f),D3DXToRadian(0.0f),D3DXToRadian(0.0f))




//パンチのフェーズ(状態)
enum PUNCH_PHASE
{
	PUNCH_PHASE_CHARGE = 0,
	PUNCH_PHASE_SWING,
	PUNCH_PHASE_RETURN,

};

//パンチパターン収納
enum PUNCH_PATTERN_INDEX
{
	PPI_RIGHT_PUNCH = 0,	//右通常パンチ
	PPI_LEFT_PUNCH,			//左通常パンチ
	R_FLASH_PUNCH,			//右瞬間移動パンチ
	L_FLASH_PUNCH,			//左瞬間移動パンチ
	PPI_DUNK_PUNCH,			//両手バーンてするパンチ

	PUNCH_NULL,				//パンチしてない状態

	PPI_MAX
};


//初期化
HRESULT InitEnemy();
//終了
void UninitEnemy();
//描画
void DrawEnemy();
//更新
void UpdateEnemy();

//その他

//モデルを初期位置に戻す関数
void EnemyPosReset();

void EnemyUpperPos();

void EnmAnimation();

void R_FLASH_PUNCH_R();

//パンチ関数群
//右通常パンチ
void RightPunch();
//左通常パンチ
void LeftPunch();
//右瞬間移動パンチ
void Right_R_FLASH_PUNCH();
//左瞬間移動パンチ
void Left_R_FLASH_PUNCH();
//両手バーンてするパンチ
void DunkPunch();

//パンチ後に元の位置に戻すやつ作るやつ
void CreatePunchEndVec();

//敵のGetter
XMODEL* GetEnemy();

//パンチ中か否かのGetter
bool GetPunch_Flg();

//パンチフェーズのGetter
PUNCH_PHASE GetPunchPhase();

//パンチの左右Getter
bool GetPunchLR();

//撃っているパンチの種類のGetter
PUNCH_PATTERN_INDEX GetPunchIndex();