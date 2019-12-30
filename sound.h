#pragma once

#include <windows.h>
#include "xaudio2.h"

//=================================================
//　サウンドラベル定義(enum)
//  ※sound.cppの31行目を記述する前に書くこと！
//=================================================
typedef enum
{
	//  例
	//  SOUND_LABEL_BGM_TITLE,	//タイトル
	//  このように記述すること。

	SOUND_LABEL_SE_HIT,	//タイトル

	SOUND_LABEL_MAX,	//曲の最大数

} SOUND_LABEL;

//==============================
//  関数宣言
//==============================
bool InitSound(HWND hWnd);
void UninitSound(void);
void PlaySound(SOUND_LABEL label);
void StopSound(SOUND_LABEL label);
void StopSound(void);
