#pragma once

#include "main.h"

//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
HRESULT InitSky(void);
void UninitSky(void);
void UpdateSky(void);
void DrawSky(void);
float GetSky_ty(void);
bool GetMoonFlag();
float GetMoonPos_y();