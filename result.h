#pragma once

#include "main.h"

//スコアの基準値
#define RANKING_BORDER1 300000
#define RANKING_BORDER2 800000


//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
void InitResult(void);
void UninitResult(void);
void UpdateResult(void);
void DrawResult(void);
bool GetRankingStart();
bool GetResultStart();
void SetResultStart_true();
void SetResultStart_false();