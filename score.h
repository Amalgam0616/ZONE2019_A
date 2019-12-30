#pragma once
#include "texture.h"

#define NUMBER_WIDTH  (300)
#define NUMBER_HEIGHT (300)

//x横位置　y縦位置　score表示する値  fig桁数  bZeroゼロの時に表示するかフラグ
//void DrawScore(float x, float y, int score, int fig, bool bZero);
void DrawScore(TextureIndex texture_index,float x, float y, int score, int fig, float sx, float sy, float offset);

//スコア描画(アルファ値あり)
void DrawScore(TextureIndex texture_index, float x, float y, int score, int fig, float sx, float sy, float offset, int alpha);

// 0 <= n <= 9
void Number_Draw(TextureIndex texture_index,float x, float y, int n, float sx, float sy);

//αチアリナンバー計算
void Number_Draw_Alpha(TextureIndex texture_index, float x, float y, int n, float sx, float sy, int alpha);

//以下、わしがお送りいたします

//g_Scoreの初期化
void InitScore(void);

//スコアの足し算処理
void AddScore(int score);

//スコアのゲッター
int GetScore(void);