
#include "light.h"
#include "myDirect3D.h"
#include "input.h"
#include "debugproc.h"
//=============================================================================
// マクロ定義
//=============================================================================
#define	NUM_LIGHT		(3)		// ライトの数

//=============================================================================
// グローバル変数
//=============================================================================
D3DLIGHT9 g_aLight[NUM_LIGHT];		// ライト情報

int R = 0;
int G = 0;
int B = 0;
//=============================================================================
// ライトの初期化処理
//=============================================================================
void InitLight(void)
{
	//Direct3DDeviceの取得
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice(); 
	D3DXVECTOR3 vecDir;		//ライトの方向

	// D3DLIGHT9構造体を0でクリアする
	ZeroMemory(&g_aLight[0], sizeof(D3DLIGHT9));

	
	//==========================================================================
	/*
	// ライトのタイプの設定(POINT⇒Positionから全方向に光を飛ばす）
	// 光の向き⇒全方向　減衰率⇒距離（Range)の2乗 
	g_aLight[0].Type = D3DLIGHT_POINT;

	// ライトの位置
	g_aLight[0].Position = D3DXVECTOR3(0.0f, 49.0f, 50.0f);
	// 拡散光
	g_aLight[0].Diffuse = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
	// 環境光
	g_aLight[0].Ambient = D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.0f);
	// 減衰率(小さいほど光が強くなる）
	g_aLight[0].Attenuation0 = 0.5f;

	D3DXVec3Normalize((D3DXVECTOR3*)&g_aLight[0].Direction, &vecDir);

	// ライト範囲
	g_aLight[0].Range = 200.0f;
	// ライトをレンダリングパイプラインに設定
	pDevice->SetLight(0, &g_aLight[0]);
	// ライトを有効に
	pDevice->LightEnable(0, TRUE);
	// ライティングモード有効
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	*/
	/*
	//==========================================================================

	// D3DLIGHT9構造体を0でクリアする
	ZeroMemory(&g_aLight[1], sizeof(D3DLIGHT9));

	// ライトのタイプの設定
	g_aLight[1].Type = D3DLIGHT_SPOT;

	// ライトの位置
	g_aLight[1].Position = D3DXVECTOR3(0.0f, 20.0f, 0.0f);
	// 拡散光
	g_aLight[1].Diffuse = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);
	// 環境光
	g_aLight[1].Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);

	// ライトの方向の設定
	vecDir = D3DXVECTOR3(0.0f, -1.0f, 0.0f);

	D3DXVec3Normalize((D3DXVECTOR3*)&g_aLight[1].Direction, &vecDir);

	// 光源の有効距離
	g_aLight[1].Range = 50.0f;
	
	// フォールオフ（円錐の内側から外側に向かって起こる光の減衰）、
	g_aLight[1].Falloff = 0.5f;

	// 減衰率(小さいほど光が強くなる）
	g_aLight[1].Attenuation0 = 0.2f;  
	
	// 内側のコーンの角度
	g_aLight[1].Theta = D3DXToRadian(40.0f);
	// 外側のコーンの角度
	g_aLight[1].Phi = D3DXToRadian(60.0f);

	// ライトをレンダリングパイプラインに設定
	pDevice->SetLight(1, &g_aLight[1]);
	// ライト1を有効に
	//pDevice->LightEnable(1, TRUE);
	// ライティングモード有効
	//pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	*/
	////==========================================================================

	
	// D3DLIGHT9構造体を0でクリアする
	ZeroMemory(&g_aLight[2], sizeof(D3DLIGHT9));

	// ライト2のタイプの設定
	g_aLight[2].Type = D3DLIGHT_DIRECTIONAL;

	// ライト2の拡散光の設定
	g_aLight[2].Diffuse = D3DXCOLOR(0.5f, 0.5f, 0.5f, 1.0f);

	// ライト2の鏡面反射光の設定
	g_aLight[2].Ambient = D3DXCOLOR(0.2f, 0.2f, 0.2f, 1.0f);

	// ライト2の方向の設定
	vecDir = D3DXVECTOR3(0.80f, 0.10f, 0.40f);
	D3DXVec3Normalize((D3DXVECTOR3*)&g_aLight[2].Direction, &vecDir);

	// ライト2をレンダリングパイプラインに設定
	pDevice->SetLight(2, &g_aLight[2]);

	// ライト2を有効に
	pDevice->LightEnable(2, TRUE);

	// ライティングモード有効
	pDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_RGBA(255, 255, 255, 255));
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	
	/*
	g_aLight[1].Type = D3DLIGHT_POINT;

	// ライトの位置
	g_aLight[1].Position = D3DXVECTOR3(0.0f, 21.0f, 0.0f);
	// 拡散光
	g_aLight[1].Diffuse = D3DXCOLOR(0.4f, 0.4f, 0.4f, 1.0f);
	// 環境光
	g_aLight[1].Ambient = D3DXCOLOR(0.3f, 0.3f, 0.3f, 0.0f);
	// 減衰率(小さいほど光が強くなる）
	g_aLight[1].Attenuation0 = 0.7f;

	D3DXVec3Normalize((D3DXVECTOR3*)&g_aLight[0].Direction, &vecDir);

	// ライト範囲
	g_aLight[1].Range = 200.0f;
	// ライトをレンダリングパイプラインに設定
	pDevice->SetLight(0, &g_aLight[1]);

	// ライトを有効に
	pDevice->LightEnable(0, TRUE);
	// ライティングモード有効
	pDevice->SetRenderState(D3DRS_LIGHTING, TRUE);
	*/
}
//=============================================================================
// ライトの終了処理
//=============================================================================
void UninitLight(void)
{
}
//=============================================================================
// ライトの更新処理
//=============================================================================
void UpdateLight(void)
{
	/*
	//Direct3DDeviceの取得
	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();

	if (Keyboard_IsPress(DIK_T))
	{
		g_aLight[0].Diffuse.r += 0.02f;
		g_aLight[0].Diffuse.g += 0.02f;
		g_aLight[0].Diffuse.b += 0.02f;
	}
	if (Keyboard_IsPress(DIK_Y))
	{
		g_aLight[0].Diffuse.r -= 0.02f;
		g_aLight[0].Diffuse.g -= 0.02f;
		g_aLight[0].Diffuse.b -= 0.02f;
	}
	if (Keyboard_IsPress(DIK_4))
	{
		g_aLight[0].Range += 1.0f;
	}
	if (Keyboard_IsPress(DIK_5))
	{
		g_aLight[0].Range -= 1.0f;
	}
	if (Keyboard_IsPress(DIK_K))
	{
		R++;
		G++;
		B++;
		pDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_RGBA(R, G, B, 255));
	}
	if (Keyboard_IsPress(DIK_L))
	{
		R--;
		G--;
		B--;
		pDevice->SetRenderState(D3DRS_AMBIENT, D3DCOLOR_RGBA(R, G, B, 255));
	}
	
	DebugProc_Print((char *)"減衰点:%f\n", g_aLight[0].Attenuation0);
	DebugProc_Print((char *)"光の範囲:%f\n", g_aLight[0].Position.y);
	DebugProc_Print((char *)"鏡面反射光:%d,%d,%d\n", R,G,B);
	
	// ライトをレンダリングパイプラインに設定
	pDevice->SetLight(0, &g_aLight[0]);
	*/
}

