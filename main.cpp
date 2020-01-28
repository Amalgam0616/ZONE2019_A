#include "main.h"
#include "scene.h"
#include "fade.h"
#include "time.h"
#include "myDirect3D.h"
#include "input.h"
#include "texture.h"
#include "camera.h"
#include "light.h"
#include "debugproc.h"
#include "text.h"
#include "field.h"
#include "sound.h"

#define _USE_MATH_DEFINES
#include <math.h>
#include <time.h>
#include <mmsystem.h>

//追加のライブラリ使用
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dinput8.lib")
#pragma comment(lib, "winmm.lib")

//====================================
//  定数定義
//====================================
#define CLASS_NAME     "GameWindow"		// ウインドウクラスの名前
#define WINDOW_CAPTION "ZONE"			// ゲームの名前（ウインドウに表示される名前）

//==============================
//   関数宣言
//==============================
static bool InitMain(HINSTANCE hInst);
static void UninitMain(void);
static void UpdateMain(void);
static void DrawMain(void);

// ウィンドウプロシージャ(コールバック関数)
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
//==============================
//  グローバル変数宣言
//==============================
static HWND g_hWnd;		// ウィンドウハンドル
int g_GameFrame;		//60フレームごとに0に初期化される
int g_GameFrameLapse;	//プログラム開始時点～終了までの経過フレームを保持

//　デバッグ用
#ifdef _DEBUG
int					g_nCountFPS;			// FPSカウンタ
#endif
bool				g_bDispDebug = true;	// デバッグ表示ON/OFF

