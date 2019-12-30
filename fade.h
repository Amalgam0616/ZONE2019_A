#pragma once

#include <d3d9.h>
#include "scene.h"

//====================================
//  フェード定義(enum)
//====================================
enum FADE_STATE 
{
	FADE_STATE_NONE = 0,
	FADE_STATE_IN,			//フェードイン
	FADE_STATE_OUT			//フェードアウト
};

//==============================
//   関数宣言
//==============================
void InitFade(void);		//初期化
void UninitFade(void);		//解放
void UpdateFade(void);			//更新
void DrawFade(void);			//描画

void SetFade(SCENE_INDEX Scene);	//FadeIn,FadeOutを行い次のシーンに移る。引数に移るシーンのINDEXを与える
FADE_STATE GetFadeState();			//現在のFadeの状態を取得する