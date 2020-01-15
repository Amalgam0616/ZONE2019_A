#pragma once

#include "main.h"

//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSky(void);
void UninitSky(void);
void UpdateSky(void);
void DrawSky(void);
float GetSky_ty(void);
bool GetMoonFlag();
float GetMoonPos_y();