#include "camera.h"
#include "myDirect3D.h"
#include "input.h"
#include <stdlib.h>
#include "player.h"
#include "enemy.h"
#include "scene.h"
#include "sky.h"
#include "game.h"
//=============================================================================
// マクロ定義
//=============================================================================

//Z座標初期値
#define CAMERA_POS_Z (80.0f)

//カメラの視点(カメラの座標)初期位置(これいる？)
#define	CAMERA_POSV_X		(0.0f)
#define	CAMERA_POSV_Y		(6.0f)
#define	CAMERA_POSV_Z		(-CAMERA_POS_Z)

//カメラの注視点初期位置(これいる？)
#define	CAMERA_POSR_X		(0.0f)
#define	CAMERA_POSR_Y		(3.0f)
#define	CAMERA_POSR_Z		(0.0f)

//ビュー平面の視野角(これは要りそう)
#define	VIEW_ANGLE			(D3DXToRadian(45.0f))

//ビュー平面のアスペクト比(これは要りそう)
#define	VIEW_ASPECT			((float)SCREEN_WIDTH / (float)SCREEN_HEIGHT)

//ビュー平面のNearZ、FarZ値(これは要りそう)
#define	VIEW_NEAR_Z			(1.0f)		//ここを0.0f指定してしまうと、無限に遠くまで描画するようになってしまうので注意
#define	VIEW_FAR_Z			(5000.0f)

//カメラのY軸の可動域
#define CAMERA_TOP_LIMIT    (330.0f)
#define CAMERA_BOTTOM_LIMIT (210.0f)

// カメラ移動時の半径(これいる？)
#define	CAMERA_RAD		(CAMERA_POS_Z)

//カメラの基本回転速度(後で特殊な動きさせるため仮置き)
#define CAMERA_SPEED	(5.0f)
#define TITLECAMERA_SPEED	(0.2f)

//移動時間(のちに消去希望)
#define AXIS1_TIME 3.0f
#define YSPIN_TIME 6.0f
#define XSPIN_TIME 3.0f


//=============================================================================
// グローバル変数
//=============================================================================
CAMERA g_Camera;					//カメラ情報
float g_CameraRot_X;				//カメラ角度(X)
float g_CameraRot_Y;				//カメラ角度(Y)
float g_CameraRot_Z;				//カメラ角度(Z)
static bool g_Reflect_Flg;			//タイトルで往復するときのフラグ
int g_TitleCamera_frame;			//タイトルのフレーム数
bool g_Player_Camera;				//カメラをプレイヤーの視点にするフラグ
int g_Upper_Cnt;					//アッパーの時にカメラを止めるカウント

//=========================================================================
//ここからカメラの特殊挙動用の変数
//=========================================================================
static float g_Axis1_Scond;	//仮の移動時間(単位は秒)
static bool g_Axis1_Flg;		//固定軸移動のスイッチ
static int g_Axis1_Time_Frame;	//固定軸移動の起動時間(移動時間をフレーム数で受け取る)

static float g_Yspin_Scond;		//仮の移動時間(単位は秒)
static bool g_Yspin_Flg;		//Y軸回転移動のスイッチ
static int g_Yspin_Time_Frame;	//Y軸回転移動の起動時間(移動時間をフレーム数で受け取る)

static float g_Xspin_Scond;		//仮の移動時間(単位は秒)
static bool g_Xspin_Flg;		//Y軸回転移動のスイッチ
static int g_Xspin_Time_Frame;	//Y軸回転移動の起動時間(移動時間をフレーム数で受け取る)

//=============================================================================
// カメラの初期化処理
//=============================================================================
void InitCamera(void)
{
	CameraResetPos();						//カメラの座標初期化
	g_Upper_Cnt = 0;

}

//=============================================================================
// カメラの終了処理
//=============================================================================
void UninitCamera(void)
{
	//シーンごとに初期化したりするんだろうけど、シームレスなシーン移行だと要らないのかもしれない
}

