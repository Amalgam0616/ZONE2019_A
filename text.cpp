#include "myDirect3D.h"
#include "text.h"

// グローバル変数宣言 ===================================================================

//フォントリスト
static const LPCSTR g_FontList[FONT_INDEX_MAX] =
{
	"Terminal",	//テスト用
};

// 関数宣言 ============================================================================

//フォント生成関数
HRESULT Text_CriateFont(LPD3DXFONT *p_Font,int w, int h, FONT_INDEX index, bool isItalic)
{
	//D3DDeviceの取得
	LPDIRECT3DDEVICE9 p_D3DDevice = GetD3DDevice();

	//フォントの生成
	if (FAILED(D3DXCreateFontA(p_D3DDevice,			//Direct3Dデバイス
		h, w,										//文字の高さ、幅
		FW_HEAVY, 1,								//フォントスタイル、ミップマップのレベル(?)
		isItalic,									//斜体(イタリック)にするかどうか
		SHIFTJIS_CHARSET,							//文字セット
		OUT_TT_ONLY_PRECIS, ANTIALIASED_QUALITY,	//出力精度、出力品質
		FF_DONTCARE,								//フォントピッチとファミリ
		g_FontList[index], p_Font)))				//フォント名、フォントポインタ
	{
		return E_FAIL;
	}

	return S_OK;
}

//終了関数の予定だったけど、使う場所でやった方がいいのでやり方だけ残しときます
void UninitText()
{
	//フォントポインタの解放
	//SAFE_RELEASE(LPD3DXFONT型のフォントポインタ);
}

//テキスト描画
void TextDraw(LPD3DXFONT p_Font,const char *text, int X, int Y, FONT_INDEX index, D3DXCOLOR color)
{
	RECT rect = { X,Y,0,0 };	//描画位置の座標設定
	
	p_Font->DrawTextA(NULL,		//スプライトポインタ(NULL指定可)
		text,					//描画文字
		-1,						//文字数(-1指定で全部になる)
		&rect,					//描画範囲
		DT_LEFT | DT_NOCLIP,	//フォーマット
		color);					//色
}
