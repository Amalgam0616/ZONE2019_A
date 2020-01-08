#pragma once

#include "main.h"

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitCamera(void);
void UninitCamera(void);
void UpdateCamera(void);

void SetCamera(void);

//各シーンごとの初期化
void Init_TitleCamera();
void Init_GameCamera();
void Init_RankingCamera();

//カメラの座標初期化関数
void CameraResetPos();

//カメラがY軸回転(水平移動)する関数
void Camera_Rot_Y();

//カメラがX軸回転(垂直移動)する関数
void Camera_Rot_X();

//現在のカメラの座標と注視点との距離を返す関数
//引数： 0:Xの距離を返す 1:Yの距離を返す 2:Zの距離を返す 3:全ての距離を返す
D3DXVECTOR3 Camera_Length(int Return_Num);

//固定軸移動：指定座標に移動する関数
//引数1：カメラの移動開始位置の座標
//引数2：カメラの終了位置の座標
//引数3：移動時間(秒)
void Camera_Move_Axis1(D3DXVECTOR3(Start_Pos), D3DXVECTOR3(End_Pos), float Move_Time);

//Y軸移動：Y軸移動する関数
//引数1：カメラの移動開始位置の座標
//引数2：どれだけY軸回転するか(正の値なら右に回り込む)
//引数3：移動時間(秒)
void Camera_Move_Yspin(D3DXVECTOR3(Start_Pos), float Rota_Y, float Rota_Time);

//X軸移動：X軸移動する関数
//引数1：カメラの移動開始位置の座標
//引数2：どれだけX軸回転するか(正の値なら上に回り込む)
//引数3：移動時間(秒)
void Camera_Move_Xspin(D3DXVECTOR3(Start_Pos), float Rota_X, float Rota_Time);

//追加部分1==========================================
//以下最後のパンチ関係のカメラワーク
void LastPunchCamera1();

void LastPunchCamera2();

void LastPunchCamera3();

void LastPunchCamera4();

void Finish_CameraMove();
//追加部分1==========================================