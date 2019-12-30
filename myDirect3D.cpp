#include "myDirect3D.h"
#include "main.h"

//==============================
//  マクロ宣言
//==============================
#define SAFE_RELEASE(p) {if(p){(p)->Release();(p)=NULL;}}	//安全に解放する

//==============================
//  グローバル変数宣言
//==============================
static LPDIRECT3D9 g_pD3D = NULL;             // Direct3Dインターフェース
static LPDIRECT3DDEVICE9 g_pD3DDevice = NULL; // Direct3Dデバイス

//=====================================================
//  D3DDevice初期化処理
//  戻り値→初期化に成功：true、失敗：false
//=====================================================
bool InitD3D(HWND hWnd)
{
	// Direct3Dインターフェースの取得
	g_pD3D = Direct3DCreate9(D3D_SDK_VERSION);

	if (g_pD3D == NULL) {
		// Direct3Dインターフェースの取得に失敗
		MessageBox(hWnd, "Direct3Dインターフェースの作成に失敗しました", "エラー", MB_OK);
		return false;
	}

	// デバイスの生成に利用する情報構造体の作成
	D3DPRESENT_PARAMETERS d3dpp = {};

	d3dpp.BackBufferWidth = SCREEN_WIDTH;					// バックバッファの横幅				
	d3dpp.BackBufferHeight = SCREEN_HEIGHT;					// バックバッファの縦幅
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;				// バックバッファのフォーマット指定
	d3dpp.BackBufferCount = 1;								// バックバッファの数
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;				// スワップエフェクト（スワップ方法）の設定		
	d3dpp.Windowed = TRUE;									// ウィンドウモード or フルスクリーン
	d3dpp.EnableAutoDepthStencil = TRUE;                        // 深度バッファ・ステンシルバッファの使用				
	d3dpp.AutoDepthStencilFormat = D3DFMT_D16;                  // 深度バッファ・ステンシルバッファのフォーマット指定
	d3dpp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT; // フルスクリーン時のリフレッシュレートの指定
	d3dpp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;	// リフレッシュレートとPresent処理の関係

	// Direct3Dデバイスの取得
	if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, GetHWND(), D3DCREATE_HARDWARE_VERTEXPROCESSING, &d3dpp, &g_pD3DDevice))) 
	{ 
		//HALが駄目ならHEL(ソフトウェアエミュレーション)
		if (FAILED(g_pD3D->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hWnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_pD3DDevice)))
		{
			// デバイスの作成に失敗
			MessageBox(hWnd, "Direct3Dデバイスの作成に失敗しました", "エラー", MB_OK);
			return false;
		}
	}

	//==================================================================================
	//機能初期化
	//==================================================================================

	// Zバッファー処理を有効にする
	g_pD3DDevice->SetRenderState(D3DRS_ZENABLE, TRUE);

	// ライトを有効にする
	g_pD3DDevice->SetRenderState(D3DRS_LIGHTING, TRUE);

	// アンビエントライト（環境光）を設定する
	g_pD3DDevice->SetRenderState(D3DRS_AMBIENT, 0x00111111);
	// スペキュラ（鏡面反射）を有効にする
	g_pD3DDevice->SetRenderState(D3DRS_SPECULARENABLE, TRUE);

	//アドレッシングモード：ラップ（画像を繰り返す）
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);

	//テクスチャフィルター：ポイント
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_POINT);

	// アルファブレンドの設定 ここ
	g_pD3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, TRUE);
	g_pD3DDevice->SetRenderState(D3DRS_SRCBLEND, D3DBLEND_SRCALPHA);
	g_pD3DDevice->SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG0, D3DTA_DIFFUSE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLORARG1, D3DTA_TEXTURE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_MODULATE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG0, D3DTA_DIFFUSE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	g_pD3DDevice->SetSamplerState(0, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	// 頂点カラーとテクスチャのブレンド設定
	//	g_pD3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTOP_MODULATE);

	return true;
}
//==============================
//  D3DDevice終了処理
//==============================
void UninitD3D(void)
{
	// Direct3Dデバイスの解放
	SAFE_RELEASE(g_pD3D);

	// Direct3Dインターフェースの解放
	SAFE_RELEASE(g_pD3DDevice);
}
//==============================
//  D3DDeviceの取得
//==============================
LPDIRECT3DDEVICE9 GetD3DDevice()
{
	return g_pD3DDevice;
}