//=============================================================================
// カメラの更新処理
//=============================================================================
void UpdateCamera(void)
{
	//====================================================================================
	//カメラ座標変更
	//====================================================================================

	if (GetScene() == SCENE_INDEX_TITLE)
	{
		//g_TitleCamera_frame ++;
		if (g_Reflect_Flg && g_CameraRot_X > 330)
		{
			g_Reflect_Flg = false;
		}
		else if (!g_Reflect_Flg && g_CameraRot_X < 210)
		{
			g_Reflect_Flg = true;
		}

		//タイトルの時の更新処理
		if (g_Reflect_Flg)
		{
			//Camera_Move_Yspin(D3DXVECTOR3(CAMERA_POSV_X, CAMERA_POSV_Y, CAMERA_POSV_Z), 60.0f, 3.0f);
			g_CameraRot_X += TITLECAMERA_SPEED;
			g_CameraRot_Z += TITLECAMERA_SPEED;
			Camera_Rot_Y();
		}
		else
		{
			g_CameraRot_X -= TITLECAMERA_SPEED;
			g_CameraRot_Z -= TITLECAMERA_SPEED;
			Camera_Rot_Y();
		}
	}
	else if (GetScene() == SCENE_INDEX_GAME)
	{
		//ゲームの時の更新処理

		g_Player_Camera = true;

		//追加部分1(ここにあったデバック処理は恐らく使わない＆別のファイルに退避済みなので消してOK)==========================================
		//最後のパンチのカメラワーク関連
		if (GetLastPunchPhase() == PUNCH_PHASE_SET)
		{

		}
		else if (GetLastPunchPhase() == PUNCH_PHASE_PUNCH)
		{

		}
		else if (GetLastPunchPhase() == PUNCH_PHASE_SLOW)
		{

		}
		else if (GetLastPunchPhase() == PUNCH_PHASE_STOP)
		{
			//カメラを全体像が見える位置に移動させる
			g_Camera.posR = D3DXVECTOR3(0, 30.0f, 0);
			g_Camera.posV = D3DXVECTOR3(150.0f, 60.0f, 0);
		}
		else if (GetLastPunchPhase() == PUNCH_PHASE_YOIN)
		{
			g_Camera.posR = D3DXVECTOR3(0, 30.0f, 0);
			g_Camera.posV = D3DXVECTOR3(150.0f, 60.0f, 0);
		}
		else if (GetLastPunchPhase() >= PUNCH_PHASE_FLYAWAY)
		{
			//敵の顔をカメラの中心点にして吹っ飛んだ敵を追う
			Finish_CameraMove();
		}
		else
		{
			g_Camera.posV.x = (GetPlayer() + 1)->Pos.x;
			g_Camera.posV.z = (GetPlayer() + 1)->Pos.z - 7;
		}
		//追加部分1==========================================
	}
	else if (GetScene() == SCENE_INDEX_RANKING)
	{
		//ランキングの時の更新処理
	}

}

//=============================================================================
// カメラの設定処理
//=============================================================================

//カメラ情報の設定(Draw関数を使うところ(モデルとかをDrawする直前がベター)に置いてください)
void SetCamera(void)
{
	LPDIRECT3DDEVICE9 p_Device = GetD3DDevice();

	// プロジェクションマトリックスの初期化
	D3DXMatrixIdentity(&g_Camera.mtxProjection);

	// プロジェクションマトリックスの作成
	D3DXMatrixPerspectiveFovLH(&g_Camera.mtxProjection,
		VIEW_ANGLE,				// ビュー平面の視野角
		VIEW_ASPECT,			// ビュー平面のアスペクト比
		VIEW_NEAR_Z,			// ビュー平面のNearZ値
		VIEW_FAR_Z);			// ビュー平面のFarZ値

// プロジェクションマトリックスの設定
	p_Device->SetTransform(D3DTS_PROJECTION, &g_Camera.mtxProjection);

	// ビューマトリックスの初期化
	D3DXMatrixIdentity(&g_Camera.mtxView);

	// ビューマトリックスの作成
	D3DXMatrixLookAtLH(&g_Camera.mtxView,
		&g_Camera.posV,		// カメラの視点
		&g_Camera.posR,		// カメラの注視点
		&g_Camera.vecU);	// カメラの上方向ベクトル

// ビューマトリックスの設定
	p_Device->SetTransform(D3DTS_VIEW, &g_Camera.mtxView);
}

void Init_TitleCamera()
{
	CameraResetPos();						//カメラの座標初期化

	g_TitleCamera_frame = 0;				//タイトルのフレーム数の初期化
	g_Reflect_Flg = false;					//フラグの初期化
}

void Init_GameCamera()
{
	CameraResetPos();						//カメラの座標初期化

	//カメラの注視点の設定
	g_Camera.posR = D3DXVECTOR3(GetEnemy()->Pos.x, GetEnemy()->Pos.y + 7.0f, GetEnemy()->Pos.z);

	g_Axis1_Scond = AXIS1_TIME;				//固定軸移動の移動時間(バグるからとりあえず代入した)
	g_Axis1_Flg = false;					//フラグの初期化

	g_Yspin_Scond = YSPIN_TIME;				//Y軸回転移動の移動時間(バグるからとりあえず代入した)
	g_Yspin_Flg = false;					//フラグの初期化

	g_Xspin_Scond = XSPIN_TIME;				//X軸回転移動の移動時間(バグるからとりあえず代入した)
	g_Xspin_Flg = false;					//フラグの初期化
}

void Init_RankingCamera()
{
	g_Camera.posR = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
}

void Camera_Rot_Y()
{
	g_Camera.posV.x = cosf(D3DXToRadian(g_CameraRot_X)) * CAMERA_RAD;
	g_Camera.posV.z = sinf(D3DXToRadian(g_CameraRot_Z)) * CAMERA_RAD;
}

void Camera_Rot_X()
{
	g_Camera.posV.y = cosf(D3DXToRadian(g_CameraRot_Y)) * CAMERA_RAD;
}

