#pragma once

#include "main.h"

//�X�R�A�̊�l
#define RANKING_BORDER1 300000
#define RANKING_BORDER2 800000


//*****************************************************************************
// �v���g�^�C�v�錾
//*****************************************************************************
void InitResult(void);
void UninitResult(void);
void UpdateResult(void);
void DrawResult(void);
bool GetRankingStart();
bool GetResultStart();
void SetResultStart_true();
void SetResultStart_false();