//==============================
//   メイン
//==============================
int APIENTRY WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	// 使用しない一時変数を明示
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// ウィンドウクラス構造体の設定
	WNDCLASS wc = {};
	wc.lpfnWndProc = WndProc;                          // ウィンドウプロシージャの指定
	wc.lpszClassName = CLASS_NAME;                     // クラス名の設定
	wc.hInstance = hInstance;                          // インスタンスハンドルの指定
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);          // マウスカーソルを指定
	wc.hbrBackground = (HBRUSH)(COLOR_BACKGROUND + 1); // ウインドウのクライアント領域の背景色を設定

	// クラス登録
	RegisterClass(&wc);


	// ウィンドウスタイル
	DWORD window_style = WS_OVERLAPPEDWINDOW & ~(WS_MAXIMIZEBOX | WS_THICKFRAME);

	// 基本矩形座標
	RECT window_rect = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };

	// 指定したクライアント領域を確保するために新たな矩形座標を計算
	AdjustWindowRect(&window_rect, window_style, FALSE);

	// 新たなWindowの矩形座標から幅と高さを算出
	int window_width = window_rect.right - window_rect.left;
	int window_height = window_rect.bottom - window_rect.top;

	// プライマリモニターの画面解像度取得
	int desktop_width = GetSystemMetrics(SM_CXSCREEN);
	int desktop_height = GetSystemMetrics(SM_CYSCREEN);

	// デスクトップの真ん中にウィンドウが生成されるように座標を計算
	// ※ただし万が一、デスクトップよりウィンドウが大きい場合は左上に表示
	int window_x = max((desktop_width - window_width) / 2, 0);
	int window_y = max((desktop_height - window_height) / 2, 0);

	// ウィンドウの生成
	g_hWnd = CreateWindow
	(
		CLASS_NAME,     // ウィンドウクラス
		WINDOW_CAPTION, // ウィンドウテキスト
		WS_VISIBLE | WS_POPUP ,   // ウィンドウスタイル
		window_x,       // ウィンドウ座標x
		window_y,       // ウィンドウ座標y
		window_width,   // ウィンドウの幅
		window_height,  // ウィンドウの高さ
		NULL,           // 親ウィンドウハンドル
		NULL,           // メニューハンドル
		hInstance,      // インスタンスハンドル
		NULL            // 追加のアプリケーションデータ
	);

	if (g_hWnd == NULL)
	{
		// ウィンドウハンドルが何らかの理由で生成出来なかった場合-1を返し終了
		return -1;
	}

	// 指定のウィンドウハンドルのウィンドウを指定の方法で表示
	ShowWindow(g_hWnd, nCmdShow);
	UpdateWindow(g_hWnd);

	// ゲームの初期化
	if (!InitMain(hInstance))
	{
		MessageBox(g_hWnd, "どこかの初期化でミス発生", "エラー", MB_OK);
	}

	DWORD dwExecLastTime;
	DWORD dwFPSLastTime;
	DWORD dwCurrentTime;
	DWORD dwFrameCount;

	// 分解能を設定
	timeBeginPeriod(1);

	// フレームカウント初期化
	dwExecLastTime = dwFPSLastTime = timeGetTime();
	dwCurrentTime = dwFrameCount = 0;

	//==========================
	//  ゲームメインループ
	//==========================
	MSG msg = {}; // msg.message == WM_NULL

	while (WM_QUIT != msg.message)
	{

		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			// メッセージがある場合はメッセージ処理を優先
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			dwCurrentTime = timeGetTime();
			if ((dwCurrentTime - dwFPSLastTime) >= 500)	// 0.5秒ごとに実行
			{
#ifdef _DEBUG
				g_nCountFPS = dwFrameCount * 1000 / (dwCurrentTime - dwFPSLastTime);
#endif
				dwFPSLastTime = dwCurrentTime;
				dwFrameCount = 0;
			}

			if ((dwCurrentTime - dwExecLastTime) >= (1000 / 60))
			{
#ifdef _DEBUG
				DebugProc_Print((char *)"FPS:%d\n", g_nCountFPS);
				//DebugProc_Print((char *)"FPS:%d\n", sizeof(LPDIRECT3DDEVICE9));
#endif
				dwExecLastTime = dwCurrentTime;
				// ゲームの更新
				UpdateMain();
				// ゲームの描画
				DrawMain();

				dwFrameCount++;
			}
		}
	}

	// ゲームの終了処理
	UninitMain();

	return (int)msg.wParam;
}
//==============================================
// ウィンドウプロシージャ(コールバック関数)
//==============================================
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE)
		{
			SendMessage(hWnd, WM_CLOSE, 0, 0); // WM_CLOSEメッセージの送信
		}
		break;

	case WM_CLOSE:
		if (MessageBox(hWnd, "本当に終了してよろしいですか？", "確認", MB_OKCANCEL | MB_DEFBUTTON2) == IDOK)
		{
			DestroyWindow(hWnd); // 指定のウィンドウにWM_DESTROYメッセージを送る
		}
		return 0; // DefWindowProc関数にメッセージを流さず終了することによって何もなかったことにする

	case WM_DESTROY: // ウィンドウの破棄メッセージ
		PostQuitMessage(0); // WM_QUITメッセージの送信
		return 0;
	};

	// 通常メッセージ処理はこの関数に任せる
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}
//==============================
//   メインの初期化処理
//==============================
bool InitMain(HINSTANCE hInst)
{
	//シードの初期化
	srand((unsigned int)time(NULL));

	// ゲームの初期化(Direct3Dの初期化)
	if (!InitD3D(g_hWnd))
	{
		//初期化に失敗したらfalseを返す
		return false;
	}
	// DirectInputの初期化（キーボード）
	if (!InitKeyboard(hInst, g_hWnd))
	{
		//初期化に失敗したらfalseを返す
		return false;
	}
	// DirectInputの初期化（ゲームパッド）
	if (!InitGamePad(hInst, g_hWnd))
	{
		//初期化に失敗したらfalseを返す
		return false;
	}


	//  全テクスチャファイルのロード
	LoadTexture();

	//  デバッグ表示処理 
	DebugProc_Initialize();

	//  カメラの初期化処理
	InitCamera();

	//  ライトの初期化処理
	InitLight();

	//サウンドの初期化処理
	InitSound(g_hWnd);

	//  シーンの初期化
	InitScene();

	//  タイトルシーンのセット
	SetScene(SCENE_INDEX_TITLE);

	return true;
}
//==============================
//   メインの終了処理
//==============================
void UninitMain()
{
	//  カメラの終了処理
	UninitCamera();

	//  ライトの終了処理
	UninitLight();

	//  音楽の終了処理
	UninitSound();

	//  テクスチャの解放
	UninitTexture();

	//  DirectInputの終了処理
	UninitKeyboard();

	//  DirectInputの終了処理
	UninitGamePad();

	//  シーンの終了処理
	UninitScene();

	// ゲームの終了処理(Direct3Dの終了処理)
	UninitD3D();
}
//==============================
//   メインの更新処理
//==============================
void UpdateMain()
{
	//  キーボードの状態を更新する
	UpdateKeyboard();
	//  キーボードの状態を更新する
	UpdateGamePad();

	// カメラの更新処理
	UpdateCamera();

	// ライトの更新処理
	UpdateLight();

	//  シーンの更新
	UpdateScene();
}
//==============================
//   メインの描画処理
//==============================
void DrawMain()
{
	LPDIRECT3DDEVICE9 pD3DDevice = GetD3DDevice();

	// 画面のクリア
	pD3DDevice->Clear(0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_RGBA(0, 0, 0, 255), 1.0f, 0);

	// 描画バッチ命令の開始
	pD3DDevice->BeginScene();

	// カメラのセット
	SetCamera();

	// デバッグ表示の描画処理
	if (g_bDispDebug)
	{
		//DebugProc_Draw();
	}

	// シーンの描画
	DrawScene();

	// フェードの描画
	DrawFade();

	// 描画バッチ命令の終了
	pD3DDevice->EndScene();

	// バックバッファをフリップ（タイミングはD3DPRESENT_PARAMETERSの設定による）
	pD3DDevice->Present(NULL, NULL, NULL, NULL);

}
//============================
//  ウィンドウハンドルの取得
//============================
HWND GetHWND()
{
	return g_hWnd;
}
