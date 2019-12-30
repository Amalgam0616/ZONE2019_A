#include "main.h"
#include "myDirect3D.h"
#include "fade.h"
#include "scene.h"

//==============================
//   フェード構造体
//==============================
typedef struct FadeVertex_tag
{
	D3DXVECTOR4 position;
	D3DCOLOR color;
} FadeVertex;
#define FVF_FADE_VERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE)


//==============================
//   グローバル変数宣言
//==============================
static FADE_STATE g_FadeState;							//フェードの状態
static SCENE_INDEX g_FadeNextScene;						//次のシーンの保持
static float g_FadeAlpha;								//フェードのアルファ値
static D3DXCOLOR g_FadeColor(0.0f, 0.0f, 0.0f, 1.0f);	//フェードの色

//==============================
//  フェード初期化処理
//==============================
void InitFade(void)
{
	g_FadeState = FADE_STATE_NONE;		//状態を空にする
	g_FadeNextScene = SCENE_INDEX_NONE;	//次のシーンの保持状態を空にする
	g_FadeAlpha = 0.0f;					//アルファを０にする
}
//==============================
//  フェード終了処理
//==============================
void UninitFade(void)
{
}
//==============================
//  フェード更新処理
//==============================
void UpdateFade(void)
{
	//フェードアウトのときは透明→黒になる
	//SetFade(SCENE_INDEX Scene)関数を呼び出した時g_FadeStateはFADE_STATE_OUTになる
	if (g_FadeState == FADE_STATE_OUT) 
	{
		if (g_FadeAlpha >= 1.0f) 
		{
			g_FadeAlpha = 1.0f;
			g_FadeState = FADE_STATE_IN;
			SetScene(g_FadeNextScene);
		}
		g_FadeAlpha += 1.0f / 20.0f;
	}
	//フェードインのときは黒→透明になる
	else if (g_FadeState == FADE_STATE_IN) 
	{
		if (g_FadeAlpha <= 0.0f) 
		{
			g_FadeAlpha = 0.0f;
			g_FadeState = FADE_STATE_NONE;
		}
		g_FadeAlpha -= 1.0f / 20.0f;
	}
}
//==============================
//  フェード描画処理
//==============================
void DrawFade(void)
{
	//もしフェード状態でない場合はすぐにDrawFadeの処理を終わらせる
	if (g_FadeState == FADE_STATE_NONE) 
	{
		return;
	}

	g_FadeColor.a = g_FadeAlpha;
	D3DCOLOR c = g_FadeColor;

	FadeVertex v[] = 
	{
		{ D3DXVECTOR4(        0.0f,          0.0f, 0.0f, 1.0f), c }, 
		{ D3DXVECTOR4(SCREEN_WIDTH,          0.0f, 0.0f, 1.0f), c }, 
		{ D3DXVECTOR4(        0.0f, SCREEN_HEIGHT, 0.0f, 1.0f), c }, 
		{ D3DXVECTOR4(SCREEN_WIDTH, SCREEN_HEIGHT, 0.0f, 1.0f), c }, 
	};

	LPDIRECT3DDEVICE9 pDevice = GetD3DDevice();

	pDevice->SetFVF(FVF_FADE_VERTEX);
	pDevice->SetTexture(0, NULL);
	pDevice->DrawPrimitiveUP(D3DPT_TRIANGLESTRIP, 2, v, sizeof(FadeVertex));
}
//==============================
//  フェードのセット
//==============================
void SetFade(SCENE_INDEX Scene) 
{
	g_FadeNextScene = Scene;
	g_FadeState = FADE_STATE_OUT;
}
//==============================
//  現在のフェード状態の取得
//==============================
FADE_STATE GetFadeState() 
{
	return g_FadeState;
}