D3DXVECTOR3 Camera_Length(int Return_Num)
{
	D3DXVECTOR3 leng = D3DXVECTOR3(0.0f, 0.0f, 0.0f);

	switch (Return_Num)
	{
	case 0:
		leng.x = fabs(g_Camera.posV.x - g_Camera.posR.x);
		return leng;

	case 1:
		leng.y = fabs(g_Camera.posV.y - g_Camera.posR.y);
		return leng;

	case 2:
		leng.z = fabs(g_Camera.posV.z - g_Camera.posR.z);
		return leng;

	case 3:
		leng.x = fabs(g_Camera.posV.x - g_Camera.posR.x);
		leng.y = fabs(g_Camera.posV.y - g_Camera.posR.y);
		leng.z = fabs(g_Camera.posV.z - g_Camera.posR.z);
		return leng;

		//念のため
	default:
		return leng;
	}
}

void CameraResetPos()
{
	//スイッチ文でシーンごとに初期位置設定することになるかもしれん。要確認
	g_Camera.posV = D3DXVECTOR3(CAMERA_POSV_X, CAMERA_POSV_Y, CAMERA_POSV_Z);
	g_Camera.posR = D3DXVECTOR3(CAMERA_POSR_X, CAMERA_POSR_Y, CAMERA_POSR_Z);
	g_Camera.vecU = D3DXVECTOR3(0.0f, 1.0f, 0.0f);

	g_Player_Camera = false;

	g_CameraRot_X = 270.0f;			//カメラ角度(X)
	g_CameraRot_Y = 270.0f;			//カメラ角度(Y)
	g_CameraRot_Z = 270.0f;			//カメラ角度(Z)

}

//固定軸移動：軸移動関数
void Camera_Move_Axis1(D3DXVECTOR3(Start_Pos), D3DXVECTOR3(End_Pos), float Move_Time)
{
	g_Camera.posV.x += (End_Pos.x - Start_Pos.x) / (Move_Time * 60);
	g_Camera.posV.y += (End_Pos.y - Start_Pos.y) / (Move_Time * 60);
	g_Camera.posV.z += (End_Pos.z - Start_Pos.z) / (Move_Time * 60);
}

//Y軸回転移動：カメラ回転
void Camera_Move_Yspin(D3DXVECTOR3(Start_Pos), float Rota_Y, float Spin_Time)
{
	g_CameraRot_X += Rota_Y / (Spin_Time * 60);
	g_CameraRot_Z += Rota_Y / (Spin_Time * 60);
	g_Camera.posV.x = cosf(D3DXToRadian(g_CameraRot_X)) * CAMERA_RAD;
	g_Camera.posV.z = sinf(D3DXToRadian(g_CameraRot_Z)) * CAMERA_RAD;
}

//X軸回転移動：カメラ回転
void Camera_Move_Xspin(D3DXVECTOR3(Start_Pos), float Rota_X, float Spin_Time)
{
	g_CameraRot_Y += Rota_X / (Spin_Time * 60);
	g_Camera.posV.y = cosf(D3DXToRadian(g_CameraRot_Y)) * CAMERA_RAD;
}

//追加部分2==========================================
//以下最後のパンチのカメラ関連(作ったけど微妙だったからまた後で調整)
//フェーズ番号を後につける
void LastPunchCamera1()
{
	g_Camera.posV.z += 16.0f / (LAST_PUNCH_WAVE_FRAME + 15);
}

void LastPunchCamera2()
{
	static float LastPunch_CameraRotXZ2 = 15.0f;
	static float LastPunch_CameraRotY2 = 30.0f;
	//敵とカメラの距離
	static float EtoC_Distance = GetEnemy()->Pos.z - g_Camera.posV.z;
	g_Camera.posV += D3DXVECTOR3(cos(D3DXToRadian(LastPunch_CameraRotXZ2)) * EtoC_Distance / (LAST_PUNCH_SET_FRAME + 15), sin(D3DXToRadian(LastPunch_CameraRotY2)) * EtoC_Distance / (LAST_PUNCH_SET_FRAME + 15), sin(D3DXToRadian(LastPunch_CameraRotXZ2)) * EtoC_Distance / (LAST_PUNCH_SET_FRAME + 15));
}

void LastPunchCamera3()
{

}

void LastPunchCamera4()
{

}
//追加部分2==========================================
void Finish_CameraMove()
{
	g_Camera.posR = D3DXVECTOR3((GetEnemy() + 1)->Pos);
	if (GetSkyFlag())
	{
		//g_Camera.posV.x = (GetEnemy() + 1)->Pos.x;
		g_Camera.posV.y = (GetEnemy() + 1)->Pos.y;
		g_Camera.posV.z = (GetEnemy() + 1)->Pos.z - 20.0f;
	}
}

//=============================================================================
// カメラの取得
//=============================================================================
CAMERA *GetCamera(void)
{
	return &g_Camera;
}
