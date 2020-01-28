#pragma once

#include "main.h"

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitGauge(void);
void UninitGauge(void);
void UpdateGauge(void);
void DrawGauge(void);

//ゲージを増やす関数、引数は何パーセント増やすか
void AddGauge(int Percent);
//ゲージを減らすか関数、引数は何パーセント減らすか
void ReduceGauge(int Percent);

int GetGauge();