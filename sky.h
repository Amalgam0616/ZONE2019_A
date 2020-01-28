#pragma once

#include "main.h"
#include "Xfile.h"
//*****************************************************************************
// プロトタイプ宣言
//*****************************************************************************
HRESULT InitSky(void);
void UninitSky(void);
void UpdateSky(void);
void DrawSky(void);

bool GetMoonFlag();
bool GetSatelliteFlag();
bool GetAirplaneFlag();
XMODEL* GetSkyModel();

void SetDrawStop();
int GetCopy_Score();

void SetCopy_Score();