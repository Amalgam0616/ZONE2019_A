#pragma once

/*=================================================================
作成者: Amalgam
概要：テキスト関連が使えるようになる関数です
　　　大体はリーダーの3d.cppからテキスト関連だけ持ってきた感じです。
　　　多少使いやすさを考慮して改変を入れてあります。

　　　正直これに関しては、分割してみたは良いけど使い方はさっぱりわからないです。
=================================================================*/

#include "main.h"

//テキストインデックス 追加する際はここと text.cpp のフォントリストを変更してください
enum FONT_INDEX
{
	FONT_INDEX_001 = 0,

	FONT_INDEX_MAX,
};

//=============================================================================
// プロトタイプ宣言
//=============================================================================

//フォントを生成する関数
//引数１：LPD3DXFONT：フォント情報を入れたいポインタ
//引数２：FONT_INDEX：使用するフォントのインデックスNo.
//引数３：int：文字の幅
//引数４：int：文字の高さ
HRESULT Text_CriateFont(LPD3DXFONT *p_Font, int w, int h, FONT_INDEX index, bool isItalic);

//テキストを描画する関数
//引数１：LPD3DXFONT：使用するフォント
//引数２：char*：表示する文字列のポインタ
//引数３：int：座標(ヨコ)
//引数４：int：座標(タテ)
//引数５：FONT_INDEX：使うフォントのインデックス　デフォルトは FONT_INDEX_001
//引数６：D3DXCOLOR：色指定　デフォルトは黒
void TextDraw(LPD3DXFONT p_Font, const char *text, int X, int Y, FONT_INDEX index = FONT_INDEX_001, D3DXCOLOR color = D3DXCOLOR(0, 0, 0, 255));

//終了関数
void UninitText